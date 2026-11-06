/* CKIT.EXE overlay section 21, module 2d91: the area editor's screen, screen 2
 * of editloop.c's loop.  area_edit is called with the screen the editor
 * comes back from, editloop.c's env and the settings record (EdState); it
 * takes in what env->bits reports for that screen, runs this screen in
 * area_loop when it stays here, and leaves in env->bits what the next
 * screen is handed.  area_loop sets up a map editor record (MapEd) on its
 * stack, points ed_state at it, and polls the dialog items until a command
 * names another screen: the pointer over the map (map_paint, paint_at),
 * the hotkeys (hotkey_do), the menu bar's three menus (menu_pick: file_pick,
 * map_pick with the views, the four placements and "Move through walls",
 * util_pick) and the bar driven from the keyboard (bar_keys, pick_step,
 * pick_mouse, pick_open, pick_hilite).  undo_apply undoes the last change
 * by its kind (EdState.undo_kind: 1 painting, 2 a paint value, 3 a spot, 4 the
 * marker) and undo_label switches the item's text between "Undo" and
 * "Un*do"; items_mark marks the dialog items and menu entries for the
 * settings.  area_start takes the tool and view from ed_mode (edscreen.c's
 * ed_mode_get), forces the map view when a's low six bits are 4 or less,
 * hands view and a to section 23's 2d91:041b, and puts the marker on the
 * area's first spot.
 *
 * Section 21 (parent section 20, which is empty and has no parent; no
 * section names 21 as its parent): payload at EXE 0x072a60, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 21.  The module is stored frame 2d91 (load-based 3d91), EXE
 * 0x072a60..0x075d85 (13,093 bytes, 29 routines), the whole section from
 * its first byte to its last retf (2d91:3324); eleven zero bytes follow to
 * the section's end (0x075d90).  One Turbo C large-model _TEXT segment,
 * built with Turbo C 2.01, tcc -ml -O -Z -d; functions are defined in
 * shipped order.  Definition comments give the load-based address; extern
 * comments the far pointer as stored in the shipped call, or the DGROUP
 * offset.
 *
 * Section 21's relocation table files all 271 of its sites under 2d91, all
 * in this range (EXE 0x072b13..0x075d5c), all far calls.  The section's
 * three vectors land here: 18de:1c6f -> area_edit (editloop.c's
 * area_edit, screen 2), 18de:1f99 -> items_mark (edscreen.c's
 * items_mark) and 18de:1fad -> area_start (edscreen.c's area_start),
 * each far-called once, from sections 10, 18 and 18.  The module far-calls
 * area_start twice ahead of its definition; no other far call in the image
 * stores frame 2d91 into section 21 (the 2d91 calls of sections 23, 25
 * and 26 name their own modules).  It near-calls 20 of its routines
 * ahead of their definitions, so those are static; undo_reset,
 * marker_place, map_paint, undo_apply, view_sync and undo_pending are
 * reached only by near calls after their definitions, so for these six the
 * bytes do not decide static or public (they are static here).  It calls
 * the resident code directly, and sections 3, 5, 10, 13, 15, 16, 18 and 23
 * only through vector stubs (156 calls).
 *
 * FRUA's own code: the sharing map pairs all 29 routines a-only, so every
 * name here is chosen from the code; cur_area_save, whose callee in section
 * 23 has not landed, keeps a placeholder.
 *
 * No _DATA and no _BSS: the module reads and writes mapedit.c's _BSS
 * (section 16), view3d.c's geo, the menu entries at 0098..0103 that
 * edscreen.c names, the texts behind 024c, 0358 and 0360 (edscreen.c's and
 * mapedit.c's names), glbdata.c's design_dir, and four DGROUP variables no
 * landed module defines (hook_key, tick_slot, menu_escaped, test_mode).
 * ed_state holds the address of area_loop's MapEd (mapedit.c and
 * edscreen.c declare it a struct EdState pointer; map_button_cb's write
 * through it lands on MapEd.f6, which map_paint reads).
 *
 * Forced by the bytes: area_edit's three shared tails as gotos (done,
 * dirty, s14; written out in each case, -O keeps other copies than the
 * shipped ones and area_edit is 7 bytes longer), its short v and k (an int
 * goes into si or di), its default's break, and the int compare of
 * mask_kind's result; EdState's f12 unsigned and flags signed (shifted
 * with sar); ed_ev_row, edit_ev_col and ed_event_dir unsigned char (as
 * char, 6 of 29 routines bake) and mouse_pending returning char (as int, 27
 * of 29); EdEnv.bits a signed long (unsigned calls LXURSH@, where the
 * shipped code calls LXRSH@); map_paint's px = py = -99 apart from
 * row = col = -99, and its beep in an else; undo_apply's four cases'
 * locals in one union at the frame's bottom, which save[16] sizes (block
 * locals for each case bake 28 of 29); (items + n - 1)->flags; pick_step's
 * upper-casing inside its condition; pick_hilite's for (i = 0, k = 0;
 * ...); bar_keys testing rc in a statement of its own.
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   a screen's result; signed here    */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct Pos {                    /* a place and facing, 5 bytes            */
    char x;                     /* +0                                     */
    char y;                     /* +1                                     */
    unsigned char facing;       /* +2                                     */
    char facing_wall;           /* +3                                     */
    char event;                 /* +4                                     */
};

struct Spot {                   /* a place the area keeps, 4 bytes        */
    unsigned char y;            /* +0                                     */
    unsigned char x;            /* +1                                     */
    unsigned char facing;       /* +2                                     */
    unsigned char which;        /* +3   in EdState.undo_spot: which spot  */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0                                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    unsigned char sets[3];      /* +4                                     */
    unsigned char flags;        /* +7                                     */
    unsigned char back[4];      /* +8                                     */
    char pad_c[2];              /* +c   not read here                     */
    struct Spot spots_wide[26]; /* +e   bound: spacing to mapedit.c's +76 */
};

struct Geo {                    /* view3d.c's geo record                  */
    struct Pos pos;             /* +0   d414..d418                        */
    char attr_5;                /* +5                                     */
    char attr_6;                /* +6                                     */
    char show_map;              /* +7                                     */
    struct Area far *area;      /* +8   d41c                              */
    char attr_c;                /* +c                                     */
    char attr_d;                /* +d                                     */
};

struct EdState {                /* the editor's settings, 0x51 bytes      */
    int screen;                 /* +0   the screen area_edit returns      */
    int prev;                   /* +2   the screen to come back to        */
    unsigned char view;         /* +4   0 the 3D view, 1 the map          */
    unsigned char tool;         /* +5   0 wall 1 backdrop 2 zone 3 event  */
    int f6;                     /* +6   a request to the next screen      */
    char f8;                    /* +8   not read here                     */
    char move;                  /* +9   0 "Move through walls" marked     */
    unsigned char paint[7];     /* +a   by SIDE_A .. EVENT                */
    char flags;                 /* +11  bits 0, 1 undo_pending; 2 from 14 */
    unsigned char undo_kind;    /* +12  the undo kind, 0 none             */
    char both;                  /* +13  takes view (mapedit.c: back side) */
    unsigned char undo_slot;    /* +14  kind 2: which paint value         */
    unsigned char old[7];       /* +15  kind 1: the values painted over   */
    unsigned char undo_paint[7]; /* +1c kind 2: the paint values before   */
    struct Spot undo_spot;      /* +23  kind 3: the spot before           */
    struct Pos undo_pos;        /* +27  kind 4: the marker before         */
    struct Pos pos;             /* +2c  the party marker in the map view  */
    struct Spot undo_spots[8];  /* +31  kind 3, n of 8 or more: all eight */
};

