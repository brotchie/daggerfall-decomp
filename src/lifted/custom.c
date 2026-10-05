/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern int screen_buffer;
extern signed char xn_mouse_cursor_drawn;
extern char D_00175420[];
extern char D_00175429[];
extern char D_00175436[];
extern char D_00175443[];
extern char D_00175450[];
extern char D_0017545D[];
extern char D_0017546A[];
extern char D_00175477[];
extern char D_00175482[];
extern char D_0017548B[];
extern char D_00175499[];
extern char D_001754AA[];
extern char D_001754B8[];
extern char D_001754C3[];
extern char D_001754D0[];
extern char D_001754DD[];
extern char D_001754EE[];
extern char D_001754FE[];
extern char D_0017550B[];
extern char D_00175530[];
extern char D_00175537[];
extern char D_0017553E[];
extern char D_00175553[];
extern char D_0017556B[];
extern char D_0017557E[];
extern char D_0017558C[];
extern char D_0017559E[];
extern char D_001755B3[];
extern char D_001755C9[];
extern char D_001755DC[];
extern char D_001755E2[];
extern char D_0017567E[];
extern char D_0017568B[];
extern char D_00175698[];
extern char skill_names[];
extern char classmaker_help_topics[];
extern char classmaker_advantage_names[];
extern int D_0018087E[];
extern int D_0018089E[];
extern int D_001808AE[];
extern int D_001808BE[];
extern int D_001808D2[];
extern int D_001808E6[];
extern int D_001808FE[];
extern char classmaker_disadvantage_names[];
extern int D_0018096E[];
extern int D_0018097A[];
extern int D_00180986[];
extern int D_00180992[];
extern int D_001809A2[];
extern int D_001809B6[];
extern char *classmaker_advantage_costs[];
extern char *classmaker_disadvantage_costs[];
extern char classmaker_buttons[];
extern short classmaker_attribute_up_box;
extern short D_00185646;
extern short D_00185648;
extern short D_0018564A;
extern int D_0018564C;
extern short classmaker_attribute_down_box;
extern short D_00185652;
extern short D_00185654;
extern short D_00185656;
extern int D_00185658;
extern char classmaker_reputation_buttons[];
extern char D_0018565E[];
extern char D_00185662[];
extern char classmaker_specials_buttons[];
extern short classmaker_help_texts[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char D_00190BE4[];
extern int D_00190BE8;
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d68;
extern short classmaker_done;
extern short chargen_selected_attribute;
extern short D_00190D72;
extern short D_00190D74;
extern char classmaker_screen[];
extern short D_00190D78;
extern short D_00190D7C;
extern signed char classmaker_special_counts[];
extern signed char D_00190D7F;
extern short classmaker_special_list;
extern short D_00190D84;
extern short D_00190D86;
extern char scratch_190de4[];
extern int scratch_190de8;
extern int scratch_190dec;
extern char scratch_190df0[];
extern int scratch_190df4;
extern int scratch_190df8;
extern int scratch_190dfc;
extern int D_00190E00;
extern int D_00190E0C;
extern int D_00190E10;
extern int D_00190EE0;
extern char D_00190EE4[];
extern signed char D_001940D4;
extern signed char D_001940D8;
extern struct record *player_object;
extern int magic_window_image;
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern char scratch_buffer[];
extern char classmaker_file[];
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern int D_0019981C;
extern signed char classmaker_specials[];
extern signed char D_00199821[];
extern signed char D_0019982E[];
extern signed char D_0019982F[];
extern char *classmaker_current_buttons;

extern int classmaker_draw(short);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int disk_create(int);
extern int list_popup_update(void);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int mc_strncpy();
extern int write();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A1054();
extern int memchr();
extern int xn_str_fill_u16();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_draw_get_rect();
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void classmaker_draw_reputations(void);
extern void classmaker_input_text(int, short, int);
extern void classmaker_select_attribute(int);
extern void classmaker_specials_screen(void);
extern void itemmaker_reset(void);
extern void itemmaker_select_tab(unsigned char);
extern void list_popup_open(int);
int classmaker_pick_from_list(int, int);
int classmaker_update_advancement(void);
int classmaker_skill_taken(short);
void classmaker_set_reputation(int);
void classmaker_save_file(void);
void career_advantages_text(void);
void career_disadvantages_text(void);
void career_specials_line(int, int);
#pragma aux mc_set_location parm routine [];

void classmaker_run(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    short l_1C;
    short l_18;

    mc_memset((int)player_class + 16, -1, 12, (int)D_00175420, 88, 12);
    player_class->hp_per_level = 8;
    player_class->flags = 5120;
    xn_str_fill_u16((int)player_class + 58, 50, 16);
    classmaker_special_counts[0] = (D_00190D7F = (D_00190D7C = (D_00190D78 = (classmaker_done = (*(short *)scratch_190d66 = (*(short *)scratch_190d64 = 0))))));
    xn_mouse_cursor_drawn &= 254;
    scratch_190d68 = 115;
    classmaker_select_attribute(0);
    *(short *)classmaker_screen = 0;
    D_00190D84 = 65535;
    D_00190D86 = 0;
    D_001940D8 |= 1;
    if (*(signed char *)classmaker_file != 0) {
        disk_read_file((int)classmaker_file, (int)player_class);
    }
    disk_read_file((int)D_00175429, screen_buffer);
    scratch_190df4 = mc_malloc(64000, (int)D_00175420, 105);
    mc_memcpy(scratch_190df4, screen_buffer, 64000, (int)D_00175420, 106, 4);
    scratch_190dec = disk_read_file((int)D_00175436, 0);
    *(int *)scratch_190de4 = mc_malloc(5520, (int)D_00175420, 109);
    xn_draw_get_rect(219, 46, 40, 138, *(int *)scratch_190de4, 0);
    *(int *)scratch_190df0 = disk_read_file((int)D_00175443, 0);
    scratch_190de8 = mc_malloc(((int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 4)) * ((int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 6)), (int)D_00175420, 113);
    scratch_190df8 = disk_read_file((int)D_00175450, 0);
    D_00190E00 = disk_read_file((int)D_0017545D, 0);
    scratch_190dfc = disk_read_file((int)D_0017546A, 0);
    l_28 = 1132;
    *(int *)D_00190BE4 = *(int *)((char *)l_28);
    while (classmaker_done == 0) {
        keys_world_actions();
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        if (mouse_buttons == 0) {
            D_00190D86 = 0;
            D_00190D72 = 16;
            D_00190D74 = 16;
        } else if (*(short *)classmaker_screen == 0) {
            if (mouse_x > classmaker_attribute_down_box && mouse_x < D_00185654 && mouse_y > D_00185652 && mouse_y < D_00185656) {
                sound_play(203, (int)player_object, 100);
                ((int (*)())(D_00185658))();
            }
            if (mouse_x > classmaker_attribute_up_box && mouse_x < D_00185648 && mouse_y > D_00185646 && mouse_y < D_0018564A) {
                sound_play(203, (int)player_object, 100);
                ((int (*)())(D_0018564C))();
            }
        }
        if (D_00190D86 != 0) (D_00190D86)--;
        if (*(short *)classmaker_screen == 0) {
            *(int *)&classmaker_current_buttons = (int)classmaker_buttons;
            *(int *)&l_1C = 28;
        } else if (((int)(short)*(short *)classmaker_screen) == 1) {
            *(int *)&classmaker_current_buttons = (int)classmaker_reputation_buttons;
            *(int *)&l_1C = 6;
        } else {
            *(int *)&classmaker_current_buttons = (int)classmaker_specials_buttons;
            if (((int)(short)(*(short *)classmaker_screen & 16)) != 0) {
                l_2C = 1;
            } else {
                l_2C = 3;
            }
            *(int *)&l_1C = l_2C;
        }
        if (((int)(unsigned char)game_mode) != 8) {
            if (*(short *)classmaker_screen == 0 && mouse_buttons != 0 && mouse_buttons != mouse_buttons_prev) {
                *(int *)&l_18 = 0;
                for (; (short)(short)*(int *)&l_18 < l_1C; (*(int *)&l_18)++) {
                    if (mouse_x > *(short *)((char *)(int)(classmaker_current_buttons + (((int)(short)l_18) * 12))) && mouse_x < *(short *)(classmaker_current_buttons + 4 + (((int)(short)l_18) * 12)) && mouse_y > *(short *)(classmaker_current_buttons + 2 + (((int)(short)l_18) * 12)) && mouse_y < *(short *)(classmaker_current_buttons + 6 + (((int)(short)l_18) * 12))) {
                        sound_play(203, (int)player_object, 100);
                        ((int (*)())(*(int *)(classmaker_current_buttons + 8 + (((int)(short)l_18) * 12))))();
                        break;
                    }
                }
            } else if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                *(int *)&l_18 = 0;
                for (; (short)(short)*(int *)&l_18 < l_1C; (*(int *)&l_18)++) {
                    if (mouse_x > *(short *)((char *)(int)(classmaker_current_buttons + (((int)(short)l_18) * 12))) && mouse_x < *(short *)(classmaker_current_buttons + 4 + (((int)(short)l_18) * 12)) && mouse_y > *(short *)(classmaker_current_buttons + 2 + (((int)(short)l_18) * 12)) && mouse_y < *(short *)(classmaker_current_buttons + 6 + (((int)(short)l_18) * 12))) {
                        sound_play(203, (int)player_object, 100);
                        ((int (*)())(*(int *)(classmaker_current_buttons + 8 + (((int)(short)l_18) * 12))))();
                        break;
                    }
                }
            }
        }
        if (*(short *)classmaker_screen == 0) {
            classmaker_draw(1);
        } else if (((int)(short)*(short *)classmaker_screen) == 1) {
            classmaker_draw_reputations();
        } else {
            classmaker_specials_screen();
        }
        classmaker_update_advancement();
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00175420, 205, 4);
        l_24 = 1132;
        if (*(int *)D_00190BE4 != *(int *)((char *)l_24)) {
            D_00190D74++;
            l_20 = 1132;
            *(int *)D_00190BE4 = *(int *)((char *)l_20);
        }
    }
    D_001940D8 &= 254;
    mc_free(*(int *)scratch_190de4, (int)D_00175420, 217);
    mc_free(scratch_190dec, (int)D_00175420, 218);
    mc_free(scratch_190de8, (int)D_00175420, 219);
    mc_free(*(int *)scratch_190df0, (int)D_00175420, 220);
    mc_free(scratch_190df4, (int)D_00175420, 221);
    mc_free(scratch_190df8, (int)D_00175420, 222);
    mc_free(scratch_190dfc, (int)D_00175420, 223);
    mc_free(D_00190E00, (int)D_00175420, 224);
    mc_memset((int)scratch_190de4, 0, 512, (int)D_00175420, 226, 512);
}

