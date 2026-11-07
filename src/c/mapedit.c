/* CKIT.EXE overlay section 16, module 29f4: the area editor's map.  The
 * accessors of an area square (geo.area's 6-byte cells): a wall's side and
 * kind nibbles by direction, the event byte, the backdrop (bits 0..1) and
 * the zone (bits 2..4); painting a wall, its back side and the four walls
 * of a square, a backdrop, a zone or an event at a square, keeping the
 * values overwritten in the settings record; turning and stepping the
 * party marker with the direction keys and scrolling the map window; the
 * map window itself (a grid of squares placed by view3d.c's map window
 * variables, a variant over the big picture, and one square redrawn), the
 * 3D view, and the status panel (the design and area names, the size, the
 * column and row, the facing, the walls ahead, left and right, the
 * backdrop, the zone and its name, the event).
 *
 * Section 16 (parent section 15, itself a child of 13; no section names 16
 * as its parent): payload at EXE 0x064510, link segment 29f4, which sections
 * 17 and 18 share.  A bare stored SEG:OFF in this file means section 16.
 * The module is stored frame 29f4 (load-based 39f4), EXE 0x064510..0x067cf8
 * (14,312 bytes, 56 routines), the whole section from its first byte; eight
 * zero bytes follow to the section's end (0x067d00).  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.
 *
 * Section 16's relocation table files all 252 of its sites under 29f4, all
 * in this range.  41 routines are vector targets, by stubs 18de:1d0f..1e9f;
 * sections 21 and 23 far-call the stubs, section 18 calls 1e3b once and
 * sections 25 and 26 call 1e63 once each.  1d55 (map_button_cb) and 1e45
 * (map_key_hook) are far-called by no one; sections 18 and 23 push each as a
 * callback (18de:1d55, 18de:1e45).  No other section far-calls frame 29f4.
 * The module far-calls 23 of its own routines ahead of their definitions, so
 * they are public; it near-calls undo_kind_put, undo_kind_get, grid_map_draw,
 * grid_cell_draw and overland_cell_draw ahead of their definitions, so those are
 * static.  back_at and zone_at are reached only by near calls after their
 * definitions: for these two the bytes do not decide static or public.  It
 * calls section 13's view3d.c directly (map_scroll, map_mark, map_marker,
 * wall_kind_at, wall_side_at, view_draw, wild_pic_open: 28 calls), writes
 * view3d.c's map window variables, and reaches section 5's labels.c and
 * section 10's edit_dirty_set through vector stubs; it makes no call into
 * section 15.
 *
 * FRUA's own code: the sharing map pairs 55 of the 56 routines a-only and
 * compass_draw (1cf4) partial with DQK frame.c's frame_main, which rule (b)
 * does not reach, so every name here is chosen from the code.
 *
 * _DATA is DGROUP 8d90..8de4: row_step and col_step (indexed by direction
 * 0..8; the code uses 2, 4, 6, 8) and 13 literals stored as 12 (-d stores "" on the NUL of "%*s").  Below it
 * 8d5a..8d8f is numfield.c's (section 15); 8de5 is a pad byte; 8de6 has
 * word hits in sections 17 and 24 to 26.  _BSS is d42a..d444, 13 variables
 * (the map window's scroll origin, pixel place and size, the rows and
 * columns shown, the current settings, the "Undo" text and the event
 * square), in the order their names give (goldbox.bss_order); all are
 * public, as each has word hits in sections 18, 21 or 23.  d422..d429 below
 * is charview.c's (section 14); no word in the image names d445; d446 to
 * d44f are read mostly by section 17.
 *
 * __abs__ is Turbo C's inline abs, which <stdlib.h>'s abs() expands to;
 * declaring it here keeps this file free of # directives.
 */

struct Res {                    /* tiles.c's resource                     */
    int slot;                   /* +0                                     */
    int index;                  /* +2                                     */
    char tail[2];               /* +4                                     */
};

struct MapCell {                /* view3d.c's square of the area, 6 bytes */
    unsigned char wall[4];      /* +0  by dir / 2: kind high, side low    */
    unsigned char event;        /* +4                                     */
    unsigned char bits;         /* +5  backdrop 0..1, zone 2..4           */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0                                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    unsigned char sets[3];      /* +4    wall set by slot, 0 none         */
    unsigned char flags;        /* +7                                     */
    unsigned char back[4];      /* +8    backdrop tile by backdrop 0..3   */
    char pad_c[0x6a];           /* +c    not read here                    */
    char name[16];              /* +76   the area's name                  */
    char zone_names[8][16];     /* +86   by zone 0..7                     */
    char pad_106[0xa];          /* +106  not read here                    */
    int zone_texts[8];          /* +110  not read here                    */
    char pad_120[2];            /* +120  not read here                    */
    struct MapCell cells[0x240]; /* +122                                  */
};

