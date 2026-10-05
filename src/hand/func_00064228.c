/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00064228 */
struct obj { char pad[35]; unsigned char f35; };
struct ent {                    /* 39 bytes */
    unsigned short id;
    unsigned char f2;
    unsigned char f3;
    char pad4[31];
    struct obj *o;              /* 35 */
};
struct dict { char pad[31]; char *base; };
extern char D_00175940[];
extern struct dict *D_00195AC4;
extern struct ent D_00199D78[];
extern int link_count;
extern char D_001A3A80;
extern char D_001A3A81;
extern void fatal_error(char *);
extern struct obj *object_find_by_id(struct dict *, char *);

void links_resolve(void)
{
    struct ent *e;
    struct ent *end;

    if (link_count >= 1024)
        fatal_error(D_00175940);
    for (e = D_00199D78, end = D_00199D78 + link_count; e < end; e++) {
        if (e->f2 == 6)
            e->f2 = 2;
        if (e->f3 == 108)
            e->f3 = 100;
        e->o = object_find_by_id(D_00195AC4, (char *)((int)D_00195AC4->base + e->id - 1));
        if (e->o != 0)
            e->o->f35 = 255;
    }
    D_001A3A80 = D_001A3A81 = 0;
}
