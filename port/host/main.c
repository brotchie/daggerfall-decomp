/* main.c: the native build's entry (docs/port.md). Starts SDL, points the DOS file layer at
   the game's folders and calls the game's main (FALL.EXE 0x10010) with its one argument, the
   config file, as the DOS game was started (`FALL.EXE Z.CFG`).

   usage: fall [--game DIR] [--overlay DIR] [CONFIG]
     --game DIR     the installed game (only read): ARENA2, the .BNK and .CFG files
     --overlay DIR  where the game's writes go (saves, its config), read before DIR
     CONFIG         the config file, Z.CFG by default (written to the overlay when missing) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_host.h"

extern int func_00010010(short argc, char **argv);     /* the game's main */

/* what the installer writes, as tools/fallemu.py makes it */
static const char z_cfg[] = "type 4\r\npath C:\\ARENA2\\\r\npathcd C:\\ARENA2\\\r\n"
                            "maps mapsave.sav\r\nmapfile maps.bsa\r\ncontrols 1\r\n";

static int sdl_started;

void host_shutdown(void)
{
    if (sdl_started) {
        SDL_Quit();
        sdl_started = 0;
    }
}

void port_unimplemented(const char *name)
{
    fprintf(stderr, "port: %s is not in the native build yet\n", name);
    host_shutdown();
    abort();
}

int main(int argc, char **argv)
{
    const char *game = getenv("DAGGER_GAME");
    const char *overlay = getenv("DAGGER_OVERLAY");
    const char *config = "Z.CFG";
    char path[1024];
    char *game_argv[3];
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--game") == 0 && i + 1 < argc)
            game = argv[++i];
        else if (strcmp(argv[i], "--overlay") == 0 && i + 1 < argc)
            overlay = argv[++i];
        else
            config = argv[i];
    }
    if (game == NULL) {
        fprintf(stderr, "usage: fall --game DIR [--overlay DIR] [CONFIG]\n");
        return 2;
    }
    dos_set_dirs(game, overlay ? overlay : "overlay");
    if (!dos_host_path(config, 0, path, sizeof path)) {
        FILE *f;
        dos_host_path(config, 1, path, sizeof path);
        f = fopen(path, "wb");
        if (f == NULL) {
            perror(path);
            return 1;
        }
        fwrite(z_cfg, 1, sizeof z_cfg - 1, f);
        fclose(f);
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    sdl_started = 1;

    game_argv[0] = "FALL.EXE";
    game_argv[1] = (char *)config;
    game_argv[2] = NULL;
    func_00010010(2, game_argv);
    host_shutdown();
    return 0;
}
