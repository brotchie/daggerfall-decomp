/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009830F */
#include "records.h"
extern char *inv_right_container;
extern unsigned game_minutes;

void trade_schedule_shop_repairs(void)
{
    int longest_index;
    int total;
    int unused;
    char *object;
    int index;
    int longest;
    int minutes;
    double scale;
    char *item;

    index = 0;
    object = (char *)((struct record *)inv_right_container)->children;
    while (object != 0) {
        item = RECORD_DATA(object);
        minutes = (*(unsigned short *)(item + 46) - *(unsigned short *)(item + 44)) * 1440 / 1000 + 1440;
        if (minutes > total) {
            total = minutes;
            longest_index = index;
        }
        index++;
        object = (char *)((struct record *)object)->next;
    }
    longest = total;
    index = 0;
    object = (char *)((struct record *)inv_right_container)->children;
    while (object != 0) {
        if (index != longest_index) {
            item = RECORD_DATA(object);
            total += (*(unsigned short *)(item + 46) - *(unsigned short *)(item + 44)) * 1440 / 1000 + 720;
        }
        index++;
        object = (char *)((struct record *)object)->next;
    }
    scale = (double)total / longest;
    object = (char *)((struct record *)inv_right_container)->children;
    while (object != 0) {
        item = RECORD_DATA(object);
        minutes = (*(unsigned short *)(item + 46) - *(unsigned short *)(item + 44)) * 1440 / 1000 + 1440;
        *(unsigned *)(object + 43) = game_minutes + (unsigned)(minutes * scale);
        object = (char *)((struct record *)object)->next;
    }
}
