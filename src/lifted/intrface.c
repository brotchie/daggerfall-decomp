/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern int D_000C5400;
extern short D_000CEA30;
extern short D_000CEA34;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int dungeon_water_level;
extern short D_00142940;
extern short D_00142944;
extern char D_00176A68[];
extern char D_00176A72[];
extern char D_00176A7C[];
extern int D_001788CF;
extern unsigned char player_environment;
extern short D_001789FD;
extern short D_001789FF;
extern short player_speed;
extern int D_001845C8;
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern int D_00187C86;
extern int D_00187C8A;
extern int D_00187C8E;
extern int D_00187C92;
extern int D_00187C96;
extern int D_00187C9A;
extern int D_00187C9E;
extern int D_00187CA4;
extern signed char D_00187CA8;
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D6;
extern unsigned char D_001940D7;
extern signed char D_001940DA;
extern signed char player_motion_flags;
extern int jump_velocity;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern int D_00195AB0;
extern int vertical_velocity;
extern struct record *spell_ready_missile;
extern struct character *player_character;
extern int cursor_arrow_image;
extern struct career *player_class;
extern int game_minutes;
extern char D_00195C44[];
extern struct record *D_00195C70;
extern int D_00195C74;
extern struct record *D_00195CB8;
extern int D_00195CD4;
extern int D_00195CD8;
extern int D_00195DB8;
extern char D_00195E30[];
extern char D_00195E4E[];
extern signed char mouse_control_mode;
extern signed char mouse_turn_rate;
extern signed char view_cursor_active;
extern short D_00195F44;
extern short D_00195F4E;
extern short D_00195F5A;
extern short mouse_motion_x;
extern short mouse_motion_y;
extern signed char D_00196272;
extern signed char player_on_ground;
extern signed char mouse_buttons_prev;
extern signed char in_dungeon_water;
extern signed char D_0019628E;
extern signed char D_00196296;
extern int D_00196D60;
extern char collide_flags[];
extern int D_001A5A60;
extern int D_001A5A64;
extern char D_001A5A68[];
extern int D_001A5ACC;
extern int D_001A5AD0;
extern int D_001A5ADC;
extern int D_001A5AE4;
extern int D_001A5AE8;
extern char turn_this_frame[];
extern int D_001A5AFC;
extern int D_001A5B00;
extern int D_001A5B04;
extern int move_angle_offset;
extern int D_001A5B0C;
extern int D_001A5B10;
extern int D_001A5B14;
extern int cursor_region_images;
extern short steer_row_y1;
extern short steer_row_y2;
extern short steer_col_x1;
extern short steer_col_x2;
extern short steer_weight_down;
extern short steer_weight_right;
extern short steer_region_width;
extern short steer_region_height;
extern short steer_weight_left;
extern short steer_weight_up;
extern short D_001A5B30;
extern short steer_key_region;
extern signed char D_001A5B34;

