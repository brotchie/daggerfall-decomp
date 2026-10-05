/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008A113 */
#include "records.h"

extern int game_minutes;

int spfx_shield(struct record *spell, int slot, struct record *target)
{
    struct character *target_char;
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    target_char = &target->data.character;
    target_char->conditions |= 0x400000;
    target_char->shield_points = spell_data->cast_magnitudes[slot];
    target_char->shield_end_time = spell_data->cast_durations[slot] + game_minutes;
    return 1;
}
