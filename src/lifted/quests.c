/* quests.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char disk_last_file_size[];
extern char D_00174F47[];
extern char D_00174F57[];
extern char D_00174F5C[];
extern char D_00174F71[];
extern char D_00174F8F[];
extern unsigned char player_environment;
extern char material_to_hit[];
extern char D_00185128[];
extern signed char D_0018512E[];
extern signed char body_part_armor_slots[];
extern signed char text_buffer[];
extern char D_00190BE4[];
extern char D_001917E4[];
extern char D_00191834[];
extern struct record *nonworld_root;
extern struct record *D_00195A00;
extern struct building *current_building;
extern struct record *D_00195AC4;
extern struct record *D_00195AF4;
extern char D_00195B84[];
extern char D_00195B85[];
extern struct character *player_character;
extern int game_minutes;
extern int D_00195C44;
extern struct record *D_00195D00;
extern int quest_potential_questor;
extern short D_00195F68;
extern char D_001961F5[];
extern signed char D_00196282;
extern struct quest *current_quest;
extern int quest_debug_data;
extern signed char D_001997AE;
extern char D_001A5BE4[];

extern struct faction *faction_find(short);
extern void *quest_section(struct quest *, int);
extern struct record *func_000310E1(struct record *, struct record *);
extern int quest_init_resources(struct quest *);
extern int quest_offer_prompt(struct quest *);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern int disk_read_file(int, int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_find(int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int rand();
extern int srand();
extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int stricmp();
extern int toupper();
extern int strnicmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int tolower();
extern int func_000A14E8();
extern void faction_change_reputation(struct faction *, int);
extern void rumor_add_quest(struct quest *, int, int, int);
extern void quest_timers_start_all(void);
extern void msgbox_show_rsc(int, int);
extern void quest_end(struct quest *);
extern void fatal_error(int);
extern void logbook_prune_quests(void);
extern void object_foreach(struct record *, int);
extern void object_foreach_open(struct record *, int);
int quest_start(int);
int func_0004C526(struct record *);
int quest_free_id(void);
int quest_is_active(int);
int func_0004CF37(struct item *);
int func_0004CFA7(struct record *, int);
void func_0004C588(struct record *);
void func_0004C759(void);
void func_0004C8EB(struct record *);
void quest_pick_for_npc(struct record *);
void func_0004CC14(struct record *);
#pragma aux func_000A0ED9 parm routine [];

int quest_start(int a1)
{
    struct record *l_28;
    struct quest *l_24;
    int l_20;
    int l_1C;

    if (*(signed char *)((char *)a1) == 0) return 0;
    if (stricmp(a1, (int)D_00174F5C) == 0) return 0;
    if (quest_is_active(a1) != 0) return 0;
    func_000A0ED9(172, (int)D_00174F47);
    mc_sprintf((int)text_buffer, (int)D_00174F57, (int)D_001917E4, a1);
    l_1C = open((int)text_buffer, 512);
    if (l_1C < 0) {
        func_000A0ED9(177, (int)D_00174F47);
        mc_sprintf((int)text_buffer, (int)D_00174F57, (int)D_00191834, a1);
        l_1C = open((int)text_buffer, 512);
        if (l_1C < 0) return 0;
    }
    func_0009DEA7(l_1C);
    l_20 = disk_read_file(a1, 0);
    logbook_prune_quests();
    l_28 = object_create_child(D_00195A00, 0, *(int *)disk_last_file_size);
    l_28->type = 14;
    l_28->flags = 3;
    l_24 = &l_28->data.quest;
    mc_memcpy(l_24, l_20, (int)(short)*(short *)disk_last_file_size, (int)D_00174F47, 193, 4);
    if (l_20 != 0 && l_20 != (-1751672937)) {
        mc_free(l_20, (int)D_00174F47, 194);
        l_20 = -1751672937;
    }
    l_28->quest_id = (l_24->id = quest_free_id());
    func_000A14E8(l_24->name, a1, 8, (int)D_00174F47, 197, 9);
    if (((int)(short)D_00195F68) == 240) {
        D_00195F68 = current_building->faction_id;
    }
    if (toupper((int)(unsigned char)l_24->name[5]) == 89) {
        l_24->faction_id = D_00195F68;
    } else {
        l_24->faction_id = 0;
    }
    current_quest = l_24;
    D_00195D00 = l_28;
    if (quest_init_resources(l_24) == 0) {
        quest_end(l_24);
        msgbox_show_rsc(600, 1);
        return 0;
    }
    quest_timers_start_all();
    if (tolower((int)(unsigned char)l_24->name[0]) != 115 && quest_offer_prompt(l_24) == 0) {
        quest_end(l_24);
        return 0;
    }
    logbook_prune_quests();
    func_0004C759();
    quest_debug_data = (int)current_quest;
    rumor_add_quest(current_quest, 1005, 0, 4);
    return (int)l_24;
}

void quest_start_pending(void)
{
    if (*(signed char *)D_001961F5 == 0) return;
    quest_start((int)D_001961F5);
    *(signed char *)D_001961F5 = 0;
}

int func_0004C4A0(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_18 = D_00195C44;
    for (l_1C = 0; l_1C < a2; l_1C++) {
        if (stricmp(l_18, a1) == 0) return a2;
        l_18 += func_000A0DF4(l_18) + 1;
    }
    mc_strncpy(l_18, a1, 4, (int)D_00174F47, 383);
    return a2 + 1;
}

int func_0004C526(struct record *a1)
{
    if (a1->type != 8) return 0;
    if (a1->data.person.faction_id == *(int *)D_00190BE4) {
        D_00195AF4 = a1;
        return 1;
    }
    return 0;
}

void func_0004C588(struct record *a1)
{
    struct record *l_18;

    if (a1->twin != 0) return;
    if (a1->type == 41 && (((unsigned)a1->id) >> 16) == 800) {
        *(int *)D_00190BE4 = a1->data.building.faction_id;
        D_00195AF4 = 0;
        object_find((int)D_00195AC4, (int)func_0004C526);
        if (D_00195AF4 == 0) return;
        if (D_00195AF4->twin != 0) {
            func_000A0ED9(422, (int)D_00174F47);
            mc_sprintf((int)text_buffer, (int)D_00174F71, (int)(unsigned short)(short)D_00195AF4->image);
            fatal_error((int)text_buffer);
        }
        a1->id = D_00195AF4->id;
        mc_memcpy(&a1->x, &D_00195AF4->x, 12, (int)D_00174F47, 427, 4);
        D_00195AF4->quest_id = a1->quest_id;
        a1->twin = D_00195AF4;
        D_00195AF4->twin = a1;
        D_00195AF4->data.person.flags |= 128;
        if (((int)(unsigned short)(a1->flags & 2048)) != 0) a1->flags |= 0x200;
        return;
    }
    if ((D_00195AC4->id >> 16) != (((unsigned)a1->id) >> 16)) return;
    l_18 = object_find_by_id(D_00195AC4, a1->id);
    if (l_18 == 0 || a1->type != l_18->type) {
        func_000310E1(a1, l_18);
        return;
    }
    l_18->quest_id = a1->quest_id;
    l_18->twin = a1;
    a1->twin = l_18;
    if (((int)(unsigned short)(a1->flags & 2048)) == 0) return;
    l_18->flags |= 0xA00;
}

void func_0004C759(void)
{
    D_00196282 = 0;
    object_foreach_open(nonworld_root, (int)func_0004C588);
}

int quest_free_id(void)
{
    struct record *l_28;
    struct quest *l_24;
    int l_20;
    int l_1C;

    l_28 = D_00195A00->children;
    l_20 = D_00195C44;
    mc_memset(l_20, 0, 256, (int)D_00174F47, 478, 4);
    while (l_28 != 0) {
        l_24 = &l_28->data.quest;
        (*(signed char *)((char *)(l_24->id + l_20)))++;
        l_28 = l_28->next;
    }
    for (l_1C = 1; l_1C < 256; l_1C++) {
        if (*(signed char *)((char *)(l_20 + l_1C)) == 0) return l_1C;
    }
    fatal_error((int)D_00174F8F);
    return 0;
}

void qaction_op46_hide_npc(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_14;

    l_14 = a2->args[1].object;
    l_14->flags |= 0xA00;
    if (l_14->twin == 0) return;
    l_14->twin->flags |= 0xA00;
}

void func_0004C874(struct quest *a1, struct qbn_op *a2)
{
}

void qaction_op48_restore_npc(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_14;

    l_14 = a2->args[1].object;
    l_14->flags &= ~0xA00;
    if (l_14->twin == 0) return;
    l_14->twin->flags &= ~0xA00;
}

void func_0004C8CF(struct quest *a1, struct qbn_op *a2)
{
}

void func_0004C8EB(struct record *a1)
{
    struct faction *l_24;
    struct faction *l_20;
    int l_1C;
    struct building *l_18;

    if (a1->type != 8) return;
    l_18 = object_building(a1);
    if (l_18 != 0 && l_18->type == 11) return;
    if (l_18 != 0 && l_18->type == 14) return;
    if (((int)player_environment) != 3) {
        if (l_18 == 0 || l_18->type >= 16 || l_18->type == 1) return;
    }
    l_1C = (int)RECORD_DATA(a1);
    if (*(short *)((char *)l_1C) == 0) return;
    l_24 = faction_find((int)(short)*(short *)((char *)l_1C));
    l_20 = l_24;
    while (l_20->parent != 0) l_20 = l_20->parent;
    if (l_20->id == 108 || l_20->id == 42 || l_20->id == 40 || l_20->id == 41) return;
    if (l_24->type != 2 && l_24->type != 12 && l_24->type != 4) return;
    if (l_24->type == 4) {
        if ((rand() % 100) > 50) return;
    } else {
        if ((rand() % 100) > 25) return;
    }
    *(signed char *)((char *)l_1C + 2) |= 128;
}

void func_0004CA9D(void)
{
    int l_18;

    l_18 = rand();
    if (((int)player_environment) == 3) {
        if ((((unsigned)D_00195AC4->id) >> 16) != 50027 && (((unsigned)D_00195AC4->id) >> 16) != 50029 && (((unsigned)D_00195AC4->id) >> 16) != 50033) {
            return;
        }
    }
    srand(((unsigned)game_minutes) / 1440);
    object_foreach(D_00195AC4, (int)func_0004C8EB);
    srand(l_18);
}

int func_0004CB2F(struct record *a1)
{
    return (int)(unsigned char)(a1->data.person.flags & 128);
}

void quest_pick_for_npc(struct record *a1)
{
    struct faction *l_18;

    if (a1 == 0) return;
    l_18 = faction_find((int)(short)a1->data.person.faction_id);
    if (l_18->type == 4 && l_18->id != 407) {
        quest_pick_file(82, 0, 48, 67, player_character->level);
        return;
    }
    if (((int)player_environment) == 3) return;
    quest_pick_file(65, 75, 48, 67, player_character->level);
}

void func_0004CC14(struct record *a1)
{
    if (a1->type != 8 || *(int *)D_00195B84 == 0) return;
    if (((int)(unsigned char)(a1->data.person.flags & 128)) == 0) return;
    if (((struct bf8_7_1 *)&D_00195B85)->f != 0) {
        (*(int *)D_00195B84)++;
    } else {
        (*(int *)D_00195B84)--;
    }
    if (*(int *)D_00195B84 != 0) return;
    D_00195AF4 = a1;
}

int quest_find_potential_questor(void)
{
    *(int *)D_00195B84 = 32768;
    object_foreach(D_00195AC4, (int)func_0004CC14);
    if (*(int *)D_00195B84 == 32768) return 0;
    *(int *)D_00195B84 = rand_range(0, (*(int *)D_00195B84 & 32767) - 1) + 1;
    D_00195AF4 = 0;
    object_foreach(D_00195AC4, (int)func_0004CC14);
    quest_potential_questor = (int)D_00195AF4;
    return (int)D_00195AF4;
}

int func_0004CD10(struct record *a1)
{
    int l_1C;

    l_1C = rand();
    srand(a1->id);
    *(signed char *)D_001961F5 = 0;
    quest_pick_for_npc(a1);
    srand(l_1C);
    if (*(signed char *)D_001961F5 != 0) {
        quest_potential_questor = (int)a1;
        *(signed char *)D_001961F5 = 0;
        return 1;
    }
    return 0;
}

int func_0004CD80(int a1)
{
    if (D_001997AE != 0) return 0;
    if (((int)(unsigned short)*(short *)((char *)a1)) != 510) return 0;
    D_001997AE = 1;
    quest_start((int)D_001A5BE4);
    return 1;
}

void quest_reward_faction(struct quest *a1)
{
    struct faction *l_18;

    if (a1->faction_id == 0) return;
    l_18 = faction_find(a1->faction_id);
    faction_change_reputation(l_18, 5);
}

void func_0004CE24(struct quest *a1, int a2)
{
    struct qbn_person *l_18;
    int l_14;

    l_18 = quest_section(a1, 3);
    for (l_14 = 0; a1->section_counts[3] > l_14; l_14++, l_18++) {
        if (l_18->kind == 21) {
            rumor_add_quest(a1, ((a2 != 0) ? 1008 : 1009), l_18->object->id, 2);
            return;
        }
    }
}

int quest_is_active(int a1)
{
    struct record *l_20;
    struct quest *l_1C;

    l_20 = D_00195A00->children;
    while (l_20 != 0) {
        if (l_20->type == 14) {
            l_1C = &l_20->data.quest;
            if (strnicmp(l_1C->name, a1, 8) == 0) return 1;
        }
        l_20 = l_20->next;
    }
    return 0;
}

int func_0004CF37(struct item *a1)
{
    int l_1C;

    l_1C = 0;
    if (a1->armor_type == 2) {
        return (((int)(short)*(short *)(material_to_hit + (a1->material * 2))) + l_1C) + 45;
    }
    return l_1C + ((int)(short)*(short *)(D_00185128 + (a1->armor_type * 2)));
}

int func_0004CFA7(struct record *a1, int a2)
{
    struct character *l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    struct record *l_1C;
    struct item *l_18;

    l_20 = 0;
    l_30 = 100;
    l_34 = &a1->data.character;
    if (l_34->equipped[(int)(unsigned char)body_part_armor_slots[a2]] != 0) {
        l_30 -= func_0004CF37(&l_34->equipped[(int)(unsigned char)body_part_armor_slots[a2]]->data.item);
    }
    l_1C = l_34->equipped[EQUIP_LEFT_HAND];
    if (l_1C != 0) {
        l_18 = &l_1C->data.item;
        if (l_18->group == 2) {
            l_2C = l_18->index - 7;
            if (l_2C >= 0 && l_2C <= 3) {
                for (l_28 = 0; l_28 < 4; l_28++) {
                    if (((int)(unsigned char)D_0018512E[(l_2C << 2) + l_28]) == a2) {
                        l_30 -= (l_2C + 1) * 5;
                        break;
                    }
                }
            }
        }
    }
    for (l_28 = 0; l_28 < 27; l_28++) {
        if (l_34->equipped[l_28] != 0) {
            l_18 = &l_34->equipped[l_28]->data.item;
            if (l_18->enchantments[0].type == (-1)) continue;
            l_24 = 0;
            do {
                if (l_18->enchantments[l_24].type == (-1) || l_24 >= 10) break;
                if (l_18->enchantments[l_24].type == 12) {
                    l_30 += -25;
                    l_20 = 1;
                } else if (l_18->enchantments[l_24].type == 24) {
                    l_30 += 25;
                    l_20 = 1;
                }
                l_24++;
            } while (l_20 == 0);
        }
        if (l_20 != 0) break;
    }
    if (l_30 > 100) return 100;
    if (l_30 < (-100)) return -100;
    return l_30;
}

void character_update_armor_values(struct record *a1)
{
    int l_1C;
    struct character *l_18;

    l_18 = &a1->data.character;
    for (l_1C = 0; l_1C < 7; l_1C++) {
        l_18->armor_values[l_1C] = func_0004CFA7(a1, l_1C);
    }
}
