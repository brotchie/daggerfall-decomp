/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00052B50 */
struct flc {
    char pad[18];
    short pitch;            /* 0x12 */
    char pad2[10];
    unsigned char *buf;     /* 0x1e */
};
extern char D_00175404[];   /* __FILE__ */
extern void func_000CE483(unsigned char *, unsigned short, int);   /* fill with a word */
extern void func_000A1023(unsigned char *, unsigned char *, int, char *, int, int); /* copy */

/* decode an FLC DELTA_FLC (word-oriented delta) chunk into f->buf */
void flc_decode_ss2(unsigned char *src, struct flc *f)
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
            offset += w * -1 * f->pitch;
        } else if ((w & 0xC000) == 0x8000) {
            f->buf[f->pitch * (line + 1) - 1] = w & 0xff;
        } else if (w != 0) {
            x = *src & 0xff;
            ++src;
            for (i = 0; i < w; i++) {
                count = *src++;
                if (count < 0) {
                    func_000CE483(f->buf + f->pitch * line + x, *(unsigned short *)src, -count * 2);
                    x -= count * 2;
                    src += 2;
                } else if (count > 0) {
                    func_000A1023(f->buf + f->pitch * line + x, src, count * 2, D_00175404, 564, 4);
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
            offset += f->pitch;
        } else if (w == 0) {
            line++;
            done++;
        }
    }
}
