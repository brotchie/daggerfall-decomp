/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039635 */
struct dur { signed char base; signed char plus; signed char per_level; };
struct spell {
    char pad[23];
    struct dur dur[3];
};
extern struct spell *D_00178A0A;
extern short D_00195F30;
extern short D_0019961C[];

int func_00039635(void)
{
    short cost;

    cost = D_0019961C[0] * D_00178A0A->dur[D_00195F30].base;
    cost += (D_00178A0A->dur[D_00195F30].plus / D_00178A0A->dur[D_00195F30].per_level) * D_0019961C[1];
    return cost;
}
