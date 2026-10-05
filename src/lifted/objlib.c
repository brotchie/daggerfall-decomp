/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int D_001343C0;
extern char D_00176C20[];
extern char D_00176C29[];
extern char D_00176C4F[];
extern char D_00176C6B[];
extern char D_00176C80[];
extern unsigned char player_environment;
extern short D_00187CB4[];
extern char D_00187D30[];
extern signed char D_00187DAC[];
extern char D_00187DC0[];
extern char D_00187DE8[];
extern char D_00187EC8[];
extern signed char text_buffer[];
extern char text_macro_fpc[];
extern char D_001910AC[];
extern char frame_counter[];
extern int D_001959BC;
extern struct building *current_building;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct settings *game_settings;
extern int D_00195C44;
extern int sound_last_size;
extern signed char current_region;
extern signed char current_climate;
extern int D_001967F0;
extern int D_001967F4;
extern int D_001967F8;
extern char D_001967FC[];
extern int D_00196800;
extern int D_00196808;
extern char *rmb_record_ptr;
extern signed char D_001968BB;
extern char *rmb_block;
extern struct model_node model_cache_nodes[];
extern char sound_cache[];
extern char D_001A8430[];
extern char D_001A8434[];
extern char D_001A8438[];
extern int model_heap_free;
extern struct model_node *model_cache_root;
extern int sound_cache_bytes;
extern int D_001A9438;
extern int D_001A9440;
extern int arch3d_bsa;
extern int dagger_snd;
extern char model_heap[];
extern signed char D_001A949C;
extern signed char D_001A949D;

