/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E62 */
struct bf8_0_1 { unsigned char f:1; };
extern char D_00185097[];
extern char player_character[];
extern void spell_remove_effect_type(int, int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern void spfx_cure_disease(int, int);
extern int object_free_single(int);

int spfx_cure(int a1, int a2, int a3)
{
    unsigned char *l_28;
    unsigned char *l_24;
    int l_20;
    int l_1C;
    unsigned char *l_18;
    int l_14;

    l_28 = (unsigned char *)a1 + 71;
    l_24 = (unsigned char *)a3 + 71;
    if (rand_range(1, 100) > l_28[a2 + 86]) {
        hud_message_add(*(int *)D_00185097);
        return 0;
    }
    switch (l_28[a2 * 2 + 1]) {
    case 0:
        spfx_cure_disease(a3, (int)l_24);
        if (*(int *)(*(char **)player_character + 499) != 0) {
            *(int *)(*(char **)player_character + 499) = 0;
            *(short *)(*(char **)player_character + 108) = 0;
        }
        break;
    case 1:
        l_20 = *(int *)((char *)a3 + 63);
        while (l_20 != 0) {
            if (*(unsigned char *)l_20 == 11) {
                l_18 = (unsigned char *)l_20 + 71;
                if (*l_18 > 127) {
                    for (l_14 = 0; l_14 < 8; l_14++) {
                        *(short *)(l_24 + l_14 * 2 + 32) += *(short *)(l_18 + l_14 * 2 + 31);
                        if (*(short *)(l_24 + l_14 * 2 + 32) > *(short *)(l_24 + l_14 * 2 + 48))
                            *(short *)(l_24 + l_14 * 2 + 32) = *(short *)(l_24 + l_14 * 2 + 48);
                    }
                    l_20 = object_free_single(l_20);
                } else {
                    l_20 = *(int *)((char *)l_20 + 55);
                }
            } else {
                l_20 = *(int *)((char *)l_20 + 55);
            }
        }
        break;
    case 2:
        if (((struct bf8_0_1 *)(l_24 + 137))->f == 0) return 0;
        spell_remove_effect_type(a3, 0);
        l_24[137] &= 254;
    case 3:
        break;
    }
    return 0;
}
