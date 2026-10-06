/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D6FF */
#include "structs.h"
extern char D_00170D55[];
extern char D_00170D7F[];
extern struct text_rsc_entry *scratch_buffer;
extern int text_rsc_file;
extern int text_expand_wrap(unsigned short, short, unsigned char *, char *, char *);
extern void lseek(int, int, int);
extern void *mc_malloc(int, char *, int);
extern int read(int, void *, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

iptr text_rsc_load_variant(short id, unsigned short flags, short width, int variant)
{
    short n;
    short i;
    short k;
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
        text = mc_malloc(1024, D_00170D55, 121);
        mc_set_location(122, D_00170D55);
        mc_sprintf((char *)text, D_00170D7F, id);
        return (iptr)text;
    }
    size = scratch_buffer[i + 1].offset - scratch_buffer[i].offset;
    lseek(text_rsc_file, scratch_buffer[i].offset, 0);
    text = mc_malloc(size + 16, D_00170D55, 129);
    wrap_buf = mc_malloc(size < 4096 ? 8192 : size * 2, D_00170D55, 130);
    expand_buf = mc_malloc(size < 4096 ? 8192 : size * 2, D_00170D55, 131);
    read(text_rsc_file, text, size + 8);
    k = 0;
    i = 1;
    while (text[k] != 0xfe) {
        if (text[k] == 0xff && text[k + 1] != 0xfe)
            i++;
        k++;
    }
    i = variant;
    k = 0;
    while (i != 0) {
        while (text[k++] != 0xff)
            ;
        i--;
    }
    while (text[k] < 0xfe)
        text[i++] = text[k++];
    text[i] = 0;
    return text_expand_wrap(flags, width, text, wrap_buf, expand_buf);
}
