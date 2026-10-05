/* matched by the real Watcom C32 10.0a (-d2): a run of disease.c from 0x0006630B to 0x00066352, kept together for its switch table's alignment */
#include "records.h"

struct pick {
    char pad0[12];
    struct record *obj;         /* 0x0c */
};
extern char D_00175970[];        /* __FILE__ */
extern unsigned char D_00186DE3[];
extern unsigned char D_001940D8;
extern struct record *player_entity;
extern struct record *D_00195AC4;
extern struct character *player_character;
extern struct career *player_class;
extern unsigned char current_region;
extern unsigned char D_00196294;
extern int region_dungeon_type_counts;
extern struct faction *faction_find_type_in_region(short, int);
extern void func_0001E34D(struct pick *, int, int);
extern void msgbox_show_rsc(int, int);
extern void time_pass(int);
extern void paperdoll_draw(int, int);
extern void item_make(int, int, struct item *);
extern void disease_toggle_memberships_cb(struct record *);
extern void func_00066853(struct record *, unsigned char);
extern int disease_is_lycanthrope(void);
extern void location_free(struct pick *);
extern void map_goto_location(unsigned char, int, unsigned short, int);
extern void spfx_cure_disease(struct record *, struct character *);
extern struct record *object_create_child(struct record *, int, int);
extern void object_foreach(struct record *, void (*)(struct record *));
extern struct record *object_find_item(struct record *, short, short);
extern void inv_store_item(struct record *);
extern int marker_find_nth(struct record *, int, int);
extern void player_to_nearest_marker(struct record *, int);
extern int rand(void);
extern void mc_memcpy(void *, void *, int, char *, int, int);

void disease_toggle_memberships_cb(struct record *a1)
{
    if (a1->type == 10) {
        a1->type = 29;
        return;
    }
    if (a1->type == 29)
        a1->type = 10;
}

void disease_become_vampire(void)
{
    struct pick s;
    int u48;
    struct record *o2;
    int u40;
    struct disease *p;
    int u38;
    int i;
    struct record *o1;
    int u2c;
    int kind;
    int saved;
    int u20;
    struct faction *d;
    int excess;

    if (player_character->race > 7 || disease_is_lycanthrope() != 0)
        return;
    player_character->special_infection_time = 0;
    player_character->special_infection = 0;
    msgbox_show_rsc(401, 1);
    saved = D_00196294;
    D_00196294 = 1;
    time_pass(30240);
    D_00196294 = saved;
    if (region_dungeon_type_counts != 0) {
        func_0001E34D(&s, 0, rand() % region_dungeon_type_counts);
        map_goto_location(current_region, 3, s.obj->image, 0);
        if (marker_find_nth(D_00195AC4, 9, 0) != 0)
            player_to_nearest_marker(D_00195AC4, 9);
        location_free(&s);
    }
    d = faction_find_type_in_region(current_region, 7);
    if (d != 0)
        kind = d->vampire_clan;
    else
        kind = 153;
    D_001940D8 |= 8;
    player_character->flags |= 20;
    o1 = object_create_child(player_entity, 0, 47);
    o1->type = 11;
    o1->flags = 0x8003;
    o2 = object_create_child(player_entity, 0, 74);
    o2->type = 28;
    o2->flags = 3;
    p = &o1->data.disease;
    p->id = 100;
    mc_memcpy(&o2->data.career, player_class, 74, D_00175970, 407, 4);
    for (i = 0; i < 8; i++) {
        if (i == 1)
            continue;
        p->drained[i] = 20;
        player_character->attributes[i] += 20;
        player_character->base_attributes[i] += 20;
        excess = player_character->base_attributes[i] - 100;
        if (excess > 0) {
            player_character->base_attributes[i] -= excess;
            player_character->attributes[i] -= excess;
            p->drained[i] -= excess;
        }
    }
    player_character->skills[3].value += 30;
    player_character->skills[21].value += 30;
    player_character->skills[16].value += 30;
    player_character->skills[34].value += 30;
    player_character->skills[18].value += 30;
    player_character->skills[30].value += 30;
    player_class->immunity_flags |= 0x41;
    player_class->flags |= 0x30;
    spfx_cure_disease(player_entity, player_character);
    object_foreach(player_entity->children, disease_toggle_memberships_cb);
    player_character->original_race = player_character->race;
    player_character->race = 8;
    player_character->min_metal_to_hit = 2;
    o2 = object_find_item(player_entity->children, 27, 0);
    if (o2 == 0) {
        o2 = object_create_child(D_00195AC4, 0, 107);
        o2->type = 2;
        o2->flags |= 1;
        item_make(27, 0, &o2->data.item);
        inv_store_item(o2);
    }
    i = 0;
    while (D_00186DE3[i] != 255)
        func_00066853(o2, D_00186DE3[i++]);
    switch (kind - 150) {
    case 0:
        func_00066853(o2, 85);
        break;
    case 5:
        func_00066853(o2, 50);
        break;
    case 4:
        func_00066853(o2, 10);
        break;
    case 2:
        func_00066853(o2, 64);
        break;
    case 7:
        p->drained[1] += 20;
        player_character->attributes[1] += 20;
        player_character->base_attributes[1] += 20;
        excess = player_character->base_attributes[i] - 100;
        if (excess > 0) {
            player_character->base_attributes[1] -= excess;
            player_character->attributes[1] -= excess;
            p->drained[1] -= excess;
        }
        break;
    case 6:
        func_00066853(o2, 17);
        break;
    case 8:
        func_00066853(o2, 11);
        func_00066853(o2, 12);
        func_00066853(o2, 13);
        break;
    case 3:
        func_00066853(o2, 23);
        func_00066853(o2, 6);
        break;
    case 1:
        func_00066853(o2, 20);
        func_00066853(o2, 33);
        break;
    }
    player_character->vampire_clan = kind;
    paperdoll_draw(0, 0);
}
