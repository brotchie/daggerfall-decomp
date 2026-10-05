/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007A983 */
#include "records.h"

#pragma pack(1)
struct Vec { int x; int y; int z; };
#pragma pack()
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern unsigned char mouse_buttons;
extern char D_00176884[];
extern char D_001768DF[];
extern char D_00176909[];
extern char D_00176915[];
extern char D_0017691F[];
extern char D_00176927[];
extern char D_0017694A[];
extern char D_00176964[];
extern short D_001788D3[];
extern unsigned char player_environment;
extern unsigned short spell_last_cast_id;
extern int D_00186503[];
extern int screen_shake;
extern char text_rsc_buffer[];
extern char D_001917E4[];
extern struct record *nonworld_root;
extern char inventory_containers[];
extern int D_00195A00;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern int D_00195B44;
extern struct location *current_location;
extern struct character *player_character;
extern struct career *player_class;
extern int game_minutes;
extern struct settings *game_settings;
extern int D_00195C40;
extern int D_00195D48;
extern int D_00195D84;
extern int D_00195DA0;
extern char view_cursor_active;
extern unsigned char current_region;
extern unsigned char mouse_buttons_prev;
extern char D_00196289;
extern char D_0019629B;
extern int save_file_handle;
extern int save_version;
extern char cfg_mapsave_file[];
extern char D_001A94A0[];
extern char D_001A94B0[];
extern int func_00020057(int, int);
extern void automap_restore_seen(void);
extern void func_00028F8B();
extern void quests_relink_all(int);
extern void func_00030C10(void);
extern void func_0004B5CF(void);
extern void func_0004C759(void);
extern void player_refresh_paperdoll(void);
extern void fatal_error(char *);
extern void links_load(int);
extern void sound_set_volume(short);
extern void sound_stop_ambient(void);
extern void mem_check_now(int);
extern int disk_file_exists(char *);
extern void disk_copy_file(char *, char *, char *);
extern void weapon_reload_sprites(void);
extern void monster_reload_anims(void);
extern void savetree_read_records(struct record *);
extern void load_relink_all(void);
extern void func_0007A4B3(void);
extern void func_0007A8BE(void);
extern void load_copy_automap_files(char *);
extern void savevars_read(char *);
extern void func_0007C432(void);
extern void load_fix_objects(void);
extern struct building *object_building(struct record *);
extern void location_unload(unsigned short);
extern void map_goto_location(int, int, int, int);
extern void object_free_children(struct record *);
extern struct record *object_free_single(struct record *);
extern struct record *object_reparent(struct record *, struct record *);
extern void object_unlink(struct record *);
extern void object_foreach(struct record *, void (*)());
extern int object_tree_size(struct record *);
extern void inv_reset_left_list(void);
extern int func_0009DEA7(int);
extern int mc_memset(void *, int, int, char *, int, int);
extern int func_000A00CB(int, void *, int);
extern int func_000C2FF5();
extern int func_0012A2D0();
extern int func_0012B136();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int open(char *, ...);
extern int mc_sprintf(char *, char *, ...);

int load_game(char *name)
{
    int fd;
    int l20;
    int sz;
    unsigned short l18;
    struct Vec vec;
    int l24;
    char buf[1024];

    func_000A0ED9(637, D_00176884);
    mc_sprintf(buf, D_00176909, name);
    func_000A0ED9(638, D_00176884);
    mc_sprintf(text_rsc_buffer, D_001768DF, buf, D_00176927);
    fd = open(text_rsc_buffer, 512);
    if (fd < 0)
        return 0;
    func_000A00CB(fd, &save_version, 4);
    func_0009DEA7(fd);
    mem_check_now(1002);
    location_unload(D_00195AC4->image);
    if (save_version < 293 || save_version > 294)
        fatal_error(D_0017694A);
    mc_memset(inventory_containers, 0, 36, D_00176884, 655, 36);
    object_unlink(player_object);
    object_free_children(player_object);
    object_free_children(D_00195AC4);
    object_free_children(nonworld_root);
    object_reparent(D_00195AC4, player_object);
    player_entity = 0;
    disk_copy_file(D_00176915, buf, D_001917E4);
    disk_copy_file(D_0017691F, buf, D_001917E4);
    disk_copy_file(cfg_mapsave_file, buf, D_001917E4);
    load_copy_automap_files(buf);
    savevars_read(name);
    func_0007C432();
    l24 = current_region;
    current_region = 255;
    func_000A0ED9(677, D_00176884);
    mc_sprintf(text_rsc_buffer, D_001768DF, buf, D_00176927);
    save_file_handle = open(text_rsc_buffer, 512);
    func_000A00CB(save_file_handle, &save_version, 4);
    func_000A00CB(save_file_handle, &vec, 12);
    func_000A00CB(save_file_handle, &l18, 2);
    func_000A00CB(save_file_handle, &player_environment, 1);
    func_00020057(vec.x, vec.z);
    if (l18 != 65535) {
        map_goto_location(l24, player_environment, l18, 0);
    } else {
        D_000C23C4 = vec.x;
        D_000C23C8 = vec.y;
        D_000C23CC = vec.z;
        func_000C2FF5();
        mc_memset(D_001A94B0, 0, 16, D_00176884, 696, 16);
        mc_memset(D_001A94A0, 0, 16, D_00176884, 697, 16);
    }
    object_free_single(player_object);
    sz = current_location->building_count * 26;
    func_000A00CB(save_file_handle, &sz, 4);
    func_000A00CB(save_file_handle, current_location->buildings, sz);
    savetree_read_records(D_00195AC4);
    savetree_read_records(nonworld_root);
    links_load(save_file_handle);
    func_0009DEA7(save_file_handle);
    load_fix_objects();
    func_0007A4B3();
    func_00030C10();
    monster_reload_anims();
    load_relink_all();
    inv_reset_left_list();
    weapon_reload_sprites();
    player_refresh_paperdoll();
    func_0007A8BE();
    quests_relink_all(D_00195A00);
    func_0004C759();
    if (player_environment == 3)
        automap_restore_seen();
    if (player_environment == 2)
        D_00195D84 = D_00186503[(current_building = object_building(player_object))->type];
    if ((int)(unsigned short)(*(unsigned short *)game_settings & 1) != 0)
        func_0012A2D0(160, 100, 160, 100);
    else
        func_0012A2D0(160, 77, 160, 77);
    sound_set_volume(game_settings->music_volume);
    mouse_buttons = mouse_buttons_prev = 0;
    func_0012B136();
    while (mouse_buttons != 0)
        func_0012B136();
    D_00195C40 = *(int *)0x46c;
    D_00196289 = 0;
    func_0004B5CF();
    D_00195D48 = 10000;
    D_0019629B = 0;
    spell_last_cast_id = 65535;
    screen_shake = 0;
    view_cursor_active = 0;
    if (disk_file_exists(D_00176964) != 0) {
        game_settings->view_flags |= 4;
        D_00195DA0 = 434;
    } else {
        game_settings->view_flags &= ~4;
        D_00195DA0 = 380;
    }
    sound_stop_ambient();
    object_foreach(D_00195AC4, func_00028F8B);
    D_00195B44 = game_minutes;
    mem_check_now(1003);
    l20 = object_tree_size(D_00195AC4);
    l20 = object_tree_size(nonworld_root);
    player_character->max_magicka = player_character->attributes[ATTR_INT] * D_001788D3[(player_class->flags >> 10) & 7] / 256;
    if (player_character->magicka < 0)
        player_character->magicka = 0;
    return 1;
}
