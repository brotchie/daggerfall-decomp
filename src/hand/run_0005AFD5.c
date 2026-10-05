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
extern int hud_message_add(char *);
extern void spell_cast_queued_run(void);

int cast_player_spell(struct record *a1)
{
    struct spell *l_1C;

    l_1C = &a1->data.spell;
    a1->caster = player_entity;
    spell_last_cast_id = l_1C->id;
    if (l_1C->id == 92) {
        disease_lycanthrope_shapechange(0);
        return 1;
    }
    if (D_0019629A == 0 && (player_character->conditions & 0x100) != 0)
        return 1;
    spell_cast_busy = 130 - player_character->attributes[ATTR_INT] * 50;
    quests_raise_event_all(73, a1, 0);
    switch (l_1C->target) {
    case 0:
        cast_spell_on(a1, player_entity, 0);
        return 1;
    case 1:
        hud_message_add(D_001842D5);
        spell_ready_touch = a1;
        return 0;
    case 2:
        hud_message_add(D_001842D5);
        spell_ready_missile = a1;
        return 0;
    case 3:
        hud_message_add(D_001842D5);
        spell_ready_missile = a1;
        return 0;
    case 4:
        hud_message_add(D_001842D5);
        spell_ready_missile = a1;
        return 0;
    default:
        return 1;
    }
}

int cast_item_spell_at(struct record *a1, struct record *a2)
{
    struct spell *l_18;

    l_18 = &a1->data.spell;
    a1->caster = player_entity;
    if (l_18->id == 92) {
        disease_lycanthrope_shapechange(0);
        return 1;
    }
    switch (l_18->target) {
    case 0:
        cast_spell_on(a1, player_entity, 0);
        return 1;
    case 1:
        cast_spell_on(a1, a2, 0);
        return 1;
    case 2:
        cast_spell_on(a1, a2, 0);
        return 1;
    case 3:
        spell_cast_queue_count = 0;
        a1->x = player_object->x;
        a1->y = player_object->y;
        a1->z = player_object->z;
        spell_area_effect(a1);
        spell_cast_queued_run();
        return 1;
    case 4:
        return 1;
    }
    return 1;
}
