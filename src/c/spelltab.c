/* CKIT.EXE data-only module between sfxctl.c and charpic.c: the spell tables,
 * DGROUP 2438..38d1.  spell_name_table and spell_info_table are indexed by spell
 * number from 1, so each is declared from its first element and read as
 * (table - 1)[spell]: the readers' spell_names and spell_info, whose element 0
 * the shipped code addresses (spell_info's lies inside spell_name_table, at
 * 264a).  Both run to spell 138, the name table's last entry a null pointer.
 * The string literals follow the tables, DGROUP 323c..38d1, as Turbo C emits
 * them (-d stores each repeated name once).  Dark Queen of Krynn's spelltab.c
 * with FRUA's own spell list and without DQK's scroll kinds.
 *
 * 2437, after sfxctl.c's literals, is the link's word-alignment pad.  The
 * module has no routines; its empty _TEXT sits at charpic.c's start.
 */

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0   0 cleric, 1 druid, 2 magic-user   */
    unsigned char level;        /* +2   spell level, from 1               */
    unsigned char range;        /* +3   0xff touch                        */
    unsigned char range_per_level; /* +4                                  */
    unsigned char duration;     /* +5                                     */
    unsigned char duration_per_level; /* +6                               */
    unsigned char targeting;    /* +7   low 4 bits the kind               */
    int target;                 /* +8   1 caster, 2 one chosen, 3 party   */
    int save_effect;            /* +a   0: no saving throw                */
    int save_type;              /* +c                                     */
    unsigned char effect;       /* +e   effect that blocks it; 0: none    */
    int in_combat;              /* +f   0: camp only                      */
    unsigned char casting_time; /* +11  a third of it delays the caster   */
    unsigned char ai_priority;  /* +12                                    */
    int hostile;                /* +13  0: cast on the caster's own side  */
    unsigned char byte_15;      /* +15  not read                          */
};

