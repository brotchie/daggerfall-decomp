/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000684E9 */
extern char D_00175A08[];
extern char D_00175A10[];
extern char D_00175A18[];
extern char D_00175A20[];
extern char D_00175A28[];
extern char D_00190D63[];
extern char D_00195A9C[];
extern char D_00196272[];
extern char D_00196274[];
extern char D_001A3AAC[];
extern char D_001A3AB4[];
extern char D_001A3ABC[];
extern char D_001A3ADC[];
extern char D_001A3AE0[];
extern char D_001A3AE4[];
extern void func_00068731(void);
extern void func_0006899B(void);
extern void func_0007D0DA(void);
#pragma aux func_000A1A0A parm routine [] value [8087];
extern double func_000A1A0A(double);

void func_000684E9(int a1, int a2, int a3, int a4)
{
    *(int *)D_001A3ADC = -1;
    *(int *)D_001A3AE4 = a3 + 71;
    *(int *)D_001A3AE0 = a4 + 71;
    if (a2 > 1) {
        /* the original multiplied by a literal (fmul [const]); with the constant as an extern
         * D_ symbol 10.0a would emit fld/fmulp when storing straight to memory, and the
         * (float) conversion (a no-op on the x87 stack) keeps the result in ST(0) instead */
        *(double *)D_001A3ABC = (float)(func_000A1A0A(a2 * *(double *)D_00175A08) * *(double *)D_00175A10);
    }
    *(double *)D_001A3AB4 = a1 * a2;
    *(double *)D_001A3AAC = ((((1.0 - (*(short *)(*(char **)D_001A3AE0 + 42) * *(double *)D_00175A18)) + (*(short *)(*(char **)D_001A3AE0 + 147) * *(double *)D_00175A18)) - (*(short *)(*(char **)D_001A3AE0 + 283) * *(double *)D_00175A20)) - ((21 - *(unsigned char *)(*(char **)D_00195A9C + 25)) * *(double *)D_00175A28)) * *(double *)D_001A3AB4;
    func_0006899B();
    func_0007D0DA();
    *(signed char *)D_00196274 = 13;
    *(signed char *)D_00196272 = 1;
    func_00068731();
    *(signed char *)D_00190D63 = 0;
}
