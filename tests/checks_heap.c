#include "checks.h"
#include <pthread.h>
#include <time.h>

RT_CHECK(heap_reuses_frees_and_rejects_unknown_addresses) {
    uint32_t a = rt_alloc(24), b = rt_alloc(24);
    assert(a && b && a != b && !(a & 15) && !(b & 15));
    assert(rt_heap_usable_size(a) == 32);                       // classe de 32 octets
    uint32_t m1 = rt_alloc(100000), m2 = rt_alloc(100000);
    assert(m1 && m2 && rt_heap_usable_size(m1) == 102400);      // zones de 4 Kio au-delà de 16 Kio
    assert(!rt_free(m1) && !rt_free(m2));
    uint32_t m3 = rt_alloc(150000);                              // réutilise les deux zones fusionnées
    assert(m3 == m1 || m3 < m2);
    assert(!rt_free(m3));
    memset(GPTR(a), 0xab, 24);
    assert(rt_free(a) == 0 && rt_heap_usable_size(a) == 0);
    assert(rt_free(a) == -1);                                     // double libération détectée
    assert(rt_free(a + 16) == -1 && rt_free(0x10000000) == -1);  // adresses inconnues
    assert(rt_alloc(20) == a);                                    // même classe : réutilisé
    // realloc : agrandit en recopiant, garde le bloc si la taille tient.
    uint32_t r = rt_realloc(a, 30);
    assert(r == a);
    memcpy(GPTR(r), "0123456789abcdefghijklmnopqrst", 30);
    uint32_t grown = rt_realloc(r, 5000);
    assert(grown && grown != r && !memcmp(GPTR(grown), "0123456789abcdefghijklmnopqrst", 30));
    assert(rt_heap_usable_size(r) == 0 && rt_heap_usable_size(grown) >= 5000);
    assert(rt_free(grown) == 0 && rt_free(b) == 0);
}

RT_CHECK(heap_large_blocks_coalesce) {
    uint32_t x = rt_alloc(3u << 20), y = rt_alloc(3u << 20), z = rt_alloc(3u << 20);
    assert(x && y && z && y == x + (3u << 20) && z == y + (3u << 20));
    W8(z + (3u << 20) - 1, 7);                                    // fin du bloc projetée
    assert(!rt_free(x) && !rt_free(z) && !rt_free(y));           // trois zones fusionnées
    uint32_t big = rt_alloc(9u << 20);
    assert(big == x);                                             // tient dans la zone fusionnée
    assert(!rt_free(big));
}

static void *churn(void *seed) {
    uint32_t live[64] = {0};
    unsigned s = (unsigned)(uintptr_t)seed;
    for (int i = 0; i < 20000; ++i) {
        s = s * 1103515245u + 12345u;
        unsigned k = (s >> 8) & 63;
        if (live[k]) { assert(R8(live[k]) == (uint8_t)k); assert(!rt_free(live[k])); live[k] = 0; }
        else { live[k] = rt_alloc(1 + ((s >> 16) & 2047)); assert(live[k]); W8(live[k], k); }
    }
    for (int k = 0; k < 64; ++k) if (live[k]) assert(!rt_free(live[k]));
    return NULL;
}
RT_CHECK(heap_concurrent_churn) {
    pthread_t threads[4];
    for (uintptr_t i = 0; i < 4; ++i) assert(!pthread_create(&threads[i], NULL, churn, (void *)(i + 1)));
    for (int i = 0; i < 4; ++i) assert(!pthread_join(threads[i], NULL));
}

// L'ancien tas (liste chaînée) était quadratique : 50 000 blocs vivants puis
// libérés prenaient des dizaines de secondes. Borne large pour UBSan.
RT_CHECK(heap_scales_linearly) {
    enum { N = 50000 };
    static uint32_t blocks[N];
    struct timespec t0, t1; clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; ++i) assert((blocks[i] = rt_alloc(16 + (uint32_t)(i % 200))));
    for (int i = 0; i < N; i += 2) assert(!rt_free(blocks[i]));
    for (int i = 1; i < N; i += 2) assert(!rt_free(blocks[i]));
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double seconds = (double)(t1.tv_sec - t0.tv_sec) + (double)(t1.tv_nsec - t0.tv_nsec) / 1e9;
    assert(seconds < 1.0);
}

// Blocs libérés puis jamais réutilisés (adresses toujours nouvelles) : les
// cases supprimées remplissent la table, qui doit se réorganiser à taille
// égale au lieu de doubler à chaque fois.
RT_CHECK(heap_table_stays_bounded_under_churn) {
    enum { N = 3000 };
    static uint32_t blocks[N];
    size_t before = rt_heap_table_capacity();
    for (uint32_t size = 16; size <= 1024; size += 16) {   // une classe par tour : adresses nouvelles
        for (int i = 0; i < N; ++i) assert((blocks[i] = rt_alloc(size)));
        for (int i = 0; i < N; ++i) assert(!rt_free(blocks[i]));
    }
    size_t after = rt_heap_table_capacity();
    assert(after <= (before > 8192 ? before : 8192));
}
