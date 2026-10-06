/* qcom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern struct region regions[];
extern signed char mouse_buttons;
extern char D_001707F0[];
extern char D_001707F7[];
extern char D_00170801[];
extern char D_001708ED[];
extern short D_0017A120[];
extern signed char D_001841E3[];
extern signed char text_buffer[];
extern char D_001911E4[];
extern signed char quest_global_states[];
extern iptr D_00195984[];
extern iptr D_00195988;
extern iptr D_0019598C;
extern iptr D_00195990;
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
extern iptr qbn_opcode_arg_counts;
extern signed char current_region;
extern unsigned char D_0019626F;
extern signed char game_mode;
extern signed char D_00196298;
extern signed char night_sky_loaded;
extern iptr quest_debug_object;
extern struct quest *current_quest;
extern struct record *quest_reward_container;
extern iptr quest_debug_data;
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
extern int quest_arg_state(struct qbn_op *, short);
extern void *quest_section(struct quest *, int);
extern void *quest_record(struct quest *, int, int);
extern int qcond_op57_item_used(struct quest *, struct qbn_op *);
extern int quest_deliveries_done(struct quest *);
extern iptr quest_start(char *);
extern int sound_play(int, struct record *, int);
extern iptr disk_resolve_path(iptr);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern struct record *object_find_by_id(struct record *, iptr);
extern int xn_vid_play(char *, int, int, int);
extern int xn_mouse_poll_clamped(void);
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
extern void quest_timer_update(struct quest *, struct qbn_timer *, short);
extern void qaction_op12_start_stop_timer(struct quest *, struct qbn_op *, short);
extern void qaction_op35_cycle_state(struct quest *, struct qbn_op *);
extern void qaction_op34_pick_one_state(struct quest *, struct qbn_op *);
extern void qaction_op29_prompt(struct quest *, struct qbn_op *);
extern void quest_set_state(struct quest *, struct qbn_op *, short);
extern void quest_set_arg_state(struct quest *, struct qbn_op *, int, int);
extern void qaction_op04_give_reward(struct quest *, struct qbn_op *);
extern void qaction_op19_reveal_location(struct quest *, struct qbn_op *, int);
extern void func_0003077F(struct record *, int);
extern void quest_give_item_to_player(struct record *);
extern void qaction_op37_repute_exceeds(struct quest *, struct qbn_op *);
extern void quest_face_add(struct record *, int, int, int);
extern void quest_face_remove(int);
extern void func_00030F63(struct quest *, struct qbn_op *);
extern void qaction_op69_cast_spell_on_foe(struct quest *, struct qbn_op *);
extern void func_00031658(struct quest *, struct qbn_op *, int);
extern void qaction_op83_teleport_pc(struct qbn_op *);
extern void quest_show_message(struct quest *, int);
extern void quest_op_done(struct quest *, struct qbn_op *);
extern void qaction_place_foe(struct qbn_op *, iptr);
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
extern void fatal_error(char *);
extern void disease_infect(struct record *, unsigned char *, int, int);
extern void disease_cure_vampirism(void);
extern void disease_cure_lycanthropy(void);
extern void logbook_add_entry(unsigned char, int, int);
extern void logbook_remove_entry(unsigned char, int);
extern void rest_close(void);
extern void location_reveal(int, int);
extern void spfx_cure_disease(struct record *, struct character *);
extern void inventory_open_container(struct record *, int, int);
extern void travel_button_exit(int);
int qaction_op25_countdown(struct quest *, struct qbn_op *);
void quest_relink_after_load(struct quest *);
void qaction_op11_remove_topics(struct quest *, struct qbn_op *);
void qaction_op10_add_topics(struct quest *, struct qbn_op *);
#pragma aux mc_set_location parm routine [];

void quest_run_opcodes(struct quest *quest)
{
    int op_index;
    struct qbn_op *op;
    int combined;
    int text_delta;
    int ended;
    struct qbn_state *state;
    signed char *group_states;
    signed char *faction_states;
    int i;
    struct record *object;
    struct faction *faction;
    iptr path;
    {
        int unused1;
        int unused2;
        int unused3;
        int unused4;
        int unused5;
        int unused6;
        int unused7;
        int unused8;
        int unused9;
        int unused10;
        int unused11;

        ended = 0;
        current_quest = quest;
        op = quest_section(quest, 8);
        for (op_index = 0; quest->section_counts[8] > op_index; op_index++, op++) {
            switch (op->opcode) {
            case 7:
                if (quest_arg_state(op, 0) != 0) {
                    for (i = 1; i < 5; i++) {
                        state = (struct qbn_state *)op->args[i].record;
                        if (state == 0) continue;
                        if (state->is_global != 0) {
                            quest_global_states[state->value] = 0;
                        } else {
                            state->value = 0;
                        }
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 8:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0 && ((int)(unsigned char)current_region) != 31) {
                    mc_set_location(64, D_001707F0);
                    mc_sprintf(D_001911E4, D_001707F7, rand_range(op->args[1].value, op->args[2].value));
                    quest_start(D_001911E4);
                    quest_op_done(quest, op);
                }
                break;
            case 10:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op10_add_topics(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 6:
                if (quest_arg_state(op, 0) != 0 && quest_deliveries_done(quest) != 0 && (((quest_reward_container == 0) || ((quest_reward_container->children == 0))) ? 1 : 0) != 0) {
                    quest_op_done(quest, op);
                    if (ended == 0) {
                        if (((int)(unsigned char)(quest->flags & 2)) == 0 && quest->faction_id != 0) {
                            faction_change_reputation(faction_find(quest->faction_id), -2);
                        }
                        rumor_add_quest(quest, ((((int)(unsigned char)(quest->flags & 2)) != 0) ? 1007 : 1006), 0, 8);
                        quest_add_questor_rumor(quest, (int)(unsigned char)(quest->flags & 2));
                        rumor_file_purge();
                    }
                    quest_end(quest);
                    ended = 1;
                    op_index = quest->section_counts[8];
                }
                break;
            case 11:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op11_remove_topics(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 12:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op12_start_stop_timer(quest, op, 64);
                    quest_op_done(quest, op);
                }
                break;
            case 13:
                if (quest_arg_state(op, 0) != 0) {
                    qaction_op12_start_stop_timer(quest, op, 0);
                    quest_op_done(quest, op);
                }
                break;
            case 9:
                if (quest_arg_state(op, 0) != 0) {
                    qaction_op09_spawn_repeat(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 87:
                if (quest_arg_state(op, 0) != 0) {
                    qaction_op87_respawn(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 4:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (((int)(unsigned char)game_mode) == 16) {
                        rest_close();
                        D_0019626F = 0;
                    }
                    quest->flags |= 2;
                    quest_op_done(quest, op);
                    msgbox_show_quest_text(current_quest, 1004, 1);
                    qaction_op04_give_reward(quest, op);
                }
                break;
            case 22:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_place_foe(op, (iptr)op->args[2].record);
                    quest_op_done(quest, op);
                }
                break;
            case 24:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    logbook_remove_entry((int)(unsigned char)(signed char)quest->id, op->args[1].value);
                    quest_op_done(quest, op);
                }
                break;
            case 23:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    logbook_add_entry((int)(unsigned char)(signed char)quest->id, op->args[1].value, op->args[2].value);
                    quest_op_done(quest, op);
                }
                break;
            case 25:
                if (quest_arg_state(op, 0) != 0) {
                    if (qaction_op25_countdown(quest, op) != 0) quest_op_done(quest, op);
                }
                break;
            case 26:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    quest_give_item_to_player(op->args[1].object);
                    quest_op_done(quest, op);
                }
                break;
            case 34:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op34_pick_one_state(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 35:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op35_cycle_state(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 19:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op19_reveal_location(quest, op, 1);
                    quest_op_done(quest, op);
                }
                break;
            case 20:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op19_reveal_location(quest, op, 0);
                    quest_op_done(quest, op);
                }
                break;
            case 0:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_place_item(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 29:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op29_prompt(quest, op);
                }
                break;
            case 16:
                i = ((unsigned)game_minutes) / 1440;
                if (i >= op->args[1].value && i <= op->args[2].value) {
                    quest_set_state(quest, op, 1);
                } else {
                    quest_set_state(quest, op, 0);
                }
                break;
            case 17:
                if (quest_arg_state(op, 0) != 0) {
                    qaction_op17_grant_building_access(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 30:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_place_npc(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 31:
                i = ((unsigned)game_minutes) % 1440;
                if (op->args[1].value < op->args[2].value) {
                    if (i >= op->args[1].value && i <= op->args[2].value) {
                        quest_set_state(quest, op, 1);
                    } else {
                        quest_set_state(quest, op, 0);
                    }
                } else if (i >= op->args[1].value && i <= op->args[2].value) {
                    quest_set_state(quest, op, 0);
                } else {
                    quest_set_state(quest, op, 1);
                }
                break;
            case 33:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_place_foe(op, (iptr)op->args[2].record);
                    quest_op_done(quest, op);
                }
                break;
            case 36:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    quest_op_done(quest, op);
                    if (op->args[1].object != 0) func_0003077F(op->args[1].object, 1);
                    ((struct qbn_item *)op->args[1].record)->object = 0;
                }
                break;
            case 37:
                if (quest_arg_state(op, 0) != 0) {
                    qaction_op37_repute_exceeds(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 38:
                break;
            case 39:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_give_item_to_foe(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 42:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (rand_range(1, 100) < op->args[1].value) {
                        if (op->args[3].value == 32768) {
                            region_flag_set((int)(unsigned char)current_region, op->args[2].value);
                        } else if (op->args[2].value == op->args[3].value) {
                            region_flag_clear((int)(unsigned char)current_region, op->args[2].value);
                        } else {
                            region_flag_clear((int)(unsigned char)current_region, op->args[2].value);
                            region_flag_set((int)(unsigned char)current_region, op->args[3].value);
                        }
                        quest_op_done(quest, op);
                    }
                }
                break;
            case 43:
                if (quest_arg_state(op, 0) != 0 && qcond_op43_pc_at_place(quest, op) != 0) {
                    quest_op_done(quest, op);
                }
                break;
            case 44:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    func_0003077F(op->args[1].object, 1);
                    ((struct qbn_person *)op->args[1].record)->object = 0;
                    quest_op_done(quest, op);
                }
                break;
            case 45:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    func_0004C874(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 46:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op46_hide_npc(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 47:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    func_0004C8CF(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 48:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op48_restore_npc(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 49:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    spfx_cure_disease(player_entity, player_character);
                    quest_op_done(quest, op);
                }
                break;
            case 50:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    mc_set_location(358, D_001707F0);
                    mc_sprintf((char *)text_buffer, D_00170801, op->args[1].value);
                    while (mouse_buttons != 0) xn_mouse_poll_clamped();
                    path = disk_resolve_path((iptr)text_buffer);
                    xn_vid_play((char *)path, 0, 0, 1);
                    mc_memset((void *)655360, 0, 64000, D_001707F0, 362, 4);
                    palette_restore();
                    sky_loaded_frame = 10000;
                    night_sky_loaded = 0;
                    quest_op_done(quest, op);
                }
                break;
            case 51:
                if (quest_arg_state(op, 0) != 0) quest_op_done(quest, op);
                break;
            case 53:
                combined = 0;
                for (i = 1; i < 5; i++) {
                    if (op->args[i].value == (-2)) continue;
                    if (quest_arg_state(op, (int)(short)*(short *)&i) != 0) {
                        combined = 1;
                        break;
                    }
                }
                quest_set_state(quest, op, (int)(short)*(short *)&combined);
                break;
            case 52:
                i = 1;
                combined = i;
                for (; i < 5; i++) {
                    if (op->args[i].value == (-2)) continue;
                    if (quest_arg_state(op, (int)(short)*(short *)&i) == 0) {
                        combined = 0;
                        break;
                    }
                }
                if (combined != 0) quest_set_state(quest, op, (int)(short)*(short *)&combined);
                break;
            case 54:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    func_0003077F(op->args[1].object, 0);
                    ((struct qbn_item *)op->args[1].record)->object = 0;
                    quest_op_done(quest, op);
                }
                break;
            case 55:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    object = op->args[1].object;
                    if (object != 0) {
                        quest_face_add(object, (int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned short)(object->flags & 4), object->id);
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 56:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    object = op->args[1].object;
                    if (object != 0) quest_face_remove(object->id);
                    quest_op_done(quest, op);
                }
                break;
            case 57:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0 && qcond_op57_item_used(quest, op) != 0) {
                    quest_op_done(quest, op);
                }
                break;
            case 58:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    disease_cure_vampirism();
                    quest_op_done(quest, op);
                }
                break;
            case 59:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    disease_cure_lycanthropy();
                    quest_op_done(quest, op);
                }
                break;
            case 60:
                if (quest_arg_state(op, 0) != 0) {
                    sound_play(op->args[1].value, player_object, 110);
                    quest_op_done(quest, op);
                }
                break;
            case 61:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    object = op->args[1].object;
                    if (object != 0 && object->type == 65) {
                        faction_change_reputation(faction_find(object->faction_id), op->args[2].value);
                    } else if (object != 0 && object->data.building.faction_id != 0) {
                        faction_change_reputation(faction_find((int)(short)object->data.building.faction_id), op->args[2].value);
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 62:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (op->args[1].value == 32768) {
                        regions[(unsigned char)current_region].precipitation_override = (signed char)op->args[2].value;
                    } else {
                        regions[op->args[1].value].precipitation_override = (signed char)op->args[2].value;
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 63:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    object = op->args[1].object;
                    if (object != 0) {
                        quest_face_add(object, (int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], 0, object->image2);
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 64:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    object = op->args[1].object;
                    if (object != 0) quest_face_remove(object->id);
                    quest_op_done(quest, op);
                }
                break;
            case 65:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    regions[(unsigned char)current_region].legal_reputation += (short)op->args[1].value;
                    if (regions[(unsigned char)current_region].legal_reputation > 100) {
                        regions[(unsigned char)current_region].legal_reputation = 100;
                    }
                    if (regions[(unsigned char)current_region].legal_reputation < (-100)) {
                        regions[(unsigned char)current_region].legal_reputation = -100;
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 66:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    quest->text_file = (short)op->args[1].value;
                    quest_op_done(quest, op);
                }
                break;
            case 67:
                if (((int)(short)(op->flags & 1)) == 0) {
                    group_states = op->args[0].value + quest_global_states;
                    faction_states = op->args[1].value + quest_global_states;
                    i = 0;
                    while (i < 8) {
                        if (group_states[i] != 0) {
                            text_delta = i * 19;
                            break;
                        }
                        i++;
                    }
                    if (i == 8) {
                        quest->text_file = (short)op->args[3].value;
                        break;
                    }
                    i = 0;
                    while (i < 10) {
                        if (faction_states[i] != 0) {
                            if (i < 9) {
                                text_delta = i * 2;
                            } else if (i == 9) {
                                text_delta += 19;
                            }
                            break;
                        }
                        i++;
                    }
                    if (i == 10) {
                        quest->text_file = (short)op->args[3].value;
                        break;
                    }
                    if (i != 9 && faction_find(D_0017A120[i])->reputation >= op->args[2].value) {
                        text_delta++;
                    }
                    quest->text_file += (short)op->args[3].value;
                }
                break;
            case 68:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    func_00030F63(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 69:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op69_cast_spell_on_foe(quest, op);
                    quest_op_done(quest, op);
                }
                break;
            case 70:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (qcond_op70_player_has_items(quest, op) != 0) quest_op_done(quest, op);
                }
                break;
            case 72:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    disease_infect(player_entity, 0, op->args[1].value, 1);
                    quest_op_done(quest, op);
                }
                break;
            case 74:
                if (((int)(short)(op->flags & 1)) == 0) {
                    if (location_contains(player_object->x, player_object->z) != 0) {
                        if (current_location->kind >= op->args[1].value && current_location->kind <= op->args[2].value) {
                            quest_set_state(quest, op, 1);
                            quest_op_done(quest, op);
                        }
                    }
                }
                break;
            case 27:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    location_reveal(op->args[2].value, op->args[3].value);
                    quest_op_done(quest, op);
                }
                break;
            case 75:
                if (quest_arg_state(op, 0) != 0) {
                    D_00195984[0] = (iptr)op->args[1].record;
                    D_00195988 = (iptr)op->args[2].record;
                    D_0019598C = (iptr)op->args[3].record;
                    D_00195990 = (iptr)op->args[4].record;
                }
                break;
            case 76:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (((int)(unsigned char)game_mode) == 19) {
                        travel_button_exit(100);
                        func_00031658(quest, op, 1);
                    } else {
                        func_00031658(quest, op, 0);
                    }
                }
                break;
            case 77:
                quest_set_arg_state(quest, op, 0, ((player_character->level >= op->args[1].value) ? 1 : 0));
                break;
            case 79:
                if (quest_arg_state(op, 0) == 0) {
                    if (D_00196298 != 0) {
                        D_00196298 = 0;
                        faction = faction_find((short)op->args[1].value);
                        if (faction->reputation >= op->args[2].value) {
                            quest_set_arg_state(quest, op, 0, 1);
                        }
                    }
                }
                break;
            case 80:
                if (quest_arg_state(op, 0) != 0) quest_show_message(quest, op->message);
                break;
            case 81:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (op->args[1].record != 0) {
                        ((struct qbn_item *)op->args[1].record)->flags |= 64;
                    }
                    if (op->args[2].record != 0) {
                        ((struct qbn_person *)op->args[2].record)->flags |= 0x4000;
                    }
                }
                break;
            case 82:
                if (quest_arg_state(op, 0) != 0) {
                    ((struct qbn_person *)op->args[1].record)->flags |= 0x2000;
                } else {
                    ((struct qbn_person *)op->args[1].record)->flags &= ~0x2000;
                }
                break;
            case 83:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    qaction_op83_teleport_pc(op);
                }
                break;
            case 84:
                if (quest_arg_state(op, 0) != 0) {
                    if (game_minutes != op->last_minutes && (((unsigned)game_minutes) % op->args[2].value) == 0 && rand_range(1, 100) <= op->args[3].value) {
                        op->last_minutes = game_minutes;
                        sound_play(op->args[1].value, player_object, 110);
                        quest_op_done(quest, op);
                    }
                }
                break;
            case 85:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (op->args[1].object != 0) {
                        faction_find(op->args[1].object->faction_id)->flags |= 0x200;
                    }
                    quest_op_done(quest, op);
                }
                break;
            case 86:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (op->args[1].object != 0) {
                        faction_find(op->args[1].object->faction_id)->flags &= ~0x200;
                    }
                    quest_op_done(quest, op);
                }
            }
        }
        D_00195984[0] = (D_00195988 = (D_0019598C = (D_00195990 = 0)));
        if (ended == 0) quest_timers_update(quest);
        if (quest_reward_container == 0 || quest_reward_container->children == 0 || ((int)(unsigned char)game_mode) == 4) {
            return;
        }
        inventory_open_container(quest_reward_container, 0, 6);
    }
}

int quest_dispatch_event(struct quest *quest)
{
    struct qbn_op *op;
    int op_index;
    int fired;
    int has_person;
    int unused;
    struct qbn_person *qbn_person;

    op = quest_section(quest, 8);
    fired = 0;
    has_person = 0;
    for (op_index = 0; quest->section_counts[8] > op_index; op_index++, op++) {
        if (op->opcode == quest_event_code) {
            switch (op->opcode) {
            case 71:
                if (((int)(short)(op->flags & 1)) == 0) {
                    if (quest_event_object->twin != op->args[3].object) break;
                    if (((unsigned)player_character->gold) >= op->args[2].value) {
                        player_character->gold -= op->args[2].value;
                        quest_set_arg_state(quest, op, 0, 1);
                        quest_set_arg_state(quest, op, 1, 0);
                        op->flags |= 1;
                        quest_op_done(quest, op);
                        fired = 1;
                        has_person = 1;
                        qbn_person = (struct qbn_person *)op->args[3].record;
                    } else {
                        quest_set_arg_state(quest, op, 0, 0);
                        quest_set_arg_state(quest, op, 1, 1);
                    }
                }
                break;
            case 28:
                if (qcond_op28_npc_clicked(quest, op) != 0) {
                    quest_op_done(quest, op);
                    fired = 1;
                    has_person = 1;
                    qbn_person = (struct qbn_person *)op->args[1].record;
                }
                break;
            case 3:
                if (qcond_op03_item_found(quest, op) != 0) quest_op_done(quest, op);
                break;
            case 5:
                if (qcond_op05_item_dropped_at_place(quest, op) != 0) quest_op_done(quest, op);
                break;
            case 1:
                if (qcond_op01_item_given_to_npc(quest, op) != 0) {
                    quest_op_done(quest, op);
                    fired = 1;
                    has_person = 1;
                    qbn_person = (struct qbn_person *)op->args[2].record;
                }
                break;
            case 2:
                if (qcond_op02_foe_killed(quest, op) != 0) quest_op_done(quest, op);
                break;
            case 21:
                if (qcond_op21_foe_hurt(quest, op) != 0) quest_op_done(quest, op);
                break;
            case 73:
                if (((int)(short)(op->flags & 1)) == 0 && quest_arg_state(op, 0) != 0) {
                    if (quest_event_object->data.spell.id == op->args[2].value) {
                        quest_set_arg_state(quest, op, 1, 1);
                        op->flags |= 1;
                    }
                }
                break;
            case 78:
                if (quest_event_object->data.person.faction_id == op->args[1].value) {
                    quest_set_arg_state(quest, op, 1, 1);
                    quest_op_done(quest, op);
                    fired = 1;
                }
            }
        }
    }
    if (has_person != 0 && ((short)qbn_person->flags & 8192) != 0) return 0;
    return fired;
}

void quest_debug_next(void)
{
    struct record *object;
    int unused;
    int found_current;

    found_current = 0;
    object = quest_root->children;
    if (object != 0 && quest_debug_object == 0) {
        quest_debug_object = (iptr)object;
        quest_debug_data = (iptr)&object->data.quest;
        return;
    }
    while (object != 0) {
        if (object->type == 14) {
            if (found_current != 0) {
                quest_debug_object = (iptr)object;
                quest_debug_data = (iptr)&object->data.quest;
                return;
            }
            if ((iptr)object == quest_debug_object) found_current = 1;
        }
        object = object->next;
    }
    if (found_current == 0) return;
    object = quest_root->children;
    quest_debug_object = (iptr)object;
    quest_debug_data = (iptr)&object->data.quest;
}

void quests_unlink_all(struct record *object)
{
    object = object->children;
    while (object != 0) {
        if (object->type == 14) quest_unlink_for_save(&object->data.quest);
        object = object->next;
    }
}

void quests_relink_all(struct record *object)
{
    object = object->children;
    while (object != 0) {
        if (object->type == 14) quest_relink_after_load(&object->data.quest);
        object = object->next;
    }
}

void quest_relink_after_load(struct quest *quest)
{
    struct qbn_op *op;
    struct qbn_arg *arg;
    int unused1;
    struct qbn_text_var *text_var;
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct qbn_item *qbn_item;
    struct qbn_foe *foe;
    int unused2;
    int unused3;
    int j;
    int i;
    struct qbn_timer *timer;

    op = (struct qbn_op *)((iptr)quest + quest->section_offsets[8]);
    for (i = 0; quest->section_counts[8] > i; i++, op++) {
        arg = op->args;
        op->arg_count = ((int)(unsigned char)*(signed char *)((*(char **)&qbn_opcode_arg_counts + op->opcode))) - 48;
        for (j = 0; op->arg_count > j; j++, arg++) {
            if (arg->record != 0) arg->record += (iptr)quest;
            if (arg->object != 0) {
                arg->object = (struct record *)object_find_by_id(nonworld_root, (iptr)arg->object);
            }
        }
    }
    qbn_person = (struct qbn_person *)((iptr)quest + quest->section_offsets[3]);
    for (i = 0; quest->section_counts[3] > i; i++, qbn_person++) {
        if (qbn_person->object != 0) {
            qbn_person->object = (struct record *)object_find_by_id(nonworld_root, (iptr)qbn_person->object);
            if (qbn_person->object != 0 && qbn_person->object->type == 65 && qbn_person->object->faction_id == 0) {
                qbn_person->object->faction_id = qbn_person->faction_id;
            }
        }
    }
    qbn_place = (struct qbn_place *)((iptr)quest + quest->section_offsets[4]);
    for (i = 0; quest->section_counts[4] > i; i++, qbn_place++) {
        if (qbn_place->object != 0) {
            qbn_place->object = (struct record *)object_find_by_id(nonworld_root, (iptr)qbn_place->object);
            if (qbn_place->object == 0) fatal_error(D_001708ED);
        }
    }
    qbn_item = (struct qbn_item *)((iptr)quest + quest->section_offsets[0]);
    for (i = 0; quest->section_counts[0] > i; i++, qbn_item++) {
        if (qbn_item->object != 0) {
            qbn_item->object = (struct record *)object_find_by_id(nonworld_root, (iptr)qbn_item->object);
        }
        if (qbn_item->object == 0) {
            qbn_item->object = (struct record *)object_find_by_id(location_object, (iptr)qbn_item->object);
        }
    }
    foe = (struct qbn_foe *)((iptr)quest + quest->section_offsets[7]);
    for (i = 0; quest->section_counts[7] > i; i++, foe++) {
        if (foe->object != 0) {
            foe->object = (struct record *)object_find_by_id(nonworld_root, (iptr)foe->object);
        }
    }
    timer = (struct qbn_timer *)((iptr)quest + quest->section_offsets[6]);
    for (i = 0; quest->section_counts[6] > i; i++, timer++) {
        if (timer->link1 != 0) timer->link1 = object_find_by_id(nonworld_root, (iptr)timer->link1);
        if (timer->link2 != 0) timer->link2 = object_find_by_id(nonworld_root, (iptr)timer->link2);
    }
    if (quest->text_offset == 0) return;
    text_var = (struct qbn_text_var *)((iptr)quest + quest->text_offset);
    while (text_var->name[0] != 0) {
        text_var->record = quest_record(quest, (int)(short)((unsigned short)text_var->section), text_var->index);
        text_var++;
    }
}

int quest_event_clicked_faction(unsigned short faction_id)
{
    struct record *object;
    struct record *next;
    struct qbn_op *op;
    int op_index;
    int found;

    found = 0;
    object = quest_root->children;
    while (object != 0) {
        next = object->next;
        if (object->type == 14) {
            quest_tick_object = object;
            current_quest = (struct quest *)((*(iptr *)&quest_tick_data = (iptr)&object->data.quest));
            op = quest_section(current_quest, 8);
            op_index = 0;
            for (; current_quest->section_counts[8] > op_index; op_index++, op++) {
                if (op->opcode == 78 && ((int)(unsigned short)faction_id) == op->args[1].value) {
                    quest_set_arg_state(current_quest, op, 0, 1);
                    found = 1;
                }
            }
        }
        object = next;
    }
    return found;
}

iptr quest_place_or_person_object(struct quest *quest, iptr record_index, short is_person)
{
    if (is_person == 0) return (iptr)((struct qbn_place *)quest_record(quest, 4, (int)(short)*(short *)&record_index))->object;
    return (iptr)((struct qbn_person *)quest_record(quest, 3, (int)(short)*(short *)&record_index))->object;
}

void quest_timer_expire(struct quest *quest, struct qbn_timer *timer)
{
    struct qbn_state *state;
    int state_count;
    int expired;

    if (((int)(short)(timer->flags & 3)) != 0) {
        timer->flags |= 128;
    } else if (((int)(short)(timer->flags & 4)) != 0) {
        timer->flags ^= 128;
    }
    if (((int)(short)(timer->flags & 8)) != 0) {
        quest_timer_update(quest, timer, 1);
    } else {
        timer->flags &= ~0x40;
    }
    state_count = quest->section_counts[9];
    state = quest_record(quest, 9, 0);
    while (state_count != 0) {
        if (state->name_hash == timer->state_hash) {
            expired = (int)(short)(timer->flags & 128);
            if (state->is_global != 0) {
                quest_global_states[state->value] = *(signed char *)&expired;
            } else {
                state->value = *(signed char *)&expired;
            }
            return;
        }
        state++;
        state_count--;
    }
}

void quest_timer_clear_state(struct qbn_timer *timer)
{
    int state_count;
    struct qbn_state *state;

    state_count = current_quest->section_counts[9];
    state = quest_record(current_quest, 9, 0);
    while (state_count != 0) {
        if (state->name_hash == timer->state_hash) {
            if (state->is_global != 0) {
                quest_global_states[state->value] = 0;
            } else {
                state->value = 0;
            }
            return;
        }
        state++;
        state_count--;
    }
}

int quest_state_from_timer(struct quest *quest, struct qbn_op *op)
{
    struct qbn_timer *timer;
    struct qbn_state *state;
    short expired;

    if (op->args[0].value == (-1)) return 0;
    timer = quest_record(quest, 6, (short)op->args[1].value);
    state = quest_record(quest, 9, (short)op->args[0].value);
    expired = timer->flags & 128;
    if (state->is_global != 0) {
        quest_global_states[state->value] = *(signed char *)&expired;
    } else {
        state->value = *(signed char *)&expired;
    }
    return (int)(short)expired;
}

int qaction_op25_countdown(struct quest *quest, struct qbn_op *op)
{
    struct qbn_arg *counter;
    struct qbn_state *state;

    counter = &op->args[2];
    state = (struct qbn_state *)op->args[1].record;
    if (state->is_global != 0) {
        if (quest_global_states[state->value] != 0) return 0;
    } else if (state->value != 0) {
        return 0;
    }
    if (counter->value != 0) counter->value--;
    if (counter->value == 0) {
        if (op->args[1].value == (-1)) return 1;
        if (state->is_global != 0) {
            quest_global_states[state->value] = 1;
        } else {
            state->value = 1;
        }
        return 1;
    }
    return 0;
}

void qaction_op11_remove_topics(struct quest *quest, struct qbn_op *op)
{
    int i;
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct qbn_item *qbn_item;

    for (i = 1; i < 4; i++) {
        if (op->args[i].value == (-1)) continue;
        switch ((unsigned)i) {
        case 1:
            qbn_place = quest_record(quest, 4, (short)op->args[i].value);
            qbn_place->flags |= 128;
            break;
        case 2:
            qbn_person = quest_record(quest, 3, (short)op->args[i].value);
            qbn_person->flags |= 0x8000;
            break;
        case 3:
            qbn_item = quest_record(quest, 0, (short)op->args[i].value);
            qbn_item->flags |= 128;
        }
    }
}

void qaction_op10_add_topics(struct quest *quest, struct qbn_op *op)
{
    int i;
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct qbn_item *qbn_item;

    for (i = 1; i < 4; i++) {
        if (op->args[i].value == (-1)) continue;
        switch ((unsigned)i) {
        case 1:
            qbn_place = quest_record(quest, 4, (short)op->args[i].value);
            qbn_place->flags &= 127;
            break;
        case 2:
            qbn_person = quest_record(quest, 3, (short)op->args[i].value);
            qbn_person->flags &= ~0x8000;
            break;
        case 3:
            qbn_item = quest_record(quest, 0, (short)op->args[i].value);
            qbn_item->flags &= 127;
        }
    }
}
