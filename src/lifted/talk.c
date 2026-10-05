/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern signed char mouse_buttons;
extern signed char D_0012AC02;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern signed char key_down_enter;
extern signed char key_down_up;
extern signed char key_down_pgup;
extern signed char key_down_down;
extern signed char key_down_pgdn;
extern short D_00142940;
extern short D_00142948;
extern int screen_buffer;
extern int D_00147954;
extern char D_001703F0[];
extern char D_001703F7[];
extern char D_0017041B[];
extern char D_0017041C[];
extern char D_0017042A[];
extern signed char D_00178630[];
extern unsigned char player_environment;
extern char talk_faction_greetings[];
extern char D_001799AE[];
extern char D_001799B0[];
extern short D_001799E2[];
extern char D_001799F2[];
extern char talk_where_answers[];
extern char talk_tell_answers[];
extern char talk_creature_language[];
extern char D_00179ACE[];
extern char talk_buttons[];
extern char D_00179B7A[];
extern char D_00179B7C[];
extern char D_00179B7E[];
extern char D_00179B80[];
extern char talk_greeting_texts[];
extern short talk_ack_texts[];
extern short talk_regional_ids[];
extern short D_00179CC8[];
extern signed char D_00179CFA[];
extern char talk_category_building_types[];
extern int talk_category_names[];
extern int talk_regional_names[];
extern char D_0017C668[];
extern signed char D_00187CA8;
extern signed char region_event_values[];
extern signed char D_0018F045[];
extern signed char D_0018F046[];
extern signed char region_event_flags[];
extern signed char D_0018F062[];
extern signed char D_0018F063[];
extern signed char D_0018F064[];
extern signed char region_event_groups[];
extern signed char text_buffer[];
extern char D_00190BE4[];
extern signed char D_00190D0F;
extern signed char D_00190D10;
extern signed char D_00190D11;
extern signed char text_rsc_buffer[];
extern char D_00191016[];
extern signed char D_001940D4;
extern char text_macro_fcn[];
extern struct record *D_00195A00;
extern struct character *D_00195A84;
extern struct record *player_object;
extern char D_00195B84[];
extern struct location *current_location;
extern struct character *player_character;
extern int window_image;
extern struct settings *game_settings;
extern struct record *D_00195D00;
extern char *D_00195D28;
extern int talk_disposition;
extern int talk_prostitute_price;
extern short text_macro_n_text;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern char D_00196488[];
extern signed char D_001964B0[];
extern char *talk_question_lines;
extern int talk_place_topic_count;
extern int D_00196578;
extern short *talk_topics;
extern char D_00196580[];
extern int talk_text_pool_next;
extern char *talk_answer_lines;
extern int talk_saved_screen;
extern struct faction *talk_npc_own_faction;
extern struct faction *talk_npc_faction;
extern char *talk_place_topics;
extern struct character *talk_npc_record;
extern int D_001965A0;
extern int talk_npc_speech_style;
extern int talk_answer_line_count;
extern int D_001965AC;
extern int talk_answer_scroll;
extern int talk_list_count;
extern int talk_npc_attitude;
extern int talk_npc_knows;
extern int D_001965C4;
extern int D_001965C8;
extern int talk_attitude_cache[];
extern int D_001965D4;
extern int talk_question_line_count;
extern int D_001965DC;
extern struct record *talk_npc_object;
extern int D_001965E4;
extern char talk_selected_row[];
extern int talk_face_image;
extern int talk_list_bottom;
extern int talk_list_top;
extern int D_001965F8;
extern signed char D_00196612[];
extern char talk_key_text[];
extern short D_001966A2;
extern short D_001966A4;
extern short D_001966A6;
extern short talk_language_skill;
extern char talk_flags[];
extern short D_001966AC;
extern signed char talk_question_mode;
extern signed char talk_showing_categories;
extern signed char talk_topic_tab;
extern signed char talk_tone;
extern signed char talk_redraw;
extern signed char talk_news_asked;
extern signed char talk_location_category;
extern unsigned char talk_prostitute_state;
extern signed char D_001966B7;
extern signed char D_001966B8;
extern unsigned char talk_prostitute_offer;
extern signed char D_001966BA;
extern signed char D_001966BB;
extern struct faction *D_0019670C;
extern int faction_count;
extern struct faction *D_0019671C;
extern struct faction *factions;
extern struct quest *current_quest;
extern struct quest *D_00199780;

extern int talk_open(int);
extern int talk_hint_text_id(int);
extern int town_has_building(short, int);
extern int building_distance(struct building *);
extern int talk_faction_relation(short);
extern struct faction *faction_find(short);
extern int rumor_pick_news(short);
extern int quest_symbol_text(int, int, int);
extern int quest_section(int, int);
extern int quest_find_site_for_building(struct building *);
extern int parse_bio_answer_text(int);
extern int quest_find_by_id(int);
extern int quest_find_potential_questor(void);
extern struct character *npc_talk_record_build(struct record *);
extern int font_char_width(unsigned char);
extern int font_text_width(int);
extern int func_000679BB(unsigned char);
extern int item_artifact_equipped(int);
extern int sound_play(int, int, int);
extern struct membership *guild_find_membership_by_bits(unsigned char);
extern int guild_local_temple_rank(void);
extern int hud_message_add(int);
extern int key_pressed_once(unsigned char);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int func_0008B48B(int);
extern int building_name(int);
extern int rand();
extern int srand();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A1054();
extern int memchr();
extern int func_0012B136();
extern int func_00144D00();
extern int func_00144F68();
extern void func_000164EF(void);
extern void talk_load_face(unsigned short);
extern void talk_draw_answer(short, int, int, int);
extern void talk_draw_question(short, int, int, int);
extern void talk_draw_tell_list(void);
extern void skill_add_uses(int, int);
extern void msgbox_show_rsc(int, int);
extern void parse_rsc_text(int, int, int);
extern void quest_load_text(int, int, int, int);
extern void time_pass(int);
extern void logbook_copy_text(int);
extern void text_draw_colored();
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void msgbox_yes_no_rsc(int);
extern void location_free(int);
extern void func_00087D71(int, int, short);
int talk_ask_news(void);
int func_000164AB(void);
int func_000164CD(void);
int func_000169D4(int);
int talk_next_word(int);
int talk_alloc_line(void);
int building_category(struct building *);
int town_has_category(int);
int talk_roll_knows(void);
int talk_roll_attitude(void);
struct faction *faction_nth_r(struct faction *);
struct faction *faction_nth_of_type_r(struct faction *, unsigned char);
int faction_is_regional_noble(struct faction *);
int faction_has_enemy(struct faction *, struct faction *);
int faction_has_ally(struct faction *, struct faction *);
int func_0001B144(struct faction *, struct faction *);
void talk_draw(void);
void talk_prepare_where_answer(void);
void talk_close(void);
void talk_list_scroll_up(void);
void talk_list_scroll_down(void);
void talk_say_text(int);
void talk_draw_face(void);
void talk_wrap_text(int, int, int, int, int);
void talk_log_question(void);
void talk_add_line(int);
void talk_clear_question(void);
void talk_draw_place_list(void);
void talk_list_click(int);
void talk_add_quest_topics(int);
void talk_list_draw_item(int, int, int, int, int);
void talk_draw_regional_list(void);
void talk_add_regional_item(int);
void talk_draw_categories(void);
void faction_count_of_type_r(struct faction *, unsigned char);
void func_0001AE63(struct faction *);
void faction_add_reputation(struct faction *, int);
void func_0001B554(struct faction *, struct faction *, int);
void func_0001B5BE(struct faction *, int);
#pragma aux func_000A0ED9 parm routine [];

