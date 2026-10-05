/* vid.c: XnGine's VID movie player as readable C (xvid.h; see xngine.h and
   docs/xngine_readable.md).

   The player calls the game's code (Watcom C in object 1: the DPMI locks, malloc and free,
   the SOS timer and sample calls) through game_call, with the registers its arguments arrive
   in, and keeps EDX as the calls leave it: the asm unlocks with whatever EDX holds (see
   xvid.h). Code that makes DOS or BIOS calls without saving registers (xn_dos_file_exists,
   xn_kbd_read_key) is called with a register file too. */
#include "xvid.h"
#include "xgfx.h"
#include "xpal.h"

#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA        0x3C9

#define VID_BUFFER          0xFA00      /* the read buffer: 64000 bytes */
#define VID_REFILL_MARGIN   0x81        /* a delta or RLE command refills this near the end */
#define VID_AUDIO_BUFFER    0x4000

/* chunk types */
#define VID_RAW         0
#define VID_DELTA       1
#define VID_PALETTE     2
#define VID_RLE         3
#define VID_DELTA_FROM  4
#define VID_AUDIO_HEAD  0x7C
#define VID_AUDIO       0x7D

extern s32 D_0018DD5C;      /* the SOS digital device: -1 none */
extern s32 D_0018DD60;      /* the SOS digital driver's handle */
#define sound_digi_device D_0018DD5C
#define sound_digi_driver D_0018DD60

extern u8 mouse_buttons;

/* asm entries: the game's functions, and the player's own code addresses it hands out */
extern void asm_dpmi_lock_region(void);
extern void asm_dpmi_unlock_region(void);
extern void asm_sound_timer_add(void);
extern void asm_sound_timer_remove(void);
extern void asm_func_000A10A8(void);        /* malloc */
extern void asm_func_000A117E(void);        /* free */
extern void asm_func_000A2504(void);        /* SOS: start a sample (driver, sample) */
extern void asm_func_000A2687(void);        /* SOS: stop a sample (driver, handle) */
extern void asm_xn_dos_file_exists(void);
extern void asm_xn_kbd_read_key(void);
extern void asm_xn_vid_play(void);
extern void asm_xn_vid_timer_cb(void);
extern void asm_xn_vid_audio_done_cb(void);

void xn_str_copy(const char *src, char *dst);
void xn_mouse_poll_clamped(void);

/* Calls the game's code at fn with EAX and EDX (its first two arguments); *edx comes back as
   the call leaves EDX. Returns EAX. */
static u32 game_call(void (*fn)(void), u32 eax, u32 *edx)
{
    xn_regs r;

    r.eax = eax;
    r.edx = *edx;
    r.ebx = 0;
    r.ecx = 0;
    r.esi = 0;
    r.edi = 0;
    r.ebp = 0;
    xn_asmcall(fn, &r);
    *edx = r.edx;
    return r.eax;
}

/* ---- the player ------------------------------------------------------------------------- */

/* Shows the opened movie in the clip window until it ends (or a button or key, when it is
   skippable), then sets its palette. Returns EDX as the asm leaves it. */
static u32 vid_show(u32 edx)
{
    struct xn_vid_player *v = &xn_vid_state;
    xn_regs r;

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
            r.eax = 1;                  /* (what the asm has in EAX: present's argument) */
            r.edx = edx;
            xn_asmcall(asm_xn_kbd_read_key, &r);
            edx = r.edx;
            if ((u8)r.eax != 0)
                break;
        }
        xn_vid_update(edx);
        xn_gfx_present(1);
        if (v->done == 1)
            break;
    }
    edx = xn_vid_finish(edx);
    xn_pal_set(v->palette);
    return edx;
}

