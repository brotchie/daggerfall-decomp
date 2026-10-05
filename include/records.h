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
 * tools/offset_casts.py checks every offset comment against the layout. */
#ifndef RECORDS_H
#define RECORDS_H

/* a compile-time check: the build fails when a struct has the wrong size */
#define RECORD_SIZE(tag, n) typedef char tag##_size_check[(sizeof(struct tag) == (n)) ? 1 : -1]
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[((unsigned)&((struct tag *)0)->m == (n)) ? 1 : -1]

#pragma pack(1)

struct record;

/* ---- indexes ----------------------------------------------------------------------------- */

/* character attributes[] and base_attributes[], career attributes[] */
enum { ATTR_STR, ATTR_INT, ATTR_WIL, ATTR_AGI, ATTR_END, ATTR_PER, ATTR_SPD, ATTR_LUC };

/* character skills[] (DFU DFCareer.Skills order); career skills[] holds these ids */
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
    unsigned char pad08;                    /* +0x08 */
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
        int floor_y;                        /* +0x058: creatures (monster+0x9F) */
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
    short pad83;                            /* +0x083 */
    int gold;                               /* +0x085 */
    unsigned int conditions;                /* +0x089: a bit per active effect; byte 1 is equip_effect_flags */
    short magicka;                          /* +0x08D */
    short max_magicka;                      /* +0x08F */
    short reputation[5];                    /* +0x091: commoners, merchants, scholars, nobility, underworld */
    unsigned short fatigue;                 /* +0x09B: x 64 */
    struct character_skill skills[35];      /* +0x09D: SKILL_* */
    struct record *equipped[27];            /* +0x16F: EQUIP_*; right_hand [19], left_hand [21] */
    unsigned short table_flags;             /* +0x1DB: creatures (monster+0x222) */
    unsigned short attack_damage[5][2];     /* +0x1DD: creatures (monster+0x224): {min, max} */
    unsigned char pad1F1;                   /* +0x1F1 */
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
    unsigned char pad223;                   /* +0x223 */
    unsigned char pad224;                   /* +0x224: chargen's RR effect */
    char pad225[2];                         /* +0x225 */
    short faction_id;                       /* +0x227: NPCs */
    unsigned char team;                     /* +0x229 */
    unsigned char pad22A;                   /* +0x22A */
    unsigned char resist_chances[5];        /* +0x22B: by element */
    struct career career;                   /* +0x230: names.csv class; player_class points here */
};                                          /* +0x27A */
RECORD_SIZE(character, 634);

/* the creature's animation struct (monster+0x2C1), driven by XnGine's ASCR interpreter */
struct monster_anim {
    short anim_frame;               /* +0x00 */
    short pad02;                    /* +0x02 */
    char *anim_script;              /* +0x04: the ASCR record */
    char *anim_script_pos;          /* +0x08 */
    short pad0C;                    /* +0x0C */
    short timer;                    /* +0x0E */
    unsigned char anim_events;      /* +0x10: bit 0 strike, bit 1 missile */
    unsigned char anim_flags;       /* +0x11: bit 7 mirrored */
    char pad12[2];                  /* +0x12 */
    unsigned char anim_request;     /* +0x14: 255 none */
    unsigned char anim_facing;      /* +0x15 */
    unsigned char anim_record;      /* +0x16 */
    unsigned char anim_current;     /* +0x17 */
    unsigned char pad18;            /* +0x18 */
};                                  /* +0x19 */
RECORD_SIZE(monster_anim, 25);

/* the data of a type-18 creature: names.csv `monster+` offsets are from the record, 0x47 more */
struct monster {
    struct character character;     /* +0x000 */
    struct monster_anim anim;       /* +0x27A: monster+0x2C1 */
};                                  /* +0x293 */
RECORD_SIZE(monster, 659);

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
    unsigned char pad30;            /* +0x30 */
    unsigned char stack_count;      /* +0x31: also a potion's recipe index */
    unsigned short inventory_image; /* +0x32 */
    unsigned short dropped_image;   /* +0x34 */
    unsigned char material;         /* +0x36 */
    unsigned char armor_type;       /* +0x37 */
    unsigned char color;            /* +0x38 */
    unsigned int weight;            /* +0x39 */
    unsigned short enchant_points;  /* +0x3D */
    unsigned int message;           /* +0x3F: book id, letter */
    struct enchantment enchantments[10]; /* +0x43 */
};                                  /* +0x6B */
RECORD_SIZE(item, 107);

