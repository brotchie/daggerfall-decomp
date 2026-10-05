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
extern char D_000C5400[];
extern char D_000CEA30[];
extern char D_000CEA34[];
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char dungeon_water_level[];
extern char D_00142940[];
extern char D_00142944[];
extern char D_00176A68[];
extern char D_00176A72[];
extern char D_00176A7C[];
extern char D_001788CF[];
extern char player_environment[];
extern char D_001789FD[];
extern char D_001789FF[];
extern char player_speed[];
extern char D_001845C8[];
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern char D_00187C86[];
extern char D_00187C8A[];
extern char D_00187C8E[];
extern char D_00187C92[];
extern char D_00187C96[];
extern char D_00187C9A[];
extern char D_00187C9E[];
extern char D_00187CA4[];
extern char D_00187CA8[];
extern char D_001940D4[];
extern char D_001940D5[];
extern char D_001940D6[];
extern char D_001940D7[];
extern char D_001940DA[];
extern char player_motion_flags[];
extern char jump_velocity[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AB0[];
extern char vertical_velocity[];
extern struct record *spell_ready_missile;
extern struct character *player_character;
extern char cursor_arrow_image[];
extern struct career *player_class;
extern char game_minutes[];
extern char D_00195C44[];
extern struct record *D_00195C70;
extern char D_00195C74[];
extern struct record *D_00195CB8;
extern char D_00195CD4[];
extern char D_00195CD8[];
extern char D_00195DB8[];
extern char D_00195E30[];
extern char D_00195E4E[];
extern char mouse_control_mode[];
extern char mouse_turn_rate[];
extern char view_cursor_active[];
extern char D_00195F44[];
extern char D_00195F4E[];
extern char D_00195F5A[];
extern char mouse_motion_x[];
extern char mouse_motion_y[];
extern char D_00196272[];
extern char player_on_ground[];
extern char mouse_buttons_prev[];
extern char in_dungeon_water[];
extern char D_0019628E[];
extern char D_00196296[];
extern char D_00196D60[];
extern char collide_flags[];
extern char D_001A5A60[];
extern char D_001A5A64[];
extern char D_001A5A68[];
extern char D_001A5ACC[];
extern char D_001A5AD0[];
extern char D_001A5ADC[];
extern char D_001A5AE4[];
extern char D_001A5AE8[];
extern char turn_this_frame[];
extern char D_001A5AFC[];
extern char D_001A5B00[];
extern char D_001A5B04[];
extern char move_angle_offset[];
extern char D_001A5B0C[];
extern char D_001A5B10[];
extern char D_001A5B14[];
extern char cursor_region_images[];
extern char steer_row_y1[];
extern char steer_row_y2[];
extern char steer_col_x1[];
extern char steer_col_x2[];
extern char steer_weight_down[];
extern char steer_weight_right[];
extern char steer_region_width[];
extern char steer_region_height[];
extern char steer_weight_left[];
extern char steer_weight_up[];
extern char D_001A5B30[];
extern char steer_key_region[];
extern char D_001A5B34[];

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

    *(int *)cursor_region_images = disk_read_file((int)D_00176A68, 0);
    *(int *)cursor_arrow_image = disk_read_file((int)D_00176A72, 0);
    mc_memset(*(int *)D_00195C44, 0, 256, (int)D_00176A7C, 44, 4);
    *(int *)&l_18 = 0;
L80303:;
    if (((int)(short)l_18) < 10) goto L80316;
    goto L8034A;
L8030E:;
    (*(int *)&l_18)++;
    goto L80303;
L80316:;
    mc_memcpy((int)(*(char **)D_00195C44 + (((int)(short)l_18) << 4)), (int)(*(char **)cursor_arrow_image + (((int)(short)l_18) * 10)), 10, (int)D_00176A7C, 46, 4);
    goto L8030E;
L8034A:;
    func_0012B45B(*(int *)D_00195C44, 0, 0);
    intrface_set_regions();
    *(signed char *)D_00196272 = 0;
}

void intrface_set_regions(void)
{
    *(short *)steer_region_width = ((int)(short)*(short *)D_001789FD) / 3;
    *(short *)steer_region_height = ((int)(short)*(short *)D_001789FF) / 3;
    *(short *)steer_row_y1 = (((int)(short)*(short *)D_001789FF) / 3) + ((int)(short)*(short *)D_00142944);
    *(short *)steer_row_y2 = ((((int)(short)*(short *)D_001789FF) / 3) * 2) + ((int)(short)*(short *)D_00142944);
    *(short *)steer_col_x1 = (((int)(short)*(short *)D_001789FD) / 3) + ((int)(short)*(short *)D_00142940);
    *(short *)steer_col_x2 = ((((int)(short)*(short *)D_001789FD) / 3) * 2) + ((int)(short)*(short *)D_00142940);
}

