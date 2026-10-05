/* matched by the real Watcom C32 10.0a (-d2): func_0007E815 has a switch table, which 10.0a
 * aligns to 4 bytes from the start of the code segment, so it is compiled with the run of its
 * unit's functions from building_access_level (the nearest one at a multiple of 4) */
#include "records.h"

extern char D_00176A10[];
extern struct record *D_001940E4[];         /* 32 x 32 grid of object lists */
extern struct record *D_00194064[];
extern struct record *cart_overlay_image[];
extern struct record *D_001940E8[];
extern struct record *D_00194164[];
extern struct record *D_00194168[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char game_minutes[];
extern void (*D_00195CD0)(struct record *, int);
extern char D_00195CE0[];
extern char D_00195CE4[];
extern struct building *object_building(struct record *);
extern int mc_memset();
extern int func_000C7FD9();
extern int func_000C7FF4();

int building_access_level(struct building *a1)
{
    struct building *l_1C;

    if (a1 != 0) goto L7E376;
    l_1C = object_building(player_object);
    goto L7E37C;
L7E376:;
    l_1C = a1;
L7E37C:;
    if (l_1C != 0) goto L7E38B;
    return 0;
L7E38B:;
    if (((int)(unsigned char)(l_1C->flags & 1)) == 0) goto L7E3AA;
    if (l_1C->rent_expires > *(int *)game_minutes) goto L7E3AC;
L7E3AA:;
    goto L7E3B9;
L7E3AC:;
    return l_1C->access_level;
L7E3B9:;
    return 0;
}

void building_grant_access(struct building *a1, unsigned char a2, int a3)
{
    struct building *l_18;

    l_18 = a1 ? a1 : object_building(player_object);
    if (a1 == 0) return;
    if (a1->type == 15) return;
    l_18->access_level = a2;
    l_18->flags &= 248;
    l_18->flags |= 1;
    l_18->rent_expires = a3;
}

int func_0007E441(int a1)
{
    int l_44;
    int l_40;
    struct record *l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_3C = player_object->parent;
L7E45D:;
    if (l_3C->type == 1) goto L7E481;
    if (((int)(unsigned short)(l_3C->flags & 1)) == 0) goto L7E483;
L7E481:;
    goto L7E48E;
L7E483:;
    l_3C = l_3C->parent;
    goto L7E45D;
L7E48E:;
    if (l_3C->type == 43) goto L7E4A9;
    return 0;
L7E4A9:;
    l_38 = player_object->x - l_3C->x;
    l_34 = player_object->y - l_3C->y;
    l_30 = player_object->z - l_3C->z;
    l_1C = ((a1 == 2) ? 8 : 0);
    l_3C = l_3C->children;
L7E501:;
    if (l_3C->type == 43) goto L7E51B;
    l_3C = l_3C->next;
    goto L7E501;
L7E51B:;
    l_44 = (int)RECORD_DATA(l_3C);
    l_40 = *(int *)((char *)l_44 + 13);
    l_28 = 100000;
    l_24 = 0;
    l_2C = 0;
L7E542:;
    if (((int)(unsigned char)*(signed char *)((char *)l_44 + 2)) > l_2C) goto L7E563;
    goto L7E5AF;
L7E554:;
    l_2C++;
    (*(char (**)[16])&l_40)++;
    goto L7E542;
L7E563:;
    l_20 = func_000C7FF4(*(int *)((char *)l_40 + 4) - l_34, func_000C7FD9(l_38, l_30, *(int *)((char *)l_40), *(int *)((char *)l_40 + 8)));
    if (l_20 >= l_28) goto L7E5AD;
    l_28 = l_20;
    l_24 = (int)(unsigned char)(*(int *)((char *)l_40 + 12) >> l_1C);
L7E5AD:;
    goto L7E554;
L7E5AF:;
    return l_24;
}

void func_0007E5C2(void)
{
    int l_28;
    int l_24;
    struct record *l_20;
    int l_1C;
    int l_18;

    l_20 = D_00195AC4->children;
    l_1C = 10000;
    l_18 = 10000;
    mc_memset((int)D_001940E4, 0, 4096, (int)D_00176A10, 849, 4096);
L7E609:;
    if (l_20 == 0) goto L7E675;
    if (l_20->type != 47) goto L7E66A;
    if ((l_20->x - D_00195AC4->x) >= l_1C) goto L7E644;
    l_1C = l_20->x - D_00195AC4->x;
L7E644:;
    if ((l_20->z - D_00195AC4->z) >= l_18) goto L7E66A;
    l_18 = l_20->z - D_00195AC4->z;
L7E66A:;
    l_20 = l_20->next;
    goto L7E609;
L7E675:;
    if (l_1C >= 0) goto L7E686;
    l_28 = -(l_1C);
    goto L7E68D;
L7E686:;
    l_28 = 0;
L7E68D:;
    *(int *)D_00195CE0 = l_28;
    if (l_18 >= 0) goto L7E6A6;
    l_24 = -(l_18);
    goto L7E6AD;
L7E6A6:;
    l_24 = 0;
L7E6AD:;
    *(int *)D_00195CE4 = l_24;
    l_20 = D_00195AC4->children;
L7E6C0:;
    if (l_20 == 0) return;
    if (l_20->type != 47) goto L7E736;
    D_001940E4[(((int)(*(char **)D_00195CE0 + (l_20->x - D_00195AC4->x)) / 1024) + (((int)(*(char **)D_00195CE4 + (l_20->z - D_00195AC4->z)) / 1024) << 5))] = l_20;
L7E736:;
    l_20 = l_20->next;
    goto L7E6C0;
}

void func_0007E74E(void)
{
    struct record *l_20;
    int l_1C;
    int l_18;

    l_20 = D_00195AC4->children;
    l_1C = 10000;
    l_18 = 10000;
    mc_memset((int)D_001940E4, 0, 4096, (int)D_00176A10, 880, 4096);
    l_20 = D_00195AC4->children;
L7E7A0:;
    if (l_20 == 0) return;
    if (l_20->type != 38) goto L7E800;
    D_001940E4[(((l_20->x - D_00195AC4->x) / 4096) + (((l_20->z - D_00195AC4->z) / 4096) << 5))] = l_20;
L7E800:;
    l_20 = l_20->next;
    goto L7E7A0;
}

void func_0007E815(struct record *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = (a1->x - D_00195AC4->x + *(int *)D_00195CE0) / 1024;
    l_20 = (a1->z - D_00195AC4->z + *(int *)D_00195CE4) / 1024;
    l_14 = l_24 + (l_20 << 5);
    l_1C = (l_24 & 1) + (l_20 & 1) * 2;
    l_18 = ((l_20 & -2) << 5) + (l_24 & -2);
    if (D_001940E4[l_18] != 0)
        D_00195CD0(D_001940E4[l_18]->children, a2);
    if (D_001940E8[l_18] != 0)
        D_00195CD0(D_001940E8[l_18]->children, a2);
    if (D_00194164[l_18] != 0)
        D_00195CD0(D_00194164[l_18]->children, a2);
    if (D_00194168[l_18] != 0)
        D_00195CD0(D_00194168[l_18]->children, a2);
    switch (l_1C) {
    case 0:
        if (l_14 != 0 && cart_overlay_image[l_14] != 0)
            D_00195CD0(cart_overlay_image[l_14]->children, a2);
        if (l_14 > 31 && D_00194064[l_14] != 0)
            D_00195CD0(D_00194064[l_14]->children, a2);
        break;
    case 1:
        if ((l_14 & 31) < 31 && D_001940E8[l_14] != 0)
            D_00195CD0(D_001940E8[l_14]->children, a2);
        if (l_14 > 31 && D_00194064[l_14] != 0)
            D_00195CD0(D_00194064[l_14]->children, a2);
        break;
    case 2:
        if ((l_14 & 31) != 0 && cart_overlay_image[l_14] != 0)
            D_00195CD0(cart_overlay_image[l_14]->children, a2);
        if (l_14 < 992 && D_00194164[l_14] != 0)
            D_00195CD0(D_00194164[l_14]->children, a2);
        break;
    case 3:
        if ((l_14 & 31) < 31 && D_001940E8[l_14] != 0)
            D_00195CD0(D_001940E8[l_14]->children, a2);
        if (l_14 < 992 && D_00194164[l_14] != 0)
            D_00195CD0(D_00194164[l_14]->children, a2);
        break;
    }
}
