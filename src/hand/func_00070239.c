/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00070239 */
#include "records.h"

extern unsigned char scratch_190d20;
extern short guild_search_faction;
extern struct record *player_entity;
extern struct record *found_object;
extern void guild_match_membership(struct record *);
extern void object_foreach(struct record *, void (*)());

struct membership *guild_find_membership_by_faction(short faction_id)
{
    found_object = 0;
    guild_search_faction = faction_id;
    scratch_190d20 = 255;
    object_foreach(player_entity->children, guild_match_membership);
    if (found_object == 0)
        return 0;
    return &found_object->data.membership;
}
