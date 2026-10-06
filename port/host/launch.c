/* launch.c: where the native build finds the game and keeps its own files (port_host.h).

   The game folder (the install: ARENA2 and the .BNK and .CFG files, only read) comes from
   --game, else $DAGGER_GAME, else the one remembered in the settings, else the player picks it
   in a folder dialog the first time (a folder with ARENA2 in it, or its DAGGER folder).

   The overlay (where the game's writes go: saves, its config, the archives unpacked from
   PACKED.DAT) comes from --overlay, else $DAGGER_OVERLAY, else
   ~/Library/Application Support/Daggerfall. The settings file is there too. */
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

#include <SDL3/SDL.h>

#include "port_host.h"

static char overlay_path[1024], game_path[1024], settings_path[1100];

static int has_arena2(const char *dir)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    int found = 0;

    if (d == NULL)
        return 0;
    while (!found && (e = readdir(d)) != NULL)
        found = strcasecmp(e->d_name, "ARENA2") == 0;
    closedir(d);
    return found;
}

/* the install in dir, or in its DAGGER folder */
static int find_install(const char *dir, char *out, size_t size)
{
    if (dir == NULL || *dir == 0)
        return 0;
    if (has_arena2(dir)) {
        snprintf(out, size, "%s", dir);
        return 1;
    }
    snprintf(out, size, "%s/DAGGER", dir);
    return has_arena2(out);
}

static void make_dirs(const char *path)
{
    char p[1024];
    size_t i;

    snprintf(p, sizeof p, "%s", path);
    for (i = 1; p[i]; i++) {
        if (p[i] == '/') {
            p[i] = 0;
            mkdir(p, 0755);
            p[i] = '/';
        }
    }
    mkdir(p, 0755);
}

const char *launch_overlay_dir(const char *given)
{
    const char *home = getenv("HOME");

    if (given == NULL)
        given = getenv("DAGGER_OVERLAY");
    if (given != NULL)
        snprintf(overlay_path, sizeof overlay_path, "%s", given);
    else
        snprintf(overlay_path, sizeof overlay_path, "%s/Library/Application Support/Daggerfall",
                 home ? home : ".");
    make_dirs(overlay_path);
    snprintf(settings_path, sizeof settings_path, "%s/settings.txt", overlay_path);
    return overlay_path;
}

static int read_setting(const char *key, char *out, size_t size)
{
    char line[1200];
    size_t k = strlen(key);
    FILE *f = fopen(settings_path, "r");

    if (f == NULL)
        return 0;
    while (fgets(line, sizeof line, f) != NULL) {
        if (strncmp(line, key, k) == 0 && line[k] == '=') {
            line[strcspn(line, "\r\n")] = 0;
            snprintf(out, size, "%s", line + k + 1);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

static void write_setting(const char *key, const char *value)
{
    FILE *f = fopen(settings_path, "w");

    if (f == NULL)
        return;
    fprintf(f, "%s=%s\n", key, value);
    fclose(f);
}

struct pick {
    int done;
    char path[1024];
};

static void SDLCALL picked(void *userdata, const char *const *files, int filter)
{
    struct pick *p = userdata;

    (void)filter;
    if (files != NULL && files[0] != NULL)
        snprintf(p->path, sizeof p->path, "%s", files[0]);
    p->done = 1;
}

/* the folder dialog, until the player picks an install or gives up */
static int ask_for_install(char *out, size_t size)
{
    for (;;) {
        struct pick p;
        memset(&p, 0, sizeof p);
        SDL_ShowOpenFolderDialog(picked, &p, NULL, getenv("HOME"), false);
        while (!p.done)
            SDL_WaitEventTimeout(NULL, 50);
        if (p.path[0] == 0)
            return 0;
        if (find_install(p.path, out, size))
            return 1;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Daggerfall",
                                 "That folder has no ARENA2 folder. Choose the folder Daggerfall "
                                 "is installed in (the one with ARENA2 and FALL.EXE).", NULL);
    }
}

const char *launch_game_dir(const char *given)
{
    char remembered[1024];

    if (given == NULL)
        given = getenv("DAGGER_GAME");
    if (given != NULL)
        return given;
    if (read_setting("game", remembered, sizeof remembered) &&
        find_install(remembered, game_path, sizeof game_path))
        return game_path;
    if (!SDL_Init(SDL_INIT_VIDEO))
        return NULL;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Daggerfall",
                             "Choose the folder Daggerfall is installed in (the one with the "
                             "ARENA2 folder). The game's files are only read; saves go to "
                             "~/Library/Application Support/Daggerfall.", NULL);
    if (!ask_for_install(game_path, sizeof game_path))
        return NULL;
    write_setting("game", game_path);
    return game_path;
}