void intrface_free(void)
{
    if (*(int *)cursor_region_images == 0) goto L80469;
    if (*(int *)cursor_region_images != (-1751672937)) goto L8046B;
L80469:;
    goto L80489;
L8046B:;
    mc_free(*(int *)cursor_region_images, (int)D_00176A7C, 67);
    *(int *)cursor_region_images = -1751672937;
L80489:;
    if (*(int *)cursor_arrow_image == 0) goto L8049E;
    if (*(int *)cursor_arrow_image != (-1751672937)) goto L804A0;
L8049E:;
    return;
L804A0:;
    mc_free(*(int *)cursor_arrow_image, (int)D_00176A7C, 68);
    *(int *)cursor_arrow_image = -1751672937;
}

void cursor_draw_arrow(void)
{
    func_00144E84((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y, 10, 10, (int)D_001A5A68, 0);
    *(int *)D_001A5B14 = (int)(short)*(short *)mouse_x;
    *(int *)D_001A5ACC = (int)(short)*(short *)mouse_y;
    func_00144FB4((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y, 10, 10, *(int *)cursor_arrow_image);
    func_0012B47E((int)mouse_motion_x, (int)mouse_motion_y);
    *(short *)mouse_motion_x = (*(short *)mouse_motion_y = 0);
}

void func_00080637(void)
{
    func_00144FB4(*(int *)D_001A5B14, *(int *)D_001A5ACC, 10, 10, (int)D_001A5A68);
}

void func_0008066F(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    *(signed char *)D_001940D4 |= 16;
    player_object->yaw &= ~0xF800;
    l_1C = player_object->yaw;
    func_000CE6E2(l_1C, a1 << 7, (int)&l_24, (int)&l_20);
    *(int *)D_00187CA4 = a1;
    *(int *)D_001A5B04 += a1;
    *(int *)D_001A5B0C += l_24;
    *(int *)D_001A5B10 += l_20;
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
    *(int *)D_000C5400 = 8;
    *(int *)turn_this_frame = -(((*(int *)D_00195AB0 * ((((int)(short)*(short *)steer_weight_left) << 6) * *(int *)D_001A5AFC)) / 1000) / 256);
    *(short *)player_speed = (((int)(short)*(short *)player_speed) * ((int)(short)*(short *)steer_weight_up)) / 256;
}

void steer_forward(void)
{
    *(int *)turn_this_frame = 0;
    *(short *)player_speed = (((int)(short)*(short *)player_speed) * ((int)(short)*(short *)steer_weight_up)) / 220;
}

void steer_forward_right(void)
{
    *(int *)D_000C5400 = -8;
    *(int *)turn_this_frame = ((*(int *)D_00195AB0 * ((((int)(short)*(short *)steer_weight_right) << 6) * *(int *)D_001A5AFC)) / 1000) / 256;
    *(short *)player_speed = (((int)(short)*(short *)player_speed) * ((int)(short)*(short *)steer_weight_up)) / 256;
}

void steer_turn_left(void)
{
    *(int *)D_000C5400 = 8;
    *(int *)turn_this_frame = -(((*(int *)D_00195AB0 * ((((int)(short)*(short *)steer_weight_left) << 6) * *(int *)D_001A5AFC)) / 1000) / 256);
    *(short *)player_speed = 0;
}

void steer_center(void)
{
    *(short *)player_speed = 0;
    *(int *)move_angle_offset = 0;
    *(int *)turn_this_frame = 0;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) return;
    click_activate(0);
}

void steer_turn_right(void)
{
    *(int *)D_000C5400 = -8;
    *(int *)turn_this_frame = ((((((int)(short)*(short *)steer_weight_right) << 6) * *(int *)D_001A5AFC) * *(int *)D_00195AB0) / 1000) / 256;
    *(short *)player_speed = 0;
}

void steer_slide_left(void)
{
    int l_1C;
    int l_18;

    *(int *)D_000C5400 = 4;
    *(short *)player_speed = (((int)(short)*(short *)player_speed) * ((int)(short)*(short *)steer_weight_left)) / 256;
    *(int *)move_angle_offset = -512;
}

void steer_backward(void)
{
    int l_1C;
    int l_18;

    *(int *)move_angle_offset = 1024;
    *(short *)player_speed = (((int)(short)*(short *)player_speed) * ((int)(short)*(short *)steer_weight_down)) / 256;
}

void steer_slide_right(void)
{
    int l_1C;
    int l_18;

    *(int *)D_000C5400 = -4;
    *(int *)move_angle_offset = 512;
    *(short *)player_speed = (((int)(short)*(short *)player_speed) * ((int)(short)*(short *)steer_weight_right)) / 256;
}

void click_activate(int a1)
{
{
    char l_2C[20];

    if (*(signed char *)D_00187CA8 == 0) return;
    if (a1 != 0) goto L80AE3;
    if (((int)(unsigned char)*(signed char *)mouse_control_mode) != 1) goto L80AAB;
    if (*(signed char *)view_cursor_active != 0) goto L80AAD;
L80AAB:;
    goto L80AC5;
L80AAD:;
    engine_pick_object((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y, (int)l_2C);
    goto L80AE1;
L80AC5:;
    engine_pick_object(((int)(short)*(short *)mouse_x) + 6, (int)&*(signed char *)((char *)((int)(short)*(short *)mouse_y) + 6), (int)l_2C);
L80AE1:;
    goto L80B11;
L80AE3:;
    *(short *)mouse_x = *(short *)D_000CEA30;
    *(short *)mouse_y = *(short *)D_000CEA34;
    engine_pick_object((int)(short)*(short *)D_000CEA30, (int)(short)*(short *)D_000CEA34, (int)l_2C);
L80B11:;
    if ((*(int *)l_2C & 1) == 0) goto L80B31;
    if (((int)(unsigned char)*(signed char *)mouse_control_mode) == 1) {}
    func_00074024((int)l_2C, *(int *)((char *)l_2C + 4));
    return;
L80B31:;
    if ((int)spell_ready_missile == 0) return;
    cast_fire_missile((int)spell_ready_missile);
    spell_ready_missile = 0;
}
}

int intrface_key_region(void)
{
    *(short *)steer_weight_right = (*(short *)steer_weight_down = (*(short *)steer_weight_left = (*(short *)steer_weight_up = 255)));
    if (key_action_held(3) == 0) goto L80BAD;
    return 6;
L80BAD:;
    if (key_action_held(2) == 0) goto L80BC9;
    if (key_action_held(10) != 0) goto L80BCB;
L80BC9:;
    goto L80BD7;
L80BCB:;
    return 6;
L80BD7:;
    if (key_action_held(5) == 0) goto L80BF1;
    return 8;
L80BF1:;
    if (key_action_held(4) == 0) goto L80C0D;
    if (key_action_held(10) != 0) goto L80C0F;
L80C0D:;
    goto L80C1B;
L80C0F:;
    return 8;
L80C1B:;
    if (key_action_held(0) == 0) goto L80C34;
    if (key_action_held(4) != 0) goto L80C36;
L80C34:;
    goto L80C42;
L80C36:;
    return 2;
L80C42:;
    if (key_action_held(0) == 0) goto L80C5B;
    if (key_action_held(2) != 0) goto L80C5D;
L80C5B:;
    goto L80C69;
L80C5D:;
    return 0;
L80C69:;
    if (key_action_held(0) == 0) goto L80C80;
    return 1;
L80C80:;
    if (key_action_held(1) == 0) goto L80C97;
    return 7;
L80C97:;
    if (key_action_held(4) == 0) goto L80CB3;
    if (key_action_held(10) == 0) goto L80CB5;
L80CB3:;
    goto L80CBE;
L80CB5:;
    return 5;
L80CBE:;
    if (key_action_held(2) == 0) goto L80CDA;
    if (key_action_held(10) == 0) goto L80CDC;
L80CDA:;
    goto L80CE5;
L80CDC:;
    return 3;
L80CE5:;
    return -1;
}

void object_apply_gravity(struct record *a1, struct character *a2)
{
    if (*(int *)dungeon_water_level == 10000) goto L80D23;
    if (a2 == player_character) goto L80D25;
L80D23:;
    goto L80D67;
L80D25:;
    if ((a1->y - 50) <= *(int *)dungeon_water_level) goto L80D67;
    if (*(signed char *)in_dungeon_water != 0) goto L80D51;
    sound_play(86, a1, 100);
L80D51:;
    *(signed char *)in_dungeon_water = 1;
    *(int *)vertical_velocity = 0;
    return;
L80D67:;
    *(signed char *)in_dungeon_water = 0;
    if ((a2->conditions & 0x8) != 0) return;
    if (a2 != player_character) goto L80D92;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) goto L80D94;
L80D92:;
    goto L80DA3;
L80D94:;
    *(int *)vertical_velocity = 0;
    return;
L80DA3:;
    if ((a2->conditions & 0x4000) == 0) goto L80DB9;
    *(int *)vertical_velocity = 15360;
L80DB9:;
    if ((a1->y - 80) > *(int *)D_00195C74) goto L80DD3;
    if (*(int *)vertical_velocity < 0) goto L80DD5;
L80DD3:;
    goto L80DE1;
L80DD5:;
    *(int *)vertical_velocity = 0;
    return;
L80DE1:;
    *(int *)vertical_velocity += (*(int *)D_00195AB0 * 100352) / 1000;
    a1->y += ((*(int *)vertical_velocity * *(int *)D_00195AB0) / 1000) / 256;
}

void player_compute_jump_velocity(void)
{
    *(int *)jump_velocity = ((player_character->attributes[6] + player_character->attributes[0]) / 2) + 50;
    *(int *)jump_velocity += (player_character->skills[3].value * *(int *)jump_velocity) / 100;
    *(int *)jump_velocity = -(*(int *)jump_velocity);
    *(int *)jump_velocity <<= 8;
}

void func_00080EA4(int a1)
{
    if (*(signed char *)D_001A5B34 != 0) return;
    if (a1 >= 0) goto L80F00;
    camera_object->angle_z -= 8;
    camera_object->angle_z &= ~0xF800;
    if (camera_object->angle_z >= (a1 + 2047)) goto L80EFE;
    camera_object->angle_z = a1 + 2047;
L80EFE:;
    return;
L80F00:;
    camera_object->angle_z += 8;
    if (camera_object->angle_z <= a1) return;
    camera_object->angle_z = a1;
}

void func_00080F2F(void)
{
    if (camera_object->angle_z == 0) return;
    if (camera_object->angle_z >= 1024) goto L80F7C;
    camera_object->angle_z -= 8;
    if (camera_object->angle_z >= 0) goto L80F7A;
    camera_object->angle_z = 0;
L80F7A:;
    return;
L80F7C:;
    camera_object->angle_z += 8;
    if (camera_object->angle_z <= 2047) return;
    camera_object->angle_z = 0;
}

void func_00080FAB(int a1)
{
    *(signed char *)player_motion_flags &= 191;
}

int func_00080FCD(int a1)
{
    int l_20;
    int l_1C;

    l_20 = (*(int *)((char *)a1) << 8) / func_0014BC00((*(int *)((char *)a1) * *(int *)((char *)a1)) + (*(int *)((char *)a1 + 8) * *(int *)((char *)a1 + 8)));
    if (*(int *)((char *)a1) <= 0) goto L81047;
    l_1C = (l_20 * 511) / 256;
    if (*(int *)((char *)a1 + 8) >= 0) goto L81045;
    l_1C = (512 - l_1C) + 512;
L81045:;
    goto L8107E;
L81047:;
    l_1C = ((l_20 * 511) / 256) + 2047;
    if (*(int *)((char *)a1 + 8) >= 0) goto L8107E;
    l_1C = 1536 - (l_1C - 1536);
L8107E:;
    return l_1C & 2047;
}

void intrface_poll_controls(void)
{
    int l_1C;
    int l_18;

    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    func_0007EED8();
    func_00152D00();
    if (((int)(short)(*(short *)steer_key_region = intrface_key_region())) == (-1)) goto L81328;
    if (*(short *)steer_key_region == 0) goto L812F4;
    if (((int)(short)*(short *)steer_key_region) != 2) goto L812F6;
L812F4:;
    goto L81302;
L812F6:;
    if (((int)(short)*(short *)steer_key_region) != 3) goto L81304;
L81302:;
    goto L81310;
L81304:;
    if (((int)(short)*(short *)steer_key_region) != 5) goto L81312;
L81310:;
    goto L8131B;
L81312:;
    l_1C = 1;
    goto L81322;
L8131B:;
    l_1C = 0;
L81322:;
    if (l_1C == 0) return;
L81328:;
    l_18 = 1132;
    *(int *)D_001A5AE8 = *(int *)((char *)l_18);
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

    if (*(int *)D_00195DB8 == 0) goto L81447;
    (*(int *)D_00195DB8)--;
    return;
L81447:;
    *(signed char *)D_001940D7 &= 223;
    *(int *)D_001788CF = (((int)(unsigned char)(*(signed char *)mouse_turn_rate & 127)) * 2) + 4;
    player_mouse_look();
    intrface_poll_controls();
    *(signed char *)D_0019628E = 0;
    if (hud_update() != 0) return;
    l_4C = intrface_region_at((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y, (int)&l_48, (int)&l_44);
    cursor_draw((int)(short)*(short *)&l_4C);
    if (*(signed char *)D_00196272 != 0) return;
    if (key_action_held(22) != 0) goto L814DD;
    if (((struct bf8_6_1 *)&D_001940DA)->f == 0) goto L814DB;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) != 0) goto L814DD;
L814DB:;
    goto L814E2;
L814DD:;
    return;
L814E2:;
    *(int *)D_00195CD8 = (*(int *)D_00195CD4 = 0);
    l_30 = player_object->x;
    l_28 = player_object->z;
    l_38 = *(int *)vertical_velocity;
    if (((int)(short)*(short *)steer_key_region) == (-1)) goto L8152A;
    l_4C = (int)(short)*(short *)steer_key_region;
L8152A:;
    if ((player_character->conditions & 0x8) == 0) goto L8153F;
    *(signed char *)player_on_ground = 1;
L8153F:;
    if (*(signed char *)player_on_ground != 0) goto L81551;
    if (*(signed char *)in_dungeon_water == 0) goto L81553;
L81551:;
    goto L8155C;
L81553:;
    if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) goto L8155E;
