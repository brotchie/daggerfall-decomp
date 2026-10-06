/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00052B50 */
#include "structs.h"
#include "clib.h"
extern char D_00175404[];   /* __FILE__ */
extern void xn_str_fill_u16(void *, unsigned short, int);   /* fill with a word */

/* decode an FLC DELTA_FLC (word-oriented delta) chunk into f->image */
void flc_decode_ss2(unsigned char *src, struct flc_player *f)
{
    short line;
    short done;
    short i;
    short nlines;
    short w;
    unsigned short x;
    short offset;
    signed char count;

    nlines = *(short *)src;
    src += 2;
    offset = line = done = 0;
    while (line < 200 && done < nlines) {
        w = *(short *)src;
        src += 2;
        if ((w & 0xC000) == 0xC000) {
            line += w * -1;
            offset += w * -1 * f->width;
        } else if ((w & 0xC000) == 0x8000) {
            f->image[f->width * (line + 1) - 1] = w & 0xff;
        } else if (w != 0) {
            x = *src & 0xff;
            ++src;
            for (i = 0; i < w; i++) {
                count = *src++;
                if (count < 0) {
                    xn_str_fill_u16(f->image + f->width * line + x, *(unsigned short *)src, -count * 2);
                    x -= count * 2;
                    src += 2;
                } else if (count > 0) {
                    mc_memcpy(f->image + f->width * line + x, src, count * 2, D_00175404, 564, 4);
                    x += count * 2;
                    src += count * 2;
                }
                if (i < w - 1) {
                    x += (unsigned short)*src & 0xff;
                    ++src;
                }
            }
            line++;
            done++;
            offset += f->width;
        } else if (w == 0) {
            line++;
            done++;
        }
    }
}
