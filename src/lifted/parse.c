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
extern char D_00178630[];
extern char player_environment[];
extern struct spell *selected_spell;
extern char weight_fractions[];
extern char D_0017C54E[];
extern char D_0017C552[];
extern char ruler_fates[];
extern char D_0017C912[];
extern char holiday_days[];
extern char holiday_regions[];
extern char holiday_names[];
extern char D_0017CB8E[];
extern char month_names[];
extern char day_names[];
extern char ordinal_suffixes[];
extern char race_names[];
extern char D_0017CC1F[];
extern char skill_names[];
extern char D_0017CEF2[];
extern char material_names[];
extern char material_to_hit[];
extern char weapon_damage_min[];
extern char weapon_damage_max[];
extern char armor_type_names[];
extern char condition_thresholds[];
extern char condition_names[];
extern char pronoun_he[];
extern char pronoun_him[];
extern char pronoun_his[];
extern char D_0017D042[];
extern char province_names[];
extern char province_terrain_names[];
extern char attribute_rating_names[];
extern char direction_names[];
extern char item_templates[];
extern char D_00182F92[];
extern char monster_names[];
extern char ruler_titles[];
extern char D_001837E0[];
extern char region_names[];
extern char D_001841E3[];
extern char crime_names[];
extern char penalty_texts[];
extern char D_00184269[];
extern char codeword_first_words[];
extern char codeword_second_words[];
extern char monster_weights[];
extern char imperial_names[];
extern char location_type_names[];
extern char text_blank[];
extern char honorifics[];
extern char D_0018508F[];
extern char legal_reputation_names[];
extern char D_00185F88[];
extern char saved_location_name[];
extern char saved_region_name[];
extern char region_legal_reputation[];
extern char D_0018F090[];
extern char D_00190BE4[];
extern char D_00190BEC[];
extern char automap_yaw[];
extern char D_00190BF8[];
extern char D_00190BFC[];
extern char D_00190C00[];
extern char D_00190C78[];
extern char D_00190CAC[];
extern char D_00190CD4[];
extern char D_00190CD8[];
extern char D_00190CDC[];
extern char D_00190D16[];
extern char D_00190D17[];
extern char D_00190D20[];
extern char D_00190D21[];
extern char D_00190D22[];
extern char court_prison_days[];
extern char D_00190DCA[];
extern char text_macro_fpc[];
extern char text_macro_fnpc[];
extern char text_macro_fe[];
extern char text_macro_fa[];
extern char text_macro_fea[];
extern char text_macro_fpa[];
extern char D_00190EAC[];
extern char text_rsc_buffer[];
extern char D_001940D7[];
extern char text_macro_fcn[];
extern struct record *wagon_container;
extern struct record *D_001959EC;
extern struct item *D_00195A80;
extern struct character *D_00195A84;
extern char D_00195A90[];
extern char D_00195A94[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern char D_00195AC0[];
extern struct record *D_00195AC4;
extern char D_00195ACC[];
extern char text_macro_book[];
extern struct building *tavern_building;
extern char current_region_data[];
extern char weight_total[];
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195C44[];
extern char D_00195D28[];
extern char D_00195D2C[];
extern char D_00195D30[];
extern char quest_potential_questor[];
extern char D_00195D6C[];
extern char D_00195D78[];
extern char D_00195D94[];
extern char D_00195DB0[];
extern char painting_subject_text[];
extern char painting_adjective_text[];
extern char painting_prefix1_text[];
extern char painting_prefix2_text[];
extern char spell_effect_slot[];
extern char text_macro_n_text[];
extern char D_00196266[];
extern char D_00196267[];
extern char current_region[];
extern char D_00196269[];
extern char D_0019626C[];
extern char D_0019628F[];
extern char D_001962AE[];
extern char talk_key_text[];
extern char D_00196661[];
extern struct faction *D_0019670C;
extern struct faction *D_00196714;
extern struct faction *D_0019671C;
extern struct faction *D_00196720;
extern struct faction *D_00196724;
extern struct faction *D_00196728;
extern char reputation_baseline[];
extern char D_00196D7E[];
extern char D_00196D80[];
extern char D_00196D82[];
extern char D_00196D84[];
extern char D_00199640[];
extern char parse_name_seed[];
extern char D_00199734[];
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
    return func_000A0DD9(*(int *)D_00195D30, (int)text_rsc_buffer, 10);
}

int macro_agi_agility(void)
{
    return func_000A0DD9(player_character->attributes[3], (int)text_rsc_buffer, 10);
}

int func_00046754(void)
{
    return *(int *)(D_0017D042 + (((int)(unsigned char)(*(signed char *)D_00196267 ^ 1)) << 2));
}

int macro_arm_item_name(void)
{
    if (D_00195A80->enchantments[0].type != (-1)) goto L467AD;
    return (int)D_00195A80;
L467AD:;
    parse_item_name(D_00195A80, (int)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_ach_chance_per_level(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->chances[(int)(short)*(short *)spell_effect_slot].plus, (int)text_rsc_buffer, 10);
}

int macro_adr_duration_per_level(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->durations[(int)(short)*(short *)spell_effect_slot].plus, (int)text_rsc_buffer, 10);
}

int macro_1am_magnitude_per_level_min(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->magnitudes[(int)(short)*(short *)spell_effect_slot].plus_min, (int)text_rsc_buffer, 10);
}

int macro_2am_magnitude_per_level_max(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->magnitudes[(int)(short)*(short *)spell_effect_slot].plus_max, (int)text_rsc_buffer, 10);
}