extern int engine_pick_object(int, int, int);
extern int collide_move_player(struct record *, int, int, int);
extern int damage_apply(struct record *, int, int);
extern int key_action_held(int);
extern int key_action_pressed(int);
extern int object_weight(struct record *);
extern int hud_update(void);
extern int func_000657B2(int);
extern int func_000696F9(int);
extern int sound_play(int, struct record *, int);
extern int func_00069AB8(int, struct record *, int);
extern int disk_read_file(int, int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int intrface_region_at(short, short, int, int);
extern int player_try_move(int);
extern int player_try_move_vertical(int);
extern int func_00082609(unsigned short);
extern int func_0008269B(int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern int func_000C7FD9();
extern int func_000CE6E2();
extern int func_0012B136();
extern int func_0012B45B();
extern int func_0012B47E();
extern int func_00144E84();
extern int func_00144FB4();
extern int func_0014BC00();
extern int func_00152D00();
extern void skill_add_uses(int, int);
extern void cast_fire_missile(int);
extern void links_trigger(int, int);
extern void sound_stop_channel(int);
extern void fatigue_add(int);
extern void func_00074024(int, int);
extern void func_0007EED8(void);
extern void cursor_draw(short);
extern void player_mouse_look(void);
extern void intrface_steer(short, int, int, int);
int intrface_key_region(void);
int func_000824C1(void);
int func_00082657(int);
void intrface_set_regions(void);
void func_0008066F(int);
void click_activate(int);
void object_apply_gravity(struct record *, struct character *);
void func_00080EA4(int);
void func_00080F2F(void);
void intrface_poll_controls(void);
void player_horse_sounds(int);

void intrface_init(void)
{
    short l_18;

    cursor_region_images = disk_read_file((int)D_00176A68, 0);
    cursor_arrow_image = disk_read_file((int)D_00176A72, 0);
    mc_memset(*(int *)D_00195C44, 0, 256, (int)D_00176A7C, 44, 4);
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 10; (*(int *)&l_18)++) {
        mc_memcpy((int)(*(char **)D_00195C44 + (((int)(short)l_18) << 4)), (int)(((char *)cursor_arrow_image) + (((int)(short)l_18) * 10)), 10, (int)D_00176A7C, 46, 4);
    }
    func_0012B45B(*(int *)D_00195C44, 0, 0);
    intrface_set_regions();
    D_00196272 = 0;
}

void intrface_set_regions(void)
{
    steer_region_width = ((int)(short)D_001789FD) / 3;
    steer_region_height = ((int)(short)D_001789FF) / 3;
    steer_row_y1 = (((int)(short)D_001789FF) / 3) + ((int)(short)D_00142944);
    steer_row_y2 = ((((int)(short)D_001789FF) / 3) * 2) + ((int)(short)D_00142944);
    steer_col_x1 = (((int)(short)D_001789FD) / 3) + ((int)(short)D_00142940);
    steer_col_x2 = ((((int)(short)D_001789FD) / 3) * 2) + ((int)(short)D_00142940);
}

void intrface_free(void)
{
    if (cursor_region_images != 0 && cursor_region_images != (-1751672937)) {
        mc_free(cursor_region_images, (int)D_00176A7C, 67);
        cursor_region_images = -1751672937;
    }
    if (cursor_arrow_image == 0 || cursor_arrow_image == (-1751672937)) return;
    mc_free(cursor_arrow_image, (int)D_00176A7C, 68);
    cursor_arrow_image = -1751672937;
}

void cursor_draw_arrow(void)
{
    func_00144E84((int)(short)mouse_x, (int)(short)mouse_y, 10, 10, (int)D_001A5A68, 0);
    D_001A5B14 = (int)(short)mouse_x;
    D_001A5ACC = (int)(short)mouse_y;
    func_00144FB4((int)(short)mouse_x, (int)(short)mouse_y, 10, 10, cursor_arrow_image);
    func_0012B47E((int)&mouse_motion_x, (int)&mouse_motion_y);
    mouse_motion_x = (mouse_motion_y = 0);
}

void func_00080637(void)
{
    func_00144FB4(D_001A5B14, D_001A5ACC, 10, 10, (int)D_001A5A68);
}

void func_0008066F(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    D_001940D4 |= 16;
    player_object->yaw &= ~0xF800;
    l_1C = player_object->yaw;
    func_000CE6E2(l_1C, a1 << 7, (int)&l_24, (int)&l_20);
    D_00187CA4 = a1;
    D_001A5B04 += a1;
    D_001A5B0C += l_24;
    D_001A5B10 += l_20;
}

void func_000806DD(int a1, int a2)
{
    short l_14;

    l_14 = player_object->yaw;
    player_object->yaw += a2;
    func_0008066F(a1);
    player_object->yaw = *(int *)&l_14;
}

void steer_forward_left(void)
{
    D_000C5400 = 8;
    *(int *)turn_this_frame = -(((D_00195AB0 * ((((int)(short)steer_weight_left) << 6) * D_001A5AFC)) / 1000) / 256);
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_up)) / 256;
}

void steer_forward(void)
{
    *(int *)turn_this_frame = 0;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_up)) / 220;
}

