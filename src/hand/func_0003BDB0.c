/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003BDB0 */
struct ch { char pad[16]; unsigned char attr[8]; };
struct mob { char pad[337]; short level; };
extern char D_00170C67[];
extern char D_00170CC0[];
extern short D_00178A08;
extern char D_001903A4[];
extern char D_001903A5;
extern struct mob *D_00195BE0;
extern int D_00195BE8;
extern struct ch *D_00195BEC;
extern char *D_00195C44;
extern char D_00199644;
extern void func_0003B6CF(void);
extern void func_0003C010(char *, short, int);
extern int func_0003C3A8(unsigned char);
extern void func_0003EC2A(char *, int);
extern void func_00058E15(int, int);
extern int func_000A0DF4(char *);
extern void func_000A1054(char *, char *, char *, int, int);
extern void func_000CB552(int);
extern void func_000CDD81(int);
extern void func_0012DB50(int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_0003BDB0(short a1, short a2)
{
    char *p;
    short x;

    D_00199644 = 0;
    p = D_00195C44 + 55000;
    D_00178A08 = 250;
    *p = 0;
    func_0003C010(p, D_00195BEC->attr[a1], func_0003C3A8(D_00195BEC->attr[a1]));
    func_0003C010(p, D_00195BEC->attr[a1 + 1], func_0003C3A8(D_00195BEC->attr[a1 + 1]));
    func_0003C010(p, D_00195BEC->attr[a1 + 2], func_0003C3A8(D_00195BEC->attr[a1 + 2]));
    if (a2) {
        func_0003C010(p, D_00195BEC->attr[a1 + 3], func_0003C3A8(D_00195BEC->attr[a1 + 3]));
        func_0003C010(p, (int)D_00195BEC->attr[a1 + 4], func_0003C3A8(D_00195BEC->attr[a1 + 4]));
        func_0003C010(p, D_00195BEC->attr[a1 + 5], func_0003C3A8(D_00195BEC->attr[a1 + 5]));
    }
    if (D_00199644) {
        func_000A0ED9(327, D_00170C67);
        func_000A0F5C(D_001903A4, D_00170CC0, D_00195BE0->level / 10 + 1, D_00195BE0->level / 5 + 1);
        D_001903A5 = 96;
        func_000A1054(p, D_001903A4, D_00170C67, 329, 4);
    }
    p[func_000A0DF4(p) - 1] = 0;
    func_000CB552(D_00195BE8);
    func_00058E15(0, 0);
    func_0012DB50(4);
    func_0003B6CF();
    func_000CDD81(1);
    func_0003EC2A(p, 1);
    D_00178A08 = 310;
}