void talk_update(void)
{
    int l_18;

    if (talk_open(0) == 0) return;
    if (talk_redraw == 0) {
        mc_memcpy(screen_buffer, talk_saved_screen, 64000, (int)D_001703F0, 324, 4);
    } else {
        mc_memcpy(screen_buffer, window_image, 64000, (int)D_001703F0, 326, 4);
    }
    if (talk_redraw != 0) talk_draw();
    if (key_down_up != 0) {
        talk_list_scroll_up();
    } else if (key_down_down != 0) {
        talk_list_scroll_down();
    } else if (key_down_pgup != 0) {
        for (l_18 = 0; l_18 < 10; l_18++) {
            talk_list_scroll_up();
        }
    } else if (key_down_pgdn != 0) {
        for (l_18 = 0; l_18 < 10; l_18++) {
            talk_list_scroll_down();
        }
    }
    if (((talk_topic_tab == 0 && talk_showing_categories != 0) || talk_topic_tab != 0) && key_down_esc != 0) {
        talk_close();
    }
    if (key_down_esc != 0) {
        talk_redraw = 1;
        do {
        } while (key_down_esc != 0);
        talk_showing_categories = 1;
    }
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0 || ((mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) && (key_down_enter == 0 && key_down_esc == 0))) {
        return;
    }
    if (D_001966BA != 0) {
        for (l_18 = 10; l_18 < 17; l_18++) {
            if (mouse_x > *(short *)(talk_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_00179B7C + (l_18 * 12)) && mouse_y > *(short *)(D_00179B7A + (l_18 * 12)) && mouse_y < *(short *)(D_00179B7E + (l_18 * 12))) {
                sound_play(203, (int)player_object, 110);
                ((int (*)())(*(int *)(D_00179B80 + (l_18 * 12))))(l_18);
            }
        }
        return;
    }
    for (l_18 = 0; l_18 < 19; l_18++) {
        if (mouse_x > *(short *)(talk_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_00179B7C + (l_18 * 12)) && mouse_y > *(short *)(D_00179B7A + (l_18 * 12)) && mouse_y < *(short *)(D_00179B7E + (l_18 * 12))) {
            if (mouse_buttons != 0 || l_18 == 15) {
                sound_play(203, (int)player_object, 110);
                ((int (*)())(*(int *)(D_00179B80 + (l_18 * 12))))(l_18);
            }
        }
    }
}

void talk_draw(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = rand();
    srand(D_001965C8);
    if (talk_npc_own_faction->type == 4) {
        text_draw_centered_colored((int)((char *)talk_npc_own_faction + 3), 213, 53, 145, 156);
    } else {
        text_draw_centered_colored(func_0008B48B((int)talk_npc_object), 213, 53, 145, 156);
    }
    talk_draw_face();
    talk_draw_answer(190, 66, 302, 184);
    talk_clear_question();
    D_00196578 = 0;
    D_0012B508 = 244;
    func_00144D00((int)(short)*(short *)(talk_buttons + ((((int)(unsigned char)talk_tone) + 6) * 12)), (int)(short)*(short *)(D_00179B7A + ((((int)(unsigned char)talk_tone) + 6) * 12)), (int)(short)((*(short *)(D_00179B7C + ((((int)(unsigned char)talk_tone) + 6) * 12)) - *(short *)(talk_buttons + ((((int)(unsigned char)talk_tone) + 6) * 12))) + 1), (int)(short)((*(short *)(D_00179B7E + ((((int)(unsigned char)talk_tone) + 6) * 12)) - *(short *)(D_00179B7A + ((((int)(unsigned char)talk_tone) + 6) * 12))) + 1));
    func_00144F68(4, 26, 107, 40, D_001965E4);
    if (talk_question_mode != 0) {
        for (l_20 = ((int)(short)*(short *)(D_00179B7A + ((((int)(unsigned char)talk_topic_tab) + 2) * 12))) * 320; (((int)(short)*(short *)(D_00179B7E + ((((int)(unsigned char)talk_topic_tab) + 2) * 12))) * 320) >= l_20; l_20 += 320) {
            mc_memcpy(((int)(short)*(short *)(talk_buttons + ((((int)(unsigned char)talk_topic_tab) + 2) * 12))) + (screen_buffer + l_20), (window_image + l_20) + ((int)(short)*(short *)(talk_buttons + ((((int)(unsigned char)talk_topic_tab) + 2) * 12))), 107, (int)D_001703F0, 417, 4);
        }
    }
    l_28 = ((int)(unsigned char)talk_question_mode) * 1070;
    for (l_20 = ((int)(short)*(short *)(D_00179B7A + (((int)(unsigned char)talk_question_mode) * 12))) * 320; (((int)(short)*(short *)(D_00179B7E + (((int)(unsigned char)talk_question_mode) * 12))) * 320) >= l_20; l_20 += 320, l_28 += 107) {
        mc_memcpy(((int)(short)*(short *)(talk_buttons + (((int)(unsigned char)talk_question_mode) * 12))) + (screen_buffer + l_20), D_001965F8 + l_28, 107, (int)D_001703F0, 423, 4);
    }
    D_001966B7 = 1;
    if (((int)(unsigned char)talk_topic_tab) == 3) {
        parse_rsc_text(7211, 0, 0);
        mc_strncpy((int)talk_key_text, (int)text_rsc_buffer, 4, (int)D_001703F0, 430);
        parse_rsc_text(((int)(unsigned char)talk_tone) + 7212, 0, 0);
        D_00190D0F = 1;
        mc_strncpy((int)(((char *)D_00147954) + 90000), (int)text_rsc_buffer, 4, (int)D_001703F0, 433);
        talk_wrap_text((int)(((char *)D_00147954) + 90000), 134, 10, 245, 45);
        D_00190D0F = 0;
    } else {
        switch ((unsigned char)talk_question_mode) {
            break;
        case 1:
            if (talk_showing_categories != 0 && talk_topic_tab == 0) {
                talk_draw_categories();
            } else {
                talk_draw_place_list();
                if (talk_list_count == 0) break;
                parse_rsc_text(((int)(unsigned char)talk_tone) + 7225, 0, 0);
                D_00190D0F = 1;
                mc_strncpy((int)(((char *)D_00147954) + 90000), (int)text_rsc_buffer, 4, (int)D_001703F0, 448);
                talk_wrap_text((int)(((char *)D_00147954) + 90000), 134, 10, 245, 45);
                D_00190D0F = 0;
            }
            break;
        case 0:
            talk_draw_tell_list();
            if (*(short *)((char *)(int)(((char *)talk_topics) + (*(int *)talk_selected_row * 6))) == 0 && (talk_topics)[*(int *)talk_selected_row * 3 + 1] == 0) {
                parse_rsc_text(((int)(unsigned char)talk_tone) + 7231, 0, 0);
            } else {
                parse_rsc_text(((int)(unsigned char)talk_tone) + 7212, 0, 0);
            }
            D_00190D0F = 1;
            mc_strncpy((int)(((char *)D_00147954) + 90000), (int)text_rsc_buffer, 4, (int)D_001703F0, 461);
            talk_wrap_text((int)(((char *)D_00147954) + 90000), 134, 10, 245, 45);
            D_00190D0F = 0;
        }
    }
    talk_draw_question(125, 10, 245, 45);
    mc_memcpy(talk_saved_screen, screen_buffer, 64000, (int)D_001703F0, 469, 4);
    talk_redraw = 0;
    srand(l_1C);
}

void talk_start(struct record *a1)
{
    struct faction *l_18;

    talk_face_image = 0;
    D_00190D10 = 0;
    talk_npc_object = a1;
    if (((int)(unsigned short)(a1->npc_flags & 32768)) != 0) {
        msgbox_show_rsc(7205, 1);
        return;
    }
    if (a1->type != 8 && a1->type != 18 && a1->type != 53) {
        msgbox_show_rsc(7204, 1);
        return;
    }
    if (a1->type == 53 && ((int)(unsigned short)(a1->image2 & 1)) != 0) {
        hud_message_add((int)D_001703F7);
        return;
    }
    *(short *)talk_flags = 0;
    if (a1->type == 8 || a1->type == 53) {
        talk_npc_record = npc_talk_record_build(a1);
        *(signed char *)talk_flags &= 254;
    } else {
        if ((talk_npc_record = &a1->data.character)->race < 43) *(signed char *)talk_flags |= 1;
        talk_language_skill = *(short *)(talk_creature_language + (talk_npc_record->race << 2));
        D_001966A6 = *(short *)(D_00179ACE + (talk_npc_record->race << 2));
        if (talk_language_skill == 0) {
            msgbox_show_rsc(7204, 1);
            return;
        }
        if (((int)(unsigned short)talk_language_skill) == 1) {
            *(signed char *)talk_flags &= 254;
        }
    }
    D_00195A84 = talk_npc_record;
    talk_npc_faction = faction_find(talk_npc_record->faction_id);
    talk_face_image = mc_malloc(4096, (int)D_001703F0, 530);
    if (talk_npc_faction->type == 4) talk_load_face(talk_npc_faction->face);
    l_18 = talk_npc_faction;
    while (l_18 != 0) {
        if (l_18->type == 2 || l_18->type == 7 || l_18->type == 9) break;
        l_18 = l_18->parent;
    }
    talk_npc_own_faction = talk_npc_faction;
    if (l_18 != 0) talk_npc_faction = l_18;
    talk_disposition = talk_npc_faction->reputation;
    talk_disposition += guild_local_temple_rank();
    if (talk_npc_faction->social_group < 5) {
        talk_disposition += player_character->reputation[talk_npc_faction->social_group];
        talk_disposition += func_000679BB(talk_npc_faction->social_group);
    } else {
        talk_disposition += func_000679BB(100);
    }
    if (item_artifact_equipped(0) != 0) {
        talk_disposition += player_character->attributes[5] / 5;
    }
    if (talk_disposition < (-20)) {
        msgbox_show_rsc(7205, 1);
        if (talk_face_image != 0 && talk_face_image != (-1751672937)) {
            mc_free(talk_face_image, (int)D_001703F0, 562);
            talk_face_image = -1751672937;
        }
        return;
    }
    talk_prostitute_offer = 0;
    if (talk_npc_record->faction_id == 512) {
        if ((((int)(unsigned short)((short)player_character->flags & 1)) ^ ((int)(unsigned short)(talk_npc_record->flags & 1))) != 0 && ((int)(unsigned short)(game_settings->view_flags & 4)) == 0 && func_000164CD() != 0) {
            talk_prostitute_offer = 1;
            talk_prostitute_state = 0;
            return;
        }
    }
    talk_open((int)talk_npc_object);
}