struct Pos {                    /* a place and facing, 5 bytes            */
    char x;                     /* +0                                     */
    char y;                     /* +1                                     */
    unsigned char facing;       /* +2                                     */
    char facing_wall;           /* +3                                     */
    char event;                 /* +4                                     */
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

struct EdState {                /* the editor's settings                  */
    int screen;                 /* +0                                     */
    int prev;                   /* +2   not read here                     */
    unsigned char view;         /* +4   0 the 3D view, 1 the map          */
    unsigned char tool;         /* +5   0 wall 1 backdrop 2 zone 3 event  */
    int f6;                     /* +6                                     */
    char f8;                    /* +8   not read here                     */
    char move;                  /* +9   0 free movement, else walls block */
    unsigned char side_a;       /* +a   the side painted, front and back  */
    unsigned char side_b;       /* +b                                     */
    unsigned char kind_a;       /* +c   the kind painted, front and back  */
    unsigned char kind_b;       /* +d                                     */
    unsigned char back;         /* +e                                     */
    unsigned char zone;         /* +f                                     */
    char pad_10[2];             /* +10                                    */
    unsigned char undo_kind;    /* +12  undo_set's first argument     */
    char both;                  /* +13  1: paint a wall's back side too   */
    char pad_14;                /* +14                                    */
    unsigned char old_side_a;   /* +15  the old values, for undo          */
    unsigned char old_side_b;   /* +16                                    */
    unsigned char old_kind_a;   /* +17                                    */
    unsigned char old_kind_b;   /* +18                                    */
    unsigned char old_back;     /* +19                                    */
    unsigned char old_zone;     /* +1a                                    */
    unsigned char old_event;    /* +1b                                    */
    char pad_1c[0x10];          /* +1c                                    */
    struct Pos pos;             /* +2c  the party marker in the map view  */
};

struct MapEd {                  /* the map editor, passed by its callers  */
    struct EdState far *st;     /* +0                                     */
    char erase;                 /* +4   paint 0 instead of the settings   */
    char pointer_mode;          /* +5                                     */
    char pad_6[0x17];           /* +6   not read here                     */
    char hover;                 /* +1d  the mouse is over the map         */
    unsigned char shown_side;   /* +1e  what the panel shows now          */
    unsigned char shown_kind;   /* +1f                                    */
    unsigned char shown_back;   /* +20                                    */
    unsigned char shown_zone;   /* +21                                    */
    unsigned char shown_event;  /* +22                                    */
    char shown_block;           /* +23                                    */
    char shown_erase;           /* +24                                    */
    char shown_ev;              /* +25                                    */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far key_flush(void);                                 /* 0da7:00bd */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far pointer_hide_rect(int row1, int col1, int row2, int col2); /* 0f5e:0e12 */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern char far shift_held(void);                                /* 107e:0299 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far pointer_glyph_load(int icon, int frame);               /* 11d7:0cf9 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern void far piece_box(int row, int col, int n, int k);
                                /* 18de:17dd, vector to overlay 5 1e8f:1576 */
extern void far area_label(int v, char far *out);
                                /* 18de:17e7, vector to overlay 5 1e8f:0eb5 */
extern void far back_draw(unsigned char col, unsigned char row, unsigned char n,
                             char own, char raw);
                                /* 18de:17fb, vector to overlay 5 1e8f:14c3 */
extern void far label_get(unsigned char num, unsigned char kind, char far *out);
                                /* 18de:1819, vector to overlay 5 1e8f:06b0 */
extern char far cell_used(unsigned char n);
                                /* 18de:1837, vector to overlay 5 1e8f:0df0 */
extern void far edit_dirty_set(void);
                                /* 18de:1bb1, vector to overlay 10 267d:046d */
extern char far map_scroll(char far *y, char far *x, int far *top, int far *left,
                           int rows, int cols, char center);
                                                    /* overlay 13 26e3:0158 */
extern void far map_mark(int y, int x, unsigned char dir);
                                                    /* overlay 13 26e3:04b3 */
extern void far map_marker(int y, int x, int row, int col, unsigned char dir);
                                                    /* overlay 13 26e3:04fd */
extern unsigned char far wall_kind_at(int x, int y, unsigned char dir);
                                                    /* overlay 13 26e3:08aa */
extern unsigned char far wall_side_at(int x, int y, unsigned char dir);
                                                    /* overlay 13 26e3:0aa5 */
extern void far view_draw(char x, char y, unsigned char dir);
                                                    /* overlay 13 26e3:0bdb */
extern void far wild_pic_open(void);                  /* overlay 13 26e3:17e2 */

extern char far *col_row_fmt;                          /* 0148  "Col %2d Row %2d" */
extern char far *col_row_none_fmt;                          /* 014c  "Col %s Row %s"   */
extern char far *size_fmt;                          /* 0150  "Wd  %2d Ht  %2d" */
extern char far *tool_names[];                        /* 0154  "Wall", ...       */
extern char far *move_names[];                        /* 0164  "Free movement"   */
extern char far *wall_kind_names[];                        /* 016c  "Open", ...       */
extern char far *pass_names[];                        /* 01ac  "Passable", ...   */
extern char far *undo_names[];                        /* 024c  "Undo", "Un*do"   */
extern char far *compass_names[];                        /* 02e0  "North", ...      */
extern char far *right_text;                          /* 0340  "Right"           */
extern char far *left_word;                          /* 0344  "Left"            */
extern char far *facing_text;                          /* 0348  "Facing"          */
extern char far *wall_text;                          /* 034c  "Wall"            */
extern char far *block_text;                          /* 0350  "Block"           */
extern char far *place_text;                          /* 0358  "Place"           */
extern char far *edit_text;                          /* 0360  "Edit"            */
extern char far *any_text;                          /* 036c  "any"             */
extern char far *no_text;                          /* 03e8  "No"              */
extern char far *not_enterable_text;                          /* 03f4  "Not enterable"   */
extern char far *possible_text;                          /* 03f8  "possible"        */
extern char far *design_default_text;                          /* 0414  "A Game Design"   */
extern char far *zone_default_fmt;                          /* 0420  "My zone #%d"     */
extern char far *event_word;                          /* 042c  "Event"           */
extern char far *empty_text;                                     /* 3bc0 */
extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern int map_row;                                              /* 889d */
extern int map_col;                                              /* 889f */
extern int map_top;                                              /* 88a1 */
extern int map_left;                                             /* 88a3 */
extern unsigned char map_rows;                                   /* 88a5 */
extern unsigned char map_cols;                                   /* 88a6 */
extern int hook_key;                                            /* b384 */
extern char game_rec[];                                          /* ba08 */
extern struct Res far *big_res;                                  /* bcf7 */
extern char ed_mode;                                           /* d136 */
extern struct Geo geo;                                           /* d414 */

static char row_step[9] = { 0, -1, 0, 1, 1, 1, 0, -1, -1 };     /* 8d90 */
static char col_step[9] = { 0, 1, 1, 1, 0, -1, -1, -1, 0 };     /* 8d99 */

int ed_x;                                                        /* d42a */
struct EdState far *ed_state;                                    /* d42c */
int edit_y;                                                      /* d430 */
int ed_ypos;                                                     /* d432 */
unsigned char ed_rows;                                           /* d434 */
unsigned char ed_cols;                                           /* d435 */
char edit_undo[6];                                               /* d436 */
int ed_pix_x;                                                    /* d43c */
int edit_width;                                                  /* d43e */
char ed_ev_row;                                                  /* d440 */
char edit_ev_col;                                                /* d441 */
int edit_height;                                                 /* d442 */
char ed_event_dir;                                               /* d444 */

int __abs__(int v);
int far ed_item_base(struct EdState far *e);
char far area_overland(void);
char far block_paint(struct MapEd far *m, int r, int c, char redraw, char erase);
void far wall_paint(struct MapEd far *m, int r, int c, int d, char redraw, char erase);
char far cell_closed(int n);
void far map_redraw(struct MapEd far *m, char full);
void far overland_cell_redraw(int y, int x, unsigned char rows, unsigned char cols, char tool);
void far grid_cell_redraw(int y, int x, unsigned char rows, unsigned char cols, char tool);
void far panel_draw(struct MapEd far *m, char full);
static void far undo_kind_put(struct MapEd far *m, int d, char v);
static char far undo_kind_get(struct MapEd far *m, int d);
void far map_window_draw(struct EdState far *e, char direct);
void far panel_title_draw(struct EdState far *e);
void far panel_tool_draw(struct MapEd far *m);
void far overland_map_draw(int y, int x, unsigned char dir, int row, int col, int rows,
                      int cols, char tool, char marker, char direct, unsigned char zoom);
static void far grid_map_draw(int y, int x, unsigned char dir, int row, int col,
                             int rows, int cols, char tool, char marker, char direct);
int far move_key_hook(int a, int key);
void far panel_pos_draw(struct EdState far *e, int r, int c, char full);
void far cell_info_draw(struct MapEd far *m, int r, int c, int n, char full);
void far compass_draw(void);
void far wall_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
void far back_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
void far zone_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
void far event_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v);
int far dir_left(unsigned char f);
int far dir_right(unsigned char f);
static void far grid_cell_draw(int y, int x, int r, int c, unsigned char tool);
static void far overland_cell_draw(int y, int x, int r, int c, unsigned char tool);

void far ed_mode_set(char a, char b)                            /* 39f4:0000 */
{
    ed_mode = a & 0xf | (b & 0xf) << 4;
}

int far cell_event(int n)                                        /* 39f4:0019 */
{
    return geo.area->cells[n].event;
}

char far cell_event_set(int n, int v)                            /* 39f4:0033 */
{
    if (cell_event(n) != v) {
        geo.area->cells[n].event = v;
        return 1;
    }
    return 0;
}

int far cell_side(int n, int dir)                                /* 39f4:0063 */
{
    unsigned char c;

    if ((dir = (dir & 6) >> 1) >= 0 && dir < 4) {
        c = geo.area->cells[n].wall[dir];
        return c & 0xf;
    }
    return 0;
}

char far cell_side_set(int n, int dir, int v)                    /* 39f4:00a8 */
{
    unsigned char far *p;

    if (cell_side(n, dir) != v) {
        dir = (dir & 6) >> 1;
        p = &geo.area->cells[n].wall[dir];
        *p = *p & 0xf0 | v & 0xf;
        return 1;
    }
    return 0;
}

int far cell_kind(int n, int dir)                                /* 39f4:00ff */
{
    unsigned char c;

    if ((dir = (dir & 6) >> 1) >= 0 && dir < 4) {
        c = geo.area->cells[n].wall[dir];
        return c >> 4 & 0xf;
    }
    return 0;
}

char far cell_kind_set(int n, int dir, int v)                    /* 39f4:0148 */
{
    unsigned char far *p;

    if (cell_kind(n, dir) != v) {
        dir = (dir & 6) >> 1;
        p = &geo.area->cells[n].wall[dir];
        *p = *p & 0xf | (v & 0xf) << 4;
        return 1;
    }
    return 0;
}

int far cell_back(int n)                                         /* 39f4:01a3 */
{
    unsigned char c;

    c = geo.area->cells[n].bits;
    return c & 3;
}

char far cell_back_set(int n, int v)                             /* 39f4:01cb */
{
    unsigned char far *p;

    if (cell_back(n) != v) {
        p = &geo.area->cells[n].bits;
        *p = *p & 0xfc | v & 3;
        return 1;
    }
    return 0;
}

unsigned char far back_at(int x, int y)                          /* 39f4:0210 */
{
    unsigned char c;

    c = geo.area->cells[geo.area->cols * y + x].bits;
    return c & 3;
}

int far cell_zone(int n)                                         /* 39f4:023e */
{
    unsigned char c;

    c = geo.area->cells[n].bits;
    return c >> 2 & 7;
}

char far cell_zone_set(int n, int v)                             /* 39f4:026a */
{
    unsigned char far *p;

    if (cell_zone(n) != v) {
        p = &geo.area->cells[n].bits;
        *p = *p & 0xe3 | (v & 7) << 2;
        return 1;
    }
    return 0;
}

unsigned char far zone_at(int x, int y)                          /* 39f4:02b3 */
{
    unsigned char c;

    c = geo.area->cells[geo.area->cols * y + x].bits;
    return c >> 2 & 7;
}

void far undo_set(struct EdState far *e, char a, char show)  /* 39f4:02e5 */
{
    e->undo_kind = a;
    str_copy(edit_undo, undo_names[0]);
    if (show) {
        ui_mark(ed_item_base(e) + 4, 0x27, (char far *)edit_undo);
        dl_draw(0);
    }
}

void far map_hover(struct MapEd far *m)                          /* 39f4:0338 */
{
    int r;
    int c;

    if (m->st->view == 1 && m->pointer_mode != 0) {
        mouse_pos(&r, &c);
        r -= ed_ypos;
        c -= ed_pix_x;
        m->hover = r >= 0 && r < edit_height && c >= 0 && c < edit_width;
    }
}

int far map_hit_wall(int r, int c)                               /* 39f4:03a3 */
{
    int cc;
    int lo;
    int hi;
    int d;
    int rr;

    d = -2;
    r -= ed_ypos;
    c -= ed_pix_x;
    if (r >= 0 && r < edit_height && c >= 0 && c < edit_width) {
        geo.pos.y = r / 8 + edit_y;
        geo.pos.x = c / 8 + ed_x;
        rr = r % 8;
        cc = c % 8;
        lo = 4;
        hi = 7;
        d = -1;
        if ((cc == rr || hi - rr == cc) && !area_overland())
            ;
        else if (rr < lo) {
            if (cc < rr)
                d = 6;
            else if (hi - rr < cc)
                d = 2;
            else
                d = 8;
        } else {
            if (cc > rr)
                d = 2;
            else if (hi - rr > cc)
                d = 6;
            else
                d = 4;
        }
    }
    return d;
}

char far map_hit_cell(int r, int c)                              /* 39f4:0478 */
{
    int rr;
    int cc;
    int hi;

    r -= ed_ypos;
    c -= ed_pix_x;
    if (r >= 0 && r < edit_height && c >= 0 && c < edit_width) {
        geo.pos.y = r / 8 + edit_y;
        geo.pos.x = c / 8 + ed_x;
        if (area_overland())
            return 1;
        rr = r % 8;
        cc = c % 8;
        hi = 7;
        return rr != 0 && cc != 0 && rr < hi && cc < hi;
    }
    return -1;
}

char far drag_axis(int r, int c, int far *pr, int far *pc, char flag) /* 39f4:0512 */
{
    int d;

    if ((d = __abs__(r - *pr) - __abs__(c - *pc)) > 0) {
        c -= ed_pix_x;
        d = (geo.pos.facing & 6) >> 1;
        *pc = (flag && d % 2 ? (d == 1 ? 7 : 0) : 4) + (c / 8 * 8 + ed_pix_x);
        return 3;
    }
    if (d < 0) {
        r -= ed_ypos;
        d = (geo.pos.facing & 6) >> 1;
        *pr = (!flag || d % 2 ? 4 : (d ? 7 : 0)) + (r / 8 * 8 + ed_ypos);
        return 2;
    }
    return 1;
}

char far tool_paint(struct MapEd far *m, char r, char c, char d, char redraw)
                                                                 /* 39f4:05f9 */
{
    char ret;
    char k;
    int n;

    ret = 1;
    k = area_overland();
    if (m->st->tool == 0) {
        if (k)
            ret = block_paint(m, r, c, redraw, m->erase);
        else
            wall_paint(m, r, c, d, redraw, m->erase);
    } else {
        n = geo.area->cols * r + c;
        switch (m->st->tool) {
        case 1:
            m->st->old_back = cell_back(n);
            cell_back_set(n, m->erase ? 0 : m->st->back);
            break;
        case 2:
            m->st->old_zone = cell_zone(n);
            cell_zone_set(n, m->erase ? 0 : m->st->zone);
            break;
        case 3:
            if (k && cell_closed(n)) {
                ret = 0;
                break;
            }
            m->st->old_event = cell_event(n);
            if (m->erase)
                m->st->f6 = 2;
            else
                m->st->f6 = 1;
            m->st->screen = 5;
            ret = -1;
            redraw = 0;
            ed_ev_row = r;
            edit_ev_col = c;
            ed_event_dir = d;
            undo_set(m->st, 0, 0);
            break;
        default:
            ret = 0;
        }
        if (redraw) {
            if (map_scroll(&r, &c, &edit_y, &ed_x, ed_rows, ed_cols,
                           m->st->view == 0))
                map_redraw(m, 0);
            else {
                if (k)
                    overland_cell_redraw(r, c, ed_rows, ed_cols, m->st->tool);
                else
                    grid_cell_redraw(r, c, ed_rows, ed_cols, m->st->tool);
                if (m->st->pos.x == c && m->st->pos.y == r)
                    map_mark(r, c, m->st->pos.facing);
            }
            panel_draw(m, 0);
        }
    }
    return ret;
}

void far wall_paint(struct MapEd far *m, int r, int c, int d, char redraw, char erase)
                                                                 /* 39f4:089b */
{
    char cr;
    char cc;
    int c2;
    int d2;
    register int n;
    register int r2;

    n = geo.area->cols * r + c;
    m->st->old_side_a = cell_side(n, d);
    m->st->old_kind_a = cell_kind(n, d);
    cell_side_set(n, d, erase ? 0 : m->st->side_a);
    cell_kind_set(n, d, erase ? 0 : m->st->kind_a);
    if (redraw) {
        cr = r;
        cc = c;
        if (map_scroll(&cr, &cc, &edit_y, &ed_x, ed_rows, ed_cols,
                       m->st->view == 0))
            map_redraw(m, 0);
        else {
            grid_cell_redraw(r, c, ed_rows, ed_cols, m->st->tool);
            if (m->st->view == 0) {
                if (geo.pos.x == c && geo.pos.y == r)
                    map_mark(r, c, geo.pos.facing);
            } else if (m->st->pos.x == c && m->st->pos.y == r)
                map_mark(r, c, m->st->pos.facing);
        }
        panel_draw(m, 0);
    }
    if (m->st->both == 1) {
        r2 = row_step[d] + r;
        c2 = col_step[d] + c;
        d2 = d + 4;
        if (d2 > 8)
            d2 -= 8;
        if (r2 >= 0 && geo.area->rows > r2 && c2 >= 0 && geo.area->cols > c2) {
            n = geo.area->cols * r2 + c2;
            m->st->old_side_b = cell_side(n, d2);
            m->st->old_kind_b = cell_kind(n, d2);
            cell_side_set(n, d2, erase ? 0 : m->st->side_b);
            cell_kind_set(n, d2, erase ? 0 : m->st->kind_b);
            grid_cell_redraw(r2, c2, ed_rows, ed_cols, m->st->tool);
            if (m->st->pos.x == c2 && m->st->pos.y == r2)
                map_mark(r2, c2, m->st->pos.facing);
        }
    }
}

char far block_paint(struct MapEd far *m, int r, int c, char redraw, char erase)
                                                                 /* 39f4:0b65 */
{
    int r2;
    int c2;
    int n;
    int n2;
    char cr;
    char cc;
    unsigned char old;
    unsigned char v;
    char changed;
    char ev;
    int d;
    int d2;

    changed = ev = 0;
    v = erase ? 0 : m->st->kind_a;
    for (d = 2; d <= 8; d += 2) {
        r2 = row_step[d] + r;
        c2 = col_step[d] + c;
        if (r2 >= 0 && geo.area->rows > r2 && c2 >= 0 && geo.area->cols > c2) {
            d2 = d + 4;
            if (d2 > 8)
                d2 -= 8;
            n2 = geo.area->cols * r2 + c2;
            undo_kind_put(m, d, old = cell_kind(n2, d2));
            if ((old == 0) ^ (v == 0)) {
                cell_kind_set(n2, d2, v);
                changed = 1;
            }
        }
    }
    n = geo.area->cols * r + c;
    if (v && cell_closed(n) && cell_event(n)) {
        ev = 1;
        m->st->f6 = 2;
        m->st->screen = 5;
        ed_ev_row = r;
        edit_ev_col = c;
        undo_set(m->st, 1, 0);
    } else if (redraw && changed) {
        cr = r;
        cc = c;
        if (map_scroll(&cr, &cc, &edit_y, &ed_x, ed_rows, ed_cols,
                       m->st->view == 0))
            map_redraw(m, 0);
        else {
            overland_cell_redraw(r, c, ed_rows, ed_cols, m->st->tool);
            if (m->st->pos.x == c && m->st->pos.y == r)
                map_mark(r, c, m->st->pos.facing);
        }
        panel_draw(m, 0);
    }
    return ev ? -1 : 1;
}

void far block_undo(struct MapEd far *m, int r, int c)         /* 39f4:0db0 */
{
    int r2;
    int c2;
    int n2;
    char cr;
    char cc;
    unsigned char cur;
    unsigned char old;
    char changed;
    int d2;
    int d;

    changed = 0;
    for (d = 2; d <= 8; d += 2) {
        r2 = row_step[d] + r;
        c2 = col_step[d] + c;
        if (r2 >= 0 && geo.area->rows > r2 && c2 >= 0 && geo.area->cols > c2) {
            d2 = d + 4;
            if (d2 > 8)
                d2 -= 8;
            n2 = geo.area->cols * r2 + c2;
            old = cell_kind(n2, d2);
            cur = undo_kind_get(m, d2);
            if ((cur == 0) ^ (old == 0)) {
                cell_kind_set(n2, d2, cur);
                changed = 1;
                undo_kind_put(m, d2, old);
            }
        }
    }
    if (changed) {
        cr = r;
        cc = c;
        if (map_scroll(&cr, &cc, &edit_y, &ed_x, ed_rows, ed_cols,
                       m->st->view == 0))
            map_redraw(m, 0);
        else {
            overland_cell_redraw(r, c, ed_rows, ed_cols, m->st->tool);
            if (m->st->pos.x == c && m->st->pos.y == r)
                map_mark(r, c, m->st->pos.facing);
        }
        panel_draw(m, 0);
        edit_dirty_set();
    }
}

static void far undo_kind_put(struct MapEd far *m, int d, char v) /* 39f4:0f75 */
{
    switch (d) {
    case 2:
        m->st->old_kind_a = v;
        break;
    case 4:
        m->st->old_kind_b = v;
        break;
    case 6:
        m->st->old_side_a = v;
        break;
    case 8:
        m->st->old_side_b = v;
        break;
    }
}

static char far undo_kind_get(struct MapEd far *m, int d)         /* 39f4:0fd5 */
{
    char v;

    switch (d) {
    case 2:
        v = m->st->old_kind_a;
        break;
    case 4:
        v = m->st->old_kind_b;
        break;
    case 6:
        v = m->st->old_side_a;
        break;
    case 8:
        v = m->st->old_side_b;
        break;
    }
    return v;
}

void far map_redraw(struct MapEd far *m, char full)            /* 39f4:103d */
{
    map_window_draw(m->st, full);
    panel_title_draw(m->st);
    panel_tool_draw(m);
}

void far map_window_draw(struct EdState far *e, char direct)        /* 39f4:1074 */
{
    char k;
    int r;
    int c;

    if (e->view == 0) {
        r = 0x1f48;
        c = 0x1f84;
    } else {
        r = 0x1f48;
        c = 0x1f44;
    }
    pointer_glyph_load(0, 0x1a);
    if ((k = area_overland()) != 0)
        overland_map_draw(geo.pos.y, geo.pos.x, geo.pos.facing, r, c, ed_rows, ed_cols,
                     e->tool, 0, direct, k);
    else
        grid_map_draw(geo.pos.y, geo.pos.x, geo.pos.facing, r, c, ed_rows, ed_cols,
                     e->tool, e->view == 0, direct);
    if (e->view == 1)
        map_mark(e->pos.y, e->pos.x, e->pos.facing);
    key_flush();
}

void far marker_move(struct MapEd far *m, int y, int x, int dir, char redraw)
                                                                 /* 39f4:1153 */
{
    if (dir <= 0)
        dir += 8;
    else if (dir > 8)
        dir -= 8;
    if (area_overland())
        overland_cell_redraw(geo.pos.y, geo.pos.x, ed_rows, ed_cols, m->st->tool);
    else
        grid_cell_redraw(geo.pos.y, geo.pos.x, ed_rows, ed_cols, m->st->tool);
    geo.pos.y = y;
    geo.pos.x = x;
    geo.pos.facing = dir;
    if (map_scroll(&geo.pos.y, &geo.pos.x, &edit_y, &ed_x, ed_rows, ed_cols,
                   m->st->view == 0)) {
        m->st->pos = geo.pos;
        map_redraw(m, 0);
    } else {
        m->st->pos = geo.pos;
        map_mark(geo.pos.y, geo.pos.x, geo.pos.facing);
    }
    if (redraw && (m->st->view == 0 || m->pointer_mode == 0))
        panel_draw(m, 0);
}

void far map_page(struct MapEd far *m, int key, void (far *fn)(struct EdState far *))
                                                                 /* 39f4:1296 */
{
    char top;
    char left;
    int dr;
    int dc;

    dr = int_max(0, geo.pos.y - edit_y);
    dc = int_max(0, geo.pos.x - ed_x);
    top = edit_y;
    left = ed_x;
    switch (key) {
    case 0x101:
    case 0x107:
    case 0x108:
        top = int_max(0, edit_y - 19);
        break;
    case 0x103:
    case 0x104:
    case 0x105:
        top = int_min(geo.area->rows - 1, edit_y + 39);
        break;
    }
    switch (key) {
    case 0x105:
    case 0x106:
    case 0x107:
        left = int_max(0, ed_x - 20);
        break;
    case 0x101:
    case 0x102:
    case 0x103:
        left = int_min(geo.area->cols - 1, ed_x + 41);
        break;
    }
    if (map_scroll(&top, &left, &edit_y, &ed_x, 20, 21, 0)) {
        geo.pos.y = edit_y + dr;
        geo.pos.x = ed_x + dc;
        if (fn)
            fn(m->st);
        else
            map_redraw(m, 0);
    } else
        beep();
}

int far map_key_hook(int a, int key)                             /* 39f4:13f5 */
{
    if (key == 0x1b) {
        hook_key = key;
        return 1;
    }
    return move_key_hook(a, key);
}

int far move_key_hook(int a, int key)                             /* 39f4:1418 */
{
    if (key >= 0x100 && key <= 0x108 || key >= 0x140 && key <= 0x159) {
        hook_key = key;
        return 1;
    }
    hook_key = 0;
    return 0;
}

void far map_button_cb(struct MapEd far *m, int key, int v)       /* 39f4:144b */
{
    if (ed_state)
        ed_state->f6 = v;
}

void far marker_key(struct MapEd far *m, int key, void (far *fn)(struct EdState far *))
                                                                 /* 39f4:1464 */
{
    char noshift;
    char check;
    char swap;
    struct Pos save;
    int n;

    if (key == 0x152) {
        sound_toggle();
        return;
    }
    if (key == 0x153) {
        music_toggle();
        return;
    }
    n = geo.area->cols * geo.pos.y + geo.pos.x;
    noshift = !shift_held();
    if ((swap = noshift && m->st->view == 1 && key >= 0x101 && key <= 0x108) != 0) {
        save = geo.pos;
        geo.pos = m->st->pos;
    }
    check = m->st->view == 0 && m->st->move != 0;
    switch (key) {
    case 0:
        break;
    case 0x108:
        if (!noshift)
            goto vert;
        if (check && cell_kind(n, geo.pos.facing) > 1)
            goto bad;
        marker_move(m, geo.pos.y + row_step[geo.pos.facing],
                     geo.pos.x + col_step[geo.pos.facing], geo.pos.facing, 1);
        break;
    case 0x104:
        if (noshift) {
            marker_move(m, geo.pos.y, geo.pos.x, geo.pos.facing + 4, 1);
            break;
        }
    vert:
        if (geo.area->rows <= 20)
            goto bad;
        goto scroll;
    case 0x106:
        if (!noshift)
            goto horiz;
        marker_move(m, geo.pos.y, geo.pos.x, geo.pos.facing - 2, 1);
        break;
    case 0x102:
        if (!noshift)
            goto horiz;
        marker_move(m, geo.pos.y, geo.pos.x, geo.pos.facing + 2, 1);
        break;
    case 0x101:
    case 0x107:
        if (!noshift)
            goto both;
        marker_move(m, geo.pos.y, geo.pos.x, (key == 0x101 ? 2 : -2) + geo.pos.facing, 0);
        if (check && cell_kind(n, geo.pos.facing) > 1)
            beep();
        else
            marker_move(m, geo.pos.y + row_step[geo.pos.facing],
                         geo.pos.x + col_step[geo.pos.facing], geo.pos.facing, 0);
        marker_move(m, geo.pos.y, geo.pos.x, geo.pos.facing - (key == 0x101 ? 2 : -2), 1);
        break;
    case 0x103:
    case 0x105:
        if (!noshift)
            goto both;
        marker_move(m, geo.pos.y, geo.pos.x, geo.pos.facing + 4, 0);
        if (check && cell_kind(n, geo.pos.facing) > 1)
            beep();
        else
            marker_move(m, geo.pos.y + row_step[geo.pos.facing],
                         geo.pos.x + col_step[geo.pos.facing], geo.pos.facing, 0);
        marker_move(m, geo.pos.y, geo.pos.x, geo.pos.facing - 4, 1);
        break;
    both:
        if (geo.area->rows > 20)
            goto scroll;
    horiz:
        if (geo.area->cols <= 21)
            goto bad;
    scroll:
        map_page(m, key, fn);
        break;
    default:
    bad:
        beep();
    }
    if (swap)
        geo.pos = save;
}

void far panel_title_draw(struct EdState far *e)                     /* 39f4:1840 */
{
    int r2;
    int c2;
    char buf[0x20];
    int r;
    int c;

    if (e->view == 0) {
        r = 0x1f84;
        c = 0x1f44;
        r2 = 0x1f75;
        c2 = 0x1fa8;
    } else {
        r = 0x1f48;
        r2 = r + 0xc;
        c = 0x1f9c;
        c2 = c;
    }
    if (game_rec[0])
        str_format(buf, "%*s", 0x10, (char far *)game_rec);
    else
        str_format(buf, design_default_text);
    buf[16] = 0;
    text_print_at(r, c, 0x87, "%s", (char far *)buf);
    area_label(cur_area_get(), buf);
    text_print_at(r + 4, c, 0x87, buf);
    mem_move(geo.area->name, buf, 0x10);
    buf[16] = 0;
    if (buf[0])
        text_print_at(r + 8, c, 0x8c, "%s", (char far *)buf);
    text_print_at(r2, c2, 0x87, size_fmt, geo.area->cols, geo.area->rows);
}

void far panel_tool_draw(struct MapEd far *m)                       /* 39f4:196d */
{
    char edit;
    char block;
    int r;
    int c;

    if (m->st->view == 0) {
        r = 0x1f84;
        c = 0x1f94;
        text_print_at(r + 0x10, 0x1f44, 0x87, move_names[m->st->move]);
    } else {
        r = 0x1f7a;
        c = 0x1f9c;
    }
    edit = m->shown_ev && m->erase == 0;
    block = m->st->tool == 0 && area_overland();
    text_print_at(r, c, 0x8b, "%15s", empty_text);
    text_print_at(r, c, 0x8b, "%s %9s", edit ? edit_text : place_text,
                  block ? block_text : tool_names[m->st->tool]);
}

void far panel_draw(struct MapEd far *m, char full)            /* 39f4:1a71 */
{
    int c;
    char ev;
    register int n;
    register int r;

    n = geo.area->cols * geo.pos.y + geo.pos.x;
    if (m->st->view == 0) {
        geo.pos.event = cell_event(n) & 0x7f;
        view_draw(geo.pos.x, geo.pos.y, geo.pos.facing % 8);
        if (full)
            cell_info_draw(m, 0x1f4c, 0x1fa8, n, 0);
        panel_pos_draw(m->st, 0x1f7b, 0x1fa8, 1);
        cell_info_draw(m, 0x1f4c, 0x1fa8, n, 1);
    } else {
        r = 0x1f58;
        c = 0x1f9c;
        if (full)
            cell_info_draw(m, r + 4, c, n, 0);
        panel_pos_draw(m->st, r, c, m->hover);
        if (!full || m->hover)
            cell_info_draw(m, r + 4, c, n, m->hover);
    }
    if (m->st->tool == 3)
        ev = cell_event(n);
    else
        ev = 0;
    if ((m->shown_ev == 0) ^ (ev == 0)) {
        m->shown_ev = ev;
        panel_tool_draw(m);
    }
}

void far panel_pos_draw(struct EdState far *e, int r, int c, char full) /* 39f4:1bf1 */
{
    if (full)
        text_print_at(r, c, 0x87, col_row_fmt, geo.pos.x, geo.pos.y);
    else
        text_print_at(r, c, 0x87, col_row_none_fmt, " -", " -");
    if (e->view == 0)
        compass_draw();
    else if (e->tool == 0xff || e->tool == 0 && !area_overland()) {
        if (full)
            text_print_at(r + 4, c, 0x87, "%s %5s", facing_text,
                          compass_names[(geo.pos.facing & 6) >> 1]);
        else
            text_print_at(r + 4, c, 0x87, "%s -    ", facing_text);
    } else
        text_print_at(r + 4, c, 0x87, "%12s", "");
}

void far compass_draw(void)                                      /* 39f4:1cf4 */
{
    switch (geo.pos.facing) {
    case 2:
        draw_glyph(0x1f40, 0x1f40, 1, 0x19);
        break;
    case 4:
        draw_glyph(0x1f40, 0x1f40, 1, 0x17);
        break;
    case 6:
        draw_glyph(0x1f40, 0x1f40, 1, 0x18);
        break;
    case 8:
        draw_glyph(0x1f40, 0x1f40, 1, 0x16);
        break;
    }
}

void far cell_info_draw(struct MapEd far *m, int r, int c, int n, char full)
                                                                 /* 39f4:1d72 */
{
    char v;

    v = m->st->view == 0;
    if (full) {
        switch (m->st->tool) {
        case 0:
            wall_info_draw(m, r, c, n, 0, v);
            break;
        case 1:
            back_info_draw(m, r, c, n, 0, v);
            break;
        case 2:
            zone_info_draw(m, r, c, n, 0, v);
            break;
        case 3:
            event_info_draw(m, r, c, n, 0, v);
            break;
        }
    } else if (m->st->view == 0)
        fill_rect(r + 4, c, r + 0x20, c + 0x34, 8);
    else
        fill_rect(r + 4, c, r + 0x1c, c + 0x40, 8);
}

void far tool_info_draw(struct MapEd far *m, char full)            /* 39f4:1e86 */
{
    char v;
    int r;
    int c;

    if ((v = m->st->view == 0) != 0) {
        r = 0x1f84;
        c = 0x1f94;
    } else {
        r = 0x1f7a;
        c = 0x1f9c;
    }
    if (full)
        fill_rect(r + 4, c, (v ? 0x18 : 0x1a) + r, c + 0x40, 8);
    switch (m->st->tool) {
    case 0:
        wall_info_draw(m, r, c, -1, m->erase, v);
        break;
    case 1:
        back_info_draw(m, r, c, -1, m->erase, v);
        break;
    case 2:
        zone_info_draw(m, r, c, -1, m->erase, v);
        break;
    case 3:
        event_info_draw(m, r, c, -1, m->erase, v);
        break;
    }
    m->shown_erase = m->erase;
}

void far wall_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v)
                                                                 /* 39f4:1f97 */
{
    char k;
    unsigned char w;

    if (v && n >= 0) {
        w = cell_kind(n, dir_left(geo.pos.facing));
        fill_rect(r + 4, c, r + 8, c + 4, w);
        text_print_at(r + 4, c + 4, 0x87, "%s %s:", left_word, wall_text);
        text_print_at(r + 8, c, 0x87, wall_kind_names[w]);
        w = cell_kind(n, geo.pos.facing);
        fill_rect(r + 0xe, c, r + 0x12, c + 4, w);
        text_print_at(r + 0xe, c + 4, 0x87, "%s %s:", facing_text, wall_text);
        text_print_at(r + 0x12, c, 0x87, wall_kind_names[w]);
        w = cell_kind(n, dir_right(geo.pos.facing));
        fill_rect(r + 0x18, c, r + 0x1c, c + 4, w);
        text_print_at(r + 0x18, c + 4, 0x87, "%s %s:", right_text, wall_text);
        text_print_at(r + 0x1c, c, 0x87, wall_kind_names[w]);
        return;
    }
    if ((k = area_overland()) != 0) {
        if (n >= 0)
            w = cell_closed(n) ? 0xf : 0;
        r -= 0xf;
    } else {
        if (n >= 0)
            w = cell_side(n, geo.pos.facing);
        else
            w = erase ? 0 : m->st->side_a;
        if (n < 0 || m->shown_side != w) {
            if (n >= 0)
                m->shown_side = w;
            fill_rect(r + 9, c, r + 0xd, c + 4, w);
            fill_rect(r + 0x11, c + 6, r + 0x12, c + 0x12, 8);
            piece_box(r + 5, c + 6, w, 3);
        }
        if (n >= 0)
            w = cell_kind(n, geo.pos.facing);
    }
    if (n < 0)
        w = erase ? 0 : m->st->kind_a;
    if (n >= 0 && m->shown_kind == w)
        return;
    if (n >= 0)
        m->shown_kind = w;
    fill_rect(r + 0x13, c, r + 0x17, c + 4, w);
    text_print_at(r + 0x13, c + 6, 0x87, k ? pass_names[w > 0 ? 1 : 0] : wall_kind_names[w]);
}

