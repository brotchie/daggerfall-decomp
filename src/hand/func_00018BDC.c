/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00018BDC */
struct faction {
    char pad0[33];
    unsigned short id;          /* 33 */
    char pad23[21];
    int allies[3];              /* 56 */
    int enemies[3];             /* 68 */
    char pad50[8];
    struct faction *parent;     /* 88 */
};
struct thing {
    unsigned char type;
    char pad1[54];
    struct thing *next;         /* 55 */
    char pad3b[4];
    struct thing *child;        /* 63 */
    char pad43[7];
    short faction;              /* 74 */
};
extern struct faction *text_macro_fpc;
extern struct faction *text_macro_fnpc;
extern int text_macro_fe;
extern int text_macro_fa;
extern int text_macro_fae;
extern int text_macro_fea;
extern struct faction *text_macro_fpa;
extern struct thing *player_entity;
extern short D_001966AC;
extern struct faction *faction_find(short);
extern int faction_has_enemy(struct faction *, int);
extern int faction_has_ally(struct faction *, int);

int talk_faction_relation(short a1)
{
    struct thing *t;
    struct faction *other;
    struct faction *me;
    int rel;
    int i;
    int j;

    rel = 8;
    me = faction_find(a1);
    t = player_entity->child;
    while (t != 0) {
        if (t->type == 10) {
            other = faction_find(t->faction);
            text_macro_fpc = other;
            text_macro_fnpc = me;
            if (a1 == other->id)
                return 0;
            if (other->parent != 0 && me->parent != 0 && other->parent == me->parent
                || other->parent == me || me->parent == other) {
                if (other->parent != 0)
                    text_macro_fpa = other->parent;
                else
                    text_macro_fpa = other;
                if (rel > 1)
                    rel = 1;
                D_001966AC += 15;
            }
            if ((faction_has_ally(other, (int)me) || faction_has_ally(me, (int)other)) && rel > 2) {
                D_001966AC += 10;
                rel = 2;
            }
            if ((faction_has_enemy(other, (int)me) || faction_has_enemy(me, (int)other)) && rel > 3) {
                D_001966AC = 20;
                rel = 3;
            }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && other->enemies[i] == me->enemies[j] && rel > 4) {
                        text_macro_fe = other->enemies[i];
                        rel = 4;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && other->allies[i] == me->allies[j] && rel > 5) {
                        text_macro_fa = other->allies[i];
                        rel = 5;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && faction_has_ally(other, me->enemies[j]) && rel > 6) {
                        text_macro_fae = other->allies[i];
                        rel = 6;
                        D_001966AC -= 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && faction_has_enemy(other, me->allies[j]) && rel > 7) {
                        text_macro_fea = other->enemies[i];
                        rel = 7;
                        D_001966AC -= 5;
                    }
        }
        t = t->next;
    }
    return rel;
}
