/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000804C8 */
struct bits { unsigned char b0:1; unsigned char b1:1; unsigned char b2:1; };
struct rec { char pad[4]; unsigned short w; unsigned short h; char pad2[2]; unsigned short len; };
extern short D_0012AC04;
extern short D_0012AC06;
extern struct bits D_001940D4;
extern struct bits D_001940D5;
extern unsigned char D_00195E7A;
extern char D_00195E7F;
extern char D_00196272;
extern char *D_001A5B18;
extern void func_0008059B(void);
extern void func_00144FB4(int, int, int, int, char *);

void func_000804C8(short a1)
{
    char *p;
    int l_34, l_2C, l_28, l_24, l_20;   /* unused, but they have slots */
    char l_18;

    if (D_001940D5.b2) return;
    if (D_00196272 || D_001940D4.b2) {
        func_0008059B();
        return;
    }
    if (D_00195E7A == 1 && D_00195E7F == 0) return;
    if (D_00195E7A == 1 && D_00195E7F != 0) {
        func_0008059B();
        return;
    }
    p = D_001A5B18;
    while (a1-- != 0)
        p = p + ((struct rec *)p)->len + 12;
    func_00144FB4(D_0012AC04, D_0012AC06, ((struct rec *)p)->w, ((struct rec *)p)->h, p + 12);
}
