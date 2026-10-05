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
extern char player_environment[];
extern char material_to_hit[];
extern char D_00185128[];
extern char D_0018512E[];
extern char body_part_armor_slots[];
extern char text_buffer[];
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
extern char game_minutes[];
extern char D_00195C44[];
extern char D_00195D00[];
extern char quest_potential_questor[];
extern char D_00195F68[];
extern char D_001961F5[];
extern char D_00196282[];
extern struct quest *current_quest;
extern char quest_debug_data[];
extern char D_001997AE[];
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

    if (*(signed char *)((char *)a1) != 0) goto L4BDDC;
    return 0;
L4BDDC:;
    if (stricmp(a1, (int)D_00174F5C) != 0) goto L4BDF9;
    return 0;
L4BDF9:;
    if (quest_is_active(a1) == 0) goto L4BE11;
    return 0;
L4BE11:;
    func_000A0ED9(172, (int)D_00174F47);
    mc_sprintf((int)text_buffer, (int)D_00174F57, (int)D_001917E4, a1);
    l_1C = open((int)text_buffer, 512);
    if (l_1C >= 0) goto L4BEB1;
    func_000A0ED9(177, (int)D_00174F47);
    mc_sprintf((int)text_buffer, (int)D_00174F57, (int)D_00191834, a1);
    l_1C = open((int)text_buffer, 512);
    if (l_1C >= 0) goto L4BEB1;
    return 0;
L4BEB1:;
    func_0009DEA7(l_1C);
    l_20 = disk_read_file(a1, 0);
    logbook_prune_quests();
    l_28 = object_create_child(D_00195A00, 0, *(int *)disk_last_file_size);
    l_28->type = 14;
    l_28->flags = 3;
    l_24 = &l_28->data.quest;
    mc_memcpy(l_24, l_20, (int)(short)*(short *)disk_last_file_size, (int)D_00174F47, 193, 4);
    if (l_20 == 0) goto L4BF25;
    if (l_20 != (-1751672937)) goto L4BF27;
L4BF25:;
    goto L4BF40;
L4BF27:;
    mc_free(l_20, (int)D_00174F47, 194);
    l_20 = -1751672937;
L4BF40:;
    l_28->quest_id = (l_24->id = quest_free_id());
    func_000A14E8(l_24->name, a1, 8, (int)D_00174F47, 197, 9);
    if (((int)(short)*(short *)D_00195F68) != 240) goto L4BF95;
    *(short *)D_00195F68 = current_building->faction_id;
L4BF95:;
    if (toupper((int)(unsigned char)l_24->name[5]) != 89) goto L4BFBA;
    l_24->faction_id = *(short *)D_00195F68;
    goto L4BFC3;
L4BFBA:;
    l_24->faction_id = 0;
L4BFC3:;
    current_quest = l_24;
    *(int *)D_00195D00 = (int)l_28;
    if (quest_init_resources(l_24) != 0) goto L4BFFF;
    quest_end(l_24);
    msgbox_show_rsc(600, 1);
    return 0;
L4BFFF:;
    quest_timers_start_all();
    if (tolower((int)(unsigned char)l_24->name[0]) == 115) goto L4C025;
    if (quest_offer_prompt(l_24) == 0) goto L4C027;
L4C025:;
    goto L4C038;
L4C027:;
    quest_end(l_24);
    return 0;
L4C038:;
    logbook_prune_quests();
    func_0004C759();
    *(int *)quest_debug_data = (int)current_quest;
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

    l_18 = *(int *)D_00195C44;
    l_1C = 0;
L4C4C2:;
    if (l_1C < a2) goto L4C4D4;
    goto L4C4F9;
L4C4CC:;
    l_1C++;
    goto L4C4C2;
L4C4D4:;
    if (stricmp(l_18, a1) != 0) goto L4C4EB;
    return a2;
L4C4EB:;
    l_18 += func_000A0DF4(l_18) + 1;
    goto L4C4CC;
L4C4F9:;
    mc_strncpy(l_18, a1, 4, (int)D_00174F47, 383);
    return a2 + 1;
}

int func_0004C526(struct record *a1)
{
    if (a1->type == 8) goto L4C54F;
    return 0;
L4C54F:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 71)) != *(int *)D_00190BE4) goto L4C574;
    D_00195AF4 = a1;
    return 1;
