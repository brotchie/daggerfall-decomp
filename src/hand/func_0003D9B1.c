/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D9B1 */
extern char D_00170D55[];       /* __FILE__ */
extern short D_00199664;
extern void func_0004633F(char *, char *);
extern int func_0005A442(unsigned char);
extern void func_000A0024(void *, char *, int);
extern void func_000A0AD9(char *, char *, int, char *, int);

char *func_0003D9B1(unsigned short flags, short width, char *src, char *buf, char *text)
{
    short lC;
    short in;
    short w;
    short n;
    short done;
    short saved;
    char *out;

    saved = D_00199664 = flags;
    if (!(flags & 8))
        func_0004633F(src, text);
    else
        func_000A0AD9(text, src, 4, D_00170D55, 169);
    D_00199664 = saved;
    if (src != 0 && src != (char *)0x97979797) {
        func_000A0024(src, D_00170D55, 173);
        src = (char *)0x97979797;
    }
    if (!(flags & ~8)) {
        if (buf != 0 && buf != (char *)0x97979797) {
            func_000A0024(buf, D_00170D55, 177);
            buf = (char *)0x97979797;
        }
        return text;
    }
    lC = w = 0;
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
            w = 0;
            break;
        case 253:
            *out = 253;
            out[n++] = 0;
            out += n;
            n = 2;
            w = 0;
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
            w = 0;
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
            w += func_0005A442(text[in - 1]);
            if (w > width) {
                out[n++] = 0;
                *out = 252;
                out += n;
                n = 2;
                w = 0;
            }
            break;
        }
    }
    if (text != 0 && text != (char *)0x97979797) {
        func_000A0024(text, D_00170D55, 265);
        text = (char *)0x97979797;
    }
    return buf;
}
