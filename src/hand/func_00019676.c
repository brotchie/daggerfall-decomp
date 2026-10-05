/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00019676 */
#include "records.h"

struct gstate {
    char f0;
    char f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12;
    char pad13[6];
    char f19, f20, f21;
    char pad22[8];
    char f30[18];
    short f48;
    char pad50[30];
};
extern unsigned char D_00178E59;
extern unsigned char D_00178E5F;
extern int D_0017C912[];
extern struct gstate D_0018F060[];
extern int D_00195B84;
extern char current_region;
extern char D_00196269;
extern char D_001962A5;
extern int faction_count;
extern struct faction *factions;
extern void region_flag_set(int, int);
extern void region_flag_clear(int, int);
extern struct faction *faction_find_type_in_region(short, short);
extern struct faction *faction_find(short);
extern struct faction *faction_random(void);
extern int factions_can_war(struct faction *, struct faction *);
extern int faction_is_regional_noble(struct faction *);
extern int faction_has_enemy(struct faction *, struct faction *);
extern int faction_has_ally(struct faction *, struct faction *);
extern void faction_add_power(struct faction *, int);
extern int faction_max_child_power(struct faction *);
extern int faction_shared_relations(struct faction *, struct faction *);
extern int faction_tree_relation(struct faction *, struct faction *);
extern int faction_regions_border(struct faction *, struct faction *);
extern int faction_power(struct faction *);
extern void region_reset_war(struct faction *);
extern int faction_player_related(struct faction *);
extern int faction_make_alliance(struct faction *, int, struct faction *);
extern int faction_make_enemies(struct faction *, int, struct faction *);
extern int faction_break_alliance(struct faction *, int);
extern int faction_make_peace(struct faction *, int);
extern void rumor_add_faction(struct faction *, struct faction *, int, unsigned char, int);
extern void rumor_file_open(void);
extern void rumor_file_close(void);
extern void rumor_file_purge(void);
extern int rand_range(int, int);
extern int rand(void);