L4C574:;
    return 0;
}

void func_0004C588(struct record *a1)
{
    struct record *l_18;

    if (a1->twin != 0) return;
    if (a1->type != 41) goto L4C5C5;
    if ((((unsigned)a1->id) >> 16) == 800) goto L4C5CA;
L4C5C5:;
    goto L4C6CB;
L4C5CA:;
    *(int *)D_00190BE4 = (int)(unsigned short)*(short *)((char *)a1 + 89);
    D_00195AF4 = 0;
    object_find((int)D_00195AC4, (int)func_0004C526);
    if (D_00195AF4 == 0) return;
    if (D_00195AF4->twin == 0) goto L4C649;
    func_000A0ED9(422, (int)D_00174F47);
    mc_sprintf((int)text_buffer, (int)D_00174F71, (int)(unsigned short)(short)D_00195AF4->image);
    fatal_error((int)text_buffer);
L4C649:;
    a1->id = D_00195AF4->id;
    mc_memcpy(&a1->x, &D_00195AF4->x, 12, (int)D_00174F47, 427, 4);
    D_00195AF4->quest_id = a1->quest_id;
    a1->twin = D_00195AF4;
    D_00195AF4->twin = a1;
    *(signed char *)((char *)D_00195AF4 + 73) |= 128;
    if (((int)(unsigned short)(a1->flags & 2048)) == 0) goto L4C6C6;
    a1->flags |= 0x200;
L4C6C6:;
    return;
L4C6CB:;
    if ((D_00195AC4->id >> 16) != (((unsigned)a1->id) >> 16)) return;
    l_18 = object_find_by_id(D_00195AC4, a1->id);
    if (l_18 == 0) goto L4C708;
    if (a1->type == l_18->type) goto L4C715;
L4C708:;
    func_000310E1(a1, l_18);
    return;
L4C715:;
    l_18->quest_id = a1->quest_id;
    l_18->twin = a1;
    a1->twin = l_18;
    if (((int)(unsigned short)(a1->flags & 2048)) == 0) return;
    l_18->flags |= 0xA00;
}

void func_0004C759(void)
{
    *(signed char *)D_00196282 = 0;
    object_foreach_open(nonworld_root, (int)func_0004C588);
}

int quest_free_id(void)
{
    struct record *l_28;
    struct quest *l_24;
    int l_20;
    int l_1C;

    l_28 = D_00195A00->children;
    l_20 = *(int *)D_00195C44;
    mc_memset(l_20, 0, 256, (int)D_00174F47, 478, 4);
L4C7C3:;
    if (l_28 == 0) goto L4C7E8;
    l_24 = &l_28->data.quest;
    (*(signed char *)((char *)(l_24->id + l_20)))++;
    l_28 = l_28->next;
    goto L4C7C3;
L4C7E8:;
    l_1C = 1;
L4C7EF:;
    if (l_1C < 256) goto L4C802;
    goto L4C817;
L4C7FA:;
    l_1C++;
    goto L4C7EF;
L4C802:;
    if (*(signed char *)((char *)(l_20 + l_1C)) != 0) goto L4C815;
    return l_1C;
L4C815:;
    goto L4C7FA;
L4C817:;
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
    if (l_18 == 0) goto L4C930;
    if (l_18->type == 11) goto L4C932;
L4C930:;
    goto L4C937;
L4C932:;
    return;
L4C937:;
    if (l_18 == 0) goto L4C94D;
    if (l_18->type == 14) goto L4C94F;
L4C94D:;
    goto L4C954;
L4C94F:;
    return;
L4C954:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 3) goto L4C98D;
    if (l_18 == 0) goto L4C976;
    if (l_18->type < 16) goto L4C978;
L4C976:;
    goto L4C988;
L4C978:;
    if (l_18->type != 1) goto L4C98D;
L4C988:;
    return;
L4C98D:;
    l_1C = (int)RECORD_DATA(a1);
    if (*(short *)((char *)l_1C) == 0) return;
    l_24 = faction_find((int)(short)*(short *)((char *)l_1C));
    l_20 = l_24;
L4C9B7:;
    if (l_20->parent == 0) goto L4C9CB;
    l_20 = l_20->parent;
    goto L4C9B7;
