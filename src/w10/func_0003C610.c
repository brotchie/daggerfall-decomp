/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003C610 */
extern char D_00195B84[];
extern char D_00195C44[];
extern char D_001962A2[];
extern char D_001962A7[];

void func_0003C610(unsigned char *a1)
{
    unsigned char *l_18;

    if (*a1 != 11) return;
    l_18 = a1 + 71;
    if (*l_18 >= 128 && *(char *)D_001962A7 == 0 && *(short *)(l_18 + 29) != 0) {
        *(char *)D_001962A2 = 1;
        (*(char **)D_00195C44)[*(int *)D_00195B84 + 60000] = 117;
        (*(int *)D_00195B84)++;
        *(char *)D_001962A7 = 1;
    } else if (*l_18 < 100) {
        *(char *)D_001962A2 = 1;
        if ((*(unsigned short *)(a1 + 21) & 0x8000) && *(short *)(l_18 + 29) != 0) {
            (*(char **)D_00195C44)[*(int *)D_00195B84 + 60000] = *l_18 + 100;
            (*(int *)D_00195B84)++;
        }
    }
}
