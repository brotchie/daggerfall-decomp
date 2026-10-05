/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x00070496 to 0x00070887, kept together for its switch table's alignment */
struct mobile { unsigned char f0; char f1; int f2; };
struct thing {
    unsigned char type;
    char pad1[54];
    struct thing *next;         /* 55 */
    char pad3b[4];
    struct thing *child;        /* 63 */
    char pad43[4];
    struct mobile mob;          /* 71 */
};
struct cur { unsigned char f0; char f1; unsigned char f2; short f3; int f5; };
extern char D_00175EAA[];        /* __FILE__ */
extern char D_00176089[];
extern char D_00176096[];
extern char trade_price_scale[];
extern char player_environment[];
extern struct thing *player_entity;
extern char player_character[];
extern int game_minutes;
extern char D_00195D2C[];
extern char D_00195D30[];
extern char D_001960D9[];
extern char D_001961F5[];
extern struct cur *guild_membership;
extern void msgbox_show_rsc(short, int);
extern void func_0005F401(int);
extern int blessing_apply(struct mobile *, int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern void spfx_cure_disease(int, int);
extern void object_free_children(int);
extern struct thing *object_create_child(struct thing *, int, int);
extern void inventory_open_container(int, int, int);
extern int trade_adjust_price(int, int);
extern void func_00097A85(void);
extern int func_00097B2A(void);
extern int func_00097BD9(int);
extern int travel_map_open(int);
extern int func_000A0AD9();
void guild_add_membership(int, unsigned char);
int guild_confirm_price(int);

void guild_check_invitations(void)
{
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 546)) == 100) goto L704C7;
    if (*(int *)(*(char **)player_character + 529) != 0) goto L704C9;
L704C7:;
    goto L704DC;
L704C9:;
    if (((unsigned)*(int *)(*(char **)player_character + 529)) < game_minutes) goto L704DE;
L704DC:;
    goto L704EA;
L704DE:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 1) goto L704EC;
L704EA:;
    goto L70525;
L704EC:;
    *(signed char *)(*(char **)player_character + 546) = 100;
    *(int *)(*(char **)player_character + 529) = 0;
    func_000A0AD9((int)D_001961F5, (int)D_00176089, 13, (int)D_00175EAA, 1233);
L70525:;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 543)) == 100) goto L70548;
    if (*(int *)(*(char **)player_character + 533) != 0) goto L7054A;
L70548:;
    goto L7055D;
L7054A:;
    if (((unsigned)*(int *)(*(char **)player_character + 533)) < game_minutes) goto L7055F;
L7055D:;
    goto L7056B;
L7055F:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 1) goto L7056D;
L7056B:;
    return;
L7056D:;
    *(signed char *)(*(char **)player_character + 543) = 100;
    *(int *)(*(char **)player_character + 533) = 0;
    func_000A0AD9((int)D_001961F5, (int)D_00176096, 13, (int)D_00175EAA, 1243);
}

void guild_teleport(void)
{
    *(signed char *)player_environment = 1;
    travel_map_open(100);
}

void guild_join_dark_brotherhood(void)
{
    guild_add_membership(108, 0);
}

void guild_join_thieves_guild(void)
{
    guild_add_membership(42, 3);
}

void guild_add_membership(int a1, unsigned char a2)
{
{
    int l_1C;

    l_1C = (int)object_create_child(player_entity, 0, 13);
    *(signed char *)((char *)l_1C) = 10;
    *(short *)((char *)l_1C + 21) = 3;
    (guild_membership = (struct cur *)(l_1C + 71))->f3 = a1;
    guild_membership->f2 = a2;
    guild_membership->f5 = game_minutes;
    guild_membership->f0 = 0;
}
}

int guild_confirm_price(int a1)
{
    int l_1C;

    *(int *)D_00195D2C = func_00097BD9((*(int *)D_00195D2C = a1));
    *(int *)D_00195D30 = ((*(int *)D_00195D30 = trade_adjust_price(*(int *)D_00195D2C, 0)) * *(int *)trade_price_scale) / 256;
    func_00097A85();
    l_1C = func_00097B2A();
    return l_1C;
}

void guild_buy_soulgems(void)
{
    object_free_children((int)D_001960D9);
    func_0005F401((int)D_001960D9);
    inventory_open_container((int)D_001960D9, 1, 4);
}

void guild_cure_diseases(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_24 = (int)player_entity->child;
L70775:;
    if (l_24 == 0) goto L707B3;
    if (((int)(unsigned char)*(signed char *)((char *)l_24)) != 11) goto L707A8;
    l_18 = l_24 + 71;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) >= 100) goto L707A8;
    l_20++;
L707A8:;
    l_24 = *(int *)((char *)l_24 + 55);
    goto L70775;
L707B3:;
    if (*(int *)(*(char **)player_character + 499) == 0) goto L707C7;
    l_20++;
L707C7:;
    if (l_20 != 0) goto L707E1;
    msgbox_show_rsc(30, 1);
    return;
L707E1:;
    l_1C = l_20 * 250;
    if (guild_membership->f2 != 142) goto L70837;
    l_1C = (l_1C * (((10 - guild_membership->f0) << 8) / 10)) / 256;
L70837:;
    l_1C = guild_confirm_price(l_1C);
    if (l_1C < 1) return;
    if (gold_can_afford(l_1C) != 0) goto L70865;
    msgbox_show_rsc(454, 1);
    return;
L70865:;
    gold_spend(l_1C);
    spfx_cure_disease((int)player_entity, *(int *)player_character);
}

void guild_buy_blessing(void)
{
    struct thing *t;
    int n;
    int msg;
    struct mobile *m;

    msg = 0;
    m = 0;
    if (guild_membership->f2 == 142)
        return;
    t = player_entity->child;
    while (t != 0) {
        if (t->type == 30) {
            msgbox_show_rsc(454, 1);
            return;
        }
        t = t->next;
    }
    n = guild_confirm_price(100);
    if (n < 1)
        return;
    if (gold_can_afford(n) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
    gold_spend(n);
    t = object_create_child(player_entity, 0, 7);
    m = &t->mob;
    switch (guild_membership->f2) {
    case 143:
        m->f0 = 14;
        msg = 705;
        break;
    case 144:
        m->f0 = 133;
        msg = 707;
        break;
    case 145:
        m->f0 = 134;
        msg = 709;
        break;
    case 146:
        m->f0 = 129;
        msg = 710;
        break;
    case 147:
        m->f0 = 135;
        msg = 712;
        break;
    case 148:
        m->f0 = 255;
        msg = 716;
        break;
    case 149:
        m->f0 = 132;
        msg = 717;
        break;
    }
    m->f2 = (guild_membership != 0 ? guild_membership->f0 + 4 : 4) * 1440 + game_minutes;
    m->f1 = blessing_apply(m, guild_membership != 0 ? guild_membership->f0 + 10 : 8);
    if (msg != 0)
        msgbox_show_rsc(msg, 1);
}
