/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003A3CE */
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern int screen_buffer;
extern signed char xn_mouse_cursor_drawn;
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
extern signed char scratch_190d16;
extern char arena2_path[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern struct character *player_character;
extern struct career *player_class;
extern int realtime_clock_tick;
extern char scratch_buffer[];
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern unsigned char D_00196271;
extern signed char D_00196273;
extern signed char mouse_buttons_prev;
extern int D_00199634;
extern void career_background_summary(int, int);
extern void intro_play_movie(void);
extern int chargen_popup_choice(short, short, short, char *, unsigned char, unsigned char);
extern void chargen_give_starting_equipment(void);
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void quests_start_initial(void);
extern void newgame_place_player(void);
extern void palette_restore(void);
extern void game_exit(char *);
extern void newgame_init_player(void);
extern int class_questions_run(void);
extern void classmaker_run(void);
extern int sound_play_ui(int);
extern int disk_read_file(char *, int);
extern void automap_delete_files(void);
extern void saveload_menu(int);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern int wait_key_from_list(int, int);
extern void list_popup_open(int);
extern int list_popup_update(void);
extern int point_in_rect();
extern int chargen_name_character(void);
extern int mc_free();
extern int mc_memset();
extern int unlink();
extern int mc_memcpy();
extern int xn_pal_set_range_8bit();
extern int xn_pal_set_all_8bit();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_gfx_clear();
extern int xn_draw_image();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);

void title_menu(void)
{
    char unused1;                  /* unused, but they have slots */
    struct image *image;
    short choice;
    char unused2;
    signed char pick;
    short unused3;
    short unused4;
    int unused5;
    unsigned char new_game;

    while (mouse_buttons != 0) xn_mouse_poll_clamped();
L3A3EC:;
    if (mouse_buttons != 0) {
        xn_mouse_poll_clamped();
        goto L3A3EC;
    }
    disk_read_file(D_00170B90, *(int *)scratch_buffer);
    mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 55, 4);
    mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 56, 4);
    for (choice = 0; ((int)(short)*(short *)&choice) < 768; choice++) {
        (*(char **)scratch_buffer)[choice + 64000] <<= 2;
    }
    xn_pal_set_range_8bit(*(int *)scratch_buffer + 64000, 0, 256);
    xn_mouse_cursor_drawn &= 254;
