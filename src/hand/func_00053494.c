/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00053494 */
#include "records.h"
#include "clib.h"

extern signed char D_0012B508;
extern iptr screen_buffer;
extern char D_00175420[];
extern char *skill_names[];
extern struct rect classmaker_buttons[];
extern signed char text_buffer[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d6a;
extern iptr scratch_190de8;
extern struct image *scratch_190df0;
extern iptr scratch_190df4;
extern struct career *player_class;
extern void msgbox_update(void);
extern void classmaker_draw_dagger(void);
extern void text_draw_coloured(iptr, int, int, int, unsigned char);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern void xn_mouse_cursor_erase(void);
extern void xn_mouse_cursor_draw(void);
extern void xn_draw_get_rect(int, int, int, int, char *, int);
extern void xn_draw_put_rect(int, int, int, int, char *, int);
extern void xn_draw_image_transparent(int, int, int, int, char *);

int classmaker_draw(short show_name)
{
    short i;
    short x;
    short y_offset;

    xn_mouse_cursor_erase();
    mc_memcpy((void *)screen_buffer, (void *)scratch_190df4, 64000, D_00175420, 235, 4);
    classmaker_draw_dagger();
    if (*(short *)scratch_190d66 & 2) {
        xn_draw_put_rect(44, (int)(short)scratch_190d6a, scratch_190df0->width, scratch_190df0->height, (char *)scratch_190de8, 0);
        *(signed char *)scratch_190d66 &= 253;
    }
    xn_draw_get_rect(44, (int)(short)scratch_190d6a, scratch_190df0->width, scratch_190df0->height, (char *)scratch_190de8, 0);
    *(signed char *)scratch_190d66 |= 2;
    xn_draw_image_transparent(44, (int)(short)scratch_190d6a, scratch_190df0->width, scratch_190df0->height, scratch_190df0->pixels);
    text_draw_centred_coloured((iptr)itoa((int)(short)*(short *)scratch_190d64, (char *)text_buffer, 10), (int)(short)(((scratch_190df0->width + 1) >> 1) + 43), (int)(short)((((int)(short)scratch_190d6a) + (scratch_190df0->height >> 1)) - 3), 145, 141);
    if (show_name != 0)
        text_draw_coloured((iptr)player_class->name, 110, 5, 145, 141);
    text_draw_centred_coloured((iptr)itoa(player_class->hp_per_level, (char *)text_buffer, 10), 287, 55, 145, 141);
    D_0012B508 = 145;
    for (i = 0; i < 12; i++) {
        if (player_class->skills[i] < 35)
            text_draw_coloured((iptr)skill_names[player_class->skills[i]], (short)(classmaker_buttons[i + 2].x0 + 2), (short)(classmaker_buttons[i + 2].y0 + 1), 145, 141);
    }
    x = (classmaker_buttons[18].x0 + classmaker_buttons[18].x1) >> 1;
    y_offset = ((classmaker_buttons[18].y1 + classmaker_buttons[18].y0) >> 1) - classmaker_buttons[18].y0 + 3;
    for (i = 0; i < 8; i++) {
        text_draw_centred_coloured((iptr)itoa(player_class->attributes[i], (char *)text_buffer, 10), x, (short)(classmaker_buttons[i + 18].y0 + y_offset), 145, 141);
    }
    msgbox_update();
    xn_mouse_cursor_draw();
    return 0;
}
