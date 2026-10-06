/* sheet.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

#include "records.h"
#include "bitfield.h"

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
extern struct rect sheet_buttons[];
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
extern char scratch_190be4[];
extern int D_00190C34;
extern char scratch_190d64[];
extern short scratch_190d6a;
extern short D_00190D8C[];
extern char scratch_190de4[];
extern signed char D_001940D4;
extern signed char D_001940D8;
extern signed char D_001940D9;
extern struct record *player_entity;
extern struct record *player_object;
extern int creature_count;
extern struct image *D_00195B5C;
extern char D_00195B84[];
extern struct character *player_character;
extern iptr window_image;
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
extern struct image *D_00199638;
extern iptr D_0019963C;
extern int text_macro_skill;
extern signed char sheet_hth_damage_line;
extern struct membership *guild_membership;

extern struct faction *faction_find(short);
extern int list_popup_poll(void);
extern int sheet_open(short);
extern int level_skill_sum(void);
extern int skill_ready_to_advance(int, int, int, int);
extern iptr text_rsc_load(int, int, int);
extern int spellbook_open(short);
extern iptr career_specials_text(void);
extern int sound_play(int, struct record *, int);
extern int logbook_open(int);
extern iptr disk_read_file(char *, iptr);
extern int disk_open_data(char *);
extern iptr hud_message_add(char *);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int inventory_open(int, int, int);
extern int close();
extern int mc_free();
extern int mc_strncpy();
extern int itoa();
extern int strlen();
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern int func_000A1054();
extern iptr memchr();
extern int xn_draw_fullscreen_overlay_shaded();
extern int xn_gfx_present_inclusive();
extern int xn_mouse_poll_clamped();
extern int xn_font_select();
extern int xn_draw_image_transparent();
extern void career_show_biography(void);
extern void sheet_draw(void);
extern void sheet_show_career_skills(short, short);
extern void health_status_add(struct record *);
extern void msgbox_show_string(iptr, int);
extern void msgbox_show_rsc(int, int);
extern void time_pass(int);
extern void paperdoll_draw(int, int);
extern void disease_remove_skill_bonuses(void);
extern void disease_restore_skill_bonuses(void);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern void list_popup_open_strings(int);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern void inpstr_begin_text(char *, int);
extern void object_foreach(struct record *, void (*)());
int sheet_close(void);
int skill_raised_recently(int);
iptr health_status_text(void);
int career_skill_mastered(void);
void sheet_affiliation_line(struct record *);
void sheet_place_spinner(int);
void skill_add_uses(int, int);
#pragma aux mc_set_location parm routine [];

int sheet_close(void)
{
    int i;
    int total;

    if (((struct bf8_2_1 *)&D_001940D9)->f != 0 && *(short *)scratch_190d64 != 0) {
        i = 0;
        total = i;
        for (; i < 8; i++) {
            total += player_character->attributes[i];
        }
        if (total < 800) {
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
        mc_free(window_image, (iptr)D_00170C67, 128);
        window_image = -1751672937;
    }
    if ((iptr)D_00199638 != 0 && (iptr)D_00199638 != (-1751672937)) {
        mc_free((iptr)D_00199638, (iptr)D_00170C67, 129);
        D_00199638 = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_00195B5C != 0 && (iptr)D_00195B5C != (-1751672937)) {
        mc_free((iptr)D_00195B5C, (iptr)D_00170C67, 130);
        D_00195B5C = (struct image *)(iptr)-1751672937;
    }
    D_00196272 = 0;
    return 1;
}

void sheet_update(void)
{
    int button;

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
    for (button = 0; button < 23; button++) {
        if (mouse_x > sheet_buttons[button].x0 && mouse_x < sheet_buttons[button].x1 && mouse_y > sheet_buttons[button].y0 && mouse_y < sheet_buttons[button].y1) {
            if (button < 12 && ((struct bf8_2_1 *)&D_001940D9)->f != 0) continue;
            sound_play(203, player_object, 110);
            sheet_buttons[button].handler(button);
        }
    }
}

void sheet_rename(void)
{
    char *text;

    text = *(char **)scratch_buffer + 55000;
    mc_set_location(232, (iptr)D_00170C67);
    mc_sprintf((iptr)text, (iptr)D_00170C79, D_0017D1EA);
    *(strlen(text) + text + 1) = 0;
    inpstr_begin_text(player_character->name, 23);
    msgbox_show_string((iptr)text, 2);
}

void sheet_button_inventory(void)
{
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    sheet_close();
    inventory_open(1, 0, 2);
    D_001940D8 |= 32;
}

void sheet_affiliation_line(struct record *object)
{
    struct membership *membership;
    struct faction *faction;

    if (object->type != 10) return;
    membership = &object->data.membership;
    faction = faction_find(membership->faction);
    mc_set_location(255, (iptr)D_00170C67);
    mc_sprintf((iptr)text_buffer, (iptr)D_00170CA7, faction->name, *(int *)((char *)(iptr)(faction_rank_names[((int)(unsigned char)(membership->kind & 31))] + (membership->rank << 2))));
    func_000A1054(D_0019963C, (iptr)text_buffer, (iptr)D_00170C67, 256, 4);
    text_cursor_x++;
}

void sheet_show_affiliations(void)
{
    text_cursor_x = 0;
    D_0019963C = *(int *)scratch_buffer + 55000;
    mc_set_location(264, (iptr)D_00170C67);
    mc_sprintf(D_0019963C, (iptr)D_00170CB1, D_00182682);
    object_foreach(player_entity->children, sheet_affiliation_line);
    if (text_cursor_x == 0) {
        msgbox_show_rsc(19, 1);
        return;
    }
    msgbox_wrap_width = 250;
    *(signed char *)((char *)(iptr)(strlen(D_0019963C) + *(char **)&D_0019963C) - 1) = 0;
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

void sheet_format_skill(int text, short skill, int raised)
{
    {
        int colour;

        if (((int)(short)skill) == 30) sheet_hth_damage_line = 1;
        mc_set_location(349, (iptr)D_00170C67);
        mc_sprintf((iptr)text_buffer, (iptr)D_00170CDB, *(int *)(skill_names + (((int)(short)skill) << 2)), player_character->skills[(int)(short)skill].value, attribute_abbrevs[((int)(unsigned char)skill_governing_attributes[(int)(short)skill])]);
        if (raised != 0) {
            colour = 96;
        } else {
            colour = 145;
        }
        D_001903A5 = *(signed char *)&colour;
        func_000A1054(text, (iptr)text_buffer, (iptr)D_00170C67, 351, 4);
    }
}

void sheet_show_misc_skills(void)
{
    int right_colour;
    int left_colour;
    char *text;
    int skill;
    int count;
    short raised;

    text = *(char **)scratch_buffer + 55000;
    sheet_hth_damage_line = 0;
    *text = 0;
    skill = 0;
    count = skill;
    for (; ((int)(short)*(short *)&skill) < 35; skill++) {
        if (memchr(player_class->skills, (int)(short)*(short *)&skill, 12) == 0) {
            *(int *)&raised = skill_raised_recently((int)(short)*(short *)&skill);
            if (((int)(short)(count & 1)) != 0) {
                if (((int)(short)*(short *)&skill) == 30) sheet_hth_damage_line = 1;
                mc_set_location(372, (iptr)D_00170C67);
                mc_sprintf((iptr)text_buffer, (iptr)D_00170CEC, *(int *)(skill_names + (((int)(short)*(short *)&skill) << 2)), player_character->skills[(int)(short)*(short *)&skill].value, attribute_abbrevs[((int)(unsigned char)skill_governing_attributes[(int)(short)*(short *)&skill])]);
                if (raised != 0) {
                    left_colour = 96;
                } else {
                    left_colour = 145;
                }
                D_001903A5 = *(signed char *)&left_colour;
                func_000A1054((iptr)text, (iptr)text_buffer, (iptr)D_00170C67, 374, 4);
            } else {
                if (((int)(short)*(short *)&skill) == 30) sheet_hth_damage_line = 1;
                mc_set_location(379, (iptr)D_00170C67);
                mc_sprintf((iptr)text_buffer, (iptr)D_00170CFD, *(int *)(skill_names + (((int)(short)*(short *)&skill) << 2)), player_character->skills[(int)(short)*(short *)&skill].value, attribute_abbrevs[((int)(unsigned char)skill_governing_attributes[(int)(short)*(short *)&skill])]);
                if (raised != 0) {
                    right_colour = 96;
                } else {
                    right_colour = 145;
                }
                D_001903A5 = *(signed char *)&right_colour;
                func_000A1054((iptr)text, (iptr)text_buffer, (iptr)D_00170C67, 381, 4);
            }
            count++;
        }
    }
    if (sheet_hth_damage_line != 0) {
        mc_set_location(389, (iptr)D_00170C67);
        mc_sprintf((iptr)text_buffer, (iptr)D_00170CC0, (player_character->skills[30].value / 10) + 1, (player_character->skills[30].value / 5) + 1);
        D_001903A5 = 96;
        func_000A1054((iptr)text, (iptr)text_buffer, (iptr)D_00170C67, 391, 4);
    }
    if (((int)(short)(count & 1)) != 0) {
        *(strlen(text) + text - 2) = 0;
    }
    *(strlen(text) + text - 1) = 0;
    xn_draw_fullscreen_overlay_shaded(window_image);
    paperdoll_draw(0, 0);
    xn_font_select(4);
    sheet_draw();
    xn_gfx_present_inclusive(1);
    msgbox_show_string((iptr)text, 1);
}

int skill_raised_recently(int skill)
{
    if (skill < 32) return (1 << skill) & (int)player_character->skills_raised_lo;
    return (1 << (skill - 32)) & (int)player_character->skills_raised_hi;
}

void sheet_button_history(void)
{
    char *text;

    text = (char *)career_specials_text();
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    if (*text != 0) msgbox_show_string((iptr)text, 1);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (iptr)D_00170C67, 428);
        window_image = -1751672937;
    }
    career_show_biography();
    window_image = disk_read_file(D_00170C40, 0);
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
    iptr text;

    text = health_status_text();
    if (text == 0) {
        msgbox_show_rsc(18, 1);
        return;
    }
    msgbox_show_string(text, 1);
}

void sheet_select_attribute(int button)
{
    D_0019626C = *(signed char *)&button - 13;
    if (((struct bf8_2_1 *)&D_001940D9)->f == 0 || (((int)(unsigned char)(mouse_buttons & 2)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 2)) == 0)) {
        msgbox_show_rsc((int)(short)((int)(unsigned char)D_0019626C), 1);
    }
    sheet_place_spinner(button);
}

void sheet_place_spinner(int button)
{
    sheet_buttons[21].y0 = (scratch_190d6a = sheet_buttons[button].y0 + 1);
    sheet_buttons[21].y1 = scratch_190d6a + 6;
    sheet_buttons[22].y0 = scratch_190d6a + 13;
    sheet_buttons[22].y1 = scratch_190d6a + 19;
}

iptr health_status_text(void)
{
    iptr line;
    char *text;
    short i;

    D_001962A7 = (D_001962A2 = 0);
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, health_status_add);
    if (*(int *)D_00195B84 == 0) return 0;
    text = *(char **)scratch_buffer + 55000;
    i = 0;
    *text = *(signed char *)&i;
    for (; ((int)(short)i) < *(int *)D_00195B84; i++) {
        line = text_rsc_load((int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)(iptr)(((int)(short)i) + *(char **)scratch_buffer) + 60000)), 0, 310);
        func_000A1054((iptr)text, line, (iptr)D_00170C67, 525, 4);
        if (line != 0 && line != (-1751672937)) {
            mc_free(line, (iptr)D_00170C67, 526);
            line = -1751672937;
        }
        func_000A1054((iptr)text, (iptr)D_00170D0F, (iptr)D_00170C67, 527, 4);
    }
    *(strlen(text) + text + 1) = 0;
    return (iptr)text;
}

void sheet_draw_levelup_points(void)
{
    if (((struct bf8_2_1 *)&D_001940D9)->f == 0) return;
    xn_draw_image_transparent(176, (int)(short)scratch_190d6a, D_00195B5C->width, D_00195B5C->height, (iptr)D_00195B5C->pixels);
    text_draw_centred_coloured(itoa((int)(short)*(short *)scratch_190d64, (iptr)text_buffer, 10), 182, (int)(short)(((scratch_190d6a + 13) - font_height) + 1), 145, 141);
}

void sheet_levelup_adjust(int button)
{
    int *bios_ticks;
    int *bios_ticks_now;

    if (((struct bf8_2_1 *)&D_001940D9)->f == 0) return;
    bios_ticks = (int *)1132;
    if (((unsigned)(*bios_ticks - *(int *)scratch_190be4)) < 6) return;
    bios_ticks_now = (int *)1132;
    *(int *)scratch_190be4 = *bios_ticks_now;
    if (button == 21) {
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

void training_offer(iptr skills)
{
    if (((unsigned)(game_minutes - player_character->last_training_minutes)) < 720) {
        msgbox_show_rsc(4023, 1);
        return;
    }
    *(iptr *)scratch_190de4 = skills;
    D_00190C34 = 1234;
    game_mode = 23;
    D_00196272 = 1;
    trade_price = player_character->level * 100;
    if (guild_membership == 0) trade_price <<= 2;
    msgbox_yes_no_rsc(8);
}

void training_update(void)
{
    unsigned char *skills;
    char *dest;
    int row;

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
        skills = *(unsigned char **)scratch_190de4;
        row = 0;
        dest = *(char **)scratch_buffer;
        while (skills[row] != 255) {
            mc_strncpy(dest, *(int *)(skill_names + (skills[row] << 2)), 4, (iptr)D_00170C67, 615);
            dest += strlen(dest) + 1;
            row++;
        }
        *dest = 0;
        list_popup_open_strings(*(int *)scratch_buffer);
        D_00190C34 = 1236;
        return;
    case 1235:
        D_00196272 = 0;
        game_mode = 0;
        return;
    case 1236:
        if (((struct bf8_2_1 *)&D_001940D4)->f != 0 && (row = list_popup_poll()) > (-1)) {
            skills = *(unsigned char **)scratch_190de4;
            if (player_character->skills[skills[row]].value > 50) {
                msgbox_show_rsc(4022, 1);
                D_00190C34 = 1235;
                return;
            }
            gold_spend(trade_price);
            player_character->last_training_minutes = game_minutes;
            skill_add_uses(skills[row], rand_range(10, 20) * ((int)(short)skill_advance_multipliers[skills[row]]));
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
    int unused_38;
    int unused_34;
    int unused_30;
    int i;
    int value;
    int skill;
    int level;
    int any_mastered;
    int mastered;

    any_mastered = 0;
    mastered = career_skill_mastered();
    player_character->skills_raised_lo = (player_character->skills_raised_hi = 0);
    disease_remove_skill_bonuses();
    for (skill = 0; skill < 35; skill++) {
        if (skill_ready_to_advance(player_character->skills[skill].value, player_character->skills[skill].uses, player_class->advancement_multiplier, skill) != 0) {
            player_character->skills[skill].uses = 0;
            value = player_character->skills[skill].value;
            if (value >= 95 && mastered != 0) continue;
            if (player_character->skills[skill].value < 100) {
                mc_set_location(674, (iptr)D_00170C67);
                mc_sprintf((iptr)text_buffer, D_00185073, *(int *)(skill_names + (skill << 2)));
                hud_message_add(text_buffer);
                player_character->skills[skill].value++;
                if (skill < 32) {
                    player_character->skills_raised_lo |= 1 << skill;
                } else {
                    player_character->skills_raised_hi |= 1 << (skill - 32);
                }
            } else {
                value = 0;
                for (i = value; i < 6; i++) {
                    if (player_class->skills[i] == skill) value++;
                }
                if (value != 0) {
                    text_macro_skill = skill;
                    any_mastered = 1;
                }
            }
        }
    }
    for (skill = 0; skill < 8; skill++) {
        if (player_character->attributes[skill] > 100 || player_character->base_attributes[skill] > 100) {
            player_character->attributes[skill] = (player_character->base_attributes[skill] = 100);
        }
    }
    for (skill = 0; skill < 35; skill++) {
        if (player_character->skills[skill].value > 100) player_character->skills[skill].value = 100;
    }
    level = ((int)(iptr)&*(signed char *)((char *)(iptr)(level_skill_sum() - player_character->level_skill_sum_start) + 28)) / 15;
    disease_restore_skill_bonuses();
    if (level < 1) level = 1;
    if (any_mastered != 0) {
        msgbox_show_rsc(4020, 1);
        sound_play(34, player_object, 100);
    }
    if (player_character->level == level) return;
    if (player_character->level < level) {
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

void skill_add_uses(int skill, int uses)
{
    player_character->skills[skill].uses += uses;
    if (player_character->skills[skill].uses < 0) {
        player_character->skills[skill].uses = 0;
        return;
    }
    if (player_character->skills[skill].uses <= 20000) return;
    player_character->skills[skill].uses = 20000;
}

int career_skill_mastered(void)
{
    int i;

    for (i = 0; i < 6; i++) {
        if (player_character->skills[player_class->skills[i]].value == 100) return 1;
    }
    return 0;
}

int text_rsc_open(void)
{
    int ok;

    text_rsc_main_file = (*(int *)text_rsc_file = disk_open_data(D_00170D4C));
    if (*(int *)text_rsc_file > 0) {
        ok = 1;
    } else {
        ok = 0;
    }
    return ok;
}

void text_rsc_close(void)
{
    close(*(int *)text_rsc_file);
}
