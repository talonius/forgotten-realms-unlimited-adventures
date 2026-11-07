/* CKIT.EXE data-only module between approach.c and newgame.c: the combat,
 * class and effect tables, DGROUP 5fd4..6b75 -- the direction steps, the combat
 * terrain types (tile_info) and icon shapes, effect lists, the attack scores,
 * class bits and hit dice, the thief skills, the effect names (fx_names) and
 * bit_mask, then eleven small game variables mapevent.c, evcmds.c and their
 * neighbours share (cur_level .. events_running).  The string literals follow,
 * DGROUP 6728..6b75, as Turbo C emits them; fx_names' empty entries share the
 * first, "".  Dark Queen of Krynn's combtab.c with FRUA's six races, its own
 * effect names and without DQK's trailing effect table.
 *
 * Divisions follow the readers, else DQK's: fx_clear's 17 effects, then nine
 * keypad words; class_bit's seven classes, then nine bytes; hit_die_sides'
 * seven classes, then six.  dir_dx and dir_dy are indexed by facing, 8 meaning
 * none.  Nothing in the image reads byte_5fd4, digit_keypad_keys,
 * class_bit_tail, icon_color_slots or byte_6374.  5fd3, after approach.c's
 * _DATA, is the link's word-alignment pad.  No routines; its empty _TEXT sits
 * at newgame.c's start in section 6.
 */

struct TileInfo {               /* a combat terrain type, 4 bytes         */
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    unsigned char height;       /* +1                                     */
    unsigned char block;        /* +2   a line lower than this is stopped */
    char kind;                  /* +3   glyph; 0x25 open ground           */
};

struct IconCell {               /* one cell of an icon, from its corner   */
    char dx;                    /* +0   -1: no more cells                 */
    char dy;                    /* +1                                     */
};

unsigned char byte_5fd4[15] = {                 /* 5fd4 */
    0, 0, 0, 0, 0, 0, 1, 0xff, 0, 0xff, 0, 0xff, 0, 19, 1
};

char dir_dx[9] = {                              /* 5fe3  by facing, 0 north, clockwise; 8 none */
    0, 1, 1, 1, 0, -1, -1, -1, 0
};

char dir_dy[9] = {                              /* 5fec */
    -1, -1, 0, 1, 1, 1, 0, -1, 0
};

