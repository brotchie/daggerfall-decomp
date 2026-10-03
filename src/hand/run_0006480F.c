/* matched by the real Watcom C32 10.0a (-d2): a run of links.c from 0x00064589 to 0x0006480F, kept together for its switch table's alignment */
#pragma pack(1)
struct link {
    unsigned short id;
    unsigned char type;         /* 2 */
    char pad3[7];
    unsigned char count;        /* 10 */
    char pad11;
    unsigned char flags;        /* 12 */
    char pad13[26];
};
#pragma pack()
extern struct link D_00199D78[];
extern struct link *D_001A3978[];
extern int D_001A3A78;
extern int D_001A3A7C;
extern void func_000654EA(struct link *);
extern int func_000CE44C(struct link **, struct link *, int);
extern char D_00175962[];
extern int func_0006480F(int);
extern int func_000A1023();
extern char D_00143550[];
extern char D_00147954[];
extern char D_0017596A[];
extern char D_001940DA[];
extern char D_001952EC[];
extern char D_00195798[];
extern char D_001957CD[];
extern char D_001957E9[];
extern char D_001959AC[];
extern char D_00195AA0[];
extern char D_00195AA4[];
extern char D_00195AB0[];
extern char D_00195BE0[];
extern char D_0019621B[];
extern char D_00196222[];
extern char D_00196226[];
extern char D_0019622A[];
extern char D_00196276[];
extern char D_0019628C[];
extern char D_001A3A80[];
extern char D_001A3A81[];
extern int func_0002E914(int, int, int);
extern void func_0003F09F(int, int);
extern int func_0005AAE4(int, int, int);
extern void func_0006530C(int);
extern int func_00065398(int, int);
extern void func_0006546F(int, int);
extern void func_00065748(short, unsigned char);
extern int func_00065864(int);
extern void func_00065937(int, int, int, int);
extern void func_00065A8C(int, int, int);
extern int func_00069938(int, int, int);
extern int func_0007CBA1(int);
extern void func_0007CC66(void);
extern int func_0007D6AE(int, int);
extern void func_0008C566(int, short);
extern int func_0008C5C9(void);
extern void func_0008DEB4(int, int, int, int, int, int, int);
extern int func_00099922(int, int);
extern int func_000CDD81();
extern int func_00142790();

void func_00064589(int a1, int a2)
{
    int i;
    int j;
    int n;
    struct link *p;
    short id;

    i = 0;
    if (D_001A3A78 == 0) return;
    id = *(short *)((char *)a1 + 31);
    while (i < D_001A3A78) {
        if (D_00199D78[i].type != 0 && D_00199D78[i].id == id) {
            if (D_00199D78[i].type < 8 || D_00199D78[i].type > 9) {
                if (D_00199D78[i].type != a2) return;
            } else if (D_00199D78[i].type == 8) {
                if (a2 != 2 && a2 != 3 && a2 != 5 && a2 != 6)
                    return;
            } else {
                if (a2 != 2 && a2 != 3)
                    return;
            }
            p = &D_00199D78[i];
            if (func_000CE44C(D_001A3978, p, D_001A3A7C) != 0) return;
            D_001A3978[D_001A3A7C++] = p;
            n = p->count + 1;
            for (j = 0; j < n; j++, p++)
                func_000654EA(p);
        }
        i++;
    }
}

void func_00064708(void)
{
    int i;
    int n;
    int j;
    int sum;
    struct link *p;

    for (i = 0; i < D_001A3A7C; i++) {
        p = D_001A3978[i];
        n = p->count + 1;
        sum = j = 0;
        for (; j < n; j++, p++) {
            sum += func_0006480F((int)p);
            if ((p->flags & 16) != 0) {
                sum = 0;
                p->flags &= 239;
                break;
            }
        }
        if (sum == 0) {
            if (D_001A3A7C - 1 > i)
                func_000A1023(&D_001A3978[i], &D_001A3978[i + 1], (D_001A3A7C - i) * 4 - 4, D_00175962, 187, 4);
            D_001A3A7C--;
            i--;
        }
    }
}

