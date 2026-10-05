/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_001343C0[];
extern char D_00176C20[];
extern char D_00176C29[];
extern char D_00176C4F[];
extern char D_00176C6B[];
extern char D_00176C80[];
extern char player_environment[];
extern char D_00187CB4[];
extern char D_00187D30[];
extern char D_00187DAC[];
extern char D_00187DC0[];
extern char D_00187DE8[];
extern char D_00187EC8[];
extern char text_buffer[];
extern char text_macro_fpc[];
extern char D_001910AC[];
extern char frame_counter[];
extern char D_001959BC[];
extern struct building *current_building;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct settings *game_settings;
extern char D_00195C44[];
extern char sound_last_size[];
extern char current_region[];
extern char current_climate[];
extern char D_001967F0[];
extern char D_001967F4[];
extern char D_001967F8[];
extern char D_001967FC[];
extern char D_00196800[];
extern char D_00196808[];
extern char rmb_record_ptr[];
extern char D_001968BB[];
extern char rmb_block[];
extern struct model_node model_cache_nodes[];
extern char sound_cache[];
extern char D_001A8430[];
extern char D_001A8434[];
extern char D_001A8438[];
extern char model_heap_free[];
extern struct model_node *model_cache_root;
extern char sound_cache_bytes[];
extern char D_001A9438[];
extern char D_001A9440[];
extern char arch3d_bsa[];
extern char dagger_snd[];
extern char model_heap[];
extern char D_001A949C[];
extern char D_001A949D[];

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
    l_1C += ((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr)) * 66;
    l_1C += ((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr + 1)) * 17;
    l_1C += ((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr + 2)) << 4;
    if (l_1C != 17) goto L84ADE;
    return 0;
L84ADE:;
    l_48 = object_create_child(a1, 0, l_1C);
    l_48->type = 43;
    l_48->x = *(int *)D_001967F8;
    l_48->z = *(int *)D_001967F0;
    l_48->y = *(int *)D_001967F4;
    l_48->yaw = *(short *)D_001967FC;
    l_48->pad13 = 32768;
    l_48->id = D_00195AC4->id + ((int)(unsigned short)(current_location->object_counter)++);
    *(int *)D_001A9438 = l_48->id;
    l_44 = &l_48->data.block;
    mc_memcpy((int)l_44, *(int *)rmb_record_ptr, l_1C, (int)D_00176C20, 803, 4);
    l_44->models = (struct block_model *)((int)l_44 + 17);
    l_40 = l_44->models;
    l_44->flats = (struct block_flat *)((int)l_40 + (l_44->model_count * 66));
    l_3C = l_44->flats;
    l_44->section3 = (char *)((int)l_3C + (l_44->flat_count * 17));
    l_34 = (int)l_44->section3;
    l_28 = 0;
L84BDC:;
    if (l_44->model_count > l_28) goto L84BFF;
    goto L84CC4;
L84BF0:;
    l_28++;
    l_40++;
    goto L84BDC;
L84BFF:;
    rotate_xz((int)&l_40->x, (int)&l_40->z, *(int *)D_001967FC);
    l_40->x += *(int *)D_001967F8;
    l_40->z += *(int *)D_001967F0;
    l_40->model = 0;
    if (l_40->y <= 0) goto L84C51;
    if (l_40->id > 10) goto L84C53;
L84C51:;
    goto L84CA7;
L84C53:;
    l_40->model = (char *)model_get(l_40->id, l_40->variant, (((int)(unsigned char)*(signed char *)current_climate) << 2) + ((int)(unsigned char)*(signed char *)D_001A949C));
    l_40->y = (-l_40->y) - (func_000CE808((int)l_40->model) >> 8);
L84CA7:;
    l_40->y += *(int *)D_001967F4;
    l_40->yaw += *(int *)D_001967FC;
    goto L84BF0;
L84CC4:;
    l_28 = 0;
L84CCB:;
    if (l_44->flat_count > l_28) goto L84CEF;
    goto L84D94;
L84CE0:;
    l_28++;
    l_3C++;
    goto L84CCB;
