/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086B0B */
struct who {
    char pad0[7];
    int x;                  /* 0x07 */
    int y;                  /* 0x0b */
    int z;                  /* 0x0f */
    char pad13[0x1b - 0x13];
    unsigned short id;      /* 0x1b */
    char pad1d[0x1f - 0x1d];
    int f1f;
    char pad23[0x3f - 0x23];
    int f3f;
};
struct place {
    int f0;
    unsigned pad:25;
    unsigned type:5;
    unsigned pad2:2;
    char pad8[4];
    unsigned char fc;
};
extern int dungeon_water_level;
extern char D_00176C94[];       /* __FILE__ */
extern char D_00176CC9[];
extern int D_00187F2C;
extern char text_buffer[];
extern char text_rsc_buffer[];
extern unsigned D_0019599C;
extern struct who *D_00195AC4;
extern char *current_location;
extern int game_minutes;
extern int D_00195CB8;
extern int D_00195D48;
extern char D_00196289;
extern char D_0019629B;
extern struct place *location_here;
extern char loaded_location[];
extern char *loaded_location_object;
extern char *loaded_location_data;
extern void location_load_exterior(char *, int);
extern void town_load_blocks(void);
extern void func_00027947(void);
extern void func_00028D1A(void);
extern void people_spawn_tick(void);
extern void people_clear(void);
extern void parse_rsc_text(int, int, int);
extern void func_0004C759(void);
extern void func_0004CA9D(void);
extern void sound_stop_ambient(void);
extern void position_history_reset(void);
extern int hud_message_add(char *);
extern int rand_range(int, int);
extern void func_0007E74E(void);
extern void location_restore_stored(void);
extern void func_00086A71(int);
extern void location_set_discovered(int, int);
extern void object_foreach(struct who *, void (*)(int));
extern void func_0008EAF1(int, int);
extern void func_0008EB52(void);
extern int player_to_nearest_marker(struct who *, int);
extern void func_000A1023(void *, void *, int, char *, int, int);
extern int func_0014B45B(int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void town_load(int id)
{
    if (D_00195AC4->id == id)
        return;
    sound_stop_ambient();
    func_0008EB52();
    if (D_00195AC4->id == 65535)
        func_0008EAF1(D_00195AC4->f3f, D_00195AC4->f1f);
    location_load_exterior(loaded_location, id);
    func_000A1023(D_00195AC4, loaded_location_object, 55, D_00176C94, 365, 4);
    func_000A1023(current_location, loaded_location_data, 48, D_00176C94, 366, 4);
    D_00195AC4->y = func_0014B45B(D_00195AC4->x, D_00195AC4->z);
    func_00027947();
    town_load_blocks();
    location_set_discovered(id, 1);
    func_0007E74E();
    position_history_reset();
    people_clear();
    func_0004CA9D();
    object_foreach(D_00195AC4, func_00086A71);
    if (D_00187F2C != 0) {
        player_to_nearest_marker(D_00195AC4, 8);
        D_00187F2C = 0;
    }
    if (D_00196289 == 0) {
        people_spawn_tick();
        func_0004C759();
        location_restore_stored();
        func_00028D1A();
        switch (location_here->type) {
        case 4:
        case 7:
        case 10:
        case 12:
            parse_rsc_text(location_here->fc + 520, 0, 0);
            hud_message_add(text_rsc_buffer);
            break;
        default:
            func_000A0ED9(401, D_00176C94);
            func_000A0F5C(text_buffer, D_00176CC9, current_location);
            hud_message_add(text_buffer);
            break;
        }
    }
    D_00195CB8 = 0;
    D_0019629B = 0;
    D_00195D48 = 10000;
    dungeon_water_level = 10000;
    D_0019599C = game_minutes + rand_range(1400, 1700);
    func_0008EB52();
}
