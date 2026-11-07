/* CKIT.EXE overlay section 23, module 2e3e: four of the area editor's
 * screens, each run in the map view.  spot_pick polls the dialog items while
 * the pointer moves over the map, and returns 1 for the menu's first hotkey
 * (unless marker is set) or Enter (where done_ok is 0), 2 for a click on the
 * map that spot_hit places and the ok callback accepts (where done_ok is
 * set), else 0.
 *
 * access_screen is screen 13 of editloop.c's loop.  access_count floods the
 * area from the party marker three times (access_fill, recursing at most
 * 0x8c deep through fill_rec's record), passing walls whose kind (mapedit.c's
 * cell_kind) is at most 0xd, 5 and 1 and marking the squares reached 0xd,
 * 0xe and 0xf; it then shows the three counts, the total and "out of"
 * rows * cols, and the map shows the marks (access_draw) while the screen
 * runs.  marker_screen (screen 15) picks a square and wall for the party
 * marker: a pick keeps the old marker in EdState.undo_pos, moves EdState.pos
 * to geo.pos and sets env->bits' bit 0, else geo.pos is put back.
 * entry_screen (screen 14) lists the area's eight entry points
 * (spots[0..7]), blinks their numbers on the map (entry_blink); called
 * back from screen 11 with EdState.flags bit 2 clear it saves the eight in
 * EdState.undo_spots (entry from screen 2 clears bit 2 and saves nothing,
 * so Ok without a trip to screen 11 compares against stale entries, as
 * shipped).  Ok sets bit 0
 * when one changed; a click on the map returns screen 11 with bits 0 and 1
 * set; a 0 puts the saved eight back.  Called back from screen 11 with
 * env->bits' low nibble set, it stores geo.pos into the entry point bits
 * 4..11 name.  replace_screen (screen 20) counts what holds the value in
 * env->bits' low byte for the current tool (for walls each of a square's
 * four wall bytes, up to 4 a square; squares for backdrops and zones) (a wall's side and kind,
 * a backdrop or a zone), shows it beside the current setting (replace_ask,
 * replace_show) and, on Replace, puts the setting in every one, setting bit
 * 0 when any changed.  Before its pick each clears env->bits' low nibble
 * (access_screen) or low word (the others); access_screen, marker_screen
 * and replace_screen return the screen they were called with, entry_screen
 * 2 or 11.
 *
 * Section 23 (parent section 20, which is empty and has no parent; no
 * section names 23 as its parent): payload at EXE 0x078790, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 23.  The module is stored frame 2e3e (load-based 3e3e), EXE
 * 0x079268..0x07b06d (7,685 bytes, 25 routines), from 2e3e:0008, the byte
 * after modclear.c's last retf at 2e1a:0247 (2e3e:0007), to its own last
 * retf at 2e3e:1e0c; three zero bytes follow to the section's end
 * (0x07b070).  One Turbo C large-model _TEXT segment, built with Turbo C
 * 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the far
 * pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 23's relocation table files 174 sites under 2e3e, all in this
 * range (EXE 0x079293..0x07b064): 172 far calls (63 of them through vector
 * stubs) and the two segment words of the stub callbacks 18de:1e45 and
 * 18de:1d55 that spot_items hands to ui_add.  Four of the section's nine
 * vectors land here, each far-called from section 10 (editloop.c):
 * 18de:1c01 -> access_screen, 18de:1bf7 -> entry_screen, 18de:1bed ->
 * marker_screen, 18de:1c83 -> replace_screen.  No relocated word in any
 * section stores frame 2e3e (control: 29 store 2d91).  Nineteen routines
 * are near-called, have their address pushed after push cs (062f, 0f3b),
 * or stored with cs (1d7b, mov [bp+12],cs), ahead of their definitions, so they are static; entry_mark and
 * entry_list are reached only after their definitions, so for these two the
 * bytes do not decide static or public (static here).  The module calls the
 * resident code directly and sections 13, 16 and 18 only through vector
 * stubs.
 *
 * FRUA's own code: the sharing map pairs all 25 routines a-only, so every
 * name here is chosen from the code.
 *
 * _DATA is DGROUP 9170..91d3: 15 literals stored as 12 (-d puts "" on the
 * NUL of "%s:", "%s" on the tail of "%s: %d %s%s" and "%s %s" on the tail
 * of "%s %s %s").  Below it 9128..916e is areafile.c's and 916f a pad byte
 * no instruction references; 91d4 ("art") is referenced only by section 24.
 * _BSS is fill_rec, the far pointer at d528..d52b; evchain.c's _BSS ends at
 * d527; d52c is referenced once, from section 24 (no instruction
 * references d52d).  fill_rec is set from scratch_buf (which labels.c declares
 * extern and no module defines), the buffer data_load fills.
 *
 * Forced by the bytes, each against the whole module's 25 ranges: the
 * comma-chained initializations in access_count's label loop and the three
 * replace loops (a chained = copies through ax: 0, 14 and 15 of 25 for
 * access_count, replace_walls and replace_back); entry_screen's switch on
 * k with one case and a default (an if/else is 3 bytes shorter, 10 of 25);
 * spot_hit's old-style definition behind an unprototyped declaration, so
 * the caller widens wall with cbw and the callee reads a byte (a prototype
 * with char bakes 22 of 25, with int 24); EdEnv.bits signed (unsigned
 * calls LXURSH@, which no sym names); EdState.view, EdState.tool, Spot.x
 * and Spot.y, and Fill.kind unsigned char (as char: 0, 11, 9 and 1 of 25);
 * access_draw's marks plain char (unsigned drops a cbw: 4 of 25); and
 * cell_open static (public, marker_screen's push of its address becomes a
 * fixup: 5 of 25).
 *
 * Not decided by the bytes: count ? 1 : 0 or count != 0 in replace_ask,
 * char or unsigned char for access_count's row and col and Fill.mark,
 * short or int for entry_blink's i, and static or public for entry_mark and
 * entry_list; each bakes 25 of 25.
 *
 * Shipped as written: marker_screen passes spot_pick a direct flag that is
 * always 1 and replace_walls tests a both flag that is always 1.  spot_pick
 * passes MapEd.hover to 18de:1e13 after edscreen.c's ed_open (18de:1f71)
 * has set it from !f5.
 */

