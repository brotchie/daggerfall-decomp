/* qmisc.c */

#include "dagger.h"

void quest_remove_objects(unsigned char quest_id)
{
    object_delete_quest_objects(location_object, quest_id);
    object_delete_quest_objects(nonworld_root, quest_id);
}

int quest_section(char *quest, short section)
{
    short offset;
    offset = ((short *)(quest + 0x24))[section];
    return (int)(quest + offset);
}

int quest_record(char *quest, short section, short record_index)
{
    int section_start;
    section_start = quest_section(quest, section);
    return section_start + record_index * qbn_record_sizes[section];
}
