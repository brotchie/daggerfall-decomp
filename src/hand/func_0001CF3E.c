/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001CF3E */
struct npc {
    unsigned char f0;
    unsigned char face;         /* 0x01 */
    char pad2[31];
    short id;                   /* 0x21 */
};
struct msg {
    short id1;                  /* 0x00 */
    short id2;                  /* 0x02 */
    int kind;                   /* 0x04 */
    unsigned char sub;          /* 0x08 */
    unsigned char chk;          /* 0x09 */
    unsigned char zero;         /* 0x0a */
    char name[9];               /* 0x0b */
    short f20;                  /* 0x14 */
    int f22;                    /* 0x16 */
    int len;                    /* 0x1a */
    int time;                   /* 0x1e */
};
extern char D_00170464[];        /* __FILE__ */
extern unsigned char D_00190D16;
extern unsigned char D_00190D17;
extern char text_rsc_buffer[];
extern int D_00195B84;
extern int game_minutes;
extern unsigned char D_00196269;
extern unsigned char D_001962A5;
extern int rumor_file;
extern int D_00196708;
extern struct npc *D_0019670C;
extern struct npc *D_0019671C;
extern int faction_player_related(struct npc *);
extern unsigned char func_0001D66C(struct msg *);
extern void parse_rsc_text(int, int, int);
extern int rand_range(int, int);
extern void func_000A0040(char *, int, int, char *, int, int);
extern int func_000A0B42(int, void *, int);
extern int func_000A0DF4(char *);

void rumor_add_faction(struct npc *a1, struct npc *a2, int a3, unsigned char a4, int a5)
{
    int unused;
    struct msg m;
    int saved;

    saved = D_00195B84;
    if (D_001962A5 == 0)
        return;
    if (a5 != 455 && a5 != 456)
        if (a3 == 100 && faction_player_related(a1) == 0 && faction_player_related(a2) == 0) {
            D_00195B84 = saved;
            return;
        }
    D_0019671C = a1;
    D_0019670C = a2;
    D_00190D16 = a3;
    D_00190D17 = a4;
    D_00196708++;
    if (a4 != 0)
        D_00196269 = a4;
    else if (a1 != 0 && a1->face != 255)
        D_00196269 = a1->face;
    else if (a2 != 0 && a2->face != 255)
        D_00196269 = a2->face;
    else
        D_00196269 = rand_range(0, 61);
    parse_rsc_text(a5, 0, 0);
    if (a1 != 0)
        m.id1 = a1->id;
    else
        m.id1 = 0;
    if (a2 != 0)
        m.id2 = a2->id;
    else
        m.id2 = 0;
    m.kind = a3;
    m.sub = a4;
    m.chk = func_0001D66C(&m);
    m.zero = 0;
    func_000A0040(m.name, 0, 9, D_00170464, 1606, 9);
    m.f20 = 0;
    m.f22 = 0;
    m.len = func_000A0DF4(text_rsc_buffer) + 1;
    m.time = game_minutes + 43140;
    func_000A0B42(rumor_file, &m, 34);
    func_000A0B42(rumor_file, text_rsc_buffer, m.len);
    D_00195B84 = saved;
}
