/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001C713 */
struct obj {
    char type;
    char pad1[20];
    unsigned char flags;        /* 0x15 */
    char pad16[5];
    short id;                   /* 0x1b */
    char pad1d[71 - 0x1d];
    char name[560];             /* 0x47 */
    char text[74];              /* 0x277 */
};
extern char D_00170464[];       /* __FILE__ */
extern int player_entity;
extern int D_001966FC[];
extern struct obj *object_create_child(int, int, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);

unsigned short bio_person_add(char *name, char *text, int kind)
{
    struct obj *o;

    if (D_001966FC[kind] == 8)
        return 0xffff;
    o = object_create_child(player_entity, 0, 634);
    o->type = kind + 45;
    o->flags |= 3;
    o->id = D_001966FC[kind]++;
    mc_memcpy(o->name, name, 560, D_00170464, 1334, 4);
    mc_memcpy(o->text, text, 74, D_00170464, 1335, 4);
    return o->id;
}