L84CEF:;
    rotate_xz((int)&l_3C->x, (int)&l_3C->z, *(int *)D_001967FC);
    l_3C->x += *(int *)D_001967F8;
    l_3C->z += *(int *)D_001967F0;
    l_3C->y += *(int *)D_001967F4;
    if ((l_3C->image >> 7) != 199) goto L84D49;
    rmb_add_editor_marker(a1, (int)l_3C);
    goto L84D8F;
L84D49:;
    l_2C = flats_cfg_find(l_3C->image);
    if (((int)(unsigned char)(*(signed char *)((char *)l_2C + 6) & 2)) == 0) goto L84D84;
    if (((int)(unsigned short)(game_settings->view_flags & 4)) != 0) goto L84D86;
L84D84:;
    goto L84D8F;
L84D86:;
    l_3C->image = 0;
L84D8F:;
    goto L84CE0;
L84D94:;
    l_28 = 0;
L84D9B:;
    if (l_44->section3_count > l_28) goto L84DBC;
    goto L84DF5;
L84DAD:;
    l_28++;
    (*(char (**)[16])&l_34)++;
    goto L84D9B;
L84DBC:;
    rotate_xz(l_34, l_34 + 8, *(int *)D_001967FC);
    *(int *)((char *)l_34) += *(int *)D_001967F8;
    *(int *)((char *)l_34 + 8) += *(int *)D_001967F0;
    *(int *)((char *)l_34 + 4) += *(int *)D_001967F4;
    goto L84DAD;
L84DF5:;
    l_38 = (int)(*(char **)rmb_record_ptr + l_1C);
    l_30 = l_38 + (((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr + 3)) * 17);
    rmb_add_people(a1, l_38);
    rmb_add_doors(a1, l_30);
    *(int *)rmb_record_ptr = l_30 + (((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr + 4)) * 19);
    return l_48;
}

void func_00084E5E(struct record *a1)
{
    int l_20;
    struct building *l_1C;
    int l_18;

    l_1C = &current_location->buildings[a1->image];
    if (l_1C->type == 16) goto L84EC1;
    if (l_1C->faction_id != 0) goto L84EBC;
    l_1C->faction_id = *(short *)(D_00187D30 + (((int)(unsigned char)*(signed char *)current_region) * 2));
L84EBC:;
    return;
L84EC1:;
    l_1C->faction_id = ((((int)(unsigned char)(*(signed char *)D_001968BB & 16)) != 0) ? 852 : 242);
    a1 = a1->children;
L84EF4:;
    if (a1 == 0) return;
    if (a1->type != 8) goto L84F77;
    l_20 = (int)RECORD_DATA(a1);
    if (*(unsigned short *)((char *)l_20) != *(short *)(D_00187D30 + (((int)(unsigned char)*(signed char *)current_region) * 2))) goto L84F4B;
    *(short *)((char *)l_20) = *(short *)(D_00187CB4 + (((int)(unsigned char)*(signed char *)current_region) * 2));
L84F4B:;
    if (((int)(unsigned short)*(short *)((char *)l_20)) != 852) goto L84F6D;
    if (((int)(unsigned char)(*(signed char *)D_001968BB & 16)) == 0) goto L84F6F;
L84F6D:;
    goto L84F77;
L84F6F:;
    *(short *)((char *)l_20) = 242;
L84F77:;
    a1 = a1->next;
    goto L84EF4;
}

struct record *rmb_add_building(struct record *a1, int a2)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    *(int *)rmb_record_ptr = *(int *)(*(char **)rmb_block + 1475 + (a2 << 2));
    l_18 = rmb_add_subrecord(a1);
    l_18->flags = 1;
    l_18->image2 = a2;
    l_20 = (int)(unsigned char)*(signed char *)(*(char **)rmb_block + 667 + (a2 * 26));
    switch ((unsigned)l_20) {
    goto L850BA;
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
    if (((int)(unsigned short)*(short *)(*(char **)rmb_block + 661 + (a2 * 26))) == 42) goto L85088;
    if (((int)(unsigned short)*(short *)(*(char **)rmb_block + 661 + (a2 * 26))) != 108) goto L8508A;
L85088:;
    goto L850BA;
L8508A:;
    if ((l_20 % 100) > *(int *)D_00196800) goto L850BA;
    l_18->flags |= 8;
    l_18->image = 65535;
    return l_18;
default:
L850BA:;
    l_18->image = (*(int *)D_00196808)++;
    rmb_add_subrecord(l_18);
    func_00084E5E(l_18);
    return l_18;
}
}

