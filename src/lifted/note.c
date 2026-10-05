/* note.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern short D_000CEA30;
extern short D_000CEA34;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern short D_0012DA44;
extern short D_00142928;
extern short D_0014292C;
extern char D_00174FAC[];
extern char D_00174FF5[];
extern char D_00175010[];
extern int D_0017D1E6;
extern int D_0017D1F2;
extern char D_001851FF[];
extern signed char D_00185201[];
extern signed char text_rsc_buffer[];
extern signed char D_001940D5;
extern signed char D_001940D8;
extern char frame_counter[];
extern struct record *player_object;
extern int window_image;
extern char D_00195C44[];
extern short D_00195F36;
extern unsigned short D_00195F38;
extern signed char D_00196272;
extern signed char game_mode;
extern short D_001997B0;
extern short D_001997B2;
extern int D_001997BC;
extern int D_001997C0;
extern int D_001997C4;
extern char D_001997C8[];
extern int D_001997CC;
extern int D_001997D0;
extern char note_page[];
extern int D_001997D8;
extern char note_font[];
extern char D_001997DE[];
extern short D_001997E0;
extern short D_001997E2;
extern short note_page_free;
extern short D_001997E6;
extern short D_001997E8;
extern signed char note_tool;
extern signed char D_001997EB;
extern signed char D_001997EC;
extern signed char D_001997ED;

extern int sheet_open(int);
extern int func_0004DA4A(int);
extern int func_0004EAF4(int, int, int);
extern int font_text_width(int);
extern int sound_play(int, int, int);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int func_000A00CB();
extern int mc_strncpy();
extern int write();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int filelength();
extern int func_000A138E();
extern int strstr();
extern int func_0012B49E();
extern int func_0012DB50();
extern int func_001532B4();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void note_add_line(short, short, short, short);
extern void text_draw(int, int, int);
extern void text_draw_centred(int, int, int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void inpstr_begin_number(int);
extern void inpstr_begin_text(int, short);
int func_0004E5C7(int);
int func_0004E8C8(int);
int func_0004E917(int);
int func_0004E9B4(int, int);
int func_0004EBAC(int);
int func_0004EC16(int);
void func_0004E108(void);
void func_0004E18B(void);
void func_0004E1BC(void);
void note_find(void);
void func_0004EA3C(int, int);
void func_0004EC7A(void);
#pragma aux func_000A0ED9 parm routine [];

int note_close(void)
{
    func_0004E108();
    if (*(int *)note_page != 0 && *(int *)note_page != (-1751672937)) {
        mc_free(*(int *)note_page, (int)D_00174FAC, 206);
        *(int *)note_page = -1751672937;
    }
    if (D_001997C0 != 0 && D_001997C0 != (-1751672937)) {
        mc_free(D_001997C0, (int)D_00174FAC, 207);
        D_001997C0 = -1751672937;
    }
    if (D_001997D8 != 0 && D_001997D8 != (-1751672937)) {
        mc_free(D_001997D8, (int)D_00174FAC, 208);
        D_001997D8 = -1751672937;
    }
    if (D_001997ED == 0) {
        if (window_image != 0 && window_image != (-1751672937)) {
            mc_free(window_image, (int)D_00174FAC, 212);
            window_image = -1751672937;
        }
        game_mode = 0;
        D_00196272 = 0;
    }
    D_001997ED = 0;
    D_001940D5 &= 247;
    func_0009DEA7((int)(short)D_001997E8);
    if (((struct bf8_5_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 223;
        sheet_open(1);
    }
    return 1;
}

void func_0004DABF(void)
{
    int l_18;

    if (note_tool == 0 && D_001997EC == 0) {
        D_001940D5 |= 4;
        l_18 = func_0004EAF4(*(int *)note_page, (int)func_0004DA4A, 0);
        if (l_18 != 0) {
            if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 1)) != 0) {
                D_00195F36 = (D_001997E6 = 160 - (font_text_width(l_18 + 11) >> 1));
                D_00195F38 = (D_001997E0 = *(short *)((char *)l_18 + 3));
            } else {
                D_00195F36 = (D_001997E6 = *(short *)((char *)l_18 + 1));
                D_00195F38 = (D_001997E0 = *(short *)((char *)l_18 + 3));
            }
            D_001997EC = 1;
            func_0012DB50((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)((char *)l_18 + 5)]));
            inpstr_begin_text(l_18 + 11, 79);
            *(signed char *)D_001997DE |= 32;
            *(int *)D_001997C8 = l_18;
        } else {
            D_00195F36 = (D_001997E6 = mouse_x);
            D_00195F38 = (D_001997E0 = mouse_y);
            D_001997EC = 1;
            mc_memset((int)text_rsc_buffer, 0, 81, (int)D_00174FAC, 270, 2048);
            func_0012DB50((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(short)*(short *)note_font]));
            inpstr_begin_text((int)text_rsc_buffer, 79);
        }
        return;
    }
    if (((int)(unsigned char)note_tool) == 2 && ((int)(unsigned char)D_001997EC) != 2) {
        D_001997E6 = mouse_x;
        D_001997E0 = mouse_y;
        D_001997EC = 2;
        return;
    }
    if (((int)(unsigned char)note_tool) == 2 && ((int)(unsigned char)D_001997EC) == 2) {
        D_001997EC = 0;
        note_add_line((int)(short)D_001997E6, (int)(short)D_001997E0, (int)(short)mouse_x, (int)(short)mouse_y);
        return;
    }
    if (((int)(unsigned char)note_tool) != 3 || D_001997EC != 0) {
        return;
    }
    D_001997EC = 3;
    D_001997B0 = mouse_x;
    D_001997B2 = mouse_y;
}

void func_0004DD12(void)
{
    int l_18;

    l_18 = *(int *)note_page;
    while (*(signed char *)((char *)l_18) != 0) {
        switch (*(unsigned char *)((char *)l_18)) {
        case 1:
            if (D_001997ED == 0 && (((int)(short)(*(short *)D_001997DE & 32)) == 0 || l_18 != *(int *)D_001997C8)) {
                func_0012DB50((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)((char *)l_18 + 5)]));
                if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 64)) != 0) {
                    D_0012B508 = (*(signed char *)frame_counter & 15) + 240;
                } else {
                    D_0012B508 = *(signed char *)((char *)l_18 + 7);
                }
                if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 2)) != 0) {
                    if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 1)) != 0) {
                        text_draw_centered_colored(l_18 + 11, 160, (int)(short)*(short *)((char *)l_18 + 3), (int)(short)((int)(unsigned char)D_0012B508), 156);
                    } else {
                        text_draw_colored(l_18 + 11, (int)(short)*(short *)((char *)l_18 + 1), (int)(short)*(short *)((char *)l_18 + 3), (int)(short)((int)(unsigned char)D_0012B508), 156);
                    }
                } else if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 1)) != 0) {
                    text_draw_centred(l_18 + 11, 160, (int)(short)*(short *)((char *)l_18 + 3));
                } else {
                    text_draw(l_18 + 11, (int)(short)*(short *)((char *)l_18 + 1), (int)(short)*(short *)((char *)l_18 + 3));
                }
            }
            l_18 += 91;
            break;
        case 2:
            if (D_001997ED != 0) {
                (*(char (**)[11])&l_18)++;
            } else {
                if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 10) & 64)) != 0) {
                    D_0012B508 = (*(signed char *)frame_counter & 15) + 240;
                } else {
                    D_0012B508 = *(signed char *)((char *)l_18 + 9);
                }
                D_00142928 = *(short *)((char *)l_18 + 1);
                D_0014292C = *(short *)((char *)l_18 + 3);
                func_001532B4((int)(short)*(short *)((char *)l_18 + 5), (int)(short)*(short *)((char *)l_18 + 7));
                (*(char (**)[11])&l_18)++;
            }
        }
    }
    note_page_free = 3640 - (l_18 - *(short *)note_page);
}

void note_add_text(int a1)
{
    int l_18;

    if (((unsigned)((int)(short)note_page_free)) < 91) {
        msgbox_show_rsc(1700, 1);
        return;
    }
    mc_memcpy(D_001997D8, *(int *)note_page, 3640, (int)D_00174FAC, 361, 4);
    D_001940D5 |= 16;
    l_18 = *(int *)note_page;
    while (*(signed char *)((char *)l_18) != 0) {
        if (((int)(unsigned char)*(signed char *)((char *)l_18)) == 1) {
            l_18 += 91;
        } else {
            (*(char (**)[11])&l_18)++;
        }
    }
    *(signed char *)((char *)l_18) = 1;
    *(short *)((char *)l_18 + 1) = D_001997E6;
    *(short *)((char *)l_18 + 3) = D_001997E0;
    *(signed char *)((char *)l_18 + 7) = *(signed char *)D_001851FF;
    *(signed char *)((char *)l_18 + 5) = *(signed char *)note_font;
    *(signed char *)((char *)l_18 + 6) = *(signed char *)D_001997DE;
    mc_strncpy(l_18 + 11, a1, 4, (int)D_00174FAC, 381);
    *(int *)D_001997C8 = l_18;
    l_18 += 91;
    *(signed char *)((char *)l_18) = 0;
}

void func_0004E108(void)
{
    if (((struct bf8_4_1 *)&D_001940D5)->f == 0) return;
    D_001940D5 &= 239;
    lseek((int)(short)D_001997E8, ((int)(short)D_001997E2) * 3640, 0);
    write((int)(short)D_001997E8, *(int *)note_page, 3640);
    D_001997BC = filelength((int)(short)D_001997E8);
}

void func_0004E18B(void)
{
    func_0004E1BC();
    *(int *)D_001997C8 = (D_001997D0 = 0);
}

void func_0004E1BC(void)
{
    lseek((int)(short)D_001997E8, ((int)(short)D_001997E2) * 3640, 0);
    func_000A00CB((int)(short)D_001997E8, *(int *)note_page, 3640);
    mc_memcpy(D_001997D8, *(int *)note_page, 3640, (int)D_00174FAC, 450, 4);
}

void note_goto_page_prompt(void)
{
    int l_18;

    D_0012B508 = 146;
    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(459, (int)D_00174FAC);
    mc_sprintf(l_18, (int)D_00174FF5, D_0017D1E6);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    inpstr_begin_number(((int)(short)D_001997E2) + 1);
    D_001997EC = 4;
}

void func_0004E2AB(int a1)
{
{
    int l_20;
    int l_1C;

    l_1C = ((int)(short)*(short *)&a1) * 3640;
    if (((unsigned)l_1C) > D_001997BC) return;
    func_0004E108();
    D_001997E2 = a1;
    func_0004E1BC();
    if (D_001997ED != 0) return;
    sound_play(205, (int)player_object, 100);
}
}

void func_0004E30F(void)
{
    if (D_001997E2 == 0) return;
    func_0004E108();
    D_001997E2--;
    func_0004E18B();
    if (D_001997ED != 0) return;
    sound_play(205, (int)player_object, 100);
}

void func_0004E360(void)
{
    short l_18;

    *(int *)&l_18 = ((unsigned)lseek((int)(short)D_001997E8, 0, 2)) / 3640;
    func_0004E108();
    if (((int)(short)D_001997E2) < (((int)(short)l_18) - 1)) {
        D_001997E2++;
        func_0004E18B();
        if (D_001997ED == 0) sound_play(205, (int)player_object, 100);
        return;
    }
    if (*(signed char *)(*(char **)note_page) == 0) return;
    lseek((int)(short)D_001997E8, 0, 2);
    mc_memset(*(int *)note_page, 0, 3640, (int)D_00174FAC, 507, 4);
    write((int)(short)D_001997E8, *(int *)note_page, 3640);
    D_001997BC = filelength((int)(short)D_001997E8);
    D_001997E2++;
    if (D_001997ED != 0) return;
    sound_play(205, (int)player_object, 100);
}

void note_cycle_tool(void)
{
    note_tool++;
    note_tool &= 3;
}

void func_0004E48B(void)
{
    *(short *)D_001851FF = (unsigned short)(unsigned char)func_000A138E((int)(short)(mouse_x - D_000CEA30), (int)(short)((mouse_y - 1) - D_000CEA34));
    D_001997EB = (((int)(short)mouse_x) - 52) >> 3;
}

void note_cycle_font(void)
{
    (*(short *)note_font)++;
    *(short *)note_font &= 3;
}

void note_find_prompt(void)
{
    int l_18;

    func_0012DB50(4);
    D_001997C4 = *(int *)D_00195C44 + 56000;
    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(540, (int)D_00174FAC);
    mc_sprintf(l_18, (int)D_00175010, D_0017D1F2);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    mc_memset(D_001997C4, 0, 24, (int)D_00174FAC, 543, 4);
    inpstr_begin_text(D_001997C4, 23);
    *(signed char *)D_001997DE |= 128;
    *(int *)D_001997C8 = *(int *)note_page;
}

int func_0004E5C7(int a1)
{
    if (strstr(a1 + 11, D_001997C4) != 0) {
        *(int *)D_001997C8 = a1;
        return 1;
    }
    return 0;
}

void note_find(void)
{
    int l_20;
    short l_1C;
    short l_18;

    if (((int)(unsigned char)game_mode) != 9 || ((int)(short)(*(short *)D_001997DE & 128)) == 0 || *(signed char *)(((char *)D_001997C4)) == 0) {
        return;
    }
    *(short *)D_001997DE &= 127;
    l_18 = D_001997E2;
    func_0004E108();
    *(int *)&l_1C = ((unsigned)lseek((int)(short)D_001997E8, 0, 2)) / 3640;
    if (D_001997D0 != 0) {
        *(int *)D_001997C8 = D_001997D0;
    } else {
        *(int *)D_001997C8 = *(int *)note_page;
    }
    while (D_001997E2 < l_1C) {
        func_0004E1BC();
        if (func_0004EAF4(*(int *)D_001997C8, (int)func_0004E5C7, 0) != 0) goto L4E6F9;
        D_001997E2++;
        *(int *)D_001997C8 = *(int *)note_page;
    }
    D_001997E2 = *(int *)&l_18;
    func_0004E18B();
    msgbox_show_rsc(1701, 1);
    return;
L4E6F9:;
    func_0012B49E((int)(short)*(short *)(*(char **)D_001997C8 + 1), (int)(short)*(short *)(*(char **)D_001997C8 + 3));
    func_0004E1BC();
    D_001997D0 = *(int *)D_001997C8;
}

void func_0004E729(void)
{
    if (D_001997C4 == 0) return;
    if (((int)(unsigned char)*(signed char *)(*(char **)&D_001997D0)) == 1) {
        D_001997D0 = (int)(*(char **)&D_001997D0 + 91);
    } else {
        D_001997D0 += 11;
    }
    *(signed char *)D_001997DE |= 128;
    note_find();
}

void func_0004E77D(void)
{
    mc_memcpy(*(int *)note_page, D_001997D8, 3640, (int)D_00174FAC, 611, 4);
}

void func_0004E7B6(void)
{
    mc_memcpy(D_001997D8, *(int *)note_page, 3640, (int)D_00174FAC, 616, 4);
    mc_memset(*(int *)note_page, 0, 3640, (int)D_00174FAC, 617, 4);
}

void func_0004E80C(void)
{
    mc_memcpy(D_001997D8, *(int *)note_page, 3640, (int)D_00174FAC, 622, 4);
    if (*(int *)D_001997C8 == 0 || ((int)(unsigned char)*(signed char *)(*(char **)D_001997C8)) != 1) {
        return;
    }
    *(signed char *)(*(char **)D_001997C8 + 6) ^= 2;
}

void func_0004E86A(void)
{
    mc_memcpy(D_001997D8, *(int *)note_page, 3640, (int)D_00174FAC, 629, 4);
    if (*(int *)D_001997C8 == 0 || ((int)(unsigned char)*(signed char *)(*(char **)D_001997C8)) != 1) {
        return;
    }
    *(signed char *)(*(char **)D_001997C8 + 6) ^= 1;
}

int func_0004E8C8(int a1)
{
    {
        char l_28[12];

        *(signed char *)((char *)a1 + 6) &= 191;
        func_0004EA3C(a1, (int)l_28);
        if (func_0004E9B4((int)&D_001997B0, (int)l_28) != 0) *(signed char *)((char *)a1 + 6) |= 64;
        return 0;
    }
}

int func_0004E917(int a1)
{
    {
        char l_34[12];
        char l_28[12];

        *(signed char *)((char *)a1 + 10) &= 191;
        *(short *)((char *)l_28 + 4) = *(short *)((char *)a1 + 1);
        *(short *)l_28 = *(int *)((char *)l_28 + 4);
        *(short *)((char *)l_28 + 6) = *(short *)((char *)a1 + 3);
        *(short *)((char *)l_28 + 2) = *(int *)((char *)l_28 + 6);
        *(short *)((char *)l_34 + 4) = *(short *)((char *)a1 + 5);
        *(short *)l_34 = *(int *)((char *)l_34 + 4);
        *(short *)((char *)l_34 + 6) = *(short *)((char *)a1 + 7);
        *(short *)((char *)l_34 + 2) = *(int *)((char *)l_34 + 6);
        if (func_0004E9B4((int)&D_001997B0, (int)l_28) != 0 || func_0004E9B4((int)&D_001997B0, (int)l_34) != 0) {
            *(signed char *)((char *)a1 + 10) |= 64;
        }
        return 0;
    }
}

int func_0004E9B4(int a1, int a2)
{
    if (*(short *)((char *)a2 + 4) < *(short *)((char *)a1)) return 0;
    if (*(short *)((char *)a2) > *(short *)((char *)a1 + 4)) return 0;
    if (*(short *)((char *)a2 + 6) < *(short *)((char *)a1 + 2)) return 0;
    if (*(short *)((char *)a2 + 2) > *(short *)((char *)a1 + 6)) return 0;
    return 1;
}

void func_0004EA3C(int a1, int a2)
{
    int l_14;

    func_0012DB50((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)((char *)a1 + 5)]));
    l_14 = font_text_width(a1 + 11);
    *(short *)((char *)a2) = *(short *)((char *)a1 + 1);
    *(short *)((char *)a2 + 2) = *(short *)((char *)a1 + 3);
    *(short *)((char *)a2 + 4) = *(short *)((char *)a1 + 1) + l_14;
    *(short *)((char *)a2 + 6) = *(short *)((char *)a1 + 3) + D_0012DA44;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 6) & 1)) == 0) return;
    *(short *)((char *)a2) = 160 - (((int)(short)*(short *)&l_14) >> 1);
    *(short *)((char *)a2 + 4) = *(short *)((char *)a2) + l_14;
}

void func_0004EB71(void)
{
    D_001997CC = *(int *)note_page;
    func_0004EAF4(*(int *)note_page, (int)func_0004E8C8, (int)func_0004E917);
    func_0004EC7A();
}

int func_0004EBAC(int a1)
{
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 6) & 64)) != 0) return 0;
    mc_memcpy(D_001997CC, a1, 91, (int)D_00174FAC, 731, 4);
    D_001997CC = (int)(*(char **)&D_001997CC + 91);
    return 0;
}

int func_0004EC16(int a1)
{
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 10) & 64)) != 0) return 0;
    mc_memcpy(D_001997CC, a1, 11, (int)D_00174FAC, 739, 4);
    D_001997CC += 11;
    return 0;
}

void func_0004EC7A(void)
{
    func_0004EAF4(*(int *)note_page, (int)func_0004EBAC, (int)func_0004EC16);
    *(signed char *)(*(char **)&D_001997CC) = 0;
}
