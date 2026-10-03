/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005A4A9 */
struct Glyph { short w; short h; };
struct Font { char pad[6]; struct Glyph g[1]; };
extern short D_0012DA40;
extern short D_0012DA48;
extern struct Font *D_0012DA74;

int func_0005A4A9(char *s)
{
    short w;
    char *p;
    short c;
    struct Glyph *g;
    int n;

    w = 0;
    n = 0;
    p = s;
    while (*p != 0) {
        if (*p == ' ') {
            w += D_0012DA40;
        } else {
            n++;
            c = (unsigned char)*p - 33;
            g = &D_0012DA74->g[c];
            w += g->w;
        }
        p++;
    }
    return w + D_0012DA48 * n;
}
