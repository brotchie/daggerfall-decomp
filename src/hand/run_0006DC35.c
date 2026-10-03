/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x0006D91A to 0x0006DC35, kept together for its switch table's alignment */
struct prog {
    unsigned char level;        /* 0x00 */
    char pad1;
    unsigned char kind;         /* 0x02 */
    short id;                   /* 0x03 */
    unsigned int time;          /* 0x05 */
    int f9;                     /* 0x09 */
};
struct obj {
    unsigned char type;         /* 0x00 */
    char pad1[20];
    short f21;                  /* 0x15 */
    char pad17[48];
    struct prog prog;           /* 0x47 */
};
struct msgs {
    short busy;                 /* 0x00 */
    short done;                 /* 0x02 */
    short start;                /* 0x04 */
    short level[10];            /* 0x06 */
};
struct npc { char pad0[29]; short f29; char pad1f[2]; short id; };
struct pc {
    char pad0[116];
    int f116;                   /* 0x74 */
    char pad78[9];
    unsigned char f129;         /* 0x81 */
    char pad82[11];
    short f141;                 /* 0x8d */
    short f143;                 /* 0x8f */
    char pad91[398];
    unsigned char f543;         /* 0x21f */
    char pad220[2];
    unsigned char f546;         /* 0x222 */
};
struct w18 { char pad0[18]; short f18; };
extern unsigned char D_0012AC00;
extern struct msgs D_0018707B[];
extern char D_00187081[];         /* D_0018707B[0].level */
extern struct w18 *D_00195A9C;
extern char *D_00195AA0;
extern int D_00195AF4;
extern struct pc *D_00195BE0;
extern unsigned int D_00195BF4;
extern unsigned char D_00196271;
extern struct npc *D_0019671C;
extern struct prog *D_001A4A14;
extern unsigned char D_001A4A1D;
extern void func_0003F09F(short, int);
extern int func_0006FF7E(int);
extern int func_0007000C(int);
extern char *func_00070239(short);
extern int func_00070308(unsigned char);
extern void func_0007141A(int);
extern void func_0007DDC9(short);
extern void func_0008DA91(int);
extern struct obj *func_0008DCE3(char *, int, int);
extern void func_0012B136(void);
extern char D_00175EAA[];
extern char D_00175EB3[];
extern char D_00175EC1[];
extern char D_00175EEC[];
extern char D_00175F17[];
extern char D_00175F42[];
extern char D_00175F6D[];
extern char D_00175F98[];
extern char D_00175FC3[];
extern char D_00175FEE[];
extern char D_00176019[];
extern char D_00176044[];
extern char D_001788DF[];
extern char D_001832B0[];
extern char D_001837E8[];
extern char D_00186F37[];
extern char D_00186F4C[];
extern char D_00186F5F[];
extern char D_00186F74[];
extern char D_00186F88[];
extern char D_00186F9E[];
extern char D_00186FB6[];
extern char D_00186FC9[];
extern char D_00186FDB[];
extern char D_00186FEE[];
extern char D_00187001[];
extern char D_00187017[];
extern char D_0018DD94[];
extern char D_0018DDB4[];
extern char D_001903A4[];
extern char D_00195AA4[];
extern char D_00195AC4[];
extern char D_00195B50[];
extern char D_00195BDC[];
extern char D_00195BEC[];
extern char D_001960D9[];
extern char D_00196118[];
extern char D_00196268[];
extern char D_001962AB[];
struct slot { struct prog *p; int f4; int f8; int f12; int f16; };   /* 20 bytes */
extern struct slot D_001A3FAC[];
extern int D_001A41DC;
extern struct obj *D_001A41E4;
extern char D_001A41F3[];
extern char D_001A4A1A[];
extern char D_001A4A1C[];
extern int func_000192EE(short);
extern void func_000202C5(int);
extern int func_00036F69(int);
extern void func_0003C9C1(int);
extern void func_0003EC2A(int, int);
extern int func_0004C274(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern int func_00055E35(int);
extern void func_0005E37F(unsigned short, int, int, int);
extern void func_0006B7EE(int);
extern int func_0006CB53(int, int);
extern void func_0006F55F(void);
extern void func_0006F59F(void);
extern void func_0006F673(void);
extern void func_000705B0(void);
extern void func_00070715(void);
extern void func_00070755(void);
extern void func_00070887(void);
extern void func_00070D48(void);
extern void func_00070DFD(void);
extern int func_00070EC0(int);
extern int func_000710AE(short);
extern int func_0007110F(int, int, int);
extern void func_00071518(void);
extern void func_0007567B(int, int);
extern void func_000756C6(int);
extern int func_0007CBA1(int);
extern int func_0007D6AE(int, int);
extern void func_0008DA1C(int);
extern void func_0008E3F7(int, int);
extern int func_0008F246(int);
extern void func_000922F6(int, int, int);
extern void func_00097101(int);
extern int func_000A0024();
extern int func_000A0AD9();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int func_000A0F5C(int, ...);

void func_0006D91A(int a1, int a2)
{
    struct obj *obj;
    int delta;
    int r;
    int orig;
    int flag;

    orig = a1;
    flag = 0;
    a1 &= 63;
    if (D_001A4A14 == 0 && a2 != 0) {
        D_001A4A1D++;
        if (D_001A4A1D != (char)1)
            return;
        if ((orig & 64) && func_00070308(64) != 0)
            return;
        if ((orig & 128) && func_00070308(128) != 0)
            return;
        r = func_0006FF7E(a1);
        if (r == 2) {
            func_0003F09F(D_0018707B[a1].busy, 1);
            return;
        }
        if (r == 1) {
            func_0003F09F(D_0018707B[a1].done, 1);
            return;
        }
        func_0007DDC9(D_0018707B[a1].start);
        if (D_00196271 == 2)
            return;
        obj = func_0008DCE3(D_00195AA0, 0, 13);
        obj->type = 10;
        obj->f21 = 3;
        (D_001A4A14 = &obj->prog)->level = 0;
        D_001A4A14->kind = orig;
        D_001A4A14->id = D_0019671C->id;
        D_001A4A14->time = D_00195BF4;
        while (D_0012AC00)
            func_0012B136();
        func_0003F09F(D_0018707B[a1].level[0], 1);
        return;
    }
    if (D_001A4A14 == 0)
        return;
    if (D_00195BF4 - D_001A4A14->time <= 40320)
        return;
    delta = func_0007000C(a1) - D_001A4A14->level;
    if (D_0019671C->f29 < 0) {
        delta = -(D_001A4A14->level + 1);
        flag = 1;
    }
    if (delta > 0 || flag != 0) {
        D_001A4A14->level += delta;
        D_001A4A14->time = D_00195BF4;
        if (a1 == 3 && (D_001A4A14->level == 6 || D_001A4A14->level == 8))
            func_0007141A(0);
        if (a1 == 0 && D_001A4A14->level < 100)
            func_0007141A(1);
        if (D_001A4A14->level > 100) {
            func_0003F09F(668, 1);
            func_00070239(D_00195A9C->f18);
            if (D_00195AF4 != 0)
                func_0008DA91(D_00195AF4);
            if (a1 == 3)
                D_00195BE0->f546 = 0;
            if (a1 == 0)
                D_00195BE0->f543 = 0;
            D_001A4A14 = 0;
            return;
        }
        if (delta < 0) {
            func_0003F09F(667, 1);
            return;
        }
        func_0003F09F(*(short *)(D_00187081 + (a1 * 26 + D_001A4A14->level * 2)), 1);
    }
}

void func_0006DC35(int a1)
{
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = 0;
    l_20 = 0;
    *(signed char *)D_001A4A1C = 0;
    func_0008DA1C((int)D_001960D9);
    D_0019671C = (struct npc *)func_000192EE(D_00195A9C->f18);
    D_001A4A14 = (struct prog *)func_00070239(D_00195A9C->f18);
    l_2C = func_00070EC0((int)D_0019671C);
    *(int *)D_00195B50 = a1;
    func_0006D91A(l_2C, 0);
    *(short *)D_001A4A1A = ((unsigned)*(int *)((char *)a1 + 31)) >> 16;
    l_1C = func_000710AE((int)(short)*(short *)((char *)a1 + 71));
    if (l_1C != 0) goto L6DCEE;
    func_000756C6(a1);
    return;
L6DCEE:;
    l_28 = (((int)D_001A4A14 != 0) ? 1 : 0);
    func_000A0ED9(163, (int)D_00175EAA);
    func_000A0F5C((int)D_001903A4, (int)D_00175EB3, l_28 + 48);
    l_18 = func_0006CB53((int)D_001903A4, 0);
    l_30 = func_0007110F(l_18, l_28, l_1C);
    switch (l_30) {
case 0:
    func_0006D91A(l_2C, 1);
    if (l_18 == 0) goto L6DD8B;
    if (l_18 != (-1751672937)) goto L6DD8D;
L6DD8B:;
    goto L6DDA6;
L6DD8D:;
    func_000A0024(l_18, (int)D_00175EAA, 170);
    l_18 = -1751672937;
L6DDA6:;
    return;
case 1:
    func_000756C6(a1);
    if (l_18 == 0) goto L6DDC2;
    if (l_18 != (-1751672937)) goto L6DDC4;
L6DDC2:;
    goto L6DDDD;
L6DDC4:;
    func_000A0024(l_18, (int)D_00175EAA, 174);
    l_18 = -1751672937;
L6DDDD:;
    return;
case 2:
    if (l_18 == 0) goto L6DDF1;
    if (l_18 != (-1751672937)) goto L6DDF3;
L6DDF1:;
    goto L6DE0C;
L6DDF3:;
    func_000A0024(l_18, (int)D_00175EAA, 177);
    l_18 = -1751672937;
L6DE0C:;
    goto L6DE67;
case 3:
    if (l_18 == 0) goto L6DE1D;
    if (l_18 != (-1751672937)) goto L6DE1F;
L6DE1D:;
    goto L6DE38;
L6DE1F:;
    func_000A0024(l_18, (int)D_00175EAA, 180);
    l_18 = -1751672937;
L6DE38:;
    return;
default:
    if (l_18 == 0) goto L6DE4C;
    if (l_18 != (-1751672937)) goto L6DE4E;
L6DE4C:;
    goto L6DE67;
L6DE4E:;
    func_000A0024(l_18, (int)D_00175EAA, 183);
    l_18 = -1751672937;
L6DE67:;
}
    switch ((unsigned)l_2C) {
    goto L6F43A;
case 0:
    if ((int)D_001A4A14 == 0) goto L6E093;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 839:
    func_0003C9C1((int)D_00186F37);
    goto L6E093;
case 841:
    if ((D_001A4A14->level) < 1) goto L6DFC7;
    func_0006F55F();
    goto L6DFCE;
L6DFC7:;
    l_24 = 3100;
L6DFCE:;
    goto L6E093;
case 840:
    if ((D_001A4A14->level) < 3) goto L6DFF0;
    func_0008F246(1);
    goto L6DFF7;
L6DFF0:;
    l_24 = 3100;
L6DFF7:;
    goto L6E093;
case 843:
    if ((D_001A4A14->level) >= 5) goto L6E016;
    l_24 = 3100;
    goto L6E01B;
L6E016:;
    func_00070715();
L6E01B:;
    goto L6E093;
case 842:
    if ((D_001A4A14->level) >= 7) goto L6E03A;
    l_24 = 3100;
    goto L6E051;
L6E03A:;
    func_0003F09F(402, 1);
    func_000756C6(a1);
L6E051:;
    goto L6E093;
case 807:
    if (*(signed char *)((char *)a1 + 38) != 0) goto L6E081;
    func_0004C274(76, 0, 48, 66, D_001A4A14->level);
    goto L6E089;
L6E081:;
    func_000756C6(a1);
L6E089:;
    goto L6E093;
default:
    func_000756C6(a1);
L6E093:;
    goto L6F43A;
}
case 1:
    if ((int)D_001A4A14 == 0) goto L6E217;
    if (((int)(unsigned short)(*(short *)(*(char **)D_00195BEC + 4) & 8)) == 0) goto L6E0D7;
    if (D_00195BE0->f141 != D_00195BE0->f143) goto L6E0D9;
L6E0D7:;
    goto L6E101;
L6E0D9:;
    D_00195BE0->f141 = D_00195BE0->f143;
    func_0003F09F(465, 1);
L6E101:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 61:
    func_0003C9C1((int)D_00186F5F);
    goto L6E215;
case 64:
    func_00036F69(1);
    goto L6E215;
case 65:
    if ((D_001A4A14->level) < 3) goto L6E197;
    func_0006F673();
    goto L6E19E;
L6E197:;
    l_24 = 3100;
L6E19E:;
    goto L6E215;
case 802:
    if ((D_001A4A14->level) < 5) goto L6E1C0;
    func_00055E35(1);
    goto L6E1C7;
L6E1C0:;
    l_24 = 3100;
L6E1C7:;
    goto L6E215;
case 66:
    if ((D_001A4A14->level) < 6) goto L6E1E4;
    func_000202C5(a1);
    goto L6E1EB;
L6E1E4:;
    l_24 = 3100;
L6E1EB:;
    goto L6E215;
case 62:
    if ((D_001A4A14->level) < 8) goto L6E205;
    func_000705B0();
    goto L6E20C;
L6E205:;
    l_24 = 3100;
L6E20C:;
    goto L6E215;
default:
    l_20 = 1;
L6E215:;
    goto L6E21E;
L6E217:;
    l_20 = 1;
L6E21E:;
    if (l_20 == 0) goto L6E338;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 60:
    func_0006F59F();
    goto L6E338;
case 63:
    if (*(signed char *)((char *)a1 + 38) == 0) goto L6E279;
    func_000756C6(a1);
    goto L6E2D2;
L6E279:;
    if ((int)D_001A4A14 == 0) goto L6E2AB;
    func_0004C274(78, 0, 48, 66, D_00195BE0->f129);
    goto L6E2D2;
L6E2AB:;
    func_0004C274(78, 0, 48, 67, D_00195BE0->f129);
L6E2D2:;
    goto L6E338;
case 801:
    func_0007CBA1(*(int *)D_001832B0);
    func_0008DA1C((int)D_001960D9);
    *(int *)D_001788DF = ((10 - (D_001A4A14->level)) << 8) / 10;
    func_000922F6((int)D_001960D9, 4, 8);
    goto L6E338;
default:
    func_0003EC2A((int)D_00175EC1, 1);
L6E338:;
    goto L6F43A;
}
case 2:
    if ((int)D_001A4A14 == 0) goto L6E403;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 849:
    func_0003C9C1((int)D_00186F74);
    goto L6E401;
case 850:
    *(int *)D_001788DF = ((10 - (D_001A4A14->level)) << 8) / 10;
    func_0007567B(255, a1);
    goto L6E401;
case 851:
    if (*(signed char *)((char *)a1 + 38) != 0) goto L6E3F0;
    func_0004C274(77, 0, 48, 66, D_001A4A14->level);
    goto L6E3F8;
L6E3F0:;
    func_000756C6(a1);
L6E3F8:;
    goto L6E401;
default:
    l_20 = 1;
L6E401:;
    goto L6E40A;
L6E403:;
    l_20 = 1;
L6E40A:;
    if (l_20 == 0) goto L6E463;
    if (((int)(unsigned short)*(short *)((char *)a1 + 71)) != 851) goto L6E45B;
    if (*(signed char *)((char *)a1 + 38) != 0) goto L6E451;
    func_0004C274(77, 0, 48, 67, D_001A4A14->level);
    goto L6E459;
L6E451:;
    func_000756C6(a1);
L6E459:;
    goto L6E463;
L6E45B:;
    func_000756C6(a1);
L6E463:;
    goto L6F43A;
}
case 3:
    if ((int)D_001A4A14 != 0) goto L6E486;
    goto L6E57A;
