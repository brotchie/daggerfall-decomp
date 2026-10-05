/* talk.c */

#include "dagger.h"

void func_000164EF(void) { }

int func_00016507(int unused) { return 0; }

int building_distance(char *building)
{
    char *object;
    object = object_find_by_id(location_object, *(int *)(building + 0x14));
    return xn_math_approx_dist2d(*(int *)(object + 7), *(int *)(object + 0xf),
                         *(int *)(player_object + 7), *(int *)(player_object + 0xf));
}

/* between the talk.c and faction.c runs: unit not certain */
int faction_find(short id)
{
    int found;
    found = faction_find_r(factions, id);
    return found;
}

/* between the talk.c and faction.c runs: unit not certain */
int func_0001939C(int target, int faction)
{
    scratch_190be4 = target;
    D_00195B84 = 0;
    func_000193DD(faction);
    return D_00195B84;
}
