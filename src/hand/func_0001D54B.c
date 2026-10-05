/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001D54B */
#include "records.h"

extern char current_region;
extern struct faction *faction_find(short);

int rumor_is_eligible(struct rumor *rumor, short unused_faction, int regional, int roll)
{
    int unused;
    struct faction *faction1;
    struct faction *faction2;

    faction1 = 0;
    faction2 = 0;
    if (regional != 0) {
        if (current_region != (char)rumor->region)
            return 0;
        return (unsigned char)(rumor->flags & 1);
    }
    if ((int)(unsigned char)(rumor->flags & 12) == 0)
        return 0;
    if (rumor->faction1 != 0)
        faction1 = faction_find(rumor->faction1);
    if (rumor->faction2 != 0)
        faction2 = faction_find(rumor->faction2);
    if (!(faction1 || faction2 || rumor->kind != 100))
        return 1;
    if (faction1 != 0 && (int)(unsigned short)(faction1->flags & 1) != 0 || faction2 != 0 && (int)(unsigned short)(faction2->flags & 1) != 0) {
        if (roll <= 75)
            return 0;
    }
    return 1;
}
