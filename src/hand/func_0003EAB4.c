/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003EAB4 */
struct save { char pad[4]; short slot; char name[8]; };
extern char D_00170D55[];
extern char D_00170DA2[];
extern char D_00170DA7[];
extern short D_00178A08;
extern char D_001903A4[];
extern char D_00190FE4[];
extern char D_00190FEC;
extern int D_00195D6C;
extern int D_00199650;
extern struct save *D_00199764;
extern int func_0003D412(short, short, short);
extern void func_0003DCF4(int, int);
extern void func_0003E942(int);
extern int func_0006CD6E(char *);
extern void func_0009DEA7(int);
extern void func_000A0024(int, char *, int);
extern void func_000A1023(char *, char *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

int func_0003EAB4(struct save *a1, short a2, int a3, short a4)
{
    short saved;
    int h;
    int result;

    saved = D_00195D6C;
    D_00199764 = a1;
    if (a1->slot != 0) {
        func_000A0ED9(657, D_00170D55);
        func_000A0F5C(D_00190FE4, D_00170DA2, a1->slot);
    } else {
        func_000A1023(D_00190FE4, a1->name, 8, D_00170D55, 659, 2048);
    }
    D_00190FEC = 0;
    func_000A0ED9(662, D_00170D55);
    func_000A0F5C(D_001903A4, D_00170DA7, D_00190FE4);
    if ((D_00195D6C = func_0006CD6E(D_001903A4)) > 0) {
        h = func_0003D412(a2, a4 | 0x8002, D_00178A08);
        if (h == 0)
            return 0;
        func_0003DCF4(h, a3);
        if (D_00199650 != 0) {
            func_0003E942(a3);
            result = 0;
        } else {
            result = 1;
        }
        func_0009DEA7(D_00195D6C);
    }
    if (h != 0 && h != 0x97979797) {
        func_000A0024(h, D_00170D55, 685);
        h = 0x97979797;
    }
    D_00195D6C = saved;
    return result;
}
