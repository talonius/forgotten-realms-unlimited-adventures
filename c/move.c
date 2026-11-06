/* CKIT.EXE overlay 8 module 254b: moving the party.  The check that the
 * square ahead is still inside the area, the dungeon step, a step on the
 * wilderness map, the list of the quest items the party holds, the
 * adventure menu, and resolving a queued move -- with FRUA's doors:
 * bashing one by strength, picking its lock, casting Knock, the secret
 * door message, and the per-square event counters a dungeon step bumps.
 *
 * EXE 0x04480e..0x045a84, one Turbo C large-model _TEXT segment starting at
 * 254b:000e in RTLink section 8 (payload 0x41090, link segment 21d4,
 * parent section 3): module 21d4's final retf is 254b:000d, and this
 * module's first byte is the lone retf after it.  Its own final retf is
 * 254b:1283, byte 3 of paragraph 2673, where cursor.c begins at 2673:0004
 * (section 8's table files the last 21d4 site at 0x044805, 99 sites under
 * 254b from 0x044835 to 0x045a51, and the first 2673 site at 0x045ab0).
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.  Module 21d4 far-calls
 * step_dungeon; code outside the section reaches adv_menu through the
 * vector stub 18de:15a3 and move_resolve through 18de:1599.
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
 * move_ahead, step_dungeon, step_wild, quest_list and adv_menu are DQK's
 * move.c relinked, with FRUA's records and the changes the bytes show: the
 * area's rows and columns are at +2 and +3; a dungeon step also sets the
 * four flags at door_tries (door_bash clears [1], door_pick [2]) and clears
 * the 12 bytes at tavern_tales; the quest list shows 0x40 counted entries
 * under test_mode and otherwise the named items of key_item_names, and frees the
 * list it built; the adventure menu's Look also looks up the square's
 * event and runs it, and its wilderness directions no longer redraw.
 * move_resolve keeps only the shape of DQK's:
 * a dungeon step asks what is ahead first and offers Bash, Pick and Knock
 * at a locked door, and a wilderness step reads the square's text.
 * move_120c and move_1211 are empty.
 *
 * _DATA is DGROUP 75dc..762b: the code's six literals, stored as five --
 * -d stores "Blocked:" once.  _BSS is next_x and next_y, the wilderness
 * square ahead, at d202..d203.
 *
 * This is the v1.0 build (CKIT.EXE 1.0, module at file offset 0x44819,
 * TEXT 3550:0009, _DATA at DGROUP 75b0, 39 bytes: five literals stored as
 * four; _BSS at d1a4); the addresses above are v1.2's.  What differs from
 * v1.2: next_square clamps the wilderness square ahead itself -- off the
 * 0x26 by 0xf map it falls back to the party's own square -- so
 * move_resolve's wilderness step has no range tests of its own and no
 * "There is no way to go in that direction." text.
 */

struct StepEvent {              /* one of an area's 8 counted events      */
    unsigned steps;             /* +0  steps before it fires              */
    char event;                 /* +2  0: none                            */
    unsigned char zones_off;    /* +3  bit n: not facing n                */
};

struct Area {                   /* what cur_area_rec points at                */
    char pad_0[2];              /* +0   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[0x4a];           /* +4   not read here                     */
    struct StepEvent step_events[8]; /* +4e                                    */
    char pad_6e[0xa2];          /* +6e  not read here                     */
    int zone_texts_head[1];     /* +110 by what 18de:1355 returns         */
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

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[0x5a];           /* +4   not read here                     */
    unsigned char status;       /* +5e                                    */
    char pad_5f[0x12];          /* +5f  not read here                     */
    unsigned char str;          /* +71                                    */
    char pad_72[0xa];           /* +72  not read here                     */
    unsigned char str_ex;       /* +7c                                    */
    char pad_7d[0xf];           /* +7d  not read here                     */
    unsigned char pick_locks;   /* +8c  percent                           */
    char pad_8d[0x10];          /* +8d  not read here                     */
    unsigned char level[7];     /* +9d                                    */
    unsigned char old_level[7]; /* +a4                                    */
    char pad_ab[0x1b];          /* +ab  not read here                     */
    unsigned char spells[0x8d]; /* +c6                                    */
};

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Node {
    struct Node far *next;      /* +0                                    */
    char key;                   /* +4                                    */
    char text[35];              /* +5                                    */
};

extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far bank_text_get(char far *pool, int slot, char far *out); /* 03ad:0141 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern char far old_class_abilities_available(struct Hero far *h);                   /* 03d0:2100 */
extern void far status_msg(char far *fmt, char fg, char bg, ...); /* 06c9:08f7 */
extern void far msg_pause(void);                                 /* 06c9:0953 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern void far list_alloc(int n, struct Node far * far *head);  /* 083a:0575 */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far node_dir_menu(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1ae7 */
extern char far grid_small(char far *title, char far *text, char fkeys, char anim,
                           unsigned char rows, unsigned char cols); /* 083a:1be9 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern char far *far msg_text(int i);                            /* 0a75:039b */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far frame_border(void);                              /* 0d65:002a */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far party_info_display(void);             /* 18de:105d, vector to overlay 4 1c86:0406 */
extern void far party_display(void far *node);   /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);  /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far char_view(char far *done);   /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far clock_advance(unsigned char n, short unit);
                                                /* 18de:11bb, vector to overlay 19 2d91:02bf */
extern char far wall_side(char x, char y, char dir);
                                                /* 18de:1341, vector to overlay 13 26e3:09f3 */
extern unsigned char far cell_zone_at(char x, char y);
                                                /* 18de:1355, vector to overlay 13 26e3:0b87 */
extern void far view_draw(char x, char y, unsigned char dir);
                                                /* 18de:1495, vector to overlay 13 26e3:0bdb */
extern void far le_word_put(unsigned n, unsigned far *p);
                                                /* 18de:14ef, vector to overlay 4 1cff:0473 */
extern unsigned far le_word_get(unsigned far *p);
                                                /* 18de:1503, vector to overlay 4 1cff:04c8 */
extern void far events_run(char kind);        /* 18de:152b, vector to overlay 4 1cff:29dd */
extern char far wall_kind(char x, char y, char dir);
                                                /* 18de:1567, vector to overlay 13 26e3:080d */
extern char far area_cell_at(char x, char y);   /* 18de:15ad, vector to overlay 13 26e3:0ae7 */
extern void far var_name(char far *text, int n, char c);
                                                /* 18de:1a99, vector to overlay 26 2fc3:05b8 */
extern void far cast_menu(void);             /* 18de:1aa3, vector to overlay 9 21d4:0581 */
extern void far pic_release(void);                           /* overlay 3 1bbc:02b4 */
extern void far event_print(char far *text);                 /* 21d4:09a8 */
extern void far cursor_show(void);                               /* 2673:0006 */
extern void far cursor_hide(void);                               /* 2673:0067 */

extern char far *not_here_text;         /* 3b2c, "not here"               */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *explore_items;         /* 3c7c, "Move Area Cast ..."     */
extern char far *exit_items;                                     /* 3c80 */
extern char far *door_items;            /* 3cf7, "Bash Pick Knock Exit"   */
extern int tavern_tales[6];                                          /* 5b48 */
extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern char far *text_pool_block;                                /* a7ec */
extern char msg_buf[];                                          /* b259 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char redrawn;                                             /* b3f3 */
extern char debug_word[];                                          /* b3f4 */
extern char turn_done;                                           /* b414 */
extern int view_mode;                                            /* b421 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char exploration_menu_context;                                           /* b4e8 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern int word_b8d4;                                            /* b8d4 */
extern char event_text_cleared;                                           /* b8f4 */
extern char door_tries[];                /* ba04  four door flags          */
extern char key_item_names[][16];                                      /* ba3c */
extern char adventure_exit;                                           /* bcf6 */
extern int word_be66;                                            /* be66 */
extern char square_event;                                           /* c3bf */
extern char menu_escaped;                                        /* ca2a */
extern char test_mode;                                           /* cc7a */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Geo geo;                                           /* d414 */
extern struct Area far *cur_area_rec;                                /* d41c */

