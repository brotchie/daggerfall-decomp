/* matched by the real Watcom C32 10.0a (-d2): func_0007E815 has a switch table, which 10.0a
 * aligns to 4 bytes from the start of the code segment, so it is compiled with the run of its
 * unit's functions from func_0007E350 (the nearest one at a multiple of 4) */
struct obj { char pad[7]; int x; int y; int z; char pad2[44]; int f63; };
extern char D_00176A10[];
extern char D_001940E4[];
extern struct obj *D_00194064[];
extern struct obj *D_001940E0[];
extern struct obj *D_001940E8[];
extern struct obj *D_00194164[];
extern struct obj *D_00194168[];
extern char D_00195AA4[];
extern char D_00195AC4[];
extern char D_00195BF4[];
extern void (*D_00195CD0)(int, int);
extern char D_00195CE0[];
extern char D_00195CE4[];
extern unsigned char *func_0007DF35(int);
extern int func_000A0040();
extern int func_000C7FD9();
extern int func_000C7FF4();

int func_0007E350(int a1)
{
    int l_1C;

    if (a1 != 0) goto L7E376;
    l_1C = (int)func_0007DF35(*(int *)D_00195AA4);
    goto L7E37C;
L7E376:;
    l_1C = a1;
L7E37C:;
    if (l_1C != 0) goto L7E38B;
    return 0;
L7E38B:;
    if (((int)(unsigned char)(*(signed char *)((char *)l_1C + 15) & 1)) == 0) goto L7E3AA;
    if (((unsigned)*(int *)((char *)l_1C + 2)) > *(int *)D_00195BF4) goto L7E3AC;
L7E3AA:;
    goto L7E3B9;
L7E3AC:;
    return (int)(unsigned char)*(signed char *)((char *)l_1C + 8);
L7E3B9:;
    return 0;
}

void func_0007E3CD(unsigned char *a1, unsigned char a2, int a3)
{
    unsigned char *l_18;

    l_18 = a1 ? a1 : func_0007DF35(*(int *)D_00195AA4);
    if (a1 == 0) return;
    if (a1[24] == 15) return;
    l_18[8] = a2;
    l_18[15] &= 248;
    l_18[15] |= 1;
    *(int *)(l_18 + 2) = a3;
}

int func_0007E441(int a1)
{
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_3C = *(int *)(*(char **)D_00195AA4 + 67);
L7E45D:;
    if (((int)(unsigned char)*(signed char *)((char *)l_3C)) == 1) goto L7E481;
    if (((int)(unsigned short)(*(short *)((char *)l_3C + 21) & 1)) == 0) goto L7E483;
L7E481:;
    goto L7E48E;
L7E483:;
    l_3C = *(int *)((char *)l_3C + 67);
    goto L7E45D;
L7E48E:;
    if (((int)(unsigned char)*(signed char *)((char *)l_3C)) == 43) goto L7E4A9;
    return 0;
L7E4A9:;
    l_38 = *(int *)(*(char **)D_00195AA4 + 7) - *(int *)((char *)l_3C + 7);
    l_34 = *(int *)(*(char **)D_00195AA4 + 11) - *(int *)((char *)l_3C + 11);
    l_30 = *(int *)(*(char **)D_00195AA4 + 15) - *(int *)((char *)l_3C + 15);
    l_1C = ((a1 == 2) ? 8 : 0);
    l_3C = *(int *)((char *)l_3C + 63);
L7E501:;
    if (((int)(unsigned char)*(signed char *)((char *)l_3C)) == 43) goto L7E51B;
    l_3C = *(int *)((char *)l_3C + 55);
    goto L7E501;
L7E51B:;
    l_44 = l_3C + 71;
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
    int l_20;
    int l_1C;
    int l_18;

    l_20 = *(int *)(*(char **)D_00195AC4 + 63);
    l_1C = 10000;
    l_18 = 10000;
    func_000A0040((int)D_001940E4, 0, 4096, (int)D_00176A10, 849, 4096);
L7E609:;
    if (l_20 == 0) goto L7E675;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 47) goto L7E66A;
    if ((*(int *)((char *)l_20 + 7) - *(int *)(*(char **)D_00195AC4 + 7)) >= l_1C) goto L7E644;
    l_1C = *(int *)((char *)l_20 + 7) - *(int *)(*(char **)D_00195AC4 + 7);
