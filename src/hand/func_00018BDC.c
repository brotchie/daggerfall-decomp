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

int talk_faction_relation(short a1)
{
    struct record *t;
    struct faction *other;
    struct faction *me;
    int rel;
    int i;
    int j;

    rel = 8;
    me = faction_find(a1);
    t = player_entity->children;
    while (t != 0) {
        if (t->type == 10) {
            other = faction_find(t->data.membership.faction);
            scratch_190de4 = other;
            scratch_190de8 = me;
            if (a1 == other->id)
                return 0;
            if (other->parent != 0 && me->parent != 0 && other->parent == me->parent
                || other->parent == me || me->parent == other) {
                if (other->parent != 0)
                    scratch_190dfc = other->parent;
                else
                    scratch_190dfc = other;
                if (rel > 1)
                    rel = 1;
                D_001966AC += 15;
            }
            if ((faction_has_ally(other, me) || faction_has_ally(me, other)) && rel > 2) {
                D_001966AC += 10;
                rel = 2;
            }
            if ((faction_has_enemy(other, me) || faction_has_enemy(me, other)) && rel > 3) {
                D_001966AC = 20;
                rel = 3;
            }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && other->enemies[i] == me->enemies[j] && rel > 4) {
                        scratch_190dec = other->enemies[i];
                        rel = 4;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && other->allies[i] == me->allies[j] && rel > 5) {
                        scratch_190df0 = other->allies[i];
                        rel = 5;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && faction_has_ally(other, me->enemies[j]) && rel > 6) {
                        scratch_190df4 = other->allies[i];
                        rel = 6;
                        D_001966AC -= 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && faction_has_enemy(other, me->allies[j]) && rel > 7) {
                        scratch_190df8 = other->enemies[i];
                        rel = 7;
                        D_001966AC -= 5;
                    }
        }
        t = t->next;
    }
    return rel;
}
