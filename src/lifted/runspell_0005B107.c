/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *player_entity;

extern void damage_spawn_splash(struct record *, int, int);
extern void cast_spell_on(struct record *, struct record *, int);
extern void cast_creature_missile(struct record *, struct record *, struct record *);

int cast_creature_spell_at(struct record *spell, struct record *caster, struct record *target)
{
    struct spell *spell_data;

    spell_data = &spell->data.spell;
    spell->caster = caster;
    switch (spell_data->target) {
    case 0:
        cast_spell_on(spell, caster, 0);
        if (caster != player_entity) damage_spawn_splash(caster, 3, 3);
        return 1;
    case 1:
        cast_spell_on(spell, target, 0);
        return 1;
    case 2:
        cast_creature_missile(spell, caster, target);
        return 0;
    case 3:
        cast_spell_on(spell, target, 0);
        return 1;
    case 4:
        cast_creature_missile(spell, caster, target);
        return 0;
    default:
        return 1;
    }
}