struct Entry {                  /* one menu entry, 8 bytes, as menubox.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6                                     */
    char flags;                 /* +7                                     */
};

struct Menu {                   /* a pop-up menu, 0x14 bytes, menubox.c's */
    char far *title;            /* +0                                     */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8                                     */
    int col;                    /* +a                                     */
    char count;                 /* +c                                     */
    char flags;                 /* +d                                     */
    int glyph;                  /* +e                                     */
    int row;                    /* +10                                    */
    int width;                  /* +12                                    */
};

struct View {                   /* a scrolled menu's window, menubox.c's  */
    int above;                  /* +0                                     */
    int below;                  /* +2                                     */
    int count;                  /* +4                                     */
    int bottom;                 /* +6                                     */
};

struct MapEd {                  /* the map editor, mapedit.c's            */
    struct EdState far *st;     /* +0                                     */
    char erase;                 /* +4   paint 0 instead of the settings   */
    char pointer_mode;          /* +5   the mouse pointer's state         */
    int button;                 /* +6   2 erases; map_button_cb writes it  */
    struct Menu far *menus[5];  /* +8                                     */
    char nmenus;                /* +1c                                    */
    char hover;                 /* +1d  the mouse is over the map         */
    unsigned char shown[8];     /* +1e  what the status panel shows       */
};

enum { SIDE_A, SIDE_B, KIND_A, KIND_B, BACK, ZONE, EVENT };

extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far to_lower(int c);                             /* 00d7:25ab */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern int  far menu_line_key(int key);                              /* 083a:1e2d */
extern void far wait_or_key(int n);                              /* 0da7:0106 */
extern void far wait_ticks(int n);                               /* 0da7:0151 */
extern void far beep(void);                                      /* 0da7:018c */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far screen_page_save(void);                              /* 0f5e:0291 */
extern void far page_swap(void);                              /* 0f5e:02dc */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern int  far mouse_press(char mask, int far *x, int far *y); /* 0f5e:0ee9 */
extern char far mouse_clicked(int far *x, int far *y);            /* 0f5e:0fa2 */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern char far mouse_pending(void);                              /* 0f5e:1173 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern char far alt_held(void);                              /* 107e:02af */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far pointer_glyph_load(int icon, int frame);               /* 11d7:0cf9 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_poll(void);                                   /* 15e5:15e6 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far design_dir_set(char far *dir);
                                /* 18de:1369, vector to overlay 3 1b41:0354 */
extern void far design_dir_restore(void);
                                /* 18de:1373, vector to overlay 3 1b41:037c */
extern void far game_load(char on);
                                /* 18de:144f, vector to overlay 5 1e8f:1150 */
extern char far area_cell_at(char x, char y);
                                /* 18de:15ad, vector to overlay 13 26e3:0ae7 */
extern unsigned char far kind_mask(unsigned char kind, int dir);
                                /* 18de:1797, vector to overlay 5 1e8f:0f1b */
extern void far cur_area_set(unsigned char n);
                                /* 18de:17b5, vector to overlay 5 1e8f:0e91 */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern unsigned char far mask_kind(unsigned char m, int far *dir);
                                /* 18de:17c9, vector to overlay 5 1e8f:0f9e */
extern char far shift_key_down(void);
                                /* 18de:1823, vector to overlay 5 1e8f:000e */
extern void far labels_forget(void);
                                /* 18de:184b, vector to overlay 5 1e8f:0014 */
extern int  far pop_hilite(int attr);
                                /* 18de:18a5, vector to overlay 5 1ff3:0009 */
extern int  far pop_scroll(struct Menu far *m, struct View far *v, int delta, int attr,
                             int off, int pos, int far *last, int n);
                                /* 18de:18af, vector to overlay 5 1ff3:0b60 */
extern void far pop_draw(struct Menu far *m, int attr, struct View far *v);
                                /* 18de:18c3, vector to overlay 5 1ff3:0e7f */
extern int  far pop_run(struct Menu far *m, int attr, struct View far *v);
                                /* 18de:18cd, vector to overlay 5 1ff3:0804 */
extern int  far pop_width(struct Menu far *m);
                                /* 18de:18d7, vector to overlay 5 1ff3:121a */
extern void far menu_box_draw(struct Menu far *m);
                                /* 18de:1909, vector to overlay 5 219b:000e */
extern int  far menu_box_width(struct Menu far *m);
                                /* 18de:191d, vector to overlay 5 219b:0162 */
extern char far edit_dirty(void);
                                /* 18de:1b7f, vector to overlay 10 267d:047f */
extern char far edit_flag(void);
                                /* 18de:1b89, vector to overlay 10 267d:049b */
extern void far edit_flag_clear(void);
                                /* 18de:1b93, vector to overlay 10 267d:0491 */
extern void far edit_dirty_set(void);
                                /* 18de:1bb1, vector to overlay 10 267d:046d */
extern void far map_mark(int y, int x, unsigned char dir);
                                /* 18de:1ce7, vector to overlay 13 26e3:04b3 */
extern char far map_scroll(char far *y, char far *x, int far *top, int far *left,
                             int rows, int cols, char center);
                                /* 18de:1cfb, vector to overlay 13 26e3:0158 */
extern char far cell_event_set(int n, int v);
                                /* 18de:1d19, vector to overlay 16 29f4:0033 */
extern int  far cell_event(int n);
                                /* 18de:1d23, vector to overlay 16 29f4:0019 */
extern void far marker_key(struct MapEd far *m, int key, void (far *fn)(struct EdState far *));
                                /* 18de:1d41, vector to overlay 16 29f4:1464 */
extern void far map_hover(struct MapEd far *m);
                                /* 18de:1d5f, vector to overlay 16 29f4:0338 */
extern void far map_redraw(struct MapEd far *m, char full);
                                /* 18de:1d69, vector to overlay 16 29f4:103d */
extern void far undo_set(struct EdState far *e, char a, char show);
                                /* 18de:1d73, vector to overlay 16 29f4:02e5 */
extern char far cell_closed(int n);
                                /* 18de:1d7d, vector to overlay 16 29f4:35f5 */
extern void far panel_draw(struct MapEd far *m, char full);
                                /* 18de:1d87, vector to overlay 16 29f4:1a71 */
extern void far panel_tool_draw(struct MapEd far *m);
                                /* 18de:1d91, vector to overlay 16 29f4:196d */
extern void far grid_cell_redraw(int y, int x, unsigned char rows, unsigned char cols, char tool);
                                /* 18de:1daf, vector to overlay 16 29f4:2bd2 */
extern void far tool_info_draw(struct MapEd far *m, char full);
                                /* 18de:1dcd, vector to overlay 16 29f4:1e86 */
extern char far map_hit_cell(int r, int c);
                                /* 18de:1dd7, vector to overlay 16 29f4:0478 */
extern int  far map_hit_wall(int r, int c);
                                /* 18de:1de1, vector to overlay 16 29f4:03a3 */
extern void far ed_mode_set(char a, char b);
                                /* 18de:1e1d, vector to overlay 16 29f4:0000 */
extern void far map_page(struct MapEd far *m, int key, void (far *fn)(struct EdState far *));
                                /* 18de:1e27, vector to overlay 16 29f4:1296 */
extern int  far dir_right(unsigned char f);
                                /* 18de:1e31, vector to overlay 16 29f4:373c */
extern int  far ed_item_base(struct EdState far *e);
                                /* 18de:1e3b, vector to overlay 16 29f4:3774 */
extern void far panel_title_draw(struct EdState far *e);
                                /* 18de:1e4f, vector to overlay 16 29f4:1840 */
extern int  far dir_left(unsigned char f);
                                /* 18de:1e59, vector to overlay 16 29f4:3757 */
