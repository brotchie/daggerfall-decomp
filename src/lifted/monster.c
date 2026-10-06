/* monster.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern char D_00175934[];
extern signed char anim_mirror_facing[];
extern int D_00186A14[];
extern struct collide_probe D_00187B44;
extern signed char undead_daedra_ids[];
extern struct record *creature_list[];
extern char D_00190704[];
extern char scratch_190de4[];
extern signed char text_rsc_buffer[];
extern unsigned char D_001940D7;
extern signed char D_001940DA;
extern struct record *nonworld_root;
extern struct record *player_object;
extern int frame_ticks;
extern int vertical_velocity;
extern struct record *location_object;
extern struct record *ai_chosen_spell;
extern int creature_count;
extern struct character *player_character;
extern struct record *D_00195C70;
extern int ceiling_height;
extern struct record *D_00195CB8;
extern int ai_monster_flags;
extern char D_00196167[];
extern signed char player_on_ground;
extern char D_00196D54[];
extern int D_00196D58;
extern int D_00196D5C;
extern char collide_flags[];
extern int D_00199D74;
extern char D_00199D9B[];
extern int link_count;

extern int collide_move_object(struct record *, int, struct move_request *, int);
extern int damage_apply(struct record *, int, struct record *);
extern int spell_cost(struct spell *, struct character *);
extern int cast_creature_spell_at(struct record *, struct record *, struct record *);
extern int spell_player_has_spell(unsigned char);
extern int ai_turn_toward(struct record *, int);
extern int ai_stealth_check(int, int, int, int);
extern int monster_move_step(struct record *, struct record *, int);
extern int func_00063FCF(struct record *, int, int);
extern int sound_play(int, iptr, int);
extern int spawn_find_point(struct record *, int, int);
extern int rand_range(int, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_random_child_of_type(struct record *, int);
extern int object_new_id(int);
extern int abs();
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern iptr memchr();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();
extern int xn_math_angle_to_point();
extern int xn_math_yaw_offset_xz();
extern void weapon_monster_arrow(int, int);
extern void monster_init(struct record *, int);
extern void object_apply_gravity(struct record *, struct character *);
extern void object_foreach(struct record *, void (*)());
int monster_set_action_seducer(struct record *, int, int);
struct record *monster_spell_list(struct record *);
int func_0006379A(struct record *, struct record *);
int func_00063ED8(struct record *, int);
void func_0006243B(struct record *, struct record *, int);
void monster_mark_anim_slot_cb(struct record *);

void func_000622EB(struct record *monster, struct record *target, int heading, int turn_dir)
{
    struct character *monster_char;
    int detour_offset;

    monster_char = &monster->data.character;
    mc_memcpy((iptr)D_00196167, monster, 71, (iptr)D_00175934, 426, 4);
    if (monster_char->detour_steps == 1000) {
        func_0006243B(monster, target, heading);
        if (((int)(short)(*(short *)collide_flags & 10)) == 0) {
            monster_char->detour_steps = 1000;
            return;
        }
        monster_char->detour_steps = rand_range(4, 12);
    }
    if (monster_char->detour_steps != 0) {
        monster_char->detour_steps--;
    } else {
        if (turn_dir < 0) {
            detour_offset = -512;
        } else {
            detour_offset = 512;
        }
        if (monster_char->detour_side != 0) {
            monster_char->detour_yaw = detour_offset + monster->yaw;
        } else {
            monster_char->detour_yaw = monster->yaw - detour_offset;
        }
        monster_char->detour_steps = 1000;
        monster_char->detour_side ^= 1;
    }
    mc_memcpy(monster, (iptr)D_00196167, 71, (iptr)D_00175934, 457, 4);
    func_0006243B(monster, target, monster_char->detour_yaw & 2047);
}

void func_0006243B(struct record *monster, struct record *target, int heading)
{
    int saved_velocity;
    int dx;
    int dz;
    int fall_start;
    int saved_ceiling;
    int speed;
    int dy;
    unsigned char saved_on_ground;
    {
        struct move_request move;
        struct {
            struct vec3 position;       /* +0x00: the saved D_00196D54 */
            char pad0C[24];             /* +0x0C */
            struct character *character; /* +0x24: the monster's */
        } saved_pos;

        saved_pos.character = &monster->data.character;
        speed = ((saved_pos.character->attributes[ATTR_SPD] + 100) * frame_ticks) / 1000;
        if (saved_pos.character->fall_velocity != 0) {
            dz = 0;
            dx = dz;
        } else {
            xn_math_yaw_offset_xz(heading, speed, (iptr)&dx, (iptr)&dz);
        }
        move.x = monster->x + dx;
        move.y = monster->y;
        move.z = monster->z + dz;
        if (((struct bf8_0_1 *)&ai_monster_flags)->f != 0) {
            dy = move.y - (target->y - 70);
            if (abs(dy) > 10) {
                if (dy < 0) {
                    move.y += speed;
                } else {
                    move.y -= speed;
                }
            }
        }
        move.angle_x = monster->angle_x;
        move.yaw = monster->yaw;
        move.angle_z = monster->angle_z;
        saved_on_ground = player_on_ground;
        saved_velocity = vertical_velocity;
        saved_ceiling = ceiling_height;
        *(signed char *)collide_flags |= 4;
        fall_start = (vertical_velocity = saved_pos.character->fall_velocity);
        D_001940D7 |= 128;
        move.probe = &D_00187B44;
        if ((saved_pos.character->flags & 2080) != 0) {
            move.flags |= 1;
        } else {
            move.flags &= 65534;
        }
        mc_memcpy((iptr)&saved_pos.position, (iptr)D_00196D54, 12, (iptr)D_00175934, 512, 4);
        *(int *)D_00196D54 = monster->x;
        D_00196D58 = monster->y - (vertical_velocity / 256);
        D_00196D5C = monster->z;
        if ((move.y - 90) < saved_pos.character->ceiling_y) {
            move.y = saved_pos.character->ceiling_y + 90;
        }
        collide_move_object(monster, 0, &move, 0);
        player_on_ground = saved_on_ground;
        mc_memcpy((iptr)D_00196D54, (iptr)&saved_pos.position, 12, (iptr)D_00175934, 521, 4);
        if (saved_pos.character != player_character) {
            saved_pos.character->ceiling_y = ceiling_height;
        }
        if (((int)(short)(*(short *)collide_flags & 16)) != 0 && ((struct bf8_0_1 *)&ai_monster_flags)->f == 0) {
            object_apply_gravity(monster, saved_pos.character);
        } else {
            vertical_velocity = 0;
            saved_pos.character->flags &= ~0x800;
        }
        if (vertical_velocity == 0 && fall_start != 0) {
            damage_apply(monster, (int)(iptr)&*(signed char *)((char *)(iptr)((fall_start / 256) / 80) - 3), 0);
        }
        saved_pos.character->fall_velocity = vertical_velocity;
        vertical_velocity = saved_velocity;
        ceiling_height = saved_ceiling;
        if (abs(monster->y - player_object->y) <= 3000) return;
        D_001940DA |= 128;
        object_delete(monster);
    }
}

