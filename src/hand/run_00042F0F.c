/* matched by the real Watcom C32 10.0a (-d2): a run of keys from 0x425F2 to 0x42F0F, kept together for its switch table's alignment */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
struct item { char pad0[103]; unsigned short type; };
struct pc {
    char pad00[64];
    unsigned short flags;       /* 0x40 */
    char pad42[67];
    int gold;                   /* 0x85 */
    char pad89[4];
    short f141;                 /* 0x8d */
    short f143;                 /* 0x8f */
    char pad91[222];
    struct item *equip[32];     /* 0x16f */
};
struct obj {
    unsigned char type;         /* 0x00 */
    short f1;                   /* 0x01 */
    short f3;                   /* 0x03 */
    short f5;                   /* 0x05 */
    int x;                      /* 0x07 */
    int y;                      /* 0x0b */
    int z;                      /* 0x0f */
};
extern unsigned char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern unsigned char D_0012B508;
extern unsigned char D_00142314;
extern unsigned char D_00142315;
extern unsigned char key_down_ctrl;
extern unsigned char key_down_lshift;
extern unsigned char key_down_x;
extern unsigned char key_down_b;
extern unsigned char key_down_alt;
extern char D_00170E38[];        /* __FILE__ */
extern char D_00170E3F[];
extern unsigned char player_environment;
extern char *D_0018323C;
extern char *D_00183240;
extern char *D_00183244;
extern char *D_00184876;
extern short D_00187CA9;
extern char text_buffer[];
extern struct bits8 D_001940D5;
extern struct bits8 D_001940D6;
extern struct bits8 D_001940D9;
extern struct bits8 D_001940DA;
extern unsigned char player_motion_flags;
extern int D_001950E4;
extern int D_001950E8;
extern int view_look_pitch;
extern int D_001959BC;
extern int D_001959C0;
extern struct obj *camera_object;
extern struct obj *player_object;
extern char *D_00195AC4;
extern int cheat_flags;
extern char *spell_ready_missile;
extern char *spell_ready_touch;
extern struct pc *player_character;
extern int D_00195D60;
extern unsigned char mouse_control_mode;
extern unsigned char view_cursor_active;
extern short D_00195F2E;
extern short D_00195F52;
extern short D_00195F54;
extern short D_00195F62;
extern unsigned char weapon_active_hand;
extern unsigned char D_00196272;
extern unsigned char game_mode;
extern unsigned char interaction_mode;
extern unsigned char in_dungeon_water;
extern unsigned char D_001962A0;
extern int D_00199700;
extern int D_001A4A70;
extern int D_001A4A74;
extern unsigned char cheat_mode;
extern void automap_open(void);
extern void spell_add_skill_uses(char *, int);
extern void interaction_mode_cycle(int);
extern int key_action_held(int);
extern int key_action_pressed(int);
extern void options_toggle_full_screen(void);
extern void cheat_raise_reputation(void);
extern void cheat_raise_skills(void);
extern void game_exit(int);
extern int cast_recast_last(void);
extern void hud_toggle_weapon(void);
extern void magic_items_open(void);
extern void rest_open(void);
extern void cheat_return_to_last_position(void);
extern void saveload_menu(int);
extern int hud_message_add(char *);
extern int key_pressed_once(unsigned char);
extern void object_delete(char *);
extern void object_set_position(struct obj *, int, int, int, int, int, int);
extern void transport_menu(void);
extern struct obj *marker_find_nth(char *, int, int);
extern int travel_map_open(int);
extern void func_0012B49E(short, short);
extern void func_00135E90(void);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);
extern short steer_weight_down;
extern short steer_weight_right;
extern short steer_weight_left;
extern short steer_weight_up;
extern char *D_0017D1CA[];
extern char *D_0017D1EE;
extern unsigned char key_down[];
extern int D_00152A20;
extern int D_00152A24;
extern unsigned char D_00152A30;
extern unsigned char D_00152A31;
extern unsigned char key_map[];
extern void hud_status_set(char *);
extern void func_00042E8C(int);

