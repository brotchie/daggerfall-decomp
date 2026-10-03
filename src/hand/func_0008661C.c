/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008661C */
struct obj {
    char pad0[7];
    int x;                      /* 0x07 */
    char pad0b[4];
    int y;                      /* 0x0f */
    char pad13[8];
    unsigned short id;          /* 0x1b */
};
struct map {
    char pad0[32];
    unsigned char w;            /* 0x20 */
    unsigned char h;            /* 0x21 */
};
extern struct obj *D_00195AC4;
extern struct map *D_00195BDC;

int func_0008661C(int a1, int a2)
{
    if (D_00195AC4->id != 0xffff)
        if (a1 > D_00195AC4->x && D_00195AC4->x + (D_00195BDC->w << 12) > a1)
            if (a2 > D_00195AC4->y && D_00195AC4->y + (D_00195BDC->h << 12) > a2)
                return 1;
    return 0;
}
