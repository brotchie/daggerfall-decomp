/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#include "doslow.h"

struct missile_step { int x, y, z; struct record *light; };   /* a step vector, then the missile's light */
extern short xn_cam_centre_x;
extern short xn_cam_centre_y;
extern short mouse_x;
extern short mouse_y;
extern signed char D_00153408;
extern int D_0015340D;
extern short D_00153411;
extern char D_001757F4[];
extern char D_001757FF[];
extern char D_00175820[];
extern char D_00175837[];
extern char D_00175858[];
extern char D_0017586C[];
extern char D_00175881[];
extern short cast_anim_state;
extern signed char spell_effect_school[];
extern char spell_effect_settings[];
extern iptr D_0018509B;
extern short spell_last_cast_id;
extern char spell_missile_textures[];
extern char spell_cast_sounds[];
extern char spell_impact_sounds[];
extern signed char magic_school_skills[];
extern signed char spell_element_class_bits[];
extern short D_00185CEC[];
extern struct record *creature_list[];
extern int view_look_pitch;
extern int view_look_yaw;
extern int spell_points_bonus;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern char cheat_flags[];
extern struct spell *spell_records;
extern int creature_count;
extern struct record *spell_ready_missile;
extern struct record *spell_ready_touch;
extern struct record *scratch_object;
extern struct image *hud_bar_image;
extern struct character *player_character;
extern struct settings *game_settings;
extern struct record *D_00195C48;
extern iptr spell_cast_anim_fire[];
extern char D_00195F28[];
extern short spell_effect_slot;
extern short spell_ready_cost;
extern signed char D_00196291;
extern signed char D_00196292;
extern struct spell *D_00199D64;
extern int D_00199D6C;
extern signed char D_00199D71;

