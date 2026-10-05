/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000461E9 */
struct obj {
    unsigned char type;         /* 0x00 */
    char pad1[6];
    int x;                      /* 0x07 */
    int y;                      /* 0x0b */
    int z;                      /* 0x0f */
    char pad13[8];
    short f27;                  /* 0x1b */
    short f29;                  /* 0x1d */
    unsigned int f31;           /* 0x1f */
    char pad23[32];
    char *f67;                  /* 0x43 */
};
struct flag4 {
    unsigned char a;
    unsigned char b;
    unsigned short id:10;
    unsigned short f10:1;
    unsigned short kind:3;
};
extern struct obj *D_00195AC4;
extern struct flag4 dungeon_blocks[];
extern unsigned char dungeon_block_count;
extern struct obj *D_00199720;
extern struct obj *func_000461A3(void);
extern void object_delete(struct obj *);
extern struct obj *object_create_child(char *, int, int);
extern struct obj *object_find_by_id(struct obj *, int);
extern int object_new_id(int);
extern struct obj *marker_find_nth(struct obj *, int, int);

void func_000461E9(void)
{
    struct obj *p;
    int i;

    for (i = 0; i < dungeon_block_count; i++) {
        if (dungeon_blocks[i].a == 0 && dungeon_blocks[i].b == 0)
            if (dungeon_blocks[i].kind == 1 && dungeon_blocks[i].id == 9) {
                D_00199720 = marker_find_nth(D_00195AC4, 8, 0);
                D_00199720 = object_find_by_id(D_00195AC4, D_00199720->f31);
                p = func_000461A3();
                if (p != 0)
                    object_delete(p);
                p = object_create_child(D_00199720->f67, 0, 62);
                p->type = 6;
                p->f29 = 703;
                p->f27 = 0;
                p->f31 = object_new_id(D_00195AC4->f31 >> 16);
                p->x = D_00195AC4->x + 664;
                p->y = D_00195AC4->y - 1281;
                p->z = D_00195AC4->z + 2035;
            }
    }
}
