/* object.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00176E44[];
extern char D_00176E4D[];
extern char D_00176E70[];
extern char D_00187FDC[];
extern char D_00190BE4[];
extern char itemmaker_slot_kinds[];
extern struct record *nonworld_root;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct record *D_00195AF4;
extern struct location *current_location;
extern char object_heap[];
extern char loaded_location_door_count[];
extern char D_00199770[];
extern char D_001A3F94[];
extern char D_001A9AF4[];
extern char D_001A9AF8[];
extern char D_001A9AFC[];
extern char D_001A9B00[];
extern char D_001A9B04[];
extern char D_001A9B08[];
extern char D_001A9B0C[];
extern char D_001A9B10[];
extern char D_001A9B14[];
extern char D_001A9B18[];
extern char D_001A9B1C[];
extern char D_001A9B20[];
extern char D_001A9B24[];
extern char D_001A9B28[];
extern char D_001A9B2C[];
extern char D_001A9B30[];
extern char object_heap_size[];
extern char object_heap_free[];
extern char D_001A9B3C[];
extern char D_001A9B40[];
extern char D_001A9B42[];
extern char D_001A9B44[];
extern char potion_ingredient_scroll[];
extern char potion_ingredient_count[];

extern int func_00045E45(int);
extern int mem_pool_alloc(int, int);
extern int mem_pool_release(int);
extern int object_count_type(struct record *, short);
extern int rand();
extern int mc_memset();
extern int func_000A0ED9(int, int);
extern int mc_memcpy();
extern int func_000A148C(int, ...);
extern int func_000C7FD9();
extern void func_000298F3(struct record *);
extern void fatal_error(int);
extern void mem_pool_init(int, int);
extern void mem_pool_free(int);
extern void object_follow_move_cb(struct record *);
struct record *object_free_single(struct record *);
struct record *object_delete(struct record *);
struct record *object_alloc(struct record *, struct record *, int);
struct record *object_create_child(struct record *, struct record *, int);
int object_find(struct record *, int);
int object_find_type_cb(struct record *);
int object_find_by_id_cb(struct record *);
struct record *object_find_by_id(struct record *, int);
int object_random_type_cb(struct record *);
int object_new_id(int);
int object_find_quest_cb(struct record *);
void object_free_node(struct record *);
void object_free_children(struct record *);
void object_heap_release(struct record *);
void object_set_position(struct record *, int, int, int, int, int, int);
void object_unlink(struct record *);
void object_insert_after(struct record *, struct record *);
void object_add_child(struct record *, struct record *);
void object_foreach_pre(struct record *, int);
void object_foreach_post(struct record *, int);
void object_foreach(struct record *, int);
void func_0008EAA7(struct record *);
void func_0008EB25(struct record *);
void object_delete_quest_cb(struct record *);
void object_tree_size_cb(struct record *);
void func_0008ED52(struct record *);
#pragma aux func_000A0ED9 parm routine [];

void object_heap_init(void)
{
    if (*(int *)object_heap_size != 0) goto L8D905;
    if (*(int *)D_001A3F94 >= 13000) goto L8D8FB;
    *(int *)object_heap_size = 1280000;
    goto L8D905;
L8D8FB:;
    *(int *)object_heap_size = 2048000;
L8D905:;
    *(int *)object_heap_free = *(int *)object_heap_size;
    func_000A0ED9(55, (int)D_00176E44);
    func_000A148C((int)D_00176E4D, *(int *)object_heap_size);
    mem_pool_init((int)object_heap, *(int *)object_heap_size);
    (D_00195AC4 = object_alloc(0, 0, 48))->type = 1;
    D_00195AC4->image = 65535;
    D_00195AC4->id = -65535;
    (current_location = &D_00195AC4->data.location)->buildings = 0;
    (nonworld_root = object_alloc(0, 0, 0))->type = 39;
    nonworld_root->id = 700;
}

void object_heap_shutdown(void)
{
    mem_pool_free((int)object_heap);
}

void object_free_node(struct record *a1)
{
    if (a1->twin == 0) goto L8DA02;
    a1->twin->twin = 0;
L8DA02:;
    object_unlink(a1);
    object_heap_release(a1);
}

void object_free_children(struct record *a1)
{
    if (a1 == 0) return;
    object_foreach_post(a1->children, (int)object_free_node);
}

struct record *object_free_single(struct record *a1)
{
    struct record *l_1C;

    if (a1 != 0) goto L8DA6D;
    return 0;
L8DA6D:;
    l_1C = a1->next;
    object_free_node(a1);
    return l_1C;
}

struct record *object_delete(struct record *a1)
{
    struct record *l_1C;

    l_1C = a1->next;
    if (a1 != 0) goto L8DABA;
    return 0;
L8DABA:;
    object_free_children(a1);
    object_free_single(a1);
    return l_1C;
}

struct record *func_0008DADD(struct record *a1)
{
    struct record *l_1C;

    l_1C = a1->parent;
    if (a1 != 0) goto L8DB06;
    return 0;
L8DB06:;
    object_unlink(a1);
    return l_1C;
}

struct record *object_alloc(struct record *a1, struct record *a2, int a3)
{
    int l_18;
    struct record *l_14;

    l_18 = a3 + 71;
    *(int *)object_heap_free -= l_18 + 18;
    l_14 = (struct record *)mem_pool_alloc((int)object_heap, l_18);
    if (l_14 != 0) goto L8DB6B;
    fatal_error((int)D_00176E70);
L8DB6B:;
    if (a2 == 0) goto L8DBD0;
    mc_memcpy(l_14, a2, 55, (int)D_00176E44, 163, 4);
    mc_memcpy(&l_14->data, &a2->data, l_18 - 71, (int)D_00176E44, 164, 4);
    mc_memset(&l_14->next, 0, 16, (int)D_00176E44, 165, 4);
    goto L8DBFB;
L8DBD0:;
    mc_memset(l_14, 0, l_18, (int)D_00176E44, 169, 4);
    l_14->id = object_new_id(1);
L8DBFB:;
    if (a1 == 0) goto L8DC0C;
    object_insert_after(a1, l_14);
L8DC0C:;
    return l_14;
}

void object_heap_release(struct record *a1)
{
    int l_18;

    l_18 = (int)a1 - 18;
    *(int *)object_heap_free += *(int *)((char *)l_18 + 12) + 18;
    mem_pool_release((int)a1);
}

struct record *object_clone(struct record *a1)
{
    int l_24;
    struct record *l_20;
    int l_1C;

    l_24 = (int)a1 - 18;
    l_1C = *(int *)((char *)l_24 + 12);
    l_20 = object_create_child(a1->parent, 0, l_1C - 71);
    mc_memcpy(l_20, a1, 55, (int)D_00176E44, 203, 4);
    mc_memcpy(&l_20->data, &a1->data, l_1C - 71, (int)D_00176E44, 204, 4);
    return l_20;
}

struct record *object_create_child(struct record *a1, struct record *a2, int a3)
{
    struct record *l_18;
    struct record *l_14;

    l_18 = object_alloc(a1->children, a2, a3);
    l_18->parent = (struct record *)a1;
    l_14 = l_18;
L8DD1B:;
    if (l_14 == 0) goto L8DD35;
    a1->children = (struct record *)l_14;
    l_14 = l_14->prev;
    goto L8DD1B;
L8DD35:;
    return l_18;
}

struct record *object_reparent(struct record *a1, struct record *a2)
{
    object_unlink(a2);
    object_add_child(a1, a2);
    return a2;
}

void object_set_position(struct record *a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    int l_C;

    *(int *)D_001A9B2C = a1->x;
    *(int *)D_001A9B30 = a1->y;
    *(int *)D_001A9B28 = a1->z;
    *(int *)D_001A9B1C = a1->angle_x;
    *(int *)D_001A9B20 = a1->yaw;
    *(int *)D_001A9B24 = a1->angle_z;
    a1->x = a2;
    a1->y = a3;
    a1->z = a4;
    a1->angle_x = a5;
    a1->yaw = a6;
    a1->angle_z = a7;
    *(int *)D_001A9AF4 = a2;
    *(int *)D_001A9AF8 = a3;
    *(int *)D_001A9AFC = a4;
    *(int *)D_001A9B00 = a5;
    *(int *)D_001A9B04 = a6;
    *(int *)D_001A9B08 = a7;
    if (a1->type == 43) return;
    object_foreach(a1->children, (int)object_follow_move_cb);
}

void object_move_by(struct record *a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    object_set_position(a1, a1->x + a2, a1->y + a3, a1->z + a4, a1->angle_x + a5, a1->yaw + a6, a1->angle_z + a7);
}

void object_unlink(struct record *a1)
{
    if (a1->parent == 0) goto L8E02D;
    if (a1->parent->children == a1) goto L8E02F;
L8E02D:;
    goto L8E03E;
L8E02F:;
    a1->parent->children = a1->next;
L8E03E:;
    if (a1->prev == 0) goto L8E056;
    a1->prev->next = a1->next;
L8E056:;
    if (a1->next == 0) goto L8E06E;
    a1->next->prev = a1->prev;
L8E06E:;
    a1->next = 0;
    a1->prev = a1->next;
    a1->parent = a1->prev;
}

void object_insert_after(struct record *a1, struct record *a2)
{
    a2->next = a1->next;
    if (a1->next == 0) goto L8E0CE;
    a1->next->prev = (struct record *)a2;
L8E0CE:;
    a1->next = (struct record *)a2;
    a2->prev = (struct record *)a1;
    a2->parent = a1->parent;
}

void object_add_child(struct record *a1, struct record *a2)
{
    if (a1->children == 0) goto L8E121;
    object_insert_after(a1->children, a2);
    return;
L8E121:;
    a1->children = (struct record *)a2;
    a2->parent = (struct record *)a1;
    a2->prev = 0;
    a2->next = a2->prev;
}

void func_0008E152(struct record *a1, struct record *a2)
{
    struct record *l_18;
    struct record *l_14;

    if (a1->next == a2) goto L8E17B;
    if (a2->next != a1) goto L8E180;
L8E17B:;
    goto L8E212;
L8E180:;
    l_18 = a2->next;
    l_14 = a2->prev;
    if (a1->prev == 0) goto L8E1A7;
    a1->prev->next = (struct record *)a2;
L8E1A7:;
    a2->next = a1->next;
    a2->prev = a1->prev;
    if (a2->next == 0) goto L8E1D4;
    a2->next->prev = (struct record *)a2;
L8E1D4:;
    if (a2->prev == 0) goto L8E1E9;
    a2->prev->next = (struct record *)a1;
L8E1E9:;
    a1->next = (struct record *)l_18;
    a1->prev = (struct record *)l_14;
    if (a1->next == 0) goto L8E210;
    a1->next->prev = (struct record *)a1;
L8E210:;
    goto L8E283;
L8E212:;
    if (a2->next != a1) goto L8E22F;
    l_18 = a1;
    a1 = a2;
    a2 = l_18;
L8E22F:;
    if (a1->prev == 0) goto L8E244;
    a1->prev->next = (struct record *)a2;
L8E244:;
    if (a2->next == 0) goto L8E259;
    a2->next->prev = (struct record *)a1;
L8E259:;
    a2->prev = a1->prev;
    a1->next = a2->next;
    a1->prev = (struct record *)a2;
    a2->next = (struct record *)a1;
L8E283:;
    if (a1->prev != 0) goto L8E295;
    if (a1->parent != 0) goto L8E297;
L8E295:;
    goto L8E2A3;
L8E297:;
    a1->parent->children = (struct record *)a1;
L8E2A3:;
    if (a2->prev != 0) goto L8E2B5;
    if (a2->parent != 0) goto L8E2B7;
L8E2B5:;
    return;
L8E2B7:;
    a2->parent->children = (struct record *)a2;
}

void func_0008E2CC(struct record *a1, int a2, int a3)
{
L8E2E1:;
    if (a1 == 0) return;
    if (func_000C7FD9(a1->x, a1->z, player_object->x, player_object->z) >= a3) goto L8E344;
    ((int (*)())(a2))(a1);
    if (a1->children == 0) goto L8E331;
    if ((a1->flags & 1) == 0) goto L8E333;
L8E331:;
    goto L8E344;
L8E333:;
    func_0008E2CC(a1->children, a2, a3);
L8E344:;
    a1 = a1->next;
    goto L8E2E1;
}

void object_foreach_pre(struct record *a1, int a2)
{
    struct record *l_14;

L8E36A:;
    if (a1 == 0) return;
    l_14 = a1->next;
    ((int (*)())(a2))(a1);
    if (a1->children == 0) goto L8E396;
    object_foreach_pre(a1->children, a2);
L8E396:;
    a1 = l_14;
    goto L8E36A;
}

void object_foreach_post(struct record *a1, int a2)
{
    struct record *l_14;

L8E3BA:;
    if (a1 == 0) return;
    l_14 = a1->next;
    if (a1->children == 0) goto L8E3E0;
    object_foreach_post(a1->children, a2);
L8E3E0:;
    ((int (*)())(a2))(a1);
    a1 = l_14;
    goto L8E3BA;
}

void object_foreach(struct record *a1, int a2)
{
    struct record *l_14;

L8E40A:;
    if (a1 == 0) return;
    l_14 = a1->next;
    if (a1->children == 0) goto L8E430;
    object_foreach(a1->children, a2);
L8E430:;
    ((int (*)())(a2))(a1);
    a1 = l_14;
    goto L8E40A;
}

void func_0008E447(struct record *a1, int a2)
{
    struct record *l_14;

L8E45A:;
    if (a1 == 0) return;
    l_14 = a1->next;
    if (a1->children == 0) goto L8E481;
    if (a1->type != 4) goto L8E483;
L8E481:;
    goto L8E491;
L8E483:;
    func_0008E447(a1->children, a2);
L8E491:;
    ((int (*)())(a2))(a1);
    a1 = l_14;
    goto L8E45A;
}

void object_foreach_open(struct record *a1, int a2)
{
L8E4BB:;
    if (a1 == 0) return;
    ((int (*)())(a2))(a1);
    if (a1->children == 0) goto L8E4E5;
    if ((a1->flags & 1) == 0) goto L8E4E7;
L8E4E5:;
    goto L8E4F5;
L8E4E7:;
    object_foreach_open(a1->children, a2);
L8E4F5:;
    a1 = a1->next;
    goto L8E4BB;
}

void func_0008E509(struct record *a1, int a2)
{
    struct record *l_14;

L8E51C:;
    if (a1 == 0) return;
    l_14 = a1->next;
    if (((int (*)())(a2))(a1) != 0) goto L8E53E;
    if (a1->children != 0) goto L8E540;
L8E53E:;
    goto L8E54E;
L8E540:;
    func_0008E509(a1->children, a2);
L8E54E:;
    a1 = l_14;
    goto L8E51C;
}

int object_find(struct record *a1, int a2)
{
L8E572:;
    if (a1 == 0) goto L8E5B1;
    if (((int (*)())(a2))(a1) == 0) goto L8E58B;
    return 1;
L8E58B:;
    if (object_find(a1->children, a2) == 0) goto L8E5A6;
    return 1;
L8E5A6:;
    a1 = a1->next;
    goto L8E572;
L8E5B1:;
    return 0;
}

int object_find_open(struct record *a1, int a2)
{
L8E5D7:;
    if (a1 == 0) goto L8E636;
    if (((int (*)())(a2))(a1) == 0) goto L8E5F0;
    return 1;
L8E5F0:;
    if (a1->children == 0) goto L8E60E;
    if ((a1->flags & 1) == 0) goto L8E610;
L8E60E:;
    goto L8E62B;
L8E610:;
    if (object_find(a1->children, a2) == 0) goto L8E62B;
    return 1;
L8E62B:;
    a1 = a1->next;
    goto L8E5D7;
L8E636:;
    return 0;
}

void object_find_item_cb(struct record *a1)
{
    struct item *l_18;

    if (((int)(short)*(short *)D_001A9B42) == (-1)) goto L8E675;
    if (a1->type == 2) goto L8E677;
L8E675:;
    return;
L8E677:;
    l_18 = &a1->data.item;
    if (l_18->group != *(short *)D_001A9B40) goto L8E6A8;
    if (l_18->index == *(short *)D_001A9B42) goto L8E6AA;
L8E6A8:;
    return;
L8E6AA:;
    *(short *)D_001A9B42 = 65535;
    D_00195AF4 = a1;
}

int object_find_type_cb(struct record *a1)
{
    if ((short)a1->type == *(short *)D_001A9B42) goto L8E74B;
    return 0;
L8E74B:;
    D_00195AF4 = a1;
    return 1;
}

struct record *object_find_type(struct record *a1, int a2)
{
    D_00195AF4 = 0;
    *(short *)D_001A9B42 = a2;
    object_find(a1, (int)object_find_type_cb);
    return D_00195AF4;
}

void object_count_type_cb(struct record *a1)
{
    if ((short)a1->type != *(short *)D_001A9B42) return;
    (*(short *)D_001A9B3C)++;
}

struct record *object_create_in_block(struct record *a1, int a2, int a3, int a4, int a5)
{
    struct record *l_10;

    if (a1 == 0) goto L8E857;
    l_10 = object_create_child(a1, 0, a3);
    goto L8E866;
L8E857:;
    l_10 = object_alloc(0, 0, a3);
L8E866:;
    l_10->type = a2;
    l_10->image = a4;
    *(short *)((char *)l_10 + 19) = a5;
    l_10->id = D_00195AC4->id + ((int)(unsigned short)(current_location->object_counter)++);
    if (l_10->id != (-1016397758)) goto L8E8D7;
    mc_memcpy((int)D_001A9B44, l_10, 71, (int)D_00176E44, 634, 4);
    *(int *)D_001A9B0C = (int)l_10;
L8E8D7:;
    return l_10;
}

int object_find_by_id_cb(struct record *a1)
{
    if (a1->id != *(int *)D_001A9B18) goto L8E910;
    *(int *)D_001A9B14 = (int)a1;
L8E910:;
    return *(int *)D_001A9B14;
}

struct record *object_find_by_id(struct record *a1, int a2)
{
    *(int *)D_001A9B18 = a2;
    *(int *)D_001A9B14 = 0;
    if (a1 != 0) goto L8E99F;
    object_foreach(D_00195AC4, (int)object_find_by_id_cb);
    *(int *)D_00199770 = *(int *)D_001A9B14;
    if (*(int *)D_001A9B14 == 0) goto L8E97C;
    return *(struct record **)D_001A9B14;
L8E97C:;
    object_foreach(nonworld_root, (int)object_find_by_id_cb);
    *(int *)D_00199770 = *(int *)D_001A9B14;
    return *(struct record **)D_001A9B14;
L8E99F:;
    object_find(a1, (int)object_find_by_id_cb);
    return *(struct record **)D_001A9B14;
}

int object_random_type_cb(struct record *a1)
{
    if (a1->type == *(signed char *)itemmaker_slot_kinds) goto L8E9EC;
    return 0;
L8E9EC:;
    if (*(int *)D_00190BE4 != 0) goto L8EA06;
    D_00195AF4 = a1;
    return 1;
L8EA06:;
    (*(int *)D_00190BE4)--;
    return 0;
}

struct record *object_random_child_of_type(struct record *a1, int a2)
{
    if ((*(int *)D_00190BE4 = object_count_type(a1->children, (int)(short)*(short *)&a2)) != 0) goto L8EA59;
    return 0;
L8EA59:;
    D_00195AF4 = 0;
    *(int *)D_00190BE4 = rand() % *(int *)D_00190BE4;
    *(signed char *)itemmaker_slot_kinds = *(signed char *)&a2;
    object_find(a1->children, (int)object_random_type_cb);
    return D_00195AF4;
}

void func_0008EAA7(struct record *a1)
{
    if ((a1->id & -65536) != *(int *)D_001A9B10) return;
    if (a1->twin == 0) goto L8EADF;
    a1->twin->twin = 0;
L8EADF:;
    object_free_single(a1);
}

void func_0008EAF1(struct record *a1, int a2)
{
    *(int *)D_001A9B10 = a2 & -65536;
    object_foreach_post(a1, (int)func_0008EAA7);
}

void func_0008EB25(struct record *a1)
{
    struct record *l_18;

    if (a1->twin == 0) return;
    l_18 = a1->twin;
}

void func_0008EB52(void)
{
    object_foreach_pre(D_00195AC4, (int)func_0008EB25);
    object_foreach_pre(nonworld_root, (int)func_0008EB25);
}

int object_new_id(int a1)
{
    int l_1C;

    if (*(int *)D_00187FDC < 63000) goto L8EBAF;
    *(int *)D_00187FDC = 10000;
L8EBAF:;
    l_1C = (a1 << 16) + (*(int *)D_00187FDC)++;
    if (object_find_by_id(D_00195AC4, l_1C) != 0) goto L8EBE8;
    if (object_find_by_id(nonworld_root, l_1C) == 0) goto L8EBEA;
L8EBE8:;
    goto L8EBAF;
L8EBEA:;
    return l_1C;
}

void object_delete_quest_cb(struct record *a1)
{
    int l_18;

    if (a1->quest_id != *(signed char *)itemmaker_slot_kinds) return;
    if (a1->type != 8) goto L8EC65;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 73) & 128)) == 0) goto L8EC65;
    a1->twin->twin = 0;
    a1->twin = 0;
    a1->quest_id = 0;
    return;
L8EC65:;
    if (*(int *)loaded_location_door_count == 0) goto L8EC86;
    if ((a1->id & -65536) == (D_00195AC4->id & -65536)) goto L8EC88;
L8EC86:;
    goto L8ECA3;
L8EC88:;
    l_18 = func_00045E45(a1->id);
    if (l_18 == 0) goto L8ECA3;
    *(signed char *)((char *)l_18 + 3) &= 15;
L8ECA3:;
    func_000298F3(a1);
    object_delete(a1);
}

void object_delete_quest_objects(struct record *a1, unsigned char a2)
{
    *(signed char *)itemmaker_slot_kinds = a2;
    object_foreach_post(a1, (int)object_delete_quest_cb);
}

void object_tree_size_cb(struct record *a1)
{
    *(int *)D_00190BE4 += *(int *)((char *)a1 - 6);
}

int object_tree_size(struct record *a1)
{
    *(int *)D_00190BE4 = 0;
    object_foreach(a1, (int)object_tree_size_cb);
    return *(int *)D_00190BE4;
}

void func_0008ED52(struct record *a1)
{
    if (a1->type != *(signed char *)itemmaker_slot_kinds) return;
    object_delete(a1);
}

void func_0008ED87(struct record *a1, unsigned char a2)
{
    *(signed char *)itemmaker_slot_kinds = a2;
    object_foreach_post(a1, (int)func_0008ED52);
}

int object_find_quest_cb(struct record *a1)
{
    if (a1->quest_id != *(signed char *)itemmaker_slot_kinds) goto L8EDEE;
    return (int)(D_00195AF4 = a1);
L8EDEE:;
    return 0;
}

struct record *object_find_quest(struct record *a1, unsigned char a2)
{
    *(signed char *)itemmaker_slot_kinds = a2;
    D_00195AF4 = 0;
    object_find(a1, (int)object_find_quest_cb);
    return D_00195AF4;
}

int potionmaker_scroll_up(void)
{
    if (*(int *)potion_ingredient_scroll == 0) goto L8EE66;
    *(int *)potion_ingredient_scroll -= 3;
L8EE66:;
    return 0;
}

int potionmaker_scroll_down(void)
{
    if ((*(int *)potion_ingredient_count - *(int *)potion_ingredient_scroll) > 12) goto L8EEA1;
    return 0;
L8EEA1:;
    *(int *)potion_ingredient_scroll += 3;
    return 0;
}