extern char far area_overland(void);
                                /* 18de:1e63, vector to overlay 16 29f4:378b */
extern void far pos_swap(struct Pos far *a, struct Pos far *b);
                                /* 18de:1e6d, vector to overlay 16 29f4:37a8 */
extern void far block_undo(struct MapEd far *m, int r, int c);
                                /* 18de:1e81, vector to overlay 16 29f4:0db0 */
extern char far tool_paint(struct MapEd far *m, char r, char c, char d, char redraw);
                                /* 18de:1e8b, vector to overlay 16 29f4:05f9 */
extern void far overland_cell_redraw(int y, int x, unsigned char rows, unsigned char cols, char tool);
                                /* 18de:1e95, vector to overlay 16 29f4:3295 */
extern char far drag_axis(int r, int c, int far *pr, int far *pc, char flag);
                                /* 18de:1e9f, vector to overlay 16 29f4:0512 */
extern void far ed_frame(int view, char menu);
                                /* 18de:1f3f, vector to overlay 18 2cf5:0165 */
extern void far ed_items(int view);
                                /* 18de:1f49, vector to overlay 18 2cf5:043e */
extern void far ed_window(char on, int view);
                                /* 18de:1f53, vector to overlay 18 2cf5:066f */
extern void far ed_enter(struct EdState far *e);
                                /* 18de:1f5d, vector to overlay 18 2cf5:0034 */
extern void far ed_layout(struct MapEd far *m);
                                /* 18de:1f67, vector to overlay 18 2cf5:025b */
extern void far ed_open(struct MapEd far *m);
                                /* 18de:1f71, vector to overlay 18 2cf5:00fe */
extern void far ed_start(struct EdState far *e, unsigned char a);
                                /* 18de:1f7b, vector to overlay 18 2cf5:000f */
extern void far ed_mode_get(unsigned char far *tool, unsigned char far *view);
                                /* 18de:1f85, vector to overlay 18 2cf5:070a */
extern void far shown_clear(struct MapEd far *m);
                                /* 18de:1f8f, vector to overlay 18 2cf5:0139 */
extern void far bar_draw(void);
                                /* 18de:1fa3, vector to overlay 15 2999:0373 */
extern void far save_items_mark(void);
                                /* 18de:1fb7, vector to overlay 23 2d91:03fb */
extern int  far bar_take(void);
                                /* 18de:1fc1, vector to overlay 15 2999:0597 */
extern void far area_save(int view, int n);
                                /* 18de:1fcb, vector to overlay 23 2d91:04cb */
extern void far area_open(int view, int a);
                                /* 18de:1fd5, vector to overlay 23 2d91:041b */
extern int  far bar_track(int x, int y);
                                /* 18de:1fdf, vector to overlay 15 2999:015f */
extern int  far bar_query(int n, int item, int q);
                                /* 18de:1fe9, vector to overlay 15 2999:046c */
int __abs__(int v);

extern struct Entry map_items[9];                   /* 0098  "3-D view", ...   */
extern struct Entry util_items[8];                  /* 00f4  "Display access", ... */
extern char far *undo_names[];                        /* 024c  "Undo", ...       */
extern char far *place_text;                          /* 0358  "Place"           */
extern char far *edit_text;                          /* 0360  "Edit"            */
extern char design_dir[0x22];                                      /* 5a54 */
extern int hook_key;                                            /* b384 */
extern char tick_slot;                                           /* b3f2 */
extern char menu_escaped;                                        /* ca2a */
extern char test_mode;                                           /* cc7a */
extern struct Geo geo;                                           /* d414 */
extern int ed_x;                                                 /* d42a */
extern struct MapEd far *ed_state;                               /* d42c */
extern int edit_y;                                               /* d430 */
extern int ed_ypos;                                              /* d432 */
extern unsigned char ed_rows;                                    /* d434 */
extern unsigned char ed_cols;                                    /* d435 */
extern char edit_undo[6];                                        /* d436 */
extern int ed_pix_x;                                             /* d43c */
extern unsigned char ed_ev_row;                                  /* d440 */
extern unsigned char edit_ev_col;                                /* d441 */
extern unsigned char ed_event_dir;                               /* d444 */

static void far area_begin(struct EdState far *e);
static char far spot_set(struct EdState far *e, unsigned char n, struct Pos far *p);
static void far pos_clamp(struct EdState far *e);
static void far paint_at(struct MapEd far *m, int y, int x, int d, char redraw);
static void far undo_label(struct MapEd far *m);
static void far undo_note(struct EdState far *e, int i, int v, char show);
static void far area_loop(struct EdState far *e);
static void far key_command(struct MapEd far *m, int key, void (far *fn)(struct EdState far *));
static void far menu_pick(struct MapEd far *m, unsigned char menu, unsigned char item);
static void far file_pick(struct MapEd far *m, unsigned char item);
static void far map_pick(struct MapEd far *m, unsigned char item);
static void far util_pick(struct MapEd far *m, unsigned char item);
static void far hotkey_do(struct MapEd far *m, int key);
static void far paint_here(struct MapEd far *m);
void far area_start(struct EdState far *e, int a);
static void far cur_area_save(struct EdState far *e, int n);
static char far bar_keys(struct MapEd far *m, int key, struct Menu far * far *menus,
                         unsigned char n, int first, struct View far *view);
static char far pick_step(struct Menu far *menu, unsigned char far *pi, int far *pitem,
                          unsigned char n, struct View far *view);
static char far pick_mouse(struct Menu far *menu, unsigned char far *pi, int far *pitem,
                           struct View far *view);
static void far pick_open(struct Menu far *menu, int n, struct View far *view);
static void far pick_hilite(struct Menu far *menu, int far *pitem, int far *pprev,
                            struct View far *view);

