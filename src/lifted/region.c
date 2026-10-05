/* region.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_000C2893[];
extern char D_000C28C4[];
extern char D_001705F8[];
extern char climate_categories[];
extern char D_00187F30[];
extern char region_event_values[];
extern char player_object[];
extern char current_region_data[];
extern char current_region[];
extern char D_00196269[];
extern char current_climate[];
extern char D_00196285[];
extern char politic_pak[];
extern char climate_pak[];

extern int mc_free();
extern int func_000C2D81();
extern void func_0001DF7F(int);
extern void region_unload(void);
unsigned char politic_region_at(int, int);
unsigned char climate_lookup(int, int);
unsigned char pak_lookup(int, int, int);
void region_enter(unsigned char, unsigned char);

void region_free_tables(void)
{
    if (*(int *)politic_pak == 0) goto L1FEE0;
    if (*(int *)politic_pak != (-1751672937)) goto L1FEE2;
L1FEE0:;
    goto L1FF00;
L1FEE2:;
    mc_free(*(int *)politic_pak, (int)D_001705F8, 33);
    *(int *)politic_pak = -1751672937;
L1FF00:;
    if (*(int *)climate_pak == 0) goto L1FF15;
    if (*(int *)climate_pak != (-1751672937)) goto L1FF17;
L1FF15:;
    return;
L1FF17:;
    mc_free(*(int *)climate_pak, (int)D_001705F8, 34);
    *(int *)climate_pak = -1751672937;
}

void region_enter(unsigned char a1, unsigned char a2)
{
    *(signed char *)current_region = a2;
    *(int *)current_region_data = ((int)region_event_values) + (((int)(unsigned char)*(signed char *)current_region) * 80);
    *(signed char *)D_00196269 = *(signed char *)current_region;
    region_unload();
    func_0001DF7F((int)(unsigned char)a2);
}

int region_update_from_player(void)
{
    unsigned char l_18;

    l_18 = politic_region_at(*(int *)(*(char **)player_object + 7), *(int *)(*(char **)player_object + 15));
    if ((signed char)l_18 == *(signed char *)current_region) goto L1FFDD;
    region_enter((int)(unsigned char)*(signed char *)current_region, (int)(unsigned char)l_18);
    return 1;
L1FFDD:;
    return 0;
}

int climate_category(void)
{
    return (int)(unsigned char)*(signed char *)(climate_categories + ((int)(unsigned char)*(signed char *)current_climate));
}

int climate_update_at_player(void)
{
    return (int)(unsigned char)climate_lookup(*(int *)(*(char **)player_object + 7), *(int *)(*(char **)player_object + 15));
}

unsigned char politic_region_at(int a1, int a2)
{
    int l_20;
    int l_1C;
    unsigned char l_18;

    l_20 = (a1 >> 15) + 2;
    l_1C = 499 - (a2 >> 15);
    if (l_1C >= 1) goto L200C7;
    l_1C = 1;
    goto L200D7;
L200C7:;
    if (l_1C <= 499) goto L200D7;
    l_1C = 499;
L200D7:;
    l_18 = pak_lookup(l_20, l_1C, *(int *)politic_pak);
    if (((int)(unsigned char)l_18) != 64) goto L200FB;
    return 31;
L200FB:;
    return l_18 & 127;
}

unsigned char climate_lookup(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = (a1 >> 15) + 2;
    l_18 = 499 - (a2 >> 15);
    if (l_18 >= 1) goto L2014D;
    l_18 = 1;
    goto L2015D;
L2014D:;
    if (l_18 <= 499) goto L2015D;
    l_18 = 499;
L2015D:;
    *(signed char *)current_climate = pak_lookup(l_1C, l_18, *(int *)climate_pak);
    if (((int)(unsigned char)*(signed char *)current_climate) != 223) goto L20195;
    *(signed char *)current_climate = 228;
    *(signed char *)D_00196285 = 1;
    return 3;
L20195:;
    *(signed char *)D_00196285 = 0;
    return *(signed char *)(climate_categories + ((int)(unsigned char)*(signed char *)current_climate));
}

unsigned char pak_lookup(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = a3;
    l_14 = (int)(*(char **)((char *)((a2 << 2) + l_18)) + a3);
    a1 -= (int)(short)*(short *)((char *)l_14);
L201EF:;
    if (a1 <= 0) goto L20207;
    (*(char (**)[3])&l_14)++;
    a1 -= (int)(short)*(short *)((char *)l_14);
    goto L201EF;
L20207:;
    return *(signed char *)((char *)l_14 + 2);
}

unsigned char func_0002021B(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = func_000C2D81(a1, a2);
    l_18 = 0;
L20243:;
    if (l_18 < 4) goto L20253;
    goto L20264;
L2024B:;
    l_18++;
    goto L20243;
L20253:;
    if (l_20 != *(int *)(D_000C2893 + (l_18 << 2))) goto L2024B;
L20264:;
    if (l_18 != 4) goto L20270;
    return 255;
L20270:;
    l_24 = (int)(*(char **)D_000C28C4 + *(int *)(D_00187F30 + (l_18 << 2)));
    l_24 += (127 - ((a2 & 32767) >> 8)) << 8;
    l_24 += (a1 & 32767) >> 8;
    return *(signed char *)((char *)l_24) & 63;
}
