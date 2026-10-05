/* rumor.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_0012B508;
extern char D_001702CC[];
extern int marquee_owned_text;
extern char region_event_flag_groups[];
extern signed char region_event_durations[];
extern signed char D_00178EB0[];
extern signed char region_event_values[];
extern signed char region_event_flags[];
extern char region_event_groups[];
extern char region_persecuted_temple[];
extern signed char text_buffer[];
extern int marquee_x;
extern int marquee_text;

extern int faction_random_of_type(unsigned char);
extern int font_char_width(unsigned char);
extern int rand_range(int, int);
extern int mc_free();
extern int mc_memcpy();
extern void text_draw(int, int, int);
void marquee_stop(void);

void marquee_update(void)
{
    int l_1C;
    int l_18;

    if (marquee_text == 0) return;
    D_0012B508 = 146;
    l_18 = (320 - marquee_x) / 4;
    if (l_18 > 80) l_18 = 80;
    mc_memcpy((int)text_buffer, marquee_text, l_18, (int)D_001702CC, 35, 160);
    text_buffer[l_18] = 0;
    text_draw((int)text_buffer, marquee_x, 140);
    marquee_x -= 2;
    l_1C = font_char_width((int)(unsigned char)*(signed char *)(*(char **)&marquee_text));
    if ((-marquee_x) > l_1C) {
        marquee_text++;
        marquee_x += l_1C;
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

void region_flag_set(int a1, int a2)
{
    int l_14;

    if (*(signed char *)(region_event_groups + (a1 * 80) + *(unsigned char *)(region_event_flag_groups + a2)) != 0) {
        for (l_14 = 0; l_14 < 29; l_14++) {
            if (*(signed char *)(region_event_flag_groups + a2) == *(signed char *)(region_event_flag_groups + l_14)) {
                region_event_flags[(a1 * 80) + l_14] = 0;
            }
        }
    }
    region_event_flags[(a1 * 80) + a2] = 1;
    *(signed char *)(region_event_groups + (a1 * 80) + *(unsigned char *)(region_event_flag_groups + a2)) = 1;
    region_event_values[(a1 * 80) + a2] = rand_range((int)(unsigned char)region_event_durations[a2 * 2], (int)(unsigned char)D_00178EB0[a2 * 2]);
    if (a2 != 18) return;
    *(short *)(region_persecuted_temple + (a1 * 80)) = *(short *)((char *)faction_random_of_type(1) + 33);
}

void region_flag_clear(int a1, int a2)
{
    region_event_flags[(a1 * 80) + a2] = 0;
    *(signed char *)(region_event_groups + (a1 * 80) + *(unsigned char *)(region_event_flag_groups + a2)) = 0;
}
