/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x00070496 to 0x00070887, kept together for its switch table's alignment */
#include "records.h"

struct mobile { unsigned char f0; char f1; int f2; };   /* a blessing's data (type 30) */
extern char D_00175EAA[];        /* __FILE__ */
extern char D_00176089[];
extern char D_00176096[];
extern char trade_price_scale[];
extern char player_environment[];
extern struct record *player_entity;
extern struct character *player_character;
extern int game_minutes;
extern char D_00195D2C[];
extern char D_00195D30[];
extern char D_001960D9[];
extern char D_001961F5[];
extern struct membership *guild_membership;
extern void msgbox_show_rsc(short, int);
extern void func_0005F401(int);
extern int blessing_apply(struct mobile *, int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern void spfx_cure_disease(int, int);
extern void object_free_children(int);
extern struct record *object_create_child(struct record *, int, int);
extern void inventory_open_container(int, int, int);
extern int trade_adjust_price(int, int);
extern void func_00097A85(void);
extern int func_00097B2A(void);
extern int func_00097BD9(int);
extern int travel_map_open(int);
extern int mc_strncpy();
void guild_add_membership(int, unsigned char);
int guild_confirm_price(int);

void guild_check_invitations(void)
{
    if (player_character->thieves_invite_count == 100) goto L704C7;
    if (player_character->thieves_invite_time != 0) goto L704C9;
L704C7:;
    goto L704DC;
L704C9:;
    if (((unsigned)player_character->thieves_invite_time) < game_minutes) goto L704DE;
L704DC:;
    goto L704EA;
L704DE:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 1) goto L704EC;
L704EA:;
    goto L70525;
L704EC:;
    player_character->thieves_invite_count = 100;
    player_character->thieves_invite_time = 0;
    mc_strncpy((int)D_001961F5, (int)D_00176089, 13, (int)D_00175EAA, 1233);
L70525:;
    if (player_character->brotherhood_invite_count == 100) goto L70548;
    if (player_character->brotherhood_invite_time != 0) goto L7054A;
L70548:;
    goto L7055D;
L7054A:;
    if (((unsigned)player_character->brotherhood_invite_time) < game_minutes) goto L7055F;
L7055D:;
    goto L7056B;
L7055F:;
    if (((int)(unsigned char)*(signed char *)player_environment) == 1) goto L7056D;
L7056B:;
    return;
L7056D:;
    player_character->brotherhood_invite_count = 100;
    player_character->brotherhood_invite_time = 0;
    mc_strncpy((int)D_001961F5, (int)D_00176096, 13, (int)D_00175EAA, 1243);
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
    struct record *l_1C;

    l_1C = object_create_child(player_entity, 0, 13);
    l_1C->type = 10;
    l_1C->flags = 3;
    (guild_membership = &l_1C->data.membership)->faction = a1;
    guild_membership->kind = a2;
    guild_membership->rank_time = game_minutes;
    guild_membership->rank = 0;
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
    struct record *l_24;
    int l_20;
    int l_1C;
    struct disease *l_18;

    l_20 = 0;
    l_24 = player_entity->children;
L70775:;
    if (l_24 == 0) goto L707B3;
    if (l_24->type != 11) goto L707A8;
    l_18 = &l_24->data.disease;
    if (l_18->id >= 100) goto L707A8;
    l_20++;
L707A8:;
    l_24 = l_24->next;
    goto L70775;
L707B3:;
    if (player_character->special_infection_time == 0) goto L707C7;
    l_20++;
L707C7:;
    if (l_20 != 0) goto L707E1;
    msgbox_show_rsc(30, 1);
    return;
L707E1:;
    l_1C = l_20 * 250;
    if (guild_membership->kind != 142) goto L70837;
    l_1C = (l_1C * (((10 - guild_membership->rank) << 8) / 10)) / 256;
L70837:;
    l_1C = guild_confirm_price(l_1C);
    if (l_1C < 1) return;
    if (gold_can_afford(l_1C) != 0) goto L70865;
    msgbox_show_rsc(454, 1);
    return;
L70865:;
    gold_spend(l_1C);
    spfx_cure_disease((int)player_entity, (int)player_character);
}

void guild_buy_blessing(void)
{
    struct record *t;
    int n;
    int msg;
    struct mobile *m;

    msg = 0;
    m = 0;
    if (guild_membership->kind == 142)
        return;
    t = player_entity->children;
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
    m = (struct mobile *)&t->data;
    switch (guild_membership->kind) {
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
    m->f2 = (guild_membership != 0 ? guild_membership->rank + 4 : 4) * 1440 + game_minutes;
    m->f1 = blessing_apply(m, guild_membership != 0 ? guild_membership->rank + 10 : 8);
    if (msg != 0)
        msgbox_show_rsc(msg, 1);
}
