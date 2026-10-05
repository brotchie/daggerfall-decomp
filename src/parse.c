/* parse.c */

#include "dagger.h"

int macro_bn_biography_name(void)
{
    int l1;
    int l2;
    l1 = rand();
    srand(parse_name_seed + 0xd81);
    l2 = name_generate(player_character[0x43], D_00190C78 & 1);
    srand(l1);
    return l2;
}

int macro_fae_player_ally_npc_enemy(void) { return scratch_190df4 + 3; }

int macro_pnq_blank(void) { return text_blank; }
