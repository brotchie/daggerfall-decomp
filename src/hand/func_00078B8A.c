/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00078B8A */
#include "records.h"

extern char D_00176844[];
extern char D_0017685B[];
extern signed char text_buffer[];
extern char D_00190704[];
extern int monster_bsa_handle;
extern int archive_find_record(int, int, int);
extern int archive_read_record(int, int, int);
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);

void monster_reload_anim_cb(struct record *object)
{
    struct character *monster_char;
    struct monster_anim *anim;
    struct career *career;
    int script_offset;
    int record_index;

    if (object->type != 18) return;
    monster_char = &object->data.character;
    career = &monster_char->career;
    anim = (struct monster_anim *)((char *)career + 74);
    script_offset = anim->anim_script_pos - anim->anim_script;
    mc_set_location(196, (int)D_00176844);
    mc_sprintf((int)text_buffer, (int)D_0017685B, monster_char->ascr_record);
    record_index = archive_find_record(monster_bsa_handle, (int)text_buffer, 8);
    ((char **)D_00190704)[monster_char->anim_slot] = anim->anim_script = (char *)archive_read_record(monster_bsa_handle, record_index, 0);
    if (anim->anim_script_pos == 0) return;
    anim->anim_script_pos = anim->anim_script + script_offset;
}
