/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"
#include "bitfield.h"

extern char D_00170129[];

extern int open(char *, ...);
extern int close();
extern int mc_free();
extern int write();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_close;

int profile_close(struct profile *profile)
{
    int handle;

    if (((struct bf8_7_1 *)&profile->flags)->f != 0) {
        handle = open(profile->path, 610, 0);
        if (handle == (-1)) {
            if (profile->buffer != 0 && (iptr)profile->buffer != (-1751672937)) {
                mc_free((iptr)profile->buffer, (iptr)D_00170129, 171);
                profile->buffer = (char *)(iptr)-1751672937;
            }
            return 0;
        }
        write(handle, (iptr)profile->buffer, profile->length);
        close(handle);
    }
    if (profile->buffer != 0 && (iptr)profile->buffer != (-1751672937)) {
        mc_free((iptr)profile->buffer, (iptr)D_00170129, 185);
        profile->buffer = (char *)(iptr)-1751672937;
    }
    return 1;
}