extern int archive_find_record(int, int, int);
extern int archive_record_size(int, int);
extern int archive_read_record(int, int, int);
extern int func_0002455D(struct record *);
extern int flats_cfg_find(int);
extern int sound_play_at_point(int, int, int, int, int);
extern int mem_pool_alloc(int, int);
extern int mem_pool_release(int);
extern int rand_range(int, int);
extern int model_get(unsigned short, int, int);
extern int func_00086093(int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int dpmi_lock_region(int, int);
extern int dpmi_unlock_region(int, int);
extern int rand();
extern int srand();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000C5280();
extern int func_000C7FD9();
extern int func_000CE6E2();
extern int func_000CE808();
extern int func_00136AD8();
extern int func_0013FE15();
extern int func_00154D00();
extern void arch3d_apply_climate_textures(int);
extern void fatal_error(int);
extern void mem_pool_init(int, int);
extern void mem_pool_free(int);
extern void rotate_xz(int, int, int);
extern void rmb_add_doors(struct record *, int);
extern void rmb_add_people(struct record *, int);
extern void rmb_add_editor_marker(struct record *, int);
extern void object_foreach(struct record *, int);
struct record *rmb_add_subrecord(struct record *);
int model_load(int, int);
int model_cache_add(int);
int func_00086041(int, int);
void func_00084E5E(struct record *);
void model_cache_purge_old(struct model_node *);
void model_cache_flush(struct model_node *);
void model_cache_purge_unused(struct model_node *, int, struct model_node *);
void model_cache_remove_node(struct model_node *, struct model_node *);
void sound_cache_trim(void);
void func_00085EF8(struct record *);
void func_0008600F(int);
#pragma aux func_000A0ED9 parm routine [];

struct record *rmb_add_subrecord(struct record *a1)
{
    struct record *l_48;
    struct block *l_44;
    struct block_model *l_40;
    struct block_flat *l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_1C = 17;
    l_1C += ((int)(unsigned char)*(signed char *)(rmb_record_ptr)) * 66;
    l_1C += ((int)(unsigned char)*(signed char *)(rmb_record_ptr + 1)) * 17;
    l_1C += ((int)(unsigned char)*(signed char *)(rmb_record_ptr + 2)) << 4;
    if (l_1C == 17) return 0;
    l_48 = object_create_child(a1, 0, l_1C);
    l_48->type = 43;
    l_48->x = D_001967F8;
    l_48->z = D_001967F0;
    l_48->y = D_001967F4;
    l_48->yaw = *(short *)D_001967FC;
    l_48->pad13 = 32768;
    l_48->id = D_00195AC4->id + ((int)(unsigned short)(current_location->object_counter)++);
    D_001A9438 = l_48->id;
    l_44 = &l_48->data.block;
    mc_memcpy((int)l_44, (int)rmb_record_ptr, l_1C, (int)D_00176C20, 803, 4);
    l_44->models = (struct block_model *)((int)l_44 + 17);
    l_40 = l_44->models;
    l_44->flats = (struct block_flat *)((int)l_40 + (l_44->model_count * 66));
    l_3C = l_44->flats;
    l_44->section3 = (char *)((int)l_3C + (l_44->flat_count * 17));
    l_34 = (int)l_44->section3;
    for (l_28 = 0; l_44->model_count > l_28; l_28++, l_40++) {
        rotate_xz((int)&l_40->x, (int)&l_40->z, *(int *)D_001967FC);
        l_40->x += D_001967F8;
        l_40->z += D_001967F0;
        l_40->model = 0;
        if (l_40->y > 0 && l_40->id > 10) {
            l_40->model = (char *)model_get(l_40->id, l_40->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
            l_40->y = (-l_40->y) - (func_000CE808((int)l_40->model) >> 8);
        }
        l_40->y += D_001967F4;
        l_40->yaw += *(int *)D_001967FC;
    }
    for (l_28 = 0; l_44->flat_count > l_28; l_28++, l_3C++) {
        rotate_xz((int)&l_3C->x, (int)&l_3C->z, *(int *)D_001967FC);
        l_3C->x += D_001967F8;
        l_3C->z += D_001967F0;
        l_3C->y += D_001967F4;
        if ((l_3C->image >> 7) == 199) {
            rmb_add_editor_marker(a1, (int)l_3C);
        } else {
            l_2C = flats_cfg_find(l_3C->image);
            if (((int)(unsigned char)(*(signed char *)((char *)l_2C + 6) & 2)) != 0 && ((int)(unsigned short)(game_settings->view_flags & 4)) != 0) {
                l_3C->image = 0;
            }
        }
    }
    for (l_28 = 0; l_44->section3_count > l_28; l_28++, (*(char (**)[16])&l_34)++) {
        rotate_xz(l_34, l_34 + 8, *(int *)D_001967FC);
        *(int *)((char *)l_34) += D_001967F8;
        *(int *)((char *)l_34 + 8) += D_001967F0;
        *(int *)((char *)l_34 + 4) += D_001967F4;
    }
    l_38 = (int)(rmb_record_ptr + l_1C);
    l_30 = l_38 + (((int)(unsigned char)*(signed char *)(rmb_record_ptr + 3)) * 17);
    rmb_add_people(a1, l_38);
    rmb_add_doors(a1, l_30);
    *(int *)&rmb_record_ptr = l_30 + (((int)(unsigned char)*(signed char *)(rmb_record_ptr + 4)) * 19);
    return l_48;
}

void func_00084E5E(struct record *a1)
{
    int l_20;
    struct building *l_1C;
    int l_18;

    l_1C = &current_location->buildings[a1->image];
    if (l_1C->type != 16) {
        if (l_1C->faction_id == 0) {
            l_1C->faction_id = *(short *)(D_00187D30 + (((int)(unsigned char)current_region) * 2));
        }
        return;
    }
    l_1C->faction_id = ((((int)(unsigned char)(D_001968BB & 16)) != 0) ? 852 : 242);
    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 8) {
            l_20 = (int)RECORD_DATA(a1);
            if (*(unsigned short *)((char *)l_20) == *(short *)(D_00187D30 + (((int)(unsigned char)current_region) * 2))) {
                *(short *)((char *)l_20) = D_00187CB4[((int)(unsigned char)current_region)];
            }
            if (((int)(unsigned short)*(short *)((char *)l_20)) == 852 && ((int)(unsigned char)(D_001968BB & 16)) == 0) {
                *(short *)((char *)l_20) = 242;
            }
        }
        a1 = a1->next;
    }
}

