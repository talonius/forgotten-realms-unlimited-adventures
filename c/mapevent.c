/* CKIT.EXE overlay 04 module 1cff: running the map's events.  The event
 * loop at the party's square, events_run, tests an event's conditions,
 * marks it done in the level's bit map and runs its kind -- text, a
 * picture, a tavern, a shop, a transfer to another level or place, a
 * question, a quest-flag change -- most of them through vector stubs into
 * sections 8, 9, 10, 13 and 19; then it follows the event's chain, queued
 * follow-ups in event_stack and a step back.  Also the adventure menu loop
 * (game_loop), the secret-door search, the tavern and its drinks, and
 * helpers that pack and unpack little-endian words and longs.
 *
 * EXE 0x02e2cf..0x031279, one Turbo C large-model _TEXT segment starting at
 * 1cff:000f in RTLink section 4 (payload 0x2db30, link segment 1c86, parent
 * section 3), after module 1c86's final retf at 1cff:000e.  Its own final
 * retf is 1cff:2fb8, byte 8 of paragraph 1ffa, where the next module begins
 * at 1ffa:0009.  Section 4's table files all 241 fixup sites in the range
 * under frame 1cff.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions
 * are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 4.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  Code outside section 4
 * reaches 17 routines only through vector stubs: 18de:13e1 -> 08f0, 13eb
 * -> 0b1c, 1409 -> 03c7, 141d -> 08e5, 14bd -> 024e, 14c7 -> 0670, 14d1 ->
 * 042b, 14db -> 0302, 14e5 -> 048c, 14ef -> 0473, 14f9 -> 04e9, 1503 ->
 * 04c8, 150d -> 0280, 1517 -> 08f4, 1521 -> 0387, 152b -> 29dd, 1535 ->
 * 24c1.  The module's own far calls (060a, 042b, 0387, 29dd) are calls
 * ahead of the definition.
 *
 * FRUA's own code: the sharing map pairs no routine of it exact or shape
 * with Dark Queen of Krynn's.
 *
 * _DATA is DGROUP 5b3e..5d42, after module 1c86's 5ae6..5b3d: five zeroed
 * variables and the literals, 14 distinct stored as 13 -- -d stores "" on
 * the terminator of the north door text (5b89).  5d43 is zero, and
 * 5d44..5d53 hold 0..15 in a shuffled order that nothing in the image
 * reads, before module 1ffa's _DATA at 5d54; no module is known to own it.  _BSS is b254..b378,
 * after module 1bfb's records (b252) and before module 1ffa's b37a; 14
 * variables in the order their names give (Turbo C's symbol-table order),
 * chosen with goldbox.bss_order.  Nothing in the image reads tmp_b258,
 * which holds the layout.  The public ones are read or written in other
 * sections: rerun_square, tavern_tales, last_pic, transfer_dest_x, transfer_dest_y, msg_buf,
 * pic_up, monster_table, step_back_due and party_has_moved by section 8 (monster_table as
 * two six-byte rows indexed 8..13 from b361 and b367, 0x41fe5..0x42750),
 * msg_buf also by section 25 (0x7ec9d), pending_load by section 6; the bytes
 * do not decide static or public for the rest, nor for the routines no stub
 * or far call reaches.
 *
 * THIS IS THE v1.0 BUILD (CKIT.EXE "Version 1.0 March 19,1993"): the
 * addresses above are v1.2's; v1.0 places the module at file 0x02e303
 * (2d03:0003, _DATA 5b2a, 487 bytes, _BSS b1f6), 12,158 code bytes.
 * Against v1.2: every event move clamps its square with
 * int_min(v, cols) and (v, rows), not cols - 1 and rows - 1;
 * event_outcome keeps ev[0xa] & 0x20, not & 0x40, in rerun_square;
 * event_transfer's test-mode transfer stops without the "Transfer module
 * ends testing!" message (so v1.0 has no status_msg call and no such
 * literal), and only a kind 0xb transfer can land on one of the area's
 * entry spots -- v1.2's other branch for ev[0xc] & 1 is absent, so every
 * other kind goes to ev[9], ev[8].  Section 6 sets pending_load through the
 * vector stub to pending_load_set instead of storing it.
 */

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28                                    */
};

struct Hero {
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x4c];           /* +c   not read here                     */
    unsigned char race;         /* +58                                    */
    unsigned char cls;          /* +59                                    */
    char pad_5a[4];             /* +5a  not read here                     */
    unsigned char status;       /* +5e                                    */
    char pad_5f[0x12e];         /* +5f  not read here                     */
    unsigned char mon_id;       /* +18d                                   */
};

struct Spot {                   /* a place in an area                     */
    unsigned char y;            /* +0                                     */
    unsigned char x;            /* +1                                     */
    unsigned char facing;       /* +2                                     */
    unsigned char which;        /* +3   not read here                     */
};

struct Encounter {
    unsigned ticks;             /* +0   not read here                     */
    unsigned char event;        /* +2   not read here                     */
    unsigned char chance;       /* +3                                     */
};

struct Area {                   /* what cur_area_rec points at                */
    char pad_0[2];              /* +0   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[0x0a];           /* +4   not read here                     */
    struct Spot spots[8];       /* +e                                     */
    struct Encounter rest_enc_head[1]; /* +2e  no bound is read                  */
};

struct Geo {
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
};

struct Event {                  /* one event_table record, 0x14 bytes        */
    unsigned char bytes[0x14];
};

