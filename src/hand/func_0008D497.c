/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008D497 */
struct lbitem { unsigned short flags; char pad[2]; char name[40]; };
struct listbox {
    char border;                /* 0 */
    unsigned char c1, c2, c3;   /* 1..3 */
    unsigned char color;        /* 4 */
    short x, y, w, h;           /* 5 */
    char pad13[16];
    short x2, y2, w2, h2;       /* 29 */
    unsigned short count;       /* 37 */
    unsigned short top;         /* 39 */
    unsigned short sel;         /* 41 */
    char pad43[2];
    short f45;                  /* 45 */
    struct lbitem *items;       /* 47 */
    int f51;                    /* 51 */
    int f55;                    /* 55 */
};
extern unsigned char D_0012B508;
extern unsigned char D_0012DA44;
extern char D_00176E38[];
extern char D_001A9AF3;
extern void text_draw(char *, int, int);
extern void picklist_clip_text(char *, short);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern void func_000CE4FA(int, int, int, int);
extern void func_00144D00(int, int, int, int);
extern void func_00144ED8(int, int, int, int, int, int);

void picklist_draw(struct listbox *a1, int a2)
{
    short l_30;
    short l_2C;
    char l_88[80];
    short l_28;
    short l_24;
    short l_20;
    short l_1C;
    short l_18;
    unsigned char l_14;

    l_28 = D_0012B508;
    l_30 = 0;
    l_14 = D_0012DA44;
    if (a1->count == 0)
        l_30 = 0;
    else
        l_30 = a1->top * a1->h2 / a1->count;
    if (a1->border) {
        func_00144ED8(a1->x, a1->y, a1->w, a1->h, a1->f51, 0);
        func_00144ED8(a1->x2, a1->y2, a1->w2, a1->h2, a1->f55, 0);
    }
    D_0012B508 = a1->color;
    l_2C = a1->x2 + 2;
    l_18 = l_30 + (a1->y2 + 1);
    l_20 = a1->w2 - 4;
    l_1C = a1->f45 - 1;
    if (a1->x2 != 0)
        func_00144D00(l_2C, l_18, l_20, l_1C);
    D_0012B508 = 123;
    func_000CE4FA(l_2C, l_18, l_2C, l_18 + l_1C - 1);
    func_000CE4FA(l_2C, l_18 + l_1C - 1, l_2C + l_20 - 1, l_18 + l_1C - 1);
    D_0012B508 = 112;
    func_000CE4FA(l_2C + l_20 - 1, l_18, l_2C + l_20 - 1, l_18 + l_1C - 2);
    func_000CE4FA(l_2C + 1, l_18, l_2C + l_20 - 1, l_18);
    l_24 = a1->top;
    l_18 = 1;
    while (l_24 < a1->count && l_18 < a1->h - l_14) {
        func_000A0AD9(l_88, a1->items[l_24].name, 80, D_00176E38, 236);
        picklist_clip_text(l_88, a1->w);
        if (l_24 != a1->sel || D_001A9AF3 != 0) {
            D_0012B508 = l_24 != a1->sel ? 156 : 0;
            text_draw(l_88, a1->x + 2, a1->y + l_18 + 1);
        }
        D_0012B508 = (a1->items[l_24].flags & 1) ? a1->c2 : a1->c1;
        if (l_24 == a1->sel)
            D_0012B508 = a1->c3;
        text_draw(l_88, a1->x + 1, a1->y + l_18);
        l_24++;
        l_18 += l_14 + 1;
    }
    D_0012B508 = l_28;
}
