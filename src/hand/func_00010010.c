/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00010010 */
extern int xn_timer_fps;
extern unsigned char text_shadow_colour;
extern char D_00170004[];
extern char D_0017000B[];
extern char D_00170035[];
extern char D_00170049[];
extern int frame_checkpoint;
extern int D_0018DC0C;
extern int D_0018DC1C;
extern int engine_running;
extern int frame_counter;
extern int player_death_timer;
extern short mouse_motion_x;
extern short mouse_motion_y;
extern unsigned char fog_colour;
extern unsigned char D_001962A5;
extern void game_frame(void);
extern void screenshot_poll(void);
extern void faction_politics_update(int);
extern void quest_debug_overlay(void);
extern void intro_play_logo(void);
extern void title_menu(void);
extern void keys_world_actions(void);
extern void kludge_print_build(int);
extern void time_pass(int);
extern void quest_init_record_sizes(void);
extern void init_game_data(void);
extern void init_video(void);
extern void init_palette(void);
extern void game_reset(void);
extern void mouse_set_bounds(int, int, int, int);
extern void hud_draw_heading_strip(int);
extern void dpmi_memory_stats(void);
extern void crash_screen(void);
extern void player_movement_update(void);
extern void config_read(char *);
extern int dpmi_lock_region(void *, int);
extern void causeway_disable_error_dump(void);
#include "clib.h"
extern void func_0009DBF9(void);
extern void xn_sys_install_crit_error_handler(void);
extern void xn_sys_yield(void);
extern void xn_gfx_present_inclusive(int);
extern int xn_timer_fps_update(void);
extern void xn_sys_zero_page_save(void);
extern void xn_render_set_mode(int);
extern void xn_mouse_read_motion(short *, short *);
#pragma aux func_0009DA1C parm routine [];
#pragma aux func_0009DB3F parm routine [];
extern void func_0009DB3F(void (*)(void));
#pragma aux func_0009DBFE parm routine [];
extern void func_0009DBFE(void (*)(void));

int func_00010010(short argc, char **argv)
{
    int fps;
    char unused;                  /* unused, but they have slots */
    int unused2;
    int unused3;
    int n;
    int unused4;
    char unused5;

    if (argc != 2) {
        func_0009DA1C(54, D_00170004);
        printf(D_0017000B);
        exit(-1);
    }
    dpmi_memory_stats();
    func_0009DB3F(crash_screen);
    func_0009DBFE(func_0009DBF9);
    causeway_disable_error_dump();
    xn_sys_install_crit_error_handler();
    dpmi_lock_region(func_00010010, 2048000);
    srand(*(int *)0x46c);
    D_0018DC1C = *(int *)0x46c;
    config_read(argv[1]);
    n = open(D_00170035, 546, 384);
    if (n < 0) {
        func_0009DA1C(75, D_00170004);
        printf(D_00170049);
        exit(-1);
    }
    close(n);
    mouse_set_bounds(0, 0, 319, 199);
    init_video();
    kludge_print_build(0);
    init_game_data();
    init_palette();
    quest_init_record_sizes();
    intro_play_logo();
    for (n = 0; n < 12; n++) {
        D_001962A5 = (n == 11);
        faction_politics_update(1);
        faction_politics_update(2);
    }
restart:
    title_menu();
    D_0018DC0C = *(int *)0x46c + 18;
    time_pass(1);
    xn_timer_fps = 23;
    xn_render_set_mode(8);
    xn_sys_zero_page_save();
    engine_running = 1;
    for (;;) {
        frame_checkpoint = 0;
        game_frame();
        frame_checkpoint = 1;
        if (player_death_timer < 0) {
            game_reset();
            player_death_timer = 0;
            goto restart;
        }
        hud_draw_heading_strip(0);
        fps = xn_timer_fps_update();
        frame_checkpoint = 2;
        xn_mouse_read_motion(&mouse_motion_x, &mouse_motion_y);
        player_movement_update();
        keys_world_actions();
        quest_debug_overlay();
        frame_checkpoint = 3;
        text_shadow_colour = fog_colour;
        xn_gfx_present_inclusive(1);
        screenshot_poll();
        frame_counter++;
        xn_sys_yield();
    }
    return 0;
}
