/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008D800 */
extern void swap_shorts(short *, short *);

int point_in_rect(short x, short y, short left, short top, short right, short bottom)
{
    if (right < left)
        swap_shorts(&right, &left);
    if (bottom < top)
        swap_shorts(&bottom, &top);
    return x >= left && x <= right && y >= top && y <= bottom;
}
