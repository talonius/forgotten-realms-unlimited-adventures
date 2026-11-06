/* CKIT.EXE overlay 03 module 1b88: the game's main and its adventure loop.
 * The two insert-disk handlers game_main installs with disk_setup, one on
 * the status line and one in the text window; game_main, reached through
 * the vector stub 18de:13d7 with argc and argv, which starts the system,
 * makes argv the message table, runs game_session while 18de:1431 says so,
 * and exits to DOS; and game_session, one pass of it: loading a saved game
 * or starting one, then the adventure until 18de:13e1 ends it.  At start-up
 * game_main calls 18de:1445 unless message 2 (argv[2] when given) is
 * "Heart", and reports memory at the end in that case.  Unless message 3
 * equals debug_word, it calls game_exit when 18de:143b returns 0 or
 * copyprot_markers[1..3] holds a 0.
 *
 * EXE 0x02c467..0x02c7ab, one Turbo C large-model _TEXT segment starting at
 * byte 7 of paragraph 1b88 in RTLink section 3 (payload 0x2bff0, link
 * segment 1b41; a root section, the parent of 4 to 10).  The first 7 bytes
 * of paragraph 1b88 are the tail of module 1b41, and the last b bytes of
 * the range lie in paragraph 1bbc, whose module starts at 1bbc:000b.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 3.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; an ovlNN_ name says which section
 * and stored segment the callee lives in, and a mod18de_ callee is an
 * RTLink vector stub whose target is named beside it.  game_main passes
 * disk_msg_text and disk_msg_status to disk_setup as handlers, and
 * near-calls game_session ahead of its definition, so that one is static
 * and prototyped.
 *
 * _DATA is three literals, DGROUP 5ab2..5acb.  This module has no _BSS.
 */

struct Hero;                    /* hero.c's; only passed here             */
struct Res;                     /* tiles.c's resource; only passed here   */

struct Geo {                    /* the party's place and the view state   */
    char x;                     /* +0   d414, not read here               */
    char y;                     /* +1   d415, not read here               */
    unsigned char facing;       /* +2   d416, not read here               */
    char facing_wall;           /* +3   d417, not read here               */
    char event;                 /* +4   d418, not read here               */
    char attr_5;                /* +5   d419                              */
    char attr_6;                /* +6   d41a                              */
    char pad_7[5];              /* +7   not read here                     */
    char attr_c;                /* +c   d420                              */
    char attr_d;                /* +d   d421                              */
};

extern void far box_draw(void);                                  /* 083a:03b2 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far prompt_attr_set(int attr);                           /* 0df0:005a */
extern void far win_text_printf(char far *fmt, ...);             /* 0df0:017b */
extern void far sys_init(int mode, char far *far *argv, int need_kb, int want_kb);
                                                                 /* 0da7:0005 */
extern int  far pointer_mode_set(int mode);                          /* 0f5e:0a85 */
extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far msg_set_table(int count, char far *far *table);  /* 0a75:0385 */
extern void far disk_setup(void (far *handler)(char kind, char disk, char far *text),
                           char subdirs, char far *arg, char save); /* 0e23:04ae */
extern void far start_load(void);                           /* 1b41:03eb */
extern void far prompt_window_push(int row, int col);                   /* 0df0:0020 */
extern char far *far msg_text(int i);                            /* 0a75:039b */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far frame_select(int n);                               /* 0d65:03c9 */
extern void far prompt_window_pop(void);                               /* 0df0:0077 */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far game_exit(void);                                 /* 0d48:002f */
extern void far dl_done(void);                               /* 15e5:1235 */
extern void far mem_report(long need);                           /* 1336:0449 */
extern void far game_sound_stop(void);                              /* 0c8e:0221 */
extern void far sys_shutdown(void);                              /* 0da7:006c */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern void far status_msg(char far *fmt, char fg, char bg, ...); /* 06c9:08f7 */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far frame_main(void);                                /* 0d65:00de */
extern void far hero_drop(char keep, char icon);                 /* 03d0:1e69 */
extern void far game_setup(void);    /* 18de:1463, vector to overlay 6 2209:003b */
extern void far game_init(void);    /* 18de:1459, vector to overlay 6 2209:010d */
extern void far game_load(char on); /* 18de:144f, vector to overlay 5 1e8f:1150 */
extern void far title_show(void);    /* 18de:1445, vector to overlay 6 226c:01ff */
extern char far journal_check(void);    /* 18de:143b, vector to overlay 6 2294:008e */
extern char far design_menu(void);    /* 18de:1431, vector to overlay 26 2d91:051f */
extern void far game_done(void);    /* 18de:10d5, vector to overlay 6 2209:010c */
extern void far game_reset(void);    /* 18de:1427, vector to overlay 6 2209:0448 */
extern void far pending_load_set(char n);  /* 18de:141d, vector to overlay 4 1cff:08e5 */
extern void far game_load_menu(void);    /* 18de:1413, vector to overlay 4 1ffa:15c7 */
extern void far prev_pos_save(void);    /* 18de:1409, vector to overlay 4 1cff:03c7 */
extern char far party_menu(char on); /* 18de:13ff, vector to overlay 9 24ad:04bf */
extern void far party_display(void far *node); /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_info_display(void);    /* 18de:105d, vector to overlay 4 1c86:0406 */
extern void far view_attrs(char a, char b, char c, char d);
                                       /* 18de:13f5, vector to overlay 13 26e3:0572 */
