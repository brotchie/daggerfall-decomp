/* init.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"

extern char disk_last_file_size[];
extern int xn_cam_x;
extern int xn_cam_y;
extern int xn_cam_z;
extern int internal_check_failed;
extern int xn_cam_near_z;
extern int xn_cam_far_z;
extern signed char D_0012B508;
extern int dungeon_water_level;
extern int xn_light_ambient;
extern iptr xn_fog_table_last;
extern char D_00175040[];
extern char D_00175047[];
extern char D_00175207[];
extern char D_00175214[];
extern char D_0017521C[];
extern char D_00175226[];
extern char D_0017523C[];
extern char D_00175253[];
extern char D_00175260[];
extern char D_00175266[];
extern char D_00175281[];
extern char D_001752B6[];
extern char D_001752D2[];
extern char D_00175307[];
extern char D_00175313[];
extern char D_00175324[];
extern char D_00175331[];
extern char D_0017533E[];
extern char D_0017534B[];
extern char D_00175358[];
extern char D_00175365[];
extern short D_001788D3[];
extern unsigned char player_environment;
extern int mem_check_level;
extern int frame_checkpoint;
extern int engine_running;
extern char saved_positions[];
extern struct region regions[];
extern signed char text_buffer[];
extern struct record *creature_list[];
extern iptr D_00190704[];
extern iptr hud_compass_image;
extern iptr D_00190908;
extern iptr D_0019090C;
extern iptr D_00190910;
extern char arena2_path[];
extern char arena2_cd_path[];
extern struct flat_cfg flats_cfg[];
extern unsigned char D_001940D7;
extern signed char D_001940D8;
extern iptr horse_overlay_image;
extern iptr cart_overlay_image;
extern int last_skill_check_minutes;
extern struct record *nonworld_root;
extern struct record *logbook_object;
extern struct record *options_object;
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *D_001959E0;
extern struct record *D_001959E4;
extern struct record *house_container;
extern struct record *ship_container;
extern struct record *room_storage_container;
extern struct record *repair_container;
extern struct record *quest_root;
extern struct record *bank_accounts;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern int monster_bsa_handle;
extern iptr D_00195AD0;
extern struct image *list_popup_image;
extern struct spell *spell_records;
extern int creature_count;
extern struct region *current_region_data;
extern iptr D_00195B64;
extern struct image *hud_bar_image;
extern iptr hud_mode_icons;
extern iptr paperdoll_mask;
extern char *hud_portrait;
extern iptr icon_image;
extern char *buttons_rci;
extern struct character *player_character;
extern iptr window_image;
extern struct career *player_class;
extern int game_minutes;
extern struct settings *game_settings;
extern char *scratch_buffer;
extern signed char cfg_map_file;
extern iptr hud_vital_bar_images;
extern iptr D_00195C80;
extern iptr D_00195C84;
extern int flats_cfg_count;
extern iptr spell_cast_anim_fire[];
extern iptr spell_cast_anim_frost;
extern iptr spell_cast_anim_poison;
extern iptr spell_cast_anim_shock;
extern iptr spell_cast_anim_magic;
extern iptr D_00195CF4;
extern iptr D_00195CF8;
extern iptr quest_face_images[];
extern iptr D_00195D18;
extern iptr D_00195D20;
extern iptr D_00195D24;
extern iptr magic_def;
extern iptr hud_portrait_overlays;
extern iptr books_path;
extern iptr compass_image;
extern iptr compass_box_image;
extern iptr qbn_opcode_arg_counts;
extern signed char climate_weathers[];
extern signed char view_cursor_active;
extern signed char D_0019621B;
extern iptr D_0019625E;
extern signed char current_region;
extern signed char player_underwater;
extern signed char fog_colour;
extern signed char exiting;
extern int maps_bsa;
extern iptr D_001997F0;
extern iptr D_001997F4;
extern int D_001997FC;
extern int D_00199800;
extern iptr D_00199804;
extern int daylight;
extern char D_001A3F60[];
extern int D_001A3F7C;
extern int D_001A3F90;
extern int D_001A3F9C;
extern char cfg_mapsave_file[];
extern int cfg_start_map;
extern signed char cfg_region;
extern int arch3d_bsa;
extern int dagger_snd;
extern iptr D_001AA5FC;

extern int climate_category(void);
extern iptr disk_read_file(char *, iptr);
extern int rand_range(int, int);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int marker_count(struct record *, int);
extern int player_to_nearest_marker(struct record *, int);
extern void xn_anim_rand_seed(void);
extern void xn_sys_restore_crit_error_handler(void);
extern void xn_world_init(void);
extern void xn_world_open(char *);
extern void xn_sys_set_dos_transfer_buffer(void);
extern void xn_shade_keep_colours_0_255(void);
extern void xn_draw_fullscreen_overlay_shaded(char *);
extern void xn_mouse_set_range_320x200(void);
extern void xn_mouse_set_sensitivity(int, int);
extern void xn_mouse_get_sensitivity(char *, char *);
extern void xn_timer_wait_ticks(unsigned);
extern void xn_kbd_numlock_off(void);
extern void xn_render_init(void);
extern void xn_render_shutdown(void);
extern void xn_render_set_mode(int);
extern void xn_cam_set_focal(int, int);
extern void xn_cam_set_view_window(int, int, int, int);
extern void xn_mouse_init(void);
extern void xn_pal_set(char *);
extern void xn_font_init(void);
extern void xn_light_init(void);
extern void xn_shade_load(int, int);
extern void xn_kbd_install(void);
extern void xn_kbd_remove(void);
extern int xn_gfx_set_mode(int, int);
extern void xn_gfx_restore_mode(void);
extern void xn_gfx_clear(int);
extern void xn_mem_init(unsigned);
extern void xn_mem_shutdown(void);
extern void xn_shade_set_fog(int);
extern void xn_joy_shutdown(void);
extern void archive_close(int);
extern void faction_load_file(void);
extern void faction_free(void);
extern void region_free_tables(void);
extern void region_enter(unsigned char, unsigned char);
extern void automap_load(void);
extern void sky_free(void);
extern void text_rsc_close(void);
extern void msgbox_free_borders(void);
extern void kludge_print_build(int);
extern void calendar_update(void);
extern void paperdoll_draw(int, int);
extern void text_draw(char *, short, short);
extern void sound_shutdown_music(void);
extern void mem_check_crt_heap(int);
extern void disk_copy_file(char *, char *, char *);
extern void file_index_build(void);
extern void file_index_free(void);
extern void weapon_reload_hand_sprites(void);
extern void weapon_free_sprites(void);
extern void func_0007EF20(void);
extern void intrface_free(void);
extern void player_compute_jump_velocity(void);
extern void model_heap_free_all(void);
extern void sound_cache_free_all(void);
extern void region_unload(void);
extern void dungeon_load(int);
extern void location_unload(int);
extern void map_goto_location(int, int, int, int);
extern void object_heap_init(void);
extern void object_heap_shutdown(void);
extern void object_free_children(struct record *);
extern void inv_reset_left_list(void);
extern void dpmi_get_free_memory(iptr);
char *cfg_skip_blanks(char *);
char *cfg_skip_token(char *);
void player_refresh_paperdoll(void);
void shutdown_free_all(void);
void shutdown_video(void);
void palette_restore(void);
void game_shutdown(void);
void newgame_init_player(void);
void character_reset_magicka(struct character *, struct career *);
void init_player_records(struct record *);
void spell_cast_anims_free(void);
#pragma aux func_0009DA1C parm routine [];
#pragma aux mc_set_location parm routine [];
#pragma aux func_000A18C3 parm routine [];

void debug_show_mem_used(void)
{
    struct bf8_0_5 *ticks_addr;

    ticks_addr = (struct bf8_0_5 *)1132;
    if (ticks_addr->f == 0) {
        dpmi_get_free_memory((iptr)D_001A3F60);
        D_001A3F9C = D_001A3F90 - (D_001A3F7C << 2);
    }
    mc_set_location(47, D_00175040);
    mc_sprintf((char *)text_buffer, D_00175047, D_001A3F9C);
    D_0012B508 = 146;
    text_draw(text_buffer, 246, 23);
}

void player_refresh_paperdoll(void)
{
    D_001940D8 |= 8;
    mem_check_crt_heap(0);
    xn_draw_fullscreen_overlay_shaded((char *)(window_image = disk_read_file(D_00175207, 0)));
    paperdoll_draw(0, 0);
    xn_gfx_clear(0);
    if (window_image == 0 || window_image == (-1751672937)) return;
    mc_free((void *)window_image, D_00175040, 246);
    window_image = -1751672937;
}

void init_world_objects(void)
{
    object_heap_init();
    D_00195AD0 = (iptr)location_object;
    init_player_records(location_object);
    inv_reset_left_list();
    calendar_update();
}

void newgame_place_player(void)
{
    int unused;
    int unused2;

    location_object->image = 65535;
    if (((int)(unsigned char)cfg_map_file) == 100) {
        player_environment = 3;
    } else {
        player_environment = 1;
    }
    current_region = cfg_region;
    current_region_data = &regions[(unsigned char)current_region];
    region_enter(0, (int)(unsigned char)current_region);
    dungeon_water_level = 10000;
    if (((int)player_environment) == 1) {
        map_goto_location((int)(unsigned char)cfg_region, (int)player_environment, cfg_start_map, 0);
    } else {
        map_goto_location((int)(unsigned char)cfg_region, 1, cfg_start_map, 0);
        player_to_nearest_marker(location_object, 8);
        dungeon_load(-1);
        automap_load();
        if (marker_count(location_object, 6) != 0) player_to_nearest_marker(location_object, 6);
    }
    xn_cam_x = player_object->x;
    xn_cam_z = player_object->z;
    xn_cam_y = player_object->y;
    view_cursor_active = 0;
    player_character->health = player_character->max_health;
}

void shutdown_free_all(void)
{
    int i;
    int unused;

    xn_joy_shutdown();
    xn_render_shutdown();
    archive_close(arch3d_bsa);
    archive_close(maps_bsa);
    archive_close(dagger_snd);
    archive_close(monster_bsa_handle);
    for (i = 0; ((int)(short)*(short *)&i) < 127; i++) {
        if (D_00190704[((int)(short)*(short *)&i)] != 0) {
            if (D_00190704[((int)(short)*(short *)&i)] != 0 && D_00190704[((int)(short)*(short *)&i)] != (-1751672937)) {
                mc_free((void *)D_00190704[((int)(short)*(short *)&i)], D_00175040, 314);
                D_00190704[((int)(short)*(short *)&i)] = -1751672937;
            }
        }
    }
    xn_mouse_set_sensitivity(D_001997FC, D_00199800);
    if (qbn_opcode_arg_counts != 0 && qbn_opcode_arg_counts != (-1751672937)) {
        mc_free((void *)qbn_opcode_arg_counts, D_00175040, 319);
        qbn_opcode_arg_counts = -1751672937;
    }
    region_free_tables();
    region_unload();
    model_heap_free_all();
    object_heap_shutdown();
    intrface_free();
    func_0007EF20();
    if ((iptr)hud_portrait != 0 && (iptr)hud_portrait != (-1751672937)) {
        mc_free((void *)hud_portrait, D_00175040, 326);
        *(iptr *)&hud_portrait = -1751672937;
    }
    if (D_001AA5FC != 0 && D_001AA5FC != (-1751672937)) {
        mc_free((void *)D_001AA5FC, D_00175040, 327);
        D_001AA5FC = -1751672937;
    }
    if ((iptr)spell_records != 0 && (iptr)spell_records != (-1751672937)) {
        mc_free(spell_records, D_00175040, 328);
        spell_records = (struct spell *)(iptr)-1751672937;
    }
    if ((iptr)buttons_rci != 0 && (iptr)buttons_rci != (-1751672937)) {
        mc_free(buttons_rci, D_00175040, 329);
        *(int *)&buttons_rci = -1751672937;
    }
    if (icon_image != 0 && icon_image != (-1751672937)) {
        mc_free((void *)icon_image, D_00175040, 330);
        icon_image = -1751672937;
    }
    if (hud_vital_bar_images != 0 && hud_vital_bar_images != (-1751672937)) {
        mc_free((void *)hud_vital_bar_images, D_00175040, 331);
        hud_vital_bar_images = -1751672937;
    }
    if (D_00195C80 != 0 && D_00195C80 != (-1751672937)) {
        mc_free((void *)D_00195C80, D_00175040, 332);
        D_00195C80 = -1751672937;
    }
    if (D_00195C84 != 0 && D_00195C84 != (-1751672937)) {
        mc_free((void *)D_00195C84, D_00175040, 333);
        D_00195C84 = -1751672937;
    }
    if (hud_compass_image != 0 && hud_compass_image != (-1751672937)) {
        mc_free((void *)hud_compass_image, D_00175040, 334);
        hud_compass_image = -1751672937;
    }
    if (D_00190908 != 0 && D_00190908 != (-1751672937)) {
        mc_free((void *)D_00190908, D_00175040, 335);
        D_00190908 = -1751672937;
    }
    if (D_0019090C != 0 && D_0019090C != (-1751672937)) {
        mc_free((void *)D_0019090C, D_00175040, 336);
        D_0019090C = -1751672937;
    }
    if (D_00190910 != 0 && D_00190910 != (-1751672937)) {
        mc_free((void *)D_00190910, D_00175040, 337);
        D_00190910 = -1751672937;
    }
    if (magic_def != 0 && magic_def != (-1751672937)) {
        mc_free((void *)magic_def, D_00175040, 338);
        magic_def = -1751672937;
    }
    if (D_00199804 != 0 && D_00199804 != (-1751672937)) {
        mc_free((void *)D_00199804, D_00175040, 339);
        D_00199804 = -1751672937;
    }
    if (D_001997F0 != 0 && D_001997F0 != (-1751672937)) {
        mc_free((void *)D_001997F0, D_00175040, 340);
        D_001997F0 = -1751672937;
    }
    if (D_00195D24 != 0 && D_00195D24 != (-1751672937)) {
        mc_free((void *)D_00195D24, D_00175040, 341);
        D_00195D24 = -1751672937;
    }
    if (D_00195CF8 != 0 && D_00195CF8 != (-1751672937)) {
        mc_free((void *)D_00195CF8, D_00175040, 342);
        D_00195CF8 = -1751672937;
    }
    if (D_00195B64 != 0 && D_00195B64 != (-1751672937)) {
        mc_free((void *)D_00195B64, D_00175040, 343);
        D_00195B64 = -1751672937;
    }
    if (paperdoll_mask != 0 && paperdoll_mask != (-1751672937)) {
        mc_free((void *)paperdoll_mask, D_00175040, 344);
        paperdoll_mask = -1751672937;
    }
    if (hud_portrait_overlays != 0 && hud_portrait_overlays != (-1751672937)) {
        mc_free((void *)hud_portrait_overlays, D_00175040, 345);
        hud_portrait_overlays = -1751672937;
    }
    if (horse_overlay_image != 0 && horse_overlay_image != (-1751672937)) {
        mc_free((void *)horse_overlay_image, D_00175040, 346);
        horse_overlay_image = -1751672937;
    }
    if (cart_overlay_image != 0 && cart_overlay_image != (-1751672937)) {
        mc_free((void *)cart_overlay_image, D_00175040, 347);
        cart_overlay_image = -1751672937;
    }
    if (compass_box_image != 0 && compass_box_image != (-1751672937)) {
        mc_free((void *)compass_box_image, D_00175040, 348);
        compass_box_image = -1751672937;
    }
    if (compass_image != 0 && compass_image != (-1751672937)) {
        mc_free((void *)compass_image, D_00175040, 349);
        compass_image = -1751672937;
    }
    if (hud_mode_icons != 0 && hud_mode_icons != (-1751672937)) {
        mc_free((void *)hud_mode_icons, D_00175040, 350);
        hud_mode_icons = -1751672937;
    }
    if ((iptr)hud_bar_image != 0 && (iptr)hud_bar_image != (-1751672937)) {
        mc_free(hud_bar_image, D_00175040, 351);
        hud_bar_image = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)list_popup_image != 0 && (iptr)list_popup_image != (-1751672937)) {
        mc_free(list_popup_image, D_00175040, 352);
        list_popup_image = (struct image *)(iptr)-1751672937;
    }
    if ((iptr)scratch_buffer != 0 && (iptr)scratch_buffer != (-1751672937)) {
        mc_free((void *)scratch_buffer, D_00175040, 353);
        *(iptr *)&scratch_buffer = -1751672937;
    }
    if (D_001997F4 != 0 && D_001997F4 != (-1751672937)) {
        mc_free((void *)D_001997F4, D_00175040, 354);
        D_001997F4 = -1751672937;
    }
    weapon_free_sprites();
    spell_cast_anims_free();
    msgbox_free_borders();
    for (i = 0; ((int)(short)*(short *)&i) < 5; i++) {
        if (quest_face_images[((int)(short)*(short *)&i)] != 0 && quest_face_images[((int)(short)*(short *)&i)] != (-1751672937)) {
            mc_free((void *)quest_face_images[((int)(short)*(short *)&i)], D_00175040, 360);
            quest_face_images[((int)(short)*(short *)&i)] = -1751672937;
        }
    }
    sky_free();
    faction_free();
    sound_shutdown_music();
    sound_cache_free_all();
    file_index_free();
}

void init_video(void)
{
    int unused;
    int unused2;

    xn_kbd_numlock_off();
    xn_timer_wait_ticks(18);
    xn_sys_set_dos_transfer_buffer();
    xn_mouse_get_sensitivity((char *)&D_001997FC, (char *)&D_00199800);
    xn_mem_init(102400);
    *(iptr *)&scratch_buffer = (iptr)mc_malloc(80780, D_00175040, 382);
    file_index_build();
    xn_gfx_set_mode(19, 1);
    xn_kbd_install();
    xn_mouse_init();
    xn_mouse_set_range_320x200();
    xn_font_init();
    xn_render_init();
    xn_cam_set_view_window(160, 77, 160, 77);
    xn_cam_set_focal(200, 180);
    xn_light_init();
    xn_anim_rand_seed();
    xn_light_ambient = 2048;
    xn_cam_near_z = 2560;
    xn_cam_far_z = 393216;
    xn_render_set_mode(8);
    xn_shade_load(0, 0);
    xn_shade_keep_colours_0_255();
    player_underwater = 0;
    disk_read_file(D_00175214, (D_001997F4 = (iptr)mc_malloc(768, D_00175040, 410)));
    xn_world_init();
    xn_world_open(D_0017521C);
}

void shutdown_video(void)
{
    xn_gfx_restore_mode();
    xn_kbd_remove();
    xn_sys_restore_crit_error_handler();
}

void init_palette(void)
{
    palette_restore();
}

void palette_restore(void)
{
    xn_pal_set((char *)D_001997F4);
    D_001940D7 |= 1;
}

void game_exit(char *message)
{
    mc_set_location(440, D_00175040);
    func_000A148C(D_00175226);
    engine_running = 0;
    mem_check_level = 0;
    exiting = 1;
    game_shutdown();
    if (message == 0) {
        func_0009DA1C(449, D_00175040);
        printf(D_0017523C);
    } else {
        func_0009DA1C(451, D_00175040);
        printf(message);
    }
    kludge_print_build(1);
    mc_set_location(455, D_00175040);
    mc_sprintf((char *)text_buffer, D_00175260, (iptr)arena2_path, (iptr)D_00175253);
    unlink((char *)text_buffer);
    exit(0);
}

void game_shutdown(void)
{
    text_rsc_close();
    shutdown_free_all();
    shutdown_video();
    xn_mem_shutdown();
}

void fatal_error(char *message)
{
    {
        char text[1024];

        mem_check_level = 0;
        if (exiting != 0) return;
        if (internal_check_failed != 0) {
            mc_set_location(480, D_00175040);
            mc_sprintf(text, D_00175266, internal_check_failed);
        } else {
            mc_strncpy(text, message, 1024, D_00175040, 482);
        }
        mc_set_location(484, D_00175040);
        func_000A148C(D_00175281, frame_checkpoint);
        mc_set_location(485, D_00175040);
        func_000A148C(D_001752B6, (iptr)text);
        mc_set_location(487, D_00175040);
        func_000A18C3(text);
        exiting = 1;
        engine_running = 0;
        mem_check_level = 0;
        game_shutdown();
        func_0009DA1C(495, D_00175040);
        printf(D_001752D2, frame_checkpoint);
        func_0009DA1C(496, D_00175040);
        printf(D_001752B6, text);
        func_0009DA1C(497, D_00175040);
        printf(D_00175307);
        mc_set_location(499, D_00175040);
        mc_sprintf(text, D_00175260, (iptr)arena2_path, (iptr)D_00175253);
        unlink(text);
        exit(10);
    }
}

void newgame_init_player(void)
{
    int level;
    struct character *character;
    struct career *career;

    player_refresh_paperdoll();
    player_compute_jump_velocity();
    mc_set_location(514, D_00175040);
    mc_sprintf((char *)text_buffer, D_00175313, (iptr)arena2_path);
    disk_copy_file(cfg_mapsave_file, text_buffer, arena2_path);
    character = player_character;
    career = player_class;
    character->fatigue = (character->attributes[ATTR_STR] + character->attributes[ATTR_END]) << 6;
    character->health = career->hp_per_level + 25;
    for (level = 1; character->level > level; level++) {
        character->health += rand_range(1, career->hp_per_level);
    }
    character->max_health_base = (int)(short)(character->max_health = character->health);
    character_reset_magicka(character, career);
    character->attack_damage[0][0] = 1;
    character->attack_damage[0][1] = 2;
    weapon_reload_hand_sprites();
}

void character_reset_magicka(struct character *character, struct career *career)
{
    character->magicka = (character->max_magicka = (character->attributes[ATTR_INT] * ((int)(short)D_001788D3[((career->flags >> 10) & 7)])) / 256);
}

iptr flats_cfg_find(int image)
{
    int i;

    for (i = 0; i < flats_cfg_count; i++) {
        if (flats_cfg[i].image == image) {
            return (iptr)&flats_cfg[i];
        }
    }
    return 0;
}

void cfg_read_line(char **cursor, char *line)
{
    *cursor = cfg_skip_blanks(*cursor);
    while (**cursor != 13) {
        *line++ = **cursor;
        (*cursor)++;
        (*(int *)disk_last_file_size)--;
    }
    *line++ = 0;
}

int cfg_read_number(char **cursor)
{
    int number;

    *cursor = cfg_skip_blanks(*cursor);
    if (*cursor == 0) return 100000;
    number = atoi(*cursor);
    *cursor = cfg_skip_token(*cursor);
    return number;
}

char *cfg_skip_blanks(char *text)
{
    while (*text <= 32) {
        text++;
        (*(int *)disk_last_file_size)--;
        if (*(int *)disk_last_file_size < 1) return 0;
    }
    return text;
}

char *cfg_skip_token(char *text)
{
    while (*text > 32) {
        text++;
        (*(int *)disk_last_file_size)--;
    }
    return text;
}

void init_player_records(struct record *root)
{
    int unused;
    iptr logbook;

    (player_object = object_create_child(root, 0, 0))->type = 4;
    player_object->flags = 3;
    player_object->x = 0;
    player_object->y = 0;
    player_object->z = 0;
    (camera_object = object_create_child(player_object, 0, 0))->type = 5;
    camera_object->flags = 2;
    camera_object->x = player_object->x;
    camera_object->y = player_object->y - 75;
    camera_object->z = player_object->y;
    (player_entity = object_create_child(player_object, 0, 634))->type = 3;
    player_entity->flags = 3;
    player_class = &(player_character = &player_entity->data.character)->career;
    player_character->pad83 = 1;
    player_character->reflexes = 2;
    player_character->mobile_id = 200;
    (inventory_containers[0] = object_create_child(player_entity, 0, 0))->type = 52;
    inventory_containers[0]->flags = 3;
    inventory_containers[0]->container_index = 0;
    *(signed char *)((char *)(*(iptr *)&D_001959DC = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    D_001959DC->flags = 3;
    D_001959DC->container_index = 1;
    *(signed char *)((char *)(*(iptr *)&D_001959E0 = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    D_001959E0->flags = 3;
    D_001959E0->container_index = 2;
    *(signed char *)((char *)(*(iptr *)&D_001959E4 = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    D_001959E4->flags = 3;
    D_001959E4->container_index = 3;
    *(signed char *)((char *)(*(iptr *)&house_container = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    house_container->flags = 3;
    house_container->container_index = 5;
    *(signed char *)((char *)(*(iptr *)&ship_container = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    ship_container->flags = 3;
    ship_container->container_index = 6;
    *(signed char *)((char *)(*(iptr *)&room_storage_container = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    room_storage_container->flags = 3;
    room_storage_container->container_index = 7;
    *(signed char *)((char *)(*(iptr *)&repair_container = (iptr)object_create_child(player_entity, 0, 0))) = 52;
    repair_container->flags = 3;
    repair_container->container_index = 8;
    *(signed char *)((char *)(*(iptr *)&quest_root = (iptr)object_create_child(player_entity, 0, 0))) = 16;
    quest_root->flags = 3;
    *(signed char *)((char *)(*(iptr *)&options_object = (iptr)object_create_child(player_entity, 0, 6))) = 23;
    options_object->flags = 3;
    *(short *)((char *)(*(iptr *)&game_settings = (iptr)options_object + 71)) = 32514;
    game_settings->sound_volume = 127;
    game_settings->music_volume = 128;
    *(signed char *)((char *)(*(iptr *)&logbook_object = (iptr)object_create_child(player_entity, 0, 3008))) = 24;
    logbook_object->flags = 3;
    logbook = (iptr)logbook_object + 71;
    *(signed char *)((char *)(*(iptr *)&bank_accounts = (iptr)object_create_child(player_entity, 0, 806))) = 25;
    bank_accounts->flags = 3;
}

void update_underwater(void)
{
    int underwater;

    if (dungeon_water_level == 10000) return;
    if ((player_object->y - 76) > dungeon_water_level) {
        underwater = 1;
    } else {
        underwater = 0;
    }
    if (((int)(unsigned char)player_underwater) == underwater) return;
    player_underwater = *(signed char *)&underwater;
    xn_shade_load(underwater, underwater);
    xn_shade_keep_colours_0_255();
}

void spell_cast_anims_load(void)
{
    spell_cast_anims_free();
    spell_cast_anim_fire[0] = disk_read_file(D_00175324, 0);
    spell_cast_anim_frost = disk_read_file(D_00175331, 0);
    spell_cast_anim_magic = disk_read_file(D_0017533E, 0);
    spell_cast_anim_poison = disk_read_file(D_0017534B, 0);
    spell_cast_anim_shock = disk_read_file(D_00175358, 0);
}

void spell_cast_anims_free(void)
{
    if (spell_cast_anim_fire[0] != 0 && spell_cast_anim_fire[0] != (-1751672937)) {
        mc_free((void *)spell_cast_anim_fire[0], D_00175040, 754);
        spell_cast_anim_fire[0] = -1751672937;
    }
    if (spell_cast_anim_frost != 0 && spell_cast_anim_frost != (-1751672937)) {
        mc_free((void *)spell_cast_anim_frost, D_00175040, 755);
        spell_cast_anim_frost = -1751672937;
    }
    if (spell_cast_anim_magic != 0 && spell_cast_anim_magic != (-1751672937)) {
        mc_free((void *)spell_cast_anim_magic, D_00175040, 756);
        spell_cast_anim_magic = -1751672937;
    }
    if (spell_cast_anim_poison != 0 && spell_cast_anim_poison != (-1751672937)) {
        mc_free((void *)spell_cast_anim_poison, D_00175040, 757);
        spell_cast_anim_poison = -1751672937;
    }
    if (spell_cast_anim_shock == 0 || spell_cast_anim_shock == (-1751672937)) {
        return;
    }
    mc_free((void *)spell_cast_anim_shock, D_00175040, 758);
    spell_cast_anim_shock = -1751672937;
}

void update_fog(void)
{
    int is_day;
    int climate;
    int minute_of_day;

    minute_of_day = ((unsigned)game_minutes) % 1440;
    if (minute_of_day > 360 && minute_of_day < 1080) {
        is_day = 1;
    } else {
        is_day = 0;
    }
    daylight = is_day;
    if (((int)player_environment) == 1) {
        if (daylight == 0) {
            xn_cam_far_z = ((game_settings->view_flags >> 8) + 129) * 1536;
            xn_fog_table_last = D_00195D20;
            xn_shade_set_fog((xn_cam_far_z >> 8) - 512);
            fog_colour = 0;
            return;
        }
        climate = climate_category();
        if (((int)(unsigned char)(climate_weathers[climate] & 127)) == 3 || ((int)(unsigned char)(climate_weathers[climate] & 128)) != 0) {
            xn_cam_far_z = ((game_settings->view_flags >> 8) + 129) * 768;
            xn_fog_table_last = D_00195D18 + 16128;
            xn_shade_set_fog(8);
            fog_colour = 119;
        } else {
            xn_cam_far_z = ((game_settings->view_flags >> 8) + 129) * 3072;
            xn_fog_table_last = D_00195CF4 + 16128;
            xn_shade_set_fog((xn_cam_far_z >> 8) - 512);
            fog_colour = 0;
        }
        return;
    }
    if (dungeon_water_level != 10000 && (player_object->y - 76) > dungeon_water_level) {
        xn_cam_far_z = ((game_settings->view_flags >> 8) + 129) * 768;
        xn_fog_table_last = D_00195CF4 + 16128;
        xn_shade_set_fog(4);
        fog_colour = 107;
        return;
    }
    xn_cam_far_z = ((game_settings->view_flags >> 8) + 129) * 1536;
    xn_fog_table_last = D_00195D20;
    xn_shade_set_fog((xn_cam_far_z >> 8) - 512);
    fog_colour = 223;
}

void game_reset(void)
{
    int region;

    if (location_object->image != 65535) location_unload(location_object->image);
    object_free_children(nonworld_root);
    frame_checkpoint = 500;
    object_delete(player_object);
    init_player_records(location_object);
    frame_checkpoint = 501;
    newgame_init_player();
    frame_checkpoint = 502;
    inv_reset_left_list();
    frame_checkpoint = 503;
    D_001940D8 |= 8;
    last_skill_check_minutes = (game_minutes = 519210);
    frame_checkpoint = 504;
    faction_load_file();
    frame_checkpoint = 505;
    for (region = 0; region < 62; region++) {
        regions[region].price_adjustment = rand_range(0, 500) + 750;
    }
    mc_memset(saved_positions, 0, 48, D_00175040, 851, 48);
    mc_memset(((char *)creature_list), 0, 512, D_00175040, 852, 512);
    creature_count = 0;
}

void books_find_path(void)
{
    int handle;

    mc_set_location(860, D_00175040);
    mc_sprintf((char *)text_buffer, D_00175365, (iptr)arena2_path);
    handle = open((char *)text_buffer, 512);
    if (handle < 0) {
        books_path = (iptr)arena2_cd_path;
        return;
    }
    books_path = (iptr)arena2_path;
    close(handle);
}

void init_link_caster(void)
{
    D_0019621B = 18;
    D_0019625E = (iptr)location_object;
}
