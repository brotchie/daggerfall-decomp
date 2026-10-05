/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009C00A */
#pragma pack(1)
struct Q { char pad[4]; char name[28]; };
struct C { char pad[0x85]; int v; };
extern char D_0012B508;
extern char D_0017743D[];
extern unsigned short travel_options;
extern short D_0018886C[];
extern int travel_selected_location;
extern char text_buffer[];
extern struct C *player_character;
extern struct Q *D_00196A7C;
extern int D_001AA680;
extern unsigned short *D_001AA6A0;
extern unsigned char D_001AA6A6;
extern void text_draw_colored(char *, short, short, int, unsigned char);
extern void text_draw_centered_colored(char *, short, short, int, unsigned char);
extern int travel_trip_cost(void);
extern void mc_strncpy(char *, char *, int, char *, int);
extern char *utoa(int, char *, int);
extern char *func_000A0DD9(int, char *, int);
extern void func_00144D00(short, short, short, short);
extern int func_00144F68();

void travel_draw_trip_popup(void)
{
    int i;

    func_00144F68(D_001AA6A0[0], D_001AA6A0[1], D_001AA6A0[2], D_001AA6A0[3], (char *)D_001AA6A0 + 12);
    if (D_001AA6A6 == 100) {
        mc_strncpy(text_buffer, D_00196A7C[travel_selected_location].name, 160, D_0017743D, 654);
        text_draw_centered_colored(text_buffer, 160, 74, 145, 156);
        return;
    }
    D_0012B508 = 199;
    for (i = 0; i < 6; i++) {
        if (travel_options & (1 << i))
            func_00144D00(D_0018886C[i * 4], D_0018886C[i * 4 + 1], 4, 4);
    }
    text_draw_colored(func_000A0DD9(player_character->v, text_buffer, 10), 148, 97, 145, 156);
    text_draw_colored(utoa(travel_trip_cost(), text_buffer, 10), 117, 107, 146, 156);
    text_draw_colored(func_000A0DD9(D_001AA680 / 1440 + 1, text_buffer, 10), 129, 117, 145, 156);
    mc_strncpy(text_buffer, D_00196A7C[travel_selected_location].name, 160, D_0017743D, 668);
    text_draw_centered_colored(text_buffer, 160, 2, 145, 156);
}
