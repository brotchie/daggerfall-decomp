/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000684E9 */
#include "records.h"

extern char D_00175A08[];
extern char D_00175A10[];
extern char D_00175A18[];
extern char D_00175A20[];
extern char D_00175A28[];
extern char D_00190D63[];
extern struct building *current_building;
extern char D_00196272[];
extern char game_mode[];
extern char D_001A3AAC[];
extern char trade_haggle_asking[];
extern char D_001A3ABC[];
extern char D_001A3ADC[];
extern struct character *D_001A3AE0;
extern char D_001A3AE4[];
extern void trade_haggle_show_offer(void);
extern void func_0006899B(void);
extern void mode_push(void);
#pragma aux exp parm routine [] value [8087];
extern double exp(double);

void func_000684E9(int a1, int a2, int a3, int a4)
{
    *(int *)D_001A3ADC = -1;
    *(int *)D_001A3AE4 = a3 + 71;
    D_001A3AE0 = (struct character *)(a4 + 71);
    if (a2 > 1) {
        /* the original multiplied by a literal (fmul [const]); with the constant as an extern
         * D_ symbol 10.0a would emit fld/fmulp when storing straight to memory, and the
         * (float) conversion (a no-op on the x87 stack) keeps the result in ST(0) instead */
        *(double *)D_001A3ABC = (float)(exp(a2 * *(double *)D_00175A08) * *(double *)D_00175A10);
    }
    *(double *)trade_haggle_asking = a1 * a2;
    *(double *)D_001A3AAC = ((((1.0 - (D_001A3AE0->attributes[5] * *(double *)D_00175A18)) + (D_001A3AE0->reputation[1] * *(double *)D_00175A18)) - (D_001A3AE0->skills[21].value * *(double *)D_00175A20)) - ((21 - current_building->quality) * *(double *)D_00175A28)) * *(double *)trade_haggle_asking;
    func_0006899B();
    mode_push();
    *(signed char *)game_mode = 13;
    *(signed char *)D_00196272 = 1;
    trade_haggle_show_offer();
    *(signed char *)D_00190D63 = 0;
}
