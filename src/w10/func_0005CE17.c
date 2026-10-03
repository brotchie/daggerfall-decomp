/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005CE17 */
extern char D_00142950[];
extern char D_00143550[];
extern char D_00175898[];
extern char D_00185CDC[];
extern char D_001959FC[];
extern char D_00195B68[];
extern char D_00195B6C[];
extern char D_00195B78[];
extern char D_00195BE0[];
extern char D_00195BF8[];
extern char D_00195C7C[];
extern char D_00195C80[];
extern char D_00195C84[];
extern char D_00195D74[];
extern char D_00196276[];
extern void func_0005D151(void);
extern int func_0005DB02(void);
extern int func_000A1023();
extern int func_00144ED8();
extern int func_00144F68();
extern int func_00144FB4();

struct img {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    short f8;
    unsigned short size;
    char data[1];
};

#define IMG(g) (*(struct img **)(g))
#define BARS ((struct img **)D_00195C7C)
#define PLAYER (*(char **)D_00195BE0)

void func_0005CE17(void)
{
    unsigned l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct img *l_18;

    if ((**(unsigned short **)D_00195BF8 & 1) == 0) {
        func_000A1023(*(char **)D_00143550 + ((int *)D_00142950)[IMG(D_00195B68)->y], IMG(D_00195B68)->data, IMG(D_00195B68)->size, D_00175898, 124, 4);
        func_00144ED8(131, 154, 47, 22, *(char **)D_00195B6C + ((int *)D_00185CDC)[*(unsigned char *)D_00196276], 0);
        l_1C = func_0005DB02();
        if (l_1C != -1) {
            l_18 = *(struct img **)D_00195D74;
            while (l_1C != 0) {
                l_18 = (struct img *)((char *)l_18 + l_18->size + 12);
                l_1C--;
            }
            func_00144F68(l_18->x, l_18->y, l_18->w, l_18->h, l_18->data);
        }
        l_2C = 23 - IMG(D_00195B78)->w / 2;
        l_28 = 176 - IMG(D_00195B78)->h / 2;
        func_00144FB4(l_2C, l_28, IMG(D_00195B78)->w, IMG(D_00195B78)->h, IMG(D_00195B78)->data);
        func_0005D151();
        l_20 = 0;
    } else {
        l_20 = -40;
    }
    if (*(short *)(PLAYER + 124) > 0) {
        l_24 = ((*(short *)(PLAYER + 124) << 8) / *(short *)(PLAYER + 126) << 5) / 256;
        if (l_24 != 0)
            func_00144F68(l_20 + 49, 32 - l_24 + 161, 4, l_24, BARS[0]->data + (32 - l_24) * 4);
    }
    if (*(unsigned short *)(PLAYER + 155) > 0) {
        l_30 = (*(short *)(PLAYER + 32) + *(short *)(PLAYER + 40)) << 6;
        l_24 = ((*(unsigned short *)(PLAYER + 155) << 8) / l_30 << 5) >> 8;
        if (l_24 != 0)
            func_00144F68(l_20 + 57, 32 - l_24 + 161, 4, l_24, BARS[1]->data + (32 - l_24) * 4);
    }
    if (*(short *)(PLAYER + 141) + *(int *)D_001959FC > 0) {
        l_24 = (((*(short *)(PLAYER + 141) + *(int *)D_001959FC) << 8) / *(short *)(PLAYER + 143) << 5) / 256;
        if (l_24 != 0)
            func_00144F68(l_20 + 65, 32 - l_24 + 161, 4, l_24, BARS[2]->data + (32 - l_24) * 4);
    }
}
