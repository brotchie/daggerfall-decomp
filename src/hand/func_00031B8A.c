/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00031B8A */
#pragma pack(1)
struct Vec { int x; int y; int z; };
struct Sub {
    short f0;                   /* 0 */
    unsigned char flags;        /* 2 */
    char pad0[15];
    short f18;                  /* 18 */
    char pad1[6];
};
struct Item {
    unsigned char type;         /* 0 */
    char pad0[6];
    struct Vec pos;             /* 7 */
    char pad1[2];
    unsigned short flags;       /* 21 */
    short owner;                /* 23 */
    short f19;                  /* 25 */
    unsigned short id;          /* 27 */
    char pad2[2];
    int f1f;                    /* 31 */
    char pad3[3];
    char c26;                   /* 38 */
    char pad4[4];
    int f2b;                    /* 43 */
    char pad5[4];
    int f33;                    /* 51 */
    char pad6[16];
    struct Sub sub;             /* 71 */
    char name[4];               /* 97 */
};
struct Npc {
    unsigned char type;         /* 0 */
    char pad0[32];
    unsigned short f21;         /* 33 */
    char pad1[12];
    unsigned short ids[4];      /* 47 */
    char pad2;
    char a38[12];               /* 56 */
    char a44[12];               /* 68 */
};
struct Kind { char pad[6]; unsigned char flags; };
struct Slot { char pad[12]; struct Item *obj; };
struct Ent6 { unsigned short a; unsigned short b; unsigned short c; };
struct Rec { char name[43]; struct Sub *recs; };
struct Req {
    char pad0[2];
    short flags;                /* 2 */
    short kind;                 /* 4 */
    short arg;                  /* 6 */
    char pad1[4];
    struct Item *obj;           /* 12 */
};
extern char D_00170A64[];
extern unsigned char D_0017A25C[];
extern short D_0017A270[];
extern int D_001959A8;
extern int D_00195AA4;
extern struct Item *D_00195AC4;
extern struct Rec *D_00195BDC;
extern char *D_00195C44;
extern struct Item *D_00195CE8;
extern unsigned char D_00196268;
extern int D_00196A8C;
extern struct Ent6 *D_00196A90;
extern char D_001970C8[];
extern int D_001970CC;
extern struct Ent6 *D_001970D0;
extern struct Item *D_001970D4;
extern struct Rec *D_001970D8;
extern char D_001970DC;
extern unsigned char D_001970DD;
extern char *D_00199764;
extern struct Npc *func_000191DA(short, int);
extern struct Npc *func_000192EE(short);
extern struct Npc *func_0001961A(unsigned char);
extern struct Slot *func_00030A23(char *, int, int);
extern int func_000339B2(struct Ent6 *, struct Req *, struct Sub *, int);
extern struct Npc *func_0003445C(char *);
extern int func_000344D3(void);
extern int func_000345A5(int);
extern struct Kind *func_000504D8(unsigned short);
extern int func_0007D6AE(int, int);
extern struct Sub *func_0007DF35(int);
extern void func_00086397(char *);
extern void func_00087E68(char *);
extern struct Item *func_0008DCE3(int, int, int);
extern int func_0008DD46(struct Item *, struct Item *);
extern int func_0008EB88(int);
extern int func_0009DC25(void);
extern int func_000A0AD9(char *, char *, int, char *, int);
extern int func_000A1023(void *, void *, int, char *, int, int);

