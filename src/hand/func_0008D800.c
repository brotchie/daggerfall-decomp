/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008D800 */
extern void func_0008D88B(short *, short *);

int func_0008D800(short x, short y, short left, short top, short right, short bottom)
{
    if (right < left)
        func_0008D88B(&right, &left);
    if (bottom < top)
        func_0008D88B(&bottom, &top);
    return x >= left && x <= right && y >= top && y <= bottom;
}
