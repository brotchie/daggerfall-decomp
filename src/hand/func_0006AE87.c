/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006AE87 */
extern int strlen(char *);

char *str_list_skip(char *a1, int a2)
{
    while (a2-- != 0) {
        a1 += strlen(a1) + 1;
        if (*a1 == 0) a1++;
    }
    return a1;
}