extern void far game_loop(void);    /* 18de:13eb, vector to overlay 4 1cff:0b1c */
extern char far pending_load_get(void);    /* 18de:13e1, vector to overlay 4 1cff:08f0 */

extern int cur_level;                                            /* 6719 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern char debug_word[];                                          /* b3f4 */
extern int view_mode;                                            /* b421 */
extern int saved_view_mode;                                            /* b433 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern unsigned char start_area;                                  /* ba38 */
extern unsigned char start_entry;                                  /* ba39 */
extern char copyprot_markers[];                                          /* c650 */
extern unsigned char entry_spot;                                  /* cb09 */
extern char test_mode;                                           /* cc7a */
extern long menu_timeout;                                        /* cfd2 */
extern char timeout_key;                                         /* cfd6 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Geo geo;                                           /* d414 */

static void far game_session(void);

void far disk_msg_status(char kind, char disk, char far *text)   /* 2b88:0007 */
{
    char buf[40];

    box_draw();
    if (kind) {
        if (disk == 'A')
            disk = '1';
        else if (disk == 'B')
            disk = '2';
        else if (disk == 'C')
            disk = '3';
        str_format(buf, text, disk);
        status_printf(0, 0x18, 0x0f, 8, buf);
    }
    screen_flush();
}

void far disk_msg_text(char kind, char disk, char far *text)     /* 2b88:0074 */
{
    if (disk == 'A')
        disk = '1';
    else if (disk == 'B')
        disk = '2';
    else if (disk == 'C')
        disk = '3';
    prompt_attr_set(0x0f);
    win_text_printf(text, disk);
}

int far game_main(int argc, char far *far *argv)                 /* 2b88:00b7 */
{
    game_setup();
    sys_init(argc, argv, 0x90, 500);
    if (pointer_mode_set(-1) == 0)
        pointer_hide();
    msg_set_table(argc, argv);
    disk_setup(disk_msg_text, 1, "Pod", 'S');
    game_init();
    start_load();
    game_load(1);
    prompt_window_push(0x1fa0, 0);
    if (str_cmp(msg_text(2), "Heart"))
        title_show();
    frame_select(0);
    prompt_window_pop();
    pal_select(0);
    disk_setup(disk_msg_status, 1, "Pod", 'S');
    pointer_show();
    menu_timeout = 0;
    timeout_key = 1;
    sfx_play(0xff);
    if (str_cmp(msg_text(3), debug_word)) {
        if (!journal_check())
            game_exit();
        for (tick_slot = 1; tick_slot < 4; tick_slot++)
            if (!copyprot_markers[tick_slot])
                game_exit();
    }
    while (design_menu())
        game_session();
    dl_done();
    if (!str_cmp(msg_text(2), "Heart"))
        mem_report(0);
    game_sound_stop();
    sys_shutdown();
    game_done();
    dos_call(0x4c00);
    return 0;
}

static void far game_session(void)                              /* 2b88:0249 */
{
    if (!test_mode)
        game_reset();
    cur_level = start_area;
    entry_spot = start_entry - 1;
    do {
        pending_load_set(0);
        sfx_play(0xff);
        if (test_mode) {
            game_load_menu();
            if (node_list == 0) {
                status_msg("No saved games!", 0x0b, 0);
                return;
            }
            if ((view_mode = saved_view_mode) == 0)
                view_mode = 4;
            prev_pos_save();
        } else if (!party_menu(1))
            break;
        if (view_mode == 3 && cursor_obj[0x24] == 1)
            frame_lower_panel();
        else {
            frame_main();
            party_display(cur_node);
            party_info_display();
        }
        view_attrs(geo.attr_c, geo.attr_d, geo.attr_5, geo.attr_6);
        game_loop();
        view_mode = saved_view_mode;
    } while (pending_load_get());
    while (cur_node)
        hero_drop(0, 1);
    sfx_play(0xff);
}
