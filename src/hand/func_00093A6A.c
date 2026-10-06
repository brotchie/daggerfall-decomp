/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00093A6A */
#include "records.h"
extern void size_fit(short *, short *, slot16, slot16);
extern void xn_draw_image_scaled(int, int, int, int, int, int, int, char *);
extern char *xn_tex_cache_lookup(int, int, iptr);
extern void xn_tex_cache_flush(void);

void inv_draw_item_image(struct item *item, struct rect *buttons, int cell)
{
    short width;
    short centre_x;
    struct texture_header *image;
    short centre_y;
    char *texture;
    short height;

    texture = xn_tex_cache_lookup(item->inventory_image >> 7, item->inventory_image & 127, -1);
    if (texture == 0) {
        xn_tex_cache_flush();
        texture = xn_tex_cache_lookup(item->inventory_image >> 7, item->inventory_image & 127, -1);
    }
    image = ((struct tex_cache_entry *)texture)->image;
    centre_x = (buttons[cell].x0 + buttons[cell].x1) >> 1;
    centre_y = (buttons[cell].y0 + buttons[cell].y1) >> 1;
    width = image->width;
    height = image->height;
    size_fit(&width, &height, buttons[cell].x1 - buttons[cell].x0 - 4, buttons[cell].y1 - buttons[cell].y0 - 4);
    xn_draw_image_scaled(centre_x - (width >> 1), centre_y - (height >> 1), width, height, image->width, image->height, image->flags | 32768, (char *)image + image->data_offset);
}
