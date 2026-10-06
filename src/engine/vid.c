/* vid.c: XnGine's VID movie player (canonical C; the interface and the module's documentation
   are in xvid.h). */
#include "xvid.h"
#include "xgfx.h"
#include "xpal.h"
#include "xdos.h"

#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA        0x3C9

#define VID_BUFFER          0xFA00      /* the read buffer: 64000 bytes of big_buffer */
#define VID_REFILL_MARGIN   0x81        /* a delta or RLE command refills this near the end */
#define VID_AUDIO_BUFFER    0x4000
#define VID_DATA_SIZE       0x4F1       /* the player's data: xn_vid_header .. xn_vid_skippable */
#define VID_TIMER_HZ        60

/* chunk types */
#define VID_RAW         0
#define VID_DELTA       1
#define VID_PALETTE     2
#define VID_RLE         3
#define VID_DELTA_FROM  4
#define VID_AUDIO_HEAD  0x7C
#define VID_AUDIO       0x7D

/* ---- the game's (object 1, Watcom C: the boundary's exits) ---------------------------------- */
int dpmi_lock_region(int address, int size);
int dpmi_unlock_region(int address, int size);
int sound_timer_add(void (*callback)(void), int rate);     /* its handle, -1 when it failed */
void sound_timer_remove(int handle);
int func_000A2504(int driver, xn_vid_sos_sample *sample);  /* SOS: start a sample: its handle */
void func_000A2687(int driver, int handle);                 /* SOS: stop a sample */
#define sos_start_sample func_000A2504
#define sos_stop_sample  func_000A2687
extern s32 D_0018DD5C;                  /* the SOS digital device: -1 none */
extern s32 D_0018DD60;                  /* the SOS digital driver's handle */
#define sound_digi_device D_0018DD5C
#define sound_digi_driver D_0018DD60
extern u8 mouse_buttons;

/* ---- other groups' (group A's: xstr.h, xmouse.h, xkbd.h) ------------------------------------ */
void xn_str_copy(const char *src, char *dst);
void xn_mouse_poll_clamped(void);
u16 xn_kbd_read_key(void);              /* the last key (AH scan code, AL ASCII), 0 when none */

/* The player's code, locked with its data while a movie plays: the timer and sample callbacks
   run under interrupts. It runs from here to vid_code_end, at the end of this file. */
static void vid_code_start(void)
{
}

static void vid_code_end(void);
#define VID_CODE_SIZE ((u8 *)vid_code_end - (u8 *)vid_code_start)

/* ---- the player ------------------------------------------------------------------------------- */

/* Shows the opened movie in the clip window until it ends (or a button or key, when it is
   skippable), then sets its palette */
static void vid_show(void)
{
    struct xn_vid_player *v = &xn_vid_state;

    xn_gfx_clip_left = v->x;
    xn_gfx_clip_right = v->x + xn_vid_header.width;
    xn_gfx_clip_top = v->y;
    xn_gfx_clip_bottom = v->y + xn_vid_header.height;
    xn_gfx_present(1);
    for (;;) {
        if (v->skippable != 0) {
            xn_mouse_poll_clamped();
            if (mouse_buttons & 3)
                break;
            if ((u8)xn_kbd_read_key() != 0)
                break;
        }
        xn_vid_update();
        xn_gfx_present(1);
        if (v->done == 1)
            break;
    }
    xn_vid_finish();
    xn_pal_set(v->palette);
}

