/* text.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_0_4 { unsigned char f:4; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern int D_000CEA24;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down[];
extern signed char key_down_esc;
extern signed char key_down_ctrl;
extern signed char key_down_lshift;
extern signed char key_down_x;
extern int screen_buffer;
extern int D_00147954;
extern char D_00170D55[];
extern char D_00170DAE[];
extern char D_00170DB7[];
extern unsigned char player_environment;
extern int D_00178A14;
extern signed char D_0017B62F[];
extern short D_0017B64F[];
extern char D_0017B657[];
extern char D_0017B659[];
extern signed char D_0017B6CD;
extern struct record *D_00190504[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D6;
extern signed char D_001940DA;
extern char frame_counter[];
extern struct record *player_object;
extern struct record *D_00195AA8;
extern int D_00195AB0;
extern struct record *D_00195AC4;
extern int creature_count;
extern char *buttons_rci;
extern struct location *current_location;
extern struct character *player_character;
extern int D_00195C40;
extern signed char climate_weathers[];
extern short D_00195F2E;
extern short D_00195F34;
extern unsigned short D_00195F38;
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char D_00196035;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern signed char D_00196091;
extern signed char current_region;
extern signed char msgbox_kind;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char D_0019628E;
extern int D_00196DA4;
extern int msgbox_spop_tiles[];
extern int msgbox_mpop_tiles;
extern int msgbox_next_page;
extern char msgbox_image[];
extern int msgbox_saved_screen;
extern short D_00199668;
extern short D_0019966A;
extern signed char D_0019966C;
extern struct record *people_list[];
extern int D_001996E8;
extern int D_001996EC;
extern int people_spawn_range;
extern int people_count;
extern struct quest *current_quest;
extern int D_00199808;

extern struct faction *faction_find_type_in_region(int, short);
extern int climate_category(void);
extern int func_00023DE0(int, int);
extern int text_rsc_load(int, int, int);
extern int func_0003E6B1(struct quest *, short, int, short);
extern int msgbox_render_rsc(short, int, int);
extern int msgbox_render_quest_text(struct quest *, short, int, int);
extern int func_000404D9(int, int);
extern int ai_angle_diff(int, int, int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int spawn_find_point(struct record *, int, int);
extern int key_pressed_once(unsigned char);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern int inpstr_update(void);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int rand();
extern int func_0009DEAC();
extern int mc_free();
extern int mc_malloc();
extern int mc_memcpy();
extern int func_000C7FD9();
extern int func_000C808D();
extern int func_000CDD81();
extern int func_000CE6E2();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00144F68();
extern int func_00144FB4();
extern int func_0014B45B();
extern unsigned char func_0002021B(int, int);
extern void func_00013D2D(int);
extern void msgbox_render(int, int);
extern void func_0003F630(int);
extern void guards_summon(int);
extern void person_pick_sprite(struct record *);
extern void keys_world_actions(void);
extern void game_exit(int);
extern void func_0006987B(void);
extern void monster_init(struct record *, int);
extern void mode_push(void);
extern void mode_pop(void);
extern void func_00080637(void);
extern void player_movement_update(void);
int msgbox_button_at(short, short);
int func_0003FDD2(struct record *, int);
int people_max_count(void);
int func_000406DC(struct record *, int);
int func_00040794(int, int);
int func_0004083A(struct record *, int, int);
int func_00040983(int, int);
void msgbox_wait(void);
void msgbox_update(void);
void msgbox_close(void);
void person_place(struct record *);
void func_000405EB(void);

void func_0003E653(int a1, int a2)
{
    int l_18;
    int l_14;

    l_18 = a2;
    l_14 = a1;
    while (*(signed char *)((char *)l_18) != 0 || *(signed char *)((char *)l_18 + 1) != 0) {
        *(signed char *)((char *)l_14) = *(signed char *)((char *)l_18);
        l_18++;
        l_14++;
    }
    *(signed char *)((char *)l_14) = 0;
    *(signed char *)((char *)l_14 + 1) = 0;
}

void msgbox_show_more_pages(int a1)
{
    while (msgbox_next_page != 0) {
        mc_memcpy(D_00147954, screen_buffer, 64000, (int)D_00170D55, 607, 4);
        msgbox_kind = 1;
        mode_push();
        game_mode = 8;
        D_00196272 = 1;
        while (mouse_buttons != 0) func_0012B136();
        mouse_buttons = (mouse_buttons_prev = 0);
        while (((int)(unsigned char)game_mode) == 8) {
            mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00170D55, 618, 4);
            keys_world_actions();
            msgbox_update();
            player_movement_update();
            func_000CDD81(0);
        }
        if (msgbox_next_page != 0) {
            D_0012B508 = 146;
            msgbox_render(msgbox_next_page, a1);
            while (mouse_buttons != 0) func_0012B136();
            mouse_buttons_prev = 0;
        }
    }
    msgbox_kind = 1;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons = (mouse_buttons_prev = 0);
    while (((int)(unsigned char)game_mode) == 8) {
        mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00170D55, 641, 4);
        keys_world_actions();
        msgbox_update();
        player_movement_update();
        func_000CDD81(0);
    }
}

void msgbox_show_quest_text(struct quest *a1, short a2, int a3)
{
    int l_18;

    if (msgbox_kind != 0) return;
    current_quest = a1;
    msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 735);
    mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 736, 4);
    func_0012DB50(4);
    if (((int)(short)*(short *)&a3) == 5) {
        D_00196271 = 0;
        l_18 = msgbox_render_quest_text(a1, (int)(short)a2, (int)msgbox_image, 4);
    } else {
        l_18 = msgbox_render_quest_text(a1, (int)(short)a2, (int)msgbox_image, 0);
    }
    if (l_18 == 0) return;
    msgbox_kind = *(signed char *)&a3;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    msgbox_wait();
}

void func_0003EFF9(struct quest *a1, short a2)
{
    int l_18;

    current_quest = a1;
    l_18 = func_0003E6B1(a1, (int)(short)a2, 0, 280);
    if (l_18 == 0 || *(signed char *)((char *)l_18) == 0) return;
    D_001940DA |= 8;
    func_00013D2D(l_18);
}

void func_0003F052(short a1)
{
    {
        int l_1C;

        l_1C = text_rsc_load((int)(short)a1, 0, 280);
        if (l_1C == 0 || *(signed char *)((char *)l_1C) == 0) return;
        D_001940DA |= 8;
        func_00013D2D(l_1C);
    }
}

void msgbox_show_rsc(int a1, int a2)
{
    {
        int l_1C;

        if (msgbox_kind != 0) return;
        func_00080637();
        msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 824);
        mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 825, 4);
        func_0012DB50(4);
        if (((int)(short)*(short *)&a2) == 5) {
            D_00196271 = 0;
            l_1C = msgbox_render_rsc((int)(short)*(short *)&a1, (int)msgbox_image, 4);
        } else {
            l_1C = msgbox_render_rsc((int)(short)*(short *)&a1, (int)msgbox_image, 0);
        }
        if (l_1C == 0) return;
        msgbox_kind = *(signed char *)&a2;
        mode_push();
        game_mode = 8;
        D_00196272 = 1;
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        msgbox_wait();
    }
}

void msgbox_open_rsc(int a1, int a2)
{
    {
        int l_1C;

        if (msgbox_kind != 0) return;
        func_00080637();
        msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 854);
        mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 855, 4);
        func_0012DB50(4);
        if (((int)(short)*(short *)&a2) == 5) {
            D_00196271 = 0;
            l_1C = msgbox_render_rsc((int)(short)*(short *)&a1, (int)msgbox_image, 4);
        } else {
            l_1C = msgbox_render_rsc((int)(short)*(short *)&a1, (int)msgbox_image, 0);
        }
        if (l_1C == 0) return;
        msgbox_kind = *(signed char *)&a2;
        mode_push();
        game_mode = 8;
        D_00196272 = 1;
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
    }
}

void msgbox_wait(void)
{
    int l_18;

    mouse_buttons = (mouse_buttons_prev = 1);
    while (((int)(unsigned char)game_mode) == 8) {
        mc_memcpy(screen_buffer, msgbox_saved_screen, 64000, (int)D_00170D55, 882, 4);
        if (key_down_ctrl != 0 && key_down_x != 0 && key_down_lshift != 0) {
            game_exit(0);
        }
        msgbox_update();
        player_movement_update();
        func_000CDD81(1);
        if (((struct bf8_0_4 *)&frame_counter)->f == 0) func_0006987B();
    }
    l_18 = 1132;
    D_00195C40 = *(int *)((char *)l_18);
    if (msgbox_saved_screen != 0 && msgbox_saved_screen != (-1751672937)) {
        mc_free(msgbox_saved_screen, (int)D_00170D55, 893);
        msgbox_saved_screen = -1751672937;
    }
    while (mouse_buttons != 0) func_0012B136();
}

void msgbox_update(void)
{
    int l_18;

    if (((int)(unsigned char)msgbox_kind) != 4 && ((int)(unsigned char)game_mode) != 8) {
        return;
    }
    D_0012B508 = 146;
    if (((struct bf8_6_1 *)&D_001940D5)->f != 0) {
        func_00144FB4(160 - (((int)(short)D_00199668) >> 1), ((int)(short)D_00195F34) - ((int)(short)D_0019966A), (int)(short)D_00199668, (int)(short)D_0019966A, *(int *)msgbox_image);
    } else {
        func_00144FB4(160 - (((int)(short)D_00199668) >> 1), 100 - (((int)(short)D_0019966A) >> 1), (int)(short)D_00199668, (int)(short)D_0019966A, *(int *)msgbox_image);
    }
    if (((int)(unsigned char)msgbox_kind) == 1) {
        if (D_00195F2E != 0) {
            D_00195F2E--;
            if (D_00195F2E == 0) goto L3F47F;
        }
        if (((struct bf8_0_1 *)&D_001940D5)->f != 0 && ((int)(unsigned char)(mouse_buttons & 3)) == 0) {
        } else if (key_pressed_once(28) == 0) {
            if (mouse_buttons == 0) return;
            if (mouse_buttons_prev != 0) return;
        }
L3F47F:;
        mouse_buttons_prev = 0;
        msgbox_close();
        while (mouse_buttons != 0) func_0012B136();
        return;
    }
    if (((int)(unsigned char)msgbox_kind) == 2) {
        if ((D_0019966C = inpstr_update()) != 0) msgbox_close();
        return;
    }
    if (((int)(unsigned char)msgbox_kind) == 4 && ((struct bf8_5_1 *)&D_001940D5)->f != 0) {
        msgbox_close();
        return;
    }
    if (((int)(unsigned char)msgbox_kind) != 5) return;
    if (((struct bf8_0_1 *)&D_001940D4)->f != 0 && key_down_esc != 0) {
        msgbox_close();
        return;
    }
    l_18 = msgbox_button_at((int)(short)mouse_x, (int)(short)mouse_y);
    if (*(short *)&l_18 == 0) return;
    sound_play(203, (int)player_object, 110);
    D_00196271 = *(signed char *)&l_18;
    msgbox_close();
}

void msgbox_close(void)
{
    if (*(int *)msgbox_image == 0) return;
    D_001940D5 &= 158;
    if (*(int *)msgbox_image != 0 && *(int *)msgbox_image != (-1751672937)) {
        mc_free(*(int *)msgbox_image, (int)D_00170D55, 950);
        *(int *)msgbox_image = -1751672937;
    }
    *(int *)msgbox_image = 0;
    msgbox_kind = 0;
    mode_pop();
    while (mouse_buttons != 0) func_0012B136();
    mouse_buttons = (mouse_buttons_prev = 0);
}

void msgbox_load_borders(void)
{
    msgbox_spop_tiles[0] = disk_read_file((int)D_00170DAE, 0);
    msgbox_mpop_tiles = disk_read_file((int)D_00170DB7, 0);
    func_0003F630(0);
}

void func_0003F699(void)
{
    if (msgbox_spop_tiles[0] != 0 && msgbox_spop_tiles[0] != (-1751672937)) {
        mc_free(msgbox_spop_tiles[0], (int)D_00170D55, 977);
        msgbox_spop_tiles[0] = -1751672937;
    }
    if (msgbox_mpop_tiles == 0 || msgbox_mpop_tiles == (-1751672937)) return;
    mc_free(msgbox_mpop_tiles, (int)D_00170D55, 978);
    msgbox_mpop_tiles = -1751672937;
}

void msgbox_draw_buttons(short a1)
{
    if (((struct bf8_6_1 *)&D_001940D5)->f != 0) {
        D_00195F38 = ((int)(short)a1) + ((((int)(short)D_00195F34) - ((int)(short)D_0019966A)) - (100 - (((int)(short)D_0019966A) >> 1)));
    } else {
        D_00195F38 = *(int *)&a1;
    }
    if (D_00196090 == 0) {
        func_00144F68(144, (int)(short)a1, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)msgbox_button_ids) - 1) << 9)));
        return;
    }
    if (D_00196091 == 0) {
        func_00144F68(112, (int)(short)a1, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)msgbox_button_ids) - 1) << 9)));
        func_00144F68(176, (int)(short)a1, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)D_00196090) - 1) << 9)));
        return;
    }
    func_00144F68(96, (int)(short)a1, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)msgbox_button_ids) - 1) << 9)));
    func_00144F68(144, (int)(short)a1, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)D_00196090) - 1) << 9)));
    func_00144F68(192, (int)(short)a1, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)D_00196091) - 1) << 9)));
}

int msgbox_button_at(short a1, short a2)
{
    if (D_00196090 == 0) {
        if (msgbox_button_keys != 0 && key_down[(int)(unsigned char)msgbox_button_keys] != 0) {
            return 1;
        }
        if (mouse_buttons == 0) return 0;
        if (mouse_buttons_prev != 0) return 0;
        if (mouse_buttons != 0 && ((int)(short)a1) > 144 && (short)a2 > D_00195F38 && ((int)(short)a1) < 176 && ((int)(short)a2) < (((int)D_00195F38) + 16)) {
            return 1;
        }
    } else if (D_00196091 == 0) {
        if (msgbox_button_keys != 0 && key_down[(int)(unsigned char)msgbox_button_keys] != 0) {
            return 1;
        }
        if (D_00196034 != 0 && key_down[(int)(unsigned char)D_00196034] != 0) {
            return 2;
        }
        if (mouse_buttons == 0) return 0;
        if (mouse_buttons_prev != 0) return 0;
        if (mouse_buttons != 0 && ((int)(short)a1) > 112 && (short)a2 > D_00195F38 && ((int)(short)a1) < 144 && ((int)(short)a2) < (((int)D_00195F38) + 16)) {
            return 1;
        }
        if (mouse_buttons != 0 && ((int)(short)a1) > 176 && (short)a2 > D_00195F38 && ((int)(short)a1) < 208 && ((int)(short)a2) < (((int)D_00195F38) + 16)) {
            return 2;
        }
    } else {
        if (msgbox_button_keys != 0 && key_down[(int)(unsigned char)msgbox_button_keys] != 0) {
            return 1;
        }
        if (D_00196034 != 0 && key_down[(int)(unsigned char)D_00196034] != 0) {
            return 2;
        }
        if (D_00196035 != 0 && key_down[(int)(unsigned char)D_00196035] != 0) {
            return 3;
        }
        if (mouse_buttons == 0) return 0;
        if (mouse_buttons_prev != 0) return 0;
        if (mouse_buttons != 0 && ((int)(short)a1) > 96 && (short)a2 > D_00195F38 && ((int)(short)a1) < 128 && ((int)(short)a2) < (((int)D_00195F38) + 16)) {
            return 1;
        }
        if (mouse_buttons != 0 && ((int)(short)a1) > 144 && (short)a2 > D_00195F38 && ((int)(short)a1) < 176 && ((int)(short)a2) < (((int)D_00195F38) + 16)) {
            return 2;
        }
        if (mouse_buttons != 0 && ((int)(short)a1) > 192 && (short)a2 > D_00195F38 && ((int)(short)a1) < 224 && ((int)(short)a2) < (((int)D_00195F38) + 16)) {
            return 3;
        }
    }
    return 0;
}

void people_spawn_tick(void)
{
    int l_18;

    D_001996E8 = people_max_count();
    if (people_count == 0 && D_001996E8 == 0) return;
    func_000405EB();
    for (l_18 = people_count; l_18 < D_001996E8; l_18++) {
        person_place(object_create_child(D_00195AC4, 0, 3));
    }
}

void people_update(void)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    if (people_count == 0) return;
    D_001996EC = 0;
    for (l_20 = 0; l_20 < people_count; l_20++) {
        if (people_list[l_20] == 0) continue;
        l_18 = people_list[l_20];
        l_1C = func_000C7FD9(l_18->x, l_18->z, player_object->x, player_object->z);
        if (l_1C > people_spawn_range) {
            if (people_count <= D_001996E8) {
                person_place(l_18);
            } else {
                object_delete(l_18);
                people_list[l_20] = 0;
                continue;
            }
        }
        if (func_0003FDD2(l_18, l_1C) == 0) {
            l_18->image = (l_18->image & -128) + 5;
            if ((l_18->image >> 7) == 399) l_18->image += 10;
        }
    }
}

int func_0003FDD2(struct record *a1, int a2)
{
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
    int l_1C;
    int l_18;

    l_30 = 0;
    l_18 = 75;
    if (creature_count == 0 && a2 < 192 && ((struct bf8_6_1 *)&D_001940D6)->f == 0 && D_0019628E == 0 && D_001996EC < 3 && ((int)(unsigned short)(a1->npc_flags & 32768)) == 0 && player_character->race < 9) {
        D_001996EC++;
        a1->image = (a1->image & -128) + 5;
        if ((a1->image >> 7) == 399) a1->image += 10;
        return 1;
    }
    a1->image &= ~0x7F;
    for (;;) {
        func_000CE6E2(a1->yaw, ((l_18 << 8) * D_00195AB0) / 1000, (int)&l_44, (int)&l_40);
        l_20 = func_0009DEAC((a1->x & 63) - 32);
        l_1C = func_0009DEAC((a1->z & 63) - 32);
        if (l_20 < 8 && l_1C < 8) {
            if ((rand() & 255) != 0) {
                l_3C = func_00040794((int)(a1->x + ((int)(short)*(short *)(D_0017B657 + ((a1->yaw >> 9) << 2)))), ((int)(short)*(short *)(D_0017B659 + ((a1->yaw >> 9) << 2))) + a1->z);
            } else {
                l_3C = 0;
            }
        } else {
            l_3C = 1;
        }
        l_38 = 0;
        l_2C = a1->yaw;
        if (l_3C == 0) a1->yaw = D_0017B64F[(rand() % 4)];
        while (l_3C == 0) {
            a1->yaw = (a1->yaw + 512) % 2048;
            func_000CE6E2(a1->yaw, (D_00195AB0 * 19200) / 1000, (int)&l_44, (int)&l_40);
            l_3C = func_00040794(((int)(short)*(short *)(D_0017B657 + ((a1->yaw >> 9) << 2))) + a1->x, ((int)(short)*(short *)(D_0017B659 + ((a1->yaw >> 9) << 2))) + a1->z);
            if (l_38++ > 8) {
                a1->yaw = l_2C;
                return 0;
            }
        }
        l_44 += a1->move_remainder & 255;
        l_40 += ((unsigned)(a1->move_remainder & 65280)) >> 8;
        a1->x += l_44 / 256;
        a1->z += l_40 / 256;
        a1->move_remainder = l_44 & 255;
        a1->move_remainder |= (l_40 & 255) << 8;
        if (a1->yaw == 1024 || a1->yaw == 0) {
            a1->x = (a1->x & -64) + 32;
        } else {
            a1->z = (a1->z & -64) + 32;
        }
        l_28 = func_000C808D(a1->x, a1->z, player_object->x, player_object->z);
        l_24 = (a1->yaw + 128) & 2047;
        l_2C = (l_24 - l_28) & 2047;
        l_2C >>= 8;
        if (l_2C > 4) {
            a1->flags |= 0x4000;
            l_2C = (int)(unsigned char)D_0017B62F[l_2C];
        } else {
            a1->flags &= ~0x4000;
        }
        a1->y = func_0014B45B(a1->x, a1->z);
        a1->image += l_2C;
        if (func_000406DC(a1, 64) != 0) break;
        a1->yaw = (a1->yaw + 512) % 2048;
        ++l_30;
        if (l_30 >= 8) break;
        l_18 += 100;
        a1->image -= l_2C;
    }
    return 1;
}

void person_place(struct record *a1)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct person *l_18;

    l_20 = 0;
    l_18 = &a1->data.person;
    a1->type = 53;
    a1->flags |= 1;
    a1->home_building = rand() % current_location->building_count;
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    a1->image2 = 0;
    a1->npc_flags = 0;
    l_18->faction_id = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15)->id;
    person_pick_sprite(a1);
    l_1C = D_000CEA24 >> 8;
    if (l_1C > 1536) {
        l_1C = 1536;
    } else {
        l_1C = 1024;
    }
    do {
        func_000CE6E2(rand_range(0, 2047), l_1C, &a1->x, &a1->z);
        a1->x += player_object->x;
        a1->z += player_object->z;
        a1->y = func_0014B45B(a1->x, a1->z);
        l_28 = func_0004083A(a1, a1->x - D_00195AC4->x, a1->z - D_00195AC4->z);
    } while (l_28 == 0 && l_20++ < 10);
    if (l_20 != 11) return;
    a1->z = 0;
    a1->y = a1->z;
    a1->x = a1->y;
}

int people_max_count(void)
{
    int l_1C;

    if (D_00195AC4->image == 65535) return 0;
    if (((int)player_environment) != 1) return 0;
    if (current_location->kind == 4 || current_location->kind == 7 || current_location->kind > 8) {
        return 0;
    }
    if (player_character->race > 8) return 0;
    if (creature_count != 0) return 0;
    if (location_contains(player_object->x, player_object->z) == 0) return 0;
    if (D_00199808 == 0) return 0;
    l_1C = (current_location->width * current_location->height) * 2;
    if (l_1C < 8) {
        l_1C = 8;
    } else if (l_1C > 30) {
        l_1C = 30;
    }
    if (((int)(unsigned char)climate_weathers[climate_category()]) >= 3) {
        l_1C >>= 1;
    }
    return l_1C;
}

void func_000405EB(void)
{
    if (D_00195AC4->image == 65535) {
        people_spawn_range = 1;
        return;
    }
    people_spawn_range = 3100;
}

int player_in_town_area(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = player_object->x - D_00195AC4->x;
    l_24 = player_object->z - D_00195AC4->z;
    l_20 = (current_location->width << 12) + 2048;
    l_1C = (current_location->height << 12) + 2048;
    if (l_28 < (-2048) || l_28 > l_20) return 0;
    if (l_24 < (-2048) || l_24 > l_1C) return 0;
    return 1;
}

int func_000406DC(struct record *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = a1->x;
    l_1C = a1->z;
    for (l_24 = 0; l_24 < people_count; l_24++) {
        if (people_list[l_24] == 0 || people_list[l_24] == a1) continue;
        l_18 = func_000C7FD9(l_20, l_1C, people_list[l_24]->x, people_list[l_24]->z);
        if (l_18 < a2) return 0;
    }
    return 1;
}

int func_00040794(int a1, int a2)
{
    if ((signed char)func_0002021B(a1, a2) == 0) return 0;
    a1 -= D_00195AC4->x;
    a2 -= D_00195AC4->z;
    if (a1 < 0 || a2 < 0) return 0;
    if ((current_location->width << 12) <= a1 || (current_location->height << 12) <= a2) return 0;
    return func_000404D9(a1, a2);
}

int func_0004083A(struct record *a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    if (a2 < 0 || a3 < 0) return 0;
    if ((current_location->width << 12) <= a2 || (current_location->height << 12) <= a3) return 0;
    if ((signed char)func_0002021B(a2 + D_00195AC4->x, a3 + D_00195AC4->z) == 0) return 0;
    if (func_000404D9(a2, a3) == 0) return 0;
    if (func_000406DC(a1, 64) == 0) return 0;
    if (func_00023DE0((int)a1, (int)&a1->x) != 0) return 0;
    l_1C = func_000C808D(player_object->x, player_object->z, a1->x, a1->z);
    l_18 = ai_angle_diff(player_object->yaw, l_1C, (int)&l_14);
    if (l_18 > 400) return 1;
    return func_00040983(a2, a3);
}

int func_00040983(int a1, int a2)
{
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    float l_20;
    float l_1C;
    unsigned char l_14;

    a1 >>= 6;
    a2 >>= 6;
    a2 = ((current_location->height << 6) - a2) - 1;
    l_38 = player_object->x - D_00195AC4->x;
    l_34 = player_object->z - D_00195AC4->z;
    l_38 >>= 6;
    l_34 >>= 6;
    l_34 = ((current_location->height << 6) - l_34) - 1;
    l_30 = a1 - l_38;
    l_2C = a2 - l_34;
    l_28 = ((func_0009DEAC(l_30) > func_0009DEAC(l_2C)) ? func_0009DEAC(l_30) : func_0009DEAC(l_2C));
    l_20 = (double)l_30 / l_28;
    l_1C = (double)l_2C / l_28;
    for (l_24 = 0; (l_28 + 1) > l_24; l_24++) {
        l_14 = *(signed char *)((char *)(int)(*(char **)&D_00196DA4 + (((current_location->width << 6) * l_34) + l_38)));
        if (l_14 != 0 && ((int)(unsigned char)l_14) < 100) return 1;
        l_38 += l_20;
        l_34 += l_1C;
    }
    return 0;
}

int guards_are_present(void)
{
    int l_1C;

    for (l_1C = 0; l_1C < creature_count; l_1C++) {
        if ((D_00190504[l_1C]->image >> 7) == 399 && D_00190504[l_1C]->data.character.team == 1) {
            return 1;
        }
    }
    return 0;
}

int is_guard_sprite(struct record *a1)
{
    return (((a1->image >> 7) == 399) ? 1 : 0);
}

int creatures_guard_mix(void)
{
    int l_20;
    int l_1C;

    if (creature_count == 0) return 0;
    if (((int)player_environment) != 1) return 2;
    l_1C = 0;
    l_20 = l_1C;
    for (; l_1C < creature_count; l_1C++) {
        if (D_00190504[l_1C]->data.character.mobile_id != 146) l_20++;
    }
    if (l_20 == 0) return 1;
    if (l_20 != creature_count) return 3;
    return 2;
}

void guard_spawn(struct record *a1)
{
    struct record *l_18;

    l_18 = object_create_child(player_object->parent, 0, 659);
    if (creature_count > 10) {
        object_delete(l_18);
        return;
    }
    l_18->type = 18;
    l_18->flags |= 1;
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    monster_init(l_18, 146);
    l_18->data.character.team = D_0017B6CD;
    if (a1 != 0) {
        l_18->x = a1->x;
        l_18->y = a1->y;
        l_18->z = a1->z;
    } else if (spawn_find_point(l_18, 512, 2048) == 0) {
        object_delete(l_18);
        l_18 = 0;
    }
    D_00195AA8 = l_18;
}

void guards_timer_tick(void)
{
    if (D_00178A14 < 0) return;
    if (D_00178A14 > 0) {
        D_00178A14--;
        return;
    }
    D_00178A14--;
    guards_summon(1);
}
