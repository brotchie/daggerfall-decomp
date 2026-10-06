/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00013D2D */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
extern char D_001702CC[];        /* __FILE__ */
extern char *marquee_owned_text;
extern struct bits8 D_001940DA;
extern int marquee_x;
extern char *marquee_text;
#include "clib.h"

void marquee_start(char *text)
{
    if (marquee_owned_text != 0 && marquee_owned_text != (char *)0x97979797) {
        mc_free(marquee_owned_text, D_001702CC, 52);
        marquee_owned_text = (char *)0x97979797;
    }
    marquee_owned_text = D_001940DA.b3 ? text : 0;
    marquee_text = text;
    marquee_x = 320;
    D_001940DA.b3 = 0;
}
