/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00028E24 */
extern unsigned char player_environment;
extern char *D_00196DA0;

#define SETBIT(map, n) ((map)[(unsigned)(n) >> 3] |= 1 << ((n) & 7))

void automap_mark_seen(char *a1)
{
    char *l_18;

    if (a1 == 0) return;
    if (player_environment != 3) return;
    if (D_00196DA0 == 0) return;
    if ((int)(unsigned short)(*(short *)(a1 + 21) & 128) != 0) return;
    a1[21] |= 128;
    l_18 = D_00196DA0 + 71;
    l_18 += 2048;
    SETBIT(l_18, *(int *)(a1 + 31) & 65535);
}
