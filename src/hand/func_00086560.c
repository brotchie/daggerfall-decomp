/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086560 */
struct area {
    int f0;
    unsigned x:25;
    unsigned f4b:6;
    unsigned off:1;
    unsigned y:24;
    unsigned w:4;
    unsigned h:4;
};
extern struct area *D_00196A80;

int func_00086560(int a1, int a2)
{
    if (D_00196A80->x <= a1 && D_00196A80->x + (D_00196A80->w << 12) > a1)
        if (D_00196A80->y <= a2 && D_00196A80->y + (D_00196A80->h << 12) > a2)
            return D_00196A80->off == 0 ? 1 : 0;
    return 0;
}