struct TileInfo tile_info[76] = {               /* 5ff5 */
    { 0xff, 1, 2, 0 }, { 0xff, 1, 2, 1 }, { 0xff, 1, 2, 2 }, { 0xff, 1, 2, 3 }, /* 00 */
    { 1, 1, 0, 4 }, { 0xff, 1, 2, 5 }, { 0xff, 1, 2, 6 }, { 0xff, 1, 2, 7 }, /* 04 */
    { 1, 1, 0, 8 }, { 0xff, 1, 2, 9 }, { 1, 1, 0, 0xa }, { 0xff, 1, 2, 0xb }, /* 08 */
    { 1, 1, 0, 0xc }, { 0xff, 1, 2, 0xd }, { 1, 1, 0, 0xe }, { 0xff, 1, 2, 0xf }, /* 0c */
    { 1, 1, 0, 0x10 }, { 0xff, 1, 2, 0x11 }, { 0xff, 1, 2, 0x12 }, { 0xff, 1, 2, 0x13 }, /* 10 */
    { 0xff, 1, 2, 0x14 }, { 0xff, 1, 2, 0x15 }, { 1, 1, 0, 0x16 }, { 1, 1, 0, 0x17 }, /* 14 */
    { 0xff, 1, 2, 0x18 }, { 2, 2, 0, 0x26 }, { 1, 1, 0, 0xe }, { 1, 1, 0, 0xd }, /* 18 */
    { 1, 1, 0, 0xc }, { 1, 1, 0, 0x14 }, { 1, 1, 0, 0x15 }, { 0xff, 1, 2, 0 }, /* 1c */
    { 0xff, 1, 2, 1 }, { 0xff, 1, 2, 2 }, { 0xff, 1, 2, 3 }, { 0xff, 1, 2, 4 }, /* 20 */
    { 1, 1, 0, 5 }, { 1, 1, 0, 6 }, { 1, 1, 0, 7 }, { 1, 1, 0, 8 }, /* 24 */
    { 1, 1, 0, 9 }, { 0xff, 1, 2, 0xa }, { 0xff, 1, 2, 0xb }, { 2, 1, 0, 0xc }, /* 28 */
    { 1, 1, 0, 0xd }, { 1, 1, 0, 0xe }, { 1, 1, 0, 0xf }, { 1, 1, 0, 0x10 }, /* 2c */
    { 1, 1, 0, 0x11 }, { 2, 1, 0, 0x12 }, { 2, 1, 0, 0x13 }, { 1, 1, 0, 0x14 }, /* 30 */
    { 1, 1, 0, 0x15 }, { 1, 1, 0, 0x16 }, { 0xff, 1, 0, 0x17 }, { 0xff, 1, 0, 0x18 }, /* 34 */
    { 0xff, 1, 0, 0x19 }, { 1, 1, 0, 0x1a }, { 1, 1, 0, 0x1b }, { 1, 1, 0, 0x1c }, /* 38 */
    { 1, 0, 0, 0x1d }, { 1, 0, 0, 0x1e }, { 0xff, 0, 0, 0x1f }, { 0xff, 1, 0, 0x20 }, /* 3c */
    { 2, 1, 0, 0x21 }, { 1, 1, 0, 0x22 }, { 0xff, 1, 0, 0x23 }, { 1, 1, 0, 0x24 }, /* 40 */
    { 1, 1, 0, 0x25 }, { 1, 1, 0, 0x16 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, /* 44 */
    { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } /* 48 */
};

struct IconCell icon_cells[6][6] = {            /* 6125  by icon size */
    { { 0, 0 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
    { { 0, 0 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
    { { 0, 0 }, { 0, 1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
    { { 0, 0 }, { 1, 0 }, { -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 } },
    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 }, { -1, -1 }, { -1, -1 } },
    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 0 }, { 2, 1 } }
};

char helpless_kinds[4] = {                      /* 616d  effects that leave a hero helpless */
    0x1f, 0x33, 0x34, 0x35
};

unsigned char disease_fx[6] = {                 /* 6171 */
    0x1f, 0x22, 0x2b, 0xac, 0xd7, 0x2c
};

unsigned char fx_clear[17] = {                  /* 6177  effects a fight's end removes */
    3, 0xb, 0x15, 0x17, 0x1b, 0x23, 0x1f, 0x33, 0x34, 0x35, 0x78, 0x76, 0x6f, 0x6a, 0xb2, 0xb1,
    0x4d
};

/* The digits '1'..'9' as keypad keys (0x81 PgUp/NE, 0x82 Right, 0x83 PgDn,
 * 0x84 Down, 0x85 End, 0x86 Left, 0x87 Home, 0x88 Up; '5' a space), nine
 * little-endian words, the last cut to its low byte: DQK v1.0's modify.c read
 * them, and nothing here does. */
unsigned char digit_keypad_keys[17] = {         /* 6188 */
    0x85, 0x00, 0x84, 0x00, 0x83, 0x00, 0x86, 0x00, 0x20, 0x00, 0x82, 0x00, 0x87, 0x00, 0x88, 0x00,
    0x81
};

unsigned char patch_dir[10] = {                 /* 6199 */
    0, 8, 2, 3, 4, 5, 6, 7, 0, 1
};

unsigned time_units[7] = {                      /* 61a3 */
    10, 10, 6, 24, 30, 12, 100
};

/* 60 - THAC0 by class and level; larger values improve the attack. */
unsigned char attack_scores_by_level[7][22] = { /* 61b1 */
    { 40, 40, 40, 40, 42, 42, 42, 44, 44, 44, 46, 46, 46, 48, 48, 48, 50, 50, 50, 51, 51, 51 }, /* Cleric */
    { 39, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 56, 56, 56 }, /* Knight */
    { 39, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 56, 56, 56 }, /* Fighter */
    { 40, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 56, 56, 56 }, /* Paladin */
    { 40, 40, 40, 42, 42, 44, 44, 46, 46, 48, 48, 50, 50, 52, 52, 54, 54, 56, 56, 56, 56, 56 }, /* Ranger */
    { 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 44, 44, 44, 44, 44, 47, 47, 47, 47, 47, 49 }, /* Magic-User */
    { 40, 40, 40, 40, 40, 41, 41, 41, 41, 44, 44, 44, 44, 46, 46, 46, 46, 48, 48, 48, 48, 50 } /* Thief */
};

unsigned char class_mask_bits[7] = {            /* 624b */
    0x02, 0x10, 0x08, 0x40, 0x40, 0x01, 0x04
};

unsigned char class_bit[7] = {                  /* 6252 */
    0x02, 0x02, 0x08, 0x10, 0x20, 0x01, 0x04
};

/* Nine bytes after class_bit; DQK, DKK and Champions keep them too, unread. */
unsigned char class_bit_tail[9] = {             /* 6259 */
    0x10, 0x10, 0x10, 0x20, 0x20, 0x20, 0x40, 0x40, 0x40
};

unsigned char hit_dice_level_limit[7] = {       /* 6262 */
    10, 10, 10, 10, 11, 12, 11
};

unsigned char class_hit_dice[7] = {             /* 6269 */
    1, 2, 1, 1, 2, 1, 1
};

unsigned char hit_die_sides[7] = {              /* 6270 */
    8, 10, 10, 10, 8, 4, 6
};

/* DKK's iconColorSlots: the palette entry each icon colour repaints; nothing
 * reads it here. */
unsigned char icon_color_slots[6] = {           /* 6277 */
    1, 2, 3, 4, 6, 7
};

unsigned char thief_skill_base[18][8] = {       /* 627d */
    { 30, 25, 20, 15, 10, 10, 85, 0 },          /* level 1 */
    { 35, 29, 25, 21, 15, 10, 86, 0 },          /* level 2 */
    { 40, 33, 30, 27, 20, 15, 87, 0 },          /* level 3 */
    { 45, 37, 35, 33, 25, 15, 88, 20 },         /* level 4 */
    { 50, 42, 40, 40, 31, 20, 90, 25 },         /* level 5 */
    { 55, 47, 45, 47, 37, 20, 92, 30 },         /* level 6 */
    { 60, 52, 50, 55, 43, 25, 94, 35 },         /* level 7 */
    { 65, 57, 55, 62, 49, 25, 96, 40 },         /* level 8 */
    { 70, 62, 60, 70, 56, 30, 98, 45 },         /* level 9 */
    { 80, 67, 65, 78, 63, 30, 99, 50 },         /* level 10 */
    { 90, 72, 70, 86, 70, 35, 99, 55 },         /* level 11 */
    { 100, 77, 75, 94, 77, 35, 99, 60 },        /* level 12 */
    { 105, 82, 80, 99, 85, 40, 99, 65 },        /* level 13 */
    { 110, 87, 85, 99, 93, 40, 99, 70 },        /* level 14 */
    { 115, 92, 90, 99, 99, 50, 100, 75 },       /* level 15 */
    { 125, 97, 95, 99, 99, 50, 100, 80 },       /* level 16 */
    { 125, 99, 99, 99, 99, 55, 100, 80 },       /* level 17 */
    { 130, 99, 99, 99, 99, 55, 100, 85 }        /* level 18 */
};

char thief_race_adjust[6][8] = {                /* 630d */
    { 5, -5, 0, 5, 10, 5, 0, 0 },               /* Elf */
    { 10, 0, 0, 0, 5, 0, 0, 0 },                /* Half-Elf */
    { 0, 10, 15, 0, 0, 0, -10, -5 },            /* Dwarf */
    { 0, 5, 10, 5, 5, 10, -15, 0 },             /* Gnome */
    { 5, 5, 5, 10, 15, 5, -15, -5 },            /* Halfling */
    { 0, 0, 0, 0, 0, 0, 0, 0 }                  /* Human */
};

char thief_dex_adjust[11][5] = {                /* 633d */
    { -15, -10, -10, -20, -10 },                /* dexterity 9 */
    { -19, -5, -10, -15, -5 },                  /* dexterity 10 */
    { -5, 0, -5, -10, 0 },                      /* dexterity 11 */
    { 0, 0, 0, -5, 0 },                         /* dexterity 12 */
    { 0, 0, 0, 0, 0 },                          /* dexterity 13 */
    { 0, 0, 0, 0, 0 },                          /* dexterity 14 */
    { 0, 0, 0, 0, 0 },                          /* dexterity 15 */
    { 0, -5, 0, 0, 0 },                         /* dexterity 16 */
    { 5, 10, 0, 5, 5 },                         /* dexterity 17 */
    { 10, 15, 5, 10, 10 },                      /* dexterity 18 */
    { 15, 20, 10, 12, 12 }                      /* dexterity 19 */
};

/* Five bytes after thief_dex_adjust (DQK has none here); not a dexterity-20
 * row, and nothing reads them. */
char byte_6374[5] = {                           /* 6374 */
    -16, -15, -14, -14, -11
};

char far *fx_names[230] = {                     /* 6379 */
    "",                                 /* 00 */
    "Blessed",                          /* 01 */
    "Cursed",                           /* 02 */
    "Harrassed by Snakes",              /* 03 */
    "Dispelling Evil",                  /* 04 */
    "Detecting Magic",                  /* 05 */
    "Immune to Electricity",            /* 06 */
    "",                                 /* 07 */
    "Protected from Evil",              /* 08 */
    "Protected from Good",              /* 09 */
    "Resistant to Cold",                /* 0a */
    "Charmed",                          /* 0b */
    "Enlarged",                         /* 0c */
    "Barkskin",                         /* 0d */
    "Friendly",                         /* 0e */
    "Slow Poison",                      /* 0f */
    "Reading Magic",                    /* 10 */
    "Shielded",                         /* 11 */
    "",                                 /* 12 */
    "Able to Find Traps",               /* 13 */
    "Fire Resistant (spell)",           /* 14 */
    "Silenced",                         /* 15 */
    "",                                 /* 16 */
    "Carries Spiritual Hammer",         /* 17 */
    "Sees Invisible",                   /* 18 */
    "Invisible (spell)",                /* 19 */
    "",                                 /* 1a */
    "Befuddled",                        /* 1b */
    "Duplicated",                       /* 1c */
    "Weakened",                         /* 1d */
    "Nauseated",                        /* 1e */
    "Helpless",                         /* 1f */
    "",                                 /* 20 */
    "Blind",                            /* 21 */
    "Diseased",                         /* 22 */
    "Confused",                         /* 23 */
    "Accursed",                         /* 24 */
    "Blinking",                         /* 25 */
    "Strengthened",                     /* 26 */
    "Hastened",                         /* 27 */
    "",                                 /* 28 */
    "Protected from Normal Missiles",   /* 29 */
    "Slowed",                           /* 2a */
    "",                                 /* 2b */
    "",                                 /* 2c */
    "Protected from Evil, 10' Radius",  /* 2d */
    "Protected from Good, 10' Radius",  /* 2e */
    "",                                 /* 2f */
    "",                                 /* 30 */
    "Affected by Prayer",               /* 31 */
    "Fire Shield, Hot flames",          /* 32 */
    "",                                 /* 33 */
    "Held",                             /* 34 */
    "Asleep",                           /* 35 */
    "Fire Shield, Cold flames",         /* 36 */
    "POISONED",                         /* 37 */
    "Invisible (ring)",                 /* 38 */
    "Within Globe of Invulnerability",  /* 39 */
    "",                                 /* 3a */
    "",                                 /* 3b */
    "",                                 /* 3c */
    "Fire Resistant (ring)",            /* 3d */
    "",                                 /* 3e */
    "In Minor Globe of Invulnerability", /* 3f */
    "",                                 /* 40 */
    "",                                 /* 41 */
    "",                                 /* 42 */
    "",                                 /* 43 */
    "Mentally Enfeebled",               /* 44 */
    "Invisible to Animals",             /* 45 */
    "",                                 /* 46 */
    "Magically Highlighted",            /* 47 */
    "",                                 /* 48 */
    "Immune to Dragon Breath",          /* 49 */
    "Wearing Magical Boots",            /* 4a */
    "Giant Slaying Sword",              /* 4b */
    "Immune to Disease",                /* 4c */
    "Confused",                         /* 4d */
    "Carrying Magical Stone",           /* 4e */
    "",                                 /* 4f */
    "",                                 /* 50 */
    "",                                 /* 51 */
    "",                                 /* 52 */
    "",                                 /* 53 */
    "",                                 /* 54 */
    "",                                 /* 55 */
    "",                                 /* 56 */
    "",                                 /* 57 */
    "",                                 /* 58 */
    "Wearing Magical Cloak",            /* 59 */
    "",                                 /* 5a */
    "",                                 /* 5b */
    "Immune to Fear",                   /* 5c */
    "Immune to Fire",                   /* 5d */
    "",                                 /* 5e */
    "",                                 /* 5f */
    "",                                 /* 60 */
    "",                                 /* 61 */
    "Immune To Cold",                   /* 62 */
    "",                                 /* 63 */
    "",                                 /* 64 */
    "",                                 /* 65 */
    "",                                 /* 66 */
    "",                                 /* 67 */
    "",                                 /* 68 */
    "",                                 /* 69 */
    "STUNNED",                          /* 6a */
    "Berserk",                          /* 6b */
    "",                                 /* 6c */
    "",                                 /* 6d */
    "",                                 /* 6e */
    "FRIGHTENED",                       /* 6f */
    "",                                 /* 70 */
    "Giant Strength",                   /* 71 */
    "",                                 /* 72 */
    "vorpal weapon",                    /* 73 */
    "",                                 /* 74 */
    "",                                 /* 75 */
    "",                                 /* 76 */
    "Resist Fire (Druid)",              /* 77 */
    "",                                 /* 78 */
    "Fire Touch",                       /* 79 */
    "Iron Skin",                        /* 7a */
    "Shielded by Mind Blank",           /* 7b */
    "",                                 /* 7c */
    "",                                 /* 7d */
    "",                                 /* 7e */
    "",                                 /* 7f */
    "",                                 /* 80 */
    "",                                 /* 81 */
    "Entangled",                        /* 82 */
    "",                                 /* 83 */
    "",                                 /* 84 */
    "",                                 /* 85 */
    "",                                 /* 86 */
    "",                                 /* 87 */
    "",                                 /* 88 */
    "",                                 /* 89 */
    "",                                 /* 8a */
    "",                                 /* 8b */
    "",                                 /* 8c */
    "",                                 /* 8d */
    "",                                 /* 8e */
    "",                                 /* 8f */
    "",                                 /* 90 */
    "",                                 /* 91 */
    "",                                 /* 92 */
    "",                                 /* 93 */
    "",                                 /* 94 */
    "",                                 /* 95 */
    "",                                 /* 96 */
    "",                                 /* 97 */
    "",                                 /* 98 */
    "",                                 /* 99 */
    "",                                 /* 9a */
    "",                                 /* 9b */
    "",                                 /* 9c */
    "",                                 /* 9d */
    "",                                 /* 9e */
    "",                                 /* 9f */
    "",                                 /* a0 */
    "",                                 /* a1 */
    "",                                 /* a2 */
    "",                                 /* a3 */
    "",                                 /* a4 */
    "",                                 /* a5 */
    "",                                 /* a6 */
    "",                                 /* a7 */
    "",                                 /* a8 */
    "",                                 /* a9 */
    "",                                 /* aa */
    "",                                 /* ab */
    "Diseased",                         /* ac */
    "",                                 /* ad */
    "",                                 /* ae */
    "",                                 /* af */
    "",                                 /* b0 */
    "",                                 /* b1 */
    "",                                 /* b2 */
    "",                                 /* b3 */
    0,                                  /* b4 */
    0,                                  /* b5 */
    0,                                  /* b6 */
    0,                                  /* b7 */
    0,                                  /* b8 */
    0,                                  /* b9 */
    0,                                  /* ba */
    0,                                  /* bb */
    0,                                  /* bc */
    0,                                  /* bd */
    0,                                  /* be */
    0,                                  /* bf */
    0,                                  /* c0 */
    0,                                  /* c1 */
    0,                                  /* c2 */
    0,                                  /* c3 */
    0,                                  /* c4 */
    0,                                  /* c5 */
    0,                                  /* c6 */
    0,                                  /* c7 */
    0,                                  /* c8 */
    0,                                  /* c9 */
    0,                                  /* ca */
    0,                                  /* cb */
    0,                                  /* cc */
    0,                                  /* cd */
    0,                                  /* ce */
    0,                                  /* cf */
    0,                                  /* d0 */
    0,                                  /* d1 */
    0,                                  /* d2 */
    0,                                  /* d3 */
    0,                                  /* d4 */
    0,                                  /* d5 */
    0,                                  /* d6 */
    0,                                  /* d7 */
    0,                                  /* d8 */
    0,                                  /* d9 */
    0,                                  /* da */
    0,                                  /* db */
    0,                                  /* dc */
    0,                                  /* dd */
    0,                                  /* de */
    0,                                  /* df */
    0,                                  /* e0 */
    0,                                  /* e1 */
    0,                                  /* e2 */
    0,                                  /* e3 */
    0,                                  /* e4 */
    0                                   /* e5 */
};

unsigned char bit_mask[8] = {                   /* 6711 */
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};

int cur_level = 5;                              /* 6719 */
char view_restore_due = 0;                      /* 671b */
char in_service_event = 0;                      /* 671c */
char chain_allowed = 1;                         /* 671d */
long train_cost = 0;                            /* 671e */
char combat_outdoors = 0;                       /* 6722 */
char byte_6723 = 0;                             /* 6723 */
char big_pic_shown = 0;                         /* 6724 */
char quit_requested = 0;                        /* 6725 */
char byte_6726 = 0;                             /* 6726 */
char events_running = 0;                        /* 6727 */
