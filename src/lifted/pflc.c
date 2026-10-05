/* pflc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern signed char key_down_y;
extern signed char key_down_n;
extern int screen_buffer;
extern char D_00175404[];
extern char D_0017540B[];
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern signed char mouse_buttons_prev;
extern struct quest *current_quest;

extern int flc_open(int, int);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int read();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int xn_pal_set_range_8bit();
extern int xn_mouse_poll_clamped();
extern int xn_pal_get();
extern int xn_draw_image();
extern int xn_draw_image_transparent();
extern void parse_rsc_text(int, int, int);
extern void quest_load_text(struct quest *, int, int, int);
extern void fatal_error(int);
extern void flc_decode_palette(int, int, unsigned char);
extern void flc_decode_lc(int, int);
extern void flc_decode_ss2(int, int);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
int flc_next_frame(int);
int flc_draw_text_page(int);
void flc_close(int);
void flc_read_frame(int);
void flc_decode_brun(int, int);
#pragma aux mc_set_location parm routine [];

void flc_show_frame(int a1)
{
    if (((int)(short)*(short *)((char *)a1 + 18)) != 320 || ((int)(short)*(short *)((char *)a1 + 20)) != 200) {
        xn_draw_image_transparent((int)(short)*(short *)((char *)a1 + 14), (int)(short)*(short *)((char *)a1 + 16), (int)(short)*(short *)((char *)a1 + 18), (int)(short)*(short *)((char *)a1 + 20), *(int *)((char *)a1 + 30));
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00175404, 113, 4);
        return;
    }
    if (*(short *)((char *)a1 + 6) != 0) {
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00175404, 117, 4);
        return;
    }
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 43)) <= 1) return;
    mc_memcpy(655360, screen_buffer, 64000, (int)D_00175404, 118, 4);
}

int flc_play_with_text(int a1, int a2, int a3, int a4)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    short l_C;

    l_18 = 0;
    mc_memset(a2, 0, 44, (int)D_00175404, 127, 4);
    if (flc_open(a1, a2) == 0) {
        mc_set_location(131, (int)D_00175404);
        mc_sprintf((int)text_buffer, (int)D_0017540B, a1);
        fatal_error((int)text_buffer);
    }
    mc_memset(screen_buffer, 0, 64000, (int)D_00175404, 135, 4);
    *(signed char *)((char *)a2 + 43) = 255;
    if (current_quest != 0) {
        quest_load_text(current_quest, a3, 0, 0);
    } else {
        parse_rsc_text(a3, 0, 0);
    }
    while (text_rsc_buffer[l_18] != 0) {
        if (((int)(unsigned char)(text_rsc_buffer[l_18] & 128)) != 0) {
            text_rsc_buffer[l_18] = 0;
        }
        l_18++;
    }
    l_1C = (int)text_rsc_buffer;
    for (;;) {
        *(short *)((char *)a2 + 6) += *(short *)((char *)a2 + 4);
        if (*(signed char *)((char *)a2 + 43) == 0) (*(short *)((char *)a2 + 6))--;
        while ((*(short *)((char *)a2 + 6))-- != 0) {
            l_14 = 1132;
            l_24 = *(int *)((char *)l_14);
            if (flc_next_frame(a2) != 0) break;
            l_20 = flc_draw_text_page(l_1C);
            if (*(short *)((char *)a2 + 6) != 0) {
                mc_memcpy(655360, screen_buffer, 64000, (int)D_00175404, 163, 4);
            } else if (((int)(unsigned char)*(signed char *)((char *)a2 + 43)) > 1) {
                mc_memcpy(655360, screen_buffer, 64000, (int)D_00175404, 164, 4);
            }
            do {
                mouse_buttons_prev = mouse_buttons;
                xn_mouse_poll_clamped();
                if (l_20 == 0 && a4 != 0) {
                    if (key_down_y != 0) {
                        flc_close(a2);
                        return 1;
                    }
                    if (key_down_n != 0) {
                        flc_close(a2);
                        return 2;
                    }
                }
                if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
                    if (l_20 != 0) {
                        l_1C = l_20;
                    } else {
                        if (a4 == 0) goto L52084;
                    }
                }
                *(int *)&l_C = 1132;
            } while ((*(int *)((char *)*(int *)&l_C) - l_24) < *(unsigned short *)((char *)a2 + 8));
        }
        if (*(signed char *)((char *)a2 + 43) == 0) break;
        if (((int)(unsigned char)*(signed char *)((char *)a2 + 43)) != 255) {
            (*(signed char *)((char *)a2 + 43))--;
            if (*(signed char *)((char *)a2 + 43) == 0) break;
        }
        lseek((int)(unsigned short)*(short *)((char *)a2 + 2), *(int *)((char *)a2 + 10), 0);
        *(short *)((char *)a2 + 6) = 0;
    }
L52084:;
    flc_close(a2);
    return 0;
}

void flc_close(int a1)
{
    if (((int)(unsigned short)(*(short *)((char *)a1) & 1)) != 0) {
        if (*(int *)((char *)a1 + 22) != 0 && *(int *)((char *)a1 + 22) != (-1751672937)) {
            mc_free(*(int *)((char *)a1 + 22), (int)D_00175404, 263);
            *(int *)((char *)a1 + 22) = -1751672937;
        }
    }
    if (((int)(unsigned short)(*(short *)((char *)a1) & 2)) != 0) {
        if (*(int *)((char *)a1 + 26) != 0 && *(int *)((char *)a1 + 26) != (-1751672937)) {
            mc_free(*(int *)((char *)a1 + 26), (int)D_00175404, 264);
            *(int *)((char *)a1 + 26) = -1751672937;
        }
    }
    if (((int)(unsigned short)(*(short *)((char *)a1) & 64)) != 0) {
        if (*(int *)((char *)a1 + 30) != 0 && *(int *)((char *)a1 + 30) != (-1751672937)) {
            mc_free(*(int *)((char *)a1 + 30), (int)D_00175404, 265);
            *(int *)((char *)a1 + 30) = -1751672937;
        }
    }
    close((int)(unsigned short)*(short *)((char *)a1 + 2));
}

int flc_next_frame(int a1)
{
    if (((int)(unsigned short)(*(short *)((char *)a1) & 4)) != 0) {
        *(signed char *)((char *)a1 + 43) = 0;
        return 1;
    }
    flc_read_frame(a1);
    if (((int)(unsigned short)(*(short *)((char *)a1) & 4)) != 0) {
        *(signed char *)((char *)a1 + 43) = 0;
        return 1;
    }
    if (((int)(unsigned short)(*(short *)((char *)a1) & 8)) != 0 && ((int)(unsigned short)(*(short *)((char *)a1) & 16)) == 0) {
        *(signed char *)((char *)a1) &= 247;
        xn_pal_set_range_8bit(*(int *)((char *)a1 + 26), 0, 256);
    }
    return 0;
}

void flc_read_frame(int a1)
{
    unsigned short l_20;
    unsigned short l_1C;
    short l_18;
    {
        char l_2C[8];

        l_20 = *(short *)((char *)a1 + 2);
        for (;;) {
            read((int)(unsigned short)l_20, (int)l_2C, 6);
            if (((int)(unsigned short)*(short *)((char *)l_2C + 4)) == 61946) break;
            lseek((int)(unsigned short)l_20, (int)(*(char **)l_2C - 6), 1);
        }
        read((int)(unsigned short)l_20, (int)&l_18, 2);
        lseek((int)(unsigned short)l_20, 8, 1);
        *(int *)&l_1C = 0;
        for (; (unsigned short)l_1C < (short)l_18; (*(int *)&l_1C)++) {
            read((int)(unsigned short)l_20, (int)l_2C, 6);
            *(int *)l_2C += -6;
            switch ((unsigned short)*(int *)((char *)l_2C + 4)) {
            case 4:
                if (((int)(unsigned short)(*(short *)((char *)a1) & 16)) != 0) {
                    lseek((int)(unsigned short)l_20, (int)(unsigned short)*(short *)l_2C, 1);
                } else {
                    xn_pal_get((int)(*(char **)((char *)a1 + 26) + 768));
                    read((int)(unsigned short)l_20, (int)(*(char **)((char *)a1 + 26) + 768), (int)(unsigned short)*(short *)l_2C);
                    flc_decode_palette(*(int *)((char *)a1 + 26), (int)&*(signed char *)(*(char **)((char *)a1 + 26) + 768), 0);
                    *(signed char *)((char *)a1) |= 8;
                }
                break;
            case 11:
                if (((int)(unsigned short)(*(short *)((char *)a1) & 16)) != 0) {
                    lseek((int)(unsigned short)l_20, (int)(unsigned short)*(short *)l_2C, 1);
                } else {
                    xn_pal_get((int)(*(char **)((char *)a1 + 26) + 768));
                    read((int)(unsigned short)l_20, (int)(*(char **)((char *)a1 + 26) + 768), (int)(unsigned short)*(short *)l_2C);
                    flc_decode_palette(*(int *)((char *)a1 + 26), (int)&*(signed char *)(*(char **)((char *)a1 + 26) + 768), 0);
                    *(signed char *)((char *)a1) |= 8;
                }
                break;
            case 13:
                mc_memset(*(int *)((char *)a1 + 30), 0, ((int)(short)*(short *)((char *)a1 + 18)) * ((int)(short)*(short *)((char *)a1 + 20)), (int)D_00175404, 347, 4);
                break;
            case 16:
                if (((int)(unsigned short)(*(short *)((char *)a1) & 128)) == 0) {
                    read((int)(unsigned short)l_20, screen_buffer, (int)(unsigned short)*(short *)l_2C);
                } else {
                    read((int)(unsigned short)l_20, *(int *)((char *)a1 + 30), (int)(unsigned short)*(short *)l_2C);
                }
                break;
            case 15:
                read((int)(unsigned short)l_20, *(int *)((char *)a1 + 22), (int)(unsigned short)*(short *)l_2C);
                flc_decode_brun(*(int *)((char *)a1 + 22), a1);
                break;
            case 12:
                read((int)(unsigned short)l_20, *(int *)((char *)a1 + 22), (int)(unsigned short)*(short *)l_2C);
                flc_decode_lc(*(int *)((char *)a1 + 22), a1);
                break;
            case 7:
                read((int)(unsigned short)l_20, *(int *)((char *)a1 + 22), (int)(unsigned short)*(short *)l_2C);
                flc_decode_ss2(*(int *)((char *)a1 + 22), a1);
                break;
            default:
                lseek((int)(unsigned short)l_20, *(int *)l_2C, 1);
            }
        }
        xn_draw_image((int)(short)*(short *)((char *)a1 + 14), (int)(short)*(short *)((char *)a1 + 16), (int)(short)*(short *)((char *)a1 + 18), (int)(short)*(short *)((char *)a1 + 20), *(int *)((char *)a1 + 30));
    }
}

void flc_decode_brun(int a1, int a2)
{
    int l_1C;
    signed char l_14;
    short l_18;

    for (l_1C = 0; (short)(short)l_1C < *(short *)((char *)a2 + 20); l_1C++) {
        ++a1;
        l_18 = 0;
        do {
            l_14 = *(signed char *)((char *)a1++);
            if (l_14 > 0) {
                mc_memset((int)(*(char **)((char *)a2 + 30) + (((int)(short)*(short *)((char *)a2 + 18)) * ((int)(short)*(short *)&l_1C))) + ((int)(short)l_18), (int)(unsigned char)*(signed char *)((char *)a1), (int)(signed char)l_14, (int)D_00175404, 443, 4);
                l_18 += (short)(signed char)l_14;
                a1++;
            } else if (l_14 < 0) {
                mc_memcpy((int)(*(char **)((char *)a2 + 30) + (((int)(short)*(short *)((char *)a2 + 18)) * ((int)(short)*(short *)&l_1C))) + ((int)(short)l_18), a1, -((int)(signed char)l_14), (int)D_00175404, 449, 4);
                l_18 -= (short)(signed char)l_14;
                a1 -= (int)(signed char)l_14;
            }
        } while ((short)(short)*(int *)&l_18 < *(short *)((char *)a2 + 18));
    }
}

int flc_draw_text_page(int a1)
{
    int l_20;
    int l_1C;

    l_1C = 0;
    l_20 = 150;
    while (*(signed char *)((char *)a1) != 0 && l_1C < 4) {
        text_draw_centred_coloured(a1, 160, (int)(short)*(short *)&l_20, 145, 156);
        l_20 += 10;
        l_1C++;
        a1 += strlen(a1) + 1;
    }
    if (*(signed char *)((char *)a1) != 0) return a1;
    return 0;
}
