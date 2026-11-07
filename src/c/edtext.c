/* CKIT.EXE data-only module between startdat.c and hero.c: the design editor's
 * text, DGROUP 002c..0e55 -- the File, Map and Utilities menus (menubox.c's
 * records), the tool, wall, art and screen names, the editor's questions, item
 * kinds and the words its screens put together ("Replace", "with", "at", ...),
 * each a far pointer (or a table of them) to a string literal; the literals
 * follow, DGROUP 049c..0e55, as Turbo C emits them: -d stores an equal literal
 * once and a literal that ends another as that one's tail ("" is the NUL of
 * "Open...").  FRUA's own: Dark Queen of Krynn has no editor.
 *
 * A table's extent is what its readers index; a pointer nothing reads takes a
 * name for what it says.  Three readers name entries inside a table:
 * areatool.c's replace_all_text is util_items[1].text, artpick.c's
 * backdrop_text and edpick.c's zone_text are tool_names[1] and [2], and
 * artpick.c's monster_editor_text is screen_titles[7]; each reads it through a
 * #define.  No routines; its empty _TEXT sits at hero.c's start.
 */

struct Entry {                  /* one menu entry, 8 bytes, as menubox.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6                                     */
    char flags;                 /* +7   4 a mark, 0x81 not pickable       */
};

struct Menu {                   /* a pop-up menu, 0x14 bytes, menubox.c's */
    char far *title;            /* +0                                     */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8   row the entries are placed from   */
    int col;                    /* +a                                     */
    char count;                 /* +c                                     */
    char flags;                 /* +d                                     */
    int glyph;                  /* +e                                     */
    int row;                    /* +10                                    */
    int width;                  /* +12                                    */
};

struct Entry file_items[10] = {                 /* 002c  the File menu */
    { "Open...", 1, 0, 0x00 },
    { "Save", 1, 0, 0x00 },
    { "Write to...", 1, 0, 0x00 },
    { "Copy from...   ", 1, 0, 0x00 },
    { "Revert to saved", 1, 0, 0x00 },
    { "(-", 1, 0, 0x81 },
    { "Global info", 1, 0, 0x00 },
    { "Print", 1, 0, 0x00 },
    { "(-", 1, 0, 0x81 },
    { "Leave", 1, 0, 0x00 }
};

struct Menu file_menu = { "File      ", file_items, 0, 0, 10, 48, 0, 0, 0 }; /* 007c */

char far *place_names[2] = {                    /* 0090 */
    "Wall placement",
    "Block placement"
};

struct Entry map_items[9] = {                   /* 0098  the Map menu; [3] the placement place_names gives */
    { "3-D view", 1, 0, 0x00 },
    { "Area view", 1, 0, 0x04 },
    { "(-", 1, 0, 0x81 },
    { "Wall placement", 1, 0, 0x04 },
    { "Backdrop placement", 1, 0, 0x00 },
    { "Zone placement", 1, 0, 0x00 },
    { "Event placement", 1, 0, 0x00 },
    { "(-", 1, 0, 0x81 },
    { "Move through walls", 1, 0, 0x00 }
};

struct Menu map_menu = { "Map       ", map_items, 0, 0, 9, 48, 0, 0, 0 }; /* 00e0 */

struct Entry util_items[8] = {                  /* 00f4  the Utilities menu; [1].text replace_all_text */
    { "Display access", 1, 0, 0x00 },
    { "Replace globally", 1, 0, 0x00 },
    { "Clear module", 1, 0, 0x00 },
    { "(-", 1, 0, 0x81 },
    { "Entry points", 1, 0, 0x00 },
    { "Place entry", 1, 0, 0x00 },
    { "(-", 1, 0, 0x81 },
    { "Test module", 1, 0, 0x00 }
};

struct Menu util_menu = { "Utilities ", util_items, 0, 0, 8, 48, 0, 0, 0 }; /* 0134 */
char far *col_row_fmt = "Col %2d Row %2d";      /* 0148 */
char far *col_row_none_fmt = "Col %s Row %s";   /* 014c */
char far *size_fmt = "Wd  %2d Ht  %2d";         /* 0150 */

char far *tool_names[4] = {                     /* 0154  by tool; [1] backdrop_text, [2] zone_text */
    "Wall",
    "Backdrop",
    "Zone",
    "Event"
};

char far *move_names[2] = {                     /* 0164 */
    "Free movement   ",
    "Movement blocked"
};

char far *wall_kind_names[16] = {               /* 016c */
    "Open         ",                    /* 0 */
    "Open secret  ",                    /* 1 */
    "Locked       ",                    /* 2 */
    "Locked secret",                    /* 3 */
    "Locked wizard",                    /* 4 */
    "Lock wiz scrt",                    /* 5 */
    "Locked key1  ",                    /* 6 */
    "Locked key2  ",                    /* 7 */
    "Locked key3  ",                    /* 8 */
    "Locked key4  ",                    /* 9 */
    "Locked key5  ",                    /* 10 */
    "Locked key6  ",                    /* 11 */
    "Locked key7  ",                    /* 12 */
    "Locked key8  ",                    /* 13 */
    "Blocked      ",                    /* 14 */
    "False Door   "                     /* 15 */
};

