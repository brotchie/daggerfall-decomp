/* records.h: the game's record structures (docs/state.md, config/names.csv; the conversion
 * rules are in docs/structs.md).
 *
 * Every game object (world object, entity, item, spell, effect, quest...) is a `struct record`:
 * the same 71-byte header (Daggerfall Unity's RecordRoot), then the record's own data at +0x47,
 * `r->data`, a union of the data structs by record type: `r->data.item.group` for an item,
 * `&r->data.character` for an entity's character record.
 *
 * Offsets in the comments are from the start of the struct (hex). Field names are the
 * names.csv names (`character+0x81 level` is `struct character`'s `level`). Fields nobody
 * has named are padNN, named by their offset. Everything is packed: fields sit at odd
 * offsets. Each struct's size is checked at compile time (RECORD_SIZE below), and
 * tools/offset_casts.py checks every offset comment against the layout.
 *
 * Anonymous unions give one offset its meaning per record type. The first member is the
 * default; a union whose first member is a padNN has none (offset_casts.py names the field
 * only under --prefer). */
#ifndef RECORDS_H
#define RECORDS_H

#include "ptrint.h"

/* a compile-time check: the build fails when a struct has the wrong size */
#define RECORD_SIZE(tag, n) typedef char tag##_size_check[(sizeof(struct tag) == (n)) ? 1 : -1]
/* RECORD_OFFSET checks a member's offset (natively with offsetof: the Watcom form casts a
   pointer to an int). The _P forms are for a struct holding pointers: exact where pointers are
   4 bytes (Watcom); in the native build (docs/port.md) pointers are 8 bytes and the struct may
   only grow */
#if defined(DAGGER_PORT)
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[(__builtin_offsetof(struct tag, m) == (n)) ? 1 : -1]
#define RECORD_SIZE_P(tag, n) typedef char tag##_size_check[(sizeof(struct tag) >= (n)) ? 1 : -1]
#define RECORD_OFFSET_P(tag, m, n) \
    typedef char tag##_##m##_offset_check[(__builtin_offsetof(struct tag, m) >= (n)) ? 1 : -1]
#else
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[((unsigned)&((struct tag *)0)->m == (n)) ? 1 : -1]
#define RECORD_SIZE_P(tag, n) RECORD_SIZE(tag, n)
#define RECORD_OFFSET_P(tag, m, n) RECORD_OFFSET(tag, m, n)
#endif

#pragma pack(1)

struct record;

/* ---- indexes ----------------------------------------------------------------------------- */

/* character attributes[] and base_attributes[], career attributes[] */
enum { ATTR_STR, ATTR_INT, ATTR_WIL, ATTR_AGI, ATTR_END, ATTR_PER, ATTR_SPD, ATTR_LUC };

/* character skills[] (DFU DFCareer.Skills order); career skills[] holds these ids. Checked
 * against the code: player_frame_update scales the speed by skills[21] (running);
 * the lycanthropy bonuses are +30 to 3, 21, 16, 34, 18, 30 (and 17): jumping, running,
 * stealth, critical strike, climbing, hand-to-hand (swimming); lockpick_door reads 13; the
 * career popup's hand-to-hand damage reads 30 (+0x151); inventory prices read 14. */
enum {
    SKILL_MEDICAL, SKILL_ETIQUETTE, SKILL_STREETWISE, SKILL_JUMPING, SKILL_ORCISH,
    SKILL_HARPY, SKILL_GIANTISH, SKILL_DRAGONISH, SKILL_NYMPH, SKILL_DAEDRIC,
    SKILL_SPRIGGAN, SKILL_CENTAURIAN, SKILL_IMPISH, SKILL_LOCKPICKING, SKILL_MERCANTILE,
    SKILL_PICKPOCKET, SKILL_STEALTH, SKILL_SWIMMING, SKILL_CLIMBING, SKILL_BACKSTABBING,
    SKILL_DODGING, SKILL_RUNNING, SKILL_DESTRUCTION, SKILL_RESTORATION, SKILL_ILLUSION,
    SKILL_ALTERATION, SKILL_THAUMATURGY, SKILL_MYSTICISM, SKILL_SHORT_BLADE, SKILL_LONG_BLADE,
    SKILL_HAND_TO_HAND, SKILL_AXE, SKILL_BLUNT_WEAPON, SKILL_ARCHERY, SKILL_CRITICAL_STRIKE
};

/* character equipped[] (DFU EquipSlots) */
enum {
    EQUIP_AMULET0, EQUIP_AMULET1, EQUIP_BRACELET0, EQUIP_BRACELET1, EQUIP_RING0, EQUIP_RING1,
    EQUIP_BRACER0, EQUIP_BRACER1, EQUIP_MARK0, EQUIP_MARK1, EQUIP_CRYSTAL0, EQUIP_CRYSTAL1,
    EQUIP_HEAD, EQUIP_RIGHT_PAULDRON, EQUIP_CLOAK0, EQUIP_LEFT_PAULDRON, EQUIP_CLOAK1,
    EQUIP_CHEST_CLOTHES, EQUIP_CUIRASS, EQUIP_RIGHT_HAND, EQUIP_GAUNTLETS, EQUIP_LEFT_HAND,
    EQUIP_SLOT22, EQUIP_GREAVES, EQUIP_LEGS_CLOTHES, EQUIP_SLOT25, EQUIP_BOOTS
};

/* ---- the class (career) record: CLASSnn.CFG, 74 bytes (names.csv `class+`) --------------- */

struct career {
    unsigned char resistance_flags;         /* +0x00 */
    unsigned char immunity_flags;           /* +0x01: 4 poison, 64 disease */
    unsigned char low_tolerance_flags;      /* +0x02 */
    unsigned char critical_weakness_flags;  /* +0x03 */
    unsigned short flags;                   /* +0x04: bit 3 no magicka from travel, bit 4 no fast travel by day */
    unsigned char rapid_healing_flags;      /* +0x06 */
    unsigned char regeneration_flags;       /* +0x07 */
    unsigned char pad08;                    /* +0x08: flags; classmaker advantage 11 sets one */
    unsigned char spell_absorption_flags;   /* +0x09 */
    unsigned char attack_modifier_flags;    /* +0x0A */
    unsigned short forbidden_materials;     /* +0x0B */
    unsigned char expert_weapons;           /* +0x0D */
    unsigned short forbidden_equipment;     /* +0x0E */
    unsigned char skills[12];               /* +0x10: 3 primary, 3 major, 6 minor */
    char name[16];                          /* +0x1C */
    char pad2C[8];                          /* +0x2C */
    unsigned char hp_per_level;             /* +0x34 */
    unsigned char pad35;                    /* +0x35 */
    int advancement_multiplier;             /* +0x36: 16.16 */
    short attributes[8];                    /* +0x3A */
};                                          /* +0x4A */
RECORD_SIZE(career, 74);

/* ---- the character record: the data of a type-3 entity (the player) and of a creature ----
 *
 * The player's is player_character (= player_entity + 0x47); a creature (type 18) has the same
 * record followed by the animation struct (struct monster). A few offsets mean something
 * else for creatures (names.csv `monster+`, at +0x47 + the character offset); they are
 * anonymous unions here, the player's name first. */

struct character_skill {
    short value;                    /* +0x00 */
    short uses;                     /* +0x02: names.csv skill_uses (+0x9F) */
    short pad4;                     /* +0x04 */
};