int func_0006480F(int a1)
{
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;

    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 12) & 1)) == 0) goto L6483D;
    return 0;
L6483D:;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 9)) == 18) goto L6485D;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 9)) != 20) goto L6486E;
L6485D:;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 4) & 8)) != 0) goto L64870;
L6486E:;
    goto L64881;
L64870:;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 12) & 2)) != 0) goto L64883;
L64881:;
    goto L6488F;
L64883:;
    return 0;
L6488F:;
    *(short *)((char *)a1 + 29) = (*(short *)((char *)a1 + 31) = (*(short *)((char *)a1 + 33) = 0));
    if (((unsigned)(*(int *)((char *)1132) - *(int *)((char *)a1 + 25))) <= ((int)(short)*(short *)((char *)a1 + 5))) goto L6490F;
    *(signed char *)((char *)a1 + 12) |= 1;
    *(int *)((char *)a1 + 25) = *(int *)((char *)1132) - ((int)(short)*(short *)((char *)a1 + 5));
    *(signed char *)((char *)a1 + 12) ^= 2;
    func_00065748((int)(short)*(short *)((char *)a1), (int)(unsigned char)*(signed char *)((char *)a1 + 12));
L6490F:;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 12) & 4)) != 0) goto L64956;
    if (*(signed char *)((char *)a1 + 3) == 0) goto L64932;
    if (*(int *)((char *)a1 + 35) != 0) goto L64934;
L64932:;
    goto L6494F;
L64934:;
    func_00069938((int)(unsigned char)*(signed char *)((char *)a1 + 3), *(int *)((char *)a1 + 35), 110);
L6494F:;
    *(signed char *)((char *)a1 + 12) |= 4;
L64956:;
    if (*(int *)((char *)a1 + 35) == 0) goto L64971;
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 35))) != 32) goto L64976;
L64971:;
    goto L64A1D;
L64976:;
    *(int *)(*(char **)((char *)a1 + 35) + 43) = *(int *)D_001959AC;
L64A1D:;
    switch (*(unsigned char *)((char *)a1 + 9)) {
case 129:
    *(signed char *)((char *)a1 + 11) &= *(signed char *)D_001A3A81 | 240;
    *(signed char *)((char *)a1 + 11) |= *(signed char *)D_001A3A80;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 11) & 15)) != (((int)(unsigned char)*(signed char *)((char *)a1 + 11)) >> 4)) goto L652AB;
