/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00018BDC */
#include "records.h"

extern struct faction *scratch_190de4;
extern struct faction *scratch_190de8;
extern struct faction *scratch_190dec;
extern struct faction *scratch_190df0;
extern struct faction *scratch_190df4;
extern struct faction *scratch_190df8;
extern struct faction *scratch_190dfc;
extern struct record *player_entity;
extern short D_001966AC;
extern struct faction *faction_find(short);
extern int faction_has_enemy(struct faction *, struct faction *);
extern int faction_has_ally(struct faction *, struct faction *);

int talk_faction_relation(short faction_id)
{
    struct record *object;
    struct faction *other;
    struct faction *faction;
    int relation;
    int i;
    int j;

    relation = 8;
    faction = faction_find(faction_id);
    object = player_entity->children;
    while (object != 0) {
        if (object->type == 10) {
            other = faction_find(object->data.membership.faction);
            scratch_190de4 = other;
            scratch_190de8 = faction;
            if (faction_id == other->id)
                return 0;
            if (other->parent != 0 && faction->parent != 0 && other->parent == faction->parent
                || other->parent == faction || faction->parent == other) {
                if (other->parent != 0)
                    scratch_190dfc = other->parent;
                else
                    scratch_190dfc = other;
                if (relation > 1)
                    relation = 1;
                D_001966AC += 15;
            }
            if ((faction_has_ally(other, faction) || faction_has_ally(faction, other)) && relation > 2) {
                D_001966AC += 10;
                relation = 2;
            }
            if ((faction_has_enemy(other, faction) || faction_has_enemy(faction, other)) && relation > 3) {
                D_001966AC = 20;
                relation = 3;
            }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && other->enemies[i] == faction->enemies[j] && relation > 4) {
                        scratch_190dec = other->enemies[i];
                        relation = 4;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && other->allies[i] == faction->allies[j] && relation > 5) {
                        scratch_190df0 = other->allies[i];
                        relation = 5;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && faction_has_ally(other, faction->enemies[j]) && relation > 6) {
                        scratch_190df4 = other->allies[i];
                        relation = 6;
                        D_001966AC -= 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && faction_has_enemy(other, faction->allies[j]) && relation > 7) {
                        scratch_190df8 = other->enemies[i];
                        relation = 7;
                        D_001966AC -= 5;
                    }
        }
        object = object->next;
    }
    return relation;
}
