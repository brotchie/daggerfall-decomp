/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000844FB */
#include "records.h"

extern struct record *location_object;
extern struct location *current_location;
extern struct record *object_create_child(struct record *, struct record *, int);

struct record *rmb_make_marker(struct record *parent, int image)
{
    struct record *marker;
    int kind;
    int data_size;

    kind = (image & 31) - 2;
    if (kind == 13 || kind == 14) {
        data_size = REC_SIZEOF(struct monster);
        marker = object_create_child(parent, 0, data_size);
        marker->mobile_id = 0;
    } else {
        marker = object_create_child(parent, 0, 0);
        marker->mobile_id = 0;
    }
    marker->type = 34;
    marker->image2 = 0;
    marker->mobile_id = 0;
    marker->image = image;
    if (kind == 9 || kind == 16) {
        marker->id = location_object->id + current_location->marker_counter++;
    } else {
        marker->id = location_object->id + current_location->object_counter++;
    }
    marker->flags = 1;
    return marker;
}
