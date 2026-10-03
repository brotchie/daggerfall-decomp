/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003946E */
struct e3 { signed char a, b, c; };
struct e5 { signed char a, b, c, d, e; };
struct tbl { char pad[14]; struct e3 t3[6]; struct e5 t5[1]; };
extern struct tbl *D_00178A0A;
extern short D_00195F30;
extern short D_0019961C;
extern short D_0019961E;
extern short D_00199620;
extern short D_00199622;

int func_0003946E(void)
{
    short v;

    v = D_0019961C * D_00178A0A->t3[D_00195F30].a;
    v += D_0019961E * (D_00178A0A->t3[D_00195F30].b / D_00178A0A->t3[D_00195F30].c);
    v += D_00199620 * ((D_00178A0A->t5[D_00195F30].a + D_00178A0A->t5[D_00195F30].b) / 2);
    v += ((D_00178A0A->t5[D_00195F30].c + D_00178A0A->t5[D_00195F30].d) / 2 / D_00178A0A->t5[D_00195F30].e) * D_00199622;
    return v;
}
