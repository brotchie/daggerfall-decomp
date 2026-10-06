/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00043F9C */
#include "records.h"
#include "clib.h"

extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern char D_00170EE8[];
extern char D_00170F2D[];
extern char D_00170F3A[];
extern char D_00170F47[];
extern struct rect options_mouse_buttons[];
extern struct record *player_object;
extern iptr D_00195B5C;
extern iptr D_00195B60;
extern unsigned char mouse_sensitivity_x;
extern unsigned char mouse_sensitivity_y;
extern char mouse_buttons_prev;
extern void options_mouse_draw(iptr);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern void cursor_draw_arrow(void);
extern void xn_gfx_present_inclusive(int);
extern void xn_mouse_get_sensitivity(unsigned char *, unsigned char *);
extern void xn_mouse_poll_clamped(void);

int options_mouse_screen(void)
{
    int button;
    int done;
    iptr background;

    done = 0;
    background = disk_read_file(D_00170F2D, 0);
    D_00195B5C = disk_read_file(D_00170F3A, 0);
    D_00195B60 = disk_read_file(D_00170F47, 0);
    xn_mouse_get_sensitivity(&mouse_sensitivity_x, &mouse_sensitivity_y);
    mouse_sensitivity_x /= 6;
    mouse_sensitivity_y /= 6;
    while (done == 0) {
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        options_mouse_draw(background);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (button = 0; button < 7; button++) {
                if (mouse_x > options_mouse_buttons[button].x0 && mouse_x < options_mouse_buttons[button].x1 && mouse_y > options_mouse_buttons[button].y0 && mouse_y < options_mouse_buttons[button].y1) {
                    sound_play(203, player_object, 100);
                    done = options_mouse_buttons[button].handler(button);
                }
            }
        }
        xn_gfx_present_inclusive(0);
    }
    if (D_00195B60 != 0 && D_00195B60 != (-1751672937)) {
        mc_free((void *)D_00195B60, D_00170EE8, 468);
        D_00195B60 = -1751672937;
    }
    if (D_00195B5C != 0 && D_00195B5C != (-1751672937)) {
        mc_free((void *)D_00195B5C, D_00170EE8, 469);
        D_00195B5C = -1751672937;
    }
    if (background != 0 && background != (-1751672937)) {
        mc_free((void *)background, D_00170EE8, 470);
        background = -1751672937;
    }
    return 0;
}
