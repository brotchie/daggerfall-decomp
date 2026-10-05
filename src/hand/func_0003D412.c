/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D412 */
#pragma pack(1)
struct ent { short id; int off; };
#pragma pack()
extern char D_00170D55[];
extern char D_00170D5C[];
extern struct ent *scratch_buffer;
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

char *text_rsc_load(short a1, unsigned short a2, short a3)
{
    short n;
    short i;
    short j;
    int l_34;
    unsigned char *l_30;
    char *l_2C;
    char *l_28;

    lseek(text_rsc_file, 0, 0);
    read(text_rsc_file, &n, 2);
    read(text_rsc_file, scratch_buffer, n);
    n /= 6;
    for (i = 0; i < n; i++) {
        if (scratch_buffer[i].id == a1)
            break;
    }
    if (i >= n) {
        if (text_missing_ok) {
            text_missing_ok = 0;
            return 0;
        }
        l_30 = mc_malloc(1024, D_00170D55, 65);
        mc_set_location(66, D_00170D55);
        mc_sprintf(l_30, D_00170D5C, a1);
        return l_30;
    }
    l_34 = scratch_buffer[i + 1].off - scratch_buffer[i].off;
    lseek(text_rsc_file, scratch_buffer[i].off, 0);
    l_30 = mc_malloc(l_34 + 16, D_00170D55, 73);
    l_2C = mc_malloc(l_34 < 4096 ? 8192 : l_34 * 2, D_00170D55, 74);
    l_28 = mc_malloc(l_34 < 4096 ? 8192 : l_34 * 2, D_00170D55, 75);
    read(text_rsc_file, l_30, l_34 + 8);
    j = 0;
    i = 1;
    while (l_30[j] != 254) {
        if (l_30[j] == 255 && l_30[j + 1] != 254)
            i++;
        j++;
    }
    if (i != 1)
        i = rand() % i;
    else
        i--;
    j = 0;
    while (i) {
        while (l_30[j++] != 255)
            ;
        i--;
    }
    while (l_30[j] < 254)
        l_30[i++] = l_30[j++];
    l_30[i] = 0;
    return text_expand_wrap(a2, a3, l_30, l_2C, l_28);
}