L8155C:;
    goto L81567;
L8155E:;
    l_60 = 1;
    goto L8156E;
L81567:;
    l_60 = 0;
L8156E:;
    l_5C = l_60;
    l_24 = l_5C;
    if (l_5C == 0) goto L81594;
    *(int *)turn_this_frame = 0;
    l_40 = *(int *)D_00187CA4;
    goto L815BB;
L81594:;
    intrface_steer((int)(short)*(short *)D_00195F4E, l_4C, l_48, l_44);
    l_40 = (int)(short)*(short *)player_speed;
    *(int *)D_00187CA4 = l_40;
L815BB:;
    if (((struct bf8_1_1 *)&D_001940D5)->f == 0) goto L815D0;
    *(signed char *)D_001940D5 &= 253;
    return;
L815D0:;
    if (*(int *)turn_this_frame == 0) goto L81623;
    func_00080EA4(*(int *)turn_this_frame / 4);
    camera_object->yaw += *(short *)turn_this_frame;
    player_object->yaw = camera_object->yaw;
    *(int *)turn_this_frame = 0;
    goto L81628;
L81623:;
    func_00080F2F();
L81628:;
    if (((struct bf8_2_1 *)&player_motion_flags)->f == 0) goto L8163A;
    l_64 = 5;
    goto L81641;