int far area_edit(int prev, struct EdEnv far *env, void far *arg)
                                                                 /* 3d91:0000 */
{
    short v;
    short k;
    struct EdState far *e;
    char dir[0x22];

    if (arg == 0)
        return 0;
    e = arg;
    e->screen = prev;
    switch (prev) {
    case 11:
        v = e->f6;
        if (!(v & 0xf)) {
            e->screen = 1;
            if (env->bits & 0xf) {
                env->bits &= 0xffc0ffffL;
                env->bits |= (env->bits & 0x3f0) << 12;
                ed_start(e, (env->bits & 0x3f0) >> 4);
                area_begin(e);
            } else
                e->f6 = 1;
            break;
        }
        e->screen = e->prev;
        e->f6 = 0;
        if (env->bits & 0xf) {
            switch (v & 0xf) {
            case 5:
                switch ((v & 0x30) >> 4) {
                case 1:
                    v = e->paint[ZONE];
                    e->paint[ZONE] = (env->bits & 0xff0) >> 4;
                    undo_note(e, ZONE, v, 0);
                    break;
                case 2:
                    e->f6 = (env->bits & 0xff0) >> 4 | 0x8000;
                    e->screen = 0x14;
                    break;
                }
                break;
            case 3:
                if (spot_set(e, (env->bits & 0xff0) >> 4, &e->pos)) {
            dirty:                      /* also case 9's, after a load */
                    edit_dirty_set();
                }
                break;
            }
        }
    done:                               /* also cases 10, 12, 8 and 9 */
        if (!e->f6)
            area_loop(e);
        break;
    case 10:
    case 14:
    case 15:
    case 17:
        e->screen = e->prev;
        v = e->f6;
        e->f6 = 0;
        if (env->bits & 0xf) {
            if (prev == 14) {
                e->flags &= ~4;
                edit_dirty_set();
                undo_set(e, 3, 0);
            } else if (prev == 15)
                undo_set(e, 4, 0);
        } else if (prev == 14 && (e->flags & 4)) {
            undo_set(e, 0, 0);
            e->flags &= ~4;
        }
        goto done;
    case 20:
        e->screen = e->prev;
        if (env->bits & 0xf) {
            edit_dirty_set();
            undo_set(e, 0, 0);
        }
        area_loop(e);
        break;
    case 12:
        e->screen = e->prev;
        if (env->bits & 0xf) {
            v = (env->bits & 0xff0) >> 4;
            switch (e->f6) {
            case 1:
                e->f6 = e->paint[SIDE_A];
                e->paint[SIDE_B] = e->paint[SIDE_A] = v & 0xf;
                undo_note(e, SIDE_A, e->f6, 0);
                e->f6 = e->paint[KIND_A];
                e->paint[KIND_B] = e->paint[KIND_A] = v >> 4 & 0xf;
                undo_note(e, KIND_A, e->f6, 0);
                e->f6 = 0;
                break;
            case 2:
                e->f6 = v | 0x8000;
            s14:                        /* also case 8's */
                e->screen = 0x14;
                break;
            }
        } else
            e->f6 = 0;
        goto done;
    case 8:
        e->screen = e->prev;
        v = e->f6;
        e->f6 = 0;
        if ((k = (env->bits & 0xff00) >> 8) & 0x80) {
            if ((int)mask_kind(k, 0) == 9) {
                switch (v) {
                case 1:
                    v = e->paint[BACK];
                    e->paint[BACK] = env->bits & 0xff;
                    undo_note(e, BACK, v, 0);
                    break;
                case 2:
                    e->f6 = env->bits & 0xff | 0x8000;
                    goto s14;
                }
            }
        }
        goto done;
    case 9:
        e->screen = e->prev;
        v = e->f6;
        e->f6 = 0;
        if (env->bits & 0xf) {
            switch (v) {
            case 1:
            case 4:
                if ((env->bits & 0xf) == 2) {
                    if (v == 4) {
                        str_copy(dir, design_dir);
                        design_dir_restore();
                        labels_forget();
                    }
                    cur_area_save(e, cur_area_get());
                    if (v == 4) {
                        design_dir_set(dir);
                        labels_forget();
                    }
                }
                k = cur_area_get();
                area_start(e, (env->bits >> 16) & 0x3f);
                if (v == 1) {
                    if (area_overland() && e->tool == 1)
                        e->tool = 0;
                    break;
                }
                design_dir_restore();
                labels_forget();
                cur_area_set(k);
                game_load(0);
                env->bits &= 0xffc0ffffL;
                env->bits |= cur_area_get() << 16;
                goto dirty;
            case 5:
                area_start(e, cur_area_get());
                break;
            case 3:
                if (cur_area_get() != (env->bits >> 16 & 0x3f)) {
                    cur_area_set(env->bits >> 16 & 0x3f);
                    edit_dirty_set();
                }
            case 2:
                cur_area_save(e, cur_area_get());
                break;
            case 10:
                if ((env->bits & 0xf) == 2)
                    cur_area_save(e, cur_area_get());
                e->f6 = 1;
                break;
            }
        }
        goto done;
    case 5:
        env->bits &= 0xffffL;
        env->bits |= cur_area_get() << 16;
        if (e->f6 & 0x8000)
            pos_swap(&e->pos, &geo.pos);
        e->screen = e->prev;
        if (!(env->bits & 0xf) && area_overland()
            && cell_closed(geo.area->cols * geo.pos.y + geo.pos.x))
            e->flags |= 2;
        else
            undo_set(e, 0, 0);
        area_loop(e);
        break;
    case 3:
        env->bits &= 0xffffL;
        env->bits |= cur_area_get() << 16;
    case 13:
    case 19:
        e->screen = e->prev;
        if (edit_flag()) {
            edit_flag_clear();
            ed_window(1, e->view);
            pos_clamp(e);
        }
        if (prev == 19 && (env->bits & 0xf)) {
            edit_dirty_set();
            undo_set(e, 0, 0);
        }
        area_loop(e);
        break;
    case 1:
        if (test_mode) {
            ed_enter(e);
            if (test_mode == 2)
                edit_dirty_set();
            test_mode = 0;
            area_begin(e);
            break;
        }
        if ((v = (env->bits & 0x3f0000L) >> 16) == 0)
            v = 5;
        env->bits &= 0xffc0ffffL;
        e->screen = 0xb;
        e->f6 = v << 4 | 0;
        break;
    default:
        if (area_overland())
            ed_start(e, cur_area_get());
        else
            ed_enter(e);
        area_begin(e);
        break;
    }
    env->bits &= 0xffff0000L;
    switch (e->screen) {
    case 5:
        if (e->f6 & 0x8000)
            pos_swap(&e->pos, &geo.pos);
        env->bits |= (unsigned char)area_cell_at(geo.pos.x, geo.pos.y) & 0xff;
        env->bits = env->bits | ((e->f6 & 6) << 10 | 0x100);
        break;
    case 9:
        if (e->view == 0)
            env->bits |= 0x2000;
        env->bits = env->bits | (e->f6 & 0xffL);
        break;
    case 10:
        v = e->f6 & 0x3f;
        env->bits = env->bits | v;
        break;
    case 20:
        env->bits = env->bits | (e->f6 & 0xff);
        break;
    case 8:
        env->shift = 1;
        k = kind_mask(9, e->f6 == 2);
        env->bits |= k << 8 | e->paint[BACK];
        break;
    case 11:
        switch (v = e->f6 & 0xf) {
        case 0:
            env->bits = env->bits | e->f6;
            break;
        case 3:
            env->bits = env->bits | v;
            break;
        case 5:
            env->bits |= e->paint[ZONE] << 4 | v | (e->f6 & 0x30) << 4;
            break;
        }
        break;
    case 12:
        v = e->paint[SIDE_A] & 0xf | v & 0xfff0;
        v = (e->paint[KIND_A] & 0xf) << 4 | v & 0xff0f;
        env->bits |= (v & 0xff) << 4 | e->f6 & 0xf;
        break;
    case 1:
        geo.pos = e->pos;
        geo.pos.facing &= 7;
        ed_mode_set(e->tool, e->view);
        break;
    }
    e->flags = 0;
    return e->screen;
}

static void far undo_reset(struct EdState far *e)                /* 3d91:0b3b */
{
    register int i;

    undo_set(e, 0, 0);
    e->both = e->view;
    e->undo_slot = e->tool == 0 ? 0 : e->tool + 3;
    for (i = 0; i < 7; i++) {
        e->old[i] = e->paint[i];
        e->undo_paint[i] = e->paint[i];
    }
}

static void far area_begin(struct EdState far *e)                /* 3d91:0baa */
{
    e->prev = e->screen;
    edit_y = -1;
    ed_x = -1;
    ed_ev_row = geo.pos.y;
    edit_ev_col = geo.pos.x;
    ed_event_dir = geo.pos.facing;
    undo_reset(e);
    area_loop(e);
}

static char far spot_set(struct EdState far *e, unsigned char n, struct Pos far *p)
                                                                 /* 3d91:0bed */
{
    char changed;
    struct Spot old;

    changed = 0;
    old = geo.area->spots_wide[n];
    if (geo.area->spots_wide[n].x != p->x) {
        geo.area->spots_wide[n].x = p->x;
        changed = 1;
    }
    if (geo.area->spots_wide[n].y != p->y) {
        geo.area->spots_wide[n].y = p->y;
        changed = 1;
    }
    if (geo.area->spots_wide[n].facing != p->facing) {
        geo.area->spots_wide[n].facing = p->facing;
        changed = 1;
    }
    if (changed) {
        e->undo_spot = old;
        e->undo_spot.which = n;
        undo_set(e, 3, 0);
    }
    return changed;
}