static unsigned char next_x;                                     /* d202 */
static unsigned char next_y;                                     /* d203 */

static void far move_120c(char x, char y, char dir);
static void far move_1211(char x, char y, char dir);
static char far party_has_class(int cls);

/* The translated Pascal unit's empty initialization block (DKK explore.pas). */
void far move_unit_init(void)                                         /* 354b:000e */
{
}

char far door_bash(void)                                         /* 354b:000f */
{
    char rc;
    struct Hero far *p;
    char done;

    done = 0;
    p = node_list;
    while (p && !done) {
        if (wall_kind(geo.x, geo.y, geo.facing) == 5
            || wall_kind(geo.x, geo.y, geo.facing) == 6) {
            switch (p->str) {
            case 0x12:
                switch (p->str_ex) {
                case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
                case 0x60: case 0x61: case 0x62: case 0x63:
                    if (fx_roll_dice(1, 6) == 1)
                        done = 1;
                    break;
                case 0x64:
                    if (fx_roll_dice(1, 6) <= 2)
                        done = 1;
                    break;
                }
                break;
            case 0x13: case 0x14:
                if (fx_roll_dice(1, 6) <= 3)
                    done = 1;
                break;
            case 0x15: case 0x16:
                if (fx_roll_dice(1, 6) <= 4)
                    done = 1;
                break;
            case 0x17:
                if (fx_roll_dice(1, 6) <= 5)
                    done = 1;
                break;
            case 0x18:
                if (fx_roll_dice(1, 8) <= 7)
                    done = 1;
                break;
            case 0x19:
                done = 1;
                break;
            }
        } else {
            switch (p->str) {
            case 3: case 4: case 5: case 6: case 7:
                if (fx_roll_dice(1, 6) == 1)
                    done = 1;
                break;
            case 8: case 9: case 0xa: case 0xb: case 0xc: case 0xd: case 0xe: case 0xf:
                if (fx_roll_dice(1, 6) <= 2)
                    done = 1;
                break;
            case 0x10: case 0x11:
                if (fx_roll_dice(1, 6) <= 3)
                    done = 1;
                break;
            case 0x12:
                if (p->str_ex < 0x33) {
                    if (fx_roll_dice(1, 6) <= 3)
                        done = 1;
                } else if (p->str_ex < 0x63) {
                    if (fx_roll_dice(1, 6) <= 4)
                        done = 1;
                } else if (fx_roll_dice(1, 6) <= 5)
                    done = 1;
                break;
            case 0x13: case 0x14:
                if (fx_roll_dice(1, 8) <= 7)
                    done = 1;
                break;
            case 0x15:
                if (fx_roll_dice(1, 0xa) <= 9)
                    done = 1;
                break;
            case 0x16: case 0x17:
                if (fx_roll_dice(1, 0xc) <= 0xb)
                    done = 1;
                break;
            case 0x18:
                if (fx_roll_dice(1, 0x14) <= 0x13)
                    done = 1;
                break;
            case 0x19:
                done = 1;
                break;
            }
        }
        p = p->next;
    }
    if (wall_kind(geo.x, geo.y, geo.facing) == 0xf)
        done = 0;
    if (!done)
        door_tries[1] = 0;
    rc = done;
    return rc;
}

char far door_pick(void)                                         /* 354b:0224 */
{
    char rc;
    struct Hero far *p;
    char done;

    done = 0;
    p = node_list;
    while (p && !done) {
        if (fx_roll_dice(1, 100) <= p->pick_locks && p->status == 0)
            done = 1;
        p = p->next;
    }
    door_tries[2] = 0;
    if (wall_kind(geo.x, geo.y, geo.facing) == 0xf)
        rc = 0;
    else {
        if (done)
            move_1211(geo.x, geo.y, geo.facing);
        move_1211(dir_dx[geo.facing] + geo.x, dir_dy[geo.facing] + geo.y,
                  (geo.facing + 4) % 8);
        rc = done;
    }
    return rc;
}

