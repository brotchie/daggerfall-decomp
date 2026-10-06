/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00098BE8 */
extern char D_00188249[];
extern char D_0018824D[];
extern signed char text_rsc_buffer[];
extern struct character *player_character;
extern void text_draw_coloured(char *, int, int, int, unsigned char);
#include "clib.h"

void inv_draw_armor_values(void)
{
    int i;

    for (i = 0; i < 7; i++)
        text_draw_coloured(itoa((100 - ((signed char *)((char *)player_character + 68))[i]) / 5, ((char *)text_rsc_buffer), 10), *(short *)(D_00188249 + (i << 3)), *(short *)(D_0018824D + (i << 3)), 145, 156);
}