struct character {
    char name[32];                          /* +0x000 */
    short attributes[8];                    /* +0x020: ATTR_*: STR INT WIL AGI END PER SPD LUC */
    short base_attributes[8];               /* +0x030 */
    unsigned short flags;                   /* +0x040: bit 0 female, 0x10 infection pending, 0x8000 not
                                               hostile; its high byte is transport_flags (+0x41) */
    unsigned char min_metal_to_hit;         /* +0x042 */
    unsigned char race;                     /* +0x043: 8 vampire, 9 werewolf, 10 wereboar; creatures: the type */
    signed char armor_values[7];            /* +0x044 */
    unsigned char anim_slot;                /* +0x04B: creatures (monster+0x92) */
    int fall_velocity;                      /* +0x04C */
    unsigned int skills_raised_lo;          /* +0x050 */
    unsigned int skills_raised_hi;          /* +0x054 */
    union {
        int level_skill_sum_start;          /* +0x058: player */
        int ceiling_y;                      /* +0x058: creatures (monster+0x9F): the ceiling height, as ceiling_height */
    };
    int max_health_base;                    /* +0x05C */
    int lycanthrope_kill_time;              /* +0x060: player */
    unsigned int shield_end_time;           /* +0x064 */
    int shapechange_form;                   /* +0x068 */
    union {
        short special_infection;            /* +0x06C: player: 0 vampirism, 1 werewolf, 2 wereboar */
        short knockback_angle;              /* +0x06C: creatures (monster+0xB3) */
    };
    short knockback_speed;                  /* +0x06E: creatures (monster+0xB5) */
    struct record *target;                  /* +0x070: the target entity, for the player too */
    union {
        int house;                          /* +0x074: player: building id of the house */
        int attack_timer;                   /* +0x074: creatures (monster+0xBB) */
    };
    int ship_owned;                         /* +0x078 */
    short health;                           /* +0x07C */
    short max_health;                       /* +0x07E */
    unsigned char face;                     /* +0x080 */
    unsigned char level;                    /* +0x081 */
    unsigned char reflexes;                 /* +0x082 */
    short pad83;                            /* +0x083: the player's is set to 1 at a new game */
    int gold;                               /* +0x085 */
    unsigned int conditions;                /* +0x089: a bit per active effect; byte 1 is equip_effect_flags */
    short magicka;                          /* +0x08D */
    short max_magicka;                      /* +0x08F */
    union {
        short reputation[5];                /* +0x091: commoners, merchants, scholars, nobility, underworld */
        short loot_table;                   /* +0x091: creatures: the loot table, 1-based (monster_init) */
    };
    unsigned short fatigue;                 /* +0x09B: x 64 */
    struct character_skill skills[35];      /* +0x09D: SKILL_* */
    struct record *equipped[27];            /* +0x16F: EQUIP_*; right_hand [19], left_hand [21] */
    unsigned short table_flags;             /* +0x1DB: creatures (monster+0x222) */
    unsigned short attack_damage[5][2];     /* +0x1DD: creatures (monster+0x224): {min, max} */
    unsigned char pad1F1;                   /* +0x1F1: creatures get 255 (monster_init) */
    unsigned char original_race;            /* +0x1F2: player */
    int special_infection_time;             /* +0x1F3: player */
    unsigned char ascr_record;              /* +0x1F7: creatures (monster+0x23E) */
    unsigned char action;                   /* +0x1F8: creatures (monster+0x23F): 0 move, 8 attack, 16 hurt,
                                               32 spell, 48 idle; the player's is set to 1 when
                                               the vampire cure quest is given */
    unsigned char target_score;             /* +0x1F9 */
    unsigned char mobile_id;                /* +0x1FA: creatures (monster+0x241) */
    short to_hit_bonus;                     /* +0x1FB */
    union {
        int last_kill_time;                 /* +0x1FD: player */
        int give_up_timer;                  /* +0x1FD: creatures (monster+0x244) */
    };
    union {
        int last_shapechange_time;          /* +0x201: player */
        int detour_yaw;                     /* +0x201: creatures: heading of a detour (func_000622EB) */
    };
    union {
        int last_meal_time;                 /* +0x205: player */
        int detour_steps;                   /* +0x205: creatures: steps left on the detour, 1000 none */
    };
    union {
        int last_training_minutes;          /* +0x209: player */
        int detour_side;                    /* +0x209: creatures: bit 0 turns the next detour left/right */
    };
    int last_stealth_check_minutes;         /* +0x20D */
    int thieves_invite_time;                /* +0x211 */
    int brotherhood_invite_time;            /* +0x215 */
    unsigned int shield_points;             /* +0x219 */
    union {
        unsigned char vampire_clan;         /* +0x21D: player */
        unsigned char nav_blocked;          /* +0x21D: creatures (monster+0x264) */
    };
    union {
        unsigned char lock_open_chance;     /* +0x21E: player */
        unsigned char nav_direction;        /* +0x21E: creatures (monster+0x265) */
    };
    union {
        unsigned char brotherhood_invite_count; /* +0x21F: player */
        unsigned char nav_turn_count;       /* +0x21F: creatures (monster+0x266) */
    };
    union {
        unsigned char hidden_load_percent;  /* +0x220: player */
        unsigned char nav_stuck_count;      /* +0x220: creatures (monster+0x267) */
    };
    unsigned char detect_kind;              /* +0x221 */
    unsigned char thieves_invite_count;     /* +0x222 */
    unsigned char codeword;                 /* +0x223: macro_dbp_codeword: high nibble the first word,
                                               low nibble the second */
    signed char reputation_mod;             /* +0x224: the biography's "RR" answers add to it;
                                               player_reaction_mod adds it to a reputation */
    union {
        unsigned short career_id;           /* +0x225: biography persons: the class (CLASS%02d.CFG) */
        unsigned short spawn_seed;          /* +0x225: creatures: their marker's spawn seed (header +0x19),
                                               given back to the corpse */
    };
    short faction_id;                       /* +0x227: NPCs */
    unsigned char team;                     /* +0x229 */
    unsigned char pad22A;                   /* +0x22A: creatures: rand_range(table +26, +27) (monster_init) */
    unsigned char resist_chances[5];        /* +0x22B: by element */
    struct career career;                   /* +0x230: the class record (names.csv class+ rows); player_class points here */
};                                          /* +0x27A */
RECORD_SIZE_P(character, 634);

/* the creature's animation struct (monster+0x2C1), driven by XnGine's ASCR interpreter */
struct monster_anim {
    short anim_frame;               /* +0x00 */
    short pad02;                    /* +0x02 */
    char *anim_script;              /* +0x04: the ASCR record */
    char *anim_script_pos;          /* +0x08 */
    short frame_count;              /* +0x0C: from the ASCR image header +22 (monster_init, object_draw_cb) */
    short timer;                    /* +0x0E */
    union {
        struct {
            unsigned char anim_events;  /* +0x10: bit 0 strike, bit 1 missile */
            unsigned char anim_flags;   /* +0x11: bit 7 mirrored */
        };
        unsigned short anim_bits;   /* +0x10: the two read as a word (object_draw_cb, ai_creature_think) */
    };
    char pad12[2];                  /* +0x12 */
    unsigned char anim_request;     /* +0x14: 255 none */
    unsigned char anim_facing;      /* +0x15 */
    unsigned char anim_record;      /* +0x16 */
    unsigned char anim_current;     /* +0x17 */
    unsigned char pad18;            /* +0x18 */
};                                  /* +0x19 */
RECORD_SIZE_P(monster_anim, 25);

/* the data of a type-18 creature: names.csv `monster+` offsets are from the record, 0x47 more */
struct monster {
    struct character character;     /* +0x000 */
    struct monster_anim anim;       /* +0x27A: monster+0x2C1 */
};                                  /* +0x293 */
RECORD_SIZE_P(monster, 659);

/* ---- items: type 2 (and 54 in repair), 107 bytes, the savetree ItemRecord ---------------- */

struct enchantment {
    short type;                     /* +0x00: -1 ends the list */
    short param;                    /* +0x02 */
};

struct item {
    char name[32];                  /* +0x00 */
    unsigned short group;           /* +0x20: 2 armor, 3 weapons, 7 books, 28 gold, ... */
    unsigned short index;           /* +0x22 */
    unsigned int value;             /* +0x24 */
    char pad28[2];                  /* +0x28 */
    unsigned short item_flags;      /* +0x2A: 0x20 identified, 0x40 hidden, 0x800 artifact */
    unsigned short condition;       /* +0x2C */
    unsigned short max_condition;   /* +0x2E */
    unsigned char magicka_bonus;    /* +0x30: enchantment 3: taken off magicka and max_magicka when unequipped */
    unsigned char stack_count;      /* +0x31: also a potion's recipe index */
    unsigned short inventory_image; /* +0x32 */
    unsigned short dropped_image;   /* +0x34 */
    unsigned char material;         /* +0x36 */
    unsigned char armor_type;       /* +0x37 */
    unsigned char color;            /* +0x38 */
    unsigned int weight;            /* +0x39 */
    unsigned short enchant_points;  /* +0x3D */
    unsigned short message;         /* +0x3F: book id, letter, painting seed (always read as 16 bits) */
    unsigned char variants;         /* +0x41: template byte 41: picture variants added to inventory_image;
                                       ingredients: compared by the potion recipes */
    unsigned char draw_order;       /* +0x42: template byte 42: the paperdoll's drawing order */
    union {
        struct enchantment enchantments[10]; /* +0x43 */
        struct {
            struct enchantment pad43;   /* +0x43 */
            int direction[3];           /* +0x47: arrows in flight: the unit direction (weapons.c) */
        } arrow;
    };
};                                  /* +0x6B */
RECORD_SIZE(item, 107);

