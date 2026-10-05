/* talk.c */

#include "dagger.h"

void func_000164EF(void) { }

int func_00016507(int a) { return 0; }

int building_distance(char *p)
{
    char *l;
    l = object_find_by_id(D_00195AC4, *(int *)(p + 0x14));
    return func_000C7FD9(*(int *)(l + 7), *(int *)(l + 0xf),
                         *(int *)(player_object + 7), *(int *)(player_object + 0xf));
}

/* between the talk.c and faction.c runs: unit not certain */
int faction_find(short a)
{
    int r;
    r = faction_find_r(factions, a);
    return r;
}

/* between the talk.c and faction.c runs: unit not certain */
int func_0001939C(int a, int b)
{
    D_00190BE4 = a;
    D_00195B84 = 0;
    func_000193DD(b);
    return D_00195B84;
}
