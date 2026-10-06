/* vpcdemo.c: the virtual PC (port/include/port_vpc.h) on its own, driven the way XnGine drives
   a PC, with the game's own files:
   - mode 13h through int 10h, a full-screen IMG into VGA memory at 0xA0000, its palette
     through the DAC's ports (3C8h, 3C9h);
   - a keyboard handler installed on int 9 with _dos_setvect, reading port 60h and chaining
     to the BIOS's, as xn_kbd_install does;
   - the mouse through int 33h (reset, ranges, position, mickeys), with a cursor drawn into
     VGA memory;
   - frames paced by the retrace at port 3DAh; the BIOS tick at 0x46C;
   - a DAGGER.SND sound through HMI SOS's calls (port/shim/sos.c), on a click or Space;
   - a song from MIDI.BSA (--song, D1.HMI by default) through SOS's MIDI calls as sos_init and
     music_play make them: HMI's OPL3 driver from HMIMDRV.386 on the virtual OPL3.

   usage: vpcdemo --game DIR [--image NAME.IMG] [--sound N] [--song NAME.HMI] [--frames N]
                  [--shot FILE] [--selftest]
   --selftest feeds SDL key and mouse events in and checks what comes out at the ports and
   services; it prints PASS or FAIL for each check and exits 0 when all pass. With
   SDL_VIDEO_DRIVER=offscreen SDL_AUDIO_DRIVER=dummy it runs without a window. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_host.h"
#include "port_vpc.h"

/* HMI SOS as the game calls it (port/shim/sos.c), by FALL.EXE's addresses */
typedef unsigned int W32;
W32 func_0009E1A1(W32 rate, W32 debug);                 /* sosTIMERInitSystem */
W32 func_0009E8FF(const char *path, W32 debug);         /* sosDIGIInitSystem */
W32 func_0009F4DE(void *driver, W32 *handle);           /* sosDIGIInitDriver */
W32 func_000A2504(W32 h, const void *start);            /* sosDIGIStartSample */
W32 func_000A2460(W32 h, W32 sample);                   /* sosDIGISampleDone */
W32 func_0009E9C2(const char *path, W32 debug);         /* sosMIDIInitSystem */
W32 func_0009EC82(const W32 *hardware, W32 *handle);    /* sosMIDIInitDriver */
W32 func_0009FEE5(W32 h, const void *bank, W32 flag);   /* sosMIDISetInsData */
W32 func_000A021C(const void *song, W32 *handle);       /* sosMIDIInitSong */
W32 func_000A27A0(W32 h);                               /* sosMIDIStartSong */
W32 func_000A2941(W32 h);                               /* sosMIDISongDone */

#include "opl3.h"

#pragma pack(push, 1)
struct sos_sample {                 /* struct sos_sample (include/structs.h, port/shim/sos.c) */
    char *data;
    char pad04[8];
    int length, length2;
    char pad14[24];
    int volume, loop, rate, bits, channels, format, pan;
    char pad48[168];
};
#pragma pack(pop)

static int fails;

static void check(int ok, const char *what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        fails++;
}

/* ---- files ------------------------------------------------------------------------------ */

static unsigned char *load(const char *dos, int *size)
{
    int fd = port_open(dos, 0x0200), n;
    unsigned char *buf;
    int len;

    if (fd < 0)
        return NULL;
    len = port_lseek(fd, 0, 2);
    port_lseek(fd, 0, 0);
    buf = malloc((size_t)len);
    n = port_read(fd, buf, (unsigned int)len);
    port_close(fd);
    *size = n;
    return buf;
}

/* record n (in directory order) of a BSA archive with numbered records (DAGGER.SND) */
static unsigned char *bsa_record(unsigned char *d, int size, int n, int *len)
{
    int count = d[0] | d[1] << 8, i, pos = 4;
    int dir = size - count * 8;

    if (n >= count)
        return NULL;
    for (i = 0; i < count; i++) {
        unsigned char *e = d + dir + i * 8;
        int sz = e[4] | e[5] << 8 | e[6] << 16 | e[7] << 24;
        if (i == n) {
            *len = sz;
            return d + pos;
        }
        pos += sz;
    }
    return NULL;
}

