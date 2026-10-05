/* matched by the real Watcom C32 10.0a (-d2): a run of runspell from 0x5AE5F to 0x5AFD5, kept together for its switch table's alignment */
struct flags138 { unsigned char b0:1; };
extern char *D_001842D5;
extern short spell_last_cast_id;
extern int player_entity;
extern char *player_object;
extern char *spell_ready_missile;
extern char *spell_ready_touch;
extern char *player_character;
extern int D_00195D5C;
extern int D_00195D60;
extern char D_0019629A;
extern void quests_raise_event_all(int, char *, int);
extern void cast_spell_on(char *, int, int);
extern void spell_area_effect(char *);
extern void disease_lycanthrope_shapechange(int);
extern int hud_message_add(char *);
extern void spell_cast_queued_run(void);

int cast_player_spell(char *a1)
{
    unsigned char *l_1C;

    l_1C = (unsigned char *)a1 + 71;
    *(int *)(a1 + 47) = player_entity;
    spell_last_cast_id = l_1C[73];
    if (l_1C[73] == 92) {
        disease_lycanthrope_shapechange(0);
        return 1;
    }
    if (D_0019629A == 0 && ((struct flags138 *)(player_character + 138))->b0 != 0)
        return 1;
    D_00195D60 = 130 - *(short *)(player_character + 34) * 50;
    quests_raise_event_all(73, a1, 0);
    switch (l_1C[7]) {
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

int cast_item_spell_at(char *a1, int a2)
{
    unsigned char *l_18;

    l_18 = (unsigned char *)a1 + 71;
    *(int *)(a1 + 47) = player_entity;
    if (l_18[73] == 92) {
        disease_lycanthrope_shapechange(0);
        return 1;
    }
    switch (l_18[7]) {
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
        D_00195D5C = 0;
        *(int *)(a1 + 7) = *(int *)(player_object + 7);
        *(int *)(a1 + 11) = *(int *)(player_object + 11);
        *(int *)(a1 + 15) = *(int *)(player_object + 15);
        spell_area_effect(a1);
        spell_cast_queued_run();
        return 1;
    case 4:
        return 1;
    }
    return 1;
}
