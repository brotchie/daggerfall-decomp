/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D2B6 */
struct pc { char pad[0x81]; unsigned char f81; unsigned char f82; };
struct st { char pad[0x36]; int f36; };
extern double D_00170D11;
extern double D_00170D19;
extern double D_00170D21;
extern short skill_advance_multipliers[];
extern struct pc *player_character;
extern struct st *player_class;
#pragma aux func_000A166C parm routine [] value [8087];
extern double func_000A166C(double, double);

int skill_ready_to_advance(int a1, int a2, int a3, int a4)
{
    double d;

    a2 = ((65536 - ((player_character->f82 - 2) << 13)) * a2) / 65536;
    d = (double)skill_advance_multipliers[a4] * a1;
    d = (player_class->f36 * d) * D_00170D11;
    d = func_000A166C(1.04, (short)player_character->f81) * d;
    d = ((d * D_00170D19) / D_00170D21) + 1.0;
    return a2 >= (int)d;
}