int macro_ark_attribute_rating(void)
{
    short l_18;

    l_18 = player_character->attributes[(int)(unsigned char)*(signed char *)D_0019626C] / 10;
    if (((int)(short)l_18) < 10) goto L4691E;
    l_18 = 9;
L4691E:;
    return *(int *)(attribute_rating_names + (((((int)(unsigned char)*(signed char *)D_0019626C) * 10) + ((int)(short)l_18)) << 2));
}

int macro_adj_painting_adjective(void)
{
    parse_rsc_text((int)(unsigned short)*(short *)painting_adjective_text, 0, 0);
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

    if (D_00195A80->index < 7) goto L46A46;
    return parse_signed_itoa(D_00195A80->index - 6, (int)text_rsc_buffer, 10);
L46A46:;
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
    l_20 = name_generate((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned char)(*(signed char *)((char *)flats_cfg_find(l_1C->image) + 6) & 1));
    srand(l_24);
    return l_20;
}

int macro_bch_base_chance(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->chances[(int)(short)*(short *)spell_effect_slot].base, (int)text_rsc_buffer, 10);
}

int macro_bdr_base_duration(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->durations[(int)(short)*(short *)spell_effect_slot].base, (int)text_rsc_buffer, 10);
}

int macro_1bm_base_magnitude_min(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->magnitudes[(int)(short)*(short *)spell_effect_slot].base_min, (int)text_rsc_buffer, 10);
}

int macro_2bm_base_magnitude_max(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->magnitudes[(int)(short)*(short *)spell_effect_slot].base_max, (int)text_rsc_buffer, 10);
}

int macro_bt_book_title(void)
{
    return *(int *)text_macro_book;
}

int macro_ba_book_author(void)
{
    return *(int *)text_macro_book + 64;
}

int macro_cn_city_name(void)
{
    if (*(int *)D_00195D94 == 0) goto L46CE3;
    return *(int *)D_00195D94;
L46CE3:;
    if (D_00195AC4->image == 65535) goto L46D02;
    return (int)current_location;
L46D02:;
    return *(int *)(region_names + (((int)(unsigned char)*(signed char *)current_region) << 2));
}

int macro_cn2_blank(void)
{
    return *(int *)text_blank;
}

int macro_ct_location_type(void)
{
    return *(int *)(location_type_names + (current_location->kind << 2));
}

