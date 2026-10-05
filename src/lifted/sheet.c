/* sheet.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char text_shadow_colour;
extern signed char D_0012B508;
extern short font_height;
extern signed char key_down_esc;
extern char D_00170C40[];
extern char D_00170C67[];
extern char D_00170C79[];
extern char D_00170CA7[];
extern char D_00170CB1[];
extern char D_00170CC0[];
extern char D_00170CDB[];
extern char D_00170CEC[];
extern char D_00170CFD[];
extern char D_00170D0F[];
extern char D_00170D4C[];
extern short msgbox_wrap_width;
extern short skill_advance_multipliers[];
extern char sheet_buttons[];
extern char D_0017B508[];
extern char D_0017B50A[];
extern char D_0017B50C[];
extern char D_0017B50E[];
extern short D_0017B604;
extern short D_0017B608;
extern short D_0017B610;
extern short D_0017B614;
extern char skill_names[];
extern signed char skill_governing_attributes[];
extern int attribute_abbrevs[];
extern char *faction_rank_names[];
extern int D_0017D1EA;
extern int D_00182682;
extern int D_00184225;
extern int D_00185073;
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern signed char D_001903A5;
extern char D_00190BE4[];
extern int D_00190C34;
extern char scratch_190d64[];
extern short D_00190D6A;
extern short D_00190D8C[];
extern char scratch_190de4[];
extern signed char D_001940D4;
extern signed char D_001940D8;
extern signed char D_001940D9;
extern struct record *player_entity;
extern struct record *player_object;
extern int creature_count;
extern char D_00195B5C[];
extern char D_00195B84[];
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern int game_minutes;
extern char scratch_buffer[];
extern int trade_price;
extern char text_rsc_file[];
extern int text_rsc_main_file;
extern short text_cursor_x;
extern signed char D_0019626C;
extern unsigned char D_0019626F;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char D_001962A2;
extern char D_001962A7;
extern char D_00199638[];
extern int D_0019963C;
extern int text_macro_skill;
extern signed char sheet_hth_damage_line;
extern struct membership *guild_membership;

extern struct faction *faction_find(short);
extern int list_popup_poll(void);
extern int sheet_open(int);
extern int level_skill_sum(void);
extern int skill_ready_to_advance(short, short, int, int);
extern int text_rsc_load(int, int, int);
extern int spellbook_open(int);
extern int career_specials_text(void);
extern int sound_play(int, struct record *, int);
extern int logbook_open(int);
extern int disk_read_file(int, int);
extern int disk_open_data(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int inventory_open(int, int, int);
extern int close();
extern int mc_free();
extern int mc_strncpy();
extern int itoa();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A1054();
extern int memchr();
extern int xn_draw_fullscreen_overlay_shaded();
extern int xn_gfx_present_inclusive();
extern int xn_mouse_poll_clamped();
extern int xn_font_select();
extern int xn_draw_image_transparent();
extern void career_show_biography(void);
extern void sheet_draw(void);
extern void sheet_show_career_skills(int, int);
extern void health_status_add(int);
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void time_pass(int);
extern void paperdoll_draw(int, int);
extern void disease_remove_skill_bonuses(void);
extern void disease_restore_skill_bonuses(void);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern void list_popup_open_strings(int);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern void inpstr_begin_text(char *, short);
extern void object_foreach(struct record *, int);
int sheet_close(void);
int skill_raised_recently(int);
int health_status_text(void);
int career_skill_mastered(void);
void sheet_affiliation_line(struct record *);
void sheet_place_spinner(int);
void skill_add_uses(int, int);
#pragma aux mc_set_location parm routine [];

int sheet_close(void)
{
    int l_20;
    int l_1C;

    if (((struct bf8_2_1 *)&D_001940D9)->f != 0 && *(short *)scratch_190d64 != 0) {
        l_20 = 0;
        l_1C = l_20;
        for (; l_20 < 8; l_20++) {
            l_1C += player_character->attributes[l_20];
        }
        if (l_1C < 800) {
            msgbox_show_string(D_00184225, 1);
            return 0;
        }
    }
    while (key_down_esc != 0);
    player_character->skills_raised_lo = (player_character->skills_raised_hi = 0);
    D_00187CA8 = 1;
    D_001940D9 &= 251;
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00170C67, 128);
        window_image = -1751672937;
    }
    if (*(int *)D_00199638 != 0 && *(int *)D_00199638 != (-1751672937)) {
        mc_free(*(int *)D_00199638, (int)D_00170C67, 129);
        *(int *)D_00199638 = -1751672937;
    }
    if (*(int *)D_00195B5C != 0 && *(int *)D_00195B5C != (-1751672937)) {
        mc_free(*(int *)D_00195B5C, (int)D_00170C67, 130);
        *(int *)D_00195B5C = -1751672937;
    }
    D_00196272 = 0;
    return 1;
}

void sheet_update(void)
{
    int l_18;

    if (sheet_open(0) == 0) return;
    xn_draw_fullscreen_overlay_shaded(window_image);
    paperdoll_draw(0, 0);
    xn_font_select(4);
    sheet_draw();
    if (key_down_esc != 0) sheet_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    D_0012B508 = 146;
    text_shadow_colour = 92;
    for (l_18 = 0; l_18 < 23; l_18++) {
        if (mouse_x > *(short *)(sheet_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_0017B50A + (l_18 * 12)) && mouse_y > *(short *)(D_0017B508 + (l_18 * 12)) && mouse_y < *(short *)(D_0017B50C + (l_18 * 12))) {
            if (l_18 < 12 && ((struct bf8_2_1 *)&D_001940D9)->f != 0) continue;
            sound_play(203, player_object, 110);
            ((int (*)())(*(int *)(D_0017B50E + (l_18 * 12))))(l_18);
        }
    }
}

void sheet_rename(void)
{
    int l_18;

    l_18 = *(int *)scratch_buffer + 55000;
    mc_set_location(232, (int)D_00170C67);
    mc_sprintf(l_18, (int)D_00170C79, D_0017D1EA);
    *(signed char *)((char *)(strlen(l_18) + l_18) + 1) = 0;
    inpstr_begin_text(player_character->name, 23);
    msgbox_show_string(l_18, 2);
}

void sheet_button_inventory(void)
{
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    sheet_close();
    inventory_open(1, 0, 2);
    D_001940D8 |= 32;
}

void sheet_affiliation_line(struct record *a1)
{
    struct membership *l_1C;
    struct faction *l_18;

    if (a1->type != 10) return;
    l_1C = &a1->data.membership;
    l_18 = faction_find(l_1C->faction);
    mc_set_location(255, (int)D_00170C67);
    mc_sprintf((int)text_buffer, (int)D_00170CA7, l_18->name, *(int *)((char *)(int)(faction_rank_names[((int)(unsigned char)(l_1C->kind & 31))] + (l_1C->rank << 2))));
    func_000A1054(D_0019963C, (int)text_buffer, (int)D_00170C67, 256, 4);
    text_cursor_x++;
}

void sheet_show_affiliations(void)
{
    text_cursor_x = 0;
    D_0019963C = *(int *)scratch_buffer + 55000;
    mc_set_location(264, (int)D_00170C67);
    mc_sprintf(D_0019963C, (int)D_00170CB1, D_00182682);
    object_foreach(player_entity->children, (int)sheet_affiliation_line);
    if (text_cursor_x == 0) {
        msgbox_show_rsc(19, 1);
        return;
    }
    msgbox_wrap_width = 250;
    *(signed char *)((char *)(int)(strlen(D_0019963C) + *(char **)&D_0019963C) - 1) = 0;
    msgbox_show_string(D_0019963C, 1);
    msgbox_wrap_width = 310;
}

void sheet_button_spellbook(void)
{
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    sheet_close();
    spellbook_open(1);
    D_001940D8 |= 32;
}

void sheet_show_primary_skills(void)
{
    sheet_show_career_skills(0, 0);
}

void sheet_show_major_skills(void)
{
    sheet_show_career_skills(3, 0);
}

void sheet_show_minor_skills(void)
{
    sheet_show_career_skills(6, 1);
}

void sheet_format_skill(int a1, short a2, int a3)
{
    {
        int l_1C;

        if (((int)(short)a2) == 30) sheet_hth_damage_line = 1;
        mc_set_location(349, (int)D_00170C67);
        mc_sprintf((int)text_buffer, (int)D_00170CDB, *(int *)(skill_names + (((int)(short)a2) << 2)), player_character->skills[(int)(short)a2].value, attribute_abbrevs[((int)(unsigned char)skill_governing_attributes[(int)(short)a2])]);
        if (a3 != 0) {
            l_1C = 96;
        } else {
            l_1C = 145;
        }
        D_001903A5 = *(signed char *)&l_1C;
        func_000A1054(a1, (int)text_buffer, (int)D_00170C67, 351, 4);
    }
}

void sheet_show_misc_skills(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    short l_18;

    l_24 = *(int *)scratch_buffer + 55000;
    sheet_hth_damage_line = 0;
    *(signed char *)((char *)l_24) = 0;
    l_20 = 0;
    l_1C = l_20;
    for (; ((int)(short)*(short *)&l_20) < 35; l_20++) {
        if (memchr(player_class->skills, (int)(short)*(short *)&l_20, 12) == 0) {
            *(int *)&l_18 = skill_raised_recently((int)(short)*(short *)&l_20);
            if (((int)(short)(l_1C & 1)) != 0) {
                if (((int)(short)*(short *)&l_20) == 30) sheet_hth_damage_line = 1;
                mc_set_location(372, (int)D_00170C67);
                mc_sprintf((int)text_buffer, (int)D_00170CEC, *(int *)(skill_names + (((int)(short)*(short *)&l_20) << 2)), player_character->skills[(int)(short)*(short *)&l_20].value, attribute_abbrevs[((int)(unsigned char)skill_governing_attributes[(int)(short)*(short *)&l_20])]);
                if (l_18 != 0) {
                    l_28 = 96;
                } else {
                    l_28 = 145;
                }
                D_001903A5 = *(signed char *)&l_28;
                func_000A1054(l_24, (int)text_buffer, (int)D_00170C67, 374, 4);
            } else {
                if (((int)(short)*(short *)&l_20) == 30) sheet_hth_damage_line = 1;
                mc_set_location(379, (int)D_00170C67);
                mc_sprintf((int)text_buffer, (int)D_00170CFD, *(int *)(skill_names + (((int)(short)*(short *)&l_20) << 2)), player_character->skills[(int)(short)*(short *)&l_20].value, attribute_abbrevs[((int)(unsigned char)skill_governing_attributes[(int)(short)*(short *)&l_20])]);
                if (l_18 != 0) {
                    l_2C = 96;
                } else {
                    l_2C = 145;
                }
                D_001903A5 = *(signed char *)&l_2C;
                func_000A1054(l_24, (int)text_buffer, (int)D_00170C67, 381, 4);
            }
            l_1C++;
        }
    }
    if (sheet_hth_damage_line != 0) {
        mc_set_location(389, (int)D_00170C67);
        mc_sprintf((int)text_buffer, (int)D_00170CC0, (player_character->skills[30].value / 10) + 1, (player_character->skills[30].value / 5) + 1);
        D_001903A5 = 96;
        func_000A1054(l_24, (int)text_buffer, (int)D_00170C67, 391, 4);
    }
    if (((int)(short)(l_1C & 1)) != 0) {
        *(signed char *)((char *)(strlen(l_24) + l_24) - 2) = 0;
    }
    *(signed char *)((char *)(strlen(l_24) + l_24) - 1) = 0;
    xn_draw_fullscreen_overlay_shaded(window_image);
    paperdoll_draw(0, 0);
    xn_font_select(4);
    sheet_draw();
    xn_gfx_present_inclusive(1);
    msgbox_show_string(l_24, 1);
}

int skill_raised_recently(int a1)
{
    if (a1 < 32) return (1 << a1) & (int)player_character->skills_raised_lo;
    return (1 << (a1 - 32)) & (int)player_character->skills_raised_hi;
}

void sheet_button_history(void)
{
    int l_18;

    l_18 = career_specials_text();
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    if (*(signed char *)((char *)l_18) != 0) msgbox_show_string(l_18, 1);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00170C67, 428);
        window_image = -1751672937;
    }
    career_show_biography();
    window_image = disk_read_file((int)D_00170C40, 0);
}

void sheet_button_log(void)
{
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    sheet_close();
    logbook_open(1);
    D_001940D8 |= 32;
}

void show_health_status(void)
{
    int l_18;

    l_18 = health_status_text();
    if (l_18 == 0) {
        msgbox_show_rsc(18, 1);
        return;
    }
    msgbox_show_string(l_18, 1);
}

void sheet_select_attribute(int a1)
{
    D_0019626C = *(signed char *)&a1 - 13;
    if (((struct bf8_2_1 *)&D_001940D9)->f == 0 || (((int)(unsigned char)(mouse_buttons & 2)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 2)) == 0)) {
        msgbox_show_rsc((int)(short)((int)(unsigned char)D_0019626C), 1);
    }
    sheet_place_spinner(a1);
}

void sheet_place_spinner(int a1)
{
    D_0017B604 = (D_00190D6A = *(short *)(D_0017B508 + (a1 * 12)) + 1);
    D_0017B608 = D_00190D6A + 6;
    D_0017B610 = D_00190D6A + 13;
    D_0017B614 = D_00190D6A + 19;
}

int health_status_text(void)
{
    int l_24;
    int l_20;
    short l_18;

    D_001962A7 = (D_001962A2 = 0);
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)health_status_add);
    if (*(int *)D_00195B84 == 0) return 0;
    l_20 = (int)(*(char **)scratch_buffer + 55000);
    l_18 = 0;
    *(signed char *)((char *)l_20) = *(signed char *)&l_18;
    for (; ((int)(short)l_18) < *(int *)D_00195B84; l_18++) {
        l_24 = text_rsc_load((int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)(int)(((int)(short)l_18) + *(char **)scratch_buffer) + 60000)), 0, 310);
        func_000A1054(l_20, l_24, (int)D_00170C67, 525, 4);
        if (l_24 != 0 && l_24 != (-1751672937)) {
            mc_free(l_24, (int)D_00170C67, 526);
            l_24 = -1751672937;
        }
        func_000A1054(l_20, (int)D_00170D0F, (int)D_00170C67, 527, 4);
    }
    *(signed char *)((char *)(strlen(l_20) + l_20) + 1) = 0;
    return l_20;
}

void sheet_draw_levelup_points(void)
{
    if (((struct bf8_2_1 *)&D_001940D9)->f == 0) return;
    xn_draw_image_transparent(176, (int)(short)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 6), *(int *)D_00195B5C + 12);
    text_draw_centred_coloured(itoa((int)(short)*(short *)scratch_190d64, (int)text_buffer, 10), 182, (int)(short)(((D_00190D6A + 13) - font_height) + 1), 145, 141);
}

void sheet_levelup_adjust(int a1)
{
    int l_1C;
    int l_18;

    if (((struct bf8_2_1 *)&D_001940D9)->f == 0) return;
    l_1C = 1132;
    if (((unsigned)(*(int *)((char *)l_1C) - *(int *)D_00190BE4)) < 6) return;
    l_18 = 1132;
    *(int *)D_00190BE4 = *(int *)((char *)l_18);
    if (a1 == 21) {
        if (*(short *)scratch_190d64 != 0 && player_character->base_attributes[(int)(unsigned char)D_0019626C] < 100) {
            (*(short *)scratch_190d64)--;
            player_character->attributes[(int)(unsigned char)D_0019626C]++;
            player_character->base_attributes[(int)(unsigned char)D_0019626C]++;
        }
        return;
    }
    if (player_character->base_attributes[(int)(unsigned char)D_0019626C] <= D_00190D8C[((int)(unsigned char)D_0019626C)]) return;
    (*(short *)scratch_190d64)++;
    player_character->attributes[(int)(unsigned char)D_0019626C]--;
    player_character->base_attributes[(int)(unsigned char)D_0019626C]--;
}

void training_offer(int a1)
{
    if (((unsigned)(game_minutes - player_character->last_training_minutes)) < 720) {
        msgbox_show_rsc(4023, 1);
        return;
    }
    *(int *)scratch_190de4 = a1;
    D_00190C34 = 1234;
    game_mode = 23;
    D_00196272 = 1;
    trade_price = player_character->level * 100;
    if (guild_membership == 0) trade_price <<= 2;
    msgbox_yes_no_rsc(8);
}

void training_update(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (((int)(unsigned char)game_mode) != 23) return;
    switch (D_00190C34) {
    case 1234:
        if (((int)D_00196271) == 2) {
            D_00190C34 = 1235;
            return;
        }
        if (gold_can_afford(trade_price) == 0) {
            msgbox_show_rsc(454, 1);
            D_00190C34 = 1235;
            return;
        }
        D_001940D8 |= 1;
        l_20 = *(int *)scratch_190de4;
        l_18 = 0;
        l_1C = *(int *)scratch_buffer;
        while (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_18))) != 255) {
            mc_strncpy(l_1C, *(int *)(skill_names + (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_18))) << 2)), 4, (int)D_00170C67, 615);
            l_1C += strlen(l_1C) + 1;
            l_18++;
        }
        *(signed char *)((char *)l_1C) = 0;
        list_popup_open_strings(*(int *)scratch_buffer);
        D_00190C34 = 1236;
        return;
    case 1235:
        D_00196272 = 0;
        game_mode = 0;
        return;
    case 1236:
        if (((struct bf8_2_1 *)&D_001940D4)->f != 0 && (l_18 = list_popup_poll()) > (-1)) {
            l_20 = *(int *)scratch_190de4;
            if (player_character->skills[(int)(unsigned char)*(signed char *)((char *)(l_20 + l_18))].value > 50) {
                msgbox_show_rsc(4022, 1);
                D_00190C34 = 1235;
                return;
            }
            gold_spend(trade_price);
            player_character->last_training_minutes = game_minutes;
            skill_add_uses((int)(unsigned char)*(signed char *)((char *)(l_20 + l_18)), rand_range(10, 20) * ((int)(short)skill_advance_multipliers[((int)(unsigned char)*(signed char *)((char *)(l_20 + l_18)))]));
            D_0012B508 = 146;
            msgbox_show_rsc(5221, 1);
            D_00190C34 = 1235;
            time_pass(180);
        }
        if (((struct bf8_2_1 *)&D_001940D4)->f != 0) return;
        D_00190C34 = 1235;
    default:;
    }
}

void raise_skills(void)
{
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = career_skill_mastered();
    player_character->skills_raised_lo = (player_character->skills_raised_hi = 0);
    disease_remove_skill_bonuses();
    for (l_24 = 0; l_24 < 35; l_24++) {
        if (skill_ready_to_advance(player_character->skills[l_24].value, player_character->skills[l_24].uses, player_class->advancement_multiplier, l_24) != 0) {
            player_character->skills[l_24].uses = 0;
            l_28 = player_character->skills[l_24].value;
            if (l_28 >= 95 && l_18 != 0) continue;
            if (player_character->skills[l_24].value < 100) {
                mc_set_location(674, (int)D_00170C67);
                mc_sprintf((int)text_buffer, D_00185073, *(int *)(skill_names + (l_24 << 2)));
                hud_message_add((int)text_buffer);
                player_character->skills[l_24].value++;
                if (l_24 < 32) {
                    player_character->skills_raised_lo |= 1 << l_24;
                } else {
                    player_character->skills_raised_hi |= 1 << (l_24 - 32);
                }
            } else {
                l_28 = 0;
                for (l_2C = l_28; l_2C < 6; l_2C++) {
                    if (player_class->skills[l_2C] == l_24) l_28++;
                }
                if (l_28 != 0) {
                    text_macro_skill = l_24;
                    l_1C = 1;
                }
            }
        }
    }
    for (l_24 = 0; l_24 < 8; l_24++) {
        if (player_character->attributes[l_24] > 100 || player_character->base_attributes[l_24] > 100) {
            player_character->attributes[l_24] = (player_character->base_attributes[l_24] = 100);
        }
    }
    for (l_24 = 0; l_24 < 35; l_24++) {
        if (player_character->skills[l_24].value > 100) player_character->skills[l_24].value = 100;
    }
    l_20 = ((int)&*(signed char *)((char *)(level_skill_sum() - player_character->level_skill_sum_start) + 28)) / 15;
    disease_restore_skill_bonuses();
    if (l_20 < 1) l_20 = 1;
    if (l_1C != 0) {
        msgbox_show_rsc(4020, 1);
        sound_play(34, player_object, 100);
    }
    if (player_character->level == l_20) return;
    if (player_character->level < l_20) {
        player_character->level++;
        D_001940D9 |= 4;
        return;
    }
    player_character->level--;
}

void levelup_check(void)
{
    if (((struct bf8_2_1 *)&D_001940D9)->f == 0 || creature_count != 0 || game_mode != 0 || ((int)D_0019626F) != 16 || player_character->race >= 9) {
        return;
    }
    sheet_open(1);
}

void skill_add_uses(int a1, int a2)
{
    player_character->skills[a1].uses += a2;
    if (player_character->skills[a1].uses < 0) {
        player_character->skills[a1].uses = 0;
        return;
    }
    if (player_character->skills[a1].uses <= 20000) return;
    player_character->skills[a1].uses = 20000;
}

int career_skill_mastered(void)
{
    int l_1C;

    for (l_1C = 0; l_1C < 6; l_1C++) {
        if (player_character->skills[player_class->skills[l_1C]].value == 100) return 1;
    }
    return 0;
}

int text_rsc_open(void)
{
    int l_1C;

    text_rsc_main_file = (*(int *)text_rsc_file = disk_open_data((int)D_00170D4C));
    if (*(int *)text_rsc_file > 0) {
        l_1C = 1;
    } else {
        l_1C = 0;
    }
    return l_1C;
}

void text_rsc_close(void)
{
    close(*(int *)text_rsc_file);
}
