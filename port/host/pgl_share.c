/* pgl_share.c: the OpenGL renderer's hand-over between the game's thread and the main thread
   (port/include/port_gl.h): three scenes (one being filled, the latest published, the one
   being shown) and the texture registry with its upload queue. */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>

#include <SDL3/SDL.h>

#include "port_gl.h"
#include "port_vpc.h"

volatile int pgl_enabled;
volatile unsigned long port_render_frames;
volatile int pgl_wide_request;
volatile unsigned long pgl_present_serial;

unsigned long long pgl_now_ns(void)
{
    return SDL_GetTicksNS();
}

/* ---- PORT_GL_WIDE_CHECK=N: the wide pass's middle against the original pass ------------------

   Every Nth wide frame the game's thread forks at the frame's start: the copy draws the sky
   and the world the original way into the game's own screen (port/engine_gl/wide.c leaves
   it narrow), puts that screen and its depth (1/z of what covers each pixel, from the
   S-buffer) in shared memory and ends; the game goes on with the wide pass and, at its end,
   compares. The two start from the same state. The copy draws nothing for OpenGL and never
   reaches the main thread's SDL.

   The texels may differ: in the wide pass the spans of a polygon that crosses the 320-column
   view's edge start farther out (the span routines step their perspective every 8 or 16
   pixels from a span's start), and a face's texture origin goes through the view's scaling,
   whose rounding changes with the half width, so a whole face can land a texel over; a flat
   (a tree, a person) that crosses the old edge too. What must agree is the sky, the screen
   before the 3D pass (byte for byte), and the geometry: the depth of every pixel (within
   1/32, the gradients' rounding). Polygons' edges differ by a pixel here and there, more while
   the view moves: more than 3% of the pixels at another depth, or any pixel of the sky, is a
   failure (a wrong stride or a lost polygon is tens of percent); its screens and depths are
   saved as wide_check_*.pgm. */
#define CHECK_SCREEN (PGL_W * PGL_H)
#define CHECK_DEPTH 65536               /* the depth's offset in the shared block */
#define CHECK_SKY (CHECK_DEPTH + CHECK_SCREEN * 4)      /* the sky's */

static int check_every = -1, check_child, check_pending;
static unsigned long check_count, check_frames, check_bad;
static unsigned char *check_shared;     /* the copy's screen, a byte (written), its depth */
static pid_t check_pid;

int pgl_check_fork_begin(void)
{
    if (check_every < 0) {
        const char *e = SDL_getenv("PORT_GL_WIDE_CHECK");

        check_every = e ? atoi(e) : 0;
    }
    if (check_every <= 0 || ++check_count % (unsigned long)check_every != 0 || vpc_quit_requested)
        return 0;                       /* (a copy made as the game quits would wait for a main
                                           thread it has not got: vpc_game_exit) */
    if (check_shared == NULL) {
        void *m = mmap(NULL, CHECK_SKY + CHECK_SCREEN, PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_ANON, -1, 0);

        if (m == MAP_FAILED) {
            check_every = 0;
            return 0;
        }
        check_shared = m;
    }
    check_shared[CHECK_SCREEN] = 0;
    fflush(stderr);
    check_pid = fork();
    if (check_pid == 0) {
        sigset_t alrm;
        int null = open("/dev/null", O_WRONLY);

        check_child = 1;
        pgl_enabled = 0;
        /* never the holder of the game's output (a test waits for it to close), and ended by
           the alarm if it is stuck on a lock another thread held when it was made (the game's
           thread has its signals blocked) */
        if (null >= 0) {
            dup2(null, 1);
            dup2(null, 2);
            close(null);
        }
        sigemptyset(&alrm);
        sigaddset(&alrm, SIGALRM);
        pthread_sigmask(SIG_UNBLOCK, &alrm, NULL);
        signal(SIGALRM, SIG_DFL);
        alarm(10);
        return 1;
    }
    if (check_pid < 0)
        check_pid = 0;
    check_pending = check_pid > 0;
    return 0;
}

int pgl_check_child(void)
{
    return check_child;
}

int pgl_check_pending(void)
{
    return check_pending;
}