int macro_clc_chance_levels(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->chances[(int)(short)*(short *)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_cld_duration_levels(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->durations[(int)(short)*(short *)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
}

int macro_clm_magnitude_levels(void)
{
    return func_000A0DD9((int)(signed char)selected_spell->magnitudes[(int)(short)*(short *)spell_effect_slot].per_level, (int)text_rsc_buffer, 10);
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
    return *(int *)(crime_names + (((int)(signed char)*(signed char *)D_00190D17) << 2));
}

int macro_cpn_shop_name(void)
{
    return building_name((int)current_building);
}

int macro_crn_current_region(void)
{
    return *(int *)(region_names + (((int)(unsigned char)*(signed char *)current_region) << 2));
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
    l_20 = name_generate((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned char)(*(signed char *)((char *)flats_cfg_find(l_1C->image) + 6) & 1));
    srand(l_24);
    return l_20;
}

int macro_dam_damage_modifier(void)
{
    return parse_signed_itoa((player_character->attributes[0] / 10) - 5, (int)text_rsc_buffer, 10);
}

int macro_dwr_room_hours_left(void)
{
    return func_000A0DD9(((unsigned)(tavern_building->rent_expires - *(int *)game_minutes)) / 60, (int)text_rsc_buffer, 10);
}

int func_00047094(void)
{
    int l_1C;

    l_1C = rand() & 7;
L470AD:;
    if ((player_character->min_metal_to_hit >> 5) != l_1C) goto L470CF;
    l_1C = rand() & 7;
    goto L470AD;
L470CF:;
    return *(int *)(province_names + (l_1C << 2));
}

int macro_di_direction(void)
{
    return *(int *)(direction_names + (((((func_000C808D(player_object->x, player_object->z, *(int *)D_00195A90, *(int *)D_00195A94) >> 2) + 32) & 511) >> 6) << 2));
}

int macro_du_blank(void)
{
    return *(int *)text_blank;
}

int macro_dat_date(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_20 = ((unsigned)(((unsigned)*(int *)game_minutes) % 518400)) / 1440;
    l_24 = l_20 / 30;
    l_28 = l_20 % 30;
    l_2C = l_20 % 7;
    if (l_28 <= 3) goto L471D6;
    l_1C = 3;
    goto L471DC;
L471D6:;
    l_1C = l_28;
L471DC:;
    func_000A0ED9(431, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171134, *(int *)(day_names + (l_2C << 2)), l_28 + 1, *(int *)(ordinal_suffixes + (l_1C << 2)), *(int *)(month_names + (l_24 << 2)));
    return (int)text_rsc_buffer;
}

int macro_dip_days_in_prison(void)
{
    return func_000A0DD9((int)(short)*(short *)court_prison_days, (int)text_rsc_buffer, 10);
}

int func_00047271(void)
{
    int l_1C;

    if ((int)current_location == 0) goto L472E2;
    l_1C = 0;
L4728F:;
    if (current_location->building_count > l_1C) goto L472AC;
    goto L472E2;
L472A4:;
    l_1C++;
    goto L4728F;
L472AC:;
    if (((int)(unsigned short)*(short *)((char *)(int)((l_1C * 26) + (char *)current_location->buildings) + 18)) != 108) goto L472E0;
    return building_name((int)((char *)current_location->buildings + (l_1C * 26)));
L472E0:;
    goto L472A4;
L472E2:;
    return (int)D_00171146;
}

int macro_dbp_codeword(void)
{
    func_000A0ED9(454, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017114E, *(int *)(codeword_first_words + ((player_character->pad223 >> 4) << 2)), *(int *)(codeword_second_words + (((int)(unsigned char)(player_character->pad223 & 15)) << 2)));
    return (int)text_rsc_buffer;
}

int func_00047373(void)
{
    return func_000A0DD9(((unsigned)((D_00195AA8->repair_due - *(int *)game_minutes) + 1439)) / 1440, (int)text_rsc_buffer, 10);
}

int func_000473BE(void)
{
    return func_000A0DD9(*(int *)D_00195D2C, (int)text_rsc_buffer, 10);
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

    l_20 = name_generate((int)(unsigned char)*(signed char *)D_00196267, 0);
    l_1C = strchr(l_20, 32);
    if (l_1C == 0) goto L474B9;
    *(signed char *)((char *)l_1C) = 0;
L474B9:;
    return l_20;
}

int macro_foc_blank(void)
{
    return *(int *)text_blank;
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
    srand(*(int *)parse_name_seed);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 1);
    srand(l_20);
    return l_1C;
}

int macro_fn2_female_name2(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(*(int *)parse_name_seed + 123);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 1);
    srand(l_20);
    return l_1C;
}

int func_000475C9(void)
{
    if (D_00196720->child == 0) goto L475F2;
    return (int)D_00196720->child->name;
L475F2:;
    return *(int *)text_blank;
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

    if (D_0019671C->type != 7) goto L476EB;
    if (D_0019671C->child != 0) goto L476ED;
L476EB:;
    goto L47701;
L476ED:;
    if (D_0019671C->child->type == 4) goto L47703;
L47701:;
    goto L47713;
L47703:;
    return (int)D_0019671C->child->name;
L47713:;
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

    if (D_0019670C->type != 7) goto L47791;
    if (D_0019670C->child != 0) goto L47793;
L47791:;
    goto L477A7;
L47793:;
    if (D_0019670C->child->type == 4) goto L477A9;
L477A7:;
    goto L477B9;
L477A9:;
    return (int)D_0019670C->child->name;
L477B9:;
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
    return *(int *)text_macro_fnpc + 3;
}

int macro_fe_shared_enemy(void)
{
    return *(int *)text_macro_fe + 3;
}

int macro_fa_shared_ally(void)
{
    return *(int *)text_macro_fa + 3;
}

int macro_fea_player_enemy_npc_ally(void)
{
    return *(int *)text_macro_fea + 3;
}

int macro_fpa_shared_faction(void)
{
    return *(int *)text_macro_fpa + 3;
}

int macro_g_pronoun_he(void)
{
    return *(int *)(pronoun_he + (((int)(unsigned char)*(signed char *)D_0019628F) << 2));
}

int macro_g2_pronoun_him(void)
{
    return *(int *)(pronoun_him + (((int)(unsigned char)*(signed char *)D_0019628F) << 2));
}

int macro_g3_pronoun_his(void)
{
    return *(int *)(pronoun_his + (((int)(unsigned char)*(signed char *)D_0019628F) << 2));
}

int macro_gii_gold_carried(void)
{
    return utoa(player_character->gold, (int)text_rsc_buffer, 10);
}

int macro_gtp_fine(void)
{
    return func_000A0DD9(*(int *)D_00190CAC, (int)text_rsc_buffer, 10);
}

int macro_gdd_temple_god(void)
{
    if ((int)guild_membership == 0) goto L47A67;
    return *(int *)(D_0017CB8E + (guild_membership->kind << 2));
L47A67:;
    return *(int *)text_blank;
}

int macro_god_local_god(void)
{
    struct faction *l_20;
    struct faction *l_1C;

    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L47AA8;
    if (current_building->type == 14) goto L47AAA;
L47AA8:;
    goto L47AED;
L47AAA:;
    l_1C = (struct faction *)faction_find((int)(short)current_building->faction_id);
    if (l_1C == 0) goto L47ACA;
    if (l_1C->parent != 0) goto L47ACC;
L47ACA:;
    goto L47ADA;
L47ACC:;
    return (int)l_1C->parent->name;
L47ADA:;
    if (l_1C == 0) goto L47AEB;
    return (int)l_1C->name;
L47AEB:;
    goto L47B1A;
L47AED:;
    l_20 = (struct faction *)faction_find((int)(short)*(short *)(D_0017C912 + (((int)(unsigned char)*(signed char *)current_region) << 2)));
    if (l_20 == 0) goto L47B1A;
    return (int)l_20->parent->name;
L47B1A:;
    return *(int *)text_blank;
}

int func_00047B2F(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(*(int *)parse_name_seed + 3457);
    l_1C = name_generate(player_character->race, (int)(unsigned char)(*(signed char *)D_00190C78 & 2));
    srand(l_20);
    return l_1C;
}

int macro_tim_time(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = ((unsigned)*(int *)game_minutes) % 1440;
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
    if (D_00195AA8->children != 0) goto L47C7E;
    return (int)D_0017115C;
L47C7E:;
    if (D_00195AA8->children->image < 43) goto L47C9D;
    return (int)D_00171164;
L47C9D:;
    return *(int *)(monster_names + (D_00195AA8->children->image << 2));
}

int macro_hod_holiday_description(void)
{
    int l_1C;

    l_1C = func_0004A8DB(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region);
    parse_rsc_text(l_1C + 8350, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_hc_blank(void)
{
    return *(int *)text_blank;
}

int macro_hct_blank(void)
{
    return *(int *)text_blank;
}

int macro_ht_blank(void)
{
    return *(int *)text_blank;
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
    return func_000A0DD9(((int)(short)*(short *)court_prison_days) * 24, (int)text_rsc_buffer, 10);
}

int macro_hpn_home_province(void)
{
    return *(int *)(province_names + (((int)(unsigned char)*(signed char *)(D_0017CC1F + player_character->race)) << 2));
}

int macro_hpw_home_terrain(void)
{
    return *(int *)(province_terrain_names + (((int)(unsigned char)*(signed char *)(D_0017CC1F + player_character->race)) << 2));
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
    if (((int)(unsigned short)(player_character->flags & 1)) == 0) goto L47EF9;
    return *(int *)D_0018508F;
L47EF9:;
    return *(int *)honorifics;
}

int macro_int_intelligence(void)
{
    return func_000A0DD9(player_character->attributes[1], (int)text_rsc_buffer, 10);
}

int macro_imp_imperial_name(void)
{
    return *(int *)(imperial_names + (((int)(unsigned char)*(signed char *)D_00196266) << 2));
}

int func_00047F72(void)
{
    return (int)D_00195A80;
}

int macro_kg_weight(void)
{
    int l_1C;

    l_1C = object_weight(D_00195AA8);
    if ((l_1C & 3) == 0) goto L47FF5;
    func_000A0ED9(847, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017116D, l_1C >> 2, *(int *)(weight_fractions + ((l_1C & 3) << 2)));
    goto L48020;
L47FF5:;
    func_000A0ED9(849, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171173, l_1C >> 2);
L48020:;
    return (int)text_rsc_buffer;
}

int macro_pow_blank(void)
{
    return *(int *)text_blank;
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
    if (l_1C == 0) goto L480F1;
    return faction_find(l_1C->faction) + 3;
L480F1:;
    if (D_0019671C == 0) goto L48107;
    return (int)D_0019671C->name;
L48107:;
    return *(int *)text_blank;
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
    return *(int *)(D_0017D042 + (((int)(unsigned char)*(signed char *)D_00196267) << 2));
}

int macro_luc_luck(void)
{
    return func_000A0DD9(player_character->attributes[7], (int)text_rsc_buffer, 10);
}

int func_000481C4(void)
{
    return func_000A0DD9(*(int *)D_00190BEC, (int)text_rsc_buffer, 10);
}

int macro_lev_guild_rank(void)
{
    if ((int)guild_membership == 0) goto L4823F;
    return *(int *)((char *)(int)(*(char **)(D_0017CEF2 + (((int)(unsigned char)(guild_membership->kind & 63)) << 2)) + (guild_membership->rank << 2)));
L4823F:;
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

    l_20 = (int)(short)*(short *)(region_legal_reputation + (((int)(unsigned char)*(signed char *)current_region) * 80));
    l_1C = 0;
    if (l_20 >= (-100)) goto L48307;
    l_20 = -100;
    goto L48314;
L48307:;
    if (l_20 <= 100) goto L48314;
    l_20 = 100;
L48314:;
    if (l_20 < *(int *)(D_0017C54E + (l_1C << 3))) goto L48336;
    if (l_20 <= *(int *)(D_0017C552 + (l_1C << 3))) goto L4833E;
L48336:;
    l_1C++;
    goto L48314;
L4833E:;
    return *(int *)(legal_reputation_names + (l_1C << 2));
}

int macro_mad_magic_resist(void)
{
    return func_000A0DD9(player_character->attributes[2] / 10, (int)text_rsc_buffer, 10);
}

int macro_mat_material(void)
{
    if (D_00195A80->group != 2) goto L483E8;
    if (D_00195A80->index >= 7) goto L483E6;
    if (D_00195A80->index != 5) goto L483E8;
L483E6:;
    goto L483EA;
L483E8:;
    goto L483F3;
L483EA:;
    return (int)D_00171177;
L483F3:;
    if (D_00195A80->group != 2) goto L48418;
    if (D_00195A80->armor_type != 2) goto L4841A;
L48418:;
    goto L48435;
L4841A:;
    return *(int *)(armor_type_names + (D_00195A80->armor_type << 2));
L48435:;
    return *(int *)(material_names + (D_00195A80->material << 2));
}

int func_0004845B(void)
{
    return (int)D_00195A80;
}

int macro_it_item_name(void)
{
    if (D_00195A80->enchantments[0].type != (-1)) goto L484A4;
    return (int)D_00195A80;
L484A4:;
    parse_item_name(D_00195A80, (int)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_mt_blank(void)
{
    return *(int *)text_blank;
}

int macro_mn_male_name(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(*(int *)parse_name_seed + 3457);
    l_1C = name_generate((int)(unsigned char)(rand() & 7), 0);
    srand(l_20);
    return l_1C;
}

int macro_mn2_male_name2(void)
{
    int l_20;
    int l_1C;

    l_20 = rand();
    srand(*(int *)parse_name_seed + 9543);
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
    l_1C = name_generate((int)(unsigned char)*(signed char *)D_00196267, (int)(unsigned char)(rand() & 1));
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

    l_1C = *(int *)D_00190EAC;
    return *(int *)((char *)l_1C + 16);
}

int macro_mpw_magic_powers(void)
{
    if (((int)(unsigned short)(D_00195A80->item_flags & 2048)) == 0) goto L486A8;
    parse_rsc_text(D_00195A80->enchantments[9].param + 8700, 0, 0);
    return (int)text_rsc_buffer;
L486A8:;
    return enchant_powers_text((int)D_00195A80);
}

int macro_nh_holiday_name(void)
{
    return holiday_name(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region);
}

int macro_nhd_holiday_date(void)
{
    return calendar_format_date(((int)(short)*(short *)(holiday_days + (func_0004A8DB(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region) * 2))) * 1440, (int)text_rsc_buffer);
}

int macro_nt_nearby_tavern(void)
{
    return parse_town_building_name(15);
}

int func_00048765(void)
{
    return *(int *)D_00195AC0 + 28;
}

int macro_n_npc_name(void)
{
    if (*(short *)text_macro_n_text == 0) goto L487BD;
    parse_rsc_text((int)(unsigned short)*(short *)text_macro_n_text, 0, 0);
    return (int)text_rsc_buffer;
L487BD:;
    if (((struct bf8_2_1 *)&D_001940D7)->f == 0) goto L487D7;
    return name_generate(8, 0);
L487D7:;
    if ((int)D_00195A84 == 0) goto L487EA;
    return (int)D_00195A84;
L487EA:;
    return name_generate((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned char)(rand() & 1));
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
    l_1C = *(int *)(ruler_fates + (rand_range(0, 5) << 2));
    srand(l_20);
    return l_1C;
}

int macro_ol1_old_leader(void)
{
    int l_20;
    int l_1C;

    if (D_0019671C->type != 7) goto L488EA;
    if (D_0019671C->child != 0) goto L488EC;
L488EA:;
    goto L48900;
L488EC:;
    if (D_0019671C->child->type == 4) goto L48902;
L48900:;
    goto L48912;
L48902:;
    return (int)D_0019671C->child->name;
L48912:;
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
L4899C:;
    if (player_character->name[l_1C] == 0) goto L489BD;
    if (((int)(unsigned char)player_character->name[l_1C]) != 32) goto L489BF;
L489BD:;
    goto L489DA;
L489BF:;
    *(signed char *)(text_rsc_buffer + l_1C) = player_character->name[l_1C];
    l_1C++;
    goto L4899C;
L489DA:;
    *(signed char *)(text_rsc_buffer + l_1C) = 0;
    return (int)text_rsc_buffer;
}

int macro_per_personality(void)
{
    return func_000A0DD9(player_character->attributes[5], (int)text_rsc_buffer, 10);
}

int macro_po_potion_name(void)
{
    func_000A0ED9(1157, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_00171178, *(int *)D_00195ACC + 67);
    return (int)text_rsc_buffer;
}

int macro_pp1_painting_prefix1(void)
{
    parse_rsc_text((int)(unsigned short)*(short *)painting_prefix1_text, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_pp2_painting_prefix2(void)
{
    parse_rsc_text((int)(unsigned short)*(short *)painting_prefix2_text, 0, 0);
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
    if (((int)(signed char)*(signed char *)D_00190D16) != 2) goto L48B44;
    parse_expand(*(int *)D_00184269, *(int *)D_00195C44);
    goto L48B6D;
L48B44:;
    mc_strncpy(*(int *)D_00195C44, *(int *)(penalty_texts + (((int)(signed char)*(signed char *)D_00190D16) << 2)), 4, (int)D_0017110C, 1182);
L48B6D:;
    return *(int *)D_00195C44;
}

int macro_pdg_more_prison_days(void)
{
    return func_000A0DD9((int)(short)*(short *)D_00190DCA, (int)text_rsc_buffer, 10);
}

int macro_prn_prison_name(void)
{
    *(signed char *)D_001940D7 |= 4;
    parse_rsc_text(8100, 0, 0);
    *(signed char *)D_001940D7 &= 251;
    return (int)text_rsc_buffer;
}

int macro_pqn_questor_name(void)
{
    return func_0008B48B(*(int *)quest_potential_questor);
}

int macro_pqp_questor_place(void)
{
    int l_1C;

    l_1C = object_building(*(int *)quest_potential_questor);
    return building_name(l_1C);
}

int macro_pd_blank(void)
{
    return *(int *)text_blank;
}

int macro_ph_blank(void)
{
    return *(int *)text_blank;
}

int macro_plm_blank(void)
{
    return *(int *)text_blank;
}

int macro_plq_blank(void)
{
    return *(int *)text_blank;
}

int macro_pn_blank(void)
{
    return *(int *)text_blank;
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
    if (D_00195A80->max_condition == 0) goto L48DE3;
    l_1C = (D_00195A80->condition * 100) / D_00195A80->max_condition;
    goto L48DE7;
L48DE3:;
    l_1C = 0;
L48DE7:;
    if (l_1C <= *(unsigned char *)(condition_thresholds + ((int)(unsigned char)l_18))) goto L48DFF;
    l_18++;
    goto L48DE7;
L48DFF:;
    return *(int *)(condition_names + (((int)(unsigned char)l_18) << 2));
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
    return *(int *)text_blank;
}

int macro_qdt_quest_date(void)
{
    int l_20;
    int l_1C;

    l_1C = *(int *)game_minutes;
    *(int *)game_minutes = *(int *)D_00190BE4;
    l_20 = macro_dat_date();
    *(int *)game_minutes = l_1C;
    return l_20;
}

int macro_ra_player_race(void)
{
    if (player_character->race <= 7) goto L4945E;
    return *(int *)(race_names + (player_character->original_race << 2));
L4945E:;
    return *(int *)(race_names + (player_character->race << 2));
}

int macro_rf_empty(void)
{
    return (int)D_00171177;
}

int macro_rt_ruler_title(void)
{
    struct faction *l_1C;

    l_1C = faction_find_type_in_region((int)(short)((int)(unsigned char)*(signed char *)current_region), 14);
    return parse_faction_ruler_title(l_1C);
}

int parse_faction_ruler_title(struct faction *a1)
{
    if (a1 != 0) goto L49502;
    return *(int *)D_001837E0;
L49502:;
    if (a1->ruler == 0) goto L49524;
    return *(int *)(ruler_titles + (a1->ruler << 2));
L49524:;
    if (a1->parent == 0) goto L49562;
    a1 = a1->parent;
    if (a1->ruler == 0) goto L49558;
    return *(int *)(ruler_titles + (a1->ruler << 2));
L49558:;
    return *(int *)D_001837E0;
L49562:;
    return *(int *)D_001837E0;
}

int macro_reg_previous_region(void)
{
    return *(int *)(region_names + (((int)(unsigned char)*(signed char *)D_00196269) << 2));
}

int macro_rn_ruler_name(void)
{
    struct faction *l_1C;

    l_1C = faction_find_type_in_region((int)(short)((int)(unsigned char)*(signed char *)current_region), 7);
    if (l_1C->child == 0) goto L495E3;
    if (l_1C->child->type == 4) goto L495E5;
L495E3:;
    goto L495F3;
L495E5:;
    return (int)l_1C->child->name;
L495F3:;
    l_1C = faction_find_type_in_region((int)(short)((int)(unsigned char)*(signed char *)current_region), 14);
    return func_0008B43B((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned char)(l_1C->region & 1), l_1C->seed & 65535);
}

int macro_r1_commoners_change(void)
{
    if (player_character->reputation[0] >= *(short *)reputation_baseline) goto L49673;
    return (int)D_00171185;
L49673:;
    if (player_character->reputation[0] <= *(short *)reputation_baseline) goto L49691;
    return (int)D_0017118B;
L49691:;
    return (int)D_00171192;
}

int macro_r2_merchants_change(void)
{
    if (player_character->reputation[1] >= *(short *)D_00196D7E) goto L496D1;
    return (int)D_00171185;
L496D1:;
    if (player_character->reputation[1] <= *(short *)D_00196D7E) goto L496EF;
    return (int)D_0017118B;
L496EF:;
    return (int)D_00171192;
}

int macro_r3_scholars_change(void)
{
    if (player_character->reputation[2] >= *(short *)D_00196D80) goto L4972F;
    return (int)D_00171185;
L4972F:;
    if (player_character->reputation[2] <= *(short *)D_00196D80) goto L4974D;
    return (int)D_0017118B;
L4974D:;
    return (int)D_00171192;
}

int macro_r4_nobility_change(void)
{
    if (player_character->reputation[3] >= *(short *)D_00196D82) goto L4978D;
    return (int)D_00171185;
L4978D:;
    if (player_character->reputation[3] <= *(short *)D_00196D82) goto L497AB;
    return (int)D_0017118B;
L497AB:;
    return (int)D_00171192;
}

int macro_r5_underworld_change(void)
{
    if (player_character->reputation[4] >= *(short *)D_00196D84) goto L497EB;
    return (int)D_00171185;
L497EB:;
    if (player_character->reputation[4] <= *(short *)D_00196D84) goto L49809;
    return (int)D_0017118B;
L49809:;
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
    parse_rsc_text((int)(unsigned short)*(short *)painting_subject_text, 0, 0);
    return (int)text_rsc_buffer;
}

int func_0004992E(void)
{
    return (int)D_00196661;
}

int func_00049950(void)
{
    return func_000A0DD9(*(int *)D_00195D30, (int)text_rsc_buffer, 10);
}

int macro_ski_skill_name(void)
{
    return *(int *)(skill_names + (*(int *)D_00199640 << 2));
}

int macro_oth_oath(void)
{
    parse_rsc_text(D_00195A84->race + 201, 0, 0);
    return (int)text_rsc_buffer;
}

int macro_sng_blank(void)
{
    return *(int *)text_blank;
}

int macro_reg_where_target_region(void)
{
    return *(int *)(region_names + (((int)(unsigned char)*(signed char *)(*(char **)D_00195D28 + 7)) << 2));
}

int macro_t_ruler_title(void)
{
    return parse_faction_ruler_title(faction_find_type_in_region((int)(short)((int)(unsigned char)*(signed char *)current_region), 7));
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
    return *(int *)D_00195DB0;
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
    if (l_20 >= 0) goto L49C23;
    l_20 = 0;
L49C23:;
    if (l_1C >= 0) goto L49C30;
    l_1C = 0;
L49C30:;
    func_000A0ED9(1687, (int)D_0017110C);
    mc_sprintf((int)text_rsc_buffer, (int)D_0017119C, l_20, l_1C);
    return (int)text_rsc_buffer;
}

int macro_wep_weapon_name(void)
{
    if (D_00195A80->enchantments[0].type != (-1)) goto L49C96;
    return (int)D_00195A80;
L49C96:;
    parse_item_name(D_00195A80, (int)text_rsc_buffer);
    return (int)text_rsc_buffer;
}

int macro_wpn_poison(void)
{
    struct disease *l_1C;

    if (D_00195AA8->children != 0) goto L49CDB;
    return (int)D_0017115C;
L49CDB:;
    l_1C = &D_00195AA8->children->data.disease;
    return *(int *)(D_00182F92 + (l_1C->id << 2));
}

int func_00049D0C(void)
{
    return func_000A0DD9(*(int *)automap_yaw, (int)text_rsc_buffer, 10);
}

int func_00049D3E(void)
{
    return func_000A0DD9(*(int *)D_00190BF8, (int)text_rsc_buffer, 10);
}

int func_00049D70(void)
{
    return func_000A0DD9(*(int *)D_00190BFC, (int)text_rsc_buffer, 10);
}

int func_00049DA2(void)
{
    return func_000A0DD9(*(int *)D_00190C00, (int)text_rsc_buffer, 10);
}

int func_00049DD4(void)
{
    if (*(int *)D_00190CD4 != 0) goto L49E04;
    func_000CE3E3((int)text_rsc_buffer, (int)player_character);
    return (int)text_rsc_buffer;
L49E04:;
    return func_0004A07F(*(int *)D_00190CD4, (int)(signed char)*(signed char *)D_00190D20);
}

int func_00049E25(void)
{
    return *(int *)(pronoun_he + (((int)(signed char)*(signed char *)D_00190D20) << 2));
}

int func_00049E53(void)
{
    return *(int *)(pronoun_him + (((int)(signed char)*(signed char *)D_00190D20) << 2));
}

int func_00049E81(void)
{
    return *(int *)(pronoun_his + (((int)(signed char)*(signed char *)D_00190D20) << 2));
}

int func_00049EAF(void)
{
    return func_0004A07F(*(int *)D_00190CD8, (int)(signed char)*(signed char *)D_00190D21);
}

int func_00049EDE(void)
{
    return *(int *)(pronoun_he + (((int)(signed char)*(signed char *)D_00190D21) << 2));
}

int func_00049F0C(void)
{
    return *(int *)(pronoun_him + (((int)(signed char)*(signed char *)D_00190D21) << 2));
}

int func_00049F3A(void)
{
    return *(int *)(pronoun_his + (((int)(signed char)*(signed char *)D_00190D21) << 2));
}

int func_00049F68(void)
{
    return func_0004A07F(*(int *)D_00190CDC, (int)(signed char)*(signed char *)D_00190D22);
}

int func_00049F97(void)
{
    return *(int *)(pronoun_he + (((int)(signed char)*(signed char *)D_00190D22) << 2));
}

int func_00049FC5(void)
{
    return *(int *)(pronoun_him + (((int)(signed char)*(signed char *)D_00190D22) << 2));
}

int func_00049FF3(void)
{
    return *(int *)(pronoun_his + (((int)(signed char)*(signed char *)D_00190D22) << 2));
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
L4A1C6:;
    if (*(signed char *)((char *)l_24) == 0) goto L4A1EB;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_24) + 1))) & 32)) != 0) goto L4A1ED;
L4A1EB:;
    goto L4A20B;
L4A1ED:;
    *(signed char *)((char *)l_20) = *(signed char *)((char *)l_24);
    l_1C++;
    l_24++;
    l_20++;
    goto L4A1C6;
L4A20B:;
    *(signed char *)((char *)l_20) = 0;
    *(int *)D_00199734 = atoi((int)l_50);
    return l_1C;
}
}

