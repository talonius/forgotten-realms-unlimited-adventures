/* CKIT.EXE overlay 06 module 2209: allocating the game's buffers and
 * starting a new game.
 * game_setup reads the configuration, allocates the shared buffers and has
 * poolinit.c size the slot pools; game_init loads the font, sets up the
 * palette, clears the party, combat and view state, and reads ITEMS.DAT
 * into item_table and item.dat into item_templates; game_reset clears part of
 * that state again.  arena_alloc hands out storage from one block that
 * db_buffer returns, and never frees it.
 *
 * EXE 0x0390fe..0x03972b, one Turbo C large-model _TEXT segment starting at
 * byte e of paragraph 2209 in RTLink section 6 (payload 0x38da0, link
 * segment 21d4; a child of section 3).  The first 14 bytes of paragraph
 * 2209 are the tail of module 21fa, and the last 11 bytes of the range lie
 * in paragraph 226c, whose module starts at 226c:000b.  Built with Turbo C
 * 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order; bake
 * consumes them in that order.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a mod18de_ callee is an RTLink
 * vector stub whose target is named beside it.  game_setup, game_init,
 * game_reset and game_done are reached through the vector stubs 18de:1463,
 * 18de:1459, 18de:1427 and 18de:10d5, which section 3's gamemain.c calls
 * game_setup, game_init, game_reset and game_done; misc.c also
 * calls 18de:10d5.  poolinit.c far-calls arena_alloc, as game_setup
 * does.
 *
 * _DATA is DGROUP 6b76..6bbe: color_table, alloc_next and five literals.
 * _BSS is empty: every other variable this module writes is declared extern.
 *
 * THIS IS THE v1.0 BUILD (CKIT.EXE "Version 1.0 March 19,1993"): the
 * addresses above are v1.2's; v1.0 places the module at file 0x0390ce
 * (320b:000e, _DATA 6b44).  Against v1.2, v1.0's game_init also calls the
 * empty resident routine v10_0cf6_0006 before debug_password_init and clears
 * v10_byte_caac (a v1.0-only byte, DGROUP caac) after no_study; game_init and game_reset
 * clear pending_load through mapevent.c's vector stub 18de:141d instead of
 * storing it directly.
 */

struct Res;                     /* tiles.c's resource handle              */
struct ItemType;                /* hero.c's                               */
struct Hero;                    /* hero.c's                               */
struct Map;                     /* approach.c's                           */
struct Event;                    /* hero.c's                               */
struct Item;                    /* hero.c's                               */

struct Combatant {              /* cmbsetup.c's                           */
    int x;                      /* +0                                     */
    int y;                      /* +2                                     */
    char number;                /* +4                                     */
    unsigned char size;         /* +5                                     */
};

struct Pic {                    /* picview.c's picture window state       */
    unsigned char frames;       /* +0                                     */
    unsigned char anim;         /* +1                                     */
    struct Res far *res;        /* +2                                     */
};

struct Slot {                   /* 5 bytes                                */
    unsigned char flag;         /* +0                                     */
    long value;                 /* +1                                     */
};

struct Geo {                    /* the party's place                      */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
    char pad_3[4];              /* +3   only the 5-byte clear reaches it  */
    char show_map;              /* +7   d41b                              */
};

extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far textbank_init(void);                             /* 03ad:000a */
extern void far res_alloc(struct Res far * far *pp, unsigned n); /* 05f5:04b4 */
extern void far res_reset(void);                                 /* 05f5:0a8d */
extern void far input_done(void);                                /* 06c9:0021 */
extern void far button_hook_install(void);                       /* 083a:02ef */
extern void far frame_invalidate(void);                          /* 083a:0d3c */
extern void far set_step_mode(char c);                           /* 083a:239f */
extern void far sfx_setup(void);                               /* 0c8e:014e */
extern void far sfx_init(void);                               /* 0c8e:014f */
extern void far v10_0cf6_0006(void);                             /* v1.0 0cf6:0006, an empty routine */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern void far db_init(long size);                              /* 107e:0009 */
extern void far font_load(char disk, char far *name);            /* 10f8:03f1 */
extern void far pal_init(void);                                  /* 1157:000d */
extern void far pal_commit(void);                              /* 1157:0457 */
extern void far glyph_library_open(char mode, char far *name, int slot); /* 11d7:0031 */
extern void far color_map_set(char far *map);                    /* 11d7:011d */
extern void far pal_glyph(int slot, int n, char fade);           /* 11d7:0f28 */
extern void far glyph_palette_load(char far *lib, int n);              /* 12d2:0537 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far dl_init(int max);                                /* 15e5:11ea */
extern void far ticks_start(void);   /* 18de:1931, vector to overlay 14 28aa:3a44 */
extern void far area_alloc(void);   /* 18de:193b, vector to overlay 13 26e3:0557 */
extern void far pending_load_set(char n); /* 18de:141d, vector to overlay 4 1cff:08e5, mapevent.c's pending_load setter */
extern void far fx_install(void);   /* 18de:1945, vector to overlay 11 26e3:317e */
extern void far spellfx_init(void);   /* 18de:194f, vector to overlay 12 26e3:44d2 */
extern void far config_load(void);                               /* 21d4:0000 */
extern void far video_check(void);                               /* 21d4:00e2 */
extern void far party_pool_init(int n);                            /* 21fa:0001 */
extern void far npc_pool_init(int n);                            /* 21fa:0025 */
extern void far item_pool_init(int n);                            /* 21fa:0049 */
extern void far item_pool_select(int n);                            /* 21fa:006d */
extern void far effect_pool_init(int n);                            /* 21fa:007c */
extern void far cloud_pool_init(int n);                            /* 21fa:00a0 */
extern void far combat_pool_init(int n);                            /* 21fa:00c4 */
extern void far debug_password_init(void);                             /* 21fa:00e8 */

void far *far arena_alloc(int n);

extern char byte_6723;                                           /* 6723 */
extern char big_pic_shown;                                           /* 6724 */
extern struct Event far *event_table;                                /* a7f0 */
extern struct Hero far *cur_node;                                /* b3dd */
extern struct ItemType far *item_table;                          /* b40a */
extern char view_kind;                                           /* b40e */
extern char view_flag;                                           /* b40f */
extern char view_kind_saved;                                           /* b417 */
extern int view_mode;                                            /* b421 */
extern char esc_check;                                           /* b423 */
extern char game_saved;                                           /* b425 */
extern char confirm_anim;                                        /* b42a */
extern char text_slow;                                           /* b431 */
extern int saved_view_mode;                                            /* b433 */
extern char exploration_menu_context;                                           /* b4e8 */
extern char resting;                                             /* b4eb */
extern struct Res far *icon_res;                                 /* b71c */
extern int word_b738;                                            /* b738 */
extern char arr_b73c[];                                          /* b73c */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Slot arr_b75f[];                                   /* b75f */
extern char picture_hides_roster;                                           /* b761 */
extern struct Item far *pool_items;                              /* b8e2 */
extern char byte_bcef;                                           /* bcef */
extern char encounter_sprite_shown;                                           /* bcf0 */
extern char adventure_exit;                                           /* bcf6 */
extern unsigned char v10_byte_caac;     /* v1.0 only: caac, also cleared by cmbsetup.c and view3d.c */
extern struct Res far *big_res;                                  /* bcf7 */
extern char no_study;                                            /* be68 */
extern char year_rolled;                                         /* c354 */
extern char train_costs_waived;                                           /* c39a */
extern void far *back_res;                                       /* c39b */
extern char saved_search_flag;                                           /* c3a2 */
extern char pic_name[];                                          /* c3a7 */
extern char byte_c3c0;                                           /* c3c0 */
extern char monsters_queued;                                           /* c3c1 */
extern char byte_c3c3;                                           /* c3c3 */
extern char item_handler_active;                                           /* c523 */
extern long long_c525;                                           /* c525 */
extern char pic_saved[];                                         /* c656 */
extern struct Map far *cur_map;                                  /* c66b */
extern char byte_c670;                                           /* c670 */
extern char byte_ca14;                                           /* ca14 */
extern unsigned encounter_ticks;                                 /* ca1b */
extern struct Pic pic;                                           /* ca22 */
extern struct Res far *wall_set_res[3];                              /* caf8 */
extern char vault_slot;                                           /* cc6e */
extern struct Hero far *selected_target;                               /* cc6f */
extern unsigned char combatant_count;                            /* cc73 */
extern char flag_cc74;                                           /* cc74 */
extern char flag_cc75;                                           /* cc75 */
extern char move_flag;                                           /* cc79 */
extern char byte_cc7b;                                           /* cc7b */
extern struct Combatant combatants[68];                          /* cc7c */
extern void far *target_scores;                                       /* ce61 */
extern struct Res far *pic_res;                                  /* cfd8 */
extern char byte_cfdc;                                           /* cfdc */
extern char arr_d0fe[];                                          /* d0fe */
extern char far *item_templates;                                       /* d122 */
extern char order_mode;                                           /* d126 */
extern char byte_d127;                                           /* d127 */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Hero far *previous_highlighted_character;                               /* d12d */
extern struct Geo geo;                                           /* d414 */

