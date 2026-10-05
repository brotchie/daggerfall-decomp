/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CA85 */
extern unsigned char text_shadow_colour;
extern unsigned char D_0012B508;
extern void text_draw_centred_shadow(char *, short, short);

void text_draw_centered_colored(char *a1, short a2, short a3, short a4, int a5)
{
    short l_10;
    short l_C;

    l_10 = D_0012B508;
    l_C = text_shadow_colour;
    D_0012B508 = a4;
    text_shadow_colour = a5;
    text_draw_centred_shadow(a1, a2, a3);
    D_0012B508 = l_10;
    text_shadow_colour = l_C;
}
