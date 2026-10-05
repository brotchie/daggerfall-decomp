/* steal.c */

#include "dagger.h"

/* between the steal.c and camera.c runs: unit not certain */
int door_find_key(char *p1, int p2, int p3)
{
    found_object = 0;
    D_00190BE4 = p2;
    D_00190BE8 = p3;
    object_foreach(*(int *)(p1 + 0x3f), door_key_match_cb);
    return found_object;
}
