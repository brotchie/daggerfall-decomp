/* matched by the real Watcom C32 10.0a (-d2): a run of custom.c from 0x0005506F to 0x000553B2, kept together for its switch table's alignment */
#include "records.h"

#pragma pack(1)
struct slot { unsigned char kind; unsigned char bit; };
#pragma pack()
extern unsigned char mouse_buttons;
extern char D_00175420[];
extern signed char classmaker_special_counts[];
extern short scratch_190d68;
extern short classmaker_special_list;
extern short D_00190D84;
extern int scratch_190de4;
extern unsigned short *scratch_190dec;
extern struct career *player_class;
extern struct slot classmaker_specials[][7];
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern int xn_mouse_poll_clamped();
extern void xn_bits_set_or_clear_u8(void *, int, int);     /* sets or clears bits in a byte */
extern void xn_bits_set_or_clear_u16(void *, int, int);     /* in a u16 */
extern int xn_draw_image();
extern int xn_draw_image_transparent();
void classmaker_set_advantage(int, int);
void classmaker_set_disadvantage(int, int);

void classmaker_draw_dagger(void)
{
    xn_draw_image(219, 46, 40, 138, scratch_190de4);
    xn_draw_image_transparent(219, scratch_190d68, scratch_190dec[2], scratch_190dec[3], (char *)scratch_190dec + 12);
}

void classmaker_specials_remove(void)
{
    if (D_00190D84 == -1) return;
    if (classmaker_special_list == 0)
        classmaker_set_advantage(D_00190D84, 1);
    else
        classmaker_set_disadvantage(D_00190D84, 1);
    if (D_00190D84 != 6)
        mc_memcpy(&classmaker_specials[classmaker_special_list][D_00190D84], &classmaker_specials[classmaker_special_list][D_00190D84 + 1], (6 - D_00190D84) * 2, D_00175420, 952, 4);
    classmaker_special_counts[classmaker_special_list]--;
    while (mouse_buttons != 0)
        xn_mouse_poll_clamped();
}

void classmaker_set_advantage(int index, int clear)
{
    int bit;

    bit = classmaker_specials[classmaker_special_list][index].bit;
    switch (classmaker_specials[classmaker_special_list][index].kind) {
    case 0:
        xn_bits_set_or_clear_u8(&player_class->resistance_flags, 1 << bit, clear);
        break;
    case 1:
        xn_bits_set_or_clear_u8(&player_class->immunity_flags, 1 << bit, clear);
        break;
    case 2:
        xn_bits_set_or_clear_u16(&player_class->flags, 1, clear);
        break;
    case 3:
        xn_bits_set_or_clear_u8(&player_class->spell_absorption_flags, 1 << bit, clear);
        break;
    case 4:
        xn_bits_set_or_clear_u8(&player_class->rapid_healing_flags, 1 << bit, clear);
        break;
    case 5:
        xn_bits_set_or_clear_u8(&player_class->regeneration_flags, 1 << bit, clear);
        break;
    case 6:
        xn_bits_set_or_clear_u8(&player_class->attack_modifier_flags, 1 << bit, clear);
        break;
    case 7:
        xn_bits_set_or_clear_u16(&player_class->flags, 2, clear);
        break;
    case 8:
        player_class->flags &= 0xE3FF;
        if (clear == 0)
            player_class->flags |= bit << 10;
        else
            player_class->flags = 5120;
        break;
    case 9:
        xn_bits_set_or_clear_u16(&player_class->flags, 4, clear);
        break;
    case 10:
        xn_bits_set_or_clear_u8(&player_class->expert_weapons, 1 << bit, clear);
        break;
    case 11:
        xn_bits_set_or_clear_u8(&player_class->pad08, 1 << bit, clear);
        break;
    }
}

void classmaker_set_disadvantage(int index, int clear)
{
    int bit;

    bit = classmaker_specials[classmaker_special_list][index].bit;
    switch (classmaker_specials[classmaker_special_list][index].kind) {
    case 0:
        xn_bits_set_or_clear_u16(&player_class->flags, 8, clear);
        break;
    case 1:
        xn_bits_set_or_clear_u16(&player_class->flags, (1 << bit) << 4, clear);
        break;
    case 2:
        xn_bits_set_or_clear_u16(&player_class->attack_modifier_flags, (1 << bit) << 4, clear);
        break;
    case 3:
        xn_bits_set_or_clear_u16(&player_class->flags, (1 << bit) << 6, clear);
        break;
    case 4:
        xn_bits_set_or_clear_u16(&player_class->flags, (1 << bit) << 8, clear);
        break;
    case 5:
        xn_bits_set_or_clear_u16(&player_class->forbidden_equipment, 1 << bit, clear);
        break;
    case 6:
        xn_bits_set_or_clear_u8(&player_class->low_tolerance_flags, 1 << bit, clear);
        break;
    case 7:
        xn_bits_set_or_clear_u8(&player_class->critical_weakness_flags, 1 << bit, clear);
        break;
    case 8:
        xn_bits_set_or_clear_u16(&player_class->forbidden_equipment, (1 << bit) << 6, clear);
        break;
    case 9:
        xn_bits_set_or_clear_u16(&player_class->forbidden_equipment, (1 << bit) << 9, clear);
        break;
    case 10:
        xn_bits_set_or_clear_u16(&player_class->forbidden_materials, 1 << bit, clear);
        break;
    }
}
