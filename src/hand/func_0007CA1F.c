/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CA1F */
extern unsigned char text_shadow_colour;
extern unsigned char D_0012B508;
extern void text_draw_shadow(char *, short, short);

void text_draw_coloured(char *a1, short a2, int a3, int a4, int a5)
{
    short l1;
    short l2;

    l1 = D_0012B508;
    l2 = text_shadow_colour;
    D_0012B508 = a4;
    text_shadow_colour = a5;
    text_draw_shadow(a1, a2, a3);
    D_0012B508 = l1;
    text_shadow_colour = l2;
}
