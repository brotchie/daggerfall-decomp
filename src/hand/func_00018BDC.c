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
extern struct faction *D_00190DE4;
extern struct faction *D_00190DE8;
extern int D_00190DEC;
extern int D_00190DF0;
extern int D_00190DF4;
extern int D_00190DF8;
extern struct faction *D_00190DFC;
extern struct thing *D_00195AA0;
extern short D_001966AC;
extern struct faction *func_000192EE(short);
extern int func_0001AD21(struct faction *, int);
extern int func_0001AD97(struct faction *, int);

int func_00018BDC(short a1)
{
    struct thing *t;
    struct faction *other;
    struct faction *me;
    int rel;
    int i;
    int j;

    rel = 8;
    me = func_000192EE(a1);
    t = D_00195AA0->child;
    while (t != 0) {
        if (t->type == 10) {
            other = func_000192EE(t->faction);
            D_00190DE4 = other;
            D_00190DE8 = me;
            if (a1 == other->id)
                return 0;
            if (other->parent != 0 && me->parent != 0 && other->parent == me->parent
                || other->parent == me || me->parent == other) {
                if (other->parent != 0)
                    D_00190DFC = other->parent;
                else
                    D_00190DFC = other;
                if (rel > 1)
                    rel = 1;
                D_001966AC += 15;
            }
            if ((func_0001AD97(other, (int)me) || func_0001AD97(me, (int)other)) && rel > 2) {
                D_001966AC += 10;
                rel = 2;
            }
            if ((func_0001AD21(other, (int)me) || func_0001AD21(me, (int)other)) && rel > 3) {
                D_001966AC = 20;
                rel = 3;
            }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && other->enemies[i] == me->enemies[j] && rel > 4) {
                        D_00190DEC = other->enemies[i];
                        rel = 4;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && other->allies[i] == me->allies[j] && rel > 5) {
                        D_00190DF0 = other->allies[i];
                        rel = 5;
                        D_001966AC += 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->allies[i] != 0 && func_0001AD97(other, me->enemies[j]) && rel > 6) {
                        D_00190DF4 = other->allies[i];
                        rel = 6;
                        D_001966AC -= 5;
                    }
            for (i = 0; i < 3; i++)
                for (j = 0; j < 3; j++)
                    if (other->enemies[i] != 0 && func_0001AD21(other, me->allies[j]) && rel > 7) {
                        D_00190DF8 = other->enemies[i];
                        rel = 7;
                        D_001966AC -= 5;
                    }
        }
        t = t->next;
    }
    return rel;
}
