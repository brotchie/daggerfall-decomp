/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000845F1 */
#include "records.h"

extern char D_00187D30[];
extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct settings *game_settings;
extern signed char current_region;
extern char *flats_cfg_find(int);
extern struct record *rmb_make_light(struct record *, int, short);
extern struct record *rmb_make_marker(struct record *, int);
extern struct record *object_create_child(struct record *, struct record *, int);

struct record *rmb_make_flat(struct record *a1, short a2, short a3, int a4)
{
    char *l_20;
    struct record *l_1C;

    if (a4 != 0) {
        switch (a2 >> 7) {
        case 199:
            l_1C = rmb_make_marker(a1, a2);
            break;
        case 210:
            l_1C = rmb_make_light(a1, a3 >> 8, a3 & 255);
            break;
        default:
            l_1C = object_create_child(a1, 0, 0);
            l_1C->type = 33;
            l_1C->pad13 = 8000;
            l_1C->image = a2;
            l_1C->id = D_00195AC4->id + current_location->object_counter++;
            l_20 = flats_cfg_find(a2);
            if ((l_20[6] & 2) && (game_settings->view_flags & 4))
                l_1C->image = 0;
            break;
        }
    } else {
        l_1C = object_create_child(a1, 0, 3);
        l_1C->type = 8;
        l_1C->id = D_00195AC4->id + current_location->object_counter++;
        l_1C->image = a2;
        l_1C->pad13 = 8000;
        l_20 = flats_cfg_find(a2);
        if ((l_20[6] & 2) && (game_settings->view_flags & 4))
            l_1C->image = 0;
        if (a3 == 0)
            a3 = *(short *)(D_00187D30 + (unsigned char)current_region * 2);
        l_1C->data.person.faction_id = a3;
    }
    return l_1C;
}
