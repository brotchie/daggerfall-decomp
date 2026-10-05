/* matched by the real Watcom C32 10.0a (-d2): a run of automap.c from 0x00027A10 to 0x0002814E, kept together for its switch table's alignment */
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char key_down_esc;
extern signed char key_down_up;
extern signed char key_down_left;
extern signed char key_down_right;
extern signed char key_down_down;
extern int screen_buffer;
extern char D_00170794[];
extern char D_001707AE[];
extern char D_001707B8[];
extern char D_001707C3[];
extern unsigned char player_environment;
extern char town_map_buttons[];
extern char D_0017A0BE[];
extern char D_0017A0C0[];
extern char D_0017A0C2[];
extern char D_0017A0C4[];
extern char D_0017A103[];
extern int D_0018507F;
extern signed char text_buffer[];
extern signed char D_00190CE5;
extern char text_macro_fpc[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern int game_minutes;
extern char D_00195C44[];
extern signed char D_00196272;
extern signed char mouse_buttons_prev;
extern int D_00196D88;
extern int D_00196D8C;
extern int D_00196D90;
extern int D_00196D94;
extern int D_00196D98;
extern char D_00196D9C[];
extern int D_00196DA4;
extern void screenshot_poll(void);
extern int automap_move_forward(int);
extern int automap_move_back(int);
extern int automap_move_left(int);
extern int automap_move_right(int);
extern void town_map_draw(void);
extern int func_000281AF(void);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int disk_open_rw(int);
extern int hud_message_add(int);
extern int location_contains(int, int);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int func_000A00CB();
extern int write();
extern int func_000CDD81();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_00144F68();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern void func_000286F6(void);
extern int mc_memcpy();
extern void func_000A134C(short, short, int);
#pragma aux func_000A0ED9 parm routine [];

struct kb { unsigned char _:3; unsigned char f:1; };
#define SCR (*(unsigned char **)&screen_buffer)
#define MAP (*(unsigned char **)&D_00196DA4)
#define VX (D_00196D88)
#define VY (D_00196D8C)

void town_map_open(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = 0;
    if (((int)player_environment) != 1) return;
    if (location_contains(player_object->x, player_object->z) == 0) {
        hud_message_add(D_0018507F);
        return;
    }
    if (current_location->kind == 7 || current_location->kind == 4 || current_location->kind >= 9) {
        hud_message_add(D_0018507F);
        return;
    }
    D_00196D98 = (D_00196D90 = 0);
    D_00196D94 = 0;
    D_00190CE5 = 0;
    mc_memset(*(int *)D_00195C44, 0, 50000, (int)D_001707AE, 624, 4);
    func_000A0ED9(625, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, D_00195AC4->id >> 16);
    l_18 = disk_open_rw((int)text_buffer);
    if (l_18 != (-1)) {
        func_000A00CB(l_18, *(int *)D_00195C44, 50000);
        *(int *)(*(char **)D_00195C44) = game_minutes;
        lseek(l_18, 0, 0);
        write(l_18, *(int *)D_00195C44, 4);
        func_0009DEA7(l_18);
    }
    mouse_buttons = (mouse_buttons_prev = 0);
    l_1C = (int)(unsigned char)D_00196272;
    D_00196272 = 1;
    *(int *)text_macro_fpc = disk_read_file((int)D_00170794, 0);
    *(int *)D_00196D9C = disk_read_file((int)D_001707C3, 0);
    while (l_24 == 0) {
        func_0012B2EB();
        town_map_draw();
        func_00144F68((int)(unsigned short)*(short *)(*(char **)D_00196D9C), (int)(unsigned short)*(short *)(*(char **)D_00196D9C + 2), (int)(unsigned short)*(short *)(*(char **)D_00196D9C + 4), (int)(unsigned short)*(short *)(*(char **)D_00196D9C + 6), *(int *)D_00196D9C + 12);
        func_0012B3ED();
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
        if (key_down_left != 0) {
            automap_move_left(2);
        } else if (key_down_right != 0) {
            automap_move_right(3);
        } else if (key_down_up != 0) {
            automap_move_forward(0);
        } else if (key_down_down != 0) {
            automap_move_back(1);
        }
        if (key_down_esc != 0) l_24 = 1;
        if (((int)(unsigned char)(mouse_buttons & 3)) != 0) {
            for (l_20 = 0; l_20 < 6; l_20++) {
                if (mouse_x > *(short *)(town_map_buttons + (l_20 * 12)) && mouse_x < *(short *)(D_0017A0C0 + (l_20 * 12)) && mouse_y > *(short *)(D_0017A0BE + (l_20 * 12)) && mouse_y < *(short *)(D_0017A0C2 + (l_20 * 12))) {
                    if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 1)) == 0) {
                        sound_play(203, player_object, 100);
                    }
                    l_24 = ((int (*)())(*(int *)(D_0017A0C4 + (l_20 * 12))))(l_20);
                }
            }
        }
        screenshot_poll();
        func_000CDD81(1);
    }
    while (key_down_esc != 0);
    if (*(int *)text_macro_fpc != 0 && *(int *)text_macro_fpc != (-1751672937)) {
        mc_free(*(int *)text_macro_fpc, (int)D_001707AE, 679);
        *(int *)text_macro_fpc = -1751672937;
    }
    if (*(int *)D_00196D9C != 0 && *(int *)D_00196D9C != (-1751672937)) {
        mc_free(*(int *)D_00196D9C, (int)D_001707AE, 680);
        *(int *)D_00196D9C = -1751672937;
    }
    D_00196272 = *(signed char *)&l_1C;
    if (D_00190CE5 == 0) return;
    func_000A0ED9(686, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, D_00195AC4->id >> 16);
    disk_write_arena2_file((int)text_buffer, *(int *)D_00195C44, func_000281AF());
}

