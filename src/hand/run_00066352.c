/* matched by the real Watcom C32 10.0a (-d2): a run of disease.c from 0x0006630B to 0x00066352, kept together for its switch table's alignment */
struct pick {
    char pad0[12];
    struct obj *obj;            /* 0x0c */
};
struct obj {
    unsigned char type;         /* 0x00 */
    char pad01[20];
    unsigned short flags;       /* 0x15 */
    char pad17[4];
    unsigned short f27;         /* 0x1b */
    char pad1d[34];
    struct obj *list;           /* 0x3f */
    char pad43[4];
    unsigned char data[1];      /* 0x47 */
};
struct item {
    unsigned char cond;         /* 0x00 */
    char pad01[30];
    short stats[8];             /* 0x1f */
};
struct dis { char pad0[35]; unsigned short kind; };
struct pc {
    char pad00[32];
    short stats[8];             /* 0x20 */
    short maxstats[8];          /* 0x30 */
    unsigned char f64;          /* 0x40 */
    char pad41;
    unsigned char f66;          /* 0x42 */
    unsigned char f67;          /* 0x43 */
    char pad44[40];
    short f108;                 /* 0x6c */
    char pad6e[65];
    short f175;                 /* 0xaf */
    char padb1[76];
    short f253;                 /* 0xfd */
    char padff[10];
    short f265;                 /* 0x109 */
    char pad10b[16];
    short f283;                 /* 0x11b */
    char pad11d[52];
    short f337;                 /* 0x151 */
    char pad153[22];
    short f361;                 /* 0x169 */
    char pad16b[135];
    unsigned char f498;         /* 0x1f2 */
    int f499;                   /* 0x1f3 */
    char pad1f7[38];
    unsigned char f541;         /* 0x21d */
};
extern char D_00175970[];        /* __FILE__ */
extern unsigned char D_00186DE3[];
extern unsigned char D_001940D8;
extern struct obj *player_entity;
extern struct obj *D_00195AC4;
extern struct pc *player_character;
extern unsigned char *player_class;
extern unsigned char current_region;
extern unsigned char D_00196294;
extern int region_dungeon_type_counts;
extern struct dis *faction_find_type_in_region(short, int);
extern void func_0001E34D(struct pick *, int, int);
extern void msgbox_show_rsc(int, int);
extern void time_pass(int);
extern void paperdoll_draw(int, int);
extern void item_make(int, int, unsigned char *);
extern void disease_toggle_memberships_cb(struct obj *);
extern void func_00066853(struct obj *, unsigned char);
extern int disease_is_lycanthrope(void);
extern void location_free(struct pick *);
extern void map_goto_location(unsigned char, int, unsigned short, int);
extern void spfx_cure_disease(struct obj *, struct pc *);
extern struct obj *object_create_child(struct obj *, int, int);
extern void object_foreach(struct obj *, void (*)(struct obj *));
extern struct obj *object_find_item(struct obj *, short, short);
extern void inv_store_item(struct obj *);
extern int marker_find_nth(struct obj *, int, int);
extern void player_to_nearest_marker(struct obj *, int);
extern int func_0009DC25(void);
extern void func_000A1023(void *, void *, int, char *, int, int);

void disease_toggle_memberships_cb(struct obj *a1)
{
    if (a1->type == 10) {
        a1->type = 29;
        return;
    }
    if (a1->type == 29)
        a1->type = 10;
}

void disease_become_vampire(void)
{
    struct pick s;
    int u48;
    struct obj *o2;
    int u40;
    struct item *p;
    int u38;
    int i;
    struct obj *o1;
    int u2c;
    int kind;
    int saved;
    int u20;
    struct dis *d;
    int excess;

    if (player_character->f67 > 7 || disease_is_lycanthrope() != 0)
        return;
    player_character->f499 = 0;
    player_character->f108 = 0;
    msgbox_show_rsc(401, 1);
    saved = D_00196294;
    D_00196294 = 1;
    time_pass(30240);
    D_00196294 = saved;
    if (region_dungeon_type_counts != 0) {
        func_0001E34D(&s, 0, func_0009DC25() % region_dungeon_type_counts);
        map_goto_location(current_region, 3, s.obj->f27, 0);
        if (marker_find_nth(D_00195AC4, 9, 0) != 0)
            player_to_nearest_marker(D_00195AC4, 9);
        location_free(&s);
    }
    d = faction_find_type_in_region(current_region, 7);
    if (d != 0)
        kind = d->kind;
    else
        kind = 153;
    D_001940D8 |= 8;
    player_character->f64 |= 20;
    o1 = object_create_child(player_entity, 0, 47);
    o1->type = 11;
    o1->flags = 0x8003;
    o2 = object_create_child(player_entity, 0, 74);
    o2->type = 28;
    o2->flags = 3;
    p = (struct item *)o1->data;
    p->cond = 100;
    func_000A1023(o2->data, player_class, 74, D_00175970, 407, 4);
    for (i = 0; i < 8; i++) {
        if (i == 1)
            continue;
        p->stats[i] = 20;
        player_character->stats[i] += 20;
        player_character->maxstats[i] += 20;
        excess = player_character->maxstats[i] - 100;
        if (excess > 0) {
            player_character->maxstats[i] -= excess;
            player_character->stats[i] -= excess;
            p->stats[i] -= excess;
        }
    }
    player_character->f175 += 30;
    player_character->f283 += 30;
    player_character->f253 += 30;
    player_character->f361 += 30;
    player_character->f265 += 30;
    player_character->f337 += 30;
    player_class[1] |= 0x41;
    player_class[4] |= 0x30;
    spfx_cure_disease(player_entity, player_character);
    object_foreach(player_entity->list, disease_toggle_memberships_cb);
    player_character->f498 = player_character->f67;
    player_character->f67 = 8;
    player_character->f66 = 2;
    o2 = object_find_item(player_entity->list, 27, 0);
    if (o2 == 0) {
        o2 = object_create_child(D_00195AC4, 0, 107);
        o2->type = 2;
        o2->flags |= 1;
        item_make(27, 0, o2->data);
        inv_store_item(o2);
    }
    i = 0;
    while (D_00186DE3[i] != 255)
        func_00066853(o2, D_00186DE3[i++]);
    switch (kind - 150) {
    case 0:
        func_00066853(o2, 85);
        break;
    case 5:
        func_00066853(o2, 50);
        break;
    case 4:
        func_00066853(o2, 10);
        break;
    case 2:
        func_00066853(o2, 64);
        break;
    case 7:
        p->stats[1] += 20;
        player_character->stats[1] += 20;
        player_character->maxstats[1] += 20;
        excess = player_character->maxstats[i] - 100;
        if (excess > 0) {
            player_character->maxstats[1] -= excess;
            player_character->stats[1] -= excess;
            p->stats[1] -= excess;
        }
        break;
    case 6:
        func_00066853(o2, 17);
        break;
    case 8:
        func_00066853(o2, 11);
        func_00066853(o2, 12);
        func_00066853(o2, 13);
        break;
    case 3:
        func_00066853(o2, 23);
        func_00066853(o2, 6);
        break;
    case 1:
        func_00066853(o2, 20);
        func_00066853(o2, 33);
        break;
    }
    player_character->f541 = kind;
    paperdoll_draw(0, 0);
}