struct Effect;

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far bank_text_get(char far *pool, int slot, char far *out); /* 03ad:0141 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far item_remove(struct Hero far *h, struct Item far *it); /* 03d0:13c4 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern void far hero_drop(char keep, char icon);                 /* 03d0:1e69 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far msg_pause(void);                                 /* 06c9:0953 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far press_return_clear(void);                        /* 083a:053a */
extern void far press_return_wait(char anim);                            /* 083a:054a */
extern char far node_prompt(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1a07 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far txt_get(char far *pool, int slot, char far *out); /* 0cf0:046e */
extern void far frame_main(void);                                /* 0d65:00de */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far clock_advance(unsigned char n, short unit);
                                        /* 18de:11bb, vector to overlay 19 2d91:02bf */
extern unsigned char far cell_zone_at(char x, char y);
                                        /* 18de:1355, vector to overlay 13 26e3:0b87 */
extern void far view_draw(char x, char y, unsigned char dir);
                                        /* 18de:1495, vector to overlay 13 26e3:0bdb */
extern void far event_print(char far *text);
                                        /* 18de:153f, vector to overlay 8 21d4:09a8 */
extern void far view_update(void);     /* 18de:1549, vector to overlay 8 21d4:074e */
extern void far area_enter(char n);   /* 18de:1553, vector to overlay 8 21d4:0a33 */
extern void far event_init(void);     /* 18de:155d, vector to overlay 8 21d4:0001 */
extern char far wall_kind(char x, char y, char dir);
                                        /* 18de:1567, vector to overlay 13 26e3:080d */
extern void far camp_menu(char far *done);
                                        /* 18de:1571, vector to overlay 9 21d4:2ab9 */
extern void far monster_show(char a, char b, unsigned char c, unsigned char far *p);
                                        /* 18de:157b, vector to overlay 8 21d4:07b4 */
extern void far monster_approach(void);     /* 18de:1585, vector to overlay 8 21d4:0959 */
extern unsigned char far monster_dist(char x, char y, unsigned char dir);
                                        /* 18de:158f, vector to overlay 8 21d4:133a */
extern void far move_resolve(void);     /* 18de:1599, vector to overlay 8 254b:0e76 */
extern char far adv_menu(void);     /* 18de:15a3, vector to overlay 8 254b:0a59 */
extern char far area_cell_at(char x, char y);
                                        /* 18de:15ad, vector to overlay 13 26e3:0ae7 */
extern char far menu_horiz(char far *title, char far *text, char a, char b);
                                        /* 18de:15b7, vector to overlay 8 21d4:0208 */
extern void far shop_menu(void);     /* 18de:15c1, vector to overlay 10 2431:05b4 */
extern char far treasure_mask_add(unsigned char far *mask);
                                        /* 18de:15cb, vector to overlay 10 2431:1141 */
extern void far view_3d(void);     /* 18de:15d5, vector to overlay 8 21d4:0a1e */
extern void far view_map(void);     /* 18de:15df, vector to overlay 8 21d4:0a09 */
extern void far geo_load(unsigned char pic);
                                        /* 18de:15e9, vector to overlay 13 26e3:1750 */
extern void far event_vault(unsigned char far *ev);
                                        /* 18de:15f3, vector to overlay 8 21d4:35df */
extern void far event_inn(unsigned char far *ev, int n);
                                        /* 18de:15fd, vector to overlay 8 21d4:353b */
extern void far event_train(unsigned char far *ev, int n);
                                        /* 18de:1607, vector to overlay 8 21d4:29b7 */
extern char far event_temple(unsigned char far *ev);
                                        /* 18de:1611, vector to overlay 8 21d4:1e60 */
extern void far event_sounds(unsigned char far *ev);
                                        /* 18de:161b, vector to overlay 8 21d4:3662 */
extern char far event_encounter(unsigned char far *ev);
                                        /* 18de:1625, vector to overlay 8 21d4:186b */
extern void far event_npc(unsigned char far *ev);
                                        /* 18de:162f, vector to overlay 8 21d4:33b8 */
extern void far event_walk(unsigned char far *ev);
                                        /* 18de:1639, vector to overlay 8 21d4:2ab8 */
extern void far event_damage(unsigned char far *ev);
                                        /* 18de:1643, vector to overlay 8 21d4:1c3e */
extern void far event_treasure(unsigned char far *ev, char c);
                                        /* 18de:164d, vector to overlay 8 21d4:2550 */
extern void far event_combat(unsigned char far *ev, char c);
                                        /* 18de:1657, vector to overlay 8 21d4:1444 */
extern char far event_question(unsigned char far *ev);
                                        /* 18de:1661, vector to overlay 8 21d4:2d26 */
extern void far event_who(unsigned char far *ev);
                                        /* 18de:166b, vector to overlay 8 21d4:3469 */
extern void far event_xp(unsigned char far *ev);
                                        /* 18de:1675, vector to overlay 8 21d4:27b5 */
extern char far event_menu(unsigned char far *ev);
                                        /* 18de:167f, vector to overlay 8 21d4:36a4 */
extern char far event_password(unsigned char far *ev);
                                        /* 18de:1689, vector to overlay 8 21d4:2665 */
extern char far event_pay(unsigned char far *ev);
                                        /* 18de:1693, vector to overlay 8 21d4:321a */
extern char far event_check(unsigned char far *ev);
                                        /* 18de:169d, vector to overlay 8 21d4:2ef9 */
extern void far big_open(unsigned char id);                      /* overlay 3 1bbc:0334 */
extern void far big_redraw(void);                                /* overlay 3 1bbc:03a5 */
extern void far bigpic_release(void);                              /* overlay 3 1bbc:03e0 */
extern void far pool_clear(void);                                /* overlay 3 1bfb:0002 */
extern void far view_refresh(void);                              /* 1c86:0000 */
extern void far party_rows(struct Hero far *hl, char hilite);   /* 1c86:0064 */
extern void far party_display(struct Hero far *hl);                /* 1c86:023a */
extern void far party_info_display(void);                               /* 1c86:0406 */
extern void far party_key_select(unsigned char key);                  /* 1c86:06b6 */
extern void far game_load_menu(void);                             /* 1ffa:15c7 */

extern char far *empty_text;                                     /* 3bc0 */
extern char far *tavern_items;                                       /* 3cdb */
extern char far *town_items;                                       /* 3cfb */
extern int cur_level;                                            /* 6719 */
extern char view_restore_due;                                           /* 671b */
extern char in_service_event;                                           /* 671c */
extern char chain_allowed;                                           /* 671d */
extern char byte_6723;                                           /* 6723 */
extern char big_pic_shown;                                           /* 6724 */
extern char quit_requested;                                           /* 6725 */
extern char byte_6726;                                           /* 6726 */
extern char events_running;                                           /* 6727 */
extern unsigned char encounter_range;                                  /* 73e3 */
extern unsigned char far *cur_event;                              /* a7e4 */
extern char far *text_pool_block;                                /* a7ec */
extern struct Event far *event_table;                               /* a7f0 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern char redrawn;                                             /* b3f3 */
extern char turn_done;                                           /* b414 */
extern unsigned char text_col;                                   /* b419 */
extern unsigned char text_row;                                   /* b41a */
extern int view_mode;                                            /* b421 */
extern char esc_check;                                           /* b423 */
extern char game_saved;                                           /* b425 */
extern char byte_b429;                                           /* b429 */
extern char confirm_anim;                                        /* b42a */
extern char text_slow;                                           /* b431 */
extern int saved_view_mode;                                            /* b433 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char exploration_menu_context;                                           /* b4e8 */
extern char event_pic;                                           /* b727 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char picture_hides_roster;                                           /* b761 */
extern struct Item far *pool_items;                              /* b8e2 */
extern char adventure_exit;                                           /* bcf6 */
extern char saved_search_flag;                                           /* c3a2 */
extern char event_sprite;                                           /* c671 */
extern char menu_escaped;                                        /* ca2a */
extern char test_mode;                                           /* cc7a */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern unsigned char encounter_images_shown[2];                                /* d139 */
extern struct Geo geo;                                           /* d414 */
extern struct Area far *cur_area_rec;                                /* d41c */


void far event_view_draw(void);
void far party_pos_set(void);
void far event_done_clear(int n);
void far events_run(unsigned char n);

static char skip_redraw = 0;                                       /* 5b3e */
char rerun_square = 0;                                              /* 5b3f */
static unsigned char event_stack[8] = { 0 };                        /* 5b40 */
int tavern_tales[6] = { 0 };                                         /* 5b48 */
char last_pic = 0;                                              /* 5b54 */

static char moved_last_pass;                                            /* b254 */
unsigned char transfer_dest_x;                                          /* b255 */
unsigned char transfer_dest_y;                                          /* b256 */
static char transfer_new_level;                                            /* b257 */
static char tmp_b258;                                            /* b258 */
char msg_buf[0x10a];                                            /* b259 */
char pic_up;                                                  /* b363 */
static struct Hero far *saved_node;                               /* b364 */
char pending_load;                                                  /* b368 */
char monster_table[12];                                              /* b369 */
char step_back_due;                                                  /* b375 */
static char chain_next_due;                                          /* b376 */
static char node_restoring;                                          /* b377 */
char party_has_moved;                                                 /* b378 */

char far event_stack_push(unsigned char c)                          /* 2cff:000f */
{
    char found;
    unsigned char i;

    found = 0;
    if (c != 0 && event_stack[7] == 0)
        for (i = 0; i <= 7; i++)
            if (event_stack[i] == 0) {
                event_stack[i] = c;
                event_done_clear(c);
                i = 7;
                found = 1;
            }
    return found;
}

char far event_stack_pop(void)                                     /* 2cff:006c */
{
    char n;
    char i;

    n = 0;
    for (i = 7; i > -1; i--)
        if (event_stack[i] != 0) {
            n = event_stack[i];
            event_stack[i] = 0;
            i = -1;
        }
    return n;
}

char far event_outcome(unsigned char far *ev, char alt)          /* 2cff:00b5 */
{
    char r;

    r = 0;
    if (alt) {
        combat_text_clear();
        if (ev[0xd] != 0) {
            bank_text_get(text_pool_block, ev[0xd] + (ev[0xe] << 8), msg_buf);
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
            press_return_wait(1);
        }
        switch ((ev[0xa] & 0xc) >> 2) {
        case 1:
            r = ev[0x10];
            break;
        case 2:
            transfer_dest_x = int_min(ev[0x11], cur_area_rec->cols);
            transfer_dest_y = int_min(ev[0x12], cur_area_rec->rows);
            geo.facing = (ev[0xa] & 0x30) >> 3;
            party_pos_set();
            event_view_draw();
            party_has_moved = 1;
            break;
        case 3:
            step_back_due = 1;
        }
    } else {
        combat_text_clear();
        if (ev[0xb] != 0) {
            bank_text_get(text_pool_block, ev[0xb] + (ev[0xc] << 8), msg_buf);
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
            press_return_wait(1);
        }
        switch (ev[0xa] & 3) {
        case 1:
            r = ev[0xf];
            break;
        case 2:
            transfer_dest_x = int_min(ev[0x11], cur_area_rec->cols);
            transfer_dest_y = int_min(ev[0x12], cur_area_rec->rows);
            geo.facing = (ev[0xa] & 0x30) >> 3;
            party_pos_set();
            event_view_draw();
            party_has_moved = 1;
            break;
        case 3:
            step_back_due = 1;
        }
    }
    rerun_square = ev[0xa] & 0x20;
    return r;
}

void far event_view_restore(void)                                     /* 2cff:024e */
{
    if (view_mode == 3) {
        frame_lower_panel();
        view_redraw();
    } else {
        picture_hides_roster = 0;
        frame_main();
        view_update();
        party_display(cur_node);
    }
}

char far event_text_show(unsigned char far *ev)                    /* 2cff:0280 */
{
    char shown;

    shown = 0;
    combat_text_clear();
    if (*(int far *)(ev + 4) != 0 && ev[0] != 0x16) {
        shown = 1;
        bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
        cursor_obj[0x39] = 0;
        event_print(msg_buf);
        if (pic_up) {
            press_return_wait(1);
            pic_up = 0;
        } else
            press_return_wait(1);
        combat_text_clear();
        box_draw();
    }
    return shown;
}

char far party_has_npc(unsigned char kind)                       /* 2cff:0302 */
{
    char found;
    struct Hero far *h;

    found = 0;
    for (h = node_list; h != 0 && !found; h = h->next) {
        if (kind == 0) {
            if (h->mon_id < 0x80 && h->mon_id >= 0x70)
                if (h->status == 0 || h->status == 1 || h->status == 3 || h->status == 5)
                    found = 1;
        } else if (h->mon_id == kind)
            if (h->status == 0 || h->status == 1 || h->status == 3 || h->status == 5)
                found = 1;
    }
    return found;
}

void far event_view_draw(void)                                     /* 2cff:0387 */
{
    if (!skip_redraw && view_mode != 3 && (view_mode != 2 || saved_view_mode != 3)) {
        view_draw(geo.x, geo.y, geo.facing);
        party_info_display();
        text_pause();
    }
    skip_redraw = 0;
}

void far prev_pos_save(void)                                     /* 2cff:03c7 */
{
    if (view_mode == 4) {
        cursor_obj[0x17] = geo.x;
        cursor_obj[0x18] = geo.y;
    } else {
        cursor_obj[0x17] = cursor_obj[0x25];
        cursor_obj[0x18] = cursor_obj[0x26];
    }
}

void far prev_pos_restore(void)                                     /* 2cff:03f7 */
{
    if (view_mode == 4) {
        geo.x = cursor_obj[0x17];
        geo.y = cursor_obj[0x18];
    } else {
        cursor_obj[0x25] = cursor_obj[0x17];
        cursor_obj[0x26] = cursor_obj[0x18];
    }
    prev_pos_save();
}

void far party_pos_set(void)                                     /* 2cff:042b */
{
    prev_pos_save();
    if (view_mode == 4) {
        geo.x = transfer_dest_x;
        geo.y = transfer_dest_y;
    } else if (view_mode == 3) {
        cursor_obj[0x25] = transfer_dest_x;
        cursor_obj[0x26] = transfer_dest_y;
    } else if (view_mode == 2) {
        if (saved_view_mode == 4) {
            geo.x = transfer_dest_x;
            geo.y = transfer_dest_y;
        } else if (saved_view_mode == 3) {
            cursor_obj[0x25] = transfer_dest_x;
            cursor_obj[0x26] = transfer_dest_y;
        }
    }
}

void far le_word_put(unsigned n, unsigned char far *p)         /* 2cff:0473 */
{
    p[0] = n;
    p[1] = n >> 8;
}

void far le_long_put(long n, unsigned char far *p)             /* 2cff:048c */
{
    p[0] = n;
    p[1] = n >> 8;
    p[2] = n >> 16;
    p[3] = n >> 24;
}

unsigned far le_word_get(unsigned char far *p)                 /* 2cff:04c8 */
{
    unsigned short n;

    n = p[1] << 8 | p[0];
    return n;
}

long far le_long_get(unsigned char far *p)                     /* 2cff:04e9 */
{
    long n;

    n = (long)p[1] << 8 | p[0] | (long)p[2] << 16 | (long)p[3] << 24;
    return n;
}

char far event_done_test(int n)                                    /* 2cff:0552 */
{
    unsigned char mask;
    char set;

    n = n - 1;
    n = cur_level * 100 + n;
    mask = 1;
    mask = mask << n % 8;
    if (cursor_obj[0x20d + n / 8] & mask)
        set = 1;
    else
        set = 0;
    return set;
}

void far event_done_set(int n)                                    /* 2cff:05ac */
{
    unsigned char mask;

    n = n - 1;
    n = cur_level * 100 + n;
    mask = 1;
    mask = mask << n % 8;
    cursor_obj[0x20d + n / 8] = cursor_obj[0x20d + n / 8] | mask;
}

void far event_done_clear(int n)                                    /* 2cff:060a */
{
    unsigned char mask;

    n = n - 1;
    n = cur_level * 100 + n;
    mask = 1;
    mask = mask << n % 8;
    mask = ~mask;
    cursor_obj[0x20d + n / 8] = cursor_obj[0x20d + n / 8] & mask;
}

void far event_pic_show(unsigned char far *ev)                    /* 2cff:0670 */
{
    char same;

    same = 0;
    byte_6726 = 0;
    if (ev[6] != 0) {
        if (ev[6] >= 0xf0) {
            if (ev[6] != last_pic) {
                mem_fill(encounter_images_shown, 2, 0);
                picture_hides_roster = 1;
                encounter_images_shown[1] = 1;
                confirm_anim = 1;
                big_open(ev[6]);
                big_redraw();
                picture_hides_roster = 1;
                redrawn = 0;
                bigpic_release();
                big_pic_shown = 1;
                view_restore_due = 1;
                last_pic = ev[6];
            } else
                same = 1;
        } else {
            picture_hides_roster = 0;
            if (ev[7] & 0x80) {
                if (ev[6] != last_pic) {
                    mem_fill(encounter_images_shown, 2, 0);
                    event_sprite = 0xff;
                    event_pic = ev[6];
                    if (event_pic != 0) {
                        last_pic = ev[6];
                        byte_6726 = 1;
                    }
                } else
                    same = 1;
            } else if (view_mode != 3) {
                mem_fill(encounter_images_shown, 2, 0);
                last_pic = 0;
                event_pic = 0xff;
                event_sprite = ev[6];
            } else {
                mem_fill(encounter_images_shown, 2, 0);
                last_pic = 0;
                event_pic = 0xff;
                event_sprite = 0xff;
            }
            if (ev[0] == 1 || ev[0] == 0xf || ev[0] == 0x21) {
                if (ev[0] == 0xf) {
                    if (encounter_range != 0xff)
                        cursor_obj[0x38] = encounter_range - 1;
                } else
                    cursor_obj[0x38] = (ev[0xe] & 0x60) >> 5;
            } else
                cursor_obj[0x38] = ev[7] & 3;
            cursor_obj[0x37] = monster_dist(geo.x, geo.y, geo.facing);
            if (cursor_obj[0x38] < cursor_obj[0x37])
                cursor_obj[0x37] = cursor_obj[0x38];
            if (ev[0] == 1 && (ev[0xc] & 0x20) && view_mode != 3 && !same) {
                view_draw(geo.x, geo.y, geo.facing);
                party_info_display();
                monster_show(event_sprite, event_pic, cursor_obj[0x37], encounter_images_shown);
                monster_approach();
            } else if (!same) {
                monster_show(event_sprite, event_pic, cursor_obj[0x37], encounter_images_shown);
                party_display(cur_node);
                screen_flush();
            }
            big_pic_shown = 0;
        }
    }
    if (event_pic != -1)
        pic_up = 1;
    byte_6726 = 0;
}

void far pending_load_set(char c)                                   /* 2cff:08e5 */
{
    pending_load = c;
}

char far pending_load_get(void)                                     /* 2cff:08f0 */
{
    return pending_load;
}

void far enter_camp(char check)                               /* 2cff:08f4 */
{
    unsigned char cell;
    char done;

    if (saved_view_mode == 3)
        cell = cell_zone_at(cursor_obj[0x25], cursor_obj[0x26]);
    else
        cell = cell_zone_at(geo.x, geo.y);
    if (check && (cur_area_rec->rest_enc_head[cell].chance & 0x80))
        cursor_obj[0x2c] = 100;
    else
        cursor_obj[0x2c] = 0;
    done = 0;
    camp_menu(&done);
    if (done && !in_service_event)
        view_redraw();
    redrawn = 1;
    view_refresh();
    game_saved = 0;
}

void far secret_doors_search(void)                                     /* 2cff:0992 */
{
    cursor_obj[0x39] = 0;
    if ((cursor_obj[0x19] & 1) && view_mode == 4) {
        if (wall_kind(geo.x, geo.y, 0) == 1 || wall_kind(geo.x, geo.y, 0) == 3
            || wall_kind(geo.x, geo.y, 0) == 5) {
            combat_text_clear();
            event_print("The party has discovered a secret door to the north.");
            msg_pause();
        }
        if (wall_kind(geo.x, geo.y, 2) == 1 || wall_kind(geo.x, geo.y, 2) == 3
            || wall_kind(geo.x, geo.y, 2) == 5) {
            combat_text_clear();
            event_print("The party has discovered a secret door to the east.");
            msg_pause();
        }
        if (wall_kind(geo.x, geo.y, 4) == 1 || wall_kind(geo.x, geo.y, 4) == 3
            || wall_kind(geo.x, geo.y, 4) == 5) {
            combat_text_clear();
            event_print("The party has discovered a secret door to the south.");
            msg_pause();
        }
        if (wall_kind(geo.x, geo.y, 6) == 1 || wall_kind(geo.x, geo.y, 6) == 3
            || wall_kind(geo.x, geo.y, 6) == 5) {
            combat_text_clear();
            event_print("The party has discovered a secret door to the west.");
            msg_pause();
        }
    }
}

void far game_loop(void)                                     /* 2cff:0b1c */
{
    char choice;
    struct Hero far *h;
    char mode;
    char cell;
    char quit;

    quit = 0;
    do {
        saved_node = cur_node;
        redrawn = 1;
        node_restoring = 0;
        transfer_new_level = 0;
        pending_load_set(0);
        if (cursor_obj[0x16] == 0) {
            if (esc_check)
                byte_b429 = 7;
            else {
                byte_b429 = 4;
                party_display(cur_node);
            }
        } else
            byte_b429 = cursor_obj[0x16];
        if (cursor_obj[0x22] == 0)
            view_mode = 3;
        area_enter(byte_b429);
        event_init();
        area_enter(byte_b429);
        if (esc_check) {
            h = node_list;
            while (h != 0) {
                cur_node = h;
                h = h->next;
                hero_drop(1, 1);
            }
            return;
        }
        if (view_mode != 3 && cursor_obj[0x86] != 0) {
            if (cur_level >= 5)
                view_mode = 4;
            if (cursor_obj[0x85] == 0) {
                view_redraw();
                redrawn = 1;
                view_refresh();
            }
        }
        if (cur_level >= 5 && cursor_obj[0x86] == 0) {
            view_mode = 4;
            view_redraw();
        }
        redrawn = 1;
        if (cursor_obj[0x85] == 0)
            view_refresh();
        exploration_menu_context = 0;
        mode = 1;
        if (cursor_obj[0x85] == 0)
            view_refresh();
        if (cursor_obj[0x86] == 0) {
            cell = area_cell_at(geo.x, geo.y);
            events_run(cell);
            cursor_obj[0x86] = 1;
            if (!adventure_exit) {
                party_info_display();
                view_refresh();
            }
        } else if (cursor_obj[0x85] != 0)
            events_run(cursor_obj[0x85]);
        else if (cursor_obj[0x31] != 0)
            events_run(cursor_obj[0x31]);
        while (!adventure_exit) {
            ui_mode = mode;
            choice = adv_menu();
            mode = ui_mode;
            saved_node = cur_node;
            if (!transfer_new_level)
                cursor_obj[0x16] = byte_b429;
            while ((choice == 6 || choice == 4) && !adventure_exit) {
                if (choice == 4) {
                    enter_camp(1);
                    choice = -1;
                    quit = 1;
                } else if (choice == 6) {
                    saved_search_flag = cursor_obj[0x19] & 1;
                    cursor_obj[0x19] = 1;
                    redrawn = 1;
                    view_refresh();
                    secret_doors_search();
                    cursor_obj[0x19] = saved_search_flag;
                    quit = 1;
                    choice = -1;
                } else
                    quit = 0;
            }
            if (!quit) {
                if (view_mode == 4)
                    combat_text_clear();
                move_resolve();
                view_refresh();
                secret_doors_search();
                confirm_anim = 0;
                transfer_new_level = 0;
            } else
                quit = 0;
        }
    } while (!adventure_exit);
    adventure_exit = 0;
    h = node_list;
    while (h != 0) {
        cur_node = h;
        h = h->next;
        hero_drop(0, 1);
    }
    if (pending_load_get())
        game_load_menu();
}

void far event_text(unsigned char far *ev)                    /* 2cff:0e21 */
{
    unsigned char bit;
    char color;
    int i;

    combat_text_clear();
    event_view_draw();
    if (ev[6] != 0)
        event_pic_show(ev);
    bit = 1;
    text_row = 0x11;
    text_col = 1;
    if (ev[0x12] != 0)
        sfx_play(ev[0x12]);
    for (i = 0; i <= 4; i++) {
        if (((int far *)(ev + 8))[i] != 0) {
            bank_text_get(text_pool_block, ((int far *)(ev + 8))[i], msg_buf);
            if ((bit << 2) & ev[7])
                color = 3;
            else
                color = 7;
            text_slow = 1;
            if (ev[4] & bit) {
                text_wrap(1, 0x11, 0x26, 0x16, color, 0, 0, msg_buf);
                press_return_wait(0);
                text_row = 0x11;
                text_col = 1;
                combat_text_clear();
                box_draw();
            } else
                text_wrap(1, 0x11, 0x26, 0x16, color, 0, 0, msg_buf);
            text_slow = 0;
        }
        bit = bit << 1;
    }
    mem_fill(encounter_images_shown, 2, 0);
    if (view_mode == 4 && ev[6] >= 0xf0) {
        view_redraw();
        view_refresh();
        party_display(cur_node);
        party_info_display();
    }
    cursor_obj[0x39] = 0;
    if (ev[4] & 0x20)
        step_back_due = 1;
}

unsigned char far tavern_drink(unsigned char far *ev)           /* 2cff:0fae */
{
    unsigned char price;
    unsigned char pick;

    price = 0;
    combat_text_clear();
    cursor_obj[0x39] = 1;
    event_print("\"what's your pleasure?\"");
    txt_get(text_pool_block, ev[0x10] + (ev[0x11] << 8) - 1, msg_buf);
    pick = menu_horiz("", msg_buf, 0, 1);
    switch (pick) {
    case 0:
        price = 0;
        break;
    case 1:
        price = 6;
        break;
    case 2:
        price = 10;
        break;
    case 3:
        price = 15;
        break;
    case 4:
        price = 30;
    }
    return price;
}

char far event_tavern(unsigned char far *ev)                    /* 2cff:1059 */
{
    unsigned char key;
    char r;
    unsigned char i;
    unsigned char count;
    unsigned char n;
    unsigned char total;
    int texts[10];

    r = 0;
    total = 0;
    if (ev[6] != 0)
        event_pic_show(ev);
    if (!event_text_show(ev) || ev[0] == 0x16) {
        combat_text_clear();
        cursor_obj[0x39] = 0;
        event_print("The party enters a tavern.");
        if (pic_up) {
            press_return_wait(1);
            pic_up = 0;
        }
        combat_text_clear();
    }
    n = 0;
    for (i = 0; i <= 9; i++)
        switch (i) {
        case 0:
            texts[n] = ev[8] + (ev[9] << 8);
            if (texts[n] != 0)
                n++;
            break;
        case 1:
            texts[n] = ev[0xa] + (ev[0xb] << 8);
            if (texts[n] != 0)
                n++;
            break;
        case 2:
            texts[n] = ev[0xc] + (ev[0xd] << 8);
            if (texts[n] != 0)
                n++;
            break;
        case 3:
            texts[n] = ev[0xe] + (ev[0xf] << 8);
            if (texts[n] != 0)
                n++;
            break;
        case 4:
            texts[n] = tavern_tales[0];
            if (texts[n] != 0)
                n++;
            break;
        case 5:
            texts[n] = tavern_tales[1];
            if (texts[n] != 0)
                n++;
            break;
        case 6:
            texts[n] = tavern_tales[2];
            if (texts[n] != 0)
                n++;
            break;
        case 7:
            texts[n] = tavern_tales[3];
            if (texts[n] != 0)
                n++;
            break;
        case 8:
            texts[n] = tavern_tales[4];
            if (texts[n] != 0)
                n++;
            break;
        case 9:
            texts[n] = tavern_tales[5];
            if (texts[n] != 0)
                n++;
            break;
        }
    i = 0;
    turn_done = 0;
    while (!turn_done) {
        do {
            count = 0;
            if (ev[7] & 0x20)
                hotkey_add(0, &count);
            if (n != 0)
                hotkey_add(1, &count);
            if (ev[7] & 0x10)
                hotkey_add(2, &count);
            hotkey_add(3, &count);
            key = menu_run("", tavern_items, 1, 1);
            if (menu_escaped && key != 0x1b) {
                if (view_mode != 3 && cursor_obj[0x22] == 1) {
                    party_rows(cur_node, 0);
                    party_key_select(key);
                    party_rows(cur_node, 1);
                }
                key = 0xff;
            }
        } while (key >= 4 && (key != 0x1b || !menu_escaped));
        combat_text_clear();
        switch (key) {
        case 0:
            if (ev[0x10] + ev[0x11] == 0 || ev[0] == 0x16 || !(ev[7] & 0x20)) {
                cursor_obj[0x39] = 0;
                event_print("The party orders a local drink.");
            } else {
                total += tavern_drink(ev);
                if (total >= 60 && ev[0x13] != 0) {
                    r = ev[0x13];
                    if (r)
                        turn_done = 1;
                } else {
                    combat_text_clear();
                    cursor_obj[0x39] = 0;
                    event_print("The party has a drink.");
                }
            }
            break;
        case 1:
            if (ev[7] & 8)
                i = random_n(n);
            if (i >= n)
                i = 0;
            bank_text_get(text_pool_block, texts[i], msg_buf);
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
            i++;
            break;
        case 2:
            r = ev[0x12];
            if (r)
                turn_done = 1;
            else {
                combat_text_clear();
                cursor_obj[0x39] = 0;
                event_print("Everyone runs away. There's no one to fight!");
            }
            break;
        case 3:
            turn_done = 1;
            break;
        }
    }
    if (ev[7] & 4)
        step_back_due = 1;
    return r;
}

void far event_shop(unsigned char far *ev)                    /* 2cff:14b7 */
{
    char ok;
    int k;

    if (ev[6] == 0) {
        ev[6] = 0xd2;
        ev[7] = ev[7] | 0x80;
    }
    event_pic_show(ev);
    combat_text_clear();
    if (ev[0] == 0x16) {
        cursor_obj[0x39] = 0;
        event_print("The party enters a local shop.");
    } else {
        cursor_obj[0x39] = 1;
        event_print("\"May I help you?\"");
    }
    pool_clear();
    ok = 1;
    for (k = 3; k >= 0; k--)
        if (treasure_mask_add(ev + k * 3u + 8))
            ok = 0;
    if (ok == 1)
        pool_items = 0;
    cursor_obj[0x28] = ev[5];
    shop_menu();
    pool_clear();
    if (ev[7] & 4)
        step_back_due = 1;
    combat_text_clear();
}

void far event_transfer(unsigned char far *ev, char kind)         /* 2cff:1597 */
{
    struct Hero far *h;
    struct Item far *it;
    char go;
    char drow;
    char show;
    char stay;
    unsigned char x;
    unsigned char y;
    unsigned char dir;
    unsigned char slot;

    rerun_square = ev[0xc] & 4;
    if (ev[6] != 0)
        event_pic_show(ev);
    if (ev[7] & 0x20) {
        if (*(int far *)(ev + 4) != 0) {
            bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
            combat_text_clear();
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
        }
        go = (unsigned char)confirm("", 1) == !((ev[7] & 0x40) >> 6);
        box_draw();
        combat_text_clear();
    } else {
        go = 1;
        if (!event_text_show(ev) && ev[6] != 0) {
            text_pause();
            text_pause();
            text_pause();
        }
    }
    show = *(int far *)(ev + 0xa) != 0;
    stay = !(ev[7] & 0x10) && view_mode == 4 && kind != 0x22;
    if (go) {
        if (test_mode && kind == 0xb) {
            adventure_exit = 1;
            return;
        }
        if (*(int far *)(ev + 0xa) != 0)
            bank_text_get(text_pool_block, *(int far *)(ev + 0xa), msg_buf);
        if (kind == 0xb) {
            if (ev[0xe] != cur_level && ev[0xe] != 0) {
                transfer_new_level = 1;
                chain_allowed = 0;
            }
            if (ev[0xe] != 0)
                cur_level = ev[0xe];
            if (ev[0xc] & 8) {
                drow = 0;
                for (h = node_list; h != 0; h = h->next) {
                    drow &= 1;
                    for (it = h->items; it != 0; it = it->next)
                        if (it->type == 0x3e || it->type == 0x3f) {
                            item_remove(h, it);
                            it = h->items;
                            drow = 3;
                        }
                    if (drow & 2)
                        recalc_stats(h);
                }
                if (drow) {
                    text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1,
                              "The party's drow equipment dissolves in the sunlight.");
                    press_return_clear();
                }
            }
            drow = !(ev[0xc] & 1);
            x = ev[9];
            y = ev[8];
            dir = (ev[7] & 0xc) >> 1;
            slot = ev[0xd];
            geo_load(cur_level);
            if (cur_level <= 4)
                view_map();
            else
                view_3d();
            event_init();
            if (!drow) {
                transfer_dest_x = int_min(cur_area_rec->spots[slot - 1].x, cur_area_rec->cols);
                transfer_dest_y = int_min(cur_area_rec->spots[slot - 1].y, cur_area_rec->rows);
                geo.facing = cur_area_rec->spots[slot - 1].facing & 7;
            } else {
                transfer_dest_x = int_min(x, cur_area_rec->cols);
                transfer_dest_y = int_min(y, cur_area_rec->rows);
                geo.facing = dir;
            }
        } else {
            transfer_dest_x = int_min(ev[9], cur_area_rec->cols);
            transfer_dest_y = int_min(ev[8], cur_area_rec->rows);
            geo.facing = (ev[7] & 0xc) >> 1;
        }
        party_pos_set();
        cursor_obj[0x17] = transfer_dest_x;
        cursor_obj[0x18] = transfer_dest_y;
        event_view_restore();
        if (show) {
            combat_text_clear();
            cursor_obj[0x39] = 0;
            event_print(msg_buf);
            press_return_wait(1);
        }
        party_has_moved = 1;
    } else {
        if (stay) {
            prev_pos_restore();
            view_update();
            party_has_moved = 1;
        }
        if (!stay || go)
            event_view_restore();
    }
}

