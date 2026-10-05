/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00024C88 */
#include "records.h"

struct dun { short f0; char pad[78]; };
extern char *D_00147954;
extern char D_00170738[];        /* __FILE__ */
extern char D_00170765[];
extern char D_00170773[];
extern char D_00170777[];
extern unsigned char D_00178630[];   /* _IsTable */
extern int bio_modifiers;
extern int D_0018DDD8;
extern int D_0018DDDC;
extern int D_0018DDE0;
extern int D_0018DDE4;
extern struct dun region_legal_reputation[];
extern signed char text_buffer[];
extern int scratch_190be4[];
extern int scratch_190cac;
extern short scratch_190d68;
extern short scratch_190d6a;
extern struct record *player_entity;
extern struct character *player_character;
extern char forced_material;
extern struct faction *faction_find(short);
extern unsigned short bio_person_add(struct character *, char *, int);
extern unsigned char *career_skip_word(unsigned char *);
extern void func_000252C7(int);
extern void func_000252E2(int);
extern void parse_expand(char *, struct character *);
extern void item_make(unsigned short, int, struct item *);
extern int disk_read_file(char *, char *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern void inv_store_item(struct record *);
extern void inv_merge_arrows(struct record *, struct record *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern int atoi(unsigned char *);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

unsigned char *career_answer_effect(unsigned char *text)
{
    int number;
    struct record *object;
    struct faction *faction;
    struct character *person;
    char *person_class;
    struct item *item;
    unsigned char kind;

    while (*text <= 32)
        text++;
    if (*(unsigned short *)text == 0x5047)
        player_character->gold += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x6672) {
        text += 2;
        faction = faction_find(atoi(text));
        if (faction != 0)
            faction->reputation += atoi(career_skip_word(text));
    } else if (*(unsigned short *)text == 0x7272)
        region_legal_reputation[scratch_190d68].f0 += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x494D) {
        text = career_skip_word(text);
        if (*text == '+')
            func_000252C7(atoi(text));
        else
            func_000252E2(atoi(text));
    } else if (*(unsigned short *)text == 0x5252)
        player_character->reputation_mod += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x4452)
        bio_modifiers += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x524D)
        D_0018DDD8 += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x4854)
        D_0018DDDC += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x5052)
        D_0018DDE0 += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x5446)
        D_0018DDE4 += atoi(career_skip_word(text));
    else if (*(unsigned short *)text == 0x4541 || *(unsigned short *)text == 0x4641 || *(unsigned short *)text == 0x4F41) {
        kind = text[1];
        person = (struct character *)(D_00147954 + 70000);
        person_class = D_00147954 + 75000;
        mc_memset(person, 0, 560, D_00170738, 279, 4);
        mc_memset(person_class, 0, 74, D_00170738, 280, 4);
        text = career_skip_word(text);
        if (*text == 'F')
            person->flags |= 1;
        else if (*text == 'O')
            person->flags |= (player_character->flags & 1) ^ 1;
        text = career_skip_word(text);
        person->race = atoi(text);
        text = career_skip_word(text);
        number = atoi(text);
        person->career_id = number;    /* the person's class: CLASS%02d.CFG */
        text = career_skip_word(text);
        person->level = atoi(text);
        mc_set_location(292, D_00170738);
        mc_sprintf(((char *)text_buffer), D_00170765, number);
        disk_read_file(((char *)text_buffer), person_class);
        if (kind == 'E') {
            parse_expand(D_00170773, person);
            bio_person_add(person, person_class, 0);
        } else {
            parse_expand(D_00170777, person);
            bio_person_add(person, person_class, 1);
        }
    } else if (*(unsigned short *)text == 0x5449) {
        text = career_skip_word(text);
        number = atoi(text);
        object = object_create_child(player_entity, 0, 107);
        object->type = 2;
        object->flags = 1;
        text = career_skip_word(text);
        item = &object->data.item;
        forced_material = atoi(career_skip_word(text)) + 1;
        item_make(number, atoi(text), item);
        if (item->group == 3 && item->index == 18) {
            item->stack_count = 1;
            inv_merge_arrows(player_entity, object, 1);
        } else
            inv_store_item(object);
    } else if (*text == '&')
        scratch_190d6a = 0;
    else if (*text == '#')
        scratch_190be4[scratch_190cac] = atoi(text + 1);
    else if (*text == '!')
        scratch_190be4[scratch_190cac + 12] = atoi(text + 1);
    else if (*text == '?')
        scratch_190be4[scratch_190cac + 24] = atoi(text + 1);
    else if (D_00178630[(unsigned char)(*text + 1)] & 0x20) {
        number = atoi(text);
        if (number >= 35)
            number = 0;
        player_character->skills[number].value += atoi(career_skip_word(text));
    } else if (*text == 'r' && D_00178630[(unsigned char)(text[1] + 1)] & 0x20) {
        number = atoi(text + 1);
        player_character->reputation[number] += atoi(career_skip_word(text));
    }
    while (*text != '\n')
        text++;
    text++;
    return text;
}
