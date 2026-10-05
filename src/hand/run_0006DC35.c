/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x0006D91A to 0x0006DC35, kept together for its switch table's alignment */
#include "records.h"

struct msgs {
    short busy;                 /* 0x00 */
    short done;                 /* 0x02 */
    short start;                /* 0x04 */
    short level[10];            /* 0x06 */
};
extern unsigned char mouse_buttons;
extern struct msgs guild_messages[];
extern char guild_rank_messages[];         /* guild_messages[0].level */
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *D_00195AF4;
extern struct character *player_character;
extern unsigned int game_minutes;
extern unsigned char D_00196271;
extern struct faction *D_0019671C;
extern struct membership *guild_membership;
extern unsigned char D_001A4A1D;
extern void msgbox_show_rsc(short, int);
extern int guild_join_check(int);
extern int guild_rank_for_skills(int);
extern struct membership *guild_find_membership_by_faction(short);
extern int guild_find_membership_by_bits(unsigned char);
extern void func_0007141A(int);
extern void msgbox_yes_no_rsc(short);
extern void object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern void func_0012B136(void);
extern char D_00175EAA[];
extern char D_00175EB3[];
extern char D_00175EC1[];
extern char D_00175EEC[];
extern char D_00175F17[];
extern char D_00175F42[];
extern char D_00175F6D[];
extern char D_00175F98[];
extern char D_00175FC3[];
extern char D_00175FEE[];
extern char D_00176019[];
extern char D_00176044[];
extern char trade_price_scale[];
extern char D_001832B0[];
extern char region_names[];
extern char D_00186F37[];
extern char D_00186F4C[];
extern char D_00186F5F[];
extern char D_00186F74[];
extern char D_00186F88[];
extern char D_00186F9E[];
extern char D_00186FB6[];
extern char D_00186FC9[];
extern char D_00186FDB[];
extern char D_00186FEE[];
extern char D_00187001[];
extern char D_00187017[];
extern char saved_location_name[];
extern char saved_region_name[];
extern char text_buffer[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct record *guild_npc_object;
extern struct location *current_location;
extern struct career *player_class;
extern char D_001960D9[];
extern char D_00196118[];
extern char current_region[];
extern char D_001962AB[];
struct slot { char *p; int f4; int f8; int f12; int f16; };   /* 20 bytes: p is a type-43 record's data */
extern struct slot bank_houses_for_sale[];
extern int D_001A41DC;
extern struct record *D_001A41E4;
extern char bank_house_count[];
extern char D_001A4A1A[];
extern char D_001A4A1C[];
extern struct faction *faction_find(short);
extern void daedra_summon(struct record *);
extern int spellmaker_open(int);
extern void training_offer(int);
extern void msgbox_show_string(int, int);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern int itemmaker_open(int);
extern void func_0005E37F(unsigned short, int, int, struct item *);
extern void bank_add_house_for_sale(struct record *);
extern int disk_read_file(int, int);
extern void guild_buy_potions(void);
extern void guild_buy_spells(void);
extern void guild_buy_magic_items(void);
extern void guild_teleport(void);
extern void guild_buy_soulgems(void);
extern void guild_cure_diseases(void);
extern void guild_buy_blessing(void);
extern void guild_donate(void);
extern void guild_temple_quest(void);
extern int guild_kind_of_faction(int);
extern int guild_service_label(short);
extern int guild_menu(int, int, int);
extern void guild_heal(void);
extern void shop_open_repair(int, struct record *);
extern void npc_talk(struct record *);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern void object_free_children(int);
extern void object_foreach(struct record *, int);
extern int potionmaker_open(int);
extern void inventory_open_container(int, int, int);
extern void inv_store_item(struct record *);
extern int mc_free();
extern int mc_strncpy();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);