char far *spell_name_table[138] = {             /* 2438 */
    "Bless",                            /* 1 */
    "Curse",                            /* 2 */
    "Cure Light Wounds",                /* 3 */
    "Cause Light Wounds",               /* 4 */
    "Detect Magic",                     /* 5 */
    "Protection From Evil",             /* 6 */
    "Protection from Good",             /* 7 */
    "Resist Cold",                      /* 8 */
    "Burning Hands",                    /* 9 */
    "Charm Person",                     /* 10 */
    "Detect Magic",                     /* 11 */
    "Enlarge",                          /* 12 */
    "Reduce",                           /* 13 */
    "Friends",                          /* 14 */
    "Magic Missile",                    /* 15 */
    "Protection From Evil",             /* 16 */
    "Protection From Good",             /* 17 */
    "Read Magic",                       /* 18 */
    "Shield",                           /* 19 */
    "Shocking Grasp",                   /* 20 */
    "Sleep",                            /* 21 */
    "Find Traps",                       /* 22 */
    "Hold Person",                      /* 23 */
    "Resist Fire",                      /* 24 */
    "Silence, 15' Radius",              /* 25 */
    "Slow Poison",                      /* 26 */
    "Snake Charm",                      /* 27 */
    "Spiritual Hammer",                 /* 28 */
    "Detect Invisibility",              /* 29 */
    "Invisibility",                     /* 30 */
    "Knock",                            /* 31 */
    "Mirror Image",                     /* 32 */
    "Ray of Enfeeblement",              /* 33 */
    "Stinking Cloud",                   /* 34 */
    "Strength",                         /* 35 */
    "Heal",                             /* 36 */
    "Cure Blindness",                   /* 37 */
    "Cause Blindness",                  /* 38 */
    "Cure Disease",                     /* 39 */
    "Cause Disease",                    /* 40 */
    "Dispel Magic",                     /* 41 */
    "Prayer",                           /* 42 */
    "Remove Curse",                     /* 43 */
    "Bestow Curse",                     /* 44 */
    "Blink",                            /* 45 */
    "Dispel Magic",                     /* 46 */
    "Fireball",                         /* 47 */
    "Haste",                            /* 48 */
    "Hold Person",                      /* 49 */
    "Invisibility, 10' Radius",         /* 50 */
    "Lightning Bolt",                   /* 51 */
    "Prot. From Evil, 10' Radius",      /* 52 */
    "Prot. From Good, 10' Radius",      /* 53 */
    "Prot. From Normal Missiles",       /* 54 */
    "Slow",                             /* 55 */
    "Harm",                             /* 56 */
    "Prot. From Evil, 10' Radius",      /* 57 */
    "Cure Serious Wounds",              /* 58 */
    "Silence 15' radius",               /* 59 */
    "Detect Magic",                     /* 60 */
    "Remove Curse",                     /* 61 */
    "Bless",                            /* 62 */
    "Charm Person",                     /* 63 */
    "Burning Hands",                    /* 64 */
    "",                                 /* 65 */
    "Cause Serious Wounds",             /* 66 */
    "Neutralize Poison",                /* 67 */
    "Poison",                           /* 68 */
    "Prot. From Evil, 10' Radius",      /* 69 */
    "Sticks to Snakes",                 /* 70 */
    "Cure Critical Wounds",             /* 71 */
    "Cause Critical Wounds",            /* 72 */
    "Dispel Evil",                      /* 73 */
    "Flame Strike",                     /* 74 */
    "Raise Dead",                       /* 75 */
    "Slay Living",                      /* 76 */
    "Detect Magic",                     /* 77 */
    "Entangle",                         /* 78 */
    "Faerie Fire",                      /* 79 */
    "Invisibility to Animals",          /* 80 */
    "Charm Monsters",                   /* 81 */
    "Confusion",                        /* 82 */
    "Dimension Door",                   /* 83 */
    "Fear",                             /* 84 */
    "Fire Shield",                      /* 85 */
    "Fumble",                           /* 86 */
    "Ice Storm",                        /* 87 */
    "Minor Globe Of Invulnerability",   /* 88 */
    "Remove Curse",                     /* 89 */
    "Barkskin",                         /* 90 */
    "Cloud Kill",                       /* 91 */
    "Cone of Cold",                     /* 92 */
    "Feeblemind",                       /* 93 */
    "Hold Monsters",                    /* 94 */
    "Scroll of Protection",             /* 95 */
    "Charm Person Or Mammal",           /* 96 */
    "",                                 /* 97 */
    "Cure Light Wounds",                /* 98 */
    "",                                 /* 99 */
    "Bestow Curse",                     /* 100 */
    "Blade Barrier",                    /* 101 */
    "Restoration",                      /* 102 */
    "Energy Drain",                     /* 103 */
    "Destruction",                      /* 104 */
    "Resurrection",                     /* 105 */
    "Cure Disease",                     /* 106 */
    "Neutralize Poison",                /* 107 */
    "Hold Animal",                      /* 108 */
    "Prot. from Fire",                  /* 109 */
    "Death Spell",                      /* 110 */
    "Disintegrate",                     /* 111 */
    "Globe of Invulnerability",         /* 112 */
    "Stone to Flesh",                   /* 113 */
    "Flesh to Stone",                   /* 114 */
    "Delayed Blast Fireball",           /* 115 */
    "Mass Invisibility",                /* 116 */
    "Power Word Stun",                  /* 117 */
    "Fire Touch",                       /* 118 */
    "Iron Skin",                        /* 119 */
    "Mass Charm",                       /* 120 */
    "Otto's Irr. Dance",                /* 121 */
    "Mind Blank",                       /* 122 */
    "Power Word Blind",                 /* 123 */
    "Meteor Swarm",                     /* 124 */
    "Power Word Kill",                  /* 125 */
    "Monster Summoning",                /* 126 */
    "Potion Of Speed",                  /* 127 */
    "Potion Of Frost",                  /* 128 */
    "Javelin Of Lightning",             /* 129 */
    "Wand Of Paralyzation",             /* 130 */
    "Potion Of Healing",                /* 131 */
    "Elixir Of Youth",                  /* 132 */
    "Necklace Of Missiles",             /* 133 */
    "Wand Of Magic Missiles",           /* 134 */
    "Scroll of Protection",             /* 135 */
    "Potion Of Invisibility",           /* 136 */
    "Potion Of Extra Healing",          /* 137 */
    0                                   /* 138 */
};