int parse_signed_itoa(int a1, int a2, int a3)
{
    if (a1 <= 0) goto L4A263;
    *(signed char *)((char *)a2) = 43;
    func_000A0DD9(a1, a2 + 1, a3);
    goto L4A271;
L4A263:;
    func_000A0DD9(a1, a2, a3);
L4A271:;
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
L4A2CD:;
    if (l_20 == 0) goto L4A2E9;
    if (func_000CE44C((int)((char *)&D_001959EC), (int)l_20, 4) == 0) goto L4A2EB;
L4A2E9:;
    goto L4A2F6;
L4A2EB:;
    l_20 = l_20->parent;
    goto L4A2CD;
L4A2F6:;
    if (l_20 != 0) return;
    if (*(signed char *)D_001962AE != 0) goto L4A317;
    if (a1->parent == wagon_container) goto L4A319;
L4A317:;
    goto L4A31E;
L4A319:;
    return;
L4A31E:;
    if (l_1C->group != 28) goto L4A339;
    if (l_1C->index == 0) goto L4A33B;
L4A339:;
    goto L4A355;
L4A33B:;
    *(int *)weight_total += ((unsigned)l_1C->value) / 100;
    return;
L4A355:;
    if (l_1C->enchantments[0].type == (-1)) goto L4A3D6;
    l_18 = 0;
L4A36C:;
    if (l_18 < 10) goto L4A37F;
    goto L4A3D6;
L4A377:;
    l_18++;
    goto L4A36C;
L4A37F:;
    switch ((unsigned short)l_1C->enchantments[l_18].type) {
case 11:
    (*(int *)weight_total)++;
    return;
case 23:
    l_18 = l_1C->weight << 2;
    if (l_18 >= 20) goto L4A3C9;
    l_18 = 20;
L4A3C9:;
    *(int *)weight_total += l_18;
    return;
default:
    goto L4A377;
L4A3D6:;
    *(int *)weight_total += l_1C->weight;
}
}