int xn_vid_play(const char *path, s32 x, s32 y, s32 skippable)
{
    struct xn_vid_player *v = &xn_vid_state;
    s32 left = xn_gfx_clip_left, right = xn_gfx_clip_right;
    s32 top = xn_gfx_clip_top, bottom = xn_gfx_clip_bottom;
    int played = 0;
    u32 edx;
    xn_regs r;

    /* the timer and sample callbacks run under interrupts: lock the player's data and code */
    edx = 0x4F1;
    game_call(asm_dpmi_lock_region, (u32)&xn_vid_header, &edx);
    edx = 0x907;
    game_call(asm_dpmi_lock_region, (u32)asm_xn_vid_play, &edx);
    v->skippable = skippable;
    xn_str_copy(path, v->path);
    r.edx = (u32)v->path;
    xn_asmcall(asm_xn_dos_file_exists, &r);
    edx = r.edx;
    if (!(r.eflags & XN_CF)) {
        edx = y;
        xn_vid_open(x, y, v->path);
        if (v->timer != -1) {
            edx = vid_show(v->y);
            played = 1;
        }
    }
    if (!played) {
        /* (the buffers' pointers: 0 since the last movie, unless the timer failed) */
        game_call(asm_func_000A117E, (u32)v->audio_buf_a, &edx);
        game_call(asm_func_000A117E, (u32)v->audio_buf_b, &edx);
    }
    game_call(asm_dpmi_unlock_region, (u32)&xn_vid_header, &edx);
    game_call(asm_dpmi_unlock_region, (u32)asm_xn_vid_play, &edx);
    xn_gfx_clip_bottom = bottom;
    xn_gfx_clip_top = top;
    xn_gfx_clip_right = right;
    xn_gfx_clip_left = left;
    return played;
}

void xn_vid_play_r(xn_regs *r)
{
    r->eax = xn_vid_play((const char *)r->eax, r->edx, r->ebx, r->ecx);
    XN_SETFLAG(r, XN_CF, r->eax == 0);
}

void xn_vid_open(s32 x, s32 y, const char *path)
{
    struct xn_vid_player *v = &xn_vid_state;
    u32 edx;
    xn_regs r;

    v->file_pos = 0;
    v->x = x;
    v->y = y;
    v->path_arg = (char *)path;
    v->audio_state = 0;
    v->buf = big_buffer;
    v->buf_end = v->buf + VID_BUFFER;
    v->refill_mark = v->buf_end - VID_REFILL_MARGIN;
    edx = VID_AUDIO_BUFFER;
    v->audio_buf_a = (u8 *)game_call(asm_func_000A10A8, VID_AUDIO_BUFFER, &edx);
    edx = VID_AUDIO_BUFFER;
    game_call(asm_dpmi_lock_region, (u32)v->audio_buf_a, &edx);
    v->audio_buf_b = (u8 *)game_call(asm_func_000A10A8, VID_AUDIO_BUFFER, &edx);
    edx = VID_AUDIO_BUFFER;
    game_call(asm_dpmi_lock_region, (u32)v->audio_buf_b, &edx);
    v->audio_buffer = 0;
    v->sample_done = 0;
    v->sample_restart = 0;
    {
        u8 *p = (u8 *)&xn_vid_sample;
        u32 i;

        for (i = 0; i < sizeof(xn_vid_sample); i++)
            p[i] = 0;
    }
    v->audio_ticks = 0;
    v->frame_ticks = 0;
    edx = 60;                           /* Hz */
    v->timer = game_call(asm_sound_timer_add, (u32)asm_xn_vid_timer_cb, &edx);

    v->file = 0;
    r.eax = 0x3D00;                     /* open, read only (no error check) */
    r.edx = (u32)v->path_arg;
    xn_int21(&r);
    v->file = (u16)r.eax;
    r.ebx = v->file;
    r.ecx = sizeof(xn_vid_header);
    r.edx = (u32)&xn_vid_header;
    r.eax = 0x3F00;                     /* read the header */
    xn_int21(&r);

    v->dest_offset = xn_gfx_row_offset[v->y] + v->x;
    /* (a 16-bit subtraction: the width's top half stays) */
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
    xn_vid_decode_chunk((u32)v->buf);   /* (EDX: the buffer, from the fill) */
}

void xn_vid_update(u32 edx)
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
        edx = xn_vid_audio_stop(edx);
    }
    xn_vid_decode_chunk(edx);
}

u32 xn_vid_finish(u32 edx)
{
    struct xn_vid_player *v = &xn_vid_state;
    u8 *p;

    edx = xn_vid_audio_stop(edx);
    game_call(asm_dpmi_unlock_region, (u32)v->audio_buf_a, &edx);   /* (size: EDX) */
    game_call(asm_dpmi_unlock_region, (u32)v->audio_buf_b, &edx);
    p = v->audio_buf_a;
    v->audio_buf_a = 0;
    if (p != 0)
        game_call(asm_func_000A117E, (u32)p, &edx);
    p = v->audio_buf_b;
    v->audio_buf_b = 0;
    if (p != 0)
        game_call(asm_func_000A117E, (u32)p, &edx);
    if (!(v->flags & 1))
        xn_vid_close_file();
    v->done = 1;
    game_call(asm_sound_timer_remove, v->timer, &edx);
    return edx;
}

