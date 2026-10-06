/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B6CF */
#include "records.h"
#include "bitfield.h"
#include "clib.h"

extern struct rect sheet_buttons[23];
extern short font_height;
extern char D_00170C67[];
extern char D_00170C6F[];
extern char D_00170C73[];
extern char *race_names[];
extern signed char text_buffer[];
extern signed char D_001940D9;
extern char D_00195A08[];
extern struct record *player_entity;
extern iptr D_00195B64;
extern struct character *player_character;
extern struct career *player_class;
extern void sheet_draw_levelup_points(void);
extern int object_weight(struct record *);
extern void text_draw_coloured(char *, int, int, int, unsigned char);
extern void text_draw_centred_coloured(char *, int, int, int, unsigned char);
extern int gold_total(void);
extern int carry_capacity(void);
extern void xn_draw_put_rect(int, int, int, int, char *, int);
#pragma aux mc_set_location parm routine [];

void sheet_draw(void)
{
    short *attributes;
    int unused;
    int weight;
    short level;
    short i;
    short colour;
    short x;
    short value;

    text_draw_coloured(player_character->name, 41, 4, 145, 141);
    text_draw_coloured(player_class->name, 46, 24, 145, 141);
    level = player_character->level;
    text_draw_coloured(itoa(level, ((char *)text_buffer), 10), 45, 34, 145, 141);
    text_draw_coloured(race_names[player_character->race], 41, 14, 145, 141);
    mc_set_location(180, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C6F, gold_total());
    text_draw_coloured(((char *)text_buffer), 39, 44, 145, 141);
    mc_set_location(183, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C73, player_character->health, player_character->max_health);
    text_draw_centred_coloured(((char *)text_buffer), 72, 64, 145, 141);
    mc_set_location(185, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C73, player_character->fatigue >> 6, player_character->attributes[0] + player_character->attributes[4]);
    text_draw_centred_coloured(((char *)text_buffer), 77, 54, 145, 141);
    x = ((sheet_buttons[13].x0 + sheet_buttons[13].x1) >> 1) + 1;
    if (((struct bf8_2_1 *)&D_001940D9)->f) attributes = player_character->base_attributes;
    else attributes = player_character->attributes;
    for (i = 0; i < 8; i++) {
        if (i == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            value = attributes[i] + *(short *)D_00195A08;
        else
            value = attributes[i];
        if (value < player_character->base_attributes[i]) colour = 240;
        else if (value > player_character->base_attributes[i]) colour = 96;
        else colour = 145;
        if (i == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            text_draw_centred_coloured(itoa(attributes[i] + *(int *)D_00195A08, ((char *)text_buffer), 10), x, (short)((sheet_buttons[i + 13].y1 - font_height) - 2), colour, 141);
        else
            text_draw_centred_coloured(itoa(attributes[i], ((char *)text_buffer), 10), x, (short)((sheet_buttons[i + 13].y1 - font_height) - 2), colour, 141);
    }
    xn_draw_put_rect(192, 1, 125, 197, (char *)D_00195B64, 0);
    weight = object_weight(player_entity) >> 2;
    mc_set_location(218, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C73, weight, carry_capacity());
    text_draw_coloured(((char *)text_buffer), 91, 74, 145, 141);
    sheet_draw_levelup_points();
}
