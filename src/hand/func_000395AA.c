/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000395AA */
/* spell effect duration: base, plus, per level */
struct spell_dur {
    signed char base;
    signed char plus;
    signed char per_level;
};
struct spell {
    char pad[14];
    struct spell_dur dur[3];
};
extern struct spell *D_00178A0A;    /* current spell */
extern short D_00195F30;            /* current effect */
extern short D_0019961C[];          /* cost factors */

int func_000395AA(void)
{
    short cost;

    cost = D_00178A0A->dur[D_00195F30].base * D_0019961C[0];
    cost += (D_00178A0A->dur[D_00195F30].plus / D_00178A0A->dur[D_00195F30].per_level) * D_0019961C[1];
    return cost;
}
