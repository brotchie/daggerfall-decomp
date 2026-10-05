/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000934F6 */
extern int screen_buffer;
extern char D_0017704C[];
extern char D_001770B0[];
extern char D_001770B3[];
extern char D_001770B8[];
extern signed char text_buffer[];
extern char D_00195AA8[];
extern char inv_right_container[];
extern int D_00195B80;
extern int game_minutes;
extern int trade_mode;
extern unsigned char D_0019626F;
extern signed char game_mode;
extern int D_001AA420;
extern char inv_selected_item[];
extern char inv_left_container[];
extern char color_remap_tables[];
struct Rect { short x0, y0, x1, y1; char pad[4]; };

extern void text_draw_coloured(char *, short, short, int, unsigned char);
extern void inv_draw_item_image(char *, struct Rect *, short);
extern void func_00093BD9(char *, struct Rect *, short);
extern void inv_draw_cell_mark(int, int, struct Rect *, short);
extern int mc_strncpy();
extern int mc_memcpy();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, ...);


#define SCREEN (*(char **)&screen_buffer)
#define BUFS ((char **)&D_001AA420)
#define MODE ((unsigned char)game_mode)
#define MODE2 (D_0019626F)
#define U16(p, o) (*(unsigned short *)((p) + (o)))

int inv_draw_item_cell(char *a1, short a2, struct Rect *a3)
{
    short l_10;
    int l_30;
    int l_34;
    short l_18;
    short l_1C;
    int l_40;
    int l_28;
    char *l_38;
    short l_20;
    short l_14;

    l_40 = 0;
    *(char **)D_00195AA8 = a1;
    l_38 = a1 + 71;
    if (a1 == *(char **)inv_selected_item && MODE != 10 && MODE2 != 10) {
        for (l_14 = a3[a2].y0; l_14 <= a3[a2].y1; l_14++)
            mc_memcpy(SCREEN + l_14 * 320 + a3[a2].x0, (char *)(int)BUFS[0] + l_14 * 320 + a3[a2].x0,
                          a3[a2].x1 - a3[a2].x0 + 1, D_0017704C, 681, 4);
    }
    if (*(unsigned char *)a1 == 54) {
        for (l_14 = a3[a2].y0; l_14 <= a3[a2].y1; l_14++)
            mc_memcpy(SCREEN + l_14 * 320 + a3[a2].x0, (char *)(int)BUFS[1] + l_14 * 320 + a3[a2].x0,
                          a3[a2].x1 - a3[a2].x0 + 1, D_0017704C, 687, 4);
    }
    if (a1[38] != 0) {
        for (l_14 = a3[a2].y0; l_14 <= a3[a2].y1; l_14++)
            mc_memcpy(SCREEN + l_14 * 320 + a3[a2].x0, (char *)(int)BUFS[2] + l_14 * 320 + a3[a2].x0,
                          a3[a2].x1 - a3[a2].x0 + 1, D_0017704C, 693, 4);
    }
    if (a1 != *(char **)inv_left_container && a1 != *(char **)inv_right_container && *(short *)(l_38 + 67) != -1
        && MODE != 10 && MODE2 != 10)
        inv_draw_cell_mark(380, 5, a3, a2);
    if ((U16(l_38, 42) & 64) && MODE != 10 && MODE2 != 10)
        inv_draw_cell_mark(380, 7, a3, a2);
    if (U16(a1, 21) & 32)
        inv_draw_cell_mark(380, 6, a3, a2);
    *(char **)&D_00195B80 = *(char **)color_remap_tables + (*(unsigned char *)(l_38 + 56) << 8);
    if (U16(l_38, 32) == 3 && U16(l_38, 34) == 8) {
        l_40++;
        (*(short *)(l_38 + 50))++;
    }
    if (U16(l_38, 32) == 23 && U16(l_38, 34) == 0)
        func_00093BD9(l_38, a3, a2);
    else
        inv_draw_item_image(l_38, a3, a2);
    *(short *)(l_38 + 50) -= l_40;
    *(char **)&D_00195B80 = *(char **)color_remap_tables;
    if (U16(l_38, 32) == 3 && U16(l_38, 34) == 18) {
        mc_set_location(723, D_0017704C);
        mc_sprintf(((char *)text_buffer), D_001770B0, *(unsigned char *)(l_38 + 49));
        text_draw_coloured(((char *)text_buffer), a3[a2].x0 + 3, a3[a2].y0 + 2, 145, 156);
    }
    if (trade_mode == 3 && *(unsigned char *)a1 == 54) {
        if ((unsigned)game_minutes >= *(unsigned *)(a1 + 43)) {
            *(short *)(l_38 + 44) = *(short *)(l_38 + 46);
            mc_strncpy(((char *)text_buffer), D_001770B3, 160, D_0017704C, 732);
        } else {
            l_28 = (*(unsigned *)(a1 + 43) - (unsigned)game_minutes) / 1440;
            if (l_28 == 0) l_28++;
            mc_set_location(738, D_0017704C);
            mc_sprintf(((char *)text_buffer), D_001770B8, l_28);
        }
        text_draw_coloured(((char *)text_buffer), a3[a2].x0 + 3, a3[a2].y0 + 2, 145, 156);
    }
    return 1;
}
