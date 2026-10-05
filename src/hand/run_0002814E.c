/* matched by the real Watcom C32 10.0a (-d2): a run of automap.c from 0x00027A10 to 0x0002814E, kept together for its switch table's alignment */
#include "records.h"

extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char key_down_esc[];
extern char key_down_up[];
extern char key_down_left[];
extern char key_down_right[];
extern char key_down_down[];
extern char screen_buffer[];
extern char D_00170794[];
extern char D_001707AE[];
extern char D_001707B8[];
extern char D_001707C3[];
extern char player_environment[];
extern char town_map_buttons[];
extern char D_0017A0BE[];
extern char D_0017A0C0[];
extern char D_0017A0C2[];
extern char D_0017A0C4[];
extern char D_0017A103[];
extern char D_0018507F[];
extern char text_buffer[];
extern char D_00190CE5[];
extern char text_macro_fpc[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern char game_minutes[];
extern char D_00195C44[];
extern char D_00196272[];
extern char mouse_buttons_prev[];
extern char D_00196D88[];
extern char D_00196D8C[];
extern char D_00196D90[];
extern char D_00196D94[];
extern char D_00196D98[];
extern char D_00196D9C[];
extern char D_00196DA4[];
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
#define SCR (*(unsigned char **)screen_buffer)
#define MAP (*(unsigned char **)D_00196DA4)
#define VX (*(int *)D_00196D88)
#define VY (*(int *)D_00196D8C)

void town_map_open(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = 0;
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) return;
    if (location_contains(player_object->x, player_object->z) != 0) goto L27A5D;
    hud_message_add(*(int *)D_0018507F);
    return;
L27A5D:;
    if (current_location->kind == 7) goto L27A81;
    if (current_location->kind != 4) goto L27A83;
L27A81:;
    goto L27A95;
L27A83:;
    if (current_location->kind < 9) goto L27AA4;
L27A95:;
    hud_message_add(*(int *)D_0018507F);
    return;
L27AA4:;
    *(int *)D_00196D98 = (*(int *)D_00196D90 = 0);
    *(int *)D_00196D94 = 0;
    *(signed char *)D_00190CE5 = 0;
    mc_memset(*(int *)D_00195C44, 0, 50000, (int)D_001707AE, 624, 4);
    func_000A0ED9(625, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, D_00195AC4->id >> 16);
    l_18 = disk_open_rw((int)text_buffer);
    if (l_18 == (-1)) goto L27B70;
    func_000A00CB(l_18, *(int *)D_00195C44, 50000);
    *(int *)(*(char **)D_00195C44) = *(int *)game_minutes;
    lseek(l_18, 0, 0);
    write(l_18, *(int *)D_00195C44, 4);
    func_0009DEA7(l_18);
L27B70:;
    *(signed char *)mouse_buttons = (*(signed char *)mouse_buttons_prev = 0);
    l_1C = (int)(unsigned char)*(signed char *)D_00196272;
    *(signed char *)D_00196272 = 1;
    *(int *)text_macro_fpc = disk_read_file((int)D_00170794, 0);
    *(int *)D_00196D9C = disk_read_file((int)D_001707C3, 0);
L27BB4:;
    if (l_24 != 0) goto L27D78;
    func_0012B2EB();
    town_map_draw();
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_00196D9C), (int)(unsigned short)*(short *)(*(char **)D_00196D9C + 2), (int)(unsigned short)*(short *)(*(char **)D_00196D9C + 4), (int)(unsigned short)*(short *)(*(char **)D_00196D9C + 6), *(int *)D_00196D9C + 12);
    func_0012B3ED();
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    func_0012B2D3((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y);
    if (*(signed char *)key_down_left == 0) goto L27C4A;
    automap_move_left(2);
    goto L27C84;
L27C4A:;
    if (*(signed char *)key_down_right == 0) goto L27C5F;
    automap_move_right(3);
    goto L27C84;
L27C5F:;
    if (*(signed char *)key_down_up == 0) goto L27C71;
    automap_move_forward(0);
    goto L27C84;
L27C71:;
    if (*(signed char *)key_down_down == 0) goto L27C84;
    automap_move_back(1);
L27C84:;
    if (*(signed char *)key_down_esc == 0) goto L27C94;
    l_24 = 1;
L27C94:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 3)) == 0) goto L27D64;
    l_20 = 0;