void far event_town(unsigned char far *ev)                    /* 2cff:19e9 */
{
    unsigned char tmp[0x14];
    unsigned char count;
    unsigned char key;

    key = 0xff;
    menu_escaped = 0;
    quit_requested = 0;
    while (key != 6 && !pending_load_get() && !quit_requested) {
        if (menu_escaped)
            menu_escaped = 0;
        else {
            byte_6723 = 1;
            if (ev[6] != 0)
                event_pic_show(ev);
            else
                view_refresh();
            if (*(int far *)(ev + 4) != 0) {
                bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
                combat_text_clear();
                cursor_obj[0x39] = 0;
                event_print(msg_buf);
            }
            party_display(cur_node);
            byte_6723 = 0;
        }
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        count = 0;
        if (ev[7] & 1)
            hotkey_add(0, &count);
        if (ev[7] & 2)
            hotkey_add(1, &count);
        if (ev[7] & 4)
            hotkey_add(2, &count);
        if (ev[7] & 8)
            hotkey_add(3, &count);
        if (ev[7] & 0x10)
            hotkey_add(4, &count);
        if (ev[7] & 0x20)
            hotkey_add(5, &count);
        hotkey_add(6, &count);
        key = node_prompt(empty_text, town_items, 1, 1);
        if (menu_escaped) {
            if (key == 0x1b) {
                key = 6;
                continue;
            }
            if (!picture_hides_roster) {
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
            }
            continue;
        }
        tmp[0] = 0x16;
        switch (key) {
        case 0:
            tmp[4] = (ev[9] & 0xe0) >> 5;
            tmp[5] = ev[0xc] & 0x1f;
            tmp[6] = 0xdc;
            tmp[7] = 0x80;
            *(int *)(tmp + 0xa) = 0;
            *(int *)(tmp + 8) = 0;
            tmp[0xc] = 0;
            event_temple(tmp);
            break;
        case 1:
            mem_move(ev, tmp, 0x14);
            *(int *)(tmp + 4) = 0;
            tmp[0] = 0x16;
            tmp[9] = ev[0xc] & 0x1f;
            tmp[6] = 0xe2;
            tmp[7] = 0x80;
            event_train(tmp, 0);
            break;
        case 2:
            tmp[5] = ev[9] & 0x1f;
            tmp[6] = 0xd2;
            tmp[7] = 0x80;
            tmp[8] = 0;
            tmp[9] = 0;
            tmp[0xa] = 0;
            tmp[0xb] = 0;
            tmp[0xc] = 0;
            tmp[0xd] = 0;
            tmp[0xe] = ev[0xe];
            tmp[0xf] = ev[0xf];
            tmp[0x10] = ev[0x10];
            tmp[0x11] = ev[0x11];
            tmp[0x12] = ev[0x12];
            tmp[0x13] = ev[0x13];
            event_shop(tmp);
            break;
        case 3:
            saved_view_mode = view_mode;
            tmp[0] = 0x16;
            tmp[6] = 0xe7;
            tmp[7] = 0x80;
            event_inn(tmp, 0);
            view_mode = saved_view_mode;
            break;
        case 4:
            tmp[0] = 0x16;
            tmp[6] = 0x86;
            tmp[7] = 0xa0;
            tmp[8] = ev[0xa];
            tmp[9] = ev[0xb];
            tmp[0xa] = 0;
            tmp[0xb] = 0;
            tmp[0xc] = 0;
            tmp[0xd] = 0;
            tmp[0xe] = 0;
            tmp[0xf] = 0;
            tmp[0x10] = 0;
            tmp[0x11] = 0;
            tmp[0x12] = 0;
            tmp[0x13] = 0;
            event_tavern(tmp);
            break;
        case 5:
            tmp[0] = 0x16;
            saved_view_mode = view_mode;
            tmp[6] = 0xc1;
            tmp[7] = 0x80;
            event_vault(tmp);
            view_mode = saved_view_mode;
            break;
        }
        if (key != 7)
            box_draw();
    }
    combat_text_clear();
}

