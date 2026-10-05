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
extern struct area *location_here;

int location_here_contains(int a1, int a2)
{
    if (location_here->x <= a1 && location_here->x + (location_here->w << 12) > a1)
        if (location_here->y <= a2 && location_here->y + (location_here->h << 12) > a2)
            return location_here->off == 0 ? 1 : 0;
    return 0;
}
