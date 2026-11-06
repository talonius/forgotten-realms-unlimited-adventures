/* CKIT.EXE overlay 8 module 21d4: the map events' commands.  The event
 * setup, the menus an event shows (hot-keyed horizontal ones and a
 * vertical list built from a "~"-marked string), damage messages, the
 * view refresh after an event, the monster pictures and their approach,
 * loading monsters and wandering monsters into the combat list, and the
 * handlers module 1cff of section 4 (mapevent.c) runs through vector
 * stubs: combat, an encounter's menu, damage, the temple, treasure, a
 * password, experience, training, a walk, a yes/no question, an ability
 * or skill check, paying money, an NPC joining, picking a member by id,
 * the inn, the vault, sounds and a text menu.
 *
 * EXE 0x041090..0x04480e, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 21d4, the first module of RTLink section 8 (payload
 * 0x41090, link segment 21d4, parent section 3).  Its final retf is
 * 21d4:377d, byte d of paragraph 254b, where move.c begins with the lone
 * retf at 254b:000e; section 8's table files all 320 fixup sites in the
 * range under frame 21d4 and none after it.  Built with Turbo C 2.01, tcc
 * -ml -O -Z -d.  Functions are defined in shipped order; bake consumes
 * them in that order.  Sections 6, 7, 9 and 10 hold other bytes at link
 * segment 21d4.
 *
 * A bare stored seg:off in this file means section 8.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and an ovlNN_ name which section and stored
 * segment.  A mod18de_ callee is an RTLink vector stub; its comment names
 * the routine the stub reaches.
 *
 * Code outside section 8 reaches 28 routines only through vector stubs:
 * 18de:155d -> 0001, 15b7 -> 0208, 1549 -> 074e, 157b -> 07b4, 1585 ->
 * 0959, 153f -> 09a8, 15df -> 0a09, 15d5 -> 0a1e, 1553 -> 0a33, 158f ->
 * 133a, 1657 -> 1444, 1625 -> 186b, 1643 -> 1c3e, 1611 -> 1e60, 164d ->
 * 2550, 1689 -> 2665, 1675 -> 27b5, 1607 -> 29b7, 1639 -> 2ab8, 1661 ->
 * 2d26, 169d -> 2ef9, 1693 -> 321a, 162f -> 33b8, 166b -> 3469, 15fd ->
 * 353b, 15f3 -> 35df, 161b -> 3662, 167f -> 36a4; mapevent.c and
 * saveload.c call them by those stub names.  movement.c far-calls 09a8 three
 * times as event_print.  The other routines are reached only by near
 * calls after their definitions, so the bytes do not decide static or
 * public for them; nothing reaches the lone retf evcmds_unit_init.
 *
 * Mostly FRUA's own code on the ancestry of Dark Queen of Krynn's
 * eclcmds.c (dqk 893e699): the sharing map pairs party_hp_span shape and
 * menu_horiz, menu_vert, inflict_damage, view_update, hero_load,
 * monster_dist and wander_load "modified" with DQK's routines of those
 * names; monster_add is DQK's monster_add with FRUA's records (the map
 * gives it wander_load as its top partner).  The handlers from
 * event_combat on pair with DQK only partial or not at all: FRUA's own
 * code, written from the bytes.  FRUA's changes in the paired routines:
 * menu_opts takes no option string, handles "^" marks and doubled marks,
 * and has no digit class; menu_horiz has no hotkey_reset and waits through press_return_wait;
 * inflict_damage starts a new line, pauses past row 0x15, clears no text
 * and redraws through 18de:1067; view_update refreshes unconditionally;
 * hero_load takes a fifth argument that clears the side byte;
 * monster_dist does not set cursor_obj[0x38]; wander_load takes its
 * monster from the area record (+106) where DQK reads a table, and stops
 * at 0x3c combatants, monster_add at 0x32; both size the icon with
 * icon_size; the Hero record is 0x18e bytes, Item 0x3e and Effect 0xa.
 *
 * _DATA is DGROUP 73e2..75db: encounter_range (public: mapevent.c reads it) and
 * the static encounter_surprise, both 0xff, the initializers of menu_opts' and
 * menu_list's local arrays, then the literals, 26 distinct stored once
 * each -- "" first in source order, so it has its own byte.  73e1 is a
 * zero pad byte after the literals section 7 references at 72ae..73e0
 * (module 25e1's, not landed), and move.c's _DATA begins at 75dc.  The
 * module has no _BSS.
 */

struct Pool;                    /* strutil.c's                            */
struct Pic;                     /* picview.c's                            */

struct Spot {                   /* a place in an area                     */
    unsigned char y;            /* +0                                     */
    unsigned char x;            /* +1                                     */
    unsigned char facing;       /* +2  low 3 bits                         */
    unsigned char which;        /* +3   not read here                     */
};

struct Area {                   /* what cur_area_rec points at                */
    char pad_0[7];              /* +0   not read here                     */
    unsigned char flags;        /* +7   bit 0                             */
    char pad_8[6];              /* +8   not read here                     */
    struct Spot spots[8];       /* +e                                     */
    char pad_2e[0xd8];          /* +2e  not read here                     */
    unsigned char wander_id;    /* +106                                   */
    unsigned char level;        /* +107                                   */
};

struct Geo {                    /* the party's place and the view state   */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
    char facing_wall;           /* +3   d417                              */
    char event;                 /* +4   d418                              */
    char pad_5[2];              /* +5   not read here                     */
    char show_map;              /* +7   d41b                              */
};

struct Effect {                 /* 10 bytes, from effect_pool               */
    char pad_0[6];              /* +0   not read here                     */
    struct Effect far *next;    /* +6                                     */
};

struct Item {                   /* 0x3e bytes, from item_record_pool             */
    struct Item far *next;      /* +0                                     */
    char pad_4[0x3a];           /* +4   not read here                     */
};

