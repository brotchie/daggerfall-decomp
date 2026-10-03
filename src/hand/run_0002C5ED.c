/* matched by the real Watcom C32 10.0a (-d2): a run of qcom.c from 0x0002C4FA to 0x0002C5ED, kept together for its switch table's alignment */
struct actor { char pad[35]; unsigned char kind; };
struct task {
    char pad0[2];
    short flags;
    unsigned char type;
    int arg1;
    int arg2;
    int start;
    unsigned int delay;
    struct actor *who;
    struct actor *whom;
    char pad29[4];
};
struct quest { char pad[28]; short ntasks; };
extern short D_00178A0E;
extern char D_0017A13C[];
extern int D_00195BF4;
extern struct quest *D_00199764;
extern void func_0002C5ED(struct quest *, struct task *, short);
extern struct actor *func_0002C96B(struct quest *, struct actor *, short);
extern void func_0002C9C2(struct quest *, struct task *);
extern void func_0002CAB0(struct task *);
extern unsigned int func_0002FF2C(struct quest *, struct actor *, struct actor *);
extern struct task *func_000309E8(struct quest *, int);
extern int func_0007D6AE(int, int);
extern int func_000A1079(char *, int, int);

void func_0002C4FA(struct quest *a1)
{
    int i;
    struct task *t;

    t = func_000309E8(a1, 6);
    for (i = 0; a1->ntasks > i; i++, t++) {
        if (t->flags & 2) {
            t->flags &= ~128;
            func_0002CAB0(t);
        }
        if (t->flags & 64)
            func_0002C5ED(a1, t, 0);
    }
}

void func_0002C589(void)
{
    struct task *t;
    int i;

    t = func_000309E8(D_00199764, 6);
    for (i = 0; D_00199764->ntasks > i; i++, t++)
        func_0002C5ED(D_00199764, t, 1);
}

void func_0002C5ED(struct quest *a1, struct task *a2, short a3)
{
    int l_18;
    short saved;

    saved = D_00178A0E;
    if (a3) {
        D_00178A0E = 537;
        if (!(a2->flags & 1024)) {
            a2->flags |= 1024;
            switch (a2->type) {
            case 2:
            case 4:
                a2->who = func_0002C96B(a1, a2->who, a2->flags & 256);
                a2->whom = 0;
                break;
            case 3:
            case 5:
                a2->who = func_0002C96B(a1, a2->who, a2->flags & 256);
                a2->whom = func_0002C96B(a1, a2->whom, a2->flags & 512);
                break;
            default:
                a2->who = a2->whom = 0;
                break;
            }
        }
        a2->start = D_00195BF4;
        switch (a2->type) {
        case 0:
            a2->delay = func_0007D6AE(a2->arg1, a2->arg2);
            break;
        case 1:
            a2->delay = a2->arg1;
            break;
        case 2:
            a2->delay = func_0002FF2C(a1, 0, a2->who) * 384 >> 8;
            if (func_000A1079(D_0017A13C, a2->who->kind, 5))
                a2->delay += 10080;
            break;
        case 3:
            a2->delay = func_0002FF2C(a1, a2->who, a2->whom) * 384 >> 8;
            if (func_000A1079(D_0017A13C, a2->who->kind, 5) || func_000A1079(D_0017A13C, a2->whom->kind, 5))
                a2->delay += 10080;
            break;
        case 4:
            a2->delay = func_0002FF2C(a1, 0, a2->who) * 384 >> 8;
            if (func_000A1079(D_0017A13C, a2->who->kind, 5))
                a2->delay += 10080;
            break;
        case 5:
            a2->delay = func_0002FF2C(a1, 0, a2->who) * 384 >> 8;
            a2->delay += func_0002FF2C(a1, a2->who, a2->whom) * 384 >> 8;
            if (func_000A1079(D_0017A13C, a2->who->kind, 5))
                a2->delay += 10080;
            if (func_000A1079(D_0017A13C, a2->whom->kind, 5))
                a2->delay += 10080;
            break;
        }
        if (a2->flags & 16)
            a2->delay <<= 1;
        D_00178A0E = saved;
    } else if (D_00195BF4 - a2->start > a2->delay) {
        func_0002C9C2(a1, a2);
    }
}
