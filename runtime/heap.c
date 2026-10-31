// Tas invité (malloc/free/realloc des imports libc) dans 0x80000000-0xc0000000.
// Métadonnées hors de la mémoire invitée : un débordement invité ne peut pas
// corrompre l'allocateur. Toutes les opérations sont en O(1) amorti :
// - petits blocs (<= 16 Kio) : classes de taille, listes libres par classe ;
// - au-delà : multiples de 4 Kio, zones libres triées par adresse, fusionnées
//   avec leurs voisines et réutilisables pour toute taille (sinon la mémoire
//   libérée dans une classe ne servait plus aux autres : un chargement de carte
//   épuisait le tas) ;
// - table de hachage adresse -> taille pour free/realloc/malloc_usable_size.
// Remplace une liste chaînée parcourue entièrement à chaque appel (coût
// quadratique pendant la traduction des shaders par togl).
#include "rt.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_BEGIN 0x80000000u
#define HEAP_END 0xdf000000u        // sous les données importées (0xe0000000)
#define MAP_STEP 0x400000u        // projection par tranches de 4 Mio
#define SMALL_MAX 0x4000u         // au-delà : zones de 4 Kio fusionnables
#define LARGE_ALIGN 0x1000u
#define CLASS_COUNT 80            // 64 classes de 16 o jusqu'à 1 Kio, puis 4 par puissance de 2 jusqu'à 16 Kio

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint32_t top = HEAP_BEGIN, mapped = HEAP_BEGIN;
static uint64_t live_bytes;

typedef struct { uint32_t *items; size_t count, capacity; } Stack;
static Stack free_small[CLASS_COUNT];

typedef struct { uint32_t addr, size; } Extent;
static Extent *free_large;
static size_t large_count, large_capacity;

// Table adresse -> taille arrondie ; addr 0 = vide, size 0 = supprimé.
typedef struct { uint32_t addr, size; } Slot;
static Slot *table;
static size_t table_capacity, table_used;   // used : occupés + supprimés

static unsigned class_of(uint32_t size, uint32_t *rounded) {
    if (size <= 1024) {
        unsigned k = (size + 15) / 16; if (!k) k = 1;
        *rounded = k * 16;
        return k - 1;
    }
    unsigned e = 32 - (unsigned)__builtin_clz(size - 1);   // 2^(e-1) < size <= 2^e
    uint32_t step = 1u << (e - 3);
    uint32_t m = (size + step - 1) / step;                // 5..8
    *rounded = m * step;
    return 64 + (e - 11) * 4 + (m - 5);
}

static size_t hash(uint32_t addr) { return (size_t)((addr >> 4) * 2654435761u); }

// Agrandit, ou réorganise à taille égale si ce sont surtout des cases
// supprimées qui remplissent la table.
static int table_grow(void) {
    size_t live = 0;
    for (size_t i = 0; i < table_capacity; ++i) live += table[i].addr && table[i].size;
    size_t capacity = !table_capacity ? 4096 : live * 2 > table_capacity ? table_capacity * 2 : table_capacity;
    Slot *grown = calloc(capacity, sizeof(*grown));
    if (!grown) return -1;
    for (size_t i = 0; i < table_capacity; ++i) {
        if (!table[i].addr || !table[i].size) continue;
        size_t j = hash(table[i].addr) & (capacity - 1);
        while (grown[j].addr) j = (j + 1) & (capacity - 1);
        grown[j] = table[i];
    }
    free(table); table = grown; table_capacity = capacity; table_used = live;
    return 0;
}
static int table_put(uint32_t addr, uint32_t size) {
    if ((table_used + 1) * 4 >= table_capacity * 3 && table_grow()) return -1;
    size_t i = hash(addr) & (table_capacity - 1);
    while (table[i].addr && table[i].size) i = (i + 1) & (table_capacity - 1);
    if (!table[i].addr) ++table_used;
    table[i] = (Slot){addr, size};
    return 0;
}
static Slot *table_find(uint32_t addr) {
    if (!table_capacity) return NULL;
    size_t i = hash(addr) & (table_capacity - 1);
    while (table[i].addr) {
        if (table[i].addr == addr && table[i].size) return &table[i];
        i = (i + 1) & (table_capacity - 1);
    }
    return NULL;
}

static int push(Stack *s, uint32_t value) {
    if (s->count == s->capacity) {
        size_t capacity = s->capacity ? s->capacity * 2 : 64;
        uint32_t *grown = realloc(s->items, capacity * sizeof(*grown));
        if (!grown) return -1;
        s->items = grown; s->capacity = capacity;
    }
    s->items[s->count++] = value;
    return 0;
}

static uint32_t bump(uint32_t size) {
    if (size > HEAP_END - top) return 0;
    uint32_t addr = top, end = top + size;
    if (end > mapped) {
        uint64_t want = ((uint64_t)end + MAP_STEP - 1) & ~(uint64_t)(MAP_STEP - 1);
        if (want > HEAP_END) want = HEAP_END;
        if (rt_map(mapped, want - mapped)) return 0;
        mapped = (uint32_t)want;
    }
    top = end;
    return addr;
}