extern int collide_line_of_sight(struct record *, struct record *);
extern int collide_creature_within(struct record *, iptr, int);
extern int spell_cost(struct spell *, struct character *);
extern int player_in_daylight(void);
extern int cast_player_spell(struct record *);
extern int cast_item_spell_at(struct record *, struct record *);
extern int cast_creature_spell_at(struct record *, struct record *, struct record *);
extern int spell_missile_update(struct record *, int);
extern int sound_play(int, struct record *, int);
extern iptr hud_message_add(iptr);
extern int rand_range(int, int);
extern int spfx_resist_roll(int, int, struct character *, struct career *, int, int);
extern int spell_extend_duration(struct record *, struct spell *, int);
extern struct record *object_delete(struct record *);
extern struct record *object_clone(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_item(struct record *, short, short);
extern int object_new_id(int);
extern void *xn_vec_unit_direction(void *, void *, void *);
extern void *xn_vec_advance(void *, int, void *);
extern int xn_math_approx_dist2d(int, int, int, int);
extern int xn_math_approx_hypot(int, int);
extern int xn_math_angle_to_point(int, int, int, int);
extern int spell_effect_dispatch(int, void *, int, void *);
extern void xn_draw_spell_icon(int, int, int);
extern void xn_draw_cast_anim_mirrored(void *, int, int);
extern int spell_find_effect_type(char *, int);
extern int spell_has_no_effects(void *);
extern void xn_math_advance_pitch_yaw(int, int, int, void *);
extern void damage_spawn_splash(struct record *, int, int);
extern void damage_knockback(struct record *, int, int, int);
extern void spell_add_skill_uses(struct spell *, int);
extern void spell_cast_queue(struct record *, struct record *);
extern void spell_end(struct record *);
extern void object_foreach(struct record *, void (*)());
int spell_resist_check(struct record *, struct record **);
struct spell *spell_find_active_effect(struct record *, int, int *, int *);
int spell_apply_effect(struct record *, int, struct record *);
void spellbook_find_last_cast_cb(struct record *);
void spell_compute_values(struct spell *, uslot16, int);
void func_0005C856(struct record *, struct record *);
void cast_anim_start(int);
void func_0005CA28(struct record *);
void spell_lookup_name(struct spell *);

int cast_item_strike_spell(int spell_id, struct record *target)
{
    int i;
    struct record *spell;

    i = 0;
    spell = object_create_child(player_object->parent, 0, 89);
    while (spell_records[i].name[0] == 0 || spell_records[i].id != spell_id) i++;
    spell->type = 9;
    spell->id = object_new_id(((unsigned)location_object->id) >> 16);
    mc_memcpy(&spell->data.spell, &spell_records[i], 89, D_001757F4, 121, 4);
    spell->data.spell.icon = 250;
    i = spell_cost(&spell->data.spell, player_character);
    if (cast_item_spell_at(spell, target) != 0) object_delete(spell);
    return i;
}

int cast_creature_spell(struct record *caster, struct record *target, int spell_id)
{
    int i;
    struct record *spell;

    i = 0;
    spell = object_create_child(location_object, 0, 89);
    while (spell_records[i].name[0] == 0 || spell_records[i].id != spell_id) i++;
    spell->type = 9;
    spell->id = object_new_id(((unsigned)location_object->id) >> 16);
    spell->caster = caster;
    mc_memcpy(&spell->data.spell, &spell_records[i], 89, D_001757F4, 144, 4);
    if (D_00196292 != 0) spell->data.spell.icon = 250;
    i = spell_cost((struct spell *)((char *)spell + 89), &caster->data.character);
    if (cast_creature_spell_at(spell, caster, target) != 0) object_delete(spell);
    return i;
}

void cast_spell_on(struct record *spell, struct record *target, int no_save)
{
    struct record *cast;
    int slot;
    int lasting;
    int extended;
    int percent;
    struct spell *spell_data;

    extended = 0;
    if (spell->caster != player_entity && ((struct bf8_7_1 *)&cheat_flags)->f != 0) return;
    if (spell->caster == player_entity && target == player_entity && spell->data.spell.target == 3) return;
    *(short *)D_00195F28 = 512;
    spell_data = &spell->data.spell;
    if (spell_data->target != 2 && spell_data->target != 4) {
        sound_play((int)(short)*(short *)(spell_cast_sounds + (spell_data->element * 2)), player_object, 110);
        if (spell->caster == player_entity && D_00196291 == 0) {
            cast_anim_start(spell_data->element);
        } else {
            damage_spawn_splash(spell->caster, 3, 3);
        }
    }
    if (no_save == 0) {
        percent = spell_resist_check(spell, &target);
        if (percent == 0) {
            hud_message_add(D_0018509B);
            return;
        }
    } else {
        percent = 100;
    }
    cast = object_clone(spell);
    cast->caster = spell->caster;
    cast->id = object_new_id(801);
    spell_data = &cast->data.spell;
    spell_compute_values(spell_data, cast->caster->data.character.level, percent);
    slot = 0;
    lasting = slot;
    for (; slot < 3; slot++) {
        if (spell_data->effects[slot].type == 255) continue;
        if ((spell_data->icon < 200 || spell_data->icon == 250) && spell_extend_duration(target, spell_data, slot) != 0) {
            extended++;
        } else {
            spell_apply_effect(cast, slot, target);
        }
        if (spell_data->effects[slot].type != 255 && ((int)(unsigned char)(*(signed char *)(spell_effect_settings + (spell_data->effects[slot].type * 12)) & 1)) != 0) {
            lasting++;
        }
    }
    lasting -= extended;
    if (lasting == 0) {
        object_delete(cast);
        return;
    }
    object_reparent(target, cast);
}

void spellbook_find_last_cast_cb(struct record *object)
{
    if (object->type != 9) return;
    if ((short)((unsigned short)object->data.spell.id) != spell_last_cast_id) return;
    scratch_object = object;
}

int cast_recast_last(void)
{
    struct record *found;
    struct record *cast;
    struct spell *spell_data;
    int cost;

    if ((iptr)spell_ready_missile != 0 || (iptr)spell_ready_touch != 0) {
        hud_message_add((iptr)D_001757FF);
        return 0;
    }
    if (((int)(short)spell_last_cast_id) == (-1)) return 0;
    found = object_find_item(player_entity->children, 27, 0);
    if (found == 0) {
        hud_message_add((iptr)D_00175820);
        return 0;
    }
    scratch_object = 0;
    object_foreach(found->children, spellbook_find_last_cast_cb);
    found = scratch_object;
    spell_data = &found->data.spell;
    cost = (int)(short)spell_ready_cost;
    if ((player_character->magicka + spell_points_bonus) < cost) {
        hud_message_add((iptr)D_00175837);
        return 0;
    }
    spell_add_skill_uses(spell_data, 1);
    if (spell_points_bonus != 0) {
        if (cost > spell_points_bonus) {
            cost -= spell_points_bonus;
            spell_points_bonus = 0;
        } else {
            spell_points_bonus -= cost;
            spell_ready_cost = 0;
        }
    }
    player_character->magicka -= cost;
    cast = object_create_child(player_object->parent, 0, 89);
    cast->type = 9;
    cast->id = object_new_id(((unsigned)location_object->id) >> 16);
    mc_memcpy(&cast->data.spell, &found->data.spell, 89, D_001757F4, 443, 4);
    if (cast_player_spell(cast) != 0) object_delete(cast);
    return 1;
}

int spell_resist_check(struct record *spell, struct record **target)
{
    struct character *target_char;
    struct career *target_class;
    int unused;
    int cost;
    int chance;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    if (spell_data->target == 0) return 100;
    if (spell->caster == *target && spell_data->target == 3) return 100;
    target_char = &(*target)->data.character;
    target_class = &target_char->career;
    if (((int)(unsigned char)(target_class->spell_absorption_flags & 7)) != 0 || (target_char->conditions & 0x200) != 0) {
        if ((target_char->conditions & 0x200) != 0 || ((int)(unsigned char)(target_class->spell_absorption_flags & 4)) != 0 || (((int)(unsigned char)(target_class->spell_absorption_flags & 2)) != 0 && player_in_daylight() == 0) || (((int)(unsigned char)(target_class->spell_absorption_flags & 1)) != 0 && player_in_daylight() != 0)) {
            if ((target_char->conditions & 0x200) != 0) {
                if (spell_find_active_effect(*target, 20, &chance, 0) == 0) {
                    chance = target_char->level + 25;
                    if (rand_range(1, 100) > chance) goto L5B5A7;
                } else {
                    if (rand_range(1, 100) > chance) goto L5B5A7;
                }
            }
            cost = spell_cost(spell_data, target_char);
            if ((cost + target_char->magicka) <= target_char->max_magicka) {
                target_char->magicka += cost;
                hud_message_add((iptr)D_00175858);
                return 0;
            }
        }
    }
L5B5A7:;
    if ((target_char->conditions & 0x400) != 0) {
        spell_find_active_effect(*target, 21, &chance, 0);
        if (rand_range(1, 100) <= chance) {
            *target = spell->caster;
            target_char = &(*target)->data.character;
            target_class = &target_char->career;
            hud_message_add((iptr)D_0017586C);
        }
    }
    if ((target_char->conditions & 0x800) != 0) {
        spell_find_active_effect(*target, 22, &chance, 0);
        if (rand_range(1, 100) <= chance) {
            hud_message_add((iptr)D_00175881);
            return 0;
        }
    }
    return spfx_resist_roll(spell_data->element, (int)(unsigned char)spell_element_class_bits[spell_data->element], target_char, target_class, 2, (-(spell->caster->data.character.level - target_char->level)) * 5);
}

int spell_base_cost(struct spell *spell, struct character *caster, int unused)
{
    int i;
    int cost;
    int total;

    i = 0;
    total = i;
    for (; i < 3; i++) {
        if (spell->effects[i].type == 255) continue;
        cost = spell->effect_costs[i];
        cost = ((110 - caster->skills[(int)(unsigned char)magic_school_skills[(int)(unsigned char)spell_effect_school[spell->effects[i].type]]].value) * cost) / 100;
        total += cost;
    }
    return total;
}

struct spell *spell_find_active_effect(struct record *object, int effect_type, int *chance_out, int *magnitude_out)
{
    struct spell *spell;
    int slot;

    object = object->children;
    while (object != 0) {
        if (object->type == 9) {
            spell = &object->data.spell;
            for (slot = 0; slot < 3; slot++) {
                if (spell->effects[slot].type == effect_type) {
                    if (chance_out != 0) *chance_out = spell->cast_chances[slot];
                    if (magnitude_out != 0) *magnitude_out = spell->cast_magnitudes[slot];
                    spell_effect_slot = slot;
                    scratch_object = object;
                    return spell;
                }
            }
        }
        object = object->next;
    }
    return 0;
}

int spell_apply_effect(struct record *spell, int slot, struct record *target)
{
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    return spell_effect_dispatch(spell_data->effects[slot].type, spell, slot, target);
}

void spell_compute_values(struct spell *spell, uslot16 level, int percent)
{
    int slot;

    if (spell->icon >= 200) *(int *)&level = 8;
    for (slot = 0; slot < 3; slot++) {
        if (spell->effects[slot].type == 255) continue;
        if (((int)spell->durations[slot].base) != (-1)) {
            if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (spell->effects[slot].type * 12)) & 1)) != 0) {
                spell->cast_durations[slot] = ((((int)spell->durations[slot].base) + (((int)spell->durations[slot].plus) * (((int)(unsigned short)level) / ((int)spell->durations[slot].per_level)))) * percent) / 100;
            } else {
                spell->cast_durations[slot] = 0;
            }
        } else {
            spell->cast_durations[slot] = 65535;
        }
        if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (spell->effects[slot].type * 12)) & 2)) != 0) {
            spell->cast_chances[slot] = ((int)spell->chances[slot].base) + (((int)spell->chances[slot].plus) * (((int)(unsigned short)level) / ((int)spell->chances[slot].per_level)));
        }
        if (((int)(unsigned char)(*(signed char *)(spell_effect_settings + (spell->effects[slot].type * 12)) & 4)) != 0) {
            spell->cast_magnitudes[slot] = (percent * (rand_range((int)spell->magnitudes[slot].base_min, (int)spell->magnitudes[slot].base_max) + ((((int)(unsigned short)level) / ((int)spell->magnitudes[slot].per_level)) * rand_range((int)spell->magnitudes[slot].plus_min, (int)spell->magnitudes[slot].plus_max)))) / 100;
            *(short *)&spell->magnitudes[slot].plus_min = 0;
        }
    }
}