/* a named record of MIDI.BSA (18-byte directory entries) */
static unsigned char *bsa_named(unsigned char *d, int size, const char *name, int *len)
{
    int count = d[0] | d[1] << 8, i, pos = 4;
    int dir = size - count * 18;

    for (i = 0; i < count; i++) {
        unsigned char *e = d + dir + i * 18;
        int sz = e[14] | e[15] << 8 | e[16] << 16 | e[17] << 24;
        if (strncasecmp((char *)e, name, 14) == 0) {
            *len = sz;
            return d + pos;
        }
        pos += sz;
    }
    return NULL;
}

/* ---- the keyboard handler, as XnGine's ------------------------------------------------- */

static vpc_handler old_int9;
static unsigned char seen[64];
static int nseen;

static void demo_int9(void)
{
    unsigned char b = xn_inb(0x60);

    if (nseen < (int)sizeof seen)
        seen[nseen++] = b;
    old_int9();                     /* chain to the BIOS's: the shift flags, int 16h */
    xn_outb(0x20, 0x20);
}

/* ---- the picture -------------------------------------------------------------------------- */

static unsigned char *vga;
static unsigned char picture[64000];

static void draw_cursor(int x, int y, unsigned char colour)
{
    int i;

    memcpy(vga, picture, 64000);
    for (i = -4; i <= 4; i++) {
        if (x + i >= 0 && x + i < 320 && y >= 0 && y < 200)
            vga[y * 320 + x + i] = colour;
        if (y + i >= 0 && y + i < 200 && x >= 0 && x < 320)
            vga[(y + i) * 320 + x] = colour;
    }
}

static void wait_retrace(void)
{
    while (xn_inb(0x3DA) & 8)
        ;
    while (!(xn_inb(0x3DA) & 8))
        ;
}

static unsigned int bios_ticks(void)
{
    const unsigned char *t = VPC_LOW(0x46C);
    return (unsigned int)(t[0] | t[1] << 8 | t[2] << 16 | (unsigned int)t[3] << 24);
}

static void push_key(SDL_Scancode s, int down)
{
    SDL_Event e;

    memset(&e, 0, sizeof e);
    e.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    e.key.scancode = s;
    e.key.down = down;
    SDL_PushEvent(&e);
}

