/* pickbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char screen_buffer[];
extern char D_00170DE4[];
extern char D_00170DF7[];
extern char D_00170E04[];
extern char D_00184876[];
extern char D_00187CA8[];
extern char D_001940D4[];
extern char D_001940D8[];
extern struct record *player_object;
extern char spellshop_icons[];
extern char window_image[];
extern char D_00195D60[];
extern char player_death_timer[];
extern char D_0019626F[];
extern char D_00196272[];
extern char game_mode[];
extern char spellbook_saved_screen[];

extern int spellbook_build_list(void);
extern int key_action_held(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int mc_malloc();
extern int mc_memcpy();
extern int func_0012DB50();
extern void hud_status_set(int);

int spellbook_open(short a1)
{
    int l_20;

    if (((int)(unsigned char)*(signed char *)D_0019626F) != 5) goto L417F1;
    if (((int)(unsigned char)*(signed char *)game_mode) == 8) goto L417F3;
L417F1:;
    goto L417FF;
L417F3:;
    return 1;
L417FF:;
    if (*(int *)player_death_timer <= 0) goto L41814;
    return 0;
L41814:;
    if (a1 != 0) goto L41837;
    if (*(signed char *)game_mode != 0) goto L41832;
    if (key_action_held(28) != 0) goto L41837;
L41832:;
    goto L4190D;
L41837:;
    if (*(int *)D_00195D60 == 0) goto L41856;
    hud_status_set(*(int *)D_00184876);
    return 0;
L41856:;
    func_0012DB50(4);
    *(signed char *)D_001940D8 &= 254;
    if (spellbook_build_list() != 0) goto L4187C;
    return 0;
L4187C:;
    *(signed char *)D_001940D4 |= 128;
    *(signed char *)D_00187CA8 = 0;
    *(signed char *)D_001940D8 |= 2;
    *(int *)spellbook_saved_screen = mc_malloc(64000, (int)D_00170DE4, 101);
    mc_memcpy(*(int *)spellbook_saved_screen, *(int *)screen_buffer, 64000, (int)D_00170DE4, 102, 4);
    *(signed char *)game_mode = 5;
    *(int *)window_image = disk_read_file((int)D_00170DF7, 0);
    *(int *)spellshop_icons = disk_read_file((int)D_00170E04, 0);
    *(signed char *)D_00196272 = 1;
    sound_play(237, player_object, 100);
L4190D:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 5) goto L41922;
    l_20 = 1;
    goto L41929;
L41922:;
    l_20 = 0;
L41929:;
    return l_20;
}
