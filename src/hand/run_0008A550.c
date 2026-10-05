/* matched by the real Watcom C32 10.0a (-d2): a run of spfx.c from 0x0008A496 to 0x0008A550, kept together for its switch table's alignment */
#include "records.h"

extern unsigned spell_resist_flags[];
extern int object_delete(struct record *);
void spfx_effect_end(struct spell *e, int i, struct record *target);

int spfx_wall_of_frost(int spell_object, int effect, int target)
{
    return 0;
}

int spfx_wall_of_poison(int spell_object, int effect, int target)
{
    return 0;
}

void spell_end(struct record *obj)
{
    struct spell *e;
    int i;

    e = &obj->data.spell;
    for (i = 0; i < 3; i++) {
        if (e->effects[i].type == 255)
            continue;
        spfx_effect_end(e, i, obj->parent);
    }
    object_delete(obj);
}

void spfx_effect_end(struct spell *e, int i, struct record *target)
{
    struct character *m;

    m = &target->data.character;
    switch (e->effects[i].type) {
    case 0:
        m->conditions &= ~0x1;
        break;
    case 7:
        m->attributes[e->effects[i].subtype] += *(short *)&e->magnitudes[i].plus_min;
        break;
    case 8:
        m->conditions &= ~spell_resist_flags[e->effects[i].subtype];
        break;
    case 9:
        m->attributes[e->effects[i].subtype] -= e->cast_magnitudes[i];
        break;
    case 11:
        break;
    case 13:
        m->conditions &= ~0x4;
        break;
    case 14:
        m->conditions &= ~0x8;
        break;
    case 15:
        m->conditions &= ~0x10;
        break;
    case 16:
        m->conditions &= ~0x20;
        break;
    case 17:
        m->conditions &= ~0x40;
        break;
    case 18:
        m->conditions &= ~0x80;
        break;
    case 19:
        m->conditions &= ~0x100;
        break;
    case 20:
        m->conditions &= ~0x200;
        break;
    case 21:
        m->conditions &= ~0x400;
        break;
    case 22:
        m->conditions &= ~0x800;
        break;
    case 23:
        m->conditions &= ~0x1000;
        break;
    case 24:
        m->conditions &= ~0x2000;
        break;
    case 25:
        m->conditions &= ~0x4000;
        break;
    case 26:
        m->conditions &= ~0x8000;
        break;
    case 27:
        m->conditions &= ~0x10000;
        break;
    case 28:
        m->conditions &= ~0x20000;
        break;
    case 29:
        m->conditions &= ~0x40000;
        break;
    case 30:
        m->conditions &= ~0x80000;
        break;
    case 31:
        m->conditions &= ~0x100000;
        break;
    case 32:
        m->conditions &= ~0x200000;
        break;
    case 35:
        m->conditions &= ~0x400000;
        break;
    case 39:
        m->conditions &= ~0x800000;
        break;
    case 42:
        m->conditions &= ~0x1000000;
        break;
    case 44:
        m->conditions &= ~0x2000000;
        break;
    case 45:
        m->conditions &= ~0x4000000;
        break;
    case 46:
        m->conditions &= ~0x8000000;
        break;
    }
}