L6E486:;
    switch ((unsigned short)(*(short *)((char *)a1 + 71) - 803)) {
case 0:
    func_0003C9C1((int)D_00186F4C);
    goto L6E57A;
case 1:
    if (*(signed char *)((char *)a1 + 38) != 0) goto L6E4EE;
    func_0004C274(79, 0, 48, 66, D_001A4A14->level);
    goto L6E4F6;
L6E4EE:;
    func_000756C6(a1);
L6E4F6:;
    goto L6E57A;
case 2:
    if ((D_001A4A14->level) >= 2) goto L6E515;
    l_24 = 3100;
    goto L6E57A;
L6E515:;
    *(int *)D_001788DF = 128;
    func_0008DA1C((int)D_001960D9);
    func_000922F6((int)D_001960D9, 2, 6);
    goto L6E57A;
case 3:
    if ((D_001A4A14->level) >= 4) goto L6E559;
    l_24 = 3100;
    goto L6E57A;
L6E559:;
    func_0003F09F(402, 1);
    func_000756C6(a1);
    goto L6E57A;
default:
    func_000756C6(a1);
L6E57A:;
    goto L6F43A;
}
case 68:
case 69:
case 70:
case 71:
case 72:
case 73:
case 74:
case 75:
case 76:
case 77:
    if ((int)D_001A4A14 == 0) goto L6E7B0;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 845:
    if (((1 << (D_001A4A14->level)) & D_001A4A14->f9) == 0) goto L6E5EC;
    func_0003F09F(461, 1);
    goto L6E7AE;
