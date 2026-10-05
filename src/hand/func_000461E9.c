/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000461E9 */
#include "records.h"
struct flag4 {
    unsigned char a;
    unsigned char b;
    unsigned short id:10;
    unsigned short f10:1;
    unsigned short kind:3;
};
extern struct record *location_object;
extern struct flag4 dungeon_blocks[];
extern unsigned char dungeon_block_count;
extern struct record *D_00199720;
extern struct record *kludge_find_door(void);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern struct record *marker_find_nth(struct record *, int, int);

void kludge_fix_dungeon_door(void)
{
    struct record *object;
    int i;

    for (i = 0; i < dungeon_block_count; i++) {
        if (dungeon_blocks[i].a == 0 && dungeon_blocks[i].b == 0)
            if (dungeon_blocks[i].kind == 1 && dungeon_blocks[i].id == 9) {
                D_00199720 = marker_find_nth(location_object, 8, 0);
                D_00199720 = object_find_by_id(location_object, D_00199720->id);
                object = kludge_find_door();
                if (object != 0)
                    object_delete(object);
                object = object_create_child(D_00199720->parent, 0, 62);
                object->type = 6;
                object->image2 = 703;
                object->image = 0;
                object->id = object_new_id(location_object->id >> 16);
                object->x = location_object->x + 664;
                object->y = location_object->y - 1281;
                object->z = location_object->z + 2035;
            }
    }
}
