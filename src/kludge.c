/* kludge.c */

#include "dagger.h"

char *item_add_random_to_container(char *p1, int p2)
{
    char *l1;
    char *l2;
    l1 = object_create_child(p1, 0, 0x6b);
    *l1 = 2;
    l2 = l1 + 0x47;
    item_make_random((unsigned short)p2, l2);
    return l1;
}