int monster_set_action(struct record *monster, int angle, int action)
{
    struct monster_anim *anim;
    int facing;
    int player_angle;
    int view_yaw;
    int unused;

    if (monster->data.character.race == 29) return monster_set_action_seducer(monster, angle, action);
    if (monster->data.character.race >= 60 && action != 0 && action != 60 && action != 8) return 0;
    anim = &monster->data.monster.anim;
    player_angle = xn_math_angle_to_point(monster->x, monster->z, player_object->x, player_object->z);
    view_yaw = (monster->yaw + 128) & 2047;
    facing = (view_yaw - player_angle) & 2047;
    facing >>= 8;
    if (facing > 4) {
        facing = (int)(unsigned char)anim_mirror_facing[facing];
        anim->anim_flags |= 128;
    } else {
        anim->anim_flags &= 127;
    }
    anim->anim_facing = *(signed char *)&facing;
    if (anim->anim_request != 255) return 0;
    if (D_00199D74 != 0) return 0;
    D_00199D74 = 1;
    monster->data.character.action = *(signed char *)&action;
    anim->anim_request = *(signed char *)&action;
    return 1;
}

int monster_set_action_seducer(struct record *monster, int angle, int action)
{
    struct monster_anim *anim;
    struct character *monster_char;
    int facing;
    int player_angle;
    int view_yaw;

    anim = &monster->data.monster.anim;
    if (anim->anim_request == 57) return 0;
    monster_char = &monster->data.character;
    monster_char->action = *(signed char *)&action;
    player_angle = xn_math_angle_to_point(monster->x, monster->z, player_object->x, player_object->z);
    view_yaw = (monster->yaw + 128) & 2047;
    facing = (view_yaw - player_angle) & 2047;
    if (((int)(unsigned short)(monster_char->flags & 16384)) != 0) {
        if (action == 8 || action == 32) {
            action = 56;
        } else if (action == 0) {
            action = 59;
        }
        facing = 0;
    }
    facing >>= 8;
    if (facing > 4) {
        facing = (int)(unsigned char)anim_mirror_facing[facing];
        anim->anim_flags |= 128;
    } else {
        anim->anim_flags &= 127;
    }
    if (anim->anim_request == 8 || anim->anim_request == 32) return 0;
    anim->anim_facing = *(signed char *)&facing;
    anim->anim_request = *(signed char *)&action;
    return 1;
}

