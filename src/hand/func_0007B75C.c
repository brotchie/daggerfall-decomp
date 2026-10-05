/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B75C */
extern signed char D_0012B508;
extern short D_00142928;
extern short D_0014292C;
extern int screen_buffer;
extern int D_00147954;
extern char D_00176884[];
extern char saveload_buttons[];
extern char D_00187A92[];
extern char D_00187A94[];
extern char D_00187A96[];
extern char D_00195B5C[];
extern int window_image;
extern char scratch_buffer[];
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern int mc_memcpy();
extern int xn_draw_image();
extern int xn_draw_line_to();

void saveload_draw(int saving, int used_slots, int slot)
{
    int i;

    mc_memcpy(screen_buffer, window_image, 64000, (int)D_00176884, 977, 4);
    if (saving == 0) goto L7B7D9;
    xn_draw_image((int)(unsigned short)*(short *)(*(char **)D_00195B5C), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 2), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 6), (int)(*(char **)D_00195B5C + 12));
L7B7D9:;
    for (i = 0; i < 6; i++) {
        if (((1 << i) & used_slots) != 0) {
            xn_draw_image((((i) < 3) ? 40 : 200), ((i % 3) * 65) + 4, 80, 50, (int)(*(char **)&D_00147954 + (i * 4000)));
            text_draw_centred_coloured((int)(*(char **)scratch_buffer + (i << 5)), (int)(short)(((i) < 3) ? 80 : 246), (int)(short)(((i % 3) * 65) + 57), 145, 156);
        }
    }

    i = ((slot < 3) ? slot : slot + 3);
    D_0012B508 = 146;
    D_00142928 = *(short *)(saveload_buttons + (i * 12)) - 1;
    D_0014292C = *(short *)(D_00187A92 + (i * 12)) - 1;
    xn_draw_line_to((int)(short)(*(short *)(D_00187A94 + (i * 12)) + 1), (int)(short)(*(short *)(D_00187A92 + (i * 12)) - 1));
    xn_draw_line_to((int)(short)(*(short *)(D_00187A94 + (i * 12)) + 1), (int)(short)(*(short *)(D_00187A96 + (i * 12)) + 1));
    xn_draw_line_to((int)(short)(*(short *)(saveload_buttons + (i * 12)) - 1), (int)(short)(*(short *)(D_00187A96 + (i * 12)) + 1));
    xn_draw_line_to((int)(short)(*(short *)(saveload_buttons + (i * 12)) - 1), (int)(short)(*(short *)(D_00187A92 + (i * 12)) - 1));
}
