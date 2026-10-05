/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000845F1 */
#include "records.h"

extern char D_00187D30[];
extern struct record *location_object;
extern struct location *current_location;
extern struct settings *game_settings;
extern signed char current_region;
extern struct flat_cfg *flats_cfg_find(int);
extern struct record *rmb_make_light(struct record *, int, short);
extern struct record *rmb_make_marker(struct record *, int);
extern struct record *object_create_child(struct record *, struct record *, int);

struct record *rmb_make_flat(struct record *parent, short image, short faction_id, int is_flat)
{
    struct flat_cfg *flat_cfg;
    struct record *object;

    if (is_flat != 0) {
        switch (image >> 7) {
        case 199:
            object = rmb_make_marker(parent, image);
            break;
        case 210:
            object = rmb_make_light(parent, faction_id >> 8, faction_id & 255);
            break;
        default:
            object = object_create_child(parent, 0, 0);
            object->type = 33;
            object->pad13 = 8000;
            object->image = image;
            object->id = location_object->id + current_location->object_counter++;
            flat_cfg = flats_cfg_find(image);
            if ((flat_cfg->flags & 2) && (game_settings->view_flags & 4))
                object->image = 0;
            break;
        }
    } else {
        object = object_create_child(parent, 0, 3);
        object->type = 8;
        object->id = location_object->id + current_location->object_counter++;
        object->image = image;
        object->pad13 = 8000;
        flat_cfg = flats_cfg_find(image);
        if ((flat_cfg->flags & 2) && (game_settings->view_flags & 4))
            object->image = 0;
        if (faction_id == 0)
            faction_id = *(short *)(D_00187D30 + (unsigned char)current_region * 2);
        object->data.person.faction_id = faction_id;
    }
    return object;
}
