/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00099922 */
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern unsigned char player_environment;
extern signed char D_00196297;
extern char doors_moving[];
extern void func_00063DDC(int);
extern void links_trigger(int, int);
extern int sound_play(int, int, int);

int door_start_swing(int a1, int a2)
{
    int l_20;

    if (((struct bf8_6_1 *)((char *)a1 + 46))->f != 0) return 0;
    if (((struct bf8_7_1 *)((char *)a1 + 46))->f != 0 && a2 != 0) {
        *(int *)((char *)a1 + 43) = *(int *)1132 | 1073741824;
    } else if (((struct bf8_7_1 *)((char *)a1 + 46))->f == 0 && a2 == 0) {
        *(int *)((char *)a1 + 43) = *(int *)1132 | (-1073741824);
    }
    l_20 = 0;
    do {
    } while (*(int *)(doors_moving + (l_20++ << 2)) != 0);
    l_20--;
    *(int *)(doors_moving + (l_20 << 2)) = a1;
    if (a2 == 0) sound_play(((((int)player_environment) == 2) ? 362 : 27), a1, 100);
    func_00063DDC(0);
    if (D_00196297 != 0) links_trigger(a1, 10);
    return 1;
}
