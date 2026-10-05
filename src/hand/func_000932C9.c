/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000932C9 */
struct rect { short x0, y0, x1, y1; char pad[4]; };
extern char *screen_buffer;
extern char D_0017704C[];        /* __FILE__ */
extern struct rect inv_buttons[];
extern void func_000A1023(char *, char *, int, char *, int, int);

void inv_blit_rect_from_image(int a1, char *a2)
{
    int y;

    for (y = inv_buttons[a1].y0; y <= inv_buttons[a1].y1; y++)
        func_000A1023(y * 320 + screen_buffer + inv_buttons[a1].x0, y * 320 + a2 + inv_buttons[a1].x0, inv_buttons[a1].x1 - inv_buttons[a1].x0 + 1, D_0017704C, 631, 4);
}