int talk_prostitute_update(void)
{
    switch (talk_prostitute_offer) {
    case 1:
        switch (talk_prostitute_state) {
        case 0:
            msgbox_yes_no_rsc(7200);
            talk_prostitute_state = 1;
            break;
        case 1:
            if (((int)D_00196271) == 2) {
                talk_open((int)talk_npc_object);
            } else if ((talk_prostitute_price = rand_range(50, 75) - talk_disposition) > 0) {
                msgbox_yes_no_rsc(7202);
                talk_prostitute_state = 3;
            } else {
                msgbox_yes_no_rsc(7201);
                talk_prostitute_state = 2;
            }
            break;
        case 3:
            if (((int)D_00196271) != 1) goto L15B7E;
            player_character->gold -= talk_prostitute_price;
            if (((unsigned)(player_character->gold + talk_prostitute_price)) >= talk_prostitute_price) goto L15B7E;
            player_character->gold += talk_prostitute_price;
            msgbox_show_rsc(7203, 1);
            talk_prostitute_offer = 0;
            break;
        case 2:
L15B7E:;
            if (((int)D_00196271) == 2) {
                talk_open((int)talk_npc_object);
            } else {
                func_000164EF();
                time_pass(player_character->attributes[4]);
                talk_open((int)talk_npc_object);
            }
        }
    }
    return (int)talk_prostitute_offer;
}

void talk_button_where_is(void)
{
    talk_list_top = 0;
    talk_list_bottom = 13;
    *(int *)D_00196580 = 0;
    *(int *)talk_selected_row = 1;
    talk_question_mode = 1;
    talk_redraw = 1;
}

void talk_prepare_where_answer(void)
{
    talk_npc_knows = talk_roll_knows();
    D_001965DC = 2;
    if ((unsigned char)talk_topic_tab > 0) {
        *(int *)(D_00195D28 + 18) = *(int *)((char *)(int)((*(int *)talk_selected_row * 19) + talk_place_topics) + 3);
        *(short *)(D_00195D28 + 4) = 0;
        return;
    }
    if (((int)(unsigned char)talk_location_category) == 14) {
        *(signed char *)(D_00195D28 + 16) = 5;
        *(signed char *)(D_00195D28 + 7) = current_region;
        *(int *)(D_00195D28 + 18) = 0;
        *(short *)(D_00195D28 + 4) = 0;
        return;
    }
    *(signed char *)(D_00195D28 + 16) = 5;
    *(signed char *)(D_00195D28 + 7) = current_region;
    *(int *)(D_00195D28 + 18) = *(int *)(talk_place_topics + 3 + (((int)(unsigned char)D_001964B0[*(int *)talk_selected_row]) * 19));
    *(short *)(D_00195D28 + 4) = 0;
}

void talk_button_tell_me_about(void)
{
    talk_list_top = 0;
    talk_list_bottom = 13;
    *(int *)D_00196580 = 0;
    *(int *)talk_selected_row = 0;
    talk_question_mode = 0;
    talk_redraw = 1;
}

int talk_ask_news(void)
{
    int l_1C;

    talk_npc_knows = talk_roll_knows();
    D_001965DC = 3;
    if (*(int *)talk_selected_row != 0) return 0;
    l_1C = rumor_pick_news((int)(short)talk_npc_faction->id);
    if (l_1C == 0) return 0;
    mc_strncpy(D_00147954 + 90000, l_1C, 4, (int)D_001703F0, 699);
    D_001966BB = 1;
    mc_strncpy(D_00147954 + 95000, l_1C, 4, (int)D_001703F0, 701);
    talk_wrap_text(D_00147954 + 90000, 190, 66, 302, 191);
    return 1;
}

void talk_tab_location(void)
{
    *(int *)talk_selected_row = 1;
    *(int *)D_00196580 = 0;
    talk_topic_tab = 0;
    talk_redraw = 1;
}

void talk_tab_people(void)
{
    *(int *)D_00196580 = 0;
    talk_topic_tab = 1;
    talk_redraw = 1;
}

void talk_tab_things(void)
{
    *(int *)D_00196580 = 0;
    talk_topic_tab = 2;
    talk_redraw = 1;
}

void talk_tab_work(void)
{
    *(int *)D_00196580 = 0;
    talk_topic_tab = 3;
    talk_redraw = 1;
}

void talk_button_tone(int a1)
{
    talk_tone = *(signed char *)&a1 - 6;
    talk_npc_attitude = talk_roll_attitude();
    talk_redraw = 1;
}

void talk_button_copy_to_logbook(void)
{
    if (D_001966BB == 0) return;
    logbook_copy_text(D_00147954 + 95000);
}

void talk_close(void)
{
    int l_18;

    do {
    } while (key_down_esc != 0);
    while (mouse_buttons != 0) func_0012B136();
    if (D_001966BA != 0) {
        talk_npc_object->npc_flags |= 0x8000;
        if (talk_npc_object->type == 53) talk_npc_object->image2 |= 1;
    }
    if (talk_face_image != 0 && talk_face_image != (-1751672937)) {
        mc_free(talk_face_image, (int)D_001703F0, 760);
        talk_face_image = -1751672937;
    }
    D_00190D10 = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_001703F0, 762);
        window_image = -1751672937;
    }
    if (D_001965E4 != 0 && D_001965E4 != (-1751672937)) {
        mc_free(D_001965E4, (int)D_001703F0, 763);
        D_001965E4 = -1751672937;
    }
    if (talk_saved_screen != 0 && talk_saved_screen != (-1751672937)) {
        mc_free(talk_saved_screen, (int)D_001703F0, 764);
        talk_saved_screen = -1751672937;
    }
    if (D_001965F8 != 0 && D_001965F8 != (-1751672937)) {
        mc_free(D_001965F8, (int)D_001703F0, 765);
        D_001965F8 = -1751672937;
    }
    game_mode = 0;
    D_00196272 = 0;
    D_00187CA8 = 1;
}

void talk_list_scroll_up(void)
{
    if (talk_topic_tab == 0 && ((int)(unsigned char)talk_question_mode) == 1 && talk_showing_categories != 0) {
        return;
    }
    if (talk_list_top == 0) return;
    (talk_list_top)--;
    talk_list_bottom = talk_list_top + 13;
    talk_redraw = 1;
}

void talk_list_scroll_down(void)
{
    if (talk_topic_tab == 0 && ((int)(unsigned char)talk_question_mode) == 1 && talk_showing_categories != 0) {
        return;
    }
    if (talk_list_bottom >= talk_list_count) return;
    (talk_list_top)++;
    talk_list_bottom = talk_list_top + 13;
    talk_redraw = 1;
}

void func_00016192(void)
{
    if (*(int *)D_00196580 >= 0) return;
    *(int *)D_00196580 += 4;
    talk_redraw = 1;
}

void func_000161C1(void)
{
    if (((D_00196578 + 6) + *(int *)D_00196580) <= 100) return;
    *(int *)D_00196580 -= 4;
    talk_redraw = 1;
}

void talk_answer_scroll_up(void)
{
    if (talk_answer_scroll != 0) (talk_answer_scroll)--;
    talk_redraw = 1;
}

