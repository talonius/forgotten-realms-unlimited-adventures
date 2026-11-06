/* CKIT.EXE overlay 09 module 24ad: the party menu.  Creating, deleting,
 * modifying and training characters, changing a human's class, viewing,
 * adding and removing party members, loading and saving the game, and
 * leaving the menu to play or to the caller.
 *
 * EXE 0x048d6a..0x049ea1, one Turbo C large-model _TEXT segment starting at
 * byte a of paragraph 24ad, the last module of RTLink section 9 (payload
 * 0x45fd0, link segment 21d4, parent section 3).  The first ten bytes of
 * paragraph 24ad end camp.c's module 21d4; fifteen zero bytes follow this
 * module's last retf to the section's end.  Built with Turbo C 2.01, tcc
 * -ml -O -Z -d.  Functions are defined in shipped order; bake consumes them
 * in that order.
 *
 * A bare stored seg:off in this file means section 9.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and a mod18de_ callee is an RTLink vector
 * stub whose target is named beside it.  mode_change is section 3's, this
 * section's parent, and is called directly.
 *
 * party_menu is reached through the vector stub 18de:13ff, gamemain.c's
 * party_menu.  party_drop is also far-called from camp.c; party_menu
 * far-calls party_add and party_drop ahead of their definitions, and
 * near-calls party_delete ahead of its own, so it is static.
 *
 * This is the v1.0 build (CKIT.EXE 1.0, file 0x48d5d..0x49e49); the
 * addresses above are v1.2's.  v1.0's party_dialog does not grey out
 * Create/Delete, Load..Exit and item 0xb when test_mode (a saved game is
 * being loaded) is set, and party_menu's Modify case does not copy
 * start_xp into create_xp before modify_hero.
 *
 * _DATA is DGROUP 7852..79f8, 25 stored literals; -d stores "" on the NUL
 * of "Heart".  _BSS is three ints at d204..d209, which only this module is
 * known to write.  The menu's enable flags and the texts it shows from
 * DGROUP 39d8..3c67 are extern.
 */

struct Node {
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct Hero {                   /* a party member, 0x18e bytes            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[0x5c];           /* +4   not read here                     */
    char name[0x10];            /* +60                                    */
    char pad_70[0x23];          /* +70  not read here                     */
    unsigned char control;      /* +93  bit 7: not a player character     */
    char pad_94[9];             /* +94  not read here                     */
    unsigned char level[7];     /* +9d  one per class; 0: not that class  */
    char pad_a4[0xda];          /* +a4  not read here                     */
    char active;                /* +17e 0: dumped out back on a drop      */
    char pad_17f[0xf];          /* +17f not read here                     */
};

struct Pool;
struct Effect;

extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far *far msg_text(int i);                            /* 0a75:039b */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_main(void);                                /* 0d65:00de */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far frame_gen(void);                                 /* 0d65:031b */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern char far archive_open(char disk, char far *name, int flags,
                             char (far *fn)(int fd, char far *path)); /* 0e23:01eb */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_run(int (far *idle)(void));              /* 15e5:1589 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far ui_send(int first, int last, int msg, ...);      /* 15e5:18f0 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far hero_drop(char keep, char icon);                 /* 03d0:1e69 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern char far get_picked(void);                                /* 083a:0308 */
extern void far set_picked(char c);                              /* 083a:030c */
extern struct Node far *far list_nth(int n, struct Node far *head); /* 083a:033e */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far menu_frame(int row, int col);                    /* 083a:0c1f */
extern void far frame_invalidate(void);                          /* 083a:0d3c */
extern void far set_menu_kind(int kind);                         /* 083a:13f2 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far mode_change(int mode);                           /* overlay 3 1b41:02fe */
extern void far party_display(void far *node);
                                /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_key_select(unsigned char c);
                                /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern void far char_view(char far *done);
                                /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far game_load_menu(void);
                                /* 18de:1413, vector to overlay 4 1ffa:15c7 */
extern void far pending_load_set(char n);
                                /* 18de:141d, vector to overlay 4 1cff:08e5 */
extern void far game_reset(void);
                                /* 18de:1427, vector to overlay 6 2209:0448 */
extern void far party_character_append(struct Hero far *hero);
                                /* 18de:16a7, vector to overlay 4 1ffa:1bac */
extern void far character_file_lists_free(struct Node far *list, struct Node far *files);
                                /* 18de:16b1, vector to overlay 4 1ffa:1cba */
extern void far game_save_menu(void);
                                /* 18de:16bb, vector to overlay 4 1ffa:1a85 */
extern void far character_icon_load(char on);
                                /* 18de:16c5, vector to overlay 4 1ffa:04aa */
extern void far character_file_save(struct Hero far *hero, char far *file);
                                /* 18de:16cf, vector to overlay 4 1ffa:0c2c */
extern void far character_file_load(char far *file, struct Hero far *hero, char a, char b);
                                /* 18de:16d9, vector to overlay 4 1ffa:0a69 */
extern void far character_files_scan(struct Node far * far *list, struct Node far * far *files,
                             char kind);
                                /* 18de:16e3, vector to overlay 4 1ffa:043b */
extern void far modify_hero(void);
                                /* 18de:1959, vector to overlay 6 22c2:12dc */
extern void far hero_create(void far *p);
                                /* 18de:19b3, vector to overlay 7 21d4:31bb */
extern void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove);
                                /* 18de:19bd, vector to overlay 19 2e88:1648 */
