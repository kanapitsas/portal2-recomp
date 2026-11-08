#include "rt.h"
#include "game_data.h"
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>

// Portal 2 initializes its puzzle editor even for the campaign. Missing item
// definitions produce a null ITEM_ENTRY_DOOR and a crash at guest address 0x7c.
// Check the effective read path (including user overrides and whiteouts), not
// just the original Steam folder. This is a presence check, not a KV parser.
int rt_validate_game_data(const char *program, const char *game, FILE *errors) {
    if (strcmp(program, "portal2_linux")) return 0;
    const char *relative = "portal2_dlc2/scripts/editoritems.txt";
    char path[PATH_MAX];
    int length = snprintf(path, sizeof(path), "%s/%s", game, relative);
    const char *resolved = length >= 0 && (size_t)length < sizeof(path) ? rt_read_path(path) : NULL;
    FILE *file = resolved ? fopen(resolved, "rb") : NULL;
    struct stat info;
    int valid = file && !fstat(fileno(file), &info) && S_ISREG(info.st_mode) && fgetc(file) != EOF;
    if (file) fclose(file);
    if (valid) return 0;
    if (errors) fprintf(errors,
        "Portal 2 game data is incomplete: %s is missing, unreadable or empty.\n"
        "Game folder: %s\nEffective file: %s\n"
        "An empty placeholder does not help: the game needs the real file. In Steam, right-click\n"
        "Portal 2 > Properties > Installed Files > Verify integrity of game files, or download the\n"
        "complete game data with download_depot 620 621 in the Steam console and choose depot_621\n"
        "in Natif Installer. No rebuild is needed: the data is read at launch.\n"
        "If you use a custom data folder, verify that folder, not a different Steam installation.\n",
        relative, game, resolved ? resolved : "not found (check user overrides too)");
    return -1;
}
