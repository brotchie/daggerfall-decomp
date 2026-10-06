/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#include "doslow.h"

extern int xn_snow_turn_shift;
extern short xn_cam_centre_x;
extern short xn_cam_centre_y;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int dungeon_water_level;
extern short xn_gfx_clip_left;
extern short xn_gfx_clip_top;
extern char D_00176A68[];
extern char D_00176A72[];
extern char D_00176A7C[];
extern int steer_turn_speed_max;
extern unsigned char player_environment;
extern short D_001789FD;
extern short D_001789FF;
extern short player_speed;
extern iptr D_001845C8;
extern struct collide_probe D_00187B6E;
extern struct collide_probe D_00187BB8;
extern struct collide_probe D_00187C12;
extern struct move_request D_00187C86;   /* the player's move */
extern int player_momentum;
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
extern int frame_ticks;
extern int vertical_velocity;
extern struct record *spell_ready_missile;
extern struct character *player_character;
extern iptr cursor_arrow_image;
extern struct career *player_class;
extern int game_minutes;
extern char *scratch_buffer;
extern struct record *D_00195C70;
extern int ceiling_height;
extern struct record *D_00195CB8;
extern iptr D_00195CD4;
extern iptr D_00195CD8;
extern int D_00195DB8;
extern char D_00195E30[];
extern char D_00195E4E[];
extern signed char mouse_control_mode;
extern signed char mouse_turn_rate;
extern signed char view_cursor_active;
extern short D_00195F44;
extern short player_base_speed;
extern short D_00195F5A;
extern short mouse_motion_x;
extern short mouse_motion_y;
extern signed char D_00196272;
extern signed char player_on_ground;
extern signed char mouse_buttons_prev;
extern signed char in_dungeon_water;
extern signed char D_0019628E;
extern signed char D_00196296;
extern int collide_height;
extern char collide_flags[];
extern int D_001A5A60;
extern int D_001A5A64;
extern char cursor_saved_background[];
extern int cursor_saved_y;
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
extern int cursor_saved_x;
extern iptr cursor_region_images;
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

