/* vpc_script.c: the virtual PC's scripted driver (port/include/port_vpc.h): a timeline of
   screenshots, keys and clicks from the environment, so a run can go without a person (the
   native build's bring-up and its checks against the emulator). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_vpc.h"

#define MAX_STEPS 512

struct step {
    double at;
    char cmd[16];
    char arg[240];
};

static struct step steps[MAX_STEPS];
static int n_steps, next_step;
static Uint64 start_ns;
static double shot_every, exit_after, next_shot;
static const char *shot_dir;
static int shot_count;
static int release_key = -1, release_button = -1;
static double release_at;

static double now_s(void)
{
    return (double)(SDL_GetTicksNS() - start_ns) / 1e9;
}

void vpc_script_init(void)
{
    const char *s = getenv("PORT_SCRIPT");
    const char *e = getenv("PORT_SHOT_EVERY");
    const char *x = getenv("PORT_EXIT_AFTER");

    start_ns = SDL_GetTicksNS();
    shot_dir = getenv("PORT_SHOT_DIR");
    shot_every = e ? atof(e) : 0;
    next_shot = shot_every;
    exit_after = x ? atof(x) : 0;
    while (s != NULL && *s && n_steps < MAX_STEPS) {
        const char *end = strchr(s, ';');
        size_t len = end ? (size_t)(end - s) : strlen(s);
        char buf[300];
        struct step *st = &steps[n_steps];

        if (len >= sizeof buf)
            len = sizeof buf - 1;
        memcpy(buf, s, len);
        buf[len] = 0;
        st->arg[0] = 0;
        if (sscanf(buf, " %lf %15s %239[^\n]", &st->at, st->cmd, st->arg) >= 2)
            n_steps++;
        s = end ? end + 1 : NULL;
    }
}

static int scancode(const char *name)
{
    SDL_Scancode c = SDL_GetScancodeFromName(name);

    if (c == SDL_SCANCODE_UNKNOWN)
        fprintf(stderr, "port script: no key %s\n", name);
    return (int)c;
}

static void shot(const char *path)
{
    if (vpc_screenshot(path) != 0)
        fprintf(stderr, "port script: screenshot %s failed\n", path);
    else
        fprintf(stderr, "port script: %.1f s: %s\n", now_s(), path);
}

static void run(const struct step *st)
{
    int x, y;

    if (!strcmp(st->cmd, "shot")) {
        shot(st->arg);
    } else if (!strcmp(st->cmd, "key")) {
        int c = scancode(st->arg);
        if (c) {
            vpc_input_key(c, 1);
            release_key = c;
            release_at = now_s() + 0.1;
        }
    } else if (!strcmp(st->cmd, "down") || !strcmp(st->cmd, "up")) {
        int c = scancode(st->arg);
        if (c)
            vpc_input_key(c, st->cmd[0] == 'd');
    } else if (!strcmp(st->cmd, "type")) {
        const char *p;
        for (p = st->arg; *p; p++) {
            char k[2] = {*p, 0};
            int c = *p == ' ' ? SDL_SCANCODE_SPACE : scancode(k);
            if (c) {
                vpc_input_key(c, 1);
                vpc_input_key(c, 0);
            }
        }
    } else if (!strcmp(st->cmd, "move") || !strcmp(st->cmd, "click") ||
               !strcmp(st->cmd, "rclick")) {
        if (sscanf(st->arg, "%d %d", &x, &y) == 2)
            vpc_input_mouse_to(x, y);
        if (st->cmd[0] != 'm') {
            release_button = st->cmd[0] == 'r';
            vpc_input_mouse_button(release_button, 1);
            release_at = now_s() + 0.1;
        }
    } else if (!strcmp(st->cmd, "quit")) {
        vpc_quit_requested = 1;
    } else {
        fprintf(stderr, "port script: unknown step %s\n", st->cmd);
    }
}

void vpc_script_poll(void)
{
    double t = now_s();

    if (release_key >= 0 && t >= release_at) {
        vpc_input_key(release_key, 0);
        release_key = -1;
    }
    if (release_button >= 0 && t >= release_at) {
        vpc_input_mouse_button(release_button, 0);
        release_button = -1;
    }
    while (next_step < n_steps && t >= steps[next_step].at)
        run(&steps[next_step++]);
    if (shot_every > 0 && shot_dir != NULL && t >= next_shot) {
        char path[1024];
        snprintf(path, sizeof path, "%s/shot_%04d.bmp", shot_dir, shot_count++);
        shot(path);
        next_shot += shot_every;
    }
    if (exit_after > 0 && t >= exit_after)
        vpc_quit_requested = 1;
}
