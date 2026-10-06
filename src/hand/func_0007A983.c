/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007A983 */
#include "records.h"
#include "clib.h"
#include "doslow.h"

extern int xn_cam_x;
extern int xn_cam_y;
extern int xn_cam_z;
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
extern iptr D_00186503[];
extern int screen_shake;
extern signed char text_rsc_buffer[];
extern char arena2_path[];
extern struct record *nonworld_root;
extern struct record *inventory_containers[];
extern iptr quest_root;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern int D_00195B44;
extern struct location *current_location;
extern struct character *player_character;
extern struct career *player_class;
extern int game_minutes;
extern struct settings *game_settings;
extern int realtime_clock_tick;
extern int sky_loaded_frame;
extern iptr D_00195D84;
extern int D_00195DA0;
extern char view_cursor_active;
extern unsigned char current_region;
extern unsigned char mouse_buttons_prev;
extern char world_loading;
extern char night_sky_loaded;
extern int save_file_handle;
extern int save_version;
extern char cfg_mapsave_file[];
extern char terrain_cell_ids[];
extern char terrain_cell_dirty[];
extern int climate_at(int, int);
extern void automap_restore_seen(void);
extern void func_00028F8B(struct record *);
extern void quests_relink_all(iptr);
extern void quest_faces_after_load(void);
extern void building_update_open_state(void);
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
extern void load_drop_spawned_markers(void);
extern void load_requeue_s0000021(void);
extern void load_copy_automap_files(char *);
extern void savevars_read(char *);
extern void load_reset_state(void);
extern void load_fix_objects(void);
extern struct building *object_building(struct record *);
extern void location_unload(int);
extern void map_goto_location(int, int, int, int);
extern void object_free_children(struct record *);
extern struct record *object_free_single(struct record *);
extern struct record *object_reparent(struct record *, struct record *);
extern void object_unlink(struct record *);
extern void object_foreach(struct record *, void (*)());
extern int object_tree_size(struct record *);
extern void inv_reset_left_list(void);
extern void xn_world_reload(void);
extern void xn_cam_set_view_window(int, int, int, int);
extern void xn_mouse_poll_clamped(void);
#pragma aux mc_set_location parm routine [];

int load_game(char *name)
{
    int fd;
    int l20;
    int sz;
    unsigned short l18;
    struct vec3 vec;
    int l24;
    char buf[1024];

    mc_set_location(637, D_00176884);
    mc_sprintf(buf, D_00176909, name);
    mc_set_location(638, D_00176884);
    mc_sprintf(((char *)text_rsc_buffer), D_001768DF, buf, D_00176927);
    fd = open(((char *)text_rsc_buffer), 512);
    if (fd < 0)
        return 0;
    read(fd, &save_version, 4);
    close(fd);
    mem_check_now(1002);
    location_unload(location_object->image);
    if (save_version < 293 || save_version > 294)
        fatal_error(D_0017694A);
    mc_memset((char *)inventory_containers, 0, 36, D_00176884, 655, 36);
    object_unlink(player_object);
    object_free_children(player_object);
    object_free_children(location_object);
    object_free_children(nonworld_root);
    object_reparent(location_object, player_object);
    player_entity = 0;
    disk_copy_file(D_00176915, buf, arena2_path);
    disk_copy_file(D_0017691F, buf, arena2_path);
    disk_copy_file(cfg_mapsave_file, buf, arena2_path);
    load_copy_automap_files(buf);
    savevars_read(name);
    load_reset_state();
    l24 = current_region;
    current_region = 255;
    mc_set_location(677, D_00176884);
    mc_sprintf(((char *)text_rsc_buffer), D_001768DF, buf, D_00176927);
    save_file_handle = open(((char *)text_rsc_buffer), 512);
    read(save_file_handle, &save_version, 4);
    read(save_file_handle, &vec, 12);
    read(save_file_handle, &l18, 2);
    read(save_file_handle, &player_environment, 1);
    climate_at(vec.x, vec.z);
    if (l18 != 65535) {
        map_goto_location(l24, player_environment, l18, 0);
    } else {
        xn_cam_x = vec.x;
        xn_cam_y = vec.y;
        xn_cam_z = vec.z;
        xn_world_reload();
        mc_memset(terrain_cell_dirty, 0, 16, D_00176884, 696, 16);
        mc_memset(terrain_cell_ids, 0, 16, D_00176884, 697, 16);
    }
    object_free_single(player_object);
    sz = current_location->building_count * 26;
    read(save_file_handle, &sz, 4);
    read(save_file_handle, current_location->buildings, sz);
    savetree_read_records(location_object);
    savetree_read_records(nonworld_root);
    links_load(save_file_handle);
    close(save_file_handle);
    load_fix_objects();
    load_drop_spawned_markers();
    quest_faces_after_load();
    monster_reload_anims();
    load_relink_all();
    inv_reset_left_list();
    weapon_reload_sprites();
    player_refresh_paperdoll();
    load_requeue_s0000021();
    quests_relink_all(quest_root);
    func_0004C759();
    if (player_environment == 3)
        automap_restore_seen();
    if (player_environment == 2)
        D_00195D84 = D_00186503[(current_building = object_building(player_object))->type];
    if ((int)(unsigned short)(game_settings->view_flags & 1) != 0)
        xn_cam_set_view_window(160, 100, 160, 100);
    else
        xn_cam_set_view_window(160, 77, 160, 77);
    sound_set_volume(game_settings->music_volume);
    mouse_buttons = mouse_buttons_prev = 0;
    xn_mouse_poll_clamped();
    while (mouse_buttons != 0)
        xn_mouse_poll_clamped();
    realtime_clock_tick = *(int *)DOS_LOW(0x46C);
    world_loading = 0;
    building_update_open_state();
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
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
    object_foreach(location_object, func_00028F8B);
    D_00195B44 = game_minutes;
    mem_check_now(1003);
    l20 = object_tree_size(location_object);
    l20 = object_tree_size(nonworld_root);
    player_character->max_magicka = player_character->attributes[ATTR_INT] * D_001788D3[(player_class->flags >> 10) & 7] / 256;
    if (player_character->magicka < 0)
        player_character->magicka = 0;
    return 1;
}
