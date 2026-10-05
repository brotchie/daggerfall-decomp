/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004633F */
struct macro { char name[5]; char *(*fn)(void); };   /* 9 bytes */
extern char D_0017110C[];        /* __FILE__ */
extern char D_00171114[];
extern unsigned char D_00178630[];   /* _IsTable */
extern struct macro *macro_letter_tables[];
extern short macro_letter_counts[];
extern char D_001911E4[];
extern int parse_name_seed;
extern char *D_00199730;
extern char D_00199738;
extern char *quest_symbol_text(int, unsigned char, int);
extern int func_0004A1A2(unsigned char *);
extern void fatal_error(char *);
extern int func_000998C8(char *);
extern int func_0009DC25(void);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern int func_000A0DF4(char *);
extern void func_000A1023(char *, unsigned char *, int, char *, int, int);
extern int func_000A1720(struct macro *, char *);
extern int func_000CE3FD();
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void parse_expand(unsigned char *a1, char *a2)
{
    unsigned char c;
    short n;
    short cnt;
    short idx;
    unsigned char lvl;
    char buf[1024];
    struct macro *tab;
    char *s;
    int unused;
    char *r;
    int k;

    D_00199730 = a2;
    parse_name_seed = func_0009DC25();
    while (*a1 != 0) {
        c = *a1++;
        if (c == '_' || c == '=') {
            lvl = c == '=' ? (unsigned char)1 : (unsigned char)0;
            if (lvl == 0) {
                while (*a1 == '_') {
                    lvl += 16;
                    a1++;
                }
            } else {
                while (*a1 == '=') {
                    lvl++;
                    a1++;
                }
            }
            n = 0;
            while (!(*a1 == '_' || *a1 == '`'))
                buf[n++] = *a1++;
            if (*a1 == '`') {
                buf[n] = 0;
                k = func_000998C8(buf);
                a1++;
                n = 0;
                while (*a1 != '_')
                    buf[n++] = *a1++;
                buf[n] = 0;
                s = quest_symbol_text(k, lvl, func_000998C8(buf));
            } else {
                buf[n] = 0;
                k = func_000998C8(buf);
                s = quest_symbol_text(k, lvl, 0);
            }
            while (*s != 0)
                *a2++ = *s++;
            *a2 = 0;
            a1++;
        } else {
            if (c == '%') {
                idx = *a1;
                if (D_00178630[(unsigned char)(idx + 1)] & 0xe0) {
                    idx += -97;
                    if (idx < 0)
                        idx = *a1 - 23;
                    cnt = macro_letter_counts[idx];
                    tab = macro_letter_tables[idx];
                    if (*a1 == 'z') {
                        func_000A1023(buf, a1, 3, D_0017110C, 108, 1024);
                        buf[3] = 0;
                        a1 += 3;
                        a1 += func_0004A1A2(a1);
                    } else
                        a1 += func_000CE3FD(buf, a1);
                    for (n = 0; n < cnt; n++) {
                        if (func_000A1720(&tab[n], buf) == 0) {
                            r = tab[n].fn();
                            if (r < (char *)1000) {
                                func_000A0ED9(122, D_0017110C);
                                func_000A0F5C(D_001911E4, D_00171114, r, buf);
                                fatal_error(D_001911E4);
                            }
                            func_000A0AD9(a2, r, 4, D_0017110C, 125);
                            a2 += func_000A0DF4(a2);
                            break;
                        }
                    }
                    goto next;
                }
            }
            *a2++ = c;
        }
next:
        if (D_00199738 != 0) {
            D_00199738 = 0;
            return;
        }
    }
    *a2 = 0;
}