void spell_remove_effect_type(struct record *object, int effect_type)
{
    struct spell *spell;
    int slot;

    object = object->children;
    while (object != 0) {
        if (object->type == 9) {
            spell = &object->data.spell;
            slot = spell_find_effect_type(spell, effect_type);
            if (slot != 0) {
                spell->effects[slot].type = 255;
                if (spell_has_no_effects(spell) != 0) {
                    object_delete(object);
                    return;
                }
            }
        }
        object = object->next;
    }
}

void cast_fire_missile(struct record *missile)
{
    int unused;
    struct spell *spell;
    int unused2;
    {
        int aim[3];
        struct missile_step step;

        object_reparent(location_object, missile);
        step.light = object_create_child(missile, 0, 0);
        step.light->type = 7;
        step.light->image = 48;
        step.light->light_radius = 64;
        missile->x = player_object->x;
        step.light->x = missile->x;
        missile->y = player_object->y - 50;
        step.light->y = missile->y;
        missile->z = player_object->z;
        step.light->z = missile->z;
        step.light->angle_x = 0;
        step.light->yaw = 0;
        if (D_00153408 != 0) {
            missile->angle_x = ((camera_object->angle_x + ((((((int)(short)mouse_y) + 6) - ((int)(short)xn_cam_centre_y)) * 307) >> 8)) + D_0015340D) & 2047;
            missile->yaw = (((((mouse_x + 6) - xn_cam_centre_x) * 2) + camera_object->yaw) + D_00153411) & 2047;
        } else {
            missile->angle_x = ((((((((int)(short)mouse_y) + 6) - ((int)(short)xn_cam_centre_y)) * 150) / 100) + camera_object->angle_x) + view_look_pitch) & 2047;
            missile->yaw = ((camera_object->yaw + ((((((int)(short)mouse_x) + 6) - ((int)(short)xn_cam_centre_x)) * 160) / 100)) + view_look_yaw) & 2047;
        }
        step.light->angle_z = (missile->angle_z = 0);
        mc_memset(aim, 0, 12, D_001757F4, 711, 4);
        xn_math_advance_pitch_yaw(missile->angle_x, missile->yaw, 1024, aim);
        aim[0] += missile->x;
        aim[1] += missile->y;
        aim[2] += missile->z;
        mc_memset(&step, 0, 12, D_001757F4, 717, 4);
        xn_vec_unit_direction(&missile->x, aim, missile->data.spell.missile_direction);
        xn_vec_advance(missile->data.spell.missile_direction, 110, &step);
        missile->x += step.x;
        missile->y += step.y;
        missile->z += step.z;
        step.light->x = missile->x;
        step.light->y = missile->y;
        step.light->z = missile->z;
        spell = &missile->data.spell;
        missile->missile_texture = *(short *)(spell_missile_textures + (spell->element * 2));
        missile->flags |= 0x2000;
        missile->image2 = 0;
        step.x = (player_object->x + missile->x) / 2;
        step.y = (player_object->y + missile->y) / 2;
        step.z = (player_object->z + missile->z) / 2;
        if (collide_creature_within(player_object, (iptr)&step, 65) != 0) {
            sound_play((int)(short)*(short *)(spell_impact_sounds + (missile->data.spell.element * 2)), missile, 110);
            missile->missile_texture |= 1;
            missile->image2 = 32768;
            missile->children->light_radius <<= 2;
            func_0005C856(missile, D_00195C48);
            if (spell->target == 2 || spell->target == 4) cast_anim_start(spell->element);
            return;
        }
        spell_missile_update(missile, 1);
        if (spell->target == 2 || spell->target == 4) cast_anim_start(spell->element);
        missile->y += 40;
        if (collide_line_of_sight(player_object, missile) == 0) {
            missile->y -= 40;
            sound_play((int)(short)*(short *)(spell_impact_sounds + (spell->element * 2)), missile, 110);
            missile->missile_texture |= 1;
            missile->image2 = 32768;
            missile->children->light_radius <<= 2;
            if (spell->target == 2 || spell->target == 4) cast_anim_start(spell->element);
            return;
        }
        missile->y -= 40;
        spell_missile_update(missile, 1);
        if (spell->target == 2 || spell->target == 4) cast_anim_start(spell->element);
        sound_play((int)(short)*(short *)(spell_cast_sounds + (spell->element * 2)), missile, 110);
    }
}

