/* pflc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char mouse_buttons[];
extern char D_0014231D[];
extern char D_00142339[];
extern char screen_buffer[];
extern char D_00175404[];
extern char D_0017540B[];
extern char text_buffer[];
extern char text_rsc_buffer[];
extern char mouse_buttons_prev[];
extern struct quest *current_quest;

extern int flc_open(int, int);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int func_000A00CB();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000CD33A();
extern int func_0012B136();
extern int func_0012D887();
extern int func_00144F68();
extern int func_00144FB4();
extern void parse_rsc_text(int, int, int);
extern void quest_load_text(struct quest *, int, int, int);
extern void fatal_error(int);
extern void flc_decode_palette(int, int, unsigned char);
extern void flc_decode_lc(int, int);
extern void flc_decode_ss2(int, int);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
int flc_next_frame(int);
int flc_draw_text_page(int);
void flc_close(int);
void flc_read_frame(int);
void flc_decode_brun(int, int);
#pragma aux func_000A0ED9 parm routine [];

void func_00051CF9(int a1)
{
    if (((int)(short)*(short *)((char *)a1 + 18)) != 320) goto L51D26;
    if (((int)(short)*(short *)((char *)a1 + 20)) == 200) goto L51D6D;
L51D26:;
    func_00144FB4((int)(short)*(short *)((char *)a1 + 14), (int)(short)*(short *)((char *)a1 + 16), (int)(short)*(short *)((char *)a1 + 18), (int)(short)*(short *)((char *)a1 + 20), *(int *)((char *)a1 + 30));
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00175404, 113, 4);
    return;
L51D6D:;
    if (*(short *)((char *)a1 + 6) == 0) goto L51D97;
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00175404, 117, 4);
    return;
L51D97:;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 43)) <= 1) return;
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00175404, 118, 4);
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
    if (flc_open(a1, a2) != 0) goto L51E46;
    func_000A0ED9(131, (int)D_00175404);
    mc_sprintf((int)text_buffer, (int)D_0017540B, a1);
    fatal_error((int)text_buffer);
L51E46:;
    mc_memset(*(int *)screen_buffer, 0, 64000, (int)D_00175404, 135, 4);
    *(signed char *)((char *)a2 + 43) = 255;
    if (current_quest == 0) goto L51E86;
    quest_load_text(current_quest, a3, 0, 0);
    goto L51E92;
L51E86:;
    parse_rsc_text(a3, 0, 0);
L51E92:;
    if (*(signed char *)(text_rsc_buffer + l_18) == 0) goto L51EC4;
    if (((int)(unsigned char)(*(signed char *)(text_rsc_buffer + l_18) & 128)) == 0) goto L51EBC;
    *(signed char *)(text_rsc_buffer + l_18) = 0;
L51EBC:;
    l_18++;
    goto L51E92;
L51EC4:;
    l_1C = (int)text_rsc_buffer;
L51ECB:;
    *(short *)((char *)a2 + 6) += *(short *)((char *)a2 + 4);
    if (*(signed char *)((char *)a2 + 43) != 0) goto L51EE9;
    (*(short *)((char *)a2 + 6))--;
L51EE9:;
    if ((*(short *)((char *)a2 + 6))-- == 0) goto L52033;
    l_14 = 1132;
    l_24 = *(int *)((char *)l_14);
    if (flc_next_frame(a2) != 0) goto L52033;
    l_20 = flc_draw_text_page(l_1C);
    if (*(short *)((char *)a2 + 6) == 0) goto L51F57;
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00175404, 163, 4);
    goto L51F88;
L51F57:;
    if (((int)(unsigned char)*(signed char *)((char *)a2 + 43)) <= 1) goto L51F88;
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00175404, 164, 4);
L51F88:;
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    if (l_20 != 0) goto L51FA3;
    if (a4 != 0) goto L51FA5;
L51FA3:;
    goto L51FDF;
L51FA5:;
    if (*(signed char *)D_0014231D == 0) goto L51FC2;
    flc_close(a2);
    return 1;
L51FC2:;
    if (*(signed char *)D_00142339 == 0) goto L51FDF;
    flc_close(a2);
    return 2;
L51FDF:;
    if (*(signed char *)mouse_buttons == 0) goto L51FF1;
    if (*(signed char *)mouse_buttons_prev == 0) goto L51FF3;
L51FF1:;
    goto L5200B;
L51FF3:;
    if (l_20 == 0) goto L52001;
    l_1C = l_20;
    goto L5200B;
L52001:;
    if (a4 == 0) goto L52084;
L5200B:;
    *(int *)&l_C = 1132;
    if ((*(int *)((char *)*(int *)&l_C) - l_24) < *(unsigned short *)((char *)a2 + 8)) goto L51F88;
    goto L51EE9;
L52033:;
    if (*(signed char *)((char *)a2 + 43) == 0) goto L52084;
    if (((int)(unsigned char)*(signed char *)((char *)a2 + 43)) == 255) goto L5205D;
    (*(signed char *)((char *)a2 + 43))--;
    if (*(signed char *)((char *)a2 + 43) == 0) goto L52084;
L5205D:;
    lseek((int)(unsigned short)*(short *)((char *)a2 + 2), *(int *)((char *)a2 + 10), 0);
    *(short *)((char *)a2 + 6) = 0;
    goto L51ECB;
L52084:;
    flc_close(a2);
    return 0;
}

void flc_close(int a1)
{
    if (((int)(unsigned short)(*(short *)((char *)a1) & 1)) == 0) goto L52312;
    if (*(int *)((char *)a1 + 22) == 0) goto L522F1;
    if (*(int *)((char *)a1 + 22) != (-1751672937)) goto L522F3;
L522F1:;
    goto L52312;
L522F3:;
    mc_free(*(int *)((char *)a1 + 22), (int)D_00175404, 263);
    *(int *)((char *)a1 + 22) = -1751672937;
L52312:;
    if (((int)(unsigned short)(*(short *)((char *)a1) & 2)) == 0) goto L5235C;
    if (*(int *)((char *)a1 + 26) == 0) goto L5233B;
    if (*(int *)((char *)a1 + 26) != (-1751672937)) goto L5233D;
L5233B:;
    goto L5235C;
L5233D:;
    mc_free(*(int *)((char *)a1 + 26), (int)D_00175404, 264);
    *(int *)((char *)a1 + 26) = -1751672937;
L5235C:;
    if (((int)(unsigned short)(*(short *)((char *)a1) & 64)) == 0) goto L523A6;
    if (*(int *)((char *)a1 + 30) == 0) goto L52385;
    if (*(int *)((char *)a1 + 30) != (-1751672937)) goto L52387;
L52385:;
    goto L523A6;
L52387:;
    mc_free(*(int *)((char *)a1 + 30), (int)D_00175404, 265);
    *(int *)((char *)a1 + 30) = -1751672937;
L523A6:;
    func_0009DEA7((int)(unsigned short)*(short *)((char *)a1 + 2));
}

int flc_next_frame(int a1)
{
    if (((int)(unsigned short)(*(short *)((char *)a1) & 4)) == 0) goto L523F9;
    *(signed char *)((char *)a1 + 43) = 0;
    return 1;
L523F9:;
    flc_read_frame(a1);
    if (((int)(unsigned short)(*(short *)((char *)a1) & 4)) == 0) goto L52425;
    *(signed char *)((char *)a1 + 43) = 0;
    return 1;
L52425:;
    if (((int)(unsigned short)(*(short *)((char *)a1) & 8)) == 0) goto L5244D;
    if (((int)(unsigned short)(*(short *)((char *)a1) & 16)) == 0) goto L5244F;
L5244D:;
    goto L52467;
L5244F:;
    *(signed char *)((char *)a1) &= 247;
    func_000CD33A(*(int *)((char *)a1 + 26), 0, 256);
L52467:;
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
L52496:;
    func_000A00CB((int)(unsigned short)l_20, (int)l_2C, 6);
    if (((int)(unsigned short)*(short *)((char *)l_2C + 4)) == 61946) goto L524CE;
    lseek((int)(unsigned short)l_20, (int)(*(char **)l_2C - 6), 1);
    goto L52496;
L524CE:;
    func_000A00CB((int)(unsigned short)l_20, (int)&l_18, 2);
    lseek((int)(unsigned short)l_20, 8, 1);
    *(int *)&l_1C = 0;
L524FD:;
    if ((unsigned short)l_1C < (short)l_18) goto L52518;
    goto L527A5;
L52510:;
    (*(int *)&l_1C)++;
    goto L524FD;
L52518:;
    func_000A00CB((int)(unsigned short)l_20, (int)l_2C, 6);
    *(int *)l_2C += -6;
    switch ((unsigned short)*(int *)((char *)l_2C + 4)) {
case 4:
    if (((int)(unsigned short)(*(short *)((char *)a1) & 16)) == 0) goto L525D2;
    lseek((int)(unsigned short)l_20, (int)(unsigned short)*(short *)l_2C, 1);
    goto L5261E;
L525D2:;
    func_0012D887((int)(*(char **)((char *)a1 + 26) + 768));
    func_000A00CB((int)(unsigned short)l_20, (int)(*(char **)((char *)a1 + 26) + 768), (int)(unsigned short)*(short *)l_2C);
    flc_decode_palette(*(int *)((char *)a1 + 26), (int)&*(signed char *)(*(char **)((char *)a1 + 26) + 768), 0);
    *(signed char *)((char *)a1) |= 8;
L5261E:;
    goto L527A0;
case 11:
    if (((int)(unsigned short)(*(short *)((char *)a1) & 16)) == 0) goto L5264F;
    lseek((int)(unsigned short)l_20, (int)(unsigned short)*(short *)l_2C, 1);
    goto L5269B;
L5264F:;
    func_0012D887((int)(*(char **)((char *)a1 + 26) + 768));
    func_000A00CB((int)(unsigned short)l_20, (int)(*(char **)((char *)a1 + 26) + 768), (int)(unsigned short)*(short *)l_2C);
    flc_decode_palette(*(int *)((char *)a1 + 26), (int)&*(signed char *)(*(char **)((char *)a1 + 26) + 768), 0);
    *(signed char *)((char *)a1) |= 8;
L5269B:;
    goto L527A0;
case 13:
    mc_memset(*(int *)((char *)a1 + 30), 0, ((int)(short)*(short *)((char *)a1 + 18)) * ((int)(short)*(short *)((char *)a1 + 20)), (int)D_00175404, 347, 4);
    goto L527A0;
case 16:
    if (((int)(unsigned short)(*(short *)((char *)a1) & 128)) != 0) goto L526FC;
    func_000A00CB((int)(unsigned short)l_20, *(int *)screen_buffer, (int)(unsigned short)*(short *)l_2C);
    goto L52713;
L526FC:;
    func_000A00CB((int)(unsigned short)l_20, *(int *)((char *)a1 + 30), (int)(unsigned short)*(short *)l_2C);
L52713:;
    goto L527A0;
case 15:
    func_000A00CB((int)(unsigned short)l_20, *(int *)((char *)a1 + 22), (int)(unsigned short)*(short *)l_2C);
    flc_decode_brun(*(int *)((char *)a1 + 22), a1);
    goto L527A0;
case 12:
    func_000A00CB((int)(unsigned short)l_20, *(int *)((char *)a1 + 22), (int)(unsigned short)*(short *)l_2C);
    flc_decode_lc(*(int *)((char *)a1 + 22), a1);
    goto L527A0;
case 7:
    func_000A00CB((int)(unsigned short)l_20, *(int *)((char *)a1 + 22), (int)(unsigned short)*(short *)l_2C);
    flc_decode_ss2(*(int *)((char *)a1 + 22), a1);
    goto L527A0;
default:
    lseek((int)(unsigned short)l_20, *(int *)l_2C, 1);
L527A0:;
    goto L52510;
L527A5:;
    func_00144F68((int)(short)*(short *)((char *)a1 + 14), (int)(short)*(short *)((char *)a1 + 16), (int)(short)*(short *)((char *)a1 + 18), (int)(short)*(short *)((char *)a1 + 20), *(int *)((char *)a1 + 30));
}
}
}

void flc_decode_brun(int a1, int a2)
{
    int l_1C;
    signed char l_14;
    short l_18;

    l_1C = 0;
L52915:;
    if ((short)(short)l_1C < *(short *)((char *)a2 + 20)) goto L5292E;
    return;
L52926:;
    l_1C++;
    goto L52915;
L5292E:;
    ++a1;
    l_18 = 0;
L52938:;
    l_14 = *(signed char *)((char *)a1++);
    if (l_14 <= 0) goto L52991;
    mc_memset((int)(*(char **)((char *)a2 + 30) + (((int)(short)*(short *)((char *)a2 + 18)) * ((int)(short)*(short *)&l_1C))) + ((int)(short)l_18), (int)(unsigned char)*(signed char *)((char *)a1), (int)(signed char)l_14, (int)D_00175404, 443, 4);
    l_18 += (short)(signed char)l_14;
    a1++;
    goto L529DC;
L52991:;
    if (l_14 >= 0) goto L529DC;
    mc_memcpy((int)(*(char **)((char *)a2 + 30) + (((int)(short)*(short *)((char *)a2 + 18)) * ((int)(short)*(short *)&l_1C))) + ((int)(short)l_18), a1, -((int)(signed char)l_14), (int)D_00175404, 449, 4);
    l_18 -= (short)(signed char)l_14;
    a1 -= (int)(signed char)l_14;
L529DC:;
    if ((short)(short)*(int *)&l_18 < *(short *)((char *)a2 + 18)) goto L52938;
    goto L52926;
}

int flc_draw_text_page(int a1)
{
    int l_20;
    int l_1C;

    l_1C = 0;
    l_20 = 150;
L52D5B:;
    if (*(signed char *)((char *)a1) == 0) goto L52D69;
    if (l_1C < 4) goto L52D6B;
L52D69:;
    goto L52D9F;
L52D6B:;
    text_draw_centered_colored(a1, 160, (int)(short)*(short *)&l_20, 145, 156);
    l_20 += 10;
    l_1C++;
    a1 += func_000A0DF4(a1) + 1;
    goto L52D5B;
L52D9F:;
    if (*(signed char *)((char *)a1) == 0) goto L52DAF;
    return a1;
L52DAF:;
    return 0;
}