int xn_vid_play(const char *path, s32 x, s32 y, s32 skippable)
{
    struct xn_vid_player *v = &xn_vid_state;
    s32 left = xn_gfx_clip_left, right = xn_gfx_clip_right;
    s32 top = xn_gfx_clip_top, bottom = xn_gfx_clip_bottom;
    int played = 0;

    /* the timer and sample callbacks run under interrupts: lock the player's data and code */
    dpmi_lock_region((int)&xn_vid_header, VID_DATA_SIZE);
    dpmi_lock_region((int)vid_code_start, VID_CODE_SIZE);
    v->skippable = skippable;
    xn_str_copy(path, v->path);
    if (xn_dos_file_exists(v->path)) {
        xn_vid_open(x, y, v->path);
        if (v->timer != -1) {
            vid_show();
            played = 1;
        }
    }
    if (!played) {
        /* (the buffers' pointers: 0 since the last movie, unless the timer failed) */
        game_free(v->audio_buf_a);
        game_free(v->audio_buf_b);
    }
    /* Deviation D-VID-01: the asm's unlocks got a size it never set (a leftover register) */
    dpmi_unlock_region((int)&xn_vid_header, VID_DATA_SIZE);
    dpmi_unlock_region((int)vid_code_start, VID_CODE_SIZE);
    xn_gfx_clip_bottom = bottom;
    xn_gfx_clip_top = top;
    xn_gfx_clip_right = right;
    xn_gfx_clip_left = left;
    return played;
}

void xn_vid_open(s32 x, s32 y, const char *path)
{
    struct xn_vid_player *v = &xn_vid_state;
    s32 handle;
    u8 *p;
    u32 i;

    v->file_pos = 0;
    v->x = x;
    v->y = y;
    v->path_arg = (char *)path;
    v->audio_state = 0;
    v->buf = big_buffer;
    v->buf_end = v->buf + VID_BUFFER;
    v->refill_mark = v->buf_end - VID_REFILL_MARGIN;
    v->audio_buf_a = game_malloc(VID_AUDIO_BUFFER);
    dpmi_lock_region((int)v->audio_buf_a, VID_AUDIO_BUFFER);
    v->audio_buf_b = game_malloc(VID_AUDIO_BUFFER);
    dpmi_lock_region((int)v->audio_buf_b, VID_AUDIO_BUFFER);
    v->audio_buffer = 0;
    v->sample_done = 0;
    v->sample_restart = 0;
    p = (u8 *)&xn_vid_sample;
    for (i = 0; i < sizeof(xn_vid_sample); i++)
        p[i] = 0;
    v->audio_ticks = 0;
    v->frame_ticks = 0;
    v->timer = sound_timer_add(xn_vid_timer_cb, VID_TIMER_HZ);

    v->file = 0;
    xn_dos_open_mode(v->path_arg, 0, &handle);     /* Quirk Q-VID-03: not checked */
    v->file = (u16)handle;
    xn_dos_read(v->file, &xn_vid_header, sizeof(xn_vid_header));

    v->dest_offset = xn_gfx_row_offset[v->y] + v->x;
    /* Quirk Q-VID-02: a 16-bit subtraction: the width's upper half stays */
    v->dest_skip = (xn_gfx_width & 0xFFFF0000) | (u16)(xn_gfx_width - xn_vid_header.width);
    v->flags = xn_vid_header.flags;
    v->frames_left = xn_vid_header.frames;
    v->row_width = xn_vid_header.width;
    v->width = xn_vid_header.width;
    v->rows = xn_vid_header.height;
    v->height = xn_vid_header.height;
    v->frames_done = 0;
    v->frame_waits = 0;
    xn_vid_fill();
    v->done = 0;
    v->frame_ticks = 0;
    xn_vid_decode_chunk();
}

void xn_vid_update(void)
{
    struct xn_vid_player *v = &xn_vid_state;

    if (sound_digi_device != -1 && v->audio_state == 3) {
        /* streaming: the next frame comes when the sample has played */
        if (v->sample_done != 1)
            return;
        v->sample_done = 0;
        if (v->sample_restart == 1) {
            xn_vid_audio_start();
            v->sample_restart = 0;
        }
    } else if (v->frame_ticks > 0) {
        return;
    }
    if (v->frame_waits != 0) {
        v->frame_waits = 0;
        v->sample_done = 0;
        v->audio_buffer = 0;
        xn_vid_audio_stop();
    }
    xn_vid_decode_chunk();
}

