/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"

extern char D_001700D5[];
extern int D_0018DC34;
extern int midi_bsa;
extern int D_001A3F48;
extern int D_001A3F4C;

extern int archive_find_record(int, char *, int);
extern int archive_record_size(int, int);
extern int archive_read_record(int, int, int);
extern int dpmi_lock_region(int, int);
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int func_000A021C();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_load_song;

int sos_load_song(char *name)
{
    struct sos_song *song;
    int song_handle;
    int buffer;
    int size;
    int unused;
    int record;
    int record_size;

    record = archive_find_record(midi_bsa, name, 13);
    record_size = archive_record_size(midi_bsa, record);
    size = record_size;
    buffer = mc_malloc(size + 32, (int)D_001700D5, 385);
    if (buffer == 0) return 0;
    archive_read_record(midi_bsa, record, buffer + 32);
    song = (struct sos_song *)buffer;
    mc_memset(song, 0, 32, (int)D_001700D5, 397, 4);
    song->data = (char *)(buffer + 32);
    if (func_000A021C(song, (int)&song_handle) != 0) {
        if (buffer != 0 && buffer != (-1751672937)) {
            mc_free(buffer, (int)D_001700D5, 406);
            buffer = -1751672937;
        }
        return -1;
    }
    D_0018DC34 = buffer;
    D_001A3F48 = buffer;
    dpmi_lock_region(buffer, (D_001A3F4C = size + 32));
    return song_handle;
}