void keys_world_actions(void)
{
    int u30;
    int u2c;
    int u28;
    int saved;
    struct obj *o;
    int u1c;
    int view;

    if (key_down_ctrl && key_down_x && key_down_lshift)
        game_exit(0);
    if (key_down_ctrl && key_down_b)
        saveload_menu(1);
    if (D_00196272)
        return;
    if (game_mode)
        return;
    if (key_down_alt && key_pressed_once(87))
        cheat_return_to_last_position();
    if (key_action_pressed(29)) {
        if (D_00195D60 == 0)
            cast_recast_last();
        else
            hud_message_add(D_00184876);
    }
    if (key_action_held(27))
        travel_map_open(1);
    if (key_action_held(26))
        automap_open();
    if (key_action_held(30))
        if (spell_ready_missile || spell_ready_touch) {
            player_character->f141 += D_00195F62;
            if (player_character->f141 > player_character->f143)
                player_character->f141 = player_character->f143;
            if (spell_ready_missile) {
                spell_add_skill_uses(spell_ready_missile + 71, -1);
                object_delete(spell_ready_missile);
            } else {
                spell_add_skill_uses(spell_ready_missile + 71, -1);
                object_delete(spell_ready_touch);
            }
            spell_ready_missile = spell_ready_touch = 0;
        }
    if (key_action_pressed(32))
        hud_toggle_weapon();
    if (key_action_pressed(12))
        rest_open();
    if (key_action_pressed(34) && D_001940D6.b6 && (D_001A4A70 | D_001A4A74) == 0) {
        o = (struct obj *)player_character->equip[(weapon_active_hand ^ 1) ? 21 : 19];
        if (o == 0 || ((struct item *)o)->type == 3) {
            weapon_active_hand ^= 1;
            func_000A0ED9(96, D_00170E38);
            mc_sprintf(text_buffer, D_0018323C, weapon_active_hand ? D_00183240 : D_00183244);
            D_00195F2E = 20;
            D_0012B508 = 146;
            hud_message_add(text_buffer);
        }
    }
    if (key_pressed_once(68))
        options_toggle_full_screen();
    view = interaction_mode;
    if (key_action_held(14))
        interaction_mode = 2;
    if (key_action_held(15))
        interaction_mode = 0;
    if (key_action_held(16))
        interaction_mode = 1;
    if (key_action_held(17))
        interaction_mode = 3;
    if (interaction_mode != view)
        interaction_mode_cycle(0);
    if (key_action_held(11) && (player_character->flags & 1536) == 0 && !in_dungeon_water && !D_001962A0)
        D_001940D9.b4 = 1;
    else
        D_001940D9.b4 = 0;
    if (key_action_held(31))
        magic_items_open();
    if (key_action_pressed(19))
        view_cursor_active ^= 1;
    if (key_action_held(13))
        transport_menu();
    if (key_pressed_once(2) && cheat_mode)
        D_00187CA9 ^= 1300;
    if (mouse_control_mode == 0) {
        if (key_action_pressed(22) || D_001940DA.b6 && (mouse_buttons & 1)) {
            view_look_pitch = 32;
            D_001959BC = 0;
            D_001959C0 = 0;
            player_object->f1 = camera_object->f1 = 0;
        }
        if (key_action_held(23)) {
            if (!D_001940DA.b6) {
                D_00195F54 = mouse_x;
                D_00195F52 = mouse_y;
            }
            D_001940DA.b6 = 1;
        } else {
            if (D_001940DA.b6)
                func_0012B49E(D_00195F54, D_00195F52);
            D_001940DA.b6 = 0;
        }
    } else {
        view_look_pitch = 0;
        D_001959BC = 0;
        D_001959C0 = 0;
    }
    if (key_action_held(20))
        view_look_pitch -= 32;
    else if (key_action_held(21))
        view_look_pitch += 32;
    if (view_look_pitch < -256)
        view_look_pitch = -256;
    else if (view_look_pitch > 256)
        view_look_pitch = 256;
    if ((player_character->flags & 1536) == 0 && key_action_pressed(9) && !D_001962A0)
        player_motion_flags ^= 4;
    saved = cheat_flags;
    if (key_down_ctrl && key_pressed_once(59))
        cheat_flags ^= 8;
    if (key_down_ctrl && key_pressed_once(62) && cheat_mode)
        cheat_flags ^= 64;
    if (key_down_ctrl && key_pressed_once(67) && cheat_mode)
        player_character->gold += 5000;
    if (cheat_flags != saved)
        hud_message_add(D_00170E3F);
    if (D_00142314 && cheat_mode)
        cheat_raise_reputation();
    if (D_00142315 && cheat_mode)
        cheat_raise_skills();
    if (key_pressed_once(26) && player_environment == 3 && cheat_mode) {
        D_00199700 = --D_00199700 % (D_001950E4 + D_001950E8);
        if (D_00199700 < 0)
            D_00199700 = D_001950E4 + D_001950E8 - 1;
        if (D_00199700 < D_001950E4)
            o = marker_find_nth(D_00195AC4, 9, D_00199700);
        else
            o = marker_find_nth(D_00195AC4, 16, D_00199700 - D_001950E4);
        object_set_position(player_object, o->x, o->y, o->z, o->f1, o->f3, o->f5);
        camera_object->f3 = player_object->f3;
        func_00135E90();
        D_001940D5.b1 = 1;
    }
    if (key_pressed_once(27) && player_environment == 3 && cheat_mode) {
        D_00199700 = ++D_00199700 % (D_001950E4 + D_001950E8);
        if (D_00199700 < D_001950E4)
            o = marker_find_nth(D_00195AC4, 9, D_00199700);
        else
            o = marker_find_nth(D_00195AC4, 16, D_00199700 - D_001950E4);
        object_set_position(player_object, o->x, o->y, o->z, o->f1, o->f3, o->f5);
        camera_object->f3 = player_object->f3;
        func_00135E90();
        D_001940D5.b1 = 1;
    }
}

void func_00042E0C(void)
{
}

void interaction_mode_cycle(int a1)
{
    interaction_mode = (interaction_mode + a1) & 3;
    func_000A0ED9(231, D_00170E38);
    mc_sprintf(text_buffer, D_0017D1EE, D_0017D1CA[interaction_mode]);
    hud_status_set(text_buffer);
}

void func_00042E8C(int a1)
{
    steer_weight_right = (a1 << 8) / 4096;
    steer_weight_down = (a1 << 8) / 4096;
    steer_weight_left = (-a1 << 8) / 4096;
    steer_weight_up = (-a1 << 8) / 4096;
}

int key_action_held(int a1)
{
    if (key_map[a1] >= 200) {
        switch ((unsigned char)(key_map[a1] - 200)) {
        case 0:
            return D_00152A30;
        case 1:
            return D_00152A31;
        case 2:
            return mouse_buttons & 1;
        case 3:
            return mouse_buttons & 2;
        case 12:
            return mouse_buttons & 4;
        case 4:
            func_00042E8C(D_00152A20);
            return D_00152A20 < 0 ? 1 : 0;
        case 5:
            func_00042E8C(D_00152A20);
            return D_00152A20 > 0 ? 1 : 0;
        case 6:
            func_00042E8C(D_00152A24);
            return D_00152A24 < 0 ? 1 : 0;
        case 7:
            func_00042E8C(D_00152A24);
            return D_00152A24 > 0 ? 1 : 0;
        default:
            return 0;
        }
    }
    return key_down[key_map[a1]];
}