void model_heap_init(int a1)
{
    mem_pool_init((int)model_heap, a1);
    *(int *)model_heap_free = a1;
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
L8517C:;
    if (a1 != l_20->key) goto L851A1;
    l_20->last_frame = *(int *)frame_counter;
    return (int)l_20->model;
L851A1:;
    if (((unsigned)a1) >= l_20->key) goto L851D9;
    if (l_20->left == 0) goto L851BE;
    l_20 = l_20->left;
    goto L851D7;
L851BE:;
    l_20->left = (struct model_node *)model_cache_add(a1);
    l_1C = l_20->left;
    goto L8520D;
L851D7:;
    goto L85208;
L851D9:;
    if (l_20->right == 0) goto L851ED;
    l_20 = l_20->right;
    goto L85208;
L851ED:;
    l_20->right = (struct model_node *)model_cache_add(a1);
    l_1C = l_20->right;
    goto L8520D;
L85208:;
    goto L8517C;
L8520D:;
    if (l_1C != 0) goto L8521C;
    return 0;
L8521C:;
    l_1C->last_frame = *(int *)frame_counter;
    return (int)l_1C->model;
}

int model_load(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = a2 & 131071;
    l_20 = archive_find_record(*(int *)arch3d_bsa, (int)text_buffer, l_18);
    l_1C = archive_record_size(*(int *)arch3d_bsa, l_20);
    if ((model_cache_nodes[a1].model = (char *)mem_pool_alloc((int)model_heap, l_1C)) != 0) goto L85395;
    if (*(signed char *)D_001A949D != 0) goto L8535E;
    model_cache_purge_old(model_cache_root);
    if ((model_cache_nodes[a1].model = (char *)mem_pool_alloc((int)model_heap, l_1C)) != 0) goto L8535C;
    model_cache_flush(model_cache_root);
    return 0;
L8535C:;
    goto L85395;
L8535E:;
    model_cache_purge_old(model_cache_root);
    if ((model_cache_nodes[a1].model = (char *)mem_pool_alloc((int)model_heap, l_1C)) != 0) goto L85395;
    fatal_error((int)D_00176C29);
L85395:;
    if (archive_read_record(*(int *)arch3d_bsa, l_20, (int)model_cache_nodes[a1].model) != 0) goto L853E2;
    func_000A0ED9(1104, (int)D_00176C20);
    mc_sprintf(*(int *)D_00195C44, (int)D_00176C4F, l_20);
    fatal_error(*(int *)D_00195C44);
L853E2:;
    *(int *)model_heap_free -= (l_1C + 1) & -2;
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
L8541F:;
    if (model_cache_nodes[l_20].key != 0) goto L85439;
    if ((int)model_cache_nodes[l_20].left == 0) goto L8543B;
L85439:;
    goto L85448;
L8543B:;
    if ((int)model_cache_nodes[l_20].right == 0) goto L85451;
L85448:;
    l_28 = 1;
    goto L85458;
L85451:;
    l_28 = 0;
L85458:;
    if (l_28 == 0) goto L85467;
    if (l_20 < 512) goto L85469;
L85467:;
    goto L85471;
L85469:;
    l_20++;
    goto L8541F;
L85471:;
    if (l_20 != 512) goto L85564;
    model_cache_purge_old(model_cache_root);
    l_20 = 0;
L8548F:;
    if (model_cache_nodes[l_20].key != 0) goto L854A9;
    if ((int)model_cache_nodes[l_20].left == 0) goto L854AB;
L854A9:;
    goto L854B8;
L854AB:;
    if ((int)model_cache_nodes[l_20].right == 0) goto L854C1;
L854B8:;
    l_2C = 1;
    goto L854C8;
L854C1:;
    l_2C = 0;
L854C8:;
    if (l_2C == 0) goto L854D7;
    if (l_20 < 512) goto L854D9;
L854D7:;
    goto L854E1;
L854D9:;
    l_20++;
    goto L8548F;
L854E1:;
    if (l_20 != 512) goto L85564;
    model_cache_flush(model_cache_root);
    l_20 = 0;
L854FF:;
    if (model_cache_nodes[l_20].key != 0) goto L85519;
    if ((int)model_cache_nodes[l_20].left == 0) goto L8551B;
L85519:;
    goto L85528;
L8551B:;
    if ((int)model_cache_nodes[l_20].right == 0) goto L85531;
L85528:;
    l_30 = 1;
    goto L85538;
L85531:;
    l_30 = 0;
L85538:;
    if (l_30 == 0) goto L85547;
    if (l_20 < 512) goto L85549;
L85547:;
    goto L85551;
L85549:;
    l_20++;
    goto L854FF;
L85551:;
    if (l_20 != 512) goto L85564;
    fatal_error((int)D_00176C6B);
L85564:;
    model_cache_nodes[l_20].key = a1;
    model_cache_nodes[l_20].last_frame = *(int *)frame_counter;
    model_cache_nodes[l_20].left = (model_cache_nodes[l_20].right = 0);
    if (model_load(l_20, a1) != 0) goto L855B7;
    return 0;
L855B7:;
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
    if (*(int *)model_heap_free > 204800) return;
L85636:;
    if (l_18 <= 2) goto L85648;
    if (*(int *)model_heap_free < 204800) goto L8564A;
L85648:;
    return;
L8564A:;
    model_cache_purge_unused(0, l_18, a1);
    l_18 >>= 1;
    goto L85636;
}