L8163A:;
    l_64 = 10;
L81641:;
    l_54 = l_64;
    l_58 = 0;
    if (l_5C != 0) goto L81660;
    if (((int)(unsigned char)*(signed char *)mouse_control_mode) == 1) goto L81662;
L81660:;
    goto L81670;
L81662:;
    if (key_action_held(23) != 0) goto L81672;
L81670:;
    goto L81681;
L81672:;
    *(int *)D_00187CA4 >>= 1;
    (*(int *)D_00187CA4)--;
    l_40 >>= 1;
L81681:;
    if (l_58 != 0) goto L8168D;
    if (l_40 > 0) goto L81692;
L8168D:;
    goto L81715;
L81692:;
    if (l_40 >= l_54) goto L816A9;
    l_50 = l_40;
    l_40 = 0;
    goto L816B5;
L816A9:;
    l_40 -= l_54;
    l_50 = l_54;
L816B5:;
    l_58 = player_try_move(l_50);
    if (((int)(short)(*(short *)collide_flags & 16)) != 0) goto L816E0;
    if (((int)(short)(*(short *)collide_flags & 1)) != 0) goto L816E2;
L816E0:;
    goto L816F1;
L816E2:;
    links_trigger((int)D_00195CB8, 1);
L816F1:;
    if (((int)(short)(*(short *)collide_flags & 2)) == 0) goto L81710;
    links_trigger((int)D_00195C70, 3);
