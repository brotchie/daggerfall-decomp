/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001700D5[];
extern char D_0018DC34[];
extern char midi_bsa[];
extern char D_001A3F48[];
extern char D_001A3F4C[];

extern int archive_find_record(int, int, int);
extern int archive_record_size(int, int);
extern int archive_read_record(int, int, int);
extern int dpmi_lock_region(int, int);
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int func_000A021C();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_load_song;

int sos_load_song(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_14 = archive_find_record(*(int *)midi_bsa, a1, 13);
    l_10 = archive_record_size(*(int *)midi_bsa, l_14);
    l_1C = l_10;
    l_20 = mc_malloc(l_1C + 32, (int)D_001700D5, 385);
    if (l_20 != 0) goto L118D0;
    return 0;
L118D0:;
    archive_read_record(*(int *)midi_bsa, l_14, l_20 + 32);
    l_28 = l_20;
    mc_memset(l_28, 0, 32, (int)D_001700D5, 397, 4);
    *(int *)((char *)l_28) = l_20 + 32;
    if (func_000A021C(l_28, (int)&l_24) == 0) goto L11951;
    if (l_20 == 0) goto L1192D;
    if (l_20 != (-1751672937)) goto L1192F;
L1192D:;
    goto L11948;
L1192F:;
    mc_free(l_20, (int)D_001700D5, 406);
    l_20 = -1751672937;
L11948:;
    return -1;
L11951:;
    *(int *)D_0018DC34 = l_20;
    *(int *)D_001A3F48 = l_20;
    dpmi_lock_region(l_20, (*(int *)D_001A3F4C = l_1C + 32));
    return l_24;
}
