/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00095B81 */
struct sub { char pad[32]; unsigned short state; };
struct obj {
    unsigned char type;
    char pad1[6];
    int x, y, z;
    char pad19[2];
    unsigned short flags;
    unsigned short owner;
    char pad25[42];
    int parent;
    struct sub sub;
};
extern int D_001940D6;
extern int D_001940D8;
extern int wagon_container;
extern struct obj *player_object;
extern char *player_character;
extern int D_00195D54;
extern struct obj *inv_right_rows[];
extern int inv_right_scroll;
extern short D_001AA588;
extern void inv_draw_item_cell(struct obj *, short, int);
extern int func_000C7FD9(int, int, int, int);
extern int func_000C7FF4(int, int);
extern int func_000CE44C(char *, struct obj *, int);

void inv_list_right_item(struct obj *a1, int a2)
{
    struct sub *s;

    if (a1->type != 2 && a1->type != 54 || (a1->flags & 2))
        return;
    if (a1->flags & 0x200)
        return;
    if ((D_001940D6 & 4) && a1->owner != D_00195D54)
        return;
    if ((!(D_001940D6 & 4) && a1->parent != wagon_container ? 1 : 0) && func_000C7FF4(a1->z - player_object->z, func_000C7FD9(a1->x, a1->y, player_object->x, player_object->y)) > 160)
        return;
    if (D_001AA588 >= inv_right_scroll && D_001AA588 < inv_right_scroll + 4) {
        if (!(D_001940D8 & 4) && func_000CE44C(player_character + 367, a1, 27) != 0) {
            s = &a1->sub;
            if (s->state != 1)
                return;
        }
        inv_right_rows[D_001AA588 - inv_right_scroll] = a1;
        inv_draw_item_cell(a1, D_001AA588 - inv_right_scroll, a2);
    }
    D_001AA588++;
}
