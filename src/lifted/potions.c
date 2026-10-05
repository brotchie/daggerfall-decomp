/* potions.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0012B508[];
extern char key_down_esc[];
extern char D_00176E94[];
extern char D_00176E9E[];
extern char D_00176EBA[];
extern char D_00176ED0[];
extern char D_00176EDF[];
extern char D_00176EFB[];
extern char potion_recipes[];
extern char D_00180B42[];
extern char D_00180B7D[];
extern char D_00190BE4[];
extern char text_macro_fpc[];
extern char D_001959E4[];
extern char player_entity[];
extern char player_object[];
extern char D_00195AC4[];
extern char D_00195B84[];
extern char window_image[];
extern char D_00195F28[];
extern char spellmaker_spell[];
extern char D_0019626D[];
extern char D_0019626E[];
extern char D_0019626F[];
extern char D_00196272[];
extern char game_mode[];
extern char D_0019629A[];
extern char D_001A9B8C[];
extern char D_001A9B94[];
extern char D_001A9BAC[];
extern char D_001A9BB4[];
extern char potion_cauldron[];
extern char potion_ingredients[];
extern char potion_ingredient_scroll[];
extern char potion_ingredient_count[];
extern char potion_name[];
extern char potion_cauldron_count[];

extern int cast_player_spell(int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int object_delete(int);
extern int object_create_child(int, int, int);
extern int object_new_id(int);
extern int potion_mix_unknown(int);
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern int memchr();
extern void msgbox_show_string(int, int);
extern void item_make(int, int, int);
extern void picklist_open(int);
extern void object_foreach(int, int);
extern void object_foreach_open(int, int);
extern void inv_store_item(int);
int potion_match_recipe(int, int, int, int);
int potion_have_recipe_ingredients(int);
void potion_recipe_list_cb(int);
void potionmaker_add_ingredient(int);
void func_0008FBE8(int);
void potion_make(int);
void potion_load_recipe(int);

void potionmaker_mix(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
L8EED1:;
    if (l_20 >= 64) goto L8EEE4;
    if (*(signed char *)(D_00180B7D + (l_20 * 109)) != 0) goto L8EEE9;
L8EEE4:;
    goto L8F014;
L8EEE9:;
    if (potion_match_recipe(((int)potion_recipes) + (l_20 * 109), (int)D_001A9BB4, (int)D_001A9BAC, (int)&l_1C) == 0) goto L8F009;
    l_18 = object_create_child(*(int *)D_00195AC4, 0, 107);
    *(signed char *)((char *)l_18) = 2;
    *(signed char *)((char *)l_18 + 21) |= 1;
    item_make(1, 1, l_18 + 71);
    *(int *)((char *)l_18 + 107) = (int)(unsigned short)*(short *)(D_00180B42 + (l_20 * 109));
    *(signed char *)((char *)l_18 + 120) = *(signed char *)&l_20;
    inv_store_item(l_18);
    l_18 = object_create_child(l_18, 0, 109);
    *(signed char *)((char *)l_18) = 31;
    mc_memcpy(l_18 + 71, ((int)potion_recipes) + (l_20 * 109), 109, (int)D_00176E94, 69, 4);
    l_20 = 0;
    msgbox_show_string((int)D_00176E9E, 1);
    sound_play(208, *(int *)player_object, 100);
L8EFCF:;
    if (*(int *)(potion_cauldron + (l_20 << 2)) == 0) goto L8F004;
    object_delete(*(int *)(potion_cauldron + (l_20 << 2)));
    *(int *)(potion_cauldron + (l_20++ << 2)) = 0;
    goto L8EFCF;
L8F004:;
    return;
L8F009:;
    l_20++;
    goto L8EED1;
L8F014:;
    if (potion_mix_unknown((int)spellmaker_spell) == 0) return;
    l_18 = object_create_child(*(int *)D_00195AC4, 0, 107);
    *(signed char *)((char *)l_18) = 2;
    *(signed char *)((char *)l_18 + 21) |= 1;
    item_make(1, 1, l_18 + 71);
    *(int *)((char *)l_18 + 107) = 0;
    *(signed char *)((char *)l_18 + 120) = 255;
    inv_store_item(l_18);
    l_18 = object_create_child(l_18, 0, 109);
    *(signed char *)((char *)l_18) = 31;
    mc_memcpy(l_18 + 91, (int)spellmaker_spell, 89, (int)D_00176E94, 94, 4);
    l_20 = 0;
    msgbox_show_string((int)D_00176E9E, 1);
    sound_play(208, *(int *)player_object, 100);
L8F0D6:;
    if (*(int *)(potion_cauldron + (l_20 << 2)) == 0) goto L8F0EB;
    if (l_20 < 8) goto L8F0ED;
L8F0EB:;
    return;
L8F0ED:;
    object_delete(*(int *)(potion_cauldron + (l_20 << 2)));
    *(int *)(potion_cauldron + (l_20++ << 2)) = 0;
    goto L8F0D6;
}

void potion_recipe_list_cb(int a1)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    l_18 = a1 + 71;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 32)) != 27) goto L8F168;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 34)) == 4) goto L8F16A;
L8F168:;
    return;
L8F16A:;
    *(int *)(D_00190BE4 + (*(int *)D_00195B84 << 2)) = l_18;
    *(int *)(text_macro_fpc + ((*(int *)D_00195B84)++ << 2)) = (((int)potion_recipes) + (((int)(unsigned char)*(signed char *)((char *)l_18 + 49)) * 109)) + 67;
}

void potionmaker_recipes(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)potion_recipe_list_cb);
    if (*(int *)D_00195B84 != 0) goto L8F1F6;
    msgbox_show_string((int)D_00176EBA, 1);
    return;
L8F1F6:;
    if (*(int *)D_00195B84 != 1) goto L8F20B;
    potion_make(*(int *)D_00190BE4);
    return;
L8F20B:;
    *(int *)(text_macro_fpc + (*(int *)D_00195B84 << 2)) = 0;
    picklist_open((int)text_macro_fpc);
    sound_play(205, *(int *)player_object, 100);
}

int potionmaker_open(int a1)
{
    if (((int)(unsigned char)*(signed char *)D_0019626F) != 1) goto L8F26F;
    return 1;
L8F26F:;
    if (a1 == 0) goto L8F2F0;
    *(int *)potion_name = (int)D_00176ED0;
    *(signed char *)game_mode = 1;
    *(int *)window_image = disk_read_file((int)D_00176EDF, 0);
    *(short *)potion_cauldron_count = (*(int *)potion_ingredient_scroll = 0);
    *(signed char *)D_00196272 = 1;
    mc_memset((int)potion_cauldron, 0, 32, (int)D_00176E94, 156, 32);
    mc_memset((int)D_001A9BB4, 254, 8, (int)D_00176E94, 157, 8);
L8F2F0:;
    return ((((int)(unsigned char)*(signed char *)game_mode) == 1) ? 1 : 0);
}

void potionmaker_add_ingredient(int a1)
{
    int l_18;
{
    char l_8C[112];

    *(int *)((char *)l_8C + 108) = 0;
L8F8B2:;
    if (*(int *)(potion_cauldron + (*(int *)((char *)l_8C + 108) << 2)) == 0) goto L8F8C9;
    (*(int *)((char *)l_8C + 108))++;
    goto L8F8B2;
L8F8C9:;
    l_18 = (*(int *)(potion_cauldron + (*(int *)((char *)l_8C + 108) << 2)) = *(int *)(potion_ingredients + (a1 << 2)));
    item_make((int)(unsigned short)*(short *)((char *)l_18 + 103), (int)(unsigned short)*(short *)((char *)l_18 + 105), (int)l_8C);
    *(signed char *)(D_001A9BB4 + *(int *)((char *)l_8C + 108)) = *(signed char *)((char *)l_8C + 65);
    *(signed char *)(D_001A9BAC + *(int *)((char *)l_8C + 108)) = *(signed char *)D_0019626D;
    *(signed char *)(D_001A9B8C + *(int *)((char *)l_8C + 108)) = *(signed char *)D_00195F28;
    *(signed char *)(D_001A9B94 + *(int *)((char *)l_8C + 108)) = *(signed char *)D_0019626E;
}
}

int potionmaker_in_cauldron(int a1, int a2)
{
    short l_14;
    short l_18;

    *(int *)&l_14 = 0;
L8F967:;
    if (((int)(short)l_14) < 8) goto L8F97A;
    goto L8F9CE;
L8F972:;
    (*(int *)&l_14)++;
    goto L8F967;
L8F97A:;
    *(int *)&l_18 = *(int *)(potion_cauldron + (((int)(short)l_14) << 2)) + 71;
    if (*(int *)(potion_cauldron + (((int)(short)l_14) << 2)) == 0) goto L8F9AE;
    if (((int)(unsigned short)*(short *)(*(char **)&l_18 + 34)) == a2) goto L8F9B0;
L8F9AE:;
    goto L8F9C1;
L8F9B0:;
    if (((int)(unsigned short)*(short *)(*(char **)&l_18 + 32)) == a1) goto L8F9C3;
L8F9C1:;
    goto L8F9CC;
L8F9C3:;
    return 1;
L8F9CC:;
    goto L8F972;
L8F9CE:;
    return 0;
}

void func_0008FBE8(int a1)
{
    short l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    *(int *)&l_18 = a1 + 71;
    if (((int)(unsigned short)(*(short *)(*(char **)&l_18 + 42) & 1)) == 0) return;
    *(int *)potion_ingredient_count = 1;
}

int func_0008FC3A(void)
{
    *(int *)potion_ingredient_count = 0;
    object_foreach_open(*(int *)(*(char **)D_001959E4 + 63), (int)func_0008FBE8);
    return *(int *)potion_ingredient_count;
}

int potionmaker_close(void)
{
L8FC87:;
    if (*(signed char *)key_down_esc != 0) goto L8FC87;
    *(signed char *)game_mode = 0;
    if (*(int *)window_image == 0) goto L8FCAC;
    if (*(int *)window_image != (-1751672937)) goto L8FCAE;
L8FCAC:;
    goto L8FCCC;
L8FCAE:;
    mc_free(*(int *)window_image, (int)D_00176E94, 359);
    *(int *)window_image = -1751672937;
L8FCCC:;
    *(signed char *)D_00196272 = 0;
    return 1;
}

int potion_match_recipe(int a1, int a2, int a3, int a4)
{
    int l_10;
    int l_24;
    int l_20;
    int l_1C;
    char l_A4[108];
    int l_14;
    char l_38[8];

    l_10 = 0;
    *(int *)((char *)a4) = l_10;
    l_24 = *(int *)((char *)a4);
L8FD15:;
    if (l_10 < 8) goto L8FD25;
    goto L8FD80;
L8FD1D:;
    l_10++;
    goto L8FD15;
L8FD25:;
    if (((int)(signed char)*(signed char *)((char *)(a1 + l_10))) == (-2)) goto L8FD76;
    item_make((int)(unsigned short)(short)*(signed char *)((char *)(a1 + l_10) + 10), (int)(signed char)*(signed char *)((char *)(a1 + l_10)), (int)l_A4);
    *(signed char *)((char *)l_38 + l_10) = *(signed char *)((char *)l_A4 + 65);
    *(int *)((char *)a4) += (int)(unsigned char)*(signed char *)D_0019626D;
    l_24++;
    goto L8FD7E;
L8FD76:;
    *(signed char *)((char *)l_38 + l_10) = 255;
L8FD7E:;
    goto L8FD1D;
L8FD80:;
    l_10 = 0;
    l_20 = l_10;
L8FD8D:;
    if (l_10 < 8) goto L8FD9D;
    goto L8FE01;
L8FD95:;
    l_10++;
    goto L8FD8D;
L8FD9D:;
    if (((int)(unsigned char)*(signed char *)((char *)(a2 + l_10))) == 254) goto L8FD95;
    l_14 = memchr((int)l_38, (int)(unsigned char)*(signed char *)((char *)(a2 + l_10)), 8);
    if (l_14 == 0) goto L8FDEF;
    l_20 += (int)(unsigned char)*(signed char *)((char *)(a3 + l_10));
    *(signed char *)((char *)l_14) = 255;
    l_24--;
    goto L8FDFF;
L8FDEF:;
    l_20 -= (int)(unsigned char)*(signed char *)((char *)(a3 + l_10));
L8FDFF:;
    goto L8FD95;
L8FE01:;
    if (l_24 == 0) goto L8FE10;
    return 0;
L8FE10:;
    *(int *)((char *)a4) = ((l_20 - *(int *)((char *)a4)) >> 1) + 5;
    if (*(int *)((char *)a4) >= 1) goto L8FE37;
    return 0;
L8FE37:;
    return 1;
}

void func_00090261(int a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    int l_C;

    l_10 = 1;
    if (a4 <= 1) return;
    l_C = 1;
L90290:;
    if (l_C == 0) return;
    l_14 = 0;
    l_C = l_14;
L902A7:;
    if ((a4 - l_10) > l_14) goto L902BF;
    goto L9037B;
L902B7:;
    l_14++;
    goto L902A7;
L902BF:;
    if (*(unsigned char *)((char *)(a1 + l_14)) >= *(unsigned char *)((char *)(a1 + l_14) + 1)) goto L90376;
    *(signed char *)((char *)(a1 + l_14)) ^= *(signed char *)((char *)(a1 + l_14) + 1);
    *(signed char *)((char *)(a1 + l_14) + 1) ^= *(signed char *)((char *)(a1 + l_14));
    *(signed char *)((char *)(a1 + l_14)) ^= *(signed char *)((char *)(a1 + l_14) + 1);
    *(signed char *)((char *)(a2 + l_14)) ^= *(signed char *)((char *)(a2 + l_14) + 1);
    *(signed char *)((char *)(a2 + l_14) + 1) ^= *(signed char *)((char *)(a2 + l_14));
    *(signed char *)((char *)(a2 + l_14)) ^= *(signed char *)((char *)(a2 + l_14) + 1);
    *(signed char *)((char *)(a3 + l_14)) ^= *(signed char *)((char *)(a3 + l_14) + 1);
    *(signed char *)((char *)(a3 + l_14) + 1) ^= *(signed char *)((char *)(a3 + l_14));
    *(signed char *)((char *)(a3 + l_14)) ^= *(signed char *)((char *)(a3 + l_14) + 1);
    l_C = 1;
L90376:;
    goto L902B7;
L9037B:;
    l_10++;
    goto L90290;
}

void potion_drink(int a1)
{
    int l_1C;
    int l_18;

    l_1C = object_create_child(*(int *)D_00195AC4, 0, 89);
    l_18 = (int)(unsigned char)*(signed char *)D_0019629A;
    *(signed char *)((char *)l_1C) = 9;
    *(signed char *)((char *)l_1C + 21) |= 1;
    *(int *)((char *)l_1C + 31) = object_new_id(801);
    mc_memcpy(l_1C + 71, a1 + 91, 89, (int)D_00176E94, 540, 4);
    *(signed char *)D_0019629A = 1;
    cast_player_spell(l_1C);
    *(signed char *)D_0019629A = *(signed char *)&l_18;
    object_delete(l_1C);
}

void potion_make(int a1)
{
    *(signed char *)D_0012B508 = 146;
    if (potion_have_recipe_ingredients(a1) == 0) goto L90454;
    potion_load_recipe(a1);
    return;
L90454:;
    msgbox_show_string((int)D_00176EFB, 1);
}

int potion_have_recipe_ingredients(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    mc_memset((int)potion_cauldron, 0, 32, (int)D_00176E94, 566, 32);
    l_1C = ((int)potion_recipes) + (((int)(unsigned char)*(signed char *)((char *)a1 + 49)) * 109);
    *(int *)potion_name = l_1C + 67;
L904C5:;
    if (((int)(signed char)*(signed char *)((char *)(l_1C + l_24))) == (-2)) goto L9055E;
    l_28 = 0;
    l_20 = l_28;
L904E4:;
    if (l_28 < *(int *)potion_ingredient_count) goto L904F9;
    goto L90544;
L904F1:;
    l_28++;
    goto L904E4;
L904F9:;
    if (*(unsigned short *)(*(char **)(potion_ingredients + (l_28 << 2)) + 103) != *(signed char *)((char *)(l_1C + l_24) + 10)) goto L90538;
    if (*(unsigned short *)(*(char **)(potion_ingredients + (l_28 << 2)) + 105) == *(signed char *)((char *)(l_1C + l_24))) goto L9053A;
L90538:;
    goto L90542;
L9053A:;
    l_20++;
    goto L90544;
L90542:;
    goto L904F1;
L90544:;
    if (l_20 != 0) goto L90553;
    return 0;
L90553:;
    l_24++;
    goto L904C5;
L9055E:;
    return 1;
}

void potion_load_recipe(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = ((int)potion_recipes) + (((int)(unsigned char)*(signed char *)((char *)a1 + 49)) * 109);
    *(int *)potion_name = l_18 + 67;
L905AD:;
    if (((int)(signed char)*(signed char *)((char *)(l_18 + l_1C))) == (-2)) return;
    l_20 = 0;
L905C6:;
    if (l_20 < *(int *)potion_ingredient_count) goto L905DB;
    goto L90628;
L905D3:;
    l_20++;
    goto L905C6;
L905DB:;
    if (*(unsigned short *)(*(char **)(potion_ingredients + (l_20 << 2)) + 103) != *(signed char *)((char *)(l_18 + l_1C) + 10)) goto L9061A;
    if (*(unsigned short *)(*(char **)(potion_ingredients + (l_20 << 2)) + 105) == *(signed char *)((char *)(l_18 + l_1C))) goto L9061C;
L9061A:;
    goto L90626;
L9061C:;
    potionmaker_add_ingredient(l_20);
    goto L90628;
L90626:;
    goto L905D3;
L90628:;
    l_1C++;
    goto L905AD;
}