struct record *rmb_add_building(struct record *a1, int a2)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    *(int *)&rmb_record_ptr = *(int *)(rmb_block + 1475 + (a2 << 2));
    l_18 = rmb_add_subrecord(a1);
    l_18->flags = 1;
    l_18->image2 = a2;
    l_20 = (int)(unsigned char)*(signed char *)(rmb_block + 667 + (a2 * 26));
    switch ((unsigned)l_20) {
        break;
    case 21:
    case 22:
        l_18->flags |= 8;
        l_18->image = 65535;
        return l_18;
    case 17:
    case 18:
    case 19:
    case 20:
        l_1C = rand();
        srand((int)(short)(short)l_18->id);
        l_20 = rand();
        srand(l_1C);
        if (((int)(unsigned short)*(short *)(rmb_block + 661 + (a2 * 26))) != 42 && ((int)(unsigned short)*(short *)(rmb_block + 661 + (a2 * 26))) != 108) {
            if ((l_20 % 100) <= D_00196800) {
                l_18->flags |= 8;
                l_18->image = 65535;
                return l_18;
            }
        }
    }
    l_18->image = (D_00196808)++;
    rmb_add_subrecord(l_18);
    func_00084E5E(l_18);
    return l_18;
}

void model_heap_init(int a1)
{
    mem_pool_init((int)model_heap, a1);
    model_heap_free = a1;
    (model_cache_root = model_cache_nodes)->key = 50000;
    model_cache_root->last_frame = 0;
}

void model_heap_free_all(void)
{
    mem_pool_free((int)model_heap);
}

int model_cache_find(int a1)
{
    struct model_node *l_20;
    struct model_node *l_1C;

    l_20 = model_cache_root;
    while (1) {
        if (a1 == l_20->key) {
            l_20->last_frame = *(int *)frame_counter;
            return (int)l_20->model;
        }
        if (((unsigned)a1) < l_20->key) {
            if (l_20->left != 0) {
                l_20 = l_20->left;
            } else {
                l_20->left = (struct model_node *)model_cache_add(a1);
                l_1C = l_20->left;
                break;
            }
        } else if (l_20->right != 0) {
            l_20 = l_20->right;
        } else {
            l_20->right = (struct model_node *)model_cache_add(a1);
            l_1C = l_20->right;
            break;
        }
    }
    if (l_1C == 0) return 0;
    l_1C->last_frame = *(int *)frame_counter;
    return (int)l_1C->model;
}

int model_load(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = a2 & 131071;
    l_20 = archive_find_record(arch3d_bsa, (int)text_buffer, l_18);
    l_1C = archive_record_size(arch3d_bsa, l_20);
    if ((model_cache_nodes[a1].model = (char *)mem_pool_alloc((int)model_heap, l_1C)) == 0) {
        if (D_001A949D == 0) {
            model_cache_purge_old(model_cache_root);
            if ((model_cache_nodes[a1].model = (char *)mem_pool_alloc((int)model_heap, l_1C)) == 0) {
                model_cache_flush(model_cache_root);
                return 0;
            }
        } else {
            model_cache_purge_old(model_cache_root);
            if ((model_cache_nodes[a1].model = (char *)mem_pool_alloc((int)model_heap, l_1C)) == 0) {
                fatal_error((int)D_00176C29);
            }
        }
    }
    if (archive_read_record(arch3d_bsa, l_20, (int)model_cache_nodes[a1].model) == 0) {
        func_000A0ED9(1104, (int)D_00176C20);
        mc_sprintf(D_00195C44, (int)D_00176C4F, l_20);
        fatal_error(D_00195C44);
    }
    model_heap_free -= (l_1C + 1) & -2;
    return (int)model_cache_nodes[a1].model;
}