static void far marker_place(struct EdState far *e)              /* 3d91:0cee */
{
    int d;
    int x;
    int y;

    if (e->tool || area_overland()) {
        mouse_pos(&x, &y);
        if ((d = map_hit_wall(x, y)) >= 0)
            geo.pos.facing = d;
    }
    if (area_overland())
        overland_cell_redraw(e->pos.y, e->pos.x, ed_rows, ed_cols, e->tool);
    else
        grid_cell_redraw(e->pos.y, e->pos.x, ed_rows, ed_cols, e->tool);
    if (geo.pos.facing == 0)
        geo.pos.facing = 8;
    e->undo_pos = e->pos;
    e->pos = geo.pos;
    undo_set(e, 4, 1);
    map_mark(e->pos.y, e->pos.x, e->pos.facing);
}

static void far pos_clamp(struct EdState far *e)                 /* 3d91:0df3 */
{
    char moved;

    if (geo.pos.y < 0)
        geo.pos.y = 0;
    else if (geo.area->rows <= geo.pos.y)
        geo.pos.y = geo.area->rows - 1;
    if (geo.pos.x < 0)
        geo.pos.x = 0;
    else if (geo.area->cols <= geo.pos.x)
        geo.pos.x = geo.area->cols - 1;
    moved = 0;
    if (geo.area->rows <= ed_ev_row) {
        ed_ev_row = geo.pos.y;
        moved = 1;
    }
    if (geo.area->cols <= edit_ev_col) {
        edit_ev_col = geo.pos.x;
        moved = 1;
    }
    if (moved && e->undo_kind == 1)
        undo_set(e, 0, 0);
}

static void far map_paint(struct MapEd far *m)                   /* 3d91:0e9d */
{
    int d;
    int x;
    int y;
    int px;
    int py;
    char first;
    char mode;
    int row;
    int col;

    mouse_pos(&x, &y);
    if (m->button == 2)
        m->erase = 1;
    if (m->hover) {
        mode = 0;
        px = py = -99;
        row = col = -99;
        if (m->st->tool == 0 && !area_overland()) {
            first = 1;
            do {
                if (mode) {
                    if (mode == 1 && row >= 0)
                        mode = drag_axis(x, y, &px, &py, 1);
                    switch (mode) {
                    case 2:
                        x = px;
                        break;
                    case 3:
                        y = py;
                        break;
                    }
                }
                if (x != px || y != py) {
                    if ((d = map_hit_wall(x, y)) >= 0
                        && (geo.pos.facing != d || geo.pos.y != row || geo.pos.x != col)) {
                        geo.pos.facing = d;
                        paint_at(m, geo.pos.y, geo.pos.x, geo.pos.facing, 1);
                        row = geo.pos.y;
                        col = geo.pos.x;
                    } else if (first && d == -1)
                        beep();
                    px = x;
                    py = y;
                }
                if (first) {
                    mode = alt_held();
                    first = 0;
                }
            } while (!mouse_clicked(&x, &y));
        } else {
            first = 1;
            do {
                if (mode) {
                    if (mode == 1 && row >= 0)
                        mode = drag_axis(x, y, &px, &py, 0);
                    switch (mode) {
                    case 2:
                        x = px;
                        break;
                    case 3:
                        y = py;
                        break;
                    }
                }
                if (x != px || y != py) {
                    if ((d = map_hit_cell(x, y)) > 0
                        && (geo.pos.y != row || geo.pos.x != col)) {
                        paint_at(m, geo.pos.y, geo.pos.x, geo.pos.facing, 1);
                        row = geo.pos.y;
                        col = geo.pos.x;
                    } else if (first && d == 0)
                        beep();
                    px = x;
                    py = y;
                }
                if (first) {
                    mode = alt_held();
                    first = 0;
                }
            } while (!mouse_clicked(&x, &y)
                     && (m->st->tool != 3 || (m->erase = shift_key_down()) != 0));
        }
    } else
        beep();
}

static void far paint_at(struct MapEd far *m, int y, int x, int d, char redraw)
                                                                 /* 3d91:1138 */
{
    char rc;

    m->st->both = m->st->view;
    if ((rc = tool_paint(m, y, x, d, redraw)) > 0) {
        edit_dirty_set();
        ed_ev_row = y;
        edit_ev_col = x;
        ed_event_dir = d;
        undo_set(m->st, 1, 1);
    } else if (rc == 0)
        beep();
}

static void far undo_apply(struct MapEd far *m)                  /* 3d91:11b3 */
{
    union {
        int old[7];
        struct {
            int was;
            int other;
            int k;
        } t;
        struct Spot save[16];
        struct Pos pos;
    } u;
    int j;
    register int i;
    register int n;

    switch (m->st->undo_kind) {
    case 1:
        if (m->st->tool == 0) {
            if (area_overland()) {
                block_undo(m, ed_ev_row, edit_ev_col);
                undo_label(m);
                break;
            }
            for (i = 0; i <= 3; i++) {
                u.old[i] = m->st->paint[i];
                m->st->paint[i] = m->st->old[i];
            }
        } else if (m->st->tool == 3) {
            n = geo.area->cols * ed_ev_row + edit_ev_col;
            u.old[EVENT] = cell_event(n);
            cell_event_set(n, m->st->old[EVENT]);
            m->st->old[EVENT] = u.old[EVENT];
            if (area_overland())
                overland_cell_redraw(ed_ev_row, edit_ev_col, ed_rows, ed_cols, m->st->tool);
            else
                grid_cell_redraw(ed_ev_row, edit_ev_col, ed_rows, ed_cols, m->st->tool);
            panel_draw(m, 0);
            undo_label(m);
            edit_dirty_set();
            break;
        } else {
            i = m->st->tool + 3;
            u.old[i] = m->st->paint[i];
            m->st->paint[i] = m->st->old[i];
        }
        tool_paint(m, ed_ev_row, edit_ev_col, ed_event_dir, 1);
        if (m->st->tool == 0) {
            for (i = 0; i <= 3; i++)
                m->st->paint[i] = u.old[i];
        } else
            m->st->paint[i] = u.old[i];
        undo_label(m);
        break;
    case 2:
        u.t.was = m->st->paint[m->st->undo_slot];
        m->st->paint[m->st->undo_slot] = m->st->undo_paint[m->st->undo_slot];
        if (m->st->undo_slot == 0 || m->st->undo_slot == 2) {
            u.t.k = m->st->undo_slot == 0 ? 2 : 0;
            u.t.other = m->st->paint[u.t.k];
            m->st->paint[u.t.k] = m->st->undo_paint[u.t.k];
            m->st->paint[SIDE_B] = m->st->paint[SIDE_A];
            m->st->paint[KIND_B] = m->st->paint[KIND_A];
            m->st->undo_paint[u.t.k] = u.t.other;
        }
        m->st->undo_paint[m->st->undo_slot] = u.t.was;
        tool_info_draw(m, 0);
        undo_label(m);
        edit_dirty_set();
        break;
    case 3:
        if (m->st->undo_spot.which < 8) {
            j = m->st->undo_spot.which;
            m->st->undo_spot.which = 0;
            u.save[j] = geo.area->spots_wide[j];
            geo.area->spots_wide[j] = m->st->undo_spot;
            m->st->undo_spot = u.save[j];
            m->st->undo_spot.which = j;
        } else {
            for (j = 0; j < 8; j++) {
                u.save[j] = geo.area->spots_wide[j];
                geo.area->spots_wide[j] = m->st->undo_spots[j];
                m->st->undo_spots[j] = u.save[j];
            }
        }
        undo_label(m);
        edit_dirty_set();
        break;
    case 4:
        if (area_overland())
            overland_cell_redraw(m->st->pos.y, m->st->pos.x, ed_rows, ed_cols, m->st->tool);
        else
            grid_cell_redraw(m->st->pos.y, m->st->pos.x, ed_rows, ed_cols, m->st->tool);
        u.pos = m->st->pos;
        m->st->pos = m->st->undo_pos;
        m->st->undo_pos = u.pos;
        map_mark(m->st->pos.y, m->st->pos.x, m->st->pos.facing);
        undo_label(m);
        break;
    default:
        beep();
    }
}