void faction_politics_update(int mode)
{
    int i;
    int ally_power;
    int enemy_power;
    int parent_power;
    int roll;
    int power_bonus;
    int unused;
    int j;
    int relation;
    int bonus;
    int strength;
    int enemy_strength;
    struct faction *faction;
    struct faction *other;

    faction = factions;
    if (mode == 1)
        return;
    if (D_001962A5)
        rumor_file_purge();
    rumor_file_open();
    for (i = 0; i < faction_count; i++, faction++) {
        if (!(faction->type == 7 || faction->type == 2 || faction->type == 3))
            continue;
        ally_power = (faction_power(faction->allies[0]) + faction_power(faction->allies[1]) + faction_power(faction->allies[2])) / 10;
        enemy_power = (faction_power(faction->enemies[0]) + faction_power(faction->enemies[1]) + faction_power(faction->enemies[2])) / 10;
        if (faction->parent)
            parent_power = faction->parent->power / 10;
        else
            parent_power = 0;
        roll = rand_range(0, 100);
        if (faction->politics_factor + ally_power - enemy_power + parent_power > roll)
            faction_add_power(faction, 1);
        else
            faction_add_power(faction, -1);
        if (faction->power < faction_max_child_power(faction))
            faction_add_power(faction, 1);
        if (mode == 2) {
            power_bonus = faction->power / 5;
            if (faction_is_regional_noble(faction) && D_0018F060[faction->region].f1) {
                D_0018F060[faction->region].f1 = 0;
                D_0018F060[faction->region].f2 = 1;
            }
            for (j = 0; j < 3; j++) {
                roll = rand_range(0, 100);
                if (faction->allies[j] && (faction_shared_relations(faction, faction->allies[j]) + (faction->politics_factor + power_bonus)) / 5 + 70 < roll)
                    faction_break_alliance(faction, j);
            }
            for (j = 0; j < 3; j++) {
                if (faction_regions_border(faction, faction->enemies[j]))
                    continue;
                roll = rand_range(0, 100);
                if (faction->enemies[j] && (faction_shared_relations(faction, faction->enemies[j]) + (faction->politics_factor + power_bonus)) / 5 < roll)
                    faction_make_peace(faction, j);
            }
            for (j = 0; j < 3; j++) {
                if (faction->allies[j])
                    continue;
                do {
                    other = faction_random();
                } while (!(other->type == 2 || other->type == 3 || other->type == 7));
                if (faction_has_ally(faction, other) || faction_has_enemy(faction, other))
                    continue;
                if (faction_has_enemy(faction->allies[0], other) || faction_has_enemy(faction->allies[1], other) || faction_has_enemy(faction->allies[2], other))
                    continue;
                if (faction_has_ally(faction->enemies[0], other) || faction_has_ally(faction->enemies[1], other) || faction_has_ally(faction->enemies[2], other))
                    continue;
                if (faction_tree_relation(faction, other))
                    continue;
                roll = rand_range(0, 100);
                if ((faction_shared_relations(faction, other) + (faction->politics_factor + power_bonus)) / 5 <= roll)
                    break;
                if (faction->type == 7 && faction->region != 255)
                    rumor_add_faction(faction, other, 26, faction->region, 1481);
                if (other->type == 7 && other->region != 255)
                    rumor_add_faction(other, faction, 26, other->region, 1481);
                faction_make_alliance(faction, j, other);
                break;
            }
            D_00195B84 = 0;
            if (factions_can_war(faction, faction->enemies[0]) || factions_can_war(faction, faction->enemies[1]) || factions_can_war(faction, faction->enemies[2])) {
                D_00195B84--;
                if (D_0018F060[faction->region].f3 || D_0018F060[faction->region].f4) {
                    region_reset_war(faction);
                    region_reset_war(faction->enemies[D_00195B84]);
                    j = D_00195B84;
                    D_00195B84 = 0;
                    if (factions_can_war(faction->enemies[0], faction) || factions_can_war(faction->enemies[1], faction) || factions_can_war(faction->enemies[2], faction))
                        faction->enemies[j]->enemies[D_00195B84 - 1] = 0;
                    faction->enemies[j] = 0;
                } else if (D_0018F060[faction->region].f1) {
                    other = faction->enemies[D_00195B84];
                    rumor_add_faction(faction, other, 0, faction->region, 1479);
                    rumor_add_faction(other, faction, 0, other->region, 1479);
                    region_flag_set(faction->region, 1);
                    region_flag_set(other->region, 1);
                } else if (D_0018F060[faction->region].f2) {
                    if (rand_range(1, 100) <= 5) {
                        region_flag_clear(faction->region, 1);
                        region_flag_clear(faction->enemies[D_00195B84]->region, 1);
                    } else {
                        ally_power = (faction_power(faction->allies[0]) + faction_power(faction->allies[1]) + faction_power(faction->allies[2])) / 5;
                        strength = ally_power + faction->power;
                        ally_power = (faction_power(faction->enemies[D_00195B84]->allies[0])
                             + faction_power(faction->enemies[D_00195B84]->allies[1])
                             + faction_power(faction->enemies[D_00195B84]->allies[2])) / 5;
                        enemy_strength = ally_power + faction->enemies[D_00195B84]->power;
                        faction->power -= rand_range(1, strength / 10);
                        faction->enemies[D_00195B84]->power -= rand_range(1, enemy_strength / 10);
                        if (strength < enemy_strength) {
                            if (enemy_strength - strength > strength) {
                                rumor_add_faction(faction->enemies[D_00195B84], faction, 100, 0, 1408);
                                faction_add_power(faction->enemies[D_00195B84], faction->power / 2);
                                region_flag_set(faction->enemies[D_00195B84]->region, 2);
                                region_flag_set(faction->region, 3);
                            } else {
                                rumor_add_faction(faction, faction->enemies[D_00195B84], 100, 0, 1407);
                            }
                        } else if (strength - enemy_strength > enemy_strength) {
                            rumor_add_faction(faction, faction->enemies[D_00195B84], 100, 0, 1408);
                            faction_add_power(faction, faction->enemies[D_00195B84]->power / 2);
                            region_flag_set(faction->enemies[D_00195B84]->region, 3);
                            region_flag_set(faction->region, 2);
                        } else {
                            rumor_add_faction(faction, faction->enemies[D_00195B84], 100, 0, 1407);
                        }
                    }
                }
            }
            for (j = 0; j < 3; j++) {
                if (faction->enemies[j])
                    continue;
                do {
                    other = faction_random();
                } while (!(other->type == 2 || other->type == 3 || other->type == 7));
                if (faction_has_ally(faction, other) || faction_has_enemy(faction, other))
                    continue;
                if (faction_has_enemy(faction->enemies[0], other) || faction_has_enemy(faction->enemies[1], other) || faction_has_enemy(faction->enemies[2], other))
                    continue;
                if (faction_has_ally(faction->allies[0], other) || faction_has_ally(faction->allies[1], other) || faction_has_ally(faction->allies[2], other))
                    continue;
                relation = faction_tree_relation(faction, other);
                if (relation == 1 || relation == 3)
                    continue;
                if (relation == 2)
                    bonus = 10;
                else
                    bonus = 0;
                roll = rand_range(0, 100);
                if ((faction_shared_relations(faction, other) + (faction->politics_factor + power_bonus)) / 5 + bonus + 70 >= roll)
                    break;
                if (faction->type == 7 && faction->region != 255)
                    rumor_add_faction(faction, other, 27, faction->region, 1482);
                if (other->type == 7 && other->region != 255)
                    rumor_add_faction(other, faction, 27, other->region, 1482);
                faction_make_enemies(faction, j, other);
                if (faction_is_regional_noble(faction) && faction_is_regional_noble(other) && faction_regions_border(faction, other)) {
                    rumor_add_faction(faction, other, 100, 0, 1407);
                    if (faction->type == 7 && faction->region != 255)
                        rumor_add_faction(faction, other, 28, faction->region, 1479);
                    if (other->type == 7 && other->region != 255)
                        rumor_add_faction(other, faction, 28, other->region, 1479);
                    region_flag_set(faction->region, 0);
                    region_flag_set(other->region, 0);
                }
                break;
            }
            if (!(faction->flags & 16) && (unsigned)rand_range(0, 100) > faction->politics_factor / 3 + 70) {
                if (faction->type == 7 && faction->region != 255)
                    rumor_add_faction(faction, 0, 12, faction->region, 1480);
                faction->politics_factor = rand_range(0, 50) + 20;
                faction->seed <<= 16;
                faction->seed = (faction->seed & 0xffff0000) | rand();
                if (faction_player_related(faction))
                    rumor_add_faction(faction, 0, 100, 0, 1406);
            }
            if (faction->region == 255 || faction->type != 7)
                continue;
            ally_power = (faction_power(faction->allies[0]) + faction_power(faction->allies[1]) + faction_power(faction->allies[2])) / 10;
            if (D_0018F060[faction->region].f10) {
                region_flag_clear(faction->region, 9);
            } else if (D_0018F060[faction->region].f9) {
                if ((unsigned)rand_range(0, 100) < ally_power + faction->politics_factor / 5 + faction->power / 5) {
                    rumor_add_faction(faction, 0, 7, faction->region, 1477);
                    region_flag_set(faction->region, 9);
                }
            } else if (D_0018F060[faction->region].f8) {
                rumor_add_faction(faction, 0, 7, faction->region, 1477);
                region_flag_set(faction->region, 8);
            } else if (rand_range(1, 100) <= 2) {
                if ((unsigned)rand_range(0, 100) > faction->politics_factor + ally_power) {
                    rumor_add_faction(faction, 0, 7, faction->region, 1477);
                    region_flag_set(faction->region, 7);
                }
            }
            if (D_0017C912[faction->region])
                other = faction_find(D_0017C912[faction->region]);
            else
                other = 0;
            if (D_0018F060[faction->region].f7) {
                region_flag_clear(faction->region, 6);
            } else if (D_0018F060[faction->region].f6) {
                if (other)
                    other->power--;
                faction->power--;
                if ((unsigned)rand_range(0, 100) < ally_power + faction->politics_factor / 5 + faction->power / 5) {
                    rumor_add_faction(faction, 0, 4, faction->region, 1478);
                    region_flag_set(faction->region, 6);
                }
            } else if (D_0018F060[faction->region].f5) {
                if (other)
                    other->power--;
                faction->power--;
                rumor_add_faction(faction, 0, 4, faction->region, 1478);
                region_flag_set(faction->region, 5);
            } else if (rand_range(1, 100) <= 2) {
                if ((unsigned)rand_range(0, 100) > faction->politics_factor + ally_power) {
                    if (other)
                        other->power--;
                    faction->power--;
                    rumor_add_faction(faction, 0, 4, faction->region, 1478);
                    region_flag_set(faction->region, 4);
                }
            }
            if (D_0017C912[faction->region]) {
                other = faction_find(D_0017C912[faction->region]);
                if (D_0018F060[faction->region].f19)
                    other->power--;
                if (rand_range(0, 100) < (other->power - faction->power + 5) / 5) {
                    if (other->power < faction->power * 2) {
                        D_0018F060[faction->region].f48 = other->id;
                        rumor_add_faction(faction, 0, 18, faction->region, 1476);
                        region_flag_set(faction->region, 18);
                        other->power--;
                    } else {
                        region_flag_clear(faction->region, 18);
                    }
                } else {
                    region_flag_clear(faction->region, 18);
                }
            } else {
                region_flag_clear(faction->region, 18);
            }
            if (D_0018F060[faction->region].f12)
                faction->power--;
            j = (faction_find(42)->power + faction_find(108)->power) / 2;
            if (rand_range(0, 100) < (j - faction->power + 5) / 5) {
                rumor_add_faction(0, 0, 11, faction->region, 1410);
                region_flag_set(faction->region, 11);
                faction->power--;
            } else {
                region_flag_clear(faction->region, 11);
            }
            if (D_0018F060[faction->region].f11)
                faction_add_power(faction_find_type_in_region(faction->region, 8), -1);
            other = faction_find_type_in_region(faction->region, 8);
            if (other) {
                if (rand_range(0, 100) < (other->power - faction->power + 5) / 5) {
                    rumor_add_faction(faction, 0, 10, faction->region, 1475);
                    region_flag_set(faction->region, 10);
                    other->power--;
                } else {
                    region_flag_clear(faction->region, 10);
                }
            } else {
                region_flag_clear(faction->region, 10);
            }
        }
    }
    if (mode == 2) {
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f12)
                j++;
        faction_add_power(faction_find(42), j - 1);
        faction_add_power(faction_find(108), j - 1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f20)
                j++;
        if (j >= 3)
            faction_add_power(faction_find(510), 1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f21)
                j++;
        if (j >= 3)
            faction_add_power(faction_find(510), -1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f30[D_00178E5F])
                j++;
        if (j >= 3)
            faction_add_power(faction_find(510), -1);
        for (j = i = 0; i < 62; i++)
            if (D_0018F060[i].f30[D_00178E59])
                j++;
        if (j >= 3)
            faction_add_power(faction_find(510), 1);
    }
    rumor_add_faction(0, 0, 100, 0, 1450);
    rumor_add_faction(0, 0, 100, 0, 1451);
    rumor_add_faction(0, 0, 100, 0, 1452);
    rumor_add_faction(0, 0, 100, 0, 1453);
    rumor_add_faction(0, 0, 100, 0, 1454);
    rumor_add_faction(0, 0, 100, 0, 1455);
    rumor_add_faction(0, 0, 100, 0, 1456);
    rumor_file_close();
    D_00196269 = current_region;
}
