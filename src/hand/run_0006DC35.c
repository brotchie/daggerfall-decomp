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
extern struct record *found_object;
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
extern void guild_give_map(int);
extern void msgbox_yes_no_rsc(short);
extern void object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern void xn_mouse_poll_clamped(void);
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
extern int trade_price_scale;
extern int D_001832B0;
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
extern signed char text_buffer[];
extern struct record *player_object;
extern struct record *location_object;
extern struct record *scratch_object;
extern struct location *current_location;
extern struct career *player_class;
extern char D_001960D9[];
extern int D_00196118;
extern signed char current_region;
extern signed char forced_material;
struct slot { char *p; int f4; int f8; int f12; int f16; };   /* 20 bytes: p is a type-43 record's data */
extern struct slot bank_houses_for_sale[];
extern int D_001A41DC;
extern struct record *D_001A41E4;
extern signed char bank_house_count;
extern short D_001A4A1A;
extern signed char D_001A4A1C;
extern struct faction *faction_find(short);
extern void daedra_summon(struct record *);
extern int spellmaker_open(int);
extern void training_offer(int);
extern void msgbox_show_string(int, int);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern int itemmaker_open(int);
extern void item_make_in_range(unsigned short, int, int, struct item *);
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
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, int);
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
            xn_mouse_poll_clamped();
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
            guild_give_map(0);
        if (a1 == 0 && guild_membership->rank < 100)
            guild_give_map(1);
        if (guild_membership->rank > 100) {
            msgbox_show_rsc(668, 1);
            guild_find_membership_by_faction(current_building->faction_id);
            if (found_object != 0)
                object_delete(found_object);
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
    D_001A4A1C = 0;
    object_free_children((int)D_001960D9);
    D_0019671C = faction_find(current_building->faction_id);
    guild_membership = guild_find_membership_by_faction(current_building->faction_id);
    l_2C = guild_kind_of_faction((int)D_0019671C);
    scratch_object = a1;
    guild_join_or_promote(l_2C, 0);
    D_001A4A1A = ((unsigned)a1->id) >> 16;
    l_1C = guild_service_label((int)(short)a1->data.person.faction_id);
    if (l_1C == 0) {
        npc_talk(a1);
        return;
    }
    l_28 = ((guild_membership != 0) ? 1 : 0);
    mc_set_location(163, (int)D_00175EAA);
    mc_sprintf((int)text_buffer, (int)D_00175EB3, l_28 + 48);
    l_18 = disk_read_file((int)text_buffer, 0);
    l_30 = guild_menu(l_18, l_28, l_1C);
    switch (l_30) {
    case 0:
        guild_join_or_promote(l_2C, 1);
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_00175EAA, 170);
            l_18 = -1751672937;
        }
        return;
    case 1:
        npc_talk(a1);
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_00175EAA, 174);
            l_18 = -1751672937;
        }
        return;
    case 2:
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_00175EAA, 177);
            l_18 = -1751672937;
        }
        break;
    case 3:
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_00175EAA, 180);
            l_18 = -1751672937;
        }
        return;
    default:
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_00175EAA, 183);
            l_18 = -1751672937;
        }
    }
    switch ((unsigned)l_2C) {
    case 0:
        if (guild_membership != 0) {
            switch (a1->data.person.faction_id) {
            case 839:
                training_offer((int)D_00186F37);
                break;
            case 841:
                if ((guild_membership->rank) >= 1) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 840:
                if ((guild_membership->rank) >= 3) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 843:
                if ((guild_membership->rank) < 5) {
                    l_24 = 3100;
                } else {
                    guild_buy_soulgems();
                }
                break;
            case 842:
                if ((guild_membership->rank) < 7) {
                    l_24 = 3100;
                } else {
                    msgbox_show_rsc(402, 1);
                    npc_talk(a1);
                }
                break;
            case 807:
                if (a1->quest_id == 0) {
                    quest_pick_file(76, 0, 48, 66, guild_membership->rank);
                } else {
                    npc_talk(a1);
                }
                break;
            default:
                npc_talk(a1);
            }
        }
        break;
    case 1:
        if (guild_membership != 0) {
            if (((int)(unsigned short)(player_class->flags & 8)) != 0 && player_character->magicka != player_character->max_magicka) {
                player_character->magicka = player_character->max_magicka;
                msgbox_show_rsc(465, 1);
            }
            switch (a1->data.person.faction_id) {
            case 61:
                training_offer((int)D_00186F5F);
                break;
            case 64:
                spellmaker_open(1);
                break;
            case 65:
                if ((guild_membership->rank) >= 3) {
                    guild_buy_magic_items();
                } else {
                    l_24 = 3100;
                }
                break;
            case 802:
                if ((guild_membership->rank) >= 5) {
                    itemmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 66:
                if ((guild_membership->rank) >= 6) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 62:
                if ((guild_membership->rank) >= 8) {
                    guild_teleport();
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 60:
                guild_buy_spells();
                break;
            case 63:
                if (a1->quest_id != 0) {
                    npc_talk(a1);
                } else if (guild_membership != 0) {
                    quest_pick_file(78, 0, 48, 66, player_character->level);
                } else {
                    quest_pick_file(78, 0, 48, 67, player_character->level);
                }
                break;
            case 801:
                hud_message_add(D_001832B0);
                object_free_children((int)D_001960D9);
                trade_price_scale = ((10 - (guild_membership->rank)) << 8) / 10;
                inventory_open_container((int)D_001960D9, 4, 8);
                break;
            default:
                msgbox_show_string((int)D_00175EC1, 1);
            }
        }
        break;
    case 2:
        if (guild_membership != 0) {
            switch (a1->data.person.faction_id) {
            case 849:
                training_offer((int)D_00186F74);
                break;
            case 850:
                trade_price_scale = ((10 - (guild_membership->rank)) << 8) / 10;
                shop_open_repair(255, a1);
                break;
            case 851:
                if (a1->quest_id == 0) {
                    quest_pick_file(77, 0, 48, 66, guild_membership->rank);
                } else {
                    npc_talk(a1);
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            if (a1->data.person.faction_id == 851) {
                if (a1->quest_id == 0) {
                    quest_pick_file(77, 0, 48, 67, guild_membership->rank);
                } else {
                    npc_talk(a1);
                }
            } else {
                npc_talk(a1);
            }
        }
        break;
    case 3:
        if (guild_membership == 0) {
        } else {
            switch ((unsigned short)(a1->data.person.faction_id - 803)) {
            case 0:
                training_offer((int)D_00186F4C);
                break;
            case 1:
                if (a1->quest_id == 0) {
                    quest_pick_file(79, 0, 48, 66, guild_membership->rank);
                } else {
                    npc_talk(a1);
                }
                break;
            case 2:
                if ((guild_membership->rank) < 2) {
                    l_24 = 3100;
                    break;
                }
                trade_price_scale = 128;
                object_free_children((int)D_001960D9);
                inventory_open_container((int)D_001960D9, 2, 6);
                break;
            case 3:
                if ((guild_membership->rank) < 4) {
                    l_24 = 3100;
                    break;
                }
                msgbox_show_rsc(402, 1);
                npc_talk(a1);
                break;
            default:
                npc_talk(a1);
            }
        }
        break;
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
        if (guild_membership != 0) {
            switch (a1->data.person.faction_id) {
            case 845:
                if (((1 << (guild_membership->rank)) & guild_membership->armor_received) != 0) {
                    msgbox_show_rsc(461, 1);
                    break;
                }
                guild_membership->armor_received |= 1 << (guild_membership->rank);
                l_38 = object_create_child(a1, 0, 107);
                l_38->type = 2;
                forced_material = guild_membership->rank - 1;
                if (((int)(unsigned char)forced_material) > 100) {
                    forced_material = 2;
                } else if (((int)(unsigned char)forced_material) > 10) {
                    forced_material = 10;
                }
                item_make_in_range(2, 0, 6, &l_38->data.item);
                l_38->data.item.armor_type = 2;
                l_38->x = player_object->x;
                l_38->y = player_object->y;
                l_38->z = player_object->z;
                inv_store_item(l_38);
                msgbox_show_rsc(463, 1);
                break;
            case 848:
                if (player_character->house != 0) break;
                if ((guild_membership->rank) != 9) {
                    msgbox_show_rsc(460, 1);
                    break;
                }
                object_foreach(location_object, (int)bank_add_house_for_sale);
                if (bank_house_count == 0) break;
                l_30 = rand_range(0, (unsigned char)bank_house_count - 1);
                player_character->house = bank_houses_for_sale[l_30].f12;
                D_001A41E4 = (struct record *)(bank_houses_for_sale[l_30].p - 71);
                D_001A41DC = bank_houses_for_sale[l_30].f4;
                msgbox_show_rsc(462, 1);
                mc_strncpy((int)saved_region_name, *(int *)(region_names + (((int)(unsigned char)current_region) << 2)), 32, (int)D_00175EAA, 413);
                mc_strncpy((int)saved_location_name, (int)current_location, 32, (int)D_00175EAA, 414);
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 846:
                if (a1->quest_id != 0) {
                    npc_talk(a1);
                } else if (guild_membership != 0) {
                    quest_pick_file(66, 0, 48, 66, player_character->level);
                } else {
                    quest_pick_file(66, 0, 48, 67, player_character->level);
                }
                break;
            default:
                msgbox_show_string((int)D_00175EEC, 1);
            }
        }
        break;
    case 142:
        if (guild_membership == 0) {
        } else {
            guild_heal();
            switch ((unsigned short)(a1->data.person.faction_id - 453)) {
            case 0:
                if ((guild_membership->rank) >= 1) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 1:
                if ((guild_membership->rank) >= 4) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 2:
                if ((guild_membership->rank) >= 4) {
                    guild_buy_soulgems();
                } else {
                    l_24 = 3100;
                }
                break;
            case 3:
                if ((guild_membership->rank) >= 7) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
            goto L6E949;
        }
        l_20 = 1;
L6E949:;
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 241:
                training_offer((int)D_00186F88);
                break;
            case 240:
                guild_temple_quest();
                break;
            case 810:
                guild_donate();
                break;
            case 813:
                guild_cure_diseases();
                break;
            default:
                msgbox_show_string((int)D_00175F17, 1);
            }
        }
        break;
    case 143:
        if (guild_membership != 0) {
            if ((guild_membership->rank) >= 2) guild_heal();
            switch (a1->data.person.faction_id) {
            case 462:
                if ((guild_membership->rank) >= 1) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 463:
                if ((guild_membership->rank) >= 6) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 464:
                if ((guild_membership->rank) >= 8) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 243:
                training_offer((int)D_00186F9E);
                break;
            case 810:
                guild_donate();
                break;
            case 460:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00175F42, 1);
            }
        }
        break;
    case 144:
        if (guild_membership != 0) {
            if ((guild_membership->rank) >= 1) guild_heal();
            switch (a1->data.person.faction_id) {
            case 468:
                if ((guild_membership->rank) >= 2) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 469:
                if ((guild_membership->rank) >= 5) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 470:
                if ((guild_membership->rank) >= 7) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 245:
                training_offer((int)D_00186FB6);
                break;
            case 810:
                guild_donate();
                break;
            case 466:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00175F6D, 1);
            }
        }
        break;
    case 145:
        if (guild_membership != 0) {
            if ((guild_membership->rank) >= 1) guild_heal();
            switch (a1->data.person.faction_id) {
            case 473:
                if ((guild_membership->rank) >= 4) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 474:
                if ((guild_membership->rank) >= 5) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 475:
                if ((guild_membership->rank) >= 7) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 247:
                training_offer((int)D_00186FC9);
                break;
            case 810:
                guild_donate();
                break;
            case 471:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00175F98, 1);
            }
        }
        break;
    case 146:
        if (guild_membership != 0) {
            if ((guild_membership->rank) >= 2) guild_heal();
            switch (a1->data.person.faction_id) {
            case 480:
                if ((guild_membership->rank) >= 3) {
                    guild_buy_magic_items();
                } else {
                    l_24 = 3100;
                }
                break;
            case 481:
                if ((guild_membership->rank) >= 5) {
                    itemmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 482:
                if ((guild_membership->rank) >= 6) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 249:
                training_offer((int)D_00186FDB);
                break;
            case 810:
                guild_donate();
                break;
            case 477:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00175FC3, 1);
            }
        }
        break;
    case 147:
        if (guild_membership != 0) {
            if ((guild_membership->rank) >= 2) guild_heal();
            switch (a1->data.person.faction_id) {
            case 485:
                if ((guild_membership->rank) >= 1) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 487:
                if ((guild_membership->rank) >= 5) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 488:
                if ((guild_membership->rank) >= 7) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 250:
                training_offer((int)D_00186FEE);
                break;
            case 810:
                guild_donate();
                break;
            case 484:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00175FEE, 1);
            }
        }
        break;
    case 148:
        if (guild_membership != 0) {
            guild_heal();
            switch (a1->data.person.faction_id) {
            case 490:
                if ((guild_membership->rank) >= 2) {
                    guild_buy_potions();
                } else {
                    l_24 = 3100;
                }
                break;
            case 491:
                if ((guild_membership->rank) >= 5) {
                    potionmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 492:
                if ((guild_membership->rank) >= 7) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 252:
                training_offer((int)D_00187001);
                break;
            case 810:
                guild_donate();
                break;
            case 493:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00176019, 1);
            }
        }
        break;
    case 149:
        if (guild_membership != 0) {
            if ((guild_membership->rank) >= 1) guild_heal();
            switch (a1->data.person.faction_id) {
            case 496:
                if ((guild_membership->rank) >= 3) {
                    guild_buy_spells();
                } else {
                    l_24 = 3100;
                }
                break;
            case 497:
                if ((guild_membership->rank) >= 6) {
                    spellmaker_open(1);
                } else {
                    l_24 = 3100;
                }
                break;
            case 498:
                if ((guild_membership->rank) >= 7) {
                    daedra_summon(a1);
                } else {
                    l_24 = 3100;
                }
                break;
            default:
                l_20 = 1;
            }
        } else {
            l_20 = 1;
        }
        if (l_20 != 0) {
            switch (a1->data.person.faction_id) {
            case 254:
                training_offer((int)D_00187017);
                break;
            case 810:
                guild_donate();
                break;
            case 494:
                guild_buy_blessing();
                break;
            case 813:
                guild_cure_diseases();
                break;
            case 240:
                guild_temple_quest();
                break;
            default:
                msgbox_show_string((int)D_00176044, 1);
            }
        }
    }
    if (l_24 != 0) msgbox_show_rsc((int)(short)*(short *)&l_24, 1);
    if (D_001A4A1C == 0 || D_00196118 == 0) return;
    D_001A4A1C = 0;
    inventory_open_container((int)D_001960D9, 0, 6);
}
