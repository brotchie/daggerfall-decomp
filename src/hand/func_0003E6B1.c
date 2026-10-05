/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E6B1 */
struct save { char pad[4]; short num; char name[8]; };
extern char D_00170D55[];       /* __FILE__ */
extern char D_00170DA2[];
extern char D_00170DA7[];
extern char text_buffer[];
extern char text_rsc_buffer[];
extern char D_00190FEC;
extern int D_00195D6C;
extern struct save *current_quest;
extern int text_rsc_load(int, int, int);
extern int disk_open_data(char *);
extern void func_0009DEA7(int);
extern void func_000A1023(char *, char *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

int func_0003E6B1(struct save *s, short a2, short a3, short a4)
{
    short saved;
    int res;

    saved = D_00195D6C;
    current_quest = s;
    if (s == 0)
        return 0;
    if (s->num != 0) {
        func_000A0ED9(545, D_00170D55);
        func_000A0F5C(text_rsc_buffer, D_00170DA2, s->num);
    } else {
        func_000A1023(text_rsc_buffer, s->name, 8, D_00170D55, 547, 2048);
    }
    D_00190FEC = 0;
    func_000A0ED9(550, D_00170D55);
    func_000A0F5C(text_buffer, D_00170DA7, text_rsc_buffer);
    if ((D_00195D6C = disk_open_data(text_buffer)) > 0) {
        res = text_rsc_load(a2, 0, a4);
        func_0009DEA7(D_00195D6C);
        D_00195D6C = saved;
        return res;
    }
    D_00195D6C = saved;
    return 0;
}