struct Hero {                   /* a party member, 0x18e bytes            */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x38];           /* +c   not read here                     */
    long xp;                    /* +44                                    */
    char pad_48[4];             /* +48  not read here                     */
    unsigned coins[3];          /* +4c  platinum, gems, jewelry           */
    char pad_52[8];             /* +52  not read here                     */
    unsigned char undead;       /* +5a                                    */
    char pad_5b[3];             /* +5b  not read here                     */
    unsigned char status;       /* +5e                                    */
    unsigned char side;         /* +5f                                    */
    char name[0x11];            /* +60                                    */
    unsigned char str;          /* +71                                    */
    char pad_72;                /* +72  not read here                     */
    unsigned char intel;        /* +73                                    */
    char pad_74;                /* +74  not read here                     */
    unsigned char wis;          /* +75                                    */
    char pad_76;                /* +76  not read here                     */
    unsigned char dex;          /* +77                                    */
    char pad_78;                /* +78  not read here                     */
    unsigned char con;          /* +79                                    */
    char pad_7a;                /* +7a  not read here                     */
    unsigned char cha;          /* +7b                                    */
    unsigned char str_ex;       /* +7c                                    */
    char pad_7d[4];             /* +7d  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    unsigned char icon_dim;     /* +82  bit 7 kept, low bits icon size    */
    char pad_83[8];             /* +83  not read here                     */
    unsigned char thief[8];     /* +8b  percent skills; [1] picks locks   */
    unsigned char control;      /* +93                                    */
    char pad_94[9];             /* +94  not read here                     */
    unsigned char level[7];     /* +9d                                    */
    char pad_a4[0x19];          /* +a4  not read here                     */
    unsigned char icon_slot;    /* +bd                                    */
    char pad_be[7];             /* +be  not read here                     */
    char level_up;              /* +c5                                    */
    char pad_c6[0xb8];          /* +c6  not read here                     */
    char active;                /* +17e                                   */
    char pad_17f[0xc];          /* +17f not read here                     */
    unsigned char hp;           /* +18b                                   */
    unsigned char movement;     /* +18c                                   */
    unsigned char mon_id;       /* +18d                                   */
};

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Node {                   /* a list_menu entry                      */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far bank_text_get(char far *pool, int slot, char far *out); /* 03ad:0141 */
extern unsigned char far calc_move(struct Hero far *h);          /* 03d0:0a38 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far hero_damage(struct Hero far *h, unsigned short dmg); /* 03d0:1792 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern void far hero_drop(char keep, char icon);                 /* 03d0:1e69 */
extern char far can_level_up(struct Hero far *h);                   /* 03d0:1f71 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern char far *far input_text(char far *prompt, unsigned char fg, unsigned char bg,
                                unsigned char max);              /* 06c9:05bc */
extern void far continue_prompt(char a, char b, char far *items); /* 06c9:0932 */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern struct Node far *far get_selected(void);                  /* 083a:0327 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far press_return_clear(void);                        /* 083a:053a */
extern void far press_return_wait(char anim);                            /* 083a:054a */
extern void far list_alloc(int n, struct Node far * far *head);  /* 083a:0575 */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far node_prompt(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1a07 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far int_to_str(int value, char far *buf);            /* 0a75:000d */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far str_insert(char far *s, char far *t, int pos);   /* 0a75:0164 */
extern void far str_delete(char far *s, int pos, int n);         /* 0a75:01c4 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far icon_load(char far *name, unsigned char set, unsigned char idx);
                                                                 /* 0cb5:0093 */
extern void far icon_size(unsigned char idx, unsigned char bank, int far *rows,
                          int far *cols);                        /* 0cb5:035f */
extern void far txt_get(char far *pool, int slot, char far *out); /* 0cf0:046e */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_main(void);                                /* 0d65:00de */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far view_refresh(void);     /* 18de:1053, vector to overlay 4 1c86:0000 */
extern void far party_info_display(void);     /* 18de:105d, vector to overlay 4 1c86:0406 */
extern void far party_display(void far *node);
                                        /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                        /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);
                                        /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern void far combat_main(void);     /* 18de:10df, vector to overlay 0 1b41:05a2 */
extern void far char_view(char far *done);
                                        /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern int  far fx_roll_damage(unsigned char n, unsigned char sides);
                                        /* 18de:12ab, vector to overlay 19 2e88:1624 */
extern char far fx_saving_throw(struct Hero far *h, int type, char adj);
                                        /* 18de:1323, vector to overlay 19 2e88:1552 */
extern char far wall_side(char x, char y, char dir);
                                        /* 18de:1341, vector to overlay 13 26e3:09f3 */
extern char far party_menu(char on);  /* 18de:13ff, vector to overlay 9 24ad:04bf */
extern void far prev_pos_save(void);     /* 18de:1409, vector to overlay 4 1cff:03c7 */
extern void far view_draw(char x, char y, unsigned char dir);
                                        /* 18de:1495, vector to overlay 13 26e3:0bdb */
extern void far wild_pic_open(void);     /* 18de:14a9, vector to overlay 13 26e3:17e2 */
extern void far event_view_restore(void);     /* 18de:14bd, vector to overlay 4 1cff:024e */
extern void far event_pic_show(unsigned char far *ev);
                                        /* 18de:14c7, vector to overlay 4 1cff:0670 */
extern void far party_pos_set(void);     /* 18de:14d1, vector to overlay 4 1cff:042b */
extern char far party_has_npc(unsigned char kind);
                                        /* 18de:14db, vector to overlay 4 1cff:0302 */
extern void far le_long_put(long n, unsigned char far *p);
                                        /* 18de:14e5, vector to overlay 4 1cff:048c */
extern void far le_word_put(unsigned n, unsigned far *p);
                                        /* 18de:14ef, vector to overlay 4 1cff:0473 */
extern long far le_long_get(unsigned char far *p);
                                        /* 18de:14f9, vector to overlay 4 1cff:04e9 */
extern unsigned far le_word_get(unsigned char far *p);
                                        /* 18de:1503, vector to overlay 4 1cff:04c8 */
extern char far event_text_show(unsigned char far *ev);
                                        /* 18de:150d, vector to overlay 4 1cff:0280 */
extern void far enter_camp(char check);
                                        /* 18de:1517, vector to overlay 4 1cff:08f4 */
extern void far event_view_draw(void);     /* 18de:1521, vector to overlay 4 1cff:0387 */
extern char far area_cell_at(char x, char y);
                                        /* 18de:15ad, vector to overlay 13 26e3:0ae7 */
extern void far geo_load(unsigned char pic);
                                        /* 18de:15e9, vector to overlay 13 26e3:1750 */
extern void far mon_load(unsigned char id, char recs, struct Hero far *hero);
                                        /* 18de:16f7, vector to overlay 4 1ffa:0e01 */
extern void far mon_add(unsigned char id);
                                        /* 18de:1715, vector to overlay 4 1ffa:1248 */
extern void far post_combat(void);     /* 18de:1a2b, vector to overlay 10 22ef:1290 */
extern char far fx_attack_roll_no_attacker(struct Hero far *h, unsigned char need);
                                        /* 18de:1a35, vector to overlay 19 2e88:13ac */
extern char far temple_shop(unsigned char far *ev, char sub, int a, int b, int c);
                                        /* 18de:1a3f, vector to overlay 10 254c:10fe */
extern void far pool_check(char far *has_coins, char far *has_items);
                                        /* 18de:1a49, vector to overlay 10 21d4:09ab */
extern void far share_money(void);     /* 18de:1a53, vector to overlay 10 21d4:01ee */
extern void far take_menu(void);     /* 18de:1a5d, vector to overlay 10 21d4:0701 */
extern void far pool_money(void);     /* 18de:1a67, vector to overlay 10 21d4:00f6 */
extern void far treasure_item_add(int id, char known);
                                        /* 18de:1a71, vector to overlay 10 2431:1088 */
extern long far rate_price(long price, unsigned char rate, unsigned count);
                                        /* 18de:1a7b, vector to overlay 10 254c:0063 */
extern void far pick_hero(char far *prompt, char a, char b, struct Hero far * far *out);
                                        /* 18de:1a85, vector to overlay 14 28aa:3fb9 */
extern void far vault_menu(void);     /* 18de:1a8f, vector to overlay 10 2431:08e0 */
extern void far set_data_disk(char disk);                         /* overlay 3 1b41:0083 */
extern void far mode_change(int mode);                           /* overlay 3 1b41:02fe */
extern void far pic_frame(int col, int row, char show, unsigned char n);
                                                                 /* overlay 3 1bbc:000b */
extern void far pic_open(char far *name, unsigned char id, char unused, struct Pic far *p);
                                                                 /* overlay 3 1bbc:0077 */
extern void far pic_release(void);                           /* overlay 3 1bbc:02b4 */
extern void far pic_show(unsigned char n);                       /* overlay 3 1bbc:02e9 */
extern void far step_dungeon(void);                              /* 254b:04a1 */

extern char far *continue_items;                                 /* 390c */
extern char far *press_text;            /* 3910, "~Press button or enter to continue" */
extern char far *leave_text3;           /* 3b5c, "Does the party want to go back and ..." */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *temple_title;                                       /* 3c1c */
extern char far *select_text;           /* 3c28, "Select"                 */
extern char far *temple_wish_items;     /* 3ccf, "Heal Donate View Pool Take Share Leave" */
extern char far *encounter_items;       /* 3cff, "Approach Retreat Fight Wait Talk" */
extern char rerun_square;                                           /* 5b3f */
extern char last_pic;                                           /* 5b54 */
extern int cur_level;                                            /* 6719 */
extern char view_restore_due;                                           /* 671b */
extern char in_service_event;                                           /* 671c */
extern long train_cost;                                          /* 671e */
extern char combat_outdoors;                                           /* 6722 */
extern char big_pic_shown;                                           /* 6724 */
extern char far *text_pool_block;                                /* a7ec */
extern unsigned char transfer_dest_x;                                   /* b255 */
extern unsigned char transfer_dest_y;                                   /* b256 */
extern char msg_buf[];                                          /* b259 */
extern char pic_up;                                           /* b363 */
extern char monster_table[12];                                       /* b369 */
extern char step_back_due;                                           /* b375 */
extern char party_has_moved;                                          /* b378 */
extern struct Hero far *cur_node;                                /* b3dd */
extern unsigned char next_monster_group;                                  /* b3e1 */
extern char tick_slot;                                           /* b3f2 */
extern char redrawn;                                             /* b3f3 */
extern char turn_done;                                           /* b414 */
extern unsigned char text_col;                                   /* b419 */
extern unsigned char text_row;                                   /* b41a */
extern int scratch_word;                                            /* b41f */
extern int view_mode;                                            /* b421 */
extern char pooled;                                              /* b424 */
extern char confirm_anim;                                        /* b42a */
extern char text_slow;                                           /* b431 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char exploration_menu_context;                                           /* b4e8 */
extern char event_pic;                                           /* b727 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char picture_hides_roster;                                           /* b761 */
extern long pool[3];                                             /* b8d6 */
extern char combat_outcome[4];                                         /* b8ee */
extern char event_text_cleared;                                           /* b8f4 */
extern char encounter_sprite_shown;                                           /* bcf0 */
extern char adventure_exit;                                           /* bcf6 */
extern struct Pool npc_pool;                                    /* bd01 */
extern char event_view_dirty;                                           /* be65 */
extern char event_initial_redraw;                                           /* c3be */
extern char monsters_queued;                                           /* c3c1 */
extern struct Pool effect_pool;                                    /* c3c4 */
extern char event_sprite;                                           /* c671 */
extern char shop_temple_frame;                                           /* c672 */
extern char character_select_failed;                                           /* ca0f */
extern unsigned char enemy_quarter_mask;                                  /* ca1a */
extern unsigned char ai_level;                                   /* ca21 */
extern struct Pic pic;                                           /* ca22 */
extern char menu_escaped;                                        /* ca2a */
extern unsigned char entry_spot;                                  /* cb09 */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char test_mode;                                           /* cc7a */
extern char byte_cfdd;                                           /* cfdd */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern unsigned char encounter_npc_count;                                  /* d135 */
extern unsigned char encounter_images_shown[2];                                /* d139 */
extern struct Geo geo;                                           /* d414 */
extern struct Area far *cur_area_rec;                                /* d41c */

static unsigned char encounter_surprise = 0xff;                           /* 73e2 */
unsigned char encounter_range = 0xff;                                  /* 73e3 */

/* The translated Pascal unit's empty initialization block. */
void far evcmds_unit_init(void)                                       /* 31d4:0000 */
{
}

void far event_init(void)                                        /* 31d4:0001 */
{
    confirm_anim = 0;
    event_initial_redraw = 1;
    mem_fill(encounter_images_shown, 2, 0);
    next_monster_group = 8;
    turn_done = 0;
    mem_fill(combat_outcome, 4, 0);
    cursor_obj[0x2c] = 0;
    if (cur_area_rec->flags & 1)
        cursor_obj[0x1a] = 0;
    else
        cursor_obj[0x1a] = 1;
    for (tick_slot = 0; tick_slot <= 7; tick_slot++)
        le_word_put(0, (unsigned far *)(cursor_obj + tick_slot * 2 + 0x1fd));
}

void far menu_opts(char far *menu, unsigned char far *count)     /* 31d4:0099 */
{
    unsigned char i;
    char space[2] = " ";
    char key[6] = "A";
    unsigned char n;

    *count = 0;
    n = 0;
    scratch_word = 0;
    for (i = 0; i < str_length(menu); i++) {
        if (menu[i] >= 'A' && menu[i] <= 'Z')
            menu[i] = menu[i] + 0x20;
        if (menu[i] == '~' || menu[i] == '^') {
            if (menu[i + 1] == '~' || menu[i + 1] == '^')
                str_insert(key, menu, i + 2);
            else
                hotkey_add(scratch_word, &n);
            if (menu[i] == '^') {
                menu[i] = '~';
                str_insert(space, menu, i + 1);
                i++;
            }
            str_delete(menu, i + 1, 1);
            (*count)++;
            scratch_word++;
        }
    }
}

unsigned char far menu_horiz(char far *title, char far *menu, char enter_ok,
                             char anim)                          /* 31d4:0208 */
{
    unsigned char ch;
    unsigned char count;
    unsigned char result;
    char pause;

    if (str_same(press_text, menu))
        pause = 1;
    else
        pause = 0;
    menu_opts(menu, &count);
    if (pause) {
        press_return_wait(anim);
        result = 0;
    } else {
        do {
            ch = menu_run(title, menu, 1, anim);
            if (menu_escaped && ch != 0x1b) {
                if (view_mode != 3 && cursor_obj[0x22] == 1) {
                    party_rows(cur_node, 0);
                    party_key_select(ch);
                    party_rows(cur_node, 1);
                }
                ch = 0xff;
            }
        } while (!(ch < count || (ch == 0x1b && menu_escaped && enter_ok)));
        if (ch == 0x1b)
            result = 0;
        else
            result = ch;
    }
    return result;
}

void far menu_vert(char far *title, char far *menu, char row, char col,
                   char row2, char col2, struct Node far *list, char a,
                   char far *redraw, int far *top,
                   struct Node far * far *out)                   /* 31d4:0302 */
{
    char text[51];
    unsigned char count;

    str_copy(text, menu);
    menu_opts(text, &count);
    hotkey_reset(0);
    list_menu(title, select_text, row, col, row2, col2, list, a, 0, redraw, top,
              out);
}

int far menu_list(char far *menu)                                /* 31d4:0379 */
{
    unsigned char j;
    unsigned char i;
    struct Node far *list;
    struct Node far *head;
    int top;
    char redraw;
    char n;
    char k;
    char first = 0;
    unsigned char keys[5] = { 0 };

    ui_mode = 1;
    event_text_cleared = 0;
    redraw = 1;
    j = 1;
    i = 1;
    n = 0;
    k = 0;
    turn_done = 0;
    while (!turn_done) {
        if (str_length(menu) <= i)
            turn_done = 1;
        else {
            if (menu[i] == '~' || menu[i] == '^')
                k++;
            else if (menu[i] != 0) {
                for (j = 0; menu[i] != '~' && menu[i] != '^' && menu[i] != 0; i++, j++)
                    ;
                i--;
                n++;
            }
            i++;
        }
    }
    list_alloc(n, &list);
    head = list;
    text_col = 1;
    text_row = 0x11;
    i = 1;
    n = 0;
    k = 0;
    first = 1;
    while (list != 0) {
        if (str_length(menu) > i) {
            if (menu[i] == '~' || menu[i] == '^') {
                k++;
                if (first) {
                    str_delete(menu, i, 1);
                    i--;
                }
                first = 1;
            } else {
                first = 0;
                if (menu[i] != 0) {
                    for (j = 0; menu[i] != '~' && menu[i] != '^' && menu[i] != 0; i++, j++)
                        list->text[j] = menu[i];
                    list->text[j] = 0;
                    list->key = 0;
                    list = list->next;
                    i--;
                    keys[n] = k;
                    n++;
                }
            }
            i++;
        }
    }
    list = head;
    top = 0;
    menu_vert("", "", 3, text_row + 1, 0x26, 0x17, list, 0, &redraw, &top, &head);
    top = keys[top];
    list_free(&list);
    text_clear(1, 0x11, 0x26, 0x16);
    return top;
}

void far inflict_damage(short dmg, struct Hero far *h)           /* 31d4:062a */
{
    char msg[82];
    char num[5];
    char pause;

    if (h->active == 0)
        return;
    pause = 0;
    int_to_str(dmg, num);
    if (h->hp + 10 < dmg)
        str_format(msg, "%s%s", h->name, " dies. ");
    else
        str_copy(msg, msg_format("%s%s%s%s", h->name, " is hit FOR ", num,
                                     " points of Damage."));
    if (text_col != 1) {
        text_col = 1;
        text_row++;
    }
    if (text_row > 0x15) {
        text_row = 0x11;
        pause = 1;
        continue_prompt(0xb, 0, continue_items);
    } else
        pause = 0;
    text_wrap(1, 0x11, 0x26, 0x16, 0xb, 0, pause, msg);
    hero_damage(h, dmg);
    if (h->status == 5)
        h->status = 4;
    party_display(cur_node);
}

void far view_update(void)                                       /* 31d4:074e */
{
    geo.event = area_cell_at(geo.x, geo.y);
    mem_fill(encounter_images_shown, 2, 0);
    redrawn = 1;
    view_refresh();
    party_info_display();
    byte_cfdd = 0;
    event_initial_redraw = 0;
    event_view_dirty = 0;
    confirm_anim = 0;
    encounter_sprite_shown = 0;
    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
}

void far monster_show(unsigned char sprite, unsigned char picture,
                      unsigned char dist, char far *shown)       /* 31d4:07b4 */
{
    char size;

    size = dist + 1;
    if (!shown[1] && sprite != 0xff) {
        screen_flush_mode_set(1);
        if (!shown[0]) {
            if (geo.show_map) {
                geo.show_map = 0;
                redrawn = 1;
                view_refresh();
            }
            if (cursor_obj[0x22]) {
                redrawn = 1;
                view_refresh();
                pic_open("SPRIT", sprite, 1, &pic);
                shown[0] = 1;
                encounter_sprite_shown = 1;
            }
        } else {
            redrawn = 1;
            view_refresh();
        }
        if (view_mode == 4)
            pic_show(size);
        screen_flush_mode_set(0);
    }
    if (!shown[1] && picture != 0xff && (dist == 0 || sprite == 0xff)
            && view_mode == 4 && !big_pic_shown) {
        confirm_anim = 1;
        pic_open("PIC", picture, 0, &pic);
        shown[1] = 1;
        pic_frame(3, 3, 1, 0);
        if (cursor_obj[0x85])
            view_restore_due = 1;
    } else if (!shown[1] && picture != 0xff && (dist == 0 || sprite == 0xff)
            && (view_mode == 3 || big_pic_shown)) {
        frame_main();
        party_display(cur_node);
        party_info_display();
        confirm_anim = 1;
        pic_open("PIC", picture, 0, &pic);
        shown[1] = 1;
        pic_frame(3, 3, 1, 0);
        view_restore_due = 1;
    }
}

void far monster_approach(void)                                  /* 31d4:0959 */
{
    while (cursor_obj[0x37] > 0) {
        delay(600);
        cursor_obj[0x37]--;
        screen_flush_mode_set(2);
        monster_show(event_sprite, event_pic, cursor_obj[0x37], encounter_images_shown);
        screen_flush_mode_set(0);
    }
}

void far event_print(char far *text)                         /* 31d4:09a8 */
{
    unsigned char color;

    color = cursor_obj[0x39];
    if (color)
        color = 3;
    else
        color = 7;
    event_text_cleared = 0;
    text_slow = 1;
    text_row = 0x11;
    text_col = 1;
    text_wrap(1, 0x11, 0x26, 0x16, color, 0, 0, text);
    text_slow = 0;
}

void far view_map(void)                                          /* 31d4:0a09 */
{
    cursor_obj[0x22] = 0;
    cursor_obj[0x24] = 1;
    view_mode = 3;
}

void far view_3d(void)                                           /* 31d4:0a1e */
{
    cursor_obj[0x22] = 1;
    cursor_obj[0x24] = 0;
    view_mode = 4;
}

void far area_enter(char n)                                      /* 31d4:0a33 */
{
    set_data_disk(0x33);
    if (cursor_obj[0x86] == 0) {
        if (!test_mode) {
            geo_load(cur_level);
            geo.x = cur_area_rec->spots[entry_spot].x;
            geo.y = cur_area_rec->spots[entry_spot].y;
            geo.facing = cur_area_rec->spots[entry_spot].facing & 7;
        }
        if (cur_level <= 4) {
            cursor_obj[0x25] = geo.x;
            cursor_obj[0x26] = geo.y;
            cursor_obj[0x43] = geo.x;
            cursor_obj[0x44] = geo.y;
            cursor_obj[0x17] = geo.x;
            cursor_obj[0x18] = geo.y;
            view_map();
        } else
            view_3d();
    } else {
        if (!test_mode) {
            geo.y = cursor_obj[0x44];
            geo.x = cursor_obj[0x43];
            geo.facing = cursor_obj[0x11];
        }
        if (cur_level <= 4)
            view_map();
        else
            view_3d();
    }
}

void far hero_load(char id, struct Hero far * far *h,
                   struct Item far * far *items,
                   struct Effect far * far *fx, char ally)       /* 31d4:0b0d */
{
    *h = 0;
    pool_alloc((void far * far *)h, 0x18e, &npc_pool);
    mon_load(id, 0, *h);
    *items = (*h)->items;
    *fx = (*h)->effects;
    if (ally)
        (*h)->side = 0;
}

void far monster_add(char id, unsigned char count, unsigned char set,
                     unsigned char icon, char ally)              /* 31d4:0b8c */
{
    int rows;
    int cols;
    unsigned char copies;
    struct Hero far *tail;
    struct Hero far *clone;
    struct Hero far *cr;
    struct Hero far *saved;
    struct Item far *items;
    struct Item far *prev_item;
    struct Item far *items_head;
    struct Effect far *fx;
    struct Effect far *prev_fx;
    struct Effect far *fx_head;

    saved = cur_node;
    copies = 1;
    if (encounter_npc_count < 0x32) {
        tail = 0;
        clone = 0;
        hero_load(id, &cr, &items, &fx, ally);
        if (cr->side == 1 && cursor_obj[0x27] > 0 && cursor_obj[0x27] < 6) {
            cr->hp = cr->hp * (cursor_obj[0x27] + 1) / 4;
            cr->hp_max = cr->hp_max * (cursor_obj[0x27] + 1) / 4;
            if (cr->hp == 0 || cr->hp_max == 0) {
                cr->hp = 1;
                cr->hp_max = 1;
            }
        }
        if (cr->undead)
            ai_level++;
        items_head = items;
        fx_head = fx;
        if (count <= 0)
            count = 1;
        tail = node_list;
        icon_load("CPIC", set, icon);
        icon_size(icon, 0, &rows, &cols);
        cr->icon_dim = (cr->icon_dim & 0x80) | (cols * 2 + rows - 2);
        cr->icon_slot = icon;
        while (tail->next != 0)
            tail = tail->next;
        tail->next = cr;
        tail = tail->next;
        tail->next = 0;
        encounter_npc_count++;
        copies++;
        while (copies <= count && encounter_npc_count < 0x32) {
            pool_alloc((void far * far *)&clone, 0x18e, &npc_pool);
            tail->next = clone;
            tail = tail->next;
            *tail = *cr;
            tail->next = 0;
            tail->effects = 0;
            tail->items = 0;
            copies++;
            encounter_npc_count++;
            while (items != 0) {
                if (tail->items == 0) {
                    pool_alloc((void far * far *)&tail->items, 0x3e, &item_record_pool);
                    *tail->items = *items;
                    tail->items->next = 0;
                } else {
                    prev_item = tail->items;
                    pool_alloc((void far * far *)&tail->items, 0x3e, &item_record_pool);
                    *tail->items = *items;
                    tail->items->next = prev_item;
                }
                items = items->next;
            }
            items = items_head;
            while (fx != 0) {
                if (tail->effects == 0) {
                    pool_alloc((void far * far *)&tail->effects, 0xa, &effect_pool);
                    *tail->effects = *fx;
                    tail->effects->next = 0;
                } else {
                    prev_fx = tail->effects;
                    pool_alloc((void far * far *)&tail->effects, 0xa, &effect_pool);
                    *tail->effects = *fx;
                    tail->effects->next = prev_fx;
                }
                fx = fx->next;
            }
            fx = fx_head;
        }
        cur_node = saved;
    }
}

void far monsters_place(unsigned char far *ev)                   /* 31d4:0f46 */
{
    char ally;
    register int i;

    ally = 0;
    for (i = 0; i + 8 < next_monster_group; i++)
        if (monster_table[i]) {
            switch (i) {
            case 3:
                ally = ev[0xa] & 0x20;
                break;
            case 4:
                ally = ev[0xa] & 0x40;
                break;
            case 5:
                ally = ev[0xa] & 0x80;
                break;
            default:
                ally = 0;
            }
            monster_add(monster_table[i], monster_table[i + 6], monster_table[i], i + 8, ally);
        }
}

void far wander_load(void)                                       /* 31d4:0fd0 */
{
    int rows;
    int cols;
    unsigned char id;
    unsigned char count;
    unsigned char set;
    unsigned char copies;
    struct Hero far *tail;
    struct Hero far *clone;
    struct Hero far *cr;
    struct Hero far *saved;
    struct Item far *items;
    struct Item far *prev_item;
    struct Item far *items_head;
    struct Effect far *fx;
    struct Effect far *prev_fx;
    struct Effect far *fx_head;

    if (cur_area_rec->wander_id) {
        saved = cur_node;
        copies = 1;
        tail = 0;
        clone = 0;
        id = cur_area_rec->wander_id;
        hero_load(id, &cr, &items, &fx, 0);
        cr->status = 9;
        cr->control = cr->control | 0x32;
        cr->side = cursor_obj[0x3a] == 5 ? 1 : 0;
        items_head = items;
        fx_head = fx;
        count = 10;
        set = id;
        tail = node_list;
        icon_load("CPIC", set, next_monster_group);
        icon_size(next_monster_group, 0, &rows, &cols);
        cr->icon_dim = (cr->icon_dim & 0x80) | (cols * 2 + rows - 2);
        cr->icon_slot = next_monster_group;
        while (tail->next != 0)
            tail = tail->next;
        tail->next = cr;
        tail = tail->next;
        tail->next = 0;
        encounter_npc_count++;
        copies++;
        while (copies <= count && encounter_npc_count < 0x3c) {
            pool_alloc((void far * far *)&clone, 0x18e, &npc_pool);
            tail->next = clone;
            tail = tail->next;
            *tail = *cr;
            tail->next = 0;
            tail->effects = 0;
            tail->items = 0;
            copies++;
            encounter_npc_count++;
            while (items != 0) {
                if (tail->items == 0) {
                    pool_alloc((void far * far *)&tail->items, 0x3e, &item_record_pool);
                    *tail->items = *items;
                    tail->items->next = 0;
                } else {
                    prev_item = tail->items;
                    pool_alloc((void far * far *)&tail->items, 0x3e, &item_record_pool);
                    *tail->items = *items;
                    tail->items->next = prev_item;
                }
                items = items->next;
            }
            items = items_head;
            while (fx != 0) {
                if (tail->effects == 0) {
                    pool_alloc((void far * far *)&tail->effects, 0xa, &effect_pool);
                    *tail->effects = *fx;
                    tail->effects->next = 0;
                } else {
                    prev_fx = tail->effects;
                    pool_alloc((void far * far *)&tail->effects, 0xa, &effect_pool);
                    *tail->effects = *fx;
                    tail->effects->next = prev_fx;
                }
                fx = fx->next;
            }
            fx = fx_head;
        }
        next_monster_group++;
        cur_node = saved;
    }
}

unsigned char far monster_dist(char x, char y, unsigned char facing)
                                                                 /* 31d4:133a */
{
    unsigned char result;
    char blocked;
    unsigned char dist;

    dist = 0;
    blocked = 0;
    result = 0;
    if (cursor_obj[0x22] == 0)
        result = 2;
    else
        while (dist < 2 && !blocked) {
            if (wall_side(x, y, facing) == 0) {
                dist++;
                result = dist;
                switch (facing) {
                case 0: y--; break;
                case 2: x++; break;
                case 4: y++; break;
                case 6: x--; break;
                }
            } else
                blocked = 1;
        }
    return result;
}

void far party_hp_span(unsigned char far *hi, unsigned char far *lo)
                                                                 /* 31d4:13c7 */
{
    struct Hero far *p;
    unsigned char v;

    p = node_list;
    *hi = p->movement;
    *lo = p->movement;
    while (p) {
        v = calc_move(p) / 2;
        if (*hi < v)
            *hi = v;
        if (*lo > v)
            *lo = v;
        p = p->next;
    }
}

void far event_combat(unsigned char far *ev, char random)        /* 31d4:1444 */
{
    char refresh;
    char saved;
    char add;
    unsigned char n;
    unsigned char list[6];
    unsigned char pick;

    mem_fill(list, 6, 0);
    picture_hides_roster = 0;
    saved = exploration_menu_context;
    exploration_menu_context = 0;
    refresh = 1;
    switch ((ev[0x12] & 0xc0) >> 6) {
    case 0: add = 0; break;
    case 1: add = 2; break;
    case 2: add = 0xa; break;
    case 3: add = 0x29; break;
    }
    cursor_obj[0xf] = cur_area_rec->level + add;
    cursor_obj[2] = ev[0xe] & 0x80;
    combat_outdoors = 0;
    if (cursor_obj[0x22]) {
        if (ev[8] & 0x20)
            combat_outdoors = 1;
        else
            combat_outdoors = 0;
    }
    if (encounter_surprise != 0xff) {
        if (encounter_surprise == 1)
            cursor_obj[0x2e] = 1;
        else if (encounter_surprise == 2)
            cursor_obj[0x2e] = 2;
        else if (encounter_surprise == 3)
            cursor_obj[0x2e] = 0;
        encounter_surprise = 0xff;
    } else
        cursor_obj[0x2e] = (ev[8] & 0xc0) >> 6;
    event_pic_show(ev);
    if (ev[6] == 0)
        event_view_draw();
    event_text_show(ev);
    if (random) {
        n = 0;
        for (tick_slot = 0; tick_slot <= 5; tick_slot++)
            if ((ev[tick_slot * 2 + 8] & 0x1f) && ev[tick_slot * 2 + 9]) {
                list[n] = tick_slot;
                n++;
            }
        if (n) {
            pick = list[random_n(n)];
            monster_table[next_monster_group - 8] = ev[pick * 2 + 9];
            monster_table[next_monster_group - 2] = ev[pick * 2 + 8] & 0x1f;
            monsters_queued = 1;
            next_monster_group++;
        }
    } else
        for (tick_slot = 0; tick_slot <= 5; tick_slot++)
            if (next_monster_group < 0xe) {
                if ((ev[tick_slot * 2 + 8] & 0x1f) && ev[tick_slot * 2 + 9]) {
                    monster_table[next_monster_group - 8] = ev[tick_slot * 2 + 9];
                    monster_table[next_monster_group - 2] = ev[tick_slot * 2 + 8] & 0x1f;
                    monsters_queued = 1;
                } else {
                    monster_table[next_monster_group - 8] = 0;
                    monster_table[next_monster_group - 2] = 0;
                }
                next_monster_group++;
            }
    pic_release();
    cursor_obj[0x38] = (ev[0xe] & 0x60) >> 5;
    cursor_obj[0x37] = monster_dist(geo.x, geo.y, geo.facing);
    if (cursor_obj[0x38] < cursor_obj[0x37])
        cursor_obj[0x37] = cursor_obj[0x38];
    mode_change(4);
    box_draw();
    status_printf(0, 0x18, 7, 0, "A battle begins...");
    monsters_place(ev);
    wander_load();
    if (ev[0xc] & 0x40)
        cursor_obj[0x1d] = 1;
    else
        cursor_obj[0x1d] = 0;
    enemy_quarter_mask = ((ev[0x12] & 0x20) >> 2) | ((ev[0x10] & 0xe0) >> 5);
    cursor_obj[0x1b] = ev[7] & 0x7f;
    combat_main();
    if (ev[0xc] & 0x80)
        cursor_obj[0x1e] = 1;
    else
        cursor_obj[0x1e] = 0;
    post_combat();
    if (combat_outdoors) {
        cursor_obj[0x22] = 1;
        cursor_obj[0x24] = 0;
        combat_outdoors = 0;
    }
    if (cursor_obj[0x22] == 0 && cursor_obj[0x24] == 1)
        wild_pic_open();
    if (cursor_obj[0x22] == 0 && cursor_obj[0x24] == 1)
        view_mode = 3;
    else
        view_mode = 4;
    cursor_obj[0x19] = cursor_obj[0x19] & 1;
    mem_fill(encounter_images_shown, 2, 0);
    cursor_obj[2] = 0;
    confirm_anim = 0;
    last_pic = 0;
    if (refresh)
        view_redraw();
    exploration_menu_context = saved;
}

char far event_encounter(unsigned char far *ev)                  /* 31d4:186b */
{
    unsigned char key;
    unsigned char count;
    unsigned char choice;
    char result;
    unsigned char hi;
    unsigned char lo;
    char done;

    done = 0;
    party_hp_span(&hi, &lo);
    encounter_surprise = 0xff;
    encounter_range = (ev[7] & 3) + 1;
    if (ev[6])
        event_pic_show(ev);
    else
        view_refresh();
    combat_text_clear();
    if (*(int far *)(ev + 4)) {
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    }
    done = 0;
    do {
        if (encounter_range > 0) {
            mem_fill(hotkeys, 0x28, 0xff);
            count = 0;
            if ((ev[9] & 8) && (encounter_range > 1 || !(ev[9] & 0x10)))
                hotkey_add(0, &count);
            if (ev[0xa] & 8)
                hotkey_add(1, &count);
            if ((ev[0xb] & 8) && (encounter_range < 2 || !(ev[0xb] & 0x10)))
                hotkey_add(2, &count);
            if (ev[0xc] & 8)
                hotkey_add(3, &count);
            if ((ev[0xd] & 8) && (encounter_range < 2 || !(ev[0xd] & 0x10)))
                hotkey_add(4, &count);
            key = menu_run(empty_text, encounter_items, 1, 1);
            switch (key) {
            case 0: choice = ev[9] & 7; break;
            case 1: choice = ev[0xa] & 7; break;
            case 2: choice = ev[0xb] & 7; break;
            case 3: choice = ev[0xc] & 7; break;
            case 4: choice = ev[0xd] & 7; break;
            }
        } else
            choice = ev[0xe] & 0xf;
        switch (choice) {
        case 0:
            encounter_range--;
            if (encounter_range < 0)
                encounter_range = 0;
            if (encounter_range > 0 && ev[6])
                event_pic_show(ev);
            break;
        case 1:
            done = 1;
            if (ev[8] > lo)
                encounter_surprise = 1;
            result = ev[0x11];
            break;
        case 2:
            done = 1;
            if (ev[8] <= lo)
                encounter_surprise = 2;
            result = ev[0x11];
            break;
        case 3:
            done = 1;
            encounter_surprise = 3;
            result = ev[0x11];
            break;
        case 4:
            done = 1;
            result = ev[0x12];
            break;
        case 5:
            done = 1;
            if (ev[8] <= lo)
                result = ev[0x13];
            else
                result = ev[0x11];
            break;
        }
    } while (!done);
    combat_text_clear();
    encounter_range = 0xff;
    return result;
}

void far damage_one(unsigned char far *ev, struct Hero far *h)   /* 31d4:1b2b */
{
    char ok;
    struct Hero far *p;
    struct Hero far *saved;
    unsigned char amount;

    saved = cur_node;
    amount = fx_roll_damage(ev[9], ev[0xa]) + ev[0xb];
    if (((ev[7] & 0x30) >> 4) != 3) {
        ok = fx_saving_throw(h, (ev[0xd] & 0x70) >> 4, ev[0xd] & 0xf);
        if (!ok)
            inflict_damage(amount, h);
        else
            switch ((ev[7] & 0x30) >> 4) {
            case 0:
                inflict_damage(amount, h);
                break;
            case 1:
                inflict_damage(amount / 2, h);
                break;
            }
    } else if (fx_attack_roll_no_attacker(h, ev[0xc]))
        inflict_damage(amount, h);
    p = node_list;
    adventure_exit = 1;
    while (p) {
        if (p->active)
            adventure_exit = 0;
        p = p->next;
    }
    cur_node = saved;
}

void far event_damage(unsigned char far *ev)                     /* 31d4:1c3e */
{
    struct Hero far *p;
    char pick;

    text_row = 0x11;
    if (ev[6])
        event_pic_show(ev);
    event_text_show(ev);
    text_row = 0x11;
    text_col = 1;
    text_clear(1, 0x11, 0x26, 0x16);
    for (tick_slot = 0; ev[8] > tick_slot && !adventure_exit; tick_slot++) {
        switch ((ev[7] & 0xc) >> 2) {
        case 0:
            for (p = node_list; p; p = p->next)
                damage_one(ev, p);
            break;
        case 1:
            damage_one(ev, cur_node);
            break;
        case 2:
            scratch_word = 0;
            p = node_list;
            while (p) {
                p = p->next;
                scratch_word++;
            }
            pick = random_n(scratch_word);
            tick_slot = 0;
            for (p = node_list; p; p = p->next, tick_slot++)
                if (tick_slot == pick)
                    damage_one(ev, p);
            break;
        case 3:
            for (p = node_list; p; p = p->next)
                if ((unsigned)random_n(100) <= ev[0x10])
                    damage_one(ev, p);
            break;
        }
        if (adventure_exit)
            continue_prompt(0xb, 0, continue_items);
    }
    if (adventure_exit) {
        frame_border();
        text_col = 2;
        text_row = 2;
        text_wrap(1, 1, 0x26, 0x16, 7, 0, 1, "The entire party is killed!");
        delay(3000);
    } else if (text_col != 1 || text_row != 0x11)
        press_return_clear();
}

char far event_temple(unsigned char far *ev)                     /* 31d4:1e60 */
{
    char result;
    char refresh;
    char sub;
    unsigned char count;
    unsigned char key;
    unsigned char answer;
    char text[41];
    char has_coins;
    char has_items;
    char left;
    char quit;
    char greet;
    long total;
    char donated;

    donated = 0;
    greet = 0;
    if (ev[6] == 0) {
        ev[6] = 0xdc;
        ev[7] = ev[7] | 0x80;
    }
    event_pic_show(ev);
    combat_text_clear();
    if ((ev[0xd] || ev[0xe]) && ev[0] != 0x16) {
        bank_text_get(text_pool_block, ev[0xd] + (ev[0xe] << 8), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
        if (pic_up) {
            press_return_wait(1);
            pic_up = 0;
        } else
            press_return_wait(1);
    } else if (ev[0] == 0x16) {
        cursor_obj[0x39] = 1;
        event_print("\"Welcome to our temple.\"");
        press_return_wait(1);
    }
    combat_text_clear();
    view_mode = 1;
    picture_hides_roster = 0;
    refresh = 1;
    shop_temple_frame = 0;
    view_redraw();
    shop_temple_frame = 1;
    mem_zero(pool, 0xc);
    result = 0;
    pooled = 0;
    combat_text_clear();
    if (ev[0xf] || ev[0x10]) {
        bank_text_get(text_pool_block, ev[0xf] + (ev[0x10] << 8), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    } else
        text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, "What is your wish?");
    do {
        count = 0;
        hotkey_add(0, &count);
        if (ev[7] & 8)
            hotkey_add(1, &count);
        pool_check(&has_coins, &has_items);
        hotkey_add(2, &count);
        hotkey_add(3, &count);
        if (has_coins || has_items) {
            hotkey_add(4, &count);
            hotkey_add(5, &count);
        }
        hotkey_add(6, &count);
        key = node_prompt(temple_title, temple_wish_items, 1, 0);
        if (!menu_escaped || key == 0x1b) {
            combat_text_clear();
            greet = 1;
        }
        left = 0;
        quit = 0;
        if (menu_escaped) {
            quit = 1;
            if (key == 0x1b)
                key = 3;
            else {
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
                key = 0xff;
            }
        }
        switch (key) {
        case 0:
            sub = 0;
            break;
        case 1:
            sub = 1;
            break;
        case 2:
            char_view(&menu_escaped);
            quit = 1;
            if (ev[6])
                event_pic_show(ev);
            break;
        case 3:
            if (!menu_escaped)
                pool_money();
            quit = 1;
            break;
        case 4:
            take_menu();
            view_redraw();
            quit = 1;
            break;
        case 5:
            share_money();
            quit = 1;
            break;
        case 6:
            left = 1;
            pool_check(&has_coins, &has_items);
            if (has_coins || has_items) {
                answer = 0;
                while ((has_coins || has_items) && !answer) {
                    left = 0;
                    str_copy(text, "~Yes ~No");
                    text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1,
                              "As the party starts to leave a priest");
                    text_wrap(1, 0x12, 0x26, 0x16, 7, 0, 1, "says, ");
                    text_wrap(7, 0x12, 0x26, 0x16, 3, 0, 1,
                              "\"Excuse me, but you have left");
                    text_wrap(1, 0x13, 0x26, 0x16, 3, 0, 1, "some money here.\" ");
                    press_return_clear();
                    text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, leave_text3);
                    answer = menu_horiz("", text, 0, 0);
                    if (answer == 1) {
                        total = le_long_get(cursor_obj + 0x3f);
                        total += pool[0];
                        le_long_put(total, cursor_obj + 0x3f);
                        text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1,
                                  "Thank you for your generous donation.");
                        if (*(long far *)(ev + 8) <= total && (ev[7] & 0x10))
                            donated = 1;
                        press_return_clear();
                        left = 1;
                    } else {
                        text_clear(1, 0x11, 0x26, 0x16);
                        left = 0;
                        answer = 1;
                    }
                    mem_fill(encounter_images_shown, 2, 0);
                    if (ev[6])
                        event_pic_show(ev);
                    view_redraw();
                }
            }
            break;
        }
        if (!left && !quit) {
            box_draw();
            cursor_obj[0x14] = 1;
            if (temple_shop(ev, sub, *(int far *)(ev + 8), *(int far *)(ev + 0xa),
                             ev[0xc] ? 1 : 0))
                donated = 1;
            if (donated)
                result = 1;
            else
                result = 0;
            mem_fill(encounter_images_shown, 2, 0);
            if (ev[6])
                event_pic_show(ev);
        } else if (donated)
            result = 1;
        else
            result = 0;
        if (greet && !left) {
            combat_text_clear();
            if (ev[0xf] || ev[0x10]) {
                bank_text_get(text_pool_block, ev[0xf] + (ev[0x10] << 8), msg_buf);
                cursor_obj[0x39] = 0;
                event_print(msg_buf);
            } else
                text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, "What is your wish?");
        }
        confirm_anim = 0;
        if (refresh && left)
            view_redraw();
    } while (!left);
    if (cursor_obj[0x22] == 0)
        refresh = 0;
    if (cursor_obj[0x22] == 0 && cursor_obj[0x24] == 1)
        view_mode = 3;
    else
        view_mode = 4;
    cursor_obj[0x19] = cursor_obj[0x19] & 1;
    mem_fill(encounter_images_shown, 2, 0);
    cursor_obj[0x14] = 0;
    shop_temple_frame = 1;
    if (ev[7] & 4)
        step_back_due = 1;
    return result;
}