void far back_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v)
                                                                 /* 39f4:2309 */
{
    unsigned char b;
    unsigned char t;

    if (n >= 0)
        b = cell_back(n);
    else {
        b = m->st->back;
        if (erase) {
            if (b == 0)
                return;
            b = 0;
        }
    }
    if (n >= 0 && m->shown_back == b)
        return;
    if (n >= 0)
        m->shown_back = b;
    t = geo.area->back[b];
    if (v) {
        if (n >= 0) {
            fill_rect(r + 4, c, r + 8, c + 4, b);
            text_print_at(r + 4, c + 6, 0x87, "%s %d", tool_names[1], b + 1);
            return;
        }
        fill_rect(r + 0x13, c, r + 0x17, c + 4, b);
        text_print_at(r + 0x13, c + 6, 0x87, "%s %d", tool_names[1], b + 1);
        clip_set(r + 5, c + 0xc, r + 0x11, c + 0x30);
        fill_rect(r + 5, c + 0xc, r + 0x11, c + 0x30, 0);
        back_draw(0x16, 0xd, t, n < 0, 0);
    } else {
        if (n >= 0)
            r -= 2;
        fill_rect(r + 0x15, c, r + 0x19, c + 4, b);
        text_print_at(r + 0x15, c + 6, 0x87, "%s %d", tool_names[1], b + 1);
        clip_set(r + 7, c + 0xc, r + 0x13, c + 0x30);
        fill_rect(r + 7, c + 0xc, r + 0x13, c + 0x30, 0);
        if (n == -999)
            back_draw(c > 0x1f72 ? 0x17 : 4, 6, t, c > 0x1f72, 0);
        else
            back_draw(0x18, n < 0 ? 0xb : 3, t, n < 0, 0);
    }
    clip_reset();
    screen_flush();
}