extern int engine_pick_object(int, int, struct pick_result *);
extern int collide_move_player(struct record *, int, struct move_request *, int);
extern int damage_apply(struct record *, int, struct record *);
extern int key_action_held(int);
extern int key_action_pressed(int);
extern int object_weight(struct record *);
extern int hud_update(void);
extern iptr links_object_motion(iptr);
extern int sound_channel_done(int);
extern int sound_play(int, struct record *, int);
extern int sound_play_loop(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern iptr hud_message_add(iptr);
extern int rand_range(int, int);
extern int intrface_region_at(int, int, int *, int *);
extern int player_try_move(int);
extern int player_try_move_vertical(int);
extern int climb_angle_ok(int);
extern int hex_digit_value(char *);
extern int xn_math_approx_dist2d(int, int, int, int);
extern void xn_math_yaw_offset_xz(int, int, int *, int *);
extern void xn_mouse_poll_clamped(void);
extern void xn_mouse_set_cursor_image(char *, int, int);
extern void xn_mouse_read_motion(short *, short *);
extern void xn_draw_get_rect(int, int, int, int, char *, int);
extern void xn_draw_image_transparent(int, int, int, int, char *);
extern int xn_math_isqrt(int);
extern void xn_joy_poll(void);
extern void skill_add_uses(int, int);
extern void cast_fire_missile(struct record *);
extern void links_trigger(struct record *, int);
extern void sound_stop_channel(int);
extern void fatigue_add(int);
extern void click_world_object(struct pick_result *, struct record *);
extern void func_0007EED8(void);
extern void cursor_draw(short);
extern void player_mouse_look(void);
extern void intrface_steer(int, int, int, int);
int intrface_key_region(void);
int player_climb_probe(void);
char *string_last_char(char *);
void intrface_set_regions(void);
void func_0008066F(int);
void click_activate(int);
void object_apply_gravity(struct record *, struct character *);
void camera_roll_turn(int);
void camera_roll_recover(void);
void intrface_poll_controls(void);
void player_horse_sounds(int);

void intrface_init(void)
{
    slot16 i;

    cursor_region_images = disk_read_file(D_00176A68, 0);
    cursor_arrow_image = disk_read_file(D_00176A72, 0);
    mc_memset((void *)scratch_buffer, 0, 256, D_00176A7C, 44, 4);
    *(int *)&i = 0;
    for (; ((int)(short)i) < 10; (*(int *)&i)++) {
        mc_memcpy((scratch_buffer + (((int)(short)i) << 4)), (((char *)cursor_arrow_image) + (((int)(short)i) * 10)), 10, D_00176A7C, 46, 4);
    }
    xn_mouse_set_cursor_image(scratch_buffer, 0, 0);
    intrface_set_regions();
    D_00196272 = 0;
}

void intrface_set_regions(void)
{
    steer_region_width = ((int)(short)D_001789FD) / 3;
    steer_region_height = ((int)(short)D_001789FF) / 3;
    steer_row_y1 = (((int)(short)D_001789FF) / 3) + ((int)(short)xn_gfx_clip_top);
    steer_row_y2 = ((((int)(short)D_001789FF) / 3) * 2) + ((int)(short)xn_gfx_clip_top);
    steer_col_x1 = (((int)(short)D_001789FD) / 3) + ((int)(short)xn_gfx_clip_left);
    steer_col_x2 = ((((int)(short)D_001789FD) / 3) * 2) + ((int)(short)xn_gfx_clip_left);
}

void intrface_free(void)
{
    if (cursor_region_images != 0 && cursor_region_images != (-1751672937)) {
        mc_free((void *)cursor_region_images, D_00176A7C, 67);
        cursor_region_images = -1751672937;
    }
    if (cursor_arrow_image == 0 || cursor_arrow_image == (-1751672937)) return;
    mc_free((void *)cursor_arrow_image, D_00176A7C, 68);
    cursor_arrow_image = -1751672937;
}

void cursor_draw_arrow(void)
{
    xn_draw_get_rect((int)(short)mouse_x, (int)(short)mouse_y, 10, 10, cursor_saved_background, 0);
    cursor_saved_x = (int)(short)mouse_x;
    cursor_saved_y = (int)(short)mouse_y;
    xn_draw_image_transparent((int)(short)mouse_x, (int)(short)mouse_y, 10, 10, (char *)cursor_arrow_image);
    xn_mouse_read_motion(&mouse_motion_x, &mouse_motion_y);
    mouse_motion_x = (mouse_motion_y = 0);
}

void cursor_restore_background(void)
{
    xn_draw_image_transparent(cursor_saved_x, cursor_saved_y, 10, 10, cursor_saved_background);
}

void func_0008066F(int distance)
{
    int dx;
    int dz;
    int yaw;
    int unused;

    D_001940D4 |= 16;
    player_object->yaw &= ~0xF800;
    yaw = player_object->yaw;
    xn_math_yaw_offset_xz(yaw, distance << 7, &dx, &dz);
    player_momentum = distance;
    D_001A5B04 += distance;
    D_001A5B0C += dx;
    D_001A5B10 += dz;
}

void func_000806DD(int distance, int yaw_offset)
{
    slot16 saved_yaw;

    saved_yaw = player_object->yaw;
    player_object->yaw += yaw_offset;
    func_0008066F(distance);
    player_object->yaw = *(int *)&saved_yaw;
}

void steer_forward_left(void)
{
    xn_snow_turn_shift = 8;
    *(int *)turn_this_frame = -(((frame_ticks * ((((int)(short)steer_weight_left) << 6) * D_001A5AFC)) / 1000) / 256);
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_up)) / 256;
}

void steer_forward(void)
{
    *(int *)turn_this_frame = 0;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_up)) / 220;
}

void steer_forward_right(void)
{
    xn_snow_turn_shift = -8;
    *(int *)turn_this_frame = ((frame_ticks * ((((int)(short)steer_weight_right) << 6) * D_001A5AFC)) / 1000) / 256;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_up)) / 256;
}

