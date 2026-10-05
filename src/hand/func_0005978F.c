/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005978F */
extern char D_0017573C[];
extern char *player_character;
extern unsigned char *D_00199B4C;
extern unsigned char **D_00199B50;
extern int mc_memmove();

void func_0005978F(unsigned char *a1, int a2)
{
    int l_18;
    int l_14;

    l_18 = 0;
    if (*(unsigned short *)(a1 + 32) == 3) {
        l_14 = *(unsigned short *)(a1 + 50) >> 7;
        if (l_14 == 432 || l_14 == 433) {
        } else if (*(unsigned short *)(player_character + 64) & 1) {
            *(short *)(a1 + 50) -= 128;
        }
        if (a2 == 19) {
            if (*(unsigned short *)(a1 + 42) & 4)
                (*(short *)(a1 + 50))++;
        } else if (a2 == 21) {
            if (*(unsigned short *)(a1 + 42) & 4)
                a1[66] += 5;
        }
    }
    while (D_00199B50[l_18] != 0 && a1[66] > D_00199B50[l_18][66])
        l_18++;
    mc_memmove(&D_00199B4C[l_18 + 1], &D_00199B4C[l_18], 27, D_0017573C, 201, 4);
    mc_memmove(&D_00199B50[l_18 + 1], &D_00199B50[l_18], 108, D_0017573C, 202, 4);
    D_00199B50[l_18] = a1;
    D_00199B4C[l_18] = a2;
}