struct SpellInfo spell_info_table[138] = {      /* 2660 */
    { 0, 1, 6, 0, 6, 0, 0xa, 3, 0, 4, 1, 2, 10, 1, 0, 0 },  /* 1 Bless */
    { 0, 1, 6, 0, 6, 0, 0xa, 0, 0, 4, 2, 1, 10, 1, 1, 0 },  /* 2 Curse */
    { 0, 1, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 5, 0, 0, 0 },     /* 3 Cure Light Wounds */
    { 0, 1, 0xff, 0, 0, 0, 4, 0, 0, 4, 0, 1, 5, 1, 1, 0 },  /* 4 Cause Light Wounds */
    { 0, 1, 3, 0, 10, 0, 0, 1, 0, 4, 5, 2, 1, 0, 0, 0 },    /* 5 Detect Magic */
    { 0, 1, 0, 0, 0, 3, 4, 2, 0, 4, 8, 2, 4, 1, 0, 0 },     /* 6 Protection From Evil */
    { 0, 1, 0, 0, 0, 3, 4, 2, 0, 4, 9, 2, 4, 1, 0, 0 },     /* 7 Protection from Good */
    { 0, 1, 0, 0, 0, 10, 4, 2, 0, 4, 10, 2, 10, 0, 0, 0 },  /* 8 Resist Cold */
    { 2, 1, 0, 0, 0, 0, 4, 0, 0, 4, 0, 1, 1, 2, 1, 0 },     /* 9 Burning Hands */
    { 2, 1, 12, 0, 60, 60, 4, 0, 1, 4, 11, 1, 1, 5, 1, 0 }, /* 10 Charm Person */
    { 2, 1, 0, 0, 0, 2, 0, 1, 0, 4, 5, 2, 1, 0, 0, 0 },     /* 11 Detect Magic */
    { 2, 1, 0, 2, 0, 10, 4, 2, 0, 4, 12, 2, 1, 0, 0, 0 },   /* 12 Enlarge */
    { 2, 1, 0, 2, 0, 10, 4, 2, 1, 4, 12, 2, 1, 0, 1, 0 },   /* 13 Reduce */
    { 2, 1, 0, 0, 0, 1, 0, 1, 0, 4, 14, 0, 1, 0, 0, 0 },    /* 14 Friends */
    { 2, 1, 6, 4, 0, 0, 4, 0, 0, 4, 0, 1, 1, 5, 1, 0 },     /* 15 Magic Missile */
    { 2, 1, 0, 0, 0, 2, 4, 2, 0, 4, 8, 2, 1, 1, 0, 0 },     /* 16 Protection From Evil */
    { 2, 1, 0, 0, 0, 2, 4, 2, 0, 4, 9, 2, 1, 1, 0, 0 },     /* 17 Protection From Good */
    { 2, 1, 0, 0, 0, 2, 0, 1, 0, 4, 16, 0, 10, 0, 0, 0 },   /* 18 Read Magic */
    { 2, 1, 0, 0, 0, 5, 0, 1, 0, 4, 17, 2, 1, 2, 0, 0 },    /* 19 Shield */
    { 2, 1, 0xff, 0, 0, 0, 4, 0, 0, 4, 0, 1, 1, 2, 1, 0 },  /* 20 Shocking Grasp */
    { 2, 1, 3, 4, 0, 5, 9, 0, 0, 4, 53, 1, 1, 4, 1, 1 },    /* 21 Sleep */
    { 0, 2, 0, 0, 30, 0, 0, 1, 0, 4, 19, 0, 5, 0, 0, 0 },   /* 22 Find Traps */
    { 0, 2, 6, 0, 4, 1, 6, 0, 1, 4, 52, 1, 5, 7, 1, 0 },    /* 23 Hold Person */
    { 0, 2, 0, 0, 0, 10, 4, 2, 0, 4, 20, 2, 5, 0, 0, 0 },   /* 24 Resist Fire */
    { 0, 2, 12, 0, 0, 2, 0x1f, 0, 1, 4, 21, 1, 5, 2, 1, 1 }, /* 25 Silence, 15' Radius */
    { 0, 2, 0, 0, 0, 60, 4, 2, 0, 4, 22, 0, 1, 0, 0, 0 },   /* 26 Slow Poison */
    { 0, 2, 3, 0, 0, 0, 0xf0, 0, 0, 4, 51, 1, 5, 0, 1, 0 }, /* 27 Snake Charm */
    { 0, 2, 3, 0, 0, 1, 0, 1, 0, 4, 23, 2, 5, 1, 0, 0 },    /* 28 Spiritual Hammer */
    { 2, 2, 0, 4, 0, 5, 0, 1, 0, 4, 24, 2, 2, 1, 0, 0 },    /* 29 Detect Invisibility */
    { 2, 2, 0, 0, 0, 0, 4, 2, 0, 4, 25, 2, 2, 2, 0, 0 },    /* 30 Invisibility */
    { 2, 2, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 1, 0, 0, 0 },     /* 31 Knock */
    { 2, 2, 0, 0, 0, 2, 0, 1, 0, 4, 28, 2, 2, 3, 0, 0 },    /* 32 Mirror Image */
    { 2, 2, 1, 1, 0, 1, 4, 0, 1, 4, 29, 1, 2, 1, 1, 0 },    /* 33 Ray of Enfeeblement */
    { 2, 2, 3, 0, 0, 1, 9, 0, 3, 0, 30, 1, 2, 4, 1, 1 },    /* 34 Stinking Cloud */
    { 2, 2, 0, 0, 0, 60, 0, 2, 0, 4, 38, 0, 10, 0, 0, 0 },  /* 35 Strength */
    { 0, 6, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 0, 2, 0, 0 },     /* 36 Heal */
    { 0, 3, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 10, 0, 0, 0 },    /* 37 Cure Blindness */
    { 0, 3, 0xff, 0, 0, 0, 4, 0, 1, 4, 33, 1, 10, 3, 1, 0 }, /* 38 Cause Blindness */
    { 0, 3, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 100, 0, 0, 0 },   /* 39 Cure Disease */
    { 0, 3, 0xff, 0, 0, 0, 4, 0, 1, 4, 34, 1, 100, 4, 1, 0 }, /* 40 Cause Disease */
    { 0, 3, 6, 0, 0, 0, 9, 2, 0, 4, 0, 2, 4, 1, 1, 1 },     /* 41 Dispel Magic */
    { 0, 3, 0, 0, 0, 1, 0, 3, 0, 4, 49, 2, 6, 5, 0, 0 },    /* 42 Prayer */
    { 0, 3, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 6, 0, 0, 0 },     /* 43 Remove Curse */
    { 0, 3, 0xff, 0, 0, 10, 4, 0, 1, 4, 36, 1, 6, 2, 1, 0 }, /* 44 Bestow Curse */
    { 2, 3, 0, 0, 0, 1, 0, 0, 0, 4, 37, 1, 1, 4, 0, 0 },    /* 45 Blink */
    { 2, 3, 12, 0, 0, 1, 9, 2, 0, 4, 0, 2, 3, 1, 1, 1 },    /* 46 Dispel Magic */
    { 2, 3, 10, 4, 0, 0, 0xb, 0, 2, 4, 0, 1, 3, 7, 1, 3 },  /* 47 Fireball */
    { 2, 3, 6, 0, 3, 1, 0xa, 3, 0, 4, 39, 2, 3, 5, 0, 0 },  /* 48 Haste */
    { 2, 3, 12, 0, 0, 2, 7, 0, 1, 4, 52, 1, 3, 6, 1, 0 },   /* 49 Hold Person */
    { 2, 3, 0, 0, 0, 0, 9, 3, 0, 4, 25, 2, 3, 1, 0, 0 },    /* 50 Invisibility, 10' Radius */
    { 2, 3, 4, 4, 0, 0, 8, 0, 2, 4, 0, 1, 3, 7, 1, 0 },     /* 51 Lightning Bolt */
    { 2, 3, 0, 0, 0, 2, 4, 2, 0, 4, 45, 2, 3, 1, 0, 0 },    /* 52 Prot. From Evil, 10' Radius */
    { 2, 3, 0, 0, 0, 2, 4, 2, 0, 4, 46, 2, 3, 2, 0, 0 },    /* 53 Prot. From Good, 10' Radius */
    { 2, 3, 0, 0, 0, 10, 4, 2, 0, 4, 41, 2, 3, 3, 0, 0 },   /* 54 Prot. From Normal Missiles */
    { 2, 3, 9, 4, 3, 1, 0xa, 0, 0, 4, 42, 1, 3, 4, 1, 0 },  /* 55 Slow */
    { 0, 6, 0, 0, 0, 0, 4, 0, 0, 4, 0, 1, 10, 0, 1, 0 },    /* 56 Harm */
    { 3, 0, 0, 0, 0, 2, 4, 2, 0, 4, 45, 2, 3, 1, 0, 0 },    /* 57 Prot. From Evil, 10' Radius */
    { 0, 4, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 7, 0, 0, 0 },     /* 58 Cure Serious Wounds */
    { 3, 0, 12, 0, 0, 2, 0x1f, 0, 1, 4, 21, 1, 5, 1, 1, 1 }, /* 59 Silence 15' radius */
    { 3, 0, 0, 0, 10, 0, 0, 1, 0, 4, 5, 2, 0, 0, 0, 0 },    /* 60 Detect Magic */
    { 3, 0, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 6, 0, 0, 0 },     /* 61 Remove Curse */
    { 3, 0, 6, 0, 6, 0, 0xa, 3, 0, 4, 1, 2, 10, 1, 0, 0 },  /* 62 Bless */
    { 3, 0, 12, 0, 0, 0, 4, 0, 1, 4, 11, 1, 1, 5, 1, 0 },   /* 63 Charm Person */
    { 3, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 1, 1, 1, 1, 0 },     /* 64 Burning Hands */
    { 4, 6, 12, 0, 0, 0, 4, 0, 0, 4, 0, 1, 0, 5, 1, 0 },    /* 65  */
    { 0, 4, 0, 0, 0, 0, 4, 0, 0, 4, 0, 1, 7, 3, 1, 0 },     /* 66 Cause Serious Wounds */
    { 0, 4, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 7, 0, 0, 0 },     /* 67 Neutralize Poison */
    { 0, 4, 0, 0, 0, 0, 4, 0, 1, 0, 0, 1, 7, 6, 1, 0 },     /* 68 Poison */
    { 0, 4, 3, 0, 0, 10, 4, 2, 0, 4, 45, 2, 7, 1, 0, 0 },   /* 69 Prot. From Evil, 10' Radius */
    { 0, 4, 3, 0, 0, 2, 4, 0, 0, 4, 3, 1, 7, 4, 1, 0 },     /* 70 Sticks to Snakes */
    { 0, 5, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 8, 2, 0, 0 },     /* 71 Cure Critical Wounds */
    { 0, 5, 0xff, 0, 0, 0, 4, 0, 1, 4, 0, 1, 8, 4, 1, 0 },  /* 72 Cause Critical Wounds */
    { 0, 5, 0, 0, 0, 1, 0, 0, 0, 4, 32, 1, 8, 5, 0, 0 },    /* 73 Dispel Evil */
    { 0, 5, 6, 0, 0, 0, 4, 0, 2, 4, 0, 1, 8, 6, 1, 0 },     /* 74 Flame Strike */
    { 0, 5, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 10, 0, 0, 0 },    /* 75 Raise Dead */
    { 0, 5, 3, 0, 0, 0, 4, 0, 1, 4, 0, 1, 10, 7, 1, 0 },    /* 76 Slay Living */
    { 1, 1, 3, 0, 12, 0, 0, 1, 0, 4, 5, 2, 3, 1, 0, 0 },    /* 77 Detect Magic */
    { 1, 1, 8, 0, 10, 0, 0xb, 0, 1, 4, 0, 1, 3, 1, 1, 0 },  /* 78 Entangle */
    { 1, 1, 8, 0, 0, 4, 5, 0, 1, 4, 71, 1, 3, 1, 1, 0 },    /* 79 Faerie Fire */
    { 1, 1, 0xff, 0, 10, 1, 4, 2, 0, 4, 69, 2, 4, 1, 1, 0 }, /* 80 Invisibility to Animals */
    { 2, 4, 6, 0, 0, 0, 5, 0, 1, 4, 11, 1, 4, 6, 1, 0 },    /* 81 Charm Monsters */
    { 2, 4, 12, 0, 2, 1, 0xb, 0, 1, 4, 35, 1, 4, 7, 1, 0 }, /* 82 Confusion */
    { 2, 4, 0, 12, 0, 0, 8, 0, 0, 4, 0, 1, 1, 0, 1, 0 },    /* 83 Dimension Door */
    { 2, 4, 6, 0, 0, 1, 8, 0, 1, 4, 111, 1, 4, 6, 1, 0 },   /* 84 Fear */
    { 2, 4, 0, 0, 2, 1, 0, 1, 0, 4, 0, 2, 4, 7, 0, 0 },     /* 85 Fire Shield */
    { 2, 4, 0, 4, 0, 1, 4, 0, 1, 4, 27, 1, 4, 4, 1, 0 },    /* 86 Fumble */
    { 2, 4, 0, 4, 0, 0, 0xa, 0, 0, 4, 0, 1, 4, 7, 1, 0 },   /* 87 Ice Storm */
    { 2, 4, 0, 16, 0, 1, 0, 1, 0, 4, 63, 2, 4, 6, 0, 0 },   /* 88 Minor Globe Of Invulnerability */
    { 2, 4, 0, 0, 0, 0, 4, 2, 0, 4, 0, 2, 4, 0, 0, 0 },     /* 89 Remove Curse */
    { 1, 2, 1, 0, 4, 1, 0xf0, 2, 0, 4, 13, 2, 3, 0, 0, 0 }, /* 90 Barkskin */
    { 2, 5, 2, 0, 0, 1, 9, 0, 0, 4, 0, 2, 5, 5, 1, 0 },     /* 91 Cloud Kill */
    { 2, 5, 6, 0, 0, 0, 8, 0, 2, 4, 0, 1, 5, 6, 1, 0 },     /* 92 Cone of Cold */
    { 2, 5, 16, 0, 0, 0, 4, 0, 1, 4, 68, 1, 5, 5, 1, 0 },   /* 93 Feeblemind */
    { 2, 5, 0, 2, 0, 1, 7, 0, 1, 4, 52, 1, 5, 7, 1, 0 },    /* 94 Hold Monsters */
    { 4, 6, 0, 0, 0, 1, 0, 1, 0, 4, 73, 2, 10, 1, 0, 0 },   /* 95 Scroll of Protection */
    { 1, 2, 8, 0, 60, 60, 4, 0, 1, 4, 11, 1, 4, 5, 1, 0 },  /* 96 Charm Person Or Mammal */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 25, 2, 0, 1, 0, 0 },    /* 97  */
    { 1, 2, 0xff, 0, 0, 0, 4, 2, 0, 4, 0, 2, 4, 2, 0, 0 },  /* 98 Cure Light Wounds */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, 1, 0, 0 },     /* 99  */
    { 2, 4, 3, 0, 0, 3, 4, 0, 1, 4, 36, 1, 4, 4, 1, 0 },    /* 100 Bestow Curse */
    { 0, 6, 3, 0, 0, 3, 9, 0, 1, 4, 0, 1, 9, 9, 1, 0 },     /* 101 Blade Barrier */
    { 0, 7, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0 },     /* 102 Restoration */
    { 0, 7, 0, 0, 0, 0, 4, 0, 0, 4, 0, 1, 3, 8, 1, 0 },     /* 103 Energy Drain */
    { 0, 7, 0, 0, 0, 0, 4, 0, 0, 4, 0, 1, 10, 9, 1, 0 },    /* 104 Destruction */
    { 0, 7, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 10, 0, 0, 0 },    /* 105 Resurrection */
    { 1, 3, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0 },     /* 106 Cure Disease */
    { 1, 3, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 1, 0, 0, 0 },     /* 107 Neutralize Poison */
    { 1, 3, 8, 0, 0, 2, 7, 0, 1, 4, 52, 1, 5, 4, 1, 0 },    /* 108 Hold Animal */
    { 1, 3, 0, 0, 0, 0, 4, 2, 0, 4, 119, 2, 5, 3, 0, 0 },   /* 109 Prot. from Fire */
    { 2, 6, 0, 4, 0, 0, 9, 0, 0, 4, 0, 1, 6, 3, 1, 0 },     /* 110 Death Spell */
    { 2, 6, 0, 2, 0, 0, 4, 0, 1, 4, 0, 1, 6, 8, 1, 0 },     /* 111 Disintegrate */
    { 2, 6, 0, 0, 0, 1, 0, 1, 0, 4, 57, 2, 1, 3, 0, 0 },    /* 112 Globe of Invulnerability */
    { 2, 6, 0, 4, 0, 0, 4, 2, 0, 4, 0, 2, 6, 0, 0, 0 },     /* 113 Stone to Flesh */
    { 2, 6, 0, 4, 0, 0, 4, 0, 1, 4, 65, 1, 6, 7, 1, 0 },    /* 114 Flesh to Stone */
    { 2, 7, 10, 4, 0, 0, 0xb, 0, 2, 4, 0, 1, 1, 9, 1, 0 },  /* 115 Delayed Blast Fireball */
    { 2, 7, 0, 4, 0, 0, 0xa, 3, 0, 4, 25, 2, 7, 3, 0, 0 },  /* 116 Mass Invisibility */
    { 2, 7, 0, 2, 0, 0, 4, 0, 0, 4, 106, 1, 1, 3, 1, 0 },   /* 117 Power Word Stun */
    { 2, 5, 0, 0, 0, 1, 4, 2, 0, 4, 121, 2, 6, 4, 0, 0 },   /* 118 Fire Touch */
    { 2, 5, 0, 0, 0, 1, 0, 1, 0, 4, 122, 2, 5, 3, 0, 0 },   /* 119 Iron Skin */
    { 2, 8, 0, 1, 0, 0, 9, 0, 1, 4, 11, 1, 8, 6, 1, 0 },    /* 120 Mass Charm */
    { 2, 8, 0xff, 0, 0, 0, 4, 0, 0, 4, 118, 1, 5, 5, 1, 0 }, /* 121 Otto's Irr. Dance */
    { 2, 8, 3, 0, 0, 0, 4, 2, 0, 4, 123, 2, 1, 0, 0, 0 },   /* 122 Mind Blank */
    { 2, 8, 0, 1, 0, 0, 0xa, 0, 0, 4, 0, 1, 1, 3, 1, 0 },   /* 123 Power Word Blind */
    { 2, 9, 4, 1, 0, 0, 8, 0, 2, 4, 0, 1, 9, 3, 1, 0 },     /* 124 Meteor Swarm */
    { 2, 9, 0, 1, 0, 0, 9, 0, 0, 4, 0, 1, 1, 6, 1, 0 },     /* 125 Power Word Kill */
    { 2, 9, 9, 0, 0, 0, 0, 0, 0, 4, 0, 1, 9, 0, 1, 0 },     /* 126 Monster Summoning */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 39, 2, 0, 3, 0, 0 },    /* 127 Potion Of Speed */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 113, 2, 0, 2, 0, 0 },   /* 128 Potion Of Frost */
    { 4, 6, 10, 0, 0, 0, 4, 0, 2, 4, 0, 1, 0, 7, 1, 0 },    /* 129 Javelin Of Lightning */
    { 4, 6, 6, 0, 0, 0, 4, 0, 1, 0, 52, 1, 0, 6, 1, 0 },    /* 130 Wand Of Paralyzation */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, 0, 0, 0 },     /* 131 Potion Of Healing */
    { 4, 6, 0, 0, 0, 0, 0, 3, 0, 4, 0, 2, 0, 0, 0, 0 },     /* 132 Elixir Of Youth */
    { 4, 6, 7, 0, 0, 0, 0xb, 0, 2, 4, 0, 1, 0, 7, 1, 3 },   /* 133 Necklace Of Missiles */
    { 4, 6, 12, 0, 0, 0, 4, 0, 0, 4, 0, 1, 0, 5, 1, 0 },    /* 134 Wand Of Magic Missiles */
    { 4, 6, 0, 0, 0, 1, 0, 1, 0, 4, 73, 2, 10, 1, 0, 0 },   /* 135 Scroll of Protection */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 25, 2, 0, 1, 0, 0 },    /* 136 Potion Of Invisibility */
    { 4, 6, 0, 0, 0, 0, 0, 1, 0, 4, 0, 2, 0, 1, 0, 0 },     /* 137 Potion Of Extra Healing */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }      /* 138  */
};
