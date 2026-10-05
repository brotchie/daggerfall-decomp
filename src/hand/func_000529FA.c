/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000529FA */
struct image {
    char pad0[18];
    short pitch;                /* 0x12 */
    char pad14[10];
    unsigned char *data;        /* 0x1e */
};
extern char D_00175404[];        /* __FILE__ */
extern void mc_memset(unsigned char *, int, int, char *, int, int);
extern void mc_memcpy(unsigned char *, unsigned char *, int, char *, int, int);

void flc_decode_lc(unsigned char *a1, struct image *a2)
{
    short y;
    short j;
    short top;
    short h;
    unsigned short col;
    unsigned char nruns;
    signed char cnt;

    top = *(short *)a1;
    h = *(short *)(a1 + 2);
    a1 += 4;
    for (y = top; y < top + h; y++) {
        nruns = *a1;
        a1++;
        if (nruns != 0) {
            col = (short)*a1 & 255;
            a1++;
            for (j = 0; j < nruns; j++) {
                cnt = *a1;
                a1++;
                if (cnt < 0) {
                    mc_memset(a2->data + a2->pitch * y + col, *a1, -cnt, D_00175404, 492, 4);
                    col -= cnt;
                    a1++;
                } else if (cnt > 0) {
                    mc_memcpy(a2->data + a2->pitch * y + col, a1, cnt, D_00175404, 498, 4);
                    col += cnt;
                    a1 += cnt;
                }
            }
        }
    }
}
