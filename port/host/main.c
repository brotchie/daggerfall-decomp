/* main.c: the native build's entry (docs/port.md). Starts SDL, points the DOS file layer at
   the game's folders and calls the game's main (FALL.EXE 0x10010) with its one argument, the
   config file, as the DOS game was started (`FALL.EXE Z.CFG`).

   usage: fall [--game DIR] [--overlay DIR] [--nosound] [CONFIG]
     --game DIR     the installed game (only read): ARENA2, the .BNK and .CFG files
     --overlay DIR  where the game's writes go (saves, its config), read before DIR
     --nosound      keep the install's HMISET.CFG ("No Digital Device"), as the emulator runs;
                    otherwise the overlay gets one with a Sound Blaster 16: its sound effects,
                    and its OPL3 for the music
     CONFIG         the config file, Z.CFG by default (written to the overlay when missing) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <SDL3/SDL.h>

#include "port_host.h"
#include "port_vpc.h"

extern int func_00010010(short argc, char **argv);     /* the game's main */

/* what the installer writes, as tools/fallemu.py makes it */
static const char z_cfg[] = "type 4\r\npath C:\\ARENA2\\\r\npathcd C:\\ARENA2\\\r\n"
                            "maps mapsave.sav\r\nmapfile maps.bsa\r\ncontrols 1\r\n";

/* HMI's sound setup with a Sound Blaster 16: its digital device (SETUP.INI's digital 003) and
   its FM synthesiser (MIDI 006, the OPL3 at 0x388) */
static const char hmiset_sb16[] =
    "[DIGITAL]\r\nDeviceName  = Sound Blaster 16/AWE32    \r\nDeviceIRQ   = 5\r\n"
    "DeviceDMA   = 1\r\nDevicePort  = 0x220\r\nDeviceID    = 0xe015\r\n\r\n"
    "[MIDI]\r\nDeviceName  = Sound Blaster 16          \r\nDevicePort  = 0x388\r\n"
    "DeviceID    = 0xa009\r\n";

static char *game_argv[3];

/* the game's thread (vpc_run): FALL.EXE's main */
static int run_game(void *arg)
{
    (void)arg;
    return func_00010010(2, game_argv);
}

int main(int argc, char **argv)
{
    const char *game = getenv("DAGGER_GAME");
    const char *overlay = getenv("DAGGER_OVERLAY");
    const char *config = "Z.CFG";
    int nosound = 0;
    char path[1024];
    int i, code;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--game") == 0 && i + 1 < argc)
            game = argv[++i];
        else if (strcmp(argv[i], "--overlay") == 0 && i + 1 < argc)
            overlay = argv[++i];
        else if (strcmp(argv[i], "--nosound") == 0)
            nosound = 1;
        else
            config = argv[i];
    }
    host_install_fault_handlers();
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

    if (!nosound) {
        char hmi[1024];
        FILE *f;
        snprintf(hmi, sizeof hmi, "%s/HMISET.CFG", overlay ? overlay : "overlay");
        if (access(hmi, F_OK) != 0 && (f = fopen(hmi, "wb")) != NULL) {
            fwrite(hmiset_sb16, 1, sizeof hmiset_sb16 - 1, f);
            fclose(f);
        }
    }

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    host_started();
    if (vpc_init("Daggerfall") != 0) {
        fprintf(stderr, "port: no memory for the virtual PC\n");
        return 1;
    }

    game_argv[0] = "FALL.EXE";
    game_argv[1] = (char *)config;
    game_argv[2] = NULL;
    code = vpc_run(run_game, NULL);
    host_shutdown();
    return code;
}