void far event_treasure(unsigned char far *ev, char after)      /* 31d4:2550 */
{
    char saved;
    register int i;

    cursor_obj[0x1e] = 0;
    pool[0] = le_long_get(ev + 4);
    pool[0] = pool[0] & 0x7fffffffL;
    pool[1] = le_word_get(ev + 8);
    pool[2] = le_word_get(ev + 0xa);
    i = 9;
    while (--i > 0)
        if (ev[i + 0xb])
            treasure_item_add(ev[i + 0xb], ev[7] & 0x80);
    if (after) {
        picture_hides_roster = 0;
        saved = exploration_menu_context;
        exploration_menu_context = 0;
        post_combat();
        if (cursor_obj[0x22] == 0 && cursor_obj[0x24] == 1)
            view_mode = 3;
        else
            view_mode = 4;
        cursor_obj[0x19] = cursor_obj[0x19] & 1;
        mem_fill(encounter_images_shown, 2, 0);
        confirm_anim = 0;
        view_redraw();
        exploration_menu_context = saved;
        last_pic = 0;
    }
}

char far event_password(unsigned char far *ev)                   /* 31d4:2665 */
{
    char ok;
    unsigned char tries;

    ok = 0;
    if (ev[6])
        event_pic_show(ev);
    else
        view_refresh();
    combat_text_clear();
    if (*(int far *)(ev + 4)) {
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    }
    for (tries = 1; ev[0x13] >= tries && !ok; tries++) {
        bank_text_get(text_pool_block, ev[8] + (ev[9] << 8), msg_buf);
        box_draw();
        ok = str_same(input_text("", 7, 8, 0x19), msg_buf);
        if (!ok && ev[0x13] > tries) {
            combat_text_clear();
            cursor_obj[0x39] = 0;
            event_print("wrong, try agin.");
            press_return_wait(1);
            combat_text_clear();
            if (*(int far *)(ev + 4)) {
                bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
                cursor_obj[0x39] = 0;
                event_print(msg_buf);
            }
        }
    }
    return ok;
}

