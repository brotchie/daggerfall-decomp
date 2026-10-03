/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000306FE */
#pragma pack(1)
struct ent { char pad0[7]; unsigned char flags; char pad8[4]; unsigned char kind; char pad13[4]; };
#pragma pack()
extern int D_00196A28;
extern struct ent *D_00196A9C;

void func_000306FE(int a1, int a2, int a3)
{
    int l_1C;
    struct ent *p;
    int n;
    int i;

    l_1C = *(int *)((char *)a2 + 32);
    n = *(unsigned short *)((char *)l_1C + 27);
    p = D_00196A9C;
    for (i = 0; i < D_00196A28; i++, p++) {
        if (p->kind != 255)
            if (n-- == 0) break;
    }
    p->flags |= 64;
}
