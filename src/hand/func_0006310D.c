/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006310D */
extern char *player_object;
extern void monster_play_sound(char *, int);
extern int sound_play(int, char *, int);
extern int func_0009DC25(void);
extern int func_000C7FD9(int, int, int, int);

void monster_ambient_sound(char *a1, char *a2)
{
    int l_24[2];
    int l_14;

    if (func_0009DC25() > 195) return;
    l_14 = func_000C7FD9(*(int *)(a1 + 7), *(int *)(a1 + 15), *(int *)(player_object + 7), *(int *)(player_object + 15));
    if (l_14 >= 1024) return;
    if (*(unsigned char *)(a2 + 506) == 146) {
        sound_play(11461, a1, 100);
        return;
    }
    if (*(unsigned char *)(a2 + 67) >= 43) return;
    monster_play_sound(a1, l_14);
}
