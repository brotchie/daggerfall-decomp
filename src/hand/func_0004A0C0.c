/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004A0C0 */
#include "records.h"

extern int text_blank;
extern struct location *current_location;
extern int rand_range(int, int);
extern int building_name(struct building *);

int parse_town_building_name(short a1)
{
    struct building *p;
    short i;
    short c;

    p = current_location->buildings;
    for (c = i = 0; i < current_location->building_count; i++, p++)
        if (p->type == a1) c++;
    if (c == 0) return text_blank;
    if (c == 1)
        c = 0;
    else
        c = rand_range(0, c - 1) + 1;
    p = current_location->buildings;
    while (c != 0) {
        while (p->type != a1) p++;
        c--;
    }
    return building_name(p);
}
