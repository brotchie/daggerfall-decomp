/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009190C */
#include "records.h"
#include "doslow.h"

extern unsigned scratch_190be4;
extern signed char scratch_190ce4[];
extern short scratch_190d64;
extern short chargen_selected_attribute;
extern struct character *player_character;

void chargen_attribute_arrow(int button)
{
    if (*(unsigned *)DOS_LOW(0x46C) - scratch_190be4 < 6)
        return;
    scratch_190be4 = *(unsigned *)DOS_LOW(0x46C);
    if (button == 30) {
        if (scratch_190d64 != 0) {
            if (player_character->attributes[chargen_selected_attribute] == 100)
                return;
            scratch_190d64--;
            player_character->attributes[chargen_selected_attribute]++;
        }
        return;
    }
    if (player_character->attributes[chargen_selected_attribute] <= scratch_190ce4[chargen_selected_attribute])
        return;
    if (player_character->attributes[chargen_selected_attribute] == 10)
        return;
    scratch_190d64++;
    player_character->attributes[chargen_selected_attribute]--;
}