void far event_pass_time(unsigned char far *ev)                    /* 2cff:1d8d */
{
    event_text_show(ev);
    clock_advance(ev[8], 4);
    clock_advance(ev[9], 3);
    clock_advance(ev[0xa], 1);
}

void far event_utilities(unsigned char far *ev)                    /* 2cff:1dd7 */
{
    char flag;

    switch (ev[4] & 3) {
    case 1:
        cursor_obj[0x45 + ev[5]] = ev[6];
        break;
    case 2:
        if (cursor_obj[0x45 + ev[5]] + ev[6] > 0xff)
            cursor_obj[0x45 + ev[5]] = 0xff;
        else
            cursor_obj[0x45 + ev[5]] += ev[6];
        break;
    case 3:
        if (cursor_obj[0x45 + ev[5]] - ev[6] < 0)
            cursor_obj[0x45 + ev[5]] = 0;
        else
            cursor_obj[0x45 + ev[5]] -= ev[6];
    }
    if (ev[4] & 4) {
        flag = 0;
        for (tick_slot = 0; tick_slot <= 5; tick_slot++)
            switch (tick_slot) {
            case 0:
                if (cursor_obj[0x45 + ev[7]] == 0)
                    flag = 1;
                break;
            case 1:
                if (cursor_obj[0x45 + ev[8]] == 0)
                    flag = 1;
                break;
            case 2:
                if (cursor_obj[0x45 + ev[9]] == 0)
                    flag = 1;
                break;
            case 3:
                if (cursor_obj[0x45 + ev[0xa]] == 0)
                    flag = 1;
                break;
            case 4:
                if (cursor_obj[0x45 + ev[0xb]] == 0)
                    flag = 1;
                break;
            case 5:
                if (cursor_obj[0x45 + ev[0xc]] == 0)
                    flag = 1;
                break;
            }
        cursor_obj[0x45 + ev[0xd]] = !flag;
    }
    if (ev[4] & 8) {
        flag = 0;
        for (tick_slot = 0; tick_slot <= 5; tick_slot++)
            switch (tick_slot) {
            case 0:
                if (cursor_obj[0x45 + ev[7]] != 0)
                    flag = 1;
                break;
            case 1:
                if (cursor_obj[0x45 + ev[8]] != 0)
                    flag = 1;
                break;
            case 2:
                if (cursor_obj[0x45 + ev[9]] != 0)
                    flag = 1;
                break;
            case 3:
                if (cursor_obj[0x45 + ev[0xa]] != 0)
                    flag = 1;
                break;
            case 4:
                if (cursor_obj[0x45 + ev[0xb]] != 0)
                    flag = 1;
                break;
            case 5:
                if (cursor_obj[0x45 + ev[0xc]] != 0)
                    flag = 1;
                break;
            }
        cursor_obj[0x45 + ev[0xd]] = flag;
    }
    if (ev[4] & 0x10)
        adventure_exit = 1;
}