char far *pass_names[2] = {                     /* 01ac */
    "Passable     ",
    "Impassable   "
};

char far *art_names[10] = {                     /* 01b4 */
    "No art",
    "Picture",
    "Sprite",
    "Combat icon",
    "Big Picture",
    "Overland Map",
    "Dungeon",
    "Wilderness",
    "Wall Set",
    "Backdrop"
};

char far *screen_titles[10] = {                 /* 01dc  by screen; [7] monster_editor_text */
    "",                                         /* 0 */
    "AD&D Unlimited Adventures",                /* 1 */
    "Map Editor",                               /* 2 */
    "Map Globals Editor",                       /* 3 */
    "Game Settings Editor",                     /* 4 */
    "Event Editor",                             /* 5 */
    "Chain of Events",                          /* 6 */
    "Monster Editor",                           /* 7 */
    "Art Gallery",                              /* 8 */
    "File Menu"                                 /* 9 */
};

char far *access_text = "Access";               /* 0204 */
char far *entry_point_text = "Entry Point";     /* 0208 */
char far *cell_text = "cell";                   /* 020c */
char far *out_of_text = "out of";               /* 0210 */
char far *total_text = "Total";                 /* 0214 */
char far *press_enter_text = "Press Enter...";  /* 0218 */

char far *access_names[5] = {                   /* 021c  the access legend, read in pairs */
    "Accessible",
    "Locked, acc.",
    "without a key",
    "Locked, acc.",
    "with a key"
};

/* set_kind_names, undo_kind_names, cant_redo_text and undo_lost_text: nothing
 * in the image reads them. */
char far *set_kind_names[2] = {                 /* 0230 */
    "Wall Sets",
    "Backdrop Sets"
};

char far *undo_kind_names[5] = {                /* 0238 */
    "",
    "last placement",
    "last pen setting",
    "last entry placement",
    0
};

char far *undo_names[2] = {                     /* 024c */
    "Undo",
    "Un*do"
};

char cant_redo_text[14] = " (Can't redo)";      /* 0254 */
char undo_lost_text[30] = "Last action can't be undone."; /* 0262 */

char far *module_verbs[5] = {                   /* 0280 */
    "Open",
    "Save (write to)",
    "Open (copy from)",
    "Place",
    "Select"
};

char far *ask_questions[11] = {                 /* 0294  ask_screen's questions, by bits & 0x3f */
    "Save changes to current module?",
    "Quit editing modules?",
    "Revert to last saved (cannot undo)?",
    "Cannot undo!  OK to proceed?",
    "Delete one event (cannot undo)?",
    "Data will be lost! OK to shrink map?",
    "Delete %d chained events?",
    "Delete entire current design?",
    "- Under Construction -",
    "No action to undo.",
    "This map can't hold any more events."
};

char far *pick_kind_names[4] = {                /* 02c0 */
    "Adventure Module",
    "Event Type",
    "Art Type",
    "Entry Point"
};

char far *game_design_text = "Game Design";     /* 02d0 */
char far *map_zone_text = "Map Zone";           /* 02d4 */
char far *special_effect_text = "Special Effect"; /* 02d8 */
char far *reserved_text = "Reserved";           /* 02dc */

char far *compass_names[4] = {                  /* 02e0 */
    "North",
    "East",
    "South",
    "West"
};

char far *item_kind_names[17] = {               /* 02f0 */
    "None",                             /* 0 */
    "+0 Weapons A",                     /* 1 */
    "+0 Weapons B",                     /* 2 */
    "+0 Armor",                         /* 3 */
    "+1 Weapons",                       /* 4 */
    "+2 Weapons",                       /* 5 */
    "+3 Weapons",                       /* 6 */
    "+4 Weapons",                       /* 7 */
    "+5 Weapons",                       /* 8 */
    "+1,+2 Armor",                      /* 9 */
    "+3,+4 Armor",                      /* 10 */
    "Special A",                        /* 11 */
    "Special B",                        /* 12 */
    "Rings",                            /* 13 */
    "Wands",                            /* 14 */
    "Potions",                          /* 15 */
    "Scrolls"                           /* 16 */
};

