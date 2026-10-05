/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000845F1 */
extern char D_00187D30[];
extern char D_00195AC4[];
extern char current_location[];
extern char game_settings[];
extern char current_region[];
extern char *flats_cfg_find(int);
extern char *rmb_make_light(int, int, short);
extern char *rmb_make_marker(int, int);
extern char *object_create_child(int, int, int);

char *rmb_make_flat(int a1, short a2, short a3, int a4)
{
    char *l_20;
    char *l_1C;

    if (a4 != 0) {
        switch (a2 >> 7) {
        case 199:
            l_1C = rmb_make_marker(a1, a2);
            break;
        case 210:
            l_1C = rmb_make_light(a1, a3 >> 8, a3 & 255);
            break;
        default:
            l_1C = object_create_child(a1, 0, 0);
            *l_1C = 33;
            *(short *)(l_1C + 19) = 8000;
            *(short *)(l_1C + 27) = a2;
            *(int *)(l_1C + 31) = *(int *)(*(char **)D_00195AC4 + 31) + (*(unsigned short *)(*(char **)current_location + 37))++;
            l_20 = flats_cfg_find(a2);
            if ((l_20[6] & 2) && (**(unsigned short **)game_settings & 4))
                *(short *)(l_1C + 27) = 0;
            break;
        }
    } else {
        l_1C = object_create_child(a1, 0, 3);
        *l_1C = 8;
        *(int *)(l_1C + 31) = *(int *)(*(char **)D_00195AC4 + 31) + (*(unsigned short *)(*(char **)current_location + 37))++;
        *(short *)(l_1C + 27) = a2;
        *(short *)(l_1C + 19) = 8000;
        l_20 = flats_cfg_find(a2);
        if ((l_20[6] & 2) && (**(unsigned short **)game_settings & 4))
            *(short *)(l_1C + 27) = 0;
        if (a3 == 0)
            a3 = *(short *)(D_00187D30 + *(unsigned char *)current_region * 2);
        *(short *)(l_1C + 71) = a3;
    }
    return l_1C;
}
