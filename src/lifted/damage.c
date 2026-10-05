/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern signed char mouse_buttons;
extern int screen_buffer;
extern char D_001709E4[];
extern char D_001709FB[];
extern char monster_diseases_bat[];
extern char monster_diseases_mummy[];
extern char monster_diseases_plague[];
extern char weapon_swing_sounds[];
extern char monster_parry_ids[];
extern char monster_weights[];
extern char monster_category[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern signed char D_001940DA;
extern signed char quest_global_states[];
extern int spell_points_bonus;
extern char D_00195A08[];
extern int D_00195A0C;
extern int D_00195A78;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *location_object;
extern char hud_bar_image[];
extern struct character *player_character;
extern int game_minutes;
extern struct settings *game_settings;
extern int sky_loaded_frame;
extern short D_00195DA0;
extern unsigned char D_00196271;
extern signed char game_mode;
extern signed char D_00196291;
extern signed char night_sky_loaded;
extern char D_00196DC4[];
extern signed char monsters_woken;
extern struct quest *current_quest;
extern struct record *quest_event_object;
extern struct quest *quest_tick_data;
extern struct qbn_op *quest_prompt_op;
extern struct quest *quest_prompt_quest;
extern short D_001997AA;

extern int object_weight(struct record *);
extern int cast_creature_spell(struct record *, struct record *, int);
extern struct record *spell_find_on_entity(struct record *, short, int);
extern int disk_resolve_path(int);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int travel_route(int, int, int, int, int);
extern int rand();
extern int mc_memset();
extern int memchr();
extern int xn_vid_play();
extern int xn_math_angle_to_point();
extern int xn_timer_bios_ticks();
extern int xn_math_advance_pitch_yaw();
extern int xn_mouse_poll_clamped();
extern int xn_pal_fade_to();
extern int xn_tex_cache_lookup_image();
extern void quest_run_opcodes(struct quest *);
extern void quest_show_message(struct quest *, int);
extern void quest_op_done(struct quest *, struct qbn_op *);
extern void time_pass(int);
extern void palette_restore(void);
extern void cast_spell_on(struct record *, struct record *, int);
extern void disease_infect(struct record *, int, int, int);
extern void flat_anim_restart(struct record *);
extern void object_foreach(struct record *, int);
void disease_infect_lycanthropy(struct record *, int);
void disease_infect_vampirism(struct record *);
void damage_collapse_exhausted(struct record *);
void monster_wake_cb(struct record *);
void quest_prompt_answer(void);

void damage_monster_hit_effects(struct record *a1, struct record *a2)
{
    struct character *l_1C;
    struct character *l_18;
    int l_14;

    l_1C = &a1->data.character;
    l_18 = &a2->data.character;
    switch (l_1C->race) {
    case 0:
        if (rand_range(0, 100) <= 5) disease_infect(a2, (int)monster_diseases_plague, 0, 0);
        return;
    case 3:
        if (rand_range(0, 100) <= 2) disease_infect(a2, (int)monster_diseases_bat, 0, 0);
        return;
    case 6:
        if (spell_find_on_entity(a2, 66, 0) == 0) cast_creature_spell(a1, a2, 66);
        return;
    case 9:
        if (rand() < 400) disease_infect_lycanthropy(a2, 0);
        return;
    case 10:
        l_14 = l_18->fatigue;
        l_14 -= rand_range(10, 30) << 6;
        if (l_14 < 0) l_14 = 0;
        l_18->fatigue = l_14;
        if (l_14 == 0) damage_collapse_exhausted(a2);
        return;
    case 14:
        if (rand() < 400) disease_infect_lycanthropy(a2, 1);
        return;
    case 19:
        if (rand_range(1, 100) <= 5) disease_infect(a2, (int)monster_diseases_mummy, 0, 0);
        return;
    case 20:
        if (spell_find_on_entity(a2, 66, 0) == 0) cast_creature_spell(a1, a2, 66);
        return;
    case 28:
    case 30:
        if (rand() < 400) {
            disease_infect_vampirism(a2);
            return;
        }
        if (rand_range(1, 100) > 2) return;
        disease_infect(a2, (int)monster_diseases_plague, 0, 0);
    default:;
    }
}

void disease_infect_lycanthropy(struct record *a1, int a2)
{
    if (player_character->level == 1 || player_character->race > 7) return;
    player_character->special_infection_time = game_minutes + 4320;
    player_character->special_infection = a2 + 1;
    player_character->flags |= 16;
}

void disease_infect_vampirism(struct record *a1)
{
    if (player_character->level == 1 || player_character->race > 7) return;
    player_character->special_infection_time = game_minutes + 4320;
    player_character->special_infection = 0;
    player_character->flags |= 16;
}

void damage_collapse_exhausted(struct record *a1)
{
    int l_1C;
    int l_18;

    l_1C = xn_timer_bios_ticks();
    mc_memset(655360, 0, ((((int)(unsigned short)(*(short *)((char *)((int)game_settings)) & 1)) != 0) ? 64000 : ((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2)) * 320), (int)D_001709E4, 701, 4);
    time_pass(20160);
    while ((xn_timer_bios_ticks() - l_1C) < 22);
}

void damage_spawn_splash(struct record *a1, int a2, int a3)
{
    int l_1C;
    struct record *l_18;
    struct character *l_14;
    short l_10;

    l_18 = object_create_child(a1->parent, 0, 0);
    l_18->id = object_new_id(((unsigned)location_object->id) >> 16);
    l_18->type = 42;
    l_18->x = a1->x;
    *(int *)&l_10 = xn_tex_cache_lookup_image(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    l_18->y = a1->y - (((int)(unsigned short)*(short *)(*(char **)&l_10 + 6)) >> 1);
    if (a1->type == 18) {
        l_14 = &a1->data.character;
        if (l_14->race == 1 || l_14->race == 0 || l_14->race == 3) l_18->y += 10;
    }
    l_18->z = a1->z;
    l_18->image = a2 + (D_00195DA0 << 7);
    if (a3 != (-1)) {
        l_14 = &a1->data.character;
        if (l_14->mobile_id < 128 && *(signed char *)(monster_category + l_14->race) == 0) {
            l_18->image = a3 + (D_00195DA0 << 7);
        }
    }
    l_1C = xn_math_angle_to_point(a1->x, a1->z, player_object->x, player_object->z);
    xn_math_advance_pitch_yaw(0, l_1C, 10, &l_18->x);
    flat_anim_restart(l_18);
}

void func_0002F62C(struct record *a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    struct character *l_C;

    l_14 = object_weight(a1);
    if (l_14 == 0) return;
    l_C = &a1->data.character;
    if (l_C->race < 43 && *(short *)(monster_weights + (l_C->race * 2)) == 0) return;
    if (l_C->action == 16) return;
    l_C->knockback_speed = (a4 * (((a2 - l_14) << 8) / (a2 + l_14))) / 256;
    l_C->knockback_speed = (a2 / l_14) * (a4 - l_C->knockback_speed);
    if (l_C->knockback_speed < 15) l_C->knockback_speed = 15;
    l_C->knockback_angle = a3;
    l_C->flags |= 32;
    l_C->action = 16;
}

void damage_knockback(struct record *a1, int a2, int a3, int a4)
{
    int l_18;
    int l_14;
    int l_10;
    struct character *l_C;

    l_18 = object_weight(a1);
    if (l_18 == 0) return;
    l_C = &a1->data.character;
    if (l_C->race < 43 && *(short *)(monster_weights + (l_C->race * 2)) == 0) return;
    if (l_C->action == 16) return;
    l_14 = (a4 * (((a2 - l_18) << 8) / (a2 + l_18))) / 256;
    l_C->knockback_speed = (a2 / l_18) * (a4 - l_14);
    if (l_C->knockback_speed < 15) l_C->knockback_speed = 15;
    l_C->knockback_angle = a3;
    l_C->flags |= 32;
    l_C->action = 16;
}

void damage_expire_drain_bonuses(void)
{
    if (D_00195A0C != 0 && ((unsigned)game_minutes) > D_00195A0C) {
        spell_points_bonus = 0;
    }
    if (D_00195A78 == 0 || ((unsigned)game_minutes) <= D_00195A78) return;
    *(int *)D_00195A08 = 0;
}

int damage_miss_sound(struct item *a1, int a2)
{
    if (a1 != 0) {
        if (a2 == (-1) || a2 == 200) {
            return (int)(short)*(short *)(weapon_swing_sounds + (a1->index * 2));
        }
        if (memchr((int)monster_parry_ids, a2, 28) != 0) {
            if (rand() < 32768) return rand_range(291, 299);
        }
        return (int)(short)*(short *)(weapon_swing_sounds + (a1->index * 2));
    }
    return 374;
}

void play_death_video(void)
{
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    xn_pal_fade_to((int)D_00196DC4, 50);
    mc_memset(655360, 0, 64000, (int)D_001709E4, 875, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_001709E4, 876, 4);
    palette_restore();
    l_18 = disk_resolve_path((int)D_001709FB);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    xn_vid_play(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_001709E4, 883, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_001709E4, 884, 4);
    palette_restore();
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

void monster_wake_cb(struct record *a1)
{
    if (a1->type != 18 || a1->wait_state != 99) return;
    a1->wait_state = 0;
}

void monster_wake_all(void)
{
    if (monsters_woken != 0) return;
    monsters_woken = 1;
    object_foreach(location_object, (int)monster_wake_cb);
}

void func_0002FD75(struct record *a1)
{
    struct quest *l_18;

    if (a1->type != 14) return;
    l_18 = &a1->data.quest;
    quest_run_opcodes(l_18);
}

void func_0002FDB0(struct record *a1)
{
    struct quest *l_18;

    if (a1->type != 14) return;
    l_18 = &a1->data.quest;
    if (l_18->id != D_001997AA) return;
    quest_tick_data = l_18;
    quest_event_object = a1;
}

void func_0002FE02(struct record *a1)
{
    if (a1->quest_id == 0 || ((int)(unsigned short)(a1->flags & 32768)) == 0) return;
    a1->quest_id = 0;
    a1->flags &= ~0x8000;
}

void func_0002FE4B(struct record *a1)
{
    short l_18;

    if (a1->type != 18) return;
    if ((short)((int)(unsigned char)(signed char)a1->quest_id) != current_quest->id) return;
    l_18 = *(short *)scratch_190d64;
    if ((short)a1->image2 != l_18) return;
    if (scratch_190ce4[0] != 0) {
        a1->data.character.flags |= 0x8000;
        return;
    }
    a1->data.character.flags &= ~0x8000;
}

void quest_cast_spell_on_foe_cb(struct record *a1)
{
    if (a1->type != 18) return;
    if ((short)(a1->quest_id) != current_quest->id) return;
    if (a1->image2 != *(short *)scratch_190d64) return;
    D_00196291 = 1;
    cast_spell_on(D_00195AA8, a1, 1);
    D_00196291 = 0;
}

int quest_travel_minutes(int a1, struct record *a2, struct record *a3)
{
    if (a2 == 0) return travel_route(player_object->x, player_object->y, a3->x, a3->y, 0) + 2880;
    return travel_route(a2->x, a2->y, a3->x, a3->y, 0) + 2880;
}

void qaction_op35_cycle_state(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_state *l_30[4];
    int l_18;
    short l_14;

    *(int *)&l_14 = 0;
    l_18 = (int)(short)l_14;
    for (; ((int)(short)l_14) < 4; (*(int *)&l_14)++) {
        if (a2->args[((int)(short)l_14) + 1].value != (-1) && a2->args[((int)(short)l_14) + 1].value != (-2)) {
            l_30[l_18++] = (struct qbn_state *)a2->args[((int)(short)l_14) + 1].record;
        }
    }
    if (l_18 == 0) return;
    if (l_18 == 1) {
        if (l_30[0]->is_global != 0) {
            quest_global_states[l_30[0]->value] = 1;
        } else {
            l_30[0]->value = 1;
        }
        return;
    }
    *(int *)&l_14 = 0;
    for (; ((int)(short)l_14) < l_18; (*(int *)&l_14)++) {
        if (l_30[(int)(short)l_14]->is_global != 0) {
            if (quest_global_states[l_30[(int)(short)l_14]->value] != 0) break;
        } else {
            if (l_30[(int)(short)l_14]->value != 0) break;
        }
    }
    if (((int)(short)l_14) == l_18) {
        if (l_30[0]->is_global != 0) {
            quest_global_states[l_30[0]->value] = 1;
        } else {
            l_30[0]->value = 1;
        }
        return;
    }
    if (l_30[(int)(short)l_14]->is_global != 0) {
        quest_global_states[l_30[(int)(short)l_14]->value] = 0;
    } else {
        l_30[(int)(short)l_14]->value = 0;
    }
    if (l_30[(((int)(short)l_14) + 1) % l_18]->is_global != 0) {
        quest_global_states[l_30[(((int)(short)l_14) + 1) % l_18]->value] = 1;
        return;
    }
    l_30[(((int)(short)l_14) + 1) % l_18]->value = 1;
}

void qaction_op29_prompt(struct quest *a1, struct qbn_op *a2)
{
    D_001940DA |= 32;
    quest_prompt_op = a2;
    quest_prompt_quest = a1;
    quest_op_done(a1, a2);
    quest_show_message(a1, a2->args[3].value);
    quest_prompt_answer();
}

void quest_prompt_answer(void)
{
    struct qbn_state *l_18;

    if (((struct bf8_5_1 *)&D_001940DA)->f == 0 || game_mode != 0) return;
    l_18 = *(struct qbn_state **)((char *)quest_prompt_op + 7 + (((int)D_00196271) * 15));
    if (l_18->is_global != 0) {
        quest_global_states[l_18->value] = 1;
    } else {
        l_18->value = 1;
    }
    D_001940DA &= 223;
}

void quest_set_arg_state(struct quest *a1, struct qbn_op *a2, int a3, int a4)
{
    struct qbn_state *l_C;

    if (a2->args[a3].value == (-1)) return;
    l_C = (struct qbn_state *)a2->args[a3].record;
    if (((int)(unsigned char)(a2->args[a3].negate & 1)) != 0) a4 ^= 1;
    if (l_C->is_global != 0) {
        quest_global_states[l_C->value] = *(signed char *)&a4;
        return;
    }
    l_C->value = *(signed char *)&a4;
}
