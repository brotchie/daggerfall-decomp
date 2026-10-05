/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000537B6 */
#include "records.h"

#pragma pack(1)
struct R { short x0; short y0; short x1; short y1; char pad[4]; };
extern char D_0012B508;
extern int screen_buffer;
extern char D_00175420[];
extern struct R classmaker_reputation_buttons[];
extern signed char text_buffer[];
extern int scratch_190df4;
extern char *scratch_190df8;
extern struct character *player_character;
extern void msgbox_update(void);
extern void text_draw_centred_coloured(char *, short, short, int, unsigned char);
extern char *itoa(int, char *, int);
extern int mc_memcpy();
extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern void xn_draw_fill_rect(short, short, short, short);
extern int xn_draw_image();

void classmaker_draw_reputations(void)
{
    short n;
    short h;
    short mid;
    short total;
    short m;

    xn_mouse_cursor_erase();
    mc_memcpy(screen_buffer, scratch_190df4, 64000, D_00175420, 283, 4);
    xn_draw_image(39, 5, *(unsigned short *)(scratch_190df8 + 4), *(unsigned short *)(scratch_190df8 + 6), scratch_190df8 + 12);
    h = classmaker_reputation_buttons[0].x1 - classmaker_reputation_buttons[0].x0 + 1;
    mid = (classmaker_reputation_buttons[0].y0 + classmaker_reputation_buttons[0].y1) >> 1;
    total = n = 0;
    for (; n < 5; n++) {
        m = player_character->reputation[n] * 5;
        if (player_character->reputation[n] < 0) {
            D_0012B508 = 0xf6;
            xn_draw_fill_rect(classmaker_reputation_buttons[n].x0, 82, h, -m);
        } else if (player_character->reputation[n] > 0) {
            D_0012B508 = 0xc5;
            xn_draw_fill_rect(classmaker_reputation_buttons[n].x0, 81 - m, h, m);
        }
        text_draw_centred_coloured(itoa(player_character->reputation[n], ((char *)text_buffer), 10), n * 33 + 58, 149, 145, 141);
        total += player_character->reputation[n];
    }
    text_draw_centred_coloured(itoa(-total, ((char *)text_buffer), 10), 105, 179, 145, 141);
    msgbox_update();
    xn_mouse_cursor_draw();
}