void cast_creature_missile(struct record *missile, struct record *caster, struct record *target)
{
    struct record *light;
    struct spell *spell;
    int unused1;
    int unused2;
    int unused3;
    int unused4;
    int midpoint[3];

    object_reparent(location_object, missile);
    light = object_create_child(missile, 0, 0);
    light->type = 7;
    light->image = 48;
    light->light_radius = 64;
    missile->x = caster->x;
    light->x = missile->x;
    missile->y = caster->y - 50;
    light->y = missile->y;
    missile->z = caster->z;
    light->z = missile->z;
    light->angle_x = 0;
    missile->angle_x = xn_math_angle_to_point(caster->y, caster->z, target->y, target->z);
    light->yaw = 0;
    missile->yaw = xn_math_angle_to_point(caster->x, caster->z, target->x, target->z);
    light->angle_z = (missile->angle_z = 0);
    xn_math_advance_pitch_yaw(missile->angle_x, missile->yaw, 110, &missile->x);
    light->x = missile->x;
    light->y = missile->y;
    light->z = missile->z;
    target->y -= 50;
    xn_vec_unit_direction(&missile->x, &target->x, missile->data.spell.missile_direction);
    target->y += 50;
    spell = &missile->data.spell;
    missile->missile_texture = *(short *)(spell_missile_textures + (spell->element * 2));
    missile->flags |= 0x2000;
    missile->image2 = 0;
    midpoint[0] = (caster->x + missile->x) / 2;
    midpoint[1] = (caster->y + missile->y) / 2;
    midpoint[2] = (caster->z + missile->z) / 2;
    if (collide_creature_within(caster, (iptr)midpoint, 65) != 0) {
        sound_play((int)(short)*(short *)(spell_impact_sounds + (missile->data.spell.element * 2)), missile, 110);
        missile->missile_texture |= 1;
        missile->image2 = 32768;
        missile->children->light_radius <<= 2;
        func_0005C856(missile, D_00195C48);
        return;
    }
    spell_missile_update(missile, 1);
    sound_play((int)(short)*(short *)(spell_cast_sounds + (spell->element * 2)), missile, 110);
}

