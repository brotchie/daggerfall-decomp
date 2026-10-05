/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00170404[];
extern char D_00170411[];
extern signed char D_00190D10;
extern int talk_face_image;

extern int disk_open_data(int);
extern int rand_range(int, int);
extern int func_0009DEA7();
extern int lseek();
extern int func_000A00CB();

void talk_load_face(int a1)
{
    int l_1C;
    int l_18;

    D_00190D10 = 1;
    if (a1 >= 1000) {
        l_18 = ((a1 == 1000) ? 0 : 6);
        l_18 += rand_range(0, 5);
        l_1C = disk_open_data((int)D_00170404);
        lseek(l_1C, l_18 << 12, 0);
    } else {
        D_00190D10 = 1;
        l_1C = disk_open_data((int)D_00170411);
        lseek(l_1C, a1 << 12, 0);
    }
    func_000A00CB(l_1C, talk_face_image, 4096);
    func_0009DEA7(l_1C);
}
