/* qcom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char mouse_buttons[];
extern char D_001707F0[];
extern char D_001707F7[];
extern char D_00170801[];
extern char D_001708ED[];
extern char D_0017A120[];
extern char D_001841E3[];
extern char region_precipitation_override[];
extern char region_legal_reputation[];
extern char text_buffer[];
extern char D_001911E4[];
extern char quest_global_states[];
extern char D_00195984[];
extern char D_00195988[];
extern char D_0019598C[];
extern char D_00195990[];
extern struct record *nonworld_root;
extern struct record *D_00195A00;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195D00[];
extern char D_00195D48[];
extern char qbn_opcode_arg_counts[];
extern char current_region[];
extern char D_0019626F[];
extern char game_mode[];
extern char D_00196298[];
extern char D_0019629B[];
extern char quest_debug_object[];
extern struct quest *current_quest;
extern struct record *D_00199768;
extern char quest_debug_data[];
extern struct record *quest_event_object;
extern char D_00199780[];
extern char quest_event_code[];

extern struct faction *faction_find(short);
extern int qcond_op05_event_at_place(struct quest *, struct qbn_op *);
extern int qcond_op43_pc_at_place(struct quest *, struct qbn_op *);
extern int qcond_op01_item_given_to_npc(struct quest *, struct qbn_op *);
extern int qcond_op03_event_object(struct quest *, struct qbn_op *);
extern int qcond_op21_event_same_kind(struct quest *, struct qbn_op *);
extern int qcond_op02_event_count(struct quest *, struct qbn_op *);
extern int qcond_op28_event_person(struct quest *, struct qbn_op *);
extern int qcond_op70_player_has_items(struct quest *, struct qbn_op *);
extern int quest_arg_state(struct qbn_op *, int);
extern void *quest_section(struct quest *, int);
extern void *quest_record(struct quest *, int, int);
extern int qcond_op57_item_used(struct quest *, struct qbn_op *);
extern int func_000317AE(struct quest *);
extern int quest_start(int);
extern int sound_play(int, int, int);
extern int disk_resolve_path(int);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int mc_memset();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int func_000C1500();
extern int func_0012B136();
extern void region_flag_set(int, int);
extern void region_flag_clear(int, int);
extern void faction_change_reputation(struct faction *, int);
extern void rumor_add_quest(struct quest *, int, int, int);
extern void rumor_file_purge(void);
extern void quest_op17_grant_building_access(struct quest *, struct qbn_op *);
extern void quest_op09_spawn_repeat(struct quest *, struct qbn_op *);
extern void quest_op87_respawn(struct quest *, struct qbn_op *);
extern void quest_unlink_for_save(struct quest *);
extern void quest_timers_update(struct quest *);
extern void quest_timer_update(struct quest *, struct qbn_timer *, int);
extern void qaction_op12_start_stop_timer(struct quest *, struct qbn_op *, int);
extern void qaction_op35_cycle_state(struct quest *, struct qbn_op *);
extern void qaction_op34_pick_one_state(struct quest *, struct qbn_op *);
extern void qaction_op29_prompt(struct quest *, struct qbn_op *);
extern void quest_set_state(struct quest *, struct qbn_op *, int);
extern void quest_set_arg_state(struct quest *, struct qbn_op *, int, int);
extern void qaction_op04_give_reward(struct quest *, struct qbn_op *);
extern void qaction_op19_reveal_location(struct quest *, struct qbn_op *, int);
extern void func_0003077F(struct record *, int);
extern void quest_give_item_to_player(struct record *);
extern void qaction_op37_repute_exceeds(struct quest *, struct qbn_op *);
extern void quest_face_add(struct record *, unsigned char, int, int);
extern void quest_face_remove(int);
extern void func_00030F63(struct quest *, struct qbn_op *);
extern void qaction_op69_cast_spell_on_foe(struct quest *, struct qbn_op *);
extern void func_00031658(struct quest *, struct qbn_op *, int);
extern void qaction_op83_teleport_pc(struct qbn_op *);
extern void quest_show_message(struct quest *, short);
extern void quest_op_done(struct quest *, struct qbn_op *);
extern void qaction_place_foe(struct qbn_op *, int);
extern void qaction_place_item(struct quest *, struct qbn_op *);
extern void qaction_place_npc(struct quest *, struct qbn_op *);
extern void qaction_give_item_to_foe(struct quest *, struct qbn_op *);
extern void msgbox_show_quest_text(struct quest *, short, int);
extern void quest_end(struct quest *);
extern void qaction_op46_hide_npc(struct quest *, struct qbn_op *);
extern void func_0004C874(struct quest *, struct qbn_op *);
extern void qaction_op48_restore_npc(struct quest *, struct qbn_op *);
extern void func_0004C8CF(struct quest *, struct qbn_op *);
extern void func_0004CE24(struct quest *, int);
extern void palette_restore(void);
extern void fatal_error(int);
extern void disease_infect(int, int, int, int);
extern void disease_cure_vampirism(void);
extern void disease_cure_lycanthropy(void);
extern void logbook_add_entry(unsigned char, int, int);
extern void logbook_remove_entry(unsigned char, int);
extern void rest_close(void);
extern void location_reveal(int, int);
extern void spfx_cure_disease(int, int);
extern void inventory_open_container(int, int, int);
extern void travel_button_exit(int);
int func_0002CC40(struct quest *, struct qbn_op *);
void quest_relink_after_load(struct quest *);
void qaction_op11_remove_topics(struct quest *, struct qbn_op *);
void qaction_op10_add_topics(struct quest *, struct qbn_op *);
#pragma aux func_000A0ED9 parm routine [];

void quest_run_opcodes(struct quest *a1)
{
    int l_44;
    struct qbn_op *l_40;
    int l_3C;
    int l_38;
    int l_34;
    struct qbn_state *l_30;
    int l_2C;
    int l_28;
    int l_24;
    struct record *l_20;
    struct faction *l_1C;
    int l_18;
{
    int l_74;
    int l_70;
    int l_6C;
    int l_68;
    int l_64;
    int l_60;
    int l_5C;
    int l_58;
    int l_54;
    int l_50;
    int l_4C;

    l_34 = 0;
    current_quest = a1;
    l_40 = quest_section(a1, 8);
    l_44 = 0;
L2998F:;
    if (a1->section_counts[8] > l_44) goto L29B14;
    goto L2B1F6;
L299A4:;
    l_44++;
    l_40++;
    goto L2998F;
__dagger_tbl299B4:;
L29B14:;
    switch (l_40->opcode) {
case 7:
    if (quest_arg_state(l_40, 0) == 0) goto L29BA1;
    l_24 = 1;
L29B4D:;
    if (l_24 < 5) goto L29B5D;
    goto L29B96;
L29B55:;
    l_24++;
    goto L29B4D;
L29B5D:;
    l_30 = (struct qbn_state *)l_40->args[l_24].record;
    if (l_30 == 0) goto L29B55;
    if (l_30->is_global == 0) goto L29B8D;
    *(signed char *)(quest_global_states + l_30->value) = 0;
    goto L29B94;
L29B8D:;
    l_30->value = 0;
L29B94:;
    goto L29B55;
L29B96:;
    quest_op_done(a1, l_40);
L29BA1:;
    goto L2B1F1;
case 8:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29BC5;
    if (quest_arg_state(l_40, 0) != 0) goto L29BC7;
L29BC5:;
    goto L29BD3;
L29BC7:;
    if (((int)(unsigned char)*(signed char *)current_region) != 31) goto L29BD5;
L29BD3:;
    goto L29C1D;
L29BD5:;
    func_000A0ED9(64, (int)D_001707F0);
    mc_sprintf((int)D_001911E4, (int)D_001707F7, rand_range(l_40->args[1].value, l_40->args[2].value));
    quest_start((int)D_001911E4);
    quest_op_done(a1, l_40);
L29C1D:;
    goto L2B1F1;
case 10:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29C41;
    if (quest_arg_state(l_40, 0) != 0) goto L29C43;
L29C41:;
    goto L29C59;
L29C43:;
    qaction_op10_add_topics(a1, l_40);
    quest_op_done(a1, l_40);
L29C59:;
    goto L2B1F1;
case 6:
    if (quest_arg_state(l_40, 0) == 0) goto L29C78;
    if (func_000317AE(a1) != 0) goto L29C7A;
L29C78:;
    goto L29CA4;
L29C7A:;
    if ((((D_00199768 == 0) || ((D_00199768->children == 0))) ? 1 : 0) != 0) goto L29CA9;
L29CA4:;
    goto L29D72;
L29CA9:;
    quest_op_done(a1, l_40);
    if (l_34 != 0) goto L29D59;
    if (((int)(unsigned char)(a1->flags & 2)) != 0) goto L29CD9;
    if (a1->faction_id != 0) goto L29CDB;
L29CD9:;
    goto L29CF1;
L29CDB:;
    faction_change_reputation(faction_find(a1->faction_id), -2);
L29CF1:;
    rumor_add_quest(a1, ((((int)(unsigned char)(a1->flags & 2)) != 0) ? 1007 : 1006), 0, 8);
    func_0004CE24(a1, (int)(unsigned char)(a1->flags & 2));
    rumor_file_purge();
L29D59:;
    quest_end(a1);
    l_34 = 1;
    l_44 = a1->section_counts[8];
L29D72:;
    goto L2B1F1;
case 11:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29D96;
    if (quest_arg_state(l_40, 0) != 0) goto L29D98;
L29D96:;
    goto L29DAE;
L29D98:;
    qaction_op11_remove_topics(a1, l_40);
    quest_op_done(a1, l_40);
L29DAE:;
    goto L2B1F1;
case 12:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29DD2;
    if (quest_arg_state(l_40, 0) != 0) goto L29DD4;
L29DD2:;
    goto L29DEF;
L29DD4:;
    qaction_op12_start_stop_timer(a1, l_40, 64);
    quest_op_done(a1, l_40);
L29DEF:;
    goto L2B1F1;
case 13:
    if (quest_arg_state(l_40, 0) == 0) goto L29E1A;
    qaction_op12_start_stop_timer(a1, l_40, 0);
    quest_op_done(a1, l_40);
L29E1A:;
    goto L2B1F1;
case 9:
    if (quest_arg_state(l_40, 0) == 0) goto L29E43;
    quest_op09_spawn_repeat(a1, l_40);
    quest_op_done(a1, l_40);
L29E43:;
    goto L2B1F1;
case 87:
    if (quest_arg_state(l_40, 0) == 0) goto L29E6C;
    quest_op87_respawn(a1, l_40);
    quest_op_done(a1, l_40);
L29E6C:;
    goto L2B1F1;
case 4:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29E90;
    if (quest_arg_state(l_40, 0) != 0) goto L29E92;
L29E90:;
    goto L29EDB;
L29E92:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 16) goto L29EAA;
    rest_close();
    *(signed char *)D_0019626F = 0;
L29EAA:;
    a1->flags |= 2;
    quest_op_done(a1, l_40);
    msgbox_show_quest_text(current_quest, 1004, 1);
    qaction_op04_give_reward(a1, l_40);
L29EDB:;
    goto L2B1F1;
case 22:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29EFF;
    if (quest_arg_state(l_40, 0) != 0) goto L29F01;
L29EFF:;
    goto L29F1A;
L29F01:;
    qaction_place_foe(l_40, (int)l_40->args[2].record);
    quest_op_done(a1, l_40);
L29F1A:;
    goto L2B1F1;
case 24:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29F3E;
    if (quest_arg_state(l_40, 0) != 0) goto L29F40;
L29F3E:;
    goto L29F60;
L29F40:;
    logbook_remove_entry((int)(unsigned char)(signed char)a1->id, l_40->args[1].value);
    quest_op_done(a1, l_40);
L29F60:;
    goto L2B1F1;
case 23:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29F84;
    if (quest_arg_state(l_40, 0) != 0) goto L29F86;
L29F84:;
    goto L29FAC;
L29F86:;
    logbook_add_entry((int)(unsigned char)(signed char)a1->id, l_40->args[1].value, l_40->args[2].value);
    quest_op_done(a1, l_40);
L29FAC:;
    goto L2B1F1;
case 25:
    if (quest_arg_state(l_40, 0) == 0) goto L29FD9;
    if (func_0002CC40(a1, l_40) == 0) goto L29FD9;
    quest_op_done(a1, l_40);
L29FD9:;
    goto L2B1F1;
case 26:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L29FFD;
    if (quest_arg_state(l_40, 0) != 0) goto L29FFF;
L29FFD:;
    goto L2A015;
L29FFF:;
    quest_give_item_to_player(l_40->args[1].object);
    quest_op_done(a1, l_40);
L2A015:;
    goto L2B1F1;
case 34:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A039;
    if (quest_arg_state(l_40, 0) != 0) goto L2A03B;
L2A039:;
    goto L2A051;
L2A03B:;
    qaction_op34_pick_one_state(a1, l_40);
    quest_op_done(a1, l_40);
L2A051:;
    goto L2B1F1;
case 35:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A075;
    if (quest_arg_state(l_40, 0) != 0) goto L2A077;
L2A075:;
    goto L2A08D;
L2A077:;
    qaction_op35_cycle_state(a1, l_40);
    quest_op_done(a1, l_40);
L2A08D:;
    goto L2B1F1;
case 19:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A0B1;
    if (quest_arg_state(l_40, 0) != 0) goto L2A0B3;
L2A0B1:;
    goto L2A0CE;
L2A0B3:;
    qaction_op19_reveal_location(a1, l_40, 1);
    quest_op_done(a1, l_40);
L2A0CE:;
    goto L2B1F1;
case 20:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A0F2;
    if (quest_arg_state(l_40, 0) != 0) goto L2A0F4;
L2A0F2:;
    goto L2A10C;
L2A0F4:;
    qaction_op19_reveal_location(a1, l_40, 0);
    quest_op_done(a1, l_40);
L2A10C:;
    goto L2B1F1;
case 0:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A130;
    if (quest_arg_state(l_40, 0) != 0) goto L2A132;
L2A130:;
    goto L2A148;
L2A132:;
    qaction_place_item(a1, l_40);
    quest_op_done(a1, l_40);
L2A148:;
    goto L2B1F1;
case 29:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A16C;
    if (quest_arg_state(l_40, 0) != 0) goto L2A16E;
L2A16C:;
    goto L2A179;
L2A16E:;
    qaction_op29_prompt(a1, l_40);
L2A179:;
    goto L2B1F1;
case 16:
    l_24 = ((unsigned)*(int *)game_minutes) / 1440;
    if (l_24 < l_40->args[1].value) goto L2A1A5;
    if (l_24 <= l_40->args[2].value) goto L2A1A7;
L2A1A5:;
    goto L2A1B9;
L2A1A7:;
    quest_set_state(a1, l_40, 1);
    goto L2A1C6;
L2A1B9:;
    quest_set_state(a1, l_40, 0);
L2A1C6:;
    goto L2B1F1;
case 17:
    if (quest_arg_state(l_40, 0) == 0) goto L2A1EF;
    quest_op17_grant_building_access(a1, l_40);
    quest_op_done(a1, l_40);
L2A1EF:;
    goto L2B1F1;
case 30:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A213;
    if (quest_arg_state(l_40, 0) != 0) goto L2A215;
L2A213:;
    goto L2A22B;
L2A215:;
    qaction_place_npc(a1, l_40);
    quest_op_done(a1, l_40);
L2A22B:;
    goto L2B1F1;
case 31:
    l_24 = ((unsigned)*(int *)game_minutes) % 1440;
    if (l_40->args[1].value >= l_40->args[2].value) goto L2A288;
    if (l_24 < l_40->args[1].value) goto L2A265;
    if (l_24 <= l_40->args[2].value) goto L2A267;
L2A265:;
    goto L2A279;
L2A267:;
    quest_set_state(a1, l_40, 1);
    goto L2A286;
L2A279:;
    quest_set_state(a1, l_40, 0);
L2A286:;
    goto L2A2BF;
L2A288:;
    if (l_24 < l_40->args[1].value) goto L2A29E;
    if (l_24 <= l_40->args[2].value) goto L2A2A0;
L2A29E:;
    goto L2A2AF;
L2A2A0:;
    quest_set_state(a1, l_40, 0);
    goto L2A2BF;
L2A2AF:;
    quest_set_state(a1, l_40, 1);
L2A2BF:;
    goto L2B1F1;
case 33:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A2E3;
    if (quest_arg_state(l_40, 0) != 0) goto L2A2E5;
L2A2E3:;
    goto L2A2FE;
L2A2E5:;
    qaction_place_foe(l_40, (int)l_40->args[2].record);
    quest_op_done(a1, l_40);
L2A2FE:;
    goto L2B1F1;
case 36:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A322;
    if (quest_arg_state(l_40, 0) != 0) goto L2A324;
L2A322:;
    goto L2A355;
L2A324:;
    quest_op_done(a1, l_40);
    if (l_40->args[1].object == 0) goto L2A348;
    func_0003077F(l_40->args[1].object, 1);
L2A348:;
    ((struct qbn_item *)l_40->args[1].record)->object = 0;
L2A355:;
    goto L2B1F1;
case 37:
    if (quest_arg_state(l_40, 0) == 0) goto L2A37E;
    qaction_op37_repute_exceeds(a1, l_40);
    quest_op_done(a1, l_40);
L2A37E:;
    goto L2B1F1;
case 38:
    goto L2B1F1;
case 39:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A3A7;
    if (quest_arg_state(l_40, 0) != 0) goto L2A3A9;
L2A3A7:;
    goto L2A3BF;
L2A3A9:;
    qaction_give_item_to_foe(a1, l_40);
    quest_op_done(a1, l_40);
L2A3BF:;
    goto L2B1F1;
case 42:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A3E3;
    if (quest_arg_state(l_40, 0) != 0) goto L2A3E8;
L2A3E3:;
    goto L2A476;
L2A3E8:;
    if (rand_range(1, 100) >= l_40->args[1].value) goto L2A476;
    if (l_40->args[3].value != 32768) goto L2A425;
    region_flag_set((int)(unsigned char)*(signed char *)current_region, l_40->args[2].value);
    goto L2A46B;
L2A425:;
    if (l_40->args[2].value != l_40->args[3].value) goto L2A447;
    region_flag_clear((int)(unsigned char)*(signed char *)current_region, l_40->args[2].value);
    goto L2A46B;
L2A447:;
    region_flag_clear((int)(unsigned char)*(signed char *)current_region, l_40->args[2].value);
    region_flag_set((int)(unsigned char)*(signed char *)current_region, l_40->args[3].value);
L2A46B:;
    quest_op_done(a1, l_40);
L2A476:;
    goto L2B1F1;
case 43:
    if (quest_arg_state(l_40, 0) == 0) goto L2A498;
    if (qcond_op43_pc_at_place(a1, l_40) != 0) goto L2A49A;
L2A498:;
    goto L2A4A5;
L2A49A:;
    quest_op_done(a1, l_40);
L2A4A5:;
    goto L2B1F1;
case 44:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A4C9;
    if (quest_arg_state(l_40, 0) != 0) goto L2A4CB;
L2A4C9:;
    goto L2A4F3;
L2A4CB:;
    func_0003077F(l_40->args[1].object, 1);
    ((struct qbn_person *)l_40->args[1].record)->object = 0;
    quest_op_done(a1, l_40);
L2A4F3:;
    goto L2B1F1;
case 45:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A517;
    if (quest_arg_state(l_40, 0) != 0) goto L2A519;
L2A517:;
    goto L2A52F;
L2A519:;
    func_0004C874(a1, l_40);
    quest_op_done(a1, l_40);
L2A52F:;
    goto L2B1F1;
case 46:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A553;
    if (quest_arg_state(l_40, 0) != 0) goto L2A555;
L2A553:;
    goto L2A56B;
L2A555:;
    qaction_op46_hide_npc(a1, l_40);
    quest_op_done(a1, l_40);
L2A56B:;
    goto L2B1F1;
case 47:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A58F;
    if (quest_arg_state(l_40, 0) != 0) goto L2A591;
L2A58F:;
    goto L2A5A7;
L2A591:;
    func_0004C8CF(a1, l_40);
    quest_op_done(a1, l_40);
L2A5A7:;
    goto L2B1F1;
case 48:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A5CB;
    if (quest_arg_state(l_40, 0) != 0) goto L2A5CD;
L2A5CB:;
    goto L2A5E3;
L2A5CD:;
    qaction_op48_restore_npc(a1, l_40);
    quest_op_done(a1, l_40);
L2A5E3:;
    goto L2B1F1;
case 49:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A607;
    if (quest_arg_state(l_40, 0) != 0) goto L2A609;
L2A607:;
    goto L2A624;
L2A609:;
    spfx_cure_disease((int)player_entity, (int)player_character);
    quest_op_done(a1, l_40);
L2A624:;
    goto L2B1F1;
case 50:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A648;
    if (quest_arg_state(l_40, 0) != 0) goto L2A64D;
L2A648:;
    goto L2A6E3;
L2A64D:;
    func_000A0ED9(358, (int)D_001707F0);
    mc_sprintf((int)text_buffer, (int)D_00170801, l_40->args[1].value);
L2A677:;
    if (*(signed char *)mouse_buttons == 0) goto L2A687;
    func_0012B136();
    goto L2A677;
L2A687:;
    l_18 = disk_resolve_path((int)text_buffer);
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_001707F0, 362, 4);
    palette_restore();
    *(int *)D_00195D48 = 10000;
    *(signed char *)D_0019629B = 0;
    quest_op_done(a1, l_40);
L2A6E3:;
    goto L2B1F1;
case 51:
    if (quest_arg_state(l_40, 0) == 0) goto L2A701;
    quest_op_done(a1, l_40);
L2A701:;
    goto L2B1F1;
case 53:
    l_3C = 0;
    l_24 = 1;
L2A714:;
    if (l_24 < 5) goto L2A724;
    goto L2A74C;
L2A71C:;
    l_24++;
    goto L2A714;
L2A724:;
    if (l_40->args[l_24].value == (-2)) goto L2A71C;
    if (quest_arg_state(l_40, (int)(short)*(short *)&l_24) == 0) goto L2A74A;
    l_3C = 1;
    goto L2A74C;
L2A74A:;
    goto L2A71C;
L2A74C:;
    quest_set_state(a1, l_40, (int)(short)*(short *)&l_3C);
    goto L2B1F1;
case 52:
    l_24 = 1;
    l_3C = l_24;
L2A76D:;
    if (l_24 < 5) goto L2A77D;
    goto L2A7A5;
L2A775:;
    l_24++;
    goto L2A76D;
L2A77D:;
    if (l_40->args[l_24].value == (-2)) goto L2A775;
    if (quest_arg_state(l_40, (int)(short)*(short *)&l_24) != 0) goto L2A7A3;
    l_3C = 0;
    goto L2A7A5;
L2A7A3:;
    goto L2A775;
L2A7A5:;
    if (l_3C == 0) goto L2A7BA;
    quest_set_state(a1, l_40, (int)(short)*(short *)&l_3C);
L2A7BA:;
    goto L2B1F1;
case 54:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A7DE;
    if (quest_arg_state(l_40, 0) != 0) goto L2A7E0;
L2A7DE:;
    goto L2A805;
L2A7E0:;
    func_0003077F(l_40->args[1].object, 0);
    ((struct qbn_item *)l_40->args[1].record)->object = 0;
    quest_op_done(a1, l_40);
L2A805:;
    goto L2B1F1;
case 55:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A829;
    if (quest_arg_state(l_40, 0) != 0) goto L2A82B;
L2A829:;
    goto L2A878;
L2A82B:;
    l_20 = l_40->args[1].object;
    if (l_20 == 0) goto L2A86D;
    quest_face_add(l_20, (int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned short)(l_20->flags & 4), l_20->id);
L2A86D:;
    quest_op_done(a1, l_40);
L2A878:;
    goto L2B1F1;
case 56:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A89C;
    if (quest_arg_state(l_40, 0) != 0) goto L2A89E;
L2A89C:;
    goto L2A8C3;
L2A89E:;
    l_20 = l_40->args[1].object;
    if (l_20 == 0) goto L2A8B8;
    quest_face_remove(l_20->id);
L2A8B8:;
    quest_op_done(a1, l_40);
L2A8C3:;
    goto L2B1F1;
case 57:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A8E7;
    if (quest_arg_state(l_40, 0) != 0) goto L2A8E9;
L2A8E7:;
    goto L2A8F8;
L2A8E9:;
    if (qcond_op57_item_used(a1, l_40) != 0) goto L2A8FA;
L2A8F8:;
    goto L2A905;
L2A8FA:;
    quest_op_done(a1, l_40);
L2A905:;
    goto L2B1F1;
case 58:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A929;
    if (quest_arg_state(l_40, 0) != 0) goto L2A92B;
L2A929:;
    goto L2A93B;
L2A92B:;
    disease_cure_vampirism();
    quest_op_done(a1, l_40);
L2A93B:;
    goto L2B1F1;
case 59:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A95F;
    if (quest_arg_state(l_40, 0) != 0) goto L2A961;
L2A95F:;
    goto L2A971;
L2A961:;
    disease_cure_lycanthropy();
    quest_op_done(a1, l_40);
L2A971:;
    goto L2B1F1;
case 60:
    if (quest_arg_state(l_40, 0) == 0) goto L2A9A5;
    sound_play(l_40->args[1].value, (int)player_object, 110);
    quest_op_done(a1, l_40);
L2A9A5:;
    goto L2B1F1;
case 61:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2A9C9;
    if (quest_arg_state(l_40, 0) != 0) goto L2A9CE;
L2A9C9:;
    goto L2AA3B;
L2A9CE:;
    l_20 = l_40->args[1].object;
    if (l_20 == 0) goto L2A9EC;
    if (l_20->type == 65) goto L2A9EE;
L2A9EC:;
    goto L2AA07;
L2A9EE:;
    faction_change_reputation(faction_find((int)(short)*(short *)((char *)l_20 + 25)), l_40->args[2].value);
    goto L2AA30;
L2AA07:;
    if (l_20 == 0) goto L2AA17;
    if (*(short *)((char *)l_20 + 89) != 0) goto L2AA19;
L2AA17:;
    goto L2AA30;
L2AA19:;
    faction_change_reputation(faction_find((int)(short)*(short *)((char *)l_20 + 89)), l_40->args[2].value);
L2AA30:;
    quest_op_done(a1, l_40);
L2AA3B:;
    goto L2B1F1;
case 62:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AA5F;
    if (quest_arg_state(l_40, 0) != 0) goto L2AA61;
L2AA5F:;
    goto L2AAA4;
L2AA61:;
    if (l_40->args[1].value != 32768) goto L2AA86;
    *(signed char *)(region_precipitation_override + (((int)(unsigned char)*(signed char *)current_region) * 80)) = (signed char)l_40->args[2].value;
    goto L2AA99;
L2AA86:;
    *(signed char *)(region_precipitation_override + (l_40->args[1].value * 80)) = (signed char)l_40->args[2].value;
L2AA99:;
    quest_op_done(a1, l_40);
L2AAA4:;
    goto L2B1F1;
case 63:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AAC8;
    if (quest_arg_state(l_40, 0) != 0) goto L2AACA;
L2AAC8:;
    goto L2AB06;
L2AACA:;
    l_20 = l_40->args[1].object;
    if (l_20 == 0) goto L2AAFB;
    quest_face_add(l_20, (int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), 0, l_20->image2);
L2AAFB:;
    quest_op_done(a1, l_40);
L2AB06:;
    goto L2B1F1;
case 64:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AB2A;
    if (quest_arg_state(l_40, 0) != 0) goto L2AB2C;
L2AB2A:;
    goto L2AB51;
L2AB2C:;
    l_20 = l_40->args[1].object;
    if (l_20 == 0) goto L2AB46;
    quest_face_remove(l_20->id);
L2AB46:;
    quest_op_done(a1, l_40);
L2AB51:;
    goto L2B1F1;
case 65:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AB75;
    if (quest_arg_state(l_40, 0) != 0) goto L2AB77;
L2AB75:;
    goto L2ABEC;
L2AB77:;
    *(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80)) += (short)l_40->args[1].value;
    if (((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80))) <= 100) goto L2ABB8;
    *(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80)) = 100;
L2ABB8:;
    if (((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80))) >= (-100)) goto L2ABE1;
    *(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80)) = 65436;
L2ABE1:;
    quest_op_done(a1, l_40);
L2ABEC:;
    goto L2B1F1;
case 66:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AC10;
    if (quest_arg_state(l_40, 0) != 0) goto L2AC12;
L2AC10:;
    goto L2AC2B;
L2AC12:;
    a1->text_file = (short)l_40->args[1].value;
    quest_op_done(a1, l_40);
L2AC2B:;
    goto L2B1F1;
case 67:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AD35;
    l_2C = l_40->args[0].value + ((int)quest_global_states);
    l_28 = l_40->args[1].value + ((int)quest_global_states);
    l_24 = 0;
L2AC6C:;
    if (l_24 >= 8) goto L2AC8E;
    if (*(signed char *)((char *)(l_2C + l_24)) == 0) goto L2AC86;
    l_38 = l_24 * 19;
    goto L2AC8E;
L2AC86:;
    l_24++;
    goto L2AC6C;
L2AC8E:;
    if (l_24 != 8) goto L2ACA7;
    a1->text_file = (short)l_40->args[3].value;
    goto L2B1F1;
L2ACA7:;
    l_24 = 0;
L2ACAE:;
    if (l_24 >= 10) goto L2ACE3;
    if (*(signed char *)((char *)(l_28 + l_24)) == 0) goto L2ACDB;
    if (l_24 >= 9) goto L2ACCF;
    l_38 = l_24 * 2;
    goto L2ACD9;
L2ACCF:;
    if (l_24 != 9) goto L2ACD9;
    l_38 += 19;
L2ACD9:;
    goto L2ACE3;
L2ACDB:;
    l_24++;
    goto L2ACAE;
L2ACE3:;
    if (l_24 != 10) goto L2ACFC;
    a1->text_file = (short)l_40->args[3].value;
    goto L2B1F1;
L2ACFC:;
    if (l_24 == 9) goto L2AD1F;
    if (faction_find(*(short *)(D_0017A120 + (l_24 * 2)))->reputation >= l_40->args[2].value) goto L2AD21;
L2AD1F:;
    goto L2AD27;
L2AD21:;
    l_38++;
L2AD27:;
    a1->text_file += (short)l_40->args[3].value;
L2AD35:;
    goto L2B1F1;
case 68:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AD59;
    if (quest_arg_state(l_40, 0) != 0) goto L2AD5B;
L2AD59:;
    goto L2AD71;
L2AD5B:;
    func_00030F63(a1, l_40);
    quest_op_done(a1, l_40);
L2AD71:;
    goto L2B1F1;
case 69:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AD95;
    if (quest_arg_state(l_40, 0) != 0) goto L2AD97;
L2AD95:;
    goto L2ADAD;
L2AD97:;
    qaction_op69_cast_spell_on_foe(a1, l_40);
    quest_op_done(a1, l_40);
L2ADAD:;
    goto L2B1F1;
case 70:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2ADD1;
    if (quest_arg_state(l_40, 0) != 0) goto L2ADD3;
L2ADD1:;
    goto L2ADED;
L2ADD3:;
    if (qcond_op70_player_has_items(a1, l_40) == 0) goto L2ADED;
    quest_op_done(a1, l_40);
L2ADED:;
    goto L2B1F1;
case 72:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AE11;
    if (quest_arg_state(l_40, 0) != 0) goto L2AE13;
L2AE11:;
    goto L2AE35;
L2AE13:;
    disease_infect((int)player_entity, 0, l_40->args[1].value, 1);
    quest_op_done(a1, l_40);
L2AE35:;
    goto L2B1F1;
case 74:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AEA5;
    if (location_contains(player_object->x, player_object->z) == 0) goto L2AEA5;
    if (current_location->kind < l_40->args[1].value) goto L2AE88;
    if (current_location->kind <= l_40->args[2].value) goto L2AE8A;
L2AE88:;
    goto L2AEA5;
L2AE8A:;
    quest_set_state(a1, l_40, 1);
    quest_op_done(a1, l_40);
L2AEA5:;
    goto L2B1F1;
case 27:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AEC9;
    if (quest_arg_state(l_40, 0) != 0) goto L2AECB;
L2AEC9:;
    goto L2AEE7;
L2AECB:;
    location_reveal(l_40->args[2].value, l_40->args[3].value);
    quest_op_done(a1, l_40);
L2AEE7:;
    goto L2B1F1;
case 75:
    if (quest_arg_state(l_40, 0) == 0) goto L2AF26;
    *(int *)D_00195984 = (int)l_40->args[1].record;
    *(int *)D_00195988 = (int)l_40->args[2].record;
    *(int *)D_0019598C = (int)l_40->args[3].record;
    *(int *)D_00195990 = (int)l_40->args[4].record;
L2AF26:;
    goto L2B1F1;
case 76:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2AF4A;
    if (quest_arg_state(l_40, 0) != 0) goto L2AF4C;
L2AF4A:;
    goto L2AF81;
L2AF4C:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 19) goto L2AF74;
    travel_button_exit(100);
    func_00031658(a1, l_40, 1);
    goto L2AF81;
L2AF74:;
    func_00031658(a1, l_40, 0);
L2AF81:;
    goto L2B1F1;
case 77:
    quest_set_arg_state(a1, l_40, 0, ((player_character->level >= l_40->args[1].value) ? 1 : 0));
    goto L2B1F1;
case 79:
    if (quest_arg_state(l_40, 0) != 0) goto L2B017;
    if (*(signed char *)D_00196298 == 0) goto L2B017;
    *(signed char *)D_00196298 = 0;
    l_1C = faction_find((short)l_40->args[1].value);
    if (l_1C->reputation < l_40->args[2].value) goto L2B017;
    quest_set_arg_state(a1, l_40, 0, 1);
L2B017:;
    goto L2B1F1;
case 80:
    if (quest_arg_state(l_40, 0) == 0) goto L2B039;
    quest_show_message(a1, l_40->message);
L2B039:;
    goto L2B1F1;
case 81:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2B05D;
    if (quest_arg_state(l_40, 0) != 0) goto L2B05F;
L2B05D:;
    goto L2B085;
L2B05F:;
    if (l_40->args[1].record == 0) goto L2B072;
    ((struct qbn_item *)l_40->args[1].record)->flags |= 64;
L2B072:;
    if (l_40->args[2].record == 0) goto L2B085;
    ((struct qbn_person *)l_40->args[2].record)->flags |= 64;
L2B085:;
    goto L2B1F1;
case 82:
    if (quest_arg_state(l_40, 0) == 0) goto L2B0A4;
    ((struct qbn_person *)l_40->args[1].record)->flags |= 32;
    goto L2B0AE;
L2B0A4:;
    ((struct qbn_person *)l_40->args[1].record)->flags &= 223;
L2B0AE:;
    goto L2B1F1;
case 83:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2B0D2;
    if (quest_arg_state(l_40, 0) != 0) goto L2B0D4;
L2B0D2:;
    goto L2B0DC;
L2B0D4:;
    qaction_op83_teleport_pc(l_40);
L2B0DC:;
    goto L2B1F1;
case 84:
    if (quest_arg_state(l_40, 0) == 0) goto L2B15A;
    if (*(int *)game_minutes == l_40->last_minutes) goto L2B110;
    if ((((unsigned)*(int *)game_minutes) % l_40->args[2].value) == 0) goto L2B112;
L2B110:;
    goto L2B12B;
L2B112:;
    if (rand_range(1, 100) <= l_40->args[3].value) goto L2B12D;
L2B12B:;
    goto L2B15A;
L2B12D:;
    l_40->last_minutes = *(int *)game_minutes;
    sound_play(l_40->args[1].value, (int)player_object, 110);
    quest_op_done(a1, l_40);
L2B15A:;
    goto L2B1F1;
case 85:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2B17E;
    if (quest_arg_state(l_40, 0) != 0) goto L2B180;
L2B17E:;
    goto L2B1A7;
L2B180:;
    if (l_40->args[1].object == 0) goto L2B19C;
    faction_find((int)(short)*(short *)((char *)l_40->args[1].object + 25))->flags |= 0x200;
L2B19C:;
    quest_op_done(a1, l_40);
L2B1A7:;
    goto L2B1F1;
case 86:
    if (((int)(short)(l_40->flags & 1)) != 0) goto L2B1C8;
    if (quest_arg_state(l_40, 0) != 0) goto L2B1CA;
L2B1C8:;
    goto L2B1F1;
L2B1CA:;
    if (l_40->args[1].object == 0) goto L2B1E6;
    faction_find((int)(short)*(short *)((char *)l_40->args[1].object + 25))->flags &= ~0x200;
L2B1E6:;
    quest_op_done(a1, l_40);
default:
L2B1F1:;
    goto L299A4;
L2B1F6:;
    *(int *)D_00195984 = (*(int *)D_00195988 = (*(int *)D_0019598C = (*(int *)D_00195990 = 0)));
    if (l_34 != 0) goto L2B22C;
    quest_timers_update(a1);
L2B22C:;
    if (D_00199768 == 0) goto L2B240;
    if (D_00199768->children != 0) goto L2B242;
L2B240:;
    goto L2B24E;
L2B242:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 4) goto L2B250;
L2B24E:;
    return;
L2B250:;
    inventory_open_container((int)D_00199768, 0, 6);
}
}
}

int quest_dispatch_event(struct quest *a1)
{
    struct qbn_op *l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_30 = quest_section(a1, 8);
    l_28 = 0;
    l_24 = 0;
    l_2C = 0;
L2B2A1:;
    if (a1->section_counts[8] > l_2C) goto L2B2C1;
    goto L2B5BA;
L2B2B2:;
    l_2C++;
    l_30++;
    goto L2B2A1;
L2B2C1:;
    if (l_30->opcode != *(short *)quest_event_code) goto L2B5B5;
    switch (l_30->opcode) {
    goto L2B5B5;
case 71:
    if (((int)(short)(l_30->flags & 1)) != 0) goto L2B43A;
    if (quest_event_object->twin != l_30->args[3].object) goto L2B5B5;
    if (((unsigned)player_character->gold) < l_30->args[2].value) goto L2B416;
    player_character->gold -= l_30->args[2].value;
    quest_set_arg_state(a1, l_30, 0, 1);
    quest_set_arg_state(a1, l_30, 1, 0);
    l_30->flags |= 1;
    quest_op_done(a1, l_30);
    l_28 = 1;
    l_24 = 1;
    l_1C = (int)l_30->args[3].record;
    goto L2B43A;
L2B416:;
    quest_set_arg_state(a1, l_30, 0, 0);
    quest_set_arg_state(a1, l_30, 1, 1);
L2B43A:;
    goto L2B5B5;
case 28:
    if (qcond_op28_event_person(a1, l_30) == 0) goto L2B470;
    quest_op_done(a1, l_30);
    l_28 = 1;
    l_24 = 1;
    l_1C = (int)l_30->args[1].record;
L2B470:;
    goto L2B5B5;
case 3:
    if (qcond_op03_event_object(a1, l_30) == 0) goto L2B48F;
    quest_op_done(a1, l_30);
L2B48F:;
    goto L2B5B5;
case 5:
    if (qcond_op05_event_at_place(a1, l_30) == 0) goto L2B4AE;
    quest_op_done(a1, l_30);
L2B4AE:;
    goto L2B5B5;
case 1:
    if (qcond_op01_item_given_to_npc(a1, l_30) == 0) goto L2B4E4;
    quest_op_done(a1, l_30);
    l_28 = 1;
    l_24 = 1;
    l_1C = (int)l_30->args[2].record;
L2B4E4:;
    goto L2B5B5;
case 2:
    if (qcond_op02_event_count(a1, l_30) == 0) goto L2B503;
    quest_op_done(a1, l_30);
L2B503:;
    goto L2B5B5;
case 21:
    if (qcond_op21_event_same_kind(a1, l_30) == 0) goto L2B522;
    quest_op_done(a1, l_30);
L2B522:;
    goto L2B5B5;
case 73:
    if (((int)(short)(l_30->flags & 1)) != 0) goto L2B546;
    if (quest_arg_state(l_30, 0) != 0) goto L2B548;
L2B546:;
    goto L2B579;
L2B548:;
    if (quest_event_object->data.spell.id != l_30->args[2].value) goto L2B579;
    quest_set_arg_state(a1, l_30, 1, 1);
    l_30->flags |= 1;
L2B579:;
    goto L2B5B5;
case 78:
    if (((int)(unsigned short)*(short *)((char *)quest_event_object + 71)) != l_30->args[1].value) goto L2B5B5;
    quest_set_arg_state(a1, l_30, 1, 1);
    quest_op_done(a1, l_30);
    l_28 = 1;
default:
L2B5B5:;
    goto L2B2B2;
L2B5BA:;
    if (l_24 == 0) goto L2B5D1;
    if (((int)(short)(*(short *)((char *)l_1C + 2) & 8192)) != 0) goto L2B5D3;
L2B5D1:;
    goto L2B5DC;
L2B5D3:;
    return 0;
L2B5DC:;
    return l_28;
}
}

void quest_debug_next(void)
{
    struct record *l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
    l_20 = D_00195A00->children;
    if (l_20 == 0) goto L2BBEB;
    if (*(int *)quest_debug_object == 0) goto L2BBED;
L2BBEB:;
    goto L2BC05;
L2BBED:;
    *(int *)quest_debug_object = (int)l_20;
    *(int *)quest_debug_data = (int)&l_20->data.quest;
    return;
L2BC05:;
    if (l_20 == 0) goto L2BC52;
    if (l_20->type != 14) goto L2BC47;
    if (l_18 == 0) goto L2BC35;
    *(int *)quest_debug_object = (int)l_20;
    *(int *)quest_debug_data = (int)&l_20->data.quest;
    return;
L2BC35:;
    if ((int)l_20 != *(int *)quest_debug_object) goto L2BC47;
    l_18 = 1;
L2BC47:;
    l_20 = l_20->next;
    goto L2BC05;
L2BC52:;
    if (l_18 == 0) return;
    l_20 = D_00195A00->children;
    *(int *)quest_debug_object = (int)l_20;
    *(int *)quest_debug_data = (int)&l_20->data.quest;
}

void quests_unlink_all(struct record *a1)
{
    a1 = a1->children;
L2BC9A:;
    if (a1 == 0) return;
    if (a1->type != 14) goto L2BCBA;
    quest_unlink_for_save(&a1->data.quest);
L2BCBA:;
    a1 = a1->next;
    goto L2BC9A;
}

void quests_relink_all(struct record *a1)
{
    a1 = a1->children;
L2BCE9:;
    if (a1 == 0) return;
    if (a1->type != 14) goto L2BD09;
    quest_relink_after_load(&a1->data.quest);
L2BD09:;
    a1 = a1->next;
    goto L2BCE9;
}

void quest_relink_after_load(struct quest *a1)
{
    struct qbn_op *l_48;
    struct qbn_arg *l_44;
    int l_40;
    int l_3C;
    struct qbn_place *l_38;
    struct qbn_person *l_34;
    struct qbn_item *l_30;
    struct qbn_foe *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct qbn_timer *l_18;

    l_48 = (struct qbn_op *)((int)a1 + a1->section_offsets[8]);
    l_1C = 0;
L2C0CF:;
    if (a1->section_counts[8] > l_1C) goto L2C0EF;
    goto L2C171;
L2C0E0:;
    l_1C++;
    l_48++;
    goto L2C0CF;
L2C0EF:;
    l_44 = l_48->args;
    l_48->arg_count = ((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)qbn_opcode_arg_counts + l_48->opcode))) - 48;
    l_20 = 0;
L2C11A:;
    if (l_48->arg_count > l_20) goto L2C137;
    goto L2C16C;
L2C128:;
    l_20++;
    l_44++;
    goto L2C11A;
L2C137:;
    if (l_44->record == 0) goto L2C149;
    l_44->record += (int)a1;
L2C149:;
    if (l_44->object == 0) goto L2C16A;
    l_44->object = (struct record *)object_find_by_id(nonworld_root, (int)l_44->object);
L2C16A:;
    goto L2C128;
L2C16C:;
    goto L2C0E0;
L2C171:;
    l_34 = (struct qbn_person *)((int)a1 + a1->section_offsets[3]);
    l_1C = 0;
L2C187:;
    if (a1->section_counts[3] > l_1C) goto L2C1A7;
    goto L2C207;
L2C198:;
    l_1C++;
    l_34++;
    goto L2C187;
L2C1A7:;
    if (l_34->object == 0) goto L2C205;
    l_34->object = (struct record *)object_find_by_id(nonworld_root, (int)l_34->object);
    if (l_34->object == 0) goto L2C1E3;
    if (l_34->object->type == 65) goto L2C1E5;
L2C1E3:;
    goto L2C1F2;
L2C1E5:;
    if (*(short *)((char *)l_34->object + 25) == 0) goto L2C1F4;
L2C1F2:;
    goto L2C205;
L2C1F4:;
    *(short *)((char *)l_34->object + 25) = l_34->faction_id;
L2C205:;
    goto L2C198;
L2C207:;
    l_38 = (struct qbn_place *)((int)a1 + a1->section_offsets[4]);
    l_1C = 0;
L2C21D:;
    if (a1->section_counts[4] > l_1C) goto L2C23A;
    goto L2C270;
L2C22B:;
    l_1C++;
    l_38++;
    goto L2C21D;
L2C23A:;
    if (l_38->object == 0) goto L2C26E;
    l_38->object = (struct record *)object_find_by_id(nonworld_root, (int)l_38->object);
    if (l_38->object != 0) goto L2C26E;
    fatal_error((int)D_001708ED);
L2C26E:;
    goto L2C22B;
L2C270:;
    l_30 = (struct qbn_item *)((int)a1 + a1->section_offsets[0]);
    l_1C = 0;
L2C286:;
    if (a1->section_counts[0] > l_1C) goto L2C2A3;
    goto L2C2E7;
L2C294:;
    l_1C++;
    l_30++;
    goto L2C286;
L2C2A3:;
    if (l_30->object == 0) goto L2C2C4;
    l_30->object = (struct record *)object_find_by_id(nonworld_root, (int)l_30->object);
L2C2C4:;
    if (l_30->object != 0) goto L2C2E5;
    l_30->object = (struct record *)object_find_by_id(D_00195AC4, (int)l_30->object);
L2C2E5:;
    goto L2C294;
L2C2E7:;
    l_2C = (struct qbn_foe *)((int)a1 + a1->section_offsets[7]);
    l_1C = 0;
L2C2FD:;
    if (a1->section_counts[7] > l_1C) goto L2C31A;
    goto L2C33D;
L2C30B:;
    l_1C++;
    l_2C++;
    goto L2C2FD;
L2C31A:;
    if (l_2C->object == 0) goto L2C33B;
    l_2C->object = (struct record *)object_find_by_id(nonworld_root, (int)l_2C->object);
L2C33B:;
    goto L2C30B;
L2C33D:;
    l_18 = (struct qbn_timer *)((int)a1 + a1->section_offsets[6]);
    l_1C = 0;
L2C353:;
    if (a1->section_counts[6] > l_1C) goto L2C370;
    goto L2C3B4;
L2C361:;
    l_1C++;
    l_18++;
    goto L2C353;
L2C370:;
    if (l_18->link1 == 0) goto L2C391;
    l_18->link1 = (int)object_find_by_id(nonworld_root, l_18->link1);
L2C391:;
    if (l_18->link2 == 0) goto L2C3B2;
    l_18->link2 = (int)object_find_by_id(nonworld_root, l_18->link2);
L2C3B2:;
    goto L2C361;
L2C3B4:;
    if (*(int *)((char *)a1 + 56) == 0) return;
    l_3C = (int)a1 + *(int *)((char *)a1 + 56);
L2C3C9:;
    if (*(signed char *)((char *)l_3C) == 0) return;
    *(int *)((char *)l_3C + 23) = (int)quest_record(a1, (int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)l_3C + 20)), (int)(short)*(short *)((char *)l_3C + 21));
    (*(char (**)[27])&l_3C)++;
    goto L2C3C9;
}

int quest_event_clicked_faction(unsigned short a1)
{
    struct record *l_2C;
    struct record *l_28;
    struct qbn_op *l_30;
    short l_20;
    short l_1C;

    *(int *)&l_1C = 0;
    l_2C = D_00195A00->children;
L2C429:;
    if (l_2C == 0) goto L2C4E7;
    l_28 = l_2C->next;
    if (l_2C->type != 14) goto L2C4DC;
    *(int *)D_00195D00 = (int)l_2C;
    current_quest = (struct quest *)((*(int *)D_00199780 = (int)&l_2C->data.quest));
    l_30 = quest_section(current_quest, 8);
    *(int *)&l_20 = 0;
L2C485:;
    if (current_quest->section_counts[8] > *(int *)&l_20) goto L2C4A4;
    goto L2C4DC;
L2C495:;
    (*(int *)&l_20)++;
    l_30++;
    goto L2C485;
L2C4A4:;
    if (l_30->opcode != 78) goto L2C4BD;
    if (((int)(unsigned short)a1) == l_30->args[1].value) goto L2C4BF;
L2C4BD:;
    goto L2C4DA;
L2C4BF:;
    quest_set_arg_state(current_quest, l_30, 0, 1);
    *(int *)&l_1C = 1;
L2C4DA:;
    goto L2C495;
L2C4DC:;
    l_2C = l_28;
    goto L2C429;
L2C4E7:;
    return *(int *)&l_1C;
}

int func_0002C96B(struct quest *a1, int a2, short a3)
{
    if (a3 != 0) goto L2C9A0;
    return *(int *)((char *)quest_record(a1, 4, (int)(short)*(short *)&a2) + 16);
L2C9A0:;
    return *(int *)((char *)quest_record(a1, 3, (int)(short)*(short *)&a2) + 12);
}

void quest_timer_expire(struct quest *a1, struct qbn_timer *a2)
{
    struct qbn_state *l_1C;
    int l_18;
    int l_14;

    if (((int)(short)(a2->flags & 3)) == 0) goto L2C9EF;
    a2->flags |= 128;
    goto L2CA07;
L2C9EF:;
    if (((int)(short)(a2->flags & 4)) == 0) goto L2CA07;
    a2->flags ^= 128;
L2CA07:;
    if (((int)(short)(a2->flags & 8)) == 0) goto L2CA2A;
    quest_timer_update(a1, a2, 1);
    goto L2CA31;
L2CA2A:;
    a2->flags &= ~0x40;
L2CA31:;
    l_18 = a1->section_counts[9];
    l_1C = quest_record(a1, 9, 0);
L2CA4D:;
    if (l_18 == 0) return;
    if (l_1C->name_hash != a2->state_hash) goto L2CA98;
    l_14 = (int)(short)(a2->flags & 128);
    if (l_1C->is_global == 0) goto L2CA8D;
    *(signed char *)(quest_global_states + l_1C->value) = *(signed char *)&l_14;
    goto L2CA96;
L2CA8D:;
    l_1C->value = *(signed char *)&l_14;
L2CA96:;
    return;
L2CA98:;
    l_1C++;
    l_18--;
    goto L2CA4D;
}

void func_0002CAB0(struct qbn_timer *a1)
{
    int l_1C;
    struct qbn_state *l_18;

    l_1C = current_quest->section_counts[9];
    l_18 = quest_record(current_quest, 9, 0);
L2CAE1:;
    if (l_1C == 0) return;
    if (l_18->name_hash != a1->state_hash) goto L2CB1B;
    if (l_18->is_global == 0) goto L2CB12;
    *(signed char *)(quest_global_states + l_18->value) = 0;
    goto L2CB19;
L2CB12:;
    l_18->value = 0;
L2CB19:;
    return;
L2CB1B:;
    l_18++;
    l_1C--;
    goto L2CAE1;
}

int func_0002CBA6(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_timer *l_20;
    struct qbn_state *l_1C;
    short l_14;

    if (a2->args[0].value != (-1)) goto L2CBCB;
    return 0;
L2CBCB:;
    l_20 = quest_record(a1, 6, (short)a2->args[1].value);
    l_1C = quest_record(a1, 9, (short)a2->args[0].value);
    l_14 = l_20->flags & 128;
    if (l_1C->is_global == 0) goto L2CC24;
    *(signed char *)(quest_global_states + l_1C->value) = *(signed char *)&l_14;
    goto L2CC2D;
L2CC24:;
    l_1C->value = *(signed char *)&l_14;
L2CC2D:;
    return (int)(short)l_14;
}

int func_0002CC40(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_arg *l_1C;
    struct qbn_state *l_18;

    l_1C = &a2->args[2];
    l_18 = (struct qbn_state *)a2->args[1].record;
    if (l_18->is_global == 0) goto L2CC90;
    if (*(signed char *)(quest_global_states + l_18->value) == 0) goto L2CC8E;
    return 0;
L2CC8E:;
    goto L2CCA2;
L2CC90:;
    if (l_18->value == 0) goto L2CCA2;
    return 0;
L2CCA2:;
    if (l_1C->value == 0) goto L2CCB1;
    l_1C->value--;
L2CCB1:;
    if (l_1C->value != 0) goto L2CCF9;
    if (a2->args[1].value != (-1)) goto L2CCCC;
    return 1;
L2CCCC:;
    if (l_18->is_global == 0) goto L2CCE9;
    *(signed char *)(quest_global_states + l_18->value) = 1;
    goto L2CCF0;
L2CCE9:;
    l_18->value = 1;
L2CCF0:;
    return 1;
L2CCF9:;
    return 0;
}

void qaction_op11_remove_topics(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct qbn_place *l_1C;
    struct qbn_person *l_18;
    struct qbn_item *l_14;

    l_20 = 1;
L2CD26:;
    if (l_20 < 4) goto L2CD39;
    return;
L2CD31:;
    l_20++;
    goto L2CD26;
L2CD39:;
    if (a2->args[l_20].value == (-1)) goto L2CD31;
    switch ((unsigned)l_20) {
case 1:
    l_1C = quest_record(a1, 4, (short)a2->args[l_20].value);
    l_1C->flags |= 128;
    goto L2CDC7;
case 2:
    l_18 = quest_record(a1, 3, (short)a2->args[l_20].value);
    l_18->flags |= 128;
    goto L2CDC7;
case 3:
    l_14 = quest_record(a1, 0, (short)a2->args[l_20].value);
    l_14->flags |= 128;
default:
L2CDC7:;
    goto L2CD31;
}
}

void qaction_op10_add_topics(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct qbn_place *l_1C;
    struct qbn_person *l_18;
    struct qbn_item *l_14;

    l_20 = 1;
L2CDEF:;
    if (l_20 < 4) goto L2CE02;
    return;
L2CDFA:;
    l_20++;
    goto L2CDEF;
L2CE02:;
    if (a2->args[l_20].value == (-1)) goto L2CDFA;
    switch ((unsigned)l_20) {
case 1:
    l_1C = quest_record(a1, 4, (short)a2->args[l_20].value);
    l_1C->flags &= 127;
    goto L2CE90;
case 2:
    l_18 = quest_record(a1, 3, (short)a2->args[l_20].value);
    l_18->flags &= 127;
    goto L2CE90;
case 3:
    l_14 = quest_record(a1, 0, (short)a2->args[l_20].value);
    l_14->flags &= 127;
default:
L2CE90:;
    goto L2CDFA;
}
}