void steer_forward_right(void)
{
    D_000C5400 = -8;
    *(int *)turn_this_frame = ((D_00195AB0 * ((((int)(short)steer_weight_right) << 6) * D_001A5AFC)) / 1000) / 256;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_up)) / 256;
}

void steer_turn_left(void)
{
    D_000C5400 = 8;
    *(int *)turn_this_frame = -(((D_00195AB0 * ((((int)(short)steer_weight_left) << 6) * D_001A5AFC)) / 1000) / 256);
    player_speed = 0;
}

void steer_center(void)
{
    player_speed = 0;
    move_angle_offset = 0;
    *(int *)turn_this_frame = 0;
    if (((int)(unsigned char)(mouse_buttons & 1)) == 0) return;
    click_activate(0);
}

void steer_turn_right(void)
{
    D_000C5400 = -8;
    *(int *)turn_this_frame = ((((((int)(short)steer_weight_right) << 6) * D_001A5AFC) * D_00195AB0) / 1000) / 256;
    player_speed = 0;
}

void steer_slide_left(void)
{
    int l_1C;
    int l_18;

    D_000C5400 = 4;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_left)) / 256;
    move_angle_offset = -512;
}

void steer_backward(void)
{
    int l_1C;
    int l_18;

    move_angle_offset = 1024;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_down)) / 256;
}

void steer_slide_right(void)
{
    int l_1C;
    int l_18;

    D_000C5400 = -4;
    move_angle_offset = 512;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_right)) / 256;
}

void click_activate(int a1)
{
    {
        char l_2C[20];

        if (D_00187CA8 == 0) return;
        if (a1 == 0) {
            if (((int)(unsigned char)mouse_control_mode) == 1 && view_cursor_active != 0) {
                engine_pick_object((int)(short)mouse_x, (int)(short)mouse_y, (int)l_2C);
            } else {
                engine_pick_object(((int)(short)mouse_x) + 6, (int)&*(signed char *)((char *)((int)(short)mouse_y) + 6), (int)l_2C);
            }
        } else {
            mouse_x = D_000CEA30;
            mouse_y = D_000CEA34;
            engine_pick_object((int)(short)D_000CEA30, (int)(short)D_000CEA34, (int)l_2C);
        }
        if ((*(int *)l_2C & 1) != 0) {
            if (((int)(unsigned char)mouse_control_mode) == 1) {}
            func_00074024((int)l_2C, *(int *)((char *)l_2C + 4));
            return;
        }
        if ((int)spell_ready_missile == 0) return;
        cast_fire_missile((int)spell_ready_missile);
        spell_ready_missile = 0;
    }
}

int intrface_key_region(void)
{
    steer_weight_right = (steer_weight_down = (steer_weight_left = (steer_weight_up = 255)));
    if (key_action_held(3) != 0) return 6;
    if (key_action_held(2) != 0 && key_action_held(10) != 0) return 6;
    if (key_action_held(5) != 0) return 8;
    if (key_action_held(4) != 0 && key_action_held(10) != 0) return 8;
    if (key_action_held(0) != 0 && key_action_held(4) != 0) return 2;
    if (key_action_held(0) != 0 && key_action_held(2) != 0) return 0;
    if (key_action_held(0) != 0) return 1;
    if (key_action_held(1) != 0) return 7;
    if (key_action_held(4) != 0 && key_action_held(10) == 0) return 5;
    if (key_action_held(2) != 0 && key_action_held(10) == 0) return 3;
    return -1;
}