void xn_vid_finish(void)
{
    struct xn_vid_player *v = &xn_vid_state;
    u8 *p;

    xn_vid_audio_stop();
    dpmi_unlock_region((int)v->audio_buf_a, VID_AUDIO_BUFFER);     /* (D-VID-01) */
    dpmi_unlock_region((int)v->audio_buf_b, VID_AUDIO_BUFFER);
    p = v->audio_buf_a;
    v->audio_buf_a = 0;
    if (p != 0)
        game_free(p);
    p = v->audio_buf_b;
    v->audio_buf_b = 0;
    if (p != 0)
        game_free(p);
    if (!(v->flags & 1))
        xn_vid_close_file();
    v->done = 1;
    sound_timer_remove(v->timer);
}

/* ---- decoding ----------------------------------------------------------------------------- */

/* Copies n bytes, forward */
static void copy_bytes(u8 *dst, const u8 *src, u32 n)
{
    for (; n != 0; n--)
        *dst++ = *src++;
}

/* Copies n bytes, dwords then the rest */
static void copy_dwords(u8 *dst, const u8 *src, u32 n)
{
    u32 k;

    for (k = n >> 2; k != 0; k--, dst += 4, src += 4)
        *(u32 *)dst = *(const u32 *)src;
    copy_bytes(dst, src, n & 3);
}

/* The audio header (7Ch): the sound's delay and rate; its first audio data follows */
static void audio_header(u8 *p)
{
    struct xn_vid_player *v = &xn_vid_state;

    v->audio_delay = *(u16 *)p;
    p += 2;
    v->rate_byte = *p++;
    v->sample_rate = (u16)(1000000u / (0x100 - v->rate_byte));
    xn_vid_read_audio(p);
    v->audio_state = 1;
    v->audio_ticks = v->audio_delay;
    if (v->audio_delay != 0)
        v->audio_waiting = 0;
}

/* A palette (2): to xn_vid_palette, and to the DAC at once */
static void palette(u8 *p)
{
    struct xn_vid_player *v = &xn_vid_state;
    const u8 *c = v->palette;
    int i;

    if (p > v->buf_end - 0x300)
        p = xn_vid_refill(p);
    copy_dwords(v->palette, p, 0x300);
    v->read_ptr = p + 0x300;
    for (i = 0; i < 256; i++) {
        xn_outb(VGA_DAC_WRITE_INDEX, (u8)i);
        xn_outb(VGA_DAC_DATA, *c++);
        xn_outb(VGA_DAC_DATA, *c++);
        xn_outb(VGA_DAC_DATA, *c++);
    }
}

/* The end of a row in the frame: 1 when it was the frame's last */
static int row_done(u8 **dst, u32 *w)
{
    struct xn_vid_player *v = &xn_vid_state;

    if (--v->rows_left == 0)
        return 1;
    *dst += v->dest_skip;
    *w = v->row_width;
    return 0;
}

/* A delta frame's commands from p: a byte 80h + n skips n pixels, n < 80h copies the next n
   bytes, 0 ends the frame; runs carry over row ends. Returns where it stopped. */
static u8 *delta(u8 *p, u8 *dst, u32 w, int refill_first)
{
    struct xn_vid_player *v = &xn_vid_state;
    u32 n, m;
    u8 b;

    for (;;) {
        if (refill_first && p > v->refill_mark)
            p = xn_vid_refill(p);
        refill_first = 1;
        b = *p++;
        if (b == 0)
            return p;
        n = b & 0x7F;
        if (b & 0x80) {
            if (n == 0)
                continue;
            while (n >= w) {
                dst += w;
                n -= w;
                if (row_done(&dst, &w)) {
                    if (xn_vid_header.version >= 2)
                        p++;                /* the row-end byte */
                    return p;
                }
            }
            dst += n;
            w -= n;
        } else {
            while (n >= w) {
                m = w;
                copy_bytes(dst, p, m);
                dst += m;
                p += m;
                if (row_done(&dst, &w)) {
                    if (xn_vid_header.version >= 2)
                        p++;
                    return p;
                }
                n -= m;
                if (n == 0)
                    break;
            }
            if (n != 0 && n < w) {
                copy_bytes(dst, p, n);
                dst += n;
                p += n;
                w -= n;
            }
        }
    }
}

