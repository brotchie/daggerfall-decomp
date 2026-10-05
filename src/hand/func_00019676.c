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
extern int func_0001AC53(struct faction *, struct faction *);
extern int faction_is_regional_noble(struct faction *);
extern int faction_has_enemy(struct faction *, struct faction *);
extern int faction_has_ally(struct faction *, struct faction *);
extern void faction_add_power(struct faction *, int);
extern int func_0001AEBE(struct faction *);
extern int func_0001AEF9(struct faction *, struct faction *);
extern int func_0001B06F(struct faction *, struct faction *);
extern int func_0001B144(struct faction *, struct faction *);
extern int faction_power(struct faction *);
extern void func_0001B22E(struct faction *);
extern int faction_player_related(struct faction *);
extern void faction_make_alliance(struct faction *, int, struct faction *);
extern void faction_make_enemies(struct faction *, int, struct faction *);
extern void faction_break_alliance(struct faction *, int);
extern void faction_make_peace(struct faction *, int);
extern void rumor_add_faction(struct faction *, struct faction *, int, unsigned char, int);
extern void rumor_file_open(void);
extern void rumor_file_close(void);
extern void rumor_file_purge(void);
extern int rand_range(int, int);
extern int rand(void);

void faction_politics_update(int a1)
{
    int i;
    int a;
    int b;
    int c;
    int rnd;
    int d;
    int l_34;
    int j;
    int r;
    int bonus;
    int p1;
    int p2;
    struct faction *f;
    struct faction *g;

    f = factions;
    if (a1 == 1)
        return;
    if (D_001962A5)
        rumor_file_purge();
    rumor_file_open();
    for (i = 0; i < faction_count; i++, f++) {
        if (!(f->type == 7 || f->type == 2 || f->type == 3))
            continue;
        a = (faction_power(f->allies[0]) + faction_power(f->allies[1]) + faction_power(f->allies[2])) / 10;
        b = (faction_power(f->enemies[0]) + faction_power(f->enemies[1]) + faction_power(f->enemies[2])) / 10;
        if (f->parent)
            c = f->parent->power / 10;
        else
            c = 0;
        rnd = rand_range(0, 100);
        if (f->politics_factor + a - b + c > rnd)
            faction_add_power(f, 1);
        else
            faction_add_power(f, -1);
        if (f->power < func_0001AEBE(f))
            faction_add_power(f, 1);
        if (a1 == 2) {
            d = f->power / 5;
            if (faction_is_regional_noble(f) && D_0018F060[f->region].f1) {
                D_0018F060[f->region].f1 = 0;
                D_0018F060[f->region].f2 = 1;
            }
            for (j = 0; j < 3; j++) {
                rnd = rand_range(0, 100);
                if (f->allies[j] && (func_0001AEF9(f, f->allies[j]) + (f->politics_factor + d)) / 5 + 70 < rnd)
                    faction_break_alliance(f, j);
            }
            for (j = 0; j < 3; j++) {
                if (func_0001B144(f, f->enemies[j]))
                    continue;
                rnd = rand_range(0, 100);
                if (f->enemies[j] && (func_0001AEF9(f, f->enemies[j]) + (f->politics_factor + d)) / 5 < rnd)
                    faction_make_peace(f, j);
            }
            for (j = 0; j < 3; j++) {
                if (f->allies[j])
                    continue;
                do {
                    g = faction_random();
                } while (!(g->type == 2 || g->type == 3 || g->type == 7));
                if (faction_has_ally(f, g) || faction_has_enemy(f, g))
                    continue;
                if (faction_has_enemy(f->allies[0], g) || faction_has_enemy(f->allies[1], g) || faction_has_enemy(f->allies[2], g))
                    continue;
                if (faction_has_ally(f->enemies[0], g) || faction_has_ally(f->enemies[1], g) || faction_has_ally(f->enemies[2], g))
                    continue;
                if (func_0001B06F(f, g))
                    continue;
                rnd = rand_range(0, 100);
                if ((func_0001AEF9(f, g) + (f->politics_factor + d)) / 5 <= rnd)
                    break;
                if (f->type == 7 && f->region != 255)
                    rumor_add_faction(f, g, 26, f->region, 1481);
                if (g->type == 7 && g->region != 255)
                    rumor_add_faction(g, f, 26, g->region, 1481);
                faction_make_alliance(f, j, g);
                break;
            }
            D_00195B84 = 0;
            if (func_0001AC53(f, f->enemies[0]) || func_0001AC53(f, f->enemies[1]) || func_0001AC53(f, f->enemies[2])) {
                D_00195B84--;
                if (D_0018F060[f->region].f3 || D_0018F060[f->region].f4) {
                    func_0001B22E(f);
                    func_0001B22E(f->enemies[D_00195B84]);
                    j = D_00195B84;
                    D_00195B84 = 0;
                    if (func_0001AC53(f->enemies[0], f) || func_0001AC53(f->enemies[1], f) || func_0001AC53(f->enemies[2], f))
                        f->enemies[j]->enemies[D_00195B84 - 1] = 0;
                    f->enemies[j] = 0;
                } else if (D_0018F060[f->region].f1) {
                    g = f->enemies[D_00195B84];
                    rumor_add_faction(f, g, 0, f->region, 1479);
                    rumor_add_faction(g, f, 0, g->region, 1479);
                    region_flag_set(f->region, 1);
                    region_flag_set(g->region, 1);
                } else if (D_0018F060[f->region].f2) {
                    if (rand_range(1, 100) <= 5) {
                        region_flag_clear(f->region, 1);
                        region_flag_clear(f->enemies[D_00195B84]->region, 1);
                    } else {
                        a = (faction_power(f->allies[0]) + faction_power(f->allies[1]) + faction_power(f->allies[2])) / 5;
                        p1 = a + f->power;
                        a = (faction_power(f->enemies[D_00195B84]->allies[0])
                             + faction_power(f->enemies[D_00195B84]->allies[1])
                             + faction_power(f->enemies[D_00195B84]->allies[2])) / 5;
                        p2 = a + f->enemies[D_00195B84]->power;
                        f->power -= rand_range(1, p1 / 10);
                        f->enemies[D_00195B84]->power -= rand_range(1, p2 / 10);
                        if (p1 < p2) {
                            if (p2 - p1 > p1) {
                                rumor_add_faction(f->enemies[D_00195B84], f, 100, 0, 1408);
                                faction_add_power(f->enemies[D_00195B84], f->power / 2);
                                region_flag_set(f->enemies[D_00195B84]->region, 2);
                                region_flag_set(f->region, 3);
                            } else {
                                rumor_add_faction(f, f->enemies[D_00195B84], 100, 0, 1407);
                            }
                        } else if (p1 - p2 > p2) {
                            rumor_add_faction(f, f->enemies[D_00195B84], 100, 0, 1408);
                            faction_add_power(f, f->enemies[D_00195B84]->power / 2);
                            region_flag_set(f->enemies[D_00195B84]->region, 3);
                            region_flag_set(f->region, 2);
                        } else {
                            rumor_add_faction(f, f->enemies[D_00195B84], 100, 0, 1407);
                        }
                    }
                }
            }
            for (j = 0; j < 3; j++) {
                if (f->enemies[j])
                    continue;
                do {
                    g = faction_random();
                } while (!(g->type == 2 || g->type == 3 || g->type == 7));
                if (faction_has_ally(f, g) || faction_has_enemy(f, g))
                    continue;
                if (faction_has_enemy(f->enemies[0], g) || faction_has_enemy(f->enemies[1], g) || faction_has_enemy(f->enemies[2], g))
                    continue;
                if (faction_has_ally(f->allies[0], g) || faction_has_ally(f->allies[1], g) || faction_has_ally(f->allies[2], g))
                    continue;
                r = func_0001B06F(f, g);
                if (r == 1 || r == 3)
                    continue;
                if (r == 2)
                    bonus = 10;
                else
                    bonus = 0;
                rnd = rand_range(0, 100);
                if ((func_0001AEF9(f, g) + (f->politics_factor + d)) / 5 + bonus + 70 >= rnd)
                    break;
                if (f->type == 7 && f->region != 255)
                    rumor_add_faction(f, g, 27, f->region, 1482);
                if (g->type == 7 && g->region != 255)
                    rumor_add_faction(g, f, 27, g->region, 1482);
                faction_make_enemies(f, j, g);
                if (faction_is_regional_noble(f) && faction_is_regional_noble(g) && func_0001B144(f, g)) {
                    rumor_add_faction(f, g, 100, 0, 1407);
                    if (f->type == 7 && f->region != 255)
                        rumor_add_faction(f, g, 28, f->region, 1479);
                    if (g->type == 7 && g->region != 255)
                        rumor_add_faction(g, f, 28, g->region, 1479);
                    region_flag_set(f->region, 0);
                    region_flag_set(g->region, 0);
                }
                break;
            }
            if (!(f->flags & 16) && (unsigned)rand_range(0, 100) > f->politics_factor / 3 + 70) {
                if (f->type == 7 && f->region != 255)
                    rumor_add_faction(f, 0, 12, f->region, 1480);
                f->politics_factor = rand_range(0, 50) + 20;
                f->seed <<= 16;
                f->seed = (f->seed & 0xffff0000) | rand();
                if (faction_player_related(f))
                    rumor_add_faction(f, 0, 100, 0, 1406);
            }
            if (f->region == 255 || f->type != 7)
                continue;
            a = (faction_power(f->allies[0]) + faction_power(f->allies[1]) + faction_power(f->allies[2])) / 10;
            if (D_0018F060[f->region].f10) {
                region_flag_clear(f->region, 9);
            } else if (D_0018F060[f->region].f9) {
                if ((unsigned)rand_range(0, 100) < a + f->politics_factor / 5 + f->power / 5) {
                    rumor_add_faction(f, 0, 7, f->region, 1477);
                    region_flag_set(f->region, 9);
                }
            } else if (D_0018F060[f->region].f8) {
                rumor_add_faction(f, 0, 7, f->region, 1477);
                region_flag_set(f->region, 8);
            } else if (rand_range(1, 100) <= 2) {
                if ((unsigned)rand_range(0, 100) > f->politics_factor + a) {
                    rumor_add_faction(f, 0, 7, f->region, 1477);
                    region_flag_set(f->region, 7);
                }
            }
            if (D_0017C912[f->region])
                g = faction_find(D_0017C912[f->region]);
            else
                g = 0;
            if (D_0018F060[f->region].f7) {
                region_flag_clear(f->region, 6);
            } else if (D_0018F060[f->region].f6) {
                if (g)
                    g->power--;
                f->power--;
                if ((unsigned)rand_range(0, 100) < a + f->politics_factor / 5 + f->power / 5) {
                    rumor_add_faction(f, 0, 4, f->region, 1478);
                    region_flag_set(f->region, 6);
                }
            } else if (D_0018F060[f->region].f5) {
                if (g)
                    g->power--;
                f->power--;
                rumor_add_faction(f, 0, 4, f->region, 1478);
                region_flag_set(f->region, 5);
            } else if (rand_range(1, 100) <= 2) {
                if ((unsigned)rand_range(0, 100) > f->politics_factor + a) {
                    if (g)
                        g->power--;
                    f->power--;
                    rumor_add_faction(f, 0, 4, f->region, 1478);
                    region_flag_set(f->region, 4);
                }
            }
            if (D_0017C912[f->region]) {
                g = faction_find(D_0017C912[f->region]);
                if (D_0018F060[f->region].f19)
                    g->power--;
                if (rand_range(0, 100) < (g->power - f->power + 5) / 5) {
                    if (g->power < f->power * 2) {
                        D_0018F060[f->region].f48 = g->id;
                        rumor_add_faction(f, 0, 18, f->region, 1476);
                        region_flag_set(f->region, 18);
                        g->power--;
                    } else {
                        region_flag_clear(f->region, 18);
                    }
                } else {
                    region_flag_clear(f->region, 18);
                }
            } else {
                region_flag_clear(f->region, 18);
            }
            if (D_0018F060[f->region].f12)
                f->power--;
            j = (faction_find(42)->power + faction_find(108)->power) / 2;
            if (rand_range(0, 100) < (j - f->power + 5) / 5) {
                rumor_add_faction(0, 0, 11, f->region, 1410);
                region_flag_set(f->region, 11);
                f->power--;
            } else {
                region_flag_clear(f->region, 11);
            }
            if (D_0018F060[f->region].f11)
                faction_add_power(faction_find_type_in_region(f->region, 8), -1);
            g = faction_find_type_in_region(f->region, 8);
            if (g) {
                if (rand_range(0, 100) < (g->power - f->power + 5) / 5) {
                    rumor_add_faction(f, 0, 10, f->region, 1475);
                    region_flag_set(f->region, 10);
                    g->power--;
                } else {
                    region_flag_clear(f->region, 10);
                }
            } else {
                region_flag_clear(f->region, 10);
            }
        }
    }
    if (a1 == 2) {
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
