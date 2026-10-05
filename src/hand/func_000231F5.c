/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000231F5 */
extern unsigned char player_environment;
extern void func_0007E815(char *, int);
extern void func_0007EB0B(char *, int);
extern void object_foreach_open(int, int);

void collide_for_each_nearby(char *a1, int a2)
{
    int l_18;
    char l_28[12];

    if (player_environment != 3) {
        if (**(unsigned char **)(a1 + 67) != 1)
            object_foreach_open(*(int *)(*(char **)(a1 + 67) + 63), a2);
        else
            func_0007EB0B(a1, a2);
    } else {
        func_0007E815(a1, a2);
    }
}
