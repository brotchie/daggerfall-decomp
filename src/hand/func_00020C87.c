/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00020C87 */
#include "records.h"

struct row { short v; char pad[78]; };
struct shop { unsigned char mul; char pad1; short base; short min; short max; };
struct res { int f0; int f4; };
extern char *screen_buffer;
extern char D_001706D4[];
extern char D_001706E1[];       /* __FILE__ */
extern unsigned char player_environment;
extern int crime_reputation_loss[];
extern struct shop crime_fine_table[];
extern char D_00187CA8;
extern struct row region_legal_reputation[];
extern int scratch_190cac;
extern char scratch_190d16;
extern signed char scratch_190d17;
extern char court_state;
extern short court_prison_days;
extern unsigned char D_001940D5;
extern struct record *location_object;
extern int creature_count;
extern char *window_image;
extern int free_later_count;
extern short D_00195F34;
extern unsigned char current_region;
extern unsigned char D_0019626F;
extern char D_00196271;
extern char D_00196272;
extern unsigned char game_mode;
extern char D_0019629C;
extern int court_reputation_change;
extern struct res *D_001A4FA0;
extern void crime_remove_monster(struct record *);
extern void court_reputation_restore(void);
extern void court_close(void);
extern void court_restore_vitals(void);
extern void msgbox_show_rsc(int, int);
extern void music_play(int);
extern char *disk_read_file(char *, int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern void msgbox_choice_rsc(short, short, short, short, unsigned char, unsigned char, unsigned char);
extern int rand_range(int, int);
extern void object_free_pending(void);
extern int gold_total(void);
extern void map_goto_location(int, int, int, int);
extern void object_foreach(struct record *, void (*)());
extern int rand(void);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int xn_gfx_present_inclusive();

int court_open(int crime)
{
    int penalty;
    int half_penalty;
    int fine;
    int i;
    int steps;
    int shortfall;
    struct membership *membership;

    penalty = 0;
    half_penalty = 0;
    if (game_mode == 21 || D_0019626F == 21)
        return 1;
    if (crime != 0) {
        crime--;
        music_play(D_001A4FA0->f4);
        D_0019629C = 1;
        scratch_190d17 = crime;
        court_reputation_change = crime_reputation_loss[crime] >> 1;
        if (player_environment == 2)
            map_goto_location(current_region, 1, location_object->image, 0);
        if (region_legal_reputation[current_region].v < 0) {
            penalty = -region_legal_reputation[current_region].v;
            if (penalty > 75)
                penalty = 75;
            half_penalty = -region_legal_reputation[current_region].v / 2;
            if (half_penalty > 75)
                half_penalty = 75;
        }
        if (rand_range(1, 100) <= half_penalty)
            scratch_190d16 = 0;
        else if (rand_range(1, 100) <= penalty)
            scratch_190d16 = 0;
        else
            scratch_190d16 = 2;
        if (region_legal_reputation[current_region].v < 0)
            fine = crime_fine_table[crime].base - region_legal_reputation[current_region].v * crime_fine_table[crime].mul;
        else
            fine = crime_fine_table[crime].base + region_legal_reputation[current_region].v * crime_fine_table[crime].mul;
        if (crime_fine_table[crime].min > fine)
            fine = crime_fine_table[crime].min;
        else if (crime_fine_table[crime].max < fine)
            fine = crime_fine_table[crime].max;
        steps = fine / 40;
        for (fine = i = court_prison_days = 0; i < steps; i++) {
            if (rand() & 1)
                fine += 40;
            else
                court_prison_days += 3;
        }
        if (gold_total() < fine) {
            shortfall = fine - gold_total();
            court_prison_days += shortfall / 40;
            fine -= shortfall;
        }
        game_mode = 21;
        D_00196272 = 1;
        window_image = disk_read_file(D_001706D4, 0);
        mc_memcpy(screen_buffer, window_image, 64000, D_001706E1, 114, 4);
        xn_gfx_present_inclusive(1);
        scratch_190cac = fine;
        D_00187CA8 = 0;
        D_001940D5 |= 64;
        D_00195F34 = 194;
        membership = guild_find_membership_by_kind(0);
        if ((scratch_190d17 == 4 || scratch_190d17 == 3) && membership != 0 && membership->rank >= rand_range(0, 19)) {
            msgbox_show_rsc(551, 1);
            court_restore_vitals();
            court_reputation_restore();
            free_later_count = 0;
            object_foreach(location_object, crime_remove_monster);
            object_free_pending();
            creature_count = 0;
            court_close();
            return 0;
        }
        membership = guild_find_membership_by_kind(3);
        if ((scratch_190d17 <= 2 || scratch_190d17 == 11) && membership != 0 && membership->rank >= rand_range(0, 19)) {
            msgbox_show_rsc(550, 1);
            court_restore_vitals();
            court_reputation_restore();
            free_later_count = 0;
            object_foreach(location_object, crime_remove_monster);
            object_free_pending();
            creature_count = 0;
            court_close();
            return 0;
        }
        D_00196271 = 0;
        if (court_prison_days == 0)
            msgbox_choice_rsc(8050, 16, 17, 0, 103, 110, 0);
        else if (fine == 0)
            msgbox_choice_rsc(8050, 16, 17, 0, 103, 110, 0);
        else
            msgbox_choice_rsc(8050, 16, 17, 0, 103, 110, 0);
        court_state = 1;
    }
    return game_mode == 21;
}