void talk_answer_scroll_down(void)
{
    if ((talk_answer_line_count - 18) > talk_answer_scroll) {
        (talk_answer_scroll)++;
    }
    talk_redraw = 1;
}

void talk_button_okay(void)
{
    if (func_000164AB() != 0) return;
    if (talk_question_mode != 0 && talk_topic_tab == 0 && talk_showing_categories != 0) {
        talk_list_click(100);
        return;
    }
    if ((talk_question_mode != 0 && talk_topic_tab == 0 && talk_showing_categories != 0) || D_001966B7 == 0) {
        return;
    }
    D_001965C8 = rand();
    talk_redraw = 1;
    D_001966A4 = 0;
    talk_log_question();
    switch ((unsigned char)talk_question_mode) {
        break;
    case 1:
        talk_prepare_where_answer();
        break;
    case 0:
        if (talk_news_asked != 0) {
            talk_say_text(1457);
            *(signed char *)talk_flags &= 253;
            return;
        }
        if (talk_npc_own_faction->id != 806 && talk_npc_own_faction->id != 842) {
            talk_news_asked = 1;
        }
        if (talk_ask_news() != 0) {
            *(signed char *)talk_flags &= 253;
            if (*(int *)talk_selected_row == 0) return;
        }
    }
    *(signed char *)talk_flags &= 253;
    if (((int)(unsigned char)talk_topic_tab) == 3) {
        if (((int)player_environment) == 3 || quest_find_potential_questor() == 0) {
            talk_say_text(8078);
            return;
        }
        if (talk_npc_attitude == 0) {
            talk_say_text(8075);
        } else if (talk_npc_attitude == 1) {
            talk_say_text(8076);
        } else {
            talk_say_text(8077);
        }
        return;
    }
    if (talk_question_mode == 0) {
        talk_say_text((int)(short)*(short *)(talk_tell_answers + (((talk_npc_knows * 30) + (talk_npc_speech_style * 6)) + (talk_npc_attitude * 2))));
        return;
    }
    if (((int)(unsigned char)talk_question_mode) == 1 && ((int)(unsigned char)talk_location_category) == 14) {
        talk_say_text(28);
        return;
    }
    talk_say_text((int)(short)*(short *)(talk_where_answers + (((talk_npc_knows * 30) + (talk_npc_speech_style * 6)) + (talk_npc_attitude * 2))));
}

void func_0001647E(int a1)
{
    talk_say_text(*(int *)((char *)((D_001965DC << 2) + a1)));
}

int func_000164AB(void)
{
    return 0;
}

int func_000164CD(void)
{
    return 1;
}

void talk_say_text(int a1)
{
    parse_rsc_text(a1, 0, 0);
    mc_strncpy(D_00147954 + 90000, (int)text_rsc_buffer, 4, (int)D_001703F0, 934);
    D_001966BB = 1;
    mc_strncpy(D_00147954 + 95000, (int)text_rsc_buffer, 4, (int)D_001703F0, 936);
    talk_wrap_text(D_00147954 + 90000, 190, 66, 302, 191);
}

void talk_say_string(int a1)
{
    mc_strncpy(D_00147954 + 90000, a1, 4, (int)D_001703F0, 942);
    D_001966BB = 1;
    mc_strncpy(D_00147954 + 95000, a1, 4, (int)D_001703F0, 944);
    talk_wrap_text(D_00147954 + 90000, 190, 66, 302, 191);
}

int talk_macro_hint(int a1)
{
    int l_1C;

    l_1C = talk_hint_text_id(a1);
    if ((l_1C & 32768) != 0) {
        *(int *)&current_quest = (*(int *)&D_00199780 = quest_find_by_id((int)(short)(short)D_00190D11));
        quest_load_text((int)current_quest, l_1C & 32767, 0, 0);
    } else {
        parse_rsc_text(l_1C, 0, 0);
    }
    return (int)text_rsc_buffer;
}

void func_00016907(int a1, int a2)
{
    int l_18;
    int l_14;

    l_18 = 0;
    l_14 = 0;
    while (*(signed char *)((char *)a1) != 0) {
        if (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)a1) + 1)] & 192)) != 0) {
            text_buffer[l_18++] = *(signed char *)((char *)a1);
            l_14 = 1;
        } else {
            if (l_14 != 0) {
                text_buffer[l_18] = 0;
                l_14 = 0;
                l_18 = l_14;
                mc_strncpy(a2, func_000169D4((int)text_buffer), 4, (int)D_001703F0, 1038);
                a2 += func_000A0DF4(a2);
            }
            *(signed char *)((char *)a2++) = *(signed char *)((char *)a1++);
        }
    }
}

int func_000169D4(int a1)
{
    int l_20;
    int l_1C;

    player_character->skills[(int)(unsigned short)talk_language_skill].uses = 1;
    if (rand_range(1, 100) <= player_character->skills[(int)(unsigned short)talk_language_skill].value) {
        return a1;
    }
    l_20 = rand_range(2, 4);
    *(signed char *)D_00191016 = 0;
    for (l_1C = 0; l_1C < l_20; l_1C++) {
        func_000A1054((int)D_00191016, parse_bio_answer_text((int)(unsigned short)D_001966A6), (int)D_001703F0, 1059, 4);
    }
    return (int)D_00191016;
}

void talk_draw_face(void)
{
    func_00144F68(119, 65, 64, 64, talk_face_image);
}

void talk_wrap_text(int a1, int a2, int a3, int a4, int a5)
{
    int l_10;
    int l_C;

    l_C = 0;
    text_rsc_buffer[0] = 0;
    if (D_00190D0F == 0 && talk_answer_line_count != 0) {
        talk_add_line((int)D_0017041B);
    }
    while (*(signed char *)((char *)a1) != 0) {
        l_10 = talk_next_word((int)&a1);
        if (((l_C + l_10) + a2) < a4) {
            func_000A1054((int)text_rsc_buffer, (int)text_buffer, (int)D_001703F0, 1108, 2048);
            l_C += l_10;
        } else {
            talk_add_line((int)text_rsc_buffer);
            mc_strncpy((int)text_rsc_buffer, (int)text_buffer, 2048, (int)D_001703F0, 1114);
            l_C = l_10;
        }
    }
    talk_add_line((int)text_rsc_buffer);
    if (D_00190D0F != 0) return;
    if ((talk_answer_scroll = talk_answer_line_count - 18) >= 0) return;
    talk_answer_scroll = 0;
}

void talk_log_question(void)
{
    int l_18;

    talk_add_line((int)D_0017041B);
    for (l_18 = 0; l_18 < talk_question_line_count; l_18++) {
        *(signed char *)(*(char **)((char *)(int)(talk_question_lines + (l_18 << 2)))) |= 128;
        talk_add_line(*(int *)((char *)(int)(talk_question_lines + (l_18 << 2))));
    }
    talk_clear_question();
}

void talk_add_line(int a1)
{
    if (D_00190D0F != 0) {
        *(int *)((char *)(int)(talk_question_lines + (talk_question_line_count << 2))) = talk_alloc_line();
        mc_strncpy(*(int *)((char *)(int)(talk_question_lines + ((talk_question_line_count)++ << 2))), a1, 4, (int)D_001703F0, 1148);
        return;
    }
    *(int *)((char *)(int)(talk_answer_lines + (talk_answer_line_count << 2))) = talk_alloc_line();
    mc_strncpy(*(int *)((char *)(int)(talk_answer_lines + ((talk_answer_line_count)++ << 2))), a1, 4, (int)D_001703F0, 1153);
}

int talk_next_word(int a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = 0;
    while (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1))) > 32) {
        text_buffer[l_1C++] = *(signed char *)(*(char **)((char *)a1));
        l_20 += font_char_width((int)(unsigned char)*(signed char *)(*(char **)((char *)a1)));
        (*(int *)((char *)a1))++;
    }
    if (l_20 == 0 && *(signed char *)(*(char **)((char *)a1)) != 0) {
        text_buffer[l_1C++] = *(signed char *)(*(char **)((char *)a1));
        l_20 += font_char_width((int)(unsigned char)*(signed char *)(*(char **)((char *)a1)));
        (*(int *)((char *)a1))++;
    }
    text_buffer[l_1C] = 0;
    return l_20;
}

int talk_alloc_line(void)
{
    talk_text_pool_next += func_000A0DF4(talk_text_pool_next) + 1;
    return talk_text_pool_next;
}

