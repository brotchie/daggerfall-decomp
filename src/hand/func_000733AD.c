/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000733AD */
struct mobile { char pad0[553]; unsigned char group; };   /* group at 0x229 */
struct thing {
    unsigned char type;
    char pad1[2];
    short angle;                /* 3 */
    char pad5[2];
    int x;                      /* 7 */
    int y;                      /* 11 */
    int z;                      /* 15 */
    char pad13[2];
    unsigned short flags;       /* 21 */
    unsigned short f23;         /* 23 */
    char pad19[6];
    unsigned int f31;           /* 31 */
    char pad23[36];
    struct mobile mob;          /* 71 */
};
struct item { char pad0[15]; unsigned char flags; };
struct pick { int flags; struct thing *obj; int f8; int fc; int f10; };
struct player { char pad0[253]; short f253; char pad0ff[188]; struct thing *f443[1]; };
struct w2 { unsigned short f0; unsigned short f2; };
extern unsigned char player_environment;
extern struct thing *D_00190504[];
extern int D_001959BC;
extern struct thing *player_entity;
extern struct thing *player_object;
extern int D_00195ABC;
extern int creature_count;
extern struct player *player_character;
extern struct w2 *D_00195DC0;
extern unsigned char weapon_active_hand;
extern char crime_current;
extern char D_001962B2;
extern struct thing *people_list[];
extern int people_count;
extern void engine_pick_object(int, int, struct pick *);
extern int collide_line_of_sight(struct thing *, struct thing *);
extern void town_map_note_building(struct thing *, struct item *);
extern void damage_resolve_attack(struct thing *, struct thing *, int);
extern void damage_spawn_splash(struct thing *, int, int);
extern int damage_miss_sound(struct mobile *, int);
extern void func_0002FBCC(void);
extern void skill_add_uses(int, int);
extern int creatures_guard_mix(void);
extern void guards_summon(int);
extern void person_killed(struct thing *);
extern int ai_angle_diff(int, int, int *);
extern void func_00063DDC(struct thing *);
extern void links_trigger(struct thing *, int);
extern int sound_play(int, struct thing *, int);
extern void func_0007425E(struct thing *, int);
extern int rand_range(int, int);
extern struct item *object_building(struct thing *);
extern void building_enter(struct item *);
extern int door_start_swing(struct thing *, int);
extern int func_000C7FD9(int, int, int, int);
extern int func_000C7FF4(int, int);
extern int func_000C808D(int, int, int, int);

void weapon_melee_strike(struct thing *a1)
{
    struct pick st;
    int grp;
    int dist;
    int res;
    int i;
    int r;
    struct mobile *m;
    struct mobile *om;
    struct thing *other;
    struct thing *obj;
    int count;
    struct item *item;

    m = &a1->mob;
    grp = m->group;
    D_00190504[creature_count++] = player_entity;
    for (count = i = 0; i < creature_count; i++) {
        other = D_00190504[i];
        om = &other->mob;
        if (om->group == grp)
            continue;
        if (other == a1)
            continue;
        dist = func_000C7FF4(a1->y - other->y, func_000C7FD9(a1->x, a1->z, other->x, other->z));
        if (dist > 90)
            continue;
        if (dist > 10) {
            dist = func_000C808D(a1->x, a1->z, other->x, other->z);
            D_00195ABC = dist;
            if (a1 == player_entity)
                res = ai_angle_diff(a1->angle + D_001959BC & 2047, dist, &dist);
            else
                res = ai_angle_diff(a1->angle, dist, &dist);
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
        sound_play(damage_miss_sound(player_character->f443[weapon_active_hand] != 0 ? &player_character->f443[weapon_active_hand]->mob : 0, -1), player_object, 110);
    if (a1 == player_entity && player_environment != 3) {
        for (i = 0; i < people_count; i++) {
            if (people_list[i] == 0)
                continue;
            other = people_list[i];
            dist = func_000C7FF4(a1->y - other->y, func_000C7FD9(a1->x, a1->z, other->x, other->z));
            if (dist > 90)
                continue;
            dist = func_000C808D(a1->x, a1->z, other->x, other->z);
            D_00195ABC = dist;
            res = ai_angle_diff(a1->angle + D_001959BC & 2047, dist, &dist);
            if (res < 200) {
                damage_spawn_splash(people_list[i], 0, -1);
                person_killed(people_list[i]);
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
        r = (D_00195DC0->f2 >> 7) % 100;
        if (r != 74)
            return;
    }
    if (obj->type != 43 && func_000C7FD9(obj->x, obj->z, player_object->x, player_object->z) > 100)
        return;
    if (obj->type != 43 && (obj->f23 == 0 || (obj->flags & 320) != 0)) {
        func_0007425E(obj, 0);
        return;
    }
    if (obj->type != 43) {
        skill_add_uses(16, 1);
        func_00063DDC(0);
        if (obj->f23 <= 19 && rand_range(1, 100) <= 20 - obj->f23 && door_start_swing(obj, 0))
            obj->flags |= 320;
        if (obj->f31 >> 16 == 50027 || obj->f31 >> 16 == 50029 || obj->f31 >> 16 == 50033)
            func_0002FBCC();
    } else if (rand_range(1, 100) < 10) {
        item = object_building(obj);
        if (item != 0)
            item->flags |= 16;
        town_map_note_building(obj, item);
        building_enter(item);
    } else if (rand_range(1, 100) > player_character->f253 && (creatures_guard_mix() & 1) == 0) {
        crime_current = 0;
        guards_summon(1);
    }
    sound_play(9, obj, 100);
}