/* ---- spells: type 9, 89 bytes, the SPELLS.STD format ------------------------------------- */

struct spell_effect {
    unsigned char type;             /* +0x00: 255 none */
    unsigned char subtype;          /* +0x01 */
};

struct spell_range {                /* durations and chances: every read is signed (movsx) */
    signed char base;               /* +0x00: -1 none */
    signed char plus;               /* +0x01 */
    signed char per_level;          /* +0x02 */
};

struct spell_magnitude {            /* every read is signed (movsx) */
    signed char base_min;           /* +0x00 */
    signed char base_max;           /* +0x01 */
    signed char plus_min;           /* +0x02 */
    signed char plus_max;           /* +0x03 */
    signed char per_level;          /* +0x04 */
};

struct spell {
    struct spell_effect effects[3]; /* +0x00 */
    unsigned char element;          /* +0x06: 0 fire, 1 frost, 2 poison, 3 shock, 4 magic */
    unsigned char target;           /* +0x07: 0 caster, 1 touch, 2 distance, 3/4 areas */
    unsigned short effect_costs[3]; /* +0x08 */
    struct spell_range durations[3]; /* +0x0E */
    struct spell_range chances[3];  /* +0x17 */
    struct spell_magnitude magnitudes[3]; /* +0x20 */
    union {
        char name[25];              /* +0x2F */
        int missile_direction[3];   /* +0x2F: a missile in flight: its unit direction (cast_fire_missile) */
    };
    unsigned char icon;             /* +0x48: 200+slot item spell, 250 item/potion/strike */
    unsigned char id;               /* +0x49 */
    unsigned short cast_durations[3]; /* +0x4A: filled in at cast time */
    unsigned short cast_magnitudes[3]; /* +0x50 */
    unsigned char cast_chances[3];  /* +0x56 */
};                                  /* +0x59 */
RECORD_SIZE(spell, 89);

/* ---- diseases and poisons: type 11, 47 bytes (disease.c's table disease_table) -------------- */

struct disease {
    unsigned char id;               /* +0x00: < 100 a disease, >= 128 a poison */
    short stat_flags[11];           /* +0x01: 8 attributes, health, fatigue, magicka */
    short damage_min;               /* +0x17: poisons: the delay before it starts */
    short damage_max;               /* +0x19 */
    short days_left;                /* +0x1B: 255 forever, 254 finished */
    short stage;                    /* +0x1D */
    short drained[8];               /* +0x1F: attribute points taken so far */
};                                  /* +0x2F */
RECORD_SIZE(disease, 47);

/* ---- potion recipes: type 31, 109 bytes (potion_recipes) --------------------------------- */

struct potion_recipe {
    signed char ingredient_indices[8]; /* +0x00: -2 ends */
    unsigned short value;           /* +0x08 */
    signed char ingredient_groups[8]; /* +0x0A: compared signed (movsx) */
    char pad12[2];                  /* +0x12 */
    struct spell spell;             /* +0x14 */
};                                  /* +0x6D */
RECORD_SIZE(potion_recipe, 109);

/* ---- guild membership: type 10, 13 bytes ------------------------------------------------- */

struct membership {
    unsigned char rank;             /* +0x00: 0-9, > 100 expelled */
    unsigned char pad01;            /* +0x01 */
    unsigned char kind;             /* +0x02: 0 DB, 1 MG, 2 FG, 3 TG, 64|k knightly, 128|k temple */
    short faction;                  /* +0x03 */
    int rank_time;                  /* +0x05 */
    unsigned int armor_received;    /* +0x09: bit per rank */
};                                  /* +0x0D */
RECORD_SIZE(membership, 13);

/* ---- factions: 92 bytes, the tree at `factions` (0x19672C) ------------------------------- */

struct faction {
    unsigned char type;             /* +0x00: 2 group, 4 individual, 7 noble, 9 temple, ... */
    unsigned char region;           /* +0x01: 255 none */
    unsigned char ruler;            /* +0x02 */
    char name[26];                  /* +0x03 */
    short reputation;               /* +0x1D: -100..100 */
    short power;                    /* +0x1F */
    unsigned short id;              /* +0x21 */
    unsigned short vampire_clan;    /* +0x23 */
    unsigned short flags;           /* +0x25 */
    unsigned int seed;              /* +0x27 */
    unsigned int politics_factor;   /* +0x2B */
    unsigned short flats[2];        /* +0x2F */
    unsigned short face;            /* +0x33 */
    unsigned char race;             /* +0x35 */
    unsigned char social_group;     /* +0x36 */
    unsigned char guild_group;      /* +0x37 */
    struct faction *allies[3];      /* +0x38 */
    struct faction *enemies[3];     /* +0x44 */
    struct faction *next;           /* +0x50 */
    struct faction *child;          /* +0x54 */
    struct faction *parent;         /* +0x58 */
};                                  /* +0x5C */
RECORD_SIZE_P(faction, 92);

/* ---- banks: type 25 holds 62 accounts, one per region ------------------------------------ */

struct bank_account {
    int balance;                    /* +0x00 */
    int loan_owed;                  /* +0x04 */
    unsigned int loan_due;          /* +0x08: game_minutes */
    unsigned char flags;            /* +0x0C: bit 0 defaulted */
};                                  /* +0x0D */
RECORD_SIZE(bank_account, 13);

/* ---- the world map: region_locations (MAPTABLE), 17 bytes -------------------------------- */

struct map_location {
    unsigned int map_id;            /* +0x00: bits 0-19 map pixel, 20-31 MAPPITEM/MAPDITEM index */
    union {
        unsigned int x_type_flags;  /* +0x04 */
        struct { unsigned x:25; unsigned type:5; unsigned discovered:1; unsigned hidden:1; };
    };
    union {
        unsigned int z_size;        /* +0x08: z (north), then width and height in blocks (larger than
                                       the town for small places: 1x1 is 3,3) */
        struct { unsigned z:24; unsigned width:4; unsigned height:4; };
    };
    unsigned char dungeon_type;     /* +0x0C: 255 none */
    unsigned int services;          /* +0x0D */
};                                  /* +0x11 */
RECORD_SIZE(map_location, 17);

/* the current location's data (current_location; the data of location_object, type 1) */
struct location {
    char name[32];                  /* +0x00 */
    unsigned char width;            /* +0x20: blocks */
    unsigned char height;           /* +0x21 */
    unsigned char kind;             /* +0x22: dungeon type, or a town's size class */
    char pad23[2];                  /* +0x23 */
    unsigned short object_counter;  /* +0x25 */
    unsigned short marker_counter;  /* +0x27 */
    unsigned short building_count;  /* +0x29 */
    struct building *buildings;     /* +0x2B */
    unsigned char pad2F;            /* +0x2F */
};                                  /* +0x30 */
RECORD_SIZE_P(location, 48);

struct building {
    unsigned short name_seed;       /* +0x00 */
    unsigned int rent_expires;      /* +0x02 */
    unsigned char pad06;            /* +0x06 */
    unsigned char room;             /* +0x07: a tavern room number */
    unsigned char access_level;     /* +0x08 */
    char pad09[6];                  /* +0x09 */
    unsigned char flags;            /* +0x0F */
    char pad10[2];                  /* +0x10 */
    unsigned short faction_id;      /* +0x12 */
    unsigned int id;                /* +0x14 */
    unsigned char type;             /* +0x18: 0 alchemist ... 15 tavern, 16 palace, 17+ houses */
    unsigned char quality;          /* +0x19 */
};                                  /* +0x1A */
RECORD_SIZE(building, 26);

/* ---- the settings record: type 23 (Options), 6 bytes (game_settings) --------------------- */

struct settings {
    unsigned short view_flags;      /* +0x00: bit 0 full screen, bit 1 head bobbing, bit 2 DAGGER.GRD
                                       exists (load_game: hides FLATS.CFG flag-2 flats), bits 8-15
                                       the detail level 0..127; always read as a word */
    unsigned short sound_volume;    /* +0x02 */
    unsigned short music_volume;    /* +0x04 */
};                                  /* +0x06 */
RECORD_SIZE(settings, 6);

