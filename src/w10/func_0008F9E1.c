/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008F9E1 */
extern char D_001A9BBC[];
extern char D_001A9BDC[];
extern char D_001AA3DC[];
extern char D_001AA3E4[];
extern void func_0007CA85(char *, short, short, int, unsigned char);
extern void func_000CD1C5(int, int, int, int, char *);
extern char *func_00135D00(int, int, int);

void func_0008F9E1(char *a1)
{
    char *p;
    char *spr;
    short n;
    short unused;

    if (*(unsigned char *)a1 != 2) return;
    for (n = 0; n < 8; n++)
        if (((char **)D_001A9BBC)[n] == a1) return;
    p = a1 + 71;
    if (*(short *)(p + 67) != -1) return;
    if ((*(unsigned short *)(p + 42) & 1) == 0) return;
    if (*(int *)D_001AA3E4 >= 512) return;
    ((char **)D_001A9BDC)[*(int *)D_001AA3E4] = a1;
    n = *(short *)D_001AA3E4 - *(short *)D_001AA3DC;
    if (*(int *)D_001AA3E4 < *(int *)D_001AA3DC) {
        (*(int *)D_001AA3E4)++;
        return;
    }
    if (n < 12) {
        spr = *(char **)(func_00135D00(*(unsigned short *)(p + 50) >> 7, *(unsigned short *)(p + 50) & 127, -1) + 12);
        func_000CD1C5(n % 3 * 56 + 28 - (*(unsigned short *)(spr + 4) >> 1), n / 3 * 38 + 42 - (*(unsigned short *)(spr + 6) >> 1), *(unsigned short *)(spr + 4), *(unsigned short *)(spr + 6), spr + *(int *)(spr + 14));
        func_0007CA85(p, n % 3 * 56 + 30, n / 3 * 38 + 58 + (n % 3 == 1 ? 5 : 0), 145, 156);
    }
    (*(int *)D_001AA3E4)++;
}
