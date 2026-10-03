/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086B0B */
struct who {
    char pad0[7];
    int x;                  /* 0x07 */
    int y;                  /* 0x0b */
    int z;                  /* 0x0f */
    char pad13[0x1b - 0x13];
    unsigned short id;      /* 0x1b */
    char pad1d[0x1f - 0x1d];
    int f1f;
    char pad23[0x3f - 0x23];
    int f3f;
};
struct place {
    int f0;
    unsigned pad:25;
    unsigned type:5;
    unsigned pad2:2;
    char pad8[4];
    unsigned char fc;
};
extern int D_0012DE00;
extern char D_00176C94[];       /* __FILE__ */
extern char D_00176CC9[];
extern int D_00187F2C;
extern char D_001903A4[];
extern char D_00190FE4[];
extern unsigned D_0019599C;
extern struct who *D_00195AC4;
extern char *D_00195BDC;
extern int D_00195BF4;
extern int D_00195CB8;
extern int D_00195D48;
extern char D_00196289;
extern char D_0019629B;
extern struct place *D_00196A80;
extern char D_00196A88[];
extern char *D_00196A94;
extern char *D_00196A98;
extern void func_0001E3D3(char *, int);
extern void func_0001EC32(void);
extern void func_00027947(void);
extern void func_00028D1A(void);
extern void func_0003FC4B(void);
extern void func_0004105A(void);
extern void func_0004A6B5(int, int, int);
extern void func_0004C759(void);
extern void func_0004CA9D(void);
extern void func_00069D36(void);
extern void func_00076CF7(void);
extern int func_0007CBA1(char *);
extern int func_0007D6AE(int, int);
extern void func_0007E74E(void);
extern void func_0007F8AE(void);
extern void func_00086A71(int);
extern void func_00088213(int, int);
extern void func_0008E3F7(struct who *, void (*)(int));
extern void func_0008EAF1(int, int);
extern void func_0008EB52(void);
extern int func_0009A6D0(struct who *, int);
extern void func_000A1023(void *, void *, int, char *, int, int);
extern int func_0014B45B(int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_00086B0B(int id)
{
    if (D_00195AC4->id == id)
        return;
    func_00069D36();
    func_0008EB52();
    if (D_00195AC4->id == 65535)
        func_0008EAF1(D_00195AC4->f3f, D_00195AC4->f1f);
    func_0001E3D3(D_00196A88, id);
    func_000A1023(D_00195AC4, D_00196A94, 55, D_00176C94, 365, 4);
    func_000A1023(D_00195BDC, D_00196A98, 48, D_00176C94, 366, 4);
    D_00195AC4->y = func_0014B45B(D_00195AC4->x, D_00195AC4->z);
    func_00027947();
    func_0001EC32();
    func_00088213(id, 1);
    func_0007E74E();
    func_00076CF7();
    func_0004105A();
    func_0004CA9D();
    func_0008E3F7(D_00195AC4, func_00086A71);
    if (D_00187F2C != 0) {
        func_0009A6D0(D_00195AC4, 8);
        D_00187F2C = 0;
    }
    if (D_00196289 == 0) {
        func_0003FC4B();
        func_0004C759();
        func_0007F8AE();
        func_00028D1A();
        switch (D_00196A80->type) {
        case 4:
        case 7:
        case 10:
        case 12:
            func_0004A6B5(D_00196A80->fc + 520, 0, 0);
            func_0007CBA1(D_00190FE4);
            break;
        default:
            func_000A0ED9(401, D_00176C94);
            func_000A0F5C(D_001903A4, D_00176CC9, D_00195BDC);
            func_0007CBA1(D_001903A4);
            break;
        }
    }
    D_00195CB8 = 0;
    D_0019629B = 0;
    D_00195D48 = 10000;
    D_0012DE00 = 10000;
    D_0019599C = D_00195BF4 + func_0007D6AE(1400, 1700);
    func_0008EB52();
}