/* ---- spells: type 9, 89 bytes, the SPELLS.STD format ------------------------------------- */

struct spell_effect {
    unsigned char type;             /* +0x00: 255 none */
    unsigned char subtype;          /* +0x01 */
};

struct spell_range {                /* durations and chances */
    unsigned char base;             /* +0x00 */
    unsigned char plus;             /* +0x01 */
    unsigned char per_level;        /* +0x02 */
};

struct spell_magnitude {
    unsigned char base_min;         /* +0x00 */
    unsigned char base_max;         /* +0x01 */
    unsigned char plus_min;         /* +0x02 */
    unsigned char plus_max;         /* +0x03 */
    unsigned char per_level;        /* +0x04 */
};

struct spell {
    struct spell_effect effects[3]; /* +0x00 */
    unsigned char element;          /* +0x06: 0 fire, 1 frost, 2 poison, 3 shock, 4 magic */
    unsigned char target;           /* +0x07: 0 caster, 1 touch, 2 distance, 3/4 areas */
    unsigned short effect_costs[3]; /* +0x08 */
    struct spell_range durations[3]; /* +0x0E */
    struct spell_range chances[3];  /* +0x17 */
    struct spell_magnitude magnitudes[3]; /* +0x20 */
    char name[25];                  /* +0x2F */
    unsigned char icon;             /* +0x48: 200+slot item spell, 250 item/potion/strike */
    unsigned char id;               /* +0x49 */
    unsigned short cast_durations[3]; /* +0x4A: filled in at cast time */
    unsigned short cast_magnitudes[3]; /* +0x50 */
    unsigned char cast_chances[3];  /* +0x56 */
};                                  /* +0x59 */
RECORD_SIZE(spell, 89);

/* ---- diseases and poisons: type 11, 47 bytes (disease.c's table D_00186A64) -------------- */

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
    unsigned char ingredient_groups[8]; /* +0x0A */
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
RECORD_SIZE(faction, 92);

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
    unsigned int x_type_flags;      /* +0x04: x 0-24, type 25-29, discovered 30, hidden 31 */
    unsigned int y_size;            /* +0x08: z 0-23, area width/height 24-31 */
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
RECORD_SIZE(location, 48);

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
    unsigned char view_flags;       /* +0x00: bit 0 full screen, bit 1 head bobbing */
    unsigned char detail_level;     /* +0x01 */
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
RECORD_SIZE(picklist, 0x3B);

/* ---- quests: the data of a type-14 record is the QBN file, pointers relocated ------------ */

struct quest {
    short id;                       /* +0x00 */
    short faction_id;               /* +0x02 */
    short text_file;                /* +0x04 */
    char name[9];                   /* +0x06 */
    unsigned char flags;            /* +0x0F: bit 1 rewarded */
    short section_counts[10];       /* +0x10 */
    short section_offsets[11];      /* +0x24: from the quest start; 10 = text variables */
};                                  /* +0x3A */
RECORD_SIZE(quest, 0x3A);

struct qbn_arg {
    unsigned char negate;           /* +0x00 */
    char *record;                   /* +0x01 */
    short section;                  /* +0x05 */
    int value;                      /* +0x07: -1 none, -2 filler */
    struct record *object;          /* +0x0B */
};                                  /* +0x0F */
RECORD_SIZE(qbn_arg, 15);

struct qbn_op {                     /* section 8 */
    short opcode;                   /* +0x00 */
    unsigned short flags;           /* +0x02: bit 0 done */
    short arg_count;                /* +0x04 */
    struct qbn_arg args[5];         /* +0x06: arg 0 is the state */
    short message;                  /* +0x51 */
    int last_minutes;               /* +0x53 */
};                                  /* +0x57 */
RECORD_SIZE(qbn_op, 87);

struct qbn_state {                  /* section 9 */
    short index;                    /* +0x00 */
    unsigned char is_global;        /* +0x02 */
    unsigned char value;            /* +0x03: or the global index */
    int name_hash;                  /* +0x04 */
};                                  /* +0x08 */
RECORD_SIZE(qbn_state, 8);