void model_cache_flush(struct model_node *a1)
{
    int l_18;

    (*(signed char *)D_001A949D)++;
    l_18 = 1;
L85684:;
    if (l_18 < 512) goto L85697;
    goto L856D9;
L8568F:;
    l_18++;
    goto L85684;
L85697:;
    if (model_cache_nodes[l_18].key == 0) goto L856B1;
    if ((int)model_cache_nodes[l_18].model != 0) goto L856B3;
L856B1:;
    goto L856D7;
L856B3:;
    func_0008600F((int)model_cache_nodes[l_18].model);
    *(int *)model_heap_free += mem_pool_release((int)model_cache_nodes[l_18].model);
L856D7:;
    goto L8568F;
L856D9:;
    a1->left = 0;
    a1->right = a1->left;
    mc_memset((int)&model_cache_nodes[1], 0, 10220, (int)D_00176C20, 1189, 4);
}

void model_cache_purge_unused(struct model_node *a1, int a2, struct model_node *a3)
{
    int l_10;

    if (a3->left == 0) goto L85741;
    model_cache_purge_unused(a3, a2, a3->left);
L85741:;
    if (a3->right == 0) goto L8575B;
    model_cache_purge_unused(a3, a2, a3->right);
L8575B:;
    if (((unsigned)(*(int *)frame_counter - a3->last_frame)) < a2) goto L85776;
    model_cache_remove_node(a1, a3);
L85776:;
    if (a1->left != a1->right) goto L8578B;
    if (a1->left != 0) goto L8578D;
L8578B:;
    return;
L8578D:;
    fatal_error((int)D_00176C80);
}

void model_cache_remove_node(struct model_node *a1, struct model_node *a2)
{
    int l_1C;
    struct model_node *l_18;
    struct model_node *l_14;

    if (a1 == 0) return;
    if (a2->left != 0) goto L8584C;
    if (a2->right != 0) goto L857F3;
    if (a1->right != a2) goto L857E8;
    a1->right = 0;
    goto L857F1;
L857E8:;
    a1->left = 0;
L857F1:;
    goto L85817;
L857F3:;
    if (a1->right != a2) goto L8580C;
    a1->right = a2->right;
    goto L85817;
L8580C:;
    a1->left = a2->right;
L85817:;
    func_0008600F((int)a2->model);
    *(int *)model_heap_free += mem_pool_release((int)a2->model);
    a2->model = 0;
    a2->key = 0;
    return;
L8584C:;
    if (a2->right != 0) goto L858A9;
    if (a1->right != a2) goto L8586D;
    a1->right = a2->left;
    goto L85877;
L8586D:;
    a1->left = a2->left;
L85877:;
    func_0008600F((int)a2->model);
    *(int *)model_heap_free += mem_pool_release((int)a2->model);
    a2->model = 0;
    a2->key = 0;
    return;
L858A9:;
    l_14 = a2;
    l_18 = a2->right;
L858B8:;
    if (l_18->left == 0) goto L858D0;
    l_14 = l_18;
    l_18 = l_18->left;
    goto L858B8;
L858D0:;
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
    if (l_18 == 100000) goto L85962;
    a1->y = l_18;
L85962:;
    a1->image = func_00086041((int)(unsigned char)*(signed char *)player_environment, current_building->type);
    srand(l_14);
}