void object_apply_gravity(struct record *a1, struct character *a2)
{
    if (dungeon_water_level != 10000 && a2 == player_character) {
        if ((a1->y - 50) > dungeon_water_level) {
            if (in_dungeon_water == 0) sound_play(86, a1, 100);
            in_dungeon_water = 1;
            vertical_velocity = 0;
            return;
        }
    }
    in_dungeon_water = 0;
    if ((a2->conditions & 0x8) != 0) return;
    if (a2 == player_character && ((struct bf8_5_1 *)&player_motion_flags)->f != 0) {
        vertical_velocity = 0;
        return;
    }
    if ((a2->conditions & 0x4000) != 0) vertical_velocity = 15360;
    if ((a1->y - 80) <= D_00195C74 && vertical_velocity < 0) {
        vertical_velocity = 0;
        return;
    }
    vertical_velocity += (D_00195AB0 * 100352) / 1000;
    a1->y += ((vertical_velocity * D_00195AB0) / 1000) / 256;
}

void player_compute_jump_velocity(void)
{
    jump_velocity = ((player_character->attributes[6] + player_character->attributes[0]) / 2) + 50;
    jump_velocity += (player_character->skills[3].value * jump_velocity) / 100;
    jump_velocity = -jump_velocity;
    jump_velocity <<= 8;
}

void func_00080EA4(int a1)
{
    if (D_001A5B34 != 0) return;
    if (a1 < 0) {
        camera_object->angle_z -= 8;
        camera_object->angle_z &= ~0xF800;
        if (camera_object->angle_z < (a1 + 2047)) camera_object->angle_z = a1 + 2047;
        return;
    }
    camera_object->angle_z += 8;
    if (camera_object->angle_z <= a1) return;
    camera_object->angle_z = a1;
}

void func_00080F2F(void)
{
    if (camera_object->angle_z == 0) return;
    if (camera_object->angle_z < 1024) {
        camera_object->angle_z -= 8;
        if (camera_object->angle_z < 0) camera_object->angle_z = 0;
        return;
    }
    camera_object->angle_z += 8;
    if (camera_object->angle_z <= 2047) return;
    camera_object->angle_z = 0;
}

void func_00080FAB(int a1)
{
    player_motion_flags &= 191;
}

int func_00080FCD(int a1)
{
    int l_20;
    int l_1C;

    l_20 = (*(int *)((char *)a1) << 8) / func_0014BC00((*(int *)((char *)a1) * *(int *)((char *)a1)) + (*(int *)((char *)a1 + 8) * *(int *)((char *)a1 + 8)));
    if (*(int *)((char *)a1) > 0) {
        l_1C = (l_20 * 511) / 256;
        if (*(int *)((char *)a1 + 8) < 0) l_1C = (512 - l_1C) + 512;
    } else {
        l_1C = ((l_20 * 511) / 256) + 2047;
        if (*(int *)((char *)a1 + 8) < 0) l_1C = 1536 - (l_1C - 1536);
    }
    return l_1C & 2047;
}

void intrface_poll_controls(void)
{
    int l_1C;
    int l_18;

    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    func_0007EED8();
    func_00152D00();
    if (((int)(short)(steer_key_region = intrface_key_region())) != (-1)) {
        if (steer_key_region != 0 && ((int)(short)steer_key_region) != 2 && ((int)(short)steer_key_region) != 3 && ((int)(short)steer_key_region) != 5) {
            l_1C = 1;
        } else {
            l_1C = 0;
        }
        if (l_1C == 0) return;
    }
    l_18 = 1132;
    D_001A5AE8 = *(int *)((char *)l_18);
}

