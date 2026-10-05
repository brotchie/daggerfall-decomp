/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006F7B5 */
#include "records.h"

#pragma pack(1)
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct Box { short x0; short y0; short x1; short y1; void (*fn)(); };
extern unsigned char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern char key_down_esc;
extern struct spell *selected_spell;
extern struct Box spellshop_buttons[];
extern struct spell *spell_records;
extern struct Img *window_image;
extern unsigned char *D_00195C44;
extern unsigned char mouse_buttons_prev;
extern char D_001A9AB8[];
extern unsigned short D_001A9AE1;
extern void spellshop_close(void);
extern void spellshop_buy(void);
extern void spellshop_draw_spell(struct spell *);
extern short picklist_frame(char *);
extern int func_0012DB50();
extern int func_00144F68();

void spellshop_update(void)
{
    int unused1;
    struct Img *img;
    short i;
    short rc;
    short unused2;

    img = window_image;
    func_00144F68(img->x, img->y, img->w, img->h, img->data);
    func_0012DB50(4);
    rc = picklist_frame(D_001A9AB8);
    if (rc > -1) {
        spellshop_close();
        spellshop_buy();
        return;
    }
    spellshop_draw_spell(selected_spell = &spell_records[D_00195C44[20000 + D_001A9AE1]]);
    if (key_down_esc != 0) {
        spellshop_close();
        return;
    }
    if (!(mouse_buttons != 0 && (mouse_buttons == 0 || mouse_buttons_prev == 0)))
        return;
    for (i = 0; i < 5; i++) {
        if (mouse_x > spellshop_buttons[i].x0 && mouse_x < spellshop_buttons[i].x1
          && mouse_y > spellshop_buttons[i].y0 && mouse_y < spellshop_buttons[i].y1)
            spellshop_buttons[i].fn();
    }
}
