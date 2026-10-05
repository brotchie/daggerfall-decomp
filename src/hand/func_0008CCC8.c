/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008CCC8 */
#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct entry {
    short val;
    short idx;
    char name[40];
};
struct list {
    char pad0[37];
    unsigned short count;       /* 37 */
    char pad27[8];
    struct entry *items;        /* 47 */
};
#pragma pack()
extern char D_00176E38[];
extern struct bits8 D_001940D8;
extern void picklist_update_thumb(struct list *);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern void func_000A0E0D(void *, void *, int, char *, int, int);
extern int func_000A0E3B(char *, char *);

void picklist_add(struct list *l, char *name, short val)
{
    int unused;
    short i;

    if (D_001940D8.b0) {
        if (l->count == 0) {
            func_000A0AD9(l->items->name, name, 40, D_00176E38, 70);
            l->items->val = val;
            l->items->idx = l->count;
        } else {
            for (i = 0; i < l->count; i++) {
                if (func_000A0E3B(l->items[i].name, name) >= 0) {
                    func_000A0E0D(&l->items[i + 1], &l->items[i], (l->count - i) * 44, D_00176E38, 80, 4);
                    func_000A0AD9(l->items[i].name, name, 40, D_00176E38, 81);
                    l->items[i].val = val;
                    l->items[i].idx = l->count;
                    goto done;
                }
            }
            func_000A0AD9(l->items[l->count].name, name, 40, D_00176E38, 87);
            l->items[l->count].val = val;
            l->items[l->count].idx = l->count;
        }
    } else {
        func_000A0AD9(l->items[l->count].name, name, 40, D_00176E38, 94);
        l->items[l->count].val = val;
        l->items[l->count].idx = l->count;
    }
done:
    l->count++;
    picklist_update_thumb(l);
}
