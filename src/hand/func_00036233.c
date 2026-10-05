/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00036233 */
struct node {
    int next;
    char pad4[4];
    int x, y, z;
    unsigned char kind:6;
    int data;
};
struct place { int x, y, z; short f12; short f14; char pad16[3]; int f19; };
struct spot { short f0; char pad2[6]; short f8; };
struct item { unsigned short f0; short f2; unsigned char f4; unsigned char f5; char pad6[4]; unsigned char f10; };
struct sub { unsigned short f0; unsigned char f2; };
struct obj {
    unsigned char type;
    char pad1[6];
    int x, y, z;
    unsigned short f19;
    char pad21[2];
    unsigned short f23;
    char pad25[2];
    unsigned short f27;
    unsigned short f29;
    int f31;
    char f35;
    short f36;
    char pad38[33];
    struct sub sub;
    char pad74[9];
    int f83;
};
struct ofs { signed char dx; signed char dz; char pad2; int flags; };
struct names { char pad[20]; char name[1][8]; };
struct pair { int off; int id; };
struct anim { char pad[6]; unsigned short f6; char pad8[16]; short f24; };
struct res { char pad[12]; struct anim *anim; };
extern char D_00170AD2[];
extern unsigned char D_0017A834[];
extern struct obj *D_00195AC4;
extern char D_001962A1;
extern struct pair D_001985D4[];
extern char *D_001995E8;
extern struct item *D_001995F0;
extern struct spot *D_001995F4;
extern int D_001995F8;
extern int D_001995FC;
extern struct names *D_00199604;
extern struct ofs *D_00199608;
extern struct place *D_0019960C;
extern int D_00199614;
extern void func_000361B7(struct place *);
extern void func_000369A0(struct obj *, char *);
extern void fatal_error(char *);
extern int rand_range(int, int);
extern struct obj *rmb_make_marker(int, int);
extern struct obj *rmb_make_flat(int, short, short, int);
extern void object_free_single(struct obj *);
extern struct obj *object_create_in_block(int, unsigned char, int, int, int);
extern void func_000C7F07(int, int, int, int *);
extern struct res *func_00135D00(int, int, int);
extern void func_00135E39(void);

void rdb_create_objects(int a1, struct node *a2, int a3)
{
    int l_30;
    struct obj *obj;
    struct sub *sub;
    struct res *res;
    int l_20;
    int l_1C;
    int off;
    int l_14;
    int h;

    do {
        off = (char *)a2 - D_001995E8;
        switch (a2->kind) {
        case 1:
            D_0019960C = (struct place *)(D_001995E8 + a2->data);
            if (D_0019960C->f19 <= 0)
                func_000361B7(D_0019960C);
            obj = object_create_in_block(a1, 6, 62, 0, a3);
            func_000369A0(obj, D_00199604->name[D_0019960C->f12]);
            if ((obj->f29 == 703 || obj->f29 == 704) && (D_00199608->dx || D_00199608->dz)) {
                object_free_single(obj);
                obj = 0;
                break;
            }
            obj->f36 = 0;
            func_000C7F07(D_0019960C->x, D_0019960C->y, D_0019960C->z, &obj->f83);
            if (obj->f29 == 550) {
                obj->type = 32;
                obj->f23 = D_0017A834[D_0019960C->f14 >> 4];
            }
            break;
        case 2:
            D_001995F4 = (struct spot *)(D_001995E8 + a2->data);
            obj = object_create_in_block(a1, 7, 0, D_001995F4->f0, a3);
            obj->f23 = D_001995F4->f8;
            break;
        case 3:
            D_001995F0 = (struct item *)(D_001995E8 + a2->data);
            if ((D_001995F0->f0 >> 7) == 199) {
                switch ((D_001995F0->f0 & 31) - 2) {
                case 14:
                    obj = rmb_make_marker(a1, D_001995F0->f0);
                    obj->f23 = D_001995F0->f5;
                    obj->f19 = D_001995F0->f4;
                    obj->f35 = D_001995F0->f2;
                    obj->f36 = D_001995F0->f10;
                    break;
                case 13:
                    obj = rmb_make_marker(a1, D_001995F0->f0);
                    obj->f23 = D_001995F0->f5;
                    obj->f19 = D_001995F0->f2;
                    if (obj->f19 == 0)
                        obj->f19 = rand_range(1, 6);
                    obj->f36 = D_001995F0->f10;
                    break;
                case 8:
                    if (D_001995F0->f2 != 0)
                        D_001995F8 = D_001995F0->f2;
                    if (D_001995F0->f5 != 0)
                        D_001995FC = -(D_001995F0->f5 << 3);
                    D_001962A1 = D_001995F0->f4;
                    if ((D_00199608->flags & 4) == 0) {
                        obj = 0;
                        break;
                    }
                default:
                    obj = rmb_make_marker(a1, D_001995F0->f0);
                    break;
                }
            } else if (D_001995F0->f10 == 29) {
                obj = rmb_make_flat(a1, D_001995F0->f0, D_001995F0->f4 + (D_001995F0->f5 << 8), 0);
                sub = &obj->sub;
                if (D_001995F0->f2 & 16)
                    sub->f2 |= 32;
                if (D_001995F0->f2 & 32)
                    sub->f2 |= 16;
                sub->f0 = (D_001995F0->f5 << 8) + D_001995F0->f4;
            } else {
                obj = object_create_in_block(a1, 33, 0, D_001995F0->f0, a3);
            }
            break;
        }
        if (obj != 0) {
            D_001985D4[D_00199614].off = off;
            D_001985D4[D_00199614].id = obj->f31;
            if (D_00199614++ > 512)
                fatal_error(D_00170AD2);
            obj->x = D_00195AC4->x + a2->x + (D_00199608->dx << 11);
            obj->y = D_00195AC4->y + a2->y;
            obj->z = D_00195AC4->z + a2->z + (D_00199608->dz << 11);
            if (a2->kind == 3) {
                res = func_00135D00(obj->f27 >> 7, obj->f27 & 127, 0);
                if (res == 0) {
                    func_00135E39();
                    res = func_00135D00(obj->f27 >> 7, obj->f27 & 127, 0);
                }
                h = res->anim->f6 * (res->anim->f24 + 256) / 256;
                obj->y += h >> 1;
            }
        }
        a2 = (struct node *)(D_001995E8 + a2->next);
    } while ((char *)a2 - D_001995E8 > 0);
}
