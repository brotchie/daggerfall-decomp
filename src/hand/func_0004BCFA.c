/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004BCFA */
#include "ptrint.h"
#include <dos.h>
extern char D_00174F47[];        /* __FILE__ */
extern char D_00174F50[];
extern char D_00174F57[];
extern signed char text_buffer[];
extern char arena2_path[];
extern char arena2_cd_path[];
extern iptr quest_start(char *);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void quests_start_initial(void)
{
    struct find_t find_data;
    int status;

    mc_set_location(142, D_00174F47);
    mc_sprintf(((char *)text_buffer), D_00174F57, arena2_path, D_00174F50);
    status = func_000A13DA(((char *)text_buffer), 0, &find_data);
    if (status != 0) {
        mc_set_location(146, D_00174F47);
        mc_sprintf(((char *)text_buffer), D_00174F57, arena2_cd_path, D_00174F50);
        status = func_000A13DA(((char *)text_buffer), 0, &find_data);
    }
    while (status == 0) {
        quest_start(find_data.name);
        status = func_000A13F7(&find_data);
    }
}