int object_weight(struct record *a1)
{
    struct item *l_20;
    struct character *l_1C;

    *(int *)weight_total = 0;
    object_foreach(a1->children, (int)object_weight_add);
    if (a1 != player_entity) goto L4A444;
    return (int)(*(char **)weight_total + (((unsigned)a1->data.character.gold) / 100));
L4A444:;
    if (a1->type != 2) goto L4A4D5;
    l_20 = &a1->data.item;
    if (l_20->group != 3) goto L4A482;
    if (l_20->index == 18) goto L4A484;
L4A482:;
    goto L4A4A2;
L4A484:;
    return (int)(*(char **)weight_total + (l_20->stack_count * l_20->weight));
L4A4A2:;
    if (l_20->group == 23) goto L4A4C8;
    object_weight_add(a1);
    return *(int *)weight_total;
L4A4C8:;
    return *(int *)weight_total;
L4A4D5:;
    if (a1->type != 18) goto L4A555;
    l_1C = &a1->data.character;
    if (l_1C->race <= 43) goto L4A534;
    if (((int)(unsigned short)(l_1C->flags & 1)) == 0) goto L4A525;
    return *(int *)weight_total + 240;
L4A525:;
    return *(int *)weight_total + 350;
L4A534:;
    return (int)(*(char **)weight_total + ((int)(short)*(short *)(monster_weights + (l_1C->race * 2))));
L4A555:;
    return *(int *)weight_total;
}

