/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00053494 */
#include "records.h"

extern signed char D_0012B508;
extern int screen_buffer;
extern char D_00175420[];
extern char skill_names[];
extern char classmaker_buttons[];
extern char D_001854F6[];
extern short D_001855CC;
extern short D_001855CE;
extern short D_001855D0;
extern short D_001855D2;
extern signed char text_buffer[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short D_00190D6A;
extern int scratch_190de8;
extern char scratch_190df0[];
extern int scratch_190df4;
extern struct career *player_class;
extern void msgbox_update(void);
extern void classmaker_draw_dagger(void);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern int itoa();
extern int mc_memcpy();
extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern int xn_draw_get_rect();
extern int xn_draw_put_rect();
extern int xn_draw_image_transparent();

int classmaker_draw(short a1)
{
    short l_24;
    short l_18;
    short l_1C;

    xn_mouse_cursor_erase();
    mc_memcpy(screen_buffer, scratch_190df4, 64000, (int)D_00175420, 235, 4);
    classmaker_draw_dagger();
    if (*(short *)scratch_190d66 & 2) {
        xn_draw_put_rect(44, (int)(short)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 4), (int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 6), scratch_190de8, 0);
        *(signed char *)scratch_190d66 &= 253;
    }
    xn_draw_get_rect(44, (int)(short)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 4), (int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 6), scratch_190de8, 0);
    *(signed char *)scratch_190d66 |= 2;
    xn_draw_image_transparent(44, (int)(short)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 4), (int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 6), (int)(*(char **)scratch_190df0 + 12));
    text_draw_centred_coloured(itoa((int)(short)*(short *)scratch_190d64, (int)text_buffer, 10), (int)(short)(((((int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 4)) + 1) >> 1) + 43), (int)(short)((((int)(short)D_00190D6A) + (((int)(unsigned short)*(short *)(*(char **)scratch_190df0 + 6)) >> 1)) - 3), 145, 141);
    if (a1 != 0)
        text_draw_coloured((int)player_class->name, 110, 5, 145, 141);
    text_draw_centred_coloured(itoa(player_class->hp_per_level, (int)text_buffer, 10), 287, 55, 145, 141);
    D_0012B508 = 145;
    for (l_24 = 0; l_24 < 12; l_24++) {
        if (player_class->skills[l_24] < 35)
            text_draw_coloured(*(int *)(skill_names + (player_class->skills[l_24] << 2)), (short)(*(short *)(classmaker_buttons + (l_24 + 2) * 12) + 2), (short)(*(short *)(D_001854F6 + (l_24 + 2) * 12) + 1), 145, 141);
    }
    l_18 = (D_001855CC + D_001855D0) >> 1;
    l_1C = ((D_001855D2 + D_001855CE) >> 1) - D_001855CE + 3;
    for (l_24 = 0; l_24 < 8; l_24++) {
        text_draw_centred_coloured(itoa(player_class->attributes[l_24], (int)text_buffer, 10), l_18, (short)(*(short *)(D_001854F6 + (l_24 + 18) * 12) + l_1C), 145, 141);
    }
    msgbox_update();
    xn_mouse_cursor_draw();
    return 0;
}