/* ---- the pick list widget (shared_picklist and others) ----------------------------------- */

struct picklist_rect {
    short x, y, w, h;               /* +0x00 */
};

struct picklist_entry {
    unsigned short flags;           /* +0x00 */
    unsigned short index;           /* +0x02 */
    char text[40];                  /* +0x04 */
};                                  /* +0x2C */

struct picklist {
    unsigned char framed;           /* +0x00 */
    unsigned char colour_normal;    /* +0x01 */
    unsigned char colour_flagged;   /* +0x02 */
    unsigned char colour_selected;  /* +0x03 */
    unsigned char colour_thumb;     /* +0x04 */
    struct picklist_rect list_rect;          /* +0x05 */
    struct picklist_rect up_rect;            /* +0x0D */
    struct picklist_rect down_rect;          /* +0x15 */
    struct picklist_rect bar_rect;           /* +0x1D */
    unsigned short count;           /* +0x25 */
    unsigned short top;             /* +0x27 */
    unsigned short selected;        /* +0x29 */
    unsigned short visible_rows;    /* +0x2B */
    short thumb_height;             /* +0x2D */
    struct picklist_entry *entries; /* +0x2F */
    char *list_background;          /* +0x33 */
    char *bar_background;           /* +0x37 */
};                                  /* +0x3B */
RECORD_SIZE_P(picklist, 0x3B);

/* ---- quests: the data of a type-14 record is the QBN file, pointers relocated ------------ */

struct quest {
    short id;                       /* +0x00 */
    short faction_id;               /* +0x02 */
    short text_file;                /* +0x04 */
    char name[9];                   /* +0x06 */
    unsigned char flags;            /* +0x0F: bit 1 rewarded */
    short section_counts[10];       /* +0x10 */
    short section_offsets[10];      /* +0x24: from the quest start */
    int text_offset;                /* +0x38: section 10, the text variables (struct qbn_text_var),
                                       0 none: a u16 and a zero u16 in the file (DFU's Null1), read
                                       as one int (quest_init_resources, quest_relink_after_load) */
};                                  /* +0x3C */
RECORD_SIZE(quest, 0x3C);

struct qbn_arg {
    unsigned char negate;           /* +0x00 */
    char *record;                   /* +0x01 */
    short section;                  /* +0x05 */
    int value;                      /* +0x07: -1 none, -2 filler */
    struct record *object;          /* +0x0B */
};                                  /* +0x0F */
RECORD_SIZE_P(qbn_arg, 15);

struct qbn_op {                     /* section 8 */
    short opcode;                   /* +0x00 */
    unsigned short flags;           /* +0x02: bit 0 done */
    short arg_count;                /* +0x04 */
    struct qbn_arg args[5];         /* +0x06: arg 0 is the state */
    short message;                  /* +0x51 */
    int last_minutes;               /* +0x53 */
};                                  /* +0x57 */
RECORD_SIZE_P(qbn_op, 87);

struct qbn_state {                  /* section 9 */
    short index;                    /* +0x00 */
    unsigned char is_global;        /* +0x02 */
    unsigned char value;            /* +0x03: or the global index */
    int name_hash;                  /* +0x04 */
};                                  /* +0x08 */
RECORD_SIZE(qbn_state, 8);

struct qbn_timer {                  /* section 6 */
    short pad00;                    /* +0x00 */
    unsigned short flags;           /* +0x02: 64 running, 128 expired, 0x400 links resolved, 16 doubled,
                                       256/512 link1/link2 is a person (qaction_op12_start_stop_timer reads it signed) */
    unsigned char type;             /* +0x04: 0 random, 1 fixed, 2-5 travel time to places/persons */
    int minimum;                    /* +0x05 */
    int maximum;                    /* +0x09 */
    int start;                      /* +0x0D: game_minutes */
    unsigned int delay;             /* +0x11: expired when game_minutes - start > delay (unsigned) */
    struct record *link1;           /* +0x15: a place or person index in the file, then its object */
    struct record *link2;           /* +0x19 */
    int state_hash;                 /* +0x1D */
};                                  /* +0x21 */
RECORD_SIZE_P(qbn_timer, 33);

struct qbn_item {                   /* section 0 */
    char pad00[2];                  /* +0x00 */
    unsigned char flags;            /* +0x02: bit 7 topic hidden */
    short group;                    /* +0x03: item group (< 0 random); gold: the maximum; 100 a template */
    short index;                    /* +0x05: item index (-1 random); gold: the minimum, -1 by level */
    int symbol;                     /* +0x07: the name's hash (quest_symbol_text) */
    struct record *object;          /* +0x0B */
    short messages[2];              /* +0x0F */
};                                  /* +0x13 */
RECORD_SIZE_P(qbn_item, 19);

struct qbn_person {                 /* section 3 */
    char pad00[2];                  /* +0x00 */
    unsigned short flags;           /* +0x02: always a word: low byte 0 here, 255 random, else elsewhere;
                                       0x100 any building, 0x200/0x400 the gender, 0x1000/0x4000 door
                                       flags; 0x2000 opcode 82, 0x4000 opcode 81, 0x8000 topic hidden */
    short kind;                     /* +0x04: 21 gets the questor rumors */
    short faction_id;               /* +0x06 */
    int symbol;                     /* +0x08: the name's hash (quest_symbol_text) */
    struct record *object;          /* +0x0C */
    short messages[2];              /* +0x10 */
};                                  /* +0x14 */
RECORD_SIZE_P(qbn_person, 20);

struct qbn_place {                  /* section 4 (DFU Places.txt: p1 p2 p3) */
    char pad00[2];                  /* +0x00 */
    unsigned char flags;            /* +0x02: bit 7 topic hidden, bit 6 opcode 81 */
    signed char scope;              /* +0x03: 0 a fixed place (10 once made), > 0 another location
                                       (counted down), -1 here (quest_init_place) */
    unsigned short p1;              /* +0x04: 0 a building, else a door/marker; a fixed place: the
                                       id's high word */
    short p2;                       /* +0x06: the building type (< 0 any shop, 17-20 houses, 11 temple);
                                       a fixed place: the id's low word */
    short p3;                       /* +0x08: -1 any door, 0 door flag 0x4000, 1 door flag 0x1000 */
    char pad0A[2];                  /* +0x0A */
    int symbol;                     /* +0x0C: the name's hash (quest_symbol_text) */
    struct record *object;          /* +0x10 */
    short messages[2];              /* +0x14 */
};                                  /* +0x18 */
RECORD_SIZE_P(qbn_place, 24);

struct qbn_foe {                    /* section 7 */
    char pad00[3];                  /* +0x00 */
    unsigned char type;             /* +0x03: the monster type */
    unsigned char count;            /* +0x04: a byte (qaction_op09_spawn_repeat, qaction_op09_spawn_repeat) */
    unsigned char killed;           /* +0x05: counted up by qcond_op02_foe_killed */
    int symbol;                     /* +0x06: the name's hash (quest_symbol_text) */
    struct record *object;          /* +0x0A */
};                                  /* +0x0E */
RECORD_SIZE_P(qbn_foe, 14);

struct qbn_text_var {               /* section 10 (quest text_offset): the text variables */
    char name[20];                  /* +0x00: 0 ends the list */
    unsigned char section;          /* +0x14 */
    short index;                    /* +0x15 */
    char *record;                   /* +0x17: quest_record(section, index) (quest_init_resources) */
};                                  /* +0x1B */
RECORD_SIZE_P(qbn_text_var, 27);

/* ---- NPCs, blessings, the automap -------------------------------------------------------- */

/* the data of a quest NPC (type 41, 58 bytes): its building entry, then its town's name
 * (quest_init_person copies current_location's; quest_symbol_text reads it) */
struct quest_npc {
    struct building building;       /* +0x00 */
    char location_name[32];         /* +0x1A */
};                                  /* +0x3A */
RECORD_SIZE(quest_npc, 58);

/* the data of a type-8 NPC (a flat person in a building or block; 3 bytes, rmb_make_flat) */
struct person {
    unsigned short faction_id;      /* +0x00: guild_service_dispatch picks the service by it */
    unsigned char flags;            /* +0x02: bit 3 shopkeeper (the building menus), bit 4 female,
                                       bit 7 a potential quest giver; others from the RMB flat */
};                                  /* +0x03 */
RECORD_SIZE(person, 3);

