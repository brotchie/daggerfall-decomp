/* qinit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00170A64[];
extern char D_00178A10[];
extern char D_00185F88[];
extern char region_price_adjustment[];
extern char D_001940D5[];
extern char D_00195984[];
extern char nonworld_root[];
extern char D_00195A00[];
extern char camera_object[];
extern char player_object[];
extern char D_00195AC4[];
extern char player_character[];
extern char game_minutes[];
extern char D_00195D00[];
extern char qbn_opcode_arg_counts[];
extern char current_region[];
extern char D_00196299[];
extern char D_001962A3[];
extern char faction_count[];
extern char factions[];
extern char D_001970DC[];
extern char current_quest[];
extern char D_00199780[];
extern char qbn_record_sizes[];
extern char guild_membership[];

extern int faction_find(short);
extern int faction_player_related(int);
extern int quest_section(int, int);
extern int quest_record(int, int, int);
extern int func_000310E1(int, int);
extern int quest_init_person(int);
extern int quest_init_place(int);
extern int spawn_find_point(int, int, int);
extern int rand_range(int, int);
extern int object_free_single(int);
extern int object_delete(int);
extern int object_create_child(int, int, int);
extern int object_reparent(int, int);
extern int object_find_by_id(int, int);
extern int object_new_id(int);
extern int rand();
extern int mc_strncpy();
extern int mc_memcpy();
extern void item_make_random(unsigned short, int);
extern void item_make(int, int, int);
extern void monster_init(int, int);
extern void monster_init_gear(int);
extern void map_goto_location(int, int, int, int);
int quest_init_item(int);
int quest_init_foe(int);
int quest_record_object(int, int);
int quest_place_object(int, int);

int quest_init_item(int a1)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_34 = *(int *)D_00195AC4 + 71;
    l_1C = 0;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 2) & 2)) == 0) goto L330CE;
    if (((int)(short)*(short *)((char *)a1 + 5)) != (-1)) goto L3301E;
    if (*(short *)(*(char **)current_quest + 2) == 0) goto L32F74;
    l_1C = faction_find((int)(short)*(short *)(*(char **)current_quest + 2));
    if (l_1C == 0) goto L32F3F;
    if (faction_player_related(l_1C) != 0) goto L32F41;
L32F3F:;
    goto L32F53;
L32F41:;
    l_24 = ((int)(unsigned char)*(signed char *)(*(char **)guild_membership)) + 1;
    goto L32F72;
L32F53:;
    l_24 = (((int)(unsigned char)*(signed char *)(*(char **)player_character + 129)) / 2) + 1;
L32F72:;
    goto L32F93;
L32F74:;
    l_24 = (((int)(unsigned char)*(signed char *)(*(char **)player_character + 129)) / 2) + 1;
L32F93:;
    if (l_24 <= 10) goto L32FA0;
    l_24 = 10;
L32FA0:;
    if (l_1C == 0) goto L32FB2;
    l_20 = (int)(short)*(short *)((char *)l_1C + 31);
    goto L32FB9;
L32FB2:;
    l_20 = 50;
L32FB9:;
    l_28 = ((l_20 + 50) * ((((int)&*(signed char *)((char *)(((int)(unsigned short)*(short *)(region_price_adjustment + (((int)(unsigned char)*(signed char *)current_region) * 80))) / 2) + 500)) * rand_range(l_24 * 150, l_24 * 200)) / 1000)) / 100;
    goto L33034;
L3301E:;
    l_28 = rand_range((int)(short)*(short *)((char *)a1 + 5), (int)(short)*(short *)((char *)a1 + 3));
L33034:;
    if (l_28 >= 1) goto L33041;
    l_28 = 1;
L33041:;
    l_30 = object_create_child(*(int *)nonworld_root, 0, 107);
    *(signed char *)((char *)l_30) = 2;
    *(short *)((char *)l_30 + 29) = *(short *)D_00178A10;
    *(short *)((char *)l_30 + 21) = 0;
    *(signed char *)((char *)l_30 + 38) = *(signed char *)(*(char **)current_quest);
    *(int *)((char *)l_30 + 31) = object_new_id(700);
    *(int *)((char *)a1 + 11) = l_30;
    l_2C = l_30 + 71;
    item_make(28, 0, l_2C);
    *(int *)((char *)l_2C + 36) = l_28;
    *(short *)((char *)l_30 + 27) = *(short *)((char *)l_2C + 52);
    goto L33229;
L330CE:;
    if (*(short *)((char *)a1 + 3) >= 0) goto L33107;
L330D8:;
    *(short *)((char *)a1 + 3) = rand() % 28;
    if (*(int *)(D_00185F88 + (((int)(short)*(short *)((char *)a1 + 3)) << 2)) == 0) goto L330D8;
L33107:;
    l_30 = object_create_child(*(int *)nonworld_root, 0, 107);
    *(signed char *)((char *)l_30) = 2;
    *(short *)((char *)l_30 + 29) = *(short *)D_00178A10;
    *(short *)((char *)l_30 + 21) = 0;
    *(signed char *)((char *)l_30 + 38) = *(signed char *)(*(char **)current_quest);
    *(int *)((char *)l_30 + 31) = object_new_id(700);
    *(int *)((char *)a1 + 11) = l_30;
    l_2C = l_30 + 71;
    *(short *)((char *)l_2C + 63) = 0;
    if (*(short *)((char *)a1 + 5) < 0) goto L33199;
    item_make((int)(unsigned short)*(short *)((char *)a1 + 3), (int)(short)*(short *)((char *)a1 + 5), l_2C);
    goto L331BB;
L33199:;
    item_make_random((int)(unsigned short)*(short *)((char *)a1 + 3), l_2C);
    *(short *)((char *)a1 + 5) = *(short *)((char *)l_2C + 34);
L331BB:;
    *(short *)((char *)l_30 + 27) = *(short *)((char *)l_2C + 52);
    if (((int)(unsigned short)*(short *)((char *)l_2C + 32)) != 9) goto L331EB;
    if (((int)(unsigned short)*(short *)((char *)l_2C + 34)) == 5) goto L331ED;
L331EB:;
    goto L331F7;
L331ED:;
    if (*(short *)((char *)a1 + 17) != 0) goto L331F9;
L331F7:;
    goto L33229;
L331F9:;
    *(short *)((char *)l_2C + 63) = *(short *)((char *)a1 + 17);
    mc_strncpy(l_2C + 10, (int)&*(signed char *)(*(char **)current_quest + 6), 4, (int)D_00170A64, 572);
L33229:;
    return l_30;
}

int quest_init_foe(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = object_create_child(*(int *)nonworld_root, 0, 659);
    l_20 = *(int *)D_00195AC4 + 71;
    *(signed char *)((char *)l_24) = 18;
    *(signed char *)((char *)l_24 + 21) |= 1;
    *(int *)((char *)l_24 + 43) = rand();
    *(int *)((char *)l_24 + 31) = object_new_id(700);
    *(int *)((char *)a1 + 10) = l_24;
    *(short *)((char *)l_24 + 29) = (*(int *)D_00178A10)++;
    *(signed char *)((char *)l_24 + 38) = *(signed char *)(*(char **)current_quest);
    monster_init(l_24, (int)(unsigned char)*(signed char *)((char *)a1 + 3));
    l_1C = l_24 + 71;
    *(signed char *)((char *)l_1C + 553) = 1;
    return l_24;
}

int quest_init_resources(int a1)
{
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
    int l_28;
    short l_1C;
    int l_24;
    short l_18;

    l_2C = 0;
    *(int *)current_quest = a1;
    l_4C = a1 + ((int)(short)*(short *)((char *)a1 + 42));
    *(signed char *)D_001970DC = 0;
    l_30 = 0;
L33334:;
    if (((int)(short)*(short *)((char *)a1 + 22)) > l_30) goto L33351;
    goto L33375;
L33342:;
    l_30++;
    (*(char (**)[20])&l_4C)++;
    goto L33334;
L33351:;
    *(int *)((char *)l_4C + 12) = 0;
    if (quest_init_person(l_4C) != 0) goto L33373;
    return 0;
L33373:;
    goto L33342;
L33375:;
    if (*(signed char *)D_001970DC == 0) goto L333FB;
    l_4C = a1 + ((int)(short)*(short *)((char *)a1 + 42));
    *(signed char *)D_001970DC = 0;
    l_30 = 0;
L3339F:;
    if (((int)(short)*(short *)((char *)a1 + 22)) > l_30) goto L333BC;
    goto L333DF;
L333AD:;
    l_30++;
    (*(char (**)[20])&l_4C)++;
    goto L3339F;
L333BC:;
    if (*(int *)((char *)l_4C + 12) != 0) goto L333AD;
    if (quest_init_person(l_4C) != 0) goto L333DD;
    return 0;
L333DD:;
    goto L333AD;
L333DF:;
    if (l_2C++ <= 20) goto L333F6;
    return 0;
L333F6:;
    goto L33375;
L333FB:;
    l_50 = a1 + ((int)(short)*(short *)((char *)a1 + 44));
    l_30 = 0;
L33411:;
    if (((int)(short)*(short *)((char *)a1 + 24)) > l_30) goto L3342E;
    goto L33452;
L3341F:;
    l_30++;
    (*(char (**)[24])&l_50)++;
    goto L33411;
L3342E:;
    *(int *)((char *)l_50 + 16) = 0;
    if (quest_init_place(l_50) != 0) goto L33450;
    return 0;
L33450:;
    goto L3341F;
L33452:;
    l_48 = a1 + ((int)(short)*(short *)((char *)a1 + 36));
    l_30 = 0;
L33468:;
    if (((int)(short)*(short *)((char *)a1 + 16)) > l_30) goto L33488;
    goto L334F6;
L33479:;
    l_30++;
    (*(char (**)[19])&l_48)++;
    goto L33468;
L33488:;
    if (((int)(unsigned char)(*(signed char *)((char *)l_48 + 2) & 2)) != 0) goto L334A5;
    if (((int)(short)*(short *)((char *)l_48 + 3)) == 100) goto L334A7;
L334A5:;
    goto L334D2;
L334A7:;
    mc_memcpy(l_48, *(int *)(D_00195984 + (((int)(short)*(short *)((char *)l_48 + 5)) << 2)), 19, (int)D_00170A64, 660, 4);
    goto L334F4;
L334D2:;
    *(int *)((char *)l_48 + 11) = 0;
    if (quest_init_item(l_48) != 0) goto L334F4;
    return 0;
L334F4:;
    goto L33479;
L334F6:;
    l_44 = a1 + ((int)(short)*(short *)((char *)a1 + 50));
    l_30 = 0;
L3350C:;
    if (((int)(short)*(short *)((char *)a1 + 30)) > l_30) goto L33529;
    goto L3354D;
L3351A:;
    l_30++;
    (*(char (**)[14])&l_44)++;
    goto L3350C;
L33529:;
    *(int *)((char *)l_44 + 10) = 0;
    if (quest_init_foe(l_44) != 0) goto L3354B;
    return 0;
L3354B:;
    goto L3351A;
L3354D:;
    l_40 = a1 + ((int)(short)*(short *)((char *)a1 + 52));
    l_30 = 0;
L33563:;
    if (((int)(short)*(short *)((char *)a1 + 32)) > l_30) goto L33583;
    goto L33692;
L33574:;
    l_30++;
    (*(char (**)[87])&l_40)++;
    goto L33563;
L33583:;
    l_3C = l_40 + 6;
    *(int *)((char *)l_40 + 83) = *(int *)game_minutes;
    *(short *)((char *)l_40 + 4) = ((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)qbn_opcode_arg_counts + ((int)(short)*(short *)((char *)l_40))))) - 48;
    *(int *)&l_1C = 0;
L335BB:;
    if (((int)(short)*(short *)((char *)l_40 + 4)) > *(int *)&l_1C) goto L335DB;
    goto L3368D;
L335CC:;
    (*(int *)&l_1C)++;
    (*(char (**)[15])&l_3C)++;
    goto L335BB;
L335DB:;
    if (*(int *)((char *)l_3C + 1) == 305419896) goto L33674;
    if (*(int *)((char *)l_3C + 7) == (-1)) goto L335FD;
    if (*(int *)((char *)l_3C + 7) != (-2)) goto L335FF;
L335FD:;
    goto L3365E;
L335FF:;
    l_28 = *(int *)((char *)l_3C + 1) & 255;
    l_24 = *(int *)((char *)l_3C + 1) >> 8;
    l_38 = a1 + ((int)(short)*(short *)((char *)((l_24 * 2) + a1) + 36));
    l_38 += ((int)(short)*(short *)(qbn_record_sizes + (l_24 * 2))) * l_28;
    *(int *)((char *)l_3C + 1) = l_38;
    *(int *)((char *)l_3C + 11) = quest_record_object(l_24, l_38);
    goto L33672;
L3365E:;
    *(int *)((char *)l_3C + 1) = 0;
    *(int *)((char *)l_3C + 11) = 0;
L33672:;
    goto L33688;
L33674:;
    *(int *)((char *)l_3C + 1) = 0;
    *(int *)((char *)l_3C + 11) = 0;
L33688:;
    goto L335CC;
L3368D:;
    goto L33574;
L33692:;
    if (*(int *)((char *)a1 + 56) == 0) goto L336DA;
    l_34 = a1 + *(int *)((char *)a1 + 56);
L336A7:;
    if (*(signed char *)((char *)l_34) == 0) goto L336DA;
    *(int *)((char *)l_34 + 23) = quest_record(a1, (int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)l_34 + 20)), (int)(short)*(short *)((char *)l_34 + 21));
    (*(char (**)[27])&l_34)++;
    goto L336A7;
L336DA:;
    return 1;
}

int quest_record_object(int a1, int a2)
{
    switch ((unsigned)a1) {
    goto L33751;
case 3:
    return *(int *)((char *)a2 + 12);
case 0:
    return *(int *)((char *)a2 + 11);
case 4:
    return *(int *)((char *)a2 + 16);
case 7:
    return *(int *)((char *)a2 + 10);
default:
L33751:;
    return 0;
}
}

void qaction_place_foe(int a1, int a2)
{
    int l_14;

    l_14 = *(int *)((char *)a1 + 32);
    if (quest_place_object(l_14, a2) == 0) return;
    if (a2 != 0) goto L3379C;
    *(signed char *)D_00196299 = 1;
L3379C:;
    monster_init_gear(l_14);
}

int func_000337AD(int a1, int a2, int a3)
{
    if (((int)(short)*(short *)((char *)a2 + 6)) <= (-1)) goto L338A4;
    if (((int)(short)*(short *)((char *)a2 + 6)) < 17) goto L337EA;
    if (((int)(short)*(short *)((char *)a2 + 6)) <= 20) goto L337EC;
L337EA:;
    goto L337FC;
L337EC:;
    if (((int)(unsigned char)*(signed char *)((char *)a3 + 24)) >= 17) goto L337FE;
L337FC:;
    goto L3380E;
L337FE:;
    if (((int)(unsigned char)*(signed char *)((char *)a3 + 24)) <= 20) goto L33813;
L3380E:;
    goto L338A4;
L33813:;
    if (((int)(short)*(short *)((char *)a2 + 8)) != (-1)) goto L33838;
    return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 20480);
L33838:;
    if (((int)(short)*(short *)((char *)a2 + 8)) == 1) goto L3388B;
    return (((((int)(unsigned short)(*(short *)((char *)a1 + 2) & 16384)) != 0) && (((int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096)) == 0)) ? 1 : 0);
L3388B:;
    return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096);
L338A4:;
    if (((int)(short)*(short *)((char *)a2 + 6)) <= (-1)) goto L33921;
    if (((int)(unsigned char)*(signed char *)((char *)a3 + 24)) != 11) goto L338D5;
    if ((short)((int)(unsigned char)*(signed char *)((char *)a3 + 24)) == *(short *)((char *)a2 + 6)) goto L338D7;
L338D5:;
    goto L338E8;
L338D7:;
    if (((int)(unsigned short)*(short *)((char *)a3 + 18)) == 40) goto L338EA;
L338E8:;
    goto L338EC;
L338EA:;
    goto L33921;
L338EC:;
    if (((int)(short)*(short *)((char *)a2 + 6)) != 11) goto L33904;
    return 0;
L33904:;
    if ((short)((int)(unsigned char)*(signed char *)((char *)a3 + 24)) == *(short *)((char *)a2 + 6)) goto L33921;
    return 0;
L33921:;
    if (((int)(short)*(short *)((char *)a2 + 8)) != (-1)) goto L33943;
    return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 20480);
L33943:;
    if (((int)(short)*(short *)((char *)a2 + 8)) == 1) goto L33993;
    return (((((int)(unsigned short)(*(short *)((char *)a1 + 2) & 16384)) != 0) && (((int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096)) == 0)) ? 1 : 0);
L33993:;
    return (int)(unsigned short)(*(short *)((char *)a1 + 2) & 4096);
}

int quest_place_object(int a1, int a2)
{
    int l_1C;
    short l_14;

    if (a2 != 0) goto L33F7F;
    if (spawn_find_point(a1, 512, 1024) == 0) goto L33F73;
    l_1C = object_create_child(*(int *)(*(char **)player_object + 67), 0, 0);
    *(int *)((char *)l_1C + 7) = *(int *)((char *)a1 + 7);
    *(int *)((char *)l_1C + 11) = *(int *)((char *)a1 + 11);
    *(int *)((char *)l_1C + 15) = *(int *)((char *)a1 + 15);
    func_000310E1(a1, l_1C);
    object_free_single(l_1C);
    return *(int *)((char *)a1 + 31);
L33F73:;
    return 0;
L33F7F:;
    if (*(int *)((char *)a1 + 51) == 0) goto L33F93;
    object_delete(*(int *)((char *)a1 + 51));
L33F93:;
    l_1C = object_find_by_id(*(int *)nonworld_root, *(int *)(*(char **)((char *)a2 + 16) + 31));
    if (l_1C != 0) goto L33FBB;
    return 0;
L33FBB:;
    object_reparent(l_1C, a1);
    *(int *)((char *)a1 + 31) = object_new_id(((unsigned)*(int *)((char *)l_1C + 31)) >> 16);
    l_1C = *(int *)((char *)a1 + 63);
L33FE5:;
    if (l_1C == 0) goto L3400F;
    *(int *)((char *)l_1C + 31) = object_new_id(((unsigned)*(int *)(*(char **)((char *)l_1C + 67) + 31)) >> 16);
    l_1C = *(int *)((char *)l_1C + 55);
    goto L33FE5;
L3400F:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) == 2) goto L3402D;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 18) goto L3402F;
L3402D:;
    goto L34068;
L3402F:;
    l_14 = *(short *)((char *)a1 + 89);
    mc_memcpy(a1 + 71, (int)&*(signed char *)(*(char **)((char *)a2 + 16) + 71), 26, (int)D_00170A64, 924, 4);
    *(short *)((char *)a1 + 89) = *(int *)&l_14;
L34068:;
    if ((((unsigned)*(int *)((char *)a1 + 31)) >> 16) != (((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16)) goto L3408D;
    a1 = func_000310E1(a1, 0);
L3408D:;
    return *(int *)((char *)a1 + 31);
}

void qaction_place_item(int a1, int a2)
{
    int l_14;

    l_14 = *(int *)((char *)a2 + 32);
    if (*(int *)((char *)l_14 + 51) == 0) goto L340DC;
    object_delete(*(int *)((char *)l_14 + 51));
    *(int *)((char *)l_14 + 51) = 0;
L340DC:;
    quest_place_object(l_14, *(int *)((char *)a2 + 37));
}

void qaction_place_npc(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    if (*(int *)((char *)a2 + 28) != (-1)) goto L3419A;
    l_1C = *(int *)((char *)a2 + 47);
    map_goto_location((int)(unsigned char)*(signed char *)current_region, 3, (int)(unsigned short)*(short *)((char *)l_1C + 27), (int)(unsigned short)*(short *)((char *)l_1C + 29));
    l_1C = *(int *)(*(char **)((char *)a2 + 47) + 51);
    if (l_1C == 0) goto L34198;
    *(int *)(*(char **)player_object + 7) = *(int *)((char *)l_1C + 7);
    *(int *)(*(char **)player_object + 11) = *(int *)((char *)l_1C + 11);
    *(int *)(*(char **)player_object + 15) = *(int *)((char *)l_1C + 15);
    *(short *)(*(char **)player_object + 3) = *(short *)(*(char **)camera_object + 3);
    *(signed char *)D_001940D5 |= 2;
L34198:;
    return;
L3419A:;
    l_1C = *(int *)((char *)a2 + 32);
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) == 65) return;
    if (*(int *)((char *)l_1C + 51) == 0) goto L341C6;
    object_delete(*(int *)((char *)l_1C + 51));
L341C6:;
    *(int *)((char *)l_1C + 51) = 0;
    quest_place_object(l_1C, *(int *)((char *)a2 + 37));
}

void qaction_give_item_to_foe(int a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = *(int *)((char *)a2 + 32);
    if (*(int *)((char *)l_1C + 51) == 0) goto L34221;
    object_delete(*(int *)((char *)l_1C + 51));
    *(int *)((char *)l_1C + 51) = 0;
L34221:;
    l_14 = *(int *)(*(char **)((char *)a2 + 47) + 51);
    if (l_14 == 0) goto L342B0;
    l_18 = object_create_child(l_14, 0, 107);
    *(signed char *)((char *)l_18) = 2;
    *(int *)((char *)l_18 + 31) = object_new_id(((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16);
    *(short *)((char *)l_18 + 27) = *(short *)((char *)l_1C + 27);
    *(signed char *)((char *)l_18 + 38) = *(signed char *)((char *)a1);
    *(int *)((char *)l_18 + 51) = l_1C;
    *(int *)((char *)l_1C + 51) = l_18;
    mc_memcpy(l_18 + 71, l_1C + 71, 107, (int)D_00170A64, 1008, 4);
L342B0:;
    l_14 = *(int *)((char *)a2 + 47);
    *(int *)((char *)l_1C + 31) = object_new_id(((unsigned)*(int *)((char *)l_14 + 31)) >> 16);
    object_reparent(l_14, l_1C);
}

int quest_find_site_for_building(int a1)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_34 = *(int *)(*(char **)D_00195A00 + 63);
L342FF:;
    if (l_34 == 0) goto L34448;
    l_30 = *(int *)((char *)l_34 + 55);
    if (((int)(unsigned char)*(signed char *)((char *)l_34)) != 14) goto L3443D;
    *(int *)D_00195D00 = l_34;
    *(int *)D_00199780 = l_34 + 71;
    l_28 = quest_section(*(int *)D_00199780, 4);
    l_1C = 0;
L34351:;
    if (((int)(short)*(short *)(*(char **)D_00199780 + 24)) > l_1C) goto L34370;
    goto L343BD;
L34361:;
    l_1C++;
    (*(char (**)[24])&l_28)++;
    goto L34351;
L34370:;
    l_2C = *(int *)((char *)l_28 + 16);
    if (((int)(unsigned char)(*(signed char *)((char *)l_28 + 2) & 64)) == 0) goto L34393;
    *(signed char *)D_001962A3 = 1;
    goto L3439A;
L34393:;
    *(signed char *)D_001962A3 = 0;
L3439A:;
    if (l_2C == 0) goto L343AE;
    if (*(int *)((char *)a1 + 20) == *(int *)((char *)l_2C + 91)) goto L343B0;
L343AE:;
    goto L343BB;
L343B0:;
    return l_2C;
L343BB:;
    goto L34361;
L343BD:;
    l_24 = quest_section(*(int *)D_00199780, 3);
    l_1C = 0;
L343D6:;
    if (((int)(short)*(short *)(*(char **)D_00199780 + 22)) > l_1C) goto L343F5;
    goto L3443D;
L343E6:;
    l_1C++;
    (*(char (**)[20])&l_24)++;
    goto L343D6;
L343F5:;
    l_20 = *(int *)((char *)l_24 + 12) + 71;
    if (((int)(short)(*(short *)((char *)l_24 + 2) & 16384)) == 0) goto L3441B;
    *(signed char *)D_001962A3 = 1;
    goto L34422;
L3441B:;
    *(signed char *)D_001962A3 = 0;
L34422:;
    if (*(int *)((char *)a1 + 20) != *(int *)((char *)l_20 + 20)) goto L3443B;
    return *(int *)((char *)l_24 + 12);
L3443B:;
    goto L343E6;
L3443D:;
    l_34 = l_30;
    goto L342FF;
L34448:;
    return 0;
}

int func_0003445C(int a1)
{
    int l_20;
    int l_1C;

    l_1C = 0;
    l_20 = 0;
L3447B:;
    if (l_20 < 3) goto L3448B;
    goto L344A1;
L34483:;
    l_20++;
    goto L3447B;
L3448B:;
    if (*(int *)((char *)((l_20 << 2) + a1)) == 0) goto L3449F;
    l_1C++;
L3449F:;
    goto L34483;
L344A1:;
    if (l_1C != 0) goto L344B0;
    return 0;
L344B0:;
    return *(int *)((char *)((rand_range(0, l_1C - 1) << 2) + a1));
}

int func_000344D3(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
L344EE:;
    if (l_20 < *(int *)faction_count) goto L34503;
    goto L3451E;
L344FB:;
    l_20++;
    goto L344EE;
L34503:;
    if (*(short *)((char *)(int)(*(char **)factions + (l_20 * 92)) + 29) >= 0) goto L3451C;
    l_1C++;
L3451C:;
    goto L344FB;
L3451E:;
    if (l_1C != 0) goto L34530;
    return 0;
L34530:;
    l_1C = rand_range(0, l_1C - 1);
    l_20 = 0;
L34545:;
    if (l_20 < *(int *)faction_count) goto L3455A;
    goto L34591;
L34552:;
    l_20++;
    goto L34545;
L3455A:;
    if (*(short *)((char *)(int)(*(char **)factions + (l_20 * 92)) + 29) >= 0) goto L34552;
    if (l_1C != 0) goto L34589;
    return (int)(unsigned short)*(short *)((char *)(int)(*(char **)factions + (l_20 * 92)) + 33);
L34589:;
    l_1C--;
    goto L34552;
L34591:;
    return 0;
}

int quest_object_in_use(int a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_2C = *(int *)(*(char **)D_00195A00 + 63);
L345C1:;
    if (l_2C == 0) goto L346A4;
    if (((int)(unsigned char)*(signed char *)((char *)l_2C)) != 14) goto L34696;
    l_20 = l_2C + 71;
    l_28 = quest_section(l_20, 4);
    l_1C = 0;
L345FE:;
    if (((int)(short)*(short *)((char *)l_20 + 24)) > l_1C) goto L3461B;
    goto L34640;
L3460C:;
    l_1C++;
    (*(char (**)[24])&l_28)++;
    goto L345FE;
L3461B:;
    if (*(int *)((char *)l_28 + 16) == 0) goto L3463E;
    if (*(int *)(*(char **)((char *)l_28 + 16) + 31) != a1) goto L3463E;
    return 1;
L3463E:;
    goto L3460C;
L34640:;
    l_24 = quest_section(l_20, 3);
    l_1C = 0;
L34657:;
    if (((int)(short)*(short *)((char *)l_20 + 22)) > l_1C) goto L34674;
    goto L34696;
L34665:;
    l_1C++;
    (*(char (**)[20])&l_24)++;
    goto L34657;
L34674:;
    if (*(int *)((char *)l_24 + 12) == 0) goto L34694;
    if (*(int *)(*(char **)((char *)l_24 + 12) + 31) != a1) goto L34694;
    return 1;
L34694:;
    goto L34665;
L34696:;
    l_2C = *(int *)((char *)l_2C + 55);
    goto L345C1;
L346A4:;
    return 0;
}
