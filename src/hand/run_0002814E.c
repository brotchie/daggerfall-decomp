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
extern struct rect town_map_buttons[];
extern char D_0017A103[];
extern int D_0018507F;
extern signed char text_buffer[];
extern signed char scratch_190ce5;
extern char scratch_190de4[];
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern int game_minutes;
extern char scratch_buffer[];
extern signed char D_00196272;
extern signed char mouse_buttons_prev;
extern int town_map_view_x;
extern int town_map_view_y;
extern int D_00196D90;
extern int D_00196D94;
extern int D_00196D98;
extern struct image *D_00196D9C;
extern int D_00196DA4;
extern void screenshot_poll(void);
extern int automap_move_forward(int);
extern int automap_move_back(int);
extern int automap_move_left(int);
extern int automap_move_right(int);
extern void town_map_draw(void);
extern int town_notes_size(void);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern int disk_write_arena2_file(char *, int, int);
extern int disk_open_rw(char *);
extern int hud_message_add(int);
extern int location_contains(int, int);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int read();
extern int write();
extern int xn_gfx_present_inclusive();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern int xn_draw_image();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern void town_map_draw_notes(void);
extern int mc_memcpy();
extern void func_000A134C(short, short, int);
#pragma aux mc_set_location parm routine [];

struct kb { unsigned char _:3; unsigned char f:1; };
#define SCR (*(unsigned char **)&screen_buffer)
#define MAP (*(unsigned char **)&D_00196DA4)
#define VX (town_map_view_x)
#define VY (town_map_view_y)

void town_map_open(void)
{
    int done;
    int i;
    int saved_screen_active;
    int handle;

    done = 0;
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
    scratch_190ce5 = 0;
    mc_memset(*(int *)scratch_buffer, 0, 50000, (int)D_001707AE, 624, 4);
    mc_set_location(625, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, location_object->id >> 16);
    handle = disk_open_rw(text_buffer);
    if (handle != (-1)) {
        read(handle, *(int *)scratch_buffer, 50000);
        *(int *)(*(char **)scratch_buffer) = game_minutes;
        lseek(handle, 0, 0);
        write(handle, *(int *)scratch_buffer, 4);
        close(handle);
    }
    mouse_buttons = (mouse_buttons_prev = 0);
    saved_screen_active = (int)(unsigned char)D_00196272;
    D_00196272 = 1;
    *(int *)scratch_190de4 = disk_read_file(D_00170794, 0);
    D_00196D9C = (struct image *)disk_read_file(D_001707C3, 0);
    while (done == 0) {
        xn_mouse_cursor_erase();
        town_map_draw();
        xn_draw_image(D_00196D9C->x, D_00196D9C->y, D_00196D9C->width, D_00196D9C->height, (int)D_00196D9C->pixels);
        xn_mouse_cursor_draw();
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        if (key_down_left != 0) {
            automap_move_left(2);
        } else if (key_down_right != 0) {
            automap_move_right(3);
        } else if (key_down_up != 0) {
            automap_move_forward(0);
        } else if (key_down_down != 0) {
            automap_move_back(1);
        }
        if (key_down_esc != 0) done = 1;
        if (((int)(unsigned char)(mouse_buttons & 3)) != 0) {
            for (i = 0; i < 6; i++) {
                if (mouse_x > town_map_buttons[i].x0 && mouse_x < town_map_buttons[i].x1 && mouse_y > town_map_buttons[i].y0 && mouse_y < town_map_buttons[i].y1) {
                    if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 1)) == 0) {
                        sound_play(203, player_object, 100);
                    }
                    done = town_map_buttons[i].handler(i);
                }
            }
        }
        screenshot_poll();
        xn_gfx_present_inclusive(1);
    }
    while (key_down_esc != 0);
    if (*(int *)scratch_190de4 != 0 && *(int *)scratch_190de4 != (-1751672937)) {
        mc_free(*(int *)scratch_190de4, (int)D_001707AE, 679);
        *(int *)scratch_190de4 = -1751672937;
    }
    if ((int)D_00196D9C != 0 && (int)D_00196D9C != (-1751672937)) {
        mc_free((int)D_00196D9C, (int)D_001707AE, 680);
        D_00196D9C = (struct image *)-1751672937;
    }
    D_00196272 = *(signed char *)&saved_screen_active;
    if (scratch_190ce5 == 0) return;
    mc_set_location(686, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, location_object->id >> 16);
    disk_write_arena2_file(text_buffer, *(int *)scratch_buffer, town_notes_size());
}

void town_map_draw(void)
{
    int repeat;
    int map_height;
    int map_width;
    int map_y;
    int map_x;
    int player_x;
    int player_y;
    int screen_x;
    int screen_y;
    int colour;
    int pixel;

    mc_memcpy(screen_buffer, *(int *)scratch_190de4, 64000, D_001707AE, 695, 4);
    player_x = player_object->x - location_object->x;
    player_y = player_object->z - location_object->z;
    player_x >>= 6;
    player_y >>= 6;
    player_y = (current_location->height << 6) - player_y - 1;
    VX = player_x;
    VY = player_y;
    VX -= 37;
    VY -= 20;
    VX += D_00196D98;
    VY += D_00196D90;
    map_height = current_location->height << 6;
    map_width = current_location->width << 6;
    if (VX < 0) {
        D_00196D98 -= VX;
        VX = 0;
    }
    if (VX > map_width - 38) {
        D_00196D98 -= VX - (map_width - 38);
    }
    if (VY < 0) {
        D_00196D90 -= VY;
        VY = 0;
    }
    if (VY > map_height - 10) {
        D_00196D90 -= VY - (map_height - 10);
    }
    screen_y = 10;
    map_y = VY;
    while (screen_y < 170 && map_y < map_height) {
        for (repeat = 0; repeat < 2; repeat++) {
            map_x = VX;
            screen_x = 10;
            while (screen_x < 310 && map_x < map_width) {
                pixel = MAP[map_width * map_y + map_x];
                if (!(pixel == 0 || pixel == 251 || pixel == 250)) {
                    colour = D_0017A103[MAP[map_width * map_y + map_x]];
                    SCR[screen_y * 320 + screen_x] = colour;
                    SCR[screen_y * 320 + screen_x + 1] = colour;
                }
                screen_x += 2;
                map_x++;
            }
            screen_y++;
        }
        map_y++;
    }
    town_map_draw_notes();
    if ((*(struct kb *)0x46c).f == 0) return;
    screen_x = (player_x - VX) * 2 + 10;
    screen_y = (player_y - VY) * 2 + 10;
    if (screen_y >= 169) return;
    func_000A134C(screen_x, screen_y, 145);
    func_000A134C(screen_x + 1, screen_y, 145);
    func_000A134C(screen_x, screen_y + 1, 145);
    func_000A134C(screen_x + 1, screen_y + 1, 145);
}

void town_map_scroll(int direction)
{
    switch (direction) {
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