/* the data of a blessing (type 30), bought from a temple */
struct blessing {
    unsigned char target;           /* +0x00: skill id, 128|attribute, or 255 regional legal reputation */
    unsigned char amount;           /* +0x01: what blessing_apply added (blessing_remove reads it unsigned) */
    unsigned int end_time;          /* +0x02: game_minutes when guild_expire_blessings removes it */
    unsigned char region;           /* +0x06: target 255: the region (blessing_remove) */
};                                  /* +0x07 */
RECORD_SIZE(blessing, 7);

/* the data of the logbook record (type 24, logbook_object): the quests' log messages */
struct logbook {
    short quest_ids[32];            /* +0x000: 0 a free slot */
    short message_ids[32][10];      /* +0x040: QRC message ids per quest, 0 none */
    int message_times[32][10];      /* +0x2C0: game_minutes when each was logged */
    char places[32][32];            /* +0x7C0: the place names (current_location) for %cn */
};                                  /* +0xBC0 */
RECORD_SIZE(logbook, 3008);

/* the data of a 3D object (types 6 models, 32 doors; also D_001A945E for arrows in flight):
 * XnGine's model instance, which object_draw_cb fills and xn_model_submit draws. It is the
 * engine's struct xn_model_handle (58 bytes, 70 natively), field for field; the objects'
 * data is 4 bytes more (MODEL_INSTANCE_DATA_SIZE) */
struct model_instance {
    char *model;                    /* +0x00: model_get's result, 0 none */
    char *lights;                   /* +0x04: the engine's (xn_model_handle): this frame's light
                                       list */
    char *matrix;                   /* +0x08: the engine's: this frame's matrix slot */
    char angles[20];                /* +0x0C: xn_model_set_angles, xn_model_compose_angles */
    int x;                          /* +0x20 */
    int y;                          /* +0x24 */
    int z;                          /* +0x28 */
    int missile_angles[3];          /* +0x2C: arrows: the heading, angle_z, 0 */
    unsigned char frame;            /* +0x38: the engine's: the animation frame */
    unsigned char draw_flags;       /* +0x39: the engine's: bit 0 occluded, bit 1 drawn */
};                                  /* +0x3A */
RECORD_SIZE_P(model_instance, 58);
/* the data size of a type-6 or type-32 object (62 under Watcom) */
#define MODEL_INSTANCE_DATA_SIZE (REC_SIZEOF(struct model_instance) + 4)

/* the data of the automap record (type 51), saved as AT%05d.AMF */
struct automap {
    char notes[2048];               /* +0x0000: u16 object id, string */
    unsigned char seen_bits[8192];  /* +0x0800: a bit per object id (id low word) */
};                                  /* +0x2800 */
RECORD_SIZE(automap, 10240);

/* ---- RMB and RDB blocks (BLOCKS.BSA; Daggerfall Unity's BlocksFile and DFBlock) ---------- */

/* An RMB file (a town block) is a header (struct rmb_file), then one subrecord pair per
 * building (exterior, interior: struct block's counts, then the lists), then the block's misc
 * 3D objects and flats. A type-43 object's data is a copy of one subrecord (rmb_add_subrecord).
 * An RDB file (a dungeon block) is struct rdb_file, then object lists of struct rdb_object
 * whose resources (rdb_model, rdb_light, rdb_flat, rdb_action) are found by offsets from the
 * start of the file. */

/* a 3D object of an RMB block, 66 bytes (DFU RmbBlock3dObjectRecord) */
struct block_model {
    unsigned short id;              /* +0x00: model id (418/410 shelves); <= 10 none (DFU ObjectId1) */
    unsigned char variant;          /* +0x02: model_get's second argument; the shelf kind (DFU
                                       ObjectId2: the model is id * 100 + variant) */
    unsigned char kind;             /* +0x03: DFU ObjectType */
    char *model;                    /* +0x04: the loaded model (model_get), 0 none (DFU Unknown1) */
    char *lights;                   /* +0x08: DFU Unknown2; the engine's, once the block is drawn
                                       (&model is an xn_model_handle): the light list */
    char *matrix;                   /* +0x0C: DFU Unknown3; the engine's: the matrix slot */
    char pad10[20];                 /* +0x10: DFU NullValue1 (8 bytes), XPos1, YPos1, ZPos1 (the
                                       engine writes +0x18..+0x20) */
    int x;                          /* +0x24: made absolute by rmb_add_subrecord */
    int y;                          /* +0x28 */
    int z;                          /* +0x2C */
    char pad30[4];                  /* +0x30: DFU NullValue2 */
    int yaw;                        /* +0x34: the block's yaw is added (DFU YRotation, a short, and
                                       Unknown4) */
    char pad38[10];                 /* +0x38: DFU NullValue3, Unknown5, NullValue4 */
};                                  /* +0x42 */
RECORD_SIZE_P(block_model, 66);

/* a flat of an RMB block, 17 bytes (DFU RmbBlockFlatObjectRecord); people use the same layout
 * (DFU RmbBlockPeopleRecord) */
struct block_flat {
    int x;                          /* +0x00: made absolute by rmb_add_subrecord */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    unsigned short image;           /* +0x0C: archive<<7 | record (DFU TextureBitfield); 199<<7
                                       editor markers */
    unsigned short faction_id;      /* +0x0E: DFU FactionID */
    unsigned char flags;            /* +0x10: DFU Flags */
};                                  /* +0x11 */
RECORD_SIZE(block_flat, 17);

/* a door of an RMB block, 19 bytes (DFU RmbBlockDoorRecord; rmb_add_doors, after the people) */
struct block_door {
    int x;                          /* +0x00 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    short yaw;                      /* +0x0C: the block's yaw is added (DFU YRotation) */
    short image2;                   /* +0x0E: the door object's image2 and image: model id */
    unsigned char image;            /* +0x10:   image2 * 100 + image (DFU OpenRotation and
                                       DoorModelIndex) */
    unsigned char lock_level;       /* +0x11: DFU Unknown */
    unsigned char pad12;            /* +0x12: DFU NullValue1 */
};                                  /* +0x13 */
RECORD_SIZE(block_door, 19);

/* an RMB block's section-3 record, 16 bytes (DFU RmbBlockSection3Record; rmb_add_subrecord
 * makes it absolute) */
struct block_section3 {
    int x;                          /* +0x00 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int data;                       /* +0x0C: func_0007E441 takes byte 0 (or 1, mode 2) of the
                                       nearest one's (DFU Unknown1, Unknown2, Unknown3) */
};                                  /* +0x10 */
RECORD_SIZE(block_section3, 16);

/* the data of a type-43 object: an RMB subrecord's counts and lists (rmb_add_subrecord copies
 * the header and the three lists, then points the pointers at the copies). In the file it is
 * DFU's RmbBlockHeader: the five counts, then six unknown shorts where the copy has the
 * pointers. */
struct block {
    unsigned char model_count;      /* +0x00: struct block_model */
    unsigned char flat_count;       /* +0x01: struct block_flat */
    unsigned char section3_count;   /* +0x02: struct block_section3 */
    unsigned char people_count;     /* +0x03: 17-byte people after the subrecord (not copied) */
    unsigned char door_count;       /* +0x04: 19-byte doors after them (not copied) */
    struct block_model *models;     /* +0x05 */
    struct block_flat *flats;       /* +0x09 */
    struct block_section3 *section3; /* +0x0D */
};                                  /* +0x11 */
RECORD_SIZE_P(block, 17);

/* where a building's subrecords go in an RMB block (DFU RmbFldBlockPositions, 20 bytes;
 * town_block_place_building) */
struct rmb_block_position {
    int unknown1;                   /* +0x00: town_block_place_building keeps it in D_00196804 */
    int unknown2;                   /* +0x04: and this in D_001967EC */
    int x;                          /* +0x08: from the block's corner */
    int z;                          /* +0x0C */
    int yaw;                        /* +0x10: DFU YRotation */
};                                  /* +0x14 */
RECORD_SIZE(rmb_block_position, 20);

/* an RMB file (DFU RmbFldHeader, then the block data): town_block_load_rmb reads it from
 * BLOCKS.BSA into the buffer rmb_block points to, and rmb_index_records keeps pointers in two
 * of its unknown areas */
