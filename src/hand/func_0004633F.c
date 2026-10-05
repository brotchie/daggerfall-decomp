/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004633F */
struct macro { char name[5]; char *(*fn)(void); };   /* 9 bytes */
extern char D_0017110C[];        /* __FILE__ */
extern char D_00171114[];
extern unsigned char D_00178630[];   /* _IsTable */
extern struct macro *macro_letter_tables[];
extern short macro_letter_counts[];
extern char D_001911E4[];
extern int parse_name_seed;
extern char *parse_output;
extern char D_00199738;
extern char *quest_symbol_text(int, unsigned char, int);
extern int parse_read_number(unsigned char *);
extern void fatal_error(char *);
extern int string_hash(char *);
extern int rand(void);
extern void mc_strncpy(char *, char *, int, char *, int);
extern int strlen(char *);
extern void mc_memcpy(char *, unsigned char *, int, char *, int, int);
extern int strcmp(struct macro *, char *);
extern int xn_str_copy_alnum();
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

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

    parse_output = a2;
    parse_name_seed = rand();
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
                k = string_hash(buf);
                a1++;
                n = 0;
                while (*a1 != '_')
                    buf[n++] = *a1++;
                buf[n] = 0;
                s = quest_symbol_text(k, lvl, string_hash(buf));
            } else {
                buf[n] = 0;
                k = string_hash(buf);
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
                        mc_memcpy(buf, a1, 3, D_0017110C, 108, 1024);
                        buf[3] = 0;
                        a1 += 3;
                        a1 += parse_read_number(a1);
                    } else
                        a1 += xn_str_copy_alnum(buf, a1);
                    for (n = 0; n < cnt; n++) {
                        if (strcmp(&tab[n], buf) == 0) {
                            r = tab[n].fn();
                            if (r < (char *)1000) {
                                mc_set_location(122, D_0017110C);
                                mc_sprintf(D_001911E4, D_00171114, r, buf);
                                fatal_error(D_001911E4);
                            }
                            mc_strncpy(a2, r, 4, D_0017110C, 125);
                            a2 += strlen(a2);
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
