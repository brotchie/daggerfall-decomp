/* main.c: the native build's entry (docs/port.md). Starts SDL, points the DOS file layer at
   the game's folders and calls the game's main (FALL.EXE 0x10010) with its one argument, the
   config file, as the DOS game was started (`FALL.EXE Z.CFG`).

   usage: fall [--game DIR] [--overlay DIR] [--nosound] [--gl] [--modern] [CONFIG]
     --game DIR     the installed game (only read): ARENA2, the .BNK and .CFG files; else
                    $DAGGER_GAME, the one remembered, or a folder dialog (launch.c)
     --overlay DIR  where the game's writes go (saves, its config), read before DIR; else
                    $DAGGER_OVERLAY or ~/Library/Application Support/Daggerfall
     --nosound      keep the install's HMISET.CFG ("No Digital Device"), as the emulator runs;
                    otherwise the overlay gets one with a Sound Blaster 16: its sound effects,
                    and its OPL3 for the music
     --gl           draw the 3D view with OpenGL (as PORT_GL=1; port/include/port_gl.h)
     --modern       modern controls (as PORT_CONTROLS=modern): mouse look, WASD, Shift to run
                    (port_controls_loaded below)
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

/* ---- modern controls ----------------------------------------------------------------------- */

/* The game's controls (docs/state.md "Input"): key_map holds the 38 actions of the CONTROLS
   screen as scan codes (202 is mouse button 1), mouse_control_mode 1 is VIEW: the mouse turns
   and pitches the player (player_mouse_look), ACTIVATE CURSOR toggles a cursor for the HUD. */
extern unsigned char key_map[38], default_key_map[38];
extern unsigned char mouse_control_mode, view_cursor_active, game_mode;
extern unsigned char D_00196272;            /* screen_active: the sheet, travel map, automap */
extern unsigned char D_00187CA8;            /* world_running: 0 while the inventory is open */
extern int xn_cam_centre_x, xn_cam_centre_y;

/* the modern scheme over the classic keys: {action, scan code or binding} */
static const unsigned char modern_keys[][2] = {
    {0, 0x11},      /* FORWARD: W (was Up, which still turns: TURN keys stay the arrows) */
    {1, 0x1F},      /* REVERSE: S */
    {3, 0x1E},      /* SLIDE LEFT: A */
    {5, 0x20},      /* SLIDE RIGHT: D */
    {8, 0x39},      /* JUMP: Space */
    {9, 0x2E},      /* CROUCH: C */
    {11, 0x2A},     /* RUN: left Shift, held */
    {18, 0x12},     /* ACTIVATE CENTER OBJECT: E, what the crosshair is on */
    {19, 0x0F},     /* ACTIVATE CURSOR: Tab, the cursor for the HUD and back */
    {27, 0x2F},     /* TRAVEL MAP: V (was W) */
    {30, 0x22},     /* ABORT SPELL: G (was E) */
    {32, 0x21},     /* READY WEAPON: F (was A) */
    {33, 202},      /* SWING WEAPON: left mouse button, held while the mouse moves */
    {34, 0x23},     /* SWITCH HAND: H (was S) */
    {35, 0x24},     /* STATUS: J (was I) */
    {37, 0x17},     /* INVENTORY: I (was F6) */
};

/* init_game_data's last word on the controls (src/hand/func_0004ED36.c, DAGGER_PORT): with
   PORT_CONTROLS=modern, controls still at the classic defaults (no controls file, or one saved
   unchanged) become the modern scheme in VIEW mode. Controls changed on the CONTROLS screen
   are the player's and stay. */
void port_controls_loaded(void)
{
    const char *c = getenv("PORT_CONTROLS");
    size_t k;

    if (c == NULL || strcmp(c, "modern") != 0)
        return;
    if (memcmp(key_map, default_key_map, sizeof key_map) != 0) {
        fprintf(stderr, "port: the controls were changed on the CONTROLS screen: kept\n");
        return;
    }
    for (k = 0; k < sizeof modern_keys / sizeof modern_keys[0]; k++)
        key_map[modern_keys[k][0]] = modern_keys[k][1];
    mouse_control_mode = 1;
    view_cursor_active = 0;
    fprintf(stderr, "port: modern controls (mouse look, WASD, Shift runs, E uses, Tab cursor)\n");
}

