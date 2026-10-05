/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001C713 */
#include "records.h"

extern char D_00170464[];       /* __FILE__ */
extern struct record *player_entity;
extern int D_001966FC[];
extern struct record *object_create_child(struct record *, int, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);

/* a biography person: a type 45+kind record holding a character record and its class */
unsigned short bio_person_add(struct character *name, struct career *text, int kind)
{
    struct record *o;

    if (D_001966FC[kind] == 8)
        return 0xffff;
    o = object_create_child(player_entity, 0, 634);
    o->type = kind + 45;
    o->flags |= 3;
    o->image = D_001966FC[kind]++;
    mc_memcpy(&o->data.character, name, 560, D_00170464, 1334, 4);
    mc_memcpy(&o->data.character.career, text, 74, D_00170464, 1335, 4);
    return o->image;
}