L6E5EC:;
    D_001A4A14->f9 |= 1 << (D_001A4A14->level);
    l_38 = (int)func_0008DCE3((char *)a1, 0, 107);
    *(signed char *)((char *)l_38) = 2;
    *(signed char *)D_001962AB = D_001A4A14->level - 1;
    if (((int)(unsigned char)*(signed char *)D_001962AB) <= 100) goto L6E640;
    *(signed char *)D_001962AB = 2;
    goto L6E653;
L6E640:;
    if (((int)(unsigned char)*(signed char *)D_001962AB) <= 10) goto L6E653;
    *(signed char *)D_001962AB = 10;
L6E653:;
    func_0005E37F(2, 0, 6, l_38 + 71);
    *(signed char *)((char *)l_38 + 126) = 2;
    *(int *)((char *)l_38 + 7) = *(int *)(*(char **)D_00195AA4 + 7);
    *(int *)((char *)l_38 + 11) = *(int *)(*(char **)D_00195AA4 + 11);
    *(int *)((char *)l_38 + 15) = *(int *)(*(char **)D_00195AA4 + 15);
    func_00097101(l_38);
    func_0003F09F(463, 1);
    goto L6E7AE;
case 848:
    if (D_00195BE0->f116 != 0) goto L6E7AE;
    if ((D_001A4A14->level) == 9) goto L6E6EB;
    func_0003F09F(460, 1);
    goto L6E7AE;
