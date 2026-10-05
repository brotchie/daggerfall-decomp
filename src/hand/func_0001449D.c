/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001449D */
#pragma pack(1)
struct S { char c0; short s1; short s3; short s5; int x7; int x11; int x15; };
struct P { int x; int y; int z; };
extern struct S *player_object;
extern struct P D_00120288;
extern void object_set_position(struct S *, int, int, int, int, int, int);

void func_0001449D(void)
{
    int l_1C;
    int l_18;

    l_1C = player_object->x7 + (D_00120288.x >> 9);
    l_18 = player_object->x15 + (D_00120288.z >> 9);
    object_set_position(player_object, l_1C, player_object->x11, l_18, player_object->s1, player_object->s3, player_object->s5);
}
