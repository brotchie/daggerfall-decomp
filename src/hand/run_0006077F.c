/* matched by the real Watcom C32 10.0a (-d2): a run of equip.c from 0x00060430 to 0x0006077F, kept together for its switch table's alignment */
struct pair {
    unsigned char a;
    unsigned char b;
};
struct tmpl {
    char name[32];              /* 0x00 */
    unsigned char used;         /* 0x20 */
    unsigned char f33;          /* 0x21 */
    unsigned char f34;          /* 0x22 */
    struct pair pairs[10];      /* 0x23 */
    short f55;                  /* 0x37 */
    int f57;                    /* 0x39 */
    unsigned char f61;          /* 0x3d */
};
struct spot {
    short x;
    short y;
};
struct item {
    char pad00[32];
    unsigned short f32;         /* 0x20 */
    unsigned short f34;         /* 0x22 */
    int f36;                    /* 0x24 */
    char pad28[2];
    unsigned short f42;         /* 0x2a */
    short f44;                  /* 0x2c */
    short f46;                  /* 0x2e */
    char pad30[2];
    short icon;                 /* 0x32 */
    char pad34[2];
    unsigned char f54;          /* 0x36 */
    unsigned char f55;          /* 0x37 */
    char pad38[11];
    struct spot spots[10];      /* 0x43 */
};
struct pc { char pad0[64]; unsigned short flags; };
extern char D_001758B8[];        /* __FILE__ */
extern unsigned char D_001865CA[];
extern unsigned char D_00186634[];
extern struct pc *player_character;
extern int magic_def_count;
extern struct tmpl *magic_def;
extern int enchant_item_value(struct item *);
extern void item_make_random(unsigned short, struct item *);
extern void item_make(unsigned char, unsigned char, struct item *);
extern void func_0005E874(struct item *);
extern int func_000602C0(int, unsigned short);
extern int rand_range(int, int);
extern void mc_strncpy(void *, char *, int, char *, int);

void item_make_magic(struct item *a1, int a2)
{
    int n;
    int i;
    int kind;
    int j;
    int r;

    if (a2 == -1) {
        n = i = 0;
        for (; i < magic_def_count; i++)
            if (magic_def[i].used == 0)
                n++;
        n = rand_range(0, n - 1);
    } else {
        n = a2;
    }
    for (i = 0; i < magic_def_count; i++) {
        if (magic_def[i].used == 0) {
            if (n == 0)
                break;
            n--;
        }
    }
    switch (magic_def[i].f33) {
    case 0:
        kind = D_001865CA[rand_range(0, 6)];
        break;
    case 1:
        kind = D_00186634[rand_range(0, 4)];
        break;
    case 2:
        kind = 3;
        break;
    }
    if (kind == 12 || kind == 6) {
        if (player_character->flags & 1)
            kind = 12;
        else
            kind = 6;
    }
    do
        item_make_random(kind, a1);
    while (a1->f32 == 3 && a1->f34 == 18);
    mc_strncpy(a1, magic_def[i].name, 32, D_001758B8, 1039);
    for (j = 0; j < 10; j++) {
        if (magic_def[i].pairs[j].a == 255)
            break;
        if (magic_def[i].pairs[j].b == 255) {
            a1->spots[j].x = magic_def[i].pairs[j].a;
            a1->spots[j].y = 0xffff;
        } else {
            a1->spots[j].x = magic_def[i].pairs[j].a;
            a1->spots[j].y = magic_def[i].pairs[j].b;
        }
    }
    a1->f44 = a1->f46 = magic_def[i].f55;
    a1->f54 = magic_def[i].f61;
    if (a1->f54 == 0 && (a1->f32 == 2 || a1->f32 == 3))
        func_0005E874(a1);
    if (a1->f32 == 2) {
        a1->f55 = 2;
        r = func_000602C0(2, a1->f34);
        if (r != -1)
            a1->icon = r + (a1->icon & -128);
    }
    a1->f36 = enchant_item_value(a1);
}

void item_make_artifact(struct item *a1, int a2)
{
    int n;
    int i;
    int j;
    int sel;

    if (a2 == -1) {
        n = i = 0;
        for (; i < magic_def_count; i++)
            if (magic_def[i].used)
                n++;
        n = rand_range(0, n - 1);
    } else {
        n = a2;
    }
    sel = n;
    for (i = 0; i < magic_def_count; i++) {
        if (magic_def[i].used) {
            if (n == 0)
                break;
            n--;
        }
    }
    item_make(magic_def[i].f33, magic_def[i].f34, a1);
    mc_strncpy(a1, magic_def[i].name, 32, D_001758B8, 1094);
    for (j = 0; j < 10; j++) {
        if (magic_def[i].pairs[j].a == 255)
            break;
        if (magic_def[i].pairs[j].b == 255) {
            a1->spots[j].x = magic_def[i].pairs[j].a;
            a1->spots[j].y = 0xffff;
        } else {
            a1->spots[j].x = magic_def[i].pairs[j].a;
            a1->spots[j].y = magic_def[i].pairs[j].b;
        }
    }
    a1->f44 = a1->f46 = magic_def[i].f55;
    a1->f36 = magic_def[i].f57;
    a1->f54 = magic_def[i].f61;
    if (a1->f32 == 2)
        a1->f55 = 2;
    a1->f42 |= 0x820;
    a1->spots[9].y = sel;
    switch (i) {
    case 0:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 12;
        break;
    case 1:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 13;
        a1->f42 |= 4;
        break;
    case 2:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 10;
        a1->f42 |= 4;
        break;
    case 3:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 8;
        break;
    case 4:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 19;
        break;
    case 5:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 16;
        break;
    case 6:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 25;
        break;
    case 7:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 18;
        break;
    case 8:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 21;
        break;
    case 9:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 2;
        break;
    case 46:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 24;
        break;
    case 47:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 26;
        break;
    case 48:
        a1->icon = (player_character->flags & 1) != 0 ? 0xd880 : 0xd800;
        break;
    case 49:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 15;
        break;
    case 50:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 3;
        break;
    case 51:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 9;
        break;
    case 52:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 23;
        break;
    case 53:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 17;
        break;
    case 54:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 7;
        break;
    case 55:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 1;
        break;
    case 56:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 22;
        break;
    case 57:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 20;
        break;
    case 58:
        a1->icon = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 5;
        a1->f42 |= 4;
        break;
    }
}