extern void far train_hero(void);
                                /* 18de:19db, vector to overlay 7 25e1:06b9 */
extern int  far human_old_class(struct Hero far *hero);
                                /* 18de:1a03, vector to overlay 7 25e1:061d */
extern int  far change_class(struct Hero far *hero);
                                /* 18de:1a0d, vector to overlay 7 25e1:0154 */
extern char far is_human(struct Hero far *hero);
                                /* 18de:1a17, vector to overlay 14 2ccd:0c1f */

extern char party_items[12];                                     /* 39d8 */
extern char far *exit_unsaved_text;                              /* 3a74 */
extern char far *add_text;                                       /* 3ab8 */
extern char far *delete_text;                                    /* 3abc */
extern char far *add_items;                                      /* 3c60 */
extern char far *delete_items;                                   /* 3c64 */
extern char design_dir[];                                          /* 5a54 */
extern char quit_requested;                                           /* 6725 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char view_kind;                                           /* b40e */
extern int view_mode;                                            /* b421 */
extern char esc_check;                                           /* b423 */
extern char game_saved;                                           /* b425 */
extern int saved_view_mode;                                            /* b433 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Pool party_pool;                                    /* b76e */
extern char door_tries[];                                         /* ba04 */
extern char adventure_exit;                                           /* bcf6 */
extern int file_kind;                                            /* bcfb */
extern int frame_col;                                            /* c3a3 */
extern int frame_row;                                            /* c3a5 */
extern char free_train;                                          /* c524 */
extern char prev_single_human;                                           /* ca0e */
extern char menu_escaped;                                        /* ca2a */
extern void far *node_pool;                                      /* ce5d */
extern struct Hero far *node_list;                               /* d129 */
extern char training_selection_changed;                                           /* d138 */

static int last_pick;                                            /* d204 */
static int pick_key;                                             /* d206 */
static int pick_first;                                           /* d208 */

void far party_add(void);
static void party_delete(void);
void far party_drop(void);

char far party_key(int id, short key)                            /* 34ad:000a */
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
    else if (key == 0x153)
        music_toggle();
    else if (cur_node) {
        if (key == 'J' || key == 'j') {
            if (str_cmp(msg_text(2), "Heart"))
                free_train = 0;
            else {
                free_train = !free_train;
                if (free_train)
                    error_box("training on");
                else
                    error_box("training off");
                if (!cursor_obj[0x30])
                    party_items[3] = 0;
                ui_mark(id, 0x14);
                return 1;
            }
        } else if (key == 0x107 || key == 0x105 || key == 0x108 || key == 0x104) {
            prev_single_human = is_human(cur_node) && human_old_class(cur_node) == 0x11;
            if (key == 0x107 || key == 0x108)
                key = 0x87;
            else
                key = 0x85;
            party_key_select(key);
            party_display(cur_node);
            training_selection_changed = (is_human(cur_node) && human_old_class(cur_node) == 0x11)
                        ^ prev_single_human && cursor_obj[0x30] > 0;
            return 1;
        }
    }
    return 0;
}

