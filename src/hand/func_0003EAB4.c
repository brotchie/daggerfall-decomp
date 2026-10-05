/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003EAB4 */
#include "records.h"

extern char D_00170D55[];
extern char D_00170DA2[];
extern char D_00170DA7[];
extern short msgbox_wrap_width;
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern char D_00190FEC;
extern int text_rsc_file;
extern int msgbox_next_page;
extern struct quest *current_quest;
extern int text_rsc_load(short, short, short);
extern void msgbox_render(int, int);
extern void msgbox_show_more_pages(int);
extern int disk_open_data(char *);
extern void close(int);
extern void mc_free(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

int msgbox_render_quest_text(struct quest *a1, short a2, int a3, short a4)
{
    short saved;
    int h;
    int result;

    saved = text_rsc_file;
    current_quest = a1;
    if (a1->text_file != 0) {
        mc_set_location(657, D_00170D55);
        mc_sprintf(((char *)text_rsc_buffer), D_00170DA2, a1->text_file);
    } else {
        mc_memcpy(((char *)text_rsc_buffer), a1->name, 8, D_00170D55, 659, 2048);
    }
    D_00190FEC = 0;
    mc_set_location(662, D_00170D55);
    mc_sprintf(((char *)text_buffer), D_00170DA7, ((char *)text_rsc_buffer));
    if ((text_rsc_file = disk_open_data(((char *)text_buffer))) > 0) {
        h = text_rsc_load(a2, a4 | 0x8002, msgbox_wrap_width);
        if (h == 0)
            return 0;
        msgbox_render(h, a3);
        if (msgbox_next_page != 0) {
            msgbox_show_more_pages(a3);
            result = 0;
        } else {
            result = 1;
        }
        close(text_rsc_file);
    }
    if (h != 0 && h != 0x97979797) {
        mc_free(h, D_00170D55, 685);
        h = 0x97979797;
    }
    text_rsc_file = saved;
    return result;
}
