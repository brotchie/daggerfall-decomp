/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00011F93 */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) func_00011F93;
struct res {
    char pad[0xa0];
    char *start;            /* 0xa0 */
    char *pos;              /* 0xa4 */
};
extern int func_00012D3B(signed char *);
extern int func_000A0D13(signed char *);

int func_00011F93(struct res *r, int *out)
{
    signed char buf[32];
    char *p;
    int val;
    int n;

    if (r->pos != 0)
        p = r->pos;
    else
        p = r->start;
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
        r->pos = ++p;
    else
        r->pos = p;
    if (buf[1] == 'x')
        val = func_00012D3B(buf + 2);
    else
        val = func_000A0D13(buf);
    *out = val;
    return 1;
}
