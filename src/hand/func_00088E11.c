/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E11 */
#include "records.h"
extern char *D_001832C4[];
extern signed char D_001940D8;
extern int game_minutes;
extern int D_001A99F4;
extern void spfx_create_item_cb(int);
extern void spfx_show_choice_list(iptr, iptr);

void spfx_create_item(struct record *spell_object, int effect, int target)
{
    D_001940D8 &= 254;
    D_001A99F4 = spell_object->data.spell.cast_durations[effect] + game_minutes;
    spfx_show_choice_list((iptr)(char *)D_001832C4, (iptr)spfx_create_item_cb);
}
