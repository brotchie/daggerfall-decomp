/* engsupp.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern char D_001702D4[];
extern char player_environment[];
extern char D_00179954[];
extern char D_0017995C[];
extern char D_00179966[];
extern char climate_texture_sets[];
extern char itemmaker_slot_kinds[];
extern char D_00190CE5[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char hud_bar_image[];
extern struct settings *game_settings;
extern char D_00195C88[];
extern char D_00195CD0[];
extern char D_00195D3C[];
extern char D_00195D54[];
extern char D_00195DC0[];
extern char D_00196120[];
extern char D_0019629F[];
extern char D_00196478[];
extern char D_0019647C[];
extern char D_00196484[];
extern char climate_index[];

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

    l_14 = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2));
    if (a2 <= l_14) goto L13FA8;
    return 0;
L13FA8:;
    mc_memset(a3, 0, 18, (int)D_001702D4, 38, 4);
    *(int *)D_00196484 = a3;
    if (*(int *)((char *)(*(int *)D_00195C88 = func_0012A608(a1, a2)) + 4) != 1) goto L13FEC;
    return 0;
L13FEC:;
    if (*(int *)(*(char **)D_00195C88 + 4) == 0) goto L1401C;
    *(int *)D_0019647C = *(int *)(*(char **)D_00195C88 + 4);
    *(int *)D_00195DC0 = *(int *)(*(char **)D_00195C88);
    world_for_each_object((int)func_0001410F);
    goto L14030;
L1401C:;
    *(int *)D_00196478 = *(int *)D_00195C88;
    world_for_each_object((int)func_00014334);
L14030:;
    return *(int *)(*(char **)D_00196484) & 1;
}

int func_00014048(int a1, short a2)
{
    short l_18;

    *(int *)&l_18 = a1 + 8;
    if ((short)((unsigned short)(unsigned char)*(signed char *)((char *)a1)) > a2) goto L1407A;
    return 0;
L1407A:;
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
    if (((int)(short)a2) < *(int *)((char *)l_20 + 8)) goto L140CD;
    return 0;
L140CD:;
    l_24 = l_20 + *(int *)((char *)l_20 + 60);
L140D9:;
    if ((short)(short)*(int *)&l_18 >= a2) goto L140FD;
    l_24 += (((int)(unsigned char)*(signed char *)((char *)l_24)) << 3) + 8;
    (*(int *)&l_18)++;
    goto L140D9;
L140FD:;
    return l_24;
}

int func_0001410F(struct record *a1)
{
    int l_28;
    struct block *l_24;
    struct block_model *l_20;
    int l_1C;

    if (((struct bf8_0_1 *)(*(char **)D_00196484))->f == 0) goto L14136;
    return 0;
L14136:;
    switch (a1->type) {
case 43:
    l_24 = &a1->data.block;
    l_20 = l_24->models;
    l_1C = 0;
L14196:;
    if (l_24->model_count > l_1C) goto L141B9;
    goto L14227;
L141AA:;
    l_1C++;
    l_20++;
    goto L14196;
L141B9:;
    if ((int)&l_20->model != *(int *)D_0019647C) goto L14225;
    *(int *)D_00195D54 = l_1C;
    *(int *)D_00195D3C = (int)l_20;
    *(signed char *)(*(char **)D_00196484) |= 13;
    *(int *)(*(char **)D_00196484 + 4) = (int)a1;
    *(int *)(*(char **)D_00196484 + 8) = (int)&*(signed char *)((char *)(l_1C << 8) + func_00014438(*(int *)D_0019647C, *(int *)D_00195DC0));
    *(short *)(*(char **)D_00196484 + 12) = l_1C;
    return 1;
L14225:;
    goto L141AA;
L14227:;
    return 0;
case 56:
    l_20 = (struct block_model *)RECORD_DATA(a1);
    l_1C = 0;
L14243:;
    if (a1->model_count > l_1C) goto L14268;
    goto L142D0;
L14259:;
    l_1C++;
    l_20++;
    goto L14243;
L14268:;
    if ((int)&l_20->model != *(int *)D_0019647C) goto L142CE;
    *(signed char *)(*(char **)D_00196484) |= 5;
    *(int *)(*(char **)D_00196484 + 4) = (int)a1;
    *(int *)(*(char **)D_00196484 + 8) = func_00014438(*(int *)D_0019647C, *(int *)D_00195DC0);
    *(short *)(*(char **)D_00196484 + 14) = l_20->id;
    *(short *)(*(char **)D_00196484 + 16) = l_20->variant;
    return 1;
L142CE:;
    goto L14259;
L142D0:;
    return 0;
case 6:
case 32:
    l_28 = (int)RECORD_DATA(a1);
    if (*(int *)D_0019647C != l_28) goto L14320;
    *(signed char *)(*(char **)D_00196484) |= 5;
    *(int *)(*(char **)D_00196484 + 4) = (int)a1;
    *(int *)(*(char **)D_00196484 + 8) = func_00014438(l_28, *(int *)D_00195DC0);
    return 1;
default:
L14320:;
    return 0;
}
}

int func_00014438(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_18 = 0;
L14452:;
    if (l_18 < *(int *)(*(char **)((char *)a1) + 8)) goto L14469;
    goto L1448A;
L14461:;
    l_18++;
    goto L14452;
L14469:;
    l_1C = func_00014096(a1, (int)(short)*(short *)&l_18);
    if (l_1C != a2) goto L14488;
    return l_18;
L14488:;
    goto L14461;
L1448A:;
    return -1;
}

void arch3d_apply_climate_textures(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    *(signed char *)D_00190CE5 = climate_category();
    *(signed char *)itemmaker_slot_kinds = *(signed char *)(climate_texture_sets + *(int *)climate_index);
    l_1C = a1 + *(int *)((char *)a1 + 60);
    l_18 = 0;
L14556:;
    if (l_18 < *(int *)((char *)a1 + 8)) goto L1456B;
    return;
L14563:;
    l_18++;
    goto L14556;
L1456B:;
    l_20 = l_1C;
    *(short *)((char *)l_20 + 2) = (*(short *)((char *)l_20 + 2) & 127) | (texture_archive_for_climate(((int)(unsigned short)*(short *)((char *)l_20 + 2)) >> 7, (int)(unsigned short)(*(short *)((char *)l_20 + 2) & 127)) << 7);
    l_1C += (((int)(unsigned char)*(signed char *)((char *)l_1C)) << 3) + 8;
    goto L14563;
}

void dungeon_choose_textures(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = rand();
    srand(((unsigned)D_00195AC4->id) >> 16);
    l_1C = (int)(unsigned char)*(signed char *)(climate_texture_sets + climate_category());
    if (l_1C == 1) return;
    mc_memcpy((int)D_00179966, (int)D_0017995C, 10, (int)D_001702D4, 279, 10);
    l_24 = 0;
L147F8:;
    if (l_24 < 5) goto L14808;
    goto L14841;
L14800:;
    l_24++;
    goto L147F8;
L14808:;
    l_20 = rand_range(0, 4);
    if (l_20 != 2) goto L14821;
    l_20 += 2;
L14821:;
    l_20 += (int)(short)*(short *)(D_00179954 + (l_1C * 2));
    *(short *)(D_00179966 + (l_24 * 2)) = l_20;
    goto L14800;
L14841:;
    srand(l_18);
}

void interior_stock_shelves(struct record *a1, struct building *a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = (int)RECORD_DATA(a1);
    l_18 = *(int *)((char *)l_1C + 5);
    l_14 = 0;
L1487F:;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) > l_14) goto L148A2;
    return;
L14893:;
    l_14++;
    (*(char (**)[66])&l_18)++;
    goto L1487F;
L148A2:;
    if (((int)(unsigned short)*(short *)((char *)l_18)) == 418) goto L148DC;
    if (((int)(unsigned short)*(short *)((char *)l_18)) != 410) goto L148DA;
    if (func_0001490D((int)(unsigned char)*(signed char *)((char *)l_18 + 2)) != 0) goto L148DC;
L148DA:;
    goto L14902;
L148DC:;
    shop_generate_stock((int)D_00196120, (int)(unsigned char)*(signed char *)((char *)l_18 + 2), a2->quality, a2->type, l_14);
L14902:;
    goto L14893;
}

int func_0001490D(int a1)
{
{
    int l_20;

    if (a1 == 3) goto L1492A;
    if (a1 != 4) goto L1492C;
L1492A:;
    goto L14932;
L1492C:;
    if (a1 != 7) goto L14934;
L14932:;
    goto L1493A;
L14934:;
    if (a1 != 8) goto L1493C;
L1493A:;
    goto L14942;
L1493C:;
    if (a1 != 50) goto L14944;
L14942:;
    goto L1494A;
L14944:;
    if (a1 != 51) goto L1494C;
L1494A:;
    goto L1495A;
L1494C:;
    if (a1 < 32) goto L14958;
    if (a1 <= 38) goto L1495A;
L14958:;
    goto L14963;
L1495A:;
    l_20 = 1;
    goto L1496A;
L14963:;
    l_20 = 0;
L1496A:;
    return l_20;
}
}

void world_for_each_object(int a1)
{
    struct record *l_24;
    struct record *l_20;
    struct record *l_1C;
    unsigned short l_18;

    *(int *)D_00195CD0 = (int)object_find;
    if (((int)(unsigned char)*(signed char *)player_environment) >= 3) goto L14A49;
    if (player_object->parent->type == 1) goto L149D1;
    object_find(player_object->parent->children, a1);
    goto L149DE;
L149D1:;
    func_0007EB0B(player_object, a1);
L149DE:;
    l_24 = D_00195AC4->children;
L149E9:;
    if (l_24 == 0) goto L14A44;
    l_20 = l_24->next;
    l_1C = l_24->children;
    l_18 = l_24->flags;
    if (l_24->type == 38) goto L14A3C;
    ((int (*)())(a1))(l_24);
    if (((int)(unsigned short)(*(int *)&l_18 & 1)) != 0) goto L14A3C;
    object_find(l_1C, a1);
L14A3C:;
    l_24 = l_20;
    goto L149E9;
L14A44:;
    goto L14AD4;
L14A49:;
    if (*(signed char *)D_0019629F == 0) goto L14A61;
    object_find(D_00195AC4, a1);
    goto L14A6E;
L14A61:;
    func_0007E815(player_object, a1);
L14A6E:;
    l_24 = D_00195AC4->children;
L14A79:;
    if (l_24 == 0) goto L14AD4;
    l_20 = l_24->next;
    l_1C = l_24->children;
    l_18 = l_24->flags;
    if (l_24->type == 47) goto L14ACC;
    ((int (*)())(a1))(l_24);
    if (((int)(unsigned short)(*(int *)&l_18 & 1)) != 0) goto L14ACC;
    object_find(l_1C, a1);
L14ACC:;
    l_24 = l_20;
    goto L14A79;
L14AD4:;
    *(int *)D_00195CD0 = (int)object_foreach_open;
}
