/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00059909 */
extern unsigned char *paperdoll_slots;
extern char **paperdoll_items;
extern void paperdoll_draw_item(char *, int, int, int);

void paperdoll_draw_items(int a1, int a2)
{
    int i;

    i = 0;
    while (paperdoll_items[i] != 0) {
        paperdoll_draw_item(paperdoll_items[i], a1, a2, paperdoll_slots[i] + 64);
        i++;
    }
}
