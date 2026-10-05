/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D412 */
#include "structs.h"
extern char D_00170D55[];
extern char D_00170D5C[];
extern struct text_rsc_entry *scratch_buffer;
extern int text_rsc_file;
extern char text_missing_ok;
extern char *text_expand_wrap(unsigned short, short, unsigned char *, char *, char *);
extern int rand(void);
extern int lseek(int, int, int);
extern char *mc_malloc(int, char *, int);
extern int read(int, void *, int);
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, ...);

char *text_rsc_load(short id, unsigned short flags, short width)
{
    short n;
    short i;
    short j;
    int size;
    unsigned char *text;
    char *wrap_buf;
    char *expand_buf;

    lseek(text_rsc_file, 0, 0);
    read(text_rsc_file, &n, 2);
    read(text_rsc_file, scratch_buffer, n);
    n /= 6;
    for (i = 0; i < n; i++) {
        if (scratch_buffer[i].id == id)
            break;
    }
    if (i >= n) {
        if (text_missing_ok) {
            text_missing_ok = 0;
            return 0;
        }
        text = mc_malloc(1024, D_00170D55, 65);
        mc_set_location(66, D_00170D55);
        mc_sprintf(text, D_00170D5C, id);
        return text;
    }
    size = scratch_buffer[i + 1].offset - scratch_buffer[i].offset;
    lseek(text_rsc_file, scratch_buffer[i].offset, 0);
    text = mc_malloc(size + 16, D_00170D55, 73);
    wrap_buf = mc_malloc(size < 4096 ? 8192 : size * 2, D_00170D55, 74);
    expand_buf = mc_malloc(size < 4096 ? 8192 : size * 2, D_00170D55, 75);
    read(text_rsc_file, text, size + 8);
    j = 0;
    i = 1;
    while (text[j] != 254) {
        if (text[j] == 255 && text[j + 1] != 254)
            i++;
        j++;
    }
    if (i != 1)
        i = rand() % i;
    else
        i--;
    j = 0;
    while (i) {
        while (text[j++] != 255)
            ;
        i--;
    }
    while (text[j] < 254)
        text[i++] = text[j++];
    text[i] = 0;
    return text_expand_wrap(flags, width, text, wrap_buf, expand_buf);
}
