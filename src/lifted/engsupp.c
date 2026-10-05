/* engsupp.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern char D_001702D4[];
extern unsigned char player_environment;
extern short D_00179954[];
extern char D_0017995C[];
extern short D_00179966[];
extern signed char climate_texture_sets[];
extern signed char itemmaker_slot_kinds[];
extern signed char D_00190CE5;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char *hud_bar_image;
extern struct settings *game_settings;
extern char *D_00195C88;
extern int D_00195CD0;
extern char *D_00195D3C;
extern char D_00195D54[];
extern int D_00195DC0;
extern char D_00196120[];
extern signed char D_0019629F;
extern int D_00196478;
extern int D_0019647C;
extern char *D_00196484;
extern int climate_index;

extern int func_00014334(int);
extern int texture_archive_for_climate(int, unsigned short);
extern int climate_category(void);
extern int rand_range(int, int);
extern int object_find(struct record *, int);
extern int rand();
extern int srand();
extern int mc_memset();
extern int mc_memcpy();
extern int func_0012A608();
extern void shop_generate_stock(int, int, int, int, int);
extern void func_0007E815(struct record *, int);
extern void func_0007EB0B(struct record *, int);
extern void object_foreach_open(int, int);
int func_00014096(int, short);
int func_0001410F(struct record *);
int func_00014438(int, int);
int func_0001490D(int);
void world_for_each_object(int);

int engine_pick_object(int a1, int a2, int a3)
{
    int l_14;

    l_14 = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(hud_bar_image + 2));
    if (a2 > l_14) return 0;
    mc_memset(a3, 0, 18, (int)D_001702D4, 38, 4);
    *(int *)&D_00196484 = a3;
    if (*(int *)((char *)(*(int *)&D_00195C88 = func_0012A608(a1, a2)) + 4) == 1) return 0;
    if (*(int *)(D_00195C88 + 4) != 0) {
        D_0019647C = *(int *)(D_00195C88 + 4);
        D_00195DC0 = *(int *)(D_00195C88);
        world_for_each_object((int)func_0001410F);
    } else {
        D_00196478 = (int)D_00195C88;
        world_for_each_object((int)func_00014334);
    }
    return *(int *)(D_00196484) & 1;
}

int func_00014048(int a1, short a2)
{
    short l_18;

    *(int *)&l_18 = a1 + 8;
    if ((short)((unsigned short)(unsigned char)*(signed char *)((char *)a1)) <= a2) return 0;
    *(int *)&l_18 += ((int)(short)a2) << 3;
    return *(int *)&l_18;
}

int func_00014096(int a1, short a2)
{
    int l_20;
    int l_24;
    short l_18;

    l_20 = *(int *)((char *)a1);
    *(int *)&l_18 = 0;
    if (((int)(short)a2) >= *(int *)((char *)l_20 + 8)) return 0;
    l_24 = l_20 + *(int *)((char *)l_20 + 60);
    while ((short)(short)*(int *)&l_18 < a2) {
        l_24 += (((int)(unsigned char)*(signed char *)((char *)l_24)) << 3) + 8;
        (*(int *)&l_18)++;
    }
    return l_24;
}

int func_0001410F(struct record *a1)
{
    int l_28;
    struct block *l_24;
    struct block_model *l_20;
    int l_1C;

    if (((struct bf8_0_1 *)(D_00196484))->f != 0) return 0;
    switch (a1->type) {
    case 43:
        l_24 = &a1->data.block;
        l_20 = l_24->models;
        for (l_1C = 0; l_24->model_count > l_1C; l_1C++, l_20++) {
            if ((int)&l_20->model == D_0019647C) {
                *(int *)D_00195D54 = l_1C;
                *(int *)&D_00195D3C = (int)l_20;
                *(signed char *)(D_00196484) |= 13;
                *(int *)(D_00196484 + 4) = (int)a1;
                *(int *)(D_00196484 + 8) = (int)&*(signed char *)((char *)(l_1C << 8) + func_00014438(D_0019647C, D_00195DC0));
                *(short *)(D_00196484 + 12) = l_1C;
                return 1;
            }
        }
        return 0;
    case 56:
        l_20 = (struct block_model *)RECORD_DATA(a1);
        for (l_1C = 0; a1->model_count > l_1C; l_1C++, l_20++) {
            if ((int)&l_20->model == D_0019647C) {
                *(signed char *)(D_00196484) |= 5;
                *(int *)(D_00196484 + 4) = (int)a1;
                *(int *)(D_00196484 + 8) = func_00014438(D_0019647C, D_00195DC0);
                *(short *)(D_00196484 + 14) = l_20->id;
                *(short *)(D_00196484 + 16) = l_20->variant;
                return 1;
            }
        }
        return 0;
    case 6:
    case 32:
        l_28 = (int)RECORD_DATA(a1);
        if (D_0019647C == l_28) {
            *(signed char *)(D_00196484) |= 5;
            *(int *)(D_00196484 + 4) = (int)a1;
            *(int *)(D_00196484 + 8) = func_00014438(l_28, D_00195DC0);
            return 1;
        }
    }
    return 0;
}

int func_00014438(int a1, int a2)
{
    int l_1C;
    int l_18;

    for (l_18 = 0; l_18 < *(int *)(*(char **)((char *)a1) + 8); l_18++) {
        l_1C = func_00014096(a1, (int)(short)*(short *)&l_18);
        if (l_1C == a2) return l_18;
    }
    return -1;
}

void arch3d_apply_climate_textures(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    D_00190CE5 = climate_category();
    itemmaker_slot_kinds[0] = climate_texture_sets[climate_index];
    l_1C = a1 + *(int *)((char *)a1 + 60);
    for (l_18 = 0; l_18 < *(int *)((char *)a1 + 8); l_18++) {
        l_20 = l_1C;
        *(short *)((char *)l_20 + 2) = (*(short *)((char *)l_20 + 2) & 127) | (texture_archive_for_climate(((int)(unsigned short)*(short *)((char *)l_20 + 2)) >> 7, (int)(unsigned short)(*(short *)((char *)l_20 + 2) & 127)) << 7);
        l_1C += (((int)(unsigned char)*(signed char *)((char *)l_1C)) << 3) + 8;
    }
}

void dungeon_choose_textures(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = rand();
    srand(((unsigned)D_00195AC4->id) >> 16);
    l_1C = (int)(unsigned char)climate_texture_sets[climate_category()];
    if (l_1C == 1) return;
    mc_memcpy((int)D_00179966, (int)D_0017995C, 10, (int)D_001702D4, 279, 10);
    for (l_24 = 0; l_24 < 5; l_24++) {
        l_20 = rand_range(0, 4);
        if (l_20 == 2) l_20 += 2;
        l_20 += (int)(short)D_00179954[l_1C];
        D_00179966[l_24] = l_20;
    }
    srand(l_18);
}

void interior_stock_shelves(struct record *a1, struct building *a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = (int)RECORD_DATA(a1);
    l_18 = *(int *)((char *)l_1C + 5);
    for (l_14 = 0; ((int)(unsigned char)*(signed char *)((char *)l_1C)) > l_14; l_14++, (*(char (**)[66])&l_18)++) {
        if (((int)(unsigned short)*(short *)((char *)l_18)) == 418 || (((int)(unsigned short)*(short *)((char *)l_18)) == 410 && func_0001490D((int)(unsigned char)*(signed char *)((char *)l_18 + 2)) != 0)) {
            shop_generate_stock((int)D_00196120, (int)(unsigned char)*(signed char *)((char *)l_18 + 2), a2->quality, a2->type, l_14);
        }
    }
}

int func_0001490D(int a1)
{
    {
        int l_20;

        if (a1 == 3 || a1 == 4 || a1 == 7 || a1 == 8 || a1 == 50 || a1 == 51 || (a1 >= 32 && a1 <= 38)) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        return l_20;
    }
}

void world_for_each_object(int a1)
{
    struct record *l_24;
    struct record *l_20;
    struct record *l_1C;
    unsigned short l_18;

    D_00195CD0 = (int)object_find;
    if (((int)player_environment) < 3) {
        if (player_object->parent->type != 1) {
            object_find(player_object->parent->children, a1);
        } else {
            func_0007EB0B(player_object, a1);
        }
        l_24 = D_00195AC4->children;
        while (l_24 != 0) {
            l_20 = l_24->next;
            l_1C = l_24->children;
            l_18 = l_24->flags;
            if (l_24->type != 38) {
                ((int (*)())(a1))(l_24);
                if (((int)(unsigned short)(*(int *)&l_18 & 1)) == 0) object_find(l_1C, a1);
            }
            l_24 = l_20;
        }
    } else {
        if (D_0019629F != 0) {
            object_find(D_00195AC4, a1);
        } else {
            func_0007E815(player_object, a1);
        }
        l_24 = D_00195AC4->children;
        while (l_24 != 0) {
            l_20 = l_24->next;
            l_1C = l_24->children;
            l_18 = l_24->flags;
            if (l_24->type != 47) {
                ((int (*)())(a1))(l_24);
                if (((int)(unsigned short)(*(int *)&l_18 & 1)) == 0) object_find(l_1C, a1);
            }
            l_24 = l_20;
        }
    }
    D_00195CD0 = (int)object_foreach_open;
}
