/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00010585 */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
struct mobile {
    char pad0[34];
    short f34;                  /* 0x22 */
    char pad24[8];
    short f44;                  /* 0x2c */
    char pad2e[18];
    unsigned short flags;       /* 0x40 */
    char pad42[34];
    unsigned int f100;          /* 0x64 */
    char pad68[35];
    struct bits8 f139;          /* 0x8b */
    char pad8c;
    short f141;                 /* 0x8d */
    short f143;                 /* 0x8f */
    char pad91[114];
    short f259;                 /* 0x103 */
    char pad105[22];
    short f283;                 /* 0x11b */
    char pad11d[252];
    int f537;                   /* 0x219 */
};
struct thing { unsigned char type; char pad[70]; struct mobile mob; };   /* 10.0a packs structs (-zp1) */
struct w0 { unsigned short f0; };
struct w4 { char pad[4]; unsigned short f4; };
struct w6 { char pad[6]; unsigned short f6; };
struct nib { unsigned char lo:4; };
extern unsigned char mouse_buttons;
extern char D_00170077[];
extern char D_0017008C[];
extern short D_001788D3[];
extern unsigned char player_environment;
extern int D_00187CA9;
extern struct bits8 D_001940D9;
extern struct bits8 player_motion_flags;
extern int horse_overlay_image;
extern int cart_overlay_image;
extern int D_0019597C;
extern int D_00195980;
extern struct nib frame_counter;
extern struct thing *D_00195A88;
extern int player_entity;
extern int D_00195AB0;
extern int clothing_gender_group;
extern int creature_count;
extern int D_00195B18;
extern struct w6 *hud_bar_image;
extern struct mobile *player_character;
extern struct w4 *player_class;
extern unsigned int game_minutes;
extern struct w0 *game_settings;
extern unsigned int D_00195C4C;
extern int trespassing;
extern int D_00195D60;
extern int player_death_timer;
extern int D_00195D8C;
extern short D_00195F4E;
extern unsigned char msgbox_kind;
extern unsigned char game_mode;
extern unsigned char in_dungeon_water;
extern unsigned char crime_current;
extern unsigned char D_0019628C;
extern unsigned char D_0019628D;
extern unsigned char D_0019628E;
extern unsigned char D_001962A0;
extern short D_001A3AA8;
extern void play_death_video(void);
extern void msgbox_close(void);
extern void guards_summon(int);
extern void spell_remove_effect_type(int, int);
extern void func_0006987B(void);
extern void sound_update_ambient(void);
extern int hud_message_add(char *);
extern int building_access_level(int);
extern int func_0007E441(int);
extern void func_000CB39A(int, int, int, int);
extern void func_000CDC99(int);
extern void func_0012B136(void);

void player_frame_update(void)
{
    struct mobile *rec;
    int sound;
    int speed;
    int old;
    int rnd;

    old = trespassing;
    switch (player_environment) {
    case 1:
        trespassing = 0;
        break;
    case 2:
        trespassing = func_0007E441(2) - building_access_level(0);
        if (trespassing < 0)
            trespassing = 0;
        break;
    case 3:
        trespassing = D_0019628C > 5;
        break;
    }
    if (trespassing) {
        crime_current = old == 0 ? 2 : 3;
        guards_summon(0);
    }
    if (D_00195A88 != 0 && D_00195A88->type == 18)
        rec = &D_00195A88->mob;
    else
        rec = player_character;
    if (player_motion_flags.b2)
        speed = rec->f44 + 50;
    else
        speed = D_00187CA9 + (rec->f44 - 50);
    sound = 0;
    if (player_character->flags & 512) {
        speed += 225;
        sound = horse_overlay_image;
    } else if (player_character->flags & 1024) {
        speed += 100;
        sound = cart_overlay_image;
    } else if (D_001940D9.b4 && !in_dungeon_water) {
        speed = speed * (((player_character->f283 << 8) / 200) + 320) / 256;
    } else if ((D_001962A0 || in_dungeon_water) && !player_character->f139.b4) {
        speed = (speed >>= 2) + speed * ((player_character->f259 << 8) / 200) / 256;
    }
    if (sound != 0 && game_mode == 0) {
        if (D_0019628E == 0)
            rnd = 0;
        else
            rnd = (*(unsigned int *)0x46c >> 1) & 3;
        func_000CB39A(sound, rnd, (game_settings->f0 & 1) ? hud_bar_image->f6 : 0, 0);
    }
    D_00195F4E = (speed * D_00195AB0) / 1000;
    if (player_character->flags & 1) {
        clothing_gender_group = 12;
        D_00195B18 = 0;
    } else {
        clothing_gender_group = 6;
        D_00195B18 = 512;
    }
    if ((D_00195C4C = (100 - player_character->f44) * 2 + 70) < 70)
        D_00195C4C = 70;
    else if (D_00195C4C > 800)
        D_00195C4C = 800;
    if (D_00195D60 != 0) {
        D_00195D60 -= D_00195AB0;
        if (D_00195D60 < 0)
            D_00195D60 = 0;
    }
    if (D_0019597C > 0) {
        D_0019597C -= D_00195AB0;
        if (D_0019597C <= 0) {
            D_0019597C = 0;
            hud_message_add(D_00170077);
        }
    }
    if (D_00195980 > 0) {
        D_00195980 -= D_00195AB0;
        if (D_00195980 <= 0) {
            D_00195980 = 0;
            hud_message_add(D_0017008C);
        }
    }
    if (game_mode != 8 && game_mode != 2 && msgbox_kind != 0)
        msgbox_close();
    if (creature_count == 0)
        D_0019628D = 0;
    player_character->f143 = (rec->f34 * D_001788D3[(player_class->f4 >> 10) & 7]) / 256;
    player_character->f143 += D_001A3AA8;
    if (player_character->f141 > player_character->f143)
        player_character->f141 = player_character->f143;
    if (player_death_timer > 0) {
        player_death_timer -= D_00195AB0;
        if (player_death_timer == 0)
            player_death_timer--;
    }
    if (player_death_timer < 0) {
        play_death_video();
        while (mouse_buttons)
            func_0012B136();
    }
    if (D_00195D8C != 0) {
        D_00195D8C -= D_00195AB0;
        if (D_00195D8C < 0)
            D_00195D8C = 0;
    }
    func_000CDC99(273);
    func_000CDC99(278);
    if (player_character->f139.b6 && player_character->f100 < game_minutes) {
        spell_remove_effect_type(player_entity, 35);
        player_character->f139.b6 = 0;
        player_character->f537 = 0;
    }
    if (frame_counter.lo == 0)
        func_0006987B();
    sound_update_ambient();
}