L3A496:;
    while (1) {
        if (mouse_buttons != 0) {
            xn_mouse_poll_clamped();
            goto L3A496;
        }
        D_001940D4 &= 254;
        pick = wait_key_from_list(D_0017B4B0, 3);
        if (((int)(signed char)pick) == (-1) || ((int)(signed char)pick) == 2 || (((int)(signed char)pick) == (-2) && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 125, 145, 165, 158) != 0)) {
            game_exit(0);
        }
        if (pick == 0 || (((int)(signed char)pick) == (-2) && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 72, 46, 217, 58) != 0)) {
            mc_memset(655360, 0, 64000, (int)D_00170B88, 72, 4);
            palette_restore();
            saveload_menu(0);
            if (scratch_190d16 != 0) goto L3A3EC;
            return;
        }
        if (((int)(signed char)pick) == 1 || (((int)(signed char)pick) == (-2) && point_in_rect((int)(short)mouse_x, (int)(short)mouse_y, 74, 100, 217, 112) != 0)) {
            break;
        }
    }
    D_00196273 = 1;
    mc_set_location(97, (int)D_00170B88);
    mc_sprintf((int)text_buffer, (int)D_00170B9D, (int)arena2_path);
    unlink((int)text_buffer);
    D_00196273 = 0;
    mc_memset((int)player_character, 0, 560, (int)D_00170B88, 122, 4);
    player_character->pad83 = 1;
    player_character->reflexes = 2;
    player_character->mobile_id = 200;
    mc_memset((int)player_class, 0, 74, (int)D_00170B88, 126, 4);
    player_character->level = 1;
    new_game = 1;
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
        disk_read_file(D_00170BAD, *(int *)scratch_buffer);
        for (choice = 0; ((int)(short)*(short *)&choice) < 768; choice++) {
            *(signed char *)((char *)(int)(*(char **)scratch_buffer + ((int)(short)*(short *)&choice))) <<= 2;
        }
        xn_pal_set_all_8bit(*(int *)scratch_buffer);
        while (((int)D_00196271) != 1) {
            disk_read_file(D_00170BB5, *(int *)scratch_buffer);
            mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 161, 4);
            text_draw_centred_coloured((int)D_00170BC2, 160, 16, 145, 156);
            disk_read_file(D_00170BE6, *(int *)scratch_buffer);
            xn_mouse_cursor_drawn &= 254;
            D_001940D4 |= 1;
            pick = 255;
            while (pick < 0) {
                while (((int)(signed char)pick) != (-2)) {
                    pick = wait_key_from_list(0, 0);
                    if (((int)(signed char)pick) == (-1)) goto L3A3EC;
                }
                pick = *(signed char *)((char *)(int)(*(char **)scratch_buffer + (mouse_y * 320 + mouse_x)));
                if (pick == 0) pick = 255;
                if (pick > 0) {
                    while (mouse_buttons != 0) xn_mouse_poll_clamped();
                    D_0012B508 = 146;
                    msgbox_button_ids = 4;
                    D_00196090 = 5;
                    msgbox_button_keys = 21;
                    D_00196034 = 49;
                    disk_read_file(D_00170BB5, *(int *)scratch_buffer);
                    mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 186, 4);
                    mouse_buttons = (mouse_buttons_prev = 0);
                    xn_mouse_poll_clamped();
                    while (mouse_buttons != 0) xn_mouse_poll_clamped();
                    sound_play_ui(((int)(signed char)pick) + 208);
                    D_001940D5 |= 128;
                    msgbox_show_rsc((int)(short)(((unsigned short)(unsigned char)D_0017B4B7[(int)(signed char)pick]) + 2000), 5);
                }
            }
            player_character->race = pick - 1;
        }
        if (chargen_popup_choice(2200, 7, 8, D_00170BB5, 50, 33) == 0) {
            player_character->flags &= ~0x1;
        } else {
            player_character->flags |= 1;
        }
        disk_read_file(D_00170BB5, *(int *)scratch_buffer);
        mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 213, 4);
        image = (struct image *)disk_read_file(D_00170BF3, 0);
        xn_draw_image(68, 28, 184, 144, image);
        if (image != 0 && (int)image != (-1751672937)) {
            mc_free(image, (int)D_00170B88, 217);
            image = (struct image *)-1751672937;
        }
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        xn_mouse_cursor_drawn &= 254;
        while (1) {
            keys_world_actions();
            xn_mouse_poll_clamped();
            xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
            mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 226, 4);
            if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                if (((int)(short)mouse_x) > 68 && ((int)(short)mouse_x) < 251 && ((int)(short)mouse_y) > 60 && ((int)(short)mouse_y) < 111) {
                    choice = 2;
                    break;
                }
                if (((int)(short)mouse_x) > 68 && ((int)(short)mouse_x) < 251 && ((int)(short)mouse_y) > 112 && ((int)(short)mouse_y) < 171) {
                    choice = 1;
                    break;
                }
            }
        }
        if (((int)(short)*(short *)&choice) == 2) {
L3AABE:;
            disk_read_file(D_00170BAD, *(int *)scratch_buffer);
            for (choice = 0; ((int)(short)*(short *)&choice) < 768; choice++) {
                *(signed char *)((char *)(int)(*(char **)scratch_buffer + ((int)(short)*(short *)&choice))) <<= 2;
            }
            xn_pal_set_all_8bit(*(int *)scratch_buffer);
            disk_read_file(D_00170BB5, *(int *)scratch_buffer);
            mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 251, 4);
            list_popup_open((int)D_0017CCFA);
            for (;;) {
                keys_world_actions();
                xn_mouse_poll_clamped();
                mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 258, 4);
                choice = list_popup_update();
                if (((int)(short)*(short *)&choice) > (-1)) {
                    D_00199634 = (int)(short)*(short *)&choice;
                    if (((int)(short)*(short *)&choice) == 18) {
                        classmaker_run();
                        break;
                    }
                    sound_play_ui(217);
                    if (chargen_popup_choice((int)(short)(choice + 2100), 4, 5, D_00170BB5, 21, 49) == 0) {
                        mc_set_location(272, (int)D_00170B88);
                        mc_sprintf((int)text_buffer, (int)D_00170C00, (int)(short)*(short *)&choice);
                        disk_read_file(text_buffer, (int)player_class);
                        break;
                    }
                    disk_read_file(D_00170BB5, *(int *)scratch_buffer);
                    mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 278, 4);
                    list_popup_open((int)D_0017CCFA);
                }
                xn_mouse_cursor_drawn &= 254;
                xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
                mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 283, 4);
            }
        } else {
            if ((D_00199634 = class_questions_run()) < 0) goto L3AABE;
            disk_read_file(D_00170BAD, *(int *)scratch_buffer);
            for (choice = 0; ((int)(short)*(short *)&choice) < 768; choice++) {
                *(signed char *)((char *)(int)(*(char **)scratch_buffer + ((int)(short)*(short *)&choice))) <<= 2;
            }
            xn_pal_set_all_8bit(*(int *)scratch_buffer);
        }
        disk_read_file(D_00170BB5, *(int *)scratch_buffer);
        mc_memcpy(screen_buffer, *(int *)scratch_buffer, 64000, (int)D_00170B88, 299, 4);
        image = (struct image *)disk_read_file(D_00170C0E, 0);
        xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
        if (image != 0 && (int)image != (-1751672937)) {
            mc_free(image, (int)D_00170B88, 303);
            image = (struct image *)-1751672937;
        }
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        xn_mouse_cursor_drawn &= 254;
        while (1) {
            keys_world_actions();
            xn_mouse_poll_clamped();
            xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
            mc_memcpy(655360, screen_buffer, 64000, (int)D_00170B88, 311, 4);
            if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                if (((int)(short)mouse_x) > 76 && ((int)(short)mouse_x) < 243 && ((int)(short)mouse_y) > 58 && ((int)(short)mouse_y) < 111) {
                    choice = 0;
                    break;
                }
                if (((int)(short)mouse_x) > 76 && ((int)(short)mouse_x) < 243 && ((int)(short)mouse_y) > 130 && ((int)(short)mouse_y) < 176) {
                    choice = 1;
                    break;
                }
            }
        }
        player_character->gold = 100;
        career_background_summary(D_00199634, (int)(short)*(short *)&choice);
        palette_restore();
    } while (chargen_name_character() != 0);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 335, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_00170B88, 336, 4);
    intro_play_movie();
    chargen_give_starting_equipment();
    newgame_init_player();
    xn_gfx_clear(0);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 343, 4);
    palette_restore();
    newgame_place_player();
    realtime_clock_tick = *(int *)((char *)1132);
    quests_start_initial();
}
