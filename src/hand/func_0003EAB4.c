/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003EAB4 */
struct save { char pad[4]; short slot; char name[8]; };
extern char D_00170D55[];
extern char D_00170DA2[];
extern char D_00170DA7[];
extern short D_00178A08;
extern char text_buffer[];
extern char text_rsc_buffer[];
extern char D_00190FEC;
extern int D_00195D6C;
extern int msgbox_next_page;
extern struct save *current_quest;
extern int text_rsc_load(short, short, short);
extern void msgbox_render(int, int);
extern void msgbox_show_more_pages(int);
extern int disk_open_data(char *);
extern void func_0009DEA7(int);
extern void mc_free(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

int msgbox_render_quest_text(struct save *a1, short a2, int a3, short a4)
{
    short saved;
    int h;
    int result;

    saved = D_00195D6C;
    current_quest = a1;
    if (a1->slot != 0) {
        func_000A0ED9(657, D_00170D55);
        mc_sprintf(text_rsc_buffer, D_00170DA2, a1->slot);
    } else {
        mc_memcpy(text_rsc_buffer, a1->name, 8, D_00170D55, 659, 2048);
    }
    D_00190FEC = 0;
    func_000A0ED9(662, D_00170D55);
    mc_sprintf(text_buffer, D_00170DA7, text_rsc_buffer);
    if ((D_00195D6C = disk_open_data(text_buffer)) > 0) {
        h = text_rsc_load(a2, a4 | 0x8002, D_00178A08);
        if (h == 0)
            return 0;
        msgbox_render(h, a3);
        if (msgbox_next_page != 0) {
            msgbox_show_more_pages(a3);
            result = 0;
        } else {
            result = 1;
        }
        func_0009DEA7(D_00195D6C);
    }
    if (h != 0 && h != 0x97979797) {
        mc_free(h, D_00170D55, 685);
        h = 0x97979797;
    }
    D_00195D6C = saved;
    return result;
}
