/* qmisc.c */

#include "dagger.h"

void quest_remove_objects(unsigned char a)
{
    object_delete_quest_objects(location_object, a);
    object_delete_quest_objects(nonworld_root, a);
}

int quest_section(char *p1, short p2)
{
    short l;
    l = ((short *)(p1 + 0x24))[p2];
    return (int)(p1 + l);
}

int quest_record(char *p1, short p2, short p3)
{
    int l;
    l = quest_section(p1, p2);
    return l + p3 * qbn_record_sizes[p2];
}
