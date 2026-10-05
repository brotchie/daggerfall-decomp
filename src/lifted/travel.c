/* travel.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_enter;
extern short D_00142928;
extern short D_0014292C;
extern short xn_gfx_clip_bottom;
extern int screen_buffer;
extern char D_0017743D[];
extern char D_00177446[];
extern char D_00177455[];
extern char D_0017745D[];
extern char D_00177460[];
extern char D_0017748E[];
extern char D_00177493[];
extern char D_001774A3[];
extern signed char travel_filter;
extern char travel_options[];
extern int D_001835E8[];
extern int D_001837E4[];
extern char region_names[];
extern int D_001846F4;
extern signed char D_00187CA8;
extern signed char location_type_dot_colour[];
extern signed char location_type_category[];
extern short D_00188790[];
extern short D_00188798[];
extern char travel_bar_buttons[];
extern char D_001887A2[];
extern char D_001887A4[];
extern char D_001887A6[];
extern char D_001887A8[];
extern char travel_popup_buttons[];
extern char D_001887F6[];
extern char D_001887F8[];
extern char D_001887FA[];
extern char D_001887FC[];
extern char region_map_origins[];
extern char D_0018889A[];
extern char D_00188994[];
extern char D_0018899C[];
extern char D_001889AC[];
extern signed char D_001889BC;
extern int travel_selected_location;
extern int D_001889C1;
extern int terrain_travel_modifiers[];
extern signed char text_buffer[];
extern int scratch_190cac;
extern signed char scratch_190ce4[];
extern signed char scratch_190ce5;
extern signed char scratch_190ce7;
extern signed char D_00190CE8;
extern signed char scratch_190d16;
extern char scratch_190d64[];
extern char scratch_190d66[];
extern signed char text_rsc_buffer[];
extern char D_00190FE8[];
extern struct record *player_entity;
extern struct record *player_object;
extern char cheat_flags[];
extern char D_00195B5C[];
extern struct character *player_character;
extern int window_image;
extern char scratch_buffer[];
extern int sky_loaded_frame;
extern int text_macro_travel_city;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char climate_is_ocean;
extern signed char night_sky_loaded;
extern int town_building_counter;
extern int region_location_count;
extern char *D_00196A7C;
extern struct map_location *region_locations;
extern char D_001AA64C[];
extern char D_001AA650[];
extern char D_001AA654[];
extern char D_001AA658[];
extern char D_001AA65C[];
extern int D_001AA660;
extern int D_001AA664;
extern char D_001AA668[];
extern int D_001AA66C;
extern char D_001AA670[];
extern int travel_ocean_pixels;
extern int D_001AA678;
extern int D_001AA67C;
extern int D_001AA680;
extern char *D_001AA684;
extern char D_001AA688[];
extern char D_001AA68C[];
extern int D_001AA690;
extern int D_001AA694;
extern int travel_transport_factor;
extern int D_001AA6A0;
extern signed char D_001AA6A4;
extern signed char D_001AA6A5;
extern signed char D_001AA6A6;

extern int climate_at(int, int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int guild_find_membership_by_bits(unsigned char);
extern int key_pressed_once(unsigned char);
extern int gold_can_afford(int);
extern int travel_map_open(int);
extern int travel_route(int, int, int, int, int);
extern int mc_free();
extern int mc_memset();
extern int mc_strncpy();
extern int strlen();
extern int stricmp();
extern int strnicmp();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A134C();
extern int xn_math_approx_dist2d();
extern int xn_draw_zoom4x();
extern int xn_draw_mark_matching();
extern int xn_gfx_present_inclusive();
extern int xn_kbd_flush();
extern int xn_draw_image();
extern int xn_draw_image_transparent();
extern int xn_draw_line_to();
extern void maploads_load_region(int);
extern void region_load_location_names(int);
extern void region_location_names_release(void);
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void palette_restore(void);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern void msgbox_yes_no_rsc(int);
extern void inpstr_begin_text(int, short);
extern void object_foreach(int, int);
extern void travel_draw_trip_popup(void);
int travel_location_at_cursor(void);
int travel_find_location(int);
int travel_trip_cost(void);
void travel_draw_locations(void);
void travel_open_region(int);
void travel_mark_player(int);
void travel_draw_hover_name(void);
void func_0009BE38(void);
void travel_popup_exit(void);
void travel_button_find(void);
void travel_open_trip(void);
void travel_check_transport_item(struct record *);
void travel_load_region_part(void);
void travel_toggle_zoom(void);
void func_0009D5AC(int, int, int);
void travel_draw_buttons(void);
#pragma aux mc_set_location parm routine [];

void travel_map_update(void)
{
    int l_50;
    int l_4C;
    int l_48;
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    struct map_location *l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (travel_map_open(0) == 0) return;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_0017743D, 198, 4);
    travel_draw_buttons();
    if (scratch_190ce4[0] != 0) {
        if (D_00190CE8 != 0) {
            func_0009D5AC((int)(*(char **)&screen_buffer + 3840), (int)&*(signed char *)(*(char **)D_001AA668 + 12), 51200);
        } else {
            mc_memcpy((int)(*(char **)&screen_buffer + 3840), (int)&*(signed char *)(*(char **)D_001AA668 + 12), 51200, (int)D_0017743D, 208, 4);
            xn_draw_image_transparent((int)(unsigned short)*(short *)(*(char **)D_001AA670), (int)(unsigned short)*(short *)(*(char **)D_001AA670 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA670 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA670 + 6), (int)(*(char **)D_001AA670 + 12));
        }
        if (((int)(unsigned char)D_001AA6A4) > 1) {
            if (((int)(unsigned char)(D_001AA6A5 & 1)) == 0) {
                xn_draw_image((int)(unsigned short)*(short *)(*(char **)D_001AA654), (int)(unsigned short)*(short *)(*(char **)D_001AA654 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA654 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA654 + 6), (int)(*(char **)D_001AA654 + 12));
            } else {
                xn_draw_image((int)(unsigned short)*(short *)(*(char **)D_001AA658), (int)(unsigned short)*(short *)(*(char **)D_001AA658 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA658 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA658 + 6), (int)(*(char **)D_001AA658 + 12));
            }
            if (((int)(unsigned char)D_001AA6A4) > 2) {
                if (((int)(unsigned char)(D_001AA6A5 & 2)) == 0) {
                    xn_draw_image((int)(unsigned short)*(short *)(*(char **)D_001AA64C), (int)(unsigned short)*(short *)(*(char **)D_001AA64C + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA64C + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA64C + 6), (int)(*(char **)D_001AA64C + 12));
                } else {
                    xn_draw_image((int)(unsigned short)*(short *)(*(char **)D_001AA650), (int)(unsigned short)*(short *)(*(char **)D_001AA650 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA650 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA650 + 6), (int)(*(char **)D_001AA650 + 12));
                }
            }
        }
    }
    if (scratch_190ce5 != 0) {
        travel_draw_trip_popup();
        l_30 = 1132;
        scratch_190cac = *(int *)((char *)l_30);
    }
    if (scratch_190ce5 == 0) {
        travel_draw_hover_name();
        if (*(int *)D_001AA688 != (-1) && D_00190CE8 == 0) {
            l_2C = 1132;
            if (((unsigned)(*(int *)((char *)l_2C) - scratch_190cac)) > 50) {
                *(int *)D_001AA688 = -1;
            } else {
                l_28 = 1132;
                if (((struct bf8_3_1 *)((char *)l_28))->f != 0) {
                    if (scratch_190ce4[0] != 0) {
                        D_0012B508 = 244;
                        D_00142928 = *(short *)D_001AA688;
                        D_0014292C = 12;
                        xn_draw_line_to((int)(short)*(short *)D_001AA688, 172);
                        D_00142928 = 0;
                        D_0014292C = *(short *)D_001AA68C;
                        xn_draw_line_to(320, (int)(short)*(short *)D_001AA68C);
                    } else {
                        xn_draw_mark_matching(D_001AA66C, screen_buffer, ((int)(unsigned char)current_region) + 128, 64000);
                    }
                }
            }
        }
    }
    if (((int)(unsigned char)game_mode) != 19) return;
    if (scratch_190ce7 != 0) {
        while (key_down_enter != 0);
        key_pressed_once(28);
        scratch_190ce7 = 0;
        l_50 = travel_find_location((int)text_rsc_buffer);
        if (l_50 != (-1)) {
            l_34 = region_locations + l_50;
            if (((int)(unsigned char)D_001AA6A4) < 2) {
                l_44 = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
                l_40 = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
            } else {
                l_44 = ((int)(short)*(short *)((char *)(int)(D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
                l_40 = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
            }
            l_4C = ((unsigned)((l_34->x_type_flags & 33554431) - l_44)) >> 15;
            l_48 = ((unsigned)(-((l_34->z_size & 16777215) - l_40))) >> 15;
            if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
                l_4C <<= 2;
                l_48 <<= 2;
                l_48 += -6;
                l_4C += -2;
            }
            *(int *)D_001AA688 = l_4C;
            *(int *)D_001AA68C = l_48 + 13;
            l_24 = 1132;
            scratch_190cac = *(int *)((char *)l_24);
            text_macro_travel_city = (int)(D_00196A7C + ((travel_selected_location = l_50) << 5)) + 4;
            l_20 = 1132;
            D_001AA694 = (int)(*(char **)((char *)l_20) + 24);
            while (key_down_enter != 0);
            key_pressed_once(28);
        } else {
            msgbox_show_rsc(13, 1);
        }
    }
    l_1C = 1132;
    if (D_001AA694 != 0 && ((unsigned)*(int *)((char *)l_1C)) > D_001AA694) {
        D_001AA694 = 0;
        msgbox_yes_no_rsc(31);
        if (((int)D_00196271) == 1) {
            travel_open_trip();
        } else {
            l_18 = 1132;
            scratch_190cac = *(int *)((char *)l_18);
        }
    }
    if (scratch_190ce5 != 0) {
        if (((int)(unsigned char)D_001AA6A6) == 100) {
            l_38 = 10;
            l_3C = 7;
        } else {
            l_38 = 8;
            l_3C = 0;
        }
        for (l_50 = 0; l_50 < 10; l_50++) {
            if (mouse_x > *(short *)(travel_popup_buttons + (l_50 * 12)) && mouse_x < *(short *)(D_001887F8 + (l_50 * 12)) && mouse_y > *(short *)(D_001887F6 + (l_50 * 12)) && mouse_y < *(short *)(D_001887FA + (l_50 * 12))) {
                ((int (*)())(*(int *)(D_001887FC + (l_50 * 12))))(l_50);
            }
        }
        return;
    }
    if (key_pressed_once(33) != 0) {
        mouse_buttons = 1;
        mouse_buttons_prev = 0;
        travel_button_find();
        mouse_buttons = 0;
        return;
    }
    for (l_50 = 0; l_50 < 7; l_50++) {
        if (mouse_x > *(short *)(travel_bar_buttons + (l_50 * 12)) && mouse_x < *(short *)(D_001887A4 + (l_50 * 12)) && mouse_y > *(short *)(D_001887A2 + (l_50 * 12)) && mouse_y < *(short *)(D_001887A6 + (l_50 * 12))) {
            ((int (*)())(*(int *)(D_001887A8 + (l_50 * 12))))(l_50);
        }
    }
}

void travel_button_exit(int a1)
{
    if (a1 != 100 && (((((int)(unsigned char)(mouse_buttons & 1)) == 0) || ((((int)(unsigned char)(mouse_buttons_prev & 1)) != 0))) ? 1 : 0) != 0) {
        return;
    }
    if (scratch_190ce4[0] != 0) {
        func_0009BE38();
        return;
    }
    sound_play(203, (int)player_object, 110);
    mc_memset(screen_buffer, 0, 64000, (int)D_0017743D, 367, 4);
    xn_gfx_present_inclusive(1);
    palette_restore();
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
    D_00187CA8 = 1;
    maploads_load_region((int)(unsigned char)current_region);
    region_location_names_release();
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_0017743D, 380);
        window_image = -1751672937;
    }
    if (D_001AA6A0 != 0 && D_001AA6A0 != (-1751672937)) {
        mc_free(D_001AA6A0, (int)D_0017743D, 381);
        D_001AA6A0 = -1751672937;
    }
    if (D_001AA66C != 0 && D_001AA66C != (-1751672937)) {
        mc_free(D_001AA66C, (int)D_0017743D, 382);
        D_001AA66C = -1751672937;
    }
    if (*(int *)D_00195B5C != 0 && *(int *)D_00195B5C != (-1751672937)) {
        mc_free(*(int *)D_00195B5C, (int)D_0017743D, 383);
        *(int *)D_00195B5C = -1751672937;
    }
    if (*(int *)D_001AA670 != 0 && *(int *)D_001AA670 != (-1751672937)) {
        mc_free(*(int *)D_001AA670, (int)D_0017743D, 384);
        *(int *)D_001AA670 = -1751672937;
    }
    if (*(int *)D_001AA64C != 0 && *(int *)D_001AA64C != (-1751672937)) {
        mc_free(*(int *)D_001AA64C, (int)D_0017743D, 385);
        *(int *)D_001AA64C = -1751672937;
    }
    if (*(int *)D_001AA650 != 0 && *(int *)D_001AA650 != (-1751672937)) {
        mc_free(*(int *)D_001AA650, (int)D_0017743D, 386);
        *(int *)D_001AA650 = -1751672937;
    }
    if (*(int *)D_001AA654 != 0 && *(int *)D_001AA654 != (-1751672937)) {
        mc_free(*(int *)D_001AA654, (int)D_0017743D, 387);
        *(int *)D_001AA654 = -1751672937;
    }
    if (*(int *)D_001AA658 != 0 && *(int *)D_001AA658 != (-1751672937)) {
        mc_free(*(int *)D_001AA658, (int)D_0017743D, 388);
        *(int *)D_001AA658 = -1751672937;
    }
    if (*(int *)D_001AA65C != 0 && *(int *)D_001AA65C != (-1751672937)) {
        mc_free(*(int *)D_001AA65C, (int)D_0017743D, 389);
        *(int *)D_001AA65C = -1751672937;
    }
    if (D_001AA660 != 0 && D_001AA660 != (-1751672937)) {
        mc_free(D_001AA660, (int)D_0017743D, 390);
        D_001AA660 = -1751672937;
    }
    if (*(int *)D_001AA668 != 0 && *(int *)D_001AA668 != (-1751672937)) {
        mc_free(*(int *)D_001AA668, (int)D_0017743D, 392);
        *(int *)D_001AA668 = -1751672937;
    }
    game_mode = 0;
    D_00196272 = 0;
}

void travel_draw_locations(void)
{
    struct map_location *l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    short l_18;

    l_30 = region_locations;
    *(int *)&l_18 = (int)(short)xn_gfx_clip_bottom;
    xn_gfx_clip_bottom = 160;
    if (((int)(unsigned char)D_001AA6A4) < 2) {
        l_2C = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
        l_28 = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
    } else {
        l_2C = ((int)(short)*(short *)((char *)(int)(D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
        l_28 = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
    }
    for (l_24 = 0; l_24 < region_location_count; l_24++, l_30++) {
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((l_30->x_type_flags & 0x40000000) == 0 || (l_30->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        if ((((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)(l_30->x_type_flags << 2)) >> 27]))) == 0) continue;
        l_20 = ((unsigned)((l_30->x_type_flags & 33554431) - l_2C)) >> 15;
        l_1C = ((unsigned)(-((l_30->z_size & 16777215) - l_28))) >> 15;
        if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
            l_20 <<= 2;
            l_1C <<= 2;
            l_1C += -6;
            l_20 += -2;
        }
        if (l_20 == 139 && l_1C == 165) (town_building_counter)++;
        func_000A134C((int)(short)(l_20 + 12), (int)(short)(l_1C + 1), (int)(unsigned char)location_type_dot_colour[((unsigned)(l_30->x_type_flags << 2)) >> 27]);
    }
    xn_gfx_clip_bottom = *(int *)&l_18;
}

void travel_open_region(int a1)
{
    int l_18;

    scratch_190ce4[0] = *(signed char *)&a1 + 1;
    D_001AA6A4 = (D_001AA6A5 = 0);
    if (a1 == 0 || a1 == 1 || a1 == 16) {
        maploads_load_region(a1);
        region_load_location_names(a1);
        travel_load_region_part();
    } else {
        if (*(int *)D_001AA668 != 0 && *(int *)D_001AA668 != (-1751672937)) {
            mc_free(*(int *)D_001AA668, (int)D_0017743D, 460);
            *(int *)D_001AA668 = -1751672937;
        }
        mc_set_location(461, (int)D_0017743D);
        mc_sprintf((int)text_buffer, (int)D_00177446, a1);
        *(int *)D_001AA668 = disk_read_file((int)text_buffer, 0);
        maploads_load_region(a1);
        region_load_location_names(a1);
        l_18 = screen_buffer;
        screen_buffer = *(int *)D_001AA668;
        travel_draw_locations();
        screen_buffer = l_18;
    }
    switch ((unsigned)a1) {
    case 0:
        D_001AA6A4 = 2;
        *(int *)&D_001AA684 = (int)D_00188994;
        break;
    case 1:
        D_001AA6A4 = 4;
        *(int *)&D_001AA684 = (int)D_0018899C;
        break;
    case 16:
        D_001AA6A4 = 4;
        *(int *)&D_001AA684 = (int)D_001889AC;
        break;
    default:
        D_001AA6A4 = 0;
    }
    travel_mark_player(a1);
}

void travel_mark_player(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (((int)(unsigned char)current_region) == a1) {
        if (((int)(unsigned char)D_001AA6A4) < 2) {
            l_20 = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
            l_1C = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
        } else {
            l_20 = ((int)(short)*(short *)((char *)(int)(D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
            l_1C = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
        }
        l_28 = (player_object->x - l_20) / 32768;
        l_24 = (-(player_object->z - l_1C)) / 32768;
        if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
            l_28 <<= 2;
            l_24 <<= 2;
            l_24 += -6;
            l_28 += -2;
        }
        *(int *)D_001AA688 = l_28;
        *(int *)D_001AA68C = l_24 + 13;
        l_18 = 1132;
        scratch_190cac = *(int *)((char *)l_18);
        return;
    }
    *(int *)D_001AA688 = -1;
}

void travel_draw_hover_name(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (((int)(short)mouse_y) > 171) return;
    if (scratch_190ce4[0] != 0) {
        l_18 = travel_location_at_cursor();
        if (l_18 != (-1)) {
            if (((struct bf8_3_1 *)&cheat_flags)->f != 0) {
                if ((((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)((region_locations)[l_18].x_type_flags << 2)) >> 27]))) != 0) {
                    mc_set_location(541, (int)D_0017743D);
                    mc_sprintf((int)text_buffer, (int)D_00177455, D_001837E4[((int)(signed char)scratch_190ce4[0])], (int)(D_00196A7C + (l_18 << 5)) + 4);
                } else {
                    mc_strncpy((int)text_buffer, D_001AA664, 160, (int)D_0017743D, 543);
                }
            } else if ((((region_locations)[l_18].x_type_flags & 0x40000000) != 0 || ((region_locations)[l_18].x_type_flags & 0x80000000) == 0) && (((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)((region_locations)[l_18].x_type_flags << 2)) >> 27]))) != 0) {
                mc_set_location(548, (int)D_0017743D);
                mc_sprintf((int)text_buffer, (int)D_00177455, D_001837E4[((int)(signed char)scratch_190ce4[0])], (int)(D_00196A7C + (l_18 << 5)) + 4);
            } else {
                mc_strncpy((int)text_buffer, D_001AA664, 160, (int)D_0017743D, 550);
            }
        } else {
            mc_strncpy((int)text_buffer, D_001AA664, 160, (int)D_0017743D, 554);
        }
        l_1C = 2;
    } else {
        l_20 = (int)(unsigned char)*(signed char *)((char *)(int)(*(char **)&D_001AA66C + ((((int)(short)mouse_y) * 320) + ((int)(short)mouse_x))));
        if (l_20 < 128 || l_20 > 191) return;
        mc_set_location(562, (int)D_0017743D);
        mc_sprintf((int)text_buffer, (int)D_0017745D, D_001835E8[l_20]);
        l_1C = 2;
    }
    if (text_buffer[0] == 0) return;
    text_draw_centred_coloured((int)text_buffer, 160, (int)(short)*(short *)&l_1C, 145, 156);
}

void travel_button_map(void)
{
    int l_18;

    if (((int)(unsigned char)(mouse_buttons & 2)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 2)) == 0 && scratch_190ce4[0] != 0) {
        travel_toggle_zoom();
        return;
    }
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] != 0) {
        travel_open_trip();
        return;
    }
    D_00190CE8 = 0;
    sound_play(203, (int)player_object, 110);
    l_18 = (int)(unsigned char)*(signed char *)((char *)(int)(*(char **)&D_001AA66C + ((((int)(short)mouse_y) * 320) + ((int)(short)mouse_x))));
    if (l_18 < 128 || l_18 == 255) return;
    D_001889BC = *(signed char *)&l_18 - 128;
    D_001AA664 = *(int *)(region_names + (((int)(unsigned char)D_001889BC) << 2));
    travel_open_region((int)(unsigned char)D_001889BC);
}

void func_0009BE38(void)
{
    if (*(int *)D_001AA668 != 0) {
        if (*(int *)D_001AA668 != 0 && *(int *)D_001AA668 != (-1751672937)) {
            mc_free(*(int *)D_001AA668, (int)D_0017743D, 605);
            *(int *)D_001AA668 = -1751672937;
        }
        *(int *)D_001AA688 = -1;
    }
    scratch_190ce4[0] = 0;
    D_00190CE8 = 0;
}

void travel_toggle_option(int a1)
{
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    sound_play(203, (int)player_object, 110);
    if (a1 < 2) {
        *(signed char *)travel_options ^= 3;
    } else if (a1 < 4) {
        *(signed char *)travel_options ^= 12;
        if (((int)(unsigned short)(*(short *)travel_options & 8)) != 0 && player_character->ship_owned == 0 && gold_can_afford(travel_trip_cost()) == 0) {
            *(signed char *)travel_options ^= 12;
            D_0012B508 = 146;
            msgbox_show_rsc(454, 1);
        }
    } else {
        *(signed char *)travel_options ^= 48;
        if (((int)(unsigned short)(*(short *)travel_options & 16)) != 0 && gold_can_afford(travel_trip_cost()) == 0) {
            *(signed char *)travel_options ^= 48;
            D_0012B508 = 146;
            msgbox_show_rsc(454, 1);
        }
    }
    D_001AA680 = travel_route(player_object->x, player_object->z, D_001AA678, D_001AA67C, 0);
    if (((int)(unsigned short)(*(short *)travel_options & 3)) != 2) return;
    D_001AA680 = (D_001AA680 << 7) / 256;
}

void travel_popup_exit(void)
{
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    sound_play(203, (int)player_object, 110);
    scratch_190ce5 = 0;
}

void func_0009C27F(void)
{
    travel_popup_exit();
}

void travel_button_find(void)
{
    int l_18;

    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] == 0) return;
    D_00190CE8 = 0;
    sound_play(203, (int)player_object, 110);
    D_0012B508 = 145;
    xn_kbd_flush();
    l_18 = *(int *)scratch_buffer + 55000;
    mc_set_location(807, (int)D_0017743D);
    mc_sprintf(l_18, (int)D_00177460, D_001846F4);
    *(signed char *)((char *)(strlen(l_18) + l_18) + 1) = 0;
    text_rsc_buffer[0] = 0;
    inpstr_begin_text((int)text_rsc_buffer, 32);
    msgbox_show_string(l_18, 2);
    if (strnicmp((int)D_0017748E, (int)text_rsc_buffer, 4) == 0) {
        mc_memcpy((int)text_rsc_buffer, (int)D_00190FE8, (int)&*(signed char *)((char *)strlen((int)text_rsc_buffer) + 1), (int)D_0017743D, 812, 2048);
    }
    scratch_190ce7 = 1;
}

int travel_location_at_cursor(void)
{
    struct map_location *l_44;
    struct map_location *l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_44 = region_locations;
    l_28 = 32767;
    if (D_001AA694 != 0) return -1;
    if (((int)(short)mouse_y) > 171) return -1;
    if (D_00190CE8 != 0) {
        l_20 = (((int)(short)mouse_x) >> 2) + ((int)(short)*(short *)scratch_190d64);
        l_1C = ((((int)(short)mouse_y) - 12) >> 2) + ((int)(short)*(short *)scratch_190d66);
    } else {
        l_20 = (int)(short)mouse_x;
        l_1C = ((int)(short)mouse_y) - 12;
    }
    if (((int)(unsigned char)D_001AA6A4) < 2) {
        l_3C = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
        l_38 = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
    } else {
        l_3C = ((int)(short)*(short *)((char *)(int)(D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
        l_38 = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
    }
    if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
        l_20 >>= 2;
        l_1C >>= 2;
        l_1C += 2;
        l_20 += 2;
    }
    for (l_24 = 0; l_24 < region_location_count; l_24++, l_44++) {
        if ((((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)(l_44->x_type_flags << 2)) >> 27]))) == 0) continue;
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((l_44->x_type_flags & 0x40000000) == 0 || (l_44->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        l_34 = ((unsigned)((l_44->x_type_flags & 33554431) - l_3C)) >> 15;
        l_30 = ((unsigned)(-((l_44->z_size & 16777215) - l_38))) >> 15;
        l_2C = xn_math_approx_dist2d(l_34, l_30, l_20, l_1C);
        if (l_2C < l_28) {
            l_40 = l_44;
            l_28 = l_2C;
        }
        if (l_28 < 2) break;
    }
    if (l_24 != region_location_count) {
        D_001889C1 = (travel_selected_location = l_24);
        return l_24;
    }
    D_001889C1 = (travel_selected_location = -1);
    return -1;
}

int travel_find_location(int a1)
{
    int l_28;
    int l_24;
    struct map_location *l_20;
    int l_1C;

    l_24 = strlen(a1);
    l_20 = region_locations;
    if (l_24 == 0) return -1;
    for (l_28 = 0; l_28 < region_location_count; l_28++, l_20++) {
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((l_20->x_type_flags & 0x40000000) == 0 || (l_20->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        l_1C = (int)(D_00196A7C + (l_28 << 5)) + 4;
        if (strnicmp((int)D_0017748E, l_1C, 4) == 0) l_1C += 4;
        if (stricmp(a1, l_1C) == 0) return l_28;
    }
    l_20 = region_locations;
    for (l_28 = 0; l_28 < region_location_count; l_28++, l_20++) {
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((l_20->x_type_flags & 0x40000000) == 0 || (l_20->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        l_1C = (int)(D_00196A7C + (l_28 << 5)) + 4;
        if (strnicmp((int)D_0017748E, l_1C, 4) == 0) l_1C += 4;
        if (strnicmp(a1, l_1C, l_24) == 0) return l_28;
    }
    return -1;
}

void travel_open_trip(void)
{
    if (travel_selected_location == (-1)) return;
    scratch_190ce5 = 1;
    D_001AA678 = (region_locations)[travel_selected_location].x_type_flags & 33554431;
    D_001AA67C = (region_locations)[travel_selected_location].z_size & 16777215;
    D_001AA680 = travel_route(player_object->x, player_object->z, D_001AA678, D_001AA67C, 0);
    if (((int)(unsigned short)(*(short *)travel_options & 3)) == 2) {
        D_001AA680 = (D_001AA680 << 7) / 256;
    }
    if (((int)(unsigned short)(*(short *)travel_options & 8)) == 0 || player_character->ship_owned != 0 || gold_can_afford(travel_trip_cost()) != 0) {
        return;
    }
    *(signed char *)travel_options ^= 12;
}

int func_0009CD32(int a1)
{
    struct map_location *l_48;
    struct map_location *l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_48 = region_locations;
    l_2C = 32767;
    l_24 = player_object->x / 32768;
    l_20 = player_object->z / 32768;
    if (((int)(unsigned char)D_001AA6A4) < 2) {
        l_40 = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
        l_3C = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
    } else {
        l_40 = ((int)(short)*(short *)((char *)(int)(D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
        l_3C = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
    }
    for (l_28 = 0; l_28 < region_location_count; l_28++) {
        if ((((unsigned)(l_48->x_type_flags << 2)) >> 27) != a1) continue;
        l_38 = ((unsigned)((l_48->x_type_flags & 33554431) - l_40)) >> 15;
        l_34 = ((unsigned)(-((l_48->z_size & 16777215) - l_3C))) >> 15;
        l_30 = xn_math_approx_dist2d(l_38, l_34, l_24, l_20);
        if (l_30 < l_2C) {
            l_1C = l_28;
            l_44 = l_48;
            l_2C = l_30;
        }
        l_48++;
    }
    if (l_2C == 32767) return D_001889C1;
    return l_1C;
}

int travel_pixel_time(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = climate_at((a1 << 15) + 16384, (int)&*(signed char *)((char *)(a2 << 15) + 16384));
    l_18 = (travel_transport_factor * 102) / 256;
    if (climate_is_ocean != 0) {
        travel_ocean_pixels++;
        if (((int)(unsigned short)(*(short *)travel_options & 8)) != 0 || player_character->ship_owned != 0) {
            return 51;
        }
        return 255;
    }
    return (((256 - terrain_travel_modifiers[l_1C]) + 256) * l_18) / 256;
}

void travel_check_transport_item(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group == 23 && l_18->index == 1) {
        scratch_190d16 |= 2;
        travel_transport_factor = 128;
    }
    if (l_18->group != 23 || l_18->index != 0) return;
    scratch_190d16 |= 1;
    travel_transport_factor = 192;
}

void travel_find_transport(void)
{
    travel_transport_factor = 256;
    object_foreach((int)player_entity->children, (int)travel_check_transport_item);
}

void travel_load_region_part(void)
{
    int l_18;

    if (*(int *)D_001AA668 != 0 && *(int *)D_001AA668 != (-1751672937)) {
        mc_free(*(int *)D_001AA668, (int)D_0017743D, 1119);
        *(int *)D_001AA668 = -1751672937;
    }
    mc_set_location(1121, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_00177493, ((int)(unsigned char)D_001AA6A5) + 97, ((int)(signed char)scratch_190ce4[0]) - 1);
    *(int *)D_001AA668 = disk_read_file((int)text_buffer, 0);
    l_18 = screen_buffer;
    screen_buffer = *(int *)D_001AA668;
    travel_draw_locations();
    screen_buffer = l_18;
}

void travel_button_arrows(int a1)
{
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] == 0) return;
    if (((int)(unsigned char)D_001AA6A4) < 2) return;
    if (a1 == 3) {
        if (((int)(unsigned char)D_001AA6A4) > 2) D_001AA6A5 ^= 2;
    } else {
        D_001AA6A5 ^= 1;
    }
    sound_play(203, (int)player_object, 110);
    travel_load_region_part();
}

void travel_toggle_zoom(void)
{
    D_00190CE8 ^= 1;
    *(short *)scratch_190d64 = mouse_x - 40;
    *(short *)scratch_190d66 = mouse_y - 32;
    if (*(short *)scratch_190d64 < 0) *(short *)scratch_190d64 = 0;
    if (*(short *)scratch_190d66 < 0) *(short *)scratch_190d66 = 0;
    if ((((int)(short)*(short *)scratch_190d64) + 80) > 319) *(short *)scratch_190d64 = 239;
    if ((((int)(short)*(short *)scratch_190d66) + 40) <= 148) return;
    *(short *)scratch_190d66 = 108;
}

void func_0009D5AC(int a1, int a2, int a3)
{
    int l_10;

    a2 = (a2 + (((int)(short)*(short *)scratch_190d66) * 320)) + ((int)(short)*(short *)scratch_190d64);
    for (l_10 = 0; l_10 < 40; l_10++) {
        xn_draw_zoom4x((l_10 * 320) + a2, (l_10 * 1280) + a1, 80);
    }
}

int travel_trip_cost(void)
{
    int l_20;
    int l_1C;

    l_20 = (D_001AA680 + 59) / 60;
    l_1C = 0;
    if (((int)(unsigned short)(*(short *)travel_options & 16)) != 0 && guild_find_membership_by_bits(64) == 0) {
        l_1C = (((l_20 - travel_ocean_pixels) / 24) * 5) + 5;
    }
    if (travel_ocean_pixels != 0 && player_character->ship_owned == 0 && ((int)(unsigned short)(*(short *)travel_options & 8)) != 0 && guild_find_membership_by_bits(64) == 0) {
        l_1C += ((travel_ocean_pixels / 24) + 1) * 25;
    }
    return l_1C;
}

void travel_button_im_at(void)
{
    int l_18;

    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] != 0) {
        travel_mark_player((int)(unsigned char)current_region);
        return;
    }
    *(int *)D_001AA688 = 0;
    l_18 = 1132;
    scratch_190cac = *(int *)((char *)l_18);
}

void travel_draw_buttons(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (scratch_190ce4[0] != 0) {
        xn_draw_image(3, 175, 45, 22, *(int *)D_00195B5C);
    } else {
        xn_draw_image(3, 186, 45, 11, *(int *)D_00195B5C + 495);
    }
    for (l_24 = 0; l_24 < 4; l_24++) {
        l_20 = 1;
        if ((((int)(unsigned char)travel_filter) & (1 << l_24)) != 0) l_20 = 0;
        if ((l_24 & 1) != 0) {
            l_18 = 80;
        } else {
            l_18 = 99;
        }
        for (l_1C = 0; l_1C < 11; l_1C++) {
            mc_memcpy(((int)(short)D_00188790[l_24]) + (int)(*(char **)&screen_buffer + ((((int)(short)D_00188798[l_24]) + l_1C) * 320)), (int)(*(char **)(D_001AA65C + (l_20 << 2)) + (((((int)(short)D_00188798[l_24]) + l_1C) - 175) * 179)) + (((int)(short)D_00188790[l_24]) - 50), l_18, (int)D_0017743D, 1222, 4);
        }
    }
}

void travel_button_filter(void)
{
    int l_18;

    l_18 = 0;
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] == 0) return;
    if (((int)(short)mouse_y) < 186) {
        if (((int)(short)mouse_x) < 149) {
            travel_filter ^= 1;
        } else {
            travel_filter ^= 2;
        }
    } else if (((int)(short)mouse_x) < 149) {
        travel_filter ^= 4;
    } else {
        travel_filter ^= 8;
    }
    travel_open_region(((int)(signed char)scratch_190ce4[0]) - 1);
}

int func_0009D960(int a1, int a2)
{
    return 0;
}

void travel_show_days_left(int a1)
{
    mc_memcpy(screen_buffer, D_001AA690, 64000, (int)D_0017743D, 1259, 4);
    mc_set_location(1260, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_001774A3, ((unsigned)a1) / 1440);
    text_draw_coloured((int)text_buffer, 240, 2, 145, 156);
    xn_gfx_present_inclusive(1);
}
