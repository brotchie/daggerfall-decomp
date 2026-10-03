/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003978C */
struct spell_chance { signed char base; signed char plus; signed char per_level; };
struct spell_dur { signed char base; signed char plus; signed char per_level; };
struct spell_mag {
    signed char base_min;
    signed char base_max;
    signed char plus_min;
    signed char plus_max;
    signed char per_level;
};
struct spell {
    char pad[14];
    struct spell_chance chance[3];  /* 14 */
    struct spell_dur dur[3];        /* 23 */
    struct spell_mag mag[3];        /* 32 */
};
extern struct spell *D_00178A0A;
extern short D_00195F30;
extern short D_0019961C[];

int func_0003978C(void)
{
    short cost;
    struct spell *sp;
    short e;

    e = D_00195F30;
    sp = D_00178A0A;
    cost = ((sp->mag[e].base_max + sp->mag[e].base_min) >> 1) * D_0019961C[2];
    cost += ((sp->mag[e].plus_min + sp->mag[e].plus_max) >> 1) * (D_0019961C[3] / sp->mag[e].per_level);
    cost += sp->chance[e].base * D_0019961C[0];
    cost += sp->chance[e].plus * D_0019961C[1] / sp->chance[e].per_level;
    return cost;
}
