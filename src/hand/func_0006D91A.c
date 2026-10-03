/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006D91A */
struct prog {
    unsigned char level;        /* 0x00 */
    char pad1;
    unsigned char kind;         /* 0x02 */
    short id;                   /* 0x03 */
    unsigned int time;          /* 0x05 */
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
struct pc { char pad0[543]; unsigned char f543; char pad220[2]; unsigned char f546; };
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
extern void func_00070239(short);
extern int func_00070308(unsigned char);
extern void func_0007141A(int);
extern void func_0007DDC9(short);
extern void func_0008DA91(int);
extern struct obj *func_0008DCE3(char *, int, int);
extern void func_0012B136(void);

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
