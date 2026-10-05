/* fs2df.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170AB4[];
extern char D_00170AEC[];
extern char D_00170AF9[];
extern char D_00170B06[];
extern signed char D_001940D8;
extern int spellshop_icons;
extern int window_image;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern char D_001985D4[];
extern char D_001985D8[];
extern struct link *D_001995E4;
extern char *D_001995E8;
extern struct link *D_001995EC;
extern char *D_001995F0;
extern char *D_001995F4;
extern int D_00199600;
extern char *D_00199604;
extern int D_0019960C;
extern int D_00199614;
extern short D_00199618;
extern int spellmaker_settings_image;
extern char D_00199D78[];
extern int link_count;

extern int spellmaker_new(void);
extern int disk_read_file(int, int);
extern int mc_memset();
short func_00036A41(int);
void func_000361B7(int);
void func_00036AA7(int);
void func_00036C6F(int, int, int, unsigned char);
void func_00036DC9(int, int, int, unsigned char);
void func_00036F18(struct link *);

void func_000361B7(int a1)
{
    int l_1C;
    int l_18;

    l_18 = 0;
    l_1C = (int)(D_001995E8 + *(int *)(D_00199604 + 9020));
    while (((int)(short)*(short *)((char *)l_1C + 4)) != l_18) {
        l_1C = (int)(D_001995E8 + *(int *)((char *)l_1C));
    }
    *(int *)((char *)a1 + 19) = *(int *)((char *)l_1C + 6);
    *(short *)((char *)a1 + 14) = *(short *)((char *)l_1C + 10);
    *(signed char *)((char *)a1 + 18) = *(signed char *)((char *)l_1C + 12);
}

void func_000367E5(struct record *a1, int a2, int a3)
{
    int l_18;
    int l_14;
    int l_10;

    do {
        l_10 = a2 - (int)D_001995E8;
        switch ((unsigned char)(*(signed char *)((char *)a2 + 20) & 63)) {
        case 1:
            if (*(int *)((char *)(D_0019960C = (int)(D_001995E8 + *(int *)((char *)a2 + 21))) + 19) < 0) {
                func_000361B7(D_0019960C);
            }
            if (*(short *)(((char *)D_0019960C) + 14) != 0) {
                D_00199600 = (int)(D_001995E8 + *(int *)(((char *)D_0019960C) + 19));
                D_00199618 = func_00036A41(l_10);
                func_00036AA7((int)(unsigned char)(*(signed char *)((char *)a2 + 20) & 63));
            }
            break;
        case 2:
            *(int *)&D_001995F4 = (int)(D_001995E8 + *(int *)((char *)a2 + 21));
            break;
        case 3:
            if ((*(short *)((char *)(*(int *)&D_001995F0 = (int)(D_001995E8 + *(int *)((char *)a2 + 21))) + 2) != 0 && ((int)(unsigned short)*(short *)(D_001995F0)) != 25482) || (((int)(unsigned short)*(short *)(D_001995F0)) == 25490 && ((int)(unsigned char)*(signed char *)(D_001995F0 + 4)) == 70 && *(int *)(D_001995F0 + 6) == 16747 && ((int)(unsigned char)*(signed char *)(D_001995F0 + 10)) == 5)) {
                if (((int)(unsigned short)*(short *)(D_001995F0)) != 25488) {
                    D_00199618 = func_00036A41(l_10);
                    func_00036AA7((int)(unsigned char)(*(signed char *)((char *)a2 + 20) & 63));
                }
            }
        }
        a2 = (int)(D_001995E8 + *(int *)((char *)a2));
    } while ((a2 - (int)D_001995E8) > 0);
}

short func_00036A41(int a1)
{
    int l_1C;

    for (l_1C = 0; l_1C < D_00199614; l_1C++) {
        if (*(int *)(D_001985D4 + (l_1C << 3)) == a1) return *(short *)(D_001985D8 + (l_1C << 3));
    }
    return 0;
}

void func_00036AA7(int a1)
{
    int l_1C;
    int l_18;

    switch ((unsigned)a1) {
    case 1:
        func_00036C6F(D_0019960C, D_00199600, 0, 0);
        l_18 = *(int *)(((char *)D_00199600) + 5);
        break;
    case 2:
        func_00036C6F(0, 0, 0, (int)(unsigned char)*(signed char *)(D_001995F4 + 3));
        l_18 = *(int *)(D_001995F4 + 4);
        break;
    case 3:
        func_00036C6F(0, 0, (int)D_001995F0, (int)(unsigned char)*(signed char *)(D_001995F0 + 10));
        l_18 = *(int *)(D_001995F0 + 6);
    }
    while (l_18 > 0) {
        D_00199618 = func_00036A41(l_18);
        l_1C = (int)(D_001995E8 + l_18);
        switch ((unsigned char)(*(signed char *)((char *)l_1C + 20) & 63)) {
        case 1:
            if (*(int *)((char *)(D_0019960C = (int)(D_001995E8 + *(int *)((char *)l_1C + 21))) + 19) < 0) {
                func_000361B7(D_0019960C);
            }
            D_00199600 = (int)(D_001995E8 + *(int *)(((char *)D_0019960C) + 19));
            func_00036DC9(D_0019960C, D_00199600, 0, 0);
            l_18 = *(int *)(((char *)D_00199600) + 5);
            break;
        case 2:
            func_00036DC9(0, 0, 0, (int)(unsigned char)*(signed char *)((char *)(*(int *)&D_001995F4 = (int)(D_001995E8 + *(int *)((char *)l_1C + 21))) + 3));
            l_18 = *(int *)(D_001995F4 + 4);
            break;
        case 3:
            func_00036DC9(0, 0, (int)D_001995F0, (int)(unsigned char)*(signed char *)((char *)(*(int *)&D_001995F0 = (int)(D_001995E8 + *(int *)((char *)l_1C + 21))) + 10));
            l_18 = *(int *)(D_001995F0 + 6);
        }
    }
}

void func_00036C6F(int a1, int a2, int a3, unsigned char a4)
{
    D_001995EC = (D_001995E4 = (struct link *)(((int)D_00199D78) + ((link_count)++ * 39)));
    mc_memset((int)D_001995EC, 0, 39, (int)D_00170AB4, 447, 4);
    D_001995EC->object_id = D_00199618;
    if (a2 != 0) {
        D_001995EC->trigger = *(signed char *)((char *)a1 + 14);
        D_001995EC->param = *(signed char *)((char *)a1 + 18);
        D_001995EC->axis = *(signed char *)((char *)a2);
        D_001995EC->duration = *(short *)((char *)a2 + 1);
        D_001995EC->magnitude = *(short *)((char *)a2 + 3);
        D_001995EC->action = *(signed char *)((char *)a2 + 9);
    } else if (a3 != 0) {
        D_001995EC->trigger = *(signed char *)((char *)a3 + 2);
        D_001995EC->param = *(signed char *)((char *)a3 + 5);
        D_001995EC->axis = *(signed char *)((char *)a3 + 4);
        D_001995EC->action = a4;
    } else {
        D_001995EC->action = a4;
    }
    if (D_001995EC->action > 1 && D_001995EC->action < 8) func_00036F18(D_001995EC);
    D_001995EC->chain_count = 0;
}

void func_00036DC9(int a1, int a2, int a3, unsigned char a4)
{
    D_001995E4->chain_count++;
    D_001995EC = (struct link *)(((int)D_00199D78) + ((link_count)++ * 39));
    mc_memset((int)D_001995EC, 0, 39, (int)D_00170AB4, 490, 4);
    D_001995EC->object_id = D_00199618;
    if (a2 != 0) {
        D_001995EC->trigger = *(signed char *)((char *)a1 + 14);
        D_001995EC->param = *(signed char *)((char *)a1 + 18);
        D_001995EC->axis = *(signed char *)((char *)a2);
        D_001995EC->duration = *(short *)((char *)a2 + 1);
        D_001995EC->magnitude = *(short *)((char *)a2 + 3);
        D_001995EC->action = *(signed char *)((char *)a2 + 9);
    } else if (a3 != 0) {
        D_001995EC->trigger = *(signed char *)((char *)a3 + 2);
        D_001995EC->param = *(signed char *)((char *)a3 + 5);
        D_001995EC->axis = *(signed char *)((char *)a3 + 4);
        D_001995EC->action = a4;
    } else {
        D_001995EC->action = a4;
    }
    if (D_001995EC->action <= 1 || D_001995EC->action >= 8) return;
    func_00036F18(D_001995EC);
}

void func_00036F18(struct link *a1)
{
    a1->magnitude = a1->axis << 3;
    a1->axis = ((a1->action - 2) ^ 1) + 1;
    a1->duration = 50;
    a1->action = 1;
}

int spellmaker_open(int a1)
{
    if (((int)D_0019626F) == 2) return 1;
    if (a1 != 0) {
        game_mode = 2;
        window_image = disk_read_file((int)D_00170AEC, 0);
        spellshop_icons = disk_read_file((int)D_00170AF9, 0);
        spellmaker_settings_image = disk_read_file((int)D_00170B06, 0);
        D_00196272 = 1;
        D_001940D8 |= 1;
        spellmaker_new();
    }
    return ((((int)(unsigned char)game_mode) == 2) ? 1 : 0);
}
