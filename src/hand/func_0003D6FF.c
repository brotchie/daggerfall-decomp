/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D6FF */
struct entry { short id; int offset; };
extern char D_00170D55[];
extern char D_00170D7F[];
extern struct entry *scratch_buffer;
extern int text_rsc_file;
extern int text_expand_wrap(unsigned short, short, unsigned char *, char *, char *);
extern void lseek(int, int, int);
extern void *mc_malloc(int, char *, int);
extern int read(int, void *, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

int text_rsc_load_variant(short a1, unsigned short a2, short a3, int a4)
{
    short n;
    short i;
    short k;
    int size;
    unsigned char *buf;
    char *b2;
    char *b3;

    lseek(text_rsc_file, 0, 0);
    read(text_rsc_file, &n, 2);
    read(text_rsc_file, scratch_buffer, n);
    n /= 6;
    for (i = 0; i < n; i++) {
        if (scratch_buffer[i].id == a1)
            break;
    }
    if (i >= n) {
        buf = mc_malloc(1024, D_00170D55, 121);
        mc_set_location(122, D_00170D55);
        mc_sprintf((char *)buf, D_00170D7F, a1);
        return (int)buf;
    }
    size = scratch_buffer[i + 1].offset - scratch_buffer[i].offset;
    lseek(text_rsc_file, scratch_buffer[i].offset, 0);
    buf = mc_malloc(size + 16, D_00170D55, 129);
    b2 = mc_malloc(size < 4096 ? 8192 : size * 2, D_00170D55, 130);
    b3 = mc_malloc(size < 4096 ? 8192 : size * 2, D_00170D55, 131);
    read(text_rsc_file, buf, size + 8);
    k = 0;
    i = 1;
    while (buf[k] != 0xfe) {
        if (buf[k] == 0xff && buf[k + 1] != 0xfe)
            i++;
        k++;
    }
    i = a4;
    k = 0;
    while (i != 0) {
        while (buf[k++] != 0xff)
            ;
        i--;
    }
    while (buf[k] < 0xfe)
        buf[i++] = buf[k++];
    buf[i] = 0;
    return text_expand_wrap(a2, a3, buf, b2, b3);
}
