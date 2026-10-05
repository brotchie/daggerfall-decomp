#include "records.h"
/* rumor.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_0012B508;
extern char D_001702CC[];
extern int marquee_owned_text;
extern char region_event_flag_groups[];
extern signed char region_event_durations[];
extern signed char D_00178EB0[];
extern struct region regions[];
extern signed char text_buffer[];
extern int marquee_x;
extern int marquee_text;

extern struct faction *faction_random_of_type(unsigned char);
extern int font_char_width(unsigned char);
extern int rand_range(int, int);
extern int mc_free();
extern int mc_memcpy();
extern void text_draw(int, int, int);
void marquee_stop(void);

void marquee_update(void)
{
    int char_width;
    int count;

    if (marquee_text == 0) return;
    D_0012B508 = 146;
    count = (320 - marquee_x) / 4;
    if (count > 80) count = 80;
    mc_memcpy((int)text_buffer, marquee_text, count, (int)D_001702CC, 35, 160);
    text_buffer[count] = 0;
    text_draw((int)text_buffer, marquee_x, 140);
    marquee_x -= 2;
    char_width = font_char_width((int)(unsigned char)*(signed char *)(*(char **)&marquee_text));
    if ((-marquee_x) > char_width) {
        marquee_text++;
        marquee_x += char_width;
    }
    if (*(signed char *)(*(char **)&marquee_text) != 0) return;
    marquee_stop();
}

void marquee_stop(void)
{
    if (marquee_owned_text != 0 && marquee_owned_text != (-1751672937)) {
        mc_free(marquee_owned_text, (int)D_001702CC, 62);
        marquee_owned_text = -1751672937;
    }
    marquee_owned_text = 0;
    marquee_text = 0;
}

void region_flag_set(int region, int flag)
{
    int i;

    if (regions[region].groups[region_event_flag_groups[flag]] != 0) {
        for (i = 0; i < 29; i++) {
            if ((signed char)region_event_flag_groups[flag] == (signed char)region_event_flag_groups[i]) {
                regions[region].flags[i] = 0;
            }
        }
    }
    regions[region].flags[flag] = 1;
    regions[region].groups[region_event_flag_groups[flag]] = 1;
    regions[region].values[flag] = rand_range((int)(unsigned char)region_event_durations[flag * 2], (int)(unsigned char)D_00178EB0[flag * 2]);
    if (flag != 18) return;
    regions[region].persecuted_temple = faction_random_of_type(1)->id;
}

void region_flag_clear(int region, int flag)
{
    regions[region].flags[flag] = 0;
    regions[region].groups[region_event_flag_groups[flag]] = 0;
}