void far zone_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v)
                                                                 /* 39f4:259e */
{
    unsigned char z;
    char name[17];

    if (n >= 0)
        z = cell_zone(n);
    else if (erase)
        z = 0;
    else
        z = m->st->zone;
    if (n < 0 || m->shown_zone != z) {
        if (n >= 0)
            m->shown_zone = z;
        fill_rect(r + 4, c, r + 8, c + 4, z);
        text_print_at(r + 4, c + 6, 0x87, "%s %d", tool_names[2], z + 1);
        mem_move(geo.area->zone_names[z], name, 0x10);
        if (v && n >= 0)
            name[13] = 0;
        else
            name[16] = 0;
        text_print_at(r + 8, c, 0x87, "%*s", v && n >= 0 ? 0xd : 0x10,
                      name[0] ? (char far *)name : msg_format(zone_default_fmt, z + 1));
        area_overland();
    }
    if (n >= 0 && area_overland() && cell_closed(n) != m->shown_block) {
        m->shown_block = cell_closed(n);
        text_print_at(r + 0x10, c, 0x87, "%16s", m->shown_block ? not_enterable_text : "");
    }
}

void far event_info_draw(struct MapEd far *m, int r, int c, int n, char erase, char v)
                                                                 /* 39f4:273e */
{
    char all;
    char block;
    char drawn;
    unsigned char ev;
    char buf[16];

    if ((all = n < 0) != 0)
        n = geo.area->cols * geo.pos.y + geo.pos.x;
    ev = erase ? 0 : cell_event(n);
    block = drawn = 0;
    if (all) {
        if (m->st->view == 1 && m->hover == 0)
            ev = 0;
        if ((m->shown_ev == 0) ^ (ev == 0)) {
            m->shown_ev = ev;
            panel_tool_draw(m);
            m->shown_block = 0xff;
        }
    }
    if (area_overland() && ((block = cell_closed(n)) != m->shown_block ||
                           all && m->shown_erase != m->erase)) {
        if (!all) {
            tool_info_draw(m, 0);
            drawn = 1;
            m->shown_block = block;
            if (block == 0)
                m->shown_event = 0xff;
        } else {
            text_print_at(r + 8, c, 0x80, "%16s", block ? possible_text : "");
            if (block) {
                erase = 1;
                ev = 0;
            }
        }
    } else if (!all && (m->erase && m->shown_ev || !m->erase && m->shown_ev != ev)) {
        tool_info_draw(m, 0);
        drawn = 1;
    }
    if (!all && m->shown_event == ev)
        return;
    if (!all) {
        m->shown_event = ev;
        if (!drawn) {
            tool_info_draw(m, 0);
            drawn = 1;
        }
    }
    if (block && !all) {
        text_print_at(r + 4, c, 0x87, "%16s", not_enterable_text);
        return;
    }
    if (all && ev == 0 && m->erase == 0)
        str_format(buf, "%s %s", block ? no_text : any_text, event_word);
    else
        label_get(cell_used(ev), 10, buf);
    if (v && n >= 0 && !all)
        buf[13] = 0;
    text_print_at(r + 4, c, erase ? 0x80 : 0x87, "%*s", v && n >= 0 && !all ? 0xd : 0xf,
                  (char far *)buf);
}

