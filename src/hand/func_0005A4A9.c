/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005A4A9 */
struct Glyph { short w; short h; };
struct Font { char pad[6]; struct Glyph g[1]; };
extern short font_space_width;
extern short font_char_spacing;
extern struct Font *D_0012DA74;

int font_text_width(char *text)
{
    short width;
    char *p;
    short glyph_index;
    struct Glyph *glyph;
    int char_count;

    width = 0;
    char_count = 0;
    p = text;
    while (*p != 0) {
        if (*p == ' ') {
            width += font_space_width;
        } else {
            char_count++;
            glyph_index = (unsigned char)*p - 33;
            glyph = &D_0012DA74->g[glyph_index];
            width += glyph->w;
        }
        p++;
    }
    return width + font_char_spacing * char_count;
}