/* An RLE frame's commands: a byte 80h + n, then a colour, fills n pixels; n < 80h copies the
   next n bytes (0 does nothing); runs carry over row ends; the frame ends after its last row
   (no end byte). Returns where it stopped. */
static u8 *rle(u8 *p, u8 *dst, u32 w)
{
    struct xn_vid_player *v = &xn_vid_state;
    u32 n, m;
    u8 b, colour;

    for (;;) {
        if (p > v->refill_mark)
            p = xn_vid_refill(p);
        b = *p++;
        n = b & 0x7F;
        if (n == 0)
            continue;
        if (b & 0x80) {
            colour = *p++;
            while (n >= w) {
                m = w;
                for (; w != 0; w--)
                    *dst++ = colour;
                if (row_done(&dst, &w))
                    return p;
                n -= m;
                if (n == 0)
                    break;
            }
            if (n != 0 && n < w) {
                w -= n;
                for (; n != 0; n--)
                    *dst++ = colour;
            }
        } else {
            while (n >= w) {
                m = w;
                copy_bytes(dst, p, m);
                dst += m;
                p += m;
                if (row_done(&dst, &w))
                    return p;
                n -= m;
                if (n == 0)
                    break;
            }
            if (n != 0 && n < w) {
                copy_bytes(dst, p, n);
                dst += n;
                p += n;
                w -= n;
            }
        }
    }
}

void xn_vid_decode_chunk(void)
{
    struct xn_vid_player *v = &xn_vid_state;
    u8 *p, *dst, next;
    u32 w, next_w;
    u16 start;
    u8 type;

    for (;;) {
        p = v->read_ptr;
        dst = screen_buffer + v->dest_offset;
        v->rows_left = v->rows;
        w = v->row_width;
        type = *p++;
        switch (type) {
        case VID_DELTA_FROM:
            xn_vid_schedule_frame(*(u16 *)p);
            start = *(u16 *)(p + 2);
            p += 4;
            v->rows_left -= start;
            if (v->rows_left != 0)
                p = delta(p, dst + xn_gfx_row_offset[start], w, 1);
            break;
        case VID_DELTA:
            xn_vid_schedule_frame(*(u16 *)p);
            p = delta(p + 2, dst, w, 0);
            break;
        case VID_RLE:
            xn_vid_schedule_frame(*(u16 *)p);
            p = rle(p + 2, dst, w);
            break;
        case VID_RAW:
            xn_vid_schedule_frame(*(u16 *)p);
            p += 2;
            next_w = w;
            do {
                w = next_w;
                if (v->buf_end - v->row_width < p) {
                    p = xn_vid_refill(p);
                    /* Quirk Q-VID-04: the refill leaves the file handle in BX, from which the
                       asm reloads the width of the rows after this one */
                    next_w = (v->row_width & 0xFFFF0000) | v->file;
                }
                copy_bytes(dst, p, w);
                dst += w + v->dest_skip;
                p += w;
            } while (--v->rows_left != 0);
            break;
        case VID_AUDIO_HEAD:
            audio_header(p);
            return;
        case VID_AUDIO:
            xn_vid_read_audio(p);
            return;
        case VID_PALETTE:
            palette(p);
            return;
        default:
            xn_vid_finish();
            return;
        }

        /* the frame is done */
        p = xn_vid_refill(p);
        next = *p;
        v->read_ptr = p;
        v->frames_done++;
        if (--v->frames_left == 0) {
            xn_vid_finish();
            return;
        }
        if ((s8)next < VID_AUDIO_HEAD) {
            if (v->audio_state == 3)
                v->frame_waits = 1;
            return;
        }
        /* audio chunks after a frame (7Ch..7Fh) are read at once */
    }
}

void xn_vid_close_file(void)
{
    struct xn_vid_player *v = &xn_vid_state;

    v->audio_state = 0;
    if (v->file == 0)
        return;
    xn_dos_close(v->file);
    v->file = 0;
}

