/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001700D5[];

extern int open(char *, ...);
extern int close();
extern int mc_free();
extern int lseek();
extern int mc_malloc();
extern int read();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_load_file;

int sos_load_file(char *path)
{
    int buffer;
    int size;
    int handle;

    handle = open(path, 512);
    if (handle == (-1)) return 0;
    size = lseek(handle, 0, 2);
    lseek(handle, 0, 0);
    buffer = mc_malloc(size, (int)D_001700D5, 441);
    if (buffer == 0) {
        close(handle);
        return 0;
    }
    if (read(handle, buffer, size) != size) {
        close(handle);
        if (buffer != 0 && buffer != (-1751672937)) {
            mc_free(buffer, (int)D_001700D5, 457);
            buffer = -1751672937;
        }
        return 0;
    }
    close(handle);
    return buffer;
}