L6E6EB:;
    func_0008E3F7(*(int *)D_00195AC4, (int)func_0006B7EE);
    if (*(signed char *)D_001A41F3 == 0) goto L6E7AE;
    l_30 = func_0007D6AE(0, *(unsigned char *)D_001A41F3 - 1);
    D_00195BE0->f116 = D_001A3FAC[l_30].f12;
    D_001A41E4 = (struct obj *)((char *)D_001A3FAC[l_30].p - 71);
    D_001A41DC = D_001A3FAC[l_30].f4;
    func_0003F09F(462, 1);
    func_000A0AD9((int)D_0018DDB4, *(int *)(D_001837E8 + (((int)(unsigned char)*(signed char *)D_00196268) << 2)), 32, (int)D_00175EAA, 413);
    func_000A0AD9((int)D_0018DD94, *(int *)D_00195BDC, 32, (int)D_00175EAA, 414);
    goto L6E7AE;
default:
    l_20 = 1;
L6E7AE:;
    goto L6E7B7;
L6E7B0:;
    l_20 = 1;
L6E7B7:;
    if (l_20 == 0) goto L6E854;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 846:
    if (*(signed char *)((char *)a1 + 38) == 0) goto L6E7EA;
    func_000756C6(a1);
    goto L6E843;
L6E7EA:;
    if ((int)D_001A4A14 == 0) goto L6E81C;
    func_0004C274(66, 0, 48, 66, D_00195BE0->f129);
    goto L6E843;