struct Entry {                  /* one menu entry, 8 bytes, as menubox.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6                                     */
    char flags;                 /* +7                                     */
};

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
    unsigned char which;        /* +3                                     */
};

struct MapCell {                /* view3d.c's square of the area, 6 bytes */
    unsigned char wall[4];      /* +0   by dir / 2: kind high, side low   */
    unsigned char event;        /* +4   not read here                     */
    unsigned char bits;         /* +5   backdrop 0..1, zone 2..4          */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[0xa];            /* +4   not read here                     */
    struct Spot spots_wide[26]; /* +e   the entry points are the first 8  */
    char pad_76[0xac];          /* +76  not read here                     */
    struct MapCell cells[0x240]; /* +122                                  */
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
    int screen;                 /* +0   not read here                     */
    int prev;                   /* +2   not read here                     */
    unsigned char view;         /* +4   0 the 3D view, 1 the map          */
    unsigned char tool;         /* +5   0 wall 1 backdrop 2 zone 3 event  */
    int f6;                     /* +6   not read here                     */
    char f8;                    /* +8   not read here                     */
    char move;                  /* +9   not read here                     */
    unsigned char paint[7];     /* +a   by SIDE_A .. EVENT                */
    char flags;                 /* +11  bit 2: the entry points are saved */
    unsigned char undo_kind;    /* +12  not read here                     */
    char both;                  /* +13  not read here                     */
    unsigned char undo_slot;    /* +14  not read here                     */
    unsigned char old[7];       /* +15  not read here                     */
    unsigned char undo_paint[7]; /* +1c not read here                     */
    struct Spot undo_spot;      /* +23                                    */
    struct Pos undo_pos;        /* +27  the marker before                 */
    struct Pos pos;             /* +2c  the party marker in the map view  */
    struct Spot undo_spots[8];  /* +31  the entry points before           */
};

struct MapEd {                  /* the map editor, mapedit.c's            */
    struct EdState far *st;     /* +0                                     */
    char erase;                 /* +4                                     */
    char pointer_mode;          /* +5                                     */
    int button;                 /* +6                                     */
    struct Menu far *menus[5];  /* +8                                     */
    char nmenus;                /* +1c                                    */
    char hover;                 /* +1d  the mouse is over the map         */
    unsigned char shown[8];     /* +1e                                    */
};

struct Fill {                   /* the flood fill's work record           */
    int at;                     /* +0   the neighbour being entered       */
    int count;                  /* +2   rows * cols                       */
    unsigned char kind;         /* +4   the highest wall kind passed      */
    char mark;                  /* +5   the value this pass marks with    */
    char seen[0x240];           /* +6   by square                         */
    int depth;                  /* +246 the recursion depth               */
};

enum { SIDE_A, SIDE_B, KIND_A, KIND_B, BACK, ZONE, EVENT };

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern int  far menu_line_key(int key);                              /* 083a:1e2d */
extern void far menu_line_show(char far *title, char far *text, char fkeys); /* 083a:1e9a */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far wait_ticks(int n);                               /* 0da7:0151 */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern char far shift_held(void);                                /* 107e:0299 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_poll(void);                                   /* 15e5:15e6 */
extern void far dl_end(void);                               /* 15e5:187a */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far map_mark(int y, int x, unsigned char dir);
                                /* 18de:1ce7, vector to overlay 13 26e3:04b3 */
extern char far map_scroll(char far *y, char far *x, int far *top, int far *left,
                             int rows, int cols, char center);
                                /* 18de:1cfb, vector to overlay 13 26e3:0158 */
extern void far map_window_draw(struct EdState far *e, char direct);
                                /* 18de:1d0f, vector to overlay 16 29f4:1074 */
extern void far map_cell_xy(int y, int x, unsigned char rows, unsigned char cols,
                             int far *pr, int far *pc);
                                /* 18de:1d2d, vector to overlay 16 29f4:2a0d */
extern void far wall_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
                                /* 18de:1d37, vector to overlay 16 29f4:1f97 */
extern void far marker_key(struct MapEd far *m, int key, void (far *fn)(struct EdState far *));
                                /* 18de:1d41, vector to overlay 16 29f4:1464 */
extern void far zone_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
                                /* 18de:1d4b, vector to overlay 16 29f4:259e */
