/* names.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int namegen_part_offsets[];
extern int namegen_part_counts[];
extern char namegen_syllable[];

extern int rand();
extern int lseek();
extern int func_000A00CB();

void namegen_read_part(short a1, short a2)
{
    lseek((int)(short)a1, namegen_part_offsets[((int)(short)a2)] + ((rand() % namegen_part_counts[((int)(short)a2)]) * 10), 0);
    func_000A00CB((int)(short)a1, (int)namegen_syllable, 10);
}
