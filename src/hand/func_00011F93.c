/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00011F93 */
#include "structs.h"
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_get_number;
extern int profile_hex_to_int(signed char *);
extern int atoi(signed char *);

int profile_get_number(struct profile *r, int *out)
{
    signed char buf[32];
    char *p;
    int val;
    int n;

    if (r->next_value != 0)
        p = r->next_value;
    else
        p = r->value;
    if (p == 0)
        return 0;
    while (*p == ' ')
        p++;
    if (*p == '\r')
        return 0;
    n = 0;
    while (!(*p == '\r' || *p == ',' || *p == ' '))
        buf[n++] = *p++;
    buf[n] = 0;
    if (n == 0)
        return 0;
    while (*p == ' ')
        p++;
    if (*p == ',')
        r->next_value = ++p;
    else
        r->next_value = p;
    if (buf[1] == 'x')
        val = profile_hex_to_int(buf + 2);
    else
        val = atoi(buf);
    *out = val;
    return 1;
}
