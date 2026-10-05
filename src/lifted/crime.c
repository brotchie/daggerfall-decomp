/* crime.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int screen_buffer;
extern char D_001706E1[];
extern char D_00179EA8[];
extern signed char D_00187CA8;
extern signed char region_punishment_flags[];
extern char region_legal_reputation[];
extern struct record *creature_list[];
extern int D_00190CAC;
extern signed char scratch_190d16;
extern unsigned char court_state;
extern short court_prison_days;
extern short court_extra_days;
extern signed char D_001940D5;
extern struct record *player_entity;
extern struct record *location_object;
extern int creature_count;
extern struct character *player_character;
extern int window_image;
extern int player_death_timer;
extern short D_00195F34;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char crime_current;
extern signed char D_001962B2;
extern char court_reputation_change[];
extern int D_001A4A70[];
extern int D_001A4A74;

extern int faction_find_type_in_region(int, short);
extern int court_open(int);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int gold_total_alias(void);
extern int object_delete(int);
extern int player_to_random_marker(int, int);
extern int mc_free();
extern int mc_memcpy();
extern void faction_change_reputation(int, int);
extern void prison_serve_sentence(short);
extern void skill_add_uses(int, int);
extern void msgbox_show_rsc(int, int);
extern void time_pass(int);
extern void msgbox_choice_rsc(short, unsigned char, unsigned char, int, unsigned char, unsigned char, unsigned char);
extern void gold_spend(int);
extern void gold_remove_all(void);
void court_reputation_restore(void);
void court_close(void);
void court_remove_creatures(void);
void court_restore_vitals(void);

void court_frame(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (court_open(0) == 0) return;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_001706E1, 172, 4);
    if (((int)(unsigned char)game_mode) == 8) return;
    D_001940D5 |= 64;
    D_00195F34 = 194;
    switch (court_state) {
        return;
    case 1:
        if (((int)D_00196271) == 1) {
            if (scratch_190d16 == 0) {
                court_state = 5;
                return;
            }
            if (((int)(signed char)scratch_190d16) == 1) {
                court_state = 6;
                return;
            }
            D_00190CAC >>= 1;
            court_prison_days >>= 1;
            if (gold_can_afford(D_00190CAC) != 0) {
                gold_spend(D_00190CAC);
                court_reputation_restore();
                if (court_prison_days != 0) {
                    court_state = 3;
                } else {
                    player_to_random_marker((int)location_object, 8);
                    court_close();
                }
            } else {
                court_prison_days += (court_extra_days = ((-(gold_total_alias() - D_00190CAC)) / 40) + 1);
                gold_remove_all();
                court_reputation_restore();
                msgbox_show_rsc(8052, 1);
                court_state = 2;
            }
        } else {
            msgbox_choice_rsc(8064, 18, 19, 0, 108, 100, 0);
            court_state = 8;
        }
        return;
    case 2:
        msgbox_show_rsc(8055, 1);
        court_state = 3;
        return;
    case 3:
        player_to_random_marker((int)location_object, 8);
        prison_serve_sentence((int)(short)court_prison_days);
        court_restore_vitals();
        court_reputation_restore();
        court_state = 100;
        return;
    case 5:
        msgbox_show_rsc(8063, 1);
        region_punishment_flags[((int)(unsigned char)current_region) * 80] |= 1;
        player_to_random_marker((int)location_object, 8);
        court_state = 100;
        return;
    case 6:
        msgbox_show_rsc(8060, 1);
        region_punishment_flags[((int)(unsigned char)current_region) * 80] |= 2;
        court_state = 7;
        return;
    case 7:
        player_to_random_marker((int)location_object, 4);
        court_state = 100;
        return;
    case 8:
        if (((int)D_00196271) == 2) {
            l_20 = player_character->skills[2].value;
        } else {
            l_20 = player_character->skills[1].value;
        }
        skill_add_uses(l_20, 1);
        l_1C = ((player_character->attributes[5] + l_20) / 2) + ((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80)));
        if (l_1C < 5) {
            l_1C = 5;
        } else if (l_1C > 95) {
            l_1C = 95;
        }
        if (rand_range(1, 100) <= l_1C) {
            msgbox_show_rsc(8062, 1);
            court_state = 9;
            return;
        }
        if (scratch_190d16 == 0) {
            court_state = 5;
            return;
        }
        if (((int)(signed char)scratch_190d16) == 1) {
            court_state = 6;
            return;
        }
        l_1C = rand_range(1, 100);
        l_1C += (int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80));
        if (l_1C > 75) {
            D_00190CAC >>= 1;
        } else if (l_1C < 25) {
            D_00190CAC <<= 1;
        }
        court_state = 2;
        return;
    case 9:
        player_to_random_marker((int)location_object, 12);
        court_state = 100;
        return;
    case 100:
        court_close();
    default:;
    }
}

void crime_reputation_penalty(void)
{
    int l_18;

    *(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80)) -= *(short *)(D_00179EA8 + (((int)(unsigned char)crime_current) << 2));
    l_18 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15);
    faction_change_reputation(l_18, -(*(int *)(D_00179EA8 + (((int)(unsigned char)crime_current) << 2)) >> 1));
}

void court_reputation_restore(void)
{
    int l_18;

    *(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80)) += *(short *)court_reputation_change - 1;
    l_18 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15);
    faction_change_reputation(l_18, (-(*(int *)court_reputation_change - 1)) / 2);
    D_001A4A70[0] = (D_001A4A74 = 0);
    D_001962B2 = 1;
}

void court_close(void)
{
    short l_18;

    *(int *)&l_18 = player_character->fatigue;
    time_pass(240);
    player_character->fatigue = *(int *)&l_18;
    D_00187CA8 = 1;
    game_mode = 0;
    D_00196272 = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_001706E1, 350);
        window_image = -1751672937;
    }
    court_remove_creatures();
    D_001940D5 &= 191;
}

void court_remove_creatures(void)
{
    int l_18;

    for (l_18 = 0; l_18 < creature_count; l_18++) {
        if ((int)creature_list[l_18] == (int)player_entity) continue;
        object_delete((int)creature_list[l_18]);
    }
}

void court_restore_vitals(void)
{
    player_character->health = player_character->max_health;
    player_character->magicka = player_character->max_magicka;
    player_character->fatigue = (player_character->attributes[0] + player_character->attributes[4]) << 6;
    player_death_timer = 0;
}
