/* pflc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern signed char key_down_y;
extern signed char key_down_n;
extern iptr screen_buffer;
extern char D_00175404[];
extern char D_0017540B[];
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern signed char mouse_buttons_prev;
extern struct quest *current_quest;

extern int flc_open(int, struct flc_player *);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int read();
extern int strlen();
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern int mc_memcpy();
extern int xn_pal_set_range_8bit();
extern int xn_mouse_poll_clamped();
extern int xn_pal_get();
extern int xn_draw_image();
extern int xn_draw_image_transparent();
extern void parse_rsc_text(int, int, int);
extern void quest_load_text(struct quest *, int, short, int);
extern void fatal_error(char *);
extern void flc_decode_palette(unsigned char *, unsigned char *, unsigned char);
extern void flc_decode_lc(unsigned char *, struct flc_player *);
extern void flc_decode_ss2(unsigned char *, struct flc_player *);
extern void text_draw_centred_coloured(char *, int, int, int, unsigned char);
int flc_next_frame(struct flc_player *);
char *flc_draw_text_page(char *);
void flc_close(struct flc_player *);
void flc_read_frame(struct flc_player *);
void flc_decode_brun(signed char *, struct flc_player *);
#pragma aux mc_set_location parm routine [];

void flc_show_frame(struct flc_player *anim)
{
    if (anim->width != 320 || anim->height != 200) {
        xn_draw_image_transparent(anim->x, anim->y, anim->width, anim->height, anim->image);
        mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00175404, 113, 4);
        return;
    }
    if (anim->frames_left != 0) {
        mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00175404, 117, 4);
        return;
    }
    if (anim->loops <= 1) return;
    mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00175404, 118, 4);
}

int flc_play_with_text(int name, struct flc_player *anim, int text_id, int ask_yes_no)
{
    int frame_start;
    char *next_page;
    char *page;
    int i;
    int *ticks_addr;
    short ticks_addr2;

    i = 0;
    mc_memset(anim, 0, 44, (iptr)D_00175404, 127, 4);
    if (flc_open(name, anim) == 0) {
        mc_set_location(131, (iptr)D_00175404);
        mc_sprintf((iptr)text_buffer, (iptr)D_0017540B, name);
        fatal_error(text_buffer);
    }
    mc_memset(screen_buffer, 0, 64000, (iptr)D_00175404, 135, 4);
    anim->loops = 255;
    if (current_quest != 0) {
        quest_load_text(current_quest, text_id, 0, 0);
    } else {
        parse_rsc_text(text_id, 0, 0);
    }
    while (text_rsc_buffer[i] != 0) {
        if (((int)(unsigned char)(text_rsc_buffer[i] & 128)) != 0) {
            text_rsc_buffer[i] = 0;
        }
        i++;
    }
    page = (char *)text_rsc_buffer;
    for (;;) {
        anim->frames_left += anim->frame_count;
        if (anim->loops == 0) anim->frames_left--;
        while (anim->frames_left-- != 0) {
            ticks_addr = (int *)1132;
            frame_start = *ticks_addr;
            if (flc_next_frame(anim) != 0) break;
            next_page = flc_draw_text_page(page);
            if (anim->frames_left != 0) {
                mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00175404, 163, 4);
            } else if (anim->loops > 1) {
                mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00175404, 164, 4);
            }
            do {
                mouse_buttons_prev = mouse_buttons;
                xn_mouse_poll_clamped();
                if (next_page == 0 && ask_yes_no != 0) {
                    if (key_down_y != 0) {
                        flc_close(anim);
                        return 1;
                    }
                    if (key_down_n != 0) {
                        flc_close(anim);
                        return 2;
                    }
                }
                if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
                    if (next_page != 0) {
                        page = next_page;
                    } else {
                        if (ask_yes_no == 0) goto L52084;
                    }
                }
                *(int *)&ticks_addr2 = 1132;
            } while ((*(int *)((char *)*(iptr *)&ticks_addr2) - frame_start) < anim->ticks_per_frame);
        }
        if (anim->loops == 0) break;
        if (anim->loops != 255) {
            (anim->loops)--;
            if (anim->loops == 0) break;
        }
        lseek(anim->handle, anim->loop_offset, 0);
        anim->frames_left = 0;
    }
L52084:;
    flc_close(anim);
    return 0;
}

void flc_close(struct flc_player *anim)
{
    if ((anim->flags & 1) != 0) {
        if (anim->chunk != 0 && anim->chunk != (char *)(iptr)-1751672937) {
            mc_free(anim->chunk, (iptr)D_00175404, 263);
            anim->chunk = (char *)(iptr)-1751672937;
        }
    }
    if ((anim->flags & 2) != 0) {
        if (anim->palette != 0 && anim->palette != (char *)(iptr)-1751672937) {
            mc_free(anim->palette, (iptr)D_00175404, 264);
            anim->palette = (char *)(iptr)-1751672937;
        }
    }
    if ((anim->flags & 64) != 0) {
        if (anim->image != 0 && anim->image != (char *)(iptr)-1751672937) {
            mc_free(anim->image, (iptr)D_00175404, 265);
            anim->image = (char *)(iptr)-1751672937;
        }
    }
    close(anim->handle);
}

int flc_next_frame(struct flc_player *anim)
{
    if ((anim->flags & 4) != 0) {
        anim->loops = 0;
        return 1;
    }
    flc_read_frame(anim);
    if ((anim->flags & 4) != 0) {
        anim->loops = 0;
        return 1;
    }
    if ((anim->flags & 8) != 0 && (anim->flags & 16) == 0) {
        *(signed char *)&anim->flags &= 247;
        xn_pal_set_range_8bit(anim->palette, 0, 256);
    }
    return 0;
}

void flc_read_frame(struct flc_player *anim)
{
    unsigned short handle;
    unsigned short i;
    short chunk_count;
    {
        struct {
            int size;               /* +0x00: the chunk's, header included */
            unsigned short type;    /* +0x04: 0xF1FA a frame; 4, 11 palettes, 13 black, 16 copy,
                                       15 BRUN, 12 LC, 7 SS2 */
            short pad06;            /* +0x06 */
        } chunk;

        handle = anim->handle;
        for (;;) {
            read((int)(unsigned short)handle, (iptr)&chunk, 6);
            if (chunk.type == 61946) break;
            lseek((int)(unsigned short)handle, chunk.size - 6, 1);
        }
        read((int)(unsigned short)handle, (iptr)&chunk_count, 2);
        lseek((int)(unsigned short)handle, 8, 1);
        *(int *)&i = 0;
        for (; (unsigned short)i < (short)chunk_count; (*(int *)&i)++) {
            read((int)(unsigned short)handle, (iptr)&chunk, 6);
            chunk.size += -6;
            switch (chunk.type) {
            case 4:
                if ((anim->flags & 16) != 0) {
                    lseek((int)(unsigned short)handle, (unsigned short)chunk.size, 1);
                } else {
                    xn_pal_get(anim->palette + 768);
                    read((int)(unsigned short)handle, anim->palette + 768, (unsigned short)chunk.size);
                    flc_decode_palette(anim->palette, anim->palette + 768, 0);
                    *(signed char *)&anim->flags |= 8;
                }
                break;
            case 11:
                if ((anim->flags & 16) != 0) {
                    lseek((int)(unsigned short)handle, (unsigned short)chunk.size, 1);
                } else {
                    xn_pal_get(anim->palette + 768);
                    read((int)(unsigned short)handle, anim->palette + 768, (unsigned short)chunk.size);
                    flc_decode_palette(anim->palette, anim->palette + 768, 0);
                    *(signed char *)&anim->flags |= 8;
                }
                break;
            case 13:
                mc_memset(anim->image, 0, anim->width * anim->height, (iptr)D_00175404, 347, 4);
                break;
            case 16:
                if ((anim->flags & 128) == 0) {
                    read((int)(unsigned short)handle, screen_buffer, (unsigned short)chunk.size);
                } else {
                    read((int)(unsigned short)handle, anim->image, (unsigned short)chunk.size);
                }
                break;
            case 15:
                read((int)(unsigned short)handle, anim->chunk, (unsigned short)chunk.size);
                flc_decode_brun(anim->chunk, anim);
                break;
            case 12:
                read((int)(unsigned short)handle, anim->chunk, (unsigned short)chunk.size);
                flc_decode_lc(anim->chunk, anim);
                break;
            case 7:
                read((int)(unsigned short)handle, anim->chunk, (unsigned short)chunk.size);
                flc_decode_ss2(anim->chunk, anim);
                break;
            default:
                lseek((int)(unsigned short)handle, chunk.size, 1);
            }
        }
        xn_draw_image(anim->x, anim->y, anim->width, anim->height, anim->image);
    }
}

