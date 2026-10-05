/* potions.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char D_0012B508;
extern signed char key_down_esc;
extern char D_00176E94[];
extern char D_00176E9E[];
extern char D_00176EBA[];
extern char D_00176ED0[];
extern char D_00176EDF[];
extern char D_00176EFB[];
extern char potion_recipes[];
extern char D_00180B42[];
extern signed char D_00180B7D[];
extern char scratch_190be4[];
extern char scratch_190de4[];
extern struct record *D_001959E4;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern char D_00195B84[];
extern int window_image;
extern char D_00195F28[];
extern char spellmaker_spell[];
extern signed char D_0019626D;
extern signed char D_0019626E;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char D_0019629A;
extern signed char D_001A9B8C[];
extern signed char D_001A9B94[];
extern signed char D_001A9BAC[];
extern signed char D_001A9BB4[];
extern struct record *potion_cauldron[];
extern struct record *potion_ingredients[];
extern char potion_ingredient_scroll[];
extern char potion_ingredient_count[];
extern int potion_name;
extern short potion_cauldron_count;

extern int cast_player_spell(struct record *);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int potion_mix_unknown(int);
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern int memchr();
extern void msgbox_show_string(int, int);
extern void item_make(int, int, struct item *);
extern void list_popup_open(int);
extern void object_foreach(struct record *, int);
extern void object_foreach_open(struct record *, int);
extern void inv_store_item(struct record *);
int potion_match_recipe(struct potion_recipe *, int, int, int);
int potion_have_recipe_ingredients(struct item *);
void potion_recipe_list_cb(struct record *);
void potionmaker_add_ingredient(int);
void potion_ingredient_cb(struct record *);
void potion_make(struct item *);
void potion_load_recipe(struct item *);

void potionmaker_mix(void)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    l_20 = 0;
    while (l_20 < 64 && D_00180B7D[l_20 * 109] != 0) {
        if (potion_match_recipe((struct potion_recipe *)(((int)potion_recipes) + (l_20 * 109)), (int)D_001A9BB4, (int)D_001A9BAC, (int)&l_1C) != 0) {
            l_18 = object_create_child(location_object, 0, 107);
            l_18->type = 2;
            l_18->flags |= 1;
            item_make(1, 1, &l_18->data.item);
            l_18->data.item.value = (int)(unsigned short)*(short *)(D_00180B42 + (l_20 * 109));
            l_18->data.item.stack_count = *(signed char *)&l_20;
            inv_store_item(l_18);
            l_18 = object_create_child(l_18, 0, 109);
            l_18->type = 31;
            mc_memcpy(&l_18->data.potion_recipe, ((int)potion_recipes) + (l_20 * 109), 109, (int)D_00176E94, 69, 4);
            l_20 = 0;
            msgbox_show_string((int)D_00176E9E, 1);
            sound_play(208, (int)player_object, 100);
            while ((int)potion_cauldron[l_20] != 0) {
                object_delete(potion_cauldron[l_20]);
                potion_cauldron[l_20++] = 0;
            }
            return;
        }
        l_20++;
    }
    if (potion_mix_unknown((int)spellmaker_spell) == 0) return;
    l_18 = object_create_child(location_object, 0, 107);
    l_18->type = 2;
    l_18->flags |= 1;
    item_make(1, 1, &l_18->data.item);
    l_18->data.item.value = 0;
    l_18->data.item.stack_count = 255;
    inv_store_item(l_18);
    l_18 = object_create_child(l_18, 0, 109);
    l_18->type = 31;
    mc_memcpy(&l_18->data.potion_recipe.spell, (int)spellmaker_spell, 89, (int)D_00176E94, 94, 4);
    l_20 = 0;
    msgbox_show_string((int)D_00176E9E, 1);
    sound_play(208, (int)player_object, 100);
    while ((int)potion_cauldron[l_20] != 0 && l_20 < 8) {
        object_delete(potion_cauldron[l_20]);
        potion_cauldron[l_20++] = 0;
    }
}

void potion_recipe_list_cb(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group != 27 || l_18->index != 4) return;
    *(int *)(scratch_190be4 + (*(int *)D_00195B84 << 2)) = (int)l_18;
    *(int *)(scratch_190de4 + ((*(int *)D_00195B84)++ << 2)) = (((int)potion_recipes) + (l_18->stack_count * 109)) + 67;
}

void potionmaker_recipes(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)potion_recipe_list_cb);
    if (*(int *)D_00195B84 == 0) {
        msgbox_show_string((int)D_00176EBA, 1);
        return;
    }
    if (*(int *)D_00195B84 == 1) {
        potion_make(*(struct item **)scratch_190be4);
        return;
    }
    *(int *)(scratch_190de4 + (*(int *)D_00195B84 << 2)) = 0;
    list_popup_open((int)scratch_190de4);
    sound_play(205, (int)player_object, 100);
}

int potionmaker_open(int a1)
{
    if (((int)D_0019626F) == 1) return 1;
    if (a1 != 0) {
        potion_name = (int)D_00176ED0;
        game_mode = 1;
        window_image = disk_read_file((int)D_00176EDF, 0);
        potion_cauldron_count = (*(int *)potion_ingredient_scroll = 0);
        D_00196272 = 1;
        mc_memset((int)((char *)potion_cauldron), 0, 32, (int)D_00176E94, 156, 32);
        mc_memset((int)D_001A9BB4, 254, 8, (int)D_00176E94, 157, 8);
    }
    return ((((int)(unsigned char)game_mode) == 1) ? 1 : 0);
}

void potionmaker_add_ingredient(int a1)
{
    struct record *l_18;
    {
        char l_8C[112];

        *(int *)((char *)l_8C + 108) = 0;
        while ((int)potion_cauldron[*(int *)((char *)l_8C + 108)] != 0) {
            (*(int *)((char *)l_8C + 108))++;
        }
        l_18 = (struct record *)(*(int *)((char *)potion_cauldron + (*(int *)((char *)l_8C + 108) << 2)) = (int)potion_ingredients[a1]);
        item_make(l_18->data.item.group, l_18->data.item.index, (struct item *)l_8C);
        D_001A9BB4[*(int *)((char *)l_8C + 108)] = *(signed char *)((char *)l_8C + 65);
        D_001A9BAC[*(int *)((char *)l_8C + 108)] = D_0019626D;
        D_001A9B8C[*(int *)((char *)l_8C + 108)] = *(signed char *)D_00195F28;
        D_001A9B94[*(int *)((char *)l_8C + 108)] = D_0019626E;
    }
}

int potionmaker_in_cauldron(int a1, int a2)
{
    short l_14;
    short l_18;

    *(int *)&l_14 = 0;
    for (; ((int)(short)l_14) < 8; (*(int *)&l_14)++) {
        *(int *)&l_18 = (int)potion_cauldron[(int)(short)l_14] + 71;
        if ((int)potion_cauldron[(int)(short)l_14] != 0 && ((int)(unsigned short)*(short *)(*(char **)&l_18 + 34)) == a2 && ((int)(unsigned short)*(short *)(*(char **)&l_18 + 32)) == a1) {
            return 1;
        }
    }
    return 0;
}

void potion_ingredient_cb(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (((int)(unsigned short)(l_18->item_flags & 1)) == 0) return;
    *(int *)potion_ingredient_count = 1;
}

int potion_have_ingredients(void)
{
    *(int *)potion_ingredient_count = 0;
    object_foreach_open(D_001959E4->children, (int)potion_ingredient_cb);
    return *(int *)potion_ingredient_count;
}

int potionmaker_close(void)
{
    while (key_down_esc != 0);
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00176E94, 359);
        window_image = -1751672937;
    }
    D_00196272 = 0;
    return 1;
}

int potion_match_recipe(struct potion_recipe *a1, int a2, int a3, int a4)
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
    for (; l_10 < 8; l_10++) {
        if (a1->ingredient_indices[l_10] != (-2)) {
            item_make((int)(unsigned short)(short)a1->ingredient_groups[l_10], (int)(signed char)a1->ingredient_indices[l_10], (struct item *)l_A4);
            *(signed char *)((char *)l_38 + l_10) = *(signed char *)((char *)l_A4 + 65);
            *(int *)((char *)a4) += (int)(unsigned char)D_0019626D;
            l_24++;
        } else {
            *(signed char *)((char *)l_38 + l_10) = 255;
        }
    }
    l_10 = 0;
    l_20 = l_10;
    for (; l_10 < 8; l_10++) {
        if (((int)(unsigned char)*(signed char *)((char *)(a2 + l_10))) == 254) continue;
        l_14 = memchr((int)l_38, (int)(unsigned char)*(signed char *)((char *)(a2 + l_10)), 8);
        if (l_14 != 0) {
            l_20 += (int)(unsigned char)*(signed char *)((char *)(a3 + l_10));
            *(signed char *)((char *)l_14) = 255;
            l_24--;
        } else {
            l_20 -= (int)(unsigned char)*(signed char *)((char *)(a3 + l_10));
        }
    }
    if (l_24 != 0) return 0;
    *(int *)((char *)a4) = ((l_20 - *(int *)((char *)a4)) >> 1) + 5;
    if (*(int *)((char *)a4) < 1) return 0;
    return 1;
}

void potion_sort_ingredients(int a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    int l_C;

    l_10 = 1;
    if (a4 <= 1) return;
    l_C = 1;
    while (l_C != 0) {
        l_14 = 0;
        l_C = l_14;
        for (; (a4 - l_10) > l_14; l_14++) {
            if (*(unsigned char *)((char *)(a1 + l_14)) < *(unsigned char *)((char *)(a1 + l_14) + 1)) {
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
            }
        }
        l_10++;
    }
}

void potion_drink(struct record *a1)
{
    struct record *l_1C;
    int l_18;

    l_1C = object_create_child(location_object, 0, 89);
    l_18 = (int)(unsigned char)D_0019629A;
    l_1C->type = 9;
    l_1C->flags |= 1;
    l_1C->id = object_new_id(801);
    mc_memcpy(&l_1C->data.spell, &a1->data.potion_recipe.spell, 89, (int)D_00176E94, 540, 4);
    D_0019629A = 1;
    cast_player_spell(l_1C);
    D_0019629A = *(signed char *)&l_18;
    object_delete(l_1C);
}

void potion_make(struct item *a1)
{
    D_0012B508 = 146;
    if (potion_have_recipe_ingredients(a1) != 0) {
        potion_load_recipe(a1);
        return;
    }
    msgbox_show_string((int)D_00176EFB, 1);
}

int potion_have_recipe_ingredients(struct item *a1)
{
    int l_28;
    int l_24;
    int l_20;
    struct potion_recipe *l_1C;

    l_24 = 0;
    mc_memset((int)((char *)potion_cauldron), 0, 32, (int)D_00176E94, 566, 32);
    l_1C = (struct potion_recipe *)(((int)potion_recipes) + (a1->stack_count * 109));
    potion_name = (int)l_1C->spell.name;
    while (l_1C->ingredient_indices[l_24] != (-2)) {
        l_28 = 0;
        l_20 = l_28;
        for (; l_28 < *(int *)potion_ingredient_count; l_28++) {
            if (potion_ingredients[l_28]->data.item.group == l_1C->ingredient_groups[l_24] && potion_ingredients[l_28]->data.item.index == l_1C->ingredient_indices[l_24]) {
                l_20++;
                break;
            }
        }
        if (l_20 == 0) return 0;
        l_24++;
    }
    return 1;
}

void potion_load_recipe(struct item *a1)
{
    int l_20;
    int l_1C;
    struct potion_recipe *l_18;

    l_1C = 0;
    l_18 = (struct potion_recipe *)(((int)potion_recipes) + (a1->stack_count * 109));
    potion_name = (int)l_18->spell.name;
    while (l_18->ingredient_indices[l_1C] != (-2)) {
        for (l_20 = 0; l_20 < *(int *)potion_ingredient_count; l_20++) {
            if (potion_ingredients[l_20]->data.item.group == l_18->ingredient_groups[l_1C] && potion_ingredients[l_20]->data.item.index == l_18->ingredient_indices[l_1C]) {
                potionmaker_add_ingredient(l_20);
                break;
            }
        }
        l_1C++;
    }
}