extern void far map_button_cb(struct MapEd far *m, int key, int v);
                                /* 18de:1d55, vector to overlay 16 29f4:144b */
extern char far cell_closed(int n);
                                /* 18de:1d7d, vector to overlay 16 29f4:35f5 */
extern void far panel_draw(struct MapEd far *m, char full);
                                /* 18de:1d87, vector to overlay 16 29f4:1a71 */
extern char far cell_back_set(int n, int v);
                                /* 18de:1d9b, vector to overlay 16 29f4:01cb */
extern char far cell_kind_set(int n, int dir, int v);
                                /* 18de:1da5, vector to overlay 16 29f4:0148 */
extern int  far cell_back(int n);
                                /* 18de:1db9, vector to overlay 16 29f4:01a3 */
extern int  far cell_kind(int n, int dir);
                                /* 18de:1dc3, vector to overlay 16 29f4:00ff */
extern char far map_hit_cell(int r, int c);
                                /* 18de:1dd7, vector to overlay 16 29f4:0478 */
extern int  far map_hit_wall(int r, int c);
                                /* 18de:1de1, vector to overlay 16 29f4:03a3 */
extern char far cell_zone_set(int n, int v);
                                /* 18de:1deb, vector to overlay 16 29f4:026a */
extern char far cell_side_set(int n, int dir, int v);
                                /* 18de:1df5, vector to overlay 16 29f4:00a8 */
extern int  far cell_zone(int n);
                                /* 18de:1dff, vector to overlay 16 29f4:023e */
extern int  far cell_side(int n, int dir);
                                /* 18de:1e09, vector to overlay 16 29f4:0063 */
extern void far panel_pos_draw(struct EdState far *e, int r, int c, char full);
                                /* 18de:1e13, vector to overlay 16 29f4:1bf1 */
extern void far map_page(struct MapEd far *m, int key, void (far *fn)(struct EdState far *));
                                /* 18de:1e27, vector to overlay 16 29f4:1296 */
extern int  far map_key_hook(int a, int key);
                                /* 18de:1e45, vector to overlay 16 29f4:13f5 */
extern void far panel_title_draw(struct EdState far *e);
                                /* 18de:1e4f, vector to overlay 16 29f4:1840 */
extern char far area_overland(void);
                                /* 18de:1e63, vector to overlay 16 29f4:378b */
extern void far back_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
                                /* 18de:1e77, vector to overlay 16 29f4:2309 */
extern void far ed_frame(int view, char menu);
                                /* 18de:1f3f, vector to overlay 18 2cf5:0165 */
extern void far ed_window(char on, int view);
                                /* 18de:1f53, vector to overlay 18 2cf5:066f */
extern void far ed_open(struct MapEd far *m);
                                /* 18de:1f71, vector to overlay 18 2cf5:00fe */
extern void far ed_mode_get(unsigned char far *tool, unsigned char far *view);
                                /* 18de:1f85, vector to overlay 18 2cf5:070a */
extern void far shown_clear(struct MapEd far *m);
                                /* 18de:1f8f, vector to overlay 18 2cf5:0139 */

extern struct Entry util_items[];                            /* 00f4  the Utilities menu   */
#define replace_all_text (util_items[1].text)                 /* "Replace globally"    */
extern char far *col_row_fmt;                          /* 0148  "Col %2d Row %2d" */
extern char far *tool_names[];                        /* 0154  "Wall", ...       */
extern char far *access_text;                          /* 0204  "Access"          */
extern char far *entry_point_text;                          /* 0208  "Entry Point"     */
extern char far *cell_text;                          /* 020c  "cell"            */
extern char far *out_of_text;                          /* 0210  "out of"          */
extern char far *total_text;                          /* 0214  "Total"           */
extern char far *access_names[];                        /* 021c  "Accessible", ... */
extern char far *compass_names[];                        /* 02e0  "North", ...      */
extern char far *facing_text;                          /* 0348  "Facing"          */
extern char far *wall_text;                          /* 034c  "Wall"            */
extern char far *place_text;                          /* 0358  "Place"           */
extern char far *replace_text;                          /* 035c  "Replace"         */
extern char far *an_text;                          /* 0368  "an"              */
extern char far *at_text;                          /* 0370  "at"              */
extern char far *to_text;                          /* 0374  "to"              */
extern char far *with_text;                          /* 0380  "with"            */
extern char far *this_text;                          /* 038c  "this"            */
extern char far *self_replace_text;                          /* 039c  "An item cannot replace itself!" */
extern char far *none_to_replace_text;                          /* 03a0  "Nothing was found to replace!" */
extern char far *ok_text;                          /* 03c4  "Ok"              */
extern char far *cancel_text;                          /* 03c8  "Cancel"          */
extern char far *mark_text;                          /* 03fc  "Mark"            */
extern char far *current_text;                          /* 0450  "Current"         */
extern char far *mark_cell_text;                          /* 0498  "Mark cell wall or" */
extern char far *continue_items;                                 /* 390c */
extern char far *empty_text;                                     /* 3bc0 */
extern int map_row;                                              /* 889d */
extern int map_col;                                              /* 889f */
extern int map_top;                                              /* 88a1 */
extern int map_left;                                             /* 88a3 */
extern unsigned char map_rows;                                   /* 88a5 */
extern unsigned char map_cols;                                   /* 88a6 */
extern int hook_key;                                            /* b384 */
extern unsigned char far *scratch_buf;                              /* c3c8 */
extern char menu_escaped;                                        /* ca2a */
extern struct Geo geo;                                           /* d414 */
extern int ed_x;                                                 /* d42a */
extern struct MapEd far *ed_state;                               /* d42c */
extern int edit_y;                                               /* d430 */
extern int ed_ypos;                                              /* d432 */
extern unsigned char ed_rows;                                    /* d434 */
extern unsigned char ed_cols;                                    /* d435 */
extern int ed_pix_x;                                             /* d43c */
extern int edit_width;                                           /* d43e */
extern int edit_height;                                          /* d442 */

