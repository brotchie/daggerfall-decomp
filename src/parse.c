/* parse.c */

#include "dagger.h"

iptr macro_bn_biography_name(void)
{
    int saved_seed;
    iptr name;
    saved_seed = rand();
    srand(parse_name_seed + 0xd81);
    name = name_generate(player_character[0x43], D_00190C78 & 1);
    srand(saved_seed);
    return name;
}

iptr macro_fae_player_ally_npc_enemy(void) { return scratch_190df4 + 3; }

int macro_pnq_blank(void) { return text_blank; }