struct rmb_file {
    unsigned char block_data_count; /* +0x0000: DFU NumBlockDataRecords: the buildings, each an
                                       exterior and an interior subrecord */
    unsigned char misc_model_count; /* +0x0001: DFU NumMisc3dObjectRecords */
    unsigned char misc_flat_count;  /* +0x0002: DFU NumMiscFlatObjectRecords */
    struct rmb_block_position positions[32]; /* +0x0003: DFU BlockPositions */
    struct building buildings[32];  /* +0x0283: DFU BuildingDataList */
    struct block *block_data[32];   /* +0x05C3: DFU Section2UnknownData: rmb_index_records points
                                       it at each building's subrecords */
    int block_data_sizes[32];       /* +0x0643: DFU BlockDataSizes */
    struct block_model *misc_models; /* +0x06C3: DFU GroundData's 8-byte header: rmb_index_records
                                       points these at the misc 3D objects and flats */
    struct block_flat *misc_flats;  /* +0x06C7 */
    unsigned char ground_tiles[256]; /* +0x06CB: DFU GroundTiles, 16 x 16 (town_block_apply_ground) */
    unsigned char ground_scenery[256]; /* +0x07CB: DFU GroundScenery, 16 x 16 */
    unsigned char automap[4096];    /* +0x08CB: DFU AutoMapData, 64 x 64 (town_map_add_block) */
    char name[13];                  /* +0x18CB: DFU Name; the 429 bytes of names are copied to
                                       the town block object's (38) data */
    char other_names[32][13];       /* +0x18D8: DFU OtherNames, by building */
    char data[1];                   /* +0x1A78: the subrecords, then the misc objects */
};
RECORD_OFFSET_P(rmb_file, data, 0x1A78);

/* a model name of an RDB file (DFU RdbModelReference, 8 bytes: "55000DOR"; rdb_model_id_from_name
 * reads the digits) */
struct rdb_model_reference {
    char model_id[5];               /* +0x00 */
    char description[3];            /* +0x05 */
};                                  /* +0x08 */
RECORD_SIZE(rdb_model_reference, 8);

/* an RDB file's object section header (DFU RdbObjectHeader, 512 bytes) */
struct rdb_object_header {
    int unknown_offset;             /* +0x000: DFU UnknownOffset: a list of struct rdb_unknown_entry */
    int unknown1;                   /* +0x004 */
    int unknown2;                   /* +0x008 */
    int unknown3;                   /* +0x00C */
    int length;                     /* +0x010: of the RDB record */
    char unknown4[32];              /* +0x014 */
    char dagr[4];                   /* +0x034: "DAGR" */
    char unknown5[456];             /* +0x038 */
};                                  /* +0x200 */
RECORD_SIZE(rdb_object_header, 512);

/* the head of an RDB file (DFU RdbBlockHeader, the model lists and RdbObjectHeader):
 * dungeon_load_rdb_block reads the file into scratch_buffer (rdb_data, rdb_loaded_file) */
struct rdb_file {
    int unknown1;                   /* +0x0000 */
    int width;                      /* +0x0004: of the object root grid */
    int height;                     /* +0x0008 */
    int object_root_offset;         /* +0x000C: width x height offsets of object lists (0 none) */
    int unknown2;                   /* +0x0010 */
    struct rdb_model_reference model_references[750]; /* +0x0014: rdb_model.model_index */
    unsigned int model_data[750];   /* +0x1784: DFU ModelDataList */
    struct rdb_object_header object_header; /* +0x233C */
};                                  /* +0x253C */
RECORD_SIZE(rdb_file, 9532);

/* an entry of the list at rdb_object_header.unknown_offset (DFU does not read it): for a
 * model whose action_offset is negative, rdb_model_find_action takes the action offset,
 * trigger and sound of the entry whose key is 0 */
struct rdb_unknown_entry {
    int next;                       /* +0x00: offset from the start of the file */
    short key;                      /* +0x04 */
    int action_offset;              /* +0x06 */
    short trigger_flag_starting_lock; /* +0x0A */
    unsigned char sound_index;      /* +0x0C */
};                                  /* +0x0D */
RECORD_SIZE(rdb_unknown_entry, 13);

/* an object of an RDB object list (DFU RdbObject, 25 bytes) */
struct rdb_object {
    int next;                       /* +0x00: offset from the start of the file; 0 or less ends */
    int previous;                   /* +0x04 */
    int x;                          /* +0x08: from the block's corner */
    int y;                          /* +0x0C */
    int z;                          /* +0x10 */
    unsigned char type:6;           /* +0x14: DFU Type (RdbResourceTypes): 1 model (struct
                                       rdb_model), 2 light (rdb_light), 3 flat (rdb_flat) */
    unsigned char type_bits:2;      /*        the code masks them off */
    int resource_offset;            /* +0x15 */
};                                  /* +0x19 */
RECORD_SIZE(rdb_object, 25);

/* an RDB model resource (DFU RdbModelResource, 23 bytes) */
struct rdb_model {
    int x_rotation;                 /* +0x00 */
    int y_rotation;                 /* +0x04 */
    int z_rotation;                 /* +0x08 */
    short model_index;              /* +0x0C: into rdb_file.model_references */
    short trigger_flag_starting_lock; /* +0x0E: the low half of DFU's u32: the link's trigger (low
                                       byte); doors: the lock (>> 4) */
    short pad10;                    /* +0x10: its high half */
    unsigned char sound_index;      /* +0x12: the link's param */
    int action_offset;              /* +0x13: its struct rdb_action, 0 none; negative:
                                       rdb_model_find_action */
};                                  /* +0x17 */
RECORD_SIZE(rdb_model, 23);

/* a model's action (DFU RdbActionResource, 10 bytes): a link record (struct link) */
struct rdb_action {
    unsigned char axis;             /* +0x00 */
    unsigned short duration;        /* +0x01 */
    unsigned short magnitude;       /* +0x03 */
    int next_object_offset;         /* +0x05: the object it activates next, 0 or less none */
    unsigned char action;           /* +0x09: DFU Flags (RdbActionFlags) */
};                                  /* +0x0A */
RECORD_SIZE(rdb_action, 10);

/* an RDB light resource (DFU RdbLightResource, 10 bytes); in an action chain the code reads an
 * action at +0x03 and the next object at +0x04 (rdb_build_action_chain) */
struct rdb_light {
    short image;                    /* +0x00: DFU Unknown1's low half: the light object's image */
    char pad02;                     /* +0x02 */
    unsigned char action;           /* +0x03: Unknown1's high byte */
    int next_object_offset;         /* +0x04: DFU Unknown2 */
    short radius;                   /* +0x08 */
};                                  /* +0x0A */
RECORD_SIZE(rdb_light, 10);

/* an RDB flat resource (DFU RdbFlatResource, 11 bytes) */
struct rdb_flat {
    unsigned short image;           /* +0x00: archive<<7 | record (DFU TextureBitfield); 199<<7
                                       editor markers */
    short flags;                    /* +0x02: DFU Flags: the link's trigger (low byte); markers: the
                                       creature or the link flag; NPCs: 16, 32 the gender bits */
    unsigned char magnitude;        /* +0x04: the link's axis; markers: the creature; NPCs: the
                                       faction's low byte */
    unsigned char sound_index;      /* +0x05: the link's param; markers: the trigger range; NPCs:
                                       the faction's high byte */
    int next_object_offset;         /* +0x06: in an action chain */
    unsigned char action;           /* +0x0A: DFU Action; 29 an NPC */
};                                  /* +0x0B */
RECORD_SIZE(rdb_flat, 11);

/* ---- the record --------------------------------------------------------------------------- */

/* a record's data, by record type */
union record_data {
    struct character character;     /* 3 the player's entity, 18 creatures */
    struct monster monster;         /* 18 creatures */
    struct item item;               /* 2 items, 54 items in repair */
    struct spell spell;             /* 9 spells */
    struct membership membership;   /* 10 guild memberships */
    struct disease disease;         /* 11 diseases and poisons */
    struct career career;           /* 28 a saved class (vampires, lycanthropes) */
    struct potion_recipe potion_recipe; /* 31 */
    struct location location;       /* 1 the location */
    struct quest quest;             /* 14 a quest: the QBN file */
    struct settings settings;       /* 23 options */
    struct logbook logbook;         /* 24 */
    struct bank_account bank_accounts[62]; /* 25 one per region */
    struct person person;           /* 8 NPCs */
    struct blessing blessing;       /* 30 */
    struct building building;       /* 40 quest places, 41 quest NPCs, 64 stored buildings */
    struct quest_npc quest_npc;     /* 41 */
    struct block block;             /* 43 RMB blocks */
    struct automap automap;         /* 51 */
    struct model_instance instance; /* 6 models, 32 doors */
};

