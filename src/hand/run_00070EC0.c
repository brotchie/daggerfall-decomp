/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x70AFA to 0x70EC0, kept together for its switch table's alignment */
struct rep { short value; char pad[78]; };
extern unsigned char key_down_enter;
extern int D_00185077;
extern unsigned char *D_00187545;
extern struct rep region_legal_reputation[];
extern int inpstr_result;
extern unsigned char *guild_npc_object;
extern unsigned char *player_character;
extern unsigned char current_region;
extern unsigned char *D_0019671C;
extern unsigned char *guild_membership;
extern void msgbox_show_rsc(int, int);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void npc_talk(unsigned char *);
extern int rand_range(int, int);
extern void msgbox_prompt_number(int, int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern int func_0009DEAC(short);

void blessing_remove(unsigned char *a1)
{
    if (a1[0] == 255) {
        region_legal_reputation[a1[6]].value -= a1[1];
        return;
    }
    if (a1[0] & 128) {
        *(short *)(player_character + 32 + (a1[0] & 127) * 2) -= a1[1];
        return;
    }
    *(short *)(player_character + 157 + a1[0] * 6) -= a1[1];
}

int blessing_apply(unsigned char *a1, int a2)
{
    int l_18;

    if (a1[0] == 255) {
        region_legal_reputation[current_region].value += a2;
        if (region_legal_reputation[current_region].value > 100) {
            l_18 = a2 - (region_legal_reputation[current_region].value - 100);
            region_legal_reputation[current_region].value = 100;
        }
    } else if (a1[0] & 128) {
        *(short *)(player_character + 32 + (a1[0] & 127) * 2) += a2;
        if (*(short *)(player_character + 32 + (a1[0] & 127) * 2) > 100) {
            l_18 = a2 - (*(short *)(player_character + 32 + (a1[0] & 127) * 2) - 100);
            *(short *)(player_character + 32 + (a1[0] & 127) * 2) = 100;
        }
    } else {
        *(short *)(player_character + 157 + a1[0] * 6) += a2;
        if (*(short *)(player_character + 157 + a1[0] * 6) > 100) {
            l_18 = a2 - (*(short *)(player_character + 157 + a1[0] * 6) - 100);
            *(short *)(player_character + 157 + a1[0] * 6) = 100;
        }
    }
    return a2;
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
    if (rand_range(1, 100) <= inpstr_result * 2 / (func_0009DEAC(*(short *)(D_0019671C + 29)) + 1))
        (*(short *)(D_0019671C + 29))++;
    msgbox_show_rsc(703, 1);
}

void guild_temple_quest(void)
{
    if (guild_npc_object[38] != 0) {
        npc_talk(guild_npc_object);
        return;
    }
    if (guild_membership != 0) {
        quest_pick_file(*(D_00187545 - 142 + guild_membership[2]), 67, 48, 66, guild_membership[0]);
        return;
    }
    quest_pick_file(*(D_00187545 - 142 + guild_membership[2]), 67, 48, 67, player_character[129]);
}

int guild_kind_of_faction(unsigned char *a1)
{
    switch (*(unsigned short *)(a1 + 33)) {
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
