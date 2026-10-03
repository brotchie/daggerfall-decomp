/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007E3CD */
extern int D_00195AA4;
extern unsigned char *func_0007DF35(int);

void func_0007E3CD(unsigned char *a1, unsigned char a2, int a3)
{
    unsigned char *l_18;

    l_18 = a1 ? a1 : func_0007DF35(D_00195AA4);
    if (a1 == 0) return;
    if (a1[24] == 15) return;
    l_18[8] = a2;
    l_18[15] &= 248;
    l_18[15] |= 1;
    *(int *)(l_18 + 2) = a3;
}