int far party_dialog(void)                                       /* 34ad:01ba */
{
    register int i;
    register int pick;

    frame_invalidate();
    dl_begin();
    ui_add(1, 0x1f7b, 0x1f94, "Create Character", 0x20, 'C', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f82, 0x1f94, "Delete Character", 0x20, 'D', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f89, 0x1f44, "Modify Character", 0x20, 'M', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f90, 0x1f44, "Train Character", 0x20, 'T', 0x24, 0x12, 0x14, 0x15,
           0);
    ui_add(1, 0x1f97, 0x1f44, "Human Change Class", 0x20, 'H', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f9e, 0x1f44, "View Character", 0x20, 'V', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f7b, 0x1f44, "Add Character", 0x20, 'A', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f82, 0x1f44, "Remove Character", 0x20, 'R', 0x24, 0x12, 0x14, 0x15,
           0);
    ui_add(1, 0x1f89, 0x1f94, "Load Saved Game", 0x20, 'L', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f90, 0x1f94, "Save Current Game", 0x20, 'S', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f97, 0x1f94, "Begin Adventuring", 0x20, 'B', 0x24, 0x12, 0x14, 0x15,
           1, 0x1f9e, 0x1f94, "Exit From Play", 0x20, 'E', 0x24, 0x12, 0x14, 0x15,
           0);
    ui_add(7, party_key, 0x14, 0);
    frame_row = 0x1f50;
    frame_col = 0x1f44;
    set_menu_kind(9);
    menu_frame(frame_row, frame_col);
    for (i = 0; i < 12; i++)
        if (party_items[i])
            ui_mark(i, 0x18);
        else
            ui_mark(i, 0x10);
    dl_draw(1);
    screen_flush_mode_set(0);
    screen_flush();
    pick = ui_run(0);
    if (get_picked()) {
        pick = 5;
        set_picked(0);
    }
    last_pick = pick;
    dl_end();
    return pick;
}