void far map_cell_xy(int y, int x, unsigned char rows, unsigned char cols, int far *pr,
                      int far *pc)                               /* 39f4:2a0d */
{
    int dy;
    int dx;

    if ((dy = y - map_top) < 0 || (dx = x - map_left) < 0 || rows <= dy || cols <= dx)
        *pr = *pc = -1;
    else {
        *pr = dy * 8 + map_row;
        *pc = dx * 8 + map_col;
    }
}

static void far grid_map_draw(int y, int x, unsigned char dir, int row, int col,
                             int rows, int cols, char tool, char marker, char direct)
                                                                 /* 39f4:2a77 */
{
    int r0;
    int c0;
    int r;
    int c;

    if (direct == 0)
        screen_draw_begin(1);
    r0 = int_max(row, 0x1f40);
    c0 = int_max(col, 0x1f40);
    text_xy(r0, c0, &r0, &c0);
    fill_rect(r0, c0, r0 + rows * 8, c0 + cols * 8, 8);
    if (direct != 0)
        screen_draw_begin(1);
    map_row = r0;
    map_col = c0;
    map_rows = int_min(int_max(rows, 1), geo.area->rows);
    map_cols = int_min(int_max(cols, 1), geo.area->cols);
    for (r = 0; r < map_rows; r++) {
        for (c = 0; c < map_cols; c++)
            grid_cell_draw(r + map_top, c + map_left, r0, c0 + c * 8, tool);
        r0 += 8;
    }
    if (marker)
        map_marker(y - map_top, x - map_left, map_row, map_col, dir);
    screen_flush();
}