void steer_turn_left(void)
{
    xn_snow_turn_shift = 8;
    *(int *)turn_this_frame = -(((frame_ticks * ((((int)(short)steer_weight_left) << 6) * D_001A5AFC)) / 1000) / 256);
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
    xn_snow_turn_shift = -8;
    *(int *)turn_this_frame = ((((((int)(short)steer_weight_right) << 6) * D_001A5AFC) * frame_ticks) / 1000) / 256;
    player_speed = 0;
}

void steer_slide_left(void)
{
    int unused;
    int unused2;

    xn_snow_turn_shift = 4;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_left)) / 256;
    move_angle_offset = -512;
}

void steer_backward(void)
{
    int unused;
    int unused2;

    move_angle_offset = 1024;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_down)) / 256;
}

void steer_slide_right(void)
{
    int unused;
    int unused2;

    xn_snow_turn_shift = -4;
    move_angle_offset = 512;
    player_speed = (((int)(short)player_speed) * ((int)(short)steer_weight_right)) / 256;
}

void click_activate(int at_view_centre)
{
    {
        struct pick_result pick;

        if (D_00187CA8 == 0) return;
        if (at_view_centre == 0) {
            if (((int)(unsigned char)mouse_control_mode) == 1 && view_cursor_active != 0) {
                engine_pick_object((int)(short)mouse_x, (int)(short)mouse_y, &pick);
            } else {
                engine_pick_object(((int)(short)mouse_x) + 6, (int)(iptr)&*(signed char *)((char *)(iptr)((int)(short)mouse_y) + 6), &pick);
            }
        } else {
            mouse_x = xn_cam_centre_x;
            mouse_y = xn_cam_centre_y;
            engine_pick_object((int)(short)xn_cam_centre_x, (int)(short)xn_cam_centre_y, &pick);
        }
        if ((pick.flags & 1) != 0) {
            if (((int)(unsigned char)mouse_control_mode) == 1) {}
            click_world_object(&pick, pick.object);
            return;
        }
        if ((iptr)spell_ready_missile == 0) return;
        cast_fire_missile(spell_ready_missile);
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

void object_apply_gravity(struct record *object, struct character *character)
{
    if (dungeon_water_level != 10000 && character == player_character) {
        if ((object->y - 50) > dungeon_water_level) {
            if (in_dungeon_water == 0) sound_play(86, object, 100);
            in_dungeon_water = 1;
            vertical_velocity = 0;
            return;
        }
    }
    in_dungeon_water = 0;
    if ((character->conditions & 0x8) != 0) return;
    if (character == player_character && ((struct bf8_5_1 *)&player_motion_flags)->f != 0) {
        vertical_velocity = 0;
        return;
    }
    if ((character->conditions & 0x4000) != 0) vertical_velocity = 15360;
    if ((object->y - 80) <= ceiling_height && vertical_velocity < 0) {
        vertical_velocity = 0;
        return;
    }
    vertical_velocity += (frame_ticks * 100352) / 1000;
    object->y += ((vertical_velocity * frame_ticks) / 1000) / 256;
}

void player_compute_jump_velocity(void)
{
    jump_velocity = ((player_character->attributes[6] + player_character->attributes[0]) / 2) + 50;
    jump_velocity += (player_character->skills[3].value * jump_velocity) / 100;
    jump_velocity = -jump_velocity;
    jump_velocity <<= 8;
}

void camera_roll_turn(int target_roll)
{
    if (D_001A5B34 != 0) return;
    if (target_roll < 0) {
        camera_object->angle_z -= 8;
        camera_object->angle_z &= ~0xF800;
        if (camera_object->angle_z < (target_roll + 2047)) camera_object->angle_z = target_roll + 2047;
        return;
    }
    camera_object->angle_z += 8;
    if (camera_object->angle_z <= target_roll) return;
    camera_object->angle_z = target_roll;
}

void camera_roll_recover(void)
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

void player_motion_clear_bit6(int unused)
{
    player_motion_flags &= 191;
}

int vector_yaw(int *vector)
{
    int sine;
    int yaw;

    sine = (vector[0] << 8) / xn_math_isqrt((vector[0] * vector[0]) + (vector[2] * vector[2]));
    if (vector[0] > 0) {
        yaw = (sine * 511) / 256;
        if (vector[2] < 0) yaw = (512 - yaw) + 512;
    } else {
        yaw = ((sine * 511) / 256) + 2047;
        if (vector[2] < 0) yaw = 1536 - (yaw - 1536);
    }
    return yaw & 2047;
}

void intrface_poll_controls(void)
{
    int not_turning;
    int *bios_ticks;

    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    func_0007EED8();
    xn_joy_poll();
    if (((int)(short)(steer_key_region = intrface_key_region())) != (-1)) {
        if (steer_key_region != 0 && ((int)(short)steer_key_region) != 2 && ((int)(short)steer_key_region) != 3 && ((int)(short)steer_key_region) != 5) {
            not_turning = 1;
        } else {
            not_turning = 0;
        }
        if (not_turning == 0) return;
    }
    bios_ticks = (int *)DOS_LOW(0x46C);
    D_001A5AE8 = *bios_ticks;
}

void player_movement_update(void)
{
    int airborne_test2;
    int step_size_test;
    int airborne_test;
    int airborne;
    int blocked;
    int step_size;
    int step;
    int region;
    int region_x;
    int dy;
    int distance;
    int jump_impulse;
    int prev_velocity;
    int fall_damage;
    int start_x;
    int saved_y;
    int start_z;
    int landing_check;
    int climb_chance;

    if (D_00195DB8 != 0) {
        D_00195DB8--;
        return;
    }
    D_001940D7 &= 223;
    steer_turn_speed_max = (((int)(unsigned char)(mouse_turn_rate & 127)) * 2) + 4;
    player_mouse_look();
    intrface_poll_controls();
    D_0019628E = 0;
    if (hud_update() != 0) return;
    region = intrface_region_at((int)(short)mouse_x, (int)(short)mouse_y, &region_x, &dy);
    cursor_draw((int)(short)*(short *)&region);
    if (D_00196272 != 0) return;
    if (key_action_held(22) != 0 || (((struct bf8_6_1 *)&D_001940DA)->f != 0 && ((int)(unsigned char)(mouse_buttons & 1)) != 0)) {
        return;
    }
    D_00195CD8 = (D_00195CD4 = 0);
    start_x = player_object->x;
    start_z = player_object->z;
    prev_velocity = vertical_velocity;
    if (((int)(short)steer_key_region) != (-1)) {
        region = (int)(short)steer_key_region;
    }
    if ((player_character->conditions & 0x8) != 0) player_on_ground = 1;
    if (player_on_ground == 0 && in_dungeon_water == 0 && ((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
        airborne_test = 1;
    } else {
        airborne_test = 0;
    }
    airborne = airborne_test;
    landing_check = airborne;
    if (airborne != 0) {
        *(int *)turn_this_frame = 0;
        distance = player_momentum;
    } else {
        intrface_steer((int)(short)player_base_speed, region, region_x, dy);
        distance = (int)(short)player_speed;
        player_momentum = distance;
    }
    if (((struct bf8_1_1 *)&D_001940D5)->f != 0) {
        D_001940D5 &= 253;
        return;
    }
    if (*(int *)turn_this_frame != 0) {
        camera_roll_turn(*(int *)turn_this_frame / 4);
        camera_object->yaw += *(short *)turn_this_frame;
        player_object->yaw = camera_object->yaw;
        *(int *)turn_this_frame = 0;
    } else {
        camera_roll_recover();
    }
    if (((struct bf8_2_1 *)&player_motion_flags)->f != 0) {
        step_size_test = 5;
    } else {
        step_size_test = 10;
    }
    step_size = step_size_test;
    blocked = 0;
    if (airborne == 0 && ((int)(unsigned char)mouse_control_mode) == 1 && key_action_held(23) != 0) {
        player_momentum >>= 1;
        player_momentum--;
        distance >>= 1;
    }
    while (blocked == 0 && distance > 0) {
        if (distance < step_size) {
            step = distance;
            distance = 0;
        } else {
            distance -= step_size;
            step = step_size;
        }
        blocked = player_try_move(step);
        if (((int)(short)(*(short *)collide_flags & 16)) == 0 && ((int)(short)(*(short *)collide_flags & 1)) != 0) {
            links_trigger(D_00195CB8, 1);
        }
        if (((int)(short)(*(short *)collide_flags & 2)) != 0) links_trigger(D_00195C70, 3);
    }
    if (landing_check != 0 && player_on_ground != 0) landing_check = 1;
    if (distance == 0 && (iptr)D_00195CB8 != 0 && links_object_motion((iptr)D_00195CB8) != 0) {
        player_try_move(0);
    }
    dy = 0;
    if (((int)(short)(*(short *)collide_flags & 2)) != 0 && D_001A5B30 == 0 && climb_angle_ok((int)(unsigned short)D_00195F5A) != 0 && xn_math_approx_dist2d(player_object->x, player_object->z, start_x, start_z) < 5 && (player_character->conditions & 0x8) == 0 && ((int)(unsigned short)(player_character->flags & 1536)) == 0) {
        if (((unsigned)(BIOS_TICKS - D_001A5AE4)) > 14) {
            if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
                hud_message_add(D_001845C8);
                skill_add_uses(18, 1);
            }
            player_motion_flags |= 32;
            dy = (-((int)(short)player_speed)) / 3;
            if (((unsigned)(game_minutes - D_001A5B00)) > 1) {
                skill_add_uses(18, 1);
                D_001A5B00 = game_minutes;
                climb_chance = player_character->skills[18].value;
                if (player_character->race == 6) climb_chance += 30;
                if ((player_character->conditions & 0x20000) == 0 && rand_range(1, 100) > 95 && rand_range(1, 100) > climb_chance) {
                    player_motion_flags &= 223;
                    D_001A5B30 = 1;
                }
            }
        }
    } else {
        if (((struct bf8_5_1 *)&player_motion_flags)->f != 0 && D_001A5B30 == 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
            if (player_climb_probe() == 0) player_motion_flags &= 223;
        }
        D_001A5AE4 = BIOS_TICKS;
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
            jump_impulse = jump_velocity * 2;
        } else {
            jump_impulse = jump_velocity;
        }
        if (((int)(unsigned short)(player_class->flags & 2)) != 0) {
            jump_impulse = (int)(iptr)(((char *)(iptr)jump_velocity) + (jump_velocity >> 1));
        }
        dy = -1;
        vertical_velocity += jump_impulse;
        D_00195F44 = 1;
        fatigue_add(-11);
        airborne = 1;
        player_on_ground = 0;
        D_00196296 = 1;
    }
    if (landing_check != 0 && player_on_ground != 0) landing_check = 1;
    if (key_action_held(6) != 0 && ((player_character->conditions & 0x8) != 0 || in_dungeon_water != 0) && (player_object->y - 100) > ceiling_height) {
        if (((int)player_environment) != 1 || (collide_height - 1024) <= player_object->y) {
            dy += (frame_ticks * (-80)) / 1000;
        }
    } else if (key_action_held(7) != 0 && ((player_character->conditions & 0x8) != 0 || in_dungeon_water != 0)) {
        dy += (frame_ticks * 80) / 1000;
    }
    if (in_dungeon_water != 0 && object_weight(player_entity) > 250 && key_action_held(7) == 0 && (player_character->conditions & 1048584) == 0) {
        dy += (frame_ticks * 80) / 1000;
    }
    saved_y = player_object->y;
    if (player_on_ground == 0 && in_dungeon_water == 0 && ((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
        airborne_test2 = 1;
    } else {
        airborne_test2 = 0;
    }
    airborne = airborne_test2;
    landing_check = airborne;
    if (((int)(short)(*(short *)collide_flags & 16)) != 0 || airborne != 0 || (dungeon_water_level != 10000 && (player_object->y - 50) > dungeon_water_level)) {
        saved_y = player_object->y;
        object_apply_gravity(player_object, player_character);
        dy += player_object->y - saved_y;
        player_object->y = saved_y;
    } else {
        D_001A5B30 = 0;
        vertical_velocity = 0;
    }
    if (landing_check != 0 && player_on_ground != 0) landing_check = 1;
    player_try_move_vertical(dy);
    if (player_on_ground != 0) vertical_velocity = 0;
    if (vertical_velocity < 0 && D_00195F44 != 0 && (player_object->y - 99) < ceiling_height) {
        player_object->y = saved_y;
    }
    if (landing_check != 0 && player_on_ground != 0) landing_check = 1;
    if (D_00195CD8 != 0) {
        D_001A5B30 = 0;
        if (player_on_ground != 0) vertical_velocity = 0;
        mc_memcpy(D_00195E30, (void *)D_00195CD8, 30, D_00176A7C, 695, 4);
        D_00195CD8 = (iptr)D_00195E30;
    }
    if (D_00195CD4 != 0) {
        mc_memcpy(D_00195E4E, (void *)D_00195CD4, 30, D_00176A7C, 701, 4);
        D_00195CD4 = (iptr)D_00195E4E;
    }
    if (vertical_velocity == 0 && prev_velocity != 0) {
        fall_damage = ((prev_velocity / 256) / 40) - 7;
        if (fall_damage > 0) {
            fall_damage = fall_damage * fall_damage;
            fall_damage = fall_damage / 2;
            damage_apply(player_entity, fall_damage, 0);
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
    if (landing_check != 0 && player_on_ground != 0) landing_check = 1;
    player_horse_sounds(player_momentum);
    if (dungeon_water_level == 10000 || (player_object->y - 76) <= dungeon_water_level || rand() >= 100) {
        return;
    }
    sound_play(384, player_object, 100);
}

void player_horse_sounds(int speed)
{
    int sound;

    if (((int)(unsigned short)(player_character->flags & 1536)) == 0) return;
    sound = (((((int)(short)player_base_speed) - (((int)(short)player_base_speed) >> 2)) < speed) ? 366 : 365);
    if (((int)(unsigned short)(player_character->flags & 1024)) != 0) sound = 372;
    if (speed == 0) {
        if (D_001A5AD0 == (-1)) return;
        if (sound_channel_done(D_001A5AD0) != 0) return;
        sound_stop_channel(D_001A5AD0);
        D_001A5AD0 = -1;
        return;
    }
    if (D_001A5AD0 == (-1)) {
        D_001A5AD0 = sound_play_loop(sound, player_object, 100);
    } else if (sound != D_001A5ADC) {
        sound_stop_channel(D_001A5AD0);
        D_001A5AD0 = sound_play_loop(sound, player_object, 100);
    }
    D_001A5ADC = sound;
}

void player_horse_sounds_stop(void)
{
    if (sound_channel_done((D_001A5AD0 = -1)) != 0) return;
    sound_stop_channel(D_001A5AD0);
}

int player_climb_probe(void)
{
    iptr shape;
    int result;
    int x;
    int z;

    xn_math_yaw_offset_xz((player_object->yaw + move_angle_offset) & 2047, 1024, &x, &z);
    x += player_object->x << 5;
    z += player_object->z << 5;
    x += D_001A5A64;
    z += D_001A5A60;
    D_001A5A64 = x & 31;
    D_001A5A60 = z & 31;
    D_00187C86.x = x / 32;
    D_00187C86.y = player_object->y - 32;
    D_00187C86.z = z / 32;
    D_00187C86.angle_x = player_object->angle_x;
    D_00187C86.yaw = player_object->yaw;
    D_00187C86.angle_z = player_object->angle_z;
    if (((struct bf8_2_1 *)&player_motion_flags)->f != 0) {
        shape = (iptr)&D_00187C12;
    } else {
        shape = (iptr)&D_00187B6E;
    }
    D_00187C86.probe = (struct collide_probe *)shape;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) D_00187C86.probe = &D_00187BB8;
    *(signed char *)collide_flags |= 4;
    player_motion_flags &= 223;
    result = collide_move_player(player_object, 0, &D_00187C86, 1);
    return result;
}

char *string_last_char(char *text)
{
    char *p;
    char *last;

    p = text;
    last = 0;
    while (*p != 0) last = p++;
    return last;
}

int parse_hex_string(char *text)
{
    char *p;
    int value;
    int place;

    place = 1;
    p = string_last_char(text);
    if (p == 0) return -1;
    value = hex_digit_value(p);
    while (p != text) {
        p--;
        place <<= 4;
        value += hex_digit_value(p) * place;
    }
    return value;
}
