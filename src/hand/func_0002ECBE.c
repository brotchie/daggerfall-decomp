/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002ECBE */
#include "records.h"

struct vec3 { int x, y, z; };
struct move { struct vec3 pos; int f12, f16, f20; char *name; };
extern unsigned char D_0012B508;
extern char D_001709A1[];
extern char D_001709C6[];
extern char D_001709E4[];
extern char D_001709ED[];
struct pic { short file, rec; };
extern struct pic monster_corpse_textures[];
extern char D_00187B44[];
extern signed char text_buffer[];
extern unsigned char D_001940D7;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *found_object;
extern struct record *scratch_object;
extern struct character *player_character;
extern int game_minutes;
extern unsigned short *game_settings;
extern int ceiling_height;
extern int player_death_timer;
extern short D_00195DA0;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move *, int);
extern void damage_find_empty_soul_trap_cb(struct record *);
extern void damage_drop_at_death_cb(struct record *);
extern void quest_raise_event(int, struct record *, int);
extern int item_artifact_equipped(int);
extern void sound_play(int, struct record *, int);
extern void guild_count_crime(int, unsigned char);
extern int hud_message_add(char *);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern void object_foreach(struct record *, void (*)(struct record *));
extern int mc_memset();
extern int mc_memcpy();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, ...);

void damage_creature_death(struct record *a1)
{
    int l_30;
    struct character *l_2C;
    struct record *l_28;
    struct move l_4C;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = &a1->data.character;
    l_20 = player_on_ground;
    l_1C = ceiling_height;
    if (a1 == player_entity) {
        sound_play((player_character->flags & 1) + (player_character->race * 3 + 2) ? 258 : 243, player_object, 100);
        player_death_timer = 1000;
        return;
    }
    if (l_2C->mobile_id == 146)
        guild_count_crime(6, 1);
    found_object = a1;
    object_foreach(a1->children, damage_drop_at_death_cb);
    l_28 = a1->children;
    l_18 = item_artifact_equipped(9);
    if (l_2C->race < 43) {
        if (l_18 != 0) goto found;
        while (l_28 != 0) {
            if (l_28->type == 19) {
found:
                scratch_object = 0;
                object_foreach(player_entity->children, damage_find_empty_soul_trap_cb);
                if (scratch_object == 0 && l_28->type == 19) {
                    D_0012B508 = 146;
                    hud_message_add(D_001709A1);
                    hud_message_add(D_001709C6);
                    return;
                }
                if (scratch_object != 0) {
                    if (l_18 == 0 && rand_range(0, 100) > l_28->soul_creature)
                        break;
                    l_28 = object_create_child(scratch_object, 0, 0);
                    l_28->flags = 3;
                    l_28->type = 20;
                    l_28->soul_creature = l_2C->race;
                }
                break;
            }
            l_28 = l_28->next;
        }
    }
    if (a1 != player_entity)
        sound_play(17, a1, 105);
    player_character->last_kill_time = game_minutes;
    quest_raise_event(2, a1, 0);
    l_2C = &a1->data.character;
    mc_set_location(587, D_001709E4);
    mc_sprintf(((char *)text_buffer), D_001709ED, l_2C->name);
    hud_message_add(((char *)text_buffer));
    a1->type = 44;
    if (l_2C->mobile_id < 43) {
        if (*game_settings & 4)
            a1->image = (D_00195DA0 << 7) + 1;
        else
            a1->image = (monster_corpse_textures[l_2C->mobile_id].file << 7) + monster_corpse_textures[l_2C->mobile_id].rec;
    } else {
        a1->image = (D_00195DA0 << 7) + 1;
    }
    collide_flags = 0;
    D_001940D7 |= 32;
    mc_memcpy(&l_4C, &a1->x, 12, D_001709E4, 605, 4);
    mc_memset(&l_4C.f12, 0, 12, D_001709E4, 606, 4);
    l_4C.name = D_00187B44;
    collide_move_object(a1, 0, &l_4C, 0);
    player_on_ground = l_20;
    ceiling_height = l_1C;
}