static void far undo_label(struct MapEd far *m)                  /* 3d91:1740 */
{
    register int i;

    i = !str_cmp(edit_undo, undo_names[0]);
    str_copy(edit_undo, undo_names[i]);
    ui_mark(ed_item_base(m->st) + 4, 0x27, (char far *)edit_undo);
    dl_draw(0);
}

static void far undo_note(struct EdState far *e, int i, int v, char show)
                                                                 /* 3d91:17ad */
{
    e->undo_slot = i;
    e->undo_paint[i] = v;
    undo_set(e, 2, show);
}

static void far view_sync(struct MapEd far *m)                   /* 3d91:17d9 */
{
    if (m->st->view == 0) {
        geo.pos = m->st->pos;
        if (m->st->undo_kind == 1
            && (m->st->pos.y != ed_ev_row || m->st->pos.x != edit_ev_col
                || m->st->pos.facing != ed_event_dir))
            undo_set(m->st, 0, 1);
    } else {
        ed_ev_row = geo.pos.y;
        edit_ev_col = geo.pos.x;
        ed_event_dir = geo.pos.facing;
    }
}

void far items_mark(struct MapEd far *m)                         /* 3d91:186b */
{
    char direct;
    register int id;

    id = ed_item_base(m->st);
    direct = area_overland();
    if (m->st->view == 0) {
        ui_mark(id + 2, 0x18);
        if (m->pointer_mode)
            ui_mark(id + 5, 0x10);
        if (m->st->tool == 0) {
            ui_mark(id + 1, 0x18);
            ui_mark(id + 3, 0x18);
        } else {
            ui_mark(id + 1, 0x10);
            ui_mark(id + 3, 0x10);
        }
        map_items[8].flags = m->st->move ? 0 : 4;
    } else {
        ui_mark(id + 2, 0x18);
        if (m->pointer_mode)
            ui_mark(id + 5, 0x18);
        map_items[8].flags = (m->st->move ? 0 : 4) + 0x81;
        if (direct) {
            util_items[0].flags = 0x81;
            ui_mark(id + 1, 0x10);
            ui_mark(id + 3, 0x10);
        } else {
            util_items[0].flags = 0;
            if (m->st->tool == 0) {
                ui_mark(id + 1, 0x18);
                ui_mark(id + 3, 0x18);
            } else {
                ui_mark(id + 1, 0x10);
                ui_mark(id + 3, 0x10);
            }
        }
    }
    if (m->st->tool == 3) {
        ui_mark(id, 0x10);
        util_items[1].flags = 0x81;
        if (area_cell_at(m->st->pos.x, m->st->pos.y)) {
            ui_mark(id + 2, 0x27, edit_text);
            ui_mark(id + 2, 0x20, *edit_text);
            ui_mark(id + 2, 0x21, to_lower(*edit_text));
        } else {
            ui_mark(id + 2, 0x27, place_text);
            ui_mark(id + 2, 0x20, *place_text);
            ui_mark(id + 2, 0x21, to_lower(*place_text));
        }
    } else {
        ui_mark(id + 2, 0x27, place_text);
        ui_mark(id + 2, 0x20, *place_text);
        ui_mark(id + 2, 0x21, to_lower(*place_text));
        if (direct && m->st->tool != 2)
            util_items[1].flags = 0x81;
        else
            util_items[1].flags = 0;
        ui_mark(id, 0x18);
    }
    if (m->st->undo_kind == 0)
        ui_mark(ed_item_base(m->st) + 4, 0x10);
    else
        ui_mark(ed_item_base(m->st) + 4, 0x18);
    save_items_mark();
}

static char far undo_pending(struct MapEd far *m)                /* 3d91:1b9c */
{
    char done;

    done = 0;
    for (tick_slot = 0; tick_slot < 3; tick_slot++) {
        if (m->st->flags & 1) {
            switch (tick_slot) {
            case 0:
            case 1:
                undo_apply(m);
                panel_title_draw(m->st);
                panel_tool_draw(m);
                done = 1;
                if (tick_slot == 1)
                    undo_set(m->st, 0, 1);
            }
        }
        m->st->flags >>= 1;
    }
    return done;
}

static void far area_loop(struct EdState far *e)                 /* 3d91:1c36 */
{
    char direct;
    int k;
    int r;
    int c;
    char last_y;
    char last_x;
    char last_dir;
    struct MapEd m;
    register int key;

    last_y = (char)0x80;
    last_x = (char)0x80;
    last_dir = (char)0x80;
    m.st = e;
    ed_state = &m;
    ed_open(&m);
    ed_frame(m.st->view, 1);
    ed_layout(&m);
    map_scroll(&geo.pos.y, &geo.pos.x, &edit_y, &ed_x, ed_rows, ed_cols, m.st->view == 0);
    if (!undo_pending(&m))
        map_redraw(&m, 1);
    panel_draw(&m, 0);
    tool_info_draw(&m, 0);
    m.st->f6 = 0;
    while (m.st->f6 == 0) {
        direct = m.st->tool == 0 && !area_overland();
        if (!m.pointer_mode) {
            r = geo.pos.y * 8 + ed_ypos;
            c = geo.pos.x * 8 + ed_pix_x;
            switch ((geo.pos.facing & 6) >> 1) {
            case 0:
                r++;
                c += 4;
                break;
            case 1:
                c += 7;
                r += 4;
                break;
            case 2:
                r += 7;
                c += 4;
                break;
            case 3:
                c++;
                r += 4;
                break;
            }
        }
        while ((key = ui_poll()) < 0) {
            input_poll();
            if ((shift_key_down() == 0) ^ (m.erase == 0)) {
                m.erase = !m.erase;
                tool_info_draw(&m, 0);
            }
            if (m.st->view == 1) {
                if (m.pointer_mode)
                    mouse_pos(&r, &c);
                if (direct && (k = map_hit_wall(r, c)) >= -1) {
                    if (k >= 0)
                        geo.pos.facing = k;
                    if (last_dir == geo.pos.facing && last_y == geo.pos.y && last_x == geo.pos.x
                        && m.hover)
                        continue;
                    last_y = geo.pos.y;
                    last_x = geo.pos.x;
                    last_dir = geo.pos.facing;
                    m.hover = 1;
                    panel_draw(&m, 0);
                } else if (!direct && map_hit_cell(r, c) >= 0) {
                    if (last_y == geo.pos.y && last_x == geo.pos.x && m.hover)
                        continue;
                    last_y = geo.pos.y;
                    last_x = geo.pos.x;
                    m.hover = 1;
                    panel_draw(&m, 0);
                } else if (m.hover) {
                    m.hover = 0;
                    panel_draw(&m, 1);
                    shown_clear(&m);
                    if (m.st->tool == 3)
                        tool_info_draw(&m, 1);
                    last_y = last_x = last_dir = (char)0x80;
                }
            }
        }
        if ((k = menu_line_key(key)) >= 0)
            hotkey_do(&m, k);
        else {
            switch (key) {
            case 0:
                k = bar_take();
                menu_pick(&m, k >> 8, k & 0xff);
                break;
            case 1:
                key_command(&m, hook_key, 0);
                break;
            default:
                if (m.st->view == 0) {
                    k = 0;
                    switch (key) {
                    case 2:
                        k = 0x108;
                        break;
                    case 3:
                        k = 0x104;
                        break;
                    case 4:
                        k = 0x102;
                        break;
                    case 5:
                        k = 0x106;
                        break;
                    }
                    key_command(&m, k, 0);
                } else {
                    switch (key) {
                    case 2:
                        map_paint(&m);
                        break;
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        map_page(&m, (key - 3) * 2 + 0x102, 0);
                        break;
                    }
                }
            }
        }
        items_mark(&m);
    }
    dl_end();
}

