/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000733AD */
#include "records.h"

struct pick { int flags; struct record *obj; int f8; int fc; int f10; };
struct w2 { unsigned short f0; unsigned short f2; };
extern unsigned char player_environment;
extern struct record *creature_list[];
extern int view_look_yaw;
extern struct record *player_entity;
extern struct record *player_object;
extern int D_00195ABC;
extern int creature_count;
extern struct character *player_character;
extern struct w2 *click_face_texture;
extern unsigned char weapon_active_hand;
extern char crime_current;
extern char D_001962B2;
extern struct record *people_list[];
extern int people_count;
extern void engine_pick_object(int, int, struct pick *);
extern int collide_line_of_sight(struct record *, struct record *);
extern void town_map_note_building(struct record *, struct building *);
extern void damage_resolve_attack(struct record *, struct record *, int);
extern void damage_spawn_splash(struct record *, int, int);
extern int damage_miss_sound(struct item *, int);
extern void monster_wake_all(void);
extern void skill_add_uses(int, int);
extern int creatures_guard_mix(void);
extern void guards_summon(int);
extern void pedestrian_killed(struct record *);
extern int ai_angle_diff(int, int, int *);
extern void func_00063DDC(struct record *);
extern void links_trigger(struct record *, int);
extern int sound_play(int, struct record *, int);
extern void door_try_open(struct record *, int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern void building_enter(struct building *);
extern int door_start_swing(struct record *, int);
extern int xn_math_approx_dist2d(int, int, int, int);
extern int xn_math_approx_hypot(int, int);
extern int xn_math_angle_to_point(int, int, int, int);

void weapon_melee_strike(struct record *a1)
{
    struct pick st;
    int grp;
    int dist;
    int res;
    int i;
    int r;
    struct character *m;
    struct character *om;
    struct record *other;
    struct record *obj;
    int count;
    struct building *item;

    m = &a1->data.character;
    grp = m->team;
    creature_list[creature_count++] = player_entity;
    for (count = i = 0; i < creature_count; i++) {
        other = creature_list[i];
        om = &other->data.character;
        if (om->team == grp)
            continue;
        if (other == a1)
            continue;
        dist = xn_math_approx_hypot(a1->y - other->y, xn_math_approx_dist2d(a1->x, a1->z, other->x, other->z));
        if (dist > 90)
            continue;
        if (dist > 10) {
            dist = xn_math_angle_to_point(a1->x, a1->z, other->x, other->z);
            D_00195ABC = dist;
            if (a1 == player_entity)
                res = ai_angle_diff(a1->yaw + view_look_yaw & 2047, dist, &dist);
            else
                res = ai_angle_diff(a1->yaw, dist, &dist);
        } else
            res = 1;
        if (res < 200 && collide_line_of_sight(a1, other)) {
            if (a1 == player_entity) {
                func_00063DDC(other);
                damage_resolve_attack(a1, other, weapon_active_hand * 2 + 19);
                count++;
            } else
                damage_resolve_attack(a1, other, 19);
        }
        if (D_001962B2 != 0) {
            D_001962B2 = 0;
            return;
        }
    }
    if (count == 0 && a1 == player_entity)
        sound_play(damage_miss_sound(player_character->equipped[19 + weapon_active_hand] != 0 ? &player_character->equipped[19 + weapon_active_hand]->data.item : 0, -1), player_object, 110);
    if (a1 == player_entity && player_environment != 3) {
        for (i = 0; i < people_count; i++) {
            if (people_list[i] == 0)
                continue;
            other = people_list[i];
            dist = xn_math_approx_hypot(a1->y - other->y, xn_math_approx_dist2d(a1->x, a1->z, other->x, other->z));
            if (dist > 90)
                continue;
            dist = xn_math_angle_to_point(a1->x, a1->z, other->x, other->z);
            D_00195ABC = dist;
            res = ai_angle_diff(a1->yaw + view_look_yaw & 2047, dist, &dist);
            if (res < 200) {
                damage_spawn_splash(people_list[i], 0, -1);
                pedestrian_killed(people_list[i]);
                count++;
            }
        }
    }
    if (a1 != player_entity || count != 0)
        return;
    engine_pick_object(160, 100, &st);
    if ((st.flags & 1) == 0)
        return;
    obj = st.obj;
    links_trigger(obj, 5);
    if (obj->type != 32 && obj->type != 43)
        return;
    if (obj->type == 43) {
        r = (click_face_texture->f2 >> 7) % 100;
        if (r != 74)
            return;
    }
    if (obj->type != 43 && xn_math_approx_dist2d(obj->x, obj->z, player_object->x, player_object->z) > 100)
        return;
    if (obj->type != 43 && (obj->lock_level == 0 || (obj->flags & 320) != 0)) {
        door_try_open(obj, 0);
        return;
    }
    if (obj->type != 43) {
        skill_add_uses(16, 1);
        func_00063DDC(0);
        if (obj->lock_level <= 19 && rand_range(1, 100) <= 20 - obj->lock_level && door_start_swing(obj, 0))
            obj->flags |= 320;
        if (obj->id >> 16 == 50027 || obj->id >> 16 == 50029 || obj->id >> 16 == 50033)
            monster_wake_all();
    } else if (rand_range(1, 100) < 10) {
        item = object_building(obj);
        if (item != 0)
            item->flags |= 16;
        town_map_note_building(obj, item);
        building_enter(item);
    } else if (rand_range(1, 100) > player_character->skills[SKILL_STEALTH].value && (creatures_guard_mix() & 1) == 0) {
        crime_current = 0;
        guards_summon(1);
    }
    sound_play(9, obj, 100);
}
