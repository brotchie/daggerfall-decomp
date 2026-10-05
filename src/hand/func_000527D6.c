/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000527D6 */
extern char D_00175404[];
extern void mc_memcpy(unsigned char *, unsigned char *, int, char *, int, int);

void flc_decode_palette(unsigned char *palette, unsigned char *chunk, unsigned char to_6bit)
{
    short packet_count;
    short i;
    short n;
    short colours;

    packet_count = *(short *)chunk;
    chunk += 2;
    for (i = 0; i < packet_count; i++) {
        palette += chunk[0] * 3;
        colours = chunk[1];
        chunk += 2;
        colours = colours != 0 ? colours : 256;
        if (to_6bit != 0) {
            for (n = 0; n < colours; n++, chunk += 3, palette += 3) {
                palette[0] = chunk[0] >> 2;
                palette[1] = chunk[1] >> 2;
                palette[2] = chunk[2] >> 2;
            }
        } else {
            n = colours * 3;
            mc_memcpy(palette, chunk, n, D_00175404, 412, 4);
            chunk += n;
            palette += n;
        }
    }
}