void pgl_check_child_done(const unsigned char *screen, const unsigned int *depth,
                          const unsigned char *sky)
{
    if (!check_child)
        return;
    memcpy(check_shared, screen, CHECK_SCREEN);
    memcpy(check_shared + CHECK_DEPTH, depth, CHECK_SCREEN * 4);
    memcpy(check_shared + CHECK_SKY, sky, CHECK_SCREEN);
    check_shared[CHECK_SCREEN] = 1;
    _exit(0);
}

static void dump_pgm(const char *name, const unsigned char *px)
{
    const char *dir = SDL_getenv("PORT_SHOT_DIR");
    char path[1024];
    FILE *f;

    snprintf(path, sizeof path, "%s/%s", dir ? dir : ".", name);
    f = fopen(path, "wb");
    if (f == NULL)
        return;
    fprintf(f, "P5\n%d %d\n255\n", PGL_W, PGL_H);
    fwrite(px, CHECK_SCREEN, 1, f);
    fclose(f);
    fprintf(stderr, "port: wide check: wrote %s\n", path);
}

/* a depth map as a picture: nearer brighter, none black */
static void dump_depth(const char *name, const unsigned int *d)
{
    static unsigned char px[CHECK_SCREEN];
    int i;

    for (i = 0; i < CHECK_SCREEN; i++) {
        unsigned int v = d[i] >> 22;

        px[i] = d[i] == 0 ? 0 : (unsigned char)(v > 254 ? 255 : v + 1);
    }
    dump_pgm(name, px);
}

static int depth_differs(unsigned int a, unsigned int b)
{
    unsigned int m = a > b ? a : b, d = a > b ? a - b : b - a;

    if (a == 0 || b == 0)
        return a != b;
    return d > m / 32;
}

void pgl_check_fork_end(const unsigned char *screen, const unsigned int *depth,
                        const unsigned char *sky_now)
{
    const unsigned int *orig;
    const unsigned char *sky_orig;
    Uint64 t0;
    int status, i, texels = 0, depths = 0, sky = 0, bad;

    if (check_pid <= 0)
        return;
    check_pending = 0;
    t0 = SDL_GetTicksNS();
    while (waitpid(check_pid, &status, WNOHANG) == 0) {
        if (SDL_GetTicksNS() - t0 > 5000000000ull) {
            kill(check_pid, SIGKILL);
            waitpid(check_pid, &status, 0);
            break;
        }
        SDL_DelayNS(200000);
    }
    check_pid = 0;
    check_frames++;
    if (!check_shared[CHECK_SCREEN]) {
        fprintf(stderr, "port: wide check %lu: the original pass did not finish\n", check_frames);
        return;
    }
    orig = (const unsigned int *)(check_shared + CHECK_DEPTH);
    sky_orig = check_shared + CHECK_SKY;
    for (i = 0; i < CHECK_SCREEN; i++) {
        texels += check_shared[i] != screen[i];
        depths += depth_differs(orig[i], depth[i]);
        sky += sky_orig[i] != sky_now[i];
    }
    bad = depths * 100 > CHECK_SCREEN * 3 || sky != 0;
    if (bad && check_bad++ == 0) {
        dump_pgm("wide_check_original.pgm", check_shared);
        dump_pgm("wide_check_wide.pgm", screen);
        dump_pgm("wide_check_original_sky.pgm", sky_orig);
        dump_pgm("wide_check_wide_sky.pgm", sky_now);
        dump_depth("wide_check_original_depth.pgm", orig);
        dump_depth("wide_check_wide_depth.pgm", depth);
    }
    fprintf(stderr, "port: wide check %lu: %s: depth differs at %d pixels (%.2f%%), the sky at "
            "%d; %d pixels' colours differ (%.2f%%)\n", check_frames, bad ? "FAILED" : "ok",
            depths, depths * 100.0 / CHECK_SCREEN, sky, texels, texels * 100.0 / CHECK_SCREEN);
}

void *pgl_realloc(void *p, size_t n)
{
    return realloc(p, n);
}

int pgl_vertex_light(void)
{
    const char *e = SDL_getenv("PORT_GL_VERTEX_LIGHT");

    return e && *e == '1';
}