int func_00031B8A(struct Req *r)
{
    struct Ent6 *list;
    struct Ent6 *e;
    int n;
    int found;
    int i;
    int any;
    int tries;
    int npc;
    struct Item *src;
    struct Item *it;
    struct Item *it2;
    struct Rec *rec;
    int *buf;
    struct Sub *sub;
    struct Kind *kind;
    struct Npc *np;
    struct Slot *slot;

    npc = 0;
    r->flags &= 0x0fff;
    if (r->kind == 21) {
        if (D_00195CE8 == 0 || D_00195CE8->f33 != 0)
            return 0;
        kind = func_000504D8(D_00195CE8->id);
        it = func_0008DCE3(D_001959A8, 0, 58);
        sub = func_0007DF35(D_00195AA4);
        if (sub != 0) {
            if ((unsigned short)sub->f18 == 65535)
                sub->f18 = 510;
            if (sub->f18 == 0)
                sub->f18 = func_000191DA(D_00196268, 15)->f21;
            func_000A1023(&it->sub, sub, 26, D_00170A64, 64, 4);
        } else if (D_00195CE8->sub.f0 != 0) {
            it->sub.f18 = D_00195CE8->sub.f0;
        }
        if (D_00195CE8->sub.f0 != 0)
            it->f19 = D_00195CE8->sub.f0;
        else
            it->f19 = sub->f18;
        func_000A0AD9(it->name, (char *)D_00195BDC, 4, D_00170A64, 76);
        func_000A1023(&it->pos, &D_00195CE8->pos, 12, D_00170A64, 77, 4);
        it->type = 41;
        it->id = D_00195CE8->id;
        r->obj = it;
        it->flags = 514;
        it->flags |= (int)(unsigned char)(kind->flags & 1) != 0 ? 4 : 0;
        it->owner = (unsigned short)D_00196268;
        it->c26 = *D_00199764;
        it->f1f = D_00195CE8->f1f;
        it->f2b = it->f1f;
        if ((int)(unsigned short)(it->flags & 4) != 0)
            it->sub.flags |= 16;
        else
            it->sub.flags &= ~16;
        return 1;
    }
    if (r->kind == -8) {
        it = func_0008DCE3(D_001959A8, 0, 0);
        it->type = 65;
        it->id = 0;
        r->obj = it;
        it->f2b = 0;
        it->f1f = func_0008EB88(800);
        it->flags = 514;
        it->f19 = r->arg;
        it->flags |= (int)(unsigned char)(func_000504D8(it->id)->flags & 1) != 0 ? 4 : 0;
        it->owner = (unsigned short)D_00196268;
        it->c26 = *D_00199764;
        return 1;
    }
    if (!(r->kind != -1 || (int)(short)(r->flags & 0x100) != 0)) {
        np = func_000192EE(r->arg);
        if (np->type == 4) {
            kind = func_000504D8(np->ids[0]);
            it = func_0008DCE3(D_001959A8, 0, 58);
            it->f19 = np->f21;
            it->type = 41;
            it->id = np->ids[0];
            it->flags = 514;
            it->flags |= (int)(unsigned char)(kind->flags & 1) != 0 ? 4 : 0;
            it->owner = (unsigned short)D_00196268;
            it->c26 = *D_00199764;
            r->obj = it;
            it->f2b = it->f1f = func_0008EB88(800);
            it->sub.f18 = r->arg;
            return 1;
        }
    }
    if (r->kind == -2) {
        if (r->arg == 10)
            npc = D_0017A270[func_0007D6AE(0, 9)];
        else
            npc = func_0001961A(r->arg)->f21;
        r->kind = -1;
    }
    if (!(r->kind != -4 || r->arg != 10000)) {
        i = func_000344D3();
        if (i == 0) {
            r->kind = -6;
        } else {
            r->kind = -1;
            r->arg = i;
        }
    }
    if (r->kind >= -5 && r->kind <= -3 || r->kind == -7) {
        slot = func_00030A23(D_00199764, 3, r->arg);
        if (slot == 0) {
            r->obj = 0;
            D_001970DC++;
            return 1;
        }
        it = slot->obj;
        if (it == 0) {
            r->obj = 0;
            D_001970DC++;
            return 1;
        }
        switch ((unsigned short)(r->kind + 7)) {
        case 0:
            ((unsigned char *)&r->flags)[1] &= 0xf9;
            r->flags |= (int)(unsigned short)(it->flags & 4) != 0 ? 1024 : 512;
            r->kind = -6;
            break;
        case 2:
            r->arg = it->sub.f18;
            r->kind = -1;
            break;
        case 3:
            np = func_000192EE(it->sub.f18);
            if (np != 0) {
                np = func_0003445C(np->a44);
            } else {
                r->obj = 0;
                r->kind = -5;
                D_001970DC++;
                return 1;
            }
            if (np != 0)
                r->arg = np->f21;
            else
                r->arg = 510;
            r->kind = -1;
            break;
        case 4:
            np = func_000192EE(it->sub.f18);
            if (np != 0) {
                np = func_0003445C(np->a38);
            } else {
                r->obj = 0;
                r->kind = -5;
                D_001970DC++;
                return 1;
            }
            if (np != 0)
                r->arg = np->f21;
            else
                r->arg = 510;
            r->kind = -1;
            break;
        }
    }
    if ((int)(short)(r->flags & 0xff) == 255)
        r->flags = (r->flags & 0xff00) + (func_0009DC25() & 1);
    tries = 0;
retry:
    if ((int)(short)(r->flags & 0xff) == 0) {
        list = D_00196A90;
        n = D_00196A8C;
        src = D_00195AC4;
        rec = D_00195BDC;
    } else {
        func_00086397(D_001970C8);
        func_00087E68(D_001970C8);
        n = D_001970CC;
        list = D_001970D0;
        src = D_001970D4;
        rec = D_001970D8;
    }
    buf = (int *)D_00195C44;
    found = 0;
    any = 0;
    if ((int)(short)(r->flags & 0x600) != 0)
        D_001970DD = (int)(short)(r->flags & 0x200) != 0 ? 1 : 0;
    else
        D_001970DD = func_0009DC25() & 1;
    if ((int)(short)(r->flags & 0x100) == 0) {
        for (found = i = 0, e = list; i < n; i++, e++) {
            if (e->a != 65535 && func_000339B2(e, r, &rec->recs[e->a], 0) != 0)
                buf[found++] = (e->a << 16) + e->c;
        }
    }
    if (r->kind != -6 && found == 0 || (int)(short)(r->flags & 0x100) != 0) {
        any = 1;
        for (i = 0, e = list; i < n; i++, e++) {
            if ((int)(short)(r->flags & 0x100) != 0 && func_000339B2(e, r, &rec->recs[e->a], 1) != 0)
                buf[found++] = (e->a << 16) + e->c;
            else if (e->a != 65535 && func_000339B2(e, r, &rec->recs[e->a], 1) != 0)
                buf[found++] = (e->a << 16) + e->c;
        }
    }
    if (found == 0 && (int)(short)(r->flags & 0xff) == 0) {
        if (r->kind == -1)
            npc = r->arg;
        if (r->kind == -2)
            npc = func_0001961A(r->arg)->f21;
        r->kind = -6;
    }
    if (found == 0 && r->kind != -6) {
        if (r->kind == -1)
            npc = r->arg;
        if (r->kind == -2)
            npc = func_0001961A(r->arg)->f21;
        if (tries < 50)
            goto retry;
        r->kind = -6;
        tries = 0;
        goto retry;
    }
    if (found == 0)
        return 0;
    i = func_0009DC25() % found;
    if (any != 0 && func_000345A5((src->f1f & 0xffff0000) + (buf[i] & 0xffff)) != 0 && ++tries < 100)
        goto retry;
    it = func_0008DCE3(D_001959A8, 0, 58);
    func_000A1023(&it->pos, &src->pos, 12, D_00170A64, 307, 4);
    func_000A1023(&it->sub, &rec->recs[(unsigned)buf[i] >> 16], 26, D_00170A64, 308, 4);
    func_000A0AD9(it->name, (char *)rec, 4, D_00170A64, 309);
    if (it->sub.f18 == 0)
        it->sub.f18 = func_000191DA(D_00196268, 15)->f21;
    if (npc != 0)
        it->sub.f18 = npc;
    else if (r->kind == -1)
        it->sub.f18 = r->arg;
    it->type = 41;
    it->id = 0;
    r->obj = it;
    if (any != 0 && (int)(short)(r->flags & 0x100) != 0) {
        it->f1f = func_0008EB88((unsigned)src->f1f >> 16);
        it->f2b = it->f1f;
    } else {
        it->f1f = (src->f1f & 0xffff0000) + (buf[i] & 0xffff);
        it->f2b = it->f1f;
    }
    it->flags = 514;
    if (npc != 0) {
        it->id = func_000192EE(npc)->ids[D_001970DD];
        it->f19 = npc;
    } else if (r->kind == -1) {
        it->id = func_000192EE(r->arg)->ids[D_001970DD];
        it->f19 = r->arg;
    } else if (it->sub.f18 > 0) {
        it->id = func_000192EE(it->sub.f18)->ids[D_001970DD];
        it->f19 = it->sub.f18;
    } else {
        it->id = D_0017A25C[D_001970DD * 10 + func_0007D6AE(0, 9)] + 23296;
        it->f19 = 510;
    }
    kind = func_000504D8(it->id);
    it->flags |= (int)(unsigned char)(kind->flags & 1) != 0 ? 4 : 0;
    it->owner = (unsigned short)D_00196268;
    it->c26 = *D_00199764;
    if (any != 0 && (int)(short)(r->flags & 0x100) == 0) {
        it2 = func_0008DCE3(D_001959A8, 0, 26);
        it2->type = 40;
        it2->f1f = it->f1f;
        it2->c26 = *D_00199764;
        func_0008DD46(it2, it);
        it->f1f = func_0008EB88((unsigned)it->f1f >> 16);
    }
    func_00086397(D_001970C8);
    return 1;
}
