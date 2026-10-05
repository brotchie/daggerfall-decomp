/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000684E9 */
#include "records.h"

extern double D_00175A08;
extern double D_00175A10;
extern double D_00175A18;
extern double D_00175A20;
extern double D_00175A28;
extern signed char D_00190D63;
extern struct building *current_building;
extern signed char D_00196272;
extern signed char game_mode;
extern double trade_haggle_minimum;
extern double trade_haggle_asking;
extern double D_001A3ABC;
extern int D_001A3ADC;
extern struct character *D_001A3AE0;
extern int D_001A3AE4;
extern void trade_haggle_show_offer(void);
extern void trade_haggle_step(void);
extern void mode_push(void);
#pragma aux exp parm routine [] value [8087];
extern double exp(double);

void trade_haggle_start(int a1, int a2, int a3, int a4)
{
    D_001A3ADC = -1;
    D_001A3AE4 = a3 + 71;
    D_001A3AE0 = (struct character *)(a4 + 71);
    if (a2 > 1) {
        /* the original multiplied by a literal (fmul [const]); with the constant as an extern
         * D_ symbol 10.0a would emit fld/fmulp when storing straight to memory, and the
         * (float) conversion (a no-op on the x87 stack) keeps the result in ST(0) instead */
        D_001A3ABC = (float)(exp(a2 * D_00175A08) * D_00175A10);
    }
    trade_haggle_asking = a1 * a2;
    trade_haggle_minimum = ((((1.0 - (D_001A3AE0->attributes[5] * D_00175A18)) + (D_001A3AE0->reputation[1] * D_00175A18)) - (D_001A3AE0->skills[21].value * D_00175A20)) - ((21 - current_building->quality) * D_00175A28)) * trade_haggle_asking;
    trade_haggle_step();
    mode_push();
    game_mode = 13;
    D_00196272 = 1;
    trade_haggle_show_offer();
    D_00190D63 = 0;
}
