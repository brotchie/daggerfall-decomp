/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000202C5 */
#include "records.h"

extern char D_00170604[];
extern char D_00170634[];
extern char D_0017063D[];
extern char D_00179E60[];
extern char D_00179E66[];
extern unsigned char D_00179E7F[];
extern unsigned char D_00179E90[];
extern unsigned char D_00179E94[];
extern unsigned char D_001850D4[];
extern int D_001850E5[];
extern signed char text_buffer[];
extern unsigned game_minutes;
extern struct settings *game_settings;
extern int trade_price;
extern unsigned char climate_weathers[];
extern char D_001961F5[];
extern unsigned char D_00196271;
extern struct faction *D_0019671C;
extern int current_quest;
extern struct faction *faction_find(short);
extern int climate_category(void);
extern void msgbox_show_string(char *, short);
extern void msgbox_show_rsc(int, int);
extern int flc_play_with_text(int, char *, int, int);
extern struct record *monster_summon_near_player(int);
extern int rand_range(int, int);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern int rand(void);
extern int srand(int);
extern int mc_memset();
extern int mc_strncpy();
extern char *xn_str_find_u16(char *, short, int);
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, ...);

#define REGION(o) D_001850D4[D_00179E94[(o)->id]]

void daedra_summon(struct record *object)
{
    char flc[44];
    struct faction *guild;
    struct faction *daedra;
    char *day_entry;
    int daedra_id;
    int chance;
    int n;
    int saved_seed;
    unsigned char *daedra_entry;
    struct person *person;
    unsigned char weather;
    unsigned char quest_letter;

    quest_letter = 48;
    person = &object->data.person;
    guild = faction_find(person->faction_id);
    while (guild->parent != 0)
        guild = guild->parent;
    if (guild->id == 40 || guild->type == 8) {
    } else {
        guild = guild->child;
    }
    switch (guild->id) {
    case 40:
        day_entry = xn_str_find_u16(D_00179E60, game_minutes % 518400 / 1440, 16);
        if (day_entry == 0 || D_00179E66 == day_entry) {
            msgbox_show_rsc(480, 1);
            return;
        }
        daedra_id = (day_entry - D_00179E60) / 2 + 1;
        daedra = faction_find(daedra_id);
        D_0019671C = daedra;
        if (REGION(daedra) == 56) {
            if (game_settings->view_flags & 4) {
                msgbox_show_rsc(400, 1);
                return;
            }
            quest_letter = 120;
        }
        trade_price = (100 - guild->reputation) * 1000 + 100000;
        msgbox_yes_no_rsc(481);
        if (D_00196271 == 2) return;
        chance = 30;
        break;
    case 21:
    case 22:
    case 24:
    case 26:
    case 27:
    case 29:
    case 33:
    case 35:
    case 36:
    case 82:
    case 84:
    case 88:
    case 92:
    case 94:
    case 98:
    case 106:
        day_entry = xn_str_find_u16(D_00179E60, game_minutes % 518400 / 1440, 16);
        if (day_entry == 0 || D_00179E66 == day_entry) {
            msgbox_show_rsc(480, 1);
            return;
        }
        daedra_id = (day_entry - D_00179E60) / 2 + 1;
        for (n = 0; n < 3; n++) {
            if (guild->enemies[n] != 0 && guild->enemies[n]->id == daedra_id) {
                msgbox_show_string(D_00170604, 1);
                return;
            }
        }
        daedra = faction_find(daedra_id);
        D_0019671C = daedra;
        if (REGION(daedra) == 56) {
            if (game_settings->view_flags & 4) {
                msgbox_show_rsc(400, 1);
                return;
            }
            quest_letter = 120;
        }
        trade_price = (100 - guild->reputation) * 1000 + 100000;
        msgbox_yes_no_rsc(481);
        if (D_00196271 == 2) return;
        chance = 30;
        break;
    default:
        saved_seed = rand();
        srand(game_minutes / 1440);
        daedra_id = 4;
        if (guild->id != 419) {
            while (daedra_id == 4)
                daedra_id = rand_range(1, 16);
        }
        daedra = faction_find(daedra_id);
        D_0019671C = daedra;
        if (REGION(daedra) == 56) {
            if (game_settings->view_flags & 4) {
                msgbox_show_rsc(400, 1);
                return;
            }
            quest_letter = 120;
        }
        trade_price = (100 - guild->reputation) * 1000 + 100000;
        msgbox_yes_no_rsc(481);
        if (D_00196271 == 2) return;
        chance = 30;
        srand(saved_seed);
        break;
    }
    if (trade_price < 0)
        trade_price = -trade_price;
    if (trade_price > 200000)
        trade_price = 200000;
    if (gold_can_afford(trade_price) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
    weather = climate_weathers[climate_category()];
    n = 5;
    if (weather == 6)
        n = 15;
    if (rand() % 101 <= n) {
        daedra_id = 9;
        daedra = faction_find(daedra_id);
    }
    chance += daedra->reputation;
    if (D_00179E7F[daedra_id] == 100 || weather == D_00179E7F[daedra_id])
        chance += 30;
    gold_spend(trade_price);
    if (rand_range(1, 100) > chance) {
        msgbox_show_rsc(484, 1);
        return;
    }
    if (daedra->flags & 64) {
        daedra_entry = &REGION(daedra);
        mc_memset(flc, 0, 44, D_00170634, 189, 4);
        current_quest = 0;
        flc_play_with_text(D_001850E5[daedra_entry - D_001850D4], flc, 482, 0);
        object = monster_summon_near_player(D_00179E90[rand_range(0, 4)]);
        if (object != 0)
            object->data.character.team = 1;
        return;
    }
    daedra->flags |= 64;
    mc_set_location(200, D_00170634);
    mc_sprintf(((char *)text_buffer), D_0017063D, REGION(daedra), quest_letter);
    mc_strncpy(D_001961F5, ((char *)text_buffer), 13, D_00170634, 201);
}