void flc_decode_brun(signed char *chunk, struct flc_player *anim)
{
    int y;
    signed char count;
    short x;

    for (y = 0; (short)(short)y < anim->height; y++) {
        ++chunk;
        x = 0;
        do {
            count = *chunk++;
            if (count > 0) {
                mc_memset((iptr)(anim->image + (anim->width * ((int)(short)*(short *)&y))) + ((int)(short)x), (int)(unsigned char)*chunk, (int)(signed char)count, (iptr)D_00175404, 443, 4);
                x += (short)(signed char)count;
                chunk++;
            } else if (count < 0) {
                mc_memcpy((iptr)(anim->image + (anim->width * ((int)(short)*(short *)&y))) + ((int)(short)x), chunk, -((int)(signed char)count), (iptr)D_00175404, 449, 4);
                x -= (short)(signed char)count;
                chunk -= (int)(signed char)count;
            }
        } while ((short)(short)*(int *)&x < anim->width);
    }
}

char *flc_draw_text_page(char *text)
{
    int y;
    int line;

    line = 0;
    y = 150;
    while (*text != 0 && line < 4) {
        text_draw_centred_coloured(text, 160, (int)(short)*(short *)&y, 145, 156);
        y += 10;
        line++;
        text += strlen(text) + 1;
    }
    if (*text != 0) return text;
    return 0;
}