static pgl_scene scenes[3];
static pgl_scene *building = &scenes[0], *latest = &scenes[1], *showing = &scenes[2];
static int latest_new;
static SDL_Mutex *lock;

static void init_lock(void)
{
    /* the first call comes from the main thread, before the game's thread starts */
    if (lock == NULL)
        lock = SDL_CreateMutex();
}

pgl_scene *pgl_building(void)
{
    return building;
}

void pgl_publish(void)
{
    pgl_scene *t;

    init_lock();
    SDL_LockMutex(lock);
    t = latest;
    latest = building;
    building = t;
    latest_new = 1;
    SDL_UnlockMutex(lock);
}

pgl_scene *pgl_latest(void)
{
    pgl_scene *t;

    init_lock();
    SDL_LockMutex(lock);
    if (latest_new) {
        t = showing;
        showing = latest;
        latest = t;
        latest_new = 0;
    }
    SDL_UnlockMutex(lock);
    return showing->serial ? showing : NULL;
}

/* ---- textures ------------------------------------------------------------------------------ */

/* the registry: hash -> id, open addressing (the game's thread only) */
static uint64_t *keys;
static int *ids;
static unsigned int cap, used;
static int next_id;

/* the upload queue (both threads) */
typedef struct upload {
    struct upload *next;
    int id, w, h;
    unsigned char *pixels;
} upload;
static upload *queue_head, *queue_tail;

static void grow(void)
{
    uint64_t *ok = keys;
    int *oi = ids;
    unsigned int oc = cap, i;

    cap = cap ? cap * 2 : 4096;
    keys = calloc(cap, sizeof *keys);
    ids = malloc(cap * sizeof *ids);
    used = 0;
    for (i = 0; i < oc; i++)
        if (ok[i]) {
            unsigned int j = (unsigned int)(ok[i] * 0x9E3779B97F4A7C15ull >> 40) & (cap - 1);

            while (keys[j])
                j = (j + 1) & (cap - 1);
            keys[j] = ok[i];
            ids[j] = oi[i];
            used++;
        }
    free(ok);
    free(oi);
}

static unsigned int slot(uint64_t hash)
{
    unsigned int j = (unsigned int)(hash * 0x9E3779B97F4A7C15ull >> 40) & (cap - 1);

    while (keys[j] && keys[j] != hash)
        j = (j + 1) & (cap - 1);
    return j;
}

int pgl_texture_id(uint64_t hash)
{
    unsigned int j;

    if (cap == 0)
        return -1;
    if (hash == 0)
        hash = 1;
    j = slot(hash);
    return keys[j] ? ids[j] : -1;
}

int pgl_texture_add(uint64_t hash, int w, int h, const unsigned char *pixels, int stride)
{
    upload *u;
    unsigned int j;
    int y;

    if (hash == 0)
        hash = 1;
    if ((used + 1) * 2 > cap)
        grow();
    j = slot(hash);
    if (keys[j])
        return ids[j];
    keys[j] = hash;
    ids[j] = next_id++;
    used++;

    u = malloc(sizeof *u);
    u->next = NULL;
    u->id = ids[j];
    u->w = w;
    u->h = h;
    u->pixels = malloc((size_t)w * h);
    for (y = 0; y < h; y++)
        memcpy(u->pixels + (size_t)y * w, pixels + (size_t)y * stride, (size_t)w);
    init_lock();
    SDL_LockMutex(lock);
    if (queue_tail)
        queue_tail->next = u;
    else
        queue_head = u;
    queue_tail = u;
    SDL_UnlockMutex(lock);
    return u->id;
}

int pgl_texture_next(int *id, int *w, int *h, unsigned char **pixels)
{
    upload *u;

    init_lock();
    SDL_LockMutex(lock);
    u = queue_head;
    if (u) {
        queue_head = u->next;
        if (queue_head == NULL)
            queue_tail = NULL;
    }
    SDL_UnlockMutex(lock);
    if (u == NULL)
        return 0;
    *id = u->id;
    *w = u->w;
    *h = u->h;
    *pixels = u->pixels;
    free(u);
    return 1;
}