/* ---- the header ------------------------------------------------------------------------ */

struct record {
    unsigned char type;             /* +0x00: 2 item, 3 character, 9 spell, 11 effect, ... */
    union {
        short angle_x;              /* +0x01: the pitch */
        short draw_frame;           /* +0x01: flats, items, creatures, NPCs, loot (2, 8, 18, 33, 34,
                                       44, 53): frame_counter when last drawn (object_draw_cb) */
    };
    short yaw;                      /* +0x03: 2048 to a turn, clockwise from north */
    short angle_z;                  /* +0x05 */
    int x;                          /* +0x07: 1/40 m east */
    int y;                          /* +0x0B: 1/40 m, height */
    int z;                          /* +0x0F: 1/40 m north */
    union {
        unsigned short pad13;       /* +0x13: 8000 flats, lights and doors; 32768 RMB blocks (43) and
                                       town blocks (38) */
        unsigned short mobile_id;   /* +0x13: markers (34): the creature to spawn (a monster table index,
                                       then its id; bit 7 no water check); corpses (44): the creature's */
        unsigned short anim_time;   /* +0x13: animated flats: the frame time (ticks >> 5) */
        unsigned char block_special; /* +0x13: dungeon block quarters (47): the start marker's special
                                       flag (castle music, no ambient sound) */
    };
    unsigned short flags;           /* +0x15: 0x20 not owned, 0x02 not listed */
    union {
        unsigned short pad17;       /* +0x17 */
        unsigned short owner;       /* +0x17: shop items: the shop; quest places (40): the quest id;
                                       tavern room markers: the room; type 57: the building count */
        unsigned short lock_level;  /* +0x17: doors (type 32) */
        unsigned short missile_texture; /* +0x17: spell missiles (9): spell_missile_textures[element],
                                       bit 0 once it has hit */
        unsigned short light_radius; /* +0x17: lights (7): 64, << 2 on a missile's impact */
        unsigned short missile_yaw; /* +0x17: arrows in flight (2): the heading of the velocity */
        unsigned short flat_count;  /* +0x17: type 56: the 17-byte flats after its models */
        unsigned short trigger_range; /* +0x17: markers: 0-6 (place_marker_in_range) */
        unsigned short light_level; /* +0x17: dungeon block quarters (47) (sky_update) */
        unsigned short building_type; /* +0x17: building objects: the building's type */
        unsigned short detect_distance; /* +0x17: creatures: Detect's distance */
        unsigned short anim_frame;  /* +0x17: animated flats: the frame counter */
        unsigned short home_region; /* +0x17: quest NPCs (41, 65): the region (the name set) */
        unsigned short home_building; /* +0x17: pedestrians (53): a building index (pedestrian_place) */
    };
    union {
        short pad19;                /* +0x19: a new loot pile gets 1 */
        unsigned short lockpick_skill_tried; /* +0x19: doors */
        short faction_id;           /* +0x19: quest NPCs (41, 65): qbn_person.faction_id */
        short region;               /* +0x19: quest places (40) */
        unsigned short npc_flags;   /* +0x19: pedestrians (53): 0x4000 set before every pickpocket
                                       attempt (click_pedestrian), 0x8000 a failed pickpocket */
        short water_level;          /* +0x19: dungeon block quarters (47): 10000 none */
        unsigned short spawn_seed;  /* +0x19: markers: the spawn's srand() seed; corpses get it back */
        short from_player;          /* +0x19: arrows in flight (2): 1 fired by the player */
    };
    union {
        unsigned short pad1B;       /* +0x1B */
        unsigned short image;       /* +0x1B: archive<<7 | record */
        unsigned short soul_creature; /* +0x1B: trapped souls (20): the creature (monster) id */
        unsigned short trap_chance; /* +0x1B: soul-trap effects (19): the chance (cast_chances) */
        unsigned short model_count; /* +0x1B: type 56: its 66-byte RMB models */
        unsigned short building_index; /* +0x1B: building objects, building markers (43), stored
                                       buildings (64): index into current_location->buildings */
        unsigned short block_number; /* +0x1B: dungeon block quarters (47): the RDB block number */
        unsigned short shelf_index; /* +0x1B: items on a shop shelf (36): the shelf model's index */
        unsigned short location_index; /* +0x1B: the location object: its index, 0xFFFF wilderness */
        unsigned short seen_count;  /* +0x1B: the automap (51): bytes of seen bits */
        unsigned short container_index; /* +0x1B: item containers (52): their index in
                                       inventory_containers (0-3 the inventory tabs, 4 the wagon,
                                       5 house, 6 ship, 7 room storage, 8 repairs) */
    };
    union {
        unsigned short pad1D;       /* +0x1D */
        unsigned short image2;      /* +0x1D: DFU Picture2; 3D objects: the model id's hundreds */
        unsigned short trap_duration; /* +0x1D: soul-trap effects (19): rounds (cast_durations) */
    };
    unsigned int id;                /* +0x1F */
    unsigned char link_flag;        /* +0x23 */
    union {
        short pad24;                /* +0x24 */
        short wait_state;           /* +0x24: creatures (monster+0x24): 99 asleep until hurt */
        short door_angle;           /* +0x24: doors (32): 0..512, set by doors_update */
    };
    unsigned char quest_id;         /* +0x26 */
    unsigned int parent_id;         /* +0x27: in the save file */
    union {
        unsigned int pad2B;         /* +0x2B */
        unsigned int repair_due;    /* +0x2B: items in repair (54): game_minutes when ready; a rented
                                       room's loot pile (58): the rent's end */
        unsigned int expire_minutes; /* +0x2B: spell-created and conjured items: game_minutes when
                                       they vanish */
        int name_seed;              /* +0x2B: NPCs (8, 53), quest NPCs and foes: the name seed */
        unsigned int door_swing;    /* +0x2B: doors (32): bits 0-29 the BIOS tick the swing began,
                                       bit 30 swinging, bit 31 open */
        unsigned int building_id;   /* +0x2B: stored buildings (64): the building's id; quest places
                                       and NPCs (40, 41, 65): a copy of their id */
        unsigned int created_minutes; /* +0x2B: the automap (51): expires 43200 minutes after */
        unsigned int move_frame;    /* +0x2B: models (6): frame_counter when a link last moved it */
        unsigned int move_remainder; /* +0x2B: pedestrians (53): the x and z fractions of the last
                                       move (bytes 0 and 1) */
        unsigned int monster_arrow; /* +0x2B: arrows in flight (2): 1 when a creature fired it
                                       (weapon_monster_arrow; no reader found) */
    };
    union {
        unsigned int pad2F;         /* +0x2F */
        struct record *caster;      /* +0x2F: spells (type 9) */
        int draw_handle;            /* +0x2F: other drawn objects: the XnGine draw handle
                                       (object_draw_cb, automap_draw_object_cb, pick_sprite_cb) */
        unsigned int home_id;       /* +0x2F: items in repair (54), room loot piles (58): the id of
                                       the object to go back to */
    };
    struct record *twin;            /* +0x33 */
    struct record *next;            /* +0x37 */
    struct record *prev;            /* +0x3B */
    struct record *children;        /* +0x3F */
    struct record *parent;          /* +0x43 */
    union record_data data;         /* +0x47: the record's data */
};
RECORD_OFFSET_P(record, data, 0x47);

/* Offsets and sizes the code once wrote as numbers (docs/port.md): as int constants, as the
   numbers were, so the code that uses them does not change under Watcom; natively they follow
   the native layout (pointers are 8 bytes there). */
#if defined(DAGGER_PORT)
#define REC_OFFSETOF(type, member) ((int)__builtin_offsetof(type, member))
#else
#define REC_OFFSETOF(type, member) ((int)&((type *)0)->member)
#endif
#define REC_SIZEOF(type) ((int)sizeof(type))

