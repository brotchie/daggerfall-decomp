/* names.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int namegen_part_offsets[];
extern int namegen_part_counts[];
extern char namegen_syllable[];

extern int rand();
extern int lseek();
extern int read();

void namegen_read_part(short handle, short part)
{
    lseek((int)(short)handle, namegen_part_offsets[((int)(short)part)] + ((rand() % namegen_part_counts[((int)(short)part)]) * 10), 0);
    read((int)(short)handle, (int)namegen_syllable, 10);
}