char far party_menu(char in_game)                                /* 34ad:04bf */
{
    struct Hero far *p;
    short count;
    char redraw;
    short kind;
    struct Effect far *fx;
    register int pick;

    pick_key = 0x87;
    saved_view_mode = view_mode;
    view_mode = 0;
    training_selection_changed = 1;
    mem_fill(door_tries, 4, 1);
    pick = 1;
    pick_first = 0;
    mode_change(6);
    for (;;) {
        screen_flush_mode_set(1);
        if (pick <= 11)
            frame_gen();
        else
            screen_draw_begin(1);
        if (cur_node) {
            party_display(cur_node);
            party_items[1] = 1;
            party_items[2] = 1;
            party_items[3] = cursor_obj[0x30] > 0 || free_train;
            party_items[4] = party_items[3];
            party_items[5] = 1;
            party_items[6] = 1;
            count = 0;
            for (p = node_list; p; p = p->next)
                if (!(p->control & 0x80))
                    count++;
            if (count > 5)
                party_items[6] = 0;
            party_items[7] = 1;
            party_items[8] = 1;
            party_items[9] = 1;
            party_items[10] = 1;
        } else {
            party_items[1] = 1;
            party_items[2] = 0;
            party_items[3] = 0;
            party_items[4] = 0;
            party_items[5] = 0;
            party_items[6] = 1;
            party_items[7] = 0;
            party_items[8] = 1;
            party_items[9] = 0;
            party_items[10] = 0;
        }
        pick = party_dialog();
        if (pick != 7 && pick <= 11)
            frame_border();
        if (pick == 1)
            party_display(cur_node);
        switch (pick) {
        case 0:
            if (party_items[0])
                hero_create(0);
            break;
        case 1:
            if (party_items[1])
                party_delete();
            break;
        case 2:
            if (party_items[2]) {
                modify_hero();
                game_saved = 0;
            }
            break;
        case 3:
            if (party_items[3]) {
                train_hero();
                game_saved = 0;
            }
            break;
        case 4:
            if (party_items[4]) {
                kind = change_class(cur_node);
                if (kind != 0x11) {
                    if (find_effect(cur_node, 8, &fx))
                        fx_remove(cur_node, 8, 0);
                    if (find_effect(cur_node, 0x69, &fx))
                        fx_remove(cur_node, 0x69, 0);
                    switch (kind) {
                    case 3:
                        fx_add(cur_node, 8, 0, 0xff, 0);
                        break;
                    case 4:
                        fx_add(cur_node, 0x69, 0, 0xff, 0);
                        break;
                    }
                }
                game_saved = 0;
            }
            break;
        case 5:
            if (party_items[5])
                char_view(&redraw);
            break;
        case 6:
            if (party_items[6]) {
                party_add();
                game_saved = 0;
            }
            break;
        case 7:
            if (party_items[7] && cur_node) {
                if (!(cur_node->control & 0x80)) {
                    character_file_save(cur_node, "");
                    hero_drop(0, 1);
                } else {
                    frame_border();
                    party_drop();
                }
                if (!cur_node) {
                    if (in_game) {
                        game_reset();
                        view_mode = 0;
                    } else
                        adventure_exit = 1;
                }
                game_saved = 0;
            }
            break;
        case 8:
            if (party_items[8] &&
                (!cur_node || game_saved || confirm("Game not saved. Load anyway? ", 1))) {
                if (in_game) {
                    while (cur_node)
                        hero_drop(0, 1);
                    game_load_menu();
                    if (!cur_node) {
                        game_reset();
                        saved_view_mode = view_mode;
                        view_mode = 0;
                    }
                } else {
                    pending_load_set(1);
                    adventure_exit = 1;
                    return 1;
                }
            }
            break;
        case 9:
            if (party_items[9] && node_list)
                game_save_menu();
            break;
        case 10:
            if (party_items[10] && (node_list || esc_check)) {
                if (!(view_mode = saved_view_mode))
                    view_mode = 4;
                if (!cursor_obj[0x86] && view_kind != 9) {
                    if (view_mode == 3 && cursor_obj[0x24] == 1)
                        frame_lower_panel();
                    else
                        frame_main();
                    party_display(cur_node);
                } else
                    switch (view_mode) {
                    case 0:
                    case 4:
                        frame_main();
                        party_display(cur_node);
                        break;
                    case 3:
                        frame_lower_panel();
                        break;
                    }
                box_draw();
                cursor_obj[0x30] = 0;
                game_saved = 0;
                return 1;
            }
            break;
        case 11:
            quit_requested = 1;
            if (party_items[11]) {
                if (node_list && !game_saved && !confirm(exit_unsaved_text, 0))
                    game_save_menu();
                else
                    return 0;
            }
            break;
        }
        if (!pick == 10 && pick <= 11)  /* 34ad:0998, always false as shipped */
            frame_border();
    }
}

char far in_party(char far *name)                                /* 34ad:09bc */
{
    struct Hero far *h;

    for (h = node_list; h; h = h->next)
        if (str_same(h->name, name))
            return 1;
    return 0;
}

