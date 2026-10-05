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
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern int potion_mix_unknown(int);
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern int memchr();
extern void msgbox_show_string(char *, short);
extern void item_make(int, int, struct item *);
extern void list_popup_open(int);
extern void object_foreach(struct record *, void (*)());
extern void object_foreach_open(struct record *, void (*)());
extern void inv_store_item(struct record *);
int potion_match_recipe(struct potion_recipe *, signed char *, signed char *, int *);
int potion_have_recipe_ingredients(struct item *);
void potion_recipe_list_cb(struct record *);
void potionmaker_add_ingredient(int);
void potion_ingredient_cb(struct record *);
void potion_make(struct item *);
void potion_load_recipe(struct item *);

void potionmaker_mix(void)
{
    int i;
    int value;
    struct record *potion;

    i = 0;
    while (i < 64 && D_00180B7D[i * 109] != 0) {
        if (potion_match_recipe((struct potion_recipe *)(((int)potion_recipes) + (i * 109)), D_001A9BB4, D_001A9BAC, &value) != 0) {
            potion = object_create_child(location_object, 0, 107);
            potion->type = 2;
            potion->flags |= 1;
            item_make(1, 1, &potion->data.item);
            potion->data.item.value = (int)(unsigned short)*(short *)(D_00180B42 + (i * 109));
            potion->data.item.stack_count = *(signed char *)&i;
            inv_store_item(potion);
            potion = object_create_child(potion, 0, 109);
            potion->type = 31;
            mc_memcpy(&potion->data.potion_recipe, ((int)potion_recipes) + (i * 109), 109, (int)D_00176E94, 69, 4);
            i = 0;
            msgbox_show_string(D_00176E9E, 1);
            sound_play(208, player_object, 100);
            while ((int)potion_cauldron[i] != 0) {
                object_delete(potion_cauldron[i]);
                potion_cauldron[i++] = 0;
            }
            return;
        }
        i++;
    }
    if (potion_mix_unknown((int)spellmaker_spell) == 0) return;
    potion = object_create_child(location_object, 0, 107);
    potion->type = 2;
    potion->flags |= 1;
    item_make(1, 1, &potion->data.item);
    potion->data.item.value = 0;
    potion->data.item.stack_count = 255;
    inv_store_item(potion);
    potion = object_create_child(potion, 0, 109);
    potion->type = 31;
    mc_memcpy(&potion->data.potion_recipe.spell, (int)spellmaker_spell, 89, (int)D_00176E94, 94, 4);
    i = 0;
    msgbox_show_string(D_00176E9E, 1);
    sound_play(208, player_object, 100);
    while ((int)potion_cauldron[i] != 0 && i < 8) {
        object_delete(potion_cauldron[i]);
        potion_cauldron[i++] = 0;
    }
}

void potion_recipe_list_cb(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (item->group != 27 || item->index != 4) return;
    *(int *)(scratch_190be4 + (*(int *)D_00195B84 << 2)) = (int)item;
    *(int *)(scratch_190de4 + ((*(int *)D_00195B84)++ << 2)) = (((int)potion_recipes) + (item->stack_count * 109)) + 67;
}

void potionmaker_recipes(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, potion_recipe_list_cb);
    if (*(int *)D_00195B84 == 0) {
        msgbox_show_string(D_00176EBA, 1);
        return;
    }
    if (*(int *)D_00195B84 == 1) {
        potion_make(*(struct item **)scratch_190be4);
        return;
    }
    *(int *)(scratch_190de4 + (*(int *)D_00195B84 << 2)) = 0;
    list_popup_open((int)scratch_190de4);
    sound_play(205, player_object, 100);
}

int potionmaker_open(int show)
{
    if (((int)D_0019626F) == 1) return 1;
    if (show != 0) {
        potion_name = (int)D_00176ED0;
        game_mode = 1;
        window_image = disk_read_file(D_00176EDF, 0);
        potion_cauldron_count = (*(int *)potion_ingredient_scroll = 0);
        D_00196272 = 1;
        mc_memset((int)((char *)potion_cauldron), 0, 32, (int)D_00176E94, 156, 32);
        mc_memset((int)D_001A9BB4, 254, 8, (int)D_00176E94, 157, 8);
    }
    return ((((int)(unsigned char)game_mode) == 1) ? 1 : 0);
}

void potionmaker_add_ingredient(int index)
{
    struct record *ingredient;
    {
        char item_buf[112];

        *(int *)((char *)item_buf + 108) = 0;
        while ((int)potion_cauldron[*(int *)((char *)item_buf + 108)] != 0) {
            (*(int *)((char *)item_buf + 108))++;
        }
        ingredient = (struct record *)(*(int *)((char *)potion_cauldron + (*(int *)((char *)item_buf + 108) << 2)) = (int)potion_ingredients[index]);
        item_make(ingredient->data.item.group, ingredient->data.item.index, (struct item *)item_buf);
        D_001A9BB4[*(int *)((char *)item_buf + 108)] = *(signed char *)((char *)item_buf + 65);
        D_001A9BAC[*(int *)((char *)item_buf + 108)] = D_0019626D;
        D_001A9B8C[*(int *)((char *)item_buf + 108)] = *(signed char *)D_00195F28;
        D_001A9B94[*(int *)((char *)item_buf + 108)] = D_0019626E;
    }
}

