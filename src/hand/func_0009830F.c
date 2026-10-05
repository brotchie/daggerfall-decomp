/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009830F */
extern char *inv_right_container;
extern unsigned game_minutes;

void trade_schedule_shop_repairs(void)
{
    int longest_index;
    int total;
    int unused;
    char *object;
    int index;
    int longest;
    int minutes;
    double scale;
    char *item;

    index = 0;
    object = *(char **)(inv_right_container + 63);
    while (object != 0) {
        item = object + 71;
        minutes = (*(unsigned short *)(item + 46) - *(unsigned short *)(item + 44)) * 1440 / 1000 + 1440;
        if (minutes > total) {
            total = minutes;
            longest_index = index;
        }
        index++;
        object = *(char **)(object + 55);
    }
    longest = total;
    index = 0;
    object = *(char **)(inv_right_container + 63);
    while (object != 0) {
        if (index != longest_index) {
            item = object + 71;
            total += (*(unsigned short *)(item + 46) - *(unsigned short *)(item + 44)) * 1440 / 1000 + 720;
        }
        index++;
        object = *(char **)(object + 55);
    }
    scale = (double)total / longest;
    object = *(char **)(inv_right_container + 63);
    while (object != 0) {
        item = object + 71;
        minutes = (*(unsigned short *)(item + 46) - *(unsigned short *)(item + 44)) * 1440 / 1000 + 1440;
        *(unsigned *)(object + 43) = game_minutes + (unsigned)(minutes * scale);
        object = *(char **)(object + 55);
    }
}
