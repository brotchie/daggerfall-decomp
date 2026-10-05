/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005A4A9 */
struct Glyph { short w; short h; };
struct Font { char pad[6]; struct Glyph g[1]; };
extern short font_space_width;
extern short font_char_spacing;
extern struct Font *D_0012DA74;

int font_text_width(char *s)
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
            w += font_space_width;
        } else {
            n++;
            c = (unsigned char)*p - 33;
            g = &D_0012DA74->g[c];
            w += g->w;
        }
        p++;
    }
    return w + font_char_spacing * n;
}
