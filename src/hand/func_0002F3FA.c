/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002F3FA */
#include "structs.h"
extern short xn_gfx_clip_bottom;
extern struct image *hud_bar_image;
extern unsigned short *game_settings;
extern void screen_shake_start(int);
extern int xn_draw_view_checkerboard(void);

void damage_player_hurt(int n)
{
    int saved;

    saved = xn_gfx_clip_bottom;
    xn_gfx_clip_bottom = (*game_settings & 1) ? 199 : hud_bar_image->y - 2;
    xn_draw_view_checkerboard();
    xn_gfx_clip_bottom = saved;
    if (n < 4)
        n = 4;
    else if (n > 32)
        n = 32;
    screen_shake_start(n << 3);
}
