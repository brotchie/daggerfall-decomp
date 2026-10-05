/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000461E9 */
#include "records.h"
struct flag4 {
    unsigned char a;
    unsigned char b;
    unsigned short id:10;
    unsigned short f10:1;
    unsigned short kind:3;
};
extern struct record *D_00195AC4;
extern struct flag4 dungeon_blocks[];
extern unsigned char dungeon_block_count;
extern struct record *D_00199720;
extern struct record *func_000461A3(void);
extern void object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern struct record *marker_find_nth(struct record *, int, int);

void func_000461E9(void)
{
    struct record *p;
    int i;

    for (i = 0; i < dungeon_block_count; i++) {
        if (dungeon_blocks[i].a == 0 && dungeon_blocks[i].b == 0)
            if (dungeon_blocks[i].kind == 1 && dungeon_blocks[i].id == 9) {
                D_00199720 = marker_find_nth(D_00195AC4, 8, 0);
                D_00199720 = object_find_by_id(D_00195AC4, D_00199720->id);
                p = func_000461A3();
                if (p != 0)
                    object_delete(p);
                p = object_create_child(D_00199720->parent, 0, 62);
                p->type = 6;
                p->image2 = 703;
                p->image = 0;
                p->id = object_new_id(D_00195AC4->id >> 16);
                p->x = D_00195AC4->x + 664;
                p->y = D_00195AC4->y - 1281;
                p->z = D_00195AC4->z + 2035;
            }
    }
}