struct qbn_timer {                  /* section 6 */
    short pad00;                    /* +0x00 */
    unsigned short flags;           /* +0x02: 64 running, 128 expired, 0x400 links resolved */
    unsigned char type;             /* +0x04 */
    int minimum;                    /* +0x05 */
    int maximum;                    /* +0x09 */
    int start;                      /* +0x0D */
    int delay;                      /* +0x11 */
    int link1;                      /* +0x15 */
    int link2;                      /* +0x19 */
    int state_hash;                 /* +0x1D */
};                                  /* +0x21 */
RECORD_SIZE(qbn_timer, 33);

struct qbn_item {                   /* section 0 */
    char pad00[2];                  /* +0x00 */
    unsigned char flags;            /* +0x02: bit 7 topic hidden */
    char pad03[8];                  /* +0x03 */
    struct record *object;          /* +0x0B */
    short messages[2];              /* +0x0F */
};                                  /* +0x13 */
RECORD_SIZE(qbn_item, 19);

struct qbn_person {                 /* section 3 */
    char pad00[3];                  /* +0x00 */
    unsigned char flags;            /* +0x03 */
    short kind;                     /* +0x04 */
    short faction_id;               /* +0x06 */
    char pad08[4];                  /* +0x08 */
    struct record *object;          /* +0x0C */
    short messages[2];              /* +0x10 */
};                                  /* +0x14 */
RECORD_SIZE(qbn_person, 20);

struct qbn_place {                  /* section 4 */
    char pad00[2];                  /* +0x00 */
    unsigned char flags;            /* +0x02 */
    char pad03[13];                 /* +0x03 */
    struct record *object;          /* +0x10 */
    short messages[2];              /* +0x14 */
};                                  /* +0x18 */
RECORD_SIZE(qbn_place, 24);

struct qbn_foe {                    /* section 7 */
    char pad00[3];                  /* +0x00 */
    unsigned char type;             /* +0x03 */
    short count;                    /* +0x04 */
    char pad06[4];                  /* +0x06 */
    struct record *object;          /* +0x0A */
};                                  /* +0x0E */
RECORD_SIZE(qbn_foe, 14);

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
    struct bank_account bank_accounts[62]; /* 25 one per region */
};

/* ---- the header ------------------------------------------------------------------------ */

struct record {
    unsigned char type;             /* +0x00: 2 item, 3 character, 9 spell, 11 effect, ... */
    short angle_x;                  /* +0x01 */
    short yaw;                      /* +0x03: 2048 to a turn, clockwise from north */
    short angle_z;                  /* +0x05 */
    int x;                          /* +0x07: 1/40 m east */
    int y;                          /* +0x0B: 1/40 m, height */
    int z;                          /* +0x0F: 1/40 m north */
    char pad13[2];                  /* +0x13 */
    unsigned short flags;           /* +0x15: names.csv object_flags; 0x20 not owned, 0x02 not listed */
    union {
        unsigned short owner;       /* +0x17: a shop item's owner */
        unsigned short lock_level;  /* +0x17: doors (type 32) */
    };
    union {
        short pad19;                /* +0x19: a new loot pile gets 1 */
        unsigned short lockpick_skill_tried; /* +0x19: doors */
    };
    unsigned short image;           /* +0x1B: names.csv world_image; archive<<7 | record */
    unsigned short image2;          /* +0x1D */
    unsigned int id;                /* +0x1F: names.csv record_id */
    unsigned char link_flag;        /* +0x23 */
    short wait_state;               /* +0x24: creatures (monster+0x24): 99 asleep until hurt */
    unsigned char quest_id;         /* +0x26 */
    unsigned int parent_id;         /* +0x27: in the save file */
    unsigned int repair_due;        /* +0x2B: items in repair (type 54) */
    struct record *caster;          /* +0x2F: spells (type 9) */
    struct record *twin;            /* +0x33 */
    struct record *next;            /* +0x37 */
    struct record *prev;            /* +0x3B */
    struct record *children;        /* +0x3F */
    struct record *parent;          /* +0x43 */
    union record_data data;         /* +0x47: the record's data */
};
RECORD_OFFSET(record, data, 0x47);

/* the data of a record as a char pointer: `(char *)r + 71` */
#define RECORD_DATA(r) ((char *)(r) + 0x47)

#pragma pack()

#endif