/* for the video's crosshair: the mouse is looking around the world (VIEW mode, no cursor, no
   screen over the world); *cx, *cy the view's centre */
extern volatile unsigned long port_render_frames;   /* rframe.c: the 3D frames drawn */

static int view_probe(int *cx, int *cy)
{
    static unsigned long frames;
    static Uint64 moved;
    Uint64 now = SDL_GetTicksNS();

    /* the world is being drawn: a 3D frame in the last 200 ms (not the title, a dialog over
       the frozen view, or a full-screen picture) */
    if (port_render_frames != frames) {
        frames = port_render_frames;
        moved = now;
    }
    if (now - moved > 200000000ull)
        return 0;
    if (mouse_control_mode != 1 || view_cursor_active || game_mode || D_00196272 ||
        !D_00187CA8)
        return 0;
    *cx = xn_cam_centre_x;
    *cy = xn_cam_centre_y;
    return 1;
}

extern unsigned char player_on_ground, in_dungeon_water, player_motion_flags;
extern unsigned char mouse_buttons, key_down[];
extern unsigned char mouse_turn_rate;
extern int xn_cam_pitch;

/* for the video's smooth look: the next frame adds the mouse's motion to the view, as
   player_mouse_look does (src/hand/func_00081096.c: VIEW mode without the cursor, on the
   ground, in water or climbing, SWING WEAPON not held). *reverse: the vertical sign (REVERSE
   VERTICAL); *pitch: the view's pitch now (-256..256 of 2048 a turn) */
static int look_probe(int *reverse, int *pitch)
{
    unsigned int b = key_map[33];
    int cx, cy;

    if (!view_probe(&cx, &cy))
        return 0;
    if (!(player_on_ground || in_dungeon_water || (player_motion_flags & 0x20)))
        return 0;
    if (b < 200 ? key_down[b] : b == 202 ? mouse_buttons & 1 : b == 203 ? mouse_buttons & 2
                                : b == 212 ? mouse_buttons & 4 : 0)
        return 0;
    *reverse = mouse_turn_rate & 0x80 ? -1 : 1;
    *pitch = xn_cam_pitch > 1023 ? xn_cam_pitch - 2048 : xn_cam_pitch;
    return 1;
}

/* the game's thread (vpc_run): FALL.EXE's main */
static int run_game(void *arg)
{
    (void)arg;
    return func_00010010(2, game_argv);
}

int main(int argc, char **argv)
{
    const char *game = NULL;
    const char *overlay = NULL;
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
        else if (strcmp(argv[i], "--gl") == 0)
            setenv("PORT_GL", "1", 1);
        else if (strcmp(argv[i], "--modern") == 0)
            setenv("PORT_CONTROLS", "modern", 1);
        else if (strncmp(argv[i], "-psn_", 5) == 0)
            ;                           /* what Finder passed to apps on older macOS */
        else
            config = argv[i];
    }
    host_install_fault_handlers();
    overlay = launch_overlay_dir(overlay);
    game = launch_game_dir(game);
    if (game == NULL) {
        fprintf(stderr, "usage: fall --game DIR [--overlay DIR] [--nosound] [--gl] [--modern] [CONFIG]\n");
        return 2;
    }
    dos_set_dirs(game, overlay);
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

    port_unpack_packed();

    if (!nosound) {
        char hmi[1024];
        FILE *f;
        snprintf(hmi, sizeof hmi, "%s/HMISET.CFG", overlay);
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

    vpc_set_view_probe(view_probe);
    vpc_set_look_probe(look_probe);
    game_argv[0] = "FALL.EXE";
    game_argv[1] = (char *)config;
    game_argv[2] = NULL;
    code = vpc_run(run_game, NULL);
    host_shutdown();
    return code;
}