int model_cache_add(int a1)
{
    int l_20;
    int l_1C;
    {
        int l_30;
        int l_2C;
        int l_28;

        l_20 = 0;
        for (;;) {
            if (model_cache_nodes[l_20].key != 0 || (int)model_cache_nodes[l_20].left != 0 || (int)model_cache_nodes[l_20].right != 0) {
                l_28 = 1;
            } else {
                l_28 = 0;
            }
            if (l_28 == 0 || l_20 >= 512) break;
            l_20++;
        }
        if (l_20 == 512) {
            model_cache_purge_old(model_cache_root);
            l_20 = 0;
            for (;;) {
                if (model_cache_nodes[l_20].key != 0 || (int)model_cache_nodes[l_20].left != 0 || (int)model_cache_nodes[l_20].right != 0) {
                    l_2C = 1;
                } else {
                    l_2C = 0;
                }
                if (l_2C == 0 || l_20 >= 512) break;
                l_20++;
            }
            if (l_20 == 512) {
                model_cache_flush(model_cache_root);
                l_20 = 0;
                for (;;) {
                    if (model_cache_nodes[l_20].key != 0 || (int)model_cache_nodes[l_20].left != 0 || (int)model_cache_nodes[l_20].right != 0) {
                        l_30 = 1;
                    } else {
                        l_30 = 0;
                    }
                    if (l_30 == 0 || l_20 >= 512) break;
                    l_20++;
                }
                if (l_20 == 512) fatal_error((int)D_00176C6B);
            }
        }
        model_cache_nodes[l_20].key = a1;
        model_cache_nodes[l_20].last_frame = *(int *)frame_counter;
        model_cache_nodes[l_20].left = (model_cache_nodes[l_20].right = 0);
        if (model_load(l_20, a1) == 0) return 0;
        func_0013FE15((int)model_cache_nodes[l_20].model);
        l_1C = rand();
        srand(*(int *)(model_cache_nodes[l_20].model + 12));
        arch3d_apply_climate_textures((int)model_cache_nodes[l_20].model);
        srand(l_1C);
        return (int)&model_cache_nodes[l_20];
    }
}

void model_cache_purge_old(struct model_node *a1)
{
    int l_18;

    l_18 = 200;
    if (model_heap_free > 204800) return;
    while (l_18 > 2 && model_heap_free < 204800) {
        model_cache_purge_unused(0, l_18, a1);
        l_18 >>= 1;
    }
}

void model_cache_flush(struct model_node *a1)
{
    int l_18;

    (D_001A949D)++;
    for (l_18 = 1; l_18 < 512; l_18++) {
        if (model_cache_nodes[l_18].key != 0 && (int)model_cache_nodes[l_18].model != 0) {
            func_0008600F((int)model_cache_nodes[l_18].model);
            model_heap_free += mem_pool_release((int)model_cache_nodes[l_18].model);
        }
    }
    a1->left = 0;
    a1->right = a1->left;
    mc_memset((int)&model_cache_nodes[1], 0, 10220, (int)D_00176C20, 1189, 4);
}

void model_cache_purge_unused(struct model_node *a1, int a2, struct model_node *a3)
{
    int l_10;

    if (a3->left != 0) model_cache_purge_unused(a3, a2, a3->left);
    if (a3->right != 0) model_cache_purge_unused(a3, a2, a3->right);
    if (((unsigned)(*(int *)frame_counter - a3->last_frame)) >= a2) model_cache_remove_node(a1, a3);
    if (a1->left != a1->right || a1->left == 0) return;
    fatal_error((int)D_00176C80);
}

void model_cache_remove_node(struct model_node *a1, struct model_node *a2)
{
    int l_1C;
    struct model_node *l_18;
    struct model_node *l_14;

    if (a1 == 0) return;
    if (a2->left == 0) {
        if (a2->right == 0) {
            if (a1->right == a2) {
                a1->right = 0;
            } else {
                a1->left = 0;
            }
        } else if (a1->right == a2) {
            a1->right = a2->right;
        } else {
            a1->left = a2->right;
        }
        func_0008600F((int)a2->model);
        model_heap_free += mem_pool_release((int)a2->model);
        a2->model = 0;
        a2->key = 0;
        return;
    }
    if (a2->right == 0) {
        if (a1->right == a2) {
            a1->right = a2->left;
        } else {
            a1->left = a2->left;
        }
        func_0008600F((int)a2->model);
        model_heap_free += mem_pool_release((int)a2->model);
        a2->model = 0;
        a2->key = 0;
        return;
    }
    l_14 = a2;
    l_18 = a2->right;
    while (l_18->left != 0) {
        l_14 = l_18;
        l_18 = l_18->left;
    }
    l_1C = (int)a2->model;
    a2->model = (char *)((int)l_18->model);
    a2->last_frame = l_18->last_frame;
    a2->key = l_18->key;
    l_18->model = (char *)l_1C;
    model_cache_remove_node(l_14, l_18);
}