u8 *xn_vid_refill(u8 *p)
{
    struct xn_vid_player *v = &xn_vid_state;
    u32 left;

    if (p == v->buf)
        return p;
    left = v->buf_end - p;
    v->file_pos += p - v->buf;
    copy_dwords(v->buf, p, left);
    xn_dos_read(v->file, v->buf + left, v->buf_end - (v->buf + left));
    return v->buf;
}

void xn_vid_fill(void)
{
    struct xn_vid_player *v = &xn_vid_state;

    v->read_ptr = v->buf;
    xn_dos_read(v->file, v->buf, VID_BUFFER);
}

void xn_vid_schedule_frame(u16 delay)
{
    struct xn_vid_player *v = &xn_vid_state;

    v->frame_ticks += (u16)(delay + xn_vid_header.frame_delay);
    if (v->audio_state != 1)
        return;
    v->audio_state++;
    if (v->audio_ticks != 0) {
        v->audio_waiting = 1;           /* the timer counts the delay down */
        return;
    }
    v->audio_state++;
    xn_vid_audio_start();
}

void xn_vid_read_audio(u8 *p)
{
    struct xn_vid_player *v = &xn_vid_state;
    u32 n;
    u8 *dst;

    v->audio_size = *(u16 *)p;
    p += 2;
    v->audio_left = v->audio_size;
    do {
        n = v->buf_end - p;
        if (n <= (u32)v->audio_size) {
            p = xn_vid_refill(p);
            n = VID_AUDIO_BUFFER;
        }
        if (n > (u32)v->audio_left)
            n = v->audio_left;
        v->audio_left -= n;
        /* Quirk Q-VID-01: each pass copies to the buffer's start */
        if (v->audio_buffer == 0) {
            dst = v->audio_buf_a;
            v->audio_len_a = v->audio_size;
        } else {
            dst = v->audio_buf_b;
            v->audio_len_b = v->audio_size;
        }
        copy_dwords(dst, p, n);
        p += n;
    } while (v->audio_left != 0);
    v->audio_buffer = (v->audio_buffer + 1) & 1;
    v->read_ptr = p;
}

void xn_vid_timer_cb(void)
{
    struct xn_vid_player *v = &xn_vid_state;

    v->frame_ticks--;
    if (v->audio_waiting != 0 && --v->audio_ticks == 0)
        v->audio_waiting = 0;
}

void __cdecl xn_vid_audio_done_cb(xn_vid_sos_sample *s)
{
    struct xn_vid_player *v = &xn_vid_state;

    if (v->sample_done == 1) {
        v->sample_restart = 1;          /* the last one was not taken yet */
        return;
    }
    if (v->audio_buffer == 0) {
        s->data = v->audio_buf_b;
        s->length = v->audio_len_b;
    } else {
        s->data = v->audio_buf_a;
        s->length = v->audio_len_a;
    }
    v->sample_done = 1;
}

void xn_vid_audio_stop(void)
{
    struct xn_vid_player *v = &xn_vid_state;

    if (v->audio_state == 0)
        return;
    v->audio_state = 0;
    sos_stop_sample(sound_digi_driver, v->sample_handle);
}

void xn_vid_audio_start(void)
{
    struct xn_vid_player *v = &xn_vid_state;
    xn_vid_sos_sample *s = &xn_vid_sample;

    if (v->audio_buffer == 0) {
        s->data = v->audio_buf_b;
        s->length = v->audio_len_b;
    } else {
        s->data = v->audio_buf_a;
        s->length = v->audio_len_a;
    }
    s->bits = 8;
    s->channels = 1;
    s->format = 0x8000;
    s->rate = v->sample_rate;
    s->pan = 0x8000;
    s->volume = 0x7F007F00;
    s->loop = 0;
    s->length2 = s->length;
    s->done = xn_vid_audio_done_cb;
    v->sample_handle = sos_start_sample(sound_digi_driver, s);
}

static void vid_code_end(void)
{
}
