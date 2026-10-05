/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008F9E1 */
#include "records.h"

extern char potion_cauldron[];
extern char potion_ingredients[];
extern char potion_ingredient_scroll[];
extern char potion_ingredient_count[];
extern void text_draw_centered_colored(char *, short, short, int, unsigned char);
extern void func_000CD1C5(int, int, int, int, char *);
extern char *func_00135D00(int, int, int);

void potionmaker_ingredient_cb(struct record *a1)
{
    struct item *p;
    char *spr;
    short n;
    short unused;

    if (a1->type != 2) return;
    for (n = 0; n < 8; n++)
        if (((struct record **)potion_cauldron)[n] == a1) return;
    p = &a1->data.item;
    if (p->enchantments[0].type != -1) return;
    if ((p->item_flags & 1) == 0) return;
    if (*(int *)potion_ingredient_count >= 512) return;
    ((struct record **)potion_ingredients)[*(int *)potion_ingredient_count] = a1;
    n = *(short *)potion_ingredient_count - *(short *)potion_ingredient_scroll;
    if (*(int *)potion_ingredient_count < *(int *)potion_ingredient_scroll) {
        (*(int *)potion_ingredient_count)++;
        return;
    }
    if (n < 12) {
        spr = *(char **)(func_00135D00(p->inventory_image >> 7, p->inventory_image & 127, -1) + 12);
        func_000CD1C5(n % 3 * 56 + 28 - (*(unsigned short *)(spr + 4) >> 1), n / 3 * 38 + 42 - (*(unsigned short *)(spr + 6) >> 1), *(unsigned short *)(spr + 4), *(unsigned short *)(spr + 6), spr + *(int *)(spr + 14));
        text_draw_centered_colored(p->name, n % 3 * 56 + 30, n / 3 * 38 + 58 + (n % 3 == 1 ? 5 : 0), 145, 156);
    }
    (*(int *)potion_ingredient_count)++;
}
