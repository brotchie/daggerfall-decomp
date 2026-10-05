/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00036233 */
#include "records.h"

struct node {
    int next;
    char pad4[4];
    int x, y, z;
    unsigned char kind:6;
    int data;
};
struct place { int x, y, z; short f12; short f14; char pad16[3]; int f19; };
struct spot { short f0; char pad2[6]; short f8; };
struct rdb_flat { unsigned short f0; short f2; unsigned char f4; unsigned char f5; char pad6[4]; unsigned char f10; };
struct sub { unsigned short f0; unsigned char f2; };
struct ofs { signed char dx; signed char dz; char pad2; int flags; };
struct names { char pad[20]; char name[1][8]; };
struct pair { int off; int id; };
struct anim { char pad[6]; unsigned short f6; char pad8[16]; short f24; };
struct res { char pad[12]; struct anim *anim; };
extern char D_00170AD2[];
extern unsigned char D_0017A834[];
extern struct record *location_object;
extern char D_001962A1;
extern struct pair D_001985D4[];
extern char *D_001995E8;
extern struct rdb_flat *D_001995F0;
extern struct spot *D_001995F4;
extern int D_001995F8;
extern int D_001995FC;
extern struct names *D_00199604;
extern struct ofs *D_00199608;
extern struct place *D_0019960C;
extern int D_00199614;
extern void func_000361B7(struct place *);
extern void rdb_model_id_from_name(struct record *, char *);
extern void fatal_error(char *);
extern int rand_range(int, int);
extern struct record *rmb_make_marker(struct record *, int);
extern struct record *rmb_make_flat(struct record *, short, short, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_create_in_block(struct record *, int, int, int, int);
extern void xn_model_set_angles(int, int, int, int *);
extern struct res *xn_tex_cache_lookup(int, int, int);
extern void xn_tex_cache_flush(void);

void rdb_create_objects(struct record *quarter, struct node *rdb_object, int block_index)
{
    int unused1;
    struct record *object;
    struct sub *sub;
    struct res *res;
    int unused2;
    int unused3;
    int offset;
    int unused4;
    int height;

    do {
        offset = (char *)rdb_object - D_001995E8;
        switch (rdb_object->kind) {
        case 1:
            D_0019960C = (struct place *)(D_001995E8 + rdb_object->data);
            if (D_0019960C->f19 <= 0)
                func_000361B7(D_0019960C);
            object = object_create_in_block(quarter, 6, 62, 0, block_index);
            rdb_model_id_from_name(object, D_00199604->name[D_0019960C->f12]);
            if ((object->image2 == 703 || object->image2 == 704) && (D_00199608->dx || D_00199608->dz)) {
                object_free_single(object);
                object = 0;
                break;
            }
            object->wait_state = 0;
            xn_model_set_angles(D_0019960C->x, D_0019960C->y, D_0019960C->z, (int *)(RECORD_DATA(object) + 12));
            if (object->image2 == 550) {
                object->type = 32;
                object->lock_level = D_0017A834[D_0019960C->f14 >> 4];
            }
            break;
        case 2:
            D_001995F4 = (struct spot *)(D_001995E8 + rdb_object->data);
            object = object_create_in_block(quarter, 7, 0, D_001995F4->f0, block_index);
            object->light_radius = D_001995F4->f8;
            break;
        case 3:
            D_001995F0 = (struct rdb_flat *)(D_001995E8 + rdb_object->data);
            if ((D_001995F0->f0 >> 7) == 199) {
                switch ((D_001995F0->f0 & 31) - 2) {
                case 14:
                    object = rmb_make_marker(quarter, D_001995F0->f0);
                    object->trigger_range = D_001995F0->f5;
                    object->mobile_id = D_001995F0->f4;
                    object->link_flag = D_001995F0->f2;
                    object->wait_state = D_001995F0->f10;
                    break;
                case 13:
                    object = rmb_make_marker(quarter, D_001995F0->f0);
                    object->trigger_range = D_001995F0->f5;
                    object->mobile_id = D_001995F0->f2;
                    if (object->mobile_id == 0)
                        object->mobile_id = rand_range(1, 6);
                    object->wait_state = D_001995F0->f10;
                    break;
                case 8:
                    if (D_001995F0->f2 != 0)
                        D_001995F8 = D_001995F0->f2;
                    if (D_001995F0->f5 != 0)
                        D_001995FC = -(D_001995F0->f5 << 3);
                    D_001962A1 = D_001995F0->f4;
                    if ((D_00199608->flags & 4) == 0) {
                        object = 0;
                        break;
                    }
                default:
                    object = rmb_make_marker(quarter, D_001995F0->f0);
                    break;
                }
            } else if (D_001995F0->f10 == 29) {
                object = rmb_make_flat(quarter, D_001995F0->f0, D_001995F0->f4 + (D_001995F0->f5 << 8), 0);
                sub = (struct sub *)RECORD_DATA(object);
                if (D_001995F0->f2 & 16)
                    sub->f2 |= 32;
                if (D_001995F0->f2 & 32)
                    sub->f2 |= 16;
                sub->f0 = (D_001995F0->f5 << 8) + D_001995F0->f4;
            } else {
                object = object_create_in_block(quarter, 33, 0, D_001995F0->f0, block_index);
            }
            break;
        }
        if (object != 0) {
            D_001985D4[D_00199614].off = offset;
            D_001985D4[D_00199614].id = object->id;
            if (D_00199614++ > 512)
                fatal_error(D_00170AD2);
            object->x = location_object->x + rdb_object->x + (D_00199608->dx << 11);
            object->y = location_object->y + rdb_object->y;
            object->z = location_object->z + rdb_object->z + (D_00199608->dz << 11);
            if (rdb_object->kind == 3) {
                res = xn_tex_cache_lookup(object->image >> 7, object->image & 127, 0);
                if (res == 0) {
                    xn_tex_cache_flush();
                    res = xn_tex_cache_lookup(object->image >> 7, object->image & 127, 0);
                }
                height = res->anim->f6 * (res->anim->f24 + 256) / 256;
                object->y += height >> 1;
            }
        }
        rdb_object = (struct node *)(D_001995E8 + rdb_object->next);
    } while ((char *)rdb_object - D_001995E8 > 0);
}
