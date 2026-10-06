/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000122F1 */
#include "structs.h"
#include "clib.h"
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_set_number;
extern char D_00170129[];        /* __FILE__ */

int profile_set_number(struct profile *r, int value)
{
    signed char buf[32];
    char *p;
    char *q;
    int i;
    int n;
    unsigned len;
    int cnt;

    if (r->value == 0)
        return 0;
    p = r->value;
    while (*p == ' ')
        p++;
    q = p;
    n = 0;
    while (*p != '\r')
        buf[n++] = *p++;
    buf[n] = 0;
    if (buf[1] == 'x')
        itoa(value, buf + 2, 16);
    else
        itoa(value, buf, 10);
    len = strlen(buf);
    if (len < n) {
        cnt = (int)((r->buffer + r->length) - q) - (n - len);
        mc_memmove(q, q + (n - len), cnt, D_00170129, 768, 4);
        r->length -= n - len;
    } else if (len > n) {
        if ((len - n) + r->length > r->capacity)
            return 0;
        cnt = (int)((r->buffer + r->length) - q) + (len - n);
        mc_memmove(q + (len - n), q, cnt, D_00170129, 786, 4);
        r->length += len - n;
    }
    i = 0;
    while (buf[i] != 0)
        *q++ = buf[i++];
    r->flags |= 128;
    return 1;
}
