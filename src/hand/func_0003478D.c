/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003478D */
#include "records.h"

struct s12 { int a[3]; };
extern char D_000346B8[];
extern int D_000C23B8;
extern int D_000C23BC;
extern int D_000C23C0;
extern short D_000CEA30;
extern short D_000CEA34;
extern int dungeon_water_level;
extern int D_00136911;
extern char D_00136E00[];
extern char D_00136E24[];
extern int screen_buffer;
extern char D_00170A86[];
extern unsigned char player_environment;
extern signed char D_00187CA8;
extern signed char region_precipitation_override[];
extern int view_look_pitch;
extern int D_001959BC;
extern struct record *camera_object;
extern struct record *player_object;
extern int D_00195AB0;
extern char *hud_bar_image;
extern int game_minutes;
extern struct settings *game_settings;
extern int D_00195CF4;
extern signed char climate_weathers[];
extern signed char current_region;
extern signed char D_001962A1;
extern int moon0_image;
extern int moon1_image;
extern int D_0019857C;
extern char D_001985A8[];
extern char D_001985B4[];
extern int D_001985C8;
extern int D_00199808;
extern int climate_category(void);
extern void func_00035129(int, int, int);
extern void sky_load_day(int);
extern void sky_draw_day(int, int, int, int);
extern void func_000359B4(int);
extern void sky_draw_night(int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *location_cell_at(int, int);
extern int mc_memset();
extern int mc_memcpy();
extern int func_00137000();
extern int func_00137486();
extern int func_001376C8();
extern int func_00137725();

void sky_update(void)
{
    int l_78;
    int l_74;
    int l_70;
    int l_6C;
    int l_68;
    int l_64;
    int l_60;
    int l_5C;
    int l_58;
    int l_54;
    int l_50;
    int l_4C;
    int l_48;
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    unsigned char l_1C;
    struct record *l_24;
    unsigned char l_20;
    int l_28;
    unsigned char l_18;

    *(struct s12 *)&l_74 = *(struct s12 *)D_000346B8;
    l_18 = 0;
    if (D_00187CA8 == 0) return;
    if (((int)player_environment) == 3) {
        l_24 = location_cell_at(player_object->x, player_object->z);
        if (player_object->parent != l_24) object_reparent(l_24, player_object);
        l_3C = l_24->light_level << 8;
        l_38 = D_00136911;
        if ((l_3C >> 8) != (l_38 >> 8)) {
            l_38 = ((l_3C - l_38) * D_00195AB0) / 1024;
            D_00136911 += l_38;
        } else {
            D_00136911 = l_24->light_level << 8;
        }
        dungeon_water_level = l_24->water_level;
        D_001962A1 = l_24->block_special;
        return;
    }
    if (((int)player_environment) == 1) func_000359B4(game_minutes);
    l_58 = ((unsigned)game_minutes) % 1440;
    if (l_58 > 360 && l_58 < 1080) {
        l_78 = 1;
    } else {
        l_78 = 0;
    }
    D_00199808 = l_78;
    if (((int)player_environment) == 2) {
        if (D_00199808 != 0) {
            l_58 += -360;
            if (l_58 < 45) {
                l_50 = (l_58 * 63) / 45;
            } else if (l_58 > 675) {
                l_50 = 63 - (((l_58 - 675) * 63) / 45);
            } else {
                l_50 = 63;
            }
        } else {
            l_50 = 24;
        }
        l_50 >>= 1;
        if ((D_00136911 = l_50 << 8) < 2560) D_00136911 = 2560;
        return;
    }
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        l_70 = 450;
    } else {
        l_70 = 140;
    }
    D_000C23B8 = (camera_object->angle_x + view_look_pitch) & 2047;
    D_000C23BC = 0;
    D_000C23C0 = 0;
    func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
    func_00137725((int)D_00136E00, (int)D_00136E24);
    func_00137486((int)&l_74, (int)&l_70, (int)&l_6C, (int)D_00136E00);
    func_001376C8(l_74, l_70, 3000, (int)&l_54, (int)&l_5C);
    l_44 = l_5C + 75;
    if (l_44 < 0) l_44 = 0;
    if (l_44 > 199) l_44 = 199;
    if (D_00199808 == 0) D_00136911 = 4096;
    l_4C = l_5C;
    D_0019857C = 0;
    l_40 = climate_category();
    l_1C = climate_weathers[l_40];
    if (region_precipitation_override[((int)(unsigned char)current_region) * 80] != 0) {
        l_1C = region_precipitation_override[((int)(unsigned char)current_region) * 80] - 1;
    }
    l_58 += -360;
    if (l_58 < 45) {
        l_50 = (l_58 * 63) / 45;
    } else if (l_58 > 675) {
        l_50 = 63 - (((l_58 - 675) * 63) / 45);
    } else {
        l_50 = 63;
    }
    if (D_00199808 == 0) l_50 = 8;
    D_000C23B8 = (camera_object->angle_x + view_look_pitch) & 2047;
    D_000C23BC = (camera_object->yaw + D_001959BC) & 2047;
    D_000C23C0 = 0;
    func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
    func_00137725((int)D_00136E00, (int)D_00136E24);
    l_18 = 1;
    func_00035129((int)&D_001985C8, 0, l_58);
    D_0019857C = l_50;
    if (((int)(unsigned char)(climate_weathers[l_40] & 127)) == 5) l_50 <<= 1;
    if ((D_00136911 = (l_50 >> 1) << 8) < 2048) D_00136911 = 2048;
    if (D_00199808 != 0) {
        if (l_5C > (-75)) {
            sky_load_day(game_minutes);
            sky_draw_day(l_5C, l_4C, D_00199808, l_40);
            return;
        }
        mc_memset(screen_buffer, (int)(unsigned char)*(signed char *)(((char *)D_00195CF4)), ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(hud_bar_image + 2)) * 320, (int)D_00170A86, 251, 4);
    } else {
        sky_draw_night(l_5C, l_4C);
    }
    if (l_44 == 0) return;
    if (D_00199808 == 0 && ((int)(unsigned char)l_1C) > 1) {
        mc_memset(screen_buffer, 223, 64000, (int)D_00170A86, 262, 4);
        D_0019857C = 0;
        return;
    }
    l_20 = 0;
    mc_memcpy((int)&l_68, (int)D_001985B4, 12, (int)D_00170A86, 271, 4);
    func_00137486((int)&l_68, (int)&l_64, (int)&l_60, (int)D_00136E00);
    if (l_60 > 100) {
        func_001376C8(l_68, l_64, l_60, (int)&l_54, (int)&l_5C);
        *(short *)(*(char **)&moon0_image + 6) = (l_54 + D_000CEA30) - (**(unsigned short **)&moon0_image >> 1);
        *(short *)(*(char **)&moon0_image + 8) = (((int)(short)D_000CEA34) + l_5C) - (((int)(unsigned short)*(short *)(*(char **)&moon0_image + 2)) >> 1);
        l_20 |= 1;
    }
    mc_memcpy((int)&l_68, (int)D_001985A8, 12, (int)D_00170A86, 281, 4);
    func_00137486((int)&l_68, (int)&l_64, (int)&l_60, (int)D_00136E00);
    if (l_60 > 100) {
        func_001376C8(l_68, l_64, l_60, (int)&l_54, (int)&l_5C);
        *(short *)(*(char **)&moon1_image + 6) = (l_54 + D_000CEA30) - (**(unsigned short **)&moon1_image >> 1);
        *(short *)(*(char **)&moon1_image + 8) = (((int)(short)D_000CEA34) + l_5C) - (((int)(unsigned short)*(short *)(*(char **)&moon1_image + 2)) >> 1);
        l_20 |= 2;
    }
    D_000C23B8 = (camera_object->angle_x + view_look_pitch) & 2047;
    D_000C23BC = (((((unsigned)((((unsigned)game_minutes) % 518400) * 2047)) / 518400) + (((unsigned)((((unsigned)game_minutes) % 1440) * 2047)) / 1440)) + (camera_object->yaw + D_001959BC)) & 2047;
    D_000C23C0 = 0;
    func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
    func_00137725((int)D_00136E00, (int)D_00136E24);
    if (D_00199808 != 0) if (l_50 == 63) {}
    if (D_00199808 != 0) return;
    D_0019857C = 0;
}
