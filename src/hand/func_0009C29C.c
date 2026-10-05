/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009C29C */
#include "records.h"

extern unsigned char mouse_buttons;
extern unsigned char D_00187CA8;
extern unsigned char D_001889BC;
extern int travel_selected_location;
extern unsigned char D_00190CE5;
extern struct record *player_object;
extern struct character *player_character;
extern unsigned char mouse_buttons_prev;
extern unsigned char D_00196294;
extern unsigned char D_001962A9;
extern struct map_location *D_00196A9C;
extern int D_001AA678;
extern int D_001AA67C;
extern int sound_play(int, struct record *, int);
extern void location_place_player_at_edge(int);
extern void map_goto_location(int, int, int, int);
extern void travel_button_exit(int);
extern void func_0009BE38(void);
extern int travel_route(int, int, int, int, int);
extern int func_000C808D(int, int, int, int);

void func_0009C29C(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = (((func_000C808D(player_object->x, player_object->z, D_00196A9C[travel_selected_location].x_type_flags & 0x1ffffff, D_00196A9C[travel_selected_location].y_size & 0xffffff) >> 2) + 32) & 511) >> 6;
    if ((mouse_buttons & 1) == 0 || (mouse_buttons_prev & 1) != 0)
        return;
    sound_play(203, player_object, 110);
    D_00190CE5 = 0;
    func_0009BE38();
    D_001962A9 = 1;
    D_00196294 = 1;
    D_00187CA8 = 1;
    l_1C = player_character->fatigue;
    l_20 = travel_route(player_object->x, player_object->z, D_001AA678, D_001AA67C, 1);
    func_0009BE38();
    travel_button_exit(100);
    if (l_20 != -1)
        map_goto_location(D_001889BC, 1, travel_selected_location, 0);
    if (l_20 != -1)
        location_place_player_at_edge(l_18);
    D_001962A9 = 0;
    D_00196294 = 0;
}
