/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000339B2 */
struct item { short f0; unsigned short flags; };
struct cond { short f0; short flags; short type; short value; };
extern char D_001970DD;
extern unsigned char *func_000192EE(short);

int func_000339B2(struct item *a1, struct cond *a2, unsigned char *a3, int a4)
{
    unsigned char *l_10;

    if (a4 != 0) {
        if ((a1->flags & 0x5000) != 0x4000)
            return 0;
        if (a2->type == -6 && a3[24] < 17)
            return 1;
        if (a2->type == -6)
            return 0;
        if (a2->type > -1) {
            if (a3[24] == 11 && a3[24] == a2->type && *(unsigned short *)(a3 + 18) == 40)
                return 1;
            if (a2->type == 40)
                return 0;
            if (a2->type >= 17 && a2->type <= 20 && a3[24] >= 17 && a3[24] <= 20)
                return 1;
            if (a2->type >= 17 && a2->type <= 20)
                return 0;
        }
        if (a2->type > -1 && a3[24] == a2->type)
            return 1;
        if (a2->type > -1)
            return 0;
        if (a2->type == -1 && (a1->flags & 0x4000) != 0)
            return 1;
        if (a2->type == -1)
            return 0;
        if (a2->type == -2 && *func_000192EE(a1->flags & 0x3FF) == a2->value)
            return 1;
        if (a2->type == -2)
            return 0;
        l_10 = func_000192EE(*(short *)(a3 + 18));
        return *l_10 == a2->value ? 1 : 0;
    }
    if ((a1->flags & 0x2000) == 0)
        return 0;
    if ((int)(short)(a2->flags & 0x600) != 0) {
        if (D_001970DD != 0 && (a1->flags & 0x8000) == 0)
            return 0;
        if (D_001970DD == 0 && (a1->flags & 0x8000) != 0)
            return 0;
    }
    if (a2->type == -6 && a3[24] < 17)
        return 1;
    if (a2->type == -6)
        return 0;
    if (a2->type > -1) {
        if (a3[24] == 11 && a3[24] == a2->type && *(unsigned short *)(a3 + 18) == 40)
            return 1;
        if (a2->type == 40)
            return 0;
        if (a2->type >= 17 && a2->type <= 20 && a3[24] >= 17 && a3[24] <= 20)
            return 1;
        if (a2->type >= 17 && a2->type <= 20)
            return 0;
    }
    if (a2->type > -1 && a3[24] == a2->type)
        return 1;
    if (a2->type > -1)
        return 0;
    if (a2->type == -1 && (a1->flags & 0x3FF) == a2->value)
        return 1;
    if (a2->type == -1)
        return 0;
    if (a2->type == -2 && *func_000192EE(a1->flags & 0x3FF) == a2->value)
        return 1;
    if (a2->type == -2)
        return 0;
    l_10 = func_000192EE(a1->flags & 0x3FF);
    return *l_10 == a2->value ? 1 : 0;
}
