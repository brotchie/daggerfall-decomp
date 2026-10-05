/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00097488 to 0x00097616, kept together for its switch table's alignment */
#pragma pack(1)
struct img {
    unsigned short f0;
    unsigned short f2;
    unsigned short h;
    unsigned short f6;
    unsigned short f8;
    unsigned short f10;
    char data[1];
};
#pragma pack()
extern char *player_entity;
extern unsigned char *D_00195AF4;
extern struct img *D_001AA42C;
extern struct img *D_001AA430;
extern int inv_left_scroll;
extern int inv_right_scroll;
extern short inv_right_count;
extern short inv_left_count;
extern int object_delete(unsigned char *);
extern int inv_match_arrows(int);
extern int object_find(int, int (*)(int));
extern void func_00144FB4(int, int, int, int, char *);
void inv_draw_scroll_arrow(struct img *p, int a2);

int inv_take_arrow(int a1)
{
    unsigned char *p;

    D_00195AF4 = 0;
    object_find(*(int *)(player_entity + 63), inv_match_arrows);
    if (D_00195AF4 == 0) return 0;
    if (a1 == 0) return 1;
    p = D_00195AF4 + 71;
    if (p[49] == 1) {
        object_delete(D_00195AF4);
        return 1;
    }
    p[49]--;
    return 1;
}

void inv_draw_scroll_arrows(void)
{
    int l_18;

    inv_draw_scroll_arrow(inv_left_scroll != 0 ? D_001AA42C : D_001AA430, 0);
    inv_draw_scroll_arrow(inv_right_scroll != 0 ? D_001AA42C : D_001AA430, 1);
    l_18 = inv_left_count - 4;
    inv_draw_scroll_arrow(l_18 > 0 && inv_left_scroll < l_18 ? D_001AA42C : D_001AA430, 2);
    l_18 = inv_right_count - 4;
    inv_draw_scroll_arrow(l_18 > 0 && inv_right_scroll < l_18 ? D_001AA42C : D_001AA430, 3);
}

void inv_draw_scroll_arrow(struct img *p, int a2)
{
    switch (a2) {
    case 0:
        func_00144FB4(163, 48, p->h, 20, p->data);
        break;
    case 1:
        func_00144FB4(261, 48, p->h, 20, p->data);
        break;
    case 2:
        func_00144FB4(163, p->f2 + p->f6 - 20, p->h, 20, p->data + p->f10 - p->h * 20);
        break;
    case 3:
        func_00144FB4(261, p->f2 + p->f6 - 20, p->h, 20, p->data + p->f10 - p->h * 20);
        break;
    }
}