static uint32_t large_alloc(uint32_t size) {
    for (size_t i = 0; i < large_count; ++i) {
        if (free_large[i].size < size) continue;
        uint32_t addr = free_large[i].addr;
        if (free_large[i].size == size) {
            memmove(&free_large[i], &free_large[i + 1], (large_count - i - 1) * sizeof(*free_large));
            --large_count;
        } else { free_large[i].addr += size; free_large[i].size -= size; }
        return addr;
    }
    return bump(size);
}
static int large_free(uint32_t addr, uint32_t size) {
    size_t i = 0;
    while (i < large_count && free_large[i].addr < addr) ++i;
    int left = i > 0 && free_large[i - 1].addr + free_large[i - 1].size == addr;
    int right = i < large_count && addr + size == free_large[i].addr;
    if (left && right) {
        free_large[i - 1].size += size + free_large[i].size;
        memmove(&free_large[i], &free_large[i + 1], (large_count - i - 1) * sizeof(*free_large));
        --large_count;
    } else if (left) free_large[i - 1].size += size;
    else if (right) { free_large[i].addr = addr; free_large[i].size += size; }
    else {
        if (large_count == large_capacity) {
            size_t capacity = large_capacity ? large_capacity * 2 : 64;
            Extent *grown = realloc(free_large, capacity * sizeof(*grown));
            if (!grown) return -1;
            free_large = grown; large_capacity = capacity;
        }
        memmove(&free_large[i + 1], &free_large[i], (large_count - i) * sizeof(*free_large));
        free_large[i] = (Extent){addr, size};
        ++large_count;
    }
    return 0;
}

static uint32_t alloc_locked(uint32_t size) {
    if (!size) size = 1;
    uint32_t rounded, addr;
    if (size <= SMALL_MAX) {
        unsigned k = class_of(size, &rounded);
        Stack *s = &free_small[k];
        addr = s->count ? s->items[--s->count] : bump(rounded);
    } else {
        if (size > HEAP_END - HEAP_BEGIN - LARGE_ALIGN) return 0;
        rounded = (size + LARGE_ALIGN - 1) & ~(LARGE_ALIGN - 1);
        addr = large_alloc(rounded);
    }
    if (!addr) {
        if (getenv("P2_TRACE_MMAP"))
            fprintf(stderr, "[heap] échec malloc(%u) : arène %u Mio, vivant %llu Mio, %zu zones libres\n",
                    size, (top - HEAP_BEGIN) >> 20, (unsigned long long)(live_bytes >> 20), large_count);
        return 0;
    }
    if (table_put(addr, rounded)) return 0;   // fuite d'un bloc, échec propre
    live_bytes += rounded;
    return addr;
}

// Libère un bloc connu ; -1 si l'adresse n'a pas été rendue par ce tas.
static int free_locked(uint32_t ptr) {
    Slot *slot = table_find(ptr);
    if (!slot) return -1;
    uint32_t size = slot->size;
    slot->size = 0;                     // supprimé (la case reste occupée pour le sondage)
    live_bytes -= size;
    if (size <= SMALL_MAX) {
        uint32_t rounded; unsigned k = class_of(size, &rounded);
        return push(&free_small[k], ptr);
    }
    return large_free(ptr, size);
}

uint32_t rt_alloc(uint32_t size) {
    pthread_mutex_lock(&lock);
    uint32_t addr = alloc_locked(size);
    pthread_mutex_unlock(&lock);
    return addr;
}

int rt_free(uint32_t ptr) {
    if (!ptr) return 0;
    pthread_mutex_lock(&lock);
    int result = free_locked(ptr);
    pthread_mutex_unlock(&lock);
    return result;
}

uint32_t rt_heap_usable_size(uint32_t ptr) {
    pthread_mutex_lock(&lock);
    Slot *slot = ptr ? table_find(ptr) : NULL;
    uint32_t size = slot ? slot->size : 0;
    pthread_mutex_unlock(&lock);
    return size;
}

uint32_t rt_realloc(uint32_t ptr, uint32_t size) {
    if (!ptr) return rt_alloc(size);
    if (!size) { rt_free(ptr); return 0; }
    pthread_mutex_lock(&lock);
    Slot *slot = table_find(ptr);
    if (!slot) { pthread_mutex_unlock(&lock); return 0; }
    uint32_t old = slot->size;
    if (size <= old) { pthread_mutex_unlock(&lock); return ptr; }
    uint32_t result = alloc_locked(size);
    if (result) {
        memcpy(GPTR(result), GPTR(ptr), old);
        free_locked(ptr);
    }
    pthread_mutex_unlock(&lock);
    return result;   // 0 : l'allocation d'origine est conservée
}

void rt_heap_info(uint32_t *arena, uint32_t *used, uint32_t *free_bytes, uint32_t *free_blocks) {
    pthread_mutex_lock(&lock);
    *arena = top - HEAP_BEGIN;
    *used = (uint32_t)live_bytes;
    *free_bytes = *arena - *used;
    size_t blocks = large_count;
    for (unsigned k = 0; k < CLASS_COUNT; ++k) blocks += free_small[k].count;
    *free_blocks = blocks > UINT32_MAX ? UINT32_MAX : (uint32_t)blocks;
    pthread_mutex_unlock(&lock);
}

size_t rt_heap_table_capacity(void) {
    pthread_mutex_lock(&lock);
    size_t capacity = table_capacity;
    pthread_mutex_unlock(&lock);
    return capacity;
}

void rt_heap_reset(void) {
    pthread_mutex_lock(&lock);
    for (unsigned k = 0; k < CLASS_COUNT; ++k) { free(free_small[k].items); free_small[k] = (Stack){0}; }
    free(free_large); free_large = NULL; large_count = large_capacity = 0;
    free(table); table = NULL; table_capacity = table_used = 0;
    top = mapped = HEAP_BEGIN; live_bytes = 0;
    pthread_mutex_unlock(&lock);
}