void far event_xp(unsigned char far *ev)                         /* 31d4:27b5 */
{
    long share;
    long xp;
    char all;
    struct Hero far *p;
    short j;
    unsigned char classes;

    if (ev[6])
        event_pic_show(ev);
    else
        event_view_draw();
    combat_text_clear();
    if (*(int far *)(ev + 4)) {
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    }
    if (ev[0xc])
        sfx_play(ev[0xc]);
    if (ev[6] || *(int far *)(ev + 4))
        press_return_wait(1);
    xp = le_long_get(ev + 8);
    all = !(ev[7] & 4);
    text_row = 0x11;
    text_col = 1;
    if (all) {
        text_pause();
        for (p = node_list; p; p = p->next)
            if (p->active) {
                classes = 0;
                for (j = 0; j <= 6; j++)
                    if (p->level[j] > 0)
                        classes++;
                share = xp / classes;
                p->xp = p->xp + share;
                p->level_up = can_level_up(p);
            }
        party_display(cur_node);
    } else {
        p = cur_node;
        if (p->active) {
            text_pause();
            classes = 0;
            for (j = 0; j <= 6; j++)
                if (p->level[j] > 0)
                    classes++;
            share = xp / classes;
            p->xp = p->xp + share;
            p->level_up = can_level_up(p);
        }
    }
}

