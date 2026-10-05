/* parse.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern struct region regions[];
extern char D_0017110C[];
extern char D_00171134[];
extern char D_00171146[];
extern char D_0017114E[];
extern char D_00171154[];
extern char D_0017115C[];
extern char D_00171164[];
extern char D_0017116D[];
extern char D_00171173[];
extern char D_00171177[];
extern char D_00171178[];
extern char D_00171185[];
extern char D_0017118B[];
extern char D_00171192[];
extern char D_0017119C[];
extern signed char D_00178630[];
extern unsigned char player_environment;
extern struct spell *selected_spell;
extern int weight_fractions[];
extern char legal_reputation_ranges[];
extern char D_0017C552[];
extern int ruler_fates[];
extern char D_0017C912[];
extern short holiday_days[];
extern signed char holiday_regions[];
extern int holiday_names[];
extern char D_0017CB8E[];
extern int month_names[];
extern int day_names[];
extern int ordinal_suffixes[];
extern char race_names[];
extern signed char D_0017CC1F[];
extern char skill_names[];
extern char *faction_rank_names[];
extern char material_names[];
extern char material_to_hit[];
extern char weapon_damage_min[];
extern char weapon_damage_max[];
extern char armor_type_names[];
extern unsigned char condition_thresholds[];
extern int condition_names[];
extern int pronoun_he[];
extern int pronoun_him[];
extern int pronoun_his[];
extern int D_0017D042[];
extern int province_names[];
extern int province_terrain_names[];
extern int attribute_rating_names[];
extern int direction_names[];
extern struct item_template item_templates[];
extern char D_00182F92[];
extern char monster_names[];
extern char ruler_titles[];
extern int D_001837E0;
extern char region_names[];
extern signed char D_001841E3[];
extern int crime_names[];
extern int penalty_texts[];
extern int D_00184269;
extern int codeword_first_words[];
extern int codeword_second_words[];
extern char monster_weights[];
extern int imperial_names[];
extern char location_type_names[];
extern int text_blank;
extern int honorifics;
extern int D_0018508F;
extern int legal_reputation_names[];
extern char item_group_templates[];
extern char saved_location_name[];
extern char saved_region_name[];
extern char scratch_190be4[];
extern int scratch_190bec;
extern int automap_yaw;
extern int D_00190BF8;
extern int D_00190BFC;
extern int D_00190C00;
extern char D_00190C78[];
extern int scratch_190cac;
extern int D_00190CD4;
extern int D_00190CD8;
extern int D_00190CDC;
extern signed char scratch_190d16;
extern signed char scratch_190d17;
extern signed char scratch_190d20;
extern signed char D_00190D21;
extern signed char D_00190D22;
extern short court_prison_days;
extern short court_extra_days;
extern char scratch_190de4[];
extern int scratch_190de8;
extern int scratch_190dec;
extern char scratch_190df0[];
extern int scratch_190df8;
extern int scratch_190dfc;
extern int text_macro_map_location;
extern signed char text_rsc_buffer[];
extern unsigned char D_001940D7;
extern char text_macro_fcn[];
extern struct record *wagon_container;
extern struct record *house_container;
extern struct item *text_macro_item;
extern struct character *text_macro_npc;
extern int D_00195A90;
extern int D_00195A94;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern int D_00195AC0;
extern struct record *location_object;
extern int D_00195ACC;
extern int text_macro_book;
extern struct building *tavern_building;
extern struct region *current_region_data;
extern int weight_total;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern char scratch_buffer[];
extern struct talk_where *D_00195D28;
extern int trade_total;
extern int trade_price;
extern int quest_potential_questor;
extern char text_rsc_file[];
extern int text_rsc_main_file;
extern int text_macro_city;
extern int text_macro_travel_city;
extern short painting_subject_text;
extern short painting_adjective_text;
extern short painting_prefix1_text;
extern short painting_prefix2_text;
extern short spell_effect_slot;
extern short text_macro_n_text;
extern signed char text_macro_imperial;
extern signed char D_00196267;
extern signed char current_region;
extern signed char D_00196269;
extern signed char D_0019626C;
extern signed char text_macro_gender;
extern signed char D_001962AE;
extern char talk_key_text[];
extern char D_00196661[];
extern struct faction *D_0019670C;
extern struct faction *D_00196714;
extern struct faction *D_0019671C;
extern struct faction *D_00196720;
extern struct faction *D_00196724;
extern struct faction *D_00196728;
extern short reputation_baseline;
extern short D_00196D7E;
extern short D_00196D80;
extern short D_00196D82;
extern short D_00196D84;
extern int text_macro_skill;
extern int parse_name_seed;
extern int parse_number;
extern struct membership *guild_membership;

extern int talk_macro_hint(int);
extern int talk_macro_1com(void);
extern struct faction *faction_find_type_in_region(short, short);
extern int faction_find(short);
extern int text_rsc_load(int, int, int);
extern int parse_regional_name(int, int);
extern int parse_town_building_name(short);
extern int calendar_format_date(int, int);
extern int item_armor_value(struct item *);
extern struct flat_cfg *flats_cfg_find(int);
extern int enchant_powers_text(struct item *);
extern struct membership *guild_find_membership_by_bits(unsigned char);
extern int rand_range(int, int);
extern int object_building(int);
extern int carry_capacity(void);
extern int name_generate_seeded(unsigned char, unsigned char, int);
extern int npc_display_name(int);
extern int name_generate(unsigned char, unsigned char);
extern int building_name(int);
extern int rand();
extern int srand();
extern int mc_free();
extern int mc_strncpy();
extern int atoi();
extern int utoa();
extern int itoa();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int strchr();
extern int xn_math_angle_to_point();
extern int xn_str_copy_word();
extern int xn_str_find_u32();
extern void parse_expand(int, int);
extern void object_foreach(struct record *, void (*)());
int macro_dat_date(void);
int macro_fl1_faction1_leader(void);
int macro_pcn_player_name(void);
int parse_faction_ruler_title(struct faction *);
int parse_bio_answer_text(int);
int parse_signed_itoa(int, char *, int);
int object_weight(struct record *);
int parse_stub_zero(int);
int holiday_index(int, int);
int holiday_name(int, int);
void object_weight_add(struct record *);
void parse_item_name(struct item *, char *);
void parse_rsc_text(int, int, int);
#pragma aux mc_set_location parm routine [];

int macro_fal_faction_name(void)
{
    return (int)D_00196724->name;
}

int macro_a_price(void)
{
    return itoa(trade_price, (int)text_rsc_buffer, 10);
}

int macro_agi_agility(void)
{
    return itoa(player_character->attributes[3], (int)text_rsc_buffer, 10);
}

int macro_ap_other_province(void)
{
    return D_0017D042[((int)(unsigned char)(D_00196267 ^ 1))];
}

int macro_arm_item_name(void)
{
    if (text_macro_item->enchantments[0].type == (-1)) return (int)text_macro_item;
    parse_item_name(text_macro_item, (char *)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_ach_chance_per_level(void)
{
    return itoa((int)selected_spell->chances[(int)(short)spell_effect_slot].plus, (int)text_rsc_buffer, 10);
}

int macro_adr_duration_per_level(void)
{
    return itoa((int)selected_spell->durations[(int)(short)spell_effect_slot].plus, (int)text_rsc_buffer, 10);
}

int macro_1am_magnitude_per_level_min(void)
{
    return itoa((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].plus_min, (int)text_rsc_buffer, 10);
}

int macro_2am_magnitude_per_level_max(void)
{
    return itoa((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].plus_max, (int)text_rsc_buffer, 10);
}

int macro_ark_attribute_rating(void)
{
    short rating;

    rating = player_character->attributes[(int)(unsigned char)D_0019626C] / 10;
    if (((int)(short)rating) >= 10) rating = 9;
    return attribute_rating_names[((((int)(unsigned char)D_0019626C) * 10) + ((int)(short)rating))];
}

int macro_adj_painting_adjective(void)
{
    parse_rsc_text((int)(unsigned short)painting_adjective_text, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_an_artist_name(void)
{
    return name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
}

int macro_alc_faction_name(void)
{
    return (int)D_00196728->name;
}

int macro_alf_faction_name(void)
{
    return (int)D_00196728->name;
}

int macro_mod_armor_modifier(void)
{
    int unused;

    if (text_macro_item->index >= 7) {
        return parse_signed_itoa(text_macro_item->index - 6, (char *)text_rsc_buffer, 10);
    }
    return parse_signed_itoa(item_armor_value(text_macro_item) / 10, (char *)text_rsc_buffer, 10);
}

int macro_brd_regional_name(void)
{
    int saved_seed;
    int name;
    struct record *npc;

    npc = (struct record *)parse_stub_zero(511);
    saved_seed = rand();
    srand((npc->id & 65535) ^ (((unsigned)npc->id) >> 16));
    name = name_generate((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(flats_cfg_find(npc->image)->flags & 1));
    srand(saved_seed);
    return name;
}

int macro_bch_base_chance(void)
{
    return itoa((int)selected_spell->chances[(int)(short)spell_effect_slot].base, (int)text_rsc_buffer, 10);
}

int macro_bdr_base_duration(void)
{
    return itoa((int)selected_spell->durations[(int)(short)spell_effect_slot].base, (int)text_rsc_buffer, 10);
}

int macro_1bm_base_magnitude_min(void)
{
    return itoa((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].base_min, (int)text_rsc_buffer, 10);
}

int macro_2bm_base_magnitude_max(void)
{
    return itoa((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].base_max, (int)text_rsc_buffer, 10);
}

int macro_bt_book_title(void)
{
    return text_macro_book;
}

int macro_ba_book_author(void)
{
    return text_macro_book + 64;
}

int macro_cn_city_name(void)
{
    if (text_macro_city != 0) return text_macro_city;
    if (location_object->image != 65535) return (int)current_location;
    return *(int *)(region_names + (((int)(unsigned char)current_region) << 2));
}

int macro_cn2_blank(void)
{
    return text_blank;
}

int macro_ct_location_type(void)
{
    return *(int *)(location_type_names + (current_location->kind << 2));
}

int macro_clc_chance_levels(void)
{
    return itoa((int)selected_spell->chances[(int)(short)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_cld_duration_levels(void)
{
    return itoa((int)selected_spell->durations[(int)(short)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_clm_magnitude_levels(void)
{
    return itoa((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_cti_item_value_tenth(void)
{
    return itoa(((unsigned)text_macro_item->value) / 10, (int)text_rsc_buffer, 10);
}

int macro_cne_faction_name(void)
{
    return (int)D_00196728->name;
}

int macro_cnr_faction_name(void)
{
    return (int)D_00196720->name;
}

int macro_cri_crime(void)
{
    return crime_names[((int)(signed char)scratch_190d17)];
}

int macro_cpn_shop_name(void)
{
    return building_name((int)current_building);
}

int macro_crn_current_region(void)
{
    return *(int *)(region_names + (((int)(unsigned char)current_region) << 2));
}

int macro_dae_daedra_name(void)
{
    return (int)D_0019671C->name;
}

int macro_dnc_regional_name(void)
{
    int saved_seed;
    int name;
    struct record *npc;

    npc = (struct record *)parse_stub_zero(515);
    saved_seed = rand();
    srand((npc->id & 65535) ^ (((unsigned)npc->id) >> 16));
    name = name_generate((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(flats_cfg_find(npc->image)->flags & 1));
    srand(saved_seed);
    return name;
}

int macro_dam_damage_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[0] / 10) - 5, (char *)text_rsc_buffer, 10);
}

int macro_dwr_room_hours_left(void)
{
    return itoa(((unsigned)(tavern_building->rent_expires - game_minutes)) / 60, (int)text_rsc_buffer, 10);
}

int macro_dpr_other_random_province(void)
{
    int province;

    province = rand() & 7;
    while ((player_character->min_metal_to_hit >> 5) == province) province = rand() & 7;
    return province_names[province];
}

int macro_di_direction(void)
{
    return direction_names[((((xn_math_angle_to_point(player_object->x, player_object->z, D_00195A90, D_00195A94) >> 2) + 32) & 511) >> 6)];
}

int macro_du_blank(void)
{
    return text_blank;
}

int macro_dat_date(void)
{
    int weekday;
    int day;
    int month;
    int day_of_year;
    int suffix_index;

    day_of_year = ((unsigned)(((unsigned)game_minutes) % 518400)) / 1440;
    month = day_of_year / 30;
    day = day_of_year % 30;
    weekday = day_of_year % 7;
    if (day > 3) {
        suffix_index = 3;
    } else {
        suffix_index = day;
    }
    mc_set_location(431, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171134, day_names[weekday], day + 1, ordinal_suffixes[suffix_index], month_names[month]);
    return (int)text_rsc_buffer;
}

int macro_dip_days_in_prison(void)
{
    return itoa((int)(short)court_prison_days, (int)text_rsc_buffer, 10);
}

int macro_dbl_brotherhood_building(void)
{
    int i;

    if ((int)current_location != 0) {
        for (i = 0; current_location->building_count > i; i++) {
            if (current_location->buildings[i].faction_id == 108) {
                return building_name((int)&current_location->buildings[i]);
            }
        }
    }
    return (int)D_00171146;
}

int macro_dbp_codeword(void)
{
    mc_set_location(454, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017114E, codeword_first_words[(player_character->codeword >> 4)], codeword_second_words[((int)(unsigned char)(player_character->codeword & 15))]);
    return (int)text_rsc_buffer;
}

int macro_dtr_days_left(void)
{
    return itoa(((unsigned)((scratch_current_object->repair_due - game_minutes) + 1439)) / 1440, (int)text_rsc_buffer, 10);
}

int macro_da_trade_total(void)
{
    return itoa(trade_total, (int)text_rsc_buffer, 10);
}

int macro_enc_max_encumbrance(void)
{
    return itoa(carry_capacity(), (int)text_rsc_buffer, 10);
}

int macro_end_endurance(void)
{
    return itoa(player_character->attributes[4], (int)text_rsc_buffer, 10);
}

int macro_enf_faction_name(void)
{
    return (int)D_00196728->name;
}

int macro_ef_shop_owner_name(void)
{
    char *name;
    char *space;

    name = (char *)name_generate((int)(unsigned char)D_00196267, 0);
    space = (char *)strchr(name, 32);
    if (space != 0) *space = 0;
    return (int)name;
}

int macro_foc_blank(void)
{
    return text_blank;
}

int macro_fon_building_faction(void)
{
    return faction_find((int)(short)current_building->faction_id) + 3;
}

int macro_fn_female_name(void)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(parse_name_seed);
    name = name_generate((int)(unsigned char)(rand() & 7), 1);
    srand(saved_seed);
    return name;
}

int macro_fn2_female_name2(void)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(parse_name_seed + 123);
    name = name_generate((int)(unsigned char)(rand() & 7), 1);
    srand(saved_seed);
    return name;
}

int macro_fln_faction_child_name(void)
{
    if (D_00196720->child != 0) return (int)D_00196720->child->name;
    return text_blank;
}

int macro_foe_faction_name(void)
{
    return (int)D_00196728->name;
}

int macro_fxn_faction_name(void)
{
    return (int)D_00196720->name;
}

int macro_fac1_key_text(void)
{
    return (int)talk_key_text;
}

int macro_fx1_news_faction1(void)
{
    return (int)D_0019671C->name;
}

int macro_fx2_news_faction2(void)
{
    return (int)D_0019670C->name;
}

int macro_fl1_faction1_leader(void)
{
    int name;
    int saved_seed;

    if (D_0019671C->type == 7 && D_0019671C->child != 0 && D_0019671C->child->type == 4) {
        return (int)D_0019671C->child->name;
    }
    saved_seed = rand();
    srand(D_0019671C->seed & 65535);
    name = name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
    srand(saved_seed);
    return name;
}

int macro_fl2_faction2_leader(void)
{
    int name;
    int saved_seed;

    if (D_0019670C->type == 7 && D_0019670C->child != 0 && D_0019670C->child->type == 4) {
        return (int)D_0019670C->child->name;
    }
    saved_seed = rand();
    saved_seed = rand();
    srand(D_0019671C->seed & 65535);
    name = name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
    srand(saved_seed);
    return name;
}

int macro_fcn_building_town(void)
{
    return (int)text_macro_fcn;
}

int macro_fpc_player_faction(void)
{
    return *(int *)scratch_190de4 + 3;
}

int macro_fnpc_npc_faction(void)
{
    return scratch_190de8 + 3;
}

int macro_fe_shared_enemy(void)
{
    return scratch_190dec + 3;
}

int macro_fa_shared_ally(void)
{
    return *(int *)scratch_190df0 + 3;
}

int macro_fea_player_enemy_npc_ally(void)
{
    return scratch_190df8 + 3;
}

int macro_fpa_shared_faction(void)
{
    return scratch_190dfc + 3;
}

int macro_g_pronoun_he(void)
{
    return pronoun_he[((int)(unsigned char)text_macro_gender)];
}

int macro_g2_pronoun_him(void)
{
    return pronoun_him[((int)(unsigned char)text_macro_gender)];
}

int macro_g3_pronoun_his(void)
{
    return pronoun_his[((int)(unsigned char)text_macro_gender)];
}

int macro_gii_gold_carried(void)
{
    return utoa(player_character->gold, (int)text_rsc_buffer, 10);
}

int macro_gtp_fine(void)
{
    return itoa(scratch_190cac, (int)text_rsc_buffer, 10);
}

int macro_gdd_temple_god(void)
{
    if ((int)guild_membership != 0) return *(int *)(D_0017CB8E + (guild_membership->kind << 2));
    return text_blank;
}

int macro_god_local_god(void)
{
    struct faction *region_temple;
    struct faction *temple;

    if (((int)player_environment) == 2 && current_building->type == 14) {
        temple = (struct faction *)faction_find((int)(short)current_building->faction_id);
        if (temple != 0 && temple->parent != 0) return (int)temple->parent->name;
        if (temple != 0) return (int)temple->name;
    } else {
        region_temple = (struct faction *)faction_find((int)(short)*(short *)(D_0017C912 + (((int)(unsigned char)current_region) << 2)));
        if (region_temple != 0) return (int)region_temple->parent->name;
    }
    return text_blank;
}

int parse_unused_biography_name(void)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(parse_name_seed + 3457);
    name = name_generate(player_character->race, (int)(unsigned char)(*(signed char *)D_00190C78 & 2));
    srand(saved_seed);
    return name;
}

int macro_tim_time(void)
{
    int minute_of_day;
    int hour;
    int minute;

    minute_of_day = ((unsigned)game_minutes) % 1440;
    hour = minute_of_day / 60;
    minute = minute_of_day % 60;
    mc_set_location(728, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171154, hour, minute);
    return (int)text_rsc_buffer;
}

int macro_hea_endurance_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[4] / 10) - 5, (char *)text_rsc_buffer, 10);
}

int macro_hs_held_soul(void)
{
    if (scratch_current_object->children == 0) return (int)D_0017115C;
    if (scratch_current_object->children->image >= 43) return (int)D_00171164;
    return *(int *)(monster_names + (scratch_current_object->children->image << 2));
}

int macro_hod_holiday_description(void)
{
    int holiday;

    holiday = holiday_index(game_minutes, (int)(unsigned char)current_region);
    parse_rsc_text(holiday + 8350, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_hc_blank(void)
{
    return text_blank;
}

int macro_hct_blank(void)
{
    return text_blank;
}

int macro_ht_blank(void)
{
    return text_blank;
}

int macro_hnt_hint(void)
{
    int hint;

    hint = talk_macro_hint(0);
    return hint;
}

int macro_hnt2_hint2(void)
{
    int hint;

    hint = talk_macro_hint(1);
    return hint;
}

int macro_hip_hours_in_prison(void)
{
    return itoa(((int)(short)court_prison_days) * 24, (int)text_rsc_buffer, 10);
}

int macro_hpn_home_province(void)
{
    return province_names[((int)(unsigned char)D_0017CC1F[player_character->race])];
}

int macro_hpw_home_terrain(void)
{
    return province_terrain_names[((int)(unsigned char)D_0017CC1F[player_character->race])];
}

int macro_hrg_house_region(void)
{
    return (int)saved_region_name;
}

int macro_htwn_house_town(void)
{
    return (int)saved_location_name;
}

int macro_hnr_honorific(void)
{
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) return D_0018508F;
    return honorifics;
}

int macro_int_intelligence(void)
{
    return itoa(player_character->attributes[1], (int)text_rsc_buffer, 10);
}

int macro_imp_imperial_name(void)
{
    return imperial_names[((int)(unsigned char)text_macro_imperial)];
}

int macro_itr_item_name_raw(void)
{
    return (int)text_macro_item;
}

int macro_kg_weight(void)
{
    int weight;

    weight = object_weight(scratch_current_object);
    if ((weight & 3) != 0) {
        mc_set_location(847, (int)D_0017110C);
        mc_sprintf((int)text_rsc_buffer, (int)D_0017116D, weight >> 2, weight_fractions[(weight & 3)]);
    } else {
        mc_set_location(849, (int)D_0017110C);
        mc_sprintf((int)text_rsc_buffer, (int)D_00171173, weight >> 2);
    }
    return (int)text_rsc_buffer;
}

int macro_pow_blank(void)
{
    return text_blank;
}

int macro_wth_worth(void)
{
    return itoa(text_macro_item->value, (int)text_rsc_buffer, 10);
}

int macro_jok_joke(void)
{
    parse_rsc_text(200, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_kno_knightly_order(void)
{
    struct membership *membership;

    membership = guild_find_membership_by_bits(64);
    if (membership != 0) return faction_find(membership->faction) + 3;
    if (D_0019671C != 0) return (int)D_0019671C->name;
    return text_blank;
}

int macro_key_topic(void)
{
    return (int)talk_key_text;
}

int macro_key2_topic2(void)
{
    return (int)D_00196661;
}

int macro_lp_local_province(void)
{
    return D_0017D042[((int)(unsigned char)D_00196267)];
}

int macro_luc_luck(void)
{
    return itoa(player_character->attributes[7], (int)text_rsc_buffer, 10);
}

int macro_la_number(void)
{
    return itoa(scratch_190bec, (int)text_rsc_buffer, 10);
}

int macro_lev_guild_rank(void)
{
    if ((int)guild_membership != 0) {
        return *(int *)((char *)(int)(faction_rank_names[((int)(unsigned char)(guild_membership->kind & 63))] + (guild_membership->rank << 2)));
    }
    return macro_pcn_player_name();
}

int macro_lt1_faction1_ruler_title(void)
{
    return parse_faction_ruler_title(D_0019671C);
}

int macro_lt2_faction2_ruler_title(void)
{
    return parse_faction_ruler_title(D_0019670C);
}

int macro_loc_where_building(void)
{
    return building_name((int)D_00195D28->building);
}

int macro_ltn_legal_standing(void)
{
    int reputation;
    int standing;

    reputation = regions[(unsigned char)current_region].legal_reputation;
    standing = 0;
    if (reputation < (-100)) {
        reputation = -100;
    } else if (reputation > 100) {
        reputation = 100;
    }
    while (reputation < *(int *)(legal_reputation_ranges + (standing << 3)) || reputation > *(int *)(D_0017C552 + (standing << 3))) {
        standing++;
    }
    return legal_reputation_names[standing];
}

int macro_mad_magic_resist(void)
{
    return itoa(player_character->attributes[2] / 10, (int)text_rsc_buffer, 10);
}

int macro_mat_material(void)
{
    if (text_macro_item->group == 2 && (text_macro_item->index >= 7 || text_macro_item->index == 5)) {
        return (int)D_00171177;
    }
    if (text_macro_item->group == 2 && text_macro_item->armor_type != 2) {
        return *(int *)(armor_type_names + (text_macro_item->armor_type << 2));
    }
    return *(int *)(material_names + (text_macro_item->material << 2));
}

int macro_mit_item_name_raw(void)
{
    return (int)text_macro_item;
}

int macro_it_item_name(void)
{
    if (text_macro_item->enchantments[0].type == (-1)) return (int)text_macro_item;
    parse_item_name(text_macro_item, (char *)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_mt_blank(void)
{
    return text_blank;
}

int macro_mn_male_name(void)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(parse_name_seed + 3457);
    name = name_generate((int)(unsigned char)(rand() & 7), 0);
    srand(saved_seed);
    return name;
}

int macro_mn2_male_name2(void)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(parse_name_seed + 9543);
    name = name_generate((int)(unsigned char)(rand() & 7), 0);
    srand(saved_seed);
    return name;
}

int macro_mwz_name(void)
{
    int saved_seed;
    int name;

    saved_seed = rand();
    srand(current_region_data->price_adjustment);
    name = name_generate((int)(unsigned char)D_00196267, (int)(unsigned char)(rand() & 1));
    srand(saved_seed);
    return name;
}

int macro_ml_max_loan(void)
{
    return itoa(player_character->level * 50000, (int)text_rsc_buffer, 10);
}

int macro_map_map_location(void)
{
    char *map_location;

    map_location = (char *)text_macro_map_location;
    return *(int *)(map_location + 16);
}

int macro_mpw_magic_powers(void)
{
    if (((int)(unsigned short)(text_macro_item->item_flags & 2048)) != 0) {
        parse_rsc_text(text_macro_item->enchantments[9].param + 8700, 0, 0);
        return (int)text_rsc_buffer;
    }
    return enchant_powers_text(text_macro_item);
}

int macro_nh_holiday_name(void)
{
    return holiday_name(game_minutes, (int)(unsigned char)current_region);
}

int macro_nhd_holiday_date(void)
{
    return calendar_format_date(((int)(short)holiday_days[holiday_index(game_minutes, (int)(unsigned char)current_region)]) * 1440, (int)text_rsc_buffer);
}

int macro_nt_nearby_tavern(void)
{
    return parse_town_building_name(15);
}

int func_00048765(void)
{
    return D_00195AC0 + 28;
}

int macro_n_npc_name(void)
{
    if (text_macro_n_text != 0) {
        parse_rsc_text((int)(unsigned short)text_macro_n_text, 0, 0);
        return (int)text_rsc_buffer;
    }
    if (((struct bf8_2_1 *)&D_001940D7)->f != 0) return name_generate(8, 0);
    if ((int)text_macro_npc != 0) return (int)text_macro_npc;
    return name_generate((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(rand() & 1));
}

int macro_nrn_regional_noble(void)
{
    return macro_fl1_faction1_leader();
}

int macro_fop_faction_name(void)
{
    return (int)D_00196714->name;
}

int macro_olf_old_leader_fate(void)
{
    int saved_seed;
    int fate;

    saved_seed = rand();
    srand(D_0019671C->seed & 65535);
    fate = ruler_fates[rand_range(0, 5)];
    srand(saved_seed);
    return fate;
}

int macro_ol1_old_leader(void)
{
    int name;
    int saved_seed;

    if (D_0019671C->type == 7 && D_0019671C->child != 0 && D_0019671C->child->type == 4) {
        return (int)D_0019671C->child->name;
    }
    saved_seed = rand();
    srand(((unsigned)D_0019671C->seed) >> 16);
    name = name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
    srand(saved_seed);
    return name;
}

int macro_pcn_player_name(void)
{
    return (int)player_character;
}

int macro_pcf_player_first_name(void)
{
    int i;

    i = 0;
    while (player_character->name[i] != 0 && ((int)(unsigned char)player_character->name[i]) != 32) {
        text_rsc_buffer[i] = player_character->name[i];
        i++;
    }
    text_rsc_buffer[i] = 0;
    return (int)text_rsc_buffer;
}

int macro_per_personality(void)
{
    return itoa(player_character->attributes[5], (int)text_rsc_buffer, 10);
}

int macro_po_potion_name(void)
{
    mc_set_location(1157, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171178, D_00195ACC + 67);
    return (int)text_rsc_buffer;
}

int macro_pp1_painting_prefix1(void)
{
    parse_rsc_text((int)(unsigned short)painting_prefix1_text, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_pp2_painting_prefix2(void)
{
    parse_rsc_text((int)(unsigned short)painting_prefix2_text, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_prg_persecuted_temple(void)
{
    struct faction *temple;

    temple = (struct faction *)faction_find(current_region_data->persecuted_temple);
    return (int)temple->name;
}

int macro_pen_penalty(void)
{
    if (((int)(signed char)scratch_190d16) == 2) {
        parse_expand(D_00184269, *(int *)scratch_buffer);
    } else {
        mc_strncpy(*(int *)scratch_buffer, penalty_texts[((int)(signed char)scratch_190d16)], 4, (int)D_0017110C, 1182);
    }
    return *(int *)scratch_buffer;
}

int macro_pdg_more_prison_days(void)
{
    return itoa((int)(short)court_extra_days, (int)text_rsc_buffer, 10);
}

int macro_prn_prison_name(void)
{
    D_001940D7 |= 4;
    parse_rsc_text(8100, 0, 0);
    D_001940D7 &= 251;
    return (int)text_rsc_buffer;
}

int macro_pqn_questor_name(void)
{
    return npc_display_name(quest_potential_questor);
}

int macro_pqp_questor_place(void)
{
    int building;

    building = object_building(quest_potential_questor);
    return building_name(building);
}

int macro_pd_blank(void)
{
    return text_blank;
}

int macro_ph_blank(void)
{
    return text_blank;
}

int macro_plm_blank(void)
{
    return text_blank;
}

int macro_plq_blank(void)
{
    return text_blank;
}

int macro_pn_blank(void)
{
    return text_blank;
}

int macro_prg2_persecuted_temple(void)
{
    return faction_find(regions[D_0019671C->region].persecuted_temple) + 3;
}

int macro_ptm_persecuted_temple(void)
{
    return faction_find(regions[D_0019671C->region].persecuted_temple) + 3;
}

int macro_qua_condition(void)
{
    unsigned char percent;
    unsigned char level;

    level = 0;
    if (text_macro_item->max_condition != 0) {
        percent = (text_macro_item->condition * 100) / text_macro_item->max_condition;
    } else {
        percent = 0;
    }
    while (percent > condition_thresholds[(int)(unsigned char)level]) level++;
    return condition_names[((int)(unsigned char)level)];
}

int macro_q1_bio_answer(void)
{
    return parse_bio_answer_text(0);
}

int macro_q2_bio_answer(void)
{
    return parse_bio_answer_text(1);
}

int macro_q3_bio_answer(void)
{
    return parse_bio_answer_text(2);
}

int macro_q4_bio_answer(void)
{
    return parse_bio_answer_text(3);
}

int macro_q5_bio_answer(void)
{
    return parse_bio_answer_text(4);
}

int macro_q6_bio_answer(void)
{
    return parse_bio_answer_text(5);
}

int macro_q7_bio_answer(void)
{
    return parse_bio_answer_text(6);
}

int macro_q8_bio_answer(void)
{
    return parse_bio_answer_text(7);
}

int macro_q9_bio_answer(void)
{
    return parse_bio_answer_text(8);
}

int macro_q10_bio_answer(void)
{
    return parse_bio_answer_text(9);
}

int macro_q11_bio_answer(void)
{
    return parse_bio_answer_text(10);
}

int macro_q12_bio_answer(void)
{
    return parse_bio_answer_text(11);
}

int macro_q1a_bio_answer(void)
{
    return parse_bio_answer_text(12);
}

int macro_q2a_bio_answer(void)
{
    return parse_bio_answer_text(13);
}

int macro_q3a_bio_answer(void)
{
    return parse_bio_answer_text(14);
}

int macro_q4a_bio_answer(void)
{
    return parse_bio_answer_text(15);
}

int macro_q5a_bio_answer(void)
{
    return parse_bio_answer_text(16);
}

int macro_q6a_bio_answer(void)
{
    return parse_bio_answer_text(17);
}

int macro_q7a_bio_answer(void)
{
    return parse_bio_answer_text(18);
}

int macro_q8a_bio_answer(void)
{
    return parse_bio_answer_text(19);
}

int macro_q9a_bio_answer(void)
{
    return parse_bio_answer_text(20);
}

int macro_q10a_bio_answer(void)
{
    return parse_bio_answer_text(21);
}

int macro_q11a_bio_answer(void)
{
    return parse_bio_answer_text(22);
}

int macro_q12a_bio_answer(void)
{
    return parse_bio_answer_text(23);
}

int macro_q1b_bio_answer(void)
{
    return parse_bio_answer_text(24);
}

int macro_q2b_bio_answer(void)
{
    return parse_bio_answer_text(25);
}

int macro_q3b_bio_answer(void)
{
    return parse_bio_answer_text(26);
}

int macro_q4b_bio_answer(void)
{
    return parse_bio_answer_text(27);
}

int macro_q5b_bio_answer(void)
{
    return parse_bio_answer_text(28);
}

int macro_q6b_bio_answer(void)
{
    return parse_bio_answer_text(29);
}

int macro_q7b_bio_answer(void)
{
    return parse_bio_answer_text(30);
}

int macro_q8b_bio_answer(void)
{
    return parse_bio_answer_text(31);
}

int macro_q9b_bio_answer(void)
{
    return parse_bio_answer_text(32);
}

int macro_q10b_bio_answer(void)
{
    return parse_bio_answer_text(33);
}

int macro_q11b_bio_answer(void)
{
    return parse_bio_answer_text(34);
}

int macro_q12b_bio_answer(void)
{
    return parse_bio_answer_text(35);
}

int macro_qot_blank(void)
{
    return text_blank;
}

int macro_qdt_quest_date(void)
{
    int date;
    int saved_minutes;

    saved_minutes = game_minutes;
    game_minutes = *(int *)scratch_190be4;
    date = macro_dat_date();
    game_minutes = saved_minutes;
    return date;
}

int macro_ra_player_race(void)
{
    if (player_character->race > 7) {
        return *(int *)(race_names + (player_character->original_race << 2));
    }
    return *(int *)(race_names + (player_character->race << 2));
}

int macro_rf_empty(void)
{
    return (int)D_00171177;
}

int macro_rt_ruler_title(void)
{
    struct faction *court;

    court = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 14);
    return parse_faction_ruler_title(court);
}

int parse_faction_ruler_title(struct faction *faction)
{
    if (faction == 0) return D_001837E0;
    if (faction->ruler != 0) return *(int *)(ruler_titles + (faction->ruler << 2));
    if (faction->parent != 0) {
        faction = faction->parent;
        if (faction->ruler != 0) return *(int *)(ruler_titles + (faction->ruler << 2));
        return D_001837E0;
    }
    return D_001837E0;
}

int macro_reg_previous_region(void)
{
    return *(int *)(region_names + (((int)(unsigned char)D_00196269) << 2));
}

int macro_rn_ruler_name(void)
{
    struct faction *faction;

    faction = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 7);
    if (faction->child != 0 && faction->child->type == 4) return (int)faction->child->name;
    faction = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 14);
    return name_generate_seeded((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(faction->region & 1), faction->seed & 65535);
}

int macro_r1_commoners_change(void)
{
    if (player_character->reputation[0] < reputation_baseline) return (int)D_00171185;
    if (player_character->reputation[0] > reputation_baseline) return (int)D_0017118B;
    return (int)D_00171192;
}

int macro_r2_merchants_change(void)
{
    if (player_character->reputation[1] < D_00196D7E) return (int)D_00171185;
    if (player_character->reputation[1] > D_00196D7E) return (int)D_0017118B;
    return (int)D_00171192;
}

int macro_r3_scholars_change(void)
{
    if (player_character->reputation[2] < D_00196D80) return (int)D_00171185;
    if (player_character->reputation[2] > D_00196D80) return (int)D_0017118B;
    return (int)D_00171192;
}

int macro_r4_nobility_change(void)
{
    if (player_character->reputation[3] < D_00196D82) return (int)D_00171185;
    if (player_character->reputation[3] > D_00196D82) return (int)D_0017118B;
    return (int)D_00171192;
}

int macro_r5_underworld_change(void)
{
    if (player_character->reputation[4] < D_00196D84) return (int)D_00171185;
    if (player_character->reputation[4] > D_00196D84) return (int)D_0017118B;
    return (int)D_00171192;
}

int macro_spc_magicka(void)
{
    return itoa(player_character->magicka, (int)text_rsc_buffer, 10);
}

int macro_spt_max_magicka(void)
{
    return itoa(player_character->max_magicka, (int)text_rsc_buffer, 10);
}

int macro_spd_speed(void)
{
    return itoa(player_character->attributes[6], (int)text_rsc_buffer, 10);
}

int macro_str_strength(void)
{
    return itoa(player_character->attributes[0], (int)text_rsc_buffer, 10);
}

int macro_sub_painting_subject(void)
{
    parse_rsc_text((int)(unsigned short)painting_subject_text, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_fac2_key2_text(void)
{
    return (int)D_00196661;
}

int parse_unused_price_text(void)
{
    return itoa(trade_price, (int)text_rsc_buffer, 10);
}

int macro_ski_skill_name(void)
{
    return *(int *)(skill_names + (text_macro_skill << 2));
}

int macro_oth_oath(void)
{
    parse_rsc_text(text_macro_npc->race + 201, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_sng_blank(void)
{
    return text_blank;
}

int macro_reg_where_target_region(void)
{
    return *(int *)(region_names + (D_00195D28->region << 2));
}

int macro_t_ruler_title(void)
{
    return parse_faction_ruler_title(faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 7));
}

int macro_thd_to_hit_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[3] / 10) - 5, (char *)text_rsc_buffer, 10);
}

int macro_tem_town_temple(void)
{
    return parse_town_building_name(14);
}

int func_00049AE7(void)
{
    return (int)faction_find_type_in_region(D_00195D28->region, 13)->child->name;
}

int macro_tcn_travel_city(void)
{
    return text_macro_travel_city;
}

int macro_vam_vampire_clan(void)
{
    return faction_find((int)(short)((unsigned short)player_character->vampire_clan)) + 3;
}

int macro_wil_willpower(void)
{
    return itoa(player_character->attributes[2], (int)text_rsc_buffer, 10);
}

int macro_wdm_weapon_damage(void)
{
    int weapon_index;
    int material_bonus;
    int damage_min;
    int damage_max;

    weapon_index = text_macro_item->index;
    material_bonus = ((int)(short)*(short *)(material_to_hit + (text_macro_item->material * 2))) / 5;
    damage_min = material_bonus + ((int)(short)*(short *)(weapon_damage_min + (weapon_index << 2)));
    damage_max = material_bonus + ((int)(short)*(short *)(weapon_damage_max + (weapon_index << 2)));
    if (damage_min < 0) damage_min = 0;
    if (damage_max < 0) damage_max = 0;
    mc_set_location(1687, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017119C, damage_min, damage_max);
    return (int)text_rsc_buffer;
}

int macro_wep_weapon_name(void)
{
    if (text_macro_item->enchantments[0].type == (-1)) return (int)text_macro_item;
    parse_item_name(text_macro_item, (char *)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_wpn_poison(void)
{
    struct disease *poison;

    if (scratch_current_object->children == 0) return (int)D_0017115C;
    poison = &scratch_current_object->children->data.disease;
    return *(int *)(D_00182F92 + (poison->id << 2));
}

int macro_12m_number(void)
{
    return itoa(automap_yaw, (int)text_rsc_buffer, 10);
}

int macro_12f_number(void)
{
    return itoa(D_00190BF8, (int)text_rsc_buffer, 10);
}

int macro_36m_number(void)
{
    return itoa(D_00190BFC, (int)text_rsc_buffer, 10);
}

int macro_36f_number(void)
{
    return itoa(D_00190C00, (int)text_rsc_buffer, 10);
}

int macro_1hn_hero1_name(void)
{
    if (D_00190CD4 == 0) {
        xn_str_copy_word((int)text_rsc_buffer, (int)player_character);
        return (int)text_rsc_buffer;
    }
    return parse_regional_name(D_00190CD4, (int)(signed char)scratch_190d20);
}

int macro_1g1_hero1_he(void)
{
    return pronoun_he[((int)(signed char)scratch_190d20)];
}

int macro_1g2_hero1_him(void)
{
    return pronoun_him[((int)(signed char)scratch_190d20)];
}

int macro_1g3_hero1_his(void)
{
    return pronoun_his[((int)(signed char)scratch_190d20)];
}

int macro_2hn_hero2_name(void)
{
    return parse_regional_name(D_00190CD8, (int)(signed char)D_00190D21);
}

int macro_2g1_hero2_he(void)
{
    return pronoun_he[((int)(signed char)D_00190D21)];
}

int macro_2g2_hero2_him(void)
{
    return pronoun_him[((int)(signed char)D_00190D21)];
}

int macro_2g3_hero2_his(void)
{
    return pronoun_his[((int)(signed char)D_00190D21)];
}

int macro_3hn_hero3_name(void)
{
    return parse_regional_name(D_00190CDC, (int)(signed char)D_00190D22);
}

int macro_3g1_hero3_he(void)
{
    return pronoun_he[((int)(signed char)D_00190D22)];
}

int macro_3g2_hero3_him(void)
{
    return pronoun_him[((int)(signed char)D_00190D22)];
}

int macro_3g3_hero3_his(void)
{
    return pronoun_his[((int)(signed char)D_00190D22)];
}

int macro_1com_greeting(void)
{
    return talk_macro_1com();
}

int parse_bio_answer_text(int answer_index)
{
    parse_rsc_text((int)(short)*(short *)(scratch_190be4 + (answer_index << 2)), 0, 0);
    return (int)text_rsc_buffer;
}

int parse_read_number(signed char *text)
{
    signed char *src;
    signed char *dst;
    int length;
    {
        char digits[40];

        src = text;
        dst = (signed char *)digits;
        length = 0;
        while (*src != 0 && ((int)(unsigned char)(D_00178630[(int)(unsigned char)(*src + 1)] & 32)) != 0) {
            *dst = *src;
            length++;
            src++;
            dst++;
        }
        *dst = 0;
        parse_number = atoi((int)digits);
        return length;
    }
}

int parse_signed_itoa(int value, char *buffer, int radix)
{
    if (value > 0) {
        *buffer = 43;
        itoa(value, buffer + 1, radix);
    } else {
        itoa(value, buffer, radix);
    }
    return (int)buffer;
}

void object_weight_add(struct record *object)
{
    struct record *ancestor;
    struct item *item;
    int i;

    if (object->type != 2) return;
    item = &object->data.item;
    if (item->group == 23) return;
    ancestor = object->parent;
    while (ancestor != 0 && xn_str_find_u32((int)((char *)&house_container), (int)ancestor, 4) == 0) {
        ancestor = ancestor->parent;
    }
    if (ancestor != 0) return;
    if (D_001962AE == 0 && object->parent == wagon_container) return;
    if (item->group == 28 && item->index == 0) {
        weight_total += ((unsigned)item->value) / 100;
        return;
    }
    if (item->enchantments[0].type != (-1)) {
        for (i = 0; i < 10; i++) {
            switch ((unsigned short)item->enchantments[i].type) {
            case 11:
                weight_total++;
                return;
            case 23:
                i = item->weight << 2;
                if (i < 20) i = 20;
                weight_total += i;
                return;
            }
        }
    }
    weight_total += item->weight;
}

int object_weight(struct record *object)
{
    struct item *item;
    struct character *creature;

    weight_total = 0;
    object_foreach(object->children, object_weight_add);
    if (object == player_entity) {
        return (int)(((char *)weight_total) + (((unsigned)object->data.character.gold) / 100));
    }
    if (object->type == 2) {
        item = &object->data.item;
        if (item->group == 3 && item->index == 18) {
            return (int)(((char *)weight_total) + (item->stack_count * item->weight));
        }
        if (item->group != 23) {
            object_weight_add(object);
            return weight_total;
        }
        return weight_total;
    }
    if (object->type == 18) {
        creature = &object->data.character;
        if (creature->race > 43) {
            if (((int)(unsigned short)(creature->flags & 1)) != 0) return weight_total + 240;
            return weight_total + 350;
        }
        return (int)(((char *)weight_total) + ((int)(short)*(short *)(monster_weights + (creature->race * 2))));
    }
    return weight_total;
}

void parse_item_name(struct item *item, char *out)
{
    int i;

    i = 0;
    if (item->enchantments[0].type == 26) {
        mc_strncpy((int)text_rsc_buffer, item->name, 2048, (int)D_0017110C, 1967);
        return;
    }
    if (((int)(unsigned short)(item->item_flags & 32)) == 0) {
        mc_strncpy((int)text_rsc_buffer, (int)item_templates[((int)(short)*(short *)((char *)(int)(*(char **)(item_group_templates + (item->group << 2)) + (item->index * 2))))].name, 2048, (int)D_0017110C, 1973);
        return;
    }
    while (item->name[i] != 0) {
        if (((int)(unsigned char)item->name[i]) == 37) {
            mc_strncpy(out, (int)item_templates[((int)(short)*(short *)((char *)(int)(*(char **)(item_group_templates + (item->group << 2)) + (item->index * 2))))].name, 4, (int)D_0017110C, 1981);
            out += strlen(out);
            i += 3;
        } else {
            *out++ = item->name[i];
            i++;
        }
    }
    *out = 0;
}

void parse_rsc_text(int id, int flags, int width)
{
    char *text;
    short saved_file;

    saved_file = *(short *)text_rsc_file;
    *(int *)text_rsc_file = text_rsc_main_file;
    text = (char *)text_rsc_load((int)(short)*(short *)&id, (int)(short)*(short *)&flags, (int)(short)*(short *)&width);
    mc_strncpy((int)text_rsc_buffer, text, 2048, (int)D_0017110C, 2003);
    if (text != 0 && text != (char *)0x97979797) {
        mc_free(text, (int)D_0017110C, 2004);
        text = (char *)0x97979797;
    }
    *(int *)text_rsc_file = (int)(short)saved_file;
}

void parse_rsc_text_copy(int id, char *out)
{
    char *text;
    short saved_file;

    saved_file = *(short *)text_rsc_file;
    *(int *)text_rsc_file = text_rsc_main_file;
    text = (char *)text_rsc_load((int)(short)*(short *)&id, 0, 0);
    mc_strncpy(out, text, 4, (int)D_0017110C, 2034);
    if (text != 0 && text != (char *)0x97979797) {
        mc_free(text, (int)D_0017110C, 2035);
        text = (char *)0x97979797;
    }
    *(int *)text_rsc_file = (int)(short)saved_file;
}

int parse_stub_zero(int unused)
{
    return 0;
}

int holiday_index(int minutes, int region)
{
    short day;
    short holiday;

    *(int *)&day = (((unsigned)(((unsigned)minutes) % 518400)) / 1440) + 1;
    if (((int)(short)day) > 355) return 0;
    *(int *)&holiday = 0;
    region++;
    while (((int)(unsigned char)holiday_regions[(int)(short)holiday]) != 255) {
        if (((int)(unsigned char)holiday_regions[(int)(short)holiday]) == region && (short)(short)*(int *)&day >= holiday_days[((int)(short)holiday)]) {
            return (int)(short)holiday;
        }
        (*(int *)&holiday)++;
    }
    return 0;
}

int holiday_today(int minutes, int region)
{
    short day;
    short holiday;

    *(int *)&holiday = 0;
    region++;
    *(int *)&day = (((unsigned)(((unsigned)minutes) % 518400)) / 1440) + 1;
    if (((int)(short)day) > 355) return 0;
    while (((int)(short)holiday) < 53) {
        if ((((int)(unsigned char)holiday_regions[(int)(short)holiday]) == 255 || ((int)(unsigned char)holiday_regions[(int)(short)holiday]) == region) && (short)*(int *)&day == holiday_days[((int)(short)holiday)]) {
            return ((int)(short)holiday) + 1;
        }
        (*(int *)&holiday)++;
    }
    return 0;
}

int holiday_name(int minutes, int region)
{
    return holiday_names[holiday_index(minutes, region)];
}
