/* steal.c */

#include "dagger.h"
#include "records.h"

/* between the steal.c and camera.c runs: unit not certain */
iptr door_find_key(char *owner, int key_id, int lock_level)
{
    found_object = 0;
    scratch_190be4 = key_id;
    scratch_190be8 = lock_level;
    object_foreach((iptr)((struct record *)owner)->children, door_key_match_cb);
    return found_object;
}