void parse_item_name(struct item *a1, int a2)
{
    int l_14;

    l_14 = 0;
    if (a1->enchantments[0].type != 26) goto L4A5B1;
    mc_strncpy((int)text_rsc_buffer, a1->name, 2048, (int)D_0017110C, 1967);
    return;
L4A5B1:;
    if (((int)(unsigned short)(a1->item_flags & 32)) != 0) goto L4A613;
    mc_strncpy((int)text_rsc_buffer, ((int)item_templates) + (((int)(short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (a1->group << 2)) + (a1->index * 2)))) * 48), 2048, (int)D_0017110C, 1973);
    return;
L4A613:;
    if (a1->name[l_14] == 0) goto L4A6A6;
    if (((int)(unsigned char)a1->name[l_14]) != 37) goto L4A68B;
    mc_strncpy(a2, ((int)item_templates) + (((int)(short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (a1->group << 2)) + (a1->index * 2)))) * 48), 4, (int)D_0017110C, 1981);
    a2 += func_000A0DF4(a2);
    l_14 += 3;
    goto L4A6A1;
L4A68B:;
    *(signed char *)((char *)a2++) = a1->name[l_14];
    l_14++;
L4A6A1:;
    goto L4A613;
L4A6A6:;
    *(signed char *)((char *)a2) = 0;
}

