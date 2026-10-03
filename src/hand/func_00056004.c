/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00056004 */
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_0012AC00[];
extern char D_0012AC02[];
extern char D_0012AC04[];
extern char D_0012AC06[];
extern char D_0012B508[];
extern char D_00142309[];
extern char D_00185AF4[];
extern char D_001940D4[];
extern char D_00195AA4[];
extern char D_00195B7C[];
extern char D_00195BE8[];
extern char D_00195C44[];
extern char D_00196279[];
extern int func_000392AD(void);
extern int func_00055E35(int);
extern int func_000561D8(void);
extern void func_000562A1(void);
extern int func_00069938(int, int, int);
extern int func_000CB552();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00135E90();

struct R { short x0, y0, x1, y1; void (*fn)(int); };
#define TAB ((struct R *)D_00185AF4)
#define FLAG (((struct bf8_2_1 *)D_001940D4)->f)
#define MX (*(short *)D_0012AC04)
#define MY (*(short *)D_0012AC06)

void func_00056004(void)
{
    short l_24;
    short l_20;
    short l_1C;
    short l_18;

    if (func_00055E35(0) == 0) return;
    func_00135E90();
    *(signed char *)D_0012B508 = 146;
    func_000CB552(*(int *)D_00195BE8);
    func_0012DB50(4);
    func_000562A1();
    func_0012DB50(3);
    l_20 = 0;
    if (FLAG && (l_20 = func_000392AD()) > -1) {
        while (*D_00142309 != 0) ;
        while (*D_0012AC00 != 0) func_0012B136();
        *D_0012AC02 = 0;
        (*(void (**)(int))D_00195B7C)((*(unsigned char **)D_00195C44)[l_20 + 64000]);
        while (*D_0012AC00 != 0) func_0012B136();
        *D_0012AC02 = 0;
        return;
    }
    if (l_20 != -2 && *D_00142309 != 0) {
        func_000561D8();
    } else if (l_20 == -2) {
        while (*D_00142309 != 0) ;
    }
    if (FLAG || (*D_0012AC00 == 0 || (*D_0012AC00 != 0 && *D_00196279 != 0))) return;
    for (l_20 = 0; l_20 < 20; l_20++) {
        if (MX > TAB[l_20].x0 && MX < TAB[l_20].x1 && MY > TAB[l_20].y0 && MY < TAB[l_20].y1) {
            func_00069938(203, *(int *)D_00195AA4, 100);
            TAB[l_20].fn(l_20);
        }
    }
}
