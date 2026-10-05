/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008227A */
struct flags {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
};
#pragma pack(1)
struct place {
    int x, y, z;
    int a, b, c;
    char *name;
};
#pragma pack()
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern char D_00187C3C[];
extern struct place D_00187C86;
extern struct flags player_motion_flags;
extern char *player_object;
extern char *player_character;
extern unsigned char collide_flags;
extern int collide_move_player(char *, int, struct place *, int);

int player_try_move_vertical(int a1)
{
    int l_20;
    int l_1C;

    D_00187C86.x = *(int *)(player_object + 7);
    D_00187C86.y = *(int *)(player_object + 11) + a1;
    D_00187C86.z = *(int *)(player_object + 15);
    D_00187C86.a = *(short *)(player_object + 1);
    D_00187C86.b = *(short *)(player_object + 3);
    D_00187C86.c = *(short *)(player_object + 5);
    D_00187C86.name = player_motion_flags.b2 ? D_00187C12 : D_00187B6E;
    D_00187C86.name = (*(unsigned short *)(player_character + 64) & 1536) ? D_00187C3C : D_00187C86.name;
    if (player_motion_flags.b5)
        D_00187C86.name = D_00187BB8;
    collide_flags &= 251;
    l_20 = collide_move_player(player_object, 0, &D_00187C86, 1);
    return l_20;
}