case 1:
    l_40 = ((*(int *)((char *)1132) - *(int *)((char *)a1 + 25)) * *(int *)((char *)a1 + 17)) >> 16;
    switch ((unsigned char)(*(signed char *)((char *)a1 + 4) - 1)) {
case 0:
    l_38 = l_40 + *(int *)((char *)a1 + 13);
    *(short *)((char *)a1 + 29) = l_38 - *(short *)(*(char **)((char *)a1 + 35) + 7);
    *(int *)(*(char **)((char *)a1 + 35) + 7) = l_38;
    goto L64C01;
case 1:
    l_38 = *(int *)((char *)a1 + 13) - l_40;
    *(short *)((char *)a1 + 29) = l_38 - *(short *)(*(char **)((char *)a1 + 35) + 7);
    *(int *)(*(char **)((char *)a1 + 35) + 7) = l_38;
    goto L64C01;
case 2:
    l_38 = l_40 + *(int *)((char *)a1 + 13);
    *(short *)((char *)a1 + 31) = l_38 - *(short *)(*(char **)((char *)a1 + 35) + 11);
    *(int *)(*(char **)((char *)a1 + 35) + 11) = l_38;
    goto L64C01;
case 3:
    l_38 = *(int *)((char *)a1 + 13) - l_40;
    *(short *)((char *)a1 + 31) = l_38 - *(short *)(*(char **)((char *)a1 + 35) + 11);
    *(int *)(*(char **)((char *)a1 + 35) + 11) = l_38;
    goto L64C01;
case 4:
    l_38 = l_40 + *(int *)((char *)a1 + 13);
    *(short *)((char *)a1 + 33) = l_38 - *(short *)(*(char **)((char *)a1 + 35) + 15);
    *(int *)(*(char **)((char *)a1 + 35) + 15) = l_38;
    goto L64C01;
case 5:
    l_38 = *(int *)((char *)a1 + 13) - l_40;
    *(short *)((char *)a1 + 33) = l_38 - *(short *)(*(char **)((char *)a1 + 35) + 15);
    *(int *)(*(char **)((char *)a1 + 35) + 15) = l_38;
default:
L64C01:;
    if (*(int *)((char *)a1 + 35) == 0) goto L64C5C;
    if (*(int *)(*(char **)((char *)a1 + 35) + 51) == 0) goto L64C5C;
    l_2C = *(int *)(*(char **)(*(char **)((char *)a1 + 35) + 51) + 63);
    if (l_2C == 0) goto L64C5C;
    if (*(int *)((char *)l_2C + 51) == 0) goto L64C5C;
    func_000A1023(*(int *)((char *)l_2C + 51) + 7, (int)&*(signed char *)(*(char **)((char *)a1 + 35) + 7), 12, (int)D_00175962, 264, 4);
L64C5C:;
    goto L652AB;
}
case 130:
    *(signed char *)((char *)a1 + 11) &= *(signed char *)D_001A3A81 | 240;
    *(signed char *)((char *)a1 + 11) |= *(signed char *)D_001A3A80;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 11) & 15)) != (((int)(unsigned char)*(signed char *)((char *)a1 + 11)) >> 4)) goto L652AB;
