/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern char D_00176198[];
extern char D_0017627D[];
extern signed char text_buffer[];

extern int disk_open_data(char *);
#pragma aux mc_set_location parm routine [];

void book_read_header(char *header, int book_id)
{
    int handle;

    mc_set_location(586, D_00176198);
    mc_sprintf((char *)text_buffer, D_0017627D, book_id);
    handle = disk_open_data(text_buffer);
    read(handle, header, 234);
    close(handle);
}
