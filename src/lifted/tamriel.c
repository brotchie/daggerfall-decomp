/* tamriel.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern signed char D_0012B508;
extern char D_001711AC[];
extern unsigned char player_environment;
extern signed char building_open_hours[];
extern signed char D_0017C5B9[];
extern int month_names[];
extern int ordinal_suffixes[];
extern int D_001830E2;
extern signed char D_00187CA8;
extern int frame_checkpoint;
extern signed char region_punishment_flags[];
extern char region_legal_reputation[];
extern char region_price_adjustment[];
extern struct record *creature_list[];
extern signed char scratch_190ce4[];
extern signed char D_001940D8;
extern signed char D_001940D9;
extern struct record *quest_root;
extern struct record *bank_accounts;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *location_object;
extern int creature_count;
extern int calendar_year;
extern int calendar_weekday;
extern int calendar_month;
extern int calendar_minute;
extern int calendar_hour;
extern struct character *player_character;
extern int game_minutes;
extern int realtime_clock_tick;
extern struct record *quest_tick_object;
extern int player_death_timer;
extern signed char D_00195F24;
extern signed char D_00195F25;
extern signed char current_region;
extern signed char game_mode;
extern signed char crime_current;
extern signed char is_daytime;
extern signed char player_ailment_flags;
extern signed char D_00196298;
extern signed char D_001962A4;
extern signed char D_001962A5;
extern signed char quests_suspended;
extern signed char D_001962B0;
extern int faction_count;
extern struct faction *factions;
extern struct quest *current_quest;
extern int quest_debug_data;
extern struct record *quest_event_object;
extern struct quest *quest_tick_data;
extern int quest_ended_id;
extern short qbn_record_sizes[];
extern short D_0019978A;
extern short D_0019978C;
extern short D_0019978E;
extern short D_00199790;
extern short D_00199792;
extern short D_00199794;
extern short D_00199796;
extern short D_00199798;
extern short D_0019979A;
extern short quest_event_code;
extern int D_001AA698;

extern struct faction *faction_find_type_in_region(int, short);
extern struct faction *faction_find(short);
extern int quest_dispatch_event(struct quest *);
extern int holiday_today(int, int);
extern int poison_tick(int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_here_contains(int, int);
extern int spfx_disease_daily(int);
extern int spfx_disease_recover(int);
extern struct record *object_find_by_id(struct record *, int);
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern void region_flag_set(int, int);
extern void region_flag_clear(int, int);
extern void faction_politics_update(int);
extern void encounter_tick(int, int);
extern void automap_expire_records(void);
extern void automap_purge_old_files(void);
extern void quest_run_opcodes(int);
extern void quest_remove_objects(unsigned char);
extern void quest_faces_remove_quest(unsigned char);
extern void sky_update_moons(void);
extern void weather_roll(void);
extern void skill_add_uses(int, int);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void spell_tick(struct record *);
extern void disease_lycanthrope_tick(void);
extern void disease_special_infection_tick(void);
extern void disease_start_cure_quest(int);
extern void guild_expire_blessings(void);
extern void fatigue_update(void);
extern void spfx_walk_effect_records(struct record *, int);
extern void spfx_expire_created_items(void);
extern void object_foreach(struct record *, int);
extern void travel_show_days_left(int);
int building_is_open(struct building *);
void time_pass_minutes(int);
void calendar_update(void);
void interior_person_show_cb(struct record *);
void interior_person_hide_cb(struct record *);
void reputation_decay(void);
void region_update_prices(int);
void crime_check_beast_form_in_town(void);
void loan_due_penalty(void);
void quests_run_all(void);
#pragma aux mc_set_location parm routine [];

int calendar_format_date(int a1, int a2)
{
    short l_14;

    *(int *)&l_14 = ((unsigned)(((unsigned)(((unsigned)a1) % 518400)) / 1440)) % 30;
    mc_set_location(79, (int)D_001711AC);
    mc_sprintf(a2, D_001830E2, ((int)(short)l_14) + 1, ordinal_suffixes[((((int)(short)l_14) > 3) ? 3 : (int)(short)l_14)], month_names[(((unsigned)(((unsigned)a1) % 518400)) / 43200)]);
    return a2;
}

void time_pass(int a1)
{
    time_pass_minutes(a1);
}

void time_update_realtime(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (((struct bf8_6_1 *)&D_001940D8)->f != 0 || D_00187CA8 == 0) {
        l_1C = 1132;
        realtime_clock_tick = *(int *)((char *)l_1C);
        return;
    }
    l_20 = game_minutes;
    l_18 = 1132;
    l_24 = ((unsigned)(*(int *)((char *)l_18) - realtime_clock_tick)) / 90;
    realtime_clock_tick += l_24 * 90;
    if (((struct bf8_4_1 *)&D_001940D9)->f != 0 && ((int)(unsigned short)(player_character->flags & 1536)) == 0) {
        skill_add_uses(21, 1);
    }
    if (l_24 == 0) return;
    time_pass_minutes(l_24);
}

void time_pass_minutes(int a1)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    {
        unsigned char l_38;

        l_1C = game_minutes;
        l_18 = 0;
        if (((unsigned)a1) > 200000) a1 = 1;
        player_ailment_flags = 0;
        l_28 = ((unsigned)game_minutes) / 1440;
        l_24 = ((unsigned)game_minutes) / 10080;
        game_minutes += a1;
        l_20 = ((unsigned)game_minutes) % 1440;
        if (l_20 > 360 && l_20 < 1080) {
            l_38 = 1;
        } else {
            l_38 = 0;
        }
        is_daytime = l_38;
        l_28 = (((unsigned)game_minutes) / 1440) - l_28;
        frame_checkpoint = 2000;
        for (l_2C = 0; l_2C < l_28; l_2C++) {
            if (D_001962A4 == 0) {
                spfx_walk_effect_records(player_entity->children, (int)spfx_disease_daily);
            }
            frame_checkpoint = 2001;
            if (player_death_timer < 0) return;
            frame_checkpoint = 2002;
            weather_roll();
            frame_checkpoint = 2003;
            automap_expire_records();
        }
        frame_checkpoint = 2004;
        if (l_28 != 0) automap_purge_old_files();
        calendar_update();
        if (player_death_timer < 0) return;
        frame_checkpoint = 2005;
        for (l_2C = 0; ((unsigned)l_2C) < a1; l_2C++) {
            l_18++;
            if (D_001AA698 != 0 && (l_18 & 1023) == 0) travel_show_days_left(a1 - l_2C);
            frame_checkpoint = 2006;
            l_30 = (l_1C + l_2C) % 10080;
            if (l_30 == 0) {
                faction_politics_update(1);
                l_30 = (l_1C + l_2C) % 54720;
                if (l_30 == 0) {
                    frame_checkpoint = 2008;
                    faction_politics_update(2);
                    frame_checkpoint = 2009;
                    disease_start_cure_quest(0);
                    frame_checkpoint = 2010;
                }
            }
            if (((l_1C + l_2C) % 1440) == 0) region_update_prices(((l_30 == 0) ? 2 : 0));
            l_30 = (l_1C + l_2C) % 161280;
            if (l_30 == 0) reputation_decay();
            l_30 = (l_1C + l_2C) % 120960;
            if (l_30 == 0) disease_start_cure_quest(1);
            frame_checkpoint = 2013;
            if (D_001962A4 == 0) {
                frame_checkpoint = 2017;
                spell_tick(player_entity);
                spfx_walk_effect_records(player_entity, (int)spfx_disease_recover);
                D_00195AA8 = player_entity;
                spfx_walk_effect_records(player_entity->children, (int)poison_tick);
                fatigue_update();
                frame_checkpoint = 2018;
            }
            frame_checkpoint = 2019;
            sky_update_moons();
            frame_checkpoint = 2020;
            crime_check_beast_form_in_town();
            if (D_001962A5 != 0) quests_run_all();
            frame_checkpoint = 2022;
            loan_due_penalty();
            if (player_death_timer < 0) return;
            for (l_30 = 0; l_30 < creature_count; l_30++) {
                if ((int)creature_list[l_30] == (-1768515946)) continue;
                spell_tick(creature_list[l_30]);
                D_00195AA8 = creature_list[l_30];
                spfx_walk_effect_records(D_00195AA8->children, (int)poison_tick);
            }
            if (player_death_timer < 0) return;
        }
        frame_checkpoint = 2026;
        guild_expire_blessings();
        frame_checkpoint = 2027;
        for (l_2C = 0; ((unsigned)l_2C) < a1; l_2C++) {
            encounter_tick(((game_minutes - a1) + l_2C) + 1, 0);
        }
        if (((int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80))) < (-10) && rand_range(1, 100) < 5 && game_mode == 0) {
            crime_current = 7;
            guards_summon(0);
        }
        if (((int)(unsigned char)(region_punishment_flags[((int)(unsigned char)current_region) * 80] & 1)) != 0 && rand_range(1, 100) < 10 && game_mode == 0) {
            crime_current = 7;
            guards_summon(0);
        }
        frame_checkpoint = 2028;
        disease_lycanthrope_tick();
        frame_checkpoint = 2029;
        if (D_001962A4 == 0) disease_special_infection_tick();
        spfx_expire_created_items();
        D_00196298 = 1;
        frame_checkpoint = 2030;
    }
}

void calendar_update(void)
{
    int l_1C;
    int l_18;

    calendar_year = (((unsigned)game_minutes) / 518400) + 400;
    calendar_month = ((unsigned)(((unsigned)game_minutes) % 518400)) / 43200;
    calendar_weekday = ((unsigned)(((unsigned)game_minutes) / 1440)) % 7;
    calendar_hour = ((unsigned)(((unsigned)game_minutes) / 60)) % 24;
    calendar_minute = ((unsigned)game_minutes) % 60;
    D_00195F24 = ((unsigned)(((unsigned)game_minutes) / 1440)) % 22;
    D_00195F25 = (((unsigned)game_minutes) / 1440) & 15;
}

int player_in_daylight(void)
{
    int l_20;
    int l_1C;

    l_1C = ((unsigned)game_minutes) % 1440;
    if (l_1C < 360 || l_1C > 1080) return 0;
    if (((int)player_environment) == 1) {
        l_20 = 1;
    } else {
        l_20 = 0;
    }
    return l_20;
}

int player_in_temple(void)
{
    int l_20;
    struct building *l_1C;

    l_1C = object_building(player_object->parent);
    if (l_1C != 0) {
        if (l_1C->type == 14) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        return l_20;
    }
    return 0;
}

void func_0004B25B(struct record *a1)
{
    if (a1->type != 8) return;
    scratch_190ce4[0] = 1;
}

int building_is_open(struct building *a1)
{
    int l_28;
    struct record *l_24;
    struct record *l_20;
    struct membership *l_1C;

    l_24 = object_find_by_id(location_object, a1->id);
    if (a1->faction_id == 108 && guild_find_membership_by_kind(0) != 0) return 1;
    if (a1->faction_id == 42 && guild_find_membership_by_kind(3) != 0) return 1;
    if (a1->faction_id == 41 && (l_1C = guild_find_membership_by_kind(2)) != 0 && l_1C->rank >= 6) {
        return 1;
    }
    if (a1->faction_id == 40 && (l_1C = guild_find_membership_by_kind(1)) != 0 && l_1C->rank >= 6) {
        return 1;
    }
    l_20 = player_object->parent;
    while (l_20 != 0 && l_20->type != 43) l_20 = l_20->parent;
    if (a1->type >= 17 && a1->type <= 20 && is_daytime != 0 && (a1->id & 65535) % 100 < 50) {
        return 1;
    }
    if (a1->type > 16) return 0;
    l_28 = ((unsigned)(((unsigned)game_minutes) % 1440)) / 60;
    return (((((int)(unsigned char)building_open_hours[a1->type * 2]) <= l_28) && (((int)(unsigned char)D_0017C5B9[a1->type * 2]) > l_28)) ? 1 : 0);
}

int building_minutes_to_close(struct building *a1)
{
    int l_1C;

    l_1C = ((unsigned)(((unsigned)game_minutes) % 1440)) / 60;
    if (((int)(unsigned char)building_open_hours[a1->type * 2]) <= l_1C && ((int)(unsigned char)D_0017C5B9[a1->type * 2]) > l_1C) {
        return (((int)(unsigned char)D_0017C5B9[a1->type * 2]) * 60) - (((unsigned)game_minutes) % 1440);
    }
    return 0;
}

void interior_person_show_cb(struct record *a1)
{
    if (a1->type != 8) return;
    if ((a1->flags & 2048) != 0) return;
    a1->flags &= ~0x200;
}

void interior_person_hide_cb(struct record *a1)
{
    if (a1->type != 8) return;
    if (a1->quest_id != 0) return;
    a1->flags |= 0x200;
}

void building_update_open_state(void)
{
    int l_1C;
    struct building *l_18;

    if (((int)player_environment) > 2) return;
    l_18 = object_building(player_object->parent);
    if (l_18 == 0) return;
    if (building_is_open(l_18) != 0) {
        if (l_18->id != player_character->house) {
            l_1C = 1;
        } else {
            l_1C = 0;
        }
        if (l_1C != 0) goto L4B638;
    }
    goto L4B64F;
L4B638:;
    object_foreach(player_object->parent->children, (int)interior_person_show_cb);
    return;
L4B64F:;
    object_foreach(player_object->parent->children, (int)interior_person_hide_cb);
}

void reputation_decay(void)
{
    int l_18;

    if (D_001962B0 == 0) {
        for (l_18 = 0; l_18 < 62; l_18++) {
            if (*(short *)(region_legal_reputation + (l_18 * 80)) > 0) {
                (*(short *)(region_legal_reputation + (l_18 * 80)))--;
            } else if (*(short *)(region_legal_reputation + (l_18 * 80)) < 0) {
                (*(short *)(region_legal_reputation + (l_18 * 80)))++;
            }
        }
    }
    for (l_18 = 0; l_18 < faction_count; l_18++) {
        if (factions[l_18].reputation > 100) {
            factions[l_18].reputation = 100;
        } else if (factions[l_18].reputation < (-100)) {
            factions[l_18].reputation = 100;
        }
        if (factions[l_18].reputation > 0) {
            factions[l_18].reputation--;
        } else if (factions[l_18].reputation < 0) {
            factions[l_18].reputation++;
        }
    }
}

void holiday_announce(void)
{
    int l_18;

    l_18 = holiday_today(game_minutes, (int)(unsigned char)current_region);
    if (l_18 == 0) return;
    D_0012B508 = 146;
    msgbox_show_rsc((int)(short)(l_18 + 8349), 1);
}

void region_update_prices(int a1)
{
    int l_20;
    int l_1C;
    struct faction *l_18;

    for (l_20 = 0; l_20 < 62; l_20++) {
        l_18 = faction_find_type_in_region((int)(short)*(short *)&l_20, 7);
        if (l_18 == 0) continue;
        l_1C = (faction_find(510)->power - l_18->power) / 5;
        l_1C = (l_1C + 50) - ((((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) - 1000) / 25);
        if (rand_range(0, 100) < l_1C) {
            *(short *)(region_price_adjustment + (l_20 * 80)) = (((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) * 51) / 50;
        } else {
            *(short *)(region_price_adjustment + (l_20 * 80)) = (((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) * 49) / 50;
        }
        if (((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) > 4000) {
            *(short *)(region_price_adjustment + (l_20 * 80)) = 4000;
        } else if (((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) < 250) {
            *(short *)(region_price_adjustment + (l_20 * 80)) = 250;
        }
        if (((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) > 2000) {
            region_flag_set(l_20, 19);
        } else if (((int)(unsigned short)*(short *)(region_price_adjustment + (l_20 * 80))) < 500) {
            region_flag_set(l_20, 20);
        } else {
            region_flag_clear(l_20, 19);
            region_flag_clear(l_20, 20);
        }
    }
}

void crime_check_beast_form_in_town(void)
{
    if (player_character->race <= 8 || ((int)player_environment) != 1 || location_here_contains(player_object->x, player_object->z) == 0 || (((unsigned)game_minutes) % 30) != 0) {
        return;
    }
    guards_summon(0);
}

void loan_due_penalty(void)
{
    int l_1C;
    struct bank_account *l_18;

    l_18 = (bank_accounts)->data.bank_accounts;
    for (l_1C = 0; l_1C < 62; l_1C++) {
        if (l_18->loan_due == 0) continue;
        if (l_18->loan_due == game_minutes) {
            *(short *)(region_legal_reputation + (l_1C * 80)) -= 5;
        }
    }
}

void quest_init_record_sizes(void)
{
    qbn_record_sizes[0] = 19;
    D_0019978A = 94;
    D_0019978C = 34;
    D_0019978E = 20;
    D_00199790 = 24;
    D_00199792 = 16;
    D_00199794 = 33;
    D_00199796 = 14;
    D_00199798 = 87;
    D_0019979A = 8;
}

void quests_run_all(void)
{
    struct record *l_1C;
    struct record *l_18;

    if (quests_suspended != 0) return;
    l_1C = quest_root->children;
    while (l_1C != 0) {
        l_18 = l_1C->next;
        if (l_1C->type == 14) {
            quest_tick_object = l_1C;
            quest_tick_data = (struct quest *)((int)&l_1C->data.quest);
            if (quest_debug_data == 0) quest_debug_data = (int)quest_tick_data;
            quest_run_opcodes((int)quest_tick_data);
        }
        l_1C = l_18;
    }
}

struct quest *quest_find_by_id(int a1)
{
    struct record *l_20;
    struct quest *l_1C;

    l_20 = quest_root->children;
    while (l_20 != 0) {
        if (l_20->type == 14) {
            l_1C = &l_20->data.quest;
            if (l_1C->id == a1) {
                quest_tick_data = l_1C;
                return l_1C;
            }
        }
        l_20 = l_20->next;
    }
    return 0;
}

void quests_raise_event_all(int a1, int a2)
{
    struct record *l_18;
    int l_14;

    quest_event_code = a1;
    quest_event_object = (struct record *)a2;
    l_18 = quest_root->children;
    while (l_18 != 0) {
        if (l_18->type == 14) quest_dispatch_event(&l_18->data.quest);
        l_18 = l_18->next;
    }
}

void quest_end(int a1)
{
    quest_ended_id = current_quest->id;
    quest_faces_remove_quest((int)(unsigned char)(signed char)current_quest->id);
    quest_remove_objects((int)(unsigned char)(signed char)current_quest->id);
    quest_debug_data = 0;
}
