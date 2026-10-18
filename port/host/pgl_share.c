/* pgl_share.c: the OpenGL renderer's hand-over between the game's thread and the main thread
   (port/include/port_gl.h): three scenes (one being filled, the latest published, the one
   being shown) and the texture registry with its upload queue. */
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_gl.h"

volatile int pgl_enabled;
volatile unsigned long port_render_frames;

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
