/* kludge.c */

#include "dagger.h"

char *item_add_random_to_container(char *container, int group)
{
    char *object;
    char *item;
    object = object_create_child(container, 0, 0x6b);
    *object = 2;
    item = object + 0x47;
    item_make_random((unsigned short)group, item);
    return object;
}