void far event_train(unsigned char far *ev, int cls)             /* 31d4:29b7 */
{
    combat_text_clear();
    if (ev[6])
        event_pic_show(ev);
    else
        view_refresh();
    if (*(int far *)(ev + 4) && !cursor_obj[0x85]) {
        combat_text_clear();
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    }
    if (confirm("Does the party want to train?", 1)) {
        box_draw();
        cursor_obj[0x30] = ev[8];
        train_cost = rate_price(1000L, ev[9], 1);
        in_service_event = 1;
        cursor_obj[0x31] = cls;
        if (!party_menu(1))
            adventure_exit = 1;
        in_service_event = 0;
        cursor_obj[0x31] = 0;
        last_pic = 0;
    }
    if (ev[7] & 4)
        step_back_due = 1;
    combat_text_clear();
}

void far event_walk(unsigned char far *ev)                       /* 31d4:2ab8 */
{
    unsigned char step;

    if (view_mode != 3) {
        event_view_draw();
        rerun_square = ev[7] & 0x20;
        prev_pos_save();
        if (ev[7] & 0x10) {
            transfer_dest_x = ev[5];
            transfer_dest_y = ev[4];
            party_pos_set();
            geo.facing = (ev[7] & 0xc) >> 1;
            view_update();
            party_info_display();
        }
        delay(600);
        for (step = 0; ev[6] >= step; step++) {
            if (ev[0xe] == step && ev[0x10] + ev[0x11]) {
                combat_text_clear();
                bank_text_get(text_pool_block, ev[0x10] + (ev[0x11] << 8), msg_buf);
                cursor_obj[0x39] = 0;
                event_print(msg_buf);
                if (pic_up) {
                    press_return_wait(1);
                    pic_up = 0;
                } else
                    press_return_wait(1);
                frame_main();
                event_view_restore();
            }
            if (ev[0xf] == step && ev[0x12] + ev[0x13]) {
                combat_text_clear();
                bank_text_get(text_pool_block, ev[0x12] + (ev[0x13] << 8), msg_buf);
                cursor_obj[0x39] = 0;
                event_print(msg_buf);
                if (pic_up) {
                    press_return_wait(1);
                    pic_up = 0;
                } else
                    press_return_wait(1);
                frame_main();
                event_view_restore();
            }
            if (ev[6] != step) {
                switch ((ev[step / 4 + 8] >> step % 4 * 2) & 3) {
                case 1:
                    geo.facing = (geo.facing + 6) % 8;
                    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
                    view_draw(geo.x, geo.y, geo.facing);
                    break;
                case 2:
                    geo.facing = (geo.facing + 2) % 8;
                    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
                    view_draw(geo.x, geo.y, geo.facing);
                    break;
                case 3:
                    step_dungeon();
                    break;
                }
                view_update();
                party_info_display();
            }
            delay(600);
            prev_pos_save();
            combat_text_clear();
            view_update();
            party_info_display();
        }
        party_has_moved = 1;
    }
}

