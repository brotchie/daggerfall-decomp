/* fs2df.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170AB4[];
extern char D_00170AEC[];
extern char D_00170AF9[];
extern char D_00170B06[];
extern char D_001940D8[];
extern char spellshop_icons[];
extern char window_image[];
extern char D_0019626F[];
extern char D_00196272[];
extern char game_mode[];
extern char D_001985D4[];
extern char D_001985D8[];
extern struct link *D_001995E4;
extern char D_001995E8[];
extern struct link *D_001995EC;
extern char D_001995F0[];
extern char D_001995F4[];
extern char D_00199600[];
extern char D_00199604[];
extern char D_0019960C[];
extern char D_00199614[];
extern char D_00199618[];
extern char spellmaker_settings_image[];
extern char D_00199D78[];
extern char link_count[];

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
    l_1C = (int)(*(char **)D_001995E8 + *(int *)(*(char **)D_00199604 + 9020));
L361E5:;
    if (((int)(short)*(short *)((char *)l_1C + 4)) == l_18) goto L36203;
    l_1C = (int)(*(char **)D_001995E8 + *(int *)((char *)l_1C));
    goto L361E5;
L36203:;
    *(int *)((char *)a1 + 19) = *(int *)((char *)l_1C + 6);
    *(short *)((char *)a1 + 14) = *(short *)((char *)l_1C + 10);
    *(signed char *)((char *)a1 + 18) = *(signed char *)((char *)l_1C + 12);
}

void func_000367E5(struct record *a1, int a2, int a3)
{
    int l_18;
    int l_14;
    int l_10;

L367FA:;
    l_10 = a2 - *(int *)D_001995E8;
    switch ((unsigned char)(*(signed char *)((char *)a2 + 20) & 63)) {
case 1:
    if (*(int *)((char *)(*(int *)D_0019960C = (int)(*(char **)D_001995E8 + *(int *)((char *)a2 + 21))) + 19) >= 0) goto L36863;
    func_000361B7(*(int *)D_0019960C);
L36863:;
    if (*(short *)(*(char **)D_0019960C + 14) == 0) goto L368A5;
    *(int *)D_00199600 = (int)(*(char **)D_001995E8 + *(int *)(*(char **)D_0019960C + 19));
    *(short *)D_00199618 = func_00036A41(l_10);
    func_00036AA7((int)(unsigned char)(*(signed char *)((char *)a2 + 20) & 63));
L368A5:;
    goto L36977;
case 2:
    *(int *)D_001995F4 = (int)(*(char **)D_001995E8 + *(int *)((char *)a2 + 21));
    goto L36977;
case 3:
    if (*(short *)((char *)(*(int *)D_001995F0 = (int)(*(char **)D_001995E8 + *(int *)((char *)a2 + 21))) + 2) == 0) goto L368F7;
    if (((int)(unsigned short)*(short *)(*(char **)D_001995F0)) != 25482) goto L36943;
L368F7:;
    if (((int)(unsigned short)*(short *)(*(char **)D_001995F0)) != 25490) goto L3691D;
    if (((int)(unsigned char)*(signed char *)(*(char **)D_001995F0 + 4)) == 70) goto L3691F;
L3691D:;
    goto L3692D;
L3691F:;
    if (*(int *)(*(char **)D_001995F0 + 6) == 16747) goto L3692F;
L3692D:;
    goto L36941;
L3692F:;
    if (((int)(unsigned char)*(signed char *)(*(char **)D_001995F0 + 10)) == 5) goto L36943;
L36941:;
    goto L36977;
L36943:;
    if (((int)(unsigned short)*(short *)(*(char **)D_001995F0)) == 25488) goto L36977;
    *(short *)D_00199618 = func_00036A41(l_10);
    func_00036AA7((int)(unsigned char)(*(signed char *)((char *)a2 + 20) & 63));
default:
L36977:;
    a2 = (int)(*(char **)D_001995E8 + *(int *)((char *)a2));
    if ((a2 - *(int *)D_001995E8) > 0) goto L367FA;
}
}

short func_00036A41(int a1)
{
    int l_1C;

    l_1C = 0;
L36A59:;
    if (l_1C < *(int *)D_00199614) goto L36A6E;
    goto L36A93;
L36A66:;
    l_1C++;
    goto L36A59;
L36A6E:;
    if (*(int *)(D_001985D4 + (l_1C << 3)) != a1) goto L36A91;
    return *(short *)(D_001985D8 + (l_1C << 3));
L36A91:;
    goto L36A66;
L36A93:;
    return 0;
}

void func_00036AA7(int a1)
{
    int l_1C;
    int l_18;

    switch ((unsigned)a1) {
case 1:
    func_00036C6F(*(int *)D_0019960C, *(int *)D_00199600, 0, 0);
    l_18 = *(int *)(*(char **)D_00199600 + 5);
    goto L36B39;
case 2:
    func_00036C6F(0, 0, 0, (int)(unsigned char)*(signed char *)(*(char **)D_001995F4 + 3));
    l_18 = *(int *)(*(char **)D_001995F4 + 4);
    goto L36B39;
case 3:
    func_00036C6F(0, 0, *(int *)D_001995F0, (int)(unsigned char)*(signed char *)(*(char **)D_001995F0 + 10));
    l_18 = *(int *)(*(char **)D_001995F0 + 6);
default:
L36B39:;
    if (l_18 <= 0) return;
    *(short *)D_00199618 = func_00036A41(l_18);
    l_1C = (int)(*(char **)D_001995E8 + l_18);
}
    switch ((unsigned char)(*(signed char *)((char *)l_1C + 20) & 63)) {
case 1:
    if (*(int *)((char *)(*(int *)D_0019960C = (int)(*(char **)D_001995E8 + *(int *)((char *)l_1C + 21))) + 19) >= 0) goto L36BBB;
    func_000361B7(*(int *)D_0019960C);
L36BBB:;
    *(int *)D_00199600 = (int)(*(char **)D_001995E8 + *(int *)(*(char **)D_0019960C + 19));
    func_00036DC9(*(int *)D_0019960C, *(int *)D_00199600, 0, 0);
    l_18 = *(int *)(*(char **)D_00199600 + 5);
    goto L36C60;
case 2:
    func_00036DC9(0, 0, 0, (int)(unsigned char)*(signed char *)((char *)(*(int *)D_001995F4 = (int)(*(char **)D_001995E8 + *(int *)((char *)l_1C + 21))) + 3));
    l_18 = *(int *)(*(char **)D_001995F4 + 4);
    goto L36C60;
case 3:
    func_00036DC9(0, 0, *(int *)D_001995F0, (int)(unsigned char)*(signed char *)((char *)(*(int *)D_001995F0 = (int)(*(char **)D_001995E8 + *(int *)((char *)l_1C + 21))) + 10));
    l_18 = *(int *)(*(char **)D_001995F0 + 6);
default:
L36C60:;
    goto L36B39;
}
}

void func_00036C6F(int a1, int a2, int a3, unsigned char a4)
{
    D_001995EC = (D_001995E4 = (struct link *)(((int)D_00199D78) + ((*(int *)link_count)++ * 39)));
    mc_memset((int)D_001995EC, 0, 39, (int)D_00170AB4, 447, 4);
    D_001995EC->object_id = *(short *)D_00199618;
    if (a2 == 0) goto L36D3C;
    D_001995EC->trigger = *(signed char *)((char *)a1 + 14);
    D_001995EC->param = *(signed char *)((char *)a1 + 18);
    D_001995EC->axis = *(signed char *)((char *)a2);
    D_001995EC->duration = *(short *)((char *)a2 + 1);
    D_001995EC->magnitude = *(short *)((char *)a2 + 3);
    D_001995EC->action = *(signed char *)((char *)a2 + 9);
    goto L36D89;
L36D3C:;
    if (a3 == 0) goto L36D7D;
    D_001995EC->trigger = *(signed char *)((char *)a3 + 2);
    D_001995EC->param = *(signed char *)((char *)a3 + 5);
    D_001995EC->axis = *(signed char *)((char *)a3 + 4);
    D_001995EC->action = a4;
    goto L36D89;
L36D7D:;
    D_001995EC->action = a4;
L36D89:;
    if (D_001995EC->action <= 1) goto L36DAD;
    if (D_001995EC->action < 8) goto L36DAF;
L36DAD:;
    goto L36DB9;
L36DAF:;
    func_00036F18(D_001995EC);
L36DB9:;
    D_001995EC->chain_count = 0;
}

void func_00036DC9(int a1, int a2, int a3, unsigned char a4)
{
    D_001995E4->chain_count++;
    D_001995EC = (struct link *)(((int)D_00199D78) + ((*(int *)link_count)++ * 39));
    mc_memset((int)D_001995EC, 0, 39, (int)D_00170AB4, 490, 4);
    D_001995EC->object_id = *(short *)D_00199618;
    if (a2 == 0) goto L36E94;
    D_001995EC->trigger = *(signed char *)((char *)a1 + 14);
    D_001995EC->param = *(signed char *)((char *)a1 + 18);
    D_001995EC->axis = *(signed char *)((char *)a2);
    D_001995EC->duration = *(short *)((char *)a2 + 1);
    D_001995EC->magnitude = *(short *)((char *)a2 + 3);
    D_001995EC->action = *(signed char *)((char *)a2 + 9);
    goto L36EE1;
L36E94:;
    if (a3 == 0) goto L36ED5;
    D_001995EC->trigger = *(signed char *)((char *)a3 + 2);
    D_001995EC->param = *(signed char *)((char *)a3 + 5);
    D_001995EC->axis = *(signed char *)((char *)a3 + 4);
    D_001995EC->action = a4;
    goto L36EE1;
L36ED5:;
    D_001995EC->action = a4;
L36EE1:;
    if (D_001995EC->action <= 1) goto L36F05;
    if (D_001995EC->action < 8) goto L36F07;
L36F05:;
    return;
L36F07:;
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
    if (((int)(unsigned char)*(signed char *)D_0019626F) != 2) goto L36F92;
    return 1;
L36F92:;
    if (a1 == 0) goto L36FE5;
    *(signed char *)game_mode = 2;
    *(int *)window_image = disk_read_file((int)D_00170AEC, 0);
    *(int *)spellshop_icons = disk_read_file((int)D_00170AF9, 0);
    *(int *)spellmaker_settings_image = disk_read_file((int)D_00170B06, 0);
    *(signed char *)D_00196272 = 1;
    *(signed char *)D_001940D8 |= 1;
    spellmaker_new();
L36FE5:;
    return ((((int)(unsigned char)*(signed char *)game_mode) == 2) ? 1 : 0);
}
