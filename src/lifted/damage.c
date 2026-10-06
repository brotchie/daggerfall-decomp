/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern iptr screen_buffer;
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
extern struct record *scratch_current_object;
extern struct record *location_object;
extern struct image *hud_bar_image;
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
extern struct record *spell_find_on_entity(struct record *, int, int);
extern int disk_resolve_path(iptr);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern int travel_route(int, int, int, int, int);
extern int rand();
extern int mc_memset();
extern iptr memchr();
extern int xn_vid_play();
extern int xn_math_angle_to_point();
extern int xn_timer_bios_ticks();
extern int xn_math_advance_pitch_yaw();
extern int xn_mouse_poll_clamped();
extern int xn_pal_fade_to();
extern iptr xn_tex_cache_lookup_image();
extern void quest_run_opcodes(struct quest *);
extern void quest_show_message(struct quest *, int);
extern void quest_op_done(struct quest *, struct qbn_op *);
extern void time_pass(int);
extern void palette_restore(void);
extern void cast_spell_on(struct record *, struct record *, int);
extern void disease_infect(struct record *, unsigned char *, int, int);
extern void flat_anim_restart(struct record *);
extern void object_foreach(struct record *, void (*)());
void disease_infect_lycanthropy(struct record *, int);
void disease_infect_vampirism(struct record *);
void damage_collapse_exhausted(struct record *);
void monster_wake_cb(struct record *);
void quest_prompt_answer(void);

void damage_monster_hit_effects(struct record *attacker, struct record *target)
{
    struct character *attacker_char;
    struct character *target_char;
    int fatigue;

    attacker_char = &attacker->data.character;
    target_char = &target->data.character;
    switch (attacker_char->race) {
    case 0:
        if (rand_range(0, 100) <= 5) disease_infect(target, monster_diseases_plague, 0, 0);
        return;
    case 3:
        if (rand_range(0, 100) <= 2) disease_infect(target, monster_diseases_bat, 0, 0);
        return;
    case 6:
        if (spell_find_on_entity(target, 66, 0) == 0) cast_creature_spell(attacker, target, 66);
        return;
    case 9:
        if (rand() < 400) disease_infect_lycanthropy(target, 0);
        return;
    case 10:
        fatigue = target_char->fatigue;
        fatigue -= rand_range(10, 30) << 6;
        if (fatigue < 0) fatigue = 0;
        target_char->fatigue = fatigue;
        if (fatigue == 0) damage_collapse_exhausted(target);
        return;
    case 14:
        if (rand() < 400) disease_infect_lycanthropy(target, 1);
        return;
    case 19:
        if (rand_range(1, 100) <= 5) disease_infect(target, monster_diseases_mummy, 0, 0);
        return;
    case 20:
        if (spell_find_on_entity(target, 66, 0) == 0) cast_creature_spell(attacker, target, 66);
        return;
    case 28:
    case 30:
        if (rand() < 400) {
            disease_infect_vampirism(target);
            return;
        }
        if (rand_range(1, 100) > 2) return;
        disease_infect(target, monster_diseases_plague, 0, 0);
    default:;
    }
}

void disease_infect_lycanthropy(struct record *target, int kind)
{
    if (player_character->level == 1 || player_character->race > 7) return;
    player_character->special_infection_time = game_minutes + 4320;
    player_character->special_infection = kind + 1;
    player_character->flags |= 16;
}

void disease_infect_vampirism(struct record *target)
{
    if (player_character->level == 1 || player_character->race > 7) return;
    player_character->special_infection_time = game_minutes + 4320;
    player_character->special_infection = 0;
    player_character->flags |= 16;
}

void damage_collapse_exhausted(struct record *target)
{
    int start_ticks;
    int unused;

    start_ticks = xn_timer_bios_ticks();
    mc_memset(655360, 0, ((((int)(unsigned short)(*(short *)((char *)((iptr)game_settings)) & 1)) != 0) ? 64000 : hud_bar_image->y * 320), (iptr)D_001709E4, 701, 4);
    time_pass(20160);
    while ((xn_timer_bios_ticks() - start_ticks) < 22);
}