struct record *monster_spell_list(struct record *object)
{
    object = object->children;
    while (object != 0) {
        if (object->type == 22) return object->children;
        object = object->next;
    }
    return 0;
}

int ai_pick_ranged_spell(int creature_index)
{
    struct record *spell;
    struct record *first_spell;
    struct spell *spell_data;
    int count;
    struct character *monster_char;

    count = 0;
    monster_char = &creature_list[creature_index]->data.character;
    if ((monster_char->conditions & 0x100) != 0) return 0;
    spell = monster_spell_list(creature_list[creature_index]);
    first_spell = spell;
    while (spell != 0) {
        spell_data = &spell->data.spell;
        if (spell_data->target == 2 || spell_data->target == 4) {
            *(iptr *)(scratch_190de4 + (count++ << 2)) = (iptr)spell;
        }
        spell = spell->next;
    }
    if (count == 0) return 0;
    spell = first_spell;
    count = rand_range(0, count - 1);
    if (spell_player_has_spell((ai_chosen_spell = (struct record *)*(iptr *)(scratch_190de4 + (count << 2)))->data.spell.id) != 0) {
        return 0;
    }
    return 1;
}

int ai_pick_touch_spell(int creature_index)
{
    struct record *spell;
    struct record *first_spell;
    struct spell *spell_data;
    int count;
    struct character *monster_char;

    count = 0;
    monster_char = &creature_list[creature_index]->data.character;
    if ((monster_char->conditions & 0x100) != 0) return 0;
    spell = monster_spell_list(creature_list[creature_index]);
    first_spell = spell;
    while (spell != 0) {
        spell_data = &spell->data.spell;
        if (spell_data->target == 0 || spell_data->target == 1) {
            *(iptr *)(scratch_190de4 + (count++ << 2)) = (iptr)spell;
        }
        spell = spell->next;
    }
    if (count == 0) return 0;
    spell = first_spell;
    count = rand_range(0, count - 1);
    if (spell_player_has_spell((ai_chosen_spell = (struct record *)*(iptr *)(scratch_190de4 + (count << 2)))->data.spell.id) != 0) {
        return 0;
    }
    return 1;
}

