/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000410D7 */
struct mob {
    char pad0[21];
    unsigned short f21;         /* 0x15 */
    char pad17[4];
    unsigned short f27;         /* 0x1b */
    char pad1d[44];
    unsigned char f73;          /* 0x49 */
};
extern unsigned char D_0017B667[];
extern short D_0017B66D[][2][4];
extern int climate_category(void);
extern int rand(void);

void person_pick_sprite(struct mob *a1)
{
    int kind;
    int flip;

    kind = climate_category();
    flip = rand() & 1;
    if ((rand() & 31) == 0) {
        a1->f27 = 51072;
        a1->f21 &= ~0x4000;
        return;
    }
    a1->f27 = D_0017B66D[D_0017B667[kind]][flip][rand() & 3] << 7;
    a1->f21 |= flip != 0 ? 16384 : 0;
    a1->f73 |= flip != 0 ? 16 : 0;
}
