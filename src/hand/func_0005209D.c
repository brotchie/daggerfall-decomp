/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005209D */
#pragma pack(1)
struct anim {
    unsigned char flags;
    char pad1;
    unsigned short handle;
    short a4;
    short a6;
    short a8;
    int a10;
    short x;
    short y;
    short w;
    short h;
    char *buf0;
    char *buf1;
    char *buf2;
};
struct hdr {
    short type;
    short pad2;
    short x;
    short y;
    short pad8[2];
};
struct info {
    char pad0[6];
    short a6;
    short w8;
    short h10;
    char pad12[4];
    unsigned size;
    char pad20[60];
    int seek;
    int a84;
    char pad88[40];
};
#pragma pack()
extern char D_00175404[];
extern unsigned short func_0006CD6E(int);
extern int func_000A0040();
extern int func_000A006E(int, int, int);
extern char *func_000A00AF(int, char *, int);
extern int func_000A00CB(int, void *, int);

int func_0005209D(int a1, struct anim *a2)
{
    struct info l_A8;
    struct hdr l_28;

    a2->handle = func_0006CD6E(a1);
    if (a2->handle < 1)
        return 0;
    func_000A00CB(a2->handle, &l_A8, 128);
    a2->a4 = l_A8.a6;
    a2->a8 = l_A8.size / 55 + 1;
    a2->w = l_A8.w8;
    a2->h = l_A8.h10;
    a2->y = a2->x = 0;
    a2->a10 = l_A8.a84;
    if (a2->buf0 == 0) {
        a2->flags |= 1;
        a2->buf0 = func_000A00AF(a2->w * a2->h, D_00175404, 225);
    }
    if (a2->buf1 == 0) {
        a2->flags |= 2;
        a2->buf1 = func_000A00AF(2050, D_00175404, 231);
    }
    func_000A0040(a2->buf1, 0, 2050, D_00175404, 233, 4);
    func_000A00CB(a2->handle, &l_28, 10);
    if ((unsigned short)l_28.x == 0xF100) {
        func_000A00CB(a2->handle, &l_28, 8);
        if (l_28.type == 3) {
            a2->x = l_28.x - (a2->w >> 1);
            a2->y = l_28.y - (a2->h >> 1);
        }
    }
    func_000A006E(a2->handle, l_A8.seek, 0);
    a2->a6 = 1;
    if (a2->buf2 == 0) {
        a2->flags |= 64;
        a2->buf2 = func_000A00AF(a2->w * a2->h, D_00175404, 255);
    }
    return 1;
}