void town_map_draw(void)
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

    mc_memcpy(screen_buffer, *(int *)text_macro_fpc, 64000, D_001707AE, 695, 4);
    l_30 = player_object->x - D_00195AC4->x;
    l_2C = player_object->z - D_00195AC4->z;
    l_30 >>= 6;
    l_2C >>= 6;
    l_2C = (current_location->height << 6) - l_2C - 1;
    VX = l_30;
    VY = l_2C;
    VX -= 37;
    VY -= 20;
    VX += D_00196D98;
    VY += D_00196D90;
    l_40 = current_location->height << 6;
    l_3C = current_location->width << 6;
    if (VX < 0) {
        D_00196D98 -= VX;
        VX = 0;
    }
    if (VX > l_3C - 38) {
        D_00196D98 -= VX - (l_3C - 38);
    }
    if (VY < 0) {
        D_00196D90 -= VY;
        VY = 0;
    }
    if (VY > l_40 - 10) {
        D_00196D90 -= VY - (l_40 - 10);
    }
    l_24 = 10;
    l_38 = VY;
    while (l_24 < 170 && l_38 < l_40) {
        for (l_44 = 0; l_44 < 2; l_44++) {
            l_34 = VX;
            l_28 = 10;
            while (l_28 < 310 && l_34 < l_3C) {
                l_1C = MAP[l_3C * l_38 + l_34];
                if (!(l_1C == 0 || l_1C == 251 || l_1C == 250)) {
                    l_20 = D_0017A103[MAP[l_3C * l_38 + l_34]];
                    SCR[l_24 * 320 + l_28] = l_20;
                    SCR[l_24 * 320 + l_28 + 1] = l_20;
                }
                l_28 += 2;
                l_34++;
            }
            l_24++;
        }
        l_38++;
    }
    func_000286F6();
    if ((*(struct kb *)0x46c).f == 0) return;
    l_28 = (l_30 - VX) * 2 + 10;
    l_24 = (l_2C - VY) * 2 + 10;
    if (l_24 >= 169) return;
    func_000A134C(l_28, l_24, 145);
    func_000A134C(l_28 + 1, l_24, 145);
    func_000A134C(l_28, l_24 + 1, 145);
    func_000A134C(l_28 + 1, l_24 + 1, 145);
}

void town_map_scroll(int a1)
{
    switch (a1) {
    case 0:
        D_00196D90--;
        break;
    case 1:
        D_00196D90++;
        break;
    case 2:
        D_00196D98--;
        break;
    case 3:
        D_00196D98++;
        break;
    }
}