L7E644:;
    if ((*(int *)((char *)l_20 + 15) - *(int *)(*(char **)D_00195AC4 + 15)) >= l_18) goto L7E66A;
    l_18 = *(int *)((char *)l_20 + 15) - *(int *)(*(char **)D_00195AC4 + 15);
L7E66A:;
    l_20 = *(int *)((char *)l_20 + 55);
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
    l_20 = *(int *)(*(char **)D_00195AC4 + 63);
L7E6C0:;
    if (l_20 == 0) return;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 47) goto L7E736;
    *(int *)(D_001940E4 + ((((int)(*(char **)D_00195CE0 + (*(int *)((char *)l_20 + 7) - *(int *)(*(char **)D_00195AC4 + 7))) / 1024) + (((int)(*(char **)D_00195CE4 + (*(int *)((char *)l_20 + 15) - *(int *)(*(char **)D_00195AC4 + 15))) / 1024) << 5)) << 2)) = l_20;
L7E736:;
    l_20 = *(int *)((char *)l_20 + 55);
    goto L7E6C0;
}

void func_0007E74E(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = *(int *)(*(char **)D_00195AC4 + 63);
    l_1C = 10000;
    l_18 = 10000;
    func_000A0040((int)D_001940E4, 0, 4096, (int)D_00176A10, 880, 4096);
    l_20 = *(int *)(*(char **)D_00195AC4 + 63);
L7E7A0:;
    if (l_20 == 0) return;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 38) goto L7E800;
    *(int *)(D_001940E4 + ((((*(int *)((char *)l_20 + 7) - *(int *)(*(char **)D_00195AC4 + 7)) / 4096) + (((*(int *)((char *)l_20 + 15) - *(int *)(*(char **)D_00195AC4 + 15)) / 4096) << 5)) << 2)) = l_20;
L7E800:;
    l_20 = *(int *)((char *)l_20 + 55);
    goto L7E7A0;
}

void func_0007E815(struct obj *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = (a1->x - (*(struct obj **)D_00195AC4)->x + *(int *)D_00195CE0) / 1024;
    l_20 = (a1->z - (*(struct obj **)D_00195AC4)->z + *(int *)D_00195CE4) / 1024;
    l_14 = l_24 + (l_20 << 5);
    l_1C = (l_24 & 1) + (l_20 & 1) * 2;
    l_18 = ((l_20 & -2) << 5) + (l_24 & -2);
    if (((struct obj **)D_001940E4)[l_18] != 0)
        D_00195CD0(((struct obj **)D_001940E4)[l_18]->f63, a2);
    if (D_001940E8[l_18] != 0)
        D_00195CD0(D_001940E8[l_18]->f63, a2);
    if (D_00194164[l_18] != 0)
        D_00195CD0(D_00194164[l_18]->f63, a2);
    if (D_00194168[l_18] != 0)
        D_00195CD0(D_00194168[l_18]->f63, a2);
    switch (l_1C) {
    case 0:
        if (l_14 != 0 && D_001940E0[l_14] != 0)
            D_00195CD0(D_001940E0[l_14]->f63, a2);
        if (l_14 > 31 && D_00194064[l_14] != 0)
            D_00195CD0(D_00194064[l_14]->f63, a2);
        break;
    case 1:
        if ((l_14 & 31) < 31 && D_001940E8[l_14] != 0)
            D_00195CD0(D_001940E8[l_14]->f63, a2);
        if (l_14 > 31 && D_00194064[l_14] != 0)
            D_00195CD0(D_00194064[l_14]->f63, a2);
        break;
    case 2:
        if ((l_14 & 31) != 0 && D_001940E0[l_14] != 0)
            D_00195CD0(D_001940E0[l_14]->f63, a2);
        if (l_14 < 992 && D_00194164[l_14] != 0)
            D_00195CD0(D_00194164[l_14]->f63, a2);
        break;
    case 3:
        if ((l_14 & 31) < 31 && D_001940E8[l_14] != 0)
            D_00195CD0(D_001940E8[l_14]->f63, a2);
        if (l_14 < 992 && D_00194164[l_14] != 0)
            D_00195CD0(D_00194164[l_14]->f63, a2);
        break;
    }
}
