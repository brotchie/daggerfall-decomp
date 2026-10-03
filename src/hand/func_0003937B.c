/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003937B */
struct spell_chance { signed char base; signed char plus; signed char per_level; };
struct spell_dur { signed char base; signed char plus; signed char per_level; };
struct spell {
    char pad[14];
    struct spell_chance chance[3];
    struct spell_dur dur[3];
};
extern struct spell *D_00178A0A;
extern short D_00195F30;
extern short D_0019961C[4];

int func_0003937B(void)
{
    short cost;

    cost = D_0019961C[0] * D_00178A0A->chance[D_00195F30].base;
    cost += (D_00178A0A->chance[D_00195F30].plus / D_00178A0A->chance[D_00195F30].per_level) * D_0019961C[1];
    cost += D_00178A0A->dur[D_00195F30].base * D_0019961C[2];
    cost += (D_00178A0A->dur[D_00195F30].plus / D_00178A0A->dur[D_00195F30].per_level) * D_0019961C[3];
    return cost;
}
