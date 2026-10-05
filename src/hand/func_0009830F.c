/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009830F */
extern char *inv_right_container;
extern unsigned game_minutes;

void trade_schedule_shop_repairs(void)
{
    int l_2C;
    int l_34;
    int l_30;
    char *l_1C;
    int l_28;
    int l_24;
    int l_20;
    double l_3C;
    char *l_18;

    l_28 = 0;
    l_1C = *(char **)(inv_right_container + 63);
    while (l_1C != 0) {
        l_18 = l_1C + 71;
        l_20 = (*(unsigned short *)(l_18 + 46) - *(unsigned short *)(l_18 + 44)) * 1440 / 1000 + 1440;
        if (l_20 > l_34) {
            l_34 = l_20;
            l_2C = l_28;
        }
        l_28++;
        l_1C = *(char **)(l_1C + 55);
    }
    l_24 = l_34;
    l_28 = 0;
    l_1C = *(char **)(inv_right_container + 63);
    while (l_1C != 0) {
        if (l_28 != l_2C) {
            l_18 = l_1C + 71;
            l_34 += (*(unsigned short *)(l_18 + 46) - *(unsigned short *)(l_18 + 44)) * 1440 / 1000 + 720;
        }
        l_28++;
        l_1C = *(char **)(l_1C + 55);
    }
    l_3C = (double)l_34 / l_24;
    l_1C = *(char **)(inv_right_container + 63);
    while (l_1C != 0) {
        l_18 = l_1C + 71;
        l_20 = (*(unsigned short *)(l_18 + 46) - *(unsigned short *)(l_18 + 44)) * 1440 / 1000 + 1440;
        *(unsigned *)(l_1C + 43) = game_minutes + (unsigned)(l_20 * l_3C);
        l_1C = *(char **)(l_1C + 55);
    }
}
