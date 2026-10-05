/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00099922 */
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char player_environment[];
extern char D_00196297[];
extern char doors_moving[];
extern void func_00063DDC(int);
extern void links_trigger(int, int);
extern int sound_play(int, int, int);

int door_start_swing(int a1, int a2)
{
    int l_20;

    if (((struct bf8_6_1 *)((char *)a1 + 46))->f == 0) goto L9994A;
    return 0;
L9994A:;
    if (((struct bf8_7_1 *)((char *)a1 + 46))->f == 0) goto L99959;
    if (a2 != 0) goto L9995B;
L99959:;
    goto L99974;
L9995B:;
    *(int *)((char *)a1 + 43) = *(int *)1132 | 1073741824;
    goto L9999C;
L99974:;
    if (((struct bf8_7_1 *)((char *)a1 + 46))->f != 0) goto L99983;
    if (a2 == 0) goto L99985;
L99983:;
    goto L9999C;
L99985:;
    *(int *)((char *)a1 + 43) = *(int *)1132 | (-1073741824);
L9999C:;
    l_20 = 0;
L999A3:;
    if (*(int *)(doors_moving + (l_20++ << 2)) != 0) goto L999A3;
    l_20--;
    *(int *)(doors_moving + (l_20 << 2)) = a1;
    if (a2 != 0) goto L99A07;
    sound_play(((((int)(unsigned char)*(signed char *)player_environment) == 2) ? 362 : 27), a1, 100);
L99A07:;
    func_00063DDC(0);
    if (*(signed char *)D_00196297 == 0) goto L99A24;
    links_trigger(a1, 10);
L99A24:;
    return 1;
}