int func_00062C15(int creature_index)
{
    struct character *monster_char;

    monster_char = &creature_list[creature_index]->data.character;
    if ((monster_char->conditions & 0x100) != 0) return 0;
    if (spell_player_has_spell((ai_chosen_spell = object_random_child_of_type(creature_list[creature_index], 9))->data.spell.id) != 0) {
        return 0;
    }
    return ((ai_chosen_spell != 0) ? 1 : 0);
}

int func_00062CB6(int creature_index)
{
    struct character *monster_char;

    monster_char = &creature_list[creature_index]->data.character;
    if ((monster_char->conditions & 0x100) != 0) return 0;
    if (spell_player_has_spell((ai_chosen_spell = object_random_child_of_type(creature_list[creature_index], 9))->data.spell.id) != 0) {
        return 0;
    }
    return ((ai_chosen_spell != 0) ? 1 : 0);
}

int monster_cast_spell(struct record *caster, struct record *target)
{
    struct record *spell;
    struct character *caster_char;

    caster_char = &caster->data.character;
    if ((caster_char->conditions & 0x100) != 0) return 0;
    if ((ai_chosen_spell = object_random_child_of_type(caster, 9)) == 0) return 0;
    if (spell_player_has_spell(ai_chosen_spell->data.spell.id) != 0) return 0;
    spell = object_create_child(caster->parent, 0, 89);
    spell->type = 9;
    spell->id = object_new_id(100);
    mc_memcpy(&spell->data.spell, &ai_chosen_spell->data.spell, 89, (iptr)D_00175934, 758, 4);
    cast_creature_spell_at(spell, caster, target);
    caster_char->magicka -= spell_cost(&spell->data.spell, caster_char);
    if (caster_char->magicka < 0) {
        caster_char->magicka = 0;
    } else if ((target->data.character.conditions & 0x400) != 0 && rand_range(1, 100) < (caster_char->attributes[1] / 2)) {
        caster_char->magicka = 0;
    }
    return 1;
}

void monster_shoot_arrow(int shooter, int target)
{
    sound_play(6, shooter, 100);
    weapon_monster_arrow(shooter, target);
}

int ai_angle_diff(int angle, int target_angle, int *dir)
{
    int diff;

    angle &= 2047;
    target_angle &= 2047;
    diff = target_angle - angle;
    if (diff == 0) return diff;
    if (diff > 1024) {
        *dir = -1;
        return angle + (2048 - target_angle);
    }
    if (diff > 0 && diff <= 1024) {
        *dir = 1;
        return target_angle - angle;
    }
    if (diff < (-1024)) {
        *dir = 1;
        return target_angle + (2048 - angle);
    }
    *dir = -1;
    return angle - target_angle;
}

void monster_mark_anim_slot_cb(struct record *object)
{
    struct character *monster_char;

    if (object->type != 18) return;
    monster_char = &object->data.character;
    text_rsc_buffer[monster_char->anim_slot] = 1;
}

int monster_alloc_anim_slot(void)
{
    int slot;

    mc_memset((iptr)text_rsc_buffer, 0, 128, (iptr)D_00175934, 829, 2048);
    object_foreach(location_object->children, monster_mark_anim_slot_cb);
    object_foreach(nonworld_root->children, monster_mark_anim_slot_cb);
    for (slot = 0; slot < 128; slot++) {
        if (text_rsc_buffer[slot] == 0 && *(int *)(D_00190704 + (slot << 2)) != 0) {
            if (*(int *)(D_00190704 + (slot << 2)) != 0 && *(int *)(D_00190704 + (slot << 2)) != (-1751672937)) {
                mc_free(*(int *)(D_00190704 + (slot << 2)), (iptr)D_00175934, 836);
                *(int *)(D_00190704 + (slot << 2)) = -1751672937;
            }
        }
    }
    slot = 0;
    while (text_rsc_buffer[slot] != 0) slot++;
    return slot;
}

iptr monster_sees_invisible(int monster_type)
{
    return memchr((iptr)undead_daedra_ids, monster_type, 14);
}