void spell_area_effect(struct record *spell)
{
    int i;
    int radius;
    int unused;

    if (spell->data.spell.target == 2) return;
    if (spell->data.spell.icon >= 250) {
        radius = 168;
    } else {
        radius = (spell->caster->data.character.level << 2) + 64;
    }
    radius = radius * 3;
    for (i = 0; i < creature_count; i++) {
        if (xn_math_approx_hypot(spell->y - creature_list[i]->y, xn_math_approx_dist2d(spell->x, spell->z, creature_list[i]->x, creature_list[i]->z)) < radius) {
            if (collide_line_of_sight(spell, creature_list[i]) == 0) continue;
            damage_knockback(creature_list[i], radius * 30, xn_math_angle_to_point(spell->x, spell->z, creature_list[i]->x, creature_list[i]->z), radius);
            func_0005C856(spell, creature_list[i]);
        }
    }
    if (xn_math_approx_hypot(spell->y - player_object->y, xn_math_approx_dist2d(spell->x, spell->z, player_object->x, player_object->z)) >= radius) return;
    spell_lookup_name(&spell->data.spell);
    spell_cast_queue(spell, player_entity);
}

void func_0005C856(struct record *spell, struct record *target)
{
    spell_lookup_name(&spell->data.spell);
    if (target->type != 18) return;
    spell_cast_queue(spell, target);
}

