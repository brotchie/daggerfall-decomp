/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001D54B */
#include "records.h"

extern char current_region;
extern struct faction *faction_find(short);

int func_0001D54B(struct rumor *r, short a2, int a3, int a4)
{
    int unused;
    struct faction *p1;
    struct faction *p2;

    p1 = 0;
    p2 = 0;
    if (a3 != 0) {
        if (current_region != (char)r->region)
            return 0;
        return (unsigned char)(r->flags & 1);
    }
    if ((int)(unsigned char)(r->flags & 12) == 0)
        return 0;
    if (r->faction1 != 0)
        p1 = faction_find(r->faction1);
    if (r->faction2 != 0)
        p2 = faction_find(r->faction2);
    if (!(p1 || p2 || r->kind != 100))
        return 1;
    if (p1 != 0 && (int)(unsigned short)(p1->flags & 1) != 0 || p2 != 0 && (int)(unsigned short)(p2->flags & 1) != 0) {
        if (a4 <= 75)
            return 0;
    }
    return 1;
}
