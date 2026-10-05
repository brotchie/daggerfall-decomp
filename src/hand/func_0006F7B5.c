/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006F7B5 */
#include "records.h"

extern unsigned char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern char key_down_esc;
extern struct spell *selected_spell;
extern struct rect spellshop_buttons[];
extern struct spell *spell_records;
extern struct image *window_image;
extern unsigned char *scratch_buffer;
extern unsigned char mouse_buttons_prev;
extern char shared_picklist[];
extern unsigned short D_001A9AE1;
extern int spellshop_close(void);
extern void spellshop_buy(void);
extern void spellshop_draw_spell(struct spell *);
extern short picklist_frame(char *);
extern int xn_font_select();
extern int xn_draw_image();

void spellshop_update(void)
{
    int unused1;
    struct image *image;
    short i;
    short picked;
    short unused2;

    image = window_image;
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    xn_font_select(4);
    picked = picklist_frame(shared_picklist);
    if (picked > -1) {
        spellshop_close();
        spellshop_buy();
        return;
    }
    spellshop_draw_spell(selected_spell = &spell_records[scratch_buffer[20000 + D_001A9AE1]]);
    if (key_down_esc != 0) {
        spellshop_close();
        return;
    }
    if (!(mouse_buttons != 0 && (mouse_buttons == 0 || mouse_buttons_prev == 0)))
        return;
    for (i = 0; i < 5; i++) {
        if (mouse_x > spellshop_buttons[i].x0 && mouse_x < spellshop_buttons[i].x1
          && mouse_y > spellshop_buttons[i].y0 && mouse_y < spellshop_buttons[i].y1)
            spellshop_buttons[i].handler();
    }
}