int potionmaker_in_cauldron(int group, int index)
{
    short i;
    struct item *item;

    *(int *)&i = 0;
    for (; ((int)(short)i) < 8; (*(int *)&i)++) {
        item = &potion_cauldron[(int)(short)i]->data.item;
        if ((int)potion_cauldron[(int)(short)i] != 0 && item->index == index && item->group == group) {
            return 1;
        }
    }
    return 0;
}

void potion_ingredient_cb(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (((int)(unsigned short)(item->item_flags & 1)) == 0) return;
    *(int *)potion_ingredient_count = 1;
}

int potion_have_ingredients(void)
{
    *(int *)potion_ingredient_count = 0;
    object_foreach_open(D_001959E4->children, potion_ingredient_cb);
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

int potion_match_recipe(struct potion_recipe *recipe, signed char *cauldron_ids, signed char *potencies, int *value_out)
{
    int i;
    int unmatched;
    int potency;
    int unused;
    char item_buf[108];
    int found;
    char recipe_ids[8];

    i = 0;
    *value_out = i;
    unmatched = *value_out;
    for (; i < 8; i++) {
        if (recipe->ingredient_indices[i] != (-2)) {
            item_make((int)(unsigned short)(short)recipe->ingredient_groups[i], (int)(signed char)recipe->ingredient_indices[i], (struct item *)item_buf);
            *(signed char *)((char *)recipe_ids + i) = *(signed char *)((char *)item_buf + 65);
            *value_out += (int)(unsigned char)D_0019626D;
            unmatched++;
        } else {
            *(signed char *)((char *)recipe_ids + i) = 255;
        }
    }
    i = 0;
    potency = i;
    for (; i < 8; i++) {
        if (((int)(unsigned char)cauldron_ids[i]) == 254) continue;
        found = memchr((int)recipe_ids, (int)(unsigned char)cauldron_ids[i], 8);
        if (found != 0) {
            potency += (int)(unsigned char)potencies[i];
            *(signed char *)((char *)found) = 255;
            unmatched--;
        } else {
            potency -= (int)(unsigned char)potencies[i];
        }
    }
    if (unmatched != 0) return 0;
    *value_out = ((potency - *value_out) >> 1) + 5;
    if (*value_out < 1) return 0;
    return 1;
}

void potion_sort_ingredients(unsigned char *keys, unsigned char *second, unsigned char *third, int count)
{
    int i;
    int pass;
    int swapped;

    pass = 1;
    if (count <= 1) return;
    swapped = 1;
    while (swapped != 0) {
        i = 0;
        swapped = i;
        for (; (count - pass) > i; i++) {
            if (keys[i] < keys[i + 1]) {
                keys[i] ^= keys[i + 1];
                keys[i + 1] ^= keys[i];
                keys[i] ^= keys[i + 1];
                second[i] ^= second[i + 1];
                second[i + 1] ^= second[i];
                second[i] ^= second[i + 1];
                third[i] ^= third[i + 1];
                third[i + 1] ^= third[i];
                third[i] ^= third[i + 1];
                swapped = 1;
            }
        }
        pass++;
    }
}

void potion_drink(struct record *recipe)
{
    struct record *spell;
    int saved_ignore_silence;

    spell = object_create_child(location_object, 0, 89);
    saved_ignore_silence = (int)(unsigned char)D_0019629A;
    spell->type = 9;
    spell->flags |= 1;
    spell->id = object_new_id(801);
    mc_memcpy(&spell->data.spell, &recipe->data.potion_recipe.spell, 89, (int)D_00176E94, 540, 4);
    D_0019629A = 1;
    cast_player_spell(spell);
    D_0019629A = *(signed char *)&saved_ignore_silence;
    object_delete(spell);
}

void potion_make(struct item *recipe_item)
{
    D_0012B508 = 146;
    if (potion_have_recipe_ingredients(recipe_item) != 0) {
        potion_load_recipe(recipe_item);
        return;
    }
    msgbox_show_string(D_00176EFB, 1);
}

int potion_have_recipe_ingredients(struct item *recipe_item)
{
    int i;
    int n;
    int found;
    struct potion_recipe *recipe;

    n = 0;
    mc_memset((int)((char *)potion_cauldron), 0, 32, (int)D_00176E94, 566, 32);
    recipe = (struct potion_recipe *)(((int)potion_recipes) + (recipe_item->stack_count * 109));
    potion_name = (int)recipe->spell.name;
    while (recipe->ingredient_indices[n] != (-2)) {
        i = 0;
        found = i;
        for (; i < *(int *)potion_ingredient_count; i++) {
            if (potion_ingredients[i]->data.item.group == recipe->ingredient_groups[n] && potion_ingredients[i]->data.item.index == recipe->ingredient_indices[n]) {
                found++;
                break;
            }
        }
        if (found == 0) return 0;
        n++;
    }
    return 1;
}

void potion_load_recipe(struct item *recipe_item)
{
    int i;
    int n;
    struct potion_recipe *recipe;

    n = 0;
    recipe = (struct potion_recipe *)(((int)potion_recipes) + (recipe_item->stack_count * 109));
    potion_name = (int)recipe->spell.name;
    while (recipe->ingredient_indices[n] != (-2)) {
        for (i = 0; i < *(int *)potion_ingredient_count; i++) {
            if (potion_ingredients[i]->data.item.group == recipe->ingredient_groups[n] && potion_ingredients[i]->data.item.index == recipe->ingredient_indices[n]) {
                potionmaker_add_ingredient(i);
                break;
            }
        }
        n++;
    }
}