static struct Fill far *fill_rec;                                /* d528 */

static void far access_count(struct Pos far *p);
static void far access_fill(unsigned char col, int n);
static void far access_mark(struct EdState far *e);
static void far access_redraw(void);
static void far access_draw(char far *seen);
static void far marker_status(struct EdState far *e);
static void far entry_blink(void);
static void far entry_number(int y, int x, unsigned char rows, unsigned char cols,
                             unsigned char i, char on);
static char far cell_open(struct EdState far *e);
static int  far replace_count(struct EdState far *e, unsigned char v);
static char far replace_ask(struct EdState far *e, int count, unsigned char from,
                            unsigned char to);
static void far replace_show(struct EdState far *e, int r, int c, unsigned char v);
static int  far replace_walls(struct EdState far *e, unsigned char side, unsigned char kind);
static int  far replace_back(struct EdState far *e, unsigned char v);
static int  far replace_zone(struct EdState far *e, unsigned char v);
static void far spot_items(int view, char esc);
static char far spot_pick(struct EdState far *e, char wall, char done_ok, char marker,
                          void (far *draw)(struct EdState far *),
                          char (far *ok)(struct EdState far *));
static void far spot_draw(struct EdState far *e);
static char far spot_hit();

int far access_screen(int prev, struct EdEnv far *env, void far *arg)
                                                                 /* 3e3e:0008 */
{
    char direct;
    struct EdState far *e;

    if (arg) {
        e = arg;
        if (e->view == 0)
            ed_window(1, 1);
        e->view = 1;
        direct = e->tool == 0 && !area_overland();
        spot_items(e->view, 0);
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        ed_frame(e->view, 0);
        menu_line_show(empty_text, continue_items, 0);
        dl_draw(1);
        geo.pos = e->pos;
        map_scroll(&geo.pos.y, &geo.pos.x, &edit_y, &ed_x, ed_rows, ed_cols, 0);
        fill_rec = (struct Fill far *)scratch_buf;
        access_count(&e->pos);
        screen_flush_mode_set(0);
        screen_flush();
        env->bits &= 0xfffffff0L;
        if (spot_pick(e, direct, 0, 0, access_mark, 0))
            env->bits |= 1;
        dl_end();
        geo.pos = e->pos;
        if (!area_overland())
            ed_mode_get(0, &e->view);
        if (e->view == 0)
            ed_window(1, 0);
    }
    return prev;
}

static void far access_count(struct Pos far *p)                  /* 3e3e:01cc */
{
    int i;
    int y;
    unsigned char row;
    unsigned char col;
    char kinds[3];
    int n;
    int counts[3];
    int x;
    int k;
    int total;

    mem_fill(counts, 6, 0);
    kinds[0] = 1;
    kinds[1] = 5;
    kinds[2] = 0xd;
    n = p->y * geo.area->cols + p->x;
    fill_rec->count = geo.area->rows * geo.area->cols;
    fill_rec->depth = 0;
    mem_fill(fill_rec->seen, fill_rec->count, 0);
    for (i = 2; i >= 0; i--) {
        fill_rec->kind = kinds[i];
        fill_rec->mark = 0xf - i;
        access_fill(p->x, n);
    }
    for (row = n = total = 0; row < geo.area->rows; row++)
        for (col = 0; col < geo.area->cols; col++) {
            if (fill_rec->seen[n]) {
                counts[0xf - (unsigned char)fill_rec->seen[n]]++;
                total++;
            }
            n++;
        }
    y = 0x1f60;
    x = 0x1f9c;
    fill_rect(y, x, y + 0x36, x + 0x40, 8);
    text_print_at(y + 4, x + 0xc, 0x8b, "%s:", access_text);
    y += 4;
    for (i = 0, k = i; i < 3; i++) {
        fill_rect(y + 4, x, y + 8, x + 4, 0xf - i);
        text_print_at(y + 4, x + 6, 0x87, access_names[k++]);
        if (i)
            text_print_at(y + 8, x + 6, 0x87, access_names[k++]);
        text_print_at((i ? 0xc : 8) + y, x + 6, 0x87, "%d %s%s.", counts[i], cell_text,
                      counts[i] == 1 ? "" : "s");
        y += i ? 0xe : 0xa;
    }
    text_print_at(y + 4, x, 0x87, "%s: %d %s%s", total_text, total, cell_text,
                  total == 1 ? "" : "s");
    text_print_at(y + 8, x, 0x87, "%s %d.", out_of_text, fill_rec->count);
}