case 8:
    l_40 = ((*(int *)((char *)1132) - *(int *)((char *)a1 + 25)) * *(int *)((char *)a1 + 17)) >> 16;
{
    int l_54;
    int l_50;
    l_50 = ((int)(unsigned char)*(signed char *)((char *)a1 + 4)) - 1;
    switch (l_50) {
case 0:
    *(short *)(*(char **)((char *)a1 + 35) + 1) = (l_40 + *(short *)((char *)a1 + 13)) & 2047;
    goto L64D97;
case 1:
    *(short *)(*(char **)((char *)a1 + 35) + 1) = (short)(*(short *)((char *)a1 + 13) - l_40) & 2047;
    goto L64D97;
case 2:
    *(short *)(*(char **)((char *)a1 + 35) + 3) = (l_40 + *(short *)((char *)a1 + 13)) & 2047;
    goto L64D97;
case 3:
    *(short *)(*(char **)((char *)a1 + 35) + 3) = (short)(*(short *)((char *)a1 + 13) - l_40) & 2047;
    goto L64D97;
case 4:
    *(short *)(*(char **)((char *)a1 + 35) + 5) = (l_40 + *(short *)((char *)a1 + 13)) & 2047;
    goto L64D97;
case 5:
    *(short *)(*(char **)((char *)a1 + 35) + 5) = (short)(*(short *)((char *)a1 + 13) - l_40) & 2047;
default:
L64D97:;
    goto L652AB;
}
case 9:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L64E69;
    *(int *)D_00195798 = 1000;
    *(signed char *)D_001957CD = *(signed char *)(*(char **)D_00195BE0 + 129);
    l_40 = 0;
L64DD5:;
    if (l_40 < 35) goto L64DE5;
    goto L64DF4;
L64DDD:;
    l_40++;
    goto L64DD5;
L64DE5:;
    *(short *)(D_001957E9 + (l_40 * 6)) = 50;
    goto L64DDD;
L64DF4:;
    *(int *)D_00196222 = *(int *)(*(char **)((char *)a1 + 35) + 7);
    *(int *)D_00196226 = *(int *)(*(char **)((char *)a1 + 35) + 11) - 40;
    *(int *)D_0019622A = *(int *)(*(char **)((char *)a1 + 35) + 15);
    if (*(signed char *)((char *)func_00065864((int)(unsigned char)*(signed char *)((char *)a1 + 3)) + 7) != 0) goto L64E51;
    func_0005AAE4(*(int *)D_00195AA0, *(int *)D_00195AA0, (int)(unsigned char)*(signed char *)((char *)a1 + 3));
    goto L64E69;
L64E51:;
    func_0005AAE4((int)D_0019621B, *(int *)D_00195AA0, (int)(unsigned char)*(signed char *)((char *)a1 + 3));
L64E69:;
    goto L652AB;
case 10:
    goto L652AB;
case 11:
    func_0003F09F((int)(short)(((unsigned short)(unsigned char)*(signed char *)((char *)a1 + 3)) + 8600), 1);
    goto L652AB;
case 12:
    func_000A1023(*(int *)D_00147954, 655360, 64000, (int)D_00175962, 315, 4);
    *(signed char *)D_001940DA |= 1;
    func_0006530C(((int)(unsigned char)*(signed char *)((char *)a1 + 3)) + 5400);
    l_30 = func_0007CBA1((int)D_0017596A);
    *(signed char *)((char *)l_30 + 3) = 0;
    func_00142790();
    func_0008C566(l_30 + 2, 16);
L64EF5:;
    if (func_0008C5C9() != 0) goto L64F30;
    func_000A1023(*(int *)D_00143550, *(int *)D_00147954, 64000, (int)D_00175962, 324, 4);
    func_0007CC66();
    func_000CDD81(1);
    goto L64EF5;
L64F30:;
    *(signed char *)D_001940DA &= 254;
    if (func_00065398(((int)(unsigned char)*(signed char *)((char *)a1 + 3)) + 5656, l_30 + 2) != 0) goto L64F5D;
    *(signed char *)((char *)a1 + 12) |= 16;
L64F5D:;
    goto L652AB;
case 13:
    goto L652AB;
case 14:
    func_0008DEB4(*(int *)D_00195AA4, *(int *)(*(char **)((char *)a1 + 74) + 7), *(int *)(*(char **)((char *)a1 + 74) + 11), *(int *)(*(char **)((char *)a1 + 74) + 15), (int)(short)*(short *)(*(char **)D_00195AA4 + 1), (int)(short)*(short *)(*(char **)D_00195AA4 + 3), (int)(short)*(short *)(*(char **)D_00195AA4 + 5));
    goto L652AB;
case 15:
    *(short *)(*(char **)((char *)a1 + 35) + 23) = (unsigned short)(unsigned char)*(signed char *)((char *)a1 + 4);
    goto L652AB;
case 16:
    if (((int)(unsigned short)(*(short *)(*(char **)((char *)a1 + 35) + 21) & 64)) == 0) goto L64FEF;
    if (func_00099922(*(int *)((char *)a1 + 35), 0) != 0) goto L64FF1;
L64FEF:;
    goto L64FFB;
L64FF1:;
    *(signed char *)(*(char **)((char *)a1 + 35) + 22) |= 1;
L64FFB:;
    goto L652AB;
case 17:
    *(signed char *)(*(char **)((char *)a1 + 35) + 21) |= 64;
    goto L652AB;
case 18:
    if (func_00099922(*(int *)((char *)a1 + 35), 0) == 0) goto L6502C;
    *(short *)(*(char **)((char *)a1 + 35) + 21) |= 320;
L6502C:;
    goto L652AB;
case 19:
    if (((int)(unsigned short)(*(short *)(*(char **)((char *)a1 + 35) + 21) & 256)) == 0) goto L6505D;
    if (func_00099922(*(int *)((char *)a1 + 35), 1) != 0) goto L6505F;
L6505D:;
    goto L65069;
L6505F:;
    *(signed char *)(*(char **)((char *)a1 + 35) + 22) &= 254;
L65069:;
    goto L652AB;
case 20:
    if (((int)(unsigned short)(*(short *)(*(char **)((char *)a1 + 35) + 21) & 256)) == 0) goto L6509A;
    if (func_00099922(*(int *)((char *)a1 + 35), 1) != 0) goto L6509C;
L6509A:;
    goto L650A6;
L6509C:;
    *(signed char *)(*(char **)((char *)a1 + 35) + 22) &= 254;
L650A6:;
    *(signed char *)(*(char **)((char *)a1 + 35) + 21) &= 191;
    goto L652AB;
case 21:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L65124;
    *(int *)D_00195798 = 1000;
    l_34 = func_0007D6AE((int)(unsigned char)*(signed char *)((char *)a1 + 3), (int)(unsigned char)*(signed char *)((char *)a1 + 4)) * ((int)(unsigned char)*(signed char *)(*(char **)D_00195BE0 + 129));
    if (l_34 != 0) goto L65115;
    l_34 = (int)(unsigned char)*(signed char *)(*(char **)D_00195BE0 + 129);
L65115:;
    func_0002E914(*(int *)D_00195AA0, l_34, 0);
L65124:;
    goto L652AB;
case 22:
    func_0006546F(3, (int)(unsigned char)*(signed char *)((char *)a1 + 4));
    goto L652AB;
case 23:
    func_0006546F(0, (int)(unsigned char)*(signed char *)((char *)a1 + 4));
    goto L652AB;
case 24:
    func_0006546F(1, (int)(unsigned char)*(signed char *)((char *)a1 + 4));
    goto L652AB;
case 25:
    func_0006546F(2, (int)(unsigned char)*(signed char *)((char *)a1 + 4));
    goto L652AB;
case 26:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L651BE;
    *(int *)D_00195798 = 1000;
    func_00065A8C(*(int *)D_00195AA0, (int)&*(signed char *)((char *)func_0007D6AE(0, 11) + 128), 0);
L651BE:;
    goto L652AB;
case 27:
    *(int *)D_00195798 -= *(int *)D_00195AB0;
    if (*(int *)D_00195798 > 0) goto L65200;
    *(int *)D_00195798 = 1000;
    func_00065937(*(int *)D_00195AA0, 0, func_0007D6AE(0, 16), 0);
L65200:;
    goto L652AB;
case 28:
    if (*(signed char *)((char *)a1 + 4) == 0) goto L65224;
    *(short *)(*(char **)D_00195BE0 + 141) -= (unsigned short)(unsigned char)*(signed char *)((char *)a1 + 4);
    goto L65230;
L65224:;
    (*(short *)(*(char **)D_00195BE0 + 141))--;
L65230:;
    goto L652AB;
case 29:
    goto L652AB;
case 30:
    if (*(signed char *)((char *)a1 + 3) == 0) goto L6525E;
    func_00069938((int)(unsigned char)*(signed char *)((char *)a1 + 3), *(int *)((char *)a1 + 35), 110);
L6525E:;
    goto L652AB;
case 31:
    *(signed char *)(D_001952EC + ((int)(unsigned char)*(signed char *)((char *)a1 + 4))) = 1;
    goto L652AB;
case 99:
    if (((int)(unsigned char)*(signed char *)D_00196276) != 1) goto L652A0;
    if (*(signed char *)((char *)a1 + 3) == 0) goto L6529E;
    func_0006530C(((int)(unsigned char)*(signed char *)((char *)a1 + 3)) + 7700);
L6529E:;
    goto L652AB;
L652A0:;
    *(signed char *)D_0019628C = *(signed char *)((char *)a1 + 4);
default:
L652AB:;
    l_3C = ((int)(unsigned char)*(signed char *)((char *)a1 + 4)) >> 4;
    if (l_3C == 0) goto L652F8;
    *(signed char *)D_001A3A81 = ~(*(signed char *)&l_3C);
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 12) & 2)) == 0) goto L652EA;
    l_54 = 0;
    goto L652F0;
L652EA:;
    l_54 = l_3C;
L652F0:;
    *(signed char *)D_001A3A80 = *(signed char *)&l_54;
L652F8:;
    return 1;
}
}
}