char far event_quest(unsigned char far *ev)                    /* 2cff:20ba */
{
    char yes;
    char next;

    yes = 0;
    if (ev[6] != 0)
        event_pic_show(ev);
    else
        view_refresh();
    bank_text_get(text_pool_block, *(int far *)(ev + 4), msg_buf);
    cursor_obj[0x39] = 0;
    event_print(msg_buf);
    switch ((ev[7] & 0x38) >> 3) {
    case 0:
        confirm("", 0);
        combat_text_clear();
        yes = 0;
        break;
    case 1:
        yes = confirm("", 0);
        combat_text_clear();
        break;
    case 2:
        yes = !confirm("", 0);
        combat_text_clear();
        break;
    case 3:
        confirm("", 0);
        combat_text_clear();
        yes = 1;
        break;
    case 4:
        yes = 0;
        if (*(int far *)(ev + 4) != 0)
            press_return_wait(1);
        else if (ev[6] != 0)
            text_pause();
        break;
    case 5:
        yes = 1;
        if (*(int far *)(ev + 4) != 0)
            press_return_wait(1);
        else if (ev[6] != 0)
            text_pause();
        break;
    }
    if (yes) {
        if (ev[7] & 0x40)
            cursor_obj[0x45 + ev[8]] = 0xff;
        else
            cursor_obj[0x45 + ev[8]]++;
        next = ev[0xa];
    } else {
        if (ev[7] & 4)
            cursor_obj[0x45 + ev[8]] = 0x80;
        next = ev[0xb];
    }
    return next;
}

