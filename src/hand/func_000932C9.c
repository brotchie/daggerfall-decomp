/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000932C9 */
struct rect { short x0, y0, x1, y1; char pad[4]; };
extern char *screen_buffer;
extern char D_0017704C[];        /* __FILE__ */
extern struct rect inv_buttons[];
extern void mc_memcpy(char *, char *, int, char *, int, int);

void inv_blit_rect_from_image(int button, char *image)
{
    int y;

    for (y = inv_buttons[button].y0; y <= inv_buttons[button].y1; y++)
        mc_memcpy(y * 320 + screen_buffer + inv_buttons[button].x0, y * 320 + image + inv_buttons[button].x0, inv_buttons[button].x1 - inv_buttons[button].x0 + 1, D_0017704C, 631, 4);
}