void talk_init_text(void)
{
    if (D_001965A0 == 0) D_001965A0 = mc_malloc(20480, (int)D_001703F0, 1238);
    if (D_001965C4 == 0) D_001965C4 = mc_malloc(20480, (int)D_001703F0, 1241);
    mc_memset(D_001965A0, 0, 20480, (int)D_001703F0, 1243, 4);
    mc_memset(D_001965C4, 0, 20480, (int)D_001703F0, 1244, 4);
    talk_text_pool_next = D_001965A0;
    *(int *)&talk_answer_lines = D_001965C4;
    talk_answer_line_count = 0;
    talk_clear_question();
}

void talk_clear_question(void)
{
    if (D_001965AC == 0) D_001965AC = mc_malloc(20480, (int)D_001703F0, 1257);
    mc_memset(D_001965AC, 0, 20480, (int)D_001703F0, 1259, 4);
    *(int *)&talk_question_lines = D_001965AC;
    talk_question_line_count = 0;
}

void talk_draw_place_list(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = (int)(short)D_00142948;
    D_00142948 = 100;
    talk_list_count = 0;
    if (talk_topic_tab == 0) {
        if (talk_list_count >= talk_list_top && talk_list_count <= talk_list_bottom) {
            talk_list_draw_item((int)D_0017041C, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), 96, 156);
        }
        (talk_list_count)++;
        if (((int)(unsigned char)talk_location_category) == 14) {
            talk_draw_regional_list();
        } else {
            for (l_24 = 0; l_24 < talk_place_topic_count; l_24++) {
                if (*(signed char *)((char *)(int)(talk_place_topics + (l_24 * 19))) != talk_location_category) continue;
                if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
                    (talk_list_count)++;
                    continue;
                }
                l_18 = building_name(*(int *)((char *)(int)((l_24 * 19) + talk_place_topics) + 3));
                if (*(int *)talk_selected_row == talk_list_count) {
                    l_20 = 244;
                    mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), l_18, 4, (int)D_001703F0, 1300);
                } else {
                    l_20 = 145;
                }
                talk_list_draw_item(l_18, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_20, 156);
                D_001964B0[(talk_list_count)++] = *(signed char *)&l_24;
            }
        }
    } else if (((int)(unsigned char)talk_topic_tab) == 1) {
        talk_add_quest_topics(5);
    } else if (((int)(unsigned char)talk_topic_tab) == 2) {
        talk_add_quest_topics(6);
    }
    D_00142948 = l_1C;
}

void talk_list_click(int a1)
{
    int l_18;

    if (a1 == 100 && talk_topic_tab == 0 && ((int)(unsigned char)talk_question_mode) == 1 && talk_showing_categories != 0) {
        talk_location_category = *(signed char *)talk_selected_row;
        talk_showing_categories = 0;
        *(int *)talk_selected_row = 1;
        talk_redraw = 1;
        return;
    }
    l_18 = (int)(((char *)talk_list_top) + ((((int)(short)mouse_y) - 71) / 7));
    if (l_18 >= talk_list_count) return;
    if (talk_topic_tab == 0 && ((int)(unsigned char)talk_question_mode) == 1) {
        if (talk_showing_categories != 0) {
            if (D_0012AC02 != 0 || key_pressed_once(28) != 0) {
                talk_location_category = D_00196612[l_18];
                talk_showing_categories = 0;
                *(int *)talk_selected_row = 1;
            } else {
                *(int *)talk_selected_row = (int)(unsigned char)D_00196612[l_18];
            }
        } else if (key_pressed_once(1) != 0 || (l_18 == 0 && (D_0012AC02 != 0 || key_pressed_once(28) != 0))) {
            do {
            } while (key_down_esc != 0);
            talk_showing_categories = 1;
        } else if (l_18 != 0) {
            *(int *)talk_selected_row = l_18;
        } else {
            *(int *)talk_selected_row = 1;
        }
    } else {
        *(int *)talk_selected_row = l_18;
    }
    talk_redraw = 1;
}

void talk_add_quest_topics(int a1)
{
    struct record *l_40;
    struct record *l_3C;
    struct record *l_38;
    struct qbn_place *l_34;
    struct qbn_person *l_30;
    struct qbn_item *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct building *l_18;

    l_40 = D_00195A00->children;
    while (l_40 != 0) {
        l_3C = l_40->next;
        if (l_40->type == 14) {
            D_00195D00 = l_40;
            current_quest = (D_00199780 = &l_40->data.quest);
            if (((unsigned)a1) >= 5) {
                if (((unsigned)a1) <= 5) goto L1776B;
                if (a1 == 6) goto L1790A;
            } else {
                if (a1 != 4) goto L17A4C;
                l_34 = (struct qbn_place *)quest_section((int)D_00199780, 4);
                for (l_24 = 0; D_00199780->section_counts[4] > l_24; l_24++, l_34++) {
                    if (l_34->object->twin == 0) continue;
                    if (((int)(unsigned char)(l_34->flags & 128)) != 0) continue;
                    l_38 = l_34->object->twin;
                    if (l_38->type == 1) continue;
                    l_28 = quest_symbol_text(l_34->symbol, 0, 0);
                    l_18 = object_building(l_38);
                    if (l_18->type < 17 && l_18->type != 1) {
                        for (l_1C = 0; l_1C < talk_place_topic_count; l_1C++) {
                            if (*(int *)((char *)(int)(talk_place_topics + (l_1C * 19)) + 3) == (int)l_18) {
                                *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 2) = (signed char)D_00199780->id;
                                *(short *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 15) = l_34->messages[0];
                                *(short *)((char *)(int)(talk_place_topics + ((talk_place_topic_count)++ * 19)) + 17) = l_34->messages[1];
                            }
                        }
                        continue;
                    }
                    talk_list_draw_item(l_28, 6, ((talk_list_count - talk_list_top) * 7) + 71, l_20, 156);
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19))) = building_category(l_18);
                    *(int *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 11) = building_distance(l_18);
                    *(int *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 3) = (int)l_18;
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 1) = 1;
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 2) = (signed char)D_00199780->id;
                    *(short *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 15) = l_34->messages[0];
                    *(short *)((char *)(int)(talk_place_topics + ((talk_place_topic_count)++ * 19)) + 17) = l_34->messages[1];
                }
                goto L17A4C;
L1776B:;
                l_30 = (struct qbn_person *)quest_section((int)D_00199780, 3);
                for (l_24 = 0; D_00199780->section_counts[3] > l_24; l_24++, l_30++) {
                    if (l_30->object == 0 || l_30->object->twin == 0) continue;
                    if ((int)l_30->object->twin == (int)talk_npc_object) continue;
                    if ((((int)(short)l_30->flags) & 32768) != 0) continue;
                    l_28 = quest_symbol_text(l_30->symbol, 0, 0);
                    if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
                        (talk_list_count)++;
                        continue;
                    }
                    if (*(int *)talk_selected_row == talk_list_count) {
                        l_20 = 244;
                        mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), l_28, 4, (int)D_001703F0, 1448);
                    } else {
                        l_20 = 145;
                    }
                    talk_list_draw_item(l_28, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_20, 156);
                    *(int *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 3) = (int)object_building(l_30->object->twin);
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 1) = 2;
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 2) = (signed char)D_00199780->id;
                    *(short *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 15) = l_30->messages[0];
                    *(short *)((char *)(int)(talk_place_topics + ((talk_list_count)++ * 19)) + 17) = l_30->messages[1];
                }
                goto L17A4C;
L1790A:;
                goto L17A4C;
                for (; D_00199780->section_counts[0] > l_24; l_24++, l_2C++) {
                    if (((int)(unsigned char)(l_2C->flags & 128)) != 0) continue;
                    l_28 = quest_symbol_text(l_2C->symbol, 0, 0);
                    if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
                        (talk_list_count)++;
                        continue;
                    }
                    if (*(int *)talk_selected_row == talk_list_count) {
                        l_20 = 244;
                        mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), l_28, 4, (int)D_001703F0, 1477);
                    } else {
                        l_20 = 145;
                    }
                    talk_list_draw_item(l_28, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_20, 156);
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 1) = 3;
                    *(signed char *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 2) = (signed char)D_00199780->id;
                    *(short *)((char *)(int)(talk_place_topics + (talk_list_count * 19)) + 15) = l_2C->messages[0];
                    *(short *)((char *)(int)(talk_place_topics + ((talk_list_count)++ * 19)) + 17) = l_2C->messages[1];
                }
            }
        }
