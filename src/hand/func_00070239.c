/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00070239 */
extern unsigned char D_00190D20;
extern short D_00190DDC;
extern char *player_entity;
extern char *D_00195AF4;
extern void func_00070191(void);
extern void object_foreach(int, void (*)(void));

char *guild_find_membership_by_faction(short a1)
{
    D_00195AF4 = 0;
    D_00190DDC = a1;
    D_00190D20 = 255;
    object_foreach(*(int *)(player_entity + 63), func_00070191);
    if (D_00195AF4 == 0)
        return 0;
    return D_00195AF4 + 71;
}
