/* hmi_seq.c: HMI SOS 4.0's MIDI song player as FALL.EXE links it (0x9E18C-0xA0AD9,
   0xA1A16-0xA2A2B and the helpers at 0xA77EB, 0xB1437, 0xB23C1, 0xAB7B7), for the native
   build. Port of the rules in build/port/agents/hmi/hmi.py (static analysis, validated on all
   132 songs: every track parses to its end).

   The song: "HMI-MIDISONG061595"; s16 at 0xD4 the tick rate (120 in every song); s16 at 0xE4 the
   track count; u32 at 0xE8 the track table (u32 offsets). A track: u32 at +0x57 its events;
   u32 at +0x63 its branch table (u8 count, then {s16 id, u32 offset of an FE 10 record}); s16
   at +0x79 its MIDI channel; s16 at +0x47 its order; designations from +0x99 to a 0.

   Each callback: first the sounding notes, in the order they started (a note's duration counts
   down; at 0 its note-off goes out), then the tracks in order, each taking every event whose
   delta has run out. Events: VLQ delta, then MIDI with running status (any byte >= 0x80 sets
   it); the status's channel is replaced by the track's hardware channel.
   - 9n key vel dur: a note-on with its duration (no note-offs in the data); on channel 9 the
     velocity is scaled by the drum channel's volume.
   - Bn: CC7 goes out as value * master / 127; CC 0x69 1 takes a hardware channel (resending
     the track group's stored controllers), 0 gives it back; 0x77 is a trigger, not sent;
     others go out and are stored.
   - Cn, En go out and are stored. FF 2F ends the track; the last one ends the song.
   - FE 10 a branch point; FE 12/14 reset a loop counter; FE 13 a local loop end; FE 15 a global
     loop end (jumps every track to the branch); FE 11/16 branches; a counter of 0xFF loops
     forever (every game song but FOLK3.HMI loops for ever this way).
   Tracks with the same MIDI channel form a group with one hardware channel: the first free of
   0-8, 10-15 (drums keep 9). */
#include <stdlib.h>
#include <string.h>

#include "hmi_seq.h"

#define MAX_SONGS 32
#define NOTE_SLOTS 256

struct track {
    unsigned base, data, branch;
    int chan, order;
    unsigned ptr, delta;
    unsigned char running;
    int alive, has_channel, plays;
    int group;                  /* index in the song's groups */
};

struct group {
    int chan;                   /* the MIDI channel (+0x79) */
    int hw;                     /* the hardware channel, -1 none */
    int users;                  /* tracks holding it */
    short ctl[128];             /* stored controller values, -1 unset */
    short program, bend_msb;
    short cc7_raw;
};

struct note {
    int used, track;
    unsigned char hw, key;
    unsigned dur;
};

struct hmi_song {
    unsigned char *d;
    size_t len;
    unsigned char *orig;        /* the file as loaded, for the rewind */
    int ntracks, playing;
    struct track *t;
    int *order;                 /* track indexes in +0x47 order */
    struct group groups[16];
    int ngroups;
    struct note notes[NOTE_SLOTS];
    int note_seq[NOTE_SLOTS], nnotes;   /* insertion order */
};

static unsigned driver_device;
static void (*driver_send)(void *, unsigned, unsigned, unsigned);
static void *driver_ctx;
static int hw_busy[16];         /* the driver's channel table: entry 9 reserved */
static unsigned master_volume = 127;
static unsigned chan9_raw, chan9_scaled;
static struct hmi_song *playing[MAX_SONGS];

static unsigned u16(const unsigned char *d, unsigned o) { return d[o] | d[o + 1] << 8; }
static int s16(const unsigned char *d, unsigned o) { return (short)u16(d, o); }
static unsigned u32(const unsigned char *d, unsigned o)
{
    return d[o] | d[o + 1] << 8 | d[o + 2] << 16 | (unsigned)d[o + 3] << 24;
}

