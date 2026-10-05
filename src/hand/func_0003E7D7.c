/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E7D7 */
extern char D_00170D55[];
extern char D_00170DA7[];
extern char text_buffer[];
extern char D_001910E4[];
extern char D_001910EC[];
extern int D_00195D6C;
extern int text_rsc_load(int, int, int);
extern int disk_open_data(char *);
extern void func_0009DEA7(int);
extern void func_000A1023(char *, char *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, ...);

int text_qrc_load(char *a1, short a2, short a3, short a4)
{
    short saved;
    int h;

    saved = D_00195D6C;
    func_000A1023(D_001910E4, a1, 8, D_00170D55, 568, 2048);
    *D_001910EC = 0;
    func_000A0ED9(570, D_00170D55);
    func_000A0F5C(text_buffer, D_00170DA7, D_001910E4);
    if ((D_00195D6C = disk_open_data(text_buffer)) > 0) {
        h = text_rsc_load(a2, 0, a4);
        func_0009DEA7(D_00195D6C);
        D_00195D6C = saved;
        return h;
    }
    D_00195D6C = saved;
    return 0;
}
