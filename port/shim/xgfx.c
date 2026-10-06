/* xgfx.c: two pixel routines that sit in the library region between Watcom's memcmp and
   _dos_findfirst (0xA134C-0xA13C7) but work on XnGine's screen: the clip window
   xn_gfx_clip_left/top/right/bottom (0x142940..4C, right and bottom exclusive), the row
   offsets xn_gfx_row_offset (0x142950, y * the screen width) and screen_buffer (0x143550).
   Register arguments: x in EAX, y in EDX, the colour in BL. */

extern int xn_gfx_clip_left, xn_gfx_clip_top, xn_gfx_clip_right, xn_gfx_clip_bottom;
extern int xn_gfx_row_offset[];
extern unsigned char *screen_buffer;
extern int D_00142928, D_0014292C;     /* pen_x, pen_y: where the last pixel went */

static int inside(int x, int y)
{
    return x >= xn_gfx_clip_left && y >= xn_gfx_clip_top && x < xn_gfx_clip_right &&
           y < xn_gfx_clip_bottom;
}

/* put_pixel (0xA134C -> 0xA1352): plots a pixel inside the clip window and moves the pen
   there; outside, nothing */
void func_000A134C(int x, int y, int colour)
{
    if (!inside(x, y))
        return;
    D_00142928 = x;
    D_0014292C = y;
    screen_buffer[xn_gfx_row_offset[y] + x] = (unsigned char)colour;
}

/* get_pixel (0xA138E -> 0xA1394): the pixel inside the clip window; outside, EAX is left as
   it came, so the result is x */
int func_000A138E(int x, int y)
{
    if (!inside(x, y))
        return x;
    return screen_buffer[xn_gfx_row_offset[y] + x];
}