static unsigned vlq(const unsigned char *d, size_t len, unsigned *p)
{
    unsigned v = 0;

    while (*p < len) {
        unsigned char b = d[(*p)++];
        v = v << 7 | (b & 0x7F);
        if (!(b & 0x80))
            break;
    }
    return v;
}

static void send(unsigned st, unsigned d1, unsigned d2)
{
    if (driver_send != NULL)
        driver_send(driver_ctx, st, d1, d2);
}

void hmi_set_driver(unsigned device, void (*fn)(void *, unsigned, unsigned, unsigned), void *ctx)
{
    driver_device = device;
    driver_send = fn;
    driver_ctx = ctx;
}

void hmi_driver_reset(void)
{
    int c;

    memset(hw_busy, 0, sizeof hw_busy);
    hw_busy[9] = 1;
    chan9_raw = chan9_scaled = 0;
    /* SOS at driver init: CC123, CC7 0, CC121 on 0-8 and 10-15 */
    for (c = 0; c < 16; c++) {
        if (c == 9)
            continue;
        send(0xB0 | c, 0x7B, 0);
        send(0xB0 | c, 0x07, 0);
        send(0xB0 | c, 0x79, 0);
    }
}

/* 0xB241E: a designation against the driver's device */
static int accepts(unsigned des, unsigned dev)
{
    if (des == 0xA000)
        return dev == 0xA000 || dev == 0xA001 || dev == 0xA008;
    if (des == 0xA002)
        return dev == 0xA002 || dev == 0xA009;
    return des == dev;
}

static unsigned scaled7(unsigned raw)
{
    return raw * master_volume / 127;
}

/* ---- the song -------------------------------------------------------------------------------- */

static void rewind_song(struct hmi_song *s)
{
    int i;

    memcpy(s->d, s->orig, s->len);          /* loop counters and controller state: as loaded */
    memset(s->notes, 0, sizeof s->notes);
    s->nnotes = 0;
    for (i = 0; i < s->ntracks; i++) {
        struct track *t = &s->t[i];
        t->ptr = t->base + t->data;
        t->delta = vlq(s->d, s->len, &t->ptr);
        t->running = 0;
        t->alive = t->plays;
        t->has_channel = 0;
    }
    for (i = 0; i < s->ngroups; i++) {
        struct group *g = &s->groups[i];
        g->hw = -1;
        g->users = 0;
        memset(g->ctl, 0xFF, sizeof g->ctl);
        g->program = g->bend_msb = g->cc7_raw = -1;
    }
}

struct hmi_song *hmi_song_new(const unsigned char *data, size_t len, int *ready)
{
    struct hmi_song *s;
    unsigned tdir;
    int i, j, any = 0;

    *ready = 0;
    if (len < 0x100 || memcmp(data, "HMI-MIDISONG061595", 19) != 0)
        return NULL;
    s = calloc(1, sizeof *s);
    s->len = len;
    s->d = malloc(len);
    s->orig = malloc(len);
    memcpy(s->d, data, len);
    memcpy(s->orig, data, len);
    s->ntracks = s16(data, 0xE4);
    tdir = u32(data, 0xE8);
    if (s->ntracks < 0 || s->ntracks > 64 || tdir + 4u * (unsigned)s->ntracks > len) {
        hmi_song_free(s);
        return NULL;
    }
    s->t = calloc((size_t)s->ntracks, sizeof *s->t);
    s->order = calloc((size_t)s->ntracks, sizeof *s->order);
    for (i = 0; i < s->ntracks; i++) {
        struct track *t = &s->t[i];
        unsigned p;
        t->base = u32(data, tdir + 4 * (unsigned)i);
        if (t->base + 0xB0 > len) {
            hmi_song_free(s);
            return NULL;
        }
        t->data = u32(data, t->base + 0x57);
        t->branch = u32(data, t->base + 0x63);
        t->chan = s16(data, t->base + 0x79);
        t->order = s16(data, t->base + 0x47);
        for (p = t->base + 0x99; p + 1 < len && u16(data, p) != 0; p += 2)
            if (accepts(u16(data, p), driver_device))
                t->plays = 1;
        any |= t->plays;
        /* the group of its MIDI channel */
        for (j = 0; j < s->ngroups && s->groups[j].chan != t->chan; j++)
            ;
        if (j == s->ngroups && s->ngroups < 16)
            s->groups[s->ngroups++].chan = t->chan;
        t->group = j < 16 ? j : 15;
        s->order[i] = i;
    }
    /* the active list in +0x47 order (0xA7593) */
    for (i = 1; i < s->ntracks; i++)
        for (j = i; j > 0 && s->t[s->order[j]].order < s->t[s->order[j - 1]].order; j--) {
            int k = s->order[j];
            s->order[j] = s->order[j - 1];
            s->order[j - 1] = k;
        }
    rewind_song(s);
    *ready = any;
    return s;
}

