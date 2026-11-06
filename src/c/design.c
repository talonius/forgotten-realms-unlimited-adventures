/* CKIT.EXE overlay section 26, module 2d91: the design menu, the program's
 * first screen.  design_menu shows "Play the Game", "Select a Design",
 * "Create New Design", "Delete the Design", "Unlock Editor", "Game
 * Settings", "Edit Modules", "Art Gallery", "Monster Editor" and "Quit
 * From Game" with the program's title, its version and the current game
 * design, and runs the pick: 0 returns 1 (play); 1 runs design_pick; 2
 * clears the design directory, loads the GAME entry and runs editloop.c's
 * edit_main in mode 6 (settings), keeping the design when it then has a
 * directory and restoring the old one when not; 3 runs edpick.c's
 * ask_screen (2e34:000e) with bits 7 and, when that leaves bits set
 * and the answer to "Are you sure?" is yes, design_delete, then selects
 * heirs.dsn; 5 asks
 * for the password and unlocks the editors when it equals the GAME
 * entry's (+0x174); 6 to 9 run edit_main in that mode; 11 asks
 * "Quit to DOS? " and returns 0.  A design with no password starts
 * unlocked (ed_unlocked); until it is unlocked items 6 to 9 are sent
 * 0x10, and after it item 5 is marked 0x10, the mark party.c gives an
 * item it disables; "Delete the Design" is marked 0x10 when the design
 * directory is heirs.dsn or tutorial.dsn.  When test_mode is set the menu
 * is not shown: it runs edit_main in mode 7 and returns 1 if test_mode is
 * still set (otherwise it shows the menu).  Overlay 3's
 * start_save (start.dat) is written after a pick, a delete and a new
 * design, and after edit_main when ed_mode changed.  design_pick lists
 * every NAME.DSN directory (in reverse of the directory search's order) by the text its GAME entry
 * starts with (its name when that is empty) and, on Select, makes the
 * picked one the design directory and returns 1; game_rec is saved
 * around the scan, and with no design it beeps.  design_delete deletes
 * each file in the design's SAVE directory, the directory, each file in
 * the design directory and the directory (DOS functions 41h and 3Ah).  design_key is the menu's key
 * hook: it drains the keyboard, handing Ins and Del to sfxctl.c's sound
 * switches.
 *
 * Section 26 (parent section 20, which is empty and has no parent; no
 * section names 26 as its parent): payload at EXE 0x080f10, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 26.  The module is stored frame 2d91 (load-based 3d91),
 * EXE 0x080f10..0x08194e (2,622 bytes, 4 routines), from the section's
 * first byte to the retf at 2e34:000d; module 2e34's first routine starts
 * at 2e34:000e (EXE 0x08194e).  One Turbo C large-model _TEXT segment,
 * built with Turbo C 2.01, tcc -ml -O -Z -d; functions are defined in
 * shipped order.  Definition comments give the load-based address; extern
 * comments the far pointer as stored in the shipped call, or the DGROUP
 * offset.
 *
 * Section 26's relocation table has 312 sites: 100 filed under 2d91, all
 * in this range (EXE 0x080f2e..0x08193b) and all far calls, 125 under 2e34
 * and 87 under 2fc3.  One of the section's twelve vectors lands here:
 * 18de:1431 -> design_menu, which gamemain.c calls as design_menu.
 * edpick.c (module 2e34) far-calls design_pick (2d91:0000, at EXE 0x081c78), the only
 * relocated word in section 26 that stores 2d91.  design_menu near-calls
 * design_pick and design_delete after their definitions and pushes
 * design_key's address after its definition, so the bytes do not decide
 * static or public for design_delete and design_key.
 *
 * FRUA's own code: the sharing map pairs design_pick and design_delete
 * a-only, and design_key and design_menu only partial (with DQK menu.c's
 * key_filter and party.c's party_dialog, the shape party.c's party_key
 * and party_dialog have here), so every name is chosen from the code.
 *
 * _DATA is DGROUP 92fc..93f9: ed_unlocked, then 20 stored literals; -d
 * stores each repeated one once ("" four times, "SAVE" three, "DSN" and
 * "heirs.dsn" twice).  The first "" keeps its own byte, 92fd.  Below it
 * 92fb is a pad byte after monedit.c's _DATA (section 25, 929c..92fa);
 * 93fa, above it, is edpick.c's (module 2e34).  No instruction outside this
 * range names 92fc..93f9.  No _BSS: every other DGROUP operand is
 * another module's.
 *
 * Forced, each re-baked against its alternative: titles = dirs = 0 (two
 * statements bake 3 of 4), the (unsigned char) cast on list_menu (0 of
 * 4), ed_mode = disk = 0 (3 of 4), bits && confirm(...) in one test
 * (3 of 4), and design_key's second if without else (2 of 4).  Not
 * forced: register on choice and pick, the for form of the walk.
 */

