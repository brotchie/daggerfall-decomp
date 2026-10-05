/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000830C7 */
#pragma pack(1)
struct obj {
    unsigned char type;         /* 0 */
    short x1;                   /* 1 */
    short f3;                   /* 3 */
    short f5;                   /* 5 */
    int x, y, z;                /* 7 */
    char pad19[2];
    unsigned short flags;       /* 21 */
    unsigned short f23;         /* 23 */
    char pad25[2];
    unsigned short snd;         /* 27 */
    unsigned short f29;         /* 29 */
    char pad31[4];
    unsigned char f35;          /* 35 */
    short f36;                  /* 36 */
    char pad38[9];
    int handle;                 /* 47 */
    char pad51[12];
    unsigned char *f63;         /* 63 */
    unsigned char *f67;         /* 67 */
    unsigned char data[1];      /* 71 */
};
struct light {                  /* 17 bytes */
    int x, y, z;
    unsigned short snd;
    unsigned short f14;
    unsigned char f16;
};
struct anim {                   /* 66 bytes */
    unsigned short id;
    unsigned char rec;
    char pad3;
    int handle;
    char pad8[58];
};
struct flame {
    int handle;
    char pad4[8];
    char pos[20];
    int x, y, z;
    int f44, f48, f52;
};
struct stat15 { unsigned short f:15; };
struct bits2 { unsigned char b0:1, b1:1, b2:1; };
#pragma pack()
extern int D_000C5404;
extern int D_001343C0;
struct race { unsigned short flags; char pad[27]; };
extern struct race monster_table_flags[];
extern short frame_counter;
extern unsigned char current_climate;
extern int D_00199808;
extern char cfg_show_markers;
extern struct flame D_001A945E;
extern unsigned char D_001A949C;
extern char D_001A949D;
extern void spell_area_effect(struct obj *);
extern void func_00073ADF(struct obj *);
extern int weapon_arrow_update(struct obj *);
extern void object_free_later(struct obj *);
extern int func_0007E1A7(struct obj *);
extern void func_0007E246(struct obj *);
extern int model_get(unsigned short, int, int);
extern void flat_animal_sound(int, int, int, int, int);
extern int func_000C013B();
extern int func_000C5280();
extern int func_000C7F14();
extern int func_000C7F98();
extern int func_00135DE4();
extern int func_00136AD8();
extern int func_001401D4();
extern int func_00154D00();