L81710:;
    goto L81681;
L81715:;
    if (l_24 == 0) goto L81724;
    if (*(signed char *)player_on_ground != 0) goto L81726;
L81724:;
    goto L8172D;
L81726:;
    l_24 = 1;
L8172D:;
    if (l_40 != 0) goto L8173C;
    if ((int)D_00195CB8 != 0) goto L8173E;
L8173C:;
    goto L8174C;
L8173E:;
    if (func_000657B2((int)D_00195CB8) != 0) goto L8174E;
L8174C:;
    goto L81755;
L8174E:;
    player_try_move(0);
L81755:;
    l_44 = 0;
    if (((int)(short)(*(short *)collide_flags & 2)) == 0) goto L81776;
    if (*(short *)D_001A5B30 == 0) goto L81778;
L81776:;
    goto L81789;
L81778:;
    if (func_00082609((int)(unsigned short)*(short *)D_00195F5A) != 0) goto L8178B;
L81789:;
    goto L817AB;
L8178B:;
    if (func_000C7FD9(player_object->x, player_object->z, l_30, l_28) < 5) goto L817AD;
L817AB:;
    goto L817BB;
L817AD:;
    if ((player_character->conditions & 0x8) == 0) goto L817BD;
L817BB:;
    goto L817D4;
L817BD:;
    if (((int)(unsigned short)(player_character->flags & 1536)) == 0) goto L817D9;
L817D4:;
    goto L818D4;
