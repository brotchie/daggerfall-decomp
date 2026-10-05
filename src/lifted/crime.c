/* crime.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char screen_buffer[];
extern char D_001706E1[];
extern char D_00179EA8[];
extern char D_00187CA8[];
extern char region_punishment_flags[];
extern char region_legal_reputation[];
extern struct record *D_00190504[];
extern char D_00190CAC[];
extern char D_00190D16[];
extern char court_state[];
extern char court_prison_days[];
extern char D_00190DCA[];
extern char D_001940D5[];
extern struct record *player_entity;
extern struct record *D_00195AC4;
extern char creature_count[];
extern struct character *player_character;
extern char window_image[];
extern char player_death_timer[];
extern char D_00195F34[];
extern char current_region[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char crime_current[];
extern char D_001962B2[];
extern char court_reputation_change[];
extern char D_001A4A70[];
extern char D_001A4A74[];

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
    mc_memcpy(*(int *)screen_buffer, *(int *)window_image, 64000, (int)D_001706E1, 172, 4);
    if (((int)(unsigned char)*(signed char *)game_mode) == 8) return;
    *(signed char *)D_001940D5 |= 64;
    *(short *)D_00195F34 = 194;
    switch (*(unsigned char *)court_state) {
    return;
case 1:
    if (((int)(unsigned char)*(signed char *)D_00196271) != 1) goto L2134D;
    if (*(signed char *)D_00190D16 != 0) goto L21291;
    *(signed char *)court_state = 5;
    return;
L21291:;
    if (((int)(signed char)*(signed char *)D_00190D16) != 1) goto L212A9;
    *(signed char *)court_state = 6;
    return;
L212A9:;
    *(int *)D_00190CAC >>= 1;
    *(short *)court_prison_days >>= 1;
    if (gold_can_afford(*(int *)D_00190CAC) == 0) goto L212FC;
    gold_spend(*(int *)D_00190CAC);
    court_reputation_restore();
    if (*(short *)court_prison_days == 0) goto L212E6;
    *(signed char *)court_state = 3;
    goto L212FA;
L212E6:;
    player_to_random_marker((int)D_00195AC4, 8);
    court_close();
L212FA:;
    goto L2134B;
L212FC:;
    *(short *)court_prison_days += (*(short *)D_00190DCA = ((-(gold_total_alias() - *(int *)D_00190CAC)) / 40) + 1);
    gold_remove_all();
    court_reputation_restore();
    msgbox_show_rsc(8052, 1);
    *(signed char *)court_state = 2;
L2134B:;
    goto L21379;
L2134D:;
    msgbox_choice_rsc(8064, 18, 19, 0, 108, 100, 0);
    *(signed char *)court_state = 8;
L21379:;
    return;
case 2:
    msgbox_show_rsc(8055, 1);
    *(signed char *)court_state = 3;
    return;
case 3:
    player_to_random_marker((int)D_00195AC4, 8);
    prison_serve_sentence((int)(short)*(short *)court_prison_days);
    court_restore_vitals();
    court_reputation_restore();
    *(signed char *)court_state = 100;
    return;
case 5:
    msgbox_show_rsc(8063, 1);
    *(signed char *)(region_punishment_flags + (((int)(unsigned char)*(signed char *)current_region) * 80)) |= 1;
    player_to_random_marker((int)D_00195AC4, 8);
    *(signed char *)court_state = 100;
    return;
case 6:
    msgbox_show_rsc(8060, 1);
    *(signed char *)(region_punishment_flags + (((int)(unsigned char)*(signed char *)current_region) * 80)) |= 2;
    *(signed char *)court_state = 7;
    return;
case 7:
    player_to_random_marker((int)D_00195AC4, 4);
    *(signed char *)court_state = 100;
    return;
case 8:
    if (((int)(unsigned char)*(signed char *)D_00196271) != 2) goto L21469;
    l_20 = player_character->skills[2].value;
    goto L21478;
L21469:;
    l_20 = player_character->skills[1].value;
L21478:;
    skill_add_uses(l_20, 1);
    l_1C = ((player_character->attributes[5] + l_20) / 2) + ((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80)));
    if (l_1C >= 5) goto L214C1;
    l_1C = 5;
    goto L214CE;
L214C1:;
    if (l_1C <= 95) goto L214CE;
    l_1C = 95;
L214CE:;
    if (rand_range(1, 100) > l_1C) goto L214FD;
    msgbox_show_rsc(8062, 1);
    *(signed char *)court_state = 9;
    return;
L214FD:;
    if (*(signed char *)D_00190D16 != 0) goto L21512;
    *(signed char *)court_state = 5;
    return;
L21512:;
    if (((int)(signed char)*(signed char *)D_00190D16) != 1) goto L21527;
    *(signed char *)court_state = 6;
    return;
L21527:;
    l_1C = rand_range(1, 100);
    l_1C += (int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80));
    if (l_1C <= 75) goto L2155B;
    *(int *)D_00190CAC >>= 1;
    goto L21567;
L2155B:;
    if (l_1C >= 25) goto L21567;
    *(int *)D_00190CAC <<= 1;
L21567:;
    *(signed char *)court_state = 2;
    return;
case 9:
    player_to_random_marker((int)D_00195AC4, 12);
    *(signed char *)court_state = 100;
    return;
case 100:
    court_close();
default:;
}
}

void crime_reputation_penalty(void)
{
    int l_18;

    *(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80)) -= *(short *)(D_00179EA8 + (((int)(unsigned char)*(signed char *)crime_current) << 2));
    l_18 = faction_find_type_in_region((int)(short)((int)(unsigned char)*(signed char *)current_region), 15);
    faction_change_reputation(l_18, -(*(int *)(D_00179EA8 + (((int)(unsigned char)*(signed char *)crime_current) << 2)) >> 1));
}

void court_reputation_restore(void)
{
    int l_18;

    *(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80)) += *(short *)court_reputation_change - 1;
    l_18 = faction_find_type_in_region((int)(short)((int)(unsigned char)*(signed char *)current_region), 15);
    faction_change_reputation(l_18, (-(*(int *)court_reputation_change - 1)) / 2);
    *(int *)D_001A4A70 = (*(int *)D_001A4A74 = 0);
    *(signed char *)D_001962B2 = 1;
}

void court_close(void)
{
    short l_18;

    *(int *)&l_18 = player_character->fatigue;
    time_pass(240);
    player_character->fatigue = *(int *)&l_18;
    *(signed char *)D_00187CA8 = 1;
    *(signed char *)game_mode = 0;
    *(signed char *)D_00196272 = 0;
    if (*(int *)window_image == 0) goto L216E4;
    if (*(int *)window_image != (-1751672937)) goto L216E6;
L216E4:;
    goto L21704;
L216E6:;
    mc_free(*(int *)window_image, (int)D_001706E1, 350);
    *(int *)window_image = -1751672937;
L21704:;
    court_remove_creatures();
    *(signed char *)D_001940D5 &= 191;
}

void court_remove_creatures(void)
{
    int l_18;

    l_18 = 0;
L2172F:;
    if (l_18 < *(int *)creature_count) goto L21744;
    return;
L2173C:;
    l_18++;
    goto L2172F;
L21744:;
    if ((int)D_00190504[l_18] == (int)player_entity) goto L2173C;
    object_delete((int)D_00190504[l_18]);
    goto L2173C;
}

void court_restore_vitals(void)
{
    player_character->health = player_character->max_health;
    player_character->magicka = player_character->max_magicka;
    player_character->fatigue = (player_character->attributes[0] + player_character->attributes[4]) << 6;
    *(int *)player_death_timer = 0;
}
