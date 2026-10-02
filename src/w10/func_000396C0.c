/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000396C0 */
/* spell effect magnitude: base min/max, plus min/max, per level */
struct spell_mag {
    signed char base_min;
    signed char base_max;
    signed char plus_min;
    signed char plus_max;
    signed char per_level;
};
struct spell {
    char pad[32];
    struct spell_mag mag[3];
};
extern struct spell *D_00178A0A;    /* current spell */
extern short D_00195F30;            /* current effect */
extern short D_0019961C[];          /* cost factors */

int func_000396C0(void)
{
    short cost;

    cost = (D_00178A0A->mag[D_00195F30].base_min + D_00178A0A->mag[D_00195F30].base_max) / 2 * D_0019961C[0];
    cost += ((D_00178A0A->mag[D_00195F30].plus_min + D_00178A0A->mag[D_00195F30].plus_max) / 2 / D_00178A0A->mag[D_00195F30].per_level) * D_0019961C[1];
    return cost;
}
