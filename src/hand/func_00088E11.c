/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E11 */
#include "records.h"
extern char D_001832C4[];
extern char D_001940D8[];
extern char game_minutes[];
extern char D_001A99F4[];
extern void spfx_create_item_cb(int);
extern void spfx_show_choice_list(int, int);

void spfx_create_item(struct record *a1, int a2, int a3)
{
    *(signed char *)D_001940D8 &= 254;
    *(int *)D_001A99F4 = a1->data.spell.cast_durations[a2] + *(int *)game_minutes;
    spfx_show_choice_list((int)D_001832C4, (int)spfx_create_item_cb);
}