struct record *monster_summon_near_player(int monster_type)
{
    struct record *monster;

    monster = object_create_child(player_object->parent, 0, 659);
    if (spawn_find_point(monster, 96, 300) != 0) {
        monster->type = 18;
        monster_init(monster, monster_type);
        monster->data.character.team = 0;
        return monster;
    }
    object_free_single(monster);
    return 0;
}

void ai_move_toward_target(struct record *monster, struct character *monster_char, struct record *target, int target_angle, int angle_diff)
{
    int octant;
    int nav_angle;

    if (((int)(unsigned short)(monster_char->flags & 384)) != 0 && ((int)(unsigned char)(monster_char->nav_blocked & 1)) == 0) {
        monster_char->nav_turn_count = 12;
        if (angle_diff >= 32) {
            ai_turn_toward(monster, target_angle);
        } else if (monster_move_step(monster, target, monster->yaw) == 0) {
            if (monster_char->fall_velocity == 0) {
                monster_char->nav_stuck_count++;
                if (monster_char->nav_stuck_count > 6) goto L635B1;
            }
            goto L635C5;
L635B1:;
            monster_char->nav_blocked |= 1;
            monster_char->nav_stuck_count = 0;
L635C5:;
        } else {
            monster_char->nav_blocked = 0;
        }
        return;
    }
    if (((int)(unsigned short)(monster_char->flags & 256)) != 0 && monster_char->nav_turn_count == 0) {
        monster_char->nav_blocked &= 254;
    }
    octant = func_0006379A(monster, target);
    switch (monster_char->nav_direction) {
    case 2:
        nav_angle = D_00186A14[octant];
        break;
    case 4:
        nav_angle = (D_00186A14[octant] + 1024) & 2047;
        break;
    case 8:
        nav_angle = (D_00186A14[octant] + 1536) & 2047;
    }
    if (((int)(unsigned char)(monster_char->nav_blocked & 1)) == 0) return;
    if (angle_diff <= 256 && func_00063ED8(monster, (target_angle + 256) / 512) != 0) return;
    if (ai_turn_toward(monster, nav_angle) != 0) return;
    if (monster_char->nav_turn_count != 0) monster_char->nav_turn_count--;
    if ((monster_char->nav_blocked & monster_char->nav_direction) != 0) {
        monster_char->nav_direction <<= 1;
        if (((int)(unsigned char)(monster_char->nav_direction & 16)) != 0) {
            monster_char->nav_direction = 2;
            monster_char->nav_blocked &= 254;
        }
        if (((int)(unsigned short)(monster_char->flags & 128)) != 0) {
            monster_char->nav_turn_count = 0;
            monster_char->nav_blocked &= 254;
        }
        return;
    }
    if (monster_move_step(monster, target, monster->yaw) != 0) return;
    monster_char->nav_blocked |= monster_char->nav_direction;
}

int func_0006379A(struct record *from, struct record *to)
{
    int west;
    int south;
    int octant;

    west = 0;
    south = 0;
    if (to->x < from->x) west = 1;
    if (to->z < from->z) south = 2;
    if (abs(to->x - from->x) > abs(to->z - from->z)) {
        octant = 1;
    } else {
        octant = 0;
    }
    octant += south;
    if (west != 0) octant = 7 - octant;
    return octant;
}