void func_00085992(struct record *a1, struct building *a2)
{
    int l_14;

    l_14 = func_0002455D(a1);
    if (l_14 == 100000) goto L859C2;
    a1->y = l_14;
L859C2:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 3) goto L859F1;
    a1->type = 33;
    a1->image = *(short *)(D_00187DC0 + (a2->type * 2));
    goto L85A25;
L859F1:;
    a1->type = 33;
    a1->image = ((unsigned short)(unsigned char)*(signed char *)(D_00187DAC + (rand() % 20))) + 27648;
    a1->pad19 = 1;
L85A25:;
    if (a1->image == 0) goto L85A42;
    if (a1->image != 65535) return;
L85A42:;
    l_14++;
}

int sound_cache_load(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    *(int *)D_001A9440 = a1;
    l_24 = 0;
L85A71:;
    if (l_24 < 256) goto L85A84;
    goto L85ABC;
L85A7C:;
    l_24++;
    goto L85A71;
L85A84:;
    if (*(int *)(D_001A8430 + (l_24 << 4)) != a1) goto L85ABA;
    *(int *)sound_last_size = *(int *)(D_001A8434 + (l_24 << 4));
    return *(int *)(D_001A8438 + (l_24 << 4));
L85ABA:;
    goto L85A7C;
L85ABC:;
    l_24 = 0;
L85AC3:;
    if (*(int *)(D_001A8438 + (l_24 << 4)) == 0) goto L85ADA;
    l_24++;
    goto L85AC3;
L85ADA:;
    l_1C = archive_find_record(*(int *)dagger_snd, (int)D_001910AC, a1);
    l_20 = archive_record_size(*(int *)dagger_snd, l_1C);
    *(int *)(sound_cache + (l_24 << 4)) = *(int *)frame_counter;
    *(int *)(D_001A8430 + (l_24 << 4)) = a1;
    *(int *)(D_001A8434 + (l_24 << 4)) = l_20;
    *(int *)(D_001A8438 + (l_24 << 4)) = mc_malloc(l_20, (int)D_00176C20, 1338);
    dpmi_lock_region(*(int *)(D_001A8438 + (l_24 << 4)), l_20 + 4096);
    archive_read_record(*(int *)dagger_snd, l_1C, *(int *)(D_001A8438 + (l_24 << 4)));
    *(int *)sound_cache_bytes += l_20;
    sound_cache_trim();
    *(int *)sound_last_size = l_20;
    return *(int *)(D_001A8438 + (l_24 << 4));
}

void sound_cache_trim(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (*(int *)sound_cache_bytes < 393216) return;
L85BD0:;
    if (*(int *)sound_cache_bytes <= 262144) return;
    l_1C = -1;
    l_20 = *(int *)frame_counter;
    l_18 = 0;
L85BF6:;
    if (l_18 < 256) goto L85C09;
    goto L85C40;
L85C01:;
    l_18++;
    goto L85BF6;
L85C09:;
    if (*(int *)(D_001A8438 + (l_18 << 4)) == 0) goto L85C01;
    if (l_20 <= *(int *)(sound_cache + (l_18 << 4))) goto L85C3E;
    l_20 = *(int *)(sound_cache + (l_18 << 4));
    l_1C = l_18;
L85C3E:;
    goto L85C01;
L85C40:;
    if (l_1C == (-1)) return;
    dpmi_unlock_region(*(int *)(D_001A8438 + (l_1C << 4)), *(int *)(D_001A8434 + (l_1C << 4)) + 4096);
    if (*(int *)(D_001A8438 + (l_1C << 4)) == 0) goto L85C8E;
    if (*(int *)(D_001A8438 + (l_1C << 4)) != (-1751672937)) goto L85C90;
L85C8E:;
    goto L85CBB;
L85C90:;
    mc_free(*(int *)(D_001A8438 + (l_1C << 4)), (int)D_00176C20, 1375);
    *(int *)(D_001A8438 + (l_1C << 4)) = -1751672937;
L85CBB:;
    *(int *)(D_001A8438 + (l_1C << 4)) = 0;
    *(int *)(D_001A8430 + (l_1C << 4)) = -1;
    *(int *)sound_cache_bytes -= *(int *)(D_001A8434 + (l_1C << 4));
    goto L85BD0;
}