L4C9CB:;
    if (l_20->id == 108) goto L4C9ED;
    if (l_20->id != 42) goto L4C9EF;
L4C9ED:;
    goto L4CA00;
L4C9EF:;
    if (l_20->id != 40) goto L4CA02;
L4CA00:;
    goto L4CA13;
L4CA02:;
    if (l_20->id != 41) goto L4CA18;
L4CA13:;
    return;
L4CA18:;
    if (l_24->type == 2) goto L4CA36;
    if (l_24->type != 12) goto L4CA38;
L4CA36:;
    goto L4CA47;
L4CA38:;
    if (l_24->type != 4) goto L4CA49;
L4CA47:;
    goto L4CA4B;
L4CA49:;
    return;
L4CA4B:;
    if (l_24->type != 4) goto L4CA74;
    if ((rand() % 100) > 50) return;
    goto L4CA8C;
L4CA74:;
    if ((rand() % 100) > 25) return;
L4CA8C:;
    *(signed char *)((char *)l_1C + 2) |= 128;
}

void func_0004CA9D(void)
{
    int l_18;

    l_18 = rand();
    if (((int)(unsigned char)*(signed char *)player_environment) != 3) goto L4CAFB;
    if ((((unsigned)D_00195AC4->id) >> 16) == 50027) goto L4CAE3;
    if ((((unsigned)D_00195AC4->id) >> 16) != 50029) goto L4CAE5;
L4CAE3:;
    goto L4CAF7;
L4CAE5:;
    if ((((unsigned)D_00195AC4->id) >> 16) != 50033) goto L4CAF9;
L4CAF7:;
    goto L4CAFB;
L4CAF9:;
    return;
L4CAFB:;
    srand(((unsigned)*(int *)game_minutes) / 1440);
    object_foreach(D_00195AC4, (int)func_0004C8EB);
    srand(l_18);
}

int func_0004CB2F(struct record *a1)
{
    return (int)(unsigned char)(*(signed char *)((char *)a1 + 73) & 128);
}

void quest_pick_for_npc(struct record *a1)
{
    struct faction *l_18;

    if (a1 == 0) return;
    l_18 = faction_find((int)(short)*(short *)((char *)a1 + 71));
    if (l_18->type != 4) goto L4CBA9;
    if (l_18->id != 407) goto L4CBAB;
L4CBA9:;
    goto L4CBD4;
L4CBAB:;
    quest_pick_file(82, 0, 48, 67, player_character->level);
    return;
L4CBD4:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 3) return;
    quest_pick_file(65, 75, 48, 67, player_character->level);
}

void func_0004CC14(struct record *a1)
{
    if (a1->type != 8) goto L4CC3D;
    if (*(int *)D_00195B84 != 0) goto L4CC3F;
L4CC3D:;
    return;
L4CC3F:;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 73) & 128)) == 0) return;
    if (((struct bf8_7_1 *)&D_00195B85)->f == 0) goto L4CC61;
    (*(int *)D_00195B84)++;
    goto L4CC67;
L4CC61:;
    (*(int *)D_00195B84)--;
L4CC67:;
    if (*(int *)D_00195B84 != 0) return;
    D_00195AF4 = a1;
}

int quest_find_potential_questor(void)
{
    *(int *)D_00195B84 = 32768;
    object_foreach(D_00195AC4, (int)func_0004CC14);
    if (*(int *)D_00195B84 != 32768) goto L4CCBE;
    return 0;
L4CCBE:;
    *(int *)D_00195B84 = rand_range(0, (*(int *)D_00195B84 & 32767) - 1) + 1;
    D_00195AF4 = 0;
    object_foreach(D_00195AC4, (int)func_0004CC14);
    *(int *)quest_potential_questor = (int)D_00195AF4;
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
    if (*(signed char *)D_001961F5 == 0) goto L4CD6C;
    *(int *)quest_potential_questor = (int)a1;
    *(signed char *)D_001961F5 = 0;
    return 1;
L4CD6C:;
    return 0;
}

