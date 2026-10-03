/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CA9A */
#pragma pack(1)
struct win {
    char kind;
    char c1, c2, c3, c4;
    short a2, a3, a4;
    short a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17;
    short z37, z39, z41, z43, last;
    char *buf0;
    char *buf1;
    char *buf2;
};
#pragma pack()
extern char D_00176E38[];
extern void func_0008CA25(struct win *);
extern char *func_000A00AF(int, char *, int);

void func_0008CA9A(struct win *a1, short a2, short a3, short a4, short a5, short a6, short a7, short a8, short a9, short a10, short a11, short a12, short a13, short a14, short a15, short a16, short a17, unsigned char a18, unsigned char a19, unsigned char a20, unsigned char a21, unsigned char a22)
{
    a1->kind = a22;
    a1->c1 = a18;
    a1->c2 = a19;
    a1->c3 = a20;
    a1->c4 = a21;
    a1->a2 = a2;
    a1->a3 = a3;
    a1->a4 = a4;
    a1->a5 = a5;
    a1->a6 = a6;
    a1->a7 = a7;
    a1->a8 = a8;
    a1->a9 = a9;
    a1->a10 = a10;
    a1->a11 = a11;
    a1->a12 = a12;
    a1->a13 = a13;
    a1->a14 = a14;
    a1->a15 = a15;
    a1->a16 = a16;
    a1->a17 = a17;
    a1->z37 = 0;
    a1->z39 = 0;
    a1->z41 = 0;
    a1->last = a1->a17 - 1;
    a1->z43 = 0;
    a1->buf0 = func_000A00AF(35200, D_00176E38, 40);
    if (a1->kind == 0) return;
    a1->buf1 = func_000A00AF(a1->a4 * a1->a5, D_00176E38, 44);
    a1->buf2 = func_000A00AF(a1->a16 * a1->a17, D_00176E38, 45);
    func_0008CA25(a1);
}