void sound_cache_free_all(void)
{
    int l_18;

    l_18 = 0;
L85D11:;
    if (l_18 < 256) goto L85D27;
    goto L85DD0;
L85D1F:;
    l_18++;
    goto L85D11;
L85D27:;
    if (*(int *)(D_001A8438 + (l_18 << 4)) == 0) goto L85DCB;
    dpmi_unlock_region(*(int *)(D_001A8438 + (l_18 << 4)), *(int *)(D_001A8434 + (l_18 << 4)) + 1024);
    if (*(int *)(D_001A8438 + (l_18 << 4)) == 0) goto L85D7E;
    if (*(int *)(D_001A8438 + (l_18 << 4)) != (-1751672937)) goto L85D80;
L85D7E:;
    goto L85DAB;
L85D80:;
    mc_free(*(int *)(D_001A8438 + (l_18 << 4)), (int)D_00176C20, 1392);
    *(int *)(D_001A8438 + (l_18 << 4)) = -1751672937;
L85DAB:;
    *(int *)(D_001A8438 + (l_18 << 4)) = 0;
    *(int *)(D_001A8430 + (l_18 << 4)) = -1;
L85DCB:;
    goto L85D1F;
L85DD0:;
    *(int *)sound_cache_bytes = 0;
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
    if (*(int *)((char *)a1 + 71) != *(int *)text_macro_fpc) goto L85F5F;
    *(int *)((char *)a1 + 71) = 0;
L85F5F:;
    return;
case 43:
    l_20 = (int)RECORD_DATA(a1);
    l_1C = *(int *)((char *)l_20 + 5);
    l_18 = 0;
L85F7D:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) > l_18) goto L85F9D;
    goto L85FB7;
L85F8E:;
    l_18++;
    (*(char (**)[66])&l_1C)++;
    goto L85F7D;
L85F9D:;
    if (*(int *)((char *)l_1C + 4) != *(int *)text_macro_fpc) goto L85FB5;
    *(int *)((char *)l_1C + 4) = 0;
L85FB5:;
    goto L85F8E;
L85FB7:;
    return;
case 56:
    l_1C = (int)RECORD_DATA(a1);
    l_18 = 0;
L85FC9:;
    if (a1->image > l_18) goto L85FEB;
    return;
L85FDC:;
    l_18++;
    (*(char (**)[66])&l_1C)++;
    goto L85FC9;
L85FEB:;
    if (*(int *)((char *)l_1C + 4) != *(int *)text_macro_fpc) goto L86003;
    *(int *)((char *)l_1C + 4) = 0;
L86003:;
    goto L85FDC;
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
    if (a1 != 3) goto L86069;
    return func_00086093((int)D_00187EC8);
L86069:;
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

    l_2C = func_000C5280(player_object->x ^ player_object->z, (*(int *)D_001343C0 / 40) << 6);
    l_2C >>= 3;
    l_2C = 256 - l_2C;
    l_30 = (l_2C * 192) >> 8;
    l_20 = func_000C5280(player_object->x ^ player_object->z, (*(int *)D_001343C0 / 40) << 6);
    l_20 >>= 3;
    l_1C = func_000C5280(player_object->x + player_object->z, (*(int *)D_001343C0 / 40) << 6);
    l_1C >>= 3;
    l_18 = func_000C5280(player_object->x - player_object->z, (*(int *)D_001343C0 / 40) << 6);
    l_18 >>= 3;
    func_000CE6E2((player_object->yaw + *(int *)D_001959BC) & 2047, 192, (int)&l_28, (int)&l_24);
    func_00136AD8((player_object->x + l_28) + (l_20 - 16), (player_object->y - 50) + (l_1C - 16), (player_object->z + l_24) + (l_18 - 16), 50, l_30, 0);
    func_00154D00((player_object->x + l_28) + (l_20 - 16), (player_object->y - 50) + (l_1C - 16), (player_object->z + l_24) + (l_18 - 16), 26883, -1, 1, 400);
}