void hmi_song_free(struct hmi_song *s)
{
    int i;

    if (s == NULL)
        return;
    for (i = 0; i < MAX_SONGS; i++)
        if (playing[i] == s)
            playing[i] = NULL;
    free(s->d);
    free(s->orig);
    free(s->t);
    free(s->order);
    free(s);
}

int hmi_song_playing(const struct hmi_song *s)
{
    return s != NULL && s->playing;
}

/* ---- channels ------------------------------------------------------------------------------ */

static int track_hw(const struct hmi_song *s, const struct track *t)
{
    if (!t->has_channel)
        return -1;
    return t->chan == 9 ? 9 : s->groups[t->group].hw;
}

/* 0xAB7B7: B0 69 01. Drums keep channel 9; a melodic group takes the first free entry, and on
   taking it the driver gets CC121 and the group's stored controllers, in controller order */
static void acquire(struct hmi_song *s, struct track *t)
{
    struct group *g = &s->groups[t->group];
    int c;

    if (t->has_channel)
        return;
    t->has_channel = 1;
    if (t->chan == 9)
        return;
    g->users++;
    if (g->hw >= 0)
        return;
    for (c = 0; c < 16 && hw_busy[c]; c++)
        ;
    if (c == 16) {
        t->has_channel = 0;         /* would need priority stealing; never in the game's songs */
        g->users--;
        return;
    }
    hw_busy[c] = 1;
    g->hw = c;
    send(0xB0 | (unsigned)c, 0x79, 0);
    /* the stored state in controller order: the program and the bend sit at the
       pseudo-controllers 0x68 and 0x69 */
    for (c = 0; c < 128; c++) {
        if (c == 0x68) {
            if (g->program >= 0)
                send(0xC0 | (unsigned)g->hw, (unsigned)g->program, 0);
        } else if (c == 0x69) {
            if (g->bend_msb >= 0)
                send(0xE0 | (unsigned)g->hw, 0, (unsigned)g->bend_msb);
        } else if (g->ctl[c] >= 0) {
            send(0xB0 | (unsigned)g->hw, (unsigned)c,
                 c == 7 ? scaled7((unsigned)g->ctl[c]) : (unsigned)g->ctl[c]);
        }
    }
}

/* 0xABBD8: B0 69 00 */
static void release(struct hmi_song *s, struct track *t)
{
    struct group *g = &s->groups[t->group];

    if (!t->has_channel)
        return;
    t->has_channel = 0;
    if (t->chan == 9)
        return;
    if (--g->users <= 0 && g->hw >= 0) {
        hw_busy[g->hw] = 0;
        g->hw = -1;
        g->users = 0;
    }
}

/* ---- notes ------------------------------------------------------------------------------------ */

static void note_add(struct hmi_song *s, int track, unsigned hw, unsigned key, unsigned dur)
{
    int i;

    for (i = 0; i < NOTE_SLOTS && s->notes[i].used; i++)
        ;
    if (i == NOTE_SLOTS)
        return;
    s->notes[i].used = 1;
    s->notes[i].track = track;
    s->notes[i].hw = (unsigned char)hw;
    s->notes[i].key = (unsigned char)key;
    s->notes[i].dur = dur;
    s->note_seq[s->nnotes++] = i;
}