L6E81C:;
    func_0004C274(66, 0, 48, 67, D_00195BE0->f129);
L6E843:;
    goto L6E854;
default:
    func_0003EC2A((int)D_00175EEC, 1);
L6E854:;
    goto L6F43A;
}
case 142:
    if ((int)D_001A4A14 != 0) goto L6E87A;
    goto L6E942;
L6E87A:;
    func_00071518();
    switch ((unsigned short)(*(short *)((char *)a1 + 71) - 453)) {
case 0:
    if ((D_001A4A14->level) < 1) goto L6E8C2;
    func_0006F55F();
    goto L6E8C9;
L6E8C2:;
    l_24 = 3100;
L6E8C9:;
    goto L6E940;
case 1:
    if ((D_001A4A14->level) < 4) goto L6E8EB;
    func_0008F246(1);
    goto L6E8F2;
L6E8EB:;
    l_24 = 3100;
L6E8F2:;
    goto L6E940;
case 2:
    if ((D_001A4A14->level) < 4) goto L6E90C;
    func_00070715();
    goto L6E913;
L6E90C:;
    l_24 = 3100;
L6E913:;
    goto L6E940;
case 3:
    if ((D_001A4A14->level) < 7) goto L6E930;
    func_000202C5(a1);
    goto L6E937;
L6E930:;
    l_24 = 3100;
L6E937:;
    goto L6E940;
default:
    l_20 = 1;
L6E940:;
    goto L6E949;
L6E942:;
    l_20 = 1;
L6E949:;
    if (l_20 == 0) goto L6E9C3;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 241:
    func_0003C9C1((int)D_00186F88);
    goto L6E9C3;
case 240:
    func_00070DFD();
    goto L6E9C3;
case 810:
    func_00070D48();
    goto L6E9C3;
case 813:
    func_00070755();
    goto L6E9C3;
default:
    func_0003EC2A((int)D_00175F17, 1);
L6E9C3:;
    goto L6F43A;
}
case 143:
    if ((int)D_001A4A14 == 0) goto L6EA92;
    if ((D_001A4A14->level) < 2) goto L6E9EB;
    func_00071518();