void far grid_cell_redraw(int y, int x, unsigned char rows, unsigned char cols, char tool)
                                                                 /* 39f4:2bd2 */
{
    int r;
    int c;

    map_cell_xy(y, x, rows, cols, &r, &c);
    if (r >= 0 && c >= 0) {
        pointer_hide_rect(r, c, r + 8, c + 8);
        grid_cell_draw(y, x, r, c, tool);
        pointer_show();
    }
}

static void far grid_cell_draw(int y, int x, int r, int c, unsigned char tool) /* 39f4:2c3c */
{
    int r2;
    int c2;
    int fill;
    int tr;
    int tc;
    int gap;
    unsigned char s8;
    unsigned char s2;
    unsigned char s4;
    unsigned char s6;
    unsigned char k8;
    unsigned char k2;
    unsigned char k4;
    unsigned char k6;

    r2 = r + 8;
    c2 = c + 8;
    fill_rect(r, c, r2, c2, 0);
    text_xy(r, c, &tr, &tc);
    fill_rect(tr, tc, tr + 1, tc + 1, 0xf);
    text_xy(r, c2 - 1, &tr, &tc);
    fill_rect(tr, tc, tr + 1, tc + 1, 0xf);
    text_xy(r2 - 1, c, &tr, &tc);
    fill_rect(tr, tc, tr + 1, tc + 1, 0xf);
    text_xy(r2 - 1, c2 - 1, &tr, &tc);
    fill_rect(tr, tc, tr + 1, tc + 1, 0xf);
    if ((s8 = wall_side_at(x, y, 8)) != 0)
        fill_rect(r, c + 1, r + 1, c2 - 1, s8);
    if ((s2 = wall_side_at(x, y, 2)) != 0)
        fill_rect(r + 1, c2 - 1, r2 - 1, c2, s2);
    if ((s4 = wall_side_at(x, y, 4)) != 0)
        fill_rect(r2 - 1, c + 1, r2, c2 - 1, s4);
    if ((s6 = wall_side_at(x, y, 6)) != 0)
        fill_rect(r + 1, c, r2 - 1, c + 1, s6);
    k8 = wall_kind_at(x, y, 8);
    k2 = wall_kind_at(x, y, 2);
    k4 = wall_kind_at(x, y, 4);
    k6 = wall_kind_at(x, y, 6);
    gap = 3;
    if (s8 && k8 <= 1)
        fill_rect(r, c + gap, r + 1, c + gap + 2, 0);
    if (s2 && k2 <= 1)
        fill_rect(r + gap, c2 - 1, r + gap + 2, c2, 0);
    if (s4 && k4 <= 1)
        fill_rect(r2 - 1, c + gap, r2, c + gap + 2, 0);
    if (s6 && k6 <= 1)
        fill_rect(r + gap, c, r + gap + 2, c + 1, 0);
    if (tool == 0) {
        gap--;
        if (k8)
            fill_rect(r + 1, c + gap, r + 2, c + gap + 4, k8);
        if (k2)
            fill_rect(r + gap, c2 - 2, r + gap + 4, c2 - 1, k2);
        if (k4)
            fill_rect(r2 - 2, c + gap, r2 - 1, c + gap + 4, k4);
        if (k6)
            fill_rect(r + gap, c + 1, r + gap + 4, c + 2, k6);
    } else if (tool != 4) {
        r++;
        c++;
        r2--;
        c2--;
        switch (tool) {
        case 2:
            fill = zone_at(x, y);
            break;
        case 1:
            fill = back_at(x, y);
            break;
        case 3:
            fill = geo.area->cells[geo.area->cols * y + x].event ? 0xf : 0;
            break;
        default:
            fill = 0;
        }
        fill_rect(r, c, r2, c2, fill);
    }
}