char far event_question(unsigned char far *ev)                   /* 31d4:2d26 */
{
    char answer;

    if (ev[6])
        event_pic_show(ev);
    else
        view_refresh();
    combat_text_clear();
    if (*(int far *)(ev + 4)) {
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    }
    answer = confirm("", 0);
    if (answer) {
        combat_text_clear();
        if (ev[0xa]) {
            bank_text_get(text_pool_block, ev[0xa] + (ev[0xb] << 8), msg_buf);
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
            press_return_wait(1);
        }
    } else {
        combat_text_clear();
        if (ev[0xc]) {
            bank_text_get(text_pool_block, ev[0xc] + (ev[0xd] << 8), msg_buf);
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
            press_return_wait(1);
        }
    }
    combat_text_clear();
    return answer;
}

char far stat_check(unsigned char far *ev, unsigned char value)  /* 31d4:2e39 */
{
    char ok;

    ok = 0;
    if (ev[7] & 0x40) {
        if (((ev[7] & 0x3c) >> 2) == 2 && value == 0x12 && ev[8] == 0x12)
            ok = cur_node->str_ex >= ev[8];
        else
            ok = ev[8] <= value;
    } else if (((ev[7] & 0x3c) >> 2) <= 7)
        ok = (unsigned)random_n(20) <= value;
    else
        ok = (unsigned)random_n(100) <= value;
    return ok;
}

