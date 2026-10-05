/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002E2CF */
struct eff { short type; short val; };
struct spell { char pad0[67]; struct eff eff[10]; };
struct mobile {
    char pad0[32];
    short f32;                  /* 0x20 */
    char pad22[107];
    short f141;                 /* 0x8d */
    short f143;                 /* 0x8f */
};
struct thing { unsigned char type; char pad[70]; struct mobile mob; };
extern int D_001959FC;
extern int D_00195A08;
extern int D_00195A0C;
extern int D_00195A78;
extern int D_00195AA8;
extern struct mobile *player_character;
extern int game_minutes;
extern char D_00196291;
extern char D_00196292;
extern int func_0002E8AE(struct mobile *, int);
extern int cast_item_strike_spell(short, struct thing *);
extern int cast_creature_spell(struct thing *, struct thing *, int);
extern void item_damage(int, int);
extern int rand_range(int, int);

void damage_weapon_strike_effects(struct spell *a1, struct thing *a2, struct thing *a3, int a4)
{
    int i;
    struct mobile *m2;
    struct mobile *m3;

    if (a1->eff[0].type == -1)
        return;
    m2 = &a2->mob;
    m3 = &a3->mob;
    for (i = 0; i < 10; i++) {
        if (a1->eff[i].type == -1)
            return;
        if (a1->eff[i].type == 2) {
            D_00196292 = 1;
            D_00196291 = 1;
            if (m2 == player_character) {
                cast_item_strike_spell(a1->eff[i].val, a3);
                item_damage(D_00195AA8, 10);
            } else {
                cast_creature_spell(a2, a3, a1->eff[i].val);
                item_damage(D_00195AA8, 10);
            }
            D_00196291 = 0;
            D_00196292 = 0;
        } else if (a1->eff[i].type == 6 && a1->eff[i].val == 1) {
            item_damage(D_00195AA8, func_0002E8AE(m2, a4 / 2) / 4 + 1);
        } else if (a1->eff[i].type == 26 && a1->eff[i].val == 2) {
            item_damage(D_00195AA8, 2);
            i = rand_range(1, 6);
            if (m3->f141 > 10) {
                m3->f141 -= i;
                if (m3->f141 < 0)
                    m3->f141 = 0;
                D_001959FC += i;
                i = D_001959FC + player_character->f141;
                if (player_character->f143 < i)
                    D_001959FC = player_character->f143 - player_character->f141;
                if (D_00195A0C == 0)
                    D_00195A0C = game_minutes + 12;
            } else if (m3->f32 > 10) {
                m3->f32 -= i;
                if (m3->f32 < 0)
                    m3->f32 = 0;
                D_00195A08 += i;
                D_00195A78 = game_minutes + 12;
            }
        }
    }
}
