/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088A68 */
struct rec131 {
    short f0;
    unsigned char f2[129];
};
extern char xn_world_nature_flat_odds[];
extern char D_00176C94[];
extern unsigned char month_seasons[];
extern unsigned char climate_texture_sets[];
extern unsigned int game_minutes;
extern unsigned char climate_weathers[];
extern int climate_index;
extern int ground_texture_archive;
extern int nature_texture_archive;
extern struct rec131 region_flats[];
extern int climate_category(void);
extern void mc_memcpy(char *, unsigned char *, int, char *, int, int);

void climate_set_textures(void)
{
    int climate;
    int season;

    if (region_flats[climate_index].f2[0] > 50)
        region_flats[climate_index].f2[0] = 50;
    mc_memcpy(xn_world_nature_flat_odds, region_flats[climate_index].f2, 129, D_00176C94, 1455, 4);
    ground_texture_archive = climate_texture_sets[climate_index] * 100 + 2;
    nature_texture_archive = region_flats[climate_index].f0;
    climate = climate_category();
    season = month_seasons[game_minutes % 518400 / 43200];
    if ((climate_weathers[climate] & 127) == 5 || season == 0 && (climate == 1 || climate == 3 || climate == 5)) {
        ground_texture_archive++;
        nature_texture_archive++;
        return;
    }
    if ((climate_weathers[climate_category()] & 127) != 4) return;
    ground_texture_archive += 2;
}
