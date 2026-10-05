/* matched by the real Watcom C32 10.0a (-d2): a run of spfx.c from 0x0008A496 to 0x0008A550, kept together for its switch table's alignment */
struct eff { unsigned char op, arg; };
struct mob {
    char pad0[32];
    short stats[8];             /* 0x20 */
    char pad30[137 - 48];
    unsigned char flags[4];     /* 0x89 */
};
extern unsigned spell_resist_flags[];
extern int object_delete(char *);
void spfx_effect_end(struct eff *e, int i, char *a3);

int func_0008A496(int a1, int a2, int a3)
{
    return 0;
}

int func_0008A4BD(int a1, int a2, int a3)
{
    return 0;
}

void spell_end(char *obj)
{
    struct eff *e;
    int i;

    e = (struct eff *)(obj + 71);
    for (i = 0; i < 3; i++) {
        if (e[i].op == 255)
            continue;
        spfx_effect_end(e, i, *(char **)(obj + 67));
    }
    object_delete(obj);
}

void spfx_effect_end(struct eff *e, int i, char *a3)
{
    struct mob *m;

    m = (struct mob *)(a3 + 71);
    switch (e[i].op) {
    case 0:
        m->flags[0] &= 254;
        break;
    case 7:
        m->stats[e[i].arg] += *(short *)((char *)e + i * 5 + 34);
        break;
    case 8:
        *(unsigned *)m->flags &= ~spell_resist_flags[e[i].arg];
        break;
    case 9:
        m->stats[e[i].arg] -= *(short *)((char *)(e + i) + 80);
        break;
    case 11:
        break;
    case 13:
        m->flags[0] &= 251;
        break;
    case 14:
        m->flags[0] &= 247;
        break;
    case 15:
        m->flags[0] &= 239;
        break;
    case 16:
        m->flags[0] &= 223;
        break;
    case 17:
        m->flags[0] &= 191;
        break;
    case 18:
        m->flags[0] &= 127;
        break;
    case 19:
        m->flags[1] &= 254;
        break;
    case 20:
        m->flags[1] &= 253;
        break;
    case 21:
        m->flags[1] &= 251;
        break;
    case 22:
        m->flags[1] &= 247;
        break;
    case 23:
        m->flags[1] &= 239;
        break;
    case 24:
        m->flags[1] &= 223;
        break;
    case 25:
        m->flags[1] &= 191;
        break;
    case 26:
        m->flags[1] &= 127;
        break;
    case 27:
        m->flags[2] &= 254;
        break;
    case 28:
        m->flags[2] &= 253;
        break;
    case 29:
        m->flags[2] &= 251;
        break;
    case 30:
        m->flags[2] &= 247;
        break;
    case 31:
        m->flags[2] &= 239;
        break;
    case 32:
        m->flags[2] &= 223;
        break;
    case 35:
        m->flags[2] &= 191;
        break;
    case 39:
        m->flags[2] &= 127;
        break;
    case 42:
        m->flags[3] &= 254;
        break;
    case 44:
        m->flags[3] &= 253;
        break;
    case 45:
        m->flags[3] &= 251;
        break;
    case 46:
        m->flags[3] &= 247;
        break;
    }
}
