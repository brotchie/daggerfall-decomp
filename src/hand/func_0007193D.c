/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007193D */
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern signed char key_down_enter;
extern char D_001760B1[];
extern char D_001760D6[];
extern char D_001760DD[];
extern char D_001760EA[];
extern char D_0017610F[];
extern char D_0017611C[];
extern int D_0017D1FA;
extern int D_0017D1FE;
extern signed char text_buffer[];
extern int D_00190CBC;
extern signed char D_00190D1A;
extern short D_00190DD0;
extern struct record *player_entity;
extern int creature_count;
extern char inpstr_result[];
extern struct character *player_character;
extern struct career *player_class;
extern char D_00195C44[];
extern unsigned char D_0019626F;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char D_00196294;
extern signed char D_00196299;
extern signed char D_0019629E;
extern int D_001A4A20;
extern char rest_image[];
extern signed char rest_loitering;
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void time_pass_minutes(int);
extern void hud_draw(void);
extern int disk_read_file(int, int);
extern void rest_recover(struct record *);
extern void rest_close(void);
extern int rest_allowed(void);
extern int rest_room_expired(void);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void inpstr_begin_number(int);
extern int point_in_rect(short, short, short, short, short, short);
extern int mc_free();
extern int func_000A0DD9();
extern int func_000A0DF4();
extern int func_0012B136();
extern int func_00143914();
extern int func_00144F68();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);

