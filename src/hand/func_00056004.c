/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00056004 */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern signed char mouse_buttons;
extern signed char D_0012AC02;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern char itemmaker_buttons[];
extern signed char D_001940D4;
extern struct record *player_object;
extern int list_popup_callback;
extern int window_image;
extern char D_00195C44[];
extern signed char mouse_buttons_prev;
extern int spells_list_poll(void);
extern int itemmaker_open(int);
extern int itemmaker_close(void);
extern void itemmaker_draw(void);
extern int sound_play(int, struct record *, int);
extern int func_000CB552();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00135E90();

struct R { short x0, y0, x1, y1; void (*fn)(int); };
#define TAB ((struct R *)itemmaker_buttons)
#define FLAG (((struct bf8_2_1 *)&D_001940D4)->f)
#define MX (mouse_x)
#define MY (mouse_y)

void itemmaker_update(void)
{
    short l_24;
    short l_20;
    short l_1C;
    short l_18;

    if (itemmaker_open(0) == 0) return;
    func_00135E90();
    D_0012B508 = 146;
    func_000CB552(window_image);
    func_0012DB50(4);
    itemmaker_draw();
    func_0012DB50(3);
    l_20 = 0;
    if (FLAG && (l_20 = spells_list_poll()) > -1) {
        while (*((char *)&key_down_esc) != 0) ;
        while (*((char *)&mouse_buttons) != 0) func_0012B136();
        *((char *)&D_0012AC02) = 0;
        (*(void (**)(int))((char *)&list_popup_callback))((*(unsigned char **)D_00195C44)[l_20 + 64000]);
        while (*((char *)&mouse_buttons) != 0) func_0012B136();
        *((char *)&D_0012AC02) = 0;
        return;
    }
    if (l_20 != -2 && *((char *)&key_down_esc) != 0) {
        itemmaker_close();
    } else if (l_20 == -2) {
        while (*((char *)&key_down_esc) != 0) ;
    }
    if (FLAG || (*((char *)&mouse_buttons) == 0 || (*((char *)&mouse_buttons) != 0 && *((char *)&mouse_buttons_prev) != 0))) return;
    for (l_20 = 0; l_20 < 20; l_20++) {
        if (MX > TAB[l_20].x0 && MX < TAB[l_20].x1 && MY > TAB[l_20].y0 && MY < TAB[l_20].y1) {
            sound_play(203, player_object, 100);
            TAB[l_20].fn(l_20);
        }
    }
}