L6E9EB:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 462:
    if ((D_001A4A14->level) < 1) goto L6EA36;
    func_0006F55F();
    goto L6EA3D;
L6EA36:;
    l_24 = 3100;
L6EA3D:;
    goto L6EA90;
case 463:
    if ((D_001A4A14->level) < 6) goto L6EA5C;
    func_0008F246(1);
    goto L6EA63;
L6EA5C:;
    l_24 = 3100;
L6EA63:;
    goto L6EA90;
case 464:
    if ((D_001A4A14->level) < 8) goto L6EA80;
    func_000202C5(a1);
    goto L6EA87;
L6EA80:;
    l_24 = 3100;
L6EA87:;
    goto L6EA90;
default:
    l_20 = 1;
L6EA90:;
    goto L6EA99;
L6EA92:;
    l_20 = 1;
L6EA99:;
    if (l_20 == 0) goto L6EB2C;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 243:
    func_0003C9C1((int)D_00186F9E);
    goto L6EB2C;
case 810:
    func_00070D48();
    goto L6EB2C;
case 460:
    func_00070887();
    goto L6EB2C;
case 813:
    func_00070755();
    goto L6EB2C;
case 240:
    func_00070DFD();
    goto L6EB2C;
default:
    func_0003EC2A((int)D_00175F42, 1);
L6EB2C:;
    goto L6F43A;
}
case 144:
    if ((int)D_001A4A14 == 0) goto L6EBFB;
    if ((D_001A4A14->level) < 1) goto L6EB54;
    func_00071518();
L6EB54:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 468:
    if ((D_001A4A14->level) < 2) goto L6EB9F;
    func_0006F55F();
    goto L6EBA6;
L6EB9F:;
    l_24 = 3100;
L6EBA6:;
    goto L6EBF9;
case 469:
    if ((D_001A4A14->level) < 5) goto L6EBC5;
    func_0008F246(1);
    goto L6EBCC;
L6EBC5:;
    l_24 = 3100;
L6EBCC:;
    goto L6EBF9;
case 470:
    if ((D_001A4A14->level) < 7) goto L6EBE9;
    func_000202C5(a1);
    goto L6EBF0;
L6EBE9:;
    l_24 = 3100;
L6EBF0:;
    goto L6EBF9;
default:
    l_20 = 1;
L6EBF9:;
    goto L6EC02;
L6EBFB:;
    l_20 = 1;