void far event_tavern_tales(unsigned char far *ev)                    /* 2cff:2259 */
{
    tavern_tales[0] = ev[8] + (ev[9] << 8);
    tavern_tales[1] = ev[0xa] + (ev[0xb] << 8);
    tavern_tales[2] = ev[0xc] + (ev[0xd] << 8);
    tavern_tales[3] = ev[0xe] + (ev[0xf] << 8);
    tavern_tales[4] = ev[0x10] + (ev[0x11] << 8);
    tavern_tales[5] = ev[0x12] + (ev[0x13] << 8);
}

void far event_flag_set(unsigned char far *ev)                    /* 2cff:22df */
{
    if (ev[6] != 0)
        event_pic_show(ev);
    else
        view_refresh();
    event_text_show(ev);
    cursor_obj[0x45 + ev[8]] = (ev[7] & 4) ? 0 : 1;
}

char far event_choice_next(unsigned char far *ev, unsigned char way, unsigned char far *p)
                                                                 /* 2cff:232e */
{
    char next;

    switch (way) {
    case 0:
        if (ev[7] & 4)
            event_stack_push(*p);
        next = ev[0xa];
        if (next) {
            chain_next_due = 1;
            skip_redraw = 1;
        } else
            skip_redraw = 0;
        if (ev[0xf] & 1)
            step_back_due = 1;
        break;
    case 1:
        if (ev[7] & 8)
            event_stack_push(*p);
        next = ev[0xb];
        if (next) {
            chain_next_due = 1;
            skip_redraw = 1;
        } else
            skip_redraw = 0;
        if (ev[0xf] & 2)
            step_back_due = 1;
        break;
    case 2:
        if (ev[7] & 0x10)
            event_stack_push(*p);
        next = ev[0xc];
        if (next) {
            chain_next_due = 1;
            skip_redraw = 1;
        } else
            skip_redraw = 0;
        if (ev[0xf] & 4)
            step_back_due = 1;
        break;
    case 3:
        if (ev[7] & 0x20)
            event_stack_push(*p);
        next = ev[0xd];
        if (next) {
            chain_next_due = 1;
            skip_redraw = 1;
        } else
            skip_redraw = 0;
        if (ev[0xf] & 8)
            step_back_due = 1;
        break;
    case 4:
        if (ev[7] & 0x40)
            event_stack_push(*p);
        next = ev[0xe];
        if (next) {
            chain_next_due = 1;
            skip_redraw = 1;
        } else
            skip_redraw = 0;
        if (ev[0xf] & 0x10)
            step_back_due = 1;
        break;
    default:
        skip_redraw = 0;
    }
    return next;
}