char far event_check(unsigned char far *ev)                      /* 31d4:2ef9 */
{
    char saved;
    char ok;
    unsigned char tries;

    event_view_draw();
    ok = 0;
    tries = 0;
    do {
        tries++;
        if (ev[6])
            event_pic_show(ev);
        else
            view_refresh();
        combat_text_clear();
        if (*(int far *)(ev + 4)) {
            bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
            combat_text_clear();
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
        }
        pick_hero("", 0, 0, &cur_node);
        saved = exploration_menu_context;
        exploration_menu_context = 0;
        character_select_failed = 0;
        exploration_menu_context = saved;
        cur_node = (struct Hero far *)get_selected();
        if (cur_node->active == 1) {
            switch ((ev[7] & 0x3c) >> 2) {
            case 0: ok = 1; break;
            case 1: ok = 0; break;
            case 2: ok = stat_check(ev, cur_node->str); break;
            case 3: ok = stat_check(ev, cur_node->intel); break;
            case 4: ok = stat_check(ev, cur_node->wis); break;
            case 5: ok = stat_check(ev, cur_node->dex); break;
            case 6: ok = stat_check(ev, cur_node->con); break;
            case 7: ok = stat_check(ev, cur_node->cha); break;
            case 8: ok = stat_check(ev, cur_node->thief[0]); break;
            case 9: ok = stat_check(ev, cur_node->thief[1]); break;
            case 10: ok = stat_check(ev, cur_node->thief[2]); break;
            case 11: ok = stat_check(ev, cur_node->thief[3]); break;
            case 12: ok = stat_check(ev, cur_node->thief[4]); break;
            case 13: ok = stat_check(ev, cur_node->thief[5]); break;
            case 14: ok = stat_check(ev, cur_node->thief[6]); break;
            case 15: ok = stat_check(ev, cur_node->thief[7]); break;
            }
            if (!ok && ev[9] > tries) {
                combat_text_clear();
                cursor_obj[0x39] = 0;
                event_print("that attempt did not succeed.");
                press_return_wait(1);
            }
        } else {
            combat_text_clear();
            cursor_obj[0x39] = 0;
            event_print(msg_format("%s is in no condition to try.", cur_node->name));
            press_return_wait(1);
            tries--;
        }
    } while (!ok && ev[9] > tries);
    return ok;
}