L27CAF:;
    if (l_20 < 6) goto L27CC2;
    goto L27D64;
L27CBA:;
    l_20++;
    goto L27CAF;
L27CC2:;
    if (*(short *)mouse_x <= *(short *)(town_map_buttons + (l_20 * 12))) goto L27CEA;
    if (*(short *)mouse_x < *(short *)(D_0017A0C0 + (l_20 * 12))) goto L27CEC;
L27CEA:;
    goto L27D00;
L27CEC:;
    if (*(short *)mouse_y > *(short *)(D_0017A0BE + (l_20 * 12))) goto L27D02;
L27D00:;
    goto L27D16;
L27D02:;
    if (*(short *)mouse_y < *(short *)(D_0017A0C2 + (l_20 * 12))) goto L27D18;
L27D16:;
    goto L27D5F;
L27D18:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L27D38;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L27D3A;
L27D38:;
    goto L27D4F;
L27D3A:;
    sound_play(203, player_object, 100);
L27D4F:;
    l_24 = ((int (*)())(*(int *)(D_0017A0C4 + (l_20 * 12))))(l_20);
L27D5F:;
    goto L27CBA;
L27D64:;
    screenshot_poll();
    func_000CDD81(1);
    goto L27BB4;
L27D78:;
    if (*(signed char *)key_down_esc != 0) goto L27D78;
    if (*(int *)text_macro_fpc == 0) goto L27D96;
    if (*(int *)text_macro_fpc != (-1751672937)) goto L27D98;
L27D96:;
    goto L27DB6;
L27D98:;
    mc_free(*(int *)text_macro_fpc, (int)D_001707AE, 679);
    *(int *)text_macro_fpc = -1751672937;
L27DB6:;
    if (*(int *)D_00196D9C == 0) goto L27DCB;
    if (*(int *)D_00196D9C != (-1751672937)) goto L27DCD;
L27DCB:;
    goto L27DEB;
L27DCD:;
    mc_free(*(int *)D_00196D9C, (int)D_001707AE, 680);
    *(int *)D_00196D9C = -1751672937;
L27DEB:;
    *(signed char *)D_00196272 = *(signed char *)&l_1C;
    if (*(signed char *)D_00190CE5 == 0) return;
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

    mc_memcpy(*(int *)screen_buffer, *(int *)text_macro_fpc, 64000, D_001707AE, 695, 4);
    l_30 = player_object->x - D_00195AC4->x;
    l_2C = player_object->z - D_00195AC4->z;
    l_30 >>= 6;
    l_2C >>= 6;
    l_2C = (current_location->height << 6) - l_2C - 1;
    VX = l_30;
    VY = l_2C;
    VX -= 37;
    VY -= 20;
    VX += *(int *)D_00196D98;
    VY += *(int *)D_00196D90;
    l_40 = current_location->height << 6;
    l_3C = current_location->width << 6;
    if (VX < 0) {
        *(int *)D_00196D98 -= VX;
        VX = 0;
    }
    if (VX > l_3C - 38) {
        *(int *)D_00196D98 -= VX - (l_3C - 38);
    }
    if (VY < 0) {
        *(int *)D_00196D90 -= VY;
        VY = 0;
    }
    if (VY > l_40 - 10) {
        *(int *)D_00196D90 -= VY - (l_40 - 10);
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
        (*(int *)D_00196D90)--;
        break;
    case 1:
        (*(int *)D_00196D90)++;
        break;
    case 2:
        (*(int *)D_00196D98)--;
        break;
    case 3:
        (*(int *)D_00196D98)++;
        break;
    }
}
