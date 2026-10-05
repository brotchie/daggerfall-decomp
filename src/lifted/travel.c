/* travel.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_enter[];
extern char D_00142928[];
extern char D_0014292C[];
extern char D_0014294C[];
extern char screen_buffer[];
extern char D_0017743D[];
extern char D_00177446[];
extern char D_00177455[];
extern char D_0017745D[];
extern char D_00177460[];
extern char D_0017748E[];
extern char D_00177493[];
extern char D_001774A3[];
extern char travel_filter[];
extern char travel_options[];
extern char D_001835E8[];
extern char D_001837E4[];
extern char region_names[];
extern char D_001846F4[];
extern char D_00187CA8[];
extern char D_00188774[];
extern char location_type_category[];
extern char D_00188790[];
extern char D_00188798[];
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
extern char D_00188898[];
extern char D_0018889A[];
extern char D_00188994[];
extern char D_0018899C[];
extern char D_001889AC[];
extern char D_001889BC[];
extern char travel_selected_location[];
extern char D_001889C1[];
extern char terrain_travel_modifiers[];
extern char text_buffer[];
extern char D_00190CAC[];
extern char itemmaker_slot_kinds[];
extern char D_00190CE5[];
extern char D_00190CE7[];
extern char D_00190CE8[];
extern char D_00190D16[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char text_rsc_buffer[];
extern char D_00190FE8[];
extern struct record *player_entity;
extern struct record *player_object;
extern char cheat_flags[];
extern char D_00195B5C[];
extern struct character *player_character;
extern char window_image[];
extern char D_00195C44[];
extern char D_00195D48[];
extern char D_00195DB0[];
extern char current_region[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char D_00196285[];
extern char D_0019629B[];
extern char D_00196808[];
extern char D_00196A28[];
extern char D_00196A7C[];
extern char D_00196A9C[];
extern char D_001AA64C[];
extern char D_001AA650[];
extern char D_001AA654[];
extern char D_001AA658[];
extern char D_001AA65C[];
extern char D_001AA660[];
extern char D_001AA664[];
extern char D_001AA668[];
extern char D_001AA66C[];
extern char D_001AA670[];
extern char D_001AA674[];
extern char D_001AA678[];
extern char D_001AA67C[];
extern char D_001AA680[];
extern char D_001AA684[];
extern char D_001AA688[];
extern char D_001AA68C[];
extern char D_001AA690[];
extern char D_001AA694[];
extern char travel_transport_factor[];
extern char D_001AA6A0[];
extern char D_001AA6A4[];
extern char D_001AA6A5[];
extern char D_001AA6A6[];

extern int func_00020057(int, int);
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
extern int func_000A0DF4();
extern int stricmp();
extern int strnicmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A134C();
extern int func_000C7FD9();
extern int func_000CDD49();
extern int func_000CDD6C();
extern int func_000CDD81();
extern int func_00142790();
extern int func_00144F68();
extern int func_00144FB4();
extern int func_001532B4();
extern void maploads_load_region(int);
extern void region_load_location_names(int);
extern void func_0001DFA2(void);
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void palette_restore(void);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
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
#pragma aux func_000A0ED9 parm routine [];

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
    mc_memcpy(*(int *)screen_buffer, *(int *)window_image, 64000, (int)D_0017743D, 198, 4);
    travel_draw_buttons();
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9AE66;
    if (*(signed char *)D_00190CE8 == 0) goto L9ACBA;
    func_0009D5AC((int)(*(char **)screen_buffer + 3840), (int)&*(signed char *)(*(char **)D_001AA668 + 12), 51200);
    goto L9AD1E;
L9ACBA:;
    mc_memcpy((int)(*(char **)screen_buffer + 3840), (int)&*(signed char *)(*(char **)D_001AA668 + 12), 51200, (int)D_0017743D, 208, 4);
    func_00144FB4((int)(unsigned short)*(short *)(*(char **)D_001AA670), (int)(unsigned short)*(short *)(*(char **)D_001AA670 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA670 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA670 + 6), (int)(*(char **)D_001AA670 + 12));
L9AD1E:;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) <= 1) goto L9AE66;
    if (((int)(unsigned char)(*(signed char *)D_001AA6A5 & 1)) != 0) goto L9AD81;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_001AA654), (int)(unsigned short)*(short *)(*(char **)D_001AA654 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA654 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA654 + 6), (int)(*(char **)D_001AA654 + 12));
    goto L9ADC2;
L9AD81:;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_001AA658), (int)(unsigned short)*(short *)(*(char **)D_001AA658 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA658 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA658 + 6), (int)(*(char **)D_001AA658 + 12));
L9ADC2:;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) <= 2) goto L9AE66;
    if (((int)(unsigned char)(*(signed char *)D_001AA6A5 & 2)) != 0) goto L9AE25;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_001AA64C), (int)(unsigned short)*(short *)(*(char **)D_001AA64C + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA64C + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA64C + 6), (int)(*(char **)D_001AA64C + 12));
    goto L9AE66;
L9AE25:;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_001AA650), (int)(unsigned short)*(short *)(*(char **)D_001AA650 + 2), (int)(unsigned short)*(short *)(*(char **)D_001AA650 + 4), (int)(unsigned short)*(short *)(*(char **)D_001AA650 + 6), (int)(*(char **)D_001AA650 + 12));
L9AE66:;
    if (*(signed char *)D_00190CE5 == 0) goto L9AE85;
    travel_draw_trip_popup();
    l_30 = 1132;
    *(int *)D_00190CAC = *(int *)((char *)l_30);
L9AE85:;
    if (*(signed char *)D_00190CE5 != 0) goto L9AF68;
    travel_draw_hover_name();
    if (*(int *)D_001AA688 == (-1)) goto L9AEA9;
    if (*(signed char *)D_00190CE8 == 0) goto L9AEAE;
L9AEA9:;
    goto L9AF68;
L9AEAE:;
    l_2C = 1132;
    if (((unsigned)(*(int *)((char *)l_2C) - *(int *)D_00190CAC)) <= 50) goto L9AED4;
    *(int *)D_001AA688 = -1;
    goto L9AF68;
L9AED4:;
    l_28 = 1132;
    if (((struct bf8_3_1 *)((char *)l_28))->f == 0) goto L9AF68;
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9AF45;
    *(signed char *)D_0012B508 = 244;
    *(short *)D_00142928 = *(short *)D_001AA688;
    *(short *)D_0014292C = 12;
    func_001532B4((int)(short)*(short *)D_001AA688, 172);
    *(short *)D_00142928 = 0;
    *(short *)D_0014292C = *(short *)D_001AA68C;
    func_001532B4(320, (int)(short)*(short *)D_001AA68C);
    goto L9AF68;
L9AF45:;
    func_000CDD6C(*(int *)D_001AA66C, *(int *)screen_buffer, ((int)(unsigned char)*(signed char *)current_region) + 128, 64000);
L9AF68:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 19) return;
    if (*(signed char *)D_00190CE7 == 0) goto L9B11A;
L9AF85:;
    if (*(signed char *)key_down_enter != 0) goto L9AF85;
    key_pressed_once(28);
    *(signed char *)D_00190CE7 = 0;
    l_50 = travel_find_location((int)text_rsc_buffer);
    if (l_50 == (-1)) goto L9B10B;
    l_34 = *(struct map_location **)D_00196A9C + l_50;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) >= 2) goto L9B00A;
    l_44 = ((int)(short)*(short *)(D_00188898 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2))) << 15;
    l_40 = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)))) << 15;
    goto L9B04C;
L9B00A:;
    l_44 = ((int)(short)*(short *)((char *)(int)(*(char **)D_001AA684 + (((int)(unsigned char)*(signed char *)D_001AA6A5) << 2)))) << 15;
    l_40 = (499 - ((int)(short)(*(short **)D_001AA684)[((int)(unsigned char)*(signed char *)D_001AA6A5) * 2 + 1])) << 15;
L9B04C:;
    l_4C = ((unsigned)((l_34->x_type_flags & 33554431) - l_44)) >> 15;
    l_48 = ((unsigned)(-((l_34->y_size & 16777215) - l_40))) >> 15;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) == 62) goto L9B08E;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) != 20) goto L9B09E;
L9B08E:;
    l_4C <<= 2;
    l_48 <<= 2;
    l_48 += -6;
    l_4C += -2;
L9B09E:;
    *(int *)D_001AA688 = l_4C;
    *(int *)D_001AA68C = l_48 + 13;
    l_24 = 1132;
    *(int *)D_00190CAC = *(int *)((char *)l_24);
    *(int *)D_00195DB0 = (int)(*(char **)D_00196A7C + ((*(int *)travel_selected_location = l_50) << 5)) + 4;
    l_20 = 1132;
    *(int *)D_001AA694 = (int)(*(char **)((char *)l_20) + 24);
L9B0F6:;
    if (*(signed char *)key_down_enter != 0) goto L9B0F6;
    key_pressed_once(28);
    goto L9B11A;
L9B10B:;
    msgbox_show_rsc(13, 1);
L9B11A:;
    l_1C = 1132;
    if (*(int *)D_001AA694 == 0) goto L9B137;
    if (((unsigned)*(int *)((char *)l_1C)) > *(int *)D_001AA694) goto L9B139;
L9B137:;
    goto L9B171;
L9B139:;
    *(int *)D_001AA694 = 0;
    msgbox_yes_no_rsc(31);
    if (((int)(unsigned char)*(signed char *)D_00196271) != 1) goto L9B160;
    travel_open_trip();
    goto L9B171;
L9B160:;
    l_18 = 1132;
    *(int *)D_00190CAC = *(int *)((char *)l_18);
L9B171:;
    if (*(signed char *)D_00190CE5 == 0) goto L9B22C;
    if (((int)(unsigned char)*(signed char *)D_001AA6A6) != 100) goto L9B19A;
    l_38 = 10;
    l_3C = 7;
    goto L9B1A8;
L9B19A:;
    l_38 = 8;
    l_3C = 0;
L9B1A8:;
    l_50 = 0;
L9B1AF:;
    if (l_50 < 10) goto L9B1C2;
    goto L9B227;
L9B1BA:;
    l_50++;
    goto L9B1AF;
L9B1C2:;
    if (*(short *)mouse_x <= *(short *)(travel_popup_buttons + (l_50 * 12))) goto L9B1EA;
    if (*(short *)mouse_x < *(short *)(D_001887F8 + (l_50 * 12))) goto L9B1EC;
L9B1EA:;
    goto L9B200;
L9B1EC:;
    if (*(short *)mouse_y > *(short *)(D_001887F6 + (l_50 * 12))) goto L9B202;
L9B200:;
    goto L9B216;
L9B202:;
    if (*(short *)mouse_y < *(short *)(D_001887FA + (l_50 * 12))) goto L9B218;
L9B216:;
    goto L9B225;
L9B218:;
    ((int (*)())(*(int *)(D_001887FC + (l_50 * 12))))(l_50);
L9B225:;
    goto L9B1BA;
L9B227:;
    return;
L9B22C:;
    if (key_pressed_once(33) == 0) goto L9B259;
    *(signed char *)mouse_buttons = 1;
    *(signed char *)mouse_buttons_prev = 0;
    travel_button_find();
    *(signed char *)mouse_buttons = 0;
    return;
L9B259:;
    l_50 = 0;
L9B260:;
    if (l_50 < 7) goto L9B273;
    return;
L9B26B:;
    l_50++;
    goto L9B260;
L9B273:;
    if (*(short *)mouse_x <= *(short *)(travel_bar_buttons + (l_50 * 12))) goto L9B29B;
    if (*(short *)mouse_x < *(short *)(D_001887A4 + (l_50 * 12))) goto L9B29D;
L9B29B:;
    goto L9B2B1;
L9B29D:;
    if (*(short *)mouse_y > *(short *)(D_001887A2 + (l_50 * 12))) goto L9B2B3;
L9B2B1:;
    goto L9B2C7;
L9B2B3:;
    if (*(short *)mouse_y < *(short *)(D_001887A6 + (l_50 * 12))) goto L9B2C9;
L9B2C7:;
    goto L9B2D6;
L9B2C9:;
    ((int (*)())(*(int *)(D_001887A8 + (l_50 * 12))))(l_50);
L9B2D6:;
    goto L9B26B;
}

void travel_button_exit(int a1)
{
    if (a1 == 100) goto L9B32F;
    if ((((((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) || ((((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) != 0))) ? 1 : 0) != 0) goto L9B331;
L9B32F:;
    goto L9B336;
L9B331:;
    return;
L9B336:;
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9B349;
    func_0009BE38();
    return;
L9B349:;
    sound_play(203, (int)player_object, 110);
    mc_memset(*(int *)screen_buffer, 0, 64000, (int)D_0017743D, 367, 4);
    func_000CDD81(1);
    palette_restore();
    *(int *)D_00195D48 = 10000;
    *(signed char *)D_0019629B = 0;
    *(signed char *)D_00187CA8 = 1;
    maploads_load_region((int)(unsigned char)*(signed char *)current_region);
    func_0001DFA2();
    if (*(int *)window_image == 0) goto L9B3C8;
    if (*(int *)window_image != (-1751672937)) goto L9B3CA;
L9B3C8:;
    goto L9B3E8;
L9B3CA:;
    mc_free(*(int *)window_image, (int)D_0017743D, 380);
    *(int *)window_image = -1751672937;
L9B3E8:;
    if (*(int *)D_001AA6A0 == 0) goto L9B3FD;
    if (*(int *)D_001AA6A0 != (-1751672937)) goto L9B3FF;
L9B3FD:;
    goto L9B41D;
L9B3FF:;
    mc_free(*(int *)D_001AA6A0, (int)D_0017743D, 381);
    *(int *)D_001AA6A0 = -1751672937;
L9B41D:;
    if (*(int *)D_001AA66C == 0) goto L9B432;
    if (*(int *)D_001AA66C != (-1751672937)) goto L9B434;
L9B432:;
    goto L9B452;
L9B434:;
    mc_free(*(int *)D_001AA66C, (int)D_0017743D, 382);
    *(int *)D_001AA66C = -1751672937;
L9B452:;
    if (*(int *)D_00195B5C == 0) goto L9B467;
    if (*(int *)D_00195B5C != (-1751672937)) goto L9B469;
L9B467:;
    goto L9B487;
L9B469:;
    mc_free(*(int *)D_00195B5C, (int)D_0017743D, 383);
    *(int *)D_00195B5C = -1751672937;
L9B487:;
    if (*(int *)D_001AA670 == 0) goto L9B49C;
    if (*(int *)D_001AA670 != (-1751672937)) goto L9B49E;
L9B49C:;
    goto L9B4BC;
L9B49E:;
    mc_free(*(int *)D_001AA670, (int)D_0017743D, 384);
    *(int *)D_001AA670 = -1751672937;
L9B4BC:;
    if (*(int *)D_001AA64C == 0) goto L9B4D1;
    if (*(int *)D_001AA64C != (-1751672937)) goto L9B4D3;
L9B4D1:;
    goto L9B4F1;
L9B4D3:;
    mc_free(*(int *)D_001AA64C, (int)D_0017743D, 385);
    *(int *)D_001AA64C = -1751672937;
L9B4F1:;
    if (*(int *)D_001AA650 == 0) goto L9B506;
    if (*(int *)D_001AA650 != (-1751672937)) goto L9B508;
L9B506:;
    goto L9B526;
L9B508:;
    mc_free(*(int *)D_001AA650, (int)D_0017743D, 386);
    *(int *)D_001AA650 = -1751672937;
L9B526:;
    if (*(int *)D_001AA654 == 0) goto L9B53B;
    if (*(int *)D_001AA654 != (-1751672937)) goto L9B53D;
L9B53B:;
    goto L9B55B;
L9B53D:;
    mc_free(*(int *)D_001AA654, (int)D_0017743D, 387);
    *(int *)D_001AA654 = -1751672937;
L9B55B:;
    if (*(int *)D_001AA658 == 0) goto L9B570;
    if (*(int *)D_001AA658 != (-1751672937)) goto L9B572;
L9B570:;
    goto L9B590;
L9B572:;
    mc_free(*(int *)D_001AA658, (int)D_0017743D, 388);
    *(int *)D_001AA658 = -1751672937;
L9B590:;
    if (*(int *)D_001AA65C == 0) goto L9B5A5;
    if (*(int *)D_001AA65C != (-1751672937)) goto L9B5A7;
L9B5A5:;
    goto L9B5C5;
L9B5A7:;
    mc_free(*(int *)D_001AA65C, (int)D_0017743D, 389);
    *(int *)D_001AA65C = -1751672937;
L9B5C5:;
    if (*(int *)D_001AA660 == 0) goto L9B5DA;
    if (*(int *)D_001AA660 != (-1751672937)) goto L9B5DC;
L9B5DA:;
    goto L9B5FA;
L9B5DC:;
    mc_free(*(int *)D_001AA660, (int)D_0017743D, 390);
    *(int *)D_001AA660 = -1751672937;
L9B5FA:;
    if (*(int *)D_001AA668 == 0) goto L9B60F;
    if (*(int *)D_001AA668 != (-1751672937)) goto L9B611;
L9B60F:;
    goto L9B62F;
L9B611:;
    mc_free(*(int *)D_001AA668, (int)D_0017743D, 392);
    *(int *)D_001AA668 = -1751672937;
L9B62F:;
    *(signed char *)game_mode = 0;
    *(signed char *)D_00196272 = 0;
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

    l_30 = *(struct map_location **)D_00196A9C;
    *(int *)&l_18 = (int)(short)*(short *)D_0014294C;
    *(short *)D_0014294C = 160;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) >= 2) goto L9B6B5;
    l_2C = ((int)(short)*(short *)(D_00188898 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2))) << 15;
    l_28 = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)))) << 15;
    goto L9B6F7;
L9B6B5:;
    l_2C = ((int)(short)*(short *)((char *)(int)(*(char **)D_001AA684 + (((int)(unsigned char)*(signed char *)D_001AA6A5) << 2)))) << 15;
    l_28 = (499 - ((int)(short)(*(short **)D_001AA684)[((int)(unsigned char)*(signed char *)D_001AA6A5) * 2 + 1])) << 15;
L9B6F7:;
    l_24 = 0;
L9B6FE:;
    if (l_24 < *(int *)D_00196A28) goto L9B71D;
    goto L9B807;
L9B70E:;
    l_24++;
    l_30++;
    goto L9B6FE;
L9B71D:;
    if (((struct bf8_3_1 *)&cheat_flags)->f != 0) goto L9B73A;
    if ((l_30->x_type_flags & 0x40000000) == 0) goto L9B738;
    if ((l_30->x_type_flags & 0x80000000) == 0) goto L9B73A;
L9B738:;
    goto L9B73C;
L9B73A:;
    goto L9B73E;
L9B73C:;
    goto L9B70E;
L9B73E:;
    if ((((int)(unsigned char)*(signed char *)travel_filter) & (1 << ((int)(unsigned char)*(signed char *)(location_type_category + (((unsigned)(l_30->x_type_flags << 2)) >> 27))))) == 0) goto L9B70E;
    l_20 = ((unsigned)((l_30->x_type_flags & 33554431) - l_2C)) >> 15;
    l_1C = ((unsigned)(-((l_30->y_size & 16777215) - l_28))) >> 15;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) == 62) goto L9B7AB;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) != 20) goto L9B7BB;
L9B7AB:;
    l_20 <<= 2;
    l_1C <<= 2;
    l_1C += -6;
    l_20 += -2;
L9B7BB:;
    if (l_20 != 139) goto L9B7CD;
    if (l_1C == 165) goto L9B7CF;
L9B7CD:;
    goto L9B7D5;
L9B7CF:;
    (*(int *)D_00196808)++;
L9B7D5:;
    func_000A134C((int)(short)(l_20 + 12), (int)(short)(l_1C + 1), (int)(unsigned char)*(signed char *)(D_00188774 + (((unsigned)(l_30->x_type_flags << 2)) >> 27)));
    goto L9B70E;
L9B807:;
    *(short *)D_0014294C = *(int *)&l_18;
}

void travel_open_region(int a1)
{
    int l_18;

    *(signed char *)itemmaker_slot_kinds = *(signed char *)&a1 + 1;
    *(signed char *)D_001AA6A4 = (*(signed char *)D_001AA6A5 = 0);
    if (a1 == 0) goto L9B852;
    if (a1 != 1) goto L9B854;
L9B852:;
    goto L9B85A;
L9B854:;
    if (a1 != 16) goto L9B874;
L9B85A:;
    maploads_load_region(a1);
    region_load_location_names(a1);
    travel_load_region_part();
    goto L9B911;
L9B874:;
    if (*(int *)D_001AA668 == 0) goto L9B889;
    if (*(int *)D_001AA668 != (-1751672937)) goto L9B88B;
L9B889:;
    goto L9B8A9;
L9B88B:;
    mc_free(*(int *)D_001AA668, (int)D_0017743D, 460);
    *(int *)D_001AA668 = -1751672937;
L9B8A9:;
    func_000A0ED9(461, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_00177446, a1);
    *(int *)D_001AA668 = disk_read_file((int)text_buffer, 0);
    maploads_load_region(a1);
    region_load_location_names(a1);
    l_18 = *(int *)screen_buffer;
    *(int *)screen_buffer = *(int *)D_001AA668;
    travel_draw_locations();
    *(int *)screen_buffer = l_18;
L9B911:;
    switch ((unsigned)a1) {
case 0:
    *(signed char *)D_001AA6A4 = 2;
    *(int *)D_001AA684 = (int)D_00188994;
    goto L9B96B;
case 1:
    *(signed char *)D_001AA6A4 = 4;
    *(int *)D_001AA684 = (int)D_0018899C;
    goto L9B96B;
case 16:
    *(signed char *)D_001AA6A4 = 4;
    *(int *)D_001AA684 = (int)D_001889AC;
    goto L9B96B;
default:
    *(signed char *)D_001AA6A4 = 0;
L9B96B:;
    travel_mark_player(a1);
}
}

void travel_mark_player(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)current_region) != a1) goto L9BAAD;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) >= 2) goto L9B9E3;
    l_20 = ((int)(short)*(short *)(D_00188898 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2))) << 15;
    l_1C = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)))) << 15;
    goto L9BA25;
L9B9E3:;
    l_20 = ((int)(short)*(short *)((char *)(int)(*(char **)D_001AA684 + (((int)(unsigned char)*(signed char *)D_001AA6A5) << 2)))) << 15;
    l_1C = (499 - ((int)(short)(*(short **)D_001AA684)[((int)(unsigned char)*(signed char *)D_001AA6A5) * 2 + 1])) << 15;
L9BA25:;
    l_28 = (player_object->x - l_20) / 32768;
    l_24 = (-(player_object->z - l_1C)) / 32768;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) == 62) goto L9BA77;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) != 20) goto L9BA87;
L9BA77:;
    l_28 <<= 2;
    l_24 <<= 2;
    l_24 += -6;
    l_28 += -2;
L9BA87:;
    *(int *)D_001AA688 = l_28;
    *(int *)D_001AA68C = l_24 + 13;
    l_18 = 1132;
    *(int *)D_00190CAC = *(int *)((char *)l_18);
    return;
L9BAAD:;
    *(int *)D_001AA688 = -1;
}

void travel_draw_hover_name(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (((int)(short)*(short *)mouse_y) > 171) return;
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9BC90;
    l_18 = travel_location_at_cursor();
    if (l_18 == (-1)) goto L9BC68;
    if (((struct bf8_3_1 *)&cheat_flags)->f == 0) goto L9BBA9;
    if ((((int)(unsigned char)*(signed char *)travel_filter) & (1 << ((int)(unsigned char)*(signed char *)(location_type_category + (((unsigned)((*(struct map_location **)D_00196A9C)[l_18].x_type_flags << 2)) >> 27))))) == 0) goto L9BB85;
    func_000A0ED9(541, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_00177455, *(int *)(D_001837E4 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)), (int)(*(char **)D_00196A7C + (l_18 << 5)) + 4);
    goto L9BBA4;
L9BB85:;
    mc_strncpy((int)text_buffer, *(int *)D_001AA664, 160, (int)D_0017743D, 543);
L9BBA4:;
    goto L9BC66;
L9BBA9:;
    if (((*(struct map_location **)D_00196A9C)[l_18].x_type_flags & 0x40000000) != 0) goto L9BBCD;
    if (((*(struct map_location **)D_00196A9C)[l_18].x_type_flags & 0x80000000) != 0) goto L9BBFD;
L9BBCD:;
    if ((((int)(unsigned char)*(signed char *)travel_filter) & (1 << ((int)(unsigned char)*(signed char *)(location_type_category + (((unsigned)((*(struct map_location **)D_00196A9C)[l_18].x_type_flags << 2)) >> 27))))) != 0) goto L9BBFF;
L9BBFD:;
    goto L9BC47;
L9BBFF:;
    func_000A0ED9(548, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_00177455, *(int *)(D_001837E4 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)), (int)(*(char **)D_00196A7C + (l_18 << 5)) + 4);
    goto L9BC66;
L9BC47:;
    mc_strncpy((int)text_buffer, *(int *)D_001AA664, 160, (int)D_0017743D, 550);
L9BC66:;
    goto L9BC87;
L9BC68:;
    mc_strncpy((int)text_buffer, *(int *)D_001AA664, 160, (int)D_0017743D, 554);
L9BC87:;
    l_1C = 2;
    goto L9BD00;
L9BC90:;
    l_20 = (int)(unsigned char)*(signed char *)((char *)(int)(*(char **)D_001AA66C + ((((int)(short)*(short *)mouse_y) * 320) + ((int)(short)*(short *)mouse_x))));
    if (l_20 < 128) goto L9BCC7;
    if (l_20 <= 191) goto L9BCC9;
L9BCC7:;
    return;
L9BCC9:;
    func_000A0ED9(562, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_0017745D, *(int *)(D_001835E8 + (l_20 << 2)));
    l_1C = 2;
L9BD00:;
    if (*(signed char *)text_buffer == 0) return;
    text_draw_centered_colored((int)text_buffer, 160, (int)(short)*(short *)&l_1C, 145, 156);
}

void travel_button_map(void)
{
    int l_18;

    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L9BD5F;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 2)) == 0) goto L9BD61;
L9BD5F:;
    goto L9BD6A;
L9BD61:;
    if (*(signed char *)itemmaker_slot_kinds != 0) goto L9BD6C;
L9BD6A:;
    goto L9BD76;
L9BD6C:;
    travel_toggle_zoom();
    return;
L9BD76:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9BD96;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9BD9B;
L9BD96:;
    return;
L9BD9B:;
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9BDAE;
    travel_open_trip();
    return;
L9BDAE:;
    *(signed char *)D_00190CE8 = 0;
    sound_play(203, (int)player_object, 110);
    l_18 = (int)(unsigned char)*(signed char *)((char *)(int)(*(char **)D_001AA66C + ((((int)(short)*(short *)mouse_y) * 320) + ((int)(short)*(short *)mouse_x))));
    if (l_18 < 128) goto L9BE01;
    if (l_18 != 255) goto L9BE03;
L9BE01:;
    return;
L9BE03:;
    *(signed char *)D_001889BC = *(signed char *)&l_18 - 128;
    *(int *)D_001AA664 = *(int *)(region_names + (((int)(unsigned char)*(signed char *)D_001889BC) << 2));
    travel_open_region((int)(unsigned char)*(signed char *)D_001889BC);
}

void func_0009BE38(void)
{
    if (*(int *)D_001AA668 == 0) goto L9BE8E;
    if (*(int *)D_001AA668 == 0) goto L9BE64;
    if (*(int *)D_001AA668 != (-1751672937)) goto L9BE66;
L9BE64:;
    goto L9BE84;
L9BE66:;
    mc_free(*(int *)D_001AA668, (int)D_0017743D, 605);
    *(int *)D_001AA668 = -1751672937;
L9BE84:;
    *(int *)D_001AA688 = -1;
L9BE8E:;
    *(signed char *)itemmaker_slot_kinds = 0;
    *(signed char *)D_00190CE8 = 0;
}

void travel_toggle_option(int a1)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9BED7;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9BEDC;
L9BED7:;
    return;
L9BEDC:;
    sound_play(203, (int)player_object, 110);
    if (a1 >= 2) goto L9BF03;
    *(signed char *)travel_options ^= 3;
    goto L9BFA8;
L9BF03:;
    if (a1 >= 4) goto L9BF60;
    *(signed char *)travel_options ^= 12;
    if (((int)(unsigned short)(*(short *)travel_options & 8)) == 0) goto L9BF2F;
    if (player_character->ship_owned == 0) goto L9BF31;
L9BF2F:;
    goto L9BF3F;
L9BF31:;
    if (gold_can_afford(travel_trip_cost()) == 0) goto L9BF41;
L9BF3F:;
    goto L9BF5E;
L9BF41:;
    *(signed char *)travel_options ^= 12;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc(454, 1);
L9BF5E:;
    goto L9BFA8;
L9BF60:;
    *(signed char *)travel_options ^= 48;
    if (((int)(unsigned short)(*(short *)travel_options & 16)) == 0) goto L9BF89;
    if (gold_can_afford(travel_trip_cost()) == 0) goto L9BF8B;
L9BF89:;
    goto L9BFA8;
L9BF8B:;
    *(signed char *)travel_options ^= 48;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc(454, 1);
L9BFA8:;
    *(int *)D_001AA680 = travel_route(player_object->x, player_object->z, *(int *)D_001AA678, *(int *)D_001AA67C, 0);
    if (((int)(unsigned short)(*(short *)travel_options & 3)) != 2) return;
    *(int *)D_001AA680 = (*(int *)D_001AA680 << 7) / 256;
}

void travel_popup_exit(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9C257;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9C259;
L9C257:;
    return;
L9C259:;
    sound_play(203, (int)player_object, 110);
    *(signed char *)D_00190CE5 = 0;
}

void func_0009C27F(void)
{
    travel_popup_exit();
}

void travel_button_find(void)
{
    int l_18;

    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9C76B;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9C770;
L9C76B:;
    return;
L9C770:;
    if (*(signed char *)itemmaker_slot_kinds == 0) return;
    *(signed char *)D_00190CE8 = 0;
    sound_play(203, (int)player_object, 110);
    *(signed char *)D_0012B508 = 145;
    func_00142790();
    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(807, (int)D_0017743D);
    mc_sprintf(l_18, (int)D_00177460, *(int *)D_001846F4);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    *(signed char *)text_rsc_buffer = 0;
    inpstr_begin_text((int)text_rsc_buffer, 32);
    msgbox_show_string(l_18, 2);
    if (strnicmp((int)D_0017748E, (int)text_rsc_buffer, 4) != 0) goto L9C84F;
    mc_memcpy((int)text_rsc_buffer, (int)D_00190FE8, (int)&*(signed char *)((char *)func_000A0DF4((int)text_rsc_buffer) + 1), (int)D_0017743D, 812, 2048);
L9C84F:;
    *(signed char *)D_00190CE7 = 1;
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

    l_44 = *(struct map_location **)D_00196A9C;
    l_28 = 32767;
    if (*(int *)D_001AA694 == 0) goto L9C892;
    return -1;
L9C892:;
    if (((int)(short)*(short *)mouse_y) <= 171) goto L9C8AC;
    return -1;
L9C8AC:;
    if (*(signed char *)D_00190CE8 == 0) goto L9C8EA;
    l_20 = (((int)(short)*(short *)mouse_x) >> 2) + ((int)(short)*(short *)D_00190D64);
    l_1C = ((((int)(short)*(short *)mouse_y) - 12) >> 2) + ((int)(short)*(short *)D_00190D66);
    goto L9C901;
L9C8EA:;
    l_20 = (int)(short)*(short *)mouse_x;
    l_1C = ((int)(short)*(short *)mouse_y) - 12;
L9C901:;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) >= 2) goto L9C946;
    l_3C = ((int)(short)*(short *)(D_00188898 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2))) << 15;
    l_38 = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)))) << 15;
    goto L9C988;
L9C946:;
    l_3C = ((int)(short)*(short *)((char *)(int)(*(char **)D_001AA684 + (((int)(unsigned char)*(signed char *)D_001AA6A5) << 2)))) << 15;
    l_38 = (499 - ((int)(short)(*(short **)D_001AA684)[((int)(unsigned char)*(signed char *)D_001AA6A5) * 2 + 1])) << 15;
L9C988:;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) == 62) goto L9C9A0;
    if (((int)(signed char)*(signed char *)itemmaker_slot_kinds) != 20) goto L9C9B0;
L9C9A0:;
    l_20 >>= 2;
    l_1C >>= 2;
    l_1C += 2;
    l_20 += 2;
L9C9B0:;
    l_24 = 0;
L9C9B7:;
    if (l_24 < *(int *)D_00196A28) goto L9C9D6;
    goto L9CA7E;
L9C9C7:;
    l_24++;
    l_44++;
    goto L9C9B7;
L9C9D6:;
    if ((((int)(unsigned char)*(signed char *)travel_filter) & (1 << ((int)(unsigned char)*(signed char *)(location_type_category + (((unsigned)(l_44->x_type_flags << 2)) >> 27))))) == 0) goto L9C9C7;
    if (((struct bf8_3_1 *)&cheat_flags)->f != 0) goto L9CA1E;
    if ((l_44->x_type_flags & 0x40000000) == 0) goto L9CA1C;
    if ((l_44->x_type_flags & 0x80000000) == 0) goto L9CA1E;
L9CA1C:;
    goto L9CA20;
L9CA1E:;
    goto L9CA22;
L9CA20:;
    goto L9C9C7;
L9CA22:;
    l_34 = ((unsigned)((l_44->x_type_flags & 33554431) - l_3C)) >> 15;
    l_30 = ((unsigned)(-((l_44->y_size & 16777215) - l_38))) >> 15;
    l_2C = func_000C7FD9(l_34, l_30, l_20, l_1C);
    if (l_2C >= l_28) goto L9CA74;
    l_40 = l_44;
    l_28 = l_2C;
L9CA74:;
    if (l_28 >= 2) goto L9C9C7;
L9CA7E:;
    if (l_24 == *(int *)D_00196A28) goto L9CAA3;
    *(int *)D_001889C1 = (*(int *)travel_selected_location = l_24);
    return l_24;
L9CAA3:;
    *(int *)D_001889C1 = (*(int *)travel_selected_location = -1);
    return -1;
}

int travel_find_location(int a1)
{
    int l_28;
    int l_24;
    struct map_location *l_20;
    int l_1C;

    l_24 = func_000A0DF4(a1);
    l_20 = *(struct map_location **)D_00196A9C;
    if (l_24 != 0) goto L9CB01;
    return -1;
L9CB01:;
    l_28 = 0;
L9CB08:;
    if (l_28 < *(int *)D_00196A28) goto L9CB27;
    goto L9CB92;
L9CB18:;
    l_28++;
    l_20++;
    goto L9CB08;
L9CB27:;
    if (((struct bf8_3_1 *)&cheat_flags)->f != 0) goto L9CB44;
    if ((l_20->x_type_flags & 0x40000000) == 0) goto L9CB42;
    if ((l_20->x_type_flags & 0x80000000) == 0) goto L9CB44;
L9CB42:;
    goto L9CB46;
L9CB44:;
    goto L9CB48;
L9CB46:;
    goto L9CB18;
L9CB48:;
    l_1C = (int)(*(char **)D_00196A7C + (l_28 << 5)) + 4;
    if (strnicmp((int)D_0017748E, l_1C, 4) != 0) goto L9CB76;
    l_1C += 4;
L9CB76:;
    if (stricmp(a1, l_1C) != 0) goto L9CB90;
    return l_28;
L9CB90:;
    goto L9CB18;
L9CB92:;
    l_20 = *(struct map_location **)D_00196A9C;
    l_28 = 0;
L9CBA1:;
    if (l_28 < *(int *)D_00196A28) goto L9CBC0;
    goto L9CC2B;
L9CBB1:;
    l_28++;
    l_20++;
    goto L9CBA1;
L9CBC0:;
    if (((struct bf8_3_1 *)&cheat_flags)->f != 0) goto L9CBDD;
    if ((l_20->x_type_flags & 0x40000000) == 0) goto L9CBDB;
    if ((l_20->x_type_flags & 0x80000000) == 0) goto L9CBDD;
L9CBDB:;
    goto L9CBDF;
L9CBDD:;
    goto L9CBE1;
L9CBDF:;
    goto L9CBB1;
L9CBE1:;
    l_1C = (int)(*(char **)D_00196A7C + (l_28 << 5)) + 4;
    if (strnicmp((int)D_0017748E, l_1C, 4) != 0) goto L9CC0F;
    l_1C += 4;
L9CC0F:;
    if (strnicmp(a1, l_1C, l_24) != 0) goto L9CC29;
    return l_28;
L9CC29:;
    goto L9CBB1;
L9CC2B:;
    return -1;
}

void travel_open_trip(void)
{
    if (*(int *)travel_selected_location == (-1)) return;
    *(signed char *)D_00190CE5 = 1;
    *(int *)D_001AA678 = (*(struct map_location **)D_00196A9C)[*(int *)travel_selected_location].x_type_flags & 33554431;
    *(int *)D_001AA67C = (*(struct map_location **)D_00196A9C)[*(int *)travel_selected_location].y_size & 16777215;
    *(int *)D_001AA680 = travel_route(player_object->x, player_object->z, *(int *)D_001AA678, *(int *)D_001AA67C, 0);
    if (((int)(unsigned short)(*(short *)travel_options & 3)) != 2) goto L9CCF0;
    *(int *)D_001AA680 = (*(int *)D_001AA680 << 7) / 256;
L9CCF0:;
    if (((int)(unsigned short)(*(short *)travel_options & 8)) == 0) goto L9CD0F;
    if (player_character->ship_owned == 0) goto L9CD11;
L9CD0F:;
    goto L9CD1F;
L9CD11:;
    if (gold_can_afford(travel_trip_cost()) == 0) goto L9CD21;
L9CD1F:;
    return;
L9CD21:;
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

    l_48 = *(struct map_location **)D_00196A9C;
    l_2C = 32767;
    l_24 = player_object->x / 32768;
    l_20 = player_object->z / 32768;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) >= 2) goto L9CDCB;
    l_40 = ((int)(short)*(short *)(D_00188898 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2))) << 15;
    l_3C = (499 - ((int)(short)*(short *)(D_0018889A + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) << 2)))) << 15;
    goto L9CE0D;
L9CDCB:;
    l_40 = ((int)(short)*(short *)((char *)(int)(*(char **)D_001AA684 + (((int)(unsigned char)*(signed char *)D_001AA6A5) << 2)))) << 15;
    l_3C = (499 - ((int)(short)(*(short **)D_001AA684)[((int)(unsigned char)*(signed char *)D_001AA6A5) * 2 + 1])) << 15;
L9CE0D:;
    l_28 = 0;
L9CE14:;
    if (l_28 < *(int *)D_00196A28) goto L9CE2C;
    goto L9CE9E;
L9CE24:;
    l_28++;
    goto L9CE14;
L9CE2C:;
    if ((((unsigned)(l_48->x_type_flags << 2)) >> 27) != a1) goto L9CE24;
    l_38 = ((unsigned)((l_48->x_type_flags & 33554431) - l_40)) >> 15;
    l_34 = ((unsigned)(-((l_48->y_size & 16777215) - l_3C))) >> 15;
    l_30 = func_000C7FD9(l_38, l_34, l_24, l_20);
    if (l_30 >= l_2C) goto L9CE95;
    l_1C = l_28;
    l_44 = l_48;
    l_2C = l_30;
L9CE95:;
    l_48++;
    goto L9CE24;
L9CE9E:;
    if (l_2C != 32767) goto L9CEB1;
    return *(int *)D_001889C1;
L9CEB1:;
    return l_1C;
}

int travel_pixel_time(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = func_00020057((a1 << 15) + 16384, (int)&*(signed char *)((char *)(a2 << 15) + 16384));
    l_18 = (*(int *)travel_transport_factor * 102) / 256;
    if (*(signed char *)D_00196285 == 0) goto L9D2CB;
    (*(int *)D_001AA674)++;
    if (((int)(unsigned short)(*(short *)travel_options & 8)) != 0) goto L9D2B9;
    if (player_character->ship_owned == 0) goto L9D2C2;
L9D2B9:;
    return 51;
L9D2C2:;
    return 255;
L9D2CB:;
    return (((256 - *(int *)(terrain_travel_modifiers + (l_1C << 2))) + 256) * l_18) / 256;
}

void travel_check_transport_item(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group != 23) goto L9D353;
    if (l_18->index == 1) goto L9D355;
L9D353:;
    goto L9D366;
L9D355:;
    *(signed char *)D_00190D16 |= 2;
    *(int *)travel_transport_factor = 128;
L9D366:;
    if (l_18->group != 23) goto L9D381;
    if (l_18->index == 0) goto L9D383;
L9D381:;
    return;
L9D383:;
    *(signed char *)D_00190D16 |= 1;
    *(int *)travel_transport_factor = 192;
}

void travel_find_transport(void)
{
    *(int *)travel_transport_factor = 256;
    object_foreach((int)player_entity->children, (int)travel_check_transport_item);
}

void travel_load_region_part(void)
{
    int l_18;

    if (*(int *)D_001AA668 == 0) goto L9D3F5;
    if (*(int *)D_001AA668 != (-1751672937)) goto L9D3F7;
L9D3F5:;
    goto L9D415;
L9D3F7:;
    mc_free(*(int *)D_001AA668, (int)D_0017743D, 1119);
    *(int *)D_001AA668 = -1751672937;
L9D415:;
    func_000A0ED9(1121, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_00177493, ((int)(unsigned char)*(signed char *)D_001AA6A5) + 97, ((int)(signed char)*(signed char *)itemmaker_slot_kinds) - 1);
    *(int *)D_001AA668 = disk_read_file((int)text_buffer, 0);
    l_18 = *(int *)screen_buffer;
    *(int *)screen_buffer = *(int *)D_001AA668;
    travel_draw_locations();
    *(int *)screen_buffer = l_18;
}

void travel_button_arrows(int a1)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9D4B8;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9D4BA;
L9D4B8:;
    return;
L9D4BA:;
    if (*(signed char *)itemmaker_slot_kinds == 0) return;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) < 2) return;
    if (a1 != 3) goto L9D4EA;
    if (((int)(unsigned char)*(signed char *)D_001AA6A4) <= 2) goto L9D4E8;
    *(signed char *)D_001AA6A5 ^= 2;
L9D4E8:;
    goto L9D4F1;
L9D4EA:;
    *(signed char *)D_001AA6A5 ^= 1;
L9D4F1:;
    sound_play(203, (int)player_object, 110);
    travel_load_region_part();
}

void travel_toggle_zoom(void)
{
    *(signed char *)D_00190CE8 ^= 1;
    *(short *)D_00190D64 = *(short *)mouse_x - 40;
    *(short *)D_00190D66 = *(short *)mouse_y - 32;
    if (*(short *)D_00190D64 >= 0) goto L9D55B;
    *(short *)D_00190D64 = 0;
L9D55B:;
    if (*(short *)D_00190D66 >= 0) goto L9D56E;
    *(short *)D_00190D66 = 0;
L9D56E:;
    if ((((int)(short)*(short *)D_00190D64) + 80) <= 319) goto L9D588;
    *(short *)D_00190D64 = 239;
L9D588:;
    if ((((int)(short)*(short *)D_00190D66) + 40) <= 148) return;
    *(short *)D_00190D66 = 108;
}

void func_0009D5AC(int a1, int a2, int a3)
{
    int l_10;

    a2 = (a2 + (((int)(short)*(short *)D_00190D66) * 320)) + ((int)(short)*(short *)D_00190D64);
    l_10 = 0;
L9D5E6:;
    if (l_10 < 40) goto L9D5F6;
    return;
L9D5EE:;
    l_10++;
    goto L9D5E6;
L9D5F6:;
    func_000CDD49((l_10 * 320) + a2, (l_10 * 1280) + a1, 80);
    goto L9D5EE;
}

int travel_trip_cost(void)
{
    int l_20;
    int l_1C;

    l_20 = (*(int *)D_001AA680 + 59) / 60;
    l_1C = 0;
    if (((int)(unsigned short)(*(short *)travel_options & 16)) == 0) goto L9D66D;
    if (guild_find_membership_by_bits(64) == 0) goto L9D66F;
L9D66D:;
    goto L9D68D;
L9D66F:;
    l_1C = (((l_20 - *(int *)D_001AA674) / 24) * 5) + 5;
L9D68D:;
    if (*(int *)D_001AA674 == 0) goto L9D6A1;
    if (player_character->ship_owned == 0) goto L9D6A3;
L9D6A1:;
    goto L9D6B7;
L9D6A3:;
    if (((int)(unsigned short)(*(short *)travel_options & 8)) != 0) goto L9D6B9;
L9D6B7:;
    goto L9D6C7;
L9D6B9:;
    if (guild_find_membership_by_bits(64) == 0) goto L9D6C9;
L9D6C7:;
    goto L9D6E5;
L9D6C9:;
    l_1C += ((*(int *)D_001AA674 / 24) + 1) * 25;
L9D6E5:;
    return l_1C;
}

void travel_button_im_at(void)
{
    int l_18;

    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9D726;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9D728;
L9D726:;
    return;
L9D728:;
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9D73F;
    travel_mark_player((int)(unsigned char)*(signed char *)current_region);
    return;
L9D73F:;
    *(int *)D_001AA688 = 0;
    l_18 = 1132;
    *(int *)D_00190CAC = *(int *)((char *)l_18);
}

void travel_draw_buttons(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (*(signed char *)itemmaker_slot_kinds == 0) goto L9D79C;
    func_00144F68(3, 175, 45, 22, *(int *)D_00195B5C);
    goto L9D7C0;
L9D79C:;
    func_00144F68(3, 186, 45, 11, *(int *)D_00195B5C + 495);
L9D7C0:;
    l_24 = 0;
L9D7C7:;
    if (l_24 < 4) goto L9D7DA;
    return;
L9D7D2:;
    l_24++;
    goto L9D7C7;
L9D7DA:;
    l_20 = 1;
    if ((((int)(unsigned char)*(signed char *)travel_filter) & (1 << l_24)) == 0) goto L9D7FE;
    l_20 = 0;
L9D7FE:;
    if ((l_24 & 1) == 0) goto L9D810;
    l_18 = 80;
    goto L9D817;
L9D810:;
    l_18 = 99;
L9D817:;
    l_1C = 0;
L9D81E:;
    if (l_1C < 11) goto L9D831;
    goto L9D8AE;
L9D829:;
    l_1C++;
    goto L9D81E;
L9D831:;
    mc_memcpy(((int)(short)*(short *)(D_00188790 + (l_24 * 2))) + (int)(*(char **)screen_buffer + ((((int)(short)*(short *)(D_00188798 + (l_24 * 2))) + l_1C) * 320)), (int)(*(char **)(D_001AA65C + (l_20 << 2)) + (((((int)(short)*(short *)(D_00188798 + (l_24 * 2))) + l_1C) - 175) * 179)) + (((int)(short)*(short *)(D_00188790 + (l_24 * 2))) - 50), l_18, (int)D_0017743D, 1222, 4);
    goto L9D829;
L9D8AE:;
    goto L9D7D2;
}

void travel_button_filter(void)
{
    int l_18;

    l_18 = 0;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L9D8F2;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L9D8F4;
L9D8F2:;
    return;
L9D8F4:;
    if (*(signed char *)itemmaker_slot_kinds == 0) return;
    if (((int)(short)*(short *)mouse_y) >= 186) goto L9D92B;
    if (((int)(short)*(short *)mouse_x) >= 149) goto L9D922;
    *(signed char *)travel_filter ^= 1;
    goto L9D929;
L9D922:;
    *(signed char *)travel_filter ^= 2;
L9D929:;
    goto L9D949;
L9D92B:;
    if (((int)(short)*(short *)mouse_x) >= 149) goto L9D942;
    *(signed char *)travel_filter ^= 4;
    goto L9D949;
L9D942:;
    *(signed char *)travel_filter ^= 8;
L9D949:;
    travel_open_region(((int)(signed char)*(signed char *)itemmaker_slot_kinds) - 1);
}

int func_0009D960(int a1, int a2)
{
    return 0;
}

void travel_show_days_left(int a1)
{
    mc_memcpy(*(int *)screen_buffer, *(int *)D_001AA690, 64000, (int)D_0017743D, 1259, 4);
    func_000A0ED9(1260, (int)D_0017743D);
    mc_sprintf((int)text_buffer, (int)D_001774A3, ((unsigned)a1) / 1440);
    text_draw_colored((int)text_buffer, 240, 2, 145, 156);
    func_000CDD81(1);
}
