/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00099922 */
#include "records.h"

struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern unsigned char player_environment;
extern signed char D_00196297;
extern char doors_moving[];
extern void func_00063DDC(int);
extern void links_trigger(struct record *, int);
extern int sound_play(int, struct record *, int);

int door_start_swing(struct record *door, int close)
{
    int i;

    if (((struct bf8_6_1 *)((char *)door + 46))->f != 0) return 0;
    if (((struct bf8_7_1 *)((char *)door + 46))->f != 0 && close != 0) {
        door->door_swing = *(int *)1132 | 1073741824;
    } else if (((struct bf8_7_1 *)((char *)door + 46))->f == 0 && close == 0) {
        door->door_swing = *(int *)1132 | (-1073741824);
    }
    i = 0;
    while (((struct record **)doors_moving)[i++] != 0);
    i--;
    ((struct record **)doors_moving)[i] = door;
    if (close == 0) {
        sound_play(((((int)player_environment) == 2) ? 362 : 27), door, 100);
    }
    func_00063DDC(0);
    if (D_00196297 != 0) links_trigger(door, 10);
    return 1;
}
