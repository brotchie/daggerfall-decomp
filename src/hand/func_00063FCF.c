/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00063FCF */
#pragma pack(1)
struct Vec { int x; int y; int z; };
struct Mob {
    char pad0;
    short ax;                   /* 0x01 */
    short ay;                   /* 0x03 */
    short az;                   /* 0x05 */
    struct Vec pos;             /* 0x07 */
    char pad1[71 - 19];
    char sub[1];                /* 0x47 */
};
struct Sub { char pad[76]; int height; };
struct Hit {
    int a;                      /* 0 */
    int y;                      /* 4 */
    int b;                      /* 8 */
    int ax;                     /* 12 */
    int ay;                     /* 16 */
    int az;                     /* 20 */
    char *tbl;                  /* 24 */
    short flags;                /* 28 */
    char pad[14];
};
extern char D_00175934[];
extern char D_00187B44[];
extern unsigned char D_001940D7;
extern unsigned char D_001940DA;
extern struct Mob *D_00195AA4;
extern int D_00195AB8;
extern int D_00195C74;
extern char D_00196277;
extern struct Vec D_00196D54;
extern int D_00196D58;
extern int D_00196D5C;
extern short D_00196D64;
extern int func_0002257C(struct Mob *, int, struct Hit *, int);
extern int func_0008DA91(struct Mob *);
extern int func_0009DEAC();
extern int func_000A1023(void *, void *, int, char *, int, int);
extern int func_000C7FD9();

int func_00063FCF(struct Mob *m, int a2, int a3)
{
    struct Hit hit;
    struct Vec unused1;
    int unused2;
    struct Vec pos;
    struct Sub *sub;
    int unused3;
    int unused4;
    struct Vec saved;
    int save28;
    int dist;
    int h;
    int unused5;
    char save10;
    int save14;

    sub = (struct Sub *)m->sub;
    func_000A1023(&pos, &m->pos, 12, D_00175934, 1271, 4);
    hit.a = a2;
    hit.y = m->pos.y;
    hit.b = a3;
    hit.ax = m->ax;
    hit.ay = m->ay;
    hit.az = m->az;
    save10 = D_00196277;
    save14 = D_00195AB8;
    save28 = D_00195C74;
    *(unsigned char *)&D_00196D64 |= 4;
    h = D_00195AB8 = sub->height;
    D_001940D7 |= 128;
    hit.tbl = D_00187B44;
    hit.flags &= ~1;
    func_000A1023(&saved, &D_00196D54, 12, D_00175934, 1293, 4);
    D_00196D54.x = m->pos.x;
    D_00196D58 = m->pos.y - D_00195AB8 / 256;
    D_00196D5C = m->pos.z;
    func_0002257C(m, 0, &hit, 0);
    D_00196277 = save10;
    func_000A1023(&D_00196D54, &saved, 12, D_00175934, 1300, 4);
    if ((int)(short)(D_00196D64 & 16) != 0)
        D_00195AB8 = 1;
    sub->height = D_00195AB8;
    D_00195AB8 = save14;
    D_00195C74 = save28;
    if (func_0009DEAC(m->pos.y - D_00195AA4->pos.y) > 3000) {
        D_001940DA |= 128;
        func_0008DA91(m);
    }
    dist = func_000C7FD9(m->pos.x, m->pos.z, pos.x, pos.z);
    return dist > 2 ? 1 : 0;
}
