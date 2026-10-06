/* matched by the real Watcom C32 10.0a (-d2): a run of runspell from 0x5AE5F to 0x5AFD5, kept together for its switch table's alignment */
#include "records.h"

extern char *D_001842D5;
extern short spell_last_cast_id;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *spell_ready_missile;
extern struct record *spell_ready_touch;
extern struct character *player_character;
extern int spell_cast_queue_count;
extern int spell_cast_busy;
extern char D_0019629A;
extern void quests_raise_event_all(int, struct record *, int);
extern void cast_spell_on(struct record *, struct record *, int);
extern void spell_area_effect(struct record *);
extern void disease_lycanthrope_shapechange(int);
extern iptr hud_message_add(char *);
extern void spell_cast_queued_run(void);

int cast_player_spell(struct record *spell_object)
{
    struct spell *spell;

    spell = &spell_object->data.spell;
    spell_object->caster = player_entity;
    spell_last_cast_id = spell->id;
    if (spell->id == 92) {
        disease_lycanthrope_shapechange(0);
        return 1;
    }
    if (D_0019629A == 0 && (player_character->conditions & 0x100) != 0)
        return 1;
    spell_cast_busy = 130 - player_character->attributes[ATTR_INT] * 50;
    quests_raise_event_all(73, spell_object, 0);
    switch (spell->target) {
    case 0:
        cast_spell_on(spell_object, player_entity, 0);
        return 1;
    case 1:
        hud_message_add(D_001842D5);
        spell_ready_touch = spell_object;
        return 0;
    case 2:
        hud_message_add(D_001842D5);
        spell_ready_missile = spell_object;
        return 0;
    case 3:
        hud_message_add(D_001842D5);
        spell_ready_missile = spell_object;
        return 0;
    case 4:
        hud_message_add(D_001842D5);
        spell_ready_missile = spell_object;
        return 0;
    default:
        return 1;
    }
}

int cast_item_spell_at(struct record *spell_object, struct record *target)
{
    struct spell *spell;

    spell = &spell_object->data.spell;
    spell_object->caster = player_entity;
    if (spell->id == 92) {
        disease_lycanthrope_shapechange(0);
        return 1;
    }
    switch (spell->target) {
    case 0:
        cast_spell_on(spell_object, player_entity, 0);
        return 1;
    case 1:
        cast_spell_on(spell_object, target, 0);
        return 1;
    case 2:
        cast_spell_on(spell_object, target, 0);
        return 1;
    case 3:
        spell_cast_queue_count = 0;
        spell_object->x = player_object->x;
        spell_object->y = player_object->y;
        spell_object->z = player_object->z;
        spell_area_effect(spell_object);
        spell_cast_queued_run();
        return 1;
    case 4:
        return 1;
    }
    return 1;
}