void monster_apply_gravity(void)
{
    struct move_request move;
    struct vec3 saved_pos;
    int i;
    int fall_start;
    struct character *monster_char;
    int saved_on_ground;
    int saved_ceiling;
    int saved_velocity;
    int damage;

    saved_velocity = vertical_velocity;
    saved_on_ground = (int)(unsigned char)player_on_ground;
    saved_ceiling = ceiling_height;
    mc_memcpy((iptr)&saved_pos, (iptr)D_00196D54, 12, (iptr)D_00175934, 1147, 4);
    for (i = 0; i < creature_count; i++) {
        monster_char = &creature_list[i]->data.character;
        if (((int)(unsigned short)(monster_char->flags & 2080)) == 0) {
            if (monster_char->fall_velocity == 0) continue;
        }
        fall_start = monster_char->fall_velocity;
        vertical_velocity = fall_start;
        mc_memcpy((iptr)D_00196D54, (iptr)creature_list[i] + 7, 12, (iptr)D_00175934, 1159, 4);
        move.x = creature_list[i]->x;
        move.y = (int)(iptr)(*(char **)((char *)creature_list[i] + 11) + (vertical_velocity / 256));
        move.z = creature_list[i]->z;
        move.angle_x = creature_list[i]->angle_x;
        move.yaw = creature_list[i]->yaw;
        move.angle_z = creature_list[i]->angle_z;
        if (((int)(unsigned short)(monster_char->flags & 2080)) != 0) {
            move.flags |= 1;
        } else {
            move.flags &= 65534;
        }
        move.probe = &D_00187B44;
        *(signed char *)collide_flags &= 251;
        collide_move_object(creature_list[i], 0, &move, 0);
        if (((int)(short)(*(short *)collide_flags & 16)) != 0) {
            object_apply_gravity(creature_list[i], monster_char);
        } else {
            vertical_velocity = 0;
            monster_char->flags &= ~0x800;
        }
        if (vertical_velocity == 0 && fall_start != 0) {
            damage = ((fall_start / 256) / 40) - 5;
            if (damage > 0) {
                damage = damage * damage;
                damage = damage / 3;
                damage_apply(creature_list[i], damage, 0);
            }
        }
        monster_char->fall_velocity = vertical_velocity;
    }
    ceiling_height = saved_ceiling;
    player_on_ground = *(signed char *)&saved_on_ground;
    vertical_velocity = saved_velocity;
    mc_memcpy((iptr)D_00196D54, (iptr)&saved_pos, 12, (iptr)D_00175934, 1201, 4);
}

void func_00063DDC(struct record *source)
{
    int i;
    int dist;
    struct character *monster_char;
    struct record *monster;

    for (i = 0; i < creature_count; i++) {
        monster = creature_list[i];
        dist = xn_math_approx_hypot(monster->y - player_object->y, xn_math_approx_dist2d(monster->x, monster->z, player_object->x, player_object->z));
        monster_char = &creature_list[i]->data.character;
        if (source == creature_list[i] || ai_stealth_check(monster_char->race, (int)(unsigned short)(monster_char->flags & 256), dist, (int)(unsigned short)(monster_char->flags & 8)) != 0) {
            monster_char->flags |= 264;
            monster_char->give_up_timer = 200;
        }
    }
}

int func_00063ED8(struct record *monster, int quadrant)
{
    int *across;
    int *along;
    int pos;
    struct character *monster_char;

    monster_char = &monster->data.character;
    if (quadrant == 0 || quadrant == 2) {
        across = &monster->x;
        along = &monster->z;
    } else {
        across = &monster->z;
        along = &monster->x;
    }
    pos = *across & 63;
    if (pos == 0) return 0;
    if (pos < 8) {
        pos = 0;
        monster_char->nav_blocked &= 254;
    } else if (pos < 32) {
        pos += -8;
    } else if (pos > 56) {
        pos = 64;
        monster_char->nav_blocked &= 254;
    } else {
        pos += 8;
    }
    pos += *across & -64;
    if (quadrant == 0 || quadrant == 2) return func_00063FCF(monster, pos, *along);
    return func_00063FCF(monster, *along, pos);
}

int func_000641CD(int id)
{
    int i;

    for (i = 0; i < link_count; i++) {
        if (*(int *)(D_00199D9B + (i * 39)) == id) return 1;
    }
    return 0;
}

void func_00064301(void)
{
    link_count = 0;
    *(int *)&D_00195CB8 = (*(int *)&D_00195C70 = 0);
}