unsigned char far spell_index(char spell, struct Hero far *h)   /* 354b:02f9 */
{
    unsigned char rc;
    unsigned char i;

    for (i = 0; i <= 0x8c && h->spells[i] != spell; i++)
        ;
    if (i <= 0x8c)
        rc = i;
    else
        rc = 0xff;
    return rc;
}

struct Hero far *far spell_caster(char spell)                    /* 354b:033b */
{
    struct Hero far *rc;
    struct Hero far *p;

    for (p = node_list; p && spell_index(spell, p) == 0xff; p = p->next)
        ;
    rc = p;
    return rc;
}

char far door_knock(void)                                        /* 354b:0387 */
{
    char rc;
    struct Hero far *h;

    h = spell_caster(0x1f);
    if (h) {
        h->spells[spell_index(0x1f, h)] |= 0x80;
        if (wall_kind(geo.x, geo.y, geo.facing) != 0xf)
            rc = 1;
        else
            rc = 0;
    } else
        rc = 0;
    return rc;
}

void far move_ahead(void)                                        /* 354b:03e3 */
{
    short nx;
    short ny;
    unsigned short face;

    nx = geo.x;
    ny = geo.y;
    face = geo.facing;
    cursor_obj[0x21] = 0;
    if (wall_kind(nx, ny, face)) {
        nx += dir_dx[face];
        ny += dir_dy[face];
        if (cur_area_rec->cols - 1 < nx) {
            geo.x = cur_area_rec->cols - 1;
            cursor_obj[0x21] = 1;
        }
        if (nx < 0) {
            geo.x = 0;
            cursor_obj[0x21] = 1;
        }
        if (cur_area_rec->rows - 1 < ny) {
            geo.y = cur_area_rec->rows - 1;
            cursor_obj[0x21] = 1;
        }
        if (ny < 0) {
            geo.y = 0;
            cursor_obj[0x21] = 1;
        }
    }
}

void far step_dungeon(void)                                      /* 354b:04a1 */
{
    sfx_play(0xb);
    delay(50);
    geo.x += dir_dx[geo.facing];
    geo.y += dir_dy[geo.facing];
    if (geo.x < 0)
        geo.x = cur_area_rec->cols - 1;
    if (cur_area_rec->cols - 1 < geo.x)
        geo.x = 0;
    if (geo.y < 0)
        geo.y = cur_area_rec->rows - 1;
    if (cur_area_rec->rows - 1 < geo.y)
        geo.y = 0;
    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
    mem_fill(door_tries, 4, 1);
    mem_fill(tavern_tales, 0xc, 0);
    geo.event = area_cell_at(geo.x, geo.y);
    if ((cursor_obj[0x19] & 1) > 0)
        clock_advance(1, 2);
    else
        clock_advance(1, 1);
}

void far step_wild(void)                                         /* 354b:05af */
{
    char nx;
    char ny;

    cursor_hide();
    cursor_obj[0x17] = cursor_obj[0x25];
    cursor_obj[0x18] = cursor_obj[0x26];
    nx = cursor_obj[0x25] + dir_dx[geo.facing];
    ny = cursor_obj[0x26] + dir_dy[geo.facing];
    if (nx < 0)
        nx = 0;
    if (nx > 0x25)
        nx = 0x25;
    if (ny < 0)
        ny = 0;
    if (ny > 0xe)
        ny = 0xe;
    cursor_obj[0x25] = nx;
    cursor_obj[0x26] = ny;
    cursor_show();
    clock_advance(0xc, 3);
}

void far next_square(void)                                         /* 354b:0649 */
{
    next_x = cursor_obj[0x25] + dir_dx[geo.facing];
    next_y = cursor_obj[0x26] + dir_dy[geo.facing];
    if (next_x < 0 || next_x > 0x25 || next_y < 0 || next_y > 0xe) {
        next_x = cursor_obj[0x25];
        next_y = cursor_obj[0x26];
    }
}

