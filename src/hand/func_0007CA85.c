/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CA85 */
extern unsigned char text_shadow_colour;
extern unsigned char D_0012B508;
extern void text_draw_centred_shadow(char *, short, short);

void text_draw_centred_coloured(char *text, short x, short y, short colour, int shadow)
{
    short old_colour;
    short old_shadow;

    old_colour = D_0012B508;
    old_shadow = text_shadow_colour;
    D_0012B508 = colour;
    text_shadow_colour = shadow;
    text_draw_centred_shadow(text, x, y);
    D_0012B508 = old_colour;
    text_shadow_colour = old_shadow;
}