char far *image_text = "Image";                 /* 0334 */
char far *monster_text = "Monster";             /* 0338 */
char far *npc_text = "NPC";                     /* 033c */
char far *right_text = "Right";                 /* 0340 */
char far *left_word = "Left";                   /* 0344 */
char far *facing_text = "Facing";               /* 0348 */
char far *wall_text = "Wall";                   /* 034c */
char far *block_text = "Block";                 /* 0350 */
char far *cant_undo_this_text = "Can't Undo this action"; /* 0354 */
char far *place_text = "Place";                 /* 0358 */
char far *replace_text = "Replace";             /* 035c */
char far *edit_text = "Edit";                   /* 0360 */
char far *a_text = "a";                         /* 0364 */
char far *an_text = "an";                       /* 0368 */
char far *any_text = "any";                     /* 036c */
char far *at_text = "at";                       /* 0370 */
char far *to_text = "to";                       /* 0374 */
char far *in_text = "in";                       /* 0378 */
char far *on_text = "on";                       /* 037c */
char far *with_text = "with";                   /* 0380 */
char far *for_text = "for";                     /* 0384 */
char far *from_text = "from";                   /* 0388 */
char far *this_text = "this";                   /* 038c */
char far *wall_obstruction_text = "wall and obstruction"; /* 0390 */
char far *obstruction_text = "Obstruction";     /* 0394 */
char far *which_text = "which";                 /* 0398 */
char far *self_replace_text = "An item cannot replace itself!"; /* 039c */
char far *none_to_replace_text = "Nothing was found to replace!"; /* 03a0 */
char far *please_text = "Please";               /* 03a4 */
char far *save_text = "Save";                   /* 03a8 */
char far *memorize_items = "Memorize Forget Leave"; /* 03ac */
char far *dont_save_text = "Don't save";        /* 03b0 */
char far *open_text = "Open";                   /* 03b4 */
char far *view_text = "View";                   /* 03b8 */
char far *select_word = "Select";               /* 03bc */
char far *leave_text = "Leave";                 /* 03c0 */
char far *ok_text = "Ok";                       /* 03c4 */
char far *cancel_text = "Cancel";               /* 03c8 */
char far *name_text = "Name";                   /* 03cc */
char far *rename_text = "Rename";               /* 03d0 */
char far *import_text = "Import";               /* 03d4 */
char far *unimport_text = "Unimport";           /* 03d8 */
char far *copy_text = "Copy";                   /* 03dc */
char far *revert_text = "Revert";               /* 03e0 */
char far *gallery_text = "Gallery";             /* 03e4 */
char far *no_text = "No";                       /* 03e8 */
char far *loaded_text = "Loaded";               /* 03ec */
char far *unavailable_text = "That item is not available."; /* 03f0 */
char far *not_enterable_text = "Not enterable"; /* 03f4 */
char far *possible_text = "possible";           /* 03f8 */
char far *mark_text = "Mark";                   /* 03fc */
char far *entry_word = "Entry";                 /* 0400 */
char far *ed_bar_items = "Select Left Place Right Undo  Mark"; /* 0404 */
char far *event_edit_items = "Ok Prev Next Undo Cancel"; /* 0408 */
char far *chain_items = "Leave Prev Next Edit Insert Delete Alt"; /* 040c */
char far *click_new_entrys_text = "Click new entry's cell wall."; /* 0410 */
char far *design_default_text = "A Game Design"; /* 0414 */
char far *dungeon_fmt = "Dungeon %02d";         /* 0418 */
char far *overland_fmt = "Overland %02d";       /* 041c */
char far *zone_default_fmt = "My zone #%d";     /* 0420 */
char far *nowhere_text = "Nowhere";             /* 0424 */
char far *module_text = "Module";               /* 0428 */
char far *event_word = "Event";                 /* 042c */
char far *zone_word = "Zone";                   /* 0430 */
char far *effect_text = "Effect";               /* 0434 */
char far *day_text = "Day";                     /* 0438 */
char far *night_text = "Night";                 /* 043c */
char far *combat_word = "Combat";               /* 0440 */
char far *slot_text = "Slot";                   /* 0444 */
char far *art_word = "Art";                     /* 0448 */
char far *default_text = "Default";             /* 044c */
char far *current_text = "Current";             /* 0450 */
char far *prev_event_text = "Previous Event";   /* 0454 */
char far *cur_event_text = "Current Event";     /* 0458 */
char far *alt_of_fmt = "(ALT %d of %d)";        /* 045c */
char far *next_event_text = "Next Event";       /* 0460 */
char far *none_text = "None";                   /* 0464 */
char far *called_text = "Called";               /* 0468 */
char far *chain_text = "chain";                 /* 046c */
char far *map_word = "Map";                     /* 0470 */

char far *slot_kind_names[3] = {                /* 0474 */
    "Rest",
    "Step",
    "Shared"
};

char far *normal_text = "Normal";               /* 0480 */
char far *special_text = "Special";             /* 0484 */
char far *text_word = "Text";                   /* 0488 */
char far *evnt_text = "Evnt";                   /* 048c */
char far *used_text = "used";                   /* 0490 */
char far *free_text = "free";                   /* 0494 */
char far *mark_cell_text = "Mark cell wall or"; /* 0498 */