void classmaker_exit_button(void)
{
    if (player_class->name[0] == 0) {
        msgbox_show_rsc(301, 1);
        return;
    }
    if (memchr((int)player_class + 16, -1, 12) != 0) {
        msgbox_show_rsc(300, 1);
        return;
    }
    if (*(short *)scratch_190d64 != 0) {
        msgbox_show_rsc(302, 1);
        return;
    }
    if (((int)(short)scratch_190d68) < 81 || ((int)(short)scratch_190d68) > 161) {
        msgbox_show_rsc(306, 1);
        return;
    }
    classmaker_save_file();
    classmaker_done = 1;
}

void classmaker_name_button(void)
{
    classmaker_input_text((int)player_class + 28, 14, (int)classmaker_draw);
}

void classmaker_skill_primary_1(void)
{
    player_class->skills[0] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_primary_2(void)
{
    player_class->skills[1] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_primary_3(void)
{
    player_class->skills[2] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_major_1(void)
{
    player_class->skills[3] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_major_2(void)
{
    player_class->skills[4] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_major_3(void)
{
    player_class->skills[5] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_minor_1(void)
{
    player_class->skills[6] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_minor_2(void)
{
    player_class->skills[7] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_minor_3(void)
{
    player_class->skills[8] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_minor_4(void)
{
    player_class->skills[9] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_minor_5(void)
{
    player_class->skills[10] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_skill_minor_6(void)
{
    player_class->skills[11] = classmaker_pick_from_list((int)skill_names, scratch_190df4);
}

void classmaker_help_button(void)
{
    msgbox_show_rsc((int)(short)classmaker_help_texts[classmaker_pick_from_list((int)classmaker_help_topics, scratch_190df4)], 1);
}

void classmaker_advantages_button(void)
{
    D_00190E0C = (int)classmaker_advantage_names;
    D_00190E10 = (int)classmaker_advantage_costs;
    *(short *)classmaker_screen = 2;
    classmaker_special_list = 0;
}

void classmaker_disadvantages_button(void)
{
    D_00190E0C = (int)classmaker_disadvantage_names;
    D_00190E10 = (int)classmaker_disadvantage_costs;
    *(short *)classmaker_screen = 3;
    classmaker_special_list = 1;
}

void classmaker_reputations_button(void)
{
    *(short *)classmaker_screen = 1;
}

void classmaker_attribute_str(void)
{
    classmaker_select_attribute(0);
}

void classmaker_attribute_int(void)
{
    classmaker_select_attribute(1);
}

void classmaker_attribute_wil(void)
{
    classmaker_select_attribute(2);
}

void classmaker_attribute_agi(void)
{
    classmaker_select_attribute(3);
}

void classmaker_attribute_end(void)
{
    classmaker_select_attribute(4);
}

void classmaker_attribute_per(void)
{
    classmaker_select_attribute(5);
}

void classmaker_attribute_spd(void)
{
    classmaker_select_attribute(6);
}

void classmaker_attribute_luc(void)
{
    classmaker_select_attribute(7);
}

void classmaker_attribute_up(void)
{
    int l_1C;
    int l_18;

    l_1C = 1132;
    if (((unsigned)(*(int *)((char *)l_1C) - D_00190BE8)) < 6) return;
    l_18 = 1132;
    D_00190BE8 = *(int *)((char *)l_18);
    if (player_class->attributes[(int)(short)chargen_selected_attribute] == 75) return;
    player_class->attributes[(int)(short)chargen_selected_attribute]++;
    (*(short *)scratch_190d64)--;
}

void classmaker_attribute_down(void)
{
    int l_1C;
    int l_18;

    l_1C = 1132;
    if (((unsigned)(*(int *)((char *)l_1C) - D_00190BE8)) < 6) return;
    l_18 = 1132;
    D_00190BE8 = *(int *)((char *)l_18);
    if (player_class->attributes[(int)(short)chargen_selected_attribute] == 10) return;
    player_class->attributes[(int)(short)chargen_selected_attribute]--;
    (*(short *)scratch_190d64)++;
}

int classmaker_pick_from_list(int a1, int a2)
{
    short l_14;
    short l_18;

    if (((int)skill_names) == a1) {
        *(int *)&l_14 = 0;
        *(int *)&l_18 = *(int *)&l_14;
        while (*(int *)((char *)((((int)(short)l_14) << 2) + a1)) != 0) {
            if (classmaker_skill_taken((int)(short)l_14) == 0) {
                scratch_190ce4[(int)(short)l_18] = *(signed char *)&l_14;
                *(int *)(D_00190EE4 + (((int)(short)(*(int *)&l_18)++) << 2)) = *(int *)((char *)(int)((char *)a1 + (((int)(short)l_14) << 2)));
            }
            l_14++;
        }
        *(int *)(D_00190EE4 + (((int)(short)l_18) << 2)) = 0;
        list_popup_open((int)D_00190EE4);
    } else {
        list_popup_open(a1);
    }
    D_001940D4 &= 254;
    for (;;) {
        keys_world_actions();
        xn_mouse_poll_clamped();
        mc_memcpy(screen_buffer, a2, 64000, (int)D_00175420, 558, 4);
        l_14 = list_popup_update();
        if (((int)(short)l_14) > (-1)) {
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            if (((int)skill_names) == a1) {
                return (int)(signed char)scratch_190ce4[(int)(short)l_14];
            }
            return (int)(short)l_14;
        }
        xn_mouse_cursor_drawn &= 254;
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00175420, 568, 4);
    }
}

void classmaker_set_reputation(int a1)
{
    int l_1C;
    int l_18;

    l_18 = (((int)(short)*(short *)(D_0018565E + (((int)(short)*(short *)&a1) * 12))) + ((int)(short)*(short *)(D_00185662 + (((int)(short)*(short *)&a1) * 12)))) >> 1;
    if (((int)(short)mouse_y) < 81) {
        player_character->reputation[(int)(short)*(short *)&a1] = (((int)(short)mouse_y) - 81) / 5;
    } else if (((int)(short)mouse_y) > 81) {
        player_character->reputation[(int)(short)*(short *)&a1] = (((int)(short)mouse_y) - 81) / 5;
    } else {
        player_character->reputation[(int)(short)*(short *)&a1] = 0;
    }
    player_character->reputation[(int)(short)*(short *)&a1] = -player_character->reputation[(int)(short)*(short *)&a1];
}

void classmaker_reputation_commoners(void)
{
    classmaker_set_reputation(0);
}

void classmaker_reputation_merchants(void)
{
    classmaker_set_reputation(1);
}

void classmaker_reputation_scholars(void)
{
    classmaker_set_reputation(2);
}

void classmaker_reputation_nobility(void)
{
    classmaker_set_reputation(3);
}

void classmaker_reputation_underworld(void)
{
    classmaker_set_reputation(4);
}

void classmaker_reputations_exit(void)
{
    int l_1C;
    short l_18;

    l_1C = 0;
    l_18 = l_1C;
    for (; ((int)(short)*(short *)&l_1C) < 5; l_1C++) {
        l_18 += player_character->reputation[(int)(short)*(short *)&l_1C];
    }
    if (l_18 != 0) {
        D_0012B508 = 145;
        msgbox_show_rsc(303, 1);
        return;
    }
    *(short *)classmaker_screen = 0;
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

int classmaker_picklist_wait(void)
{
    int l_1C;

    for (;;) {
        keys_world_actions();
        xn_mouse_poll_clamped();
        l_1C = list_popup_update();
        if (l_1C > (-1)) {
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            xn_mouse_cursor_drawn &= 254;
            return l_1C + 1;
        }
        if (l_1C == (-2)) {
            xn_mouse_cursor_drawn &= 254;
            return 0;
        }
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00175420, 655, 4);
    }
}

int classmaker_drop_duplicate_special(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_20 = (int)(unsigned char)classmaker_specials[(a1 * 14) + (((int)(unsigned char)classmaker_special_counts[a1]) * 2)];
    l_1C = (int)(unsigned char)D_00199821[(a1 * 14) + (((int)(unsigned char)classmaker_special_counts[a1]) * 2)];
    for (l_24 = 0; ((int)(unsigned char)classmaker_special_counts[a1]) > l_24; l_24++) {
        if (((int)(unsigned char)classmaker_specials[(a1 * 14) + (l_24 * 2)]) == l_20 && ((int)(unsigned char)D_00199821[(a1 * 14) + (l_24 * 2)]) == l_1C) {
            (classmaker_special_counts[a1])--;
            return 1;
        }
    }
    return 0;
}

void classmaker_specials_exit(void)
{
    *(short *)classmaker_screen = 0;
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

void classmaker_specials_add(void)
{
    if (((int)(unsigned char)classmaker_special_counts[(int)(short)classmaker_special_list]) == 7) return;
    *(signed char *)classmaker_screen |= 16;
    list_popup_open(D_00190E0C);
    D_001940D4 |= 1;
}

void classmaker_specials_picked(void)
{
    int l_18;

    *(signed char *)classmaker_screen &= 239;
    if (((int)(short)*(short *)classmaker_screen) == 2) {
        l_18 = (int)classmaker_advantage_names;
    } else {
        l_18 = (int)classmaker_disadvantage_names;
    }
    D_00190E0C = l_18;
    (classmaker_special_counts[(int)(short)classmaker_special_list])++;
}

int classmaker_update_advancement(void)
{
    int l_2C;
    int l_28;
    int l_24;
    short l_18;
    short l_1C;

    l_28 = 65536;
    l_24 = 0;
    *(int *)&l_18 = 0;
    for (; (short)(short)((int)(unsigned char)classmaker_special_counts[0]) > l_18; (*(int *)&l_18)++) {
        l_24 += *(int *)((char *)(int)(classmaker_advantage_costs[((int)(unsigned char)classmaker_specials[((int)(short)l_18) * 2])] + (((int)(unsigned char)D_00199821[((int)(short)l_18) * 2]) << 2)));
    }
    l_2C = 0;
    *(int *)&l_18 = 0;
    for (; (short)(short)((int)(unsigned char)D_00190D7F) > l_18; (*(int *)&l_18)++) {
        l_2C += *(int *)((char *)(int)(classmaker_disadvantage_costs[((int)(unsigned char)D_0019982E[((int)(short)l_18) * 2])] + (((int)(unsigned char)D_0019982F[((int)(short)l_18) * 2]) << 2)));
    }
    *(int *)&l_1C = (player_class->hp_per_level - 8) * 3277;
    if (player_class->hp_per_level < 8) *(int *)&l_1C <<= 1;
    l_28 = (int)(*(char **)&l_1C + ((l_24 + 65536) - l_2C));
    if (l_28 < 6554) l_28 = 6554;
    player_class->advancement_multiplier = l_28;
    if (l_28 > 65536) {
        scratch_190d68 = 115 - (((l_28 - 65536) * 69) / 262144);
    } else if (l_28 < 65536) {
        scratch_190d68 = (((65536 - l_28) * 69) / 65536) + 115;
    } else {
        scratch_190d68 = 115;
    }
    if (((int)(short)scratch_190d68) < 46) scratch_190d68 = 46;
    return l_28;
}

void classmaker_hp_up(void)
{
    if (player_class->hp_per_level == 30 || D_00190D86 != 0) return;
    D_00190D86 = 20;
    player_class->hp_per_level++;
}

void classmaker_hp_down(void)
{
    if (player_class->hp_per_level == 4 || D_00190D86 != 0) return;
    D_00190D86 = 20;
    player_class->hp_per_level--;
}

int classmaker_skill_taken(short a1)
{
    short l_18;

    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 12; (*(int *)&l_18)++) {
        if ((short)((unsigned short)player_class->skills[(int)(short)l_18]) == a1) return 1;
    }
    return 0;
}

void classmaker_save_file(void)
{
    int l_18;

    if (*(signed char *)classmaker_file == 0) return;
    l_18 = disk_create((int)classmaker_file);
    write(l_18, (int)player_class, 74);
    close(l_18);
}

int classmaker_special_conflicts(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    if (a1 == 0 && a2 >= 2) return 0;
    if (a1 != 0 && a2 != 6 && a2 != 7) return 0;
    for (l_18 = 0; l_18 < 2; l_18++) {
        l_14 = (int)(unsigned char)classmaker_special_counts[l_18];
        for (l_1C = 0; l_1C < l_14; l_1C++) {
            if (l_18 == 0 && ((int)(unsigned char)classmaker_specials[(l_18 * 14) + (l_1C * 2)]) < 2 && ((int)(unsigned char)D_00199821[(l_18 * 14) + (l_1C * 2)]) == a3) {
                return 1;
            }
            if (l_18 != 0 && (((int)(unsigned char)classmaker_specials[(l_18 * 14) + (l_1C * 2)]) == 6 || ((int)(unsigned char)classmaker_specials[(l_18 * 14) + (l_1C * 2)]) == 7) && ((int)(unsigned char)D_00199821[(l_18 * 14) + (l_1C * 2)]) == a3) {
                return 1;
            }
        }
    }
    return 0;
}

int career_specials_text(void)
{
    *(signed char *)((char *)(D_0019981C = *(int *)scratch_buffer + 55000)) = 0;
    career_advantages_text();
    career_disadvantages_text();
    *(signed char *)((char *)(int)(strlen(D_0019981C) + *(char **)&D_0019981C) + 1) = 0;
    return *(int *)scratch_buffer + 55000;
}

void career_advantages_text(void)
{
    int l_18;

    for (l_18 = 0; l_18 < 7; l_18++) {
        if ((player_class->resistance_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_00175477, D_0018087E[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 7; l_18++) {
        if ((player_class->immunity_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_00175482, D_0018087E[l_18]);
        }
    }
    if (((int)(unsigned short)(player_class->flags & 1)) != 0) {
        career_specials_line((int)D_0017548B, 0);
    }
    for (l_18 = 0; l_18 < 3; l_18++) {
        if ((player_class->spell_absorption_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_00175499, D_0018089E[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 3; l_18++) {
        if ((player_class->rapid_healing_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_001754AA, D_001808AE[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if ((player_class->regeneration_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_001754B8, D_001808BE[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if ((player_class->attack_modifier_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_001754C3, D_001808D2[l_18]);
        }
    }
    if (((int)(unsigned short)(player_class->flags & 2)) != 0) {
        career_specials_line((int)D_001754D0, 0);
    }
    if (((player_class->flags >> 10) & 7) < 5) {
        career_specials_line((int)D_001754DD, D_001808E6[((player_class->flags >> 10) & 7)]);
    }
    if (((int)(unsigned short)(player_class->flags & 4)) != 0) {
        career_specials_line((int)D_001754EE, 0);
    }
    for (l_18 = 0; l_18 < 6; l_18++) {
        if ((player_class->expert_weapons & (1 << l_18)) != 0) {
            career_specials_line((int)D_001754FE, D_001808FE[l_18]);
        }
    }
}

void career_disadvantages_text(void)
{
    int l_18;

    if (((int)(unsigned short)(player_class->flags & 8)) != 0) {
        career_specials_line((int)D_0017550B, 0);
    }
    for (l_18 = 0; l_18 < 2; l_18++) {
        if ((player_class->flags & (1 << (l_18 + 4))) != 0) {
            career_specials_line((int)D_00175530, D_0018096E[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if ((player_class->attack_modifier_flags & (1 << (l_18 + 4))) != 0) {
            career_specials_line((int)D_00175537, D_001808D2[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 2; l_18++) {
        if ((player_class->flags & (1 << (l_18 + 6))) != 0) {
            career_specials_line((int)D_0017553E, D_0018097A[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 2; l_18++) {
        if ((player_class->flags & (1 << (l_18 + 8))) != 0) {
            career_specials_line((int)D_00175553, D_00180986[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 6; l_18++) {
        if ((player_class->forbidden_equipment & (1 << l_18)) != 0) {
            career_specials_line((int)D_0017556B, D_001808FE[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 7; l_18++) {
        if ((player_class->low_tolerance_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_0017557E, D_0018087E[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 7; l_18++) {
        if ((player_class->critical_weakness_flags & (1 << l_18)) != 0) {
            career_specials_line((int)D_0017558C, D_0018087E[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 3; l_18++) {
        if ((player_class->forbidden_equipment & (1 << (l_18 + 6))) != 0) {
            career_specials_line((int)D_0017559E, D_00180992[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if ((player_class->forbidden_equipment & (1 << (l_18 + 9))) != 0) {
            career_specials_line((int)D_001755B3, D_001809A2[l_18]);
        }
    }
    for (l_18 = 0; l_18 < 10; l_18++) {
        if ((player_class->forbidden_materials & (1 << l_18)) != 0) {
            career_specials_line((int)D_001755C9, D_001809B6[l_18]);
        }
    }
}

void career_specials_line(int a1, int a2)
{
    if (a2 != 0) {
        mc_set_location(1221, (int)D_00175420);
        mc_sprintf((int)text_buffer, (int)D_001755DC, a1, a2);
    } else {
        mc_strncpy((int)text_buffer, a1, 160, (int)D_00175420, 1223);
    }
    func_000A1054(D_0019981C, (int)text_buffer, (int)D_00175420, 1225, 4);
    func_000A1054(D_0019981C, (int)D_001755E2, (int)D_00175420, 1226, 4);
}

int itemmaker_open(int a1)
{
    if (((int)D_0019626F) == 10 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (a1 != 0) {
        scratch_190d68 = a1;
        D_00187CA8 = 0;
        D_001940D8 |= 4;
        game_mode = 10;
        window_image = disk_read_file((int)D_0017567E, 0);
        magic_window_image = disk_read_file((int)D_0017568B, 0);
        D_00190EE0 = disk_read_file((int)D_00175698, 0);
        D_00196272 = 1;
        D_001940D8 &= 254;
        itemmaker_select_tab(15);
        itemmaker_reset();
    }
    return ((((int)(unsigned char)game_mode) == 10) ? 1 : 0);
}