void damage_spawn_splash(struct record *target, int image_record, int alt_image_record)
{
    int angle;
    struct record *splash;
    struct character *target_char;
    struct texture_header *image;

    splash = object_create_child(target->parent, 0, 0);
    splash->id = object_new_id(((unsigned)location_object->id) >> 16);
    splash->type = 42;
    splash->x = target->x;
    image = (struct texture_header *)xn_tex_cache_lookup_image(target->image >> 7, (int)(unsigned short)(target->image & 127));
    splash->y = target->y - ((image->height) >> 1);
    if (target->type == 18) {
        target_char = &target->data.character;
        if (target_char->race == 1 || target_char->race == 0 || target_char->race == 3) splash->y += 10;
    }
    splash->z = target->z;
    splash->image = image_record + (D_00195DA0 << 7);
    if (alt_image_record != (-1)) {
        target_char = &target->data.character;
        if (target_char->mobile_id < 128 && *(signed char *)(monster_category + target_char->race) == 0) {
            splash->image = alt_image_record + (D_00195DA0 << 7);
        }
    }
    angle = xn_math_angle_to_point(target->x, target->z, player_object->x, player_object->z);
    xn_math_advance_pitch_yaw(0, angle, 10, &splash->x);
    flat_anim_restart(splash);
}

void func_0002F62C(struct record *target, int force, int angle, int amount)
{
    int weight;
    int unused;
    struct character *target_char;

    weight = object_weight(target);
    if (weight == 0) return;
    target_char = &target->data.character;
    if (target_char->race < 43 && *(short *)(monster_weights + (target_char->race * 2)) == 0) return;
    if (target_char->action == 16) return;
    target_char->knockback_speed = (amount * (((force - weight) << 8) / (force + weight))) / 256;
    target_char->knockback_speed = (force / weight) * (amount - target_char->knockback_speed);
    if (target_char->knockback_speed < 15) target_char->knockback_speed = 15;
    target_char->knockback_angle = angle;
    target_char->flags |= 32;
    target_char->action = 16;
}

void damage_knockback(struct record *target, int force, int angle, int amount)
{
    int weight;
    int absorbed;
    int unused;
    struct character *target_char;

    weight = object_weight(target);
    if (weight == 0) return;
    target_char = &target->data.character;
    if (target_char->race < 43 && *(short *)(monster_weights + (target_char->race * 2)) == 0) return;
    if (target_char->action == 16) return;
    absorbed = (amount * (((force - weight) << 8) / (force + weight))) / 256;
    target_char->knockback_speed = (force / weight) * (amount - absorbed);
    if (target_char->knockback_speed < 15) target_char->knockback_speed = 15;
    target_char->knockback_angle = angle;
    target_char->flags |= 32;
    target_char->action = 16;
}

void damage_expire_drain_bonuses(void)
{
    if (D_00195A0C != 0 && ((unsigned)game_minutes) > D_00195A0C) {
        spell_points_bonus = 0;
    }
    if (D_00195A78 == 0 || ((unsigned)game_minutes) <= D_00195A78) return;
    *(int *)D_00195A08 = 0;
}

int damage_miss_sound(struct item *weapon, int target_id)
{
    if (weapon != 0) {
        if (target_id == (-1) || target_id == 200) {
            return (int)(short)*(short *)(weapon_swing_sounds + (weapon->index * 2));
        }
        if (memchr((iptr)monster_parry_ids, target_id, 28) != 0) {
            if (rand() < 32768) return rand_range(291, 299);
        }
        return (int)(short)*(short *)(weapon_swing_sounds + (weapon->index * 2));
    }
    return 374;
}

