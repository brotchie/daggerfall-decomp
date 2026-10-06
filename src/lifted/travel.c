/* travel.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#include "doslow.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_enter;
extern short D_00142928;
extern short D_0014292C;
extern short xn_gfx_clip_bottom;
extern iptr screen_buffer;
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
extern iptr D_001835E8[];
extern iptr D_001837E4[];
extern char *region_names[];
extern iptr D_001846F4;
extern signed char D_00187CA8;
extern signed char location_type_dot_colour[];
extern signed char location_type_category[];
extern short D_00188790[];
extern short D_00188798[];
extern struct rect travel_bar_buttons[];
extern struct rect travel_popup_buttons[];
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
extern struct image *D_00195B5C;
extern struct character *player_character;
extern iptr window_image;
extern char *scratch_buffer;
extern int sky_loaded_frame;
extern iptr text_macro_travel_city;
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
extern struct image *D_001AA64C;
extern struct image *D_001AA650;
extern struct image *D_001AA654;
extern struct image *D_001AA658;
extern char *D_001AA65C[];
extern iptr D_001AA660;
extern iptr D_001AA664;
extern struct image *D_001AA668;
extern iptr D_001AA66C;
extern struct image *D_001AA670;
extern int travel_ocean_pixels;
extern int D_001AA678;
extern int D_001AA67C;
extern int D_001AA680;
extern char *D_001AA684;
extern char D_001AA688[];
extern char D_001AA68C[];
extern iptr D_001AA690;
extern int D_001AA694;
extern int travel_transport_factor;
extern iptr D_001AA6A0;
extern signed char D_001AA6A4;
extern signed char D_001AA6A5;
extern signed char D_001AA6A6;

extern int climate_at(int, int);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern struct membership *guild_find_membership_by_bits(unsigned char);
extern int key_pressed_once(unsigned char);
extern int gold_can_afford(int);
extern int travel_map_open(int);
extern int travel_route(int, int, int, int, int);
extern int func_000A134C(int, int, int);
extern int xn_math_approx_dist2d(int, int, int, int);
extern void xn_draw_zoom4x(char *, char *, int);
extern void xn_draw_mark_matching(char *, char *, int, int);
extern void xn_gfx_present_inclusive(int);
extern void xn_kbd_flush(void);
extern void xn_draw_image(int, int, int, int, char *);
extern void xn_draw_image_transparent(int, int, int, int, char *);
extern void xn_draw_line_to(int, int);
extern void maploads_load_region(int);
extern void region_load_location_names(int);
extern void region_location_names_release(void);
extern void msgbox_show_string(iptr, int);
extern void msgbox_show_rsc(int, int);
extern void palette_restore(void);
extern void text_draw_coloured(iptr, int, int, int, unsigned char);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern void msgbox_yes_no_rsc(int);
extern void inpstr_begin_text(char *, int);
extern void object_foreach(iptr, iptr);
extern void travel_draw_trip_popup(void);
int travel_location_at_cursor(void);
int travel_find_location(char *);
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
void func_0009D5AC(char *, char *, int);
void travel_draw_buttons(void);
#pragma aux mc_set_location parm routine [];

void travel_map_update(void)
{
    int i;
    int x;
    int y;
    int origin_x;
    int origin_z;
    int first_button;
    int end_button;
    struct map_location *location;
    int *popup_ticks;
    int *marker_ticks;
    int *blink_ticks;
    int *found_ticks;
    int *prompt_ticks;
    int *prompt_due_ticks;
    int *cancel_ticks;

    if (travel_map_open(0) == 0) return;
    mc_memcpy((void *)screen_buffer, (void *)window_image, 64000, D_0017743D, 198, 4);
    travel_draw_buttons();
    if (scratch_190ce4[0] != 0) {
        if (D_00190CE8 != 0) {
            func_0009D5AC(*(char **)&screen_buffer + 3840, D_001AA668->pixels, 51200);
        } else {
            mc_memcpy((*(char **)&screen_buffer + 3840), D_001AA668->pixels, 51200, D_0017743D, 208, 4);
            xn_draw_image_transparent(D_001AA670->x, D_001AA670->y, D_001AA670->width, D_001AA670->height, D_001AA670->pixels);
        }
        if (((int)(unsigned char)D_001AA6A4) > 1) {
            if (((int)(unsigned char)(D_001AA6A5 & 1)) == 0) {
                xn_draw_image(D_001AA654->x, D_001AA654->y, D_001AA654->width, D_001AA654->height, D_001AA654->pixels);
            } else {
                xn_draw_image(D_001AA658->x, D_001AA658->y, D_001AA658->width, D_001AA658->height, D_001AA658->pixels);
            }
            if (((int)(unsigned char)D_001AA6A4) > 2) {
                if (((int)(unsigned char)(D_001AA6A5 & 2)) == 0) {
                    xn_draw_image(D_001AA64C->x, D_001AA64C->y, D_001AA64C->width, D_001AA64C->height, D_001AA64C->pixels);
                } else {
                    xn_draw_image(D_001AA650->x, D_001AA650->y, D_001AA650->width, D_001AA650->height, D_001AA650->pixels);
                }
            }
        }
    }
    if (scratch_190ce5 != 0) {
        travel_draw_trip_popup();
        popup_ticks = (int *)DOS_LOW(0x46C);
        scratch_190cac = *popup_ticks;
    }
    if (scratch_190ce5 == 0) {
        travel_draw_hover_name();
        if (*(int *)D_001AA688 != (-1) && D_00190CE8 == 0) {
            marker_ticks = (int *)DOS_LOW(0x46C);
            if (((unsigned)(*marker_ticks - scratch_190cac)) > 50) {
                *(int *)D_001AA688 = -1;
            } else {
                blink_ticks = (int *)DOS_LOW(0x46C);
                if (((struct bf8_3_1 *)blink_ticks)->f != 0) {
                    if (scratch_190ce4[0] != 0) {
                        D_0012B508 = 244;
                        D_00142928 = *(short *)D_001AA688;
                        D_0014292C = 12;
                        xn_draw_line_to((int)(short)*(short *)D_001AA688, 172);
                        D_00142928 = 0;
                        D_0014292C = *(short *)D_001AA68C;
                        xn_draw_line_to(320, (int)(short)*(short *)D_001AA68C);
                    } else {
                        xn_draw_mark_matching((char *)D_001AA66C, (char *)screen_buffer, ((int)(unsigned char)current_region) + 128, 64000);
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
        i = travel_find_location((char *)text_rsc_buffer);
        if (i != (-1)) {
            location = region_locations + i;
            if (((int)(unsigned char)D_001AA6A4) < 2) {
                origin_x = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
                origin_z = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
            } else {
                origin_x = ((int)(short)*(short *)((D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
                origin_z = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
            }
            x = ((unsigned)((location->x_type_flags & 33554431) - origin_x)) >> 15;
            y = ((unsigned)(-((location->z_size & 16777215) - origin_z))) >> 15;
            if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
                x <<= 2;
                y <<= 2;
                y += -6;
                x += -2;
            }
            *(int *)D_001AA688 = x;
            *(int *)D_001AA68C = y + 13;
            found_ticks = (int *)DOS_LOW(0x46C);
            scratch_190cac = *found_ticks;
            text_macro_travel_city = (iptr)(D_00196A7C + ((travel_selected_location = i) << 5)) + 4;
            prompt_ticks = (int *)DOS_LOW(0x46C);
            D_001AA694 = *prompt_ticks + 24;
            while (key_down_enter != 0);
            key_pressed_once(28);
        } else {
            msgbox_show_rsc(13, 1);
        }
    }
    prompt_due_ticks = (int *)DOS_LOW(0x46C);
    if (D_001AA694 != 0 && ((unsigned)*prompt_due_ticks) > D_001AA694) {
        D_001AA694 = 0;
        msgbox_yes_no_rsc(31);
        if (((int)D_00196271) == 1) {
            travel_open_trip();
        } else {
            cancel_ticks = (int *)DOS_LOW(0x46C);
            scratch_190cac = *cancel_ticks;
        }
    }
    if (scratch_190ce5 != 0) {
        if (((int)(unsigned char)D_001AA6A6) == 100) {
            end_button = 10;
            first_button = 7;
        } else {
            end_button = 8;
            first_button = 0;
        }
        for (i = 0; i < 10; i++) {
            if (mouse_x > travel_popup_buttons[i].x0 && mouse_x < travel_popup_buttons[i].x1 && mouse_y > travel_popup_buttons[i].y0 && mouse_y < travel_popup_buttons[i].y1) {
                travel_popup_buttons[i].handler(i);
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
    for (i = 0; i < 7; i++) {
        if (mouse_x > travel_bar_buttons[i].x0 && mouse_x < travel_bar_buttons[i].x1 && mouse_y > travel_bar_buttons[i].y0 && mouse_y < travel_bar_buttons[i].y1) {
            travel_bar_buttons[i].handler(i);
        }
    }
}

void travel_button_exit(int button)
{
    if (button != 100 && (((((int)(unsigned char)(mouse_buttons & 1)) == 0) || ((((int)(unsigned char)(mouse_buttons_prev & 1)) != 0))) ? 1 : 0) != 0) {
        return;
    }
    if (scratch_190ce4[0] != 0) {
        func_0009BE38();
        return;
    }
    sound_play(203, player_object, 110);
    mc_memset((void *)screen_buffer, 0, 64000, D_0017743D, 367, 4);
    xn_gfx_present_inclusive(1);
    palette_restore();
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
    D_00187CA8 = 1;
    maploads_load_region((int)(unsigned char)current_region);
    region_location_names_release();
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free((void *)window_image, D_0017743D, 380);
        window_image = -1751672937;
    }
    if (D_001AA6A0 != 0 && D_001AA6A0 != (-1751672937)) {
        mc_free((void *)D_001AA6A0, D_0017743D, 381);
        D_001AA6A0 = -1751672937;
    }
    if (D_001AA66C != 0 && D_001AA66C != (-1751672937)) {
        mc_free((void *)D_001AA66C, D_0017743D, 382);
        D_001AA66C = -1751672937;
    }
    if ((iptr)D_00195B5C != 0 && (iptr)D_00195B5C != (-1751672937)) {
        mc_free(D_00195B5C, D_0017743D, 383);
        D_00195B5C = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_001AA670 != 0 && (iptr)D_001AA670 != (-1751672937)) {
        mc_free(D_001AA670, D_0017743D, 384);
        D_001AA670 = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_001AA64C != 0 && (iptr)D_001AA64C != (-1751672937)) {
        mc_free(D_001AA64C, D_0017743D, 385);
        D_001AA64C = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_001AA650 != 0 && (iptr)D_001AA650 != (-1751672937)) {
        mc_free(D_001AA650, D_0017743D, 386);
        D_001AA650 = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_001AA654 != 0 && (iptr)D_001AA654 != (-1751672937)) {
        mc_free(D_001AA654, D_0017743D, 387);
        D_001AA654 = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_001AA658 != 0 && (iptr)D_001AA658 != (-1751672937)) {
        mc_free(D_001AA658, D_0017743D, 388);
        D_001AA658 = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)D_001AA65C[0] != 0 && (iptr)D_001AA65C[0] != (-1751672937)) {
        mc_free((void *)D_001AA65C[0], D_0017743D, 389);
        *(iptr *)&D_001AA65C[0] = -1751672937;
    }
    if (D_001AA660 != 0 && D_001AA660 != (-1751672937)) {
        mc_free((void *)D_001AA660, D_0017743D, 390);
        D_001AA660 = -1751672937;
    }
    if ((iptr)D_001AA668 != 0 && (iptr)D_001AA668 != (-1751672937)) {
        mc_free(D_001AA668, D_0017743D, 392);
        D_001AA668 = (struct image *)(iptr)-1751672937;
    }
    game_mode = 0;
    D_00196272 = 0;
}

void travel_draw_locations(void)
{
    struct map_location *location;
    int origin_x;
    int origin_z;
    int i;
    int x;
    int y;
    slot16 saved_clip_bottom;

    location = region_locations;
    *(int *)&saved_clip_bottom = (int)(short)xn_gfx_clip_bottom;
    xn_gfx_clip_bottom = 160;
    if (((int)(unsigned char)D_001AA6A4) < 2) {
        origin_x = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
        origin_z = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
    } else {
        origin_x = ((int)(short)*(short *)((D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
        origin_z = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
    }
    for (i = 0; i < region_location_count; i++, location++) {
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((location->x_type_flags & 0x40000000) == 0 || (location->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        if ((((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)(location->x_type_flags << 2)) >> 27]))) == 0) continue;
        x = ((unsigned)((location->x_type_flags & 33554431) - origin_x)) >> 15;
        y = ((unsigned)(-((location->z_size & 16777215) - origin_z))) >> 15;
        if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
            x <<= 2;
            y <<= 2;
            y += -6;
            x += -2;
        }
        if (x == 139 && y == 165) (town_building_counter)++;
        func_000A134C((int)(short)(x + 12), (int)(short)(y + 1), (int)(unsigned char)location_type_dot_colour[((unsigned)(location->x_type_flags << 2)) >> 27]);
    }
    xn_gfx_clip_bottom = *(int *)&saved_clip_bottom;
}

void travel_open_region(int region)
{
    iptr saved_screen;

    scratch_190ce4[0] = *(signed char *)&region + 1;
    D_001AA6A4 = (D_001AA6A5 = 0);
    if (region == 0 || region == 1 || region == 16) {
        maploads_load_region(region);
        region_load_location_names(region);
        travel_load_region_part();
    } else {
        if ((iptr)D_001AA668 != 0 && (iptr)D_001AA668 != (-1751672937)) {
            mc_free(D_001AA668, D_0017743D, 460);
            D_001AA668 = (struct image *)(iptr)-1751672937;
        }
        mc_set_location(461, D_0017743D);
        mc_sprintf((char *)text_buffer, D_00177446, region);
        D_001AA668 = (struct image *)disk_read_file(text_buffer, 0);
        maploads_load_region(region);
        region_load_location_names(region);
        saved_screen = screen_buffer;
        screen_buffer = (iptr)D_001AA668;
        travel_draw_locations();
        screen_buffer = saved_screen;
    }
    switch ((unsigned)region) {
    case 0:
        D_001AA6A4 = 2;
        *(iptr *)&D_001AA684 = (iptr)D_00188994;
        break;
    case 1:
        D_001AA6A4 = 4;
        *(iptr *)&D_001AA684 = (iptr)D_0018899C;
        break;
    case 16:
        D_001AA6A4 = 4;
        *(iptr *)&D_001AA684 = (iptr)D_001889AC;
        break;
    default:
        D_001AA6A4 = 0;
    }
    travel_mark_player(region);
}

void travel_mark_player(int region)
{
    int x;
    int y;
    int origin_x;
    int origin_z;
    int *bios_ticks;

    if (((int)(unsigned char)current_region) == region) {
        if (((int)(unsigned char)D_001AA6A4) < 2) {
            origin_x = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
            origin_z = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
        } else {
            origin_x = ((int)(short)*(short *)((D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
            origin_z = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
        }
        x = (player_object->x - origin_x) / 32768;
        y = (-(player_object->z - origin_z)) / 32768;
        if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
            x <<= 2;
            y <<= 2;
            y += -6;
            x += -2;
        }
        *(int *)D_001AA688 = x;
        *(int *)D_001AA68C = y + 13;
        bios_ticks = (int *)DOS_LOW(0x46C);
        scratch_190cac = *bios_ticks;
        return;
    }
    *(int *)D_001AA688 = -1;
}

void travel_draw_hover_name(void)
{
    int pixel;
    int y;
    int location_index;

    if (((int)(short)mouse_y) > 171) return;
    if (scratch_190ce4[0] != 0) {
        location_index = travel_location_at_cursor();
        if (location_index != (-1)) {
            if (((struct bf8_3_1 *)&cheat_flags)->f != 0) {
                if ((((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)((region_locations)[location_index].x_type_flags << 2)) >> 27]))) != 0) {
                    mc_set_location(541, D_0017743D);
                    mc_sprintf((char *)text_buffer, D_00177455, D_001837E4[((int)(signed char)scratch_190ce4[0])], (iptr)(D_00196A7C + (location_index << 5)) + 4);
                } else {
                    mc_strncpy((char *)text_buffer, (char *)D_001AA664, 160, D_0017743D, 543);
                }
            } else if ((((region_locations)[location_index].x_type_flags & 0x40000000) != 0 || ((region_locations)[location_index].x_type_flags & 0x80000000) == 0) && (((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)((region_locations)[location_index].x_type_flags << 2)) >> 27]))) != 0) {
                mc_set_location(548, D_0017743D);
                mc_sprintf((char *)text_buffer, D_00177455, D_001837E4[((int)(signed char)scratch_190ce4[0])], (iptr)(D_00196A7C + (location_index << 5)) + 4);
            } else {
                mc_strncpy((char *)text_buffer, (char *)D_001AA664, 160, D_0017743D, 550);
            }
        } else {
            mc_strncpy((char *)text_buffer, (char *)D_001AA664, 160, D_0017743D, 554);
        }
        y = 2;
    } else {
        pixel = (int)(unsigned char)*(signed char *)((*(char **)&D_001AA66C + ((((int)(short)mouse_y) * 320) + ((int)(short)mouse_x))));
        if (pixel < 128 || pixel > 191) return;
        mc_set_location(562, D_0017743D);
        mc_sprintf((char *)text_buffer, D_0017745D, D_001835E8[pixel]);
        y = 2;
    }
    if (text_buffer[0] == 0) return;
    text_draw_centred_coloured((iptr)text_buffer, 160, (int)(short)*(short *)&y, 145, 156);
}

void travel_button_map(void)
{
    int pixel;

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
    sound_play(203, player_object, 110);
    pixel = (int)(unsigned char)*(signed char *)((*(char **)&D_001AA66C + ((((int)(short)mouse_y) * 320) + ((int)(short)mouse_x))));
    if (pixel < 128 || pixel == 255) return;
    D_001889BC = *(signed char *)&pixel - 128;
    D_001AA664 = (iptr)region_names[((int)(unsigned char)D_001889BC)];
    travel_open_region((int)(unsigned char)D_001889BC);
}

void func_0009BE38(void)
{
    if ((iptr)D_001AA668 != 0) {
        if ((iptr)D_001AA668 != 0 && (iptr)D_001AA668 != (-1751672937)) {
            mc_free(D_001AA668, D_0017743D, 605);
            D_001AA668 = (struct image *)(iptr)-1751672937;
        }
        *(int *)D_001AA688 = -1;
    }
    scratch_190ce4[0] = 0;
    D_00190CE8 = 0;
}

void travel_toggle_option(int button)
{
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    sound_play(203, player_object, 110);
    if (button < 2) {
        *(signed char *)travel_options ^= 3;
    } else if (button < 4) {
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
    sound_play(203, player_object, 110);
    scratch_190ce5 = 0;
}

void func_0009C27F(void)
{
    travel_popup_exit();
}

void travel_button_find(void)
{
    iptr prompt;

    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] == 0) return;
    D_00190CE8 = 0;
    sound_play(203, player_object, 110);
    D_0012B508 = 145;
    xn_kbd_flush();
    prompt = (iptr)scratch_buffer + 55000;
    mc_set_location(807, D_0017743D);
    mc_sprintf((char *)prompt, D_00177460, D_001846F4);
    *(signed char *)((char *)(strlen((char *)prompt) + prompt) + 1) = 0;
    text_rsc_buffer[0] = 0;
    inpstr_begin_text(text_rsc_buffer, 32);
    msgbox_show_string(prompt, 2);
    if (strnicmp(D_0017748E, (char *)text_rsc_buffer, 4) == 0) {
        mc_memcpy(text_rsc_buffer, D_00190FE8, (int)(iptr)&*(signed char *)((char *)(iptr)strlen((char *)text_rsc_buffer) + 1), D_0017743D, 812, 2048);
    }
    scratch_190ce7 = 1;
}

int travel_location_at_cursor(void)
{
    struct map_location *location;
    struct map_location *nearest;
    int origin_x;
    int origin_z;
    int x;
    int y;
    int distance;
    int nearest_distance;
    int i;
    int cursor_x;
    int cursor_y;

    location = region_locations;
    nearest_distance = 32767;
    if (D_001AA694 != 0) return -1;
    if (((int)(short)mouse_y) > 171) return -1;
    if (D_00190CE8 != 0) {
        cursor_x = (((int)(short)mouse_x) >> 2) + ((int)(short)*(short *)scratch_190d64);
        cursor_y = ((((int)(short)mouse_y) - 12) >> 2) + ((int)(short)*(short *)scratch_190d66);
    } else {
        cursor_x = (int)(short)mouse_x;
        cursor_y = ((int)(short)mouse_y) - 12;
    }
    if (((int)(unsigned char)D_001AA6A4) < 2) {
        origin_x = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
        origin_z = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
    } else {
        origin_x = ((int)(short)*(short *)((D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
        origin_z = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
    }
    if (((int)(signed char)scratch_190ce4[0]) == 62 || ((int)(signed char)scratch_190ce4[0]) == 20) {
        cursor_x >>= 2;
        cursor_y >>= 2;
        cursor_y += 2;
        cursor_x += 2;
    }
    for (i = 0; i < region_location_count; i++, location++) {
        if ((((int)(unsigned char)travel_filter) & (1 << ((int)(unsigned char)location_type_category[((unsigned)(location->x_type_flags << 2)) >> 27]))) == 0) continue;
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((location->x_type_flags & 0x40000000) == 0 || (location->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        x = ((unsigned)((location->x_type_flags & 33554431) - origin_x)) >> 15;
        y = ((unsigned)(-((location->z_size & 16777215) - origin_z))) >> 15;
        distance = xn_math_approx_dist2d(x, y, cursor_x, cursor_y);
        if (distance < nearest_distance) {
            nearest = location;
            nearest_distance = distance;
        }
        if (nearest_distance < 2) break;
    }
    if (i != region_location_count) {
        D_001889C1 = (travel_selected_location = i);
        return i;
    }
    D_001889C1 = (travel_selected_location = -1);
    return -1;
}

int travel_find_location(char *name)
{
    int i;
    int length;
    struct map_location *location;
    char *location_name;

    length = strlen(name);
    location = region_locations;
    if (length == 0) return -1;
    for (i = 0; i < region_location_count; i++, location++) {
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((location->x_type_flags & 0x40000000) == 0 || (location->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        location_name = D_00196A7C + (i << 5) + 4;
        if (strnicmp(D_0017748E, location_name, 4) == 0) location_name += 4;
        if (stricmp(name, location_name) == 0) return i;
    }
    location = region_locations;
    for (i = 0; i < region_location_count; i++, location++) {
        if (((struct bf8_3_1 *)&cheat_flags)->f == 0 && ((location->x_type_flags & 0x40000000) == 0 || (location->x_type_flags & 0x80000000) != 0)) {
            continue;
        }
        location_name = D_00196A7C + (i << 5) + 4;
        if (strnicmp(D_0017748E, location_name, 4) == 0) location_name += 4;
        if (strnicmp(name, location_name, length) == 0) return i;
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

int func_0009CD32(int type)
{
    struct map_location *location;
    struct map_location *nearest;
    int origin_x;
    int origin_z;
    int x;
    int y;
    int distance;
    int nearest_distance;
    int i;
    int player_x;
    int player_z;
    int nearest_index;

    location = region_locations;
    nearest_distance = 32767;
    player_x = player_object->x / 32768;
    player_z = player_object->z / 32768;
    if (((int)(unsigned char)D_001AA6A4) < 2) {
        origin_x = ((int)(short)*(short *)(region_map_origins + (((int)(signed char)scratch_190ce4[0]) << 2))) << 15;
        origin_z = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)scratch_190ce4[0]) << 2)))) << 15;
    } else {
        origin_x = ((int)(short)*(short *)((D_001AA684 + (((int)(unsigned char)D_001AA6A5) << 2)))) << 15;
        origin_z = (499 - ((int)(short)(((short *)D_001AA684))[((int)(unsigned char)D_001AA6A5) * 2 + 1])) << 15;
    }
    for (i = 0; i < region_location_count; i++) {
        if ((((unsigned)(location->x_type_flags << 2)) >> 27) != type) continue;
        x = ((unsigned)((location->x_type_flags & 33554431) - origin_x)) >> 15;
        y = ((unsigned)(-((location->z_size & 16777215) - origin_z))) >> 15;
        distance = xn_math_approx_dist2d(x, y, player_x, player_z);
        if (distance < nearest_distance) {
            nearest_index = i;
            nearest = location;
            nearest_distance = distance;
        }
        location++;
    }
    if (nearest_distance == 32767) return D_001889C1;
    return nearest_index;
}

int travel_pixel_time(int x, int y)
{
    int climate;
    int base_time;

    climate = climate_at((x << 15) + 16384, (int)(iptr)&*(signed char *)((char *)(iptr)(y << 15) + 16384));
    base_time = (travel_transport_factor * 102) / 256;
    if (climate_is_ocean != 0) {
        travel_ocean_pixels++;
        if (((int)(unsigned short)(*(short *)travel_options & 8)) != 0 || player_character->ship_owned != 0) {
            return 51;
        }
        return 255;
    }
    return (((256 - terrain_travel_modifiers[climate]) + 256) * base_time) / 256;
}

void travel_check_transport_item(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (item->group == 23 && item->index == 1) {
        scratch_190d16 |= 2;
        travel_transport_factor = 128;
    }
    if (item->group != 23 || item->index != 0) return;
    scratch_190d16 |= 1;
    travel_transport_factor = 192;
}

void travel_find_transport(void)
{
    travel_transport_factor = 256;
    object_foreach((iptr)player_entity->children, (iptr)travel_check_transport_item);
}

void travel_load_region_part(void)
{
    iptr saved_screen;

    if ((iptr)D_001AA668 != 0 && (iptr)D_001AA668 != (-1751672937)) {
        mc_free(D_001AA668, D_0017743D, 1119);
        D_001AA668 = (struct image *)(iptr)-1751672937;
    }
    mc_set_location(1121, D_0017743D);
    mc_sprintf((char *)text_buffer, D_00177493, ((int)(unsigned char)D_001AA6A5) + 97, ((int)(signed char)scratch_190ce4[0]) - 1);
    D_001AA668 = (struct image *)disk_read_file(text_buffer, 0);
    saved_screen = screen_buffer;
    screen_buffer = (iptr)D_001AA668;
    travel_draw_locations();
    screen_buffer = saved_screen;
}

void travel_button_arrows(int button)
{
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] == 0) return;
    if (((int)(unsigned char)D_001AA6A4) < 2) return;
    if (button == 3) {
        if (((int)(unsigned char)D_001AA6A4) > 2) D_001AA6A5 ^= 2;
    } else {
        D_001AA6A5 ^= 1;
    }
    sound_play(203, player_object, 110);
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

void func_0009D5AC(char *dest, char *source, int unused)
{
    int row;

    source = (source + (((int)(short)*(short *)scratch_190d66) * 320)) + ((int)(short)*(short *)scratch_190d64);
    for (row = 0; row < 40; row++) {
        xn_draw_zoom4x((row * 320) + source, (row * 1280) + dest, 80);
    }
}

int travel_trip_cost(void)
{
    int hours;
    int cost;

    hours = (D_001AA680 + 59) / 60;
    cost = 0;
    if (((int)(unsigned short)(*(short *)travel_options & 16)) != 0 && guild_find_membership_by_bits(64) == 0) {
        cost = (((hours - travel_ocean_pixels) / 24) * 5) + 5;
    }
    if (travel_ocean_pixels != 0 && player_character->ship_owned == 0 && ((int)(unsigned short)(*(short *)travel_options & 8)) != 0 && guild_find_membership_by_bits(64) == 0) {
        cost += ((travel_ocean_pixels / 24) + 1) * 25;
    }
    return cost;
}

void travel_button_im_at(void)
{
    int *bios_ticks;

    if (((int)(unsigned char)(mouse_buttons & 1)) == 0 || ((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        return;
    }
    if (scratch_190ce4[0] != 0) {
        travel_mark_player((int)(unsigned char)current_region);
        return;
    }
    *(int *)D_001AA688 = 0;
    bios_ticks = (int *)DOS_LOW(0x46C);
    scratch_190cac = *bios_ticks;
}

void travel_draw_buttons(void)
{
    int i;
    int frame;
    int row;
    int width;

    if (scratch_190ce4[0] != 0) {
        xn_draw_image(3, 175, 45, 22, (char *)D_00195B5C);
    } else {
        xn_draw_image(3, 186, 45, 11, (char *)((iptr)D_00195B5C + 495));
    }
    for (i = 0; i < 4; i++) {
        frame = 1;
        if ((((int)(unsigned char)travel_filter) & (1 << i)) != 0) frame = 0;
        if ((i & 1) != 0) {
            width = 80;
        } else {
            width = 99;
        }
        for (row = 0; row < 11; row++) {
            mc_memcpy((void *)(((int)(short)D_00188790[i]) + (iptr)(*(char **)&screen_buffer + ((((int)(short)D_00188798[i]) + row) * 320))), (void *)((iptr)(D_001AA65C[frame] + (((((int)(short)D_00188798[i]) + row) - 175) * 179)) + (((int)(short)D_00188790[i]) - 50)), width, D_0017743D, 1222, 4);
        }
    }
}

void travel_button_filter(void)
{
    int unused;

    unused = 0;
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

int func_0009D960(int unused1, int unused2)
{
    return 0;
}

void travel_show_days_left(int minutes)
{
    mc_memcpy((void *)screen_buffer, (void *)D_001AA690, 64000, D_0017743D, 1259, 4);
    mc_set_location(1260, D_0017743D);
    mc_sprintf((char *)text_buffer, D_001774A3, ((unsigned)minutes) / 1440);
    text_draw_coloured((iptr)text_buffer, 240, 2, 145, 156);
    xn_gfx_present_inclusive(1);
}