static void note_remove_at(struct hmi_song *s, int k)
{
    s->notes[s->note_seq[k]].used = 0;
    memmove(&s->note_seq[k], &s->note_seq[k + 1], (size_t)(s->nnotes - k - 1) * sizeof(int));
    s->nnotes--;
}

/* a track's sounding notes off (a jump, the end of the song) */
static void kill_notes(struct hmi_song *s, int track)
{
    int k;

    for (k = 0; k < s->nnotes;) {
        struct note *n = &s->notes[s->note_seq[k]];
        if (track < 0 || n->track == track) {
            send(0x90 | n->hw, n->key, 0);
            note_remove_at(s, k);
        } else {
            k++;
        }
    }
}

/* ---- jumps -------------------------------------------------------------------------------------- */

/* 0xB1521: a track to the FE 10 record at off (from the track's start) */
static void jump_track(struct hmi_song *s, int ti, unsigned off)
{
    struct track *t = &s->t[ti];
    unsigned p = t->base + off + 4, n, i;
    int hw;

    kill_notes(s, ti);
    if (p >= s->len)
        return;
    n = s->d[p++];
    hw = track_hw(s, t);
    for (i = 0; i + 1 < n && p + i + 1 < s->len; i += 2) {
        unsigned cc = s->d[p + i], v = s->d[p + i + 1];
        struct group *g = &s->groups[t->group];
        if (hw < 0)
            continue;
        if (cc == 0x68) {
            g->program = (short)v;
            send(0xC0 | (unsigned)hw, v, 0);
        } else if (cc == 0x69) {
            g->bend_msb = (short)v;
            send(0xE0 | (unsigned)hw, 0, v);
        } else if (cc == 7) {
            g->ctl[7] = (short)v;
            if (hw == 9) {
                chan9_raw = v;
                chan9_scaled = scaled7(v);
            }
            send(0xB0 | (unsigned)hw, 7, scaled7(v));
        } else {
            g->ctl[cc & 127] = (short)v;
            send(0xB0 | (unsigned)hw, cc, v);
        }
    }
    p += n + 4;                         /* the restore pairs, the u32 tick */
    t->ptr = p;
    t->delta = vlq(s->d, s->len, &t->ptr);
}

/* 0xB1437: every track with branch id to its branch point; ended tracks come back */
static void global_jump(struct hmi_song *s, int id)
{
    int i, k;

    for (i = 0; i < s->ntracks; i++) {
        struct track *t = &s->t[i];
        unsigned bp;
        if (!t->branch || !t->plays)
            continue;
        bp = t->base + t->branch;
        for (k = 0; bp < s->len && k < s->d[bp]; k++) {
            unsigned e = bp + 1 + 6 * (unsigned)k;
            if (e + 6 > s->len || s16(s->d, e) != id)
                continue;
            if (!t->alive) {
                t->alive = 1;
                acquire(s, t);
            }
            jump_track(s, i, u32(s->d, e + 2));
            break;
        }
    }
}

/* ---- the event handler (0xA77EB): 1 when a delta follows ---------------------------------- */

static void stop_song(struct hmi_song *s);