struct Node {                   /* a list_menu line, as menu.c's          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   a screen's result                 */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct Pool;

extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far dos_open(int ax, char far *name, ...);       /* 00d7:26bc */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern char far *far input_text(char far *prompt, unsigned char fg, unsigned char bg,
                                unsigned char max);              /* 06c9:05bc */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_gen(void);                                 /* 0d65:031b */
extern void far beep(void);                                      /* 0da7:018c */
extern char far dir_first(char disk, char far *name, char far *ext,
                          char files, char dirs);                /* 0e23:04d5 */
extern char far *far dir_next(char far *is_dir);                 /* 0e23:0608 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_run(int (far *idle)(void));              /* 15e5:1589 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far ui_send(int first, int last, int msg, ...);      /* 15e5:18f0 */
extern void far path_ext(char far *path, char far *out);         /* 17d3:02ea */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far mode_change(int n);
                                /* 18de:10cb, vector to overlay 3 1b41:02fe */
extern void far design_dir_set(char far *dir);
                                /* 18de:1369, vector to overlay 3 1b41:0354 */
extern void far design_dir_restore(void);
                                /* 18de:1373, vector to overlay 3 1b41:037c */
extern void far start_save(void);
                                /* 18de:139b, vector to overlay 3 1b41:038f */
extern void far game_load(char on);
                                /* 18de:144f, vector to overlay 5 1e8f:1150 */
extern void far area_start_select(void);
                                /* 18de:17f1, vector to overlay 5 1e8f:1188 */
extern void far labels_forget(void);
                                /* 18de:184b, vector to overlay 5 1e8f:0014 */
extern void far edit_main(int mode);
                                /* 18de:1bbb, vector to overlay 10 267d:0002 */
extern void far view3d_noop(void);
                                /* 18de:1cf1, vector to overlay 13 26e3:0556 */
extern int  far ask_screen(int prev, struct EdEnv far *env, void far *arg); /* 2e34:000e */

extern char far *game_design_text;                          /* 02d0  "Game Design"  */
extern char far *a_text;                          /* 0364  "a"            */
extern char far *please_text;                          /* 03a4  "Please"       */
extern char far *select_word;                          /* 03bc  "Select"       */
extern char far *cancel_text;                          /* 03c8  "Cancel"       */
extern char far *quit_text;                         /* 3a6c  "Quit to DOS? " */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *title_text;                /* 3bc4  "  Unlimited Adventures" */
extern char far *version_text;              /* 3bc8  "Version 1.2 ..."        */
extern char design_dir[0x22];                                      /* 5a54 */
extern char game_rec[];                                          /* ba08 */
extern char menu_escaped;                                        /* ca2a */
extern char test_mode;                                           /* cc7a */
extern void far *node_pool;                                      /* ce5d */
extern char ed_mode;                                           /* d136 */

static char ed_unlocked = 0;                                     /* 92fc */

char far design_pick(void)                                       /* 3d91:0000 */
{
    char save[0x184];
    struct Node far *titles;
    struct Node far *dirs;
    struct Node far *sel;
    struct Node far *d;
    struct Node far *t;
    int top;
    char text[0x20];
    char far *name;
    char is_dir;
    char done;
    char redraw;
    register int choice;

    done = 0;
    mem_move(game_rec, save, 0x184);
    titles = dirs = 0;
    if (dir_first(0, "", "DSN", 0, 1))
        while ((name = dir_next(&is_dir)) != 0) {
            path_ext(name, text);
            if (str_same(text, "DSN")) {
                sel = titles;
                pool_alloc(&titles, sizeof(struct Node), node_pool);
                d = dirs;
                pool_alloc(&dirs, sizeof(struct Node), node_pool);
                if (!titles || !dirs) {
                    if (titles)
                        pool_free(titles, sizeof(struct Node), node_pool);
                    titles = sel;
                    dirs = d;
                } else {
                    titles->next = sel;
                    dirs->next = d;
                }
                titles->key = 0;
                str_copy(dirs->text, name);
                design_dir_set(name);
                game_load(0);
                design_dir_restore();
                str_copy(titles->text, game_rec);
                if (titles->text[0] == 0)
                    str_copy(titles->text, name);
            }
        }
    mem_move(save, game_rec, 0x184);
    if (titles) {
        frame_border();
        status_printf(5, 2, 0xf, 0, msg_format("%s %s %s %s:", please_text, select_word,
                                               a_text, game_design_text));
        sel = titles;
        top = 0;
        redraw = 1;
        str_format(text, "%s %s", select_word, cancel_text);
        hotkey_reset(1);
        choice = (unsigned char)list_menu(empty_text, text, 1, 4, 0x26, 0x16, titles, 1, 0,
                                          &redraw, &top, &sel);
        if (menu_escaped && choice == 0x1b)
            choice = 1;
        if (choice == 0) {
            for (t = titles, d = dirs; t != sel; t = t->next, d = d->next)
                ;
            design_dir_set(d->text);
            labels_forget();
            done = 1;
        }
        list_free(&titles);
        list_free(&dirs);
    } else
        beep();
    return done;
}

void far design_delete(void)                                     /* 3d91:0339 */
{
    char path[0x50];
    char far *name;
    char is_dir;

    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    if (dir_first(0, path, 0, 1, 0))
        while ((name = dir_next(&is_dir)) != 0) {
            path[0] = 0;
            path_append_name(path, design_dir);
            path_append_name(path, "SAVE");
            path_append_name(path, name);
            dos_open(0x4100, path);
        }
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    dos_open(0x3a00, path);
    path[0] = 0;
    path_append_name(path, design_dir);
    if (dir_first(0, path, 0, 1, 0))
        while ((name = dir_next(&is_dir)) != 0) {
            path[0] = 0;
            path_append_name(path, design_dir);
            path_append_name(path, name);
            dos_open(0x4100, path);
        }
    path[0] = 0;
    path_append_name(path, design_dir);
    dos_open(0x3a00, path);
}

char far design_key(int id, int key)                             /* 3d91:04d3 */
{
    while (key_poll()) {
        if (key == 0x152)
            sound_toggle();
        else if (key == 0x153)
            music_toggle();
        key = key_get();
    }
    if (key == 0x152)
        sound_toggle();
    if (key == 0x153)
        music_toggle();
    return 0;
}

char far design_menu(void)                                       /* 3d91:051f */
{
    long bits;
    char disk;
    register int pick;

    disk = ed_mode;
    view3d_noop();
    labels_forget();
    if (!ed_unlocked && !game_rec[0x174])
        ed_unlocked = 1;
    for (;;) {
        if (test_mode)
            pick = 7;
        else {
            mode_change(6);
            screen_flush_mode_set(1);
            frame_gen();
            dl_begin();
            ui_add(1, 0x1f7b, 0x1f44, "Play the Game", 0x20, 'P', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f82, 0x1f44, "Select a Design", 0x20, 'S', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f89, 0x1f44, "Create New Design", 0x20, 'C', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f90, 0x1f44, "Delete the Design", 0x20, 'D', 0x24, 0x12, 0x14, 0x15,
                   0);
            ui_add(1, 0x1f97, 0x1f44, "", 0x24, 0x12, 0x14, 0x15, 0x10,
                   1, 0x1f9e, 0x1f44, "Unlock Editor", 0x20, 'U', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f7b, 0x1f94, "Game Settings", 0x20, 'G', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f82, 0x1f94, "Edit Modules", 0x20, 'E', 0x24, 0x12, 0x14, 0x15,
                   0);
            ui_add(1, 0x1f89, 0x1f94, "Art Gallery", 0x20, 'A', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f90, 0x1f94, "Monster Editor", 0x20, 'M', 0x24, 0x12, 0x14, 0x15,
                   1, 0x1f97, 0x1f94, "", 0x24, 0x12, 0x14, 0x15, 0x10,
                   1, 0x1f9e, 0x1f94, "Quit From Game", 0x20, 'Q', 0x24, 0x12, 0x14, 0x15,
                   0);
            ui_add(7, design_key, 0x14, 0);
            if (str_same(design_dir, "heirs.dsn") || str_same(design_dir, "tutorial.dsn"))
                ui_mark(3, 0x10);
            if (!ed_unlocked)
                ui_send(6, 9, 0x10);
            else
                ui_mark(5, 0x10);
            dl_draw(1);
            status_printf(8, 3, 0xb, 0, title_text);
            status_printf(4, 4, 7, 0, version_text);
            status_printf(4, 9, 0xb, 0, "Current Game Design:");
            status_printf(0x19, 9, 7, 0, design_dir);
            status_printf(4, 0xa, 7, 0, game_rec);
            screen_flush_mode_set(0);
            screen_flush();
            pick = ui_run(0);
            dl_end();
        }
        switch (pick) {
        case 0:
            return 1;
        case 1:
            if (design_pick()) {
                labels_forget();
                game_load(1);
                ed_unlocked = !game_rec[0x174];
                start_save();
            }
            break;
        case 2:
            design_dir_set("");
            game_load(0);
            edit_main(6);
            if (design_dir[0]) {
                ed_mode = disk = 0;
                labels_forget();
                start_save();
                ed_unlocked = 1;
                area_start_select();
            } else {
                design_dir_restore();
                game_load(0);
            }
            break;
        case 3:
            bits = 7;
            ask_screen(0, (struct EdEnv far *)&bits, 0);
            if (bits && confirm("Are you sure?", 0)) {
                design_delete();
                labels_forget();
                design_dir_set("heirs.dsn");
                game_load(1);
                ed_unlocked = !game_rec[0x174];
                start_save();
            }
            break;
        case 5:
            frame_border();
            screen_flush();
            if (str_same(input_text("Enter Password: ", 7, 8, 0xf), game_rec + 0x174))
                ed_unlocked = 1;
            else
                beep();
            break;
        case 6:
        case 7:
        case 8:
        case 9:
            edit_main(pick);
            if (ed_mode != disk) {
                disk = ed_mode;
                start_save();
            }
            if (pick == 7 && test_mode)
                return 1;
            break;
        case 11:
            frame_border();
            if (confirm(quit_text, 0))
                return 0;
            break;
        }
    }
}
