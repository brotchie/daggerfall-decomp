/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004EAF4 */
struct e11 { unsigned char type; char rest[10]; };

unsigned char *note_page_walk(unsigned char *a1, int (*a2)(unsigned char *), int (*a3)(unsigned char *))
{
    char l_a[12];
    char l_b[12];

    while (*a1 != 0) {
        if (*a1 == 1) {
            if (a2 != 0)
                if (a2(a1) != 0)
                    return a1;
            a1 += 91;
        } else {
            if (a3 != 0)
                if (a3(a1) != 0)
                    return a1;
            (*(struct e11 **)&a1)++;
        }
    }
    return 0;
}