void far party_add(void)                                         /* 34ad:0a09 */
{
    struct Node far *list;
    struct Node far *files;
    struct Node far *sel;
    struct Node far *file;
    struct Hero far *hero;
    struct Hero far *h;
    int index;
    char redraw;
    unsigned char players;
    unsigned char rangers;
    unsigned char choice;

    text_clear(1, 1, 0x26, 0x16);
    hotkey_reset(2);
    choice = 0;
    if (menu_escaped && choice == 0x1b)
        choice = 2;
    switch (choice) {
    case 1:
        file_kind = 2;
        break;
    case 2:
        return;
    default:
        file_kind = 1;
        break;
    }
    character_files_scan(&list, &files, 0);
    if (!list)
        return;
    sel = list;
    index = 0;
    do {
        for (sel = list; sel; sel = sel->next)
            if (in_party(sel->text))
                str_copy(sel->text, msg_format("* %s", sel->text));
        redraw = 1;
        hotkey_reset(1);
        choice = list_menu(add_text, add_items, 1, 2, 0x26, 0x16, list, 1, 0,
                           &redraw, &index, &sel);
        if (menu_escaped && choice == 0x1b)
            choice = 1;
        if (choice == 0 && sel->text[0] != '*') {
            pool_alloc(&hero, 0x18e, &party_pool);
            file = list_nth(index, files);
            character_file_load(file->text, hero, 0, 1);
            str_copy(sel->text, msg_format("* %s", sel->text));
            players = 0;
            if (!node_list) {
                cursor_obj[0x20] = 0;
                party_character_append(hero);
                character_icon_load(1);
            } else {
                h = node_list;
                rangers = 0;
                /* the names' addresses are compared, never their text */
                while (h && h->name != hero->name) {
                    if (h->control < 0x80)
                        players++;
                    if (h->level[4] > 0)
                        rangers++;
                    h = h->next;
                }
                if (!h &&
                    ((!(hero->control & 0x80) && players < 6) ||
                     ((hero->control & 0x80) && cursor_obj[0x20] < 8)) &&
                    (!hero->level[4] || rangers < 3)) {
                    party_character_append(hero);
                    character_icon_load(1);
                    if (!(hero->control & 0x80))
                        players++;
                } else {
                    if (hero->level[4] > 0 && rangers > 2)
                        bottom_msg("too many rangers in party");
                    pool_free(hero, 0x18e, &party_pool);
                }
            }
        }
    } while (choice != 1 && players <= 5 && cursor_obj[0x20] <= 7);
    character_file_lists_free(list, files);
}

static void party_delete(void)                                   /* 34ad:0d33 */
{
    struct Node far *list;
    struct Node far *files;
    struct Node far *sel;
    struct Node far *file;
    struct Node far *n;
    char prompt[82];
    int index;
    char redraw;
    char choice;

    text_clear(1, 1, 0x26, 0x16);
    file_kind = 1;
    list = files = 0;
    character_files_scan(&list, &files, 1);
    if (!list)
        return;
    sel = list;
    index = 0;
    while (sel) {
        if (in_party(sel->text))
            str_copy(sel->text, msg_format("* %s", sel->text));
        sel = sel->next;
    }
    do {
        redraw = 1;
        hotkey_reset(1);
        choice = list_menu(delete_text, delete_items, 1, 2, 0x26, 0x16, list, 1, 0,
                           &redraw, &index, &sel);
        if (menu_escaped && choice == 0x1b)
            choice = 1;
        if (choice == 0) {
            str_copy(prompt, msg_format("Delete %s forever? ",
                                            sel->text + (sel->text[0] == '*' ? 2 : 0)));
            if (confirm(prompt, 0) && confirm("Are you sure? ", 0)) {
                file = list_nth(index, files);
                prompt[0] = 0;
                path_append_name(prompt, design_dir);
                path_append_name(prompt, "SAVE");
                path_append_name(prompt, file->text);
                archive_open(0, prompt, 3, 0);
                if (list == sel)
                    list = list->next;
                else {
                    for (n = list; n->next != sel; n = n->next)
                        ;
                    n->next = n->next->next;
                }
                pool_free(sel, 0x28, node_pool);
                if (files == file)
                    files = files->next;
                else {
                    for (n = files; n->next != file; n = n->next)
                        ;
                    n->next = n->next->next;
                }
                pool_free(file, 0x28, node_pool);
            }
        }
    } while (choice != 1 && list);
    character_file_lists_free(list, files);
}

void far party_drop(void)                                        /* 34ad:104a */
{
    char prompt[42];

    if (cur_node) {
        str_copy(prompt, msg_format("Drop %s forever? ", cur_node->name));
        if (confirm(prompt, 0) && confirm("Are you sure? ", 0)) {
            if (!cur_node->active)
                bottom_msg(msg_format("You dump %s out back.", cur_node->name));
            else
                bottom_msg(msg_format("%s bids you farewell.", cur_node->name));
            hero_drop(0, 1);
        } else
            bottom_msg(msg_format("%s breathes a sigh of relief.", cur_node->name));
    }
    party_display(cur_node);
}
