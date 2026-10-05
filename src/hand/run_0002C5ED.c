/* matched by the real Watcom C32 10.0a (-d2): a run of qcom.c from 0x0002C4FA to 0x0002C5ED, kept together for its switch table's alignment */
#include "records.h"

/* the timer flags are read signed here: (short)t->flags */
extern short travel_options;
extern char D_0017A13C[];
extern int game_minutes;
extern struct quest *current_quest;
extern void quest_timer_update(struct quest *, struct qbn_timer *, short);
extern struct record *quest_place_or_person_object(struct quest *, struct record *, short);
extern void quest_timer_expire(struct quest *, struct qbn_timer *);
extern void quest_timer_clear_state(struct qbn_timer *);
extern unsigned int quest_travel_minutes(struct quest *, struct record *, struct record *);
extern struct qbn_timer *quest_section(struct quest *, int);
extern int rand_range(int, int);
extern int memchr(char *, int, int);

void quest_timers_update(struct quest *a1)
{
    int i;
    struct qbn_timer *t;

    t = quest_section(a1, 6);
    for (i = 0; a1->section_counts[6] > i; i++, t++) {
        if ((short)t->flags & 2) {
            t->flags &= ~128;
            quest_timer_clear_state(t);
        }
        if ((short)t->flags & 64)
            quest_timer_update(a1, t, 0);
    }
}

void quest_timers_start_all(void)
{
    struct qbn_timer *t;
    int i;

    t = quest_section(current_quest, 6);
    for (i = 0; current_quest->section_counts[6] > i; i++, t++)
        quest_timer_update(current_quest, t, 1);
}

void quest_timer_update(struct quest *a1, struct qbn_timer *a2, short a3)
{
    int l_18;
    short saved;

    saved = travel_options;
    if (a3) {
        travel_options = 537;
        if (!((short)a2->flags & 1024)) {
            a2->flags |= 1024;
            switch (a2->type) {
            case 2:
            case 4:
                a2->link1 = quest_place_or_person_object(a1, a2->link1, (short)a2->flags & 256);
                a2->link2 = 0;
                break;
            case 3:
            case 5:
                a2->link1 = quest_place_or_person_object(a1, a2->link1, (short)a2->flags & 256);
                a2->link2 = quest_place_or_person_object(a1, a2->link2, (short)a2->flags & 512);
                break;
            default:
                a2->link1 = a2->link2 = 0;
                break;
            }
        }
        a2->start = game_minutes;
        switch (a2->type) {
        case 0:
            a2->delay = rand_range(a2->minimum, a2->maximum);
            break;
        case 1:
            a2->delay = a2->minimum;
            break;
        case 2:
            a2->delay = quest_travel_minutes(a1, 0, a2->link1) * 384 >> 8;
            if (memchr(D_0017A13C, a2->link1->link_flag, 5))
                a2->delay += 10080;
            break;
        case 3:
            a2->delay = quest_travel_minutes(a1, a2->link1, a2->link2) * 384 >> 8;
            if (memchr(D_0017A13C, a2->link1->link_flag, 5) || memchr(D_0017A13C, a2->link2->link_flag, 5))
                a2->delay += 10080;
            break;
        case 4:
            a2->delay = quest_travel_minutes(a1, 0, a2->link1) * 384 >> 8;
            if (memchr(D_0017A13C, a2->link1->link_flag, 5))
                a2->delay += 10080;
            break;
        case 5:
            a2->delay = quest_travel_minutes(a1, 0, a2->link1) * 384 >> 8;
            a2->delay += quest_travel_minutes(a1, a2->link1, a2->link2) * 384 >> 8;
            if (memchr(D_0017A13C, a2->link1->link_flag, 5))
                a2->delay += 10080;
            if (memchr(D_0017A13C, a2->link2->link_flag, 5))
                a2->delay += 10080;
            break;
        }
        if ((short)a2->flags & 16)
            a2->delay <<= 1;
        travel_options = saved;
    } else if (game_minutes - a2->start > a2->delay) {
        quest_timer_expire(a1, a2);
    }
}
