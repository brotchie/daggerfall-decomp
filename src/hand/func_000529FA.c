/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000529FA */
#include "structs.h"
extern char D_00175404[];        /* __FILE__ */
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);

void flc_decode_lc(unsigned char *chunk, struct flc_player *anim)
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
                    mc_memset(anim->image + anim->width * y + col, *chunk, -cnt, D_00175404, 492, 4);
                    col -= cnt;
                    chunk++;
                } else if (cnt > 0) {
                    mc_memcpy(anim->image + anim->width * y + col, chunk, cnt, D_00175404, 498, 4);
                    col += cnt;
                    chunk += cnt;
                }
            }
        }
    }
}