int object_draw_cb(struct obj *a1)
{
    struct flame *l_4C;
    unsigned char *l_48;
    struct light *l_44;
    struct anim *l_40;
    unsigned char *l_3C;
    unsigned char *l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    unsigned char *l_24;
    unsigned char *l_20;

    if (D_001A949D != 0)
        return 1;
    if (a1->flags & 512)
        return 0;
    if (a1->type != 9)
        a1->handle = 0;
    D_000C5404 = 0;
    switch (a1->type) {
    case 18:
        a1->x1 = frame_counter;
        l_38 = (unsigned char *)a1 + 705;
        func_000C013B(l_38);
        a1->snd = (a1->snd & -128) | (l_38[22] + l_38[21]);
        l_20 = (unsigned char *)func_00135DE4(a1->snd >> 7, a1->snd & 127);
        *(short *)(l_38 + 12) = *(short *)(l_20 + 22);
        l_34 = a1->snd >> 7;
        if (l_34 == 280 || l_34 == 281) {
            func_00136AD8(a1->x, a1->y, a1->z, 31, 256, 0);
            l_30 = 4194304;
        } else {
            l_30 = 0;
        }
        if ((a1->snd >> 7) == 268 && *(short *)l_38 >= 4)
            l_34 = 1;
        l_24 = a1->data;
        if (((struct bits2 *)(l_24 + 137))->b2 == 0) {
            if ((monster_table_flags[l_24[67]].flags & 1) && l_24[67] != 29 && a1->y - 90 > *(int *)(l_24 + 88))
                a1->handle = func_00154D00(a1->x, a1->y - 30, a1->z, a1->snd, *(short *)l_38, (*(unsigned short *)(l_38 + 16) >> 10) & 32 | 4, l_30 + 256);
            else
                a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, *(short *)l_38, (*(unsigned short *)(l_38 + 16) >> 10) & 32 | 4, l_30 + 256);
        }
        break;
    case 42:
        if (func_0007E1A7(a1) != 0) {
            object_free_later(a1);
            break;
        }
        a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, a1->f23, 1, 256);
        func_0007E246(a1);
        break;
    case 53:
        a1->x1 = frame_counter;
        if (a1->flags & 16384)
            l_34 = 36;
        else
            l_34 = 4;
        if ((a1->snd & 127) >= 5)
            a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, (unsigned)(*(int *)0x46c & 32) >> 5, l_34, 256);
        else
            a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, -1, l_34, 256);
        break;
    case 8:
        a1->x1 = frame_counter;
        if (a1->snd == 0 || a1->snd == 65535) {
            l_34 = 1;
            break;
        }
        a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, -1, 4, 256);
        break;
    case 2:
        if (a1->f29 == 998 && a1->snd == 0) {
            l_4C = &D_001A945E;
            l_4C->handle = model_get(a1->f29, a1->snd, (current_climate << 2) + D_001A949C);
            if (l_4C->handle != 0) {
                l_4C->x = a1->x;
                l_4C->y = a1->y;
                l_4C->z = a1->z;
                if (weapon_arrow_update(a1) == 0)
                    break;
                func_00073ADF(a1);
                l_4C->f44 = a1->f23;
                l_4C->f48 = a1->f5;
                l_4C->f52 = 0;
                func_001401D4(l_4C, 0);
                break;
            }
        }
        a1->x1 = frame_counter;
        if (a1->snd == 0 || a1->snd == 65535 || a1->snd == 200)
            break;
        if ((a1->snd >> 7) == 210)
            l_30 = 63;
        else
            l_30 = 0;
        a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, -1, 4, (l_30 << 16) + 256);
        break;
    case 34:
        if (cfg_show_markers == 0)
            break;
    case 33:
    case 44:
        a1->x1 = frame_counter;
        if (a1->snd == 0 || a1->snd == 65535 || a1->snd == 200)
            break;
        if ((a1->snd >> 7) == 210)
            l_30 = 63;
        else
            l_30 = 0;
        a1->handle = func_00154D00(a1->x, a1->y, a1->z, a1->snd, -1, 4, (l_30 << 16) + 256);
        break;
    case 9:
        if (a1->flags & 8192) {
            if (a1->f29 & 32768) {
                if (a1->f29 == 32768)
                    spell_area_effect(a1);
                l_3C = (unsigned char *)func_00135DE4(a1->f23 >> 7, a1->f23 & 127);
                if ((int)((struct stat15 *)&a1->f29)->f >= (int)*(unsigned short *)(l_3C + 20)) {
                    a1->f29 = 36863;
                } else {
                    func_00154D00(a1->x, a1->y, a1->z, a1->f23, ((struct stat15 *)&a1->f29)->f, 1, 4129024);
                    a1->f29++;
                    *(unsigned short *)(a1->f63 + 23) >>= 1;
                }
            } else {
                func_00154D00(a1->x, a1->y, a1->z, a1->f23, -1, 1, 256);
            }
        }
        break;
    case 7:
        if (*a1->f67 == 4) {
            a1->handle = func_00136AD8(a1->x, a1->y, a1->z, a1->snd, 255, 0);
        } else {
            l_28 = func_000C5280(a1->x ^ a1->z, (D_001343C0 / 40) << 7);
            l_28 >>= 3;
            l_28 = 256 - l_28;
            l_28 = (a1->f23 * l_28) >> 8;
            a1->handle = func_00136AD8(a1->x, a1->y, a1->z, a1->snd, l_28, 0);
        }
        break;
    case 43:
        l_48 = a1->data;
        l_40 = *(struct anim **)(l_48 + 5);
        l_44 = *(struct light **)(l_48 + 9);
        for (l_34 = 0; l_48[0] > l_34; l_34++, l_40++) {
            l_40->handle = model_get(l_40->id, l_40->rec, (current_climate << 2) + D_001A949C);
            if (l_40->handle != 0)
                func_001401D4(&l_40->handle, 0);
        }
        for (l_34 = 0; l_48[1] > l_34; l_34++, l_44++) {
            if (l_44->snd == 0 || l_44->snd == 65535)
                continue;
            if ((l_44->snd >> 7) == 199 && cfg_show_markers == 0)
                continue;
            if ((l_44->snd >> 7) == 210) {
                func_00136AD8(l_44->x, l_44->y - 16, l_44->z, l_44->f14 & 255, l_44->f14 >> 8, 0);
                l_30 = 63;
            } else {
                l_30 = 0;
            }
            func_00154D00(l_44->x, l_44->y, l_44->z, l_44->snd, -1, 4, (l_30 << 16) + 256);
        }
        break;
    case 6:
    case 32:
        if (a1->f29 == 0)
            break;
        l_4C = (struct flame *)a1->data;
        l_4C->handle = model_get(a1->f29, a1->snd, (current_climate << 2) + D_001A949C);
        if (l_4C->handle != 0) {
            l_4C->x = a1->x;
            l_4C->y = a1->y;
            l_4C->z = a1->z;
            if (a1->f29 == 998) {
                if (weapon_arrow_update(a1) == 0)
                    break;
                func_00073ADF(a1);
                l_4C->f44 = a1->f23;
                l_4C->f48 = a1->f5;
                l_4C->f52 = 0;
            } else if (a1->f35 != 255) {
                func_000C7F98(l_4C->pos, a1->f36);
            } else {
                if (a1->f29 == 610 && a1->snd == 32)
                    D_000C5404 = 3;
                func_000C7F14(l_4C->pos, a1->x1, a1->f3 + a1->f36, a1->f5);
            }
            func_001401D4(l_4C, 0);
        }
        break;
    case 56:
        l_40 = (struct anim *)a1->data;
        l_44 = (struct light *)(l_40 + a1->snd);
        for (l_34 = 0; a1->snd > l_34; l_34++, l_40++) {
            l_40->handle = model_get(l_40->id, l_40->rec, (current_climate << 2) + D_001A949C);
            if (l_40->handle != 0)
                func_001401D4(&l_40->handle, 0);
        }
        for (l_34 = 0; a1->f23 > l_34; l_34++, l_44++) {
            if (l_44->snd == 0 || l_44->snd == 65535)
                continue;
            if ((l_44->snd >> 7) == 199 && cfg_show_markers == 0)
                continue;
            if (l_44->f14 == 0) {
                if ((l_44->snd >> 7) == 210 && D_00199808 == 0) {
                    l_28 = func_000C5280(a1->x ^ a1->z, (D_001343C0 / 40) << 7);
                    l_28 >>= 3;
                    l_28 = 256 - l_28;
                    l_30 = (l_28 * 225) >> 8;
                    l_28 = l_30;
                    func_00136AD8(l_44->x, l_44->y - l_44->f16 * 3, l_44->z, 64, l_28, 0);
                } else {
                    l_30 = 0;
                }
                func_00154D00(l_44->x, l_44->y, l_44->z, l_44->snd, -1, 4, (l_30 << 16) + 256);
                flat_animal_sound(l_44->x, l_44->y, l_44->z, l_44->snd >> 7, l_44->snd & 127);
            }
        }
        break;
    }
    return 0;
}
