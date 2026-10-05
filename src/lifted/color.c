/* color.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char D_00177350[];
extern char player_environment[];
extern char D_00187B6E[];
extern char D_00187B72[];
extern char D_00187B76[];
extern char D_001886A8[];
extern char D_001886A9[];
extern char D_001886D2[];
extern char D_00190BE4[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern char game_minutes[];
extern char D_001AA5FC[];
extern char D_001AA600[];
extern char doors_moving[];

extern int sound_play(int, int, int);
extern int mc_malloc();
extern int mc_memcpy();
extern int func_000CE663();
extern int func_000CE66C();
extern int func_0014AA92();
extern void object_foreach_post(struct record *, int);
int func_00099B86(int);
void func_00099CB9(struct record *);

void func_00099689(void)
{
    int l_1C;
    int l_18;

    *(int *)D_001AA5FC = mc_malloc(8448, (int)D_00177350, 59);
    *(int *)D_001AA600 = func_000CE66C(*(int *)D_001AA5FC, 256);
    l_1C = 0;
L996CB:;
    if (l_1C < 32) goto L996DB;
    goto L99711;
L996D3:;
    l_1C++;
    goto L996CB;
L996DB:;
    l_18 = 0;
L996E2:;
    if (l_18 < 256) goto L996F5;
    goto L9970F;
L996ED:;
    l_18++;
    goto L996E2;
L996F5:;
    *(signed char *)((char *)(int)(*(char **)D_001AA600 + (l_18 + (l_1C << 8)))) = *(signed char *)&l_18;
    goto L996ED;
L9970F:;
    goto L996D3;
L99711:;
    l_1C = 1;
L99718:;
    if (l_1C < 16) goto L99728;
    goto L9975D;
L99720:;
    l_1C++;
    goto L99718;
L99728:;
    func_000CE663((int)(*(char **)D_001AA600 + (l_1C << 8)) + ((int)(unsigned char)*(signed char *)(D_001886A8 + (l_1C * 2))), (int)(unsigned char)*(signed char *)(D_001886A9 + (l_1C * 2)), 16);
    goto L99720;
L9975D:;
    func_000CE663(*(int *)D_001AA600 + 6689, 161, 15);
    func_000CE663(*(int *)D_001AA600 + 6721, 193, 15);
    func_000CE663(*(int *)D_001AA600 + 6945, 97, 15);
    func_000CE663(*(int *)D_001AA600 + 6977, 129, 15);
    func_000CE663(*(int *)D_001AA600 + 7201, 161, 15);
    func_000CE663(*(int *)D_001AA600 + 7220, 84, 2);
    func_000CE663(*(int *)D_001AA600 + 7233, 193, 15);
    *(signed char *)(*(char **)D_001AA600 + 7421) = 216;
    func_000CE663(*(int *)D_001AA600 + 7457, 97, 15);
    func_000CE663(*(int *)D_001AA600 + 7476, 84, 2);
    func_000CE663(*(int *)D_001AA600 + 7489, 129, 15);
    *(signed char *)(*(char **)D_001AA600 + 7677) = 216;
    l_1C = 0;
L99876:;
    if (l_1C < 10) goto L99886;
    return;
L9987E:;
    l_1C++;
    goto L99876;
L99886:;
    mc_memcpy((int)(*(char **)D_001AA600 + ((l_1C << 8) + 4096)) + 112, ((int)D_001886D2) + (l_1C << 4), 16, (int)D_00177350, 87, 4);
    goto L9987E;
}

int func_000998C8(int a1)
{
    short l_18;
    short l_1C;

    *(int *)&l_1C = 0;
    *(int *)&l_18 = 0;
L998E7:;
    if (*(signed char *)((char *)(((int)(short)l_18) + a1)) == 0) goto L9990F;
    *(int *)&l_1C <<= 1;
    *(int *)&l_1C += (int)(unsigned char)*(signed char *)((char *)(((int)(short)l_18) + a1));
    (*(int *)&l_18)++;
    goto L998E7;
L9990F:;
    return *(int *)&l_1C;
}

void doors_update(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_28 = 0;
L99A4C:;
    if (l_28 < 16) goto L99A5F;
    return;
L99A57:;
    l_28++;
    goto L99A4C;
L99A5F:;
    if (*(int *)(doors_moving + (l_28 << 2)) == 0) goto L99A57;
    l_20 = *(int *)(doors_moving + (l_28 << 2));
    if (((struct bf8_7_1 *)((char *)l_20 + 46))->f != 0) goto L99A92;
    if (func_00099B86(l_20) != 0) goto L99A94;
L99A92:;
    goto L99AAB;
L99A94:;
    l_1C = 1132;
    *(int *)((char *)l_20 + 43) = *(int *)((char *)l_1C) | (-1073741824);
L99AAB:;
    l_18 = 1132;
    l_24 = ((*(int *)((char *)l_18) - (*(int *)((char *)l_20 + 43) & 1073741823)) * 22) & 2047;
    if (l_24 >= 512) goto L99AE3;
    if (l_24 >= 0) goto L99B59;
L99AE3:;
    if (l_24 < 512) goto L99AF5;
    l_24 = 512;
    goto L99B02;
L99AF5:;
    if (l_24 >= 0) goto L99B02;
    l_24 = 0;
L99B02:;
    *(int *)(doors_moving + (l_28 << 2)) = 0;
    *(signed char *)((char *)l_20 + 46) &= 191;
    if (((struct bf8_7_1 *)((char *)l_20 + 46))->f != 0) goto L99B59;
    sound_play(((((int)(unsigned char)*(signed char *)player_environment) == 2) ? 361 : 26), l_20, 100);
L99B59:;
    if (((struct bf8_7_1 *)((char *)l_20 + 46))->f != 0) goto L99B6D;
    l_24 = 512 - l_24;
L99B6D:;
    *(short *)((char *)l_20 + 36) = l_24;
    goto L99A57;
}

int func_00099B86(int a1)
{
    int l_20;
    int l_1C;

    *(int *)D_00187B6E = player_object->x;
    *(int *)D_00187B72 = player_object->y;
    *(int *)D_00187B76 = player_object->z;
    l_20 = a1 + 71;
    if (*(int *)((char *)l_20) == 0) goto L99C07;
    l_1C = func_0014AA92(l_20, (int)D_00187B6E, 0);
    return (((l_1C != 0) && (l_1C != (-1))) ? 1 : 0);
L99C07:;
    return 0;
}

struct building *func_00099C1B(struct record *a1)
{
    short l_18;

    a1 = a1->parent;
L99C35:;
    if (a1 == 0) goto L99C46;
    if (a1 != D_00195AC4) goto L99C48;
L99C46:;
    goto L99CA5;
L99C48:;
    *(int *)&l_18 = 0;
L99C4F:;
    if ((short)l_18 < current_location->building_count) goto L99C6F;
    goto L99CA3;
L99C67:;
    (*(int *)&l_18)++;
    goto L99C4F;
L99C6F:;
    if (current_location->buildings[(int)(short)l_18].id != a1->id) goto L99CA1;
    return &current_location->buildings[(int)(short)l_18];
L99CA1:;
    goto L99C67;
L99CA3:;
    goto L99C35;
L99CA5:;
    return 0;
}

void func_00099CB9(struct record *a1)
{
    int l_18;

    if (a1->type != 34) return;
    l_18 = ((int)(unsigned short)(a1->image & 31)) - 2;
    if (l_18 == 13) goto L99CFC;
    if (l_18 != 14) return;
L99CFC:;
    a1->flags |= 0x200;
}

void func_00099D0D(struct record *a1)
{
    int l_18;

    l_18 = ((unsigned)*(int *)game_minutes) % 1440;
    *(int *)D_00190BE4 = (((l_18 > 360) && (l_18 < 1080)) ? 1 : 0);
    object_foreach_post(a1->children, (int)func_00099CB9);
}