/* the record header: its size (71 under Watcom), and where its links start (55) */
#define RECORD_HEADER_SIZE REC_OFFSETOF(struct record, data)
#define RECORD_LINKS_OFFSET REC_OFFSETOF(struct record, next)

/* the data of a record as a char pointer: `(char *)r + 71`; and back */
#define RECORD_DATA(r) ((char *)(r) + RECORD_HEADER_SIZE)
#define RECORD_FROM_DATA(p) ((struct record *)((char *)(p) - RECORD_HEADER_SIZE))
/* the four links next, prev, children, parent (16 bytes under Watcom) */
#define RECORD_LINKS_SIZE (RECORD_HEADER_SIZE - RECORD_LINKS_OFFSET)

/* the header of a block of the memory pools (18 under Watcom; the data follows it) */
#define MEM_BLOCK_HEADER_SIZE REC_SIZEOF(struct mem_block)

/* a capacity budgeted for 32-bit records and headers (the object heap): natively the records
   are larger (95-byte headers, 26-byte blocks, 8-byte pointers in the data), so 1.5 times */
#if defined(DAGGER_PORT)
#define NATIVE_HEAP_BYTES(n) ((n) + (n) / 2)
#else
#define NATIVE_HEAP_BYTES(n) (n)
#endif

/* the slot of a table of pointers that holds a value, 0 none (the four inventory containers,
   a character's equipped[27]): under Watcom the engine's 32-bit search, as the game called it;
   natively the slots are 8 bytes */
#if defined(DAGGER_PORT)
static __inline__ void *ptr_table_find(void *table, void *value, unsigned count)
{
    void **slot = (void **)table;

    for (; count != 0; count--, slot++)
        if (*slot == value)
            return slot;
    return 0;
}
#define PTR_TABLE_FIND(table, value, count) \
    ptr_table_find((void *)(table), (void *)(uptr)(value), (count))
#else
#define PTR_TABLE_FIND(table, value, count) \
    xn_str_find_u32((unsigned *)(table), (uptr)(value), (count))
#endif

/* the same search where the file declares xn_str_find_u32 with its own types: under Watcom
   the arguments go as they are, so the call is the original */
#if defined(DAGGER_PORT)
#define PTR_TABLE_FIND_ARGS(table, value, count) \
    ptr_table_find((void *)(table), (void *)(uptr)(value), (count))
#else
#define PTR_TABLE_FIND_ARGS(table, value, count) xn_str_find_u32(table, value, count)
#endif

/* ---- other structures (not records) --------------------------------------------------------- */

/* a rumor (RUMOR.DAT): the 34-byte header, then text_length bytes of text */
struct rumor {
    short faction1;                 /* +0x00: faction ids (rumor_add_faction, rumor_is_eligible) */
    short faction2;                 /* +0x02 */
    int kind;                       /* +0x04: the politics event; 100 not a politics rumor */
    unsigned char region;           /* +0x08: rumor_collect_local matches current_region */
    unsigned char flags;            /* +0x09: 1 regional, 2 quest NPC, 4 quest, 8 faction, 0x20 dropped */
    unsigned char quest_id;         /* +0x0A */
    char quest_name[9];             /* +0x0B */
    short message;                  /* +0x14: the quest message */
    int target;                     /* +0x16: an object id (flag 2; rumor_find_for_npc) */
    int text_length;                /* +0x1A */
    unsigned int expires;           /* +0x1E: game_minutes */
};                                  /* +0x22 */
RECORD_SIZE(rumor, 34);

/* the MAPS location record being loaded (the global loaded_location, 0x196A88) */
struct loaded_location {
    int index;                      /* +0x00 */
    int door_count;                 /* +0x04 */
    struct location_door *doors;    /* +0x08: door_count of them (location_find_door) */
    struct record *object;          /* +0x0C: the header read from MAPS, then the location (malloc 119) */
    struct location *data;          /* +0x10: object + 0x47 */
};                                  /* +0x14 */
RECORD_SIZE_P(loaded_location, 20);

/* the header of a block of the game's memory pools (jmem.c); the data follows */
struct mem_block {
    unsigned int magic;             /* +0x00: 0x69696969 in a used block */
    struct mem_block *next;         /* +0x04 */
    struct mem_block *prev;         /* +0x08 */
    unsigned int size;              /* +0x0C: the data's size */
    unsigned short flags;           /* +0x10: bit 0 used */
};                                  /* +0x12 */
RECORD_SIZE_P(mem_block, 18);

/* a memory pool (mem_pool_init, mem_pool_alloc, mem_pool_free) */
struct mem_pool {
    int pad00;                      /* +0x00: cleared by mem_pool_init */
    struct mem_block *first;        /* +0x04: the malloc'd area, one free block at first */
    int pad08;                      /* +0x08 */
    int size;                       /* +0x0C */
};                                  /* +0x10 */
RECORD_SIZE_P(mem_pool, 16);

/* an action link of a dungeon object (an RDB action record, as loaded; links.c) */
struct link {
    unsigned short object_id;       /* +0x00: the low word of the object's record id */
    unsigned char trigger;          /* +0x02: DFU RdbTriggerFlags (1 collision, 2 click, 5 attack, ...) */
    unsigned char param;            /* +0x03: the sound; the text/spell/sound of actions 9, 11, 12, 30, 99 */
    unsigned char axis;             /* +0x04: 1-6 +x -x +y -y +z -z; a second parameter for others */
    short duration;                 /* +0x05: ticks */
    short magnitude;                /* +0x07: distance or angle */
    unsigned char action;           /* +0x09: DFU RdbActionFlags (1 translate, 8 rotate, ...) */
    unsigned char chain_count;      /* +0x0A: records that follow in the same chain */
    unsigned char combination;      /* +0x0B: action 129: the low nibble collects the switches' bits,
                                       done when it equals the high nibble */
    unsigned char flags;            /* +0x0C: 1 finished, 2 reversed, 4 sound played, 16 stop, 32 axis */
    int start;                      /* +0x0D: the start position or angle */
    int speed;                      /* +0x11: 16.16 per tick */
    char pad15[4];                  /* +0x15 */
    int start_tick;                 /* +0x19: BIOS tick */
    short delta[3];                 /* +0x1D: the object's last move */
    struct record *object;          /* +0x23: the moved object (a record id in the save) */
};                                  /* +0x27 */
RECORD_SIZE_P(link, 39);

/* a house for sale at the bank (bank_houses_for_sale[20], bank_add_house_for_sale) */
struct house_for_sale {
    struct block *block;            /* +0x00: the building marker's data (a type-43 object) */
    struct building *building;      /* +0x04: its entry in current_location->buildings */
    int price;                      /* +0x08 */
    unsigned int id;                /* +0x0C: the object's id (player_character->house when bought) */
    int saved_yaw;                  /* +0x10: block->models[0].yaw, put back by bank_restore_houses */
};                                  /* +0x14 */
RECORD_SIZE_P(house_for_sale, 20);

/* a ship for sale at the bank (bank_ships_for_sale[2], bank_init_ships): a model and its price;
 * bank_draw_preview draws it as a one-model block */
struct ship_for_sale {
    struct block_model model;       /* +0x00: model 415 */
    unsigned int id;                /* +0x42: the ship's id (0x3E00001, 0x3E10001: ship_owned) */
    int price;                      /* +0x46 */
};                                  /* +0x4A */
RECORD_SIZE_P(ship_for_sale, 74);

/* a node of the model cache's binary tree (objlib.c) */
struct model_node {
    struct model_node *left;        /* +0x00 */
    struct model_node *right;       /* +0x04 */
    int last_frame;                 /* +0x08: frame_counter when last used */
    int key;                        /* +0x0C: model id + variant << 17 */
    char *model;                    /* +0x10: the ARCH3D record */
};                                  /* +0x14 */
RECORD_SIZE_P(model_node, 20);

/* what the collision code is asked to move (colstuff.c) */
struct move_request {
    int x;                          /* +0x00 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int angle_x;                    /* +0x0C */
    int yaw;                        /* +0x10 */
    int angle_z;                    /* +0x14 */
    struct collide_probe *probe;    /* +0x18: the mover's shape (D_00196D4C while it is tested) */
    unsigned short flags;           /* +0x1C: bit 0 (func_00023FA5) */
};                                  /* +0x1E */
RECORD_SIZE_P(move_request, 30);

#pragma pack()

/* the shared structures that are not records */
#include "structs.h"

#endif
