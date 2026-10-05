/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003A3CE */
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern int screen_buffer;
extern signed char D_00147964;
extern char D_00170B88[];
extern char D_00170B90[];
extern char D_00170B9D[];
extern char D_00170BAD[];
extern char D_00170BB5[];
extern char D_00170BC2[];
extern char D_00170BE6[];
extern char D_00170BF3[];
extern char D_00170C00[];
extern char D_00170C0E[];
extern int D_0017B4B0;
extern signed char D_0017B4B7[];
extern char D_0017CCFA[];
extern signed char text_buffer[];
extern signed char D_00190D16;
extern char D_001917E4[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern struct character *player_character;
extern struct career *player_class;
extern int D_00195C40;
extern int D_00195C44;
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern unsigned char D_00196271;
extern signed char D_00196273;
extern signed char mouse_buttons_prev;
extern int D_00199634;
extern void career_background_summary(int, short);
extern void intro_play_movie(void);
extern int chargen_popup_choice(int, unsigned char, unsigned char, int, unsigned char, unsigned char);
extern void func_0003B1D6(void);
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void quests_start_initial(void);
extern void newgame_place_player(void);
extern void palette_restore(void);
extern void game_exit(int);
extern void newgame_init_player(void);
extern int class_questions_run(void);
extern void classmaker_run(void);
extern int sound_play_ui(int);
extern int disk_read_file(int, int);
extern void automap_delete_files(void);
extern void saveload_menu(int);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern int wait_key_from_list(int, int);
extern void picklist_open(int);
extern int picklist_update(void);
extern int point_in_rect();
extern int chargen_name_character(void);
extern int mc_free();
extern int mc_memset();
extern int unlink();
extern int mc_memcpy();
extern int func_000CD33A();
extern int func_000CD367();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_00143914();
extern int func_00144F68();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);

void title_menu(void)
{
    char l_u0;                  /* unused, but they have slots */
    int l_3C;
    short l_28;
    char l_u1;
    signed char l_18;
    short l_u2;
    short l_u3;
    int l_u4;
    unsigned char l_1C;

    while (mouse_buttons != 0) func_0012B136();
L3A3EC:;
    if (mouse_buttons != 0) {
        func_0012B136();
        goto L3A3EC;
    }
    disk_read_file((int)D_00170B90, D_00195C44);
    mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 55, 4);
    mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 56, 4);
    for (l_28 = 0; ((int)(short)*(short *)&l_28) < 768; l_28++) {
        (*(char **)&D_00195C44)[l_28 + 64000] <<= 2;
    }
    func_000CD33A(D_00195C44 + 64000, 0, 256);
    D_00147964 &= 254;