void cast_anim_start(int element)
{
    if (((int)(short)cast_anim_state) > (-1)) return;
    cast_anim_state = element << 4;
}

void cast_anim_update(void)
{
    int y;
    int last_y;

    if (cast_anim_state < 0) return;
    if (((int)(short)(cast_anim_state & 15)) == 6) {
        if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
            last_y = 0;
        } else {
            last_y = -hud_bar_image->height;
        }
        xn_draw_cast_anim_mirrored((void *)spell_cast_anim_fire[(((int)(short)cast_anim_state) >> 4)], 0, last_y);
        cast_anim_state = 65535;
        return;
    }
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        y = 0;
    } else {
        y = -hud_bar_image->height;
    }
    xn_draw_cast_anim_mirrored((void *)spell_cast_anim_fire[(((int)(short)cast_anim_state) >> 4)], (int)(short)(cast_anim_state & 15), y);
    cast_anim_state++;
}

int spell_player_has_spell(unsigned char spell_id)
{
    struct record *object;

    object = player_entity->children;
    while (object != 0) {
        if (object->type == 9 && object->data.spell.id == spell_id) return 1;
        object = object->next;
    }
    return 0;
}

void func_0005CA28(struct record *object)
{
    if (object->type != 9) return;
    if ((signed char)D_00199D64->id != (signed char)object->data.spell.id) return;
    mc_strncpy((char *)((iptr)D_00199D64 + 47), object->data.spell.name, 25, D_001757F4, 958);
}

void spell_lookup_name(struct spell *spell)
{
    int i;
    struct record *spellbook;

    i = 0;
    while ((signed char)spell_records[i].name[0] == 0 || ((signed char)spell_records[i].id != (signed char)spell->id && i < 128)) {
        i++;
    }
    if (i >= 128) {
        spellbook = object_find_item(player_entity->children, 27, 0);
        D_00199D64 = spell;
        object_foreach(spellbook->children, func_0005CA28);
        return;
    }
    mc_strncpy(spell->name, (char *)(signed char *)&spell_records[i].name[0], 25, D_001757F4, 974);
}

void spell_hud_draw_icons(void)
{
    int icon;
    int tmp_y;
    int tmp;
    struct record *object;
    struct record *next;
    struct spell *spell;
    int i;
    int expiring;
    int x;
    int y;
    int cast_by_other;
    int columns;
    unsigned char *bios_ticks;

    D_00199D6C = 0;
    if (player_entity->children == 0) return;
    object = player_entity->children;
    D_00199D71 = 1;
    while (object != 0) {
        if (object->type == 9) {
            if ((iptr)object->caster != (iptr)player_entity) {
                tmp = 1;
            } else {
                tmp = 0;
            }
            cast_by_other = tmp;
            if (cast_by_other != 0) D_00199D71 = 1;
            spell = &object->data.spell;
            expiring = 1;
            for (i = 0; i < 3; i++) {
                if (spell->effects[i].type != 255 && spell->cast_durations[i] > 1) {
                    expiring = 0;
                    break;
                }
            }
            if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0 && cast_by_other != 0) {
                columns = 11;
            } else {
                columns = 12;
            }
            x = (int)(unsigned short)D_00185CEC[(D_00199D6C % columns)];
            if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) x += 35;
            y = ((D_00199D6C / 12) * 24) + 16;
            bios_ticks = (unsigned char *)DOS_LOW(0x46C);
            if ((expiring != 0 && ((struct bf8_3_1 *)bios_ticks)->f != 0) || expiring == 0) {
                if (cast_by_other != 0) {
                    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
                        tmp_y = 177;
                    } else {
                        tmp_y = hud_bar_image->y - 22;
                    }
                    y = tmp_y;
                } else if (spell->effects[0].type == 35 && spell->effects[1].type == 255 && player_character->shield_points == 0) {
                    next = object->next;
                    spell_end(object);
                    object = next;
                    continue;
                }
                if (spell->icon >= 200) {
                    icon = 10;
                } else {
                    icon = spell->icon;
                }
                xn_draw_spell_icon(x, y, icon);
            }
            D_00199D6C++;
        }
        object = object->next;
    }
    if (D_00199D6C != 0) return;
    for (i = 0; i < 8; i++) {
        if (player_character->attributes[i] > player_character->base_attributes[i]) {
            player_character->attributes[i] = player_character->base_attributes[i];
        }
    }
}
