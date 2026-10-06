/* qmisc.c */

#include "dagger.h"

void quest_remove_objects(unsigned char quest_id)
{
    object_delete_quest_objects(location_object, quest_id);
    object_delete_quest_objects(nonworld_root, quest_id);
}

iptr quest_section(char *quest, short section)
{
    short offset;
    offset = ((short *)(quest + 0x24))[section];
    return (iptr)(quest + offset);
}

iptr quest_record(char *quest, short section, short record_index)
{
    iptr section_start;
    section_start = quest_section(quest, section);
    return section_start + record_index * qbn_record_sizes[section];
}
