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

void inv_draw_item_image(char *a1, struct rect *a2, int a3)
{
    short l_14;
    short l_1C;
    struct img *l_20;
    short l_18;
    char *l_24;
    short l_10;

    l_24 = xn_tex_cache_lookup(*(unsigned short *)(a1 + 50) >> 7, *(unsigned short *)(a1 + 50) & 127, -1);
    if (l_24 == 0) {
        xn_tex_cache_flush();
        l_24 = xn_tex_cache_lookup(*(unsigned short *)(a1 + 50) >> 7, *(unsigned short *)(a1 + 50) & 127, -1);
    }
    l_20 = *(struct img **)(l_24 + 12);
    l_1C = (a2[a3].x0 + a2[a3].x1) >> 1;
    l_18 = (a2[a3].y0 + a2[a3].y1) >> 1;
    l_14 = l_20->w;
    l_10 = l_20->h;
    size_fit(&l_14, &l_10, a2[a3].x1 - a2[a3].x0 - 4, a2[a3].y1 - a2[a3].y0 - 4);
    xn_draw_image_scaled(l_1C - (l_14 >> 1), l_18 - (l_10 >> 1), l_14, l_10, l_20->w, l_20->h, l_20->flags | 32768, (char *)l_20 + l_20->offset);
}