char far event_pay(unsigned char far *ev)                        /* 31d4:321a */
{
    struct Hero far *h;
    unsigned short amount;
    char saved;
    char paid;

    event_view_draw();
    saved = exploration_menu_context;
    exploration_menu_context = 0;
    character_select_failed = 0;
    exploration_menu_context = saved;
    h = cur_node;
    amount = ev[8] + (ev[9] << 8);
    do {
        paid = 0;
        if (ev[6])
            event_pic_show(ev);
        else
            view_refresh();
        combat_text_clear();
        if (*(int far *)(ev + 4)) {
            bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
            combat_text_clear();
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
        }
        pick_hero("", 1, 0, &h);
        if (h == 0)
            paid = 0;
        else {
            cur_node = h;
            h = (struct Hero far *)get_selected();
            switch ((ev[7] & 0xc) >> 2) {
            case 1:
                if (h->coins[1] >= amount) {
                    h->coins[1] -= amount;
                    paid = 1;
                } else
                    paid = 0;
                break;
            case 2:
                if (h->coins[2] >= amount) {
                    h->coins[2] -= amount;
                    paid = 1;
                } else
                    paid = 0;
                break;
            case 3:
                if (h->coins[0] >= amount) {
                    h->coins[0] -= amount;
                    paid = 1;
                } else
                    paid = 0;
                break;
            }
            if (!paid) {
                combat_text_clear();
                cursor_obj[0x39] = 0;
                event_print(msg_format("%s doesn't have enough.", h->name));
                press_return_wait(1);
            }
        }
    } while (!paid && h);
    return paid;
}

void far event_npc(unsigned char far *ev)                        /* 31d4:33b8 */
{
    unsigned hp;

    if (!party_has_npc(ev[8])) {
        if (ev[6])
            event_pic_show(ev);
        else
            view_refresh();
        event_text_show(ev);
        mon_add(ev[8]);
        hp = cur_node->hp_max * ev[9] / 100;
        if (hp == 0)
            hp = 1;
        if (hp > 0xfa)
            hp = 0xfa;
        cur_node->hp = hp;
        recalc_stats(cur_node);
        party_display(cur_node);
    }
}

void far event_who(unsigned char far *ev)                        /* 31d4:3469 */
{
    struct Hero far *p;
    char found;
    char any;

    found = 0;
    if (ev[8] == 0)
        any = 1;
    else
        any = 0;
    for (p = node_list; p && !found; p = p->next)
        if ((p->mon_id == ev[8] && !any)
                || (p->mon_id >= 0x70 && p->mon_id <= 0x7f && any)) {
            found = 1;
            cur_node = p;
            hero_drop(0, 1);
        }
    if (found) {
        if (ev[6])
            event_pic_show(ev);
        else
            view_refresh();
        event_text_show(ev);
        party_display(cur_node);
    }
}

void far event_inn(unsigned char far *ev, int cls)               /* 31d4:353b */
{
    if (ev[6])
        event_pic_show(ev);
    else
        view_refresh();
    if (!event_text_show(ev) || ev[0] == 0x16) {
        combat_text_clear();
        cursor_obj[0x39] = 0;
        event_print("The party enters a local Inn.");
        if (pic_up) {
            press_return_wait(1);
            pic_up = 0;
        }
        combat_text_clear();
    }
    in_service_event = 1;
    cursor_obj[0x31] = cls;
    enter_camp(0);
    in_service_event = 0;
    cursor_obj[0x31] = 0;
    if (ev[7] & 4)
        step_back_due = 1;
}

void far event_vault(unsigned char far *ev)                      /* 31d4:35df */
{
    if (ev[6] == 0) {
        ev[6] = 0xc1;
        ev[7] = ev[7] | 0x80;
    }
    event_pic_show(ev);
    if (!event_text_show(ev)) {
        combat_text_clear();
        cursor_obj[0x39] = 0;
        event_print("The party enters the vault.");
        if (pic_up) {
            press_return_wait(1);
            pic_up = 0;
        }
    }
    vault_menu();
    if (ev[7] & 4)
        step_back_due = 1;
    combat_text_clear();
}

void far event_sounds(unsigned char far *ev)                     /* 31d4:3662 */
{
    unsigned char far *p;
    unsigned char i;

    event_view_draw();
    for (p = ev + 4, i = 0; i < 0xa; p++, i++)
        if (*p)
            sfx_play(*p);
}

char far event_menu(unsigned char far *ev)                       /* 31d4:36a4 */
{
    char result;

    result = 0;
    if (ev[6])
        event_pic_show(ev);
    else
        view_refresh();
    combat_text_clear();
    if (*(int far *)(ev + 4)) {
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
    }
    txt_get(text_pool_block, ev[8] + (ev[9] << 8) - 1, msg_buf);
    if (msg_buf[0] == 0)
        result = 0;
    else if (ev[0] == 0x15)
        result = menu_list(msg_buf);
    else
        result = menu_horiz("", msg_buf, 0, 1);
    combat_text_clear();
    return result;
}