void far quest_list(void)                                        /* 354b:0678 */
{
    struct Node far *node;
    struct Node far *list;
    char a;
    char redraw;
    char b;
    short top;
    register int count;
    register int i;

    a = 1;
    b = 2;
    redraw = 1;
    node = 0;
    list = 0;
    top = 0;
    count = 0;
    if (test_mode)
        count = 0x40;
    else
        for (i = 0x44; i <= 0x57; i++)
            if (cursor_obj[i + 1] != 0 && key_item_names[i - 0x44][0] != 0)
                count++;
    if (count != 0) {
        list_alloc(count, &list);
        node = list;
        for (i = 0x44; node != 0; i++) {
            if (test_mode) {
                var_name(node->text, i - 0x44, 1);
                str_format(node->text, "%s - %d", node->text, cursor_obj[i + 1]);
                node->key = 0;
                node = node->next;
            } else if (cursor_obj[i + 1] != 0 && key_item_names[i - 0x44][0] != 0) {
                str_copy(node->text, key_item_names[i - 0x44]);
                node->key = 0;
                node = node->next;
            }
        }
        frame_border();
        hotkey_reset(1);
        node = list;
        list_menu(empty_text, exit_items, 1, 1, 0x26, 0x16, list, a, b,
                  &redraw, &top, &list);
        list_free(&node);
    } else
        bottom_msg("Empty!");
}

char far step_event_in_zone(unsigned char dir, unsigned char n)           /* 354b:082b */
{
    char rc;

    rc = 0;
    if (cur_area_rec->step_events[n].event != 0)
        switch (dir) {
        case 0:
            if (!(cur_area_rec->step_events[n].zones_off & 1))
                rc = 1;
            break;
        case 1:
            if (!(cur_area_rec->step_events[n].zones_off & 2))
                rc = 1;
            break;
        case 2:
            if (!(cur_area_rec->step_events[n].zones_off & 4))
                rc = 1;
            break;
        case 3:
            if (!(cur_area_rec->step_events[n].zones_off & 8))
                rc = 1;
            break;
        case 4:
            if (!(cur_area_rec->step_events[n].zones_off & 0x10))
                rc = 1;
            break;
        case 5:
            if (!(cur_area_rec->step_events[n].zones_off & 0x20))
                rc = 1;
            break;
        case 6:
            if (!(cur_area_rec->step_events[n].zones_off & 0x40))
                rc = 1;
            break;
        case 7:
            if (!(cur_area_rec->step_events[n].zones_off & 0x80))
                rc = 1;
            break;
        }
    return rc;
}

void far step_events_tick(void)                                         /* 354b:094c */
{
    unsigned char cell;
    unsigned char i;

    if (view_mode == 4)
        cell = cell_zone_at(geo.x, geo.y);
    else
        cell = cell_zone_at(cursor_obj[0x25], cursor_obj[0x26]);
    for (i = 0; i <= 7; i++)
        if (step_event_in_zone(cell, i)) {
            le_word_put(le_word_get((unsigned far *)(cursor_obj + 0x1fd) + i) + 1,
                         (unsigned far *)(cursor_obj + 0x1fd) + i);
            if (le_word_get((unsigned far *)(cursor_obj + 0x1fd) + i)
                >= cur_area_rec->step_events[i].steps) {
                le_word_put(0, (unsigned far *)(cursor_obj + 0x1fd) + i);
                events_run(cur_area_rec->step_events[i].event);
            }
        }
}