L817D9:;
    if (((unsigned)(*(int *)((char *)1132) - *(int *)D_001A5AE4)) <= 14) goto L818D2;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) goto L81816;
    hud_message_add(*(int *)D_001845C8);
    skill_add_uses(18, 1);
L81816:;
    *(signed char *)player_motion_flags |= 32;
    l_44 = (-((int)(short)*(short *)player_speed)) / 3;
    if (((unsigned)(*(int *)game_minutes - *(int *)D_001A5B00)) <= 1) goto L818D2;
    skill_add_uses(18, 1);
    *(int *)D_001A5B00 = *(int *)game_minutes;
    l_20 = player_character->skills[18].value;
    if (player_character->race != 6) goto L81888;
    l_20 += 30;
L81888:;
    if ((player_character->conditions & 0x20000) != 0) goto L818AA;
    if (rand_range(1, 100) > 95) goto L818AC;
L818AA:;
    goto L818C0;
L818AC:;
    if (rand_range(1, 100) > l_20) goto L818C2;
L818C0:;
    goto L818D2;
L818C2:;
    *(signed char *)player_motion_flags &= 223;
    *(short *)D_001A5B30 = 1;
L818D2:;
    goto L81926;
L818D4:;
    if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) goto L818E7;
    if (*(short *)D_001A5B30 == 0) goto L818E9;
L818E7:;
    goto L818F9;
L818E9:;
    if (((int)(short)(*(short *)collide_flags & 2)) == 0) goto L818FB;
L818F9:;
    goto L8190B;
L818FB:;
    if (func_000824C1() != 0) goto L8190B;
    *(signed char *)player_motion_flags &= 223;
L8190B:;
    *(int *)D_001A5AE4 = *(int *)((char *)1132);
    *(int *)D_001A5B00 = *(int *)game_minutes;
L81926:;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) goto L8193D;
    if ((player_character->conditions & 0x8) == 0) goto L81947;
L8193D:;
    *(int *)vertical_velocity = 0;
L81947:;
    if ((player_object->y - 50) > *(int *)dungeon_water_level) goto L81961;
    *(signed char *)in_dungeon_water = 0;
L81961:;
    *(short *)D_00195F44 = 0;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 3)) != 3) goto L8198C;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 3)) == 1) goto L8199A;
L8198C:;
    if (key_action_held(8) == 0) goto L819A3;
L8199A:;
    if (*(signed char *)player_on_ground != 0) goto L819A5;
L819A3:;
    goto L819B3;
L819A5:;
    if ((player_character->conditions & 0x1) == 0) goto L819B5;
L819B3:;
    goto L819CC;
L819B5:;
    if (((int)(unsigned short)(player_character->flags & 1536)) == 0) goto L819D1;
L819CC:;
    goto L81A63;
L819D1:;
    skill_add_uses(3, 1);
    if ((player_character->conditions & 0x10000) == 0) goto L819FA;
    l_3C = *(int *)jump_velocity * 2;
    goto L81A02;
L819FA:;
    l_3C = *(int *)jump_velocity;
L81A02:;
    if (((int)(unsigned short)(player_class->flags & 2)) == 0) goto L81A2B;
    l_3C = (int)(*(char **)jump_velocity + (*(int *)jump_velocity >> 1));
L81A2B:;
    l_44 = -1;
    *(int *)vertical_velocity += l_3C;
    *(short *)D_00195F44 = 1;
    fatigue_add(-11);
    l_5C = 1;
    *(signed char *)player_on_ground = 0;
    *(signed char *)D_00196296 = 1;
L81A63:;
    if (l_24 == 0) goto L81A72;
    if (*(signed char *)player_on_ground != 0) goto L81A74;
L81A72:;
    goto L81A7B;
L81A74:;
    l_24 = 1;
L81A7B:;
    if (key_action_held(6) == 0) goto L81AA2;
    if ((player_character->conditions & 0x8) != 0) goto L81AA0;
    if (*(signed char *)in_dungeon_water == 0) goto L81AA2;
L81AA0:;
    goto L81AA4;
L81AA2:;
    goto L81AB7;
L81AA4:;
    if ((player_object->y - 100) > *(int *)D_00195C74) goto L81AB9;
L81AB7:;
    goto L81AF3;
L81AB9:;
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) goto L81ADB;
    if ((*(int *)D_00196D60 - 1024) > player_object->y) goto L81AF1;
L81ADB:;
    l_44 += (*(int *)D_00195AB0 * (-80)) / 1000;
L81AF1:;
    goto L81B32;
L81AF3:;
    if (key_action_held(7) == 0) goto L81B1A;
    if ((player_character->conditions & 0x8) != 0) goto L81B18;
    if (*(signed char *)in_dungeon_water == 0) goto L81B1A;
L81B18:;
    goto L81B1C;
L81B1A:;
    goto L81B32;
