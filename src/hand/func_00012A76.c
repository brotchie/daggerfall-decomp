#include "records.h"
/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00012A76 */
#include "structs.h"
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_add_item_string;
#pragma aux (sosconv) profile_find_item;
#pragma aux (sosconv) profile_set_string;
extern char D_00170129[];
extern unsigned strlen(char *);
extern void mc_memmove(char *, char *, unsigned, char *, int, int);
extern short profile_find_item(struct profile *, char *);
extern int profile_set_string(struct profile *, char *);

int profile_add_item_string(struct profile *s, char *key, char *val, int width)
{
    char *p;
    unsigned len;

    if (profile_find_item(s, key) != 0) {
        profile_set_string(s, val);
        return 1;
    }
    p = s->section;
    len = width + 4 + strlen(val);
    if (s->length + len > s->capacity)
        return 0;
    mc_memmove(p + len, p, (int)(s->buffer + s->length - p), D_00170129, 1324, 4);
    while (*key != 0) {
        *p++ = *key++;
        width--;
    }
    while (width-- != 0)
        *p++ = ' ';
    *p++ = '=';
    *p++ = ' ';
    while (*val != 0)
        *p++ = *val++;
    *p++ = '\r';
    *p++ = '\n';
    s->length += len;
    s->flags |= 0x80;
    return 1;
}
