/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009190C */
struct pc { char pad[0x20]; short stats[8]; };
extern unsigned D_00190BE4;
extern signed char itemmaker_slot_kinds[];
extern short D_00190D64;
extern short chargen_selected_attribute;
extern struct pc *player_character;

void chargen_attribute_arrow(int a1)
{
    if (*(unsigned *)0x46c - D_00190BE4 < 6)
        return;
    D_00190BE4 = *(unsigned *)0x46c;
    if (a1 == 30) {
        if (D_00190D64 != 0) {
            if (player_character->stats[chargen_selected_attribute] == 100)
                return;
            D_00190D64--;
            player_character->stats[chargen_selected_attribute]++;
        }
        return;
    }
    if (player_character->stats[chargen_selected_attribute] <= itemmaker_slot_kinds[chargen_selected_attribute])
        return;
    if (player_character->stats[chargen_selected_attribute] == 10)
        return;
    D_00190D64++;
    player_character->stats[chargen_selected_attribute]--;
}
