/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005DE74 */
#pragma pack(1)
struct itemdef {
    char name[24];
    int f24;                    /* 24 */
    short f28;                  /* 28 */
    int f30;                    /* 30 */
    int f34;                    /* 34 */
    short f38;                  /* 38 */
    unsigned char f40;          /* 40 */
    unsigned char f41;          /* 41 */
    unsigned char f42;          /* 42 */
    unsigned char f43;          /* 43 */
    short f44;                  /* 44 */
    unsigned short f46;         /* 46 */
};
struct item {
    char name[32];
    unsigned short type;        /* 32 */
    unsigned short sub;         /* 34 */
    int f36;                    /* 36 */
    short f40;                  /* 40 */
    short f42;                  /* 42 */
    short f44;                  /* 44 */
    short f46;                  /* 46 */
    unsigned char f48;          /* 48 */
    unsigned char f49;          /* 49 */
    unsigned short f50;         /* 50 */
    short f52;                  /* 52 */
    unsigned char f54;          /* 54 */
    unsigned char f55;          /* 55 */
    unsigned char f56;          /* 56 */
    int f57;                    /* 57 */
    short f61;                  /* 61 */
    short f63;                  /* 63 */
    unsigned char f65;          /* 65 */
    unsigned char f66;          /* 66 */
    char f67[40];               /* 67 */
};
#pragma pack()
extern char D_001758B8[];
extern char D_001758C0[];
extern char D_001758C1[];
extern struct itemdef D_0017D22A[];
extern unsigned char D_00190CF2;
extern char D_001911E4[];
extern char *D_00195BE0;
extern short D_00195F28;
extern unsigned char D_0019626D;
extern unsigned char D_0019626E;
extern void func_00050069(char *);
extern void func_0005E5D7(struct item *, int);
extern void func_0005E636(struct item *);
extern void func_0005E874(struct item *);
extern void func_0005EA8F(struct item *);
extern void func_0005F75B(struct item *, short);
extern void func_00060430(struct item *, int);
extern int func_0007D6AE(int, int);
extern int func_0009DC25(void);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
#pragma aux func_000A18C3 parm routine [];
extern int func_000A18C3(char *);
extern int func_000A0F5C(char *, char *, ...);

#define PFLAGS (*(short *)(D_00195BE0 + 64))

void func_0005DE74(unsigned short idx, short type, short sub, struct item *it)
{
    struct itemdef *def;
    unsigned short orig;

    orig = idx;
    if (type == 13) {
        sub = 0;
        idx = 284;
    }
    if (type == 6 && ((unsigned short)PFLAGS & 1) != 0)
        type = 12;
    if (type == 12 && ((unsigned short)PFLAGS & 1) == 0)
        type = 6;
    if (type == 4) {
        func_00060430(it, -1);
        return;
    }
    if (type == 9 && (sub < 2 || sub == 4))
        sub = 2;
    if (type == 10 && sub == 11)
        sub = 10;
    if (type == 7) {
        D_00190CF2++;
        idx = 277;
        if (sub > 3)
            sub = func_0007D6AE(0, 3);
    }
    if (idx >= 288) {
        func_000A0ED9(58, D_001758B8);
        func_000A18C3(D_001758C0);
        func_000A0ED9(59, D_001758B8);
        func_000A0F5C(D_001911E4, D_001758C1, orig, idx);
        func_00050069(D_001911E4);
    }
    def = &D_0017D22A[idx];
    if (def->f46 == 32512)
        it->sub = 0;
    func_000A0AD9(it->name, def->name, 32, D_001758B8, 68);
    it->type = type;
    it->sub = sub;
    it->f36 = def->f34;
    if (def->f30 != 0 && (def->f43 & 1) != 0) {
        D_0019626E = def->f30;
        it->f40 = 0;
    } else {
        D_0019626E = 0;
        it->f40 = def->f30;
    }
    it->f42 = (unsigned short)def->f43;
    it->f44 = it->f46 = def->f28;
    it->f48 = 0;
    if (def->f46 != 0 && def->f44 == 0)
        it->f52 = def->f46;
    if (def->f44 != 0 && def->f46 == 0)
        it->f50 = def->f44;
    if (def->f46 != 0)
        it->f50 = def->f46;
    if (def->f44 != 0)
        it->f52 = def->f44;
    if (((unsigned short)it->f50 & -128) == 31360 && ((unsigned short)PFLAGS & 1) == 0) {
        it->f50 &= 127;
        it->f50 |= 31872;
    }
    it->f54 = it->f55 = 0;
    if (type == 1 && (sub == 4 || sub == 5)) {
        if ((func_0009DC25() & 3) != 0) {
            if (sub == 4)
                it->f56 = (func_0009DC25() & 1) + 24;
            else
                it->f56 = (func_0009DC25() & 1) + 26;
        }
    } else {
        it->f56 = 18;
    }
    it->f57 = def->f24;
    it->f61 = def->f38;
    it->f65 = def->f41;
    it->f66 = def->f42;
    func_000A0040(it->f67, -1, 40, D_001758B8, 118, 40);
    D_0019626D = def->f40;
    D_00195F28 = def->f42;
    if (type == 27 && sub == 4)
        it->f49 = func_0009DC25() % 20;
    if (type == 6 || type == 12 || type == 2) {
        func_0005E636(it);
        func_0005E5D7(it, *(unsigned char *)(D_00195BE0 + 67));
    }
    if (type == 3)
        func_0005E874(it);
    if (type == 2) {
        func_0005EA8F(it);
        if (it->sub != 5 && it->sub < 7 && it->f54 == 2)
            func_0005E874(it);
    }
    if (type == 3 && sub == 18) {
        it->f49 = func_0007D6AE(1, 20);
        it->f44 = 0;
    }
    if (type == 7)
        func_0005F75B(it, sub);
    if (type == 13)
        it->f63 = func_0009DC25();
}