void func_0008591A(struct record *a1, struct building *a2)
{
    int l_18;
    int l_14;

    l_14 = rand();
    srand(a1->id & 65535);
    l_18 = func_0002455D(a1);
    if (l_18 != 100000) a1->y = l_18;
    a1->image = func_00086041((int)player_environment, current_building->type);
    srand(l_14);
}

void func_00085992(struct record *a1, struct building *a2)
{
    int l_14;

    l_14 = func_0002455D(a1);
    if (l_14 != 100000) a1->y = l_14;
    if (((int)player_environment) != 3) {
        a1->type = 33;
        a1->image = *(short *)(D_00187DC0 + (a2->type * 2));
    } else {
        a1->type = 33;
        a1->image = ((unsigned short)(unsigned char)D_00187DAC[rand() % 20]) + 27648;
        a1->pad19 = 1;
    }
    if (a1->image != 0) if (a1->image != 65535) return;
    l_14++;
}

int sound_cache_load(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    D_001A9440 = a1;
    for (l_24 = 0; l_24 < 256; l_24++) {
        if (*(int *)(D_001A8430 + (l_24 << 4)) == a1) {
            sound_last_size = *(int *)(D_001A8434 + (l_24 << 4));
            return *(int *)(D_001A8438 + (l_24 << 4));
        }
    }
    l_24 = 0;
    while (*(int *)(D_001A8438 + (l_24 << 4)) != 0) l_24++;
    l_1C = archive_find_record(dagger_snd, (int)D_001910AC, a1);
    l_20 = archive_record_size(dagger_snd, l_1C);
    *(int *)(sound_cache + (l_24 << 4)) = *(int *)frame_counter;
    *(int *)(D_001A8430 + (l_24 << 4)) = a1;
    *(int *)(D_001A8434 + (l_24 << 4)) = l_20;
    *(int *)(D_001A8438 + (l_24 << 4)) = mc_malloc(l_20, (int)D_00176C20, 1338);
    dpmi_lock_region(*(int *)(D_001A8438 + (l_24 << 4)), l_20 + 4096);
    archive_read_record(dagger_snd, l_1C, *(int *)(D_001A8438 + (l_24 << 4)));
    sound_cache_bytes += l_20;
    sound_cache_trim();
    sound_last_size = l_20;
    return *(int *)(D_001A8438 + (l_24 << 4));
}

void sound_cache_trim(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (sound_cache_bytes < 393216) return;
    while (sound_cache_bytes > 262144) {
        l_1C = -1;
        l_20 = *(int *)frame_counter;
        for (l_18 = 0; l_18 < 256; l_18++) {
            if (*(int *)(D_001A8438 + (l_18 << 4)) == 0) continue;
            if (l_20 > *(int *)(sound_cache + (l_18 << 4))) {
                l_20 = *(int *)(sound_cache + (l_18 << 4));
                l_1C = l_18;
            }
        }
        if (l_1C == (-1)) return;
        dpmi_unlock_region(*(int *)(D_001A8438 + (l_1C << 4)), *(int *)(D_001A8434 + (l_1C << 4)) + 4096);
        if (*(int *)(D_001A8438 + (l_1C << 4)) != 0 && *(int *)(D_001A8438 + (l_1C << 4)) != (-1751672937)) {
            mc_free(*(int *)(D_001A8438 + (l_1C << 4)), (int)D_00176C20, 1375);
            *(int *)(D_001A8438 + (l_1C << 4)) = -1751672937;
        }
        *(int *)(D_001A8438 + (l_1C << 4)) = 0;
        *(int *)(D_001A8430 + (l_1C << 4)) = -1;
        sound_cache_bytes -= *(int *)(D_001A8434 + (l_1C << 4));
    }
}

