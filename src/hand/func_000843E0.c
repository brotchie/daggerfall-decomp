/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000843E0 */
#include "records.h"

extern struct record *location_object;
extern struct location *current_location;
extern struct record *object_create_child(struct record *, struct record *, int);

struct record *rmb_make_door(struct record *parent, short image2, short image, int is_door)
{
    struct record *object;

    object = object_create_child(parent, 0, MODEL_INSTANCE_DATA_SIZE);
    object->type = is_door ? 32 : 6;
    object->lock_level = 0;
    object->image2 = image2;
    object->image = image;
    object->pad13 = 8000;
    object->id = location_object->id + current_location->object_counter++;
    return object;
}
