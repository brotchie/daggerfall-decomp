/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001700D5[];

extern int open(int, ...);
extern int close();
extern int mc_free();
extern int lseek();
extern int mc_malloc();
extern int read();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_load_file;

int sos_load_file(int a1)
{
    int l_18;
    int l_14;
    int l_10;

    l_10 = open(a1, 512);
    if (l_10 == (-1)) return 0;
    l_14 = lseek(l_10, 0, 2);
    lseek(l_10, 0, 0);
    l_18 = mc_malloc(l_14, (int)D_001700D5, 441);
    if (l_18 == 0) {
        close(l_10);
        return 0;
    }
    if (read(l_10, l_18, l_14) != l_14) {
        close(l_10);
        if (l_18 != 0 && l_18 != (-1751672937)) {
            mc_free(l_18, (int)D_001700D5, 457);
            l_18 = -1751672937;
        }
        return 0;
    }
    close(l_10);
    return l_18;
}
