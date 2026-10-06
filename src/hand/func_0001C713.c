/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001C713 */
#include "records.h"
#include "clib.h"

extern char D_00170464[];       /* __FILE__ */
extern struct record *player_entity;
extern int D_001966FC[];
extern struct record *object_create_child(struct record *, struct record *, int);

/* a biography person: a type 45+kind record holding a character record and its class */
unsigned short bio_person_add(struct character *person, char *career, int kind)
{
    struct record *object;

    if (D_001966FC[kind] == 8)
        return 0xffff;
    object = object_create_child(player_entity, 0, 634);
    object->type = kind + 45;
    object->flags |= 3;
    object->image = D_001966FC[kind]++;
    mc_memcpy(&object->data.character, person, 560, D_00170464, 1334, 4);
    mc_memcpy(&object->data.character.career, career, 74, D_00170464, 1335, 4);
    return object->image;
}
