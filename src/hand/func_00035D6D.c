/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00035D6D */
struct rec {
    signed char x;
    signed char y;
    unsigned short num:10;
    unsigned short pad:1;
    unsigned short kind:3;
};
struct obj {
    char pad0[7];
    int x;                  /* 0x07 */
    char pad0b[4];
    int z;                  /* 0x0f */
    short f13;
    char pad15[2];
    short f17;
    short f19;
    short id;               /* 0x1b */
};
struct hdr { int f0; int w; int f8; int tbl; };
extern char D_00170AB4[];       /* __FILE__ */
extern char D_00170ABC[];
extern char D_00170AC7[];
extern unsigned char D_0017A844[];
extern char text_buffer[];
extern struct obj *D_00195AC4;
extern char *D_00195C44;
extern unsigned char D_001962A1;
extern int blocks_bsa;
extern struct obj *D_001995D4[2][2];
extern char *D_001995E8;
extern int D_001995F8;
extern int D_001995FC;
extern struct hdr *D_00199604;
extern struct rec *D_00199608;
extern int D_00199614;
extern int archive_find_record(int, char *, int);
extern int archive_read_record(int, int, char *);
extern void rdb_create_objects(struct obj *, char *, int);
extern void func_000367E5(struct obj *, char *, int);
extern struct obj *object_create_in_block(struct obj *, unsigned char, int, int, int);
extern int func_00135E90();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void dungeon_load_rdb_block(struct rec *r)
{
    int i;
    int j;
    int *tbl;
    int n;
    struct obj *o;
    char *p;

    D_00199608 = r;
    D_001995E8 = D_00195C44;
    func_000A0ED9(59, D_00170AB4);
    func_000A0F5C(text_buffer, D_00170ABC, D_0017A844[r->kind], r->num);
    n = archive_find_record(blocks_bsa, text_buffer, 13);
    archive_read_record(blocks_bsa, n, D_001995E8);
    D_001995F8 = 16;
    D_001995FC = 10000;
    D_001962A1 = 0;
    D_00199614 = 0;
    D_00199604 = (struct hdr *)D_001995E8;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            D_001995D4[j][i] = object_create_in_block(D_00195AC4, 47, 0, 0, i * D_00199604->w + j);
            D_001995D4[j][i]->id = r->num;
            D_001995D4[j][i]->x = D_00195AC4->x + (j << 10) + (r->x << 11);
            D_001995D4[j][i]->z = D_00195AC4->z + (i << 10) + (r->y << 11);
        }
    }
    tbl = (int *)(D_00199604->tbl + D_001995E8);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (tbl[i * D_00199604->w + j] <= 0)
                continue;
            func_00135E90();
            p = tbl[i * D_00199604->w + j] + D_001995E8;
            rdb_create_objects(D_001995D4[j][i], p, i * D_00199604->w + j);
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (tbl[i * D_00199604->w + j] <= 0)
                continue;
            p = tbl[i * D_00199604->w + j] + D_001995E8;
            func_000367E5(D_001995D4[j][i], p, i * D_00199604->w + j);
            D_001995D4[j][i]->f17 = D_001995F8;
            D_001995D4[j][i]->f19 = D_001995FC;
            D_001995D4[j][i]->f13 = (unsigned short)D_001962A1;
        }
    }
    o = object_create_in_block(D_001995D4[0][0], 60, 512, 0, 0);
    func_000A0ED9(107, D_00170AB4);
    func_000A0F5C(text_buffer, D_00170AC7, D_0017A844[r->kind], r->num);
    n = archive_find_record(blocks_bsa, text_buffer, 13);
    archive_read_record(blocks_bsa, n, (char *)o + 71);
    o->x = D_001995D4[0][0]->x;
    o->z = D_001995D4[0][0]->z;
}
