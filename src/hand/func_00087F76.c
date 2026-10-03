/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00087F76 */
struct who { char pad[0x1b]; unsigned short id; };
struct npc { char pad[0xc]; struct who *who; };
extern char D_00176C94[];       /* __FILE__ */
extern int D_00187EE4[];
extern struct who *D_00195AC4;
extern int D_00196A30[];
extern int D_00196A84;
extern int D_00196A9C;
extern void func_0001E0C6(struct npc *, int);
extern void func_0001E34D(struct npc *, int, int);
extern void func_0001E3D3(struct npc *, unsigned short);
extern void func_00086397(struct npc *);
extern void func_00087D71(struct npc *, int, short);
extern void func_00087E68(struct npc *);
extern int func_0009DC25(void);
extern void func_000A0040(void *, int, int, char *, int, int);

void func_00087F76(struct npc *n, unsigned kind, int a3, int mode)
{
    int unused[3];
    int l_10;
    int r;
    int done;
    int l_1C;
    int saved;

    saved = D_00196A9C;
    done = 0;
    func_000A0040(n, 0, 20, D_00176C94, 1045, 4);
    if (mode == 0) {
        func_0001E3D3(n, D_00195AC4->id);
        return;
    }
    while (done == 0) {
        func_00086397(n);
        switch (kind) {
        case 0:
            while (a3 == -1 || a3 == 1)
                a3 = func_0009DC25() % 21;
            if (a3 > 16) {
                func_00087E68(n);
            } else {
                a3 = D_00187EE4[a3];
                func_00087D71(n, a3, -1);
            }
            break;
        case 1:
            if (a3 != -1) {
                if (D_00196A30[a3] != 0) {
                    r = func_0009DC25() % D_00196A30[a3];
                    func_0001E34D(n, a3, r);
                    break;
                }
            }
            r = func_0009DC25() % D_00196A84;
            func_0001E0C6(n, r);
            break;
        }
        if (mode == 1)
            done = n->who->id != D_00195AC4->id;
        else
            done++;
    }
}
