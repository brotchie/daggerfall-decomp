/* tamriel.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern struct region regions[];
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
extern struct record *creature_list[];
extern signed char scratch_190ce4[];
extern signed char D_001940D8;
extern signed char D_001940D9;
extern struct record *quest_root;
extern struct record *bank_accounts;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *scratch_current_object;
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
extern iptr quest_debug_data;
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

extern struct faction *faction_find_type_in_region(short, short);
extern struct faction *faction_find(short);
extern int quest_dispatch_event(struct quest *);
extern int holiday_today(int, int);
extern int poison_tick(struct disease *);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_here_contains(int, int);
extern int spfx_disease_daily(struct disease *);
extern int spfx_disease_recover(struct disease *);
extern struct record *object_find_by_id(struct record *, iptr);
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern void region_flag_set(int, int);
extern void region_flag_clear(int, int);
extern void faction_politics_update(int);
extern void encounter_tick(int, int);
extern void automap_expire_records(void);
extern void automap_purge_old_files(void);
extern void quest_run_opcodes(struct quest *);
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
extern void spfx_walk_effect_records(struct record *, int (*)());
extern void spfx_expire_created_items(void);
extern void object_foreach(struct record *, void (*)());
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

iptr calendar_format_date(int minutes, iptr buffer)
{
    short day;

    *(int *)&day = ((unsigned)(((unsigned)(((unsigned)minutes) % 518400)) / 1440)) % 30;
    mc_set_location(79, (iptr)D_001711AC);
    mc_sprintf(buffer, D_001830E2, ((int)(short)day) + 1, ordinal_suffixes[((((int)(short)day) > 3) ? 3 : (int)(short)day)], month_names[(((unsigned)(((unsigned)minutes) % 518400)) / 43200)]);
    return buffer;
}

void time_pass(int minutes)
{
    time_pass_minutes(minutes);
}

void time_update_realtime(void)
{
    int minutes;
    int old_minutes;
    int *bios_clock;
    int *bios_clock2;

    if (((struct bf8_6_1 *)&D_001940D8)->f != 0 || D_00187CA8 == 0) {
        bios_clock = (int *)1132;
        realtime_clock_tick = *bios_clock;
        return;
    }
    old_minutes = game_minutes;
    bios_clock2 = (int *)1132;
    minutes = ((unsigned)(*bios_clock2 - realtime_clock_tick)) / 90;
    realtime_clock_tick += minutes * 90;
    if (((struct bf8_4_1 *)&D_001940D9)->f != 0 && ((int)(unsigned short)(player_character->flags & 1536)) == 0) {
        skill_add_uses(21, 1);
    }
    if (minutes == 0) return;
    time_pass_minutes(minutes);
}

void time_pass_minutes(int minutes)
{
    int n;
    int i;
    int days;
    int start_week;
    int minute_of_day;
    int start_minutes;
    int steps;
    {
        unsigned char day;

        start_minutes = game_minutes;
        steps = 0;
        if (((unsigned)minutes) > 200000) minutes = 1;
        player_ailment_flags = 0;
        days = ((unsigned)game_minutes) / 1440;
        start_week = ((unsigned)game_minutes) / 10080;
        game_minutes += minutes;
        minute_of_day = ((unsigned)game_minutes) % 1440;
        if (minute_of_day > 360 && minute_of_day < 1080) {
            day = 1;
        } else {
            day = 0;
        }
        is_daytime = day;
        days = (((unsigned)game_minutes) / 1440) - days;
        frame_checkpoint = 2000;
        for (i = 0; i < days; i++) {
            if (D_001962A4 == 0) {
                spfx_walk_effect_records(player_entity->children, spfx_disease_daily);
            }
            frame_checkpoint = 2001;
            if (player_death_timer < 0) return;
            frame_checkpoint = 2002;
            weather_roll();
            frame_checkpoint = 2003;
            automap_expire_records();
        }
        frame_checkpoint = 2004;
        if (days != 0) automap_purge_old_files();
        calendar_update();
        if (player_death_timer < 0) return;
        frame_checkpoint = 2005;
        for (i = 0; ((unsigned)i) < minutes; i++) {
            steps++;
            if (D_001AA698 != 0 && (steps & 1023) == 0) travel_show_days_left(minutes - i);
            frame_checkpoint = 2006;
            n = (start_minutes + i) % 10080;
            if (n == 0) {
                faction_politics_update(1);
                n = (start_minutes + i) % 54720;
                if (n == 0) {
                    frame_checkpoint = 2008;
                    faction_politics_update(2);
                    frame_checkpoint = 2009;
                    disease_start_cure_quest(0);
                    frame_checkpoint = 2010;
                }
            }
            if (((start_minutes + i) % 1440) == 0) region_update_prices(((n == 0) ? 2 : 0));
            n = (start_minutes + i) % 161280;
            if (n == 0) reputation_decay();
            n = (start_minutes + i) % 120960;
            if (n == 0) disease_start_cure_quest(1);
            frame_checkpoint = 2013;
            if (D_001962A4 == 0) {
                frame_checkpoint = 2017;
                spell_tick(player_entity);
                spfx_walk_effect_records(player_entity, spfx_disease_recover);
                scratch_current_object = player_entity;
                spfx_walk_effect_records(player_entity->children, poison_tick);
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
            for (n = 0; n < creature_count; n++) {
                if ((iptr)creature_list[n] == (-1768515946)) continue;
                spell_tick(creature_list[n]);
                scratch_current_object = creature_list[n];
                spfx_walk_effect_records(scratch_current_object->children, poison_tick);
            }
            if (player_death_timer < 0) return;
        }
        frame_checkpoint = 2026;
        guild_expire_blessings();
        frame_checkpoint = 2027;
        for (i = 0; ((unsigned)i) < minutes; i++) {
            encounter_tick(((game_minutes - minutes) + i) + 1, 0);
        }
        if (regions[(unsigned char)current_region].legal_reputation < (-10) && rand_range(1, 100) < 5 && game_mode == 0) {
            crime_current = 7;
            guards_summon(0);
        }
        if (((int)(unsigned char)(regions[(unsigned char)current_region].punishment_flags & 1)) != 0 && rand_range(1, 100) < 10 && game_mode == 0) {
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
    int unused1;
    int unused2;

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
    int result;
    int minute_of_day;

    minute_of_day = ((unsigned)game_minutes) % 1440;
    if (minute_of_day < 360 || minute_of_day > 1080) return 0;
    if (((int)player_environment) == 1) {
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int player_in_temple(void)
{
    int result;
    struct building *building;

    building = object_building(player_object->parent);
    if (building != 0) {
        if (building->type == 14) {
            result = 1;
        } else {
            result = 0;
        }
        return result;
    }
    return 0;
}

void func_0004B25B(struct record *object)
{
    if (object->type != 8) return;
    scratch_190ce4[0] = 1;
}

int building_is_open(struct building *building)
{
    int hour;
    struct record *building_object;
    struct record *block;
    struct membership *membership;

    building_object = object_find_by_id(location_object, building->id);
    if (building->faction_id == 108 && guild_find_membership_by_kind(0) != 0) return 1;
    if (building->faction_id == 42 && guild_find_membership_by_kind(3) != 0) return 1;
    if (building->faction_id == 41 && (membership = guild_find_membership_by_kind(2)) != 0 && membership->rank >= 6) {
        return 1;
    }
    if (building->faction_id == 40 && (membership = guild_find_membership_by_kind(1)) != 0 && membership->rank >= 6) {
        return 1;
    }
    block = player_object->parent;
    while (block != 0 && block->type != 43) block = block->parent;
    if (building->type >= 17 && building->type <= 20 && is_daytime != 0 && (building->id & 65535) % 100 < 50) {
        return 1;
    }
    if (building->type > 16) return 0;
    hour = ((unsigned)(((unsigned)game_minutes) % 1440)) / 60;
    return (((((int)(unsigned char)building_open_hours[building->type * 2]) <= hour) && (((int)(unsigned char)D_0017C5B9[building->type * 2]) > hour)) ? 1 : 0);
}

int building_minutes_to_close(struct building *building)
{
    int hour;

    hour = ((unsigned)(((unsigned)game_minutes) % 1440)) / 60;
    if (((int)(unsigned char)building_open_hours[building->type * 2]) <= hour && ((int)(unsigned char)D_0017C5B9[building->type * 2]) > hour) {
        return (((int)(unsigned char)D_0017C5B9[building->type * 2]) * 60) - (((unsigned)game_minutes) % 1440);
    }
    return 0;
}

void interior_person_show_cb(struct record *object)
{
    if (object->type != 8) return;
    if ((object->flags & 2048) != 0) return;
    object->flags &= ~0x200;
}

void interior_person_hide_cb(struct record *object)
{
    if (object->type != 8) return;
    if (object->quest_id != 0) return;
    object->flags |= 0x200;
}

void building_update_open_state(void)
{
    int show;
    struct building *building;

    if (((int)player_environment) > 2) return;
    building = object_building(player_object->parent);
    if (building == 0) return;
    if (building_is_open(building) != 0) {
        if (building->id != player_character->house) {
            show = 1;
        } else {
            show = 0;
        }
        if (show != 0) goto L4B638;
    }
    goto L4B64F;
L4B638:;
    object_foreach(player_object->parent->children, interior_person_show_cb);
    return;
L4B64F:;
    object_foreach(player_object->parent->children, interior_person_hide_cb);
}

void reputation_decay(void)
{
    int i;

    if (D_001962B0 == 0) {
        for (i = 0; i < 62; i++) {
            if (regions[i].legal_reputation > 0) {
                (regions[i].legal_reputation)--;
            } else if (regions[i].legal_reputation < 0) {
                (regions[i].legal_reputation)++;
            }
        }
    }
    for (i = 0; i < faction_count; i++) {
        if (factions[i].reputation > 100) {
            factions[i].reputation = 100;
        } else if (factions[i].reputation < (-100)) {
            factions[i].reputation = 100;
        }
        if (factions[i].reputation > 0) {
            factions[i].reputation--;
        } else if (factions[i].reputation < 0) {
            factions[i].reputation++;
        }
    }
}

void holiday_announce(void)
{
    int holiday;

    holiday = holiday_today(game_minutes, (int)(unsigned char)current_region);
    if (holiday == 0) return;
    D_0012B508 = 146;
    msgbox_show_rsc((int)(short)(holiday + 8349), 1);
}

void region_update_prices(int mode)
{
    int region;
    int chance;
    struct faction *faction;

    for (region = 0; region < 62; region++) {
        faction = faction_find_type_in_region((int)(short)*(short *)&region, 7);
        if (faction == 0) continue;
        chance = (faction_find(510)->power - faction->power) / 5;
        chance = (chance + 50) - ((regions[region].price_adjustment - 1000) / 25);
        if (rand_range(0, 100) < chance) {
            regions[region].price_adjustment = (regions[region].price_adjustment * 51) / 50;
        } else {
            regions[region].price_adjustment = (regions[region].price_adjustment * 49) / 50;
        }
        if (regions[region].price_adjustment > 4000) {
            regions[region].price_adjustment = 4000;
        } else if (regions[region].price_adjustment < 250) {
            regions[region].price_adjustment = 250;
        }
        if (regions[region].price_adjustment > 2000) {
            region_flag_set(region, 19);
        } else if (regions[region].price_adjustment < 500) {
            region_flag_set(region, 20);
        } else {
            region_flag_clear(region, 19);
            region_flag_clear(region, 20);
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
    int region;
    struct bank_account *account;

    account = (bank_accounts)->data.bank_accounts;
    for (region = 0; region < 62; region++) {
        if (account->loan_due == 0) continue;
        if (account->loan_due == game_minutes) {
            regions[region].legal_reputation -= 5;
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
    struct record *object;
    struct record *next;

    if (quests_suspended != 0) return;
    object = quest_root->children;
    while (object != 0) {
        next = object->next;
        if (object->type == 14) {
            quest_tick_object = object;
            quest_tick_data = (struct quest *)((iptr)&object->data.quest);
            if (quest_debug_data == 0) quest_debug_data = (iptr)quest_tick_data;
            quest_run_opcodes(quest_tick_data);
        }
        object = next;
    }
}

struct quest *quest_find_by_id(int id)
{
    struct record *object;
    struct quest *quest;

    object = quest_root->children;
    while (object != 0) {
        if (object->type == 14) {
            quest = &object->data.quest;
            if (quest->id == id) {
                quest_tick_data = quest;
                return quest;
            }
        }
        object = object->next;
    }
    return 0;
}

void quests_raise_event_all(int event, struct record *event_object)
{
    struct record *object;
    int unused;

    quest_event_code = event;
    quest_event_object = event_object;
    object = quest_root->children;
    while (object != 0) {
        if (object->type == 14) quest_dispatch_event(&object->data.quest);
        object = object->next;
    }
}

void quest_end(int unused)
{
    quest_ended_id = current_quest->id;
    quest_faces_remove_quest((int)(unsigned char)(signed char)current_quest->id);
    quest_remove_objects((int)(unsigned char)(signed char)current_quest->id);
    quest_debug_data = 0;
}
