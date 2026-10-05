/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002ECBE */
struct vec3 { int x, y, z; };
struct move { struct vec3 pos; int f12, f16, f20; char *name; };
extern unsigned char D_0012B508;
extern char D_001709A1[];
extern char D_001709C6[];
extern char D_001709E4[];
extern char D_001709ED[];
struct pic { short file, rec; };
extern struct pic monster_corpse_textures[];
extern char D_00187B44[];
extern char text_buffer[];
extern unsigned char D_001940D7;
extern unsigned char *player_entity;
extern int player_object;
extern unsigned char *D_00195AF4;
extern int guild_npc_object;
extern unsigned char *player_character;
extern int game_minutes;
extern unsigned short *game_settings;
extern int D_00195C74;
extern int player_death_timer;
extern short D_00195DA0;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(unsigned char *, int, struct move *, int);
extern void func_0002EBDE(int);
extern void func_0002EC79(int);
extern void quest_raise_event(int, unsigned char *, int);
extern int item_artifact_equipped(int);
extern void sound_play(int, int, int);
extern void guild_count_crime(int, unsigned char);
extern int hud_message_add(char *);
extern int rand_range(int, int);
extern unsigned char *object_create_child(int, int, int);
extern void object_foreach(unsigned char *, void (*)(int));
extern int func_000A0040();
extern int func_000A1023();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, ...);

void damage_creature_death(unsigned char *a1)
{
    int l_30;
    unsigned char *l_2C;
    unsigned char *l_28;
    struct move l_4C;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = a1 + 71;
    l_20 = player_on_ground;
    l_1C = D_00195C74;
    if (a1 == player_entity) {
        sound_play((*(unsigned short *)(player_character + 64) & 1) + (player_character[67] * 3 + 2) ? 258 : 243, player_object, 100);
        player_death_timer = 1000;
        return;
    }
    if (l_2C[506] == 146)
        guild_count_crime(6, 1);
    D_00195AF4 = a1;
    object_foreach(*(unsigned char **)(a1 + 63), func_0002EC79);
    l_28 = *(unsigned char **)(a1 + 63);
    l_18 = item_artifact_equipped(9);
    if (l_2C[67] < 43) {
        if (l_18 != 0) goto found;
        while (l_28 != 0) {
            if (*l_28 == 19) {
found:
                guild_npc_object = 0;
                object_foreach(*(unsigned char **)(player_entity + 63), func_0002EBDE);
                if (guild_npc_object == 0 && *l_28 == 19) {
                    D_0012B508 = 146;
                    hud_message_add(D_001709A1);
                    hud_message_add(D_001709C6);
                    return;
                }
                if (guild_npc_object != 0) {
                    if (l_18 == 0 && rand_range(0, 100) > *(unsigned short *)(l_28 + 27))
                        break;
                    l_28 = object_create_child(guild_npc_object, 0, 0);
                    *(short *)(l_28 + 21) = 3;
                    *l_28 = 20;
                    *(unsigned short *)(l_28 + 27) = l_2C[67];
                }
                break;
            }
            l_28 = *(unsigned char **)(l_28 + 55);
        }
    }
    if (a1 != player_entity)
        sound_play(17, (int)a1, 105);
    *(int *)(player_character + 509) = game_minutes;
    quest_raise_event(2, a1, 0);
    l_2C = a1 + 71;
    func_000A0ED9(587, D_001709E4);
    func_000A0F5C(text_buffer, D_001709ED, l_2C);
    hud_message_add(text_buffer);
    *a1 = 44;
    if (l_2C[506] < 43) {
        if (*game_settings & 4)
            *(short *)(a1 + 27) = (D_00195DA0 << 7) + 1;
        else
            *(short *)(a1 + 27) = (monster_corpse_textures[l_2C[506]].file << 7) + monster_corpse_textures[l_2C[506]].rec;
    } else {
        *(short *)(a1 + 27) = (D_00195DA0 << 7) + 1;
    }
    collide_flags = 0;
    D_001940D7 |= 32;
    func_000A1023(&l_4C, a1 + 7, 12, D_001709E4, 605, 4);
    func_000A0040(&l_4C.f12, 0, 12, D_001709E4, 606, 4);
    l_4C.name = D_00187B44;
    collide_move_object(a1, 0, &l_4C, 0);
    player_on_ground = l_20;
    D_00195C74 = l_1C;
}