L6EC02:;
    if (l_20 == 0) goto L6EC95;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 245:
    func_0003C9C1((int)D_00186FB6);
    goto L6EC95;
case 810:
    func_00070D48();
    goto L6EC95;
case 466:
    func_00070887();
    goto L6EC95;
case 813:
    func_00070755();
    goto L6EC95;
case 240:
    func_00070DFD();
    goto L6EC95;
default:
    func_0003EC2A((int)D_00175F6D, 1);
L6EC95:;
    goto L6F43A;
}
case 145:
    if ((int)D_001A4A14 == 0) goto L6ED64;
    if ((D_001A4A14->level) < 1) goto L6ECBD;
    func_00071518();
L6ECBD:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 473:
    if ((D_001A4A14->level) < 4) goto L6ED08;
    func_0006F55F();
    goto L6ED0F;
L6ED08:;
    l_24 = 3100;
L6ED0F:;
    goto L6ED62;
case 474:
    if ((D_001A4A14->level) < 5) goto L6ED2E;
    func_0008F246(1);
    goto L6ED35;
L6ED2E:;
    l_24 = 3100;
L6ED35:;
    goto L6ED62;
case 475:
    if ((D_001A4A14->level) < 7) goto L6ED52;
    func_000202C5(a1);
    goto L6ED59;
L6ED52:;
    l_24 = 3100;
L6ED59:;
    goto L6ED62;
default:
    l_20 = 1;
L6ED62:;
    goto L6ED6B;
L6ED64:;
    l_20 = 1;
L6ED6B:;
    if (l_20 == 0) goto L6EDFE;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 247:
    func_0003C9C1((int)D_00186FC9);
    goto L6EDFE;
case 810:
    func_00070D48();
    goto L6EDFE;
case 471:
    func_00070887();
    goto L6EDFE;
case 813:
    func_00070755();
    goto L6EDFE;
case 240:
    func_00070DFD();
    goto L6EDFE;
default:
    func_0003EC2A((int)D_00175F98, 1);
L6EDFE:;
    goto L6F43A;
}
case 146:
    if ((int)D_001A4A14 == 0) goto L6EECD;
    if ((D_001A4A14->level) < 2) goto L6EE26;
    func_00071518();
L6EE26:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 480:
    if ((D_001A4A14->level) < 3) goto L6EE71;
    func_0006F673();
    goto L6EE78;
L6EE71:;
    l_24 = 3100;
L6EE78:;
    goto L6EECB;
case 481:
    if ((D_001A4A14->level) < 5) goto L6EE97;
    func_00055E35(1);
    goto L6EE9E;
L6EE97:;
    l_24 = 3100;
L6EE9E:;
    goto L6EECB;
case 482:
    if ((D_001A4A14->level) < 6) goto L6EEBB;
    func_000202C5(a1);
    goto L6EEC2;
L6EEBB:;
    l_24 = 3100;
L6EEC2:;
    goto L6EECB;
default:
    l_20 = 1;
L6EECB:;
    goto L6EED4;
L6EECD:;
    l_20 = 1;
L6EED4:;
    if (l_20 == 0) goto L6EF86;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 249:
    func_0003C9C1((int)D_00186FDB);
    goto L6EF86;
case 810:
    func_00070D48();
    goto L6EF86;
case 477:
    func_00070887();
    goto L6EF86;
case 813:
    func_00070755();
    goto L6EF86;
case 240:
    func_00070DFD();
    goto L6EF86;
default:
    func_0003EC2A((int)D_00175FC3, 1);
L6EF86:;
    goto L6F43A;
}
case 147:
    if ((int)D_001A4A14 == 0) goto L6F064;
    if ((D_001A4A14->level) < 2) goto L6EFAE;
    func_00071518();
L6EFAE:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 485:
    if ((D_001A4A14->level) < 1) goto L6F008;
    func_0006F55F();
    goto L6F00F;
L6F008:;
    l_24 = 3100;
L6F00F:;
    goto L6F062;
case 487:
    if ((D_001A4A14->level) < 5) goto L6F02E;
    func_0008F246(1);
    goto L6F035;
L6F02E:;
    l_24 = 3100;
L6F035:;
    goto L6F062;
case 488:
    if ((D_001A4A14->level) < 7) goto L6F052;
    func_000202C5(a1);
    goto L6F059;
