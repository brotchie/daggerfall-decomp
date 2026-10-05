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
extern int D_00190BE4[];
extern int D_00190CAC;
extern short D_00190D68;
extern short D_00190D6A;
extern struct record *player_entity;
extern struct character *player_character;
extern char D_001962AB;
extern struct faction *faction_find(short);
extern short bio_person_add(struct character *, char *, int);
extern unsigned char *career_skip_word(unsigned char *);
extern void func_000252C7(int);
extern void func_000252E2(int);
extern void parse_expand(char *, struct character *);
extern void item_make(unsigned short, int, struct item *);
extern int disk_read_file(char *, char *);
extern struct record *object_create_child(struct record *, int, int);
extern void inv_store_item(struct record *);
extern void inv_merge_arrows(struct record *, struct record *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern int atoi(unsigned char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

unsigned char *career_answer_effect(unsigned char *a1)
{
    int n;
    struct record *o;
    struct faction *f;
    struct character *p;
    char *q;
    struct item *m;
    unsigned char c;

    while (*a1 <= 32)
        a1++;
    if (*(unsigned short *)a1 == 0x5047)
        player_character->gold += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x6672) {
        a1 += 2;
        f = faction_find(atoi(a1));
        if (f != 0)
            f->reputation += atoi(career_skip_word(a1));
    } else if (*(unsigned short *)a1 == 0x7272)
        region_legal_reputation[D_00190D68].f0 += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x494D) {
        a1 = career_skip_word(a1);
        if (*a1 == '+')
            func_000252C7(atoi(a1));
        else
            func_000252E2(atoi(a1));
    } else if (*(unsigned short *)a1 == 0x5252)
        player_character->reputation_mod += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x4452)
        bio_modifiers += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x524D)
        D_0018DDD8 += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x4854)
        D_0018DDDC += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x5052)
        D_0018DDE0 += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x5446)
        D_0018DDE4 += atoi(career_skip_word(a1));
    else if (*(unsigned short *)a1 == 0x4541 || *(unsigned short *)a1 == 0x4641 || *(unsigned short *)a1 == 0x4F41) {
        c = a1[1];
        p = (struct character *)(D_00147954 + 70000);
        q = D_00147954 + 75000;
        mc_memset(p, 0, 560, D_00170738, 279, 4);
        mc_memset(q, 0, 74, D_00170738, 280, 4);
        a1 = career_skip_word(a1);
        if (*a1 == 'F')
            p->flags |= 1;
        else if (*a1 == 'O')
            p->flags |= (player_character->flags & 1) ^ 1;
        a1 = career_skip_word(a1);
        p->race = atoi(a1);
        a1 = career_skip_word(a1);
        n = atoi(a1);
        p->career_id = n;    /* the person's class: CLASS%02d.CFG */
        a1 = career_skip_word(a1);
        p->level = atoi(a1);
        func_000A0ED9(292, D_00170738);
        mc_sprintf(((char *)text_buffer), D_00170765, n);
        disk_read_file(((char *)text_buffer), q);
        if (c == 'E') {
            parse_expand(D_00170773, p);
            bio_person_add(p, q, 0);
        } else {
            parse_expand(D_00170777, p);
            bio_person_add(p, q, 1);
        }
    } else if (*(unsigned short *)a1 == 0x5449) {
        a1 = career_skip_word(a1);
        n = atoi(a1);
        o = object_create_child(player_entity, 0, 107);
        o->type = 2;
        o->flags = 1;
        a1 = career_skip_word(a1);
        m = &o->data.item;
        D_001962AB = atoi(career_skip_word(a1)) + 1;
        item_make(n, atoi(a1), m);
        if (m->group == 3 && m->index == 18) {
            m->stack_count = 1;
            inv_merge_arrows(player_entity, o, 1);
        } else
            inv_store_item(o);
    } else if (*a1 == '&')
        D_00190D6A = 0;
    else if (*a1 == '#')
        D_00190BE4[D_00190CAC] = atoi(a1 + 1);
    else if (*a1 == '!')
        D_00190BE4[D_00190CAC + 12] = atoi(a1 + 1);
    else if (*a1 == '?')
        D_00190BE4[D_00190CAC + 24] = atoi(a1 + 1);
    else if (D_00178630[(unsigned char)(*a1 + 1)] & 0x20) {
        n = atoi(a1);
        if (n >= 35)
            n = 0;
        player_character->skills[n].value += atoi(career_skip_word(a1));
    } else if (*a1 == 'r' && D_00178630[(unsigned char)(a1[1] + 1)] & 0x20) {
        n = atoi(a1 + 1);
        player_character->reputation[n] += atoi(career_skip_word(a1));
    }
    while (*a1 != '\n')
        a1++;
    a1++;
    return a1;
}