void guild_join_or_promote(int a1, int a2)
{
    struct record *obj;
    int delta;
    int r;
    int orig;
    int flag;

    orig = a1;
    flag = 0;
    a1 &= 63;
    if (guild_membership == 0 && a2 != 0) {
        D_001A4A1D++;
        if (D_001A4A1D != (char)1)
            return;
        if ((orig & 64) && guild_find_membership_by_bits(64) != 0)
            return;
        if ((orig & 128) && guild_find_membership_by_bits(128) != 0)
            return;
        r = guild_join_check(a1);
        if (r == 2) {
            msgbox_show_rsc(guild_messages[a1].busy, 1);
            return;
        }
        if (r == 1) {
            msgbox_show_rsc(guild_messages[a1].done, 1);
            return;
        }
        msgbox_yes_no_rsc(guild_messages[a1].start);
        if (D_00196271 == 2)
            return;
        obj = object_create_child(player_entity, 0, 13);
        obj->type = 10;
        obj->flags = 3;
        (guild_membership = &obj->data.membership)->rank = 0;
        guild_membership->kind = orig;
        guild_membership->faction = D_0019671C->id;
        guild_membership->rank_time = game_minutes;
        while (mouse_buttons)
            func_0012B136();
        msgbox_show_rsc(guild_messages[a1].level[0], 1);
        return;
    }
    if (guild_membership == 0)
        return;
    if (game_minutes - guild_membership->rank_time <= 40320)
        return;
    delta = guild_rank_for_skills(a1) - guild_membership->rank;
    if (D_0019671C->reputation < 0) {
        delta = -(guild_membership->rank + 1);
        flag = 1;
    }
    if (delta > 0 || flag != 0) {
        guild_membership->rank += delta;
        guild_membership->rank_time = game_minutes;
        if (a1 == 3 && (guild_membership->rank == 6 || guild_membership->rank == 8))
            func_0007141A(0);
        if (a1 == 0 && guild_membership->rank < 100)
            func_0007141A(1);
        if (guild_membership->rank > 100) {
            msgbox_show_rsc(668, 1);
            guild_find_membership_by_faction(current_building->faction_id);
            if (D_00195AF4 != 0)
                object_delete(D_00195AF4);
            if (a1 == 3)
                player_character->thieves_invite_count = 0;
            if (a1 == 0)
                player_character->brotherhood_invite_count = 0;
            guild_membership = 0;
            return;
        }
        if (delta < 0) {
            msgbox_show_rsc(667, 1);
            return;
        }
        msgbox_show_rsc(*(short *)(guild_rank_messages + (a1 * 26 + guild_membership->rank * 2)), 1);
    }
}

