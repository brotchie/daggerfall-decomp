/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005DB95 */
#pragma pack(1)
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct Obj { char pad0[3]; short angle; char pad1[2]; int x; int y; int z; };
extern int D_000C23BC;
extern char *screen_buffer;
extern char D_00175898[];
extern struct Obj *detect_target;
extern struct Obj *player_object;
extern unsigned short *game_settings;
extern char *compass_image;
extern struct Img *compass_box_image;
extern char game_mode;
extern int ai_angle_diff(int, int, int *);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern int func_000C808D();
extern int func_00144F68();

void hud_draw_heading_strip(int a1)
{
    int i;
    int off;
    int dir;
    int dist;
    int pos;

    if (a1 == 0) {
        if ((int)(unsigned short)(*game_settings & 1) == 0)
            return;
        if (game_mode != 0)
            return;
        func_00144F68(compass_box_image->x, compass_box_image->y, compass_box_image->w, compass_box_image->h, compass_box_image->data);
        off = ((short)(player_object->angle & 0x7ff) << 5) / 256;
        for (i = 185; i <= 197; i++)
            func_000A1023(screen_buffer + (i * 320 + 253), (i - 185) * 322 + (compass_image + off), 65, D_00175898, 440, 4);
        if (detect_target != 0) {
            dir = ai_angle_diff(player_object->angle, func_000C808D(player_object->x, player_object->z, detect_target->x, detect_target->z), &dist);
            dir = (dir << 5) / 256;
            if (dir > 32)
                dir = 32;
            pos = dir * dist + 57885;
            screen_buffer[pos - 2] = screen_buffer[pos - 1] = screen_buffer[pos] = screen_buffer[pos + 1] = screen_buffer[pos + 2] = 245;
            pos += 320;
            screen_buffer[pos - 1] = screen_buffer[pos] = screen_buffer[pos + 1] = 245;
            pos += 320;
            screen_buffer[pos] = 245;
        }
        return;
    }
    func_00144F68(compass_box_image->x - 250, compass_box_image->y - 11, compass_box_image->w, compass_box_image->h, compass_box_image->data);
    off = ((D_000C23BC & 2047) << 5) / 256;
    for (i = 174; i <= 186; i++)
        func_000A1023(screen_buffer + (i * 320 + 3), (i - 174) * 322 + (compass_image + off), 65, D_00175898, 460, 4);
}