static void far access_fill(unsigned char col, int n)            /* 3e3e:049c */
{
    if (fill_rec->seen[n] != fill_rec->mark) {
        fill_rec->seen[n] = fill_rec->mark;
        if (++fill_rec->depth < 0x8c) {
            if (cell_kind(n, 8) <= fill_rec->kind) {
                if ((fill_rec->at = n - geo.area->cols) < 0)
                    fill_rec->at += fill_rec->count;
                access_fill(col, fill_rec->at);
            }
            if (cell_kind(n, 4) <= fill_rec->kind) {
                if ((fill_rec->at = geo.area->cols + n) >= fill_rec->count)
                    fill_rec->at = col;
                access_fill(col, fill_rec->at);
            }
            if (cell_kind(n, 6) <= fill_rec->kind) {
                if ((fill_rec->at = col - 1) < 0)
                    fill_rec->at += geo.area->cols;
                access_fill(fill_rec->at, n - (col - fill_rec->at));
            }
            if (cell_kind(n, 2) <= fill_rec->kind) {
                if ((fill_rec->at = col + 1) >= geo.area->cols)
                    fill_rec->at -= geo.area->cols;
                access_fill(fill_rec->at, fill_rec->at - col + n);
            }
        }
        fill_rec->depth--;
    }
}

static void far access_mark(struct EdState far *e)               /* 3e3e:062f */
{
    map_window_draw(e, 0);
    access_redraw();
    map_mark(e->pos.y, e->pos.x, e->pos.facing);
}

static void far access_redraw(void)                              /* 3e3e:0663 */
{
    access_draw(fill_rec->seen);
}

static void far access_draw(char far *seen)                      /* 3e3e:0676 */
{
    short r_end;
    short c_end;
    short k;
    int r;
    int c;

    if (seen) {
        r_end = map_rows * 8 + map_row;
        c_end = map_cols * 8 + map_col;
        k = geo.area->cols * map_top + map_left;
        for (r = map_row; r < r_end; r += 8) {
            for (c = map_col; c < c_end; c += 8) {
                if (seen[k])
                    fill_rect(r, c, r + 8, c + 8, seen[k]);
                k++;
            }
            k += geo.area->cols - map_cols;
        }
    }
}

int far marker_screen(int prev, struct EdEnv far *env, void far *arg)
                                                                 /* 3e3e:0729 */
{
    char menu[0x15];
    char direct;
    struct EdState far *e;

    if (arg) {
        e = arg;
        direct = 1;
        spot_items(e->view, 0);
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        ed_frame(e->view, 0);
        str_format(menu, "%s", cancel_text);
        hotkey_reset(0);
        menu_line_show(mark_cell_text, menu, 0);
        dl_draw(1);
        marker_status(e);
        screen_flush_mode_set(0);
        screen_flush();
        env->bits &= 0xffff0000L;
        if (spot_pick(e, direct, 1, 1, 0, cell_open)) {
            e->undo_pos = e->pos;
            e->pos = geo.pos;
            env->bits |= 1;
        } else
            geo.pos = e->pos;
        dl_end();
    }
    return prev;
}

static void far marker_status(struct EdState far *e)             /* 3e3e:087e */
{
    int r;
    int c;

    r = 0x1f7a;
    c = 0x1f9c;
    fill_rect(r + 4, c, r + 0x1a, c + 0x40, 8);
    text_print_at(r + 4, c, 0x8b, "%s %s %s", mark_text, cell_text, wall_text);
    text_print_at(r + 0xc, c, 0x87, "%s %s %s", current_text, mark_text, at_text);
    text_print_at(r + 0x10, c, 0x87, col_row_fmt, e->pos.x, e->pos.y);
    text_print_at(r + 0x14, c, 0x87, "%s %5s", facing_text, compass_names[(e->pos.facing & 6) >> 1]);
}

static void far entry_mark(struct EdState far *e)                /* 3e3e:0973 */
{
    map_window_draw(e, 0);
    entry_blink();
}

static void far entry_blink(void)                                /* 3e3e:098c */
{
    unsigned char y;
    unsigned char x;
    short i;
    int n;

    pointer_hide();
    for (n = 0; n < 5; n++) {
        for (i = 7; i >= 0; i--) {
            y = geo.area->spots_wide[i].y;
            x = geo.area->spots_wide[i].x;
            entry_number(y, x, ed_rows, ed_cols, i, n % 2);
        }
        wait_ticks(2);
    }
    pointer_show();
}

static void far entry_number(int y, int x, unsigned char rows, unsigned char cols,
                             unsigned char i, char on)           /* 3e3e:0a15 */
{
    int pr;
    int pc;
    int r;
    int c;

    map_cell_xy(y, x, rows, cols, &pr, &pc);
    if (pr >= 0 && pc >= 0) {
        r = pr + 8;
        c = pc + 8;
        text_xy_to(0x1f40, 0x1f40, pr, pc, &r, &c);
        text_print_at(r + 0x1f40, c + 0x1f40, on ? 0xf : 0xf0, "%d", i + 1);
    }
}

static void far entry_list(void)                                 /* 3e3e:0aa9 */
{
    short i;
    int r;
    int c;

    r = 0x1f60;
    c = 0x1f9c;
    fill_rect(r, c, r + 0x38, c + 0x40, 8);
    text_print_at(r + 2, c, 0x8b, "%*s", 0x10, msg_format("%s %s", place_text, an_text));
    text_print_at(r + 6, c, 0x8b, "%s", entry_point_text);
    r += 0xa;
    for (i = 0; i < 8; i++) {
        text_print_at(r, c, 0xf0, "%d", i + 1);
        text_print_at(r, c + 6, 0x87, "(%2d, %2d) %5s", geo.area->spots_wide[i].x,
                      geo.area->spots_wide[i].y,
                      area_overland() ? "" : compass_names[(geo.area->spots_wide[i].facing & 6) >> 1]);
        r += 6;
    }
}

