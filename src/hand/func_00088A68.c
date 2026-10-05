/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088A68 */
struct rec131 {
    short f0;
    unsigned char f2[129];
};
extern char D_000C2BB8[];
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
extern void func_000A1023(char *, unsigned char *, int, char *, int, int);

void climate_set_textures(void)
{
    int l_1C;
    int l_18;

    if (region_flats[climate_index].f2[0] > 50)
        region_flats[climate_index].f2[0] = 50;
    func_000A1023(D_000C2BB8, region_flats[climate_index].f2, 129, D_00176C94, 1455, 4);
    ground_texture_archive = climate_texture_sets[climate_index] * 100 + 2;
    nature_texture_archive = region_flats[climate_index].f0;
    l_1C = climate_category();
    l_18 = month_seasons[game_minutes % 518400 / 43200];
    if ((climate_weathers[l_1C] & 127) == 5 || l_18 == 0 && (l_1C == 1 || l_1C == 3 || l_1C == 5)) {
        ground_texture_archive++;
        nature_texture_archive++;
        return;
    }
    if ((climate_weathers[climate_category()] & 127) != 4) return;
    ground_texture_archive += 2;
}
