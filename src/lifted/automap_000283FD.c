/* automap.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char *scratch_buffer;
extern int town_map_view_x;
extern int town_map_view_y;

extern int font_text_width(char *);
#include "clib.h"

int town_note_at(int x, int y)
{
    int index;
    int length;
    int note_x;
    int note_y;
    char *note;
    char *entry;

    index = 0;
    note = scratch_buffer + 4;
    while (*(short *)note != 0) {
        entry = note;
        note_x = ((((int)(unsigned short)*(short *)entry) - town_map_view_x) * 2) + 10;
        note_y = ((((int)(unsigned short)*(short *)(entry + 2)) - town_map_view_y) * 2) + 10;
        if (x >= note_x && y >= note_y && (font_text_width(note + 4) + note_x) >= x && (note_y + 6) >= y) {
            return index + 1;
        }
        index++;
        length = strlen(note + 4);
        note += length + 5;
    }
    return 0;
}
