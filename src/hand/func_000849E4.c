/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000849E4 */
#include "records.h"

extern struct record *rmb_make_marker(struct record *, int);

void rmb_add_editor_marker(struct record *parent, struct block_flat *flat)
{
    struct record *marker;
    int unused;
    int kind;
    int unused2;

    kind = (flat->image & 31) - 2;
    switch (kind) {
    case 8:
    case 9:
    case 13:
    case 14:
    case 16:
    case 17:
        marker = rmb_make_marker(parent, flat->image);
        marker->x = flat->x;
        marker->z = flat->z;
        marker->y = flat->y;
        break;
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
    case 10: case 11: case 12: case 15:
        break;
    }
}
