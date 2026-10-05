/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005DE74 */
#include "records.h"

#pragma pack(1)
struct itemdef {
    char name[24];
    int f24;                    /* 24 */
    short f28;                  /* 28 */
    int f30;                    /* 30 */
    int f34;                    /* 34 */
    short f38;                  /* 38 */
    unsigned char f40;          /* 40 */
    unsigned char f41;          /* 41 */
    unsigned char f42;          /* 42 */
    unsigned char f43;          /* 43 */
    short f44;                  /* 44 */
    unsigned short f46;         /* 46 */
};
#pragma pack()
extern char D_001758B8[];
extern char D_001758C0[];
extern char D_001758C1[];
extern struct itemdef item_templates[];
extern unsigned char D_00190CF2;
extern char D_001911E4[];
extern struct character *player_character;
extern short D_00195F28;
extern unsigned char D_0019626D;
extern unsigned char D_0019626E;
extern void fatal_error(char *);
extern void func_0005E5D7(struct item *, int);
extern void func_0005E636(struct item *);
extern void func_0005E874(struct item *);
extern void func_0005EA8F(struct item *);
extern void item_init_book(struct item *, short);
extern void item_make_magic(struct item *, int);
extern int rand_range(int, int);
extern int rand(void);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
#pragma aux func_000A18C3 parm routine [];
extern int func_000A18C3(char *);
extern int mc_sprintf(char *, char *, ...);

#define PFLAGS (player_character->flags)

void item_init_from_template(unsigned short idx, short type, short sub, struct item *it)
{
    struct itemdef *def;
    unsigned short orig;

    orig = idx;
    if (type == 13) {
        sub = 0;
        idx = 284;
    }
    if (type == 6 && ((unsigned short)PFLAGS & 1) != 0)
        type = 12;
    if (type == 12 && ((unsigned short)PFLAGS & 1) == 0)
        type = 6;
    if (type == 4) {
        item_make_magic(it, -1);
        return;
    }
    if (type == 9 && (sub < 2 || sub == 4))
        sub = 2;
    if (type == 10 && sub == 11)
        sub = 10;
    if (type == 7) {
        D_00190CF2++;
        idx = 277;
        if (sub > 3)
            sub = rand_range(0, 3);
    }
    if (idx >= 288) {
        func_000A0ED9(58, D_001758B8);
        func_000A18C3(D_001758C0);
        func_000A0ED9(59, D_001758B8);
        mc_sprintf(D_001911E4, D_001758C1, orig, idx);
        fatal_error(D_001911E4);
    }
    def = &item_templates[idx];
    if (def->f46 == 32512)
        it->index = 0;
    mc_strncpy(it->name, def->name, 32, D_001758B8, 68);
    it->group = type;
    it->index = sub;
    it->value = def->f34;
    if (def->f30 != 0 && (def->f43 & 1) != 0) {
        D_0019626E = def->f30;
        *(short *)it->pad28 = 0;
    } else {
        D_0019626E = 0;
        *(short *)it->pad28 = def->f30;
    }
    it->item_flags = (unsigned short)def->f43;
    it->condition = it->max_condition = def->f28;
    it->magicka_bonus = 0;
    if (def->f46 != 0 && def->f44 == 0)
        it->dropped_image = def->f46;
    if (def->f44 != 0 && def->f46 == 0)
        it->inventory_image = def->f44;
    if (def->f46 != 0)
        it->inventory_image = def->f46;
    if (def->f44 != 0)
        it->dropped_image = def->f44;
    if (((unsigned short)it->inventory_image & -128) == 31360 && ((unsigned short)PFLAGS & 1) == 0) {
        it->inventory_image &= 127;
        it->inventory_image |= 31872;
    }
    it->material = it->armor_type = 0;
    if (type == 1 && (sub == 4 || sub == 5)) {
        if ((rand() & 3) != 0) {
            if (sub == 4)
                it->color = (rand() & 1) + 24;
            else
                it->color = (rand() & 1) + 26;
        }
    } else {
        it->color = 18;
    }
    it->weight = def->f24;
    it->enchant_points = def->f38;
    it->variants = def->f41;
    it->draw_order = def->f42;
    mc_memset(it->enchantments, -1, 40, D_001758B8, 118, 40);
    D_0019626D = def->f40;
    D_00195F28 = def->f42;
    if (type == 27 && sub == 4)
        it->stack_count = rand() % 20;
    if (type == 6 || type == 12 || type == 2) {
        func_0005E636(it);
        func_0005E5D7(it, player_character->race);
    }
    if (type == 3)
        func_0005E874(it);
    if (type == 2) {
        func_0005EA8F(it);
        if (it->index != 5 && it->index < 7 && it->material == 2)
            func_0005E874(it);
    }
    if (type == 3 && sub == 18) {
        it->stack_count = rand_range(1, 20);
        it->condition = 0;
    }
    if (type == 7)
        item_init_book(it, sub);
    if (type == 13)
        it->message = rand();
}