void sound_cache_free_all(void)
{
    int l_18;

    for (l_18 = 0; l_18 < 256; l_18++) {
        if (*(int *)(D_001A8438 + (l_18 << 4)) != 0) {
            dpmi_unlock_region(*(int *)(D_001A8438 + (l_18 << 4)), *(int *)(D_001A8434 + (l_18 << 4)) + 1024);
            if (*(int *)(D_001A8438 + (l_18 << 4)) != 0 && *(int *)(D_001A8438 + (l_18 << 4)) != (-1751672937)) {
                mc_free(*(int *)(D_001A8438 + (l_18 << 4)), (int)D_00176C20, 1392);
                *(int *)(D_001A8438 + (l_18 << 4)) = -1751672937;
            }
            *(int *)(D_001A8438 + (l_18 << 4)) = 0;
            *(int *)(D_001A8430 + (l_18 << 4)) = -1;
        }
    }
    sound_cache_bytes = 0;
}

void flat_animal_sound(int a1, int a2, int a3, int a4, int a5)
{
    if (a4 != 201) return;
    if (rand() > 100) return;
    if (func_000C7FD9(a1, a3, player_object->x, player_object->z) > 768) return;
    switch ((unsigned)a5) {
        return;
    case 0:
    case 1:
        sound_play_at_point(367, a1, a2, a3, 100);
        return;
    case 3:
    case 4:
        sound_play_at_point(371, a1, a2, a3, 100);
        return;
    case 5:
    case 6:
        sound_play_at_point(370, a1, a2, a3, 100);
        return;
    case 7:
    case 8:
        sound_play_at_point(369, a1, a2, a3, 100);
        return;
    case 9:
    case 10:
        sound_play_at_point(368, a1, a2, a3, 100);
    default:;
    }
}

void func_00085EF8(struct record *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    switch (a1->type) {
    case 6:
    case 32:
        if (*(int *)((char *)a1 + 71) == *(int *)text_macro_fpc) *(int *)((char *)a1 + 71) = 0;
        return;
    case 43:
        l_20 = (int)RECORD_DATA(a1);
        l_1C = *(int *)((char *)l_20 + 5);
        for (l_18 = 0; ((int)(unsigned char)*(signed char *)((char *)l_20)) > l_18; l_18++, (*(char (**)[66])&l_1C)++) {
            if (*(int *)((char *)l_1C + 4) == *(int *)text_macro_fpc) {
                *(int *)((char *)l_1C + 4) = 0;
            }
        }
        return;
    case 56:
        l_1C = (int)RECORD_DATA(a1);
        for (l_18 = 0; a1->image > l_18; l_18++, (*(char (**)[66])&l_1C)++) {
            if (*(int *)((char *)l_1C + 4) == *(int *)text_macro_fpc) {
                *(int *)((char *)l_1C + 4) = 0;
            }
        }
    default:;
    }
}

void func_0008600F(int a1)
{
    *(int *)text_macro_fpc = a1;
    object_foreach(D_00195AC4, (int)func_00085EF8);
}

int func_00086041(int a1, int a2)
{
    if (a1 == 3) return func_00086093((int)D_00187EC8);
    return func_00086093(((int)D_00187DE8) + (rand_range(0, 7) * 28));
}

void func_00086149(void)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = func_000C5280(player_object->x ^ player_object->z, (D_001343C0 / 40) << 6);
    l_2C >>= 3;
    l_2C = 256 - l_2C;
    l_30 = (l_2C * 192) >> 8;
    l_20 = func_000C5280(player_object->x ^ player_object->z, (D_001343C0 / 40) << 6);
    l_20 >>= 3;
    l_1C = func_000C5280(player_object->x + player_object->z, (D_001343C0 / 40) << 6);
    l_1C >>= 3;
    l_18 = func_000C5280(player_object->x - player_object->z, (D_001343C0 / 40) << 6);
    l_18 >>= 3;
    func_000CE6E2((player_object->yaw + D_001959BC) & 2047, 192, (int)&l_28, (int)&l_24);
    func_00136AD8((player_object->x + l_28) + (l_20 - 16), (player_object->y - 50) + (l_1C - 16), (player_object->z + l_24) + (l_18 - 16), 50, l_30, 0);
    func_00154D00((player_object->x + l_28) + (l_20 - 16), (player_object->y - 50) + (l_1C - 16), (player_object->z + l_24) + (l_18 - 16), 26883, -1, 1, 400);
}