char far adv_menu(void)                                          /* 354b:0a59 */
{
    char rc;
    char done;
    unsigned char c;
    unsigned char count;
    char view;
    char saved;
    unsigned char k;
    unsigned char y;
    unsigned char x;

    cursor_obj[3] = 0;
    saved = ui_mode;
    switch (view_mode) {
    case 4:
        done = 0;
        c = 0xff;
        do {
            count = 0;
            for (k = 1; k <= 7; k++)
                hotkey_add(k, &count);
            ui_mode = saved;
            c = node_dir_menu(empty_text, explore_items, 1, 0);
            saved = ui_mode;
            exploration_menu_context = 1;
            if (menu_escaped == 0) {
                switch (c) {
                case 0:
                    box_draw();
                    exploration_menu_context = 1;
                    break;
                case 1:
                    if (cursor_obj[0x1a] == 0 ||
                        str_cmp(msg_text(3), debug_word) == 0) {
                        geo.show_map = !geo.show_map;
                        view_draw(geo.x, geo.y, geo.facing);
                    } else
                        status_msg(not_here_text, 0xb, 0);
                    break;
                case 2:
                    if (cur_node->status == 0)
                        cast_menu();
                    break;
                case 3:
                    char_view(&view);
                    break;
                case 4:
                    done = 1;
                    break;
                case 5:
                    cursor_obj[0x19] = cursor_obj[0x19] ^ 1;
                    party_info_display();
                    break;
                case 6:
                    cursor_obj[0x19] = cursor_obj[0x19] | 2;
                    party_info_display();
                    clock_advance(1, 2);
                    word_be66 = word_b8d4;
                    square_event = area_cell_at(geo.x, geo.y);
                    events_run(square_event);
                    done = 1;
                    break;
                case 7:
                    quest_list();
                    view_redraw();
                    break;
                default:
                    party_rows(cur_node, 0);
                    party_key_select(c);
                    party_rows(cur_node, 1);
                    party_info_display();
                }
            } else {
                switch (c) {
                case 0x88:
                    move_ahead();
                    done = 1;
                    party_info_display();
                    c = 0xff;
                    break;
                case 0x84:
                    geo.facing = (geo.facing + 4) % 8;
                    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
                    view_draw(geo.x, geo.y, geo.facing);
                    party_info_display();
                    c = 0xff;
                    break;
                case 0x86:
                    geo.facing = (geo.facing + 6) % 8;
                    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
                    view_draw(geo.x, geo.y, geo.facing);
                    party_info_display();
                    c = 0xff;
                    break;
                case 0x82:
                    geo.facing = (geo.facing + 2) % 8;
                    geo.facing_wall = wall_side(geo.x, geo.y, geo.facing);
                    view_draw(geo.x, geo.y, geo.facing);
                    party_info_display();
                    c = 0xff;
                    break;
                case 0x1b:
                    exploration_menu_context = 0;
                    break;
                default:
                    party_key_select(c);
                    party_display(cur_node);
                }
            }
        } while (done == 0);
        break;
    case 3:
        exploration_menu_context = 1;
        if (cursor_obj[0x24] != 0) {
            done = 0;
            do {
                mem_fill(hotkeys, sizeof hotkeys, 0xff);
                count = 0;
                mem_fill(hotkeys, sizeof hotkeys, 0xff);
                hotkey_add(4, &count);
                ui_mode = saved;
                x = cursor_obj[0x25];
                y = cursor_obj[0x26];
                c = grid_small(empty_text, explore_items, 1, 0, y, x);
                saved = ui_mode;
                if (menu_escaped == 0) {
                    if (c == 4)
                        done = 1;
                } else {
                    done = 1;
                    switch (c) {
                    case 0x88: geo.facing = 0; break;
                    case 0x81: geo.facing = 1; break;
                    case 0x82: geo.facing = 2; break;
                    case 0x83: geo.facing = 3; break;
                    case 0x84: geo.facing = 4; break;
                    case 0x85: geo.facing = 5; break;
                    case 0x86: geo.facing = 6; break;
                    case 0x87: geo.facing = 7; break;
                    default: done = 0;
                    }
                }
            } while (done == 0);
        }
        break;
    }
    if (event_text_cleared == 0) {
        text_clear(1, 0x11, 0x26, 0x16);
        event_text_cleared = 1;
    }
    rc = c;
    return rc;
}

void far secret_door(void)                                       /* 354b:0e5f */
{
    combat_text_clear();
    event_print("A secret door!");
    msg_pause();
}

