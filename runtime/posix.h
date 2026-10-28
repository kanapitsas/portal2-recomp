// Interfaces internes du domaine POSIX (libc.c, paths.c, posix.c, process.c,
// net.c). Non inclus par rt.h pour ne pas toucher l'en-tête commun.
#pragma once
#include "rt.h"
#include <limits.h>
#include <stdio.h>

// Erreurs : errno Darwin -> errno Linux i386 et inverse (strerror).
int rt_linux_errno(int darwin);
int rt_darwin_errno(int linux_value);
// Signaux : numéros Linux i386 <-> Darwin. 0 si pas d'équivalent.
int rt_linux_signal(int darwin);
int rt_darwin_signal(int linux_value);
// Statut wait() Darwin -> encodage Linux (numéros de signaux traduits).
uint32_t rt_linux_wait_status(int status);

// Flux invités (libc.c) : enveloppe un FILE hôte dans un handle invité.
uint32_t rt_stream_new(FILE *file);
// Retire un flux sans le fermer (pclose/freopen échoué) ; renvoie le FILE.
FILE *rt_stream_take(CPU *c, uint32_t handle);
// Recopie l'environnement hôte dans les objets invités `environ`.
int rt_environ_sync(void);
// Toutes les sources d'un répertoire invité sont fusionnées (libc.c).
// Remplit un tableau de noms (sans doublon, l'utilisateur masque l'original,
// les suppressions sont honorées). Libérer avec rt_dir_entries_free.
typedef struct { char *name; uint64_t ino; uint8_t type; } RtDirEntry;
int rt_dir_entries(const char *guest_path, RtDirEntry **entries, size_t *count);
void rt_dir_entries_free(RtDirEntry *entries, size_t count);

// Couches de fichiers (paths.c). Un chemin invité sous la racine du jeu a
// jusqu'à trois sources : données utilisateur, dépôt Linux, copie du jeu.
// Ces fonctions renvoient -1 avec errno en cas d'erreur.
// Chemin relatif à la racine du jeu, ou 0 si le chemin est hors du jeu.
int rt_path_relative(const char *path, char relative[PATH_MAX]);
// Chemin de la copie utilisateur (sans créer de répertoire).
int rt_path_overlay(const char *relative, char out[PATH_MAX]);
// Sources originales en lecture seule (dépôt Linux, puis jeu) : 0, 1 ou 2.
int rt_path_lower(const char *relative, char out[2][PATH_MAX]);
// Répertoire des marques de suppression, à la racine des données utilisateur.
#define RT_WHITEOUTS ".portal2-whiteouts"
// Suppression logique d'un fichier original (comme un « whiteout » overlayfs).
int rt_path_whiteout(const char *relative);
int rt_path_is_whiteout(const char *relative);
int rt_path_clear_whiteout(const char *relative);
// Nom d'une source originale masqué dans un répertoire relatif (supprimé
// par l'invité, ou répertoire interne des suppressions).
int rt_path_hidden_name(const char *relative_dir, const char *name);
// rt_write_path sans suivre un lien final (rename, lchown, unlink...).
const char *rt_write_path_nofollow(const char *path);
// Chemin canonique tel que l'invité le voit (realpath invité).
int rt_guest_realpath(const char *path, char out[PATH_MAX]);

// Statistiques du tas invité glibc (shims.c).
void rt_heap_info(uint32_t *arena, uint32_t *used, uint32_t *free_bytes, uint32_t *free_blocks);

int rt_path_windows_binary(const char *relative);   // .dll/.exe/.asi/.flt... de la copie Windows, sauf greffon traduit
int rt_path_is_game_source(const char *host);
unsigned long rt_paths_generation(void);   // change à chaque écriture invitée (paths.c)
int rt_path_absolute(const char *path, char out[PATH_MAX]);   // chemin hôte absolu normalisé
uint32_t rt_vm_mapped_bytes(void);   // diagnostic (vm.c)
int rt_vm_describe(uint32_t address, char *out, size_t size);   // diagnostic de faute (vm.c)
int rt_access_cached(const char *path, int mode);   // 0, -1 (errno), -2 non mémorisable