L17A4C:;
        l_40 = l_3C;
    }
}

void talk_list_draw_item(int a1, int a2, int a3, int a4, int a5)
{
    int l_14;
    short l_10;
    short l_C;

    *(int *)&l_10 = (int)(short)D_00142940;
    D_00142940 = 6;
    *(int *)&l_C = (int)(short)D_00142948;
    D_00142948 = 100;
    text_draw_colored(a1, (int)(short)(a2 + *(short *)D_00196580), (int)(short)*(short *)&a3, (int)(short)*(short *)&a4, (int)(short)*(short *)&a5);
    l_14 = font_text_width(a1);
    if (l_14 > D_00196578) D_00196578 = l_14;
    D_00142940 = *(int *)&l_10;
    D_00142948 = *(int *)&l_C;
}

int talk_macro_1com(void)
{
    if (((int)(unsigned short)(*(short *)talk_flags & 2)) != 0) {
        if (talk_disposition <= 0) {
            text_macro_n_text = *(short *)(talk_greeting_texts + (((int)(unsigned char)talk_tone) << 2));
        }
        parse_rsc_text((int)(unsigned short)((short *)talk_greeting_texts)[((int)(unsigned char)talk_tone) * 2 + 1], 0, 0);
        text_macro_n_text = 0;
    } else {
        parse_rsc_text((int)(unsigned short)talk_ack_texts[((int)(unsigned char)talk_tone)], 0, 0);
    }
    return (int)text_rsc_buffer;
}

void talk_draw_regional_list(void)
{
    int l_18;

    for (l_18 = 0; l_18 < 28; l_18++) {
        if (l_18 >= 8 && l_18 <= 17) {
            if (current_region != D_00179CFA[l_18]) continue;
            if (town_has_building((int)(short)talk_regional_ids[l_18], 0) == 0) {
                talk_add_regional_item(l_18);
            }
        } else if (l_18 < 20) {
            if (town_has_building((int)(short)talk_regional_ids[l_18], 0) == 0) {
                talk_add_regional_item(l_18);
            }
        } else if (town_has_building((int)(short)talk_regional_ids[l_18], 1) == 0) {
            talk_add_regional_item(l_18);
        }
    }
}

void talk_add_regional_item(int a1)
{
    int l_18;

    if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
        (talk_list_count)++;
        return;
    }
    func_000A0ED9(1590, (int)D_001703F0);
    mc_sprintf((int)text_buffer, (int)D_0017042A, talk_regional_names[a1]);
    if (*(int *)talk_selected_row == talk_list_count) {
        l_18 = 244;
        mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), (int)text_buffer, 4, (int)D_001703F0, 1595);
    } else {
        l_18 = 145;
    }
    talk_list_draw_item((int)text_buffer, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_18, 156);
    *(signed char *)(D_00196488 + (talk_list_count)++) = *(signed char *)&a1;
}

int talk_find_regional(int a1)
{
    {
        char l_30[20];

        func_00087D71((int)l_30, (int)(short)D_00179CC8[a1], (int)(short)talk_regional_ids[a1]);
        if (*(int *)((char *)l_30 + 12) == 0) return 0;
        mc_strncpy((int)text_macro_fcn, *(int *)((char *)l_30 + 16), 32, (int)D_001703F0, 1610);
        location_free((int)l_30);
        return 1;
    }
}

void talk_build_place_topics(void)
{
    char l_4C[20];
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    struct building *l_20;
    int l_1C;
    int l_18;

    l_20 = current_location->buildings;
    talk_place_topic_count = 0;
    for (l_38 = 0; current_location->building_count > l_38; l_38++, l_20++) {
        if (l_20->type >= 17 || l_20->type == 1) continue;
        l_1C = memchr((int)talk_category_building_types, l_20->type, 13);
        if (l_1C != 0) {
            *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19))) = l_1C - ((int)talk_category_building_types);
            *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 1) = 0;
            *(int *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 11) = building_distance(l_20);
            *(int *)((char *)(int)(talk_place_topics + ((talk_place_topic_count)++ * 19)) + 3) = (int)l_20;
        } else {
            *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19))) = 13;
            *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 1) = 0;
            *(int *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 11) = building_distance(l_20);
            *(int *)((char *)(int)(talk_place_topics + ((talk_place_topic_count)++ * 19)) + 3) = (int)l_20;
        }
    }
    talk_add_quest_topics(4);
    *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19))) = 14;
    *(signed char *)((char *)(int)(talk_place_topics + (talk_place_topic_count * 19)) + 1) = 0;
    *(int *)((char *)(int)(talk_place_topics + ((talk_place_topic_count)++ * 19)) + 3) = 0;
    l_30 = 1;
    l_34 = talk_place_topic_count - 2;
    if (l_34 < 2) return;
    while (l_30 != 0) {
        l_30 = 0;
        for (l_38 = 0; l_38 < l_34; l_38++) {
            l_18 = *(int *)((char *)(int)(talk_place_topics + (l_38 * 19)) + 11);
            l_28 = *(int *)((char *)(int)(talk_place_topics + ((l_38 + 1) * 19)) + 11);
            if (l_28 < l_18) {
                l_30 = 1;
                mc_memcpy((int)l_4C, (int)(talk_place_topics + (l_38 * 19)), 19, (int)D_001703F0, 1678, 4);
                mc_memcpy((int)(talk_place_topics + (l_38 * 19)), (int)(talk_place_topics + ((l_38 + 1) * 19)), 19, (int)D_001703F0, 1679, 4);
                mc_memcpy((int)(talk_place_topics + ((l_38 + 1) * 19)), (int)l_4C, 19, (int)D_001703F0, 1680, 4);
            }
        }
        l_34--;
    }
}

void talk_draw_categories(void)
{
    int l_1C;
    int l_18;

    talk_list_count = 0;
    if (talk_showing_categories == 0) return;
    for (l_1C = 0; l_1C < 15; l_1C++) {
        if (town_has_category(l_1C) != 0 || l_1C == 14) {
            if (*(int *)talk_selected_row == l_1C) {
                l_18 = 244;
            } else {
                l_18 = 145;
            }
            talk_list_draw_item(talk_category_names[l_1C], 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_18, 156);
            D_00196612[(talk_list_count)++] = *(signed char *)&l_1C;
        }
    }
}

int building_category(struct building *a1)
{
    int l_1C;

    if (a1->type != 1 && a1->type <= 16) {
        l_1C = memchr((int)talk_category_building_types, a1->type, 13);
        if (l_1C == 0) return 13;
        return l_1C - ((int)talk_category_building_types);
    }
    if (((int)(unsigned char)(a1->flags & 8)) != 0) return 13;
    return -1;
}

int town_has_category(int a1)
{
    int l_20;
    struct building *l_1C;

    l_1C = current_location->buildings;
    for (l_20 = 0; current_location->building_count > l_20; l_20++, l_1C++) {
        if (building_category(l_1C) == a1) return 1;
    }
    return 0;
}

void func_00018339(void)
{
    int l_1C;
    struct building *l_18;

    l_18 = current_location->buildings;
    for (l_1C = 0; current_location->building_count > l_1C; l_1C++, l_18++) {
        if (quest_find_site_for_building(l_18) != 0) l_18->flags |= 8;
    }
}

