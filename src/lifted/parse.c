/* parse.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
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
extern char D_0017C54E[];
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
extern char *D_0017CEF2[];
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
extern char item_templates[];
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
extern char D_00185F88[];
extern char saved_location_name[];
extern char saved_region_name[];
extern char region_legal_reputation[];
extern char D_0018F090[];
extern char D_00190BE4[];
extern int D_00190BEC;
extern int automap_yaw;
extern int D_00190BF8;
extern int D_00190BFC;
extern int D_00190C00;
extern char D_00190C78[];
extern int D_00190CAC;
extern int D_00190CD4;
extern int D_00190CD8;
extern int D_00190CDC;
extern signed char D_00190D16;
extern signed char D_00190D17;
extern signed char D_00190D20;
extern signed char D_00190D21;
extern signed char D_00190D22;
extern short court_prison_days;
extern short D_00190DCA;
extern char text_macro_fpc[];
extern int text_macro_fnpc;
extern int text_macro_fe;
extern char text_macro_fa[];
extern int text_macro_fea;
extern int text_macro_fpa;
extern int D_00190EAC;
extern signed char text_rsc_buffer[];
extern unsigned char D_001940D7;
extern char text_macro_fcn[];
extern struct record *wagon_container;
extern struct record *D_001959EC;
extern struct item *D_00195A80;
extern struct character *D_00195A84;
extern int D_00195A90;
extern int D_00195A94;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern int D_00195AC0;
extern struct record *D_00195AC4;
extern int D_00195ACC;
extern int text_macro_book;
extern struct building *tavern_building;
extern char current_region_data[];
extern int weight_total;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern char D_00195C44[];
extern char D_00195D28[];
extern int D_00195D2C;
extern int D_00195D30;
extern int quest_potential_questor;
extern char D_00195D6C[];
extern int D_00195D78;
extern int D_00195D94;
extern int D_00195DB0;
extern short painting_subject_text;
extern short painting_adjective_text;
extern short painting_prefix1_text;
extern short painting_prefix2_text;
extern short spell_effect_slot;
extern short text_macro_n_text;
extern signed char D_00196266;
extern signed char D_00196267;
extern signed char current_region;
extern signed char D_00196269;
extern signed char D_0019626C;
extern signed char D_0019628F;
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
extern int D_00199640;
extern int parse_name_seed;
extern int D_00199734;
extern struct membership *guild_membership;

extern int talk_macro_hint(int);
extern int talk_macro_1com(void);
extern struct faction *faction_find_type_in_region(int, short);
extern int faction_find(short);
extern int text_rsc_load(int, int, int);
extern int func_0004A07F(int, signed char);
extern int parse_town_building_name(short);
extern int calendar_format_date(int, int);
extern int func_0004CF37(int);
extern int flats_cfg_find(int);
extern int enchant_powers_text(int);
extern struct membership *guild_find_membership_by_bits(unsigned char);
extern int rand_range(int, int);
extern int object_building(int);
extern int carry_capacity(void);
extern int func_0008B43B(unsigned char, unsigned char, int);
extern int func_0008B48B(int);
extern int name_generate(unsigned char, unsigned char);
extern int building_name(int);
extern int rand();
extern int srand();
extern int mc_free();
extern int mc_strncpy();
extern int atoi();
extern int utoa();
extern int func_000A0DD9();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int strchr();
extern int func_000C808D();
extern int func_000CE3E3();
extern int func_000CE44C();
extern void parse_expand(int, int);
extern void object_foreach(struct record *, int);
int macro_dat_date(void);
int macro_fl1_faction1_leader(void);
int macro_pcn_player_name(void);
int parse_faction_ruler_title(struct faction *);
int parse_bio_answer_text(int);
int parse_signed_itoa(int, int, int);
int object_weight(struct record *);
int parse_stub_zero(int);
int func_0004A8DB(int, int);
int holiday_name(int, int);
void object_weight_add(struct record *);
void parse_item_name(struct item *, int);
void parse_rsc_text(int, int, int);
#pragma aux func_000A0ED9 parm routine [];

int func_000466C6(void)
{
    return (int)D_00196724->name;
}

int macro_a_price(void)
{
    return func_000A0DD9(D_00195D30, (int)text_rsc_buffer, 10);
}

int macro_agi_agility(void)
{
    return func_000A0DD9(player_character->attributes[3], (int)text_rsc_buffer, 10);
}

int func_00046754(void)
{
    return D_0017D042[((int)(unsigned char)(D_00196267 ^ 1))];
}

int macro_arm_item_name(void)
{
    if (D_00195A80->enchantments[0].type == (-1)) return (int)D_00195A80;
    parse_item_name(D_00195A80, (int)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_ach_chance_per_level(void)
{
    return func_000A0DD9((int)selected_spell->chances[(int)(short)spell_effect_slot].plus, (int)text_rsc_buffer, 10);
}

int macro_adr_duration_per_level(void)
{
    return func_000A0DD9((int)selected_spell->durations[(int)(short)spell_effect_slot].plus, (int)text_rsc_buffer, 10);
}

int macro_1am_magnitude_per_level_min(void)
{
    return func_000A0DD9((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].plus_min, (int)text_rsc_buffer, 10);
}

int macro_2am_magnitude_per_level_max(void)
{
    return func_000A0DD9((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].plus_max, (int)text_rsc_buffer, 10);
}

int macro_ark_attribute_rating(void)
{
    short l_18;

    l_18 = player_character->attributes[(int)(unsigned char)D_0019626C] / 10;
    if (((int)(short)l_18) >= 10) l_18 = 9;
    return attribute_rating_names[((((int)(unsigned char)D_0019626C) * 10) + ((int)(short)l_18))];
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

int func_000469B4(void)
{
    return (int)D_00196728->name;
}

int func_000469DA(void)
{
    return (int)D_00196728->name;
}

int macro_mod_armor_modifier(void)
{
    int l_1C;

    if (D_00195A80->index >= 7) {
        return parse_signed_itoa(D_00195A80->index - 6, (int)text_rsc_buffer, 10);
    }
    return parse_signed_itoa(func_0004CF37((int)D_00195A80) / 10, (int)text_rsc_buffer, 10);
}

int func_00046A7F(void)
{
    int l_24;
    int l_20;
    struct record *l_1C;

    l_1C = (struct record *)parse_stub_zero(511);
    l_24 = rand();
    srand((l_1C->id & 65535) ^ (((unsigned)l_1C->id) >> 16));
    l_20 = name_generate((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(*(signed char *)((char *)flats_cfg_find(l_1C->image) + 6) & 1));
    srand(l_24);
    return l_20;
}

int macro_bch_base_chance(void)
{
    return func_000A0DD9((int)selected_spell->chances[(int)(short)spell_effect_slot].base, (int)text_rsc_buffer, 10);
}

int macro_bdr_base_duration(void)
{
    return func_000A0DD9((int)selected_spell->durations[(int)(short)spell_effect_slot].base, (int)text_rsc_buffer, 10);
}

int macro_1bm_base_magnitude_min(void)
{
    return func_000A0DD9((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].base_min, (int)text_rsc_buffer, 10);
}

int macro_2bm_base_magnitude_max(void)
{
    return func_000A0DD9((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].base_max, (int)text_rsc_buffer, 10);
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
    if (D_00195D94 != 0) return D_00195D94;
    if (D_00195AC4->image != 65535) return (int)current_location;
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
    return func_000A0DD9((int)selected_spell->chances[(int)(short)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_cld_duration_levels(void)
{
    return func_000A0DD9((int)selected_spell->durations[(int)(short)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_clm_magnitude_levels(void)
{
    return func_000A0DD9((int)selected_spell->magnitudes[(int)(short)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int func_00046E42(void)
{
    return func_000A0DD9(((unsigned)D_00195A80->value) / 10, (int)text_rsc_buffer, 10);
}

int func_00046E82(void)
{
    return (int)D_00196728->name;
}

int func_00046EA8(void)
{
    return (int)D_00196720->name;
}

int macro_cri_crime(void)
{
    return crime_names[((int)(signed char)D_00190D17)];
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

int func_00046F78(void)
{
    int l_24;
    int l_20;
    struct record *l_1C;

    l_1C = (struct record *)parse_stub_zero(515);
    l_24 = rand();
    srand((l_1C->id & 65535) ^ (((unsigned)l_1C->id) >> 16));
    l_20 = name_generate((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(*(signed char *)((char *)flats_cfg_find(l_1C->image) + 6) & 1));
    srand(l_24);
    return l_20;
}

int macro_dam_damage_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[0] / 10) - 5, (int)text_rsc_buffer, 10);
}

int macro_dwr_room_hours_left(void)
{
    return func_000A0DD9(((unsigned)(tavern_building->rent_expires - game_minutes)) / 60, (int)text_rsc_buffer, 10);
}

int func_00047094(void)
{
    int l_1C;

    l_1C = rand() & 7;
    while ((player_character->min_metal_to_hit >> 5) == l_1C) l_1C = rand() & 7;
    return province_names[l_1C];
}

int macro_di_direction(void)
{
    return direction_names[((((func_000C808D(player_object->x, player_object->z, D_00195A90, D_00195A94) >> 2) + 32) & 511) >> 6)];
}

int macro_du_blank(void)
{
    return text_blank;
}

int macro_dat_date(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_20 = ((unsigned)(((unsigned)game_minutes) % 518400)) / 1440;
    l_24 = l_20 / 30;
    l_28 = l_20 % 30;
    l_2C = l_20 % 7;
    if (l_28 > 3) {
        l_1C = 3;
    } else {
        l_1C = l_28;
    }
    func_000A0ED9(431, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171134, day_names[l_2C], l_28 + 1, ordinal_suffixes[l_1C], month_names[l_24]);
    return (int)text_rsc_buffer;
}

int macro_dip_days_in_prison(void)
{
    return func_000A0DD9((int)(short)court_prison_days, (int)text_rsc_buffer, 10);
}

int func_00047271(void)
{
    int l_1C;

    if ((int)current_location != 0) {
        for (l_1C = 0; current_location->building_count > l_1C; l_1C++) {
            if (((int)(unsigned short)*(short *)((char *)(int)((l_1C * 26) + (char *)current_location->buildings) + 18)) == 108) {
                return building_name((int)((char *)current_location->buildings + (l_1C * 26)));
            }
        }
    }
    return (int)D_00171146;
}

int macro_dbp_codeword(void)
{
    func_000A0ED9(454, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017114E, codeword_first_words[(player_character->codeword >> 4)], codeword_second_words[((int)(unsigned char)(player_character->codeword & 15))]);
    return (int)text_rsc_buffer;
}

int func_00047373(void)
{
    return func_000A0DD9(((unsigned)((D_00195AA8->repair_due - game_minutes) + 1439)) / 1440, (int)text_rsc_buffer, 10);
}

int func_000473BE(void)
{
    return func_000A0DD9(D_00195D2C, (int)text_rsc_buffer, 10);
}

int macro_enc_max_encumbrance(void)
{
    return func_000A0DD9(carry_capacity(), (int)text_rsc_buffer, 10);
}

int macro_end_endurance(void)
{
    return func_000A0DD9(player_character->attributes[4], (int)text_rsc_buffer, 10);
}

int func_00047458(void)
{
    return (int)D_00196728->name;
}

int macro_ef_shop_owner_name(void)
{
    int l_20;
    int l_1C;

    l_20 = name_generate((int)(unsigned char)D_00196267, 0);
    l_1C = strchr(l_20, 32);
    if (l_1C != 0) *(signed char *)((char *)l_1C) = 0;
    return l_20;
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
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(parse_name_seed);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 1);
    srand(l_20);
    return l_1C;
}

int macro_fn2_female_name2(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(parse_name_seed + 123);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 1);
    srand(l_20);
    return l_1C;
}

int func_000475C9(void)
{
    if (D_00196720->child != 0) return (int)D_00196720->child->name;
    return text_blank;
}

int func_00047607(void)
{
    return (int)D_00196728->name;
}

int func_0004762D(void)
{
    return (int)D_00196720->name;
}

int func_00047653(void)
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
    int l_20;
    int l_1C;

    if (D_0019671C->type == 7 && D_0019671C->child != 0 && D_0019671C->child->type == 4) {
        return (int)D_0019671C->child->name;
    }
    l_1C = rand();
    srand(D_0019671C->seed & 65535);
    l_20 = name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
    srand(l_1C);
    return l_20;
}

int macro_fl2_faction2_leader(void)
{
    int l_20;
    int l_1C;

    if (D_0019670C->type == 7 && D_0019670C->child != 0 && D_0019670C->child->type == 4) {
        return (int)D_0019670C->child->name;
    }
    l_1C = rand();
    l_1C = rand();
    srand(D_0019671C->seed & 65535);
    l_20 = name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
    srand(l_1C);
    return l_20;
}

int macro_fcn_building_town(void)
{
    return (int)text_macro_fcn;
}

int macro_fpc_player_faction(void)
{
    return *(int *)text_macro_fpc + 3;
}

int macro_fnpc_npc_faction(void)
{
    return text_macro_fnpc + 3;
}

int macro_fe_shared_enemy(void)
{
    return text_macro_fe + 3;
}

int macro_fa_shared_ally(void)
{
    return *(int *)text_macro_fa + 3;
}

int macro_fea_player_enemy_npc_ally(void)
{
    return text_macro_fea + 3;
}

int macro_fpa_shared_faction(void)
{
    return text_macro_fpa + 3;
}

int macro_g_pronoun_he(void)
{
    return pronoun_he[((int)(unsigned char)D_0019628F)];
}

int macro_g2_pronoun_him(void)
{
    return pronoun_him[((int)(unsigned char)D_0019628F)];
}

int macro_g3_pronoun_his(void)
{
    return pronoun_his[((int)(unsigned char)D_0019628F)];
}

int macro_gii_gold_carried(void)
{
    return utoa(player_character->gold, (int)text_rsc_buffer, 10);
}

int macro_gtp_fine(void)
{
    return func_000A0DD9(D_00190CAC, (int)text_rsc_buffer, 10);
}

int macro_gdd_temple_god(void)
{
    if ((int)guild_membership != 0) return *(int *)(D_0017CB8E + (guild_membership->kind << 2));
    return text_blank;
}

int macro_god_local_god(void)
{
    struct faction *l_20;
    struct faction *l_1C;

    if (((int)player_environment) == 2 && current_building->type == 14) {
        l_1C = (struct faction *)faction_find((int)(short)current_building->faction_id);
        if (l_1C != 0 && l_1C->parent != 0) return (int)l_1C->parent->name;
        if (l_1C != 0) return (int)l_1C->name;
    } else {
        l_20 = (struct faction *)faction_find((int)(short)*(short *)(D_0017C912 + (((int)(unsigned char)current_region) << 2)));
        if (l_20 != 0) return (int)l_20->parent->name;
    }
    return text_blank;
}

int func_00047B2F(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(parse_name_seed + 3457);
    l_1C = name_generate(player_character->race, (int)(unsigned char)(*(signed char *)D_00190C78 & 2));
    srand(l_20);
    return l_1C;
}

int macro_tim_time(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = ((unsigned)game_minutes) % 1440;
    l_20 = l_24 / 60;
    l_1C = l_24 % 60;
    func_000A0ED9(728, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171154, l_20, l_1C);
    return (int)text_rsc_buffer;
}

int macro_hea_endurance_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[4] / 10) - 5, (int)text_rsc_buffer, 10);
}

int macro_hs_held_soul(void)
{
    if (D_00195AA8->children == 0) return (int)D_0017115C;
    if (D_00195AA8->children->image >= 43) return (int)D_00171164;
    return *(int *)(monster_names + (D_00195AA8->children->image << 2));
}

int macro_hod_holiday_description(void)
{
    int l_1C;

    l_1C = func_0004A8DB(game_minutes, (int)(unsigned char)current_region);
    parse_rsc_text(l_1C + 8350, 0, 0);
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
    int l_1C;

    l_1C = talk_macro_hint(0);
    return l_1C;
}

int macro_hnt2_hint2(void)
{
    int l_1C;

    l_1C = talk_macro_hint(1);
    return l_1C;
}

int func_00047DD1(void)
{
    return func_000A0DD9(((int)(short)court_prison_days) * 24, (int)text_rsc_buffer, 10);
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
    return func_000A0DD9(player_character->attributes[1], (int)text_rsc_buffer, 10);
}

int macro_imp_imperial_name(void)
{
    return imperial_names[((int)(unsigned char)D_00196266)];
}

int func_00047F72(void)
{
    return (int)D_00195A80;
}

int macro_kg_weight(void)
{
    int l_1C;

    l_1C = object_weight(D_00195AA8);
    if ((l_1C & 3) != 0) {
        func_000A0ED9(847, (int)D_0017110C);
        mc_sprintf((int)text_rsc_buffer, (int)D_0017116D, l_1C >> 2, weight_fractions[(l_1C & 3)]);
    } else {
        func_000A0ED9(849, (int)D_0017110C);
        mc_sprintf((int)text_rsc_buffer, (int)D_00171173, l_1C >> 2);
    }
    return (int)text_rsc_buffer;
}

int macro_pow_blank(void)
{
    return text_blank;
}

int macro_wth_worth(void)
{
    return func_000A0DD9(D_00195A80->value, (int)text_rsc_buffer, 10);
}

int macro_jok_joke(void)
{
    parse_rsc_text(200, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_kno_knightly_order(void)
{
    struct membership *l_1C;

    l_1C = guild_find_membership_by_bits(64);
    if (l_1C != 0) return faction_find(l_1C->faction) + 3;
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

int func_00048160(void)
{
    return D_0017D042[((int)(unsigned char)D_00196267)];
}

int macro_luc_luck(void)
{
    return func_000A0DD9(player_character->attributes[7], (int)text_rsc_buffer, 10);
}

int func_000481C4(void)
{
    return func_000A0DD9(D_00190BEC, (int)text_rsc_buffer, 10);
}

int macro_lev_guild_rank(void)
{
    if ((int)guild_membership != 0) {
        return *(int *)((char *)(int)(D_0017CEF2[((int)(unsigned char)(guild_membership->kind & 63))] + (guild_membership->rank << 2)));
    }
    return macro_pcn_player_name();
}

int macro_lt1_faction1_ruler_title(void)
{
    return parse_faction_ruler_title(D_0019671C);
}

int func_0004827C(void)
{
    return parse_faction_ruler_title(D_0019670C);
}

int macro_loc_where_building(void)
{
    return building_name(*(int *)(*(char **)D_00195D28 + 18));
}

int macro_ltn_legal_standing(void)
{
    int l_20;
    int l_1C;

    l_20 = (int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)current_region) * 80));
    l_1C = 0;
    if (l_20 < (-100)) {
        l_20 = -100;
    } else if (l_20 > 100) {
        l_20 = 100;
    }
    while (l_20 < *(int *)(D_0017C54E + (l_1C << 3)) || l_20 > *(int *)(D_0017C552 + (l_1C << 3))) {
        l_1C++;
    }
    return legal_reputation_names[l_1C];
}

int macro_mad_magic_resist(void)
{
    return func_000A0DD9(player_character->attributes[2] / 10, (int)text_rsc_buffer, 10);
}

int macro_mat_material(void)
{
    if (D_00195A80->group == 2 && (D_00195A80->index >= 7 || D_00195A80->index == 5)) {
        return (int)D_00171177;
    }
    if (D_00195A80->group == 2 && D_00195A80->armor_type != 2) {
        return *(int *)(armor_type_names + (D_00195A80->armor_type << 2));
    }
    return *(int *)(material_names + (D_00195A80->material << 2));
}

int func_0004845B(void)
{
    return (int)D_00195A80;
}

int macro_it_item_name(void)
{
    if (D_00195A80->enchantments[0].type == (-1)) return (int)D_00195A80;
    parse_item_name(D_00195A80, (int)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_mt_blank(void)
{
    return text_blank;
}

int macro_mn_male_name(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(parse_name_seed + 3457);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 0);
    srand(l_20);
    return l_1C;
}

int macro_mn2_male_name2(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(parse_name_seed + 9543);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 0);
    srand(l_20);
    return l_1C;
}

int func_00048596(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand((int)(unsigned short)*(short *)(*(char **)current_region_data + 78));
    l_1C = name_generate((int)(unsigned char)D_00196267, (int)(unsigned char)(rand() & 1));
    srand(l_20);
    return l_1C;
}

int macro_ml_max_loan(void)
{
    return func_000A0DD9(player_character->level * 50000, (int)text_rsc_buffer, 10);
}

int macro_map_map_location(void)
{
    int l_1C;

    l_1C = D_00190EAC;
    return *(int *)((char *)l_1C + 16);
}

int macro_mpw_magic_powers(void)
{
    if (((int)(unsigned short)(D_00195A80->item_flags & 2048)) != 0) {
        parse_rsc_text(D_00195A80->enchantments[9].param + 8700, 0, 0);
        return (int)text_rsc_buffer;
    }
    return enchant_powers_text((int)D_00195A80);
}

int macro_nh_holiday_name(void)
{
    return holiday_name(game_minutes, (int)(unsigned char)current_region);
}

int macro_nhd_holiday_date(void)
{
    return calendar_format_date(((int)(short)holiday_days[func_0004A8DB(game_minutes, (int)(unsigned char)current_region)]) * 1440, (int)text_rsc_buffer);
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
    if ((int)D_00195A84 != 0) return (int)D_00195A84;
    return name_generate((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(rand() & 1));
}

int macro_nrn_regional_noble(void)
{
    return macro_fl1_faction1_leader();
}

int func_0004883F(void)
{
    return (int)D_00196714->name;
}

int macro_olf_old_leader_fate(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(D_0019671C->seed & 65535);
    l_1C = ruler_fates[rand_range(0, 5)];
    srand(l_20);
    return l_1C;
}

int macro_ol1_old_leader(void)
{
    int l_20;
    int l_1C;

    if (D_0019671C->type == 7 && D_0019671C->child != 0 && D_0019671C->child->type == 4) {
        return (int)D_0019671C->child->name;
    }
    l_1C = rand();
    srand(((unsigned)D_0019671C->seed) >> 16);
    l_20 = name_generate((int)(unsigned char)(rand() & 7), (int)(unsigned char)(rand() & 1));
    srand(l_1C);
    return l_20;
}

int macro_pcn_player_name(void)
{
    return (int)player_character;
}

int macro_pcf_player_first_name(void)
{
    int l_1C;

    l_1C = 0;
    while (player_character->name[l_1C] != 0 && ((int)(unsigned char)player_character->name[l_1C]) != 32) {
        text_rsc_buffer[l_1C] = player_character->name[l_1C];
        l_1C++;
    }
    text_rsc_buffer[l_1C] = 0;
    return (int)text_rsc_buffer;
}

int macro_per_personality(void)
{
    return func_000A0DD9(player_character->attributes[5], (int)text_rsc_buffer, 10);
}

int macro_po_potion_name(void)
{
    func_000A0ED9(1157, (int)D_0017110C);
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

int func_00048AE3(void)
{
    int l_1C;

    l_1C = faction_find((int)(short)*(short *)(*(char **)current_region_data + 76));
    return l_1C + 3;
}

int macro_pen_penalty(void)
{
    if (((int)(signed char)D_00190D16) == 2) {
        parse_expand(D_00184269, *(int *)D_00195C44);
    } else {
        mc_strncpy(*(int *)D_00195C44, penalty_texts[((int)(signed char)D_00190D16)], 4, (int)D_0017110C, 1182);
    }
    return *(int *)D_00195C44;
}

int macro_pdg_more_prison_days(void)
{
    return func_000A0DD9((int)(short)D_00190DCA, (int)text_rsc_buffer, 10);
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
    return func_0008B48B(quest_potential_questor);
}

int macro_pqp_questor_place(void)
{
    int l_1C;

    l_1C = object_building(quest_potential_questor);
    return building_name(l_1C);
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

int func_00048D21(void)
{
    return faction_find((int)(short)*(short *)(D_0018F090 + (D_0019671C->region * 80))) + 3;
}

int macro_ptm_region_enemy_faction(void)
{
    return faction_find((int)(short)*(short *)(D_0018F090 + (D_0019671C->region * 80))) + 3;
}

int macro_qua_condition(void)
{
    unsigned char l_1C;
    unsigned char l_18;

    l_18 = 0;
    if (D_00195A80->max_condition != 0) {
        l_1C = (D_00195A80->condition * 100) / D_00195A80->max_condition;
    } else {
        l_1C = 0;
    }
    while (l_1C > condition_thresholds[(int)(unsigned char)l_18]) l_18++;
    return condition_names[((int)(unsigned char)l_18)];
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
    int l_20;
    int l_1C;

    l_1C = game_minutes;
    game_minutes = *(int *)D_00190BE4;
    l_20 = macro_dat_date();
    game_minutes = l_1C;
    return l_20;
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
    struct faction *l_1C;

    l_1C = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 14);
    return parse_faction_ruler_title(l_1C);
}

int parse_faction_ruler_title(struct faction *a1)
{
    if (a1 == 0) return D_001837E0;
    if (a1->ruler != 0) return *(int *)(ruler_titles + (a1->ruler << 2));
    if (a1->parent != 0) {
        a1 = a1->parent;
        if (a1->ruler != 0) return *(int *)(ruler_titles + (a1->ruler << 2));
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
    struct faction *l_1C;

    l_1C = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 7);
    if (l_1C->child != 0 && l_1C->child->type == 4) return (int)l_1C->child->name;
    l_1C = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 14);
    return func_0008B43B((int)(unsigned char)D_001841E3[(int)(unsigned char)current_region], (int)(unsigned char)(l_1C->region & 1), l_1C->seed & 65535);
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
    return func_000A0DD9(player_character->magicka, (int)text_rsc_buffer, 10);
}

int macro_spt_max_magicka(void)
{
    return func_000A0DD9(player_character->max_magicka, (int)text_rsc_buffer, 10);
}

int macro_spd_speed(void)
{
    return func_000A0DD9(player_character->attributes[6], (int)text_rsc_buffer, 10);
}

int macro_str_strength(void)
{
    return func_000A0DD9(player_character->attributes[0], (int)text_rsc_buffer, 10);
}

int macro_sub_painting_subject(void)
{
    parse_rsc_text((int)(unsigned short)painting_subject_text, 0, 0);
    return (int)text_rsc_buffer;
}

int func_0004992E(void)
{
    return (int)D_00196661;
}

int func_00049950(void)
{
    return func_000A0DD9(D_00195D30, (int)text_rsc_buffer, 10);
}

int macro_ski_skill_name(void)
{
    return *(int *)(skill_names + (D_00199640 << 2));
}

int macro_oth_oath(void)
{
    parse_rsc_text(D_00195A84->race + 201, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_sng_blank(void)
{
    return text_blank;
}

int macro_reg_where_target_region(void)
{
    return *(int *)(region_names + (((int)(unsigned char)*(signed char *)(*(char **)D_00195D28 + 7)) << 2));
}

int macro_t_ruler_title(void)
{
    return parse_faction_ruler_title(faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 7));
}

int macro_thd_to_hit_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[3] / 10) - 5, (int)text_rsc_buffer, 10);
}

int macro_tem_town_temple(void)
{
    return parse_town_building_name(14);
}

int func_00049AE7(void)
{
    return (int)faction_find_type_in_region((int)(short)((unsigned short)(unsigned char)*(signed char *)(*(char **)D_00195D28 + 7)), 13)->child->name;
}

int macro_tcn_travel_city(void)
{
    return D_00195DB0;
}

int macro_vam_vampire_clan(void)
{
    return faction_find((int)(short)((unsigned short)player_character->vampire_clan)) + 3;
}

int macro_wil_willpower(void)
{
    return func_000A0DD9(player_character->attributes[2], (int)text_rsc_buffer, 10);
}

int macro_wdm_weapon_damage(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = D_00195A80->index;
    l_24 = ((int)(short)*(short *)(material_to_hit + (D_00195A80->material * 2))) / 5;
    l_20 = l_24 + ((int)(short)*(short *)(weapon_damage_min + (l_28 << 2)));
    l_1C = l_24 + ((int)(short)*(short *)(weapon_damage_max + (l_28 << 2)));
    if (l_20 < 0) l_20 = 0;
    if (l_1C < 0) l_1C = 0;
    func_000A0ED9(1687, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017119C, l_20, l_1C);
    return (int)text_rsc_buffer;
}

int macro_wep_weapon_name(void)
{
    if (D_00195A80->enchantments[0].type == (-1)) return (int)D_00195A80;
    parse_item_name(D_00195A80, (int)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_wpn_poison(void)
{
    struct disease *l_1C;

    if (D_00195AA8->children == 0) return (int)D_0017115C;
    l_1C = &D_00195AA8->children->data.disease;
    return *(int *)(D_00182F92 + (l_1C->id << 2));
}

int func_00049D0C(void)
{
    return func_000A0DD9(automap_yaw, (int)text_rsc_buffer, 10);
}

int func_00049D3E(void)
{
    return func_000A0DD9(D_00190BF8, (int)text_rsc_buffer, 10);
}

int func_00049D70(void)
{
    return func_000A0DD9(D_00190BFC, (int)text_rsc_buffer, 10);
}

int func_00049DA2(void)
{
    return func_000A0DD9(D_00190C00, (int)text_rsc_buffer, 10);
}

int func_00049DD4(void)
{
    if (D_00190CD4 == 0) {
        func_000CE3E3((int)text_rsc_buffer, (int)player_character);
        return (int)text_rsc_buffer;
    }
    return func_0004A07F(D_00190CD4, (int)(signed char)D_00190D20);
}

int func_00049E25(void)
{
    return pronoun_he[((int)(signed char)D_00190D20)];
}

int func_00049E53(void)
{
    return pronoun_him[((int)(signed char)D_00190D20)];
}

int func_00049E81(void)
{
    return pronoun_his[((int)(signed char)D_00190D20)];
}

int func_00049EAF(void)
{
    return func_0004A07F(D_00190CD8, (int)(signed char)D_00190D21);
}

int func_00049EDE(void)
{
    return pronoun_he[((int)(signed char)D_00190D21)];
}

int func_00049F0C(void)
{
    return pronoun_him[((int)(signed char)D_00190D21)];
}

int func_00049F3A(void)
{
    return pronoun_his[((int)(signed char)D_00190D21)];
}

int func_00049F68(void)
{
    return func_0004A07F(D_00190CDC, (int)(signed char)D_00190D22);
}

int func_00049F97(void)
{
    return pronoun_he[((int)(signed char)D_00190D22)];
}

int func_00049FC5(void)
{
    return pronoun_him[((int)(signed char)D_00190D22)];
}

int func_00049FF3(void)
{
    return pronoun_his[((int)(signed char)D_00190D22)];
}

int macro_1com_greeting(void)
{
    return talk_macro_1com();
}

int parse_bio_answer_text(int a1)
{
    parse_rsc_text((int)(short)*(short *)(D_00190BE4 + (a1 << 2)), 0, 0);
    return (int)text_rsc_buffer;
}

int func_0004A1A2(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    {
        char l_50[40];

        l_24 = a1;
        l_20 = (int)l_50;
        l_1C = 0;
        while (*(signed char *)((char *)l_24) != 0 && ((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_24) + 1)] & 32)) != 0) {
            *(signed char *)((char *)l_20) = *(signed char *)((char *)l_24);
            l_1C++;
            l_24++;
            l_20++;
        }
        *(signed char *)((char *)l_20) = 0;
        D_00199734 = atoi((int)l_50);
        return l_1C;
    }
}

int parse_signed_itoa(int a1, int a2, int a3)
{
    if (a1 > 0) {
        *(signed char *)((char *)a2) = 43;
        func_000A0DD9(a1, a2 + 1, a3);
    } else {
        func_000A0DD9(a1, a2, a3);
    }
    return a2;
}

void object_weight_add(struct record *a1)
{
    struct record *l_20;
    struct item *l_1C;
    int l_18;

    if (a1->type != 2) return;
    l_1C = &a1->data.item;
    if (l_1C->group == 23) return;
    l_20 = a1->parent;
    while (l_20 != 0 && func_000CE44C((int)((char *)&D_001959EC), (int)l_20, 4) == 0) {
        l_20 = l_20->parent;
    }
    if (l_20 != 0) return;
    if (D_001962AE == 0 && a1->parent == wagon_container) return;
    if (l_1C->group == 28 && l_1C->index == 0) {
        weight_total += ((unsigned)l_1C->value) / 100;
        return;
    }
    if (l_1C->enchantments[0].type != (-1)) {
        for (l_18 = 0; l_18 < 10; l_18++) {
            switch ((unsigned short)l_1C->enchantments[l_18].type) {
            case 11:
                weight_total++;
                return;
            case 23:
                l_18 = l_1C->weight << 2;
                if (l_18 < 20) l_18 = 20;
                weight_total += l_18;
                return;
            }
        }
    }
    weight_total += l_1C->weight;
}

int object_weight(struct record *a1)
{
    struct item *l_20;
    struct character *l_1C;

    weight_total = 0;
    object_foreach(a1->children, (int)object_weight_add);
    if (a1 == player_entity) {
        return (int)(((char *)weight_total) + (((unsigned)a1->data.character.gold) / 100));
    }
    if (a1->type == 2) {
        l_20 = &a1->data.item;
        if (l_20->group == 3 && l_20->index == 18) {
            return (int)(((char *)weight_total) + (l_20->stack_count * l_20->weight));
        }
        if (l_20->group != 23) {
            object_weight_add(a1);
            return weight_total;
        }
        return weight_total;
    }
    if (a1->type == 18) {
        l_1C = &a1->data.character;
        if (l_1C->race > 43) {
            if (((int)(unsigned short)(l_1C->flags & 1)) != 0) return weight_total + 240;
            return weight_total + 350;
        }
        return (int)(((char *)weight_total) + ((int)(short)*(short *)(monster_weights + (l_1C->race * 2))));
    }
    return weight_total;
}

void parse_item_name(struct item *a1, int a2)
{
    int l_14;

    l_14 = 0;
    if (a1->enchantments[0].type == 26) {
        mc_strncpy((int)text_rsc_buffer, a1->name, 2048, (int)D_0017110C, 1967);
        return;
    }
    if (((int)(unsigned short)(a1->item_flags & 32)) == 0) {
        mc_strncpy((int)text_rsc_buffer, ((int)item_templates) + (((int)(short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (a1->group << 2)) + (a1->index * 2)))) * 48), 2048, (int)D_0017110C, 1973);
        return;
    }
    while (a1->name[l_14] != 0) {
        if (((int)(unsigned char)a1->name[l_14]) == 37) {
            mc_strncpy(a2, ((int)item_templates) + (((int)(short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (a1->group << 2)) + (a1->index * 2)))) * 48), 4, (int)D_0017110C, 1981);
            a2 += func_000A0DF4(a2);
            l_14 += 3;
        } else {
            *(signed char *)((char *)a2++) = a1->name[l_14];
            l_14++;
        }
    }
    *(signed char *)((char *)a2) = 0;
}

void parse_rsc_text(int a1, int a2, int a3)
{
    int l_14;
    short l_10;

    l_10 = *(short *)D_00195D6C;
    *(int *)D_00195D6C = D_00195D78;
    l_14 = text_rsc_load((int)(short)*(short *)&a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
    mc_strncpy((int)text_rsc_buffer, l_14, 2048, (int)D_0017110C, 2003);
    if (l_14 != 0 && l_14 != (-1751672937)) {
        mc_free(l_14, (int)D_0017110C, 2004);
        l_14 = -1751672937;
    }
    *(int *)D_00195D6C = (int)(short)l_10;
}

void parse_rsc_text_copy(int a1, int a2)
{
    int l_18;
    short l_14;

    l_14 = *(short *)D_00195D6C;
    *(int *)D_00195D6C = D_00195D78;
    l_18 = text_rsc_load((int)(short)*(short *)&a1, 0, 0);
    mc_strncpy(a2, l_18, 4, (int)D_0017110C, 2034);
    if (l_18 != 0 && l_18 != (-1751672937)) {
        mc_free(l_18, (int)D_0017110C, 2035);
        l_18 = -1751672937;
    }
    *(int *)D_00195D6C = (int)(short)l_14;
}

int parse_stub_zero(int a1)
{
    return 0;
}

int func_0004A8DB(int a1, int a2)
{
    short l_14;
    short l_18;

    *(int *)&l_14 = (((unsigned)(((unsigned)a1) % 518400)) / 1440) + 1;
    if (((int)(short)l_14) > 355) return 0;
    *(int *)&l_18 = 0;
    a2++;
    while (((int)(unsigned char)holiday_regions[(int)(short)l_18]) != 255) {
        if (((int)(unsigned char)holiday_regions[(int)(short)l_18]) == a2 && (short)(short)*(int *)&l_14 >= holiday_days[((int)(short)l_18)]) {
            return (int)(short)l_18;
        }
        (*(int *)&l_18)++;
    }
    return 0;
}

int holiday_today(int a1, int a2)
{
    short l_14;
    short l_18;

    *(int *)&l_18 = 0;
    a2++;
    *(int *)&l_14 = (((unsigned)(((unsigned)a1) % 518400)) / 1440) + 1;
    if (((int)(short)l_14) > 355) return 0;
    while (((int)(short)l_18) < 53) {
        if ((((int)(unsigned char)holiday_regions[(int)(short)l_18]) == 255 || ((int)(unsigned char)holiday_regions[(int)(short)l_18]) == a2) && (short)*(int *)&l_14 == holiday_days[((int)(short)l_18)]) {
            return ((int)(short)l_18) + 1;
        }
        (*(int *)&l_18)++;
    }
    return 0;
}

int holiday_name(int a1, int a2)
{
    return holiday_names[func_0004A8DB(a1, a2)];
}
