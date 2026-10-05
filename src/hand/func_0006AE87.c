/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006AE87 */
extern int strlen(char *);

char *str_list_skip(char *list, int count)
{
    while (count-- != 0) {
        list += strlen(list) + 1;
        if (*list == 0) list++;
    }
    return list;
}
