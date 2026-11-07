/* CKIT.EXE data-only module between charpic.c and packtext.c: the game's
 * text, DGROUP 38e8..54dc -- the menus, prompts, messages and name lists the
 * play modules print, each a far pointer (or a table of them) to a string
 * literal; the literals follow, DGROUP 3f23..54dc, as Turbo C emits them: -d
 * stores an equal literal once and a literal that ends another as that one's
 * tail ("" is the NUL of "is disrupted", "Exit" the end of "Select Exit").
 * Dark Queen of Krynn's gametext.c with FRUA's own words; a pointer no FRUA
 * module reads takes DQK's name for the same text, or one for what it says.
 *
 * The divisions are the readers' where they have them and DQK's otherwise:
 * game_text's six effect messages, camp_menus' four menus then " Level", and
 * item_words' 122 words then four more.  item_words is indexed by a word
 * number from 0; poolinit.c's boots_word is item_words[45], and effects.c
 * reads its weapon names and "+4" from it.  54dd is the link's word-alignment
 * pad before packtext.c's _DATA.  No routines; its empty _TEXT
 * sits at packtext.c's start.
 */

char far *game_text[6] = {                      /* 38e8 */
    "is disrupted",
    "Affected",
    "emits an evil stench",
    "goes berserk",
    "is healed",
    "is stunned"
};

char far *special_label = "   Special :";       /* 3900 */
char far *bonus_label = "     Bonus :";         /* 3904 */
char far *wants_text = "wants to kill ";        /* 3908 */
char far *continue_items = "Press button or enter to continue"; /* 390c */
char far *press_text = "~Press button or enter to continue"; /* 3910 */

char far *class_names[18] = {                   /* 3914  the classes, monk and the multi-classes */
    "Cleric",
    "Knight",
    "Fighter",
    "Paladin",
    "Ranger",
    "Magic-User",
    "Thief",
    "Monk",
    "Cleric/Fighter",
    "Cleric/Fighter/Magic-User",
    "Cleric/Ranger",
    "Cleric/Magic-User",
    "Cleric/Thief",
    "Fighter/Magic-User",
    "Fighter/Thief",
    "Fighter/Magic-User/Thief",
    "Magic-User/Thief",
    "Illegal type"
};

char far *race_names[7] = {                     /* 395c */
    "Elf",
    "Half-Elf",
    "Dwarf",
    "Gnome",
    "Halfling",
    "Human",
    "Monster"
};

char far *align_names[9] = {                    /* 3978 */
    "Lawful Good",
    "Lawful Neutral",
    "Lawful Evil",
    "Neutral Good",
    "True Neutral",
    "Neutral Evil",
    "Chaotic Good",
    "Chaotic Neutral",
    "Chaotic Evil"
};

char far *sex_names[2] = {                      /* 399c */
    "Male",
    "Female"
};

char far *coin_names[3] = {                     /* 39a4 */
    "Platinum",
    "Gems",
    "Jewelry"
};

char far *status_names[9] = {                   /* 39b0 */
    "Okay",
    "Animated",
    "tempgone",
    "Running",
    "Unconscious",
    "Dying",
    "Dead",
    "Petrified",
    "Gone"
};

char far *txt_gains = "gains ";                 /* 39d4 */

char party_items[12] = {                        /* 39d8  party.c's menu entries: 1 shown */
    1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1
};

