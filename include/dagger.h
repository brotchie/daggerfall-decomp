/* Shared declarations for FALL.EXE. Names are addresses until a better name is known:
 * D_XXXXXXXX for data, func_XXXXXXXX for code. Each global has one declaration here, so every
 * unit agrees on its type. */
#ifndef DAGGER_H
#define DAGGER_H

/* data */
extern int text_blank;
extern int scratch_190be4;
extern int scratch_190be8;
extern unsigned char D_00190C78;
extern int scratch_190df4;
extern char *nonworld_root;
extern char *player_object;
extern char *location_object;
extern int found_object;
extern int D_00195B84;
extern char *player_character;
extern char mouse_control_mode;
extern int factions;
extern int parse_name_seed;
extern short qbn_record_sizes[];

/* code */
extern void door_key_match_cb();
extern int faction_find_r(int, int);
extern void func_000193DD(int);
extern unsigned char climate_lookup(int, int);
extern int quest_section(char *, short);
extern void item_make_random(unsigned short, char *);
extern int name_generate(unsigned char, unsigned char);
extern char *object_create_child(char *, int, int);
extern void object_foreach(int, void (*)());
extern char *object_find_by_id(char *, int);
extern void object_delete_quest_objects(char *, int);
extern int rand(void);
extern void srand(int);
extern int xn_math_approx_dist2d(int, int, int, int);

#endif