int main(int argc, char **argv)
{
    const char *game = getenv("DAGGER_GAME"), *image = "CHGN00I0.IMG", *shot = NULL;
    const char *song_name = "D1.HMI";
    int frames = 600, sound = 203, selftest = 0, i, size;
    char path[256];
    unsigned char *img, *snd = NULL, *clip = NULL;
    int snd_size = 0, clip_len = 0;
    struct vpc_regs r;
    W32 digi = 0;
    struct sos_sample start;
    char driver_block[268];
    int mouse_x = 160, mouse_y = 100;
    unsigned int t0, f0;
    Uint64 ns0;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--game") && i + 1 < argc) game = argv[++i];
        else if (!strcmp(argv[i], "--image") && i + 1 < argc) image = argv[++i];
        else if (!strcmp(argv[i], "--sound") && i + 1 < argc) sound = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--song") && i + 1 < argc) song_name = argv[++i];
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--shot") && i + 1 < argc) shot = argv[++i];
        else if (!strcmp(argv[i], "--selftest")) selftest = 1;
    }
    if (game == NULL) {
        fprintf(stderr, "usage: vpcdemo --game DIR [--image NAME.IMG] [--sound N] [--frames N]"
                        " [--shot FILE] [--selftest]\n");
        return 2;
    }
    dos_set_dirs(game, getenv("DAGGER_OVERLAY") ? getenv("DAGGER_OVERLAY") : "overlay");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    host_started();
    vpc_init("Daggerfall: virtual PC demo");
    vga = VPC_LOW(0xA0000);

    /* mode 13h, as xn_gfx_set_mode13 asks for it */
    memset(&r, 0, sizeof r);
    r.eax = 0x0013;
    xn_int10(&r);
    memset(&r, 0, sizeof r);
    r.eax = 0x0F00;
    xn_int10(&r);
    check((r.eax & 0xFF) == 0x13, "int 10h: mode 13h set and read back");

    /* the image and its palette through the DAC */
    snprintf(path, sizeof path, "ARENA2\\%s", image);
    img = load(path, &size);
    check(img != NULL && size >= 64000, "the DOS file layer: ARENA2 image read");
    if (img != NULL && size >= 64000) {
        unsigned char pal[768];
        int pal_size;
        memcpy(picture, img, 64000);
        if (size >= 64768) {
            memcpy(pal, img + 64000, 768);              /* 6-bit, after the pixels */
        } else {
            unsigned char *col = load("ARENA2\\ART_PAL.COL", &pal_size);
            for (i = 0; i < 768 && col != NULL; i++)
                pal[i] = col[8 + i] >> 2;                /* .COL: 8-bit after an 8-byte header */
            free(col);
        }
        xn_outb(0x3C8, 0);
        for (i = 0; i < 768; i++)
            xn_outb(0x3C9, pal[i]);
        xn_outb(0x3C7, 5);
        check(xn_inb(0x3C9) == pal[15] && xn_inb(0x3C9) == pal[16] && xn_inb(0x3C9) == pal[17],
              "DAC: palette written at 3C9h reads back through 3C7h");
        memcpy(vga, picture, 64000);
    }

    /* the keyboard handler and the mouse */
    old_int9 = _dos_getvect(9);
    _dos_setvect(9, demo_int9);
    memset(&r, 0, sizeof r);
    xn_int33(&r);
    check((r.eax & 0xFFFF) == 0xFFFF, "int 33h: mouse driver present");
    r.eax = 7; r.ecx = 0; r.edx = 639; xn_int33(&r);
    r.eax = 8; r.ecx = 0; r.edx = 199; xn_int33(&r);
    r.eax = 4; r.ecx = 320; r.edx = 100; xn_int33(&r);

    /* HMI SOS, as sos_init starts it with a digital device */
    snd = load("ARENA2\\DAGGER.SND", &snd_size);
    if (snd != NULL)
        clip = bsa_record(snd, snd_size, sound, &clip_len);
    func_0009E1A1(0xFF00, 0);
    func_0009E8FF(NULL, 0);
    memset(driver_block, 0, sizeof driver_block);
    func_0009F4DE(driver_block, &digi);
    memset(&start, 0, sizeof start);
    start.data = (char *)clip;
    start.length = clip_len;
    start.volume = 0x7FFF7FFF;
    start.rate = 11025;
    start.bits = 8;
    start.channels = 1;
    start.format = 0x8000;
    start.pan = 0x8000;
    if (clip != NULL) {
        W32 h = func_000A2504(digi, &start);
        check(h < 32 && func_000A2460(digi, h) == 0, "SOS: a DAGGER.SND sample starts playing");
        if (selftest) {
            Uint64 until = SDL_GetTicksNS() + 3000000000ull;
            while (func_000A2460(digi, h) == 0 && SDL_GetTicksNS() < until)
                SDL_DelayNS(10000000);
            check(func_000A2460(digi, h) == 1, "SOS: the sample finishes");
        }
    }

    /* music, as sos_init and music_play start it: an SB16's OPL3 (0xA009) at 0x388 */
    {
        static W32 hw[12];
        static struct { char *data; char pad[28]; } song_block;
        W32 hmidi = 0, hsong = 0;
        int ml = 0, dl = 0, bl = 0, sl = 0;
        unsigned char *mel = load("MELODIC.BNK", &ml), *drm = load("DRUM.BNK", &dl);
        unsigned char *midi = load("ARENA2\\MIDI.BSA", &bl);
        unsigned char *rec = midi ? bsa_named(midi, bl, song_name, &sl) : NULL;
        hw[0] = 0xA009;
        ((unsigned char *)hw)[0x22] = 0x88;         /* HMISET.CFG's DevicePort, at +0x22 */
        ((unsigned char *)hw)[0x23] = 0x03;
        func_0009E9C2(NULL, 0);
        check(func_0009EC82(hw, &hmidi) == 0, "SOS MIDI: the OPL3 driver from HMIMDRV.386");
        if (mel && drm && rec) {
            func_0009FEE5(hmidi, mel, 1);
            func_0009FEE5(hmidi, drm, 1);
            song_block.data = malloc((size_t)sl);
            memcpy(song_block.data, rec, (size_t)sl);
            check(func_000A021C(&song_block, &hsong) == 0 && func_000A27A0(hsong) == 0,
                  "SOS MIDI: a MIDI.BSA song starts");
            if (selftest) {
                int k, keys = 0;
                SDL_DelayNS(1500000000ull);
                vpc_music_lock();
                for (k = 0; k < 18; k++)
                    keys += vpc_music_chip()->chan[k].keyon;
                vpc_music_unlock();
                check(keys > 0 && func_000A2941(hsong) == 0,
                      "music: the song plays on the OPL3 (keys down after 1.5 s)");
            }
        }
    }

    if (selftest) {
        SDL_Event e;
        /* a, then shift-a: through int 9 at port 60h, and the BIOS's buffer */
        push_key(SDL_SCANCODE_A, 1);
        push_key(SDL_SCANCODE_A, 0);
        push_key(SDL_SCANCODE_LSHIFT, 1);
        push_key(SDL_SCANCODE_A, 1);
        push_key(SDL_SCANCODE_A, 0);
        push_key(SDL_SCANCODE_LSHIFT, 0);
        push_key(SDL_SCANCODE_UP, 1);
        memset(&e, 0, sizeof e);
        e.type = SDL_EVENT_MOUSE_MOTION;
        e.motion.xrel = 90;             /* 30 game pixels at the default 960-wide window */
        e.motion.yrel = 36;             /* 10 */
        SDL_PushEvent(&e);
        memset(&e, 0, sizeof e);
        e.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
        e.button.button = SDL_BUTTON_LEFT;
        SDL_PushEvent(&e);              /* the first click takes the mouse */
        SDL_PushEvent(&e);
        frames = 40;
    }

    t0 = bios_ticks();
    f0 = 0;
    ns0 = SDL_GetTicksNS();
    for (i = 0; i < frames; i++) {
        wait_retrace();
        memset(&r, 0, sizeof r);
        r.eax = 3;
        xn_int33(&r);
        mouse_x = (int)(r.ecx & 0xFFFF) / 2;
        mouse_y = (int)(r.edx & 0xFFFF);
        draw_cursor(mouse_x, mouse_y, 15);
        memset(&r, 0, sizeof r);
        r.eax = 5;                      /* left presses since last time: a sound */
        xn_int33(&r);
        if ((r.ebx & 0xFFFF) != 0 && clip != NULL)
            func_000A2504(digi, &start);
        f0++;
        if (!selftest) {
            int k;
            while ((k = vpc_bios_key(1)) >= 0) {
                if ((k >> 8) == 0x01)   /* Esc */
                    i = frames;
                if ((k & 0xFF) == ' ' && clip != NULL)
                    func_000A2504(digi, &start);
            }
        }
    }

    {
        double secs = (SDL_GetTicksNS() - ns0) / 1e9;
        unsigned int ticks = bios_ticks() - t0;
        printf("%u frames in %.2f s (%.1f Hz); %u BIOS ticks (%.1f Hz)\n", f0, secs, f0 / secs,
               ticks, ticks / secs);
        if (selftest) {
            int k1, k2, k3;
            memset(&r, 0, sizeof r);
            r.eax = 0x0B;
            xn_int33(&r);
            check(nseen >= 7 && seen[0] == 0x1E && seen[1] == 0x9E && seen[2] == 0x2A &&
                  seen[3] == 0x1E && seen[4] == 0x9E && seen[5] == 0xAA && seen[6] == 0xE0 &&
                  seen[7] == 0x48, "keyboard: set-1 scan codes through int 9 at port 60h "
                  "(1E 9E 2A 1E 9E AA E0 48)");
            k1 = vpc_bios_key(1);
            k2 = vpc_bios_key(1);
            k3 = vpc_bios_key(1);
            check(k1 == 0x1E61 && k2 == 0x1E41 && k3 == 0x4800,
                  "int 9 chained to the BIOS: int 16h buffer a, A, Up (1E61 1E41 4800)");
            check(mouse_x == 190 && mouse_y == 110, "int 33h: the position moved 30, 10 pixels");
            check(f0 / secs > 60 && f0 / secs < 80, "3DAh: frames paced at the 70 Hz retrace");
            check(ticks / secs > 15 && ticks / secs < 22, "BIOS tick at 0x46C runs at 18.2 Hz");
        }
    }
    if (shot != NULL)
        check(vpc_screenshot(shot) == 0, "screenshot saved");

    _dos_setvect(9, old_int9);
    memset(&r, 0, sizeof r);
    r.eax = 0x0003;
    xn_int10(&r);
    host_shutdown();
    free(img);
    free(snd);
    if (selftest)
        printf("%s: %d failed\n", fails ? "FAIL" : "PASS", fails);
    return fails ? 1 : 0;
}
