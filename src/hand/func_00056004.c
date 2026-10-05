/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00056004 */
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char mouse_buttons[];
extern char D_0012AC02[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_esc[];
extern char itemmaker_buttons[];
extern char D_001940D4[];
extern char player_object[];
extern char list_popup_callback[];
extern char window_image[];
extern char D_00195C44[];
extern char mouse_buttons_prev[];
extern int spells_list_poll(void);
extern int itemmaker_open(int);
extern int itemmaker_close(void);
extern void itemmaker_draw(void);
extern int sound_play(int, int, int);
extern int func_000CB552();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00135E90();

struct R { short x0, y0, x1, y1; void (*fn)(int); };
#define TAB ((struct R *)itemmaker_buttons)
#define FLAG (((struct bf8_2_1 *)D_001940D4)->f)
#define MX (*(short *)mouse_x)
#define MY (*(short *)mouse_y)

void itemmaker_update(void)
{
    short l_24;
    short l_20;
    short l_1C;
    short l_18;

    if (itemmaker_open(0) == 0) return;
    func_00135E90();
    *(signed char *)D_0012B508 = 146;
    func_000CB552(*(int *)window_image);
    func_0012DB50(4);
    itemmaker_draw();
    func_0012DB50(3);
    l_20 = 0;
    if (FLAG && (l_20 = spells_list_poll()) > -1) {
        while (*key_down_esc != 0) ;
        while (*mouse_buttons != 0) func_0012B136();
        *D_0012AC02 = 0;
        (*(void (**)(int))list_popup_callback)((*(unsigned char **)D_00195C44)[l_20 + 64000]);
        while (*mouse_buttons != 0) func_0012B136();
        *D_0012AC02 = 0;
        return;
    }
    if (l_20 != -2 && *key_down_esc != 0) {
        itemmaker_close();
    } else if (l_20 == -2) {
        while (*key_down_esc != 0) ;
    }
    if (FLAG || (*mouse_buttons == 0 || (*mouse_buttons != 0 && *mouse_buttons_prev != 0))) return;
    for (l_20 = 0; l_20 < 20; l_20++) {
        if (MX > TAB[l_20].x0 && MX < TAB[l_20].x1 && MY > TAB[l_20].y0 && MY < TAB[l_20].y1) {
            sound_play(203, *(int *)player_object, 100);
            TAB[l_20].fn(l_20);
        }
    }
}
