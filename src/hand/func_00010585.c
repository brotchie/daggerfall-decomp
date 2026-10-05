/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00010585 */
#include "records.h"

struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
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
extern int left_hand_ready_delay;
extern struct nib frame_counter;
extern struct record *D_00195A88;
extern struct record *player_entity;
extern int frame_ticks;
extern int clothing_gender_group;
extern int creature_count;
extern int clothing_gender_offset;
extern struct image *hud_bar_image;
extern struct character *player_character;
extern struct career *player_class;
extern unsigned int game_minutes;
extern struct settings *game_settings;
extern unsigned int D_00195C4C;
extern int trespassing;
extern int spell_cast_busy;
extern int player_death_timer;
extern int D_00195D8C;
extern short player_base_speed;
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
extern void spell_remove_effect_type(struct record *, int);
extern void func_0006987B(void);
extern void sound_update_ambient(void);
extern int hud_message_add(char *);
extern int building_access_level(int);
extern int func_0007E441(int);
extern void xn_draw_cif_rle_frame(int, int, int, int);
extern void xn_tex_archive_set_translucent(int);
extern void xn_mouse_poll_clamped(void);

void player_frame_update(void)
{
    struct character *rec;
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
        rec = &D_00195A88->data.character;
    else
        rec = player_character;
    if (player_motion_flags.b2)
        speed = rec->attributes[ATTR_SPD] + 50;
    else
        speed = D_00187CA9 + (rec->attributes[ATTR_SPD] - 50);
    sound = 0;
    if (player_character->flags & 512) {
        speed += 225;
        sound = horse_overlay_image;
    } else if (player_character->flags & 1024) {
        speed += 100;
        sound = cart_overlay_image;
    } else if (D_001940D9.b4 && !in_dungeon_water) {
        speed = speed * (((player_character->skills[SKILL_RUNNING].value << 8) / 200) + 320) / 256;
    } else if ((D_001962A0 || in_dungeon_water) && (player_character->conditions & 0x100000) == 0) {
        speed = (speed >>= 2) + speed * ((player_character->skills[SKILL_SWIMMING].value << 8) / 200) / 256;
    }
    if (sound != 0 && game_mode == 0) {
        if (D_0019628E == 0)
            rnd = 0;
        else
            rnd = (*(unsigned int *)0x46c >> 1) & 3;
        xn_draw_cif_rle_frame(sound, rnd, (game_settings->view_flags & 1) ? hud_bar_image->height : 0, 0);
    }
    player_base_speed = (speed * frame_ticks) / 1000;
    if (player_character->flags & 1) {
        clothing_gender_group = 12;
        clothing_gender_offset = 0;
    } else {
        clothing_gender_group = 6;
        clothing_gender_offset = 512;
    }
    if ((D_00195C4C = (100 - player_character->attributes[ATTR_SPD]) * 2 + 70) < 70)
        D_00195C4C = 70;
    else if (D_00195C4C > 800)
        D_00195C4C = 800;
    if (spell_cast_busy != 0) {
        spell_cast_busy -= frame_ticks;
        if (spell_cast_busy < 0)
            spell_cast_busy = 0;
    }
    if (D_0019597C > 0) {
        D_0019597C -= frame_ticks;
        if (D_0019597C <= 0) {
            D_0019597C = 0;
            hud_message_add(D_00170077);
        }
    }
    if (left_hand_ready_delay > 0) {
        left_hand_ready_delay -= frame_ticks;
        if (left_hand_ready_delay <= 0) {
            left_hand_ready_delay = 0;
            hud_message_add(D_0017008C);
        }
    }
    if (game_mode != 8 && game_mode != 2 && msgbox_kind != 0)
        msgbox_close();
    if (creature_count == 0)
        D_0019628D = 0;
    player_character->max_magicka = (rec->attributes[ATTR_INT] * D_001788D3[(player_class->flags >> 10) & 7]) / 256;
    player_character->max_magicka += D_001A3AA8;
    if (player_character->magicka > player_character->max_magicka)
        player_character->magicka = player_character->max_magicka;
    if (player_death_timer > 0) {
        player_death_timer -= frame_ticks;
        if (player_death_timer == 0)
            player_death_timer--;
    }
    if (player_death_timer < 0) {
        play_death_video();
        while (mouse_buttons)
            xn_mouse_poll_clamped();
    }
    if (D_00195D8C != 0) {
        D_00195D8C -= frame_ticks;
        if (D_00195D8C < 0)
            D_00195D8C = 0;
    }
    xn_tex_archive_set_translucent(273);
    xn_tex_archive_set_translucent(278);
    if ((player_character->conditions & 0x400000) && player_character->shield_end_time < game_minutes) {
        spell_remove_effect_type(player_entity, 35);
        player_character->conditions &= ~0x400000;
        player_character->shield_points = 0;
    }
    if (frame_counter.lo == 0)
        func_0006987B();
    sound_update_ambient();
}