void far move_resolve(void)                                      /* 354b:0e76 */
{
    unsigned char kind;
    unsigned char key;
    unsigned char count;
    unsigned char cell;
    char has_text;

    cursor_obj[0x17] = geo.x;
    cursor_obj[0x18] = geo.y;
    turn_done = 0;
    switch (view_mode) {
    case 4:
        if (cursor_obj[3] < 0xff) {
            redrawn = 1;
            kind = wall_kind(geo.x, geo.y, geo.facing);
            switch (kind) {
            case 1:
                secret_door();
            case 0:
                turn_done = 1;
                break;
            case 3:
                secret_door();
            case 2:
            case 0xf:
                count = 0;
                if (door_tries[1])
                    hotkey_add(0, &count);
                if (door_tries[2] && party_has_class(6))
                    hotkey_add(1, &count);
                if (spell_caster(0x1f))
                    hotkey_add(2, &count);
                hotkey_add(3, &count);
                if (count == 0)
                    move_120c(geo.x, geo.y, geo.facing);
                else {
                    key = menu_run("Blocked:", door_items, 0, 0);
                    switch (key) {
                    case 0:
                        turn_done = door_bash();
                        break;
                    case 1:
                        turn_done = door_pick();
                        break;
                    case 2:
                        turn_done = door_knock();
                        break;
                    }
                }
                break;
            case 5:
                secret_door();
            case 4:
                count = 0;
                if (door_tries[1])
                    hotkey_add(0, &count);
                if (door_tries[2] && party_has_class(6))
                    hotkey_add(1, &count);
                if (spell_caster(0x1f))
                    hotkey_add(2, &count);
                hotkey_add(3, &count);
                if (count == 0)
                    move_120c(geo.x, geo.y, geo.facing);
                else {
                    key = menu_run("Blocked:", door_items, 0, 0);
                    switch (key) {
                    case 0:
                        turn_done = door_bash();
                        break;
                    case 1:
                        door_tries[2] = 0;
                        turn_done = 0;
                        break;
                    case 2:
                        turn_done = door_knock();
                        break;
                    }
                }
                break;
            case 0xe:
                turn_done = 0;
                break;
            case 6: case 7: case 8: case 9: case 0xa: case 0xb: case 0xc: case 0xd:
                turn_done = cursor_obj[kind + 0x3f];
                break;
            }
            if (turn_done) {
                step_dungeon();
                step_events_tick();
                square_event = area_cell_at(geo.x, geo.y);
                events_run(square_event);
            }
            if (!adventure_exit)
                party_info_display();
        } else
            cursor_obj[3] = 0;
        break;
    case 3:
        next_square();
        cell = cell_zone_at(next_x, next_y);
        bank_text_get(text_pool_block, cur_area_rec->zone_texts_head[cell], msg_buf);
        has_text = 0;
        if (msg_buf[0])
            has_text = 1;
        if (!wall_kind(cursor_obj[0x25], cursor_obj[0x26], geo.facing)) {
            step_wild();
            square_event = area_cell_at(cursor_obj[0x25], cursor_obj[0x26]);
            if (has_text && !square_event) {
                combat_text_clear();
                event_print(msg_buf);
            }
            step_events_tick();
            events_run(square_event);
            if (!adventure_exit)
                party_info_display();
        } else {
            if (has_text) {
                combat_text_clear();
                event_print(msg_buf);
            }
            cursor_obj[3] = 0xff;
        }
        break;
    default:
        cursor_obj[3] = 0;
    }
    pic_release();
}

static void far move_120c(char x, char y, char dir)              /* 354b:120c */
{
}

static void far move_1211(char x, char y, char dir)              /* 354b:1211 */
{
}

static char far party_has_class(int cls)                               /* 354b:1216 */
{
    char rc;
    struct Hero far *p;
    char found;

    found = 0;
    p = node_list;
    while (p && !found) {
        if (p->level[cls] > 0 || p->old_level[cls] > 0 && old_class_abilities_available(p))
            found = 1;
        p = p->next;
    }
    rc = found;
    return rc;
}
