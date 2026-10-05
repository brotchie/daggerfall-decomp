/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00056004 */
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern signed char mouse_double_click;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern struct rect itemmaker_buttons[];
extern signed char D_001940D4;
extern struct record *player_object;
extern int list_popup_callback;
extern int window_image;
extern char scratch_buffer[];
extern signed char mouse_buttons_prev;
extern int list_popup_poll(void);
extern int itemmaker_open(int);
extern int itemmaker_close(void);
extern void itemmaker_draw(void);
extern int sound_play(int, struct record *, int);
extern int xn_draw_fullscreen_overlay_shaded();
extern int xn_mouse_poll_clamped();
extern int xn_font_select();
extern int xn_tex_cache_begin_frame();

#define FLAG (((struct bf8_2_1 *)&D_001940D4)->f)
#define MX (mouse_x)
#define MY (mouse_y)

void itemmaker_update(void)
{
    short unused;
    short index;
    short unused2;
    short unused3;

    if (itemmaker_open(0) == 0) return;
    xn_tex_cache_begin_frame();
    D_0012B508 = 146;
    xn_draw_fullscreen_overlay_shaded(window_image);
    xn_font_select(4);
    itemmaker_draw();
    xn_font_select(3);
    index = 0;
    if (FLAG && (index = list_popup_poll()) > -1) {
        while (*((char *)&key_down_esc) != 0) ;
        while (*((char *)&mouse_buttons) != 0) xn_mouse_poll_clamped();
        *((char *)&mouse_double_click) = 0;
        (*(void (**)(int))((char *)&list_popup_callback))((*(unsigned char **)scratch_buffer)[index + 64000]);
        while (*((char *)&mouse_buttons) != 0) xn_mouse_poll_clamped();
        *((char *)&mouse_double_click) = 0;
        return;
    }
    if (index != -2 && *((char *)&key_down_esc) != 0) {
        itemmaker_close();
    } else if (index == -2) {
        while (*((char *)&key_down_esc) != 0) ;
    }
    if (FLAG || (*((char *)&mouse_buttons) == 0 || (*((char *)&mouse_buttons) != 0 && *((char *)&mouse_buttons_prev) != 0))) return;
    for (index = 0; index < 20; index++) {
        if (MX > itemmaker_buttons[index].x0 && MX < itemmaker_buttons[index].x1 && MY > itemmaker_buttons[index].y0 && MY < itemmaker_buttons[index].y1) {
            sound_play(203, player_object, 100);
            itemmaker_buttons[index].handler(index);
        }
    }
}
