/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"
#include "doslow.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern iptr screen_buffer;
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
extern char *skill_names[];
extern char *classmaker_help_topics[];
extern char *classmaker_advantage_names[];
extern iptr D_0018087E[];
extern iptr D_0018089E[];
extern iptr D_001808AE[];
extern iptr D_001808BE[];
extern iptr D_001808D2[];
extern iptr D_001808E6[];
extern iptr D_001808FE[];
extern char *classmaker_disadvantage_names[];
extern iptr D_0018096E[];
extern iptr D_0018097A[];
extern iptr D_00180986[];
extern iptr D_00180992[];
extern iptr D_001809A2[];
extern iptr D_001809B6[];
extern char *classmaker_advantage_costs[];
extern char *classmaker_disadvantage_costs[];
extern struct rect classmaker_buttons[];
extern short classmaker_attribute_up_box;
extern short D_00185646;
extern short D_00185648;
extern short D_0018564A;
extern iptr D_0018564C;
extern short classmaker_attribute_down_box;
extern short D_00185652;
extern short D_00185654;
extern short D_00185656;
extern iptr D_00185658;
extern struct rect classmaker_reputation_buttons[];
extern struct rect classmaker_specials_buttons[];
extern short classmaker_help_texts[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char scratch_190be4[];
extern int scratch_190be8;
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
extern iptr scratch_190de8;
extern iptr scratch_190dec;
extern struct image *scratch_190df0;
extern iptr scratch_190df4;
extern iptr scratch_190df8;
extern iptr scratch_190dfc;
extern iptr D_00190E00;
extern iptr D_00190E0C;
extern iptr D_00190E10;
extern iptr D_00190EE0;
extern char scratch_190ee4[];
extern signed char D_001940D4;
extern signed char D_001940D8;
extern struct record *player_object;
extern iptr magic_window_image;
extern struct character *player_character;
extern iptr window_image;
extern struct career *player_class;
extern char *scratch_buffer;
extern char classmaker_file[];
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern iptr D_0019981C;
extern signed char classmaker_specials[];
extern signed char D_00199821[];
extern signed char D_0019982E[];
extern signed char D_0019982F[];
extern struct rect *classmaker_current_buttons;

extern int classmaker_draw(short);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern int disk_create(char *);
extern int list_popup_update(void);
extern void xn_str_fill_u16(unsigned short *, int, int);
extern int xn_mouse_poll_clamped(void);
extern void xn_mouse_cursor_move(int, int);
extern void xn_draw_get_rect(int, int, int, int, char *, int);
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void classmaker_draw_reputations(void);
extern void classmaker_input_text(iptr, int, iptr);
extern void classmaker_select_attribute(slot16);
extern void classmaker_specials_screen(void);
extern void itemmaker_reset(void);
extern void itemmaker_select_tab(int);
extern void list_popup_open(iptr);
int classmaker_pick_from_list(iptr, iptr);
int classmaker_update_advancement(void);
int classmaker_skill_taken(short);
void classmaker_set_reputation(int);
void classmaker_save_file(void);
void career_advantages_text(void);
void career_disadvantages_text(void);
void career_specials_line(iptr, iptr);
#pragma aux mc_set_location parm routine [];

void classmaker_run(void)
{
    int specials_count;
    int *bios_ticks;
    int *bios_ticks_now;
    int *bios_ticks_set;
    slot16 button_count;
    slot16 button;

    mc_memset((void *)((iptr)player_class + 16), -1, 12, D_00175420, 88, 12);
    player_class->hp_per_level = 8;
    player_class->flags = 5120;
    xn_str_fill_u16((unsigned short *)((iptr)player_class + 58), 50, 16);
    classmaker_special_counts[0] = (D_00190D7F = (D_00190D7C = (D_00190D78 = (classmaker_done = (*(short *)scratch_190d66 = (*(short *)scratch_190d64 = 0))))));
    xn_mouse_cursor_drawn &= 254;
    scratch_190d68 = 115;
    classmaker_select_attribute(0);
    *(short *)classmaker_screen = 0;
    D_00190D84 = 65535;
    D_00190D86 = 0;
    D_001940D8 |= 1;
    if (*(signed char *)classmaker_file != 0) {
        disk_read_file(classmaker_file, (iptr)player_class);
    }
    disk_read_file(D_00175429, screen_buffer);
    scratch_190df4 = (iptr)mc_malloc(64000, D_00175420, 105);
    mc_memcpy((void *)scratch_190df4, (void *)screen_buffer, 64000, D_00175420, 106, 4);
    scratch_190dec = disk_read_file(D_00175436, 0);
    *(iptr *)scratch_190de4 = (iptr)mc_malloc(5520, D_00175420, 109);
    xn_draw_get_rect(219, 46, 40, 138, (char *)*(iptr *)scratch_190de4, 0);
    scratch_190df0 = (struct image *)disk_read_file(D_00175443, 0);
    scratch_190de8 = (iptr)mc_malloc(scratch_190df0->width * scratch_190df0->height, D_00175420, 113);
    scratch_190df8 = disk_read_file(D_00175450, 0);
    D_00190E00 = disk_read_file(D_0017545D, 0);
    scratch_190dfc = disk_read_file(D_0017546A, 0);
    bios_ticks = (int *)DOS_LOW(0x46C);
    *(int *)scratch_190be4 = *bios_ticks;
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
                sound_play(203, player_object, 100);
                ((int (*)())(D_00185658))();
            }
            if (mouse_x > classmaker_attribute_up_box && mouse_x < D_00185648 && mouse_y > D_00185646 && mouse_y < D_0018564A) {
                sound_play(203, player_object, 100);
                ((int (*)())(D_0018564C))();
            }
        }
        if (D_00190D86 != 0) (D_00190D86)--;
        if (*(short *)classmaker_screen == 0) {
            classmaker_current_buttons = classmaker_buttons;
            *(int *)&button_count = 28;
        } else if (((int)(short)*(short *)classmaker_screen) == 1) {
            classmaker_current_buttons = classmaker_reputation_buttons;
            *(int *)&button_count = 6;
        } else {
            classmaker_current_buttons = classmaker_specials_buttons;
            if (((int)(short)(*(short *)classmaker_screen & 16)) != 0) {
                specials_count = 1;
            } else {
                specials_count = 3;
            }
            *(int *)&button_count = specials_count;
        }
        if (((int)(unsigned char)game_mode) != 8) {
            if (*(short *)classmaker_screen == 0 && mouse_buttons != 0 && mouse_buttons != mouse_buttons_prev) {
                *(int *)&button = 0;
                for (; (short)(short)*(int *)&button < (short)button_count; (*(int *)&button)++) {
                    if (mouse_x > classmaker_current_buttons[(short)button].x0 && mouse_x < classmaker_current_buttons[(short)button].x1 && mouse_y > classmaker_current_buttons[(short)button].y0 && mouse_y < classmaker_current_buttons[(short)button].y1) {
                        sound_play(203, player_object, 100);
                        classmaker_current_buttons[(short)button].handler();
                        break;
                    }
                }
            } else if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                *(int *)&button = 0;
                for (; (short)(short)*(int *)&button < (short)button_count; (*(int *)&button)++) {
                    if (mouse_x > classmaker_current_buttons[(short)button].x0 && mouse_x < classmaker_current_buttons[(short)button].x1 && mouse_y > classmaker_current_buttons[(short)button].y0 && mouse_y < classmaker_current_buttons[(short)button].y1) {
                        sound_play(203, player_object, 100);
                        classmaker_current_buttons[(short)button].handler();
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
        mc_memcpy((void *)DOS_LOW(0xA0000), (void *)screen_buffer, 64000, D_00175420, 205, 4);
        bios_ticks_now = (int *)DOS_LOW(0x46C);
        if (*(int *)scratch_190be4 != *bios_ticks_now) {
            D_00190D74++;
            bios_ticks_set = (int *)DOS_LOW(0x46C);
            *(int *)scratch_190be4 = *bios_ticks_set;
        }
    }
    D_001940D8 &= 254;
    mc_free((void *)*(iptr *)scratch_190de4, D_00175420, 217);
    mc_free((void *)scratch_190dec, D_00175420, 218);
    mc_free((void *)scratch_190de8, D_00175420, 219);
    mc_free(scratch_190df0, D_00175420, 220);
    mc_free((void *)scratch_190df4, D_00175420, 221);
    mc_free((void *)scratch_190df8, D_00175420, 222);
    mc_free((void *)scratch_190dfc, D_00175420, 223);
    mc_free((void *)D_00190E00, D_00175420, 224);
    mc_memset(scratch_190de4, 0, 128 * PTR_SIZE, D_00175420, 226, 512);
}

void classmaker_exit_button(void)
{
    if (player_class->name[0] == 0) {
        msgbox_show_rsc(301, 1);
        return;
    }
    if (memchr((char *)((iptr)player_class + 16), -1, 12) != 0) {
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
    classmaker_input_text((iptr)player_class + 28, 14, (iptr)classmaker_draw);
}

void classmaker_skill_primary_1(void)
{
    player_class->skills[0] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_primary_2(void)
{
    player_class->skills[1] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_primary_3(void)
{
    player_class->skills[2] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_major_1(void)
{
    player_class->skills[3] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_major_2(void)
{
    player_class->skills[4] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_major_3(void)
{
    player_class->skills[5] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_minor_1(void)
{
    player_class->skills[6] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_minor_2(void)
{
    player_class->skills[7] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_minor_3(void)
{
    player_class->skills[8] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_minor_4(void)
{
    player_class->skills[9] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_minor_5(void)
{
    player_class->skills[10] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_skill_minor_6(void)
{
    player_class->skills[11] = classmaker_pick_from_list((iptr)(char *)skill_names, scratch_190df4);
}

void classmaker_help_button(void)
{
    msgbox_show_rsc((int)(short)classmaker_help_texts[classmaker_pick_from_list((iptr)(char *)classmaker_help_topics, scratch_190df4)], 1);
}

void classmaker_advantages_button(void)
{
    D_00190E0C = (iptr)(char *)classmaker_advantage_names;
    D_00190E10 = (iptr)classmaker_advantage_costs;
    *(short *)classmaker_screen = 2;
    classmaker_special_list = 0;
}

void classmaker_disadvantages_button(void)
{
    D_00190E0C = (iptr)(char *)classmaker_disadvantage_names;
    D_00190E10 = (iptr)classmaker_disadvantage_costs;
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
    int *bios_ticks;
    int *bios_ticks_now;

    bios_ticks = (int *)DOS_LOW(0x46C);
    if (((unsigned)(*bios_ticks - scratch_190be8)) < 6) return;
    bios_ticks_now = (int *)DOS_LOW(0x46C);
    scratch_190be8 = *bios_ticks_now;
    if (player_class->attributes[(int)(short)chargen_selected_attribute] == 75) return;
    player_class->attributes[(int)(short)chargen_selected_attribute]++;
    (*(short *)scratch_190d64)--;
}

void classmaker_attribute_down(void)
{
    int *bios_ticks;
    int *bios_ticks_now;

    bios_ticks = (int *)DOS_LOW(0x46C);
    if (((unsigned)(*bios_ticks - scratch_190be8)) < 6) return;
    bios_ticks_now = (int *)DOS_LOW(0x46C);
    scratch_190be8 = *bios_ticks_now;
    if (player_class->attributes[(int)(short)chargen_selected_attribute] == 10) return;
    player_class->attributes[(int)(short)chargen_selected_attribute]--;
    (*(short *)scratch_190d64)++;
}

int classmaker_pick_from_list(iptr names, iptr background)
{
    slot16 i;
    slot16 count;

    if (((iptr)(char *)skill_names) == names) {
        *(int *)&i = 0;
        *(int *)&count = *(int *)&i;
        while (*(int *)((char *)((((int)(short)i) << 2) + names)) != 0) {
            if (classmaker_skill_taken((int)(short)i) == 0) {
                scratch_190ce4[(int)(short)count] = *(signed char *)&i;
                *(int *)(scratch_190ee4 + (((int)(short)(*(int *)&count)++) << 2)) = *(int *)(((char *)names + (((int)(short)i) << 2)));
            }
            i++;
        }
        *(int *)(scratch_190ee4 + (((int)(short)count) << 2)) = 0;
        list_popup_open((iptr)scratch_190ee4);
    } else {
        list_popup_open(names);
    }
    D_001940D4 &= 254;
    for (;;) {
        keys_world_actions();
        xn_mouse_poll_clamped();
        mc_memcpy((void *)screen_buffer, (void *)background, 64000, D_00175420, 558, 4);
        i = list_popup_update();
        if (((int)(short)i) > (-1)) {
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            if (((iptr)(char *)skill_names) == names) {
                return (int)(signed char)scratch_190ce4[(int)(short)i];
            }
            return (int)(short)i;
        }
        xn_mouse_cursor_drawn &= 254;
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        mc_memcpy((void *)DOS_LOW(0xA0000), (void *)screen_buffer, 64000, D_00175420, 568, 4);
    }
}

void classmaker_set_reputation(int group)
{
    int unused;
    int centre;

    centre = ((classmaker_reputation_buttons[(int)(short)*(short *)&group].y0) + (classmaker_reputation_buttons[(int)(short)*(short *)&group].y1)) >> 1;
    if (((int)(short)mouse_y) < 81) {
        player_character->reputation[(int)(short)*(short *)&group] = (((int)(short)mouse_y) - 81) / 5;
    } else if (((int)(short)mouse_y) > 81) {
        player_character->reputation[(int)(short)*(short *)&group] = (((int)(short)mouse_y) - 81) / 5;
    } else {
        player_character->reputation[(int)(short)*(short *)&group] = 0;
    }
    player_character->reputation[(int)(short)*(short *)&group] = -player_character->reputation[(int)(short)*(short *)&group];
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
    int i;
    short total;

    i = 0;
    total = i;
    for (; ((int)(short)*(short *)&i) < 5; i++) {
        total += player_character->reputation[(int)(short)*(short *)&i];
    }
    if (total != 0) {
        D_0012B508 = 145;
        msgbox_show_rsc(303, 1);
        return;
    }
    *(short *)classmaker_screen = 0;
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

int classmaker_picklist_wait(void)
{
    int picked;

    for (;;) {
        keys_world_actions();
        xn_mouse_poll_clamped();
        picked = list_popup_update();
        if (picked > (-1)) {
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            xn_mouse_cursor_drawn &= 254;
            return picked + 1;
        }
        if (picked == (-2)) {
            xn_mouse_cursor_drawn &= 254;
            return 0;
        }
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        mc_memcpy((void *)DOS_LOW(0xA0000), (void *)screen_buffer, 64000, D_00175420, 655, 4);
    }
}

int classmaker_drop_duplicate_special(int list)
{
    int i;
    int special;
    int param;

    special = (int)(unsigned char)classmaker_specials[(list * 14) + (((int)(unsigned char)classmaker_special_counts[list]) * 2)];
    param = (int)(unsigned char)D_00199821[(list * 14) + (((int)(unsigned char)classmaker_special_counts[list]) * 2)];
    for (i = 0; ((int)(unsigned char)classmaker_special_counts[list]) > i; i++) {
        if (((int)(unsigned char)classmaker_specials[(list * 14) + (i * 2)]) == special && ((int)(unsigned char)D_00199821[(list * 14) + (i * 2)]) == param) {
            (classmaker_special_counts[list])--;
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
    iptr names;

    *(signed char *)classmaker_screen &= 239;
    if (((int)(short)*(short *)classmaker_screen) == 2) {
        names = (iptr)(char *)classmaker_advantage_names;
    } else {
        names = (iptr)(char *)classmaker_disadvantage_names;
    }
    D_00190E0C = names;
    (classmaker_special_counts[(int)(short)classmaker_special_list])++;
}

int classmaker_update_advancement(void)
{
    int disadvantage_cost;
    int multiplier;
    int advantage_cost;
    slot16 i;
    pslot16 hp_cost;

    multiplier = 65536;
    advantage_cost = 0;
    *(int *)&i = 0;
    for (; (short)(short)((int)(unsigned char)classmaker_special_counts[0]) > (short)i; (*(int *)&i)++) {
        advantage_cost += *(int *)((classmaker_advantage_costs[((int)(unsigned char)classmaker_specials[((int)(short)i) * 2])] + (((int)(unsigned char)D_00199821[((int)(short)i) * 2]) << 2)));
    }
    disadvantage_cost = 0;
    *(int *)&i = 0;
    for (; (short)(short)((int)(unsigned char)D_00190D7F) > (short)i; (*(int *)&i)++) {
        disadvantage_cost += *(int *)((classmaker_disadvantage_costs[((int)(unsigned char)D_0019982E[((int)(short)i) * 2])] + (((int)(unsigned char)D_0019982F[((int)(short)i) * 2]) << 2)));
    }
    *(iptr *)&hp_cost = (player_class->hp_per_level - 8) * 3277;
    if (player_class->hp_per_level < 8) *(iptr *)&hp_cost <<= 1;
    multiplier = (int)(iptr)(*(char **)&hp_cost + ((advantage_cost + 65536) - disadvantage_cost));
    if (multiplier < 6554) multiplier = 6554;
    player_class->advancement_multiplier = multiplier;
    if (multiplier > 65536) {
        scratch_190d68 = 115 - (((multiplier - 65536) * 69) / 262144);
    } else if (multiplier < 65536) {
        scratch_190d68 = (((65536 - multiplier) * 69) / 65536) + 115;
    } else {
        scratch_190d68 = 115;
    }
    if (((int)(short)scratch_190d68) < 46) scratch_190d68 = 46;
    return multiplier;
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

int classmaker_skill_taken(short skill)
{
    slot16 i;

    *(int *)&i = 0;
    for (; ((int)(short)i) < 12; (*(int *)&i)++) {
        if ((short)((unsigned short)player_class->skills[(int)(short)i]) == skill) return 1;
    }
    return 0;
}

void classmaker_save_file(void)
{
    int file;

    if (*(signed char *)classmaker_file == 0) return;
    file = disk_create(classmaker_file);
    write(file, player_class, 74);
    close(file);
}

int classmaker_special_conflicts(int list, int special, int param)
{
    int i;
    int list_index;
    int count;

    if (list == 0 && special >= 2) return 0;
    if (list != 0 && special != 6 && special != 7) return 0;
    for (list_index = 0; list_index < 2; list_index++) {
        count = (int)(unsigned char)classmaker_special_counts[list_index];
        for (i = 0; i < count; i++) {
            if (list_index == 0 && ((int)(unsigned char)classmaker_specials[(list_index * 14) + (i * 2)]) < 2 && ((int)(unsigned char)D_00199821[(list_index * 14) + (i * 2)]) == param) {
                return 1;
            }
            if (list_index != 0 && (((int)(unsigned char)classmaker_specials[(list_index * 14) + (i * 2)]) == 6 || ((int)(unsigned char)classmaker_specials[(list_index * 14) + (i * 2)]) == 7) && ((int)(unsigned char)D_00199821[(list_index * 14) + (i * 2)]) == param) {
                return 1;
            }
        }
    }
    return 0;
}

iptr career_specials_text(void)
{
    *(signed char *)((char *)(D_0019981C = (iptr)scratch_buffer + 55000)) = 0;
    career_advantages_text();
    career_disadvantages_text();
    *(signed char *)((char *)(strlen((char *)D_0019981C) + *(char **)&D_0019981C) + 1) = 0;
    return (iptr)scratch_buffer + 55000;
}

void career_advantages_text(void)
{
    int i;

    for (i = 0; i < 7; i++) {
        if ((player_class->resistance_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_00175477, D_0018087E[i]);
        }
    }
    for (i = 0; i < 7; i++) {
        if ((player_class->immunity_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_00175482, D_0018087E[i]);
        }
    }
    if (((int)(unsigned short)(player_class->flags & 1)) != 0) {
        career_specials_line((iptr)D_0017548B, 0);
    }
    for (i = 0; i < 3; i++) {
        if ((player_class->spell_absorption_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_00175499, D_0018089E[i]);
        }
    }
    for (i = 0; i < 3; i++) {
        if ((player_class->rapid_healing_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_001754AA, D_001808AE[i]);
        }
    }
    for (i = 0; i < 4; i++) {
        if ((player_class->regeneration_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_001754B8, D_001808BE[i]);
        }
    }
    for (i = 0; i < 4; i++) {
        if ((player_class->attack_modifier_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_001754C3, D_001808D2[i]);
        }
    }
    if (((int)(unsigned short)(player_class->flags & 2)) != 0) {
        career_specials_line((iptr)D_001754D0, 0);
    }
    if (((player_class->flags >> 10) & 7) < 5) {
        career_specials_line((iptr)D_001754DD, D_001808E6[((player_class->flags >> 10) & 7)]);
    }
    if (((int)(unsigned short)(player_class->flags & 4)) != 0) {
        career_specials_line((iptr)D_001754EE, 0);
    }
    for (i = 0; i < 6; i++) {
        if ((player_class->expert_weapons & (1 << i)) != 0) {
            career_specials_line((iptr)D_001754FE, D_001808FE[i]);
        }
    }
}

void career_disadvantages_text(void)
{
    int i;

    if (((int)(unsigned short)(player_class->flags & 8)) != 0) {
        career_specials_line((iptr)D_0017550B, 0);
    }
    for (i = 0; i < 2; i++) {
        if ((player_class->flags & (1 << (i + 4))) != 0) {
            career_specials_line((iptr)D_00175530, D_0018096E[i]);
        }
    }
    for (i = 0; i < 4; i++) {
        if ((player_class->attack_modifier_flags & (1 << (i + 4))) != 0) {
            career_specials_line((iptr)D_00175537, D_001808D2[i]);
        }
    }
    for (i = 0; i < 2; i++) {
        if ((player_class->flags & (1 << (i + 6))) != 0) {
            career_specials_line((iptr)D_0017553E, D_0018097A[i]);
        }
    }
    for (i = 0; i < 2; i++) {
        if ((player_class->flags & (1 << (i + 8))) != 0) {
            career_specials_line((iptr)D_00175553, D_00180986[i]);
        }
    }
    for (i = 0; i < 6; i++) {
        if ((player_class->forbidden_equipment & (1 << i)) != 0) {
            career_specials_line((iptr)D_0017556B, D_001808FE[i]);
        }
    }
    for (i = 0; i < 7; i++) {
        if ((player_class->low_tolerance_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_0017557E, D_0018087E[i]);
        }
    }
    for (i = 0; i < 7; i++) {
        if ((player_class->critical_weakness_flags & (1 << i)) != 0) {
            career_specials_line((iptr)D_0017558C, D_0018087E[i]);
        }
    }
    for (i = 0; i < 3; i++) {
        if ((player_class->forbidden_equipment & (1 << (i + 6))) != 0) {
            career_specials_line((iptr)D_0017559E, D_00180992[i]);
        }
    }
    for (i = 0; i < 4; i++) {
        if ((player_class->forbidden_equipment & (1 << (i + 9))) != 0) {
            career_specials_line((iptr)D_001755B3, D_001809A2[i]);
        }
    }
    for (i = 0; i < 10; i++) {
        if ((player_class->forbidden_materials & (1 << i)) != 0) {
            career_specials_line((iptr)D_001755C9, D_001809B6[i]);
        }
    }
}

void career_specials_line(iptr label, iptr detail)
{
    if (detail != 0) {
        mc_set_location(1221, D_00175420);
        mc_sprintf((char *)text_buffer, D_001755DC, label, detail);
    } else {
        mc_strncpy((char *)text_buffer, (char *)label, 160, D_00175420, 1223);
    }
    func_000A1054((char *)D_0019981C, (char *)text_buffer, D_00175420, 1225, 4);
    func_000A1054((char *)D_0019981C, D_001755E2, D_00175420, 1226, 4);
}

int itemmaker_open(int opening)
{
    if (((int)D_0019626F) == 10 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (opening != 0) {
        scratch_190d68 = opening;
        D_00187CA8 = 0;
        D_001940D8 |= 4;
        game_mode = 10;
        window_image = disk_read_file(D_0017567E, 0);
        magic_window_image = disk_read_file(D_0017568B, 0);
        D_00190EE0 = disk_read_file(D_00175698, 0);
        D_00196272 = 1;
        D_001940D8 &= 254;
        itemmaker_select_tab(15);
        itemmaker_reset();
    }
    return ((((int)(unsigned char)game_mode) == 10) ? 1 : 0);
}