char far *txt_lose_it = "Lose it? ";            /* 39e4 */
char far *txt_cant_cast_here = "Can't be cast here..."; /* 39e8 */
char far *txt_that_item = "That item";          /* 39ec */
char far *txt_combat_only = "Is a combat-only item..."; /* 39f0 */
char far *txt_use_it = "Use it? ";              /* 39f4 */
char far *txt_casts = "casts";                  /* 39f8 */
char far *txt_abort_spell = "Abort Spell";      /* 39fc */
char far *txt_spell_aborted = "Spell Aborted";  /* 3a00 */
char far *how_many_text = "How many ";          /* 3a04 */
char far *take_text = "will you take? ";        /* 3a08 */
char far *how_much_text = "How much ";          /* 3a0c */
char far *gems_text = "gems : ";                /* 3a10 */
char far *jewelry_text = "jewelry : ";          /* 3a14 */
char far *collection_text = "You have a fine collection of:"; /* 3a18 */
char far *gem_value_text = "The gem is valued at "; /* 3a1c */
char far *jewel_value_text = "The jewel is valued at "; /* 3a20 */
char far *coin_title = "select type of coin ";  /* 3a24 */
char far *trade_to_text = "trade to?";          /* 3a28 */
char far *trade_amount_text = "will you trade? "; /* 3a2c */
char far *stop_resting_text = "Stop resting? "; /* 3a30 */
char far *scribe_scroll_warning = " was going to scribe from that scroll"; /* 3a34 */
char far *items_label = "Items";                /* 3a38 */
char far *ready_label = "Ready Item";           /* 3a3c */
char far *your_text = "Your ";                  /* 3a40 */
char far *gone_text = "will be gone forever";   /* 3a44 */
char far *must_ready_text = "Must be readied";  /* 3a48 */
char far *sale_offer_text = "I'll give you ";   /* 3a4c */
char far *sale_deal_text = "is it a deal? ";    /* 3a50 */
char far *sold_text = "sold!";                  /* 3a54 */
char far *overloaded_pool_text = "overloaded. money will be put in pool."; /* 3a58 */
char far *identify_nothing_new_text = "i cannot tell anything new about your "; /* 3a5c */
char far *identify_kind_text = "it looks like some sort of "; /* 3a60 */
char far *drop_amount_text = "will you drop? "; /* 3a64 */
char far *deposit_amount_text = "will you deposit? "; /* 3a68 */
char far *quit_text = "Quit to DOS? ";          /* 3a6c */
char far *exit_text = "Exit play? ";            /* 3a70 */
char far *exit_unsaved_text = "Game not saved. Quit anyway? "; /* 3a74 */
char far *race_title = "Pick Race";             /* 3a78 */
char far *gender_title = "Pick Gender";         /* 3a7c */
char far *class_title = "Pick Class";           /* 3a80 */
char far *deity_title = "Pick Diety";           /* 3a84 */
char far *alignment_title = "Pick Alignment";   /* 3a88 */
char far *reroll_text = "reroll stats? ";       /* 3a8c */
char far *character_name_text = "character name: "; /* 3a90 */
char far *save_character_text = "save character? "; /* 3a94 */
char far *drop_text = "drop ";                  /* 3a98 */
char far *forever_text = " forever ";           /* 3a9c */
char far *are_you_sure_text = "Are you sure? "; /* 3aa0 */
char far *dumped_text = " is dumped out back."; /* 3aa4 */
char far *farewell_text = " bids you farewell."; /* 3aa8 */
char far *relief_text = " breathe a sigh of relief."; /* 3aac */
char far *cant_modify_text = " can't be modified."; /* 3ab0 */
char far *add_where_text = "add from where? ";  /* 3ab4 */
char far *add_text = "add a character: ";       /* 3ab8 */
char far *delete_text = "delete a character: "; /* 3abc */
char far *cga_unavailable_text = "sorry not in CGA"; /* 3ac0 */
char far *ready_text = "ready    action";       /* 3ac4 */
char far *new_icon_text = "new";                /* 3ac8 */
char far *old_icon_text = "old";                /* 3acc */
char far *icon_confirm_text = "is this icon ok? "; /* 3ad0 */
char far *insufficient_xp_text = "not enough experience"; /* 3ad4 */
char far *will_become_text = " will become:";   /* 3ad8 */
char far *first_level_text = "    a level ";    /* 3adc */
char far *another_level_text = "and a level ";  /* 3ae0 */
char far *train_confirm_text = "does the party wish to train? "; /* 3ae4 */
char far *petition_text = "is petitioning to become a"; /* 3ae8 */
char far *sword_text = "sword";                 /* 3aec */
char far *rose_text = "rose";                   /* 3af0 */
char far *knight_text = "knight of the ";       /* 3af4 */
char far *inexperienced_text = " is too inexperienced"; /* 3af8 */
char far *unqualified_text = " does not qualify."; /* 3afc */
char far *may_become_text = " may become a";    /* 3b00 */
char far *save_disk_text_1 = "is your save disk in drive "; /* 3b04 */
char far *save_disk_text_2 = "is your save disk in drive "; /* 3b08 */
char far *drive_question_text = ":? ";          /* 3b0c */
char far *disk_full_text = "Can't save. Disk is full."; /* 3b10 */
char far *overwrite_text = "overwrite ";        /* 3b14 */
char far *filename_question_text = "? ";        /* 3b18 */
char far *new_filename_text = "new file name : "; /* 3b1c */
char far *selected_text = "has been selected";  /* 3b20 */
char far *speed_text = "Game Speed (0=fastest 9=slowest) = "; /* 3b24 */
char far *camp_text = "the party makes camp..."; /* 3b28 */
char far *not_here_text = "not here";           /* 3b2c */
char far *ally_prompt = "attack ally? ";        /* 3b30 */
char far *range_text = "Aim: range = ";         /* 3b34 */
char far *magic_on_text = "magic on";           /* 3b38 */
char far *magic_off_text = "magic off";         /* 3b3c */
char far *flee_text = "flee ? ";                /* 3b40 */
char far *cant_go_text = "can't go there";      /* 3b44 */
char far *buys_text = " buys ";                 /* 3b48 */
char far *buys_one_text = " buys a ";           /* 3b4c */
char far *not_enough_text = "not enough money."; /* 3b50 */
char far *leave_text1 = "as the party leaves, the shopkeeper says, "; /* 3b54 */
char far *leave_text2 = "\"excuse me, but you have left some money here.\" "; /* 3b58 */
char far *leave_text3 = "Does the party want to go back and retrieve their money?"; /* 3b5c */
char far *fled_text = "the party has fled.";    /* 3b60 */
char far *lost_text = "you have lost the fight."; /* 3b64 */
char far *won_text = "the party has won.";      /* 3b68 */
char far *withdrawal_text = "the party makes a withdrawl. a small"; /* 3b6c */
char far *bank_fee_text = "fee has been assessed by the bank."; /* 3b70 */
char far *treasure_text = "the party has found treasure!"; /* 3b74 */
char far *no_treasure_text = "the party has found no treasure."; /* 3b78 */
char far *receives_text = "each character receives "; /* 3b7c */
char far *xp_text = "experience points.";       /* 3b80 */
char far *left_text = "there is still treasure left. "; /* 3b84 */
char far *claim_text = "do you want to go back and claim it?"; /* 3b88 */
char far *cure_anyway_text = "cast cure anyway? "; /* 3b8c */
char far *pay_cure_text = "pay for cure? ";     /* 3b90 */
char far *cured_text = "is cured.";             /* 3b94 */
char far *not_blind_text = "is not blind.";     /* 3b98 */
char far *not_diseased_text = "is not diseased."; /* 3b9c */
char far *cannot_raise_text = "cannot be raised."; /* 3ba0 */
char far *beyond_help_text = "is beyond our help"; /* 3ba4 */
char far *not_dead_text = "is not dead.";       /* 3ba8 */
char far *not_poisoned_text = "is not poisoned."; /* 3bac */
char far *not_cursed_text = "is not cursed.";   /* 3bb0 */
char far *not_stoned_text = "is not stoned.";   /* 3bb4 */
char far *no_lost_text = "has no lost levels."; /* 3bb8 */
char far *help_you_text = ", how can we help you?"; /* 3bbc */
char far *empty_text = "";                      /* 3bc0 */
char far *title_text = "  Unlimited Adventures"; /* 3bc4 */
char far *version_text = "Version 1.2        June 28,1993"; /* 3bc8 */
char far *txt_choose_spell = "Choose Spell:";   /* 3bcc */
char far *txt_flame_type = "Flame type: ";      /* 3bd0 */
char far *appraise_title = "Appraise : ";       /* 3bd4 */
char far *pick_text = "Pick Character ";        /* 3bd8 */
char far *modify_title = "Modify : ";           /* 3bdc */
char far *init_pointer_text = "Init mouse/joystick: "; /* 3be0 */
char far *lose_character_text = "lose character? "; /* 3be4 */
char far *load_where_text = "load from where? "; /* 3be8 */
char far *load_which_text = "load which game: "; /* 3bec */
char far *save_which_text = "save which game: "; /* 3bf0 */
char far *txt_spells_ok = "spells OK? ";        /* 3bf4 */
char far *txt_scribe_ok = "scribe these spells? "; /* 3bf8 */
char far *order_title = "party order ";         /* 3bfc */
char far *speed_title = "game speed: ";         /* 3c00 */
char far *blank_text = "";                      /* 3c04 */
char far *cursor_keys_text = "(use cursor keys) "; /* 3c08 */
char far *continue_text = "continue battle? ";  /* 3c0c */
char far *movement_left_text = "move/attack, move left = "; /* 3c10 */
char far *items_title = "items : ";             /* 3c14 */
char far *take_title = "take: ";                /* 3c18 */
char far *temple_title = "";                    /* 3c1c */
char far *main_menu_text = "Play Demo Quit";    /* 3c20 */
char far *confirm_items = "Yes No";             /* 3c24 */
char far *select_text = "Select";               /* 3c28 */
char far *select_items = "Select Exit";         /* 3c2c */
char far *txt_cast_exit = "Cast Exit";          /* 3c30 */
char far *txt_memorize_exit = "Memorize Exit";  /* 3c34 */
char far *txt_keep_exit = "Keep Exit";          /* 3c38 */
char far *txt_scribe_exit = "Scribe Exit";      /* 3c3c */
char far *txt_learn_exit = "Learn Exit";        /* 3c40 */
char far *txt_hot_cold = "Hot Cold";            /* 3c44 */
char far *appraise_items = "Gems Jewelry Exit"; /* 3c48 */
char far *sell_items = "Sell Keep";             /* 3c4c */
char far *rest_items = "Rest Days Hours Mins Add Subtract Exit"; /* 3c50 */
char far *view_items = "Items Spells Trade Deposit Drop Lay Cure Exit"; /* 3c54 */
char far *items_items = "Rdy Use Trade Deposit Drop Halve Join Sell Id Exit"; /* 3c58 */
char far *amount_items = "Add Subtract Keep Exit"; /* 3c5c */
char far *add_items = "Add Exit";               /* 3c60 */
char far *delete_items = "Delete Exit";         /* 3c64 */
char far *change_items = "Change Exit";         /* 3c68 */
char far *try_disk_items = "Ok Try another disk"; /* 3c6c */
char far *game_items = "A B C D E F G H I J";   /* 3c70 */
char far *order_items = "Select Place Exit";    /* 3c74 */
char far *speed_items = "Faster Slower Exit";   /* 3c78 */
char far *explore_items = "Move Area Cast View Encamp Search Look Inv"; /* 3c7c */
char far *exit_items = "Exit";                  /* 3c80 */
char far *command_items = "Move Aim Use Cast Turn Guard Quick@Delay Bandage View Speed End Win"; /* 3c84 */
char far *move_items = "Undo Done";             /* 3c88 */
char far *aim_items = "Next Prev Manual Target Center Exit"; /* 3c8c */
char far *target_items = "Target Center Exit";  /* 3c90 */
char far *buy_items = "Buy Exit";               /* 3c94 */
char far *shop_items = "Buy View Take Pool Share Appraise Exit"; /* 3c98 */
char spaces[35] = "                                  "; /* 3c9c */
char far *take_items = "Take Exit";             /* 3cbf */
char far *money_items_menu = "Money Items Exit"; /* 3cc3 */
char far *treasure_items = "View Take Pool Share Detect Exit"; /* 3cc7 */
char far *heal_items = "Heal Exit";             /* 3ccb */
char far *temple_wish_items = "Heal Donate View Pool Take Share Leave"; /* 3ccf */
char far *temple_items = "Heal View Fix Pool Appr. Take Share Give Exit"; /* 3cd3 */
char far *vault_items = "View Take Pool Money Items Exit"; /* 3cd7 */
char far *tavern_items = "Drink Talk Fight Leave"; /* 3cdb */
char far *icon_menus = 0;                       /* 3cdf */

