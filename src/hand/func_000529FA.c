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

void flc_decode_lc(unsigned char *chunk, struct image *image)
{
    short y;
    short j;
    short top;
    short h;
    unsigned short col;
    unsigned char nruns;
    signed char cnt;

    top = *(short *)chunk;
    h = *(short *)(chunk + 2);
    chunk += 4;
    for (y = top; y < top + h; y++) {
        nruns = *chunk;
        chunk++;
        if (nruns != 0) {
            col = (short)*chunk & 255;
            chunk++;
            for (j = 0; j < nruns; j++) {
                cnt = *chunk;
                chunk++;
                if (cnt < 0) {
                    mc_memset(image->data + image->pitch * y + col, *chunk, -cnt, D_00175404, 492, 4);
                    col -= cnt;
                    chunk++;
                } else if (cnt > 0) {
                    mc_memcpy(image->data + image->pitch * y + col, chunk, cnt, D_00175404, 498, 4);
                    col += cnt;
                    chunk += cnt;
                }
            }
        }
    }
}
