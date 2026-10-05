/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009A7B8 */
#include "records.h"

#pragma pack(1)
struct header_copy {            /* a copy of a record's 71-byte header (found_marker) */
    char pad00[7];
    int x;                      /* +0x07 */
    int y;                      /* +0x0B */
    int z;                      /* +0x0F */
    char pad13[52];
};
#pragma pack()
extern char D_00177358[];
extern unsigned char D_001940D5;
extern struct record *player_object;
extern struct record *marker_find_nearest(struct record *, int);
extern int abs(int);
extern void mc_memcpy(void *, void *, int, char *, int, int);

void ladder_climb(void)
{
    struct header_copy a;
    struct header_copy b;
    struct record *q;
    struct record *p;

    p = player_object->parent;
    while (p != 0 && (p->flags & 1) == 0)
        p = p->parent;
    if (p == 0 || p->type != 43) return;
    q = marker_find_nearest(p->children, 19);
    if (q == 0) return;
    mc_memcpy(&a, q, 71, D_00177358, 521, 4);
    q = marker_find_nearest(p->children, 20);
    if (q == 0) return;
    mc_memcpy(&b, q, 71, D_00177358, 525, 4);
    if (abs(player_object->y - a.y) > abs(player_object->y - b.y)) {
        player_object->x = a.x;
        player_object->y = a.y;
        player_object->z = a.z;
    } else {
        player_object->x = b.x;
        player_object->y = b.y;
        player_object->z = b.z;
    }
    D_001940D5 |= 2;
}