static int handle(struct hmi_song *s, int ti)
{
    struct track *t = &s->t[ti];
    const unsigned char *d = s->d;
    unsigned p = t->ptr, st, hw, d1, d2;
    int flag = 0;

    if (p >= s->len)
        return 0;
    if (d[p] >= 0x80) {
        t->running = d[p];
        flag = 1;
    }
    st = t->running;
    hw = (unsigned)track_hw(s, t);
    p += (unsigned)flag;
    d1 = p < s->len ? d[p] : 0;
    d2 = p + 1 < s->len ? d[p + 1] : 0;
    switch (st & 0xF0) {
    case 0x90: {
        unsigned dur, vel = d2;
        p += 2;
        dur = vlq(d, s->len, &p);
        if ((int)hw >= 0) {
            if (hw == 9)
                vel = vel * chan9_scaled / 127;
            send(0x90 | hw, d1, vel);
            note_add(s, ti, hw, d1, dur);
        }
        t->ptr = p;
        return 1;
    }
    case 0xB0: {
        struct group *g = &s->groups[t->group];
        t->ptr = p + 2;
        if (d1 == 0x69) {
            if (d2)
                acquire(s, t);
            else
                release(s, t);
            return 1;
        }
        if (d1 == 0x77 || d1 == 0x67 || d1 == 0x68 || d1 == 0x6A || d1 == 0x6B || d1 == 0x6C)
            return 1;               /* SOS's own controllers: not sent */
        g->ctl[d1 & 127] = (short)d2;
        if ((int)hw < 0)
            return 1;
        if (d1 == 7) {
            if (hw == 9) {
                chan9_raw = d2;
                chan9_scaled = scaled7(d2);
            }
            send(0xB0 | hw, 7, scaled7(d2));
        } else {
            send(0xB0 | hw, d1, d2);
        }
        return 1;
    }
    case 0xC0:
        s->groups[t->group].program = (short)d1;
        if ((int)hw >= 0)
            send(0xC0 | hw, d1, 0);
        t->ptr = p + 1;
        return 1;
    case 0xE0:
        s->groups[t->group].bend_msb = (short)d2;
        if ((int)hw >= 0)
            send(0xE0 | hw, d1, d2);
        t->ptr = p + 2;
        return 1;
    case 0xA0:
        if ((int)hw >= 0)
            send(0xA0 | hw, d1, d2);
        t->ptr = p + 2;
        return 1;
    case 0xD0:
        if ((int)hw >= 0)
            send(0xD0 | hw, d1, 0);
        t->ptr = p + 1;
        return 1;
    case 0x80:
        return 0;                   /* SOS stalls the track; never in the data */
    }
    if (st == 0xF0) {
        t->ptr = p + 4 + u32(d, p);
        return 1;
    }
    if (st == 0xFF) {
        if (d1 == 0x2F) {           /* end of track (0xA812F) */
            int i, any = 0;
            t->alive = 0;
            for (i = 0; i < s->ntracks; i++)
                any |= s->t[i].alive;
            if (!any)
                stop_song(s);
        }
        return 0;
    }
    if (st == 0xFE) {
        unsigned sub = d1, c;
        p = t->ptr;                 /* the FE byte */
        switch (sub) {
        case 0x10:                  /* a branch point: nothing when played through */
            t->ptr = p + 9 + d[p + 4];
            return 1;
        case 0x11:                  /* local branch: always taken (no callback) */
            jump_track(s, ti, u32(d, p + 4));
            return 0;
        case 0x12:
        case 0x14:                  /* loop start: the counter back to its initial value */
            s->d[p + 2] = d[p + 3];
            t->ptr = p + 4;
            return 1;
        case 0x13:                  /* local loop end */
            c = t->base + u32(d, p + 8);
            if (c >= s->len || s->d[c] == 0) {
                t->ptr = p + 12;
                return 1;
            }
            if (s->d[c] != 0xFF)
                s->d[c]--;
            jump_track(s, ti, u32(d, p + 4));
            return 0;
        case 0x15:                  /* global loop end */
            c = t->base + u32(d, p + 4);
            if (c >= s->len || s->d[c] == 0) {
                t->ptr = p + 8;
                return 1;
            }
            if (s->d[c] != 0xFF)
                s->d[c]--;
            global_jump(s, s16(d, p + 2));
            return 0;
        case 0x16:                  /* global branch */
            global_jump(s, s16(d, p + 2));
            return 0;
        }
    }
    return 0;                       /* not advanced: stalls, as SOS does */
}

/* ---- playback -------------------------------------------------------------------------------- */

static void stop_song(struct hmi_song *s)
{
    int i;

    kill_notes(s, -1);
    for (i = 0; i < s->ntracks; i++)
        release(s, &s->t[i]);
    s->playing = 0;
    for (i = 0; i < MAX_SONGS; i++)
        if (playing[i] == s)
            playing[i] = NULL;
    rewind_song(s);
}