L6F052:;
    l_24 = 3100;
L6F059:;
    goto L6F062;
default:
    l_20 = 1;
L6F062:;
    goto L6F06B;
L6F064:;
    l_20 = 1;
L6F06B:;
    if (l_20 == 0) goto L6F11D;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 250:
    func_0003C9C1((int)D_00186FEE);
    goto L6F11D;
case 810:
    func_00070D48();
    goto L6F11D;
case 484:
    func_00070887();
    goto L6F11D;
case 813:
    func_00070755();
    goto L6F11D;
case 240:
    func_00070DFD();
    goto L6F11D;
default:
    func_0003EC2A((int)D_00175FEE, 1);
L6F11D:;
    goto L6F43A;
}
case 148:
    if ((int)D_001A4A14 == 0) goto L6F1EA;
    func_00071518();
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 490:
    if ((D_001A4A14->level) < 2) goto L6F18E;
    func_0006F55F();
    goto L6F195;
L6F18E:;
    l_24 = 3100;
L6F195:;
    goto L6F1E8;
case 491:
    if ((D_001A4A14->level) < 5) goto L6F1B4;
    func_0008F246(1);
    goto L6F1BB;
L6F1B4:;
    l_24 = 3100;
L6F1BB:;
    goto L6F1E8;
case 492:
    if ((D_001A4A14->level) < 7) goto L6F1D8;
    func_000202C5(a1);
    goto L6F1DF;
L6F1D8:;
    l_24 = 3100;
L6F1DF:;
    goto L6F1E8;
default:
    l_20 = 1;
L6F1E8:;
    goto L6F1F1;
L6F1EA:;
    l_20 = 1;
L6F1F1:;
    if (l_20 == 0) goto L6F2A3;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 252:
    func_0003C9C1((int)D_00187001);
    goto L6F2A3;
case 810:
    func_00070D48();
    goto L6F2A3;
case 493:
    func_00070887();
    goto L6F2A3;
case 813:
    func_00070755();
    goto L6F2A3;
case 240:
    func_00070DFD();
    goto L6F2A3;
default:
    func_0003EC2A((int)D_00176019, 1);
L6F2A3:;
    goto L6F43A;
}
case 149:
    if ((int)D_001A4A14 == 0) goto L6F381;
    if ((D_001A4A14->level) < 1) goto L6F2CB;
    func_00071518();
L6F2CB:;
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 496:
    if ((D_001A4A14->level) < 3) goto L6F325;
    func_0006F59F();
    goto L6F32C;
L6F325:;
    l_24 = 3100;
L6F32C:;
    goto L6F37F;
case 497:
    if ((D_001A4A14->level) < 6) goto L6F34B;
    func_00036F69(1);
    goto L6F352;
L6F34B:;
    l_24 = 3100;
L6F352:;
    goto L6F37F;
case 498:
    if ((D_001A4A14->level) < 7) goto L6F36F;
    func_000202C5(a1);
    goto L6F376;
L6F36F:;
    l_24 = 3100;
L6F376:;
    goto L6F37F;
default:
    l_20 = 1;
L6F37F:;
    goto L6F388;
L6F381:;
    l_20 = 1;
L6F388:;
    if (l_20 == 0) goto L6F43A;
}
    switch (*(unsigned short *)((char *)a1 + 71)) {
case 254:
    func_0003C9C1((int)D_00187017);
    goto L6F43A;
case 810:
    func_00070D48();
    goto L6F43A;
case 494:
    func_00070887();
    goto L6F43A;
case 813:
    func_00070755();
    goto L6F43A;
case 240:
    func_00070DFD();
    goto L6F43A;
default:
    func_0003EC2A((int)D_00176044, 1);
}
default:
L6F43A:;
    if (l_24 == 0) goto L6F44E;
    func_0003F09F((int)(short)*(short *)&l_24, 1);
L6F44E:;
    if (*(signed char *)D_001A4A1C == 0) goto L6F460;
    if (*(int *)D_00196118 != 0) goto L6F462;
L6F460:;
    return;
L6F462:;
    *(signed char *)D_001A4A1C = 0;
    func_000922F6((int)D_001960D9, 0, 6);
}
}