int far entry_screen(int prev, struct EdEnv far *env, void far *arg)
                                                                 /* 3e3e:0bdc */
{
    char k;
    char menu[0x1a];
    struct EdState far *e;
    int i;

    if (arg == 0)
        return prev;
    e = arg;
    switch (prev) {
    case 2:
        e->flags &= ~4;
        if (e->view == 0)
            ed_window(1, 1);
        e->view = 1;
        break;
    case 0xb:
        prev = 2;
        if (!(e->flags & 4)) {
            e->flags |= 4;
            for (i = 0; i < 8; i++)
                e->undo_spots[i] = geo.area->spots_wide[i];
        }
        if (env->bits & 0xf) {
            k = (env->bits & 0xff0) >> 4;
            geo.area->spots_wide[k].x = geo.pos.x;
            geo.area->spots_wide[k].y = geo.pos.y;
            geo.area->spots_wide[k].facing = geo.pos.facing;
        }
        break;
    default:
        prev = 2;
    }
    spot_items(e->view, 1);
    screen_draw_begin(1);
    screen_flush_mode_set(1);
    ed_frame(e->view, 0);
    str_format(menu, "%s %s", ok_text, cancel_text);
    hotkey_reset(1);
    menu_line_show(empty_text, menu, 0);
    dl_draw(1);
    entry_list();
    screen_flush_mode_set(0);
    screen_flush();
    env->bits &= 0xffff0000L;
    if ((k = spot_pick(e, 1, 1, 0, entry_mark, cell_open)) != 0) {
        switch (k) {
        case 1:
            for (i = k = 0; i < 8 && !k; i++)
                if (geo.area->spots_wide[i].x != e->undo_spots[i].x
                    || geo.area->spots_wide[i].y != e->undo_spots[i].y
                    || geo.area->spots_wide[i].facing != e->undo_spots[i].facing)
                    k = 1;
            if (k) {
                env->bits |= 1;
                e->undo_spot.which = 0xff;
            }
            break;
        default:
            env->bits |= 3;
            prev = 0xb;
        }
    } else if (e->flags & 4)
        for (i = 0; i < 8; i++)
            geo.area->spots_wide[i] = e->undo_spots[i];
    dl_end();
    if (prev == 2) {
        geo.pos = e->pos;
        if (!area_overland())
            ed_mode_get(0, &e->view);
        if (e->view == 0)
            ed_window(1, 0);
    }
    return prev;
}

static char far cell_open(struct EdState far *e)                 /* 3e3e:0f3b */
{
    return !cell_closed(geo.area->cols * geo.pos.y + geo.pos.x);
}

int far replace_screen(int prev, struct EdEnv far *env, void far *arg)
                                                                 /* 3e3e:0f6a */
{
    unsigned char from;
    unsigned char to;
    struct EdState far *e;
    int n;

    if (arg) {
        e = arg;
        from = env->bits & 0xff;
        switch (e->tool) {
        case 0:
            to = 0;
            to = e->paint[SIDE_A] & 0xf | to & 0xf0;
            to = (e->paint[KIND_A] & 0xf) << 4 | to & 0xf;
            break;
        case 1:
            to = e->paint[BACK];
            break;
        case 2:
            to = e->paint[ZONE];
            break;
        }
        env->bits &= 0xffff0000L;
        if (replace_ask(e, n = replace_count(e, from), from, to)) {
            switch (e->tool) {
            case 0:
                n = replace_walls(e, from & 0xf, from >> 4 & 0xf);
                break;
            case 1:
                n = replace_back(e, from);
                break;
            case 2:
                n = replace_zone(e, from);
                break;
            }
            if (n)
                env->bits |= 1;
        }
    }
    return prev;
}

static int far replace_count(struct EdState far *e, unsigned char v) /* 3e3e:10b9 */
{
    short cells;
    short j;
    int i;
    int n;

    n = 0;
    cells = geo.area->rows * geo.area->cols;
    switch (e->tool) {
    case 0:
        for (i = 0; i < cells; i++)
            for (j = 0; j < 4; j++)
                if (geo.area->cells[i].wall[j] == v)
                    n++;
        break;
    case 1:
        for (i = 0; i < cells; i++)
            if ((geo.area->cells[i].bits & 3) == v)
                n++;
        break;
    case 2:
        for (i = 0; i < cells; i++)
            if ((geo.area->cells[i].bits >> 2 & 7) == v)
                n++;
        break;
    }
    return n;
}