static void far key_command(struct MapEd far *m, int key, void (far *fn)(struct EdState far *))
                                                                 /* 3d91:2036 */
{
    if (hook_key == 0x1b)
        file_pick(m, 0xa);
    else if (key >= 0x140 && key <= 0x159) {
        if (!bar_keys(m, key - 0xff, m->menus, m->nmenus, 0, 0))
            beep();
    } else
        marker_key(m, key, fn);
}

static void far menu_pick(struct MapEd far *m, unsigned char menu, unsigned char item)
                                                                 /* 3d91:20ab */
{
    switch (menu) {
    case 1:
        file_pick(m, item);
        break;
    case 2:
        map_pick(m, item);
        break;
    case 3:
        util_pick(m, item);
        break;
    default:
        beep();
    }
}

static void far file_pick(struct MapEd far *m, unsigned char item)
                                                                 /* 3d91:20fe */
{
    switch (item) {
    case 2:
        cur_area_save(m->st, cur_area_get());
        break;
    case 7:
        m->st->f6 = 1;
        m->st->screen = 3;
        break;
    case 8:
        m->st->f6 = 1;
        m->st->screen = 0x11;
        break;
    default:
        m->st->f6 = item;
        m->st->screen = 9;
    }
}

static void far map_pick(struct MapEd far *m, unsigned char item)
                                                                 /* 3d91:217f */
{
    int t;
    int d;
    int r;
    int c;
    register int old;

    switch (--item) {
    case 0:
    case 1:
        if (m->st->view != item) {
            old = m->st->view;
            m->st->view = item;
            screen_draw_begin(0);
            screen_flush_mode_set(1);
            ed_frame(m->st->view, 1);
            ed_window(1, m->st->view);
            view_sync(m);
            ed_items(m->st->view);
            map_items[old].flags = 0;
            map_items[item].flags = 4;
            items_mark(m);
            dl_draw(1);
            bar_draw();
            shown_clear(m);
            map_scroll(&geo.pos.y, &geo.pos.x, &edit_y, &ed_x, ed_rows, ed_cols, 1);
            if (item == 1 && m->pointer_mode) {
                mouse_pos(&r, &c);
                if ((d = map_hit_wall(r, c)) >= 0)
                    geo.pos.facing = d;
            }
            screen_flush_mode_set(0);
            screen_flush();
            if (m->st->undo_kind == 1 && m->st->tool == 0 || m->st->undo_kind == 4 && m->st->view == 0)
                undo_set(m->st, 0, 1);
            map_redraw(m, 1);
            tool_info_draw(m, 0);
            map_hover(m);
            panel_draw(m, 0);
            ed_mode_set(m->st->tool, m->st->view);
        } else
            beep();
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        if (m->st->tool != (t = item - 3)) {
            map_items[m->st->tool + 3].flags = 0;
            map_items[item].flags = 4;
            m->st->tool = t;
            shown_clear(m);
            undo_set(m->st, 0, 1);
            map_redraw(m, 1);
            tool_info_draw(m, 1);
            panel_draw(m, 1);
            ed_mode_set(m->st->tool, m->st->view);
        } else
            beep();
        break;
    case 8:
        if (m->st->view == 0) {
            m->st->move = !m->st->move;
            map_items[item].flags = m->st->move ? 0 : 4;
            panel_tool_draw(m);
        }
        break;
    default:
        beep();
    }
}

static void far util_pick(struct MapEd far *m, unsigned char item)
                                                                 /* 3d91:24dc */
{
    switch (--item) {
    case 0:
        m->st->f6 = 1;
        m->st->screen = 0xd;
        break;
    case 1:
        switch (m->st->tool) {
        case 0:
            m->st->f6 = 2;
            m->st->screen = 0xc;
            break;
        case 1:
            m->st->f6 = 2;
            m->st->screen = 8;
            break;
        case 2:
            m->st->f6 = 0x25;
            m->st->screen = 0xb;
            break;
        default:
            beep();
        }
        break;
    case 2:
        m->st->f6 = 1;
        m->st->screen = 0x13;
        break;
    case 4:
        m->st->f6 = 1;
        m->st->screen = 0xe;
        break;
    case 5:
        m->st->f6 = 3;
        m->st->screen = 0xb;
        geo.pos = m->st->pos;
        break;
    case 7:
        test_mode = 1;
        if (edit_dirty())
            test_mode++;
        geo.pos.facing &= 7;
        m->st->f6 = 1;
        m->st->screen = 1;
        break;
    default:
        beep();
    }
}

static void far hotkey_do(struct MapEd far *m, int key)          /* 3d91:2628 */
{
    struct Pos keep;
    register int v;

    if (menu_escaped) {
        if (key == 0x1b || key == 0x60)
            file_pick(m, 0xa);
        else
            beep();
        menu_escaped = 0;
    } else {
        switch (key) {
        case 0:
            switch (m->st->tool) {
            case 0:
                if (area_overland()) {
                    v = m->st->paint[KIND_A];
                    m->st->paint[KIND_B] = m->st->paint[KIND_A] = v ? 0 : 0xf;
                    undo_note(m->st, KIND_A, v, 1);
                    tool_info_draw(m, 0);
                } else {
                    m->st->f6 = 1;
                    m->st->screen = 0xc;
                }
                break;
            case 1:
                m->st->f6 = 1;
                m->st->screen = 8;
                break;
            case 2:
                m->st->f6 = 0x15;
                m->st->screen = 0xb;
                break;
            case 3:
                beep();
                break;
            }
            break;
        case 1:
        case 3:
            if (area_overland() || m->st->tool != 0)
                beep();
            else {
                if (m->st->view == 1) {
                    keep = geo.pos;
                    geo.pos = m->st->pos;
                }
                if (key == 1)
                    v = dir_left(geo.pos.facing);
                else
                    v = dir_right(geo.pos.facing);
                paint_at(m, geo.pos.y, geo.pos.x, v, 1);
                if (m->st->view == 1)
                    geo.pos = keep;
            }
            break;
        case 2:
            paint_here(m);
            break;
        case 4:
            if (m->st->undo_kind == 0) {
                m->st->f6 = 9;
                m->st->screen = 0xa;
            } else {
                undo_apply(m);
                panel_title_draw(m->st);
                panel_tool_draw(m);
            }
            break;
        case 5:
            if (m->st->view == 1) {
                if (m->hover) {
                    if (area_overland()
                        && cell_closed(geo.area->cols * geo.pos.y + geo.pos.x))
                        beep();
                    else
                        marker_place(m->st);
                } else {
                    m->st->f6 = 1;
                    m->st->screen = 0xf;
                }
            }
            break;
        }
    }
}

static void far paint_here(struct MapEd far *m)                  /* 3d91:28e9 */
{
    struct Pos keep;

    if (m->st->view == 1) {
        keep = geo.pos;
        geo.pos = m->st->pos;
    }
    paint_at(m, geo.pos.y, geo.pos.x, geo.pos.facing, 1);
    if (m->st->view == 1)
        geo.pos = keep;
    if (m->st->f6 && m->st->screen == 5)
        m->st->f6 |= 0x8000;
}

