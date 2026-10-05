/* qinit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170A64[];
extern char D_00178A10[];
extern char D_00185F88[];
extern char region_price_adjustment[];
extern signed char D_001940D5;
extern char D_00195984[];
extern struct record *nonworld_root;
extern struct record *D_00195A00;
extern struct record *camera_object;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct character *player_character;
extern int game_minutes;
extern struct record *D_00195D00;
extern int qbn_opcode_arg_counts;
extern signed char current_region;
extern signed char D_00196299;
extern signed char D_001962A3;
extern int faction_count;
extern struct faction *factions;
extern signed char D_001970DC;
extern struct quest *current_quest;
extern struct quest *D_00199780;
extern short qbn_record_sizes[];
extern struct membership *guild_membership;

extern struct faction *faction_find(short);
extern int faction_player_related(struct faction *);
extern int quest_section(struct quest *, int);
extern int quest_record(struct quest *, int, int);
extern struct record *func_000310E1(struct record *, struct record *);
extern int quest_init_person(struct qbn_person *);
extern int quest_init_place(struct qbn_place *);
extern int spawn_find_point(struct record *, int, int);
extern int rand_range(int, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern int rand();
extern int mc_strncpy();
extern int mc_memcpy();
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);
extern void monster_init(struct record *, int);
extern void monster_init_gear(struct record *);
extern void map_goto_location(int, int, int, int);
struct record *quest_init_item(struct qbn_item *);
struct record *quest_init_foe(struct qbn_foe *);
struct record *quest_record_object(int, int);
int quest_place_object(struct record *, struct qbn_place *);

struct record *quest_init_item(struct qbn_item *a1)
{
    int l_34;
    struct record *l_30;
    struct item *l_2C;
    int l_28;
    int l_24;
    int l_20;
    struct faction *l_1C;

    l_34 = (int)D_00195AC4 + 71;
    l_1C = 0;
    if ((a1->flags & 2) != 0) {
        if (a1->index == (-1)) {
            if (current_quest->faction_id != 0) {
                l_1C = faction_find(current_quest->faction_id);
                if (l_1C != 0 && faction_player_related(l_1C) != 0) {
                    l_24 = guild_membership->rank + 1;
                } else {
                    l_24 = (player_character->level / 2) + 1;
                }
            } else {
                l_24 = (player_character->level / 2) + 1;
            }
            if (l_24 > 10) l_24 = 10;
            if (l_1C != 0) {
                l_20 = l_1C->power;
            } else {
                l_20 = 50;
            }
            l_28 = ((l_20 + 50) * ((((int)&*(signed char *)((char *)(((int)(unsigned short)*(short *)(region_price_adjustment + (((int)(unsigned char)current_region) * 80))) / 2) + 500)) * rand_range(l_24 * 150, l_24 * 200)) / 1000)) / 100;
        } else {
            l_28 = rand_range(a1->index, a1->group);
        }
        if (l_28 < 1) l_28 = 1;
        l_30 = object_create_child(nonworld_root, 0, 107);
        l_30->type = 2;
        l_30->image2 = *(short *)D_00178A10;
        l_30->flags = 0;
        l_30->quest_id = (signed char)current_quest->id;
        l_30->id = object_new_id(700);
        a1->object = l_30;
        l_2C = &l_30->data.item;
        item_make(28, 0, l_2C);
        l_2C->value = l_28;
        l_30->image = l_2C->dropped_image;
    } else {
        if (a1->group < 0) {
            do {
                a1->group = rand() % 28;
            } while (*(int *)(D_00185F88 + (a1->group << 2)) == 0);
        }
        l_30 = object_create_child(nonworld_root, 0, 107);
        l_30->type = 2;
        l_30->image2 = *(short *)D_00178A10;
        l_30->flags = 0;
        l_30->quest_id = (signed char)current_quest->id;
        l_30->id = object_new_id(700);
        a1->object = l_30;
        l_2C = &l_30->data.item;
        l_2C->message = 0;
        if (a1->index >= 0) {
            item_make((int)(unsigned short)a1->group, a1->index, l_2C);
        } else {
            item_make_random((int)(unsigned short)a1->group, l_2C);
            a1->index = l_2C->index;
        }
        l_30->image = l_2C->dropped_image;
        if (l_2C->group == 9 && l_2C->index == 5 && a1->messages[1] != 0) {
            l_2C->message = a1->messages[1];
            mc_strncpy(&l_2C->name[10], (int)(signed char *)&current_quest->name[0], 4, (int)D_00170A64, 572);
        }
    }
    return l_30;
}

struct record *quest_init_foe(struct qbn_foe *a1)
{
    struct record *l_24;
    int l_20;
    struct character *l_1C;

    l_24 = object_create_child(nonworld_root, 0, 659);
    l_20 = (int)D_00195AC4 + 71;
    l_24->type = 18;
    l_24->flags |= 1;
    l_24->repair_due = rand();
    l_24->id = object_new_id(700);
    a1->object = l_24;
    l_24->image2 = (*(int *)D_00178A10)++;
    l_24->quest_id = (signed char)current_quest->id;
    monster_init(l_24, a1->type);
    l_1C = &l_24->data.character;
    l_1C->team = 1;
    return l_24;
}

int quest_init_resources(struct quest *a1)
{
    struct qbn_place *l_50;
    struct qbn_person *l_4C;
    struct qbn_item *l_48;
    struct qbn_foe *l_44;
    struct qbn_op *l_40;
    struct qbn_arg *l_3C;
    int l_38;
    struct qbn_text_var *l_34;
    int l_30;
    int l_2C;
    int l_28;
    short l_1C;
    int l_24;
    short l_18;

    l_2C = 0;
    current_quest = a1;
    l_4C = (struct qbn_person *)((char *)a1 + a1->section_offsets[3]);
    D_001970DC = 0;
    for (l_30 = 0; a1->section_counts[3] > l_30; l_30++, l_4C++) {
        l_4C->object = 0;
        if (quest_init_person(l_4C) == 0) return 0;
    }
    while (D_001970DC != 0) {
        l_4C = (struct qbn_person *)((char *)a1 + a1->section_offsets[3]);
        D_001970DC = 0;
        for (l_30 = 0; a1->section_counts[3] > l_30; l_30++, l_4C++) {
            if (l_4C->object != 0) continue;
            if (quest_init_person(l_4C) == 0) return 0;
        }
        if (l_2C++ > 20) return 0;
    }
    l_50 = (struct qbn_place *)((char *)a1 + a1->section_offsets[4]);
    for (l_30 = 0; a1->section_counts[4] > l_30; l_30++, l_50++) {
        l_50->object = 0;
        if (quest_init_place(l_50) == 0) return 0;
    }
    l_48 = (struct qbn_item *)((char *)a1 + a1->section_offsets[0]);
    for (l_30 = 0; a1->section_counts[0] > l_30; l_30++, l_48++) {
        if ((l_48->flags & 2) == 0 && l_48->group == 100) {
            mc_memcpy(l_48, *(int *)(D_00195984 + (l_48->index << 2)), 19, (int)D_00170A64, 660, 4);
        } else {
            l_48->object = 0;
            if (quest_init_item(l_48) == 0) return 0;
        }
    }
    l_44 = (struct qbn_foe *)((char *)a1 + a1->section_offsets[7]);
    for (l_30 = 0; a1->section_counts[7] > l_30; l_30++, l_44++) {
        l_44->object = 0;
        if (quest_init_foe(l_44) == 0) return 0;
    }
    l_40 = (struct qbn_op *)((char *)a1 + a1->section_offsets[8]);
    for (l_30 = 0; a1->section_counts[8] > l_30; l_30++, l_40++) {
        l_3C = l_40->args;
        l_40->last_minutes = game_minutes;
        l_40->arg_count = ((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)&qbn_opcode_arg_counts + l_40->opcode))) - 48;
        *(int *)&l_1C = 0;
        for (; l_40->arg_count > *(int *)&l_1C; (*(int *)&l_1C)++, l_3C++) {
            if ((int)l_3C->record != 305419896) {
                if (l_3C->value != (-1) && l_3C->value != (-2)) {
                    l_28 = (int)l_3C->record & 255;
                    l_24 = (int)l_3C->record >> 8;
                    l_38 = (int)a1 + a1->section_offsets[l_24];
                    l_38 += ((int)(short)qbn_record_sizes[l_24]) * l_28;
                    l_3C->record = (char *)l_38;
                    l_3C->object = quest_record_object(l_24, l_38);
                } else {
                    l_3C->record = 0;
                    l_3C->object = 0;
                }
            } else {
                l_3C->record = 0;
                l_3C->object = 0;
            }
        }
    }
    if (a1->text_offset != 0) {
        l_34 = (struct qbn_text_var *)((int)a1 + a1->text_offset);
        while (l_34->name[0] != 0) {
            l_34->record = (char *)quest_record(a1, (int)(short)((unsigned short)l_34->section), l_34->index);
            l_34++;
        }
    }
    return 1;
}

struct record *quest_record_object(int a1, int a2)
{
    switch ((unsigned)a1) {
    case 3:
        return ((struct qbn_person *)a2)->object;
    case 0:
        return ((struct qbn_item *)a2)->object;
    case 4:
        return ((struct qbn_place *)a2)->object;
    case 7:
        return ((struct qbn_foe *)a2)->object;
    }
    return 0;
}

void qaction_place_foe(struct qbn_op *a1, struct qbn_place *a2)
{
    struct record *l_14;

    l_14 = a1->args[1].object;
    if (quest_place_object(l_14, a2) == 0) return;
    if (a2 == 0) D_00196299 = 1;
    monster_init_gear(l_14);
}

int func_000337AD(int a1, struct qbn_place *a2, struct building *a3)
{
    if (a2->p2 > (-1)) {
        if (a2->p2 >= 17 && a2->p2 <= 20 && a3->type >= 17 && a3->type <= 20) {
            if (a2->p3 == (-1)) return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 20480);
            if (a2->p3 != 1) {
                return (((((int)(unsigned short)(*(short *)((char *)a1 + 2) & 16384)) != 0) && (((int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096)) == 0)) ? 1 : 0);
            }
            return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096);
        }
    }
    if (a2->p2 > (-1)) {
        if (a3->type == 11 && (short)(a3->type) == a2->p2 && a3->faction_id == 40) {
        } else {
            if (a2->p2 == 11) return 0;
            if ((short)(a3->type) != a2->p2) return 0;
        }
    }
    if (a2->p3 == (-1)) return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 20480);
    if (a2->p3 != 1) {
        return (((((int)(unsigned short)(*(short *)((char *)a1 + 2) & 16384)) != 0) && (((int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096)) == 0)) ? 1 : 0);
    }
    return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096);
}

int quest_place_object(struct record *a1, struct qbn_place *a2)
{
    struct record *l_1C;
    short l_14;

    if (a2 == 0) {
        if (spawn_find_point(a1, 512, 1024) != 0) {
            l_1C = object_create_child(player_object->parent, 0, 0);
            l_1C->x = a1->x;
            l_1C->y = a1->y;
            l_1C->z = a1->z;
            func_000310E1(a1, l_1C);
            object_free_single(l_1C);
            return a1->id;
        }
        return 0;
    }
    if (a1->twin != 0) object_delete(a1->twin);
    l_1C = object_find_by_id(nonworld_root, a2->object->id);
    if (l_1C == 0) return 0;
    object_reparent(l_1C, a1);
    a1->id = object_new_id(((unsigned)l_1C->id) >> 16);
    l_1C = a1->children;
    while (l_1C != 0) {
        l_1C->id = object_new_id(((unsigned)l_1C->parent->id) >> 16);
        l_1C = l_1C->next;
    }
    if (a1->type != 2 && a1->type != 18) {
        l_14 = a1->data.building.faction_id;
        mc_memcpy(&a1->data, &a2->object->data, 26, (int)D_00170A64, 924, 4);
        a1->data.building.faction_id = *(int *)&l_14;
    }
    if ((((unsigned)a1->id) >> 16) == (((unsigned)D_00195AC4->id) >> 16)) a1 = func_000310E1(a1, 0);
    return a1->id;
}

void qaction_place_item(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_14;

    l_14 = a2->args[1].object;
    if (l_14->twin != 0) {
        object_delete(l_14->twin);
        l_14->twin = 0;
    }
    quest_place_object(l_14, (struct qbn_place *)a2->args[2].record);
}

void qaction_place_npc(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct record *l_1C;
    int l_18;
    int l_14;

    if (a2->args[1].value == (-1)) {
        l_1C = a2->args[2].object;
        map_goto_location((int)(unsigned char)current_region, 3, l_1C->image, l_1C->image2);
        l_1C = a2->args[2].object->twin;
        if (l_1C != 0) {
            player_object->x = l_1C->x;
            player_object->y = l_1C->y;
            player_object->z = l_1C->z;
            player_object->yaw = camera_object->yaw;
            D_001940D5 |= 2;
        }
        return;
    }
    l_1C = a2->args[1].object;
    if (l_1C->type == 65) return;
    if (l_1C->twin != 0) object_delete(l_1C->twin);
    l_1C->twin = 0;
    quest_place_object(l_1C, (struct qbn_place *)a2->args[2].record);
}

void qaction_give_item_to_foe(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_1C;
    struct record *l_18;
    struct record *l_14;

    l_1C = a2->args[1].object;
    if (l_1C->twin != 0) {
        object_delete(l_1C->twin);
        l_1C->twin = 0;
    }
    l_14 = a2->args[2].object->twin;
    if (l_14 != 0) {
        l_18 = object_create_child(l_14, 0, 107);
        l_18->type = 2;
        l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
        l_18->image = l_1C->image;
        l_18->quest_id = (signed char)a1->id;
        l_18->twin = l_1C;
        l_1C->twin = l_18;
        mc_memcpy(&l_18->data, &l_1C->data, 107, (int)D_00170A64, 1008, 4);
    }
    l_14 = a2->args[2].object;
    l_1C->id = object_new_id(((unsigned)l_14->id) >> 16);
    object_reparent(l_14, l_1C);
}

struct record *quest_find_site_for_building(struct building *a1)
{
    struct record *l_34;
    struct record *l_30;
    struct record *l_2C;
    struct qbn_place *l_28;
    struct qbn_person *l_24;
    int l_20;
    int l_1C;

    l_34 = D_00195A00->children;
    while (l_34 != 0) {
        l_30 = l_34->next;
        if (l_34->type == 14) {
            D_00195D00 = l_34;
            D_00199780 = (struct quest *)((int)&l_34->data.quest);
            l_28 = (struct qbn_place *)quest_section(D_00199780, 4);
            for (l_1C = 0; D_00199780->section_counts[4] > l_1C; l_1C++, l_28++) {
                l_2C = l_28->object;
                if ((l_28->flags & 64) != 0) {
                    D_001962A3 = 1;
                } else {
                    D_001962A3 = 0;
                }
                if (l_2C != 0 && a1->id == l_2C->data.building.id) return l_2C;
            }
            l_24 = (struct qbn_person *)quest_section(D_00199780, 3);
            for (l_1C = 0; D_00199780->section_counts[3] > l_1C; l_1C++, l_24++) {
                l_20 = (int)RECORD_DATA(l_24->object);
                if (((int)(short)(l_24->flags & 16384)) != 0) {
                    D_001962A3 = 1;
                } else {
                    D_001962A3 = 0;
                }
                if (a1->id == *(int *)((char *)l_20 + 20)) return l_24->object;
            }
        }
        l_34 = l_30;
    }
    return 0;
}

int func_0003445C(int a1)
{
    int l_20;
    int l_1C;

    l_1C = 0;
    for (l_20 = 0; l_20 < 3; l_20++) {
        if (*(int *)((char *)((l_20 << 2) + a1)) != 0) l_1C++;
    }
    if (l_1C == 0) return 0;
    return *(int *)((char *)((rand_range(0, l_1C - 1) << 2) + a1));
}

int func_000344D3(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
    for (; l_20 < faction_count; l_20++) {
        if (factions[l_20].reputation < 0) l_1C++;
    }
    if (l_1C == 0) return 0;
    l_1C = rand_range(0, l_1C - 1);
    for (l_20 = 0; l_20 < faction_count; l_20++) {
        if (factions[l_20].reputation >= 0) continue;
        if (l_1C == 0) return factions[l_20].id;
        l_1C--;
    }
    return 0;
}

int quest_object_in_use(int a1)
{
    struct record *l_2C;
    struct qbn_place *l_28;
    struct qbn_person *l_24;
    struct quest *l_20;
    int l_1C;

    l_2C = D_00195A00->children;
    while (l_2C != 0) {
        if (l_2C->type == 14) {
            l_20 = &l_2C->data.quest;
            l_28 = (struct qbn_place *)quest_section(l_20, 4);
            for (l_1C = 0; l_20->section_counts[4] > l_1C; l_1C++, l_28++) {
                if (l_28->object != 0) {
                    if (l_28->object->id == a1) return 1;
                }
            }
            l_24 = (struct qbn_person *)quest_section(l_20, 3);
            for (l_1C = 0; l_20->section_counts[3] > l_1C; l_1C++, l_24++) {
                if (l_24->object != 0) {
                    if (l_24->object->id == a1) return 1;
                }
            }
        }
        l_2C = l_2C->next;
    }
    return 0;
}
