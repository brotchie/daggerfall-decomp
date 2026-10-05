/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x70AFA to 0x70EC0, kept together for its switch table's alignment */
#include "records.h"

extern unsigned char key_down_enter;
extern int D_00185077;
extern unsigned char *D_00187545;
extern struct region regions[];
extern int inpstr_result;
extern struct record *scratch_object;
extern struct character *player_character;
extern unsigned char current_region;
extern struct faction *D_0019671C;
extern struct membership *guild_membership;
extern void msgbox_show_rsc(int, int);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void npc_talk(struct record *);
extern int rand_range(int, int);
extern void msgbox_prompt_number(int, int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern int abs(short);

void blessing_remove(struct blessing *blessing)
{
    if (blessing->target == 255) {
        regions[blessing->region].legal_reputation -= blessing->amount;
        return;
    }
    if (blessing->target & 128) {
        player_character->attributes[blessing->target & 127] -= blessing->amount;
        return;
    }
    player_character->skills[blessing->target].value -= blessing->amount;
}

int blessing_apply(struct blessing *blessing, int amount)
{
    int applied;

    if (blessing->target == 255) {
        regions[current_region].legal_reputation += amount;
        if (regions[current_region].legal_reputation > 100) {
            applied = amount - (regions[current_region].legal_reputation - 100);
            regions[current_region].legal_reputation = 100;
        }
    } else if (blessing->target & 128) {
        player_character->attributes[blessing->target & 127] += amount;
        if (player_character->attributes[blessing->target & 127] > 100) {
            applied = amount - (player_character->attributes[blessing->target & 127] - 100);
            player_character->attributes[blessing->target & 127] = 100;
        }
    } else {
        player_character->skills[blessing->target].value += amount;
        if (player_character->skills[blessing->target].value > 100) {
            applied = amount - (player_character->skills[blessing->target].value - 100);
            player_character->skills[blessing->target].value = 100;
        }
    }
    return amount;
}

void guild_donate(void)
{
    msgbox_prompt_number(1000, D_00185077);
    while (key_down_enter != 0)
        ;
    if (inpstr_result < 1)
        return;
    if (gold_can_afford(inpstr_result) == 0) {
        msgbox_show_rsc(702, 1);
        return;
    }
    gold_spend(inpstr_result);
    if (rand_range(1, 100) <= inpstr_result * 2 / (abs(D_0019671C->reputation) + 1))
        D_0019671C->reputation++;
    msgbox_show_rsc(703, 1);
}

void guild_temple_quest(void)
{
    if (scratch_object->quest_id != 0) {
        npc_talk(scratch_object);
        return;
    }
    if (guild_membership != 0) {
        quest_pick_file(*(D_00187545 - 142 + guild_membership->kind), 67, 48, 66, guild_membership->rank);
        return;
    }
    quest_pick_file(*(D_00187545 - 142 + guild_membership->kind), 67, 48, 67, player_character->level);
}

int guild_kind_of_faction(struct faction *faction)
{
    switch (faction->id) {
    case 108:
        return 0;
    case 42:
        return 3;
    case 40:
        return 1;
    case 41:
        return 2;
    case 21:
    case 82:
        return 142;
    case 22:
    case 84:
        return 143;
    case 24:
    case 28:
        return 144;
    case 26:
    case 92:
        return 145;
    case 27:
    case 94:
        return 146;
    case 29:
    case 98:
        return 147;
    case 33:
    case 106:
        return 148;
    case 35:
    case 36:
        return 149;
    case 368:
        return 68;
    case 408:
        return 69;
    case 409:
        return 70;
    case 410:
        return 71;
    case 411:
        return 72;
    case 413:
        return 73;
    case 414:
        return 74;
    case 415:
        return 75;
    case 416:
        return 76;
    case 417:
        return 77;
    default:
        return -1;
    }
}
