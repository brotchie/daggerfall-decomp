/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x00070496 to 0x00070887, kept together for its switch table's alignment */
#include "records.h"

extern char D_00175EAA[];        /* __FILE__ */
extern char D_00176089[];
extern char D_00176096[];
extern int trade_price_scale;
extern unsigned char player_environment;
extern struct record *player_entity;
extern struct character *player_character;
extern int game_minutes;
extern int trade_total;
extern int trade_price;
extern char D_001960D9[];
extern char D_001961F5[];
extern struct membership *guild_membership;
extern void msgbox_show_rsc(short, int);
extern void shop_stock_soul_traps(int);
extern int blessing_apply(struct blessing *, int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern void spfx_cure_disease(int, int);
extern void object_free_children(int);
extern struct record *object_create_child(struct record *, int, int);
extern void inventory_open_container(int, int, int);
extern int trade_adjust_price(int, int);
extern void trade_make_offer(void);
extern int trade_settle_offer(void);
extern int trade_base_price(int);
extern int travel_map_open(int);
extern int mc_strncpy();
void guild_add_membership(int, unsigned char);
int guild_confirm_price(int);

void guild_check_invitations(void)
{
    if (player_character->thieves_invite_count != 100 && player_character->thieves_invite_time != 0 && ((unsigned)player_character->thieves_invite_time) < game_minutes && ((int)player_environment) == 1) {
        player_character->thieves_invite_count = 100;
        player_character->thieves_invite_time = 0;
        mc_strncpy((int)D_001961F5, (int)D_00176089, 13, (int)D_00175EAA, 1233);
    }
    if (player_character->brotherhood_invite_count == 100 || player_character->brotherhood_invite_time == 0 || ((unsigned)player_character->brotherhood_invite_time) >= game_minutes || ((int)player_environment) != 1) {
        return;
    }
    player_character->brotherhood_invite_count = 100;
    player_character->brotherhood_invite_time = 0;
    mc_strncpy((int)D_001961F5, (int)D_00176096, 13, (int)D_00175EAA, 1243);
}

void guild_teleport(void)
{
    player_environment = 1;
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

void guild_add_membership(int faction_id, unsigned char kind)
{
{
    struct record *object;

    object = object_create_child(player_entity, 0, 13);
    object->type = 10;
    object->flags = 3;
    (guild_membership = &object->data.membership)->faction = faction_id;
    guild_membership->kind = kind;
    guild_membership->rank_time = game_minutes;
    guild_membership->rank = 0;
}
}

int guild_confirm_price(int price)
{
    int result;

    trade_total = trade_base_price((trade_total = price));
    trade_price = ((trade_price = trade_adjust_price(trade_total, 0)) * trade_price_scale) / 256;
    trade_make_offer();
    result = trade_settle_offer();
    return result;
}

void guild_buy_soulgems(void)
{
    object_free_children((int)D_001960D9);
    shop_stock_soul_traps((int)D_001960D9);
    inventory_open_container((int)D_001960D9, 1, 4);
}

void guild_cure_diseases(void)
{
    struct record *object;
    int count;
    int price;
    struct disease *disease;

    count = 0;
    object = player_entity->children;
    while (object != 0) {
        if (object->type == 11) {
            disease = &object->data.disease;
            if (disease->id < 100) count++;
        }
        object = object->next;
    }
    if (player_character->special_infection_time != 0) count++;
    if (count == 0) {
        msgbox_show_rsc(30, 1);
        return;
    }
    price = count * 250;
    if (guild_membership->kind == 142) {
        price = (price * (((10 - guild_membership->rank) << 8) / 10)) / 256;
    }
    price = guild_confirm_price(price);
    if (price < 1) return;
    if (gold_can_afford(price) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
    gold_spend(price);
    spfx_cure_disease((int)player_entity, (int)player_character);
}

void guild_buy_blessing(void)
{
    struct record *object;
    int price;
    int text_id;
    struct blessing *blessing;

    text_id = 0;
    blessing = 0;
    if (guild_membership->kind == 142)
        return;
    object = player_entity->children;
    while (object != 0) {
        if (object->type == 30) {
            msgbox_show_rsc(454, 1);
            return;
        }
        object = object->next;
    }
    price = guild_confirm_price(100);
    if (price < 1)
        return;
    if (gold_can_afford(price) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
    gold_spend(price);
    object = object_create_child(player_entity, 0, 7);
    blessing = &object->data.blessing;
    switch (guild_membership->kind) {
    case 143:
        blessing->target = 14;
        text_id = 705;
        break;
    case 144:
        blessing->target = 133;
        text_id = 707;
        break;
    case 145:
        blessing->target = 134;
        text_id = 709;
        break;
    case 146:
        blessing->target = 129;
        text_id = 710;
        break;
    case 147:
        blessing->target = 135;
        text_id = 712;
        break;
    case 148:
        blessing->target = 255;
        text_id = 716;
        break;
    case 149:
        blessing->target = 132;
        text_id = 717;
        break;
    }
    blessing->end_time = (guild_membership != 0 ? guild_membership->rank + 4 : 4) * 1440 + game_minutes;
    blessing->amount = blessing_apply(blessing, guild_membership != 0 ? guild_membership->rank + 10 : 8);
    if (text_id != 0)
        msgbox_show_rsc(text_id, 1);
}