void player_movement_update(void)
{
    int l_68;
    int l_64;
    int l_60;
    int l_5C;
    int l_58;
    int l_54;
    int l_50;
    int l_4C;
    int l_48;
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;

    if (D_00195DB8 != 0) {
        D_00195DB8--;
        return;
    }
    D_001940D7 &= 223;
    D_001788CF = (((int)(unsigned char)(mouse_turn_rate & 127)) * 2) + 4;
    player_mouse_look();
    intrface_poll_controls();
    D_0019628E = 0;
    if (hud_update() != 0) return;
    l_4C = intrface_region_at((int)(short)mouse_x, (int)(short)mouse_y, (int)&l_48, (int)&l_44);
    cursor_draw((int)(short)*(short *)&l_4C);
    if (D_00196272 != 0) return;
    if (key_action_held(22) != 0 || (((struct bf8_6_1 *)&D_001940DA)->f != 0 && ((int)(unsigned char)(mouse_buttons & 1)) != 0)) {
        return;
    }
    D_00195CD8 = (D_00195CD4 = 0);
    l_30 = player_object->x;
    l_28 = player_object->z;
    l_38 = vertical_velocity;
    if (((int)(short)steer_key_region) != (-1)) {
        l_4C = (int)(short)steer_key_region;
    }
    if ((player_character->conditions & 0x8) != 0) player_on_ground = 1;
    if (player_on_ground == 0 && in_dungeon_water == 0 && ((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
        l_60 = 1;
    } else {
        l_60 = 0;
    }
    l_5C = l_60;
    l_24 = l_5C;
    if (l_5C != 0) {
        *(int *)turn_this_frame = 0;
        l_40 = D_00187CA4;
    } else {
        intrface_steer((int)(short)D_00195F4E, l_4C, l_48, l_44);
        l_40 = (int)(short)player_speed;
        D_00187CA4 = l_40;
    }
    if (((struct bf8_1_1 *)&D_001940D5)->f != 0) {
        D_001940D5 &= 253;
        return;
    }
    if (*(int *)turn_this_frame != 0) {
        func_00080EA4(*(int *)turn_this_frame / 4);
        camera_object->yaw += *(short *)turn_this_frame;
        player_object->yaw = camera_object->yaw;
        *(int *)turn_this_frame = 0;
    } else {
        func_00080F2F();
    }
    if (((struct bf8_2_1 *)&player_motion_flags)->f != 0) {
        l_64 = 5;
    } else {
        l_64 = 10;
    }
    l_54 = l_64;
    l_58 = 0;
    if (l_5C == 0 && ((int)(unsigned char)mouse_control_mode) == 1 && key_action_held(23) != 0) {
        D_00187CA4 >>= 1;
        D_00187CA4--;
        l_40 >>= 1;
    }
    while (l_58 == 0 && l_40 > 0) {
        if (l_40 < l_54) {
            l_50 = l_40;
            l_40 = 0;
        } else {
            l_40 -= l_54;
            l_50 = l_54;
        }
        l_58 = player_try_move(l_50);
        if (((int)(short)(*(short *)collide_flags & 16)) == 0 && ((int)(short)(*(short *)collide_flags & 1)) != 0) {
            links_trigger((int)D_00195CB8, 1);
        }
        if (((int)(short)(*(short *)collide_flags & 2)) != 0) links_trigger((int)D_00195C70, 3);
    }
    if (l_24 != 0 && player_on_ground != 0) l_24 = 1;
    if (l_40 == 0 && (int)D_00195CB8 != 0 && func_000657B2((int)D_00195CB8) != 0) {
        player_try_move(0);
    }
    l_44 = 0;
    if (((int)(short)(*(short *)collide_flags & 2)) != 0 && D_001A5B30 == 0 && func_00082609((int)(unsigned short)D_00195F5A) != 0 && func_000C7FD9(player_object->x, player_object->z, l_30, l_28) < 5 && (player_character->conditions & 0x8) == 0 && ((int)(unsigned short)(player_character->flags & 1536)) == 0) {
        if (((unsigned)(*(int *)((char *)1132) - D_001A5AE4)) > 14) {
            if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
                hud_message_add(D_001845C8);
                skill_add_uses(18, 1);
            }
            player_motion_flags |= 32;
            l_44 = (-((int)(short)player_speed)) / 3;
            if (((unsigned)(game_minutes - D_001A5B00)) > 1) {
                skill_add_uses(18, 1);
                D_001A5B00 = game_minutes;
                l_20 = player_character->skills[18].value;
                if (player_character->race == 6) l_20 += 30;
                if ((player_character->conditions & 0x20000) == 0 && rand_range(1, 100) > 95 && rand_range(1, 100) > l_20) {
                    player_motion_flags &= 223;
                    D_001A5B30 = 1;
                }
            }
        }
    } else {
        if (((struct bf8_5_1 *)&player_motion_flags)->f != 0 && D_001A5B30 == 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
            if (func_000824C1() == 0) player_motion_flags &= 223;
        }
        D_001A5AE4 = *(int *)((char *)1132);
        D_001A5B00 = game_minutes;
    }
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0 || (player_character->conditions & 0x8) != 0) {
        vertical_velocity = 0;
    }
    if ((player_object->y - 50) <= dungeon_water_level) {
        in_dungeon_water = 0;
    }
    D_00195F44 = 0;
    if (((((int)(unsigned char)(mouse_buttons & 3)) == 3 && ((int)(unsigned char)(mouse_buttons_prev & 3)) == 1) || key_action_held(8) != 0) && player_on_ground != 0 && (player_character->conditions & 0x1) == 0 && ((int)(unsigned short)(player_character->flags & 1536)) == 0) {
        skill_add_uses(3, 1);
        if ((player_character->conditions & 0x10000) != 0) {
            l_3C = jump_velocity * 2;
        } else {
            l_3C = jump_velocity;
        }
        if (((int)(unsigned short)(player_class->flags & 2)) != 0) {
            l_3C = (int)(((char *)jump_velocity) + (jump_velocity >> 1));
        }
        l_44 = -1;
        vertical_velocity += l_3C;
        D_00195F44 = 1;
        fatigue_add(-11);
        l_5C = 1;
        player_on_ground = 0;
        D_00196296 = 1;
    }
    if (l_24 != 0 && player_on_ground != 0) l_24 = 1;
    if (key_action_held(6) != 0 && ((player_character->conditions & 0x8) != 0 || in_dungeon_water != 0) && (player_object->y - 100) > D_00195C74) {
        if (((int)player_environment) != 1 || (D_00196D60 - 1024) <= player_object->y) {
            l_44 += (D_00195AB0 * (-80)) / 1000;
        }
    } else if (key_action_held(7) != 0 && ((player_character->conditions & 0x8) != 0 || in_dungeon_water != 0)) {
        l_44 += (D_00195AB0 * 80) / 1000;
    }
    if (in_dungeon_water != 0 && object_weight(player_entity) > 250 && key_action_held(7) == 0 && (player_character->conditions & 1048584) == 0) {
        l_44 += (D_00195AB0 * 80) / 1000;
    }
    l_2C = player_object->y;
    if (player_on_ground == 0 && in_dungeon_water == 0 && ((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
        l_68 = 1;
    } else {
        l_68 = 0;
    }
    l_5C = l_68;
    l_24 = l_5C;
    if (((int)(short)(*(short *)collide_flags & 16)) != 0 || l_5C != 0 || (dungeon_water_level != 10000 && (player_object->y - 50) > dungeon_water_level)) {
        l_2C = player_object->y;
        object_apply_gravity(player_object, player_character);
        l_44 += player_object->y - l_2C;
        player_object->y = l_2C;
    } else {
        D_001A5B30 = 0;
        vertical_velocity = 0;
    }
    if (l_24 != 0 && player_on_ground != 0) l_24 = 1;
    player_try_move_vertical(l_44);
    if (player_on_ground != 0) vertical_velocity = 0;
    if (vertical_velocity < 0 && D_00195F44 != 0 && (player_object->y - 99) < D_00195C74) {
        player_object->y = l_2C;
    }
    if (l_24 != 0 && player_on_ground != 0) l_24 = 1;
    if (D_00195CD8 != 0) {
        D_001A5B30 = 0;
        if (player_on_ground != 0) vertical_velocity = 0;
        mc_memcpy((int)D_00195E30, D_00195CD8, 30, (int)D_00176A7C, 695, 4);
        D_00195CD8 = (int)D_00195E30;
    }
    if (D_00195CD4 != 0) {
        mc_memcpy((int)D_00195E4E, D_00195CD4, 30, (int)D_00176A7C, 701, 4);
        D_00195CD4 = (int)D_00195E4E;
    }
    if (vertical_velocity == 0 && l_38 != 0) {
        l_34 = ((l_38 / 256) / 40) - 7;
        if (l_34 > 0) {
            l_34 = l_34 * l_34;
            l_34 = l_34 / 2;
            damage_apply(player_entity, l_34, 0);
            sound_play(((((int)player_environment) != 3) ? 360 : 359), player_entity, 100);
        }
    }
    if (((int)(unsigned char)mouse_control_mode) == 1 && D_00196272 == 0) {
        if (view_cursor_active != 0 && ((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 1)) == 0) {
            click_activate(0);
        }
        if (key_action_pressed(18) != 0) click_activate(1);
    } else if (D_00196272 == 0) {
        if (((int)(unsigned char)(mouse_buttons & 2)) != 0 && ((struct bf8_6_1 *)&D_001940D6)->f == 0) {
            click_activate(0);
        }
    }
    if (l_24 != 0 && player_on_ground != 0) l_24 = 1;
    player_horse_sounds(D_00187CA4);
    if (dungeon_water_level == 10000 || (player_object->y - 76) <= dungeon_water_level || rand() >= 100) {
        return;
    }
    sound_play(384, player_object, 100);
}

void player_horse_sounds(int a1)
{
    int l_18;

    if (((int)(unsigned short)(player_character->flags & 1536)) == 0) return;
    l_18 = (((((int)(short)D_00195F4E) - (((int)(short)D_00195F4E) >> 2)) < a1) ? 366 : 365);
    if (((int)(unsigned short)(player_character->flags & 1024)) != 0) l_18 = 372;
    if (a1 == 0) {
        if (D_001A5AD0 == (-1)) return;
        if (func_000696F9(D_001A5AD0) != 0) return;
        sound_stop_channel(D_001A5AD0);
        D_001A5AD0 = -1;
        return;
    }
    if (D_001A5AD0 == (-1)) {
        D_001A5AD0 = func_00069AB8(l_18, player_object, 100);
    } else if (l_18 != D_001A5ADC) {
        sound_stop_channel(D_001A5AD0);
        D_001A5AD0 = func_00069AB8(l_18, player_object, 100);
    }
    D_001A5ADC = l_18;
}

void player_horse_sounds_stop(void)
{
    if (func_000696F9((D_001A5AD0 = -1)) != 0) return;
    sound_stop_channel(D_001A5AD0);
}

int func_000824C1(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    func_000CE6E2((player_object->yaw + move_angle_offset) & 2047, 1024, (int)&l_20, (int)&l_1C);
    l_20 += player_object->x << 5;
    l_1C += player_object->z << 5;
    l_20 += D_001A5A64;
    l_1C += D_001A5A60;
    D_001A5A64 = l_20 & 31;
    D_001A5A60 = l_1C & 31;
    D_00187C86 = l_20 / 32;
    D_00187C8A = player_object->y - 32;
    D_00187C8E = l_1C / 32;
    D_00187C92 = player_object->angle_x;
    D_00187C96 = player_object->yaw;
    D_00187C9A = player_object->angle_z;
    if (((struct bf8_2_1 *)&player_motion_flags)->f != 0) {
        l_28 = (int)D_00187C12;
    } else {
        l_28 = (int)D_00187B6E;
    }
    D_00187C9E = l_28;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) D_00187C9E = (int)D_00187BB8;
    *(signed char *)collide_flags |= 4;
    player_motion_flags &= 223;
    l_24 = collide_move_player(player_object, 0, (int)&D_00187C86, 1);
    return l_24;
}

int func_00082657(int a1)
{
    int l_20;
    int l_1C;

    l_20 = a1;
    l_1C = 0;
    while (*(signed char *)((char *)l_20) != 0) l_1C = l_20++;
    return l_1C;
}

int func_00082750(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_1C = 1;
    l_24 = func_00082657(a1);
    if (l_24 == 0) return -1;
    l_20 = func_0008269B(l_24);
    while (l_24 != a1) {
        l_24--;
        l_1C <<= 4;
        l_20 += func_0008269B(l_24) * l_1C;
    }
    return l_20;
}
