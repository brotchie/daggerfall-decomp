/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000234EB */
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern char D_001789FA[];
extern char D_00179F48[];
extern char D_001940D7[];
extern char D_001940DB[];
extern char D_00195AA4[];
extern char D_00195BE0[];
extern char D_00195C70[];
extern char D_00195CB8[];
extern char D_00195CD4[];
extern char D_00195CD8[];
extern char D_00195F5A[];
extern char D_00196277[];
extern char D_00196296[];
extern char D_00196B0C[];
extern char D_00196D4C[];
extern char D_00196D50[];
extern char D_00196D64[];
extern int func_0002294E(int, int, int);
extern void func_000231F5(int, int);
extern void func_0002325A(int);
extern void func_00028E24(int);
extern int func_000657B2(int);
extern int func_0009DEAC();
extern int func_0014BDDD();

struct plane {
    int f0;
    int f4;
    int f8;
    int f12;
    int nx;
    int ny;
    int nz;
    short f28;
};

struct planes {
    int count;
    struct plane p[1];
};

#define PLANES (*(struct planes **)D_00196D50)
#define PL (PLANES->p)
#define PLAYER (*(char **)D_00195BE0)
#define FLAGS (*(short *)D_00196D64)

struct vec3 {
    int x;
    int y;
    int z;
};

struct obj {
    char f0[7];
    struct vec3 pos;
};

int func_000234EB(struct obj *a1, int a2, struct vec3 *a3, int a4)
{
    int l_44;
    int l_40;           /* l_40, l_3C and l_34 are never read: they only shape the frame */
    int l_3C;
    struct vec3 l_5C;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    short *l_10;

    l_34 = 0;
    *(int *)D_00195CB8 = *(int *)D_00195C70 = 0;
    *(short *)D_00195F5A = 10000;
    *(int *)D_00195CD8 = *(int *)D_00195CD4 = 0;
    *(int *)D_00196D50 = 0;
    *(int *)D_00196B0C = 0;
    (*(struct vec3 **)D_00196D4C = (struct vec3 *)D_00179F48)->x = a3->x;
    (*(struct vec3 **)D_00196D4C)->y = a3->y;
    (*(struct vec3 **)D_00196D4C)->z = a3->z;
    func_000231F5((int)a1, (int)func_0002325A);
    if (!(((struct bf8_3_1 *)(PLAYER + 137))->f || *(int *)D_00196B0C != 0 || *(unsigned char *)D_001789FA == 1))
        return FLAGS = 16;
    l_14 = FLAGS;
    *(char *)D_00196277 = 1;
    l_44 = func_0002294E((int)a1, a2, (int)a3);
    *(unsigned char *)D_001940D7 &= 223;
    if (*(char *)D_00196277 != 0 && *(char *)D_00196296 != 0)
        *(char *)D_00196296 = 0;
    if (*(int *)D_00195CB8 != 0 && (l_10 = (short *)func_000657B2(*(int *)D_00195CB8)) != 0) {
        if (l_10[0] != 0 || l_10[2] != 0) {
            a3->x += l_10[0];
            a3->y += l_10[1];
            a3->z += l_10[2];
            *(unsigned char *)D_00196D64 |= 4;
            l_44 = func_0002294E((int)a1, a2, (int)a3);
        }
    }
    if (*(int *)D_00195C70 != 0 && (l_10 = (short *)func_000657B2(*(int *)D_00195C70)) != 0) {
        a3->x += l_10[0];
        a3->y += l_10[1];
        a3->z += l_10[2];
        *(unsigned char *)D_00196D64 |= 4;
        l_44 = func_0002294E((int)a1, a2, (int)a3);
    }
    if (*(int *)D_00195CB8 != 0)
        func_00028E24(*(int *)D_00195CB8);
    if (*(int *)D_00195C70 != 0)
        func_00028E24(*(int *)D_00195C70);
    if (l_44 & 2)
        *(short *)D_00195F5A = 0;
    if ((int)a1 == *(int *)D_00195AA4 && ((struct bf8_5_1 *)&D_001940DB)->f)
        return 0;
    if (!(l_44 & 10) || !(l_14 & 4))
        return 0;
    l_5C.x = a3->x - a1->pos.x;
    l_5C.y = a3->y - a1->pos.y;
    l_5C.z = a3->z - a1->pos.z;
    func_0014BDDD((int)&l_5C);
    if (*(int *)D_00196D50 == 0)
        return 1;
    if (PLANES->count > 1) {
        l_24 = 0;
        l_1C = 1000000;
        for (l_20 = 0; l_20 < PLANES->count; l_20++) {
            l_18 = l_5C.x * PL[l_20].nx + l_5C.z * PL[l_20].nz;
            if (l_18 < l_1C) {
                l_1C = l_18;
                l_24 = l_20;
            }
        }
    } else {
        l_1C = l_5C.x * PL[0].nx + l_5C.z * PL[0].nz;
        l_24 = 0;
    }
    l_30 = (a3->x - a1->pos.x) * PL[l_24].nx;
    l_2C = (a3->y - a1->pos.y) * PL[l_24].ny;
    l_28 = (a3->z - a1->pos.z) * PL[l_24].nz;
    l_30 = l_28 + (l_30 + l_2C);
    l_2C = l_30 * PL[l_24].ny;
    l_28 = l_30 * PL[l_24].nz;
    l_30 = l_30 * PL[l_24].nx;
    l_30 >>= 8;
    l_2C >>= 8;
    l_28 >>= 8;
    l_30 = (a3->x - a1->pos.x) - (l_30 >> 8);
    l_2C = (a3->y - a1->pos.y) - (l_2C >> 8);
    l_28 = (a3->z - a1->pos.z) - (l_28 >> 8);
    a3->x = a1->pos.x + l_30;
    a3->y = a1->pos.y + l_2C;
    a3->z = a1->pos.z + l_28;
    *(short *)D_00195F5A = func_0009DEAC(l_1C >> 16);
    if (l_30 != 0 || l_2C != 0 || l_28 != 0) {
        *(int *)D_00195CD8 = *(int *)D_00195CD4 = 0;
        l_14 = FLAGS;
        *(char *)D_00196277 = 1;
        l_44 = func_0002294E((int)a1, a2, (int)a3);
        *(unsigned char *)D_001940D7 &= 223;
        if (*(char *)D_00196277 != 0 && *(char *)D_00196296 != 0)
            *(char *)D_00196296 = 0;
        if (*(int *)D_00195CB8 != 0)
            func_00028E24(*(int *)D_00195CB8);
        if (*(int *)D_00195C70 != 0)
            func_00028E24(*(int *)D_00195C70);
        if ((int)a1 == *(int *)D_00195AA4 && ((struct bf8_4_1 *)&D_001940DB)->f)
            return 0;
        if (!(l_44 & 10) || !(l_14 & 4))
            return 0;
        return 1;
    }
    return FLAGS;
}
