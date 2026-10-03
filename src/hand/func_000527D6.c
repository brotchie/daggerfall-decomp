/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000527D6 */
extern char D_00175404[];
extern void func_000A1023(unsigned char *, unsigned char *, int, char *, int, int);

void func_000527D6(unsigned char *a1, unsigned char *a2, unsigned char a3)
{
    short l_1C;
    short l_18;
    short l_20;
    short l_14;

    l_1C = *(short *)a2;
    a2 += 2;
    for (l_18 = 0; l_18 < l_1C; l_18++) {
        a1 += a2[0] * 3;
        l_14 = a2[1];
        a2 += 2;
        l_14 = l_14 != 0 ? l_14 : 256;
        if (a3 != 0) {
            for (l_20 = 0; l_20 < l_14; l_20++, a2 += 3, a1 += 3) {
                a1[0] = a2[0] >> 2;
                a1[1] = a2[1] >> 2;
                a1[2] = a2[2] >> 2;
            }
        } else {
            l_20 = l_14 * 3;
            func_000A1023(a1, a2, l_20, D_00175404, 412, 4);
            a2 += l_20;
            a1 += l_20;
        }
    }
}