static char far replace_ask(struct EdState far *e, int count, unsigned char from,
                            unsigned char to)                    /* 3e3e:118d */
{
    char text[0x28];
    char menu[0x10];
    short last;
    short key;
    int k;

    screen_draw_begin(1);
    screen_flush_mode_set(1);
    frame_border();
    str_copy(text, replace_all_text);
    text_print_at(0x1f48, (0x26 - str_len(text)) * 4 / 2 + 0x1f44, 0x8b, text);
    last = count ? 1 : 0;
    if (from == to) {
        last = 0;
        str_format(menu, "%s", cancel_text);
        str_copy(text, self_replace_text);
    } else if (count) {
        str_format(text, "%s %s %s %d %s %s%s?", ok_text, to_text, replace_text, count, cell_text,
                   tool_names[e->tool], count == 1 ? "" : "s");
        str_format(menu, "%s %s", replace_text, cancel_text);
    } else {
        str_copy(text, none_to_replace_text);
        str_format(menu, "%s", ok_text);
    }
    text_print_at(0x1f60, 0x1f4c, 0x8c, "%s %s:", replace_text, this_text);
    text_print_at(0x1f60, 0x1f9c, 0x8c, "%s %s:", with_text, this_text);
    text_print_at(0x1f91, (0x26 - str_len(text)) * 4 / 2 + 0x1f44, 0x8f, text);
    replace_show(e, 0x1f64, 0x1f4c, from);
    replace_show(e, 0x1f64, 0x1f9c, to);
    dl_begin();
    hotkey_reset(last);
    menu_line_show(empty_text, menu, 0);
    dl_draw(1);
    screen_flush_mode_set(0);
    screen_flush();
    do {
        while ((key = ui_poll()) < 0)
            input_poll();
        if ((k = menu_line_key(key)) >= 0 && menu_escaped) {
            if (k == 0xd)
                k = 0;
            else
                k = last;
            menu_escaped = 0;
        }
    } while (k < 0 || k > last);
    dl_end();
    return from != to && k == 0;
}

static void far replace_show(struct EdState far *e, int r, int c, unsigned char v)
                                                                 /* 3e3e:1454 */
{
    struct EdState far *hold;
    struct MapEd far *m;
    unsigned char a;
    unsigned char b;

    m = (struct MapEd far *)&hold;
    m->st = e;
    switch (e->tool) {
    case 0:
        a = e->paint[SIDE_A];
        e->paint[SIDE_A] = v & 0xf;
        b = e->paint[KIND_A];
        e->paint[KIND_A] = v >> 4 & 0xf;
        wall_info_draw(m, r, c, -1, 0, 0);
        e->paint[SIDE_A] = a;
        e->paint[KIND_A] = b;
        break;
    case 1:
        a = e->paint[BACK];
        e->paint[BACK] = v;
        back_info_draw(m, r + 2, c, -999, 0, 0);
        e->paint[BACK] = a;
        break;
    case 2:
        a = e->paint[ZONE];
        e->paint[ZONE] = v;
        zone_info_draw(m, r, c, -1, 0, 0);
        e->paint[ZONE] = a;
        break;
    }
}

static int far replace_walls(struct EdState far *e, unsigned char side, unsigned char kind)
                                                                 /* 3e3e:1563 */
{
    char both;
    short row;
    short col;
    int n;
    int count;

    if (e->paint[SIDE_A] == side && e->paint[KIND_A] == kind)
        return 0;
    both = 1;
    for (n = 0, count = n, row = n; row < geo.area->rows; row++)
        for (col = 0; col < geo.area->cols; col++) {
            if (cell_side(n, 8) == side && (!both || cell_kind(n, 8) == kind)) {
                cell_side_set(n, 8, e->paint[SIDE_A]);
                if (both)
                    cell_kind_set(n, 8, e->paint[KIND_A]);
                count++;
            }
            if (cell_side(n, 4) == side && (!both || cell_kind(n, 4) == kind)) {
                cell_side_set(n, 4, e->paint[SIDE_A]);
                if (both)
                    cell_kind_set(n, 4, e->paint[KIND_A]);
                count++;
            }
            if (cell_side(n, 2) == side && (!both || cell_kind(n, 2) == kind)) {
                cell_side_set(n, 2, e->paint[SIDE_A]);
                if (both)
                    cell_kind_set(n, 2, e->paint[KIND_A]);
                count++;
            }
            if (cell_side(n, 6) == side && (!both || cell_kind(n, 6) == kind)) {
                cell_side_set(n, 6, e->paint[SIDE_A]);
                if (both)
                    cell_kind_set(n, 6, e->paint[KIND_A]);
                count++;
            }
            n++;
        }
    return count;
}

static int far replace_back(struct EdState far *e, unsigned char v) /* 3e3e:1762 */
{
    short row;
    short col;
    int n;
    int count;

    if (e->paint[BACK] == v)
        return 0;
    for (count = 0, n = count, row = n; row < geo.area->rows; row++)
        for (col = 0; col < geo.area->cols; col++) {
            if (cell_back(n) == v) {
                cell_back_set(n, e->paint[BACK]);
                count++;
            }
            n++;
        }
    return count;
}

static int far replace_zone(struct EdState far *e, unsigned char v) /* 3e3e:17da */
{
    short row;
    short col;
    int n;
    int count;

    if (e->paint[ZONE] == v)
        return 0;
    for (count = 0, n = count, row = n; row < geo.area->rows; row++)
        for (col = 0; col < geo.area->cols; col++) {
            if (cell_zone(n) == v) {
                cell_zone_set(n, e->paint[ZONE]);
                count++;
            }
            n++;
        }
    return count;
}