void func_0001839C(void)
{
    struct record *l_38;
    struct record *l_34;
    struct qbn_place *l_30;
    struct qbn_person *l_2C;
    struct qbn_item *l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_38 = D_00195A00->children;
    while (l_38 != 0) {
        l_34 = l_38->next;
        if (l_38->type == 14) {
            D_00195D00 = l_38;
            current_quest = (D_00199780 = &l_38->data.quest);
            l_30 = (struct qbn_place *)quest_section((int)D_00199780, 4);
            for (l_20 = 0; D_00199780->section_counts[4] > l_20; l_20++, l_30++) {
                if (l_30->messages[0] == 0 && l_30->messages[1] == 0) continue;
                if (((int)(unsigned char)(l_30->flags & 128)) != 0) continue;
                *(short *)((char *)(int)(((char *)talk_topics) + (((int)(short)D_001966A2) * 6))) = l_30->messages[0];
                (talk_topics)[((int)(short)D_001966A2) * 3 + 1] = l_30->messages[1];
                (talk_topics)[((int)(short)D_001966A2) * 3 + 2] = D_00199780->id;
                (D_001966A2)++;
                if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
                    (talk_list_count)++;
                    continue;
                }
                l_24 = quest_symbol_text(l_30->symbol, 0, 0);
                if (*(int *)talk_selected_row == talk_list_count) {
                    l_1C = 244;
                    mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), l_24, 4, (int)D_001703F0, 1790);
                } else {
                    l_1C = 145;
                }
                talk_list_draw_item(l_24, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_1C, 156);
                (talk_list_count)++;
            }
            l_2C = (struct qbn_person *)quest_section((int)D_00199780, 3);
            for (l_20 = 0; D_00199780->section_counts[3] > l_20; l_20++, l_2C++) {
                if (l_2C->messages[0] == 0 && l_2C->messages[1] == 0) continue;
                if ((((int)(short)l_2C->flags) & 32768) != 0) continue;
                if (l_2C->object->twin != 0 && (int)l_2C->object->twin == (int)talk_npc_object) {
                    continue;
                }
                *(short *)((char *)(int)(((char *)talk_topics) + (((int)(short)D_001966A2) * 6))) = l_2C->messages[0];
                (talk_topics)[((int)(short)D_001966A2) * 3 + 1] = l_2C->messages[1];
                (talk_topics)[((int)(short)D_001966A2) * 3 + 2] = D_00199780->id;
                (D_001966A2)++;
                if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
                    (talk_list_count)++;
                    continue;
                }
                l_24 = quest_symbol_text(l_2C->symbol, 0, 0);
                if (*(int *)talk_selected_row == talk_list_count) {
                    l_1C = 244;
                    mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), l_24, 4, (int)D_001703F0, 1821);
                } else {
                    l_1C = 145;
                }
                talk_list_draw_item(l_24, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_1C, 156);
                (talk_list_count)++;
            }
            l_28 = (struct qbn_item *)quest_section((int)D_00199780, 0);
            for (l_20 = 0; D_00199780->section_counts[0] > l_20; l_20++, l_28++) {
                if (l_28->messages[0] == 0 && l_28->messages[1] == 0) continue;
                if (((int)(unsigned char)(l_28->flags & 128)) != 0) continue;
                if (l_28->group == 9 && l_28->index == 5) continue;
                *(short *)((char *)(int)(((char *)talk_topics) + (((int)(short)D_001966A2) * 6))) = l_28->messages[0];
                (talk_topics)[((int)(short)D_001966A2) * 3 + 1] = l_28->messages[1];
                (talk_topics)[((int)(short)D_001966A2) * 3 + 2] = D_00199780->id;
                (D_001966A2)++;
                if (talk_list_count < talk_list_top || talk_list_count > talk_list_bottom) {
                    (talk_list_count)++;
                    continue;
                }
                l_24 = quest_symbol_text(l_28->symbol, 0, 0);
                if (*(int *)talk_selected_row == talk_list_count) {
                    l_1C = 244;
                    mc_strncpy(((int)talk_key_text) + (((int)(unsigned char)D_001966B8) * 65), l_24, 4, (int)D_001703F0, 1852);
                } else {
                    l_1C = 145;
                }
                talk_list_draw_item(l_24, 6, (int)&*(signed char *)((char *)((talk_list_count - talk_list_top) * 7) + 71), l_1C, 156);
                (talk_list_count)++;
            }
        }
        l_38 = l_34;
    }
}

int talk_faction_greeting(short a1)
{
    int l_24;
    int l_28;
    short l_1C;

    *(int *)&l_1C = (int)faction_find((int)(short)a1);
    if (((int)(unsigned char)*(signed char *)(*(char **)&l_1C)) == 15 || ((int)(unsigned char)*(signed char *)(*(char **)&l_1C)) == 14) {
        return 0;
    }
    l_24 = talk_faction_relation((int)(short)a1);
    D_001966AC += *(short *)(*(char **)&l_1C + 29);
    if (((int)(unsigned char)*(signed char *)(*(char **)&l_1C + 54)) < 5) {
        D_001966AC += player_character->reputation[(int)(unsigned char)*(signed char *)(*(char **)&l_1C + 54)];
    }
    l_28 = rand_range(0, 15) - 10;
    if (((int)(short)D_001966AC) >= l_28 && ((int)(short)*(short *)(*(char **)&l_1C + 29)) >= 30) {
        return (int)(short)*(short *)(talk_faction_greetings + (l_24 * 6));
    }
    if (((int)(short)D_001966AC) >= l_28) {
        return (int)(short)*(short *)(D_001799AE + (l_24 * 6));
    }
    D_001966BA = 1;
    return (int)(short)*(short *)(D_001799B0 + (l_24 * 6));
}

int talk_roll_knows(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_1C = rand();
    if (talk_npc_own_faction->id == 806 || talk_npc_own_faction->id == 842) return 1;
    l_24 = ((1 - ((int)(unsigned char)talk_question_mode)) << 2) + ((int)(unsigned char)talk_topic_tab);
    srand(((talk_npc_object->id ^ (((unsigned)talk_npc_object->id) >> 16)) + l_24) + *(int *)talk_selected_row);
    if (rand_range(1, 20) <= (((int)(short)*(short *)(D_001799F2 + ((l_24 * 10) + (talk_npc_speech_style * 2)))) + 10)) {
        l_28 = 1;
    } else {
        l_28 = 0;
    }
    l_20 = l_28;
    srand(l_1C);
    return l_20;
}

int talk_roll_attitude(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    if (talk_tone == 0) {
        if (rand_range(1, 100) < player_character->skills[1].value) {
            l_20 += 10;
        } else {
            l_20 += -5;
        }
        if (talk_attitude_cache[0] == 0) skill_add_uses(1, 1);
    }
    if (((int)(unsigned char)talk_tone) == 2) {
        if (rand_range(1, 100) < player_character->skills[2].value) {
            l_20 += 10;
        } else {
            l_20 += -5;
        }
        if (D_001965D4 == 0) skill_add_uses(2, 1);
    }
    l_20 += (int)(short)D_001799E2[(((1 - ((int)(unsigned char)talk_question_mode)) << 2) + ((int)(unsigned char)talk_topic_tab))];
    l_20 += player_character->attributes[5] / 5;
    l_1C = rand_range(0, 20);
    if (talk_attitude_cache[((int)(unsigned char)talk_tone)] != 0) {
        l_20 = talk_attitude_cache[((int)(unsigned char)talk_tone)];
    } else {
        talk_attitude_cache[((int)(unsigned char)talk_tone)] = l_20;
    }
    if (l_20 < l_1C) return 0;
    if ((l_1C + 30) <= l_20) return 2;
    return 1;
}

struct faction *faction_find_type_in_region_r(struct faction *a1, short a2, short a3)
{
    struct faction *l_1C;

    while (a1 != 0) {
        if ((short)((unsigned short)a1->type) == a3 && a1->region == (-1)) {
            D_0019671C = a1;
        } else if ((short)((unsigned short)a1->type) == a3 && (short)((unsigned short)a1->region) == a2) {
            return a1;
        }
        if (a1->child != 0) {
            l_1C = faction_find_type_in_region_r(a1->child, (int)(short)a2, (int)(short)a3);
            if (l_1C != 0) return l_1C;
        }
        a1 = a1->next;
    }
    return 0;
}

struct faction *faction_find_r(struct faction *a1, short a2)
{
    short l_18;

    while (a1 != 0) {
        if (a1->id == (short)a2) return a1;
        if (a1->child != 0) {
            *(int *)&l_18 = (int)faction_find_r(a1->child, (int)(short)a2);
            if (*(int *)&l_18 != 0) return (struct faction *)*(int *)&l_18;
        }
        a1 = a1->next;
    }
    return 0;
}

void func_000193DD(struct faction *a1)
{
    int l_1C;
    int l_18;

    while (a1 != 0) {
        if ((int)a1 == *(int *)D_00190BE4) *(signed char *)D_00195B84 |= 1;
        for (l_18 = 0; l_18 < 3; l_18++) {
            if ((int)a1->allies[l_18] == *(int *)D_00190BE4) *(signed char *)D_00195B84 |= 1;
            if ((int)a1->enemies[l_18] == *(int *)D_00190BE4) *(signed char *)D_00195B84 |= 2;
        }
        if (a1->child != 0) func_000193DD(a1->child);
        a1 = a1->next;
    }
}

struct faction *faction_random(void)
{
    *(int *)D_00195B84 = rand_range(0, faction_count - 1);
    return faction_nth_r(factions);
}

struct faction *faction_nth_r(struct faction *a1)
{
    struct faction *l_1C;