char far event_test(unsigned char n, char quiet)              /* 2cff:24c1 */
{
    char ok;
    unsigned char v;
    struct Effect far *fx;
    unsigned char kind;
    struct Hero far *h;

    ok = 0;
    if (cur_event[0] == 0x23 && cursor_obj[0x45 + cur_event[8]] != cur_event[9])
        goto done;
    if (cur_event[0] == 0x26) {
        if (cursor_obj[0x45 + cur_event[8]] == 0 && (cur_event[7] & 4))
            goto done;
        if (cursor_obj[0x45 + cur_event[8]] != 0 && !(cur_event[7] & 4))
            goto done;
    }
    v = cur_event[2];
    if ((cur_event[1] & 1) && event_done_test(n))
        goto done;
    kind = cur_event[1] >> 3;
    if (kind != 0)
        switch (kind) {
        case 0:
            ok = 1;
            break;
        case 1:
            if (cursor_obj[0x45 + v] != 0)
                ok = 1;
            break;
        case 2:
            if (cursor_obj[0x45 + v] == 0)
                ok = 1;
            break;
        case 3:
            if (cursor_obj[8] >= 6 && cursor_obj[8] < 0x14)
                ok = 1;
            break;
        case 4:
            if (cursor_obj[8] < 6 || cursor_obj[8] >= 0x14)
                ok = 1;
            break;
        case 5:
            if ((unsigned)random_n(100) <= v)
                ok = 1;
            break;
        case 6:
            if (cursor_obj[0x19] != 0)
                ok = 1;
            break;
        case 7:
            if (cursor_obj[0x19] == 0)
                ok = 1;
            break;
        case 8:
            if (view_mode != 4)
                ok = 1;
            else
                switch (geo.facing) {
                case 0:
                    if (v & 1)
                        ok = 1;
                    break;
                case 2:
                    if (v & 2)
                        ok = 1;
                    break;
                case 4:
                    if (v & 4)
                        ok = 1;
                    break;
                case 6:
                    if (v & 8)
                        ok = 1;
                    break;
                default:
                    ok = 1;
                }
            break;
        case 9:
            if (cursor_obj[0x45 + v] == 0xff)
                ok = 1;
            break;
        case 10:
            if (cursor_obj[0x45 + v] == 0x80)
                ok = 1;
            break;
        case 11:
            if (cursor_obj[0x45 + v] != 0 && cursor_obj[0x45 + v] != 0x80
                && cursor_obj[0x45 + v] != 0xff)
                ok = 1;
            break;
        case 12:
            v = 0x13;
            for (h = node_list; h != 0; h = h->next)
                if (find_effect(h, v, &fx))
                    ok = 1;
            break;
        case 13:
            v = 0x13;
            ok = 1;
            for (h = node_list; h != 0; h = h->next)
                if (find_effect(h, v, &fx))
                    ok = 0;
            break;
        case 14:
            v = 0x18;
            for (h = node_list; h != 0; h = h->next)
                if (find_effect(h, v, &fx))
                    ok = 1;
            break;
        case 15:
            v = 0x18;
            ok = 1;
            for (h = node_list; h != 0; h = h->next)
                if (find_effect(h, v, &fx))
                    ok = 0;
            break;
        case 16:
            for (h = node_list; h != 0; h = h->next)
                switch (v) {
                case 0:
                    if (h->cls == 0 || h->cls == 8 || h->cls == 9 || h->cls == 10
                        || h->cls == 11 || h->cls == 12)
                        ok = 1;
                    break;
                case 2:
                    if (h->cls == 2 || h->cls == 8 || h->cls == 9 || h->cls == 13
                        || h->cls == 14 || h->cls == 15)
                        ok = 1;
                    break;
                case 3:
                    if (h->cls == 3)
                        ok = 1;
                    break;
                case 4:
                    if (h->cls == 4 || h->cls == 10)
                        ok = 1;
                    break;
                case 5:
                    if (h->cls == 5 || h->cls == 9 || h->cls == 11 || h->cls == 13
                        || h->cls == 15 || h->cls == 16)
                        ok = 1;
                    break;
                case 6:
                    if (h->cls == 6 || h->cls == 12 || h->cls == 14 || h->cls == 15
                        || h->cls == 16)
                        ok = 1;
                    break;
                }
            break;
        case 17:
            for (h = node_list; h != 0; h = h->next)
                if (h->race == v)
                    ok = 1;
            break;
        }
    else
        ok = 1;
done:
    if (!quiet) {
        chain_allowed = 1;
        if (ok) {
            if (cur_event[1] & 4)
                chain_allowed = 0;
        } else if (cur_event[1] & 2)
            chain_allowed = 0;
    }
    return ok;
}