static void far spot_items(int view, char esc)                   /* 3e3e:1852 */
{
    int h;
    int w;

    if (view == 1) {
        text_xy_to(0x1f40, 0x1f40, ed_rows * 8, ed_cols * 8, &h, &w);
        dl_begin();
        ui_add(7, map_key_hook, 0x14, 0);
        ui_add(5, 0x1f44, w + 0x1f44, h + 8, 4, 0x29, 5, 0x14, 0);
        ui_add(5, h + 0x1f48, 0x1f44, 4, w, 0x29, 7, 0x14, 0);
        ui_add(5, 0x1f44, 0x1f40, h + 8, 4, 0x29, 9, 0x14, 0);
        ui_add(5, 0x1f44, 0x1f44, 4, w, 0x29, 0xb, 0x14, 0);
        ui_add(5, 0x1f48, 0x1f44, h, w, 0x22, map_button_cb, 0x14, 0);
        if (esc)
            ui_add(0x29, -1, 0);
        else
            ui_add(0x29, 1, 0);
    }
}

static char far spot_pick(struct EdState far *e, char wall, char done_ok, char marker,
                          void (far *draw)(struct EdState far *),
                          char (far *ok)(struct EdState far *))  /* 3e3e:19c2 */
{
    char rc;
    char erase;
    short tool;
    short k;
    int r;
    int c;
    char last_y;
    char last_x;
    char last_dir;
    struct MapEd m;
    int key;

    erase = wall && (e->tool || area_overland());
    last_y = last_x = last_dir = (char)0x80;
    m.st = e;
    ed_state = &m;
    fill_rect(ed_ypos, ed_pix_x, ed_ypos + edit_height, ed_pix_x + edit_width, 8);
    ed_open(&m);
    panel_title_draw(e);
    if (marker) {
        clip_set(0x1f58, 0x1f9c, 0x1f7a, 0x1fdc);
        panel_draw(&m, 0);
        clip_reset();
    } else
        panel_pos_draw(e, 0x1f58, 0x1f9c, m.hover);
    if (draw == 0)
        draw = spot_draw;
    draw(e);
    rc = 0;
    while (rc == 0) {
        if (erase) {
            tool = e->tool;
            e->tool = 0xff;
        }
        while ((key = ui_poll()) < 0) {
            input_poll();
            if (marker)
                clip_set(0x1f58, 0x1f9c, 0x1f7a, 0x1fdc);
            mouse_pos(&r, &c);
            if (wall && (k = map_hit_wall(r, c)) >= -1) {
                if (k >= 0)
                    geo.pos.facing = k;
                if (last_dir != geo.pos.facing || last_y != geo.pos.y || last_x != geo.pos.x
                    || !m.hover) {
                    last_y = geo.pos.y;
                    last_x = geo.pos.x;
                    last_dir = geo.pos.facing;
                    m.hover = 1;
                    if (marker)
                        panel_draw(&m, 0);
                    else
                        panel_pos_draw(e, 0x1f58, 0x1f9c, m.hover);
                }
            } else if (!wall && map_hit_cell(r, c) >= 0) {
                if (last_y != geo.pos.y || last_x != geo.pos.x || !m.hover) {
                    last_y = geo.pos.y;
                    last_x = geo.pos.x;
                    m.hover = 1;
                    if (marker)
                        panel_draw(&m, 0);
                    else
                        panel_pos_draw(e, 0x1f58, 0x1f9c, m.hover);
                }
            } else if (m.hover) {
                m.hover = 0;
                if (marker)
                    panel_draw(&m, 1);
                else
                    panel_pos_draw(e, 0x1f58, 0x1f9c, m.hover);
                shown_clear(&m);
                last_y = last_x = last_dir = (char)0x80;
            }
            if (marker)
                clip_reset();
        }
        if (erase)
            e->tool = tool;
        if ((k = menu_line_key(key)) >= 0)
            rc = k || marker ? -1 : 1;
        else
            switch (key) {
            case 0:
                if (shift_held() && hook_key >= 0x101 && hook_key <= 0x108)
                    marker_key(&m, hook_key, draw);
                else if (hook_key == 0x1b)
                    rc = -1;
                else if (hook_key == 0xd && !done_ok)
                    rc = 1;
                else
                    beep();
                break;
            case 1:
            case 2:
            case 3:
            case 4:
                map_page(&m, (key - 1) * 2 + 0x102, draw);
                break;
            case 5:
                if (done_ok) {
                    if (spot_hit(&m, wall) && ok && ok(e))
                        rc = 2;
                } else
                    beep();
                break;
            default:
                beep();
            }
    }
    dl_end();
    return rc > 0 ? rc : 0;
}

static void far spot_draw(struct EdState far *e)                 /* 3e3e:1d7b */
{
    map_window_draw(e, 0);
}

static char far spot_hit(m, wall)                                /* 3e3e:1d90 */
    struct MapEd far *m;
    char wall;
{
    short k;
    int r;
    int c;
    char hit;

    hit = 0;
    mouse_pos(&r, &c);
    if (m->hover) {
        if (wall) {
            if ((k = map_hit_wall(r, c)) >= 0) {
                geo.pos.facing = k;
                hit = 1;
            } else if (k == -1)
                beep();
        } else if ((k = map_hit_cell(r, c)) > 0)
            hit = 1;
        else if (k == 0)
            beep();
    } else
        beep();
    return hit;
}
