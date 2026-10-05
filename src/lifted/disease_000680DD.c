/* disease.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_0_2 { unsigned char f:2; };
#include "records.h"

extern struct record *player_entity;
extern struct record *D_00195AA8;
extern char D_00195B08[];
extern char D_00195B84[];
extern struct character *player_character;
extern int game_minutes;
extern int nearest_creature_distance;
extern char nearest_creature[];
extern char D_001A3AA4[];
extern int D_001A3AA8;

extern int damage_apply(struct record *, int, int);
extern int player_in_daylight(void);
extern int player_in_temple(void);
extern int rand();
extern void item_damage(struct record *, int);
extern void enchant_extra_spell_points(struct item *, int);
extern void object_foreach(struct record *, int);
extern void item_repair_cb(int);
extern void item_break(struct record *);

void item_enchantment_tick(struct item *a1, int a2, int a3)
{
    int l_10;

    if (a2 != 3 && *(int *)D_00195B08 == 0) return;
    switch ((unsigned)a2) {
        break;
    case 3:
        *(int *)D_001A3AA4 = 0;
        enchant_extra_spell_points(a1, a3);
        player_character->max_magicka += *(short *)D_001A3AA4;
        D_001A3AA8 += *(int *)D_001A3AA4;
        if (*(int *)D_001A3AA4 != 0 && a3 >= 7 && a3 <= 10 && player_character->magicka < player_character->max_magicka) {
            player_character->magicka += *(short *)D_00195B08 * 5;
        }
        if (*(int *)D_00195B08 != 0 && ((struct bf8_0_2 *)&game_minutes)->f == 0) {
            item_damage(D_00195AA8, 1);
        }
        break;
    case 5:
        switch ((unsigned)a3) {
        case 1:
            if (player_in_daylight() == 0) return;
            goto L68256;
        case 2:
            if (player_in_daylight() != 0) return;
        default:
L68256:;
            player_character->health += *(short *)D_00195B08;
            if (player_character->health > player_character->max_health) {
                player_character->health = player_character->max_health;
            } else if ((rand() % 10) == 0) {
                item_damage(D_00195AA8, 1);
            }
        }
        break;
    case 17:
        if ((a3 == 0 && player_in_daylight() != 0) || (a3 != 0 && player_in_temple() != 0)) {
            damage_apply(player_entity, *(int *)D_00195B08, 0);
        }
        break;
    case 16:
        switch ((unsigned)a3) {
        case 1:
            if (player_in_daylight() == 0) return;
            break;
        case 2:
            if (player_in_temple() == 0) return;
        }
        if (a1->condition > *(int *)D_00195B08) {
            a1->condition -= *(short *)D_00195B08;
        } else {
            item_break(D_00195AA8);
        }
        break;
    case 21:
        switch ((unsigned)a3) {
        case 1:
            if (*(int *)D_00195B08 != 0 && ((unsigned)(game_minutes - player_character->last_kill_time)) > 1440) {
                damage_apply(player_entity, *(int *)D_00195B08, 0);
            }
            break;
        case 2:
            if (*(int *)D_00195B08 != 0 && ((unsigned)(game_minutes - player_character->last_kill_time)) > 10080) {
                damage_apply(player_entity, *(int *)D_00195B08, 0);
            }
        }
        break;
    case 8:
        *(int *)D_00195B84 = *(int *)D_00195B08;
        object_foreach(player_entity->children, (int)item_repair_cb);
        if ((rand() % 10) == 0) item_damage(D_00195AA8, 1);
        break;
    case 1:
        item_damage(D_00195AA8, 1);
        break;
    case 6:
        if (a3 == 0 && nearest_creature_distance < 128 && player_character->health != player_character->max_health) {
            damage_apply(*(struct record **)nearest_creature, *(int *)D_00195B08, 0);
            player_character->health += *(short *)D_00195B08;
            if (player_character->health > player_character->max_health) {
                player_character->health = player_character->max_health;
            }
            item_damage(D_00195AA8, *(int *)D_00195B08);
        }
    }
    if (player_character->magicka >= 0) return;
    player_character->magicka = 0;
}
