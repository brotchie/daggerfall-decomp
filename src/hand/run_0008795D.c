/* matched by the real Watcom C32 10.0a (-d2): a run of maplogic from 0x874C0 to 0x8795D, kept together for its switch table's alignment */
struct obj {
    char pad[3];
    short angle;                /* 3 */
    char pad2[2];
    int x;                      /* 7 */
    int y;                      /* 11 */
    int z;                      /* 15 */
    char pad3[8];
    unsigned short f27;         /* 27 */
    char pad4[34];
    int f63;                    /* 63 */
    int f67;                    /* 67 */
};
#pragma pack(1)
struct loc { int f0; int x; int z; char pad[5]; };
#pragma pack()
struct bits { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct flags { struct bits f[4]; };
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern char D_00176C94[];
extern unsigned char D_001789FA;
extern unsigned char D_001940D5;
extern struct obj *D_00195A98;
extern struct obj *D_00195AA4;
extern struct obj *D_00195AC4;
extern unsigned char *D_00195BDC;
extern unsigned char D_00196268;
extern unsigned char D_00196289;
extern struct loc *D_00196A9C;
extern char D_001A94A0[];
extern char D_001A94B0[];
extern void func_0001FF3F(unsigned char, unsigned char);
extern void func_00028C72(void);
extern void func_000866B4(void);
extern void func_00086794(int);
extern void func_00086D37(unsigned short);
extern void func_00087281(char *);
extern void func_0009A6D0(int, int);
extern int func_000A0040();
extern int func_000C2FF5();
extern int func_0014B45B(int, int);

void func_000874C0(unsigned a1)
{
    switch (a1) {
    case 0:
    case 1:
        D_00195AA4->x = D_00195AC4->x + (D_00195BDC[32] << 11);
        D_00195AA4->z = D_00195AC4->z - 256;
        D_00195AA4->angle = D_00195A98->angle = 0;
        break;
    case 2:
    case 3:
        D_00195AA4->x = D_00195AC4->x - 256;
        D_00195AA4->z = D_00195AC4->z + (D_00195BDC[33] << 11);
        D_00195AA4->angle = D_00195A98->angle = 512;
        break;
    case 4:
    case 5:
        D_00195AA4->x = D_00195AC4->x + (D_00195BDC[32] << 11);
        D_00195AA4->z = D_00195AC4->z + (D_00195BDC[33] << 12) + 256;
        D_00195AA4->angle = D_00195A98->angle = 1024;
        break;
    case 6:
    case 7:
        D_00195AA4->x = D_00195AC4->x + (D_00195BDC[32] << 12) + 256;
        D_00195AA4->z = D_00195AC4->z + (D_00195BDC[33] << 11);
        D_00195AA4->angle = D_00195A98->angle = 1536;
        break;
    }
    if (D_00195BDC[34] == 0)
        func_0009A6D0(D_00195AC4->f63, 8);
    D_00195AA4->y = func_0014B45B(D_00195AA4->x, D_00195AA4->z);
}

void func_000876AD(int a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = D_00195AA4->f67;
    if (D_00196268 == a1 && D_001789FA == a2 && D_00195AC4->f27 == a3) {
        if (D_001789FA == 2)
            func_00087281(*(char **)(D_00195BDC + 43) + a4 * 26);
        else
            func_0009A6D0((int)D_00195AC4, 8);
        return;
    }
    func_00086D37(D_00195AC4->f27);
    D_001789FA = a2;
    if (D_00196268 != a1)
        func_0001FF3F(D_00196268, a1);
    switch (D_001789FA) {
    case 1:
        D_000C23C4 = D_00195AA4->x = D_00196A9C[a3].x & 33554431;
        D_000C23CC = D_00195AA4->z = D_00196A9C[a3].z & 16777215;
        func_000C2FF5();
        func_000A0040(D_001A94B0, 0, 16, D_00176C94, 783, 16);
        func_000A0040(D_001A94A0, 0, 16, D_00176C94, 784, 16);
        func_000866B4();
        D_00195AA4->x = D_00195AC4->x;
        D_00195AA4->z = D_00195AC4->z;
        if (D_00195BDC[34] == 0)
            func_0009A6D0(D_00195AC4->f63, 8);
        D_000C23C8 = func_0014B45B(D_00195AA4->x, D_00195AA4->z);
        D_00195AA4->y = D_000C23C8;
        break;
    case 2:
        D_000C23C4 = D_00195AA4->x = D_00196A9C[a3].x & 33554431;
        D_000C23CC = D_00195AA4->z = D_00196A9C[a3].z & 16777215;
        func_000C2FF5();
        func_000A0040(D_001A94B0, 0, 16, D_00176C94, 803, 16);
        func_000A0040(D_001A94A0, 0, 16, D_00176C94, 804, 16);
        func_000866B4();
        D_00196289++;
        func_00087281(*(char **)(D_00195BDC + 43) + a4 * 26);
        D_00196289--;
        break;
    case 3:
        func_00086794(a3);
        func_00028C72();
    }
    D_001940D5 |= 2;
}

int func_0008795D(struct flags *a1, int a2, int a3)
{
    switch (a2) {
    case 0:
        return a1->f[1].b0;
    case 3:
        return a1->f[1].b1;
    case 5:
        return a1->f[1].b3;
    case 6:
        return a1->f[1].b4;
    case 7:
        return a1->f[1].b5;
    case 8:
        return a1->f[1].b6;
    case 10:
        return a1->f[1].b7;
    case 11:
        return a1->f[2].b0;
    case 13:
    case 14:
        switch (a3) {
        case -1:
            return *(unsigned char *)a1 > 0 ? 1 : 0;
        case 0:
        case 26:
            return a1->f[0].b0;
        case 1:
        case 21:
            return a1->f[0].b1;
        case 2:
        case 29:
            return a1->f[0].b2;
        case 3:
        case 27:
            return a1->f[0].b3;
        case 4:
        case 35:
            return a1->f[0].b4;
        case 5:
        case 24:
            return a1->f[0].b5;
        case 6:
        case 33:
            return a1->f[0].b6;
        case 7:
        case 22:
            return a1->f[0].b7;
        }
        break;
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
        return a1->f[2].b1;
    case 36:
        return a1->f[2].b2;
    case 37:
        return a1->f[2].b3;
    case 38:
        return a1->f[2].b4;
    case 39:
        return a1->f[2].b5;
    case 15:
        return a1->f[2].b7;
    case 12:
        return a1->f[2].b6;
    case 1:
        return a1->f[3].b0;
    }
    return 0;
}
