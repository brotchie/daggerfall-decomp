/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00037AB7 */
extern unsigned char *D_00178A0A;
extern unsigned char D_0017A8B2[];

int func_00037AB7(void)
{
    short l_18;

    l_18 = D_0017A8B2[D_00178A0A[0]] > D_0017A8B2[D_00178A0A[2]] ? D_0017A8B2[D_00178A0A[0]] : D_0017A8B2[D_00178A0A[2]];
    return D_0017A8B2[D_00178A0A[4]] > l_18 ? D_0017A8B2[D_00178A0A[4]] : l_18;
}