int func_0004CD80(int a1)
{
    if (*(signed char *)D_001997AE == 0) goto L4CDA3;
    return 0;
L4CDA3:;
    if (((int)(unsigned short)*(short *)((char *)a1)) == 510) goto L4CDBE;
    return 0;
L4CDBE:;
    *(signed char *)D_001997AE = 1;
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
    l_14 = 0;
L4CE4E:;
    if (a1->section_counts[3] > l_14) goto L4CE6B;
    return;
L4CE5C:;
    l_14++;
    l_18++;
    goto L4CE4E;
L4CE6B:;
    if (l_18->kind != 21) goto L4CEB3;
    rumor_add_quest(a1, ((a2 != 0) ? 1008 : 1009), l_18->object->id, 2);
    return;
L4CEB3:;
    goto L4CE5C;
}

int quest_is_active(int a1)
{
    struct record *l_20;
    struct quest *l_1C;

    l_20 = D_00195A00->children;
L4CEDA:;
    if (l_20 == 0) goto L4CF23;
    if (l_20->type != 14) goto L4CF18;
    l_1C = &l_20->data.quest;
    if (strnicmp(l_1C->name, a1, 8) != 0) goto L4CF18;
    return 1;
L4CF18:;
    l_20 = l_20->next;
    goto L4CEDA;
L4CF23:;
    return 0;
}

int func_0004CF37(struct item *a1)
{
    int l_1C;

    l_1C = 0;
    if (a1->armor_type != 2) goto L4CF7E;
    return (((int)(short)*(short *)(material_to_hit + (a1->material * 2))) + l_1C) + 45;
L4CF7E:;
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
    if (l_34->equipped[(int)(unsigned char)*(signed char *)(body_part_armor_slots + a2)] == 0) goto L4D013;
    l_30 -= func_0004CF37(&l_34->equipped[(int)(unsigned char)*(signed char *)(body_part_armor_slots + a2)]->data.item);
L4D013:;
    l_1C = l_34->equipped[EQUIP_LEFT_HAND];
    if (l_1C == 0) goto L4D0A1;
    l_18 = &l_1C->data.item;
    if (l_18->group != 2) goto L4D0A1;
    l_2C = l_18->index - 7;
    if (l_2C < 0) goto L4D061;
    if (l_2C <= 3) goto L4D063;
L4D061:;
    goto L4D0A1;
L4D063:;
    l_28 = 0;
L4D06A:;
    if (l_28 < 4) goto L4D07A;
    goto L4D0A1;
L4D072:;
    l_28++;
    goto L4D06A;
L4D07A:;
    if (((int)(unsigned char)*(signed char *)(D_0018512E + ((l_2C << 2) + l_28))) != a2) goto L4D09F;
    l_30 -= (l_2C + 1) * 5;
    goto L4D0A1;
L4D09F:;
    goto L4D072;
L4D0A1:;
    l_28 = 0;
L4D0A8:;
    if (l_28 < 27) goto L4D0BB;
    goto L4D165;
L4D0B3:;
    l_28++;
    goto L4D0A8;
L4D0BB:;
    if (l_34->equipped[l_28] == 0) goto L4D15B;
    l_18 = &l_34->equipped[l_28]->data.item;
    if (l_18->enchantments[0].type == (-1)) goto L4D0B3;
    l_24 = 0;
L4D0F9:;
    if (l_18->enchantments[l_24].type == (-1)) goto L4D111;
    if (l_24 < 10) goto L4D113;
L4D111:;
    goto L4D15B;
L4D113:;
    if (l_18->enchantments[l_24].type != 12) goto L4D132;
    l_30 += -25;
    l_20 = 1;
    goto L4D14F;
L4D132:;
    if (l_18->enchantments[l_24].type != 24) goto L4D14F;
    l_30 += 25;
    l_20 = 1;
L4D14F:;
    l_24++;
    if (l_20 == 0) goto L4D0F9;
L4D15B:;
    if (l_20 == 0) goto L4D0B3;
L4D165:;
    if (l_30 <= 100) goto L4D174;
    return 100;
L4D174:;
    if (l_30 >= (-100)) goto L4D183;
    return -100;
L4D183:;
    return l_30;
}

void character_update_armor_values(struct record *a1)
{
    int l_1C;
    struct character *l_18;

    l_18 = &a1->data.character;
    l_1C = 0;
L4D1B6:;
    if (l_1C < 7) goto L4D1C6;
    return;
L4D1BE:;
    l_1C++;
    goto L4D1B6;
L4D1C6:;
    l_18->armor_values[l_1C] = func_0004CFA7(a1, l_1C);
    goto L4D1BE;
}