void far area_start(struct EdState far *e, int a)                /* 3d91:298e */
{
    ed_mode_get(&e->tool, &e->view);
    if ((a & 0x3f) <= 4) {
        if (e->tool == 1) {
            e->tool = 0;
            ed_mode_set(e->tool, e->view);
        }
        e->view = 1;
    }
    area_open(e->view, a);
    ed_window(1, e->view);
    edit_flag_clear();
    undo_set(e, 0, 0);
    e->pos.y = geo.area->spots_wide[0].y;
    e->pos.x = geo.area->spots_wide[0].x;
    e->pos.facing = geo.area->spots_wide[0].facing;
    geo.pos = e->pos;
}

static void far cur_area_save(struct EdState far *e, int n)        /* 3d91:2a62 */
{
    area_save(e->view, n);
}

static char far bar_keys(struct MapEd far *m, int key, struct Menu far * far *menus,
                         unsigned char n, int first, struct View far *view)
                                                                 /* 3d91:2a7b */
{
    char rc;
    unsigned char i;
    unsigned char shown;
    int item;
    int prev;
    struct Menu far *menu;

    rc = 0;
    for (i = 0; key && i < n; i++)
        if (*menus[i]->title == key)
            key = 0;
    if (key)
        return 0;
    item = first;
    pointer_glyph_load(0, 1);
    prev = shown = 0;
    rc = 0;
    do {
        if ((menu = menus[i - 1]) == 0)
            break;
        if (item == 0) {
            for (item = 1; menu->count >= item && ((menu->items + item - 1)->flags & 0x83); item++)
                ;
            if (menu->count < item) {
                beep();
                break;
            }
        }
        if (i != shown) {
            screen_page_save();
            screen_flush_mode_set(1);
            pick_open(menu, i, view);
            shown = i;
        }
        if (item != prev) {
            if (prev)
                pick_hilite(menu, &prev, 0, view);
            pick_hilite(menu, &item, &prev, view);
        }
        while (!key_poll() && !mouse_pending())
            ;
        if (key_poll()) {
            rc = pick_step(menu, &i, &item, n, view);
            if (rc > 0 && item != prev) {
                if (prev)
                    pick_hilite(menu, &prev, 0, view);
                pick_hilite(menu, &item, &prev, view);
                wait_ticks(1);
            } else if (rc == 0 && i != shown) {
                prev = item = 0;
                screen_flush_mode_set(0);
                screen_page_show();
            }
        } else if (mouse_pending()) {
            if (prev)
                pick_hilite(menu, &prev, 0, view);
            if ((rc = pick_mouse(menu, &i, &item, view)) != 0)
                shown = 0;
        }
    } while (menu && !rc);
    if (shown) {
        screen_flush_mode_set(0);
        screen_page_show();
    }
    if (rc > 0)
        menu_pick(m, i, item);
    return int_max(rc, 0);
}

static char far pick_step(struct Menu far *menu, unsigned char far *pi, int far *pitem,
                          unsigned char n, struct View far *view)
                                                                 /* 3d91:2d37 */
{
    char far *s;
    unsigned char j;
    char found;
    register int key;

    found = 0;
    key = key_get();
    if (key <= 0xff && (key < 'a' || (key -= 0x20) != 0) && key > 0 && key != '-')
        for (j = 0; !found && menu->count > j; j++) {
            if (!(menu->items[j].flags & 0x81)) {
                for (s = menu->items[j].text; *s && *s != key; s++)
                    ;
                if (*s == key) {
                    found = 1;
                    *pitem = j + 1;
                }
            }
        }
    if (!found) {
        switch (key) {
        case 0x102:
        case 0x106:
            if (key == 0x102)
                *pi = *pi % n + 1;
            else
                *pi = (*pi + n - 2) % n + 1;
            break;
        case 0x104:
        case 0x108:
            if (view) {
                if (key == 0x108 && *pitem == 1 && view->above)
                    break;
                if (key == 0x104 && menu->count - int_max(view->below, 0) == *pitem
                    && view->below)
                    break;
            }
            do {
                if (key == 0x104)
                    *pitem = *pitem % menu->count + 1;
                else
                    *pitem = (menu->count + *pitem - 2) % menu->count + 1;
            } while ((menu->items + *pitem - 1)->flags & 0x83);
            break;
        case 0x11:
        case 0x1b:
            found = -1;
            break;
        case 0xd:
            if (*pitem) {
                found = 1;
                break;
            }
        default:
            beep();
        }
    }
    return found;
}

static char far pick_mouse(struct Menu far *menu, unsigned char far *pi, int far *pitem,
                           struct View far *view)                /* 3d91:2f66 */
{
    int x;
    int y;
    register int r;

    if (mouse_press(7, &x, &y)) {
        if ((r = pop_run(menu, view ? 0x70 : 0x8f, view)) < 0) {
            mouse_pos(&x, &y);
            screen_flush_mode_set(0);
            page_swap();
            if ((r = bar_track(x, y)) != 0) {
                *pi = r >> 8 & 0xf;
                *pitem = r & 0xf;
            }
        } else {
            if (r > 0)
                *pitem = r;
            screen_flush_mode_set(0);
            screen_page_show();
        }
        return r ? 1 : -1;
    }
    return 0;
}

static void far pick_open(struct Menu far *menu, int n, struct View far *view)
                                                                 /* 3d91:3031 */
{
    register int col;

    pop_width(menu);
    if (menu->flags & 4) {
        col = bar_query(n, -1, 0x28);
        menu->col = int_min(col, 0x1fe0 - menu_box_width(menu) * 4);
    }
    if (view)
        pop_draw(menu, 0x70, view);
    else
        menu_box_draw(menu);
}

static void far pick_hilite(struct Menu far *menu, int far *pitem, int far *pprev,
                            struct View far *view)               /* 3d91:30b9 */
{
    int row;
    int r;
    int c;
    int off;
    int step;
    int k;
    int dir;
    int last;
    register int i;
    register int idx;

    if (pitem == 0)
        return;
    if ((idx = *pitem - 1) < 0)
        return;
    if (menu->count <= idx)
        return;
    step = menu->flags & 0x10 ? 5 : 4;
    for (i = 0, k = 0; i < idx && menu->count > i; i++)
        if (!(menu->items[i].flags & 2))
            k++;
    if (view)
        k -= view->above;
    row = menu->row + k * step;
    off = 0;
    dir = 0;
    if (view && menu->row == row && view->above > 0)
        dir = -step;
    else if (view && view->bottom - step == row && view->below > 0)
        dir = step;
    if (dir && pprev) {
        last = -1;
        row -= menu->row;
        while (!key_poll() && !mouse_pending() && last == -1) {
            i = pop_scroll(menu, view, dir, 0x70, off, row, &last, *pitem);
            if (last == -1) {
                do
                    idx += dir / __abs__(dir);
                while (menu->items[idx].flags & 2);
                wait_or_key(1);
                if (idx >= 0 && menu->count > idx)
                    *pitem = idx + 1;
                else {
                    i = 0;
                    break;
                }
            }
        }
        if (last != -1)
            *pitem = i;
    } else {
        last = 0;
        text_xy(row, menu->col + off, &r, &c);
        if (menu->flags & 0x10) {
            if (screen_rows() >= 0x12c)
                r++;
            r++;
        }
        fill_rect(r, c, row + step, menu->col + menu->width * 4 - off,
                     pop_hilite(view ? 0x70 : 0x8f));
    }
    if (pprev)
        *pprev = last == -1 ? 0 : *pitem;
}