void far events_run(unsigned char n)                          /* 2cff:29dd */
{
    unsigned char kind;
    char moved;
    char r;
    char way;
    char again;

    again = 0;
    chain_next_due = 0;
    skip_redraw = 0;
    moved = 0;
    do {
        if (n == 0)
            chain_next_due = 0;
        else
            chain_next_due = 1;
        party_has_moved = 0;
        last_pic = 0;
        while (chain_next_due && (!party_has_moved || rerun_square || again)) {
            events_running = 1;
            again = 0;
            chain_next_due = 0;
            cur_event = event_table[n - 1].bytes;
            if (event_test(n, 0)) {
                event_done_set(n);
                kind = cur_event[0];
                switch (kind) {
                case 1:
                    event_combat(cur_event, 0);
                    break;
                case 2:
                    event_text(cur_event);
                    break;
                case 3:
                    event_treasure(cur_event, 1);
                    break;
                case 4:
                    event_view_draw();
                    event_damage(cur_event);
                    break;
                case 5:
                    event_transfer(cur_event, kind);
                    break;
                case 6:
                    event_train(cur_event, n);
                    break;
                case 7:
                    n = event_tavern(cur_event);
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    }
                    break;
                case 8:
                    event_shop(cur_event);
                    break;
                case 9:
                    if (event_temple(cur_event)) {
                        n = cur_event[0xc];
                        if (n) {
                            chain_next_due = 1;
                            skip_redraw = 1;
                        }
                    }
                    break;
                case 10:
                    way = event_menu(cur_event);
                    n = event_choice_next(cur_event, way, &n);
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    } else
                        skip_redraw = 0;
                    break;
                case 11:
                    if ((cur_event[7] & 0x10) && view_mode == 4)
                        event_view_draw();
                    event_transfer(cur_event, kind);
                    break;
                case 12:
                    event_walk(cur_event);
                    break;
                case 13:
                    event_npc(cur_event);
                    break;
                case 14:
                    if (party_has_npc(cur_event[5]))
                        event_text(cur_event);
                    break;
                case 15:
                    n = event_encounter(cur_event);
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    }
                    break;
                case 16:
                    event_utilities(cur_event);
                    break;
                case 17:
                    event_sounds(cur_event);
                    break;
                case 18:
                    n = event_outcome(cur_event, event_check(cur_event));
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    }
                    break;
                case 19:
                    n = event_outcome(cur_event, event_pay(cur_event));
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    }
                    break;
                case 20:
                    n = event_outcome(cur_event, event_password(cur_event));
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    }
                    break;
                case 21:
                    way = event_menu(cur_event);
                    n = event_choice_next(cur_event, way, &n);
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    } else
                        skip_redraw = 0;
                    break;
                case 22:
                    cursor_obj[0x85] = n;
                    event_town(cur_event);
                    cursor_obj[0x85] = 0;
                    break;
                case 23:
                    n = cur_event[8];
                    if (n)
                        chain_next_due = 1;
                    else
                        skip_redraw = 0;
                    break;
                case 24:
                    saved_view_mode = view_mode;
                    event_vault(cur_event);
                    view_mode = saved_view_mode;
                    break;
                case 25:
                    event_treasure(cur_event, 0);
                    break;
                case 26:
                    event_view_draw();
                    event_xp(cur_event);
                    break;
                case 27:
                    event_view_draw();
                    event_pass_time(cur_event);
                    break;
                case 29:
                    event_inn(cur_event, n);
                    break;
                case 32:
                    event_who(cur_event);
                    break;
                case 33:
                    event_combat(cur_event, 1);
                    break;
                case 34:
                    event_view_draw();
                    event_transfer(cur_event, kind);
                    break;
                case 35:
                    n = event_quest(cur_event);
                    if (n) {
                        chain_next_due = 1;
                        skip_redraw = 1;
                    }
                    break;
                case 36:
                    r = event_question(cur_event);
                    if (r) {
                        if (cur_event[7] & 4)
                            event_stack_push(n);
                        else if (cur_event[7] & 0x20)
                            step_back_due = 1;
                        n = cur_event[8];
                        if (n) {
                            chain_next_due = 1;
                            skip_redraw = 1;
                        } else
                            skip_redraw = 0;
                    } else {
                        if (cur_event[7] & 8)
                            event_stack_push(n);
                        else if (cur_event[7] & 0x10)
                            step_back_due = 1;
                        n = cur_event[9];
                        if (n) {
                            chain_next_due = 1;
                            skip_redraw = 1;
                        } else
                            skip_redraw = 0;
                    }
                    break;
                case 38:
                    event_flag_set(cur_event);
                    break;
                case 37:
                    event_tavern_tales(cur_event);
                    break;
                }
                if (kind != 0x17)
                    moved = 1;
            }
            if (!adventure_exit) {
                if (!chain_next_due) {
                    n = 0;
                    if (chain_allowed && !party_has_moved)
                        n = cur_event[3];
                    if (n) {
                        chain_next_due = 1;
                        if (moved)
                            skip_redraw = 1;
                    }
                }
                if (!chain_next_due) {
                    if (step_back_due) {
                        step_back_due = 0;
                        prev_pos_restore();
                        view_update();
                        party_has_moved = 1;
                    }
                    if (party_has_moved && rerun_square) {
                        n = area_cell_at(geo.x, geo.y);
                        if (n)
                            chain_next_due = 1;
                    }
                }
                rerun_square = 0;
            }
            moved_last_pass = party_has_moved;
            party_has_moved = 0;
        }
        if (!adventure_exit) {
            if (!moved_last_pass) {
                n = event_stack_pop();
                if (n)
                    again = 1;
            } else {
                n = 0;
                while (event_stack_pop())
                    ;
            }
            if (n && moved)
                skip_redraw = 1;
        } else
            n = 0;
    } while (n);
    events_running = 0;
    while (event_stack_pop())
        ;
    if (view_restore_due) {
        view_restore_due = 0;
        event_view_restore();
    }
    cursor_obj[0x39] = 0;
}
