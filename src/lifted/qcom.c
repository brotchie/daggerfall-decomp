/* qcom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern char D_001707F0[];
extern char D_001707F7[];
extern char D_00170801[];
extern char D_001708ED[];
extern short D_0017A120[];
extern signed char D_001841E3[];
extern signed char region_precipitation_override[];
extern char region_legal_reputation[];
extern signed char text_buffer[];
extern char D_001911E4[];
extern signed char quest_global_states[];
extern char D_00195984[];
extern int D_00195988;
extern int D_0019598C;
extern int D_00195990;
extern struct record *nonworld_root;
extern struct record *quest_root;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern struct record *quest_tick_object;
extern int sky_loaded_frame;
extern int qbn_opcode_arg_counts;
extern signed char current_region;
extern unsigned char D_0019626F;
extern signed char game_mode;
extern signed char D_00196298;
extern signed char night_sky_loaded;
extern int quest_debug_object;
extern struct quest *current_quest;
extern struct record *quest_reward_container;
extern int quest_debug_data;
extern struct record *quest_event_object;
extern struct quest *quest_tick_data;
extern short quest_event_code;

extern struct faction *faction_find(short);
extern int qcond_op05_item_dropped_at_place(struct quest *, struct qbn_op *);
extern int qcond_op43_pc_at_place(struct quest *, struct qbn_op *);
extern int qcond_op01_item_given_to_npc(struct quest *, struct qbn_op *);
extern int qcond_op03_item_found(struct quest *, struct qbn_op *);
extern int qcond_op21_foe_hurt(struct quest *, struct qbn_op *);
extern int qcond_op02_foe_killed(struct quest *, struct qbn_op *);
extern int qcond_op28_npc_clicked(struct quest *, struct qbn_op *);
extern int qcond_op70_player_has_items(struct quest *, struct qbn_op *);
extern int quest_arg_state(struct qbn_op *, int);
extern void *quest_section(struct quest *, int);
extern void *quest_record(struct quest *, int, int);
extern int qcond_op57_item_used(struct quest *, struct qbn_op *);
extern int quest_deliveries_done(struct quest *);
extern int quest_start(int);
extern int sound_play(int, int, int);
extern int disk_resolve_path(int);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int mc_memset();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int xn_vid_play();
extern int xn_mouse_poll_clamped();
extern void region_flag_set(int, int);
extern void region_flag_clear(int, int);
extern void faction_change_reputation(struct faction *, int);
extern void rumor_add_quest(struct quest *, int, int, int);
extern void rumor_file_purge(void);
extern void qaction_op17_grant_building_access(struct quest *, struct qbn_op *);
extern void qaction_op09_spawn_repeat(struct quest *, struct qbn_op *);
extern void qaction_op87_respawn(struct quest *, struct qbn_op *);
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
extern void quest_add_questor_rumor(struct quest *, int);
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
int qaction_op25_countdown(struct quest *, struct qbn_op *);
void quest_relink_after_load(struct quest *);
void qaction_op11_remove_topics(struct quest *, struct qbn_op *);
void qaction_op10_add_topics(struct quest *, struct qbn_op *);
#pragma aux mc_set_location parm routine [];

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
        for (l_44 = 0; a1->section_counts[8] > l_44; l_44++, l_40++) {
            switch (l_40->opcode) {
            case 7:
                if (quest_arg_state(l_40, 0) != 0) {
                    for (l_24 = 1; l_24 < 5; l_24++) {
                        l_30 = (struct qbn_state *)l_40->args[l_24].record;
                        if (l_30 == 0) continue;
                        if (l_30->is_global != 0) {
                            quest_global_states[l_30->value] = 0;
                        } else {
                            l_30->value = 0;
                        }
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 8:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0 && ((int)(unsigned char)current_region) != 31) {
                    mc_set_location(64, (int)D_001707F0);
                    mc_sprintf((int)D_001911E4, (int)D_001707F7, rand_range(l_40->args[1].value, l_40->args[2].value));
                    quest_start((int)D_001911E4);
                    quest_op_done(a1, l_40);
                }
                break;
            case 10:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op10_add_topics(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 6:
                if (quest_arg_state(l_40, 0) != 0 && quest_deliveries_done(a1) != 0 && (((quest_reward_container == 0) || ((quest_reward_container->children == 0))) ? 1 : 0) != 0) {
                    quest_op_done(a1, l_40);
                    if (l_34 == 0) {
                        if (((int)(unsigned char)(a1->flags & 2)) == 0 && a1->faction_id != 0) {
                            faction_change_reputation(faction_find(a1->faction_id), -2);
                        }
                        rumor_add_quest(a1, ((((int)(unsigned char)(a1->flags & 2)) != 0) ? 1007 : 1006), 0, 8);
                        quest_add_questor_rumor(a1, (int)(unsigned char)(a1->flags & 2));
                        rumor_file_purge();
                    }
                    quest_end(a1);
                    l_34 = 1;
                    l_44 = a1->section_counts[8];
                }
                break;
            case 11:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op11_remove_topics(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 12:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op12_start_stop_timer(a1, l_40, 64);
                    quest_op_done(a1, l_40);
                }
                break;
            case 13:
                if (quest_arg_state(l_40, 0) != 0) {
                    qaction_op12_start_stop_timer(a1, l_40, 0);
                    quest_op_done(a1, l_40);
                }
                break;
            case 9:
                if (quest_arg_state(l_40, 0) != 0) {
                    qaction_op09_spawn_repeat(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 87:
                if (quest_arg_state(l_40, 0) != 0) {
                    qaction_op87_respawn(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 4:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (((int)(unsigned char)game_mode) == 16) {
                        rest_close();
                        D_0019626F = 0;
                    }
                    a1->flags |= 2;
                    quest_op_done(a1, l_40);
                    msgbox_show_quest_text(current_quest, 1004, 1);
                    qaction_op04_give_reward(a1, l_40);
                }
                break;
            case 22:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_place_foe(l_40, (int)l_40->args[2].record);
                    quest_op_done(a1, l_40);
                }
                break;
            case 24:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    logbook_remove_entry((int)(unsigned char)(signed char)a1->id, l_40->args[1].value);
                    quest_op_done(a1, l_40);
                }
                break;
            case 23:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    logbook_add_entry((int)(unsigned char)(signed char)a1->id, l_40->args[1].value, l_40->args[2].value);
                    quest_op_done(a1, l_40);
                }
                break;
            case 25:
                if (quest_arg_state(l_40, 0) != 0) {
                    if (qaction_op25_countdown(a1, l_40) != 0) quest_op_done(a1, l_40);
                }
                break;
            case 26:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    quest_give_item_to_player(l_40->args[1].object);
                    quest_op_done(a1, l_40);
                }
                break;
            case 34:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op34_pick_one_state(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 35:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op35_cycle_state(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 19:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op19_reveal_location(a1, l_40, 1);
                    quest_op_done(a1, l_40);
                }
                break;
            case 20:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op19_reveal_location(a1, l_40, 0);
                    quest_op_done(a1, l_40);
                }
                break;
            case 0:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_place_item(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 29:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op29_prompt(a1, l_40);
                }
                break;
            case 16:
                l_24 = ((unsigned)game_minutes) / 1440;
                if (l_24 >= l_40->args[1].value && l_24 <= l_40->args[2].value) {
                    quest_set_state(a1, l_40, 1);
                } else {
                    quest_set_state(a1, l_40, 0);
                }
                break;
            case 17:
                if (quest_arg_state(l_40, 0) != 0) {
                    qaction_op17_grant_building_access(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 30:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_place_npc(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 31:
                l_24 = ((unsigned)game_minutes) % 1440;
                if (l_40->args[1].value < l_40->args[2].value) {
                    if (l_24 >= l_40->args[1].value && l_24 <= l_40->args[2].value) {
                        quest_set_state(a1, l_40, 1);
                    } else {
                        quest_set_state(a1, l_40, 0);
                    }
                } else if (l_24 >= l_40->args[1].value && l_24 <= l_40->args[2].value) {
                    quest_set_state(a1, l_40, 0);
                } else {
                    quest_set_state(a1, l_40, 1);
                }
                break;
            case 33:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_place_foe(l_40, (int)l_40->args[2].record);
                    quest_op_done(a1, l_40);
                }
                break;
            case 36:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    quest_op_done(a1, l_40);
                    if (l_40->args[1].object != 0) func_0003077F(l_40->args[1].object, 1);
                    ((struct qbn_item *)l_40->args[1].record)->object = 0;
                }
                break;
            case 37:
                if (quest_arg_state(l_40, 0) != 0) {
                    qaction_op37_repute_exceeds(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 38:
                break;
            case 39:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_give_item_to_foe(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 42:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (rand_range(1, 100) < l_40->args[1].value) {
                        if (l_40->args[3].value == 32768) {
                            region_flag_set((int)(unsigned char)current_region, l_40->args[2].value);
                        } else if (l_40->args[2].value == l_40->args[3].value) {
                            region_flag_clear((int)(unsigned char)current_region, l_40->args[2].value);
                        } else {
                            region_flag_clear((int)(unsigned char)current_region, l_40->args[2].value);
                            region_flag_set((int)(unsigned char)current_region, l_40->args[3].value);
                        }
                        quest_op_done(a1, l_40);
                    }
                }
                break;
            case 43:
                if (quest_arg_state(l_40, 0) != 0 && qcond_op43_pc_at_place(a1, l_40) != 0) {
                    quest_op_done(a1, l_40);
                }
                break;
            case 44:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    func_0003077F(l_40->args[1].object, 1);
                    ((struct qbn_person *)l_40->args[1].record)->object = 0;
                    quest_op_done(a1, l_40);
                }
                break;
            case 45:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    func_0004C874(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 46:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op46_hide_npc(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 47:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    func_0004C8CF(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 48:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op48_restore_npc(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 49:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    spfx_cure_disease((int)player_entity, (int)player_character);
                    quest_op_done(a1, l_40);
                }
                break;
            case 50:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    mc_set_location(358, (int)D_001707F0);
                    mc_sprintf((int)text_buffer, (int)D_00170801, l_40->args[1].value);
                    while (mouse_buttons != 0) xn_mouse_poll_clamped();
                    l_18 = disk_resolve_path((int)text_buffer);
                    xn_vid_play(l_18, 0, 0, 1);
                    mc_memset(655360, 0, 64000, (int)D_001707F0, 362, 4);
                    palette_restore();
                    sky_loaded_frame = 10000;
                    night_sky_loaded = 0;
                    quest_op_done(a1, l_40);
                }
                break;
            case 51:
                if (quest_arg_state(l_40, 0) != 0) quest_op_done(a1, l_40);
                break;
            case 53:
                l_3C = 0;
                for (l_24 = 1; l_24 < 5; l_24++) {
                    if (l_40->args[l_24].value == (-2)) continue;
                    if (quest_arg_state(l_40, (int)(short)*(short *)&l_24) != 0) {
                        l_3C = 1;
                        break;
                    }
                }
                quest_set_state(a1, l_40, (int)(short)*(short *)&l_3C);
                break;
            case 52:
                l_24 = 1;
                l_3C = l_24;
                for (; l_24 < 5; l_24++) {
                    if (l_40->args[l_24].value == (-2)) continue;
                    if (quest_arg_state(l_40, (int)(short)*(short *)&l_24) == 0) {
                        l_3C = 0;
                        break;
                    }
                }
                if (l_3C != 0) quest_set_state(a1, l_40, (int)(short)*(short *)&l_3C);
                break;
            case 54:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    func_0003077F(l_40->args[1].object, 0);
                    ((struct qbn_item *)l_40->args[1].record)->object = 0;
                    quest_op_done(a1, l_40);
                }
                break;
            case 55:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    l_20 = l_40->args[1].object;
                    if (l_20 != 0) {
                        quest_face_add(l_20, (int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned short)(l_20->flags & 4), l_20->id);
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 56:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    l_20 = l_40->args[1].object;
                    if (l_20 != 0) quest_face_remove(l_20->id);
                    quest_op_done(a1, l_40);
                }
                break;
            case 57:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0 && qcond_op57_item_used(a1, l_40) != 0) {
                    quest_op_done(a1, l_40);
                }
                break;
            case 58:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    disease_cure_vampirism();
                    quest_op_done(a1, l_40);
                }
                break;
            case 59:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    disease_cure_lycanthropy();
                    quest_op_done(a1, l_40);
                }
                break;
            case 60:
                if (quest_arg_state(l_40, 0) != 0) {
                    sound_play(l_40->args[1].value, (int)player_object, 110);
                    quest_op_done(a1, l_40);
                }
                break;
            case 61:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    l_20 = l_40->args[1].object;
                    if (l_20 != 0 && l_20->type == 65) {
                        faction_change_reputation(faction_find(l_20->faction_id), l_40->args[2].value);
                    } else if (l_20 != 0 && l_20->data.building.faction_id != 0) {
                        faction_change_reputation(faction_find((int)(short)l_20->data.building.faction_id), l_40->args[2].value);
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 62:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (l_40->args[1].value == 32768) {
                        region_precipitation_override[((int)(unsigned char)current_region) * 80] = (signed char)l_40->args[2].value;
                    } else {
                        region_precipitation_override[l_40->args[1].value * 80] = (signed char)l_40->args[2].value;
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 63:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    l_20 = l_40->args[1].object;
                    if (l_20 != 0) {
                        quest_face_add(l_20, (int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], 0, l_20->image2);
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 64:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    l_20 = l_40->args[1].object;
                    if (l_20 != 0) quest_face_remove(l_20->id);
                    quest_op_done(a1, l_40);
                }
                break;
            case 65:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    *(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80)) += (short)l_40->args[1].value;
                    if (((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80))) > 100) {
                        *(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80)) = 100;
                    }
                    if (((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80))) < (-100)) {
                        *(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80)) = 65436;
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 66:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    a1->text_file = (short)l_40->args[1].value;
                    quest_op_done(a1, l_40);
                }
                break;
            case 67:
                if (((int)(short)(l_40->flags & 1)) == 0) {
                    l_2C = l_40->args[0].value + ((int)quest_global_states);
                    l_28 = l_40->args[1].value + ((int)quest_global_states);
                    l_24 = 0;
                    while (l_24 < 8) {
                        if (*(signed char *)((char *)(l_2C + l_24)) != 0) {
                            l_38 = l_24 * 19;
                            break;
                        }
                        l_24++;
                    }
                    if (l_24 == 8) {
                        a1->text_file = (short)l_40->args[3].value;
                        break;
                    }
                    l_24 = 0;
                    while (l_24 < 10) {
                        if (*(signed char *)((char *)(l_28 + l_24)) != 0) {
                            if (l_24 < 9) {
                                l_38 = l_24 * 2;
                            } else if (l_24 == 9) {
                                l_38 += 19;
                            }
                            break;
                        }
                        l_24++;
                    }
                    if (l_24 == 10) {
                        a1->text_file = (short)l_40->args[3].value;
                        break;
                    }
                    if (l_24 != 9 && faction_find(D_0017A120[l_24])->reputation >= l_40->args[2].value) {
                        l_38++;
                    }
                    a1->text_file += (short)l_40->args[3].value;
                }
                break;
            case 68:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    func_00030F63(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 69:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op69_cast_spell_on_foe(a1, l_40);
                    quest_op_done(a1, l_40);
                }
                break;
            case 70:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (qcond_op70_player_has_items(a1, l_40) != 0) quest_op_done(a1, l_40);
                }
                break;
            case 72:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    disease_infect((int)player_entity, 0, l_40->args[1].value, 1);
                    quest_op_done(a1, l_40);
                }
                break;
            case 74:
                if (((int)(short)(l_40->flags & 1)) == 0) {
                    if (location_contains(player_object->x, player_object->z) != 0) {
                        if (current_location->kind >= l_40->args[1].value && current_location->kind <= l_40->args[2].value) {
                            quest_set_state(a1, l_40, 1);
                            quest_op_done(a1, l_40);
                        }
                    }
                }
                break;
            case 27:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    location_reveal(l_40->args[2].value, l_40->args[3].value);
                    quest_op_done(a1, l_40);
                }
                break;
            case 75:
                if (quest_arg_state(l_40, 0) != 0) {
                    *(int *)D_00195984 = (int)l_40->args[1].record;
                    D_00195988 = (int)l_40->args[2].record;
                    D_0019598C = (int)l_40->args[3].record;
                    D_00195990 = (int)l_40->args[4].record;
                }
                break;
            case 76:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (((int)(unsigned char)game_mode) == 19) {
                        travel_button_exit(100);
                        func_00031658(a1, l_40, 1);
                    } else {
                        func_00031658(a1, l_40, 0);
                    }
                }
                break;
            case 77:
                quest_set_arg_state(a1, l_40, 0, ((player_character->level >= l_40->args[1].value) ? 1 : 0));
                break;
            case 79:
                if (quest_arg_state(l_40, 0) == 0) {
                    if (D_00196298 != 0) {
                        D_00196298 = 0;
                        l_1C = faction_find((short)l_40->args[1].value);
                        if (l_1C->reputation >= l_40->args[2].value) {
                            quest_set_arg_state(a1, l_40, 0, 1);
                        }
                    }
                }
                break;
            case 80:
                if (quest_arg_state(l_40, 0) != 0) quest_show_message(a1, l_40->message);
                break;
            case 81:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (l_40->args[1].record != 0) {
                        ((struct qbn_item *)l_40->args[1].record)->flags |= 64;
                    }
                    if (l_40->args[2].record != 0) {
                        ((struct qbn_person *)l_40->args[2].record)->flags |= 0x4000;
                    }
                }
                break;
            case 82:
                if (quest_arg_state(l_40, 0) != 0) {
                    ((struct qbn_person *)l_40->args[1].record)->flags |= 0x2000;
                } else {
                    ((struct qbn_person *)l_40->args[1].record)->flags &= ~0x2000;
                }
                break;
            case 83:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    qaction_op83_teleport_pc(l_40);
                }
                break;
            case 84:
                if (quest_arg_state(l_40, 0) != 0) {
                    if (game_minutes != l_40->last_minutes && (((unsigned)game_minutes) % l_40->args[2].value) == 0 && rand_range(1, 100) <= l_40->args[3].value) {
                        l_40->last_minutes = game_minutes;
                        sound_play(l_40->args[1].value, (int)player_object, 110);
                        quest_op_done(a1, l_40);
                    }
                }
                break;
            case 85:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (l_40->args[1].object != 0) {
                        faction_find(l_40->args[1].object->faction_id)->flags |= 0x200;
                    }
                    quest_op_done(a1, l_40);
                }
                break;
            case 86:
                if (((int)(short)(l_40->flags & 1)) == 0 && quest_arg_state(l_40, 0) != 0) {
                    if (l_40->args[1].object != 0) {
                        faction_find(l_40->args[1].object->faction_id)->flags &= ~0x200;
                    }
                    quest_op_done(a1, l_40);
                }
            }
        }
        *(int *)D_00195984 = (D_00195988 = (D_0019598C = (D_00195990 = 0)));
        if (l_34 == 0) quest_timers_update(a1);
        if (quest_reward_container == 0 || quest_reward_container->children == 0 || ((int)(unsigned char)game_mode) == 4) {
            return;
        }
        inventory_open_container((int)quest_reward_container, 0, 6);
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
    for (l_2C = 0; a1->section_counts[8] > l_2C; l_2C++, l_30++) {
        if (l_30->opcode == quest_event_code) {
            switch (l_30->opcode) {
            case 71:
                if (((int)(short)(l_30->flags & 1)) == 0) {
                    if (quest_event_object->twin != l_30->args[3].object) break;
                    if (((unsigned)player_character->gold) >= l_30->args[2].value) {
                        player_character->gold -= l_30->args[2].value;
                        quest_set_arg_state(a1, l_30, 0, 1);
                        quest_set_arg_state(a1, l_30, 1, 0);
                        l_30->flags |= 1;
                        quest_op_done(a1, l_30);
                        l_28 = 1;
                        l_24 = 1;
                        l_1C = (int)l_30->args[3].record;
                    } else {
                        quest_set_arg_state(a1, l_30, 0, 0);
                        quest_set_arg_state(a1, l_30, 1, 1);
                    }
                }
                break;
            case 28:
                if (qcond_op28_npc_clicked(a1, l_30) != 0) {
                    quest_op_done(a1, l_30);
                    l_28 = 1;
                    l_24 = 1;
                    l_1C = (int)l_30->args[1].record;
                }
                break;
            case 3:
                if (qcond_op03_item_found(a1, l_30) != 0) quest_op_done(a1, l_30);
                break;
            case 5:
                if (qcond_op05_item_dropped_at_place(a1, l_30) != 0) quest_op_done(a1, l_30);
                break;
            case 1:
                if (qcond_op01_item_given_to_npc(a1, l_30) != 0) {
                    quest_op_done(a1, l_30);
                    l_28 = 1;
                    l_24 = 1;
                    l_1C = (int)l_30->args[2].record;
                }
                break;
            case 2:
                if (qcond_op02_foe_killed(a1, l_30) != 0) quest_op_done(a1, l_30);
                break;
            case 21:
                if (qcond_op21_foe_hurt(a1, l_30) != 0) quest_op_done(a1, l_30);
                break;
            case 73:
                if (((int)(short)(l_30->flags & 1)) == 0 && quest_arg_state(l_30, 0) != 0) {
                    if (quest_event_object->data.spell.id == l_30->args[2].value) {
                        quest_set_arg_state(a1, l_30, 1, 1);
                        l_30->flags |= 1;
                    }
                }
                break;
            case 78:
                if (quest_event_object->data.person.faction_id == l_30->args[1].value) {
                    quest_set_arg_state(a1, l_30, 1, 1);
                    quest_op_done(a1, l_30);
                    l_28 = 1;
                }
            }
        }
    }
    if (l_24 != 0 && ((int)(short)(*(short *)((char *)l_1C + 2) & 8192)) != 0) return 0;
    return l_28;
}

void quest_debug_next(void)
{
    struct record *l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
    l_20 = quest_root->children;
    if (l_20 != 0 && quest_debug_object == 0) {
        quest_debug_object = (int)l_20;
        quest_debug_data = (int)&l_20->data.quest;
        return;
    }
    while (l_20 != 0) {
        if (l_20->type == 14) {
            if (l_18 != 0) {
                quest_debug_object = (int)l_20;
                quest_debug_data = (int)&l_20->data.quest;
                return;
            }
            if ((int)l_20 == quest_debug_object) l_18 = 1;
        }
        l_20 = l_20->next;
    }
    if (l_18 == 0) return;
    l_20 = quest_root->children;
    quest_debug_object = (int)l_20;
    quest_debug_data = (int)&l_20->data.quest;
}

void quests_unlink_all(struct record *a1)
{
    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 14) quest_unlink_for_save(&a1->data.quest);
        a1 = a1->next;
    }
}

void quests_relink_all(struct record *a1)
{
    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 14) quest_relink_after_load(&a1->data.quest);
        a1 = a1->next;
    }
}

void quest_relink_after_load(struct quest *a1)
{
    struct qbn_op *l_48;
    struct qbn_arg *l_44;
    int l_40;
    struct qbn_text_var *l_3C;
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
    for (l_1C = 0; a1->section_counts[8] > l_1C; l_1C++, l_48++) {
        l_44 = l_48->args;
        l_48->arg_count = ((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)&qbn_opcode_arg_counts + l_48->opcode))) - 48;
        for (l_20 = 0; l_48->arg_count > l_20; l_20++, l_44++) {
            if (l_44->record != 0) l_44->record += (int)a1;
            if (l_44->object != 0) {
                l_44->object = (struct record *)object_find_by_id(nonworld_root, (int)l_44->object);
            }
        }
    }
    l_34 = (struct qbn_person *)((int)a1 + a1->section_offsets[3]);
    for (l_1C = 0; a1->section_counts[3] > l_1C; l_1C++, l_34++) {
        if (l_34->object != 0) {
            l_34->object = (struct record *)object_find_by_id(nonworld_root, (int)l_34->object);
            if (l_34->object != 0 && l_34->object->type == 65 && l_34->object->faction_id == 0) {
                l_34->object->faction_id = l_34->faction_id;
            }
        }
    }
    l_38 = (struct qbn_place *)((int)a1 + a1->section_offsets[4]);
    for (l_1C = 0; a1->section_counts[4] > l_1C; l_1C++, l_38++) {
        if (l_38->object != 0) {
            l_38->object = (struct record *)object_find_by_id(nonworld_root, (int)l_38->object);
            if (l_38->object == 0) fatal_error((int)D_001708ED);
        }
    }
    l_30 = (struct qbn_item *)((int)a1 + a1->section_offsets[0]);
    for (l_1C = 0; a1->section_counts[0] > l_1C; l_1C++, l_30++) {
        if (l_30->object != 0) {
            l_30->object = (struct record *)object_find_by_id(nonworld_root, (int)l_30->object);
        }
        if (l_30->object == 0) {
            l_30->object = (struct record *)object_find_by_id(location_object, (int)l_30->object);
        }
    }
    l_2C = (struct qbn_foe *)((int)a1 + a1->section_offsets[7]);
    for (l_1C = 0; a1->section_counts[7] > l_1C; l_1C++, l_2C++) {
        if (l_2C->object != 0) {
            l_2C->object = (struct record *)object_find_by_id(nonworld_root, (int)l_2C->object);
        }
    }
    l_18 = (struct qbn_timer *)((int)a1 + a1->section_offsets[6]);
    for (l_1C = 0; a1->section_counts[6] > l_1C; l_1C++, l_18++) {
        if (l_18->link1 != 0) l_18->link1 = object_find_by_id(nonworld_root, (int)l_18->link1);
        if (l_18->link2 != 0) l_18->link2 = object_find_by_id(nonworld_root, (int)l_18->link2);
    }
    if (a1->text_offset == 0) return;
    l_3C = (struct qbn_text_var *)((int)a1 + a1->text_offset);
    while (l_3C->name[0] != 0) {
        l_3C->record = quest_record(a1, (int)(short)((unsigned short)l_3C->section), l_3C->index);
        l_3C++;
    }
}

int quest_event_clicked_faction(unsigned short a1)
{
    struct record *l_2C;
    struct record *l_28;
    struct qbn_op *l_30;
    short l_20;
    short l_1C;

    *(int *)&l_1C = 0;
    l_2C = quest_root->children;
    while (l_2C != 0) {
        l_28 = l_2C->next;
        if (l_2C->type == 14) {
            quest_tick_object = l_2C;
            current_quest = (struct quest *)((*(int *)&quest_tick_data = (int)&l_2C->data.quest));
            l_30 = quest_section(current_quest, 8);
            *(int *)&l_20 = 0;
            for (; current_quest->section_counts[8] > *(int *)&l_20; (*(int *)&l_20)++, l_30++) {
                if (l_30->opcode == 78 && ((int)(unsigned short)a1) == l_30->args[1].value) {
                    quest_set_arg_state(current_quest, l_30, 0, 1);
                    *(int *)&l_1C = 1;
                }
            }
        }
        l_2C = l_28;
    }
    return *(int *)&l_1C;
}

int quest_place_or_person_object(struct quest *a1, int a2, short a3)
{
    if (a3 == 0) return *(int *)((char *)quest_record(a1, 4, (int)(short)*(short *)&a2) + 16);
    return *(int *)((char *)quest_record(a1, 3, (int)(short)*(short *)&a2) + 12);
}

void quest_timer_expire(struct quest *a1, struct qbn_timer *a2)
{
    struct qbn_state *l_1C;
    int l_18;
    int l_14;

    if (((int)(short)(a2->flags & 3)) != 0) {
        a2->flags |= 128;
    } else if (((int)(short)(a2->flags & 4)) != 0) {
        a2->flags ^= 128;
    }
    if (((int)(short)(a2->flags & 8)) != 0) {
        quest_timer_update(a1, a2, 1);
    } else {
        a2->flags &= ~0x40;
    }
    l_18 = a1->section_counts[9];
    l_1C = quest_record(a1, 9, 0);
    while (l_18 != 0) {
        if (l_1C->name_hash == a2->state_hash) {
            l_14 = (int)(short)(a2->flags & 128);
            if (l_1C->is_global != 0) {
                quest_global_states[l_1C->value] = *(signed char *)&l_14;
            } else {
                l_1C->value = *(signed char *)&l_14;
            }
            return;
        }
        l_1C++;
        l_18--;
    }
}

void quest_timer_clear_state(struct qbn_timer *a1)
{
    int l_1C;
    struct qbn_state *l_18;

    l_1C = current_quest->section_counts[9];
    l_18 = quest_record(current_quest, 9, 0);
    while (l_1C != 0) {
        if (l_18->name_hash == a1->state_hash) {
            if (l_18->is_global != 0) {
                quest_global_states[l_18->value] = 0;
            } else {
                l_18->value = 0;
            }
            return;
        }
        l_18++;
        l_1C--;
    }
}

int quest_state_from_timer(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_timer *l_20;
    struct qbn_state *l_1C;
    short l_14;

    if (a2->args[0].value == (-1)) return 0;
    l_20 = quest_record(a1, 6, (short)a2->args[1].value);
    l_1C = quest_record(a1, 9, (short)a2->args[0].value);
    l_14 = l_20->flags & 128;
    if (l_1C->is_global != 0) {
        quest_global_states[l_1C->value] = *(signed char *)&l_14;
    } else {
        l_1C->value = *(signed char *)&l_14;
    }
    return (int)(short)l_14;
}

int qaction_op25_countdown(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_arg *l_1C;
    struct qbn_state *l_18;

    l_1C = &a2->args[2];
    l_18 = (struct qbn_state *)a2->args[1].record;
    if (l_18->is_global != 0) {
        if (quest_global_states[l_18->value] != 0) return 0;
    } else if (l_18->value != 0) {
        return 0;
    }
    if (l_1C->value != 0) l_1C->value--;
    if (l_1C->value == 0) {
        if (a2->args[1].value == (-1)) return 1;
        if (l_18->is_global != 0) {
            quest_global_states[l_18->value] = 1;
        } else {
            l_18->value = 1;
        }
        return 1;
    }
    return 0;
}

void qaction_op11_remove_topics(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct qbn_place *l_1C;
    struct qbn_person *l_18;
    struct qbn_item *l_14;

    for (l_20 = 1; l_20 < 4; l_20++) {
        if (a2->args[l_20].value == (-1)) continue;
        switch ((unsigned)l_20) {
        case 1:
            l_1C = quest_record(a1, 4, (short)a2->args[l_20].value);
            l_1C->flags |= 128;
            break;
        case 2:
            l_18 = quest_record(a1, 3, (short)a2->args[l_20].value);
            l_18->flags |= 0x8000;
            break;
        case 3:
            l_14 = quest_record(a1, 0, (short)a2->args[l_20].value);
            l_14->flags |= 128;
        }
    }
}

void qaction_op10_add_topics(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct qbn_place *l_1C;
    struct qbn_person *l_18;
    struct qbn_item *l_14;

    for (l_20 = 1; l_20 < 4; l_20++) {
        if (a2->args[l_20].value == (-1)) continue;
        switch ((unsigned)l_20) {
        case 1:
            l_1C = quest_record(a1, 4, (short)a2->args[l_20].value);
            l_1C->flags &= 127;
            break;
        case 2:
            l_18 = quest_record(a1, 3, (short)a2->args[l_20].value);
            l_18->flags &= ~0x8000;
            break;
        case 3:
            l_14 = quest_record(a1, 0, (short)a2->args[l_20].value);
            l_14->flags &= 127;
        }
    }
}