void hmi_song_start(struct hmi_song *s)
{
    int i;

    if (s == NULL || s->playing)
        return;
    for (i = 0; i < MAX_SONGS && playing[i] != NULL; i++)
        ;
    if (i == MAX_SONGS)
        return;
    playing[i] = s;
    s->playing = 1;
}

void hmi_song_stop(struct hmi_song *s)
{
    if (s != NULL && s->playing)
        stop_song(s);
}

void hmi_set_master_volume(unsigned v)
{
    int i, j;

    master_volume = v > 127 ? 127 : v;
    /* 0xA1A2B: CC7 again for each group from its raw value; for the drums only the scale */
    chan9_scaled = scaled7(chan9_raw);
    for (i = 0; i < MAX_SONGS; i++) {
        struct hmi_song *s = playing[i];
        if (s == NULL)
            continue;
        for (j = 0; j < s->ngroups; j++) {
            struct group *g = &s->groups[j];
            if (g->hw >= 0 && g->ctl[7] >= 0)
                send(0xB0 | (unsigned)g->hw, 7, scaled7((unsigned)g->ctl[7]));
        }
    }
}

static void tick_song(struct hmi_song *s)
{
    int k, i;

    /* the notes, in the order they started */
    for (k = 0; k < s->nnotes;) {
        struct note *n = &s->notes[s->note_seq[k]];
        if (n->dur-- == 0) {
            if (s->t[n->track].has_channel)
                send(0x90 | n->hw, n->key, 0);
            note_remove_at(s, k);
        } else {
            k++;
        }
    }
    /* the tracks, in order */
    for (i = 0; i < s->ntracks && s->playing; i++) {
        int ti = s->order[i];
        struct track *t = &s->t[ti];
        if (!t->alive)
            continue;
        while (s->playing && t->alive && t->delta-- == 0) {
            if (handle(s, ti))
                t->delta = vlq(s->d, s->len, &t->ptr);
            else if (!t->alive || !s->playing)
                break;
        }
    }
}

void hmi_tick(void)
{
    int i;

    for (i = 0; i < MAX_SONGS; i++)
        if (playing[i] != NULL)
            tick_song(playing[i]);
}

/* the song's size when the caller does not know it (sosMIDIInitSong gets a pointer only): the
   end of its furthest track, walked as the player reads it (to FF 2F 00), plus the 16 bytes
   that follow the last one in MIDI.BSA's songs */
size_t hmi_song_size(const unsigned char *d)
{
    const size_t cap = 1 << 22;
    size_t end = 0x100;
    int n, i;
    unsigned tdir;

    if (memcmp(d, "HMI-MIDISONG061595", 19) != 0)
        return 0;
    n = s16(d, 0xE4);
    tdir = u32(d, 0xE8);
    for (i = 0; i < n && i < 64; i++) {
        unsigned base = u32(d, tdir + 4 * (unsigned)i), p = base + u32(d, base + 0x57);
        unsigned char running = 0;
        vlq(d, cap, &p);
        while (p < cap) {
            unsigned st;
            if (d[p] >= 0x80)
                running = d[p++];
            st = running;
            if ((st & 0xF0) == 0x90) {
                p += 2;
                vlq(d, cap, &p);
            } else if ((st & 0xF0) == 0xB0 || (st & 0xF0) == 0xE0 || (st & 0xF0) == 0xA0) {
                p += 2;
            } else if ((st & 0xF0) == 0xC0 || (st & 0xF0) == 0xD0) {
                p += 1;
            } else if (st == 0xF0) {
                p += 4 + u32(d, p);
            } else if (st == 0xFF) {
                p += 2;                 /* 2F 00 */
                break;
            } else if (st == 0xFE) {
                unsigned sub = d[p];
                p -= 1;                 /* the FE byte */
                p += sub == 0x10 ? 9 + d[p + 4] : sub == 0x11 || sub == 0x15 ? 8
                   : sub == 0x13 ? 12 : 4;
            } else {
                break;
            }
            vlq(d, cap, &p);
        }
        if (p > end)
            end = p;
    }
    return end + 16;
}