/* ---- decoding ----------------------------------------------------------------------------- */

/* Copies n bytes, forward */
static void copy_bytes(u8 *dst, const u8 *src, u32 n)
{
    for (; n != 0; n--)
        *dst++ = *src++;
}

/* Copies n bytes, dwords then the rest (rep movsd; rep movsb) */
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
    xn_s64 million;

    v->audio_delay = *(u16 *)p;
    p += 2;
    v->rate_byte = *p++;
    million.lo = 1000000;
    million.hi = 0;
    v->sample_rate = (u16)xn_u64_div(&million, 0x100 - v->rate_byte);
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
   next n bytes (0 does nothing); runs carry over row ends; the frame ends after its last
   row (no end byte). Returns where it stopped. */
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

void xn_vid_decode_chunk(u32 edx)
{
    struct xn_vid_player *v = &xn_vid_state;
    u8 *p, *dst, next;
    u32 w;
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
            do {
                w = v->row_width;
                if (v->buf_end - w < p)
                    p = xn_vid_refill(p);
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
            xn_vid_finish(edx);
            return;
        }

        /* the frame is done */
        p = xn_vid_refill(p);
        next = *p;
        v->read_ptr = p;
        v->frames_done++;
        if (--v->frames_left == 0) {
            xn_vid_finish(edx);
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
    xn_regs r;

    v->audio_state = 0;
    if (v->file == 0)
        return;
    r.ebx = v->file;
    r.eax = 0x3E00;                     /* close */
    xn_int21(&r);
    v->file = 0;
}

u8 *xn_vid_refill(u8 *p)
{
    struct xn_vid_player *v = &xn_vid_state;
    u32 left;
    xn_regs r;

    if (p == v->buf)
        return p;
    left = v->buf_end - p;
    v->file_pos += p - v->buf;
    copy_dwords(v->buf, p, left);
    r.edx = (u32)(v->buf + left);
    r.ecx = v->buf_end - (v->buf + left);
    r.ebx = v->file;
    r.eax = 0x3F00;                     /* read the rest of the buffer */
    xn_int21(&r);
    return v->buf;
}

void xn_vid_refill_r(xn_regs *r)
{
    if ((u8 *)r->esi != xn_vid_state.buf)
        r->ebx = (r->ebx & 0xFFFF0000) | xn_vid_state.file;
    r->esi = (u32)xn_vid_refill((u8 *)r->esi);
}

void xn_vid_fill(void)
{
    struct xn_vid_player *v = &xn_vid_state;
    xn_regs r;

    v->read_ptr = v->buf;
    r.ecx = VID_BUFFER;
    r.edx = (u32)v->buf;
    r.ebx = v->file;
    r.eax = 0x3F00;
    xn_int21(&r);
}

void xn_vid_fill_r(xn_regs *r)
{
    xn_vid_fill();
    r->edx = (u32)xn_vid_state.buf;
    r->ebx = (r->ebx & 0xFFFF0000) | xn_vid_state.file;
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
        /* each pass copies to the buffer's start (a chunk bigger than the read buffer's rest
           would overwrite its own start: kept) */
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

void xn_vid_audio_done_cb(xn_vid_sos_sample *s)
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

void xn_vid_audio_done_cb_r(xn_regs *r)
{
    xn_vid_audio_done_cb((xn_vid_sos_sample *)XN_STACK_ARG(r, 0));
}

u32 xn_vid_audio_stop(u32 edx)
{
    struct xn_vid_player *v = &xn_vid_state;

    if (v->audio_state == 0)
        return edx;
    v->audio_state = 0;
    edx = v->sample_handle;
    game_call(asm_func_000A2687, sound_digi_driver, &edx);
    return edx;
}

void xn_vid_audio_start(void)
{
    struct xn_vid_player *v = &xn_vid_state;
    xn_vid_sos_sample *s = &xn_vid_sample;
    u32 edx;

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
    s->done = asm_xn_vid_audio_done_cb;
    edx = (u32)s;
    v->sample_handle = game_call(asm_func_000A2504, sound_digi_driver, &edx);
}
