/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00078B8A */
#include "records.h"

extern char D_00176844[];
extern char D_0017685B[];
extern signed char text_buffer[];
extern char D_00190704[];
extern int D_00195AC8;
extern int archive_find_record(int, int, int);
extern int archive_read_record(int, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);

void monster_reload_anim_cb(struct record *a1)
{
    struct character *l_28;
    struct monster_anim *l_24;
    struct career *l_20;
    int l_1C;
    int l_18;

    if (a1->type != 18) return;
    l_28 = &a1->data.character;
    l_20 = &l_28->career;
    l_24 = (struct monster_anim *)((char *)l_20 + 74);
    l_1C = l_24->anim_script_pos - l_24->anim_script;
    func_000A0ED9(196, (int)D_00176844);
    mc_sprintf((int)text_buffer, (int)D_0017685B, l_28->ascr_record);
    l_18 = archive_find_record(D_00195AC8, (int)text_buffer, 8);
    ((char **)D_00190704)[l_28->anim_slot] = l_24->anim_script = (char *)archive_read_record(D_00195AC8, l_18, 0);
    if (l_24->anim_script_pos == 0) return;
    l_24->anim_script_pos = l_24->anim_script + l_1C;
}
