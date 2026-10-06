/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004633F */
/* 9 bytes in FALL.EXE (Watcom packs to 1 byte); natively the handler keeps its alignment: the
   linker takes no unaligned pointer in data (tools/port_data.py lays the table out again) */
#if !defined(DAGGER_PORT) || defined(__i386__)
#pragma pack(1)
#endif
struct macro { char name[5]; char *(*fn)(void); };   /* 9 bytes */
extern char D_0017110C[];        /* __FILE__ */
extern char D_00171114[];
extern unsigned char D_00178630[];   /* _IsTable */
extern struct macro macro_table[];     /* the 255 macros, by first letter (the data: tools/port_data.py) */
extern struct macro *macro_letter_tables[];
extern short macro_letter_counts[];
extern char D_001911E4[];
extern int parse_name_seed;
extern char *parse_output;
extern char D_00199738;
extern char *quest_symbol_text(int, int, int);
extern int parse_read_number(signed char *);
extern void fatal_error(char *);
extern int string_hash(char *);
#include "clib.h"
extern int xn_str_copy_alnum(char *, char *);
#pragma aux mc_set_location parm routine [];

void parse_expand(unsigned char *src, char *out)
{
    unsigned char ch;
    short i;
    short macro_count;
    short letter;
    unsigned char level;
    char token[1024];
    struct macro *table;
    char *text;
    int unused;
    char *result;
    int symbol_hash;

    parse_output = out;
    parse_name_seed = rand();
    while (*src != 0) {
        ch = *src++;
        if (ch == '_' || ch == '=') {
            level = ch == '=' ? (unsigned char)1 : (unsigned char)0;
            if (level == 0) {
                while (*src == '_') {
                    level += 16;
                    src++;
                }
            } else {
                while (*src == '=') {
                    level++;
                    src++;
                }
            }
            i = 0;
            while (!(*src == '_' || *src == '`'))
                token[i++] = *src++;
            if (*src == '`') {
                token[i] = 0;
                symbol_hash = string_hash(token);
                src++;
                i = 0;
                while (*src != '_')
                    token[i++] = *src++;
                token[i] = 0;
                text = quest_symbol_text(symbol_hash, level, string_hash(token));
            } else {
                token[i] = 0;
                symbol_hash = string_hash(token);
                text = quest_symbol_text(symbol_hash, level, 0);
            }
            while (*text != 0)
                *out++ = *text++;
            *out = 0;
            src++;
        } else {
            if (ch == '%') {
                letter = *src;
                if (D_00178630[(unsigned char)(letter + 1)] & 0xe0) {
                    letter += -97;
                    if (letter < 0)
                        letter = *src - 23;
                    macro_count = macro_letter_counts[letter];
                    table = macro_letter_tables[letter];
                    if (*src == 'z') {
                        mc_memcpy(token, src, 3, D_0017110C, 108, 1024);
                        token[3] = 0;
                        src += 3;
                        src += parse_read_number(src);
                    } else
                        src += xn_str_copy_alnum(token, src);
                    for (i = 0; i < macro_count; i++) {
                        if (strcmp(&table[i], token) == 0) {
                            result = table[i].fn();
                            if (result < (char *)1000) {
                                mc_set_location(122, D_0017110C);
                                mc_sprintf(D_001911E4, D_00171114, result, token);
                                fatal_error(D_001911E4);
                            }
                            mc_strncpy(out, result, 4, D_0017110C, 125);
                            out += strlen(out);
                            break;
                        }
                    }
                    goto next;
                }
            }
            *out++ = ch;
        }
next:
        if (D_00199738 != 0) {
            D_00199738 = 0;
            return;
        }
    }
    *out = 0;
}