void parse_rsc_text(int a1, int a2, int a3)
{
    int l_14;
    short l_10;

    l_10 = *(short *)D_00195D6C;
    *(int *)D_00195D6C = *(int *)D_00195D78;
    l_14 = text_rsc_load((int)(short)*(short *)&a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
    mc_strncpy((int)text_rsc_buffer, l_14, 2048, (int)D_0017110C, 2003);
    if (l_14 == 0) goto L4A71C;
    if (l_14 != (-1751672937)) goto L4A71E;
L4A71C:;
    goto L4A737;
L4A71E:;
    mc_free(l_14, (int)D_0017110C, 2004);
    l_14 = -1751672937;
L4A737:;
    *(int *)D_00195D6C = (int)(short)l_10;
}

void parse_rsc_text_copy(int a1, int a2)
{
    int l_18;
    short l_14;

    l_14 = *(short *)D_00195D6C;
    *(int *)D_00195D6C = *(int *)D_00195D78;
    l_18 = text_rsc_load((int)(short)*(short *)&a1, 0, 0);
    mc_strncpy(a2, l_18, 4, (int)D_0017110C, 2034);
    if (l_18 == 0) goto L4A889;
    if (l_18 != (-1751672937)) goto L4A88B;
L4A889:;
    goto L4A8A4;
L4A88B:;
    mc_free(l_18, (int)D_0017110C, 2035);
    l_18 = -1751672937;
L4A8A4:;
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
    if (((int)(short)l_14) <= 355) goto L4A91D;
    return 0;
L4A91D:;
    *(int *)&l_18 = 0;
    a2++;
L4A92A:;
    if (((int)(unsigned char)*(signed char *)(holiday_regions + ((int)(short)l_18))) == 255) goto L4A979;
    if (((int)(unsigned char)*(signed char *)(holiday_regions + ((int)(short)l_18))) != a2) goto L4A966;
    if ((short)(short)*(int *)&l_14 >= *(short *)(holiday_days + (((int)(short)l_18) * 2))) goto L4A968;
L4A966:;
    goto L4A971;
L4A968:;
    return (int)(short)l_18;
L4A971:;
    (*(int *)&l_18)++;
    goto L4A92A;
L4A979:;
    return 0;
}

int holiday_today(int a1, int a2)
{
    short l_14;
    short l_18;

    *(int *)&l_18 = 0;
    a2++;
    *(int *)&l_14 = (((unsigned)(((unsigned)a1) % 518400)) / 1440) + 1;
    if (((int)(short)l_14) <= 355) goto L4A9DB;
    return 0;
L4A9DB:;
    if (((int)(short)l_18) >= 53) goto L4AA34;
    if (((int)(unsigned char)*(signed char *)(holiday_regions + ((int)(short)l_18))) == 255) goto L4AA0E;
    if (((int)(unsigned char)*(signed char *)(holiday_regions + ((int)(short)l_18))) != a2) goto L4AA20;
L4AA0E:;
    if ((short)*(int *)&l_14 == *(short *)(holiday_days + (((int)(short)l_18) * 2))) goto L4AA22;
L4AA20:;
    goto L4AA2C;
L4AA22:;
    return ((int)(short)l_18) + 1;
L4AA2C:;
    (*(int *)&l_18)++;
    goto L4A9DB;
L4AA34:;
    return 0;
}

int holiday_name(int a1, int a2)
{
    return *(int *)(holiday_names + (func_0004A8DB(a1, a2) << 2));
}
