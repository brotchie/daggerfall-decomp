/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E62 */
struct bf8_0_1 { unsigned char f:1; };
extern char D_00185097[];
extern char D_00195BE0[];
extern void func_0005BC5B(int, int);
extern int func_0007CBA1(int);
extern int func_0007D6AE(int, int);
extern void func_0008AF3E(int, int);
extern int func_0008DA4D(int);

int func_00088E62(int a1, int a2, int a3)
{
    unsigned char *l_28;
    unsigned char *l_24;
    int l_20;
    int l_1C;
    unsigned char *l_18;
    int l_14;

    l_28 = (unsigned char *)a1 + 71;
    l_24 = (unsigned char *)a3 + 71;
    if (func_0007D6AE(1, 100) > l_28[a2 + 86]) {
        func_0007CBA1(*(int *)D_00185097);
        return 0;
    }
    switch (l_28[a2 * 2 + 1]) {
    case 0:
        func_0008AF3E(a3, (int)l_24);
        if (*(int *)(*(char **)D_00195BE0 + 499) != 0) {
            *(int *)(*(char **)D_00195BE0 + 499) = 0;
            *(short *)(*(char **)D_00195BE0 + 108) = 0;
        }
        break;
    case 1:
        l_20 = *(int *)((char *)a3 + 63);
        while (l_20 != 0) {
            if (*(unsigned char *)l_20 == 11) {
                l_18 = (unsigned char *)l_20 + 71;
                if (*l_18 > 127) {
                    for (l_14 = 0; l_14 < 8; l_14++) {
                        *(short *)(l_24 + l_14 * 2 + 32) += *(short *)(l_18 + l_14 * 2 + 31);
                        if (*(short *)(l_24 + l_14 * 2 + 32) > *(short *)(l_24 + l_14 * 2 + 48))
                            *(short *)(l_24 + l_14 * 2 + 32) = *(short *)(l_24 + l_14 * 2 + 48);
                    }
                    l_20 = func_0008DA4D(l_20);
                } else {
                    l_20 = *(int *)((char *)l_20 + 55);
                }
            } else {
                l_20 = *(int *)((char *)l_20 + 55);
            }
        }
        break;
    case 2:
        if (((struct bf8_0_1 *)(l_24 + 137))->f == 0) return 0;
        func_0005BC5B(a3, 0);
        l_24[137] &= 254;
    case 3:
        break;
    }
    return 0;
}
