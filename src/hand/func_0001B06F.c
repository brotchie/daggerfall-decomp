/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001B06F */
#include "records.h"

extern int D_00195B84;
extern struct faction *D_0019670C;
extern struct faction *D_0019671C;
extern unsigned char faction_subtree_search_r(struct faction *);

int faction_tree_relation(struct faction *a, struct faction *b)
{
    struct faction *save;

    save = a;
    D_0019671C = b;
    D_0019670C = a->parent;
    D_00195B84 = 0;
    if ((unsigned char)(faction_subtree_search_r(b->child) & 1))
        return 3;
    if (a->parent != 0) {
        a = a->parent->child;
        while (a != 0) {
            if (a == b)
                return 2;
            a = a->next;
        }
    }
    a = save;
    if (a->parent != 0) {
        a = a->parent;
        while (a != 0) {
            if (a == b)
                return 1;
            a = a->parent;
        }
    }
    return 0;
}