L3A496:;
    while (1) {
        if (mouse_buttons != 0) {
            func_0012B136();
            goto L3A496;
        }
        D_001940D4 &= 254;
        l_18 = wait_key_from_list(D_0017B4B0, 3);
        if (((int)(signed char)l_18) == (-1) || ((int)(signed char)l_18) == 2 || (((int)(signed char)l_18) == (-2) && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 125, 145, 165, 158) != 0)) {
            game_exit(0);
        }
        if (l_18 == 0 || (((int)(signed char)l_18) == (-2) && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 72, 46, 217, 58) != 0)) {
            mc_memset(655360, 0, 64000, (int)D_00170B88, 72, 4);
            palette_restore();
            saveload_menu(0);
            if (D_00190D16 != 0) goto L3A3EC;
            return;
        }
        if (((int)(signed char)l_18) == 1 || (((int)(signed char)l_18) == (-2) && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 74, 100, 217, 112) != 0)) {
            break;
        }
    }
    D_00196273 = 1;
    func_000A0ED9(97, (int)D_00170B88);
    mc_sprintf((int)text_buffer, (int)D_00170B9D, (int)D_001917E4);
    unlink((int)text_buffer);
    D_00196273 = 0;
    mc_memset((int)player_character, 0, 560, (int)D_00170B88, 122, 4);
    player_character->pad83 = 1;
    player_character->reflexes = 2;
    player_character->mobile_id = 200;
    mc_memset((int)player_class, 0, 74, (int)D_00170B88, 126, 4);
    player_character->level = 1;
    l_1C = 1;
    automap_delete_files();
    mc_memset(655360, 0, 64000, (int)D_00170B88, 139, 4);
    palette_restore();
    do {
        D_00196271 = 0;
        mc_memset((int)player_character, 0, 560, (int)D_00170B88, 148, 4);
        player_character->pad83 = 1;
        player_character->reflexes = 2;
        player_character->mobile_id = 200;
        mc_memset((int)player_class, 0, 74, (int)D_00170B88, 152, 4);
        disk_read_file((int)D_00170BAD, D_00195C44);
        for (l_28 = 0; ((int)(short)*(short *)&l_28) < 768; l_28++) {
            *(signed char *)((char *)(int)(*(char **)&D_00195C44 + ((int)(short)*(short *)&l_28))) <<= 2;
        }
        func_000CD367(D_00195C44);
        while (((int)D_00196271) != 1) {
            disk_read_file((int)D_00170BB5, D_00195C44);
            mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 161, 4);
            text_draw_centered_colored((int)D_00170BC2, 160, 16, 145, 156);
            disk_read_file((int)D_00170BE6, D_00195C44);
            D_00147964 &= 254;
            D_001940D4 |= 1;
            l_18 = 255;
            while (l_18 < 0) {
                while (((int)(signed char)l_18) != (-2)) {
                    l_18 = wait_key_from_list(0, 0);
                    if (((int)(signed char)l_18) == (-1)) goto L3A3EC;
                }
                l_18 = *(signed char *)((char *)(int)(*(char **)&D_00195C44 + (mouse_y * 320 + mouse_x)));
                if (l_18 == 0) l_18 = 255;
                if (l_18 > 0) {
                    while (mouse_buttons != 0) func_0012B136();
                    D_0012B508 = 146;
                    msgbox_button_ids = 4;
                    D_00196090 = 5;
                    msgbox_button_keys = 21;
                    D_00196034 = 49;
                    disk_read_file((int)D_00170BB5, D_00195C44);
                    mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 186, 4);
                    mouse_buttons = (mouse_buttons_prev = 0);
                    func_0012B136();
                    while (mouse_buttons != 0) func_0012B136();
                    sound_play_ui(((int)(signed char)l_18) + 208);
                    D_001940D5 |= 128;
                    msgbox_show_rsc((int)(short)(((unsigned short)(unsigned char)D_0017B4B7[(int)(signed char)l_18]) + 2000), 5);
                }
            }
            player_character->race = l_18 - 1;
        }
        if (chargen_popup_choice(2200, 7, 8, (int)D_00170BB5, 50, 33) == 0) {
            player_character->flags &= ~0x1;
        } else {
            player_character->flags |= 1;
        }
        disk_read_file((int)D_00170BB5, D_00195C44);
        mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 213, 4);
        l_3C = disk_read_file((int)D_00170BF3, 0);
        func_00144F68(68, 28, 184, 144, l_3C);
        if (l_3C != 0 && l_3C != (-1751672937)) {
            mc_free(l_3C, (int)D_00170B88, 217);
            l_3C = -1751672937;
        }
        while (mouse_buttons != 0) func_0012B136();
        D_00147964 &= 254;
        while (1) {
            keys_world_actions();
            func_0012B136();
            func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
            mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 226, 4);
            if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                if (((int)(short)mouse_x) > 68 && ((int)(short)mouse_x) < 251 && ((int)(short)mouse_y) > 60 && ((int)(short)mouse_y) < 111) {
                    l_28 = 2;
                    break;
                }
                if (((int)(short)mouse_x) > 68 && ((int)(short)mouse_x) < 251 && ((int)(short)mouse_y) > 112 && ((int)(short)mouse_y) < 171) {
                    l_28 = 1;
                    break;
                }
            }
        }
        if (((int)(short)*(short *)&l_28) == 2) {
L3AABE:;
            disk_read_file((int)D_00170BAD, D_00195C44);
            for (l_28 = 0; ((int)(short)*(short *)&l_28) < 768; l_28++) {
                *(signed char *)((char *)(int)(*(char **)&D_00195C44 + ((int)(short)*(short *)&l_28))) <<= 2;
            }
            func_000CD367(D_00195C44);
            disk_read_file((int)D_00170BB5, D_00195C44);
            mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 251, 4);
            picklist_open((int)D_0017CCFA);
            for (;;) {
                keys_world_actions();
                func_0012B136();
                mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 258, 4);
                l_28 = picklist_update();
                if (((int)(short)*(short *)&l_28) > (-1)) {
                    D_00199634 = (int)(short)*(short *)&l_28;
                    if (((int)(short)*(short *)&l_28) == 18) {
                        classmaker_run();
                        break;
                    }
                    sound_play_ui(217);
                    if (chargen_popup_choice((int)(short)(l_28 + 2100), 4, 5, (int)D_00170BB5, 21, 49) == 0) {
                        func_000A0ED9(272, (int)D_00170B88);
                        mc_sprintf((int)text_buffer, (int)D_00170C00, (int)(short)*(short *)&l_28);
                        disk_read_file((int)text_buffer, (int)player_class);
                        break;
                    }
                    disk_read_file((int)D_00170BB5, D_00195C44);
                    mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 278, 4);
                    picklist_open((int)D_0017CCFA);
                }
                D_00147964 &= 254;
                func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
                mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 283, 4);
            }
        } else {
            if ((D_00199634 = class_questions_run()) < 0) goto L3AABE;
            disk_read_file((int)D_00170BAD, D_00195C44);
            for (l_28 = 0; ((int)(short)*(short *)&l_28) < 768; l_28++) {
                *(signed char *)((char *)(int)(*(char **)&D_00195C44 + ((int)(short)*(short *)&l_28))) <<= 2;
            }
            func_000CD367(D_00195C44);
        }
        disk_read_file((int)D_00170BB5, D_00195C44);
        mc_memcpy(screen_buffer, D_00195C44, 64000, (int)D_00170B88, 299, 4);
        l_3C = disk_read_file((int)D_00170C0E, 0);
        func_00144F68((int)(unsigned short)*(short *)((char *)l_3C), (int)(unsigned short)*(short *)((char *)l_3C + 2), (int)(unsigned short)*(short *)((char *)l_3C + 4), (int)(unsigned short)*(short *)((char *)l_3C + 6), l_3C + 12);
        if (l_3C != 0 && l_3C != (-1751672937)) {
            mc_free(l_3C, (int)D_00170B88, 303);
            l_3C = -1751672937;
        }
        while (mouse_buttons != 0) func_0012B136();
        D_00147964 &= 254;
        while (1) {
            keys_world_actions();
            func_0012B136();
            func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
            mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 311, 4);
            if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                if (((int)(short)mouse_x) > 76 && ((int)(short)mouse_x) < 243 && ((int)(short)mouse_y) > 58 && ((int)(short)mouse_y) < 111) {
                    l_28 = 0;
                    break;
                }
                if (((int)(short)mouse_x) > 76 && ((int)(short)mouse_x) < 243 && ((int)(short)mouse_y) > 130 && ((int)(short)mouse_y) < 176) {
                    l_28 = 1;
                    break;
                }
            }
        }
        player_character->gold = 100;
        career_background_summary(D_00199634, (int)(short)*(short *)&l_28);
        palette_restore();
    } while (chargen_name_character() != 0);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 335, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_00170B88, 336, 4);
    intro_play_movie();
    func_0003B1D6();
    newgame_init_player();
    func_00143914(0);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 343, 4);
    palette_restore();
    newgame_place_player();
    D_00195C40 = *(int *)((char *)1132);
    quests_start_initial();
}
