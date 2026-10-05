/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00010010 */
extern int D_0012AA04;
extern unsigned char text_shadow_colour;
extern char D_00170004[];
extern char D_0017000B[];
extern char D_00170035[];
extern char D_00170049[];
extern int frame_checkpoint;
extern int D_0018DC0C;
extern int D_0018DC1C;
extern int D_0018DC24;
extern int frame_counter;
extern int player_death_timer;
extern short mouse_motion_x;
extern short mouse_motion_y;
extern unsigned char D_00196283;
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
extern void func_0004FF11(void);
extern void game_reset(void);
extern void mouse_set_bounds(int, int, int, int);
extern void hud_draw_heading_strip(int);
extern void func_00069E3C(void);
extern void crash_screen(void);
extern void player_movement_update(void);
extern void config_read(char *);
extern void dpmi_lock_region(void *, int);
extern void func_0009960A(void);
extern void func_0009DB11(int);
extern void func_0009DBF9(void);
extern void func_0009DC49(int);
extern void func_0009DEA7(int);
extern void func_000C0520(void);
extern void func_000C7F00(void);
extern void func_000CDD81(int);
extern int func_000CE7BB(void);
extern void func_000CE8C4(void);
extern void func_0012A254(int);
extern void func_0012B47E(short *, short *);
#pragma aux func_0009DA1C parm routine [];
extern void func_0009DA1C(int, char *);
#pragma aux func_0009DB3F parm routine [];
extern void func_0009DB3F(void (*)(void));
#pragma aux func_0009DBFE parm routine [];
extern void func_0009DBFE(void (*)(void));
extern void func_0009DAEE(char *, ...);
extern int func_0009DC59(char *, ...);

int func_00010010(short a1, char **a2)
{
    int l_3C;
    char l_u0;                  /* unused, but they have slots */
    int l_u1;
    int l_u2;
    int l_44;
    int l_u3;
    char l_u4;

    if (a1 != 2) {
        func_0009DA1C(54, D_00170004);
        func_0009DAEE(D_0017000B);
        func_0009DB11(-1);
    }
    func_00069E3C();
    func_0009DB3F(crash_screen);
    func_0009DBFE(func_0009DBF9);
    func_0009960A();
    func_000C0520();
    dpmi_lock_region(func_00010010, 2048000);
    func_0009DC49(*(int *)0x46c);
    D_0018DC1C = *(int *)0x46c;
    config_read(a2[1]);
    l_44 = func_0009DC59(D_00170035, 546, 384);
    if (l_44 < 0) {
        func_0009DA1C(75, D_00170004);
        func_0009DAEE(D_00170049);
        func_0009DB11(-1);
    }
    func_0009DEA7(l_44);
    mouse_set_bounds(0, 0, 319, 199);
    init_video();
    kludge_print_build(0);
    init_game_data();
    func_0004FF11();
    quest_init_record_sizes();
    intro_play_logo();
    for (l_44 = 0; l_44 < 12; l_44++) {
        D_001962A5 = (l_44 == 11);
        faction_politics_update(1);
        faction_politics_update(2);
    }
restart:
    title_menu();
    D_0018DC0C = *(int *)0x46c + 18;
    time_pass(1);
    D_0012AA04 = 23;
    func_0012A254(8);
    func_000CE8C4();
    D_0018DC24 = 1;
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
        l_3C = func_000CE7BB();
        frame_checkpoint = 2;
        func_0012B47E(&mouse_motion_x, &mouse_motion_y);
        player_movement_update();
        keys_world_actions();
        quest_debug_overlay();
        frame_checkpoint = 3;
        text_shadow_colour = D_00196283;
        func_000CDD81(1);
        screenshot_poll();
        frame_counter++;
        func_000C7F00();
    }
    return 0;
}
