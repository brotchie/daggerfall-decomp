/* matched by the real Watcom C32 10.0a (-d2): a run of custom.c from 0x0005506F to 0x000553B2, kept together for its switch table's alignment */
#pragma pack(1)
struct slot { unsigned char kind; unsigned char bit; };
#pragma pack()
extern unsigned char mouse_buttons;
extern char D_00175420[];
extern signed char classmaker_special_counts[];
extern short D_00190D68;
extern short classmaker_special_list;
extern short D_00190D84;
extern int text_macro_fpc;
extern unsigned short *text_macro_fe;
extern char *player_class;
extern struct slot classmaker_specials[][7];
extern void func_000A1023(void *, void *, int, char *, int, int);
extern int func_0012B136();
extern void func_000CE4A9(char *, int, int);
extern void func_000CE4B5(char *, int, int);
extern int func_00144F68();
extern int func_00144FB4();
void classmaker_set_advantage(int, int);
void classmaker_set_disadvantage(int, int);

void classmaker_draw_dagger(void)
{
    func_00144F68(219, 46, 40, 138, text_macro_fpc);
    func_00144FB4(219, D_00190D68, text_macro_fe[2], text_macro_fe[3], (char *)text_macro_fe + 12);
}

void classmaker_specials_remove(void)
{
    if (D_00190D84 == -1) return;
    if (classmaker_special_list == 0)
        classmaker_set_advantage(D_00190D84, 1);
    else
        classmaker_set_disadvantage(D_00190D84, 1);
    if (D_00190D84 != 6)
        func_000A1023(&classmaker_specials[classmaker_special_list][D_00190D84], &classmaker_specials[classmaker_special_list][D_00190D84 + 1], (6 - D_00190D84) * 2, D_00175420, 952, 4);
    classmaker_special_counts[classmaker_special_list]--;
    while (mouse_buttons != 0)
        func_0012B136();
}

void classmaker_set_advantage(int a1, int a2)
{
    int bit;

    bit = classmaker_specials[classmaker_special_list][a1].bit;
    switch (classmaker_specials[classmaker_special_list][a1].kind) {
    case 0:
        func_000CE4A9(player_class, 1 << bit, a2);
        break;
    case 1:
        func_000CE4A9(player_class + 1, 1 << bit, a2);
        break;
    case 2:
        func_000CE4B5(player_class + 4, 1, a2);
        break;
    case 3:
        func_000CE4A9(player_class + 9, 1 << bit, a2);
        break;
    case 4:
        func_000CE4A9(player_class + 6, 1 << bit, a2);
        break;
    case 5:
        func_000CE4A9(player_class + 7, 1 << bit, a2);
        break;
    case 6:
        func_000CE4A9(player_class + 10, 1 << bit, a2);
        break;
    case 7:
        func_000CE4B5(player_class + 4, 2, a2);
        break;
    case 8:
        player_class[5] &= 227;
        if (a2 == 0)
            *(short *)(player_class + 4) |= bit << 10;
        else
            *(short *)(player_class + 4) = 5120;
        break;
    case 9:
        func_000CE4B5(player_class + 4, 4, a2);
        break;
    case 10:
        func_000CE4A9(player_class + 13, 1 << bit, a2);
        break;
    case 11:
        func_000CE4A9(player_class + 8, 1 << bit, a2);
        break;
    }
}

void classmaker_set_disadvantage(int a1, int a2)
{
    int bit;

    bit = classmaker_specials[classmaker_special_list][a1].bit;
    switch (classmaker_specials[classmaker_special_list][a1].kind) {
    case 0:
        func_000CE4B5(player_class + 4, 8, a2);
        break;
    case 1:
        func_000CE4B5(player_class + 4, (1 << bit) << 4, a2);
        break;
    case 2:
        func_000CE4B5(player_class + 10, (1 << bit) << 4, a2);
        break;
    case 3:
        func_000CE4B5(player_class + 4, (1 << bit) << 6, a2);
        break;
    case 4:
        func_000CE4B5(player_class + 4, (1 << bit) << 8, a2);
        break;
    case 5:
        func_000CE4B5(player_class + 14, 1 << bit, a2);
        break;
    case 6:
        func_000CE4A9(player_class + 2, 1 << bit, a2);
        break;
    case 7:
        func_000CE4A9(player_class + 3, 1 << bit, a2);
        break;
    case 8:
        func_000CE4B5(player_class + 14, (1 << bit) << 6, a2);
        break;
    case 9:
        func_000CE4B5(player_class + 14, (1 << bit) << 9, a2);
        break;
    case 10:
        func_000CE4B5(player_class + 11, 1 << bit, a2);
        break;
    }
}
