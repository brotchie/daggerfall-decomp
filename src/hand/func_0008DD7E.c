/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008DD7E */
struct obj {
    unsigned char type;
    short ax, ay, az;
    int x, y, z;
    char pad13[48];
    struct obj *parent;
};
extern int D_001A9AF4;
extern int D_001A9AF8;
extern int D_001A9AFC;
extern short D_001A9B00;
extern short D_001A9B04;
extern short D_001A9B08;
extern short D_001A9B1C;
extern short D_001A9B20;
extern short D_001A9B24;
extern int D_001A9B28;
extern int D_001A9B2C;
extern int D_001A9B30;
extern void func_0008302D(int *, int *, int);

void func_0008DD7E(struct obj *a1)
{
    int l_20;
    int dx;
    int dz;

    if ((int)a1->parent == 3 || (int)a1->parent == 18)
        return;
    if (a1->type == 52 || a1->parent->type == 52 || a1->parent->type == 22)
        return;
    a1->ax = D_001A9B00 + (a1->ax - D_001A9B1C) & 2047;
    a1->ay = D_001A9B04 + (a1->ay - D_001A9B20) & 2047;
    a1->az = D_001A9B08 + (a1->az - D_001A9B24) & 2047;
    dx = a1->x - D_001A9B2C;
    dz = a1->z - D_001A9B28;
    func_0008302D(&dx, &dz, a1->ay);
    a1->x = D_001A9AF4 + dx;
    a1->y = D_001A9AF8 + (a1->y - D_001A9B30);
    a1->z = D_001A9AFC + dz;
}