char far *camp_menus[4] = {                     /* 3ce3 */
    "View Magic Rest Alt Fix Load Save Done Exit",
    "Cast Memorize Scribe Display Rest Exit",
    "Order Drop Speed Icon Level Exit",
    "Novice Squire Veteran Adept Champion"
};

char far *level_suffix_text = " Level";         /* 3cf3 */
char far *door_items = "Bash Pick Knock Exit";  /* 3cf7 */
char far *town_items = "Temple Hall Shop Inn Pub Vault Leave"; /* 3cfb */
char far *encounter_items = "Approach Retreat Fight Wait Talk"; /* 3cff */

char far *item_words[122] = {                   /* 3d03 */
    "",                                 /* 0 */
    "Battle Axe",                       /* 1 */
    "Hand Axe",                         /* 2 */
    "Club",                             /* 3 */
    "Dagger",                           /* 4 */
    "Dart",                             /* 5 */
    "Hammer",                           /* 6 */
    "Javelin",                          /* 7 */
    "Mace",                             /* 8 */
    "Morning Star",                     /* 9 */
    "Military Pick",                    /* 10 */
    "Awl Pike",                         /* 11 */
    "Bolt",                             /* 12 */
    "Scimitar",                         /* 13 */
    "Spear",                            /* 14 */
    "Quarter Staff",                    /* 15 */
    "Bastard Sword",                    /* 16 */
    "Broad Sword",                      /* 17 */
    "Long Sword",                       /* 18 */
    "Short Sword",                      /* 19 */
    "Two-Handed Sword",                 /* 20 */
    "Trident",                          /* 21 */
    "Composite Long Bow",               /* 22 */
    "Composite Short Bow",              /* 23 */
    "Long Bow",                         /* 24 */
    "Short Bow",                        /* 25 */
    "Fine",                             /* 26 */
    "Light Crossbow",                   /* 27 */
    "Sling",                            /* 28 */
    "Staff",                            /* 29 */
    "Arrow",                            /* 30 */
    "Leather",                          /* 31 */
    "Ring",                             /* 32 */
    "Scale",                            /* 33 */
    "Chain",                            /* 34 */
    "Banded",                           /* 35 */
    "Plate",                            /* 36 */
    "Shield",                           /* 37 */
    "Cleric",                           /* 38 */
    "Scroll",                           /* 39 */
    "Mage",                             /* 40 */
    "Helm",                             /* 41 */
    "Belt",                             /* 42 */
    "Robe",                             /* 43 */
    "Cloak",                            /* 44 */
    "Boots",                            /* 45 */
    "Ring",                             /* 46 */
    "Mail",                             /* 47 */
    "Armor",                            /* 48 */
    "Of Prot",                          /* 49 */
    "Bracers",                          /* 50 */
    "Wand",                             /* 51 */
    "Elixir",                           /* 52 */
    "Potion",                           /* 53 */
    "Youth",                            /* 54 */
    "Ruby",                             /* 55 */
    "Boulder",                          /* 56 */
    "Dragon Breath",                    /* 57 */
    "Displacement",                     /* 58 */
    "Eyes",                             /* 59 */
    "Drow",                             /* 60 */
    "Elfin Chain",                      /* 61 */
    "Ice Storm",                        /* 62 */
    "Sapphire",                         /* 63 */
    "Emerald",                          /* 64 */
    "Wizardry",                         /* 65 */
    "Hornet's Nest",                    /* 66 */
    "Fire Resistance",                  /* 67 */
    "Stone",                            /* 68 */
    "Good Luck",                        /* 69 */
    "Flail",                            /* 70 */
    "Halberd",                          /* 71 */
    "Gauntlets",                        /* 72 */
    "Periapt",                          /* 73 */
    "Health",                           /* 74 */
    "Cursed",                           /* 75 */
    "Blessed",                          /* 76 */
    "Bundle of",                        /* 77 */
    "Ogre Power",                       /* 78 */
    "Girdle",                           /* 79 */
    "Giant Strength",                   /* 80 */
    "Mirror",                           /* 81 */
    "Necklace",                         /* 82 */
    "Dragon",                           /* 83 */
    "vs Giants",                        /* 84 */
    "vorpal",                           /* 85 */
    "cold resistance",                  /* 86 */
    "Diamond",                          /* 87 */
    "Lightning",                        /* 88 */
    "Fireballs",                        /* 89 */
    "of",                               /* 90 */
    "Vulnerability",                    /* 91 */
    "Speed",                            /* 92 */
    "Silver",                           /* 93 */
    "Extra",                            /* 94 */
    "Healing",                          /* 95 */
    "Charming",                         /* 96 */
    "Fear",                             /* 97 */
    "Magic Missiles",                   /* 98 */
    "Missiles",                         /* 99 */
    "1 Spell",                          /* 100 */
    "2 Spells",                         /* 101 */
    "3 Spells",                         /* 102 */
    "Paralyzation",                     /* 103 */
    "Invisibility",                     /* 104 */
    "Cute Yellow Canary",               /* 105 */
    "AC 10",                            /* 106 */
    "AC 6",                             /* 107 */
    "AC 4",                             /* 108 */
    "AC 3",                             /* 109 */
    "AC 2",                             /* 110 */
    "+1",                               /* 111 */
    "+2",                               /* 112 */
    "+3",                               /* 113 */
    "+4",                               /* 114 */
    "+5",                               /* 115 */
    "-1",                               /* 116 */
    "-2",                               /* 117 */
    "-3",                               /* 118 */
    "Electric Immunity",                /* 119 */
    "Gaze Resistance",                  /* 120 */
    "Spiritual"                         /* 121 */
};

char far *gem_text = "Gem";                     /* 3eeb */
char far *item_word_jewelry = "Jewelry";        /* 3eef */
char far *item_word_blinking = "blinking";      /* 3ef3 */
char far *item_word_from_evil = "from evil";    /* 3ef7 */
char far *old_text = "who is looking very old"; /* 3efb */
char far *strain_text = "the strain of the last battle has worn heavily on "; /* 3eff */
char far *breath_text = "breathes deeply one last time and crumples to the ground."; /* 3f03 */
char far *he_text = "he";                       /* 3f07 */
char far *she_text = "she";                     /* 3f0b */
char far *age_death_text = "dies from age";     /* 3f0f */
char far *no_witness_text = "Sadly, no party member witnesses the passing of the champion."; /* 3f13 */
char far *one_witness_text = " weeps at the loss of such a venerable companion."; /* 3f17 */
char far *many_witness_text = " The remaining party members are overcome with grief as they search "; /* 3f1b */
char far *search_body_text = "'s body for valuables."; /* 3f1f */
