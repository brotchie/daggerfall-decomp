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
extern iptr memchr(char *, int, int);

void quest_timers_update(struct quest *quest)
{
    int i;
    struct qbn_timer *timer;

    timer = quest_section(quest, 6);
    for (i = 0; quest->section_counts[6] > i; i++, timer++) {
        if ((short)timer->flags & 2) {
            timer->flags &= ~128;
            quest_timer_clear_state(timer);
        }
        if ((short)timer->flags & 64)
            quest_timer_update(quest, timer, 0);
    }
}

void quest_timers_start_all(void)
{
    struct qbn_timer *timer;
    int i;

    timer = quest_section(current_quest, 6);
    for (i = 0; current_quest->section_counts[6] > i; i++, timer++)
        quest_timer_update(current_quest, timer, 1);
}

void quest_timer_update(struct quest *quest, struct qbn_timer *timer, short start)
{
    int unused;
    short saved_travel_options;

    saved_travel_options = travel_options;
    if (start) {
        travel_options = 537;
        if (!((short)timer->flags & 1024)) {
            timer->flags |= 1024;
            switch (timer->type) {
            case 2:
            case 4:
                timer->link1 = quest_place_or_person_object(quest, timer->link1, (short)timer->flags & 256);
                timer->link2 = 0;
                break;
            case 3:
            case 5:
                timer->link1 = quest_place_or_person_object(quest, timer->link1, (short)timer->flags & 256);
                timer->link2 = quest_place_or_person_object(quest, timer->link2, (short)timer->flags & 512);
                break;
            default:
                timer->link1 = timer->link2 = 0;
                break;
            }
        }
        timer->start = game_minutes;
        switch (timer->type) {
        case 0:
            timer->delay = rand_range(timer->minimum, timer->maximum);
            break;
        case 1:
            timer->delay = timer->minimum;
            break;
        case 2:
            timer->delay = quest_travel_minutes(quest, 0, timer->link1) * 384 >> 8;
            if (memchr(D_0017A13C, timer->link1->link_flag, 5))
                timer->delay += 10080;
            break;
        case 3:
            timer->delay = quest_travel_minutes(quest, timer->link1, timer->link2) * 384 >> 8;
            if (memchr(D_0017A13C, timer->link1->link_flag, 5) || memchr(D_0017A13C, timer->link2->link_flag, 5))
                timer->delay += 10080;
            break;
        case 4:
            timer->delay = quest_travel_minutes(quest, 0, timer->link1) * 384 >> 8;
            if (memchr(D_0017A13C, timer->link1->link_flag, 5))
                timer->delay += 10080;
            break;
        case 5:
            timer->delay = quest_travel_minutes(quest, 0, timer->link1) * 384 >> 8;
            timer->delay += quest_travel_minutes(quest, timer->link1, timer->link2) * 384 >> 8;
            if (memchr(D_0017A13C, timer->link1->link_flag, 5))
                timer->delay += 10080;
            if (memchr(D_0017A13C, timer->link2->link_flag, 5))
                timer->delay += 10080;
            break;
        }
        if ((short)timer->flags & 16)
            timer->delay <<= 1;
        travel_options = saved_travel_options;
    } else if (game_minutes - timer->start > timer->delay) {
        quest_timer_expire(quest, timer);
    }
}
