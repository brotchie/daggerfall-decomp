/* automap.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int D_000C23B8;
extern int D_000C23BC;
extern int D_000C23C0;
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern int D_000CEA24;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char D_00132F58;
extern int D_00136911;
extern char D_00136E00[];
extern char D_00136E24[];
extern signed char key_down_esc;
extern signed char key_down_up;
extern signed char key_down_left;
extern signed char key_down_right;
extern signed char key_down_down;
extern short D_0014294C;
extern int screen_buffer;
extern char D_00170794[];
extern char D_001707A1[];
extern char D_001707AE[];
extern char D_001707B8[];
extern char D_001707E4[];
extern unsigned char player_environment;
extern char automap_buttons[];
extern char D_0017A02E[];
extern char D_0017A030[];
extern char D_0017A032[];
extern char D_0017A034[];
extern signed char D_0017A11D;
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char D_00190BE4[];
extern int D_00190BE8;
extern int D_00190BEC;
extern int automap_pitch;
extern int automap_yaw;
extern int D_00190BFC;
extern int D_00190C00;
extern int D_00190C04;
extern signed char D_00190CE5;
extern short D_00190D68;
extern short D_00190D6A;
extern char text_macro_fpc[];
extern int text_macro_fnpc;
extern int D_00190E18;
extern signed char player_motion_flags;
extern char D_001940E4[];
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct record *D_00195AF4;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern struct settings *game_settings;
extern char D_00195C44[];
extern struct record *D_00195CB8;
extern signed char D_00196272;
extern signed char mouse_buttons_prev;
extern signed char D_0019629F;
extern int D_00196D88;
extern int D_00196D8C;
extern int D_00196D94;
extern struct record *D_00196DA0;
extern int D_00196DA4;
extern int D_00196DA8;
extern int D_00196DAC;
extern struct record *D_00196DB0;
extern int D_00196DB4;
extern signed char automap_top_down;
extern struct record *quest_event_object2;
extern struct record *quest_event_object;
extern signed char cfg_show_markers;

extern int engine_pick_object(int, int, int);
extern int func_000283FD(short, short);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int disk_open_data(int);
extern int disk_file_exists(int);
extern int guild_find_membership_by_kind(unsigned char);
extern struct building *object_building(struct record *);
extern int automap_draw_object_cb(int);
extern int location_contains(int, int);
extern int building_name(struct building *);
extern int inpstr_edit(int, short, short, short, short, short);
extern int object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern struct record *marker_find_first(struct record *, int);
extern int location_cell_at(int, int);
extern int rand();
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int func_000A00CB();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int mc_memmove();
extern int stricmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000C7F07();
extern int func_000CDD81();
extern int func_0012A254();
extern int func_0012A2D0();
extern int func_0012A4F0();
extern int func_0012A870();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_00135E39();
extern int func_00135E90();
extern int func_00136AB4();
extern int func_00136AD8();
extern int func_00137000();
extern int func_00137486();
extern int func_001374FC();
extern int func_00137725();
extern int func_00144F68();
extern int func_0014D23C();
extern void screenshot_poll(void);
extern void func_00027717(struct record *, int);
extern void town_map_open(void);
extern void town_map_scroll(int);
extern void func_0002829B(int);
extern void quest_set_state(struct quest *, struct qbn_op *, int);
extern void quest_set_arg_state(struct quest *, struct qbn_op *, int, int);
extern void qaction_place_foe(struct qbn_op *, int);
extern void text_draw(int, int, int);
extern void hud_draw_heading_strip(int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void world_collect_objects(void);
extern void building_grant_access(struct building *, unsigned char, int);
extern void object_foreach(struct record *, int);
int automap_move_forward(int);
int automap_move_back(int);
int automap_move_left(int);
int automap_move_right(int);
int func_00027641(struct record *);
int func_000281AF(void);
int func_0002839E(int);
int func_000284DA(int);
void automap_draw(void);
void automap_render(void);
void automap_find_record(void);
void automap_match_record(struct record *);
void func_000276B8(void);
void automap_init_view(void);
void func_00028210(int);
void func_0002830F(int);
void func_00028547(void);
void town_map_note_building(struct record *, struct building *);
void automap_draw_block_overview(void);
void func_00028DDB(struct record *);
void func_00028DFD(void);
void func_00028ED1(struct record *);
void automap_restore_seen(void);
void func_00028F8B(struct record *);
void func_000298F3(struct record *);
#pragma aux func_000A0ED9 parm routine [];

void automap_open(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    struct record *l_1C;
    struct record *l_18;

    l_2C = 0;
    if (((int)player_environment) == 1 && location_contains(player_object->x, player_object->z) != 0) {
        town_map_open();
        return;
    }
    if (((int)player_environment) == 1) return;
    l_20 = (int)(unsigned char)cfg_show_markers;
    cfg_show_markers = 1;
    mouse_buttons_prev = 0;
    automap_init_view();
    D_000CEA24 = 1048576;
    func_0012A254(8);
    automap_restore_seen();
    D_0019629F = 1;
    l_24 = (int)(unsigned char)D_00196272;
    D_00196272 = 1;
    *(int *)text_macro_fpc = disk_read_file((int)D_00170794, 0);
    D_00190E18 = disk_read_file((int)D_001707A1, 0);
    *(int *)D_00190BE4 = player_object->x;
    D_00190BE8 = player_object->y;
    D_00190BEC = player_object->z;
    D_0014294C = 199;
    func_0012A2D0(160, 84, 160, 85);
    automap_find_record();
    text_macro_fnpc = 0;
    l_18 = 0;
    if (((int)D_00195CB8 != 0 && ((int)player_environment) == 3) || ((int)player_environment) == 2) {
        if (((int)player_environment) == 3) {
            l_1C = D_00195CB8;
            while (l_1C != 0 && l_1C->type != 47 && l_1C->type != 1) l_1C = l_1C->parent;
        } else {
            l_1C = player_object->parent;
        }
        if (l_1C != 0) {
            l_18 = object_create_child(l_1C, 0, 62);
            l_18->image2 = 999;
            l_18->image = 0;
            mc_memcpy(&l_18->angle_x, &player_object->angle_x, 18, (int)D_001707AE, 130, 4);
            l_18->yaw = 2047 - l_18->yaw;
            func_000C7F07(l_18->angle_x, l_18->yaw, l_18->angle_z, (int)RECORD_DATA(l_18) + 12);
            l_18->type = 6;
            l_18->flags = 128;
            l_18->y -= 40;
        }
    }
    while (l_2C == 0) {
        automap_draw();
        if (key_down_left != 0) {
            automap_move_left(2);
        } else if (key_down_right != 0) {
            automap_move_right(3);
        } else if (key_down_up != 0) {
            automap_move_forward(0);
        } else if (key_down_down != 0) {
            automap_move_back(1);
        }
        if (key_down_esc != 0) l_2C = 1;
        if (((int)(unsigned char)(mouse_buttons & 3)) != 0) {
            for (l_28 = 0; l_28 < 12; l_28++) {
                if (mouse_x > *(short *)(automap_buttons + (l_28 * 12)) && mouse_x < *(short *)(D_0017A030 + (l_28 * 12)) && mouse_y > *(short *)(D_0017A02E + (l_28 * 12)) && mouse_y < *(short *)(D_0017A032 + (l_28 * 12))) {
                    if (((int)(unsigned char)(mouse_buttons & 3)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 3)) == 0) {
                        sound_play(203, player_object, 100);
                    }
                    l_2C = ((int (*)())(*(int *)(D_0017A034 + (l_28 * 12))))(l_28);
                }
            }
        }
        screenshot_poll();
        func_000CDD81(1);
    }
    while (key_down_esc != 0);
    if (*(int *)text_macro_fpc != 0 && *(int *)text_macro_fpc != (-1751672937)) {
        mc_free(*(int *)text_macro_fpc, (int)D_001707AE, 168);
        *(int *)text_macro_fpc = -1751672937;
    }
    if (D_00190E18 != 0 && D_00190E18 != (-1751672937)) {
        mc_free(D_00190E18, (int)D_001707AE, 169);
        D_00190E18 = -1751672937;
    }
    if (l_18 != 0) object_free_single(l_18);
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        func_0012A2D0(160, 100, 160, 100);
    } else {
        func_0012A2D0(160, 77, 160, 77);
    }
    object_foreach(D_00195AC4, (int)func_00028F8B);
    func_00028DFD();
    D_00196272 = *(signed char *)&l_24;
    cfg_show_markers = *(signed char *)&l_20;
    D_0019629F = 0;
}

void automap_draw(void)
{
    int l_18;

    func_0014D23C(-1);
    func_0012B2EB();
    mc_memcpy(screen_buffer, *(int *)text_macro_fpc, 64000, (int)D_001707AE, 192, 4);
    l_18 = D_00190E18;
    if (automap_top_down == 0) {
        func_00144F68((int)(unsigned short)*(short *)((char *)l_18), (int)(unsigned short)*(short *)((char *)l_18 + 2), (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    }
    automap_render();
    if (((int)player_environment) != 2) {
        if (D_00196DAC != 0) text_draw_colored(D_00196DAC, 2, 191, 145, 156);
        automap_draw_block_overview();
    }
    hud_draw_heading_strip(1);
    func_0012B3ED();
    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
}

int func_00026E87(void)
{
    if ((int)D_00196DB0 == 0) return 0;
    if (D_00196DAC != 0) {
        mc_strncpy((int)text_buffer, D_00196DAC, 160, (int)D_001707AE, 213);
    } else {
        mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 215, 160);
    }
    inpstr_edit((int)text_buffer, 2, 191, 300, 9, 50);
    if (D_00196DAC != 0 && text_macro_fnpc != 0) func_000276B8();
    if (text_buffer[0] != 0) func_00027717(D_00196DB0, (int)text_buffer);
    D_00196DAC = func_00027641(D_00196DB0);
    sound_play(206, player_object, 100);
    return 0;
}

int automap_button_rotate_left(void)
{
    automap_yaw = (automap_yaw + 16) & 2047;
    return 0;
}

int automap_button_rotate_right(void)
{
    automap_yaw = (automap_yaw - 16) & 2047;
    return 0;
}

int automap_button_exit(void)
{
    if (((int)player_environment) == 1) return 1;
    func_0012A254(8);
    D_000CEA24 = 396288;
    return 1;
}

void automap_render(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = D_00136911;
    func_00137000(0, automap_yaw, 0, (int)D_00136E00);
    D_000C23C4 = D_00190BFC;
    D_000C23C8 = D_00190C00;
    D_000C23CC = D_00190C04;
    func_00137486((int)&D_000C23C4, (int)&D_000C23C8, (int)&D_000C23CC, (int)D_00136E00);
    D_000C23C4 += *(int *)D_00190BE4;
    D_000C23C8 += D_00190BE8;
    D_000C23CC += D_00190BEC;
    D_000C23B8 = automap_pitch;
    D_000C23BC = (-automap_yaw) & 2047;
    D_000C23C0 = 0;
    D_00136911 = 4096;
    func_00135E90();
    func_0012A4F0();
    func_00136AB4();
    func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
    func_00137725((int)D_00136E00, (int)D_00136E24);
    player_motion_flags |= 1;
    l_28 = 0;
    l_24 = 0;
    l_20 = 65535;
    func_001374FC((int)&l_28, (int)&l_24, (int)&l_20, (int)D_00136E00);
    func_00136AD8(l_28, l_24, l_20, 28, 0, 8);
    if (((int)player_environment) == 3) {
        object_foreach(D_00195AC4, (int)automap_draw_object_cb);
    } else {
        object_foreach(player_object->parent->children, (int)automap_draw_object_cb);
    }
    player_motion_flags &= 254;
    l_18 = func_0012A870(2);
    if (l_18 != 0 || D_00132F58 != 0) {
        D_00132F58 = 0;
        func_00135E39();
    }
    D_00136911 = l_1C;
}

int automap_move_forward(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    func_00137000(0, automap_yaw, 0, (int)D_00136E00);
    l_1C = -16;
    l_24 = 0;
    l_20 = l_24;
    func_00137486((int)&l_24, (int)&l_20, (int)&l_1C, (int)D_00136E00);
    *(int *)D_00190BE4 += l_24;
    D_00190BEC += l_1C;
    return 0;
}

int automap_move_back(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    func_00137000(0, automap_yaw, 0, (int)D_00136E00);
    l_1C = 16;
    l_24 = 0;
    l_20 = l_24;
    func_00137486((int)&l_24, (int)&l_20, (int)&l_1C, (int)D_00136E00);
    *(int *)D_00190BE4 += l_24;
    D_00190BEC += l_1C;
    return 0;
}

int automap_move_left(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    func_00137000(0, automap_yaw, 0, (int)D_00136E00);
    l_24 = 16;
    l_1C = 0;
    l_20 = l_1C;
    func_00137486((int)&l_24, (int)&l_20, (int)&l_1C, (int)D_00136E00);
    *(int *)D_00190BE4 += l_24;
    D_00190BEC += l_1C;
    return 0;
}

int automap_move_right(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    func_00137000(0, automap_yaw, 0, (int)D_00136E00);
    l_24 = -16;
    l_1C = 0;
    l_20 = l_1C;
    func_00137486((int)&l_24, (int)&l_20, (int)&l_1C, (int)D_00136E00);
    *(int *)D_00190BE4 += l_24;
    D_00190BEC += l_1C;
    return 0;
}

int automap_button_upstairs(void)
{
    if (D_00190BE8 > (-3072)) D_00190BE8 -= 16;
    return 0;
}

int automap_button_downstairs(void)
{
    if (D_00190BE8 < 3072) D_00190BE8 += 16;
    return 0;
}

int automap_button_map_click(void)
{
    char l_2C[20];

    if (((int)player_environment) == 1) {
        func_00028547();
        return 0;
    }
    engine_pick_object((int)(short)mouse_x, (int)(short)mouse_y, (int)l_2C);
    if ((*(int *)l_2C & 1) != 0) {
        if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
            (*(struct record **)(l_2C + 4))->flags |= 0x400;
        } else {
            D_00196DAC = func_00027641((struct record *)(*(int *)&D_00196DB0 = *(int *)((char *)l_2C + 4)));
        }
    } else {
        D_00196DB0 = 0;
    }
    return 0;
}

void automap_find_record(void)
{
    struct record *l_18;

    D_00195AF4 = (struct record *)((*(int *)&D_00196DA0 = 0));
    if (player_entity == 0) return;
    object_foreach(player_entity->children, (int)automap_match_record);
    if (D_00195AF4 == 0) {
        l_18 = object_create_child(player_entity, 0, 10240);
        l_18->type = 51;
        l_18->id = object_new_id(D_00195AC4->id >> 16);
        l_18->created_minutes = game_minutes;
        l_18->seen_count = (current_location->object_counter + 7) / 8;
        D_00196DB4 = (int)RECORD_DATA(l_18);
        D_00196DA0 = l_18;
        return;
    }
    D_00196DB4 = (int)RECORD_DATA(D_00195AF4);
    D_00196DA0 = D_00195AF4;
}

void automap_match_record(struct record *a1)
{
    if (a1->type != 51) return;
    if ((a1->id >> 16) != (D_00195AC4->id >> 16)) return;
    D_00195AF4 = a1;
}

int func_00027641(struct record *a1)
{
    int l_1C;

    l_1C = D_00196DB4;
    while (*(signed char *)((char *)l_1C + 2) != 0) {
        text_macro_fnpc = l_1C;
        if (((int)(unsigned short)*(short *)((char *)l_1C)) == ((int)a1->id & 65535)) {
            return l_1C + 2;
        }
        l_1C += func_000A0DF4(l_1C + 2) + 3;
    }
    return 0;
}

void func_000276B8(void)
{
    int l_18;

    l_18 = func_000A0DF4(text_macro_fnpc + 2) + 3;
    mc_memmove(text_macro_fnpc, text_macro_fnpc + l_18, ((int)(*(char **)&D_00196DB4 + 2048) - text_macro_fnpc) - l_18, (int)D_001707AE, 494, 4);
}

void automap_init_view(void)
{
    int l_18;

    if (D_0017A11D != 0) return;
    D_0017A11D = 1;
    D_00190BFC = 0;
    if (((int)player_environment) == 3) {
        l_18 = -1536;
    } else {
        l_18 = -1024;
    }
    D_00190C00 = l_18;
    D_00190C04 = 0;
    automap_pitch = 512;
    automap_yaw = 0;
    automap_top_down = 1;
}

int automap_button_view_mode(void)
{
    int l_20;
    int l_1C;

    if (mouse_buttons_prev != 0) return 0;
    if (automap_top_down != 0) {
        D_00190BFC = 0;
        D_00190C00 = -768;
        if (((int)player_environment) == 3) {
            l_1C = -2048;
        } else {
            l_1C = -1024;
        }
        D_00190C04 = l_1C;
        automap_pitch = 128;
        automap_yaw = 0;
        automap_top_down = 0;
    } else {
        D_00190BFC = 0;
        if (((int)player_environment) == 3) {
            l_20 = -1536;
        } else {
            l_20 = -1024;
        }
        D_00190C00 = l_20;
        D_00190C04 = 0;
        automap_pitch = 512;
        automap_yaw = 0;
        automap_top_down = 1;
    }
    return 0;
}

void func_000278E4(void)
{
    struct record *l_1C;
    struct record *l_18;

    l_1C = player_entity->children;
    while (l_1C != 0) {
        l_18 = l_1C->next;
        if (l_1C->type == 51) {
            if (((unsigned)(game_minutes - l_1C->created_minutes)) > 43200) {
                object_free_single(l_1C);
            }
        }
        l_1C = l_18;
    }
}

void func_00027947(void)
{
    D_00196DA8 = (current_location->width * current_location->height) << 12;
    D_00196DA4 = mc_malloc(D_00196DA8, (int)D_001707AE, 582);
    mc_memset(D_00196DA4, 0, D_00196DA8, (int)D_001707AE, 583, 4);
}

void func_000279B9(void)
{
    D_00196DA8 = 0;
    if (D_00196DA4 == 0 || D_00196DA4 == (-1751672937)) return;
    mc_free(D_00196DA4, (int)D_001707AE, 598);
    D_00196DA4 = -1751672937;
}

int func_000281AF(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_1C = *(int *)D_00195C44 + 4;
    while (*(signed char *)((char *)l_1C) != 0) {
        l_24 += 5;
        l_20 = func_000A0DF4(l_1C + 4);
        l_24 += l_20;
        l_1C += l_20 + 5;
    }
    return l_24 + 4;
}

void func_00028210(int a1)
{
    D_0012B508 = 146;
    if (a1 != 0) {
        mc_strncpy((int)text_buffer, a1, 160, (int)D_001707AE, 809);
    } else {
        mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 811, 160);
    }
    inpstr_edit((int)text_buffer, 2, 191, 300, 9, 50);
}

void func_0002830F(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    l_1C = *(int *)D_00195C44 + 4;
    while (*(signed char *)((char *)l_1C) != 0 && a1 != 0) {
        l_20 = func_000A0DF4(l_1C + 4);
        l_1C += l_20 + 5;
        a1--;
    }
    l_18 = (func_000A0DF4(l_1C + 4) + l_1C) + 5;
    mc_memcpy(l_1C, l_18, (*(int *)D_00195C44 + 49999) - l_18, (int)D_001707AE, 839, 4);
}

int func_0002839E(int a1)
{
    int l_20;
    int l_1C;

    l_1C = *(int *)D_00195C44 + 4;
    while (*(short *)((char *)l_1C) != 0 && a1 != 0) {
        l_20 = func_000A0DF4(l_1C + 4);
        l_1C += l_20 + 5;
        a1--;
    }
    return l_1C;
}

int func_000284DA(int a1)
{
    int l_20;
    int l_1C;

    l_1C = *(int *)D_00195C44 + 4;
    while (*(short *)((char *)l_1C) != 0) {
        if (stricmp(l_1C + 4, a1) == 0) return 1;
        l_20 = func_000A0DF4(l_1C + 4);
        l_1C += l_20 + 5;
    }
    return 0;
}

void func_00028547(void)
{
    int l_20;
    int l_1C;
    int l_18;

    D_00190D68 = ((((int)(short)mouse_x) - 10) / 2) + D_00196D88;
    D_00190D6A = ((((int)(short)mouse_y) - 10) / 2) + D_00196D8C;
    l_20 = func_000283FD((int)(short)mouse_x, (int)(short)mouse_y);
    if (D_00196D94 != 0 && l_20 == 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_00190CE5 = 1;
        l_1C = func_0002839E(D_00196D94 - 1);
        l_18 = l_1C;
        *(short *)((char *)l_18) = D_00190D68;
        *(short *)((char *)l_18 + 2) = D_00190D6A;
        return;
    }
    if (l_20 != 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_00196D94 = l_20;
        return;
    }
    if (l_20 != 0) {
        D_00190CE5 = 1;
        l_1C = func_0002839E(l_20 - 1);
        l_18 = l_1C;
        D_00190D68 = *(short *)((char *)l_18);
        D_00190D6A = *(short *)((char *)l_18 + 2);
        mc_strncpy((int)text_buffer, l_1C + 4, 160, (int)D_001707AE, 921);
        func_0002830F(l_20 - 1);
        func_00028210((int)text_buffer);
        if (text_buffer[0] != 0) func_0002829B((int)text_buffer);
        return;
    }
    mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 928, 160);
    func_00028210((int)text_buffer);
    if (text_buffer[0] == 0) return;
    func_0002829B((int)text_buffer);
}

void func_000286F6(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    short l_18;

    l_20 = 0;
    l_1C = *(int *)D_00195C44 + 4;
    while (*(signed char *)((char *)l_1C) != 0) {
        *(int *)&l_18 = l_1C;
        l_28 = ((((int)(unsigned short)*(short *)(*(char **)&l_18)) - D_00196D88) * 2) + 10;
        l_24 = ((((int)(unsigned short)*(short *)(*(char **)&l_18 + 2)) - D_00196D8C) * 2) + 10;
        if ((l_20 + 1) == D_00196D94) {
            D_0012B508 = 244;
        } else {
            D_0012B508 = 146;
        }
        if (l_24 < 173) text_draw(l_1C + 4, l_28, l_24);
        l_2C = func_000A0DF4(l_1C + 4);
        l_1C += l_2C + 5;
        l_20++;
    }
}

void town_map_note_building(struct record *a1, struct building *a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_14 = building_name(a2);
    mc_memset(*(int *)D_00195C44, 0, 50000, (int)D_001707AE, 963, 4);
    func_000A0ED9(964, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, D_00195AC4->id >> 16);
    if (disk_file_exists((int)text_buffer) != 0) {
        disk_read_file((int)text_buffer, *(int *)D_00195C44);
        *(int *)(*(char **)D_00195C44) = game_minutes;
        mc_memcpy(*(int *)D_00195C44, *(int *)D_00195C44, 50000, (int)D_001707AE, 970, 4);
        if (func_000284DA(l_14) != 0) return;
    } else {
        *(int *)(*(char **)D_00195C44) = game_minutes;
    }
    l_1C = a1->x - D_00195AC4->x;
    l_18 = a1->z - D_00195AC4->z;
    l_1C >>= 6;
    l_18 >>= 6;
    l_18 = ((current_location->height << 6) - l_18) - 1;
    D_00190D68 = l_1C;
    D_00190D6A = l_18;
    func_0002829B(l_14);
    func_000A0ED9(986, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, D_00195AC4->id >> 16);
    disk_write_arena2_file((int)text_buffer, *(int *)D_00195C44, func_000281AF());
}

void automap_draw_block_overview(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = location_cell_at(player_object->x, player_object->z);
    l_18 = (int)marker_find_first(D_00195AC4, 8);
    if (l_18 != 0) l_18 = location_cell_at(((struct record *)l_18)->x, ((struct record *)l_18)->z);
    for (l_28 = 0; l_28 < 32; l_28++) {
        for (l_24 = 0; l_24 < 32; l_24++) {
            if (*(int *)(D_001940E4 + (((l_28 << 5) + l_24) << 2)) == 0) continue;
            l_20 = ((((31 - l_28) * 640) + 2560) + (l_24 * 2)) + 8;
            if (*(int *)(D_001940E4 + (((l_28 << 5) + l_24) << 2)) == l_1C) {
                D_0012B508 = 240;
            } else if (*(int *)(D_001940E4 + (((l_28 << 5) + l_24) << 2)) == l_18) {
                D_0012B508 = 132;
            } else {
                D_0012B508 = 145;
            }
            *(signed char *)((char *)(screen_buffer + l_20)) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + l_20) + 1) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + l_20) + 320) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + l_20) + 321) = D_0012B508;
        }
    }
}

void automap_save(void)
{
    if (((int)player_environment) != 3 || (int)D_00196DA0 == 0) {
        return;
    }
    *(int *)(*(char **)D_00195C44) = game_minutes;
    mc_memcpy((int)(*(char **)D_00195C44 + 4), (int)D_00196DA0, 10240, (int)D_001707AE, 1055, 4);
    func_000A0ED9(1056, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707E4, D_00195AC4->id >> 16);
    disk_write_arena2_file((int)text_buffer, *(int *)D_00195C44, 10244);
}

void automap_load(void)
{
    int l_18;

    if (((int)player_environment) != 3) return;
    mc_memset(*(int *)D_00195C44, 0, 50000, (int)D_001707AE, 1066, 4);
    func_000A0ED9(1067, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707E4, D_00195AC4->id >> 16);
    l_18 = disk_open_data((int)text_buffer);
    if (l_18 == (-1)) return;
    func_000A00CB(l_18, *(int *)D_00195C44, 50000);
    func_0009DEA7(l_18);
    automap_restore_seen();
}

void func_00028D1A(void)
{
    int l_24;
    struct building *l_20;
    short l_1C;
    short l_18;

    *(int *)&l_1C = -5;
    *(int *)&l_18 = -5;
    if (guild_find_membership_by_kind(0) != 0) *(int *)&l_18 = 108;
    if (guild_find_membership_by_kind(3) != 0) *(int *)&l_1C = 42;
    l_20 = current_location->buildings;
    for (l_24 = 0; current_location->building_count > l_24; l_24++, l_20++) {
        if (l_20->faction_id == (short)l_1C || l_20->faction_id == (short)l_18) {
            town_map_note_building(object_find_by_id(D_00195AC4, l_20->id), l_20);
        }
    }
}

void func_00028DDB(struct record *a1)
{
    a1->flags &= ~0x400;
}

void func_00028DFD(void)
{
    object_foreach(D_00195AC4, (int)func_00028DDB);
}

void func_00028EAA(void)
{
    automap_find_record();
    D_00196DA0 = D_00195AF4;
}

void func_00028ED1(struct record *a1)
{
    int l_18;

    if (a1->type != 6 && a1->type != 34) return;
    l_18 = (int)D_00196DA0 + 71;
    l_18 += 2048;
    if ((((int)(unsigned char)*(signed char *)((char *)((((unsigned)(a1->id & 65535)) >> 3) + l_18))) & (1 << ((a1->id & 65535) & 7))) == 0) return;
    a1->flags |= 128;
}

void automap_restore_seen(void)
{
    if ((int)D_00196DA0 == 0) automap_find_record();
    object_foreach(D_00195AC4, (int)func_00028ED1);
}

void func_00028F8B(struct record *a1)
{
    a1->flags &= ~0x80;
}

int qcond_op05_event_at_place(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_1C;
    struct record *l_18;

    l_1C = a2->args[2].object;
    if (l_1C->twin == 0) return 0;
    l_1C = l_1C->twin;
    while (l_1C != 0) {
        if (l_1C->type == 43 && ((int)(unsigned short)(l_1C->flags & 1)) != 0) break;
        l_1C = l_1C->parent;
    }
    if (l_1C->type != 43) return 0;
    l_18 = player_object->parent;
    while (l_18 != 0) {
        if (l_18->type == 43 && ((int)(unsigned short)(l_18->flags & 1)) != 0) break;
        l_18 = l_18->parent;
    }
    if (l_18->type != 43) return 0;
    if ((int)quest_event_object->twin == (int)a2->args[1].object && l_1C == l_18) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op43_pc_at_place(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct record *l_1C;
    struct record *l_18;

    l_1C = a2->args[2].object;
    if (l_1C == 0) return 0;
    if (l_1C->twin == 0) {
        quest_set_arg_state(a1, a2, 1, 0);
        return 0;
    }
    l_1C = l_1C->twin;
    while (l_1C != 0) {
        if ((l_1C->type == 43 && ((int)(unsigned short)(l_1C->flags & 1)) != 0) || l_1C->type == 47 || l_1C->type == 1) {
            break;
        }
        l_1C = l_1C->parent;
    }
    if (l_1C->type != 43 && l_1C->type != 47 && l_1C->type != 1) return 0;
    l_18 = player_object->parent;
    while (l_18 != 0) {
        if ((l_18->type == 43 && ((int)(unsigned short)(l_18->flags & 1)) != 0) || l_18->type == 1) {
            break;
        }
        l_18 = l_18->parent;
    }
    if (l_18->type != 43 && l_18->type != 1) return 0;
    if (l_1C->type == 47) l_1C = l_1C->parent;
    quest_set_arg_state(a1, a2, 1, ((l_18->id == l_1C->id) ? 1 : 0));
    return ((l_18->id == l_1C->id) ? 1 : 0);
}

void quest_op17_grant_building_access(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_18;
    struct building *l_14;

    l_18 = a2->args[1].object;
    if (l_18->twin == 0) return;
    l_14 = object_building(l_18->twin);
    if (l_14 == 0) return;
    building_grant_access(l_14, (unsigned char)a2->args[2].value, game_minutes + a2->args[3].value);
}

int qcond_op01_item_given_to_npc(struct quest *a1, struct qbn_op *a2)
{
    int l_24;
    struct record *l_20;
    struct record *l_1C;
    int l_18;

    l_20 = a2->args[1].object;
    l_1C = l_20;
    if (l_1C == 0) return 0;
    if (l_1C->twin == 0) return 0;
    l_1C = l_1C->twin;
    while (l_1C != 0 && l_1C != player_entity) l_1C = l_1C->parent;
    if (l_1C != player_entity) return 0;
    if ((a2->args[2].object->type == 65 && (short)quest_event_object2->data.person.faction_id == a2->args[2].object->faction_id) || (int)quest_event_object2->twin == (int)a2->args[2].object) {
        quest_set_state(a1, a2, 1);
        func_000298F3(a2->args[1].object->twin);
        object_delete(a2->args[1].object->twin);
        a2->args[1].object->twin = 0;
        a2->args[1].object = 0;
        return 1;
    }
    return 0;
}

int qcond_op03_event_object(struct quest *a1, struct qbn_op *a2)
{
    if ((int)quest_event_object->twin == (int)a2->args[1].object) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op21_event_same_kind(struct quest *a1, struct qbn_op *a2)
{
    if ((short)a2->args[1].object->image2 == (short)quest_event_object->image2) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op02_event_count(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_foe *l_18;

    l_18 = (struct qbn_foe *)a2->args[1].record;
    if ((short)a2->args[1].object->image2 == (short)quest_event_object->image2) {
        if ((short)(short)(l_18->killed) < (short)a2->args[2].value) {
            l_18->killed++;
            if ((short)(l_18->killed) == (short)a2->args[2].value) {
                quest_set_state(a1, a2, 1);
                return 1;
            }
        }
    }
    return 0;
}

void func_0002956B(struct record *a1)
{
    if (a1->twin == 0) return;
    a1->id = object_new_id(D_00195AC4->id >> 16);
    a1->twin->twin = 0;
    a1->twin = 0;
}

void quest_op87_respawn(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_foe *l_18;
    int l_14;

    if (a2->args[4].value == 0) return;
    if (((unsigned)(game_minutes - a2->last_minutes)) < a2->args[2].value) return;
    a2->last_minutes = game_minutes;
    l_18 = (struct qbn_foe *)a2->args[1].record;
    if (l_18->object->twin != 0) {
        if (l_18->object->twin->type != 34) return;
        object_delete(l_18->object->twin);
        if (D_00187CA8 != 0) world_collect_objects();
    }
    if ((rand() % 100) > a2->args[3].value) return;
    if (a2->args[4].value != (-1)) a2->args[4].value--;
    qaction_place_foe(a2, 0);
}

int qcond_op28_event_person(struct quest *a1, struct qbn_op *a2)
{
    if ((a2->args[1].object->type == 65 && a2->args[1].object->faction_id == (short)quest_event_object->data.person.faction_id) || ((int)quest_event_object->twin == (int)a2->args[1].object && a2->args[0].value != (-1))) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op70_player_has_items(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    for (l_20 = 2; l_20 < 5; l_20++) {
        if (a2->args[l_20].value == (-1)) continue;
        l_18 = a2->args[l_20].object;
        if (l_18 == 0 || l_18->twin == 0) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
        if ((int)l_18 == (-1768515946)) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
        l_18 = l_18->twin;
        if ((int)l_18 == (-1768515946)) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
        while (l_18 != 0 && l_18 != player_entity) l_18 = l_18->parent;
        if (l_18 != player_entity) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
    }
    quest_set_arg_state(a1, a2, 1, 1);
    return 1;
}

void func_000298F3(struct record *a1)
{
    int l_18;

    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] == a1) player_character->equipped[l_18] = 0;
    }
}
