/* intro.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char mouse_buttons;
extern int screen_buffer;
extern char D_00170B7B[];
extern char D_00170B88[];
extern char D_00170C1B[];
extern char D_00170C28[];
extern char D_00170C35[];

extern int disk_resolve_path(int);
extern int mc_memset();
extern int func_000C1500();
extern int func_0012B136();
extern void starting_equipment_give(void);
extern void palette_restore(void);

void intro_play_logo(void)
{
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
    int l_1C;
    int l_18;

    while (mouse_buttons != 0) func_0012B136();
    l_18 = disk_resolve_path((int)D_00170B7B);
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 34, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_00170B88, 35, 4);
    while (mouse_buttons != 0) func_0012B136();
    mc_memset(655360, 0, 64000, (int)D_00170B88, 39, 4);
    palette_restore();
}

void intro_play_movie(void)
{
    int l_18;

    mc_memset(655360, 0, 64000, (int)D_00170B88, 358, 4);
    while (mouse_buttons != 0) func_0012B136();
    l_18 = disk_resolve_path((int)D_00170C1B);
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 363, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_00170B88, 364, 4);
    while (mouse_buttons != 0) func_0012B136();
    l_18 = disk_resolve_path((int)D_00170C28);
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 369, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_00170B88, 370, 4);
    while (mouse_buttons != 0) func_0012B136();
    l_18 = disk_resolve_path((int)D_00170C35);
    func_000C1500(l_18, 32, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_00170B88, 375, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_00170B88, 376, 4);
    palette_restore();
}

void func_0003B1D6(void)
{
    starting_equipment_give();
}