L81B1C:;
    l_44 += (*(int *)D_00195AB0 * 80) / 1000;
L81B32:;
    if (*(signed char *)in_dungeon_water == 0) goto L81B4C;
    if (object_weight(player_entity) > 250) goto L81B4E;
L81B4C:;
    goto L81B5C;
L81B4E:;
    if (key_action_held(7) == 0) goto L81B5E;
L81B5C:;
    goto L81B6F;
L81B5E:;
    if ((player_character->conditions & 1048584) == 0) goto L81B71;
L81B6F:;
    goto L81B87;
L81B71:;
    l_44 += (*(int *)D_00195AB0 * 80) / 1000;
L81B87:;
    l_2C = player_object->y;
    if (*(signed char *)player_on_ground != 0) goto L81BA4;
    if (*(signed char *)in_dungeon_water == 0) goto L81BA6;
L81BA4:;
    goto L81BAF;
L81BA6:;
    if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) goto L81BB1;
L81BAF:;
    goto L81BBA;
L81BB1:;
    l_68 = 1;
    goto L81BC1;
L81BBA:;
    l_68 = 0;
L81BC1:;
    l_5C = l_68;
    l_24 = l_5C;
    if (((int)(short)(*(short *)collide_flags & 16)) != 0) goto L81BE3;
    if (l_5C == 0) goto L81BE5;
L81BE3:;
    goto L81C06;
L81BE5:;
    if (*(int *)dungeon_water_level == 10000) goto L81C04;
    if ((player_object->y - 50) > *(int *)dungeon_water_level) goto L81C06;
L81C04:;
    goto L81C3D;
L81C06:;
    l_2C = player_object->y;
    object_apply_gravity(player_object, player_character);
    l_44 += player_object->y - l_2C;
    player_object->y = l_2C;
    goto L81C50;
L81C3D:;
    *(short *)D_001A5B30 = 0;
    *(int *)vertical_velocity = 0;
L81C50:;
    if (l_24 == 0) goto L81C5F;
    if (*(signed char *)player_on_ground != 0) goto L81C61;
L81C5F:;
    goto L81C68;
L81C61:;
    l_24 = 1;
L81C68:;
    player_try_move_vertical(l_44);
    if (*(signed char *)player_on_ground == 0) goto L81C83;
    *(int *)vertical_velocity = 0;
L81C83:;
    if (*(int *)vertical_velocity >= 0) goto L81C96;
    if (*(short *)D_00195F44 != 0) goto L81C98;
L81C96:;
    goto L81CAB;
L81C98:;
    if ((player_object->y - 99) < *(int *)D_00195C74) goto L81CAD;
L81CAB:;
    goto L81CB9;
L81CAD:;
    player_object->y = l_2C;
L81CB9:;
    if (l_24 == 0) goto L81CC8;
    if (*(signed char *)player_on_ground != 0) goto L81CCA;
L81CC8:;
    goto L81CD1;
L81CCA:;
    l_24 = 1;
L81CD1:;
    if (*(int *)D_00195CD8 == 0) goto L81D21;
    *(short *)D_001A5B30 = 0;
    if (*(signed char *)player_on_ground == 0) goto L81CF6;
    *(int *)vertical_velocity = 0;
L81CF6:;
    mc_memcpy((int)D_00195E30, *(int *)D_00195CD8, 30, (int)D_00176A7C, 695, 4);
    *(int *)D_00195CD8 = (int)D_00195E30;
L81D21:;
    if (*(int *)D_00195CD4 == 0) goto L81D55;
    mc_memcpy((int)D_00195E4E, *(int *)D_00195CD4, 30, (int)D_00176A7C, 701, 4);
    *(int *)D_00195CD4 = (int)D_00195E4E;
L81D55:;
    if (*(int *)vertical_velocity != 0) goto L81D64;
    if (l_38 != 0) goto L81D69;
L81D64:;
    goto L81DF6;
L81D69:;
    l_34 = ((l_38 / 256) / 40) - 7;
    if (l_34 <= 0) goto L81DF6;
    l_34 = l_34 * l_34;
    l_34 = l_34 / 2;
    damage_apply(player_entity, l_34, 0);
    sound_play(((((int)(unsigned char)*(signed char *)player_environment) != 3) ? 360 : 359), player_entity, 100);
L81DF6:;
    if (((int)(unsigned char)*(signed char *)mouse_control_mode) != 1) goto L81E0B;
    if (*(signed char *)D_00196272 == 0) goto L81E0D;
L81E0B:;
    goto L81E5B;
L81E0D:;
    if (*(signed char *)view_cursor_active == 0) goto L81E26;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) != 0) goto L81E28;
L81E26:;
    goto L81E38;
L81E28:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L81E3A;
L81E38:;
    goto L81E41;