void far overland_map_draw(int y, int x, unsigned char dir, int row, int col, int rows,
                      int cols, char tool, char marker, char direct, unsigned char zoom)
                                                                 /* 39f4:3074 */
{
    int r0;
    int c0;
    int r;
    int c;
    int i;
    int j;

    if (direct == 0)
        screen_draw_begin(1);
    r0 = int_max(row, 0x1f40);
    c0 = int_max(col, 0x1f40);
    text_xy(r0, c0, &r, &c);
    fill_rect((direct ? 0 : 0x78) + r, c, r + rows * 8, c + cols * 8, 8);
    if (direct != 0)
        screen_draw_begin(1);
    zoom = int_max(1, int_min(zoom, 4));
    wild_pic_open();
    map_row = r;
    map_col = c;
    map_rows = int_min(int_max(rows, 1), geo.area->rows);
    map_cols = int_min(int_max(cols, 1), geo.area->cols);
    clip_set(r, c, r + map_rows * 8, c + map_cols * 8);
    text_xy_to(r0, c0, map_top * 8, map_left * 8, &r0, &c0);
    draw_glyph(0x1f40 - r0, 0x1f40 - c0, big_res->slot, 1);
    res_palette_load(big_res);
    clip_reset();
    if (tool != 4)
        for (i = 0; i < map_rows; i++) {
            for (j = 0; j < map_cols; j++)
                overland_cell_draw(i + map_top, j + map_left, r, c + j * 8, tool);
            r += 8;
        }
    if (marker)
        map_marker(y - map_top, x - map_left, map_row, map_col, dir);
    screen_flush();
}

