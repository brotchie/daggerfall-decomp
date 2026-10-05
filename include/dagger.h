/* Shared declarations for FALL.EXE. Names are addresses until a better name is known:
 * D_XXXXXXXX for data, func_XXXXXXXX for code. Each global has one declaration here, so every
 * unit agrees on its type. */
#ifndef DAGGER_H
#define DAGGER_H

/* data */
extern int text_blank;
extern int D_00190BE4;
extern int D_00190BE8;
extern unsigned char D_00190C78;
extern int text_macro_fae;
extern char *nonworld_root;
extern char *player_object;
extern char *D_00195AC4;
extern int D_00195AF4;
extern int D_00195B84;
extern char *player_character;
extern char mouse_control_mode;
extern int factions;
extern int parse_name_seed;
extern short qbn_record_sizes[];

/* code */
extern void func_00013981();
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
extern int func_0009DC25(void);
extern void func_0009DC49(int);
extern int func_000C7FD9(int, int, int, int);

#endif
