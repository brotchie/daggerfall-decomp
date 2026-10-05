/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E6B1 */
#include "records.h"

extern char D_00170D55[];       /* __FILE__ */
extern char D_00170DA2[];
extern char D_00170DA7[];
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern char D_00190FEC;
extern int text_rsc_file;
extern struct quest *current_quest;
extern int text_rsc_load(int, int, int);
extern int disk_open_data(char *);
extern void close(int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

int text_qrc_load_for_quest(struct quest *s, short a2, short a3, short a4)
{
    short saved;
    int res;

    saved = text_rsc_file;
    current_quest = s;
    if (s == 0)
        return 0;
    if (s->text_file != 0) {
        mc_set_location(545, D_00170D55);
        mc_sprintf(((char *)text_rsc_buffer), D_00170DA2, s->text_file);
    } else {
        mc_memcpy(((char *)text_rsc_buffer), s->name, 8, D_00170D55, 547, 2048);
    }
    D_00190FEC = 0;
    mc_set_location(550, D_00170D55);
    mc_sprintf(((char *)text_buffer), D_00170DA7, ((char *)text_rsc_buffer));
    if ((text_rsc_file = disk_open_data(((char *)text_buffer))) > 0) {
        res = text_rsc_load(a2, 0, a4);
        close(text_rsc_file);
        text_rsc_file = saved;
        return res;
    }
    text_rsc_file = saved;
    return 0;
}
