/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002ECBE */
#include "records.h"
#include "clib.h"

extern unsigned char D_0012B508;
extern char D_001709A1[];
extern char D_001709C6[];
extern char D_001709E4[];
extern char D_001709ED[];
struct pic { short file, rec; };
extern struct pic monster_corpse_textures[];
extern struct collide_probe D_00187B44;
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
extern int collide_move_object(struct record *, int, struct move_request *, int);
extern void damage_find_empty_soul_trap_cb(struct record *);
extern void damage_drop_at_death_cb(struct record *);
extern void quest_raise_event(short, struct record *, struct record *);
extern int item_artifact_equipped(int);
extern int sound_play(int, struct record *, int);
extern void guild_count_crime(int, int);
extern iptr hud_message_add(char *);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern void object_foreach(struct record *, void (*)());
#pragma aux mc_set_location parm routine [];

void damage_creature_death(struct record *creature)
{
    struct character *creature_char;
    struct record *soul;
    struct move_request move;
    int saved_on_ground;
    int saved_ceiling;
    int has_soul_artifact;

    creature_char = &creature->data.character;
    saved_on_ground = player_on_ground;
    saved_ceiling = ceiling_height;
    if (creature == player_entity) {
        sound_play((player_character->flags & 1) + (player_character->race * 3 + 2) ? 258 : 243, player_object, 100);
        player_death_timer = 1000;
        return;
    }
    if (creature_char->mobile_id == 146)
        guild_count_crime(6, 1);
    found_object = creature;
    object_foreach(creature->children, damage_drop_at_death_cb);
    soul = creature->children;
    has_soul_artifact = item_artifact_equipped(9);
    if (creature_char->race < 43) {
        if (has_soul_artifact != 0) goto found;
        while (soul != 0) {
            if (soul->type == 19) {
found:
                scratch_object = 0;
                object_foreach(player_entity->children, damage_find_empty_soul_trap_cb);
                if (scratch_object == 0 && soul->type == 19) {
                    D_0012B508 = 146;
                    hud_message_add(D_001709A1);
                    hud_message_add(D_001709C6);
                    return;
                }
                if (scratch_object != 0) {
                    if (has_soul_artifact == 0 && rand_range(0, 100) > soul->soul_creature)
                        break;
                    soul = object_create_child(scratch_object, 0, 0);
                    soul->flags = 3;
                    soul->type = 20;
                    soul->soul_creature = creature_char->race;
                }
                break;
            }
            soul = soul->next;
        }
    }
    if (creature != player_entity)
        sound_play(17, creature, 105);
    player_character->last_kill_time = game_minutes;
    quest_raise_event(2, creature, 0);
    creature_char = &creature->data.character;
    mc_set_location(587, D_001709E4);
    mc_sprintf(((char *)text_buffer), D_001709ED, creature_char->name);
    hud_message_add(((char *)text_buffer));
    creature->type = 44;
    if (creature_char->mobile_id < 43) {
        if (*game_settings & 4)
            creature->image = (D_00195DA0 << 7) + 1;
        else
            creature->image = (monster_corpse_textures[creature_char->mobile_id].file << 7) + monster_corpse_textures[creature_char->mobile_id].rec;
    } else {
        creature->image = (D_00195DA0 << 7) + 1;
    }
    collide_flags = 0;
    D_001940D7 |= 32;
    mc_memcpy(&move, &creature->x, 12, D_001709E4, 605, 4);
    mc_memset(&move.angle_x, 0, 12, D_001709E4, 606, 4);
    move.probe = &D_00187B44;
    collide_move_object(creature, 0, &move, 0);
    player_on_ground = saved_on_ground;
    ceiling_height = saved_ceiling;
}