void far overland_cell_redraw(int y, int x, unsigned char rows, unsigned char cols, char tool)
                                                                 /* 39f4:3295 */
{
    int r;
    int c;
    int tr;
    int tc;

    map_cell_xy(y, x, rows, cols, &r, &c);
    if (r >= 0 && c >= 0) {
        wild_pic_open();
        res_palette_load(big_res);
        clip_set(r, c, r + 8, c + 8);
        text_xy_to(0x1f40, 0x1f40, r - y * 8, c - x * 8, &tr, &tc);
        draw_glyph(tr + 0x1f40, tc + 0x1f40, big_res->slot, 1);
        res_palette_load(big_res);
        clip_reset();
        overland_cell_draw(y, x, r, c, tool);
    }
}

static void far overland_cell_draw(int y, int x, int r, int c, unsigned char tool) /* 39f4:337e */
{
    int ny;
    int nx;
    int od;
    int d;
    int r2;
    int c2;

    if (tool == 4)
        return;
    r2 = r + 8;
    c2 = c + 8;
    switch (tool) {
    case 0:
        for (d = 2; d <= 8; d += 2) {
            ny = dir_dy[d % 8] + y;
            nx = dir_dx[d] + x;
            od = d + 4;
            if (od > 8)
                od -= 8;
            if (ny < 0 || geo.area->rows <= ny || nx < 0 || geo.area->cols <= nx ||
                wall_kind_at(nx, ny, od) != 0)
                switch (d) {
                case 8:
                    fill_rect(r, c + 1, r + 1, c2 - 1, 0xf);
                    break;
                case 2:
                    fill_rect(r + 1, c2 - 1, r2 - 1, c2, 0xf);
                    break;
                case 4:
                    fill_rect(r2 - 1, c + 1, r2, c2 - 1, 0xf);
                    break;
                case 6:
                    fill_rect(r + 1, c, r2 - 1, c + 1, 0xf);
                    break;
                }
        }
        break;
    case 3:
        if (cell_closed(geo.area->cols * y + x))
            fill_rect(r + 3, c + 3, r2 - 3, c2 - 3, 0xf);
        else if (geo.area->cells[geo.area->cols * y + x].event) {
            fill_rect(r, c, r + 2, c2, 0xf);
            fill_rect(r, c2 - 2, r2, c2, 0xf);
            fill_rect(r2 - 2, c, r2, c2, 0xf);
            fill_rect(r, c, r2, c + 2, 0xf);
        }
        break;
    case 2:
        fill_rect(r + 2, c + 2, r2 - 2, c2 - 2, zone_at(x, y));
        if (cell_closed(geo.area->cols * y + x))
            fill_rect(r + 3, c + 3, r2 - 3, c2 - 3, 0xf);
        break;
    }
}

char far cell_closed(int n)                                      /* 39f4:35f5 */
{
    unsigned char far *p;
    unsigned char k;
    int m;

    for (k = 0; k < 4; k++) {
        switch (k) {
        case 0:
            if ((m = n - geo.area->cols) >= 0)
                p = &geo.area->cells[m].wall[2];
            else
                p = 0;
            break;
        case 1:
            m = n + 1;
            if (m % geo.area->cols > 0)
                p = &geo.area->cells[m].wall[3];
            else
                p = 0;
            break;
        case 2:
            if (n / geo.area->cols + 1 < geo.area->rows)
                p = &geo.area->cells[geo.area->cols + n].wall[0];
            else
                p = 0;
            break;
        case 3:
            m = n - 1;
            if (m % geo.area->cols < geo.area->cols - 1)
                p = &geo.area->cells[m].wall[1];
            else
                p = 0;
            break;
        }
        if (p && (*p >> 4 & 0xf) == 0)
            return 0;
    }
    return 1;
}

int far dir_right(unsigned char f)                               /* 39f4:373c */
{
    return (f / 2 % 4 + 1) * 2;
}

int far dir_left(unsigned char f)                                /* 39f4:3757 */
{
    return ((f / 2 + 2) % 4 + 1) * 2;
}

int far ed_item_base(struct EdState far *e)                      /* 39f4:3774 */
{
    return e->view == 0 ? 7 : 8;
}

char far area_overland(void)                                      /* 39f4:378b */
{
    unsigned char k;

    if ((k = cur_area_get()) > 4)
        return 0;
    return k;
}

void far pos_swap(struct Pos far *a, struct Pos far *b)          /* 39f4:37a8 */
{
    struct Pos t;

    t = *a;
    *a = *b;
    *b = t;
}
