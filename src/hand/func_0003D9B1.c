/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D9B1 */
extern char D_00170D55[];       /* __FILE__ */
extern short text_format_flags;
extern void parse_expand(unsigned char *, char *);
extern int font_char_width(unsigned char);
extern void mc_free(void *, char *, int);
extern void mc_strncpy(char *, char *, int, char *, int);

char *text_expand_wrap(unsigned short flags, short width, char *src, char *buf, char *text)
{
    short unused;
    short in;
    short line_width;
    short n;
    short done;
    short saved;
    char *out;

    saved = text_format_flags = flags;
    if (!(flags & 8))
        parse_expand(src, text);
    else
        mc_strncpy(text, src, 4, D_00170D55, 169);
    text_format_flags = saved;
    if (src != 0 && src != (char *)0x97979797) {
        mc_free(src, D_00170D55, 173);
        src = (char *)0x97979797;
    }
    if (!(flags & ~8)) {
        if (buf != 0 && buf != (char *)0x97979797) {
            mc_free(buf, D_00170D55, 177);
            buf = (char *)0x97979797;
        }
        return text;
    }
    unused = line_width = 0;
    in = 0;
    n = 0;
    done = 0;
    out = buf;
    n += 2;
    while (done == 0) {
        switch ((unsigned char)text[in++]) {
        case 0:
            *out = 252;
            out[n++] = 0;
            out[n] = 0;
            done = 1;
            line_width = 0;
            break;
        case 253:
            *out = 253;
            out[n++] = 0;
            out += n;
            n = 2;
            line_width = 0;
            if (text[in] == 0) {
                out[n - 2] = 0;
                done = 1;
            }
            break;
        case 252:
            *out = 252;
            out[n++] = 0;
            out += n;
            n = 2;
            line_width = 0;
            break;
        case 251:
            out[n++] = 251;
            out[n++] = text[in++];
            break;
        case 250:
            out[n++] = 250;
            out[n++] = text[in++];
            break;
        case 249:
            out[n++] = 249;
            out[n++] = text[in++];
            break;
        case 248:
            out[n++] = 248;
            break;
        default:
            out[n++] = text[in - 1];
            line_width += font_char_width(text[in - 1]);
            if (line_width > width) {
                out[n++] = 0;
                *out = 252;
                out += n;
                n = 2;
                line_width = 0;
            }
            break;
        }
    }
    if (text != 0 && text != (char *)0x97979797) {
        mc_free(text, D_00170D55, 265);
        text = (char *)0x97979797;
    }
    return buf;
}