static char color_table[16] = {                                  /* 6b76 */
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x17, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
static char far *alloc_next = 0;                                 /* 6b86 */

static void item_data_load(void);

char far item_table_read(int fd, char far *path)                 /* 3209:000e */
{
    register int size = 0x800;

    return dos_io(0x3f00, item_table, fd, size) == size;
}

void far game_setup(void)                                        /* 3209:003b */
{
    config_load();
    db_init(0x9400L);
    dl_init(40);
    cursor_obj = mem_alloc(0x400);
    cursor_obj = cursor_obj - 1;
    textbank_init();
    item_table = mem_alloc(0x810);
    item_templates = mem_alloc(0x11ee);
    party_pool_init(8);
    item_pool_init(640);
    item_pool_select(200);
    effect_pool_init(400);
    area_alloc();
    sfx_setup();
    target_scores = arena_alloc(0xab2);
    cur_map = arena_alloc(0x4eb);
    npc_pool_init(60);
    cloud_pool_init(70);
    combat_pool_init(68);
}

void far game_done(void)                                          /* 3209:010c */
{
}

void far game_init(void)                                         /* 3209:010d */
{
    unsigned char i;

    video_check();
    font_load('1', "game");
    ticks_start();
    text_print_at(0x1fa0, 0x1f40, 10, "Loading...Please Wait");
    sfx_init();
    glyph_library_open('1', "always", 0);
    if (is_xvga() || !pixel_shift()) {
        pal_init();
        glyph_palette_load(slot_base(0), 0);
        pal_commit();
        color_map_set(color_table);
    } else
        pal_glyph(0, 0, 0);
    res_reset();
    button_hook_install();
    v10_0cf6_0006();
    debug_password_init();
    mem_zero(combatants, sizeof(combatants));
    combatant_count = 0;
    long_c525 = 0;
    mem_zero(wall_set_res, 4);         /* slot 0 only */
    mem_fill(&pic_res, sizeof(pic_res), 0);
    mem_fill(cursor_obj + 1, 0x400, 0);
    cursor_obj[0x27] = 3;
    cursor_obj[0x22] = 1;
    byte_6723 = 0;
    big_pic_shown = 0;
    pending_load_set(0);
    vault_slot = 90;
    mem_zero(back_res, 4);
    res_alloc(&icon_res, 81);
    mem_zero(arr_d0fe, 36);
    mem_zero(arr_b73c, 31);
    for (i = 1; i <= 2; i++) {
        arr_b75f[i].flag = 0xff;
        arr_b75f[i].value = 0;
    }
    mem_fill(&pic, sizeof(pic), 0);
    big_res = 0;
    pool_items = 0;
    mem_fill(&geo, 5, 0);
    geo.x = 0;
    geo.y = 0;
    geo.facing = 4;
    byte_c3c0 = 1;
    text_slow = 1;
    encounter_ticks = 0;
    node_list = 0;
    cur_node = 0;
    selected_target = 0;
    previous_highlighted_character = 0;
    cursor_obj[0x12] = 4;
    esc_check = 0;
    geo.show_map = 0;
    cursor_obj[0x20] = 0;
    word_b738 = 1;
    resting = 0;
    saved_search_flag = 0;
    byte_c670 = 0;
    byte_cfdc = 0;
    confirm_anim = 0;
    adventure_exit = 0;
    encounter_sprite_shown = 0;
    pic_name[0] = 0;
    pic_saved[0] = 0;
    view_kind = 0xff;
    view_kind_saved = 0xff;
    game_saved = 0;
    byte_d127 = 0xe8;
    byte_cc7b = 0xdb;
    byte_ca14 = 0;
    order_mode = 0;
    move_flag = 1;
    view_flag = 0xff;
    ui_mode = 1;
    view_mode = 4;
    saved_view_mode = 0;
    item_handler_active = 0;
    no_study = 0;
    v10_byte_caac = 0;
    monsters_queued = 0;
    picture_hides_roster = 0;
    train_costs_waived = 0;
    flag_cc75 = 0;
    flag_cc74 = 0;
    exploration_menu_context = 0;
    byte_bcef = 0;
    year_rolled = 0;
    file_open('1', "ITEMS.DAT", 0, item_table_read);
    spellfx_init();
    fx_install();
    input_done();
    set_step_mode(0);
    item_data_load();
}

void far game_reset(void)                                        /* 3209:0448 */
{
    mem_fill(cursor_obj + 1, 0x400, 0);
    cursor_obj[0x27] = 3;
    cursor_obj[0x22] = 1;
    byte_6723 = 0;
    big_pic_shown = 0;
    pending_load_set(0);
    vault_slot = 90;
    mem_fill(event_table, 2000, 0);
    mem_fill(&geo, 5, 0);
    geo.x = 0;
    geo.y = 0;
    geo.facing = 4;
    byte_c3c3 = 1;
    text_slow = 1;
    encounter_ticks = 0;
    node_list = 0;
    cur_node = 0;
    previous_highlighted_character = 0;
    cursor_obj[0x12] = 4;
    geo.show_map = 0;
    cursor_obj[0x20] = 0;
    word_b738 = 1;
    resting = 0;
    saved_search_flag = 0;
    byte_c670 = 0;
    byte_cfdc = 0;
    confirm_anim = 0;
    adventure_exit = 0;
    encounter_sprite_shown = 0;
    pic_name[0] = 0;
    pic_saved[0] = 0;
    view_kind = 0xff;
    view_kind_saved = 0xff;
    game_saved = 0;
    byte_d127 = 0xe8;
    byte_cc7b = 0xdb;
    byte_ca14 = 0;
    move_flag = 1;
    view_flag = 0xff;
    frame_invalidate();
    ui_mode = 1;
    view_mode = 4;
    saved_view_mode = 0;
    item_handler_active = 0;
    train_costs_waived = 0;
    flag_cc75 = 0;
    flag_cc74 = 0;
    exploration_menu_context = 0;
    byte_bcef = 0;
    order_mode = 0;
    picture_hides_roster = 0;
    year_rolled = 0;
    monsters_queued = 0;
}

char far item_data_read(int fd, char far *path)                  /* 3209:05a8 */
{
    mem_fill(item_templates, 0x11ee, 0xff);
    if (dos_io(0x3f00, item_templates + 0x12, fd, 0x11dc) != 0x11dc)
        return 0;
    return 1;
}

static void item_data_load(void)                                 /* 3209:05ed */
{
    file_open('1', "item.dat", 0, item_data_read);
}

void far *far arena_alloc(int n)                             /* 3209:0606 */
{
    char far *p;

    if (alloc_next == 0)
        alloc_next = db_buffer();
    p = alloc_next;
    alloc_next += n;
    return p;
}