void play_death_video(void)
{
    int unused[11];
    int path;

    xn_pal_fade_to((iptr)D_00196DC4, 50);
    mc_memset(655360, 0, 64000, (iptr)D_001709E4, 875, 4);
    mc_memset(screen_buffer, 0, 64000, (iptr)D_001709E4, 876, 4);
    palette_restore();
    path = disk_resolve_path((iptr)D_001709FB);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    xn_vid_play(path, 0, 0, 1);
    mc_memset(655360, 0, 64000, (iptr)D_001709E4, 883, 4);
    mc_memset(screen_buffer, 0, 64000, (iptr)D_001709E4, 884, 4);
    palette_restore();
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

void monster_wake_cb(struct record *object)
{
    if (object->type != 18 || object->wait_state != 99) return;
    object->wait_state = 0;
}

void monster_wake_all(void)
{
    if (monsters_woken != 0) return;
    monsters_woken = 1;
    object_foreach(location_object, monster_wake_cb);
}

void func_0002FD75(struct record *object)
{
    struct quest *quest;

    if (object->type != 14) return;
    quest = &object->data.quest;
    quest_run_opcodes(quest);
}

void func_0002FDB0(struct record *object)
{
    struct quest *quest;

    if (object->type != 14) return;
    quest = &object->data.quest;
    if (quest->id != D_001997AA) return;
    quest_tick_data = quest;
    quest_event_object = object;
}

void func_0002FE02(struct record *object)
{
    if (object->quest_id == 0 || ((int)(unsigned short)(object->flags & 32768)) == 0) return;
    object->quest_id = 0;
    object->flags &= ~0x8000;
}

void func_0002FE4B(struct record *object)
{
    short foe_id;

    if (object->type != 18) return;
    if ((short)((int)(unsigned char)(signed char)object->quest_id) != current_quest->id) return;
    foe_id = *(short *)scratch_190d64;
    if ((short)object->image2 != foe_id) return;
    if (scratch_190ce4[0] != 0) {
        object->data.character.flags |= 0x8000;
        return;
    }
    object->data.character.flags &= ~0x8000;
}

void quest_cast_spell_on_foe_cb(struct record *object)
{
    if (object->type != 18) return;
    if ((short)(object->quest_id) != current_quest->id) return;
    if (object->image2 != *(short *)scratch_190d64) return;
    D_00196291 = 1;
    cast_spell_on(scratch_current_object, object, 1);
    D_00196291 = 0;
}

unsigned int quest_travel_minutes(struct quest *quest, struct record *from, struct record *to)
{
    if (from == 0) return travel_route(player_object->x, player_object->y, to->x, to->y, 0) + 2880;
    return travel_route(from->x, from->y, to->x, to->y, 0) + 2880;
}

void qaction_op35_cycle_state(struct quest *quest, struct qbn_op *op)
{
    struct qbn_state *states[4];
    int count;
    short i;

    *(int *)&i = 0;
    count = (int)(short)i;
    for (; ((int)(short)i) < 4; (*(int *)&i)++) {
        if (op->args[((int)(short)i) + 1].value != (-1) && op->args[((int)(short)i) + 1].value != (-2)) {
            states[count++] = (struct qbn_state *)op->args[((int)(short)i) + 1].record;
        }
    }
    if (count == 0) return;
    if (count == 1) {
        if (states[0]->is_global != 0) {
            quest_global_states[states[0]->value] = 1;
        } else {
            states[0]->value = 1;
        }
        return;
    }
    *(int *)&i = 0;
    for (; ((int)(short)i) < count; (*(int *)&i)++) {
        if (states[(int)(short)i]->is_global != 0) {
            if (quest_global_states[states[(int)(short)i]->value] != 0) break;
        } else {
            if (states[(int)(short)i]->value != 0) break;
        }
    }
    if (((int)(short)i) == count) {
        if (states[0]->is_global != 0) {
            quest_global_states[states[0]->value] = 1;
        } else {
            states[0]->value = 1;
        }
        return;
    }
    if (states[(int)(short)i]->is_global != 0) {
        quest_global_states[states[(int)(short)i]->value] = 0;
    } else {
        states[(int)(short)i]->value = 0;
    }
    if (states[(((int)(short)i) + 1) % count]->is_global != 0) {
        quest_global_states[states[(((int)(short)i) + 1) % count]->value] = 1;
        return;
    }
    states[(((int)(short)i) + 1) % count]->value = 1;
}

void qaction_op29_prompt(struct quest *quest, struct qbn_op *op)
{
    D_001940DA |= 32;
    quest_prompt_op = op;
    quest_prompt_quest = quest;
    quest_op_done(quest, op);
    quest_show_message(quest, op->args[3].value);
    quest_prompt_answer();
}

void quest_prompt_answer(void)
{
    struct qbn_state *state;

    if (((struct bf8_5_1 *)&D_001940DA)->f == 0 || game_mode != 0) return;
    state = *(struct qbn_state **)((char *)quest_prompt_op + 7 + (((int)D_00196271) * 15));
    if (state->is_global != 0) {
        quest_global_states[state->value] = 1;
    } else {
        state->value = 1;
    }
    D_001940DA &= 223;
}

void quest_set_arg_state(struct quest *quest, struct qbn_op *op, int arg_index, int value)
{
    struct qbn_state *state;

    if (op->args[arg_index].value == (-1)) return;
    state = (struct qbn_state *)op->args[arg_index].record;
    if (((int)(unsigned char)(op->args[arg_index].negate & 1)) != 0) value ^= 1;
    if (state->is_global != 0) {
        quest_global_states[state->value] = *(signed char *)&value;
        return;
    }
    state->value = *(signed char *)&value;
}