void guild_service_dispatch(struct record *a1)
{
    struct record *l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = 0;
    l_20 = 0;
    *(signed char *)D_001A4A1C = 0;
    object_free_children((int)D_001960D9);
    D_0019671C = faction_find(current_building->faction_id);
    guild_membership = guild_find_membership_by_faction(current_building->faction_id);
    l_2C = guild_kind_of_faction((int)D_0019671C);
    guild_npc_object = a1;
    guild_join_or_promote(l_2C, 0);
    *(short *)D_001A4A1A = ((unsigned)a1->id) >> 16;
    l_1C = guild_service_label((int)(short)a1->data.person.faction_id);
    if (l_1C != 0) goto L6DCEE;
    npc_talk(a1);
    return;
L6DCEE:;
    l_28 = ((guild_membership != 0) ? 1 : 0);
    func_000A0ED9(163, (int)D_00175EAA);
    mc_sprintf((int)text_buffer, (int)D_00175EB3, l_28 + 48);
    l_18 = disk_read_file((int)text_buffer, 0);
    l_30 = guild_menu(l_18, l_28, l_1C);
    switch (l_30) {
case 0:
    guild_join_or_promote(l_2C, 1);
    if (l_18 == 0) goto L6DD8B;
    if (l_18 != (-1751672937)) goto L6DD8D;
L6DD8B:;
    goto L6DDA6;
L6DD8D:;
    mc_free(l_18, (int)D_00175EAA, 170);
    l_18 = -1751672937;
L6DDA6:;
    return;
case 1:
    npc_talk(a1);
    if (l_18 == 0) goto L6DDC2;
    if (l_18 != (-1751672937)) goto L6DDC4;
L6DDC2:;
    goto L6DDDD;
L6DDC4:;
    mc_free(l_18, (int)D_00175EAA, 174);
    l_18 = -1751672937;
L6DDDD:;
    return;
case 2:
    if (l_18 == 0) goto L6DDF1;
    if (l_18 != (-1751672937)) goto L6DDF3;
L6DDF1:;
    goto L6DE0C;
L6DDF3:;
    mc_free(l_18, (int)D_00175EAA, 177);
    l_18 = -1751672937;
L6DE0C:;
    goto L6DE67;
case 3:
    if (l_18 == 0) goto L6DE1D;
    if (l_18 != (-1751672937)) goto L6DE1F;
L6DE1D:;
    goto L6DE38;
L6DE1F:;
    mc_free(l_18, (int)D_00175EAA, 180);
    l_18 = -1751672937;
L6DE38:;
    return;
default:
    if (l_18 == 0) goto L6DE4C;
    if (l_18 != (-1751672937)) goto L6DE4E;
L6DE4C:;
    goto L6DE67;
L6DE4E:;
    mc_free(l_18, (int)D_00175EAA, 183);
    l_18 = -1751672937;
L6DE67:;
}
    switch ((unsigned)l_2C) {
    goto L6F43A;
case 0:
    if (guild_membership == 0) goto L6E093;
    switch (a1->data.person.faction_id) {
case 839:
    training_offer((int)D_00186F37);
    goto L6E093;
case 841:
    if ((guild_membership->rank) < 1) goto L6DFC7;
    guild_buy_potions();
    goto L6DFCE;
L6DFC7:;
    l_24 = 3100;
L6DFCE:;
    goto L6E093;
case 840:
    if ((guild_membership->rank) < 3) goto L6DFF0;
    potionmaker_open(1);
    goto L6DFF7;
L6DFF0:;
    l_24 = 3100;
L6DFF7:;
    goto L6E093;
case 843:
    if ((guild_membership->rank) >= 5) goto L6E016;
    l_24 = 3100;
    goto L6E01B;
L6E016:;
    guild_buy_soulgems();
L6E01B:;
    goto L6E093;
case 842:
    if ((guild_membership->rank) >= 7) goto L6E03A;
    l_24 = 3100;
    goto L6E051;
L6E03A:;
    msgbox_show_rsc(402, 1);
    npc_talk(a1);
L6E051:;
    goto L6E093;
case 807:
    if (a1->quest_id != 0) goto L6E081;
    quest_pick_file(76, 0, 48, 66, guild_membership->rank);
    goto L6E089;
L6E081:;
    npc_talk(a1);
L6E089:;
    goto L6E093;
default:
    npc_talk(a1);
L6E093:;
    goto L6F43A;
}
case 1:
    if (guild_membership == 0) goto L6E217;
    if (((int)(unsigned short)(player_class->flags & 8)) == 0) goto L6E0D7;
    if (player_character->magicka != player_character->max_magicka) goto L6E0D9;
L6E0D7:;
    goto L6E101;
L6E0D9:;
    player_character->magicka = player_character->max_magicka;
    msgbox_show_rsc(465, 1);
L6E101:;
    switch (a1->data.person.faction_id) {
case 61:
    training_offer((int)D_00186F5F);
    goto L6E215;
case 64:
    spellmaker_open(1);
    goto L6E215;
case 65:
    if ((guild_membership->rank) < 3) goto L6E197;
    guild_buy_magic_items();
    goto L6E19E;
L6E197:;
    l_24 = 3100;
L6E19E:;
    goto L6E215;
case 802:
    if ((guild_membership->rank) < 5) goto L6E1C0;
    itemmaker_open(1);
    goto L6E1C7;
L6E1C0:;
    l_24 = 3100;
L6E1C7:;
    goto L6E215;
case 66:
    if ((guild_membership->rank) < 6) goto L6E1E4;
    daedra_summon(a1);
    goto L6E1EB;
L6E1E4:;
    l_24 = 3100;
L6E1EB:;
    goto L6E215;
case 62:
    if ((guild_membership->rank) < 8) goto L6E205;
    guild_teleport();
    goto L6E20C;
L6E205:;
    l_24 = 3100;
L6E20C:;
    goto L6E215;
default:
    l_20 = 1;
L6E215:;
    goto L6E21E;
L6E217:;
    l_20 = 1;
L6E21E:;
    if (l_20 == 0) goto L6E338;
}
    switch (a1->data.person.faction_id) {
case 60:
    guild_buy_spells();
    goto L6E338;
case 63:
    if (a1->quest_id == 0) goto L6E279;
    npc_talk(a1);
    goto L6E2D2;
L6E279:;
    if (guild_membership == 0) goto L6E2AB;
    quest_pick_file(78, 0, 48, 66, player_character->level);
    goto L6E2D2;
L6E2AB:;
    quest_pick_file(78, 0, 48, 67, player_character->level);
L6E2D2:;
    goto L6E338;
case 801:
    hud_message_add(*(int *)D_001832B0);
    object_free_children((int)D_001960D9);
    *(int *)trade_price_scale = ((10 - (guild_membership->rank)) << 8) / 10;
    inventory_open_container((int)D_001960D9, 4, 8);
    goto L6E338;
default:
    msgbox_show_string((int)D_00175EC1, 1);
L6E338:;
    goto L6F43A;
}
case 2:
    if (guild_membership == 0) goto L6E403;
    switch (a1->data.person.faction_id) {
case 849:
    training_offer((int)D_00186F74);
    goto L6E401;
case 850:
    *(int *)trade_price_scale = ((10 - (guild_membership->rank)) << 8) / 10;
    shop_open_repair(255, a1);
    goto L6E401;
case 851:
    if (a1->quest_id != 0) goto L6E3F0;
    quest_pick_file(77, 0, 48, 66, guild_membership->rank);
    goto L6E3F8;
L6E3F0:;
    npc_talk(a1);
L6E3F8:;
    goto L6E401;
default:
    l_20 = 1;
L6E401:;
    goto L6E40A;
L6E403:;
    l_20 = 1;
L6E40A:;
    if (l_20 == 0) goto L6E463;
    if (a1->data.person.faction_id != 851) goto L6E45B;
    if (a1->quest_id != 0) goto L6E451;
    quest_pick_file(77, 0, 48, 67, guild_membership->rank);
    goto L6E459;
L6E451:;
    npc_talk(a1);
L6E459:;
    goto L6E463;
L6E45B:;
    npc_talk(a1);
L6E463:;
    goto L6F43A;
}
case 3:
    if (guild_membership != 0) goto L6E486;
    goto L6E57A;
L6E486:;
    switch ((unsigned short)(a1->data.person.faction_id - 803)) {
case 0:
    training_offer((int)D_00186F4C);
    goto L6E57A;
case 1:
    if (a1->quest_id != 0) goto L6E4EE;
    quest_pick_file(79, 0, 48, 66, guild_membership->rank);
    goto L6E4F6;
L6E4EE:;
    npc_talk(a1);
L6E4F6:;
    goto L6E57A;
case 2:
    if ((guild_membership->rank) >= 2) goto L6E515;
    l_24 = 3100;
    goto L6E57A;
L6E515:;
    *(int *)trade_price_scale = 128;
    object_free_children((int)D_001960D9);
    inventory_open_container((int)D_001960D9, 2, 6);
    goto L6E57A;
case 3:
    if ((guild_membership->rank) >= 4) goto L6E559;
    l_24 = 3100;
    goto L6E57A;
L6E559:;
    msgbox_show_rsc(402, 1);
    npc_talk(a1);
    goto L6E57A;
default:
    npc_talk(a1);
L6E57A:;
    goto L6F43A;
}
case 68:
case 69:
case 70:
case 71:
case 72:
case 73:
case 74:
case 75:
case 76:
case 77:
    if (guild_membership == 0) goto L6E7B0;
    switch (a1->data.person.faction_id) {
case 845:
    if (((1 << (guild_membership->rank)) & guild_membership->armor_received) == 0) goto L6E5EC;
    msgbox_show_rsc(461, 1);
    goto L6E7AE;
L6E5EC:;
    guild_membership->armor_received |= 1 << (guild_membership->rank);
    l_38 = object_create_child(a1, 0, 107);
    l_38->type = 2;
    *(signed char *)D_001962AB = guild_membership->rank - 1;
    if (((int)(unsigned char)*(signed char *)D_001962AB) <= 100) goto L6E640;
    *(signed char *)D_001962AB = 2;
    goto L6E653;
L6E640:;
    if (((int)(unsigned char)*(signed char *)D_001962AB) <= 10) goto L6E653;
    *(signed char *)D_001962AB = 10;
L6E653:;
    func_0005E37F(2, 0, 6, &l_38->data.item);
    l_38->data.item.armor_type = 2;
    l_38->x = player_object->x;
    l_38->y = player_object->y;
    l_38->z = player_object->z;
    inv_store_item(l_38);
    msgbox_show_rsc(463, 1);
    goto L6E7AE;
case 848:
    if (player_character->house != 0) goto L6E7AE;
    if ((guild_membership->rank) == 9) goto L6E6EB;
    msgbox_show_rsc(460, 1);
    goto L6E7AE;
L6E6EB:;
    object_foreach(D_00195AC4, (int)bank_add_house_for_sale);
    if (*(signed char *)bank_house_count == 0) goto L6E7AE;
    l_30 = rand_range(0, *(unsigned char *)bank_house_count - 1);
    player_character->house = bank_houses_for_sale[l_30].f12;
    D_001A41E4 = (struct record *)(bank_houses_for_sale[l_30].p - 71);
    D_001A41DC = bank_houses_for_sale[l_30].f4;
    msgbox_show_rsc(462, 1);
    mc_strncpy((int)saved_region_name, *(int *)(region_names + (((int)(unsigned char)*(signed char *)current_region) << 2)), 32, (int)D_00175EAA, 413);
    mc_strncpy((int)saved_location_name, (int)current_location, 32, (int)D_00175EAA, 414);
    goto L6E7AE;
default:
    l_20 = 1;
L6E7AE:;
    goto L6E7B7;
L6E7B0:;
    l_20 = 1;
L6E7B7:;
    if (l_20 == 0) goto L6E854;
}
    switch (a1->data.person.faction_id) {
case 846:
    if (a1->quest_id == 0) goto L6E7EA;
    npc_talk(a1);
    goto L6E843;
L6E7EA:;
    if (guild_membership == 0) goto L6E81C;
    quest_pick_file(66, 0, 48, 66, player_character->level);
    goto L6E843;
L6E81C:;
    quest_pick_file(66, 0, 48, 67, player_character->level);
L6E843:;
    goto L6E854;
default:
    msgbox_show_string((int)D_00175EEC, 1);
L6E854:;
    goto L6F43A;
}
case 142:
    if (guild_membership != 0) goto L6E87A;
    goto L6E942;
L6E87A:;
    guild_heal();
    switch ((unsigned short)(a1->data.person.faction_id - 453)) {
case 0:
    if ((guild_membership->rank) < 1) goto L6E8C2;
    guild_buy_potions();
    goto L6E8C9;
L6E8C2:;
    l_24 = 3100;
L6E8C9:;
    goto L6E940;
case 1:
    if ((guild_membership->rank) < 4) goto L6E8EB;
    potionmaker_open(1);
    goto L6E8F2;
L6E8EB:;
    l_24 = 3100;
L6E8F2:;
    goto L6E940;
case 2:
    if ((guild_membership->rank) < 4) goto L6E90C;
    guild_buy_soulgems();
    goto L6E913;
L6E90C:;
    l_24 = 3100;
L6E913:;
    goto L6E940;
case 3:
    if ((guild_membership->rank) < 7) goto L6E930;
    daedra_summon(a1);
    goto L6E937;
L6E930:;
    l_24 = 3100;
L6E937:;
    goto L6E940;
default:
    l_20 = 1;
L6E940:;
    goto L6E949;
L6E942:;
    l_20 = 1;
L6E949:;
    if (l_20 == 0) goto L6E9C3;
}
    switch (a1->data.person.faction_id) {
case 241:
    training_offer((int)D_00186F88);
    goto L6E9C3;
case 240:
    guild_temple_quest();
    goto L6E9C3;
case 810:
    guild_donate();
    goto L6E9C3;
case 813:
    guild_cure_diseases();
    goto L6E9C3;
default:
    msgbox_show_string((int)D_00175F17, 1);
L6E9C3:;
    goto L6F43A;
}
case 143:
    if (guild_membership == 0) goto L6EA92;
    if ((guild_membership->rank) < 2) goto L6E9EB;
    guild_heal();
L6E9EB:;
    switch (a1->data.person.faction_id) {
case 462:
    if ((guild_membership->rank) < 1) goto L6EA36;
    guild_buy_potions();
    goto L6EA3D;
L6EA36:;
    l_24 = 3100;
L6EA3D:;
    goto L6EA90;
case 463:
    if ((guild_membership->rank) < 6) goto L6EA5C;
    potionmaker_open(1);
    goto L6EA63;
L6EA5C:;
    l_24 = 3100;
L6EA63:;
    goto L6EA90;
case 464:
    if ((guild_membership->rank) < 8) goto L6EA80;
    daedra_summon(a1);
    goto L6EA87;
L6EA80:;
    l_24 = 3100;
L6EA87:;
    goto L6EA90;
default:
    l_20 = 1;
L6EA90:;
    goto L6EA99;
L6EA92:;
    l_20 = 1;
L6EA99:;
    if (l_20 == 0) goto L6EB2C;
}
    switch (a1->data.person.faction_id) {
case 243:
    training_offer((int)D_00186F9E);
    goto L6EB2C;
case 810:
    guild_donate();
    goto L6EB2C;
case 460:
    guild_buy_blessing();
    goto L6EB2C;
case 813:
    guild_cure_diseases();
    goto L6EB2C;
case 240:
    guild_temple_quest();
    goto L6EB2C;
default:
    msgbox_show_string((int)D_00175F42, 1);
L6EB2C:;
    goto L6F43A;
}
case 144:
    if (guild_membership == 0) goto L6EBFB;
    if ((guild_membership->rank) < 1) goto L6EB54;
    guild_heal();
L6EB54:;
    switch (a1->data.person.faction_id) {
case 468:
    if ((guild_membership->rank) < 2) goto L6EB9F;
    guild_buy_potions();
    goto L6EBA6;
L6EB9F:;
    l_24 = 3100;
L6EBA6:;
    goto L6EBF9;
case 469:
    if ((guild_membership->rank) < 5) goto L6EBC5;
    potionmaker_open(1);
    goto L6EBCC;
L6EBC5:;
    l_24 = 3100;
L6EBCC:;
    goto L6EBF9;
case 470:
    if ((guild_membership->rank) < 7) goto L6EBE9;
    daedra_summon(a1);
    goto L6EBF0;
L6EBE9:;
    l_24 = 3100;
L6EBF0:;
    goto L6EBF9;
default:
    l_20 = 1;
L6EBF9:;
    goto L6EC02;
L6EBFB:;
    l_20 = 1;
L6EC02:;
    if (l_20 == 0) goto L6EC95;
}
    switch (a1->data.person.faction_id) {
case 245:
    training_offer((int)D_00186FB6);
    goto L6EC95;
case 810:
    guild_donate();
    goto L6EC95;
case 466:
    guild_buy_blessing();
    goto L6EC95;
case 813:
    guild_cure_diseases();
    goto L6EC95;
case 240:
    guild_temple_quest();
    goto L6EC95;
default:
    msgbox_show_string((int)D_00175F6D, 1);
L6EC95:;
    goto L6F43A;
}
case 145:
    if (guild_membership == 0) goto L6ED64;
    if ((guild_membership->rank) < 1) goto L6ECBD;
    guild_heal();
L6ECBD:;
    switch (a1->data.person.faction_id) {
case 473:
    if ((guild_membership->rank) < 4) goto L6ED08;
    guild_buy_potions();
    goto L6ED0F;
L6ED08:;
    l_24 = 3100;
L6ED0F:;
    goto L6ED62;
case 474:
    if ((guild_membership->rank) < 5) goto L6ED2E;
    potionmaker_open(1);
    goto L6ED35;
L6ED2E:;
    l_24 = 3100;
L6ED35:;
    goto L6ED62;
case 475:
    if ((guild_membership->rank) < 7) goto L6ED52;
    daedra_summon(a1);
    goto L6ED59;
L6ED52:;
    l_24 = 3100;
L6ED59:;
    goto L6ED62;
default:
    l_20 = 1;
L6ED62:;
    goto L6ED6B;
L6ED64:;
    l_20 = 1;
L6ED6B:;
    if (l_20 == 0) goto L6EDFE;
}
    switch (a1->data.person.faction_id) {
case 247:
    training_offer((int)D_00186FC9);
    goto L6EDFE;
case 810:
    guild_donate();
    goto L6EDFE;
case 471:
    guild_buy_blessing();
    goto L6EDFE;
case 813:
    guild_cure_diseases();
    goto L6EDFE;
case 240:
    guild_temple_quest();
    goto L6EDFE;
default:
    msgbox_show_string((int)D_00175F98, 1);
L6EDFE:;
    goto L6F43A;
}
case 146:
    if (guild_membership == 0) goto L6EECD;
    if ((guild_membership->rank) < 2) goto L6EE26;
    guild_heal();
L6EE26:;
    switch (a1->data.person.faction_id) {
case 480:
    if ((guild_membership->rank) < 3) goto L6EE71;
    guild_buy_magic_items();
    goto L6EE78;
L6EE71:;
    l_24 = 3100;
L6EE78:;
    goto L6EECB;
case 481:
    if ((guild_membership->rank) < 5) goto L6EE97;
    itemmaker_open(1);
    goto L6EE9E;
L6EE97:;
    l_24 = 3100;
L6EE9E:;
    goto L6EECB;
case 482:
    if ((guild_membership->rank) < 6) goto L6EEBB;
    daedra_summon(a1);
    goto L6EEC2;
L6EEBB:;
    l_24 = 3100;
L6EEC2:;
    goto L6EECB;
default:
    l_20 = 1;
L6EECB:;
    goto L6EED4;
L6EECD:;
    l_20 = 1;
L6EED4:;
    if (l_20 == 0) goto L6EF86;
}
    switch (a1->data.person.faction_id) {
case 249:
    training_offer((int)D_00186FDB);
    goto L6EF86;
case 810:
    guild_donate();
    goto L6EF86;
case 477:
    guild_buy_blessing();
    goto L6EF86;
case 813:
    guild_cure_diseases();
    goto L6EF86;
case 240:
    guild_temple_quest();
    goto L6EF86;
default:
    msgbox_show_string((int)D_00175FC3, 1);
L6EF86:;
    goto L6F43A;
}
case 147:
    if (guild_membership == 0) goto L6F064;
    if ((guild_membership->rank) < 2) goto L6EFAE;
    guild_heal();
L6EFAE:;
    switch (a1->data.person.faction_id) {
case 485:
    if ((guild_membership->rank) < 1) goto L6F008;
    guild_buy_potions();
    goto L6F00F;
L6F008:;
    l_24 = 3100;
L6F00F:;
    goto L6F062;
case 487:
    if ((guild_membership->rank) < 5) goto L6F02E;
    potionmaker_open(1);
    goto L6F035;
L6F02E:;
    l_24 = 3100;
L6F035:;
    goto L6F062;
case 488:
    if ((guild_membership->rank) < 7) goto L6F052;
    daedra_summon(a1);
    goto L6F059;
L6F052:;
    l_24 = 3100;
L6F059:;
    goto L6F062;
default:
    l_20 = 1;
L6F062:;
    goto L6F06B;
L6F064:;
    l_20 = 1;
L6F06B:;
    if (l_20 == 0) goto L6F11D;
}
    switch (a1->data.person.faction_id) {
case 250:
    training_offer((int)D_00186FEE);
    goto L6F11D;
case 810:
    guild_donate();
    goto L6F11D;
case 484:
    guild_buy_blessing();
    goto L6F11D;
case 813:
    guild_cure_diseases();
    goto L6F11D;
case 240:
    guild_temple_quest();
    goto L6F11D;
default:
    msgbox_show_string((int)D_00175FEE, 1);
L6F11D:;
    goto L6F43A;
}
case 148:
    if (guild_membership == 0) goto L6F1EA;
    guild_heal();
    switch (a1->data.person.faction_id) {
case 490:
    if ((guild_membership->rank) < 2) goto L6F18E;
    guild_buy_potions();
    goto L6F195;
L6F18E:;
    l_24 = 3100;
L6F195:;
    goto L6F1E8;
case 491:
    if ((guild_membership->rank) < 5) goto L6F1B4;
    potionmaker_open(1);
    goto L6F1BB;
L6F1B4:;
    l_24 = 3100;
L6F1BB:;
    goto L6F1E8;
case 492:
    if ((guild_membership->rank) < 7) goto L6F1D8;
    daedra_summon(a1);
    goto L6F1DF;
L6F1D8:;
    l_24 = 3100;
L6F1DF:;
    goto L6F1E8;
default:
    l_20 = 1;
L6F1E8:;
    goto L6F1F1;
L6F1EA:;
    l_20 = 1;
L6F1F1:;
    if (l_20 == 0) goto L6F2A3;
}
    switch (a1->data.person.faction_id) {
case 252:
    training_offer((int)D_00187001);
    goto L6F2A3;
case 810:
    guild_donate();
    goto L6F2A3;
case 493:
    guild_buy_blessing();
    goto L6F2A3;
case 813:
    guild_cure_diseases();
    goto L6F2A3;
case 240:
    guild_temple_quest();
    goto L6F2A3;
default:
    msgbox_show_string((int)D_00176019, 1);
L6F2A3:;
    goto L6F43A;
}
case 149:
    if (guild_membership == 0) goto L6F381;
    if ((guild_membership->rank) < 1) goto L6F2CB;
    guild_heal();
L6F2CB:;
    switch (a1->data.person.faction_id) {
case 496:
    if ((guild_membership->rank) < 3) goto L6F325;
    guild_buy_spells();
    goto L6F32C;
L6F325:;
    l_24 = 3100;
L6F32C:;
    goto L6F37F;
case 497:
    if ((guild_membership->rank) < 6) goto L6F34B;
    spellmaker_open(1);
    goto L6F352;
L6F34B:;
    l_24 = 3100;
L6F352:;
    goto L6F37F;
case 498:
    if ((guild_membership->rank) < 7) goto L6F36F;
    daedra_summon(a1);
    goto L6F376;
L6F36F:;
    l_24 = 3100;
L6F376:;
    goto L6F37F;
default:
    l_20 = 1;
L6F37F:;
    goto L6F388;
L6F381:;
    l_20 = 1;
L6F388:;
    if (l_20 == 0) goto L6F43A;
}
    switch (a1->data.person.faction_id) {
case 254:
    training_offer((int)D_00187017);
    goto L6F43A;
case 810:
    guild_donate();
    goto L6F43A;
case 494:
    guild_buy_blessing();
    goto L6F43A;
case 813:
    guild_cure_diseases();
    goto L6F43A;
case 240:
    guild_temple_quest();
    goto L6F43A;
default:
    msgbox_show_string((int)D_00176044, 1);
}
default:
L6F43A:;
    if (l_24 == 0) goto L6F44E;
    msgbox_show_rsc((int)(short)*(short *)&l_24, 1);
L6F44E:;
    if (*(signed char *)D_001A4A1C == 0) goto L6F460;
    if (*(int *)D_00196118 != 0) goto L6F462;
L6F460:;
    return;
L6F462:;
    *(signed char *)D_001A4A1C = 0;
    inventory_open_container((int)D_001960D9, 0, 6);
}
}