void rest_update(void)
{
    int l_34;
    int l_30;


    D_0019629E = 0;
    if (((int)(unsigned char)game_mode) != 16) {
        if (((int)D_0019626F) != 16 || ((int)(unsigned char)game_mode) != 8) {
            return;
        }
    }
    func_00143914(0);
    hud_draw();
    D_0019629E = 1;
    D_0012B508 = 146;
    if (rest_room_expired() != 0) {
        msgbox_show_string((int)D_001760B1, 1);
        rest_close();
        return;
    }
    if (creature_count != 0 || D_00196299 != 0) {
        msgbox_show_rsc(354, 1);
        rest_close();
        return;
    }
    if (key_down_esc != 0) {
        key_down_esc = 0;
        rest_close();
        return;
    }
    if (D_00190D1A == 0) {
        func_00144F68((int)(unsigned short)*(short *)(*(char **)rest_image), (int)(unsigned short)*(short *)(*(char **)rest_image + 2), (int)(unsigned short)*(short *)(*(char **)rest_image + 4), (int)(unsigned short)*(short *)(*(char **)rest_image + 6), (int)(*(char **)rest_image + 12));
        if (mouse_buttons != 0 && mouse_buttons_prev == 0 && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 87, 63, 134, 86) != 0) {
            while (mouse_buttons != 0) func_0012B136();
            if (rest_allowed() == 0) {
                rest_close();
                return;
            }
            D_001A4A20 = 9;
            D_00190D1A = 1;
            if (*(int *)rest_image != 0 && *(int *)rest_image != (-1751672937)) {
                mc_free(*(int *)rest_image, (int)D_001760D6, 165);
                *(int *)rest_image = -1751672937;
            }
            *(int *)rest_image = disk_read_file((int)D_001760DD, 0);
            l_30 = (int)(*(char **)D_00195C44 + 55000);
            func_000A0ED9(168, (int)D_001760D6);
            mc_sprintf(l_30, (int)D_001760EA, D_0017D1FA);
            *(signed char *)((char *)(func_000A0DF4(l_30) + l_30) + 1) = 0;
            inpstr_begin_number(0);
            msgbox_show_string(l_30, 2);
            while (key_down_enter != 0);
        } else if (mouse_buttons != 0 && mouse_buttons_prev == 0 && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 136, 63, 183, 86) != 0) {
            while (mouse_buttons != 0) func_0012B136();
            if (rest_allowed() == 0) {
                rest_close();
                return;
            }
            D_001A4A20 = 9;
            D_00190D1A = 3;
            if (*(int *)rest_image != 0 && *(int *)rest_image != (-1751672937)) {
                mc_free(*(int *)rest_image, (int)D_001760D6, 184);
                *(int *)rest_image = -1751672937;
            }
            *(int *)rest_image = disk_read_file((int)D_0017610F, 0);
            D_00190CBC = *(int *)1132;
        } else if (mouse_buttons != 0 && mouse_buttons_prev == 0 && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 185, 63, 232, 86) != 0) {
            while (mouse_buttons != 0) func_0012B136();
            D_001A4A20 = 32;
            D_00190D1A = 1;
            if (*(int *)rest_image != 0 && *(int *)rest_image != (-1751672937)) {
                mc_free(*(int *)rest_image, (int)D_001760D6, 193);
                *(int *)rest_image = -1751672937;
            }
            *(int *)rest_image = disk_read_file((int)D_001760DD, 0);
            l_30 = (int)(*(char **)D_00195C44 + 55000);
            func_000A0ED9(196, (int)D_001760D6);
            mc_sprintf(l_30, (int)D_0017611C, D_0017D1FE);
            *(signed char *)((char *)(func_000A0DF4(l_30) + l_30) + 1) = 0;
            inpstr_begin_number(0);
            msgbox_show_string(l_30, 2);
            rest_loitering = 1;
            D_00196294 = 1;
            while (key_down_enter != 0);
        }
        return;
    }
    if (((int)(signed char)D_00190D1A) == 1) {
        if (((int)(unsigned char)game_mode) != 8) {
            if (rest_loitering != 0) {
                l_34 = 3;
            } else {
                l_34 = 99;
            }
            if (*(int *)inpstr_result > l_34) {
                key_down_enter = 0;
                msgbox_show_rsc((int)(short)((rest_loitering != 0) ? 27 : 26), 1);
                rest_close();
            } else {
                D_00190DD0 = *(short *)inpstr_result;
                D_00190D1A = 2;
                D_00190CBC = *(int *)1132;
            }
        }
        return;
    }
    if (((int)(signed char)D_00190D1A) == 2) {
        func_00144F68((int)(unsigned short)*(short *)(*(char **)rest_image), (int)(unsigned short)*(short *)(*(char **)rest_image + 2), (int)(unsigned short)*(short *)(*(char **)rest_image + 4), (int)(unsigned short)*(short *)(*(char **)rest_image + 6), (int)(*(char **)rest_image + 12));
        text_draw_centered_colored(func_000A0DD9((int)(short)D_00190DD0, (int)text_buffer, 10), 118, 62, 146, 156);
        if (mouse_buttons != 0 && mouse_buttons_prev == 0 && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 140, 76, 179, 85) != 0) {
            rest_close();
        }
        if (((unsigned)(*(int *)1132 - D_00190CBC)) > D_001A4A20) {
            time_pass_minutes(60);
            if (rest_loitering == 0) rest_recover(player_entity);
            D_00190DD0--;
            D_00190CBC = *(int *)1132;
        }
        if (D_00190DD0 == 0) {
            msgbox_show_rsc((int)(short)((rest_loitering != 0) ? 349 : 353), 1);
            rest_close();
        }
        return;
    }
    if (((int)(signed char)D_00190D1A) != 3) return;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)rest_image), (int)(unsigned short)*(short *)(*(char **)rest_image + 2), (int)(unsigned short)*(short *)(*(char **)rest_image + 4), (int)(unsigned short)*(short *)(*(char **)rest_image + 6), (int)(*(char **)rest_image + 12));
    text_draw_centered_colored(func_000A0DD9((int)(short)D_00190DD0, (int)text_buffer, 10), 118, 62, 146, 156);
    if (mouse_buttons != 0 && mouse_buttons_prev == 0 && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 140, 76, 179, 85) != 0) {
        rest_close();
    } else if (((unsigned)(*(int *)1132 - D_00190CBC)) > D_001A4A20) {
        time_pass_minutes(60);
        rest_recover(player_entity);
        D_00190DD0++;
        D_00190CBC = *(int *)1132;
    }
    if (player_character->health != player_character->max_health || (player_character->magicka != player_character->max_magicka && (player_class->flags & 8) == 0) || player_character->fatigue != ((player_character->attributes[0] + player_character->attributes[4]) << 6)) {
        return;
    }
    msgbox_show_rsc(350, 1);
    rest_close();
}