    while (a1 != 0) {
        if (*(int *)D_00195B84 == 0) return a1;
        (*(int *)D_00195B84)--;
        if (a1->child != 0) {
            l_1C = faction_nth_r(a1->child);
            if (l_1C != 0) return l_1C;
        }
        a1 = a1->next;
    }
    return 0;
}

struct faction *faction_nth_of_type_r(struct faction *a1, unsigned char a2)
{
    short l_18;

    while (a1 != 0) {
        if (*(int *)D_00195B84 == 0 && a1->type == a2) return a1;
        if (a1->child != 0) {
            *(int *)&l_18 = (int)faction_nth_of_type_r(a1->child, (int)(unsigned char)a2);
            if (*(int *)&l_18 != 0) return (struct faction *)*(int *)&l_18;
        }
        if (a1->type == a2) (*(int *)D_00195B84)--;
        a1 = a1->next;
    }
    return 0;
}

void faction_count_of_type_r(struct faction *a1, unsigned char a2)
{
    int l_18;

    while (a1 != 0) {
        if (a1->type == a2) (*(int *)D_00195B84)++;
        if (a1->child != 0) faction_count_of_type_r(a1->child, (int)(unsigned char)a2);
        a1 = a1->next;
    }
}

struct faction *faction_random_of_type(unsigned char a1)
{
    *(int *)D_00195B84 = 0;
    faction_count_of_type_r(factions, (int)(unsigned char)a1);
    *(int *)D_00195B84 = rand_range(0, *(int *)D_00195B84 - 1);
    return faction_nth_of_type_r(factions, (int)(unsigned char)a1);
}

int func_0001AC53(struct faction *a1, struct faction *a2)
{
    {
        int l_20;

        (*(int *)D_00195B84)++;
        if (faction_is_regional_noble(a1) != 0 && faction_is_regional_noble(a2) != 0 && faction_has_enemy(a1, a2) != 0 && func_0001B144(a1, a2) != 0) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        return l_20;
    }
}

int faction_is_regional_noble(struct faction *a1)
{
    return (((a1->type == 7) && (a1->region != 255)) ? 1 : 0);
}

int faction_has_enemy(struct faction *a1, struct faction *a2)
{
    {
        int l_20;

        if (a2 == 0) return 0;
        if (a1 == 0) return 0;
        if ((int)a1->enemies[0] == a2 || (int)a1->enemies[1] == a2 || (int)a1->enemies[2] == a2) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        return l_20;
    }
}

int faction_has_ally(struct faction *a1, struct faction *a2)
{
    {
        int l_20;

        if (a1 == 0) return 0;
        if (a2 == 0) return 0;
        if ((int)a1->allies[0] == a2 || (int)a1->allies[1] == a2 || (int)a1->allies[2] == a2) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        return l_20;
    }
}

void faction_add_power(struct faction *a1, int a2)
{
    if (a1 == 0) return;
    a1->power += a2;
    if (a1->power > 100) {
        a1->power = 100;
        return;
    }
    if (a1->power >= 0) return;
    a1->power = 1;
}

void func_0001AE63(struct faction *a1)
{
    while (a1 != 0) {
        if (a1->child != 0) func_0001AE63(a1->child);
        if (a1->power > *(int *)D_00195B84) *(int *)D_00195B84 = a1->power;
        a1 = a1->next;
    }
}

int func_0001AEBE(struct faction *a1)
{
    *(int *)D_00195B84 = 0;
    func_0001AE63(a1->child);
    return *(int *)D_00195B84;
}

int func_0001AEF9(struct faction *a1, struct faction *a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
    for (l_20 = 0; l_20 < 3; l_20++) {
        for (l_1C = 0; l_1C < 3; l_1C++) {
            if (faction_has_ally(a1->allies[l_20], a2->allies[l_1C]) != 0) l_18 += 3;
        }
    }
    for (l_20 = 0; l_20 < 3; l_20++) {
        for (l_1C = 0; l_1C < 3; l_1C++) {
            if (faction_has_enemy(a1->enemies[l_20], a2->enemies[l_1C]) != 0) l_18 += 3;
        }
    }
    return *(int *)D_00195B84;
}

int func_0001AFD5(struct faction *a1)
{
    if (*(int *)D_00195B84 != 0) return *(int *)D_00195B84;
    while (a1 != 0) {
        if ((int)a1 == (int)D_0019670C) *(signed char *)D_00195B84 |= 2;
        if (a1 == D_0019671C) {
            *(signed char *)D_00195B84 |= 1;
            return *(int *)D_00195B84;
        }
        if (a1->child != 0 && ((struct bf8_1_1 *)&D_00195B84)->f == 0) func_0001AFD5(a1->child);
        a1 = a1->next;
    }
    return *(int *)D_00195B84;
}

int func_0001B144(struct faction *a1, struct faction *a2)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1 != 0 && a1->region != 255 && a2 != 0 && a2->region != 255) {
        l_20 = ((int)D_0017C668) + (a1->region * 11);
        l_18 = a2->region;
        for (l_1C = 0; l_1C < 11; l_1C++) {
            if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) == l_18) return 1;
        }
    }
    return 0;
}

int faction_power(struct faction *a1)
{
    if (a1 != 0) return a1->power;
    return 0;
}

void func_0001B22E(struct faction *a1)
{
    if (a1->type != 7 || a1->region == 255) return;
    region_event_groups[a1->region * 80] = 0;
    region_event_values[a1->region * 80] = 0;
    region_event_flags[a1->region * 80] = 0;
    D_0018F062[a1->region * 80] = 0;
    D_0018F063[a1->region * 80] = 0;
    D_0018F064[a1->region * 80] = 0;
}

void func_0001B2ED(struct faction *a1)
{
    if (a1->type != 7 || a1->region == 255) return;
    region_event_groups[a1->region * 80] = 1;
    D_0018F045[a1->region * 80] = 255;
    D_0018F062[a1->region * 80] = 1;
}

void func_0001B36A(struct faction *a1, int a2)
{
    if (a1->type != 7 || a1->region == 255) return;
    region_event_values[a1->region * 80] = 0;
    D_0018F045[a1->region * 80] = 0;
    D_0018F046[(a1->region * 80) + a2] = 0;
    D_0018F063[(a1->region * 80) + a2] = 1;
    region_event_groups[a1->region * 80] = 1;
}

void faction_add_reputation(struct faction *a1, int a2)
{
    if (a1 == 0) return;
    a1->reputation += a2;
    if (a1->reputation > 100) {
        a1->reputation = 100;
        return;
    }
    if (a1->reputation >= (-100)) return;
    a1->reputation = 65436;
}

void faction_change_reputation(struct faction *a1, int a2)
{
    int l_1C;
    int l_18;
    struct membership *l_14;

    if (a1 == 0) return;
    if (a1->id == 240) {
        l_14 = guild_find_membership_by_bits(128);
        if (l_14 != 0) a1 = faction_find(l_14->faction);
    }
    func_0001B5BE(a1, a2);
    for (l_1C = 0; l_1C < 3; l_1C++) {
        if (a1->allies[l_1C] != 0) faction_add_reputation(a1->allies[l_1C], a2 >> 1);
    }
    for (l_1C = 0; l_1C < 3; l_1C++) {
        if (a1->enemies[l_1C] != 0) faction_add_reputation(a1->enemies[l_1C], -(a2 >> 1));
    }
}

void func_0001B554(struct faction *a1, struct faction *a2, int a3)
{
    while (a1 != 0) {
        if (a1 == a2) {
            faction_add_reputation(a1, a3);
        } else {
            faction_add_reputation(a1, a3 >> 1);
        }
        if (a1->child != 0) func_0001B554(a1->child, a2, a3);
        a1 = a1->next;
    }
}

void func_0001B5BE(struct faction *a1, int a2)
{
    struct faction *l_18;
    struct membership *l_14;

    l_18 = a1;
    while (a1->parent != 0) {
        if (a1->id == 108) {
            faction_add_reputation(a1, a2);
            break;
        }
        a1 = a1->parent;
    }
    if (a1->parent == 0 && a1->id != 844) faction_add_reputation(a1, a2);
    if (a1->id == 844) {
        faction_add_reputation(a1, a2);
        l_14 = guild_find_membership_by_bits(64);
        if (l_14 != 0) a1 = faction_find(l_14->faction);
        if (a1 != 0) faction_add_reputation(a1, a2);
    }
    func_0001B554(a1->child, l_18, a2);
}