L81E3A:;
    click_activate(0);
L81E41:;
    if (key_action_pressed(18) == 0) goto L81E59;
    click_activate(1);
L81E59:;
    goto L81E86;
L81E5B:;
    if (*(signed char *)D_00196272 != 0) goto L81E86;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L81E7D;
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) goto L81E7F;
L81E7D:;
    goto L81E86;
L81E7F:;
    click_activate(0);
L81E86:;
    if (l_24 == 0) goto L81E95;
    if (*(signed char *)player_on_ground != 0) goto L81E97;
L81E95:;
    goto L81E9E;
L81E97:;
    l_24 = 1;
L81E9E:;
    player_horse_sounds(*(int *)D_00187CA4);
    if (*(int *)dungeon_water_level == 10000) goto L81EC7;
    if ((player_object->y - 76) > *(int *)dungeon_water_level) goto L81EC9;
L81EC7:;
    goto L81ED3;
L81EC9:;
    if (rand() < 100) goto L81ED5;
L81ED3:;
    return;
L81ED5:;
    sound_play(384, player_object, 100);
}

void player_horse_sounds(int a1)
{
    int l_18;

    if (((int)(unsigned short)(player_character->flags & 1536)) == 0) return;
    l_18 = (((((int)(short)*(short *)D_00195F4E) - (((int)(short)*(short *)D_00195F4E) >> 2)) < a1) ? 366 : 365);
    if (((int)(unsigned short)(player_character->flags & 1024)) == 0) goto L823EE;
    l_18 = 372;
L823EE:;
    if (a1 != 0) goto L82425;
    if (*(int *)D_001A5AD0 == (-1)) return;
    if (func_000696F9(*(int *)D_001A5AD0) != 0) return;
    sound_stop_channel(*(int *)D_001A5AD0);
    *(int *)D_001A5AD0 = -1;
    return;
L82425:;
    if (*(int *)D_001A5AD0 != (-1)) goto L82448;
    *(int *)D_001A5AD0 = func_00069AB8(l_18, player_object, 100);
    goto L82475;
L82448:;
    if (l_18 == *(int *)D_001A5ADC) goto L82475;
    sound_stop_channel(*(int *)D_001A5AD0);
    *(int *)D_001A5AD0 = func_00069AB8(l_18, player_object, 100);
L82475:;
    *(int *)D_001A5ADC = l_18;
}

void player_horse_sounds_stop(void)
{
    if (func_000696F9((*(int *)D_001A5AD0 = -1)) != 0) return;
    sound_stop_channel(*(int *)D_001A5AD0);
}

int func_000824C1(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    func_000CE6E2((player_object->yaw + *(int *)move_angle_offset) & 2047, 1024, (int)&l_20, (int)&l_1C);
    l_20 += player_object->x << 5;
    l_1C += player_object->z << 5;
    l_20 += *(int *)D_001A5A64;
    l_1C += *(int *)D_001A5A60;
    *(int *)D_001A5A64 = l_20 & 31;
    *(int *)D_001A5A60 = l_1C & 31;
    *(int *)D_00187C86 = l_20 / 32;
    *(int *)D_00187C8A = player_object->y - 32;
    *(int *)D_00187C8E = l_1C / 32;
    *(int *)D_00187C92 = player_object->angle_x;
    *(int *)D_00187C96 = player_object->yaw;
    *(int *)D_00187C9A = player_object->angle_z;
    if (((struct bf8_2_1 *)&player_motion_flags)->f == 0) goto L825AD;
    l_28 = (int)D_00187C12;
    goto L825B4;
L825AD:;
    l_28 = (int)D_00187B6E;
L825B4:;
    *(int *)D_00187C9E = l_28;
    if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) goto L825CF;
    *(int *)D_00187C9E = (int)D_00187BB8;
L825CF:;
    *(signed char *)collide_flags |= 4;
    *(signed char *)player_motion_flags &= 223;
    l_24 = collide_move_player(player_object, 0, (int)D_00187C86, 1);
    return l_24;
}

int func_00082657(int a1)
{
    int l_20;
    int l_1C;

    l_20 = a1;
    l_1C = 0;
L82675:;
    if (*(signed char *)((char *)l_20) == 0) goto L82688;
    l_1C = l_20++;
    goto L82675;
L82688:;
    return l_1C;
}

int func_00082750(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_1C = 1;
    l_24 = func_00082657(a1);
    if (l_24 != 0) goto L82782;
    return -1;
L82782:;
    l_20 = func_0008269B(l_24);
L8278D:;
    if (l_24 == a1) goto L827B0;
    l_24--;
    l_1C <<= 4;
    l_20 += func_0008269B(l_24) * l_1C;
    goto L8278D;
L827B0:;
    return l_20;
}
