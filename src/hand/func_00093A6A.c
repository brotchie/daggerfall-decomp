/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00093A6A */
struct rect {
    short x0, y0, x1, y1;
    int pad;
};
#pragma pack(1)
struct img {
    char pad[4];
    unsigned short w, h, flags;
    char pad2[4];
    int offset;
};
#pragma pack()
extern void size_fit(short *, short *, short, short);
extern int xn_draw_image_scaled();
extern char *xn_tex_cache_lookup(int, int, int);
extern int xn_tex_cache_flush();

void inv_draw_item_image(char *item, struct rect *rects, int cell)
{
    short width;
    short centre_x;
    struct img *image;
    short centre_y;
    char *texture;
    short height;

    texture = xn_tex_cache_lookup(*(unsigned short *)(item + 50) >> 7, *(unsigned short *)(item + 50) & 127, -1);
    if (texture == 0) {
        xn_tex_cache_flush();
        texture = xn_tex_cache_lookup(*(unsigned short *)(item + 50) >> 7, *(unsigned short *)(item + 50) & 127, -1);
    }
    image = *(struct img **)(texture + 12);
    centre_x = (rects[cell].x0 + rects[cell].x1) >> 1;
    centre_y = (rects[cell].y0 + rects[cell].y1) >> 1;
    width = image->w;
    height = image->h;
    size_fit(&width, &height, rects[cell].x1 - rects[cell].x0 - 4, rects[cell].y1 - rects[cell].y0 - 4);
    xn_draw_image_scaled(centre_x - (width >> 1), centre_y - (height >> 1), width, height, image->w, image->h, image->flags | 32768, (char *)image + image->offset);
}
