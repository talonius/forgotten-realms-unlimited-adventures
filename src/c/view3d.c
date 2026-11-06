/* CKIT.EXE overlay 13 module 26e3: the first-person view and the overhead
 * map.  The geo record (the party's place, the view colours and the area
 * pointer); the area, read from the GEO data entry as IFF chunks (a FORM of
 * type AMOD holding HDR, MAP, ENCR and STRG), its squares and the tests on
 * their walls; the wall sets, three slots of tile libraries; the view three
 * rows deep, drawn from the far row in, each wall as a piece placed at
 * pos_x[k], pos_y[k]; the overhead map, a window that scrolls over the area;
 * and the frame panels with a "back" tile.
 *
 * EXE 0x05ba40..0x05d6a1, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 26e3, the first byte of RTLink section 13 (payload
 * 0x5ba40, link segment 26e3, no parent section); fifteen zero bytes follow
 * to the section's end.  Section 13's table files all 98 of its fixup sites
 * under frame 26e3, all in this range.  Built with Turbo C 2.01, tcc -ml -O
 * -Z -d.  Functions are defined in shipped order; bake consumes them in that
 * order.  Sections 11 and 12 hold other bytes at link segment 26e3.
 *
 * A bare stored seg:off in this file means section 13.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  Nineteen routines are
 * vector targets: 18de:1cfb -> 0158, 1d05 -> 02d1, 1ce7 -> 04b3, 1cf1 ->
 * 0556, 193b -> 0557, 13f5 -> 0572, 1887 -> 0698, 1567 -> 080d, 134b ->
 * 08f0, 1341 -> 09f3, 15ad -> 0ae7, 1355 -> 0b87, 1495 -> 0bdb, 1891 ->
 * 0c85, 189b -> 15b1, 13cd -> 15da, 13af -> 1721, 15e9 -> 1750, 14a9 ->
 * 17e2.  Section 16 (a child of 15, itself a child of 13) far-calls 0158,
 * 04b3, 04fd, 08aa, 0aa5, 0bdb and 17e2 directly, and section 18 (also a
 * child of 15) calls 0572, 0d4e, 1721 and 1805; sections 14, 15 and 17 make
 * no call into frame 26e3.
 *
 * Mostly FRUA's own code, with DQK view3d.c lineage (dqk 893e699): the
 * sharing map pairs 2 routines shape with DQK view3d.c (a third, 15da,
 * with picview.c's ovl03_24da_03eb), 3 modified and 2 partial of 38.  The
 * five _DATA tables are DQK's value for value; the one-record _BSS is DQK's
 * in name only (DQK's struct Geo is 0x1e bytes, this one 0xe).  FRUA's area
 * keeps rows and cols at +2/+3 and each square's four walls, its event
 * byte and its back-tile bits in a 6-byte record; DQK's view_draw is three
 * routines here (0bdb, 0c85, 0d4e), and 0d4e draws at a given screen place.
 * The overhead map (0000..04fd) reworks DQK's fixed 11x11 map_draw into a
 * scrolling window; the chunk reader (1817..1ab8) is new, where DQK's
 * geo_load reads raw planes.
 *
 * _DATA is DGROUP 889a..8907: cur_geo, back_cur, the six map window
 * variables, DQK's five tables of ten, and four literals.  8899 is pad after
 * module 2b69's _DATA (section 12), and 8908 starts module 28aa's (section
 * 14).  Section 16 reads and writes the map window variables, section 23
 * reads them, and section 5 reads piece_cols.  _BSS is d414..d421, the
 * one record geo, between module 2b69's _BSS (ending d412) and module
 * 28aa's (d422).
 */

struct Res;                     /* tiles.c's                              */
struct Event;                    /* textbank.c's                           */

struct MapCell {                /* one square of the area, 6 bytes        */
    unsigned char wall[4];      /* +0  by dir / 2: kind high, side low    */
    unsigned char event;        /* +4                                     */
    unsigned char bits;         /* +5  back tile low 2, then 3 more       */
};

struct Area {                   /* 0xea2 bytes, what geo.area points at   */
    unsigned version;           /* +0    0x64..0x6a, swapped on load      */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    unsigned char sets[3];      /* +4    wall set by slot, 0 none         */
    unsigned char flags;        /* +7    bit 0: the overhead map allowed  */
    unsigned char back[4];      /* +8    back tile, by cell_back_at's 0..3 */
    char pad_c[0x104];          /* +c    not read here                    */
    int zone_texts[8];          /* +110  swapped on load                  */
    char pad_120[2];            /* +120  not read here                    */
    struct MapCell cells[0x240]; /* +122                                  */
};

struct Geo {                    /* the party's place and the view state   */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
    char facing_wall;           /* +3   d417, not read here               */
    char event;                 /* +4   d418, not read here               */
    char attr_5;                /* +5   d419                              */
    char attr_6;                /* +6   d41a                              */
    char show_map;              /* +7   d41b  draw the overhead map       */
    struct Area far *area;      /* +8   d41c                              */
    char attr_c;                /* +c   d420                              */
    char attr_d;                /* +d   d421                              */
};

extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col);
                                                                 /* 107e:04a7 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr);
                                                                 /* 0f5e:0040 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far glyph_library_open(char mode, char far *name, int slot); /* 11d7:0031 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern void far cell_fill(int col, int row, int width, unsigned height,
                          unsigned char attr);                   /* 05f5:0aae */
extern void far pal_black(int first, int last);                  /* 05f5:0ce2 */
extern void far glyph_cell(int col, int row, int glyph, char unused,
                           struct Res far *r);                   /* 05f5:0564 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern void far glyph_draw_at(int row, int col, char far *lib, int glyph);
                                                                 /* 11d7:0a9d */
extern void far glyph_put(int col, int row, unsigned char glyph, char unused,
                          struct Res far *r);                    /* 05f5:05bf */
extern void far mode_change(int n);
                                /* 18de:10cb, vector to overlay 3 1b41:02fe */
extern void far frame_piece_refresh(char c);                              /* 0d65:01c6 */
extern long far slot_used(int slot);                             /* 1336:0921 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far huge_move(void far *src, void far *dst, unsigned n);
                                                                 /* 00d7:08c2 */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern void far glyph_palette_load(char far *lib, int n);              /* 12d2:0537 */
extern void far clip_set(int row1, int col1, int row2, int col2);
                                                                 /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp);
                                                                 /* 05f5:01de */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern char far tile_opened(void);                                /* 05f5:04b0 */
extern void far res_glyph_recolor(unsigned char far *map, int n, struct Res far *r);
                                                                 /* 05f5:08e1 */
extern void far set_data_disk(char disk);
                                /* 18de:13a5, vector to overlay 3 1b41:0083 */
extern void far data_load(char far *name, unsigned char entry, unsigned far *len,
                             char far *buf);
                                /* 18de:1387, vector to overlay 3 1b41:0136 */
extern void far game_exit(void);                                 /* 0d48:002f */
extern void far big_open(unsigned char id);
                                /* 18de:146d, vector to overlay 3 1bbc:0334 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern int  far swap16(int v);                             /* 00d7:098b */
extern long far swap32(long v);                            /* 00d7:0994 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern char far txt_init(char far *pool, int size);              /* 0cf0:02b4 */
extern void far txt_touch(char far *pool);                       /* 0cf0:0338 */

extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern char far *text_pool_block;                                /* a7ec */
extern struct Event far *event_table;                                /* a7f0 */
extern char byte_b429;                                           /* b429 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Res far *back_res;        /* c39b  the back tile            */
extern struct Res far *wall_set_res[3];     /* caf8  the wall sets, by slot   */

static int cur_geo = -1;                /* 889a  set loaded into slot 2   */
static unsigned char back_cur = 0xff;   /* 889c  back tile loaded         */
int map_row = 0;                        /* 889d  the map window's screen  */
int map_col = 0;                        /* 889f  row and column           */
int map_top = 0;                        /* 88a1  first area row shown     */
int map_left = 0;                       /* 88a3  first area column shown  */
unsigned char map_rows = 0;             /* 88a5  area rows shown          */
unsigned char map_cols = 0;             /* 88a6  area columns shown       */
static unsigned char piece_first[10] =  /* 88a7  running sum of cols*rows;
                                           no code found reads it        */
    { 0, 2, 6, 0xa, 0x16, 0x26, 0x36, 0x6e, 0x84, 0x9a };
unsigned char piece_cols[10] =          /* 88b1  read by section 5 at
                                           0x036b0c, not by this module  */
    { 1, 1, 1, 3, 2, 2, 7, 2, 2, 1 };
static unsigned char piece_rows[10] =   /* 88bb  no code found reads it  */
    { 2, 4, 4, 4, 8, 8, 8, 0xb, 0xb, 2 };
static int pos_x[10] = { 5, 4, 6, 4, 2, 7, 2, 0, 9, 5 };         /* 88c5 */
static int pos_y[10] = { 4, 3, 3, 3, 1, 1, 1, 0, 0, 4 };         /* 88d9 */

struct Geo geo;                                                  /* d414 */

static void far map_cell(int y, int x, int row, int col);
void far map_marker(int y, int x, int row, int col, unsigned char dir);
static int far map_axis(int size, int extent, char far *p, int first, char center);
char far wall_side(char x, char y, unsigned char dir);
unsigned char far wall_kind(char x, char y, unsigned char dir);
static void far view_back(void);
unsigned char far cell_back_at(char x, char y);
void far back_load(char n);
static char far wall_kind_passable(unsigned char kind, unsigned char side);
void far view_render(int col, int row, char x, char y, unsigned char dir);
void far wall_set_load(unsigned char n, unsigned char slot);
static char far area_read(char far *buf);
static char far area_parse(char far *p);
static long far chunk_find(char far * far *pp, long want);
static char far chunk_copy(char far * far *pp, long tag, void far *dst, long size);

void far map_draw(int y, int x, unsigned char dir, int row, int col, int rows,
                  int cols, char marker, char direct)            /* 36e3:0000 */
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
            map_cell(r + map_top, c + map_left, r0, c0 + c * 8);
        r0 += 8;
    }
    if (marker)
        map_marker(y - map_top, x - map_left, map_row, map_col, dir);
    screen_flush();
}

char far map_scroll(char far *y, char far *x, int far *top, int far *left, int rows,
                    int cols, char center)                       /* 36e3:0158 */
{
    char moved;
    int r;
    int c;

    moved = 0;
    r = map_axis(rows, geo.area->rows, y, map_top, center);
    c = map_axis(cols, geo.area->cols, x, map_left, center);
    if (r != map_top) {
        moved = 1;
        map_top = r;
    }
    if (c != map_left) {
        moved = 1;
        map_left = c;
    }
    if (top)
        *top = r;
    if (left)
        *left = c;
    return moved;
}

static int far map_axis(int size, int extent, char far *p, int first,
                        char center)                             /* 36e3:01ed */
{
    if (*p < 0)
        *p = extent - -*p % extent;
    else
        *p = *p % extent;
    size = int_min(int_max(size, 1), extent);
    first = int_min(int_max(first, 0), extent - size);
    if (center)
        return int_max(0, int_min(*p - size / 2, extent - size));
    if (*p >= first + size)
        return int_max(0, int_min(*p - size + 1, extent - size));
    return *p < first ? *p : first;
}

int far map_wall(char x, char y, int dir)                        /* 36e3:02d1 */
{
    if (wall_side(x, y, dir))
        switch (wall_kind(x, y, dir)) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 15:
            return 1;
        case 1:
        case 3:
        case 5:
        case 14:
            return 2;
        }
    return 0;
}

static void far map_cell(int y, int x, int row, int col)         /* 36e3:0336 */
{
    int row2;
    int col2;
    int n;
    char k;

    row2 = row + 8;
    col2 = col + 8;
    n = 3;
    if ((k = map_wall(x, y, 8)) != 0) {
        fill_rect(row, col, row + 1, col2, 7);
        if (k == 1)
            fill_rect(row, col + n, row + 1, col + n + 2, 8);
    }
    if ((k = map_wall(x, y, 2)) != 0) {
        fill_rect(row, col2 - 1, row2, col2, 7);
        if (k == 1)
            fill_rect(row + n, col2 - 1, row + n + 2, col2, 8);
    }
    if ((k = map_wall(x, y, 4)) != 0) {
        fill_rect(row2 - 1, col, row2, col2, 7);
        if (k == 1)
            fill_rect(row2 - 1, col + n, row2, col + n + 2, 8);
    }
    if ((k = map_wall(x, y, 6)) != 0) {
        fill_rect(row, col, row2, col + 1, 7);
        if (k == 1)
            fill_rect(row + n, col, row + n + 2, col + 1, 8);
    }
}

void far map_mark(int y, int x, unsigned char dir)               /* 36e3:04b3 */
{
    int r;
    int c;

    if ((r = y - map_top) >= 0 && r < map_rows && (c = x - map_left) >= 0 && c < map_cols)
        map_marker(r, c, map_row, map_col, dir);
}

void far map_marker(int y, int x, int row, int col, unsigned char dir)
                                                                 /* 36e3:04fd */
{
    row += y * 8;
    col += x * 8;
    glyph_library_open('2', "TopView", 0xc);
    draw_glyph(row, col, 0xc, ((dir & 6) >> 1) + 0x11);
    slot_close(0xc);
}

void far view3d_noop(void)                                       /* 36e3:0556 */
{
}

void far area_alloc(void)                                        /* 36e3:0557 */
{
    geo.area = mem_alloc(sizeof(struct Area));
    geo.area->version = 0;
}

void far view_attrs(char a, char b, char c, char d)              /* 36e3:0572 */
{
    geo.attr_c = a;
    geo.attr_d = b;
    geo.attr_5 = c;
    geo.attr_6 = d;
}

void far view_frame(void)                                        /* 36e3:058f */
{
    cell_fill(2, 0x10, 0xb, 0x2c, geo.attr_c);
    cell_fill(2, 0x3c, 0xb, 2, geo.attr_5);
    cell_fill(2, 0x3e, 0xb, 0x2a, geo.attr_6);
    view_back();
}

static void far view_back(void)                                  /* 36e3:05e8 */
{
    unsigned char col;
    unsigned char row;
    unsigned char t;

    col = 2;
    row = 2;
    t = cell_back_at(geo.x, geo.y);
    t = geo.area->back[t];
    if (t >= 0x20 && (cursor_obj[8] < 6 || cursor_obj[8] >= 0x14))
        t++;
    if (t != 0) {
        if (t != back_cur) {
            back_cur = t;
            pal_black(0x20, 0xff);
            back_load(t);
        }
        glyph_cell(col, row, 1, 0, back_res);
        res_palette_load(back_res);
    }
}

void far piece_glyph(int col, register int row, unsigned char cell,
                     unsigned char k)                            /* 36e3:0698 */
{
    register int set;

    set = 0;
    while (cell > 5) {
        cell -= 5;
        set++;
    }
    if (geo.area->sets[set] == 0)
        return;
    if (++k == 10) {
        k = 0;
        if (cell > 2)
            cell = 1;
    }
    if (k < 8)
        row += 4;
    cell--;
    if (cell > 1) {
        if (set == 2)
            glyph_draw_at(row, col, db_buffer(), k + 1);
        else
            glyph_put(col, row, k + 1, 0, wall_set_res[set]);
        k = cell * 9 + k + 2;
    } else
        k = cell * 10 + k + 1;
    if (set == 2)
        glyph_draw_at(row, col, db_buffer(), k);
    else
        glyph_put(col, row, k, 0, wall_set_res[set]);
}

void far piece_at(int col, int row, char x, char y, unsigned char cell,
                  unsigned char k)                               /* 36e3:0797 */
{
    col += x * 4;
    row += y * 4;
    piece_glyph(col, row, cell, k);
}

char far in_area(char x, char y)                                 /* 36e3:07c4 */
{
    char r;

    if (x > geo.area->cols - 1 || x < 0 || y > geo.area->rows - 1 || y < 0)
        r = 0;
    else
        r = 1;
    return r;
}

unsigned char far wall_kind(char x, char y, unsigned char dir)   /* 36e3:080d */
{
    unsigned char w;
    char r;

    if (dir & 1) {
        dir--;
        x += dir_dx[dir];
        y += dir_dy[dir];
        dir += 2;
        dir &= 6;
    }
    if (!in_area(x, y))
        r = 0;
    else {
        dir = (dir & 6) >> 1;
        w = geo.area->cells[geo.area->cols * y + x].wall[dir];
        r = (w >> 4) & 0xf;
    }
    return r;
}

unsigned char far wall_kind_at(int x, int y, unsigned char dir)  /* 36e3:08aa */
{
    unsigned char w;

    dir = (dir & 6) >> 1;
    w = geo.area->cells[geo.area->cols * y + x].wall[dir];
    return (w >> 4) & 0xf;
}

char far wall_passable(char x, char y, unsigned char dir)          /* 36e3:08f0 */
{
    unsigned char w;
    char r;

    if (!in_area(x, y) && (byte_b429 == 0 || byte_b429 == 0x50))
        r = 0;
    else {
        if (x > geo.area->cols - 1)
            x = 0;
        else if (x < 0)
            x = geo.area->cols - 1;
        if (y > geo.area->rows - 1)
            y = 0;
        else if (y < 0)
            y = geo.area->rows - 1;
        dir = (dir & 6) >> 1;
        w = geo.area->cells[geo.area->cols * y + x].wall[dir];
        r = wall_kind_passable((w >> 4) & 0xf, w & 0xf);
    }
    return r;
}

static char far wall_kind_passable(unsigned char kind, unsigned char side)
                                                                 /* 36e3:09d8 */
{
    return side ? (kind >= 0xe ? 0 : 1) : 1;
}

char far wall_side(char x, char y, unsigned char dir)            /* 36e3:09f3 */
{
    unsigned char w;
    char r;

    if (x > geo.area->cols - 1)
        x = 0;
    else if (x < 0)
        x = geo.area->cols - 1;
    if (y > geo.area->rows - 1)
        y = 0;
    else if (y < 0)
        y = geo.area->rows - 1;
    dir = (dir & 6) >> 1;
    w = geo.area->cells[geo.area->cols * y + x].wall[dir];
    r = w & 0xf;
    return r;
}

unsigned char far wall_side_at(int x, int y, unsigned char dir)  /* 36e3:0aa5 */
{
    unsigned char w;

    dir = (dir & 6) >> 1;
    w = geo.area->cells[geo.area->cols * y + x].wall[dir];
    return w & 0xf;
}

char far area_cell_at(char x, char y)                             /* 36e3:0ae7 */
{
    char r;

    if (!in_area(x, y))
        r = 0;
    else
        r = geo.area->cells[geo.area->cols * y + x].event;
    return r;
}

unsigned char far cell_back_at(char x, char y)                    /* 36e3:0b37 */
{
    unsigned char w;

    if (!in_area(x, y))
        return 0;
    w = geo.area->cells[geo.area->cols * y + x].bits;
    return w & 3;
}

unsigned char far cell_zone_at(char x, char y)                    /* 36e3:0b87 */
{
    unsigned char w;

    if (!in_area(x, y))
        return 0;
    w = geo.area->cells[geo.area->cols * y + x].bits;
    return w >> 2 & 7;
}

void far view_draw(char x, char y, unsigned char dir)            /* 36e3:0bdb */
{
    int x0;
    int y0;

    mode_change(0);
    frame_piece_refresh(0);
    if (geo.show_map != 0) {
        x0 = x;
        y0 = y;
        map_scroll(&y, &x, 0, 0, 0xb, 0xb, 1);
        map_draw(y, x, dir, 0x1f4c, 0x1f4c, 0xb, 0xb, 1, 0);
        x = x0;
        y = y0;
    } else {
        screen_draw_begin(1);
        view_frame();
        view_render(0x1f4c, 0x1f4c, x, y, dir);
        screen_flush();
    }
}

void far wall_sets_load(void)                                    /* 36e3:0c85 */
{
    mode_change(0);
    if (cursor_obj[0x13] < 5)
        return;
    if (geo.area->sets[2] != 0 && geo.area->sets[2] != cur_geo) {
        wall_set_load(geo.area->sets[2], 2);
        huge_move(slot_base(*(int far *)wall_set_res[2]), db_buffer(),
                     (unsigned)slot_used(*(int far *)wall_set_res[2]));
        res_free(&wall_set_res[2]);
        cur_geo = geo.area->sets[2];
    }
    if (geo.area->sets[0] != 0 && wall_set_res[0] == 0)
        wall_set_load(geo.area->sets[0], 0);
    if (geo.area->sets[1] != 0 && wall_set_res[1] == 0)
        wall_set_load(geo.area->sets[1], 1);
}

void far view_render(register int col, int row, char x, char y,
                     unsigned char dir)                          /* 36e3:0d4e */
{
    unsigned char left;
    unsigned char right;
    unsigned char back;
    int x0;
    int y0;
    int dx;
    int dy;
    int cx;
    int cy;
    unsigned char i;
    unsigned char d;
    unsigned char c1;
    unsigned char c2;
    unsigned char prev;
    register int off;

    wall_sets_load();
    if (wall_set_res[0])
        res_palette_load(wall_set_res[0]);
    if (wall_set_res[1])
        res_palette_load(wall_set_res[1]);
    if (cur_geo >= 0 && (is_xvga() || !pixel_shift()))
        glyph_palette_load(db_buffer(), 0);
    clip_set(row, col, row + 0x2c, col + 0x2c);
    left = (dir + 6) % 8;
    back = (dir + 4) % 8;
    right = (dir + 2) % 8;
    x0 = x;
    y0 = y;
    dx = dir_dx[dir];
    dy = dir_dy[dir];
    d = 2;
    x0 += d * dx;
    y0 += d * dy;
    do {
        switch (d) {
        case 2:
            cx = x0;
            cy = y0;
            i = 0;
            off = 0;
            prev = 0;
            while (i < 4) {
                c1 = wall_side(cx, cy, dir);
                if (c1 != 0) {
                    if (prev > 0)
                        piece_at(col, row, pos_x[9] + off + 1, pos_y[9], prev, 9);
                    prev = c1;
                    if (i < 3)
                        piece_at(col, row, pos_x[0] + off, pos_y[0], c1, 0);
                } else {
                    if (prev > 0
                        && wall_side(cx - dir_dx[left], cy - dir_dy[left], left))
                        piece_at(col, row, pos_x[9] + off + 1, pos_y[9], prev, 9);
                    prev = 0;
                }
                i++;
                off -= 2;
                cx = cx + dir_dx[left];
                cy = cy + dir_dy[left];
            }
            cx = x0;
            cy = y0;
            i = 0;
            off = 0;
            prev = 0;
            while (i < 4) {
                c1 = wall_side(cx, cy, dir);
                if (c1 != 0) {
                    if (prev > 0)
                        piece_at(col, row, pos_x[9] + off - 1, pos_y[9], prev, 9);
                    prev = c1;
                    if (i != 0 && i < 3)
                        piece_at(col, row, pos_x[0] + off, pos_y[0], c1, 0);
                } else {
                    if (prev > 0
                        && wall_side(cx - dir_dx[right], cy - dir_dy[right], right))
                        piece_at(col, row, pos_x[9] + off - 1, pos_y[9], prev, 9);
                    prev = 0;
                }
                i++;
                off += 2;
                cx = cx + dir_dx[right];
                cy = cy + dir_dy[right];
            }
            cx = x0;
            cy = y0;
            i = 0;
            off = 0;
            while (i < 3) {
                c2 = wall_side(cx, cy, left);
                if (c2 != 0)
                    if (i == 0)
                        piece_at(col, row, pos_x[1] + off, pos_y[1], c2, 1);
                    else
                        piece_at(col, row, pos_x[1] + off - 1, pos_y[1], c2, 1);
                i++;
                off -= 2;
                cx = cx + dir_dx[left];
                cy = cy + dir_dy[left];
            }
            cx = x0;
            cy = y0;
            i = 0;
            off = 0;
            while (i < 3) {
                c2 = wall_side(cx, cy, right);
                if (c2 != 0)
                    if (i == 0)
                        piece_at(col, row, pos_x[2] + off, pos_y[2], c2, 2);
                    else
                        piece_at(col, row, pos_x[2] + off + 1, pos_y[2], c2, 2);
                i++;
                off += 2;
                cx = cx + dir_dx[right];
                cy = cy + dir_dy[right];
            }
            break;
        case 1:
            cx = x0 + dir_dx[left];
            cx = cx + dir_dx[left];
            cy = y0 + dir_dy[left];
            cy = cy + dir_dy[left];
            i = 0;
            off = -6;
            while (i < 3) {
                c1 = wall_side(cx, cy, dir);
                if (c1 != 0)
                    piece_at(col, row, pos_x[3] + off, pos_y[3], c1, 3);
                c2 = wall_side(cx, cy, left);
                if (i != 0 && c2 != 0)
                    piece_at(col, row, pos_x[4] + off, pos_y[4], c2, 4);
                i++;
                off += 3;
                cx = cx + dir_dx[right];
                cy = cy + dir_dy[right];
            }
            cx = x0 + dir_dx[right];
            cx = cx + dir_dx[right];
            cy = y0 + dir_dy[right];
            cy = cy + dir_dy[right];
            i = 0;
            off = 6;
            while (i < 3) {
                c1 = wall_side(cx, cy, dir);
                if (i < 2 && c1 != 0)
                    piece_at(col, row, pos_x[3] + off, pos_y[3], c1, 3);
                c2 = wall_side(cx, cy, right);
                if (i != 0 && c2 != 0)
                    piece_at(col, row, pos_x[5] + off, pos_y[5], c2, 5);
                i++;
                off -= 3;
                cx = cx + dir_dx[left];
                cy = cy + dir_dy[left];
            }
            break;
        case 0:
            cx = x0 + dir_dx[left];
            cy = y0 + dir_dy[left];
            i = 0;
            off = -7;
            while (i < 2) {
                c1 = wall_side(cx, cy, dir);
                if (c1 != 0)
                    piece_at(col, row, pos_x[6] + off, pos_y[6], c1, 6);
                c2 = wall_side(cx, cy, left);
                if (i != 0 && c2 != 0)
                    piece_at(col, row, pos_x[7] + off, pos_y[7], c2, 7);
                i++;
                off += 7;
                cx = cx + dir_dx[right];
                cy = cy + dir_dy[right];
            }
            cx = x0 + dir_dx[right];
            cy = y0 + dir_dy[right];
            i = 0;
            off = 7;
            while (i < 2) {
                c1 = wall_side(cx, cy, dir);
                if (i < 1 && c1 != 0)
                    piece_at(col, row, pos_x[6] + off, pos_y[6], c1, 6);
                c2 = wall_side(cx, cy, right);
                if (i != 0 && c2 != 0)
                    piece_at(col, row, pos_x[8] + off, pos_y[8], c2, 8);
                i++;
                off -= 7;
                cx = cx + dir_dx[left];
                cy = cy + dir_dy[left];
            }
        }
        x0 = x0 + dir_dx[back];
        y0 = y0 + dir_dy[back];
        d--;
    } while (d != 0xff);
    clip_reset();
}

void far back_load(char n)                                       /* 36e3:15b1 */
{
    set_open_mode('2');
    tile_open_file("back1", n, 0, geo.attr_c, &back_res);
}

void far back_free(void)                                         /* 36e3:15da */
{
    res_free(&back_res);
    back_cur = 0xff;
}

void far wall_set_load(unsigned char n, unsigned char slot)      /* 36e3:15ec */
{
    char name[8];
    short i;
    unsigned char map[0x100];

    if (n == 0xff)
        return;
    set_open_mode('2');
    str_format(name, "8x8d%c%d", n >= 10 ? 'c' : 'b',
               is_xvga() || !pixel_shift() ? slot + 1 : 1);
    tile_open_file(name, n, 0, 0, &wall_set_res[slot]);
    if (tile_opened() && slot != 0 && (is_xvga() || !pixel_shift())) {
        for (i = 0; i < 0x100; i++)
            map[i] = i;
        for (i = 0; i < 0x25; i++)
            map[i + 0x20] = slot * 0x25 + i + 0x20;
        for (i = 0; i < 0x30; i++)
            res_glyph_recolor(map, i, wall_set_res[slot]);
    }
}

void far wall_sets_free(char reset)                              /* 36e3:1721 */
{
    register int i;

    for (i = 0; i < 3; i++)
        res_free(&wall_set_res[i]);
    if (reset)
        cur_geo = -1;
}

void far geo_load(unsigned char n)                               /* 36e3:1750 */
{
    unsigned len;
    char far *buf;
    register int size;

    set_data_disk('3');
    mode_change(6);
    buf = db_buffer();
    data_load("GEO", n, &len, buf);
    if ((size = 0x32a2) & 1)
        size++;
    if (len != size || area_read(buf) < 0)
        game_exit();
    mode_change(0);
    cursor_obj[0x13] = n;
    if (!(geo.area->flags & 1))
        geo.show_map = 0;
    cur_geo = -1;
}

void far wild_pic_open(void)                                       /* 36e3:17e2 */
{
    big_open(geo.area->back[0] ? geo.area->back[0] : cursor_obj[0x13] - 0x11);
}

void far geo_reset(int unused)                                 /* 36e3:1805 */
{
    geo.y = geo.x = 0;
    geo.facing = 2;
}

static char far area_read(char far *buf)                       /* 36e3:1817 */
{
    return !area_parse(buf) ? -1 : 0;
}

static char far area_parse(char far *p)                         /* 36e3:1832 */
{
    long tag;
    register int i;
    register int n;

    if (p == 0)
        return 0;
    n = sizeof(struct Area);
    mem_fill(geo.area, n, 0);
    if ((tag = *(long far *)p) == 0)
        return 1;
    if (tag != 0x4d524f46L && tag != 0x20544143L && tag != 0x5453494cL)
        return 1;
    if (chunk_find(&p, 0x444f4d41L) != 0x3292L)
        return 0;
    if (!chunk_copy(&p, 0x20524448L, geo.area, 0x122L))
        return 0;
    geo.area->version = swap16(geo.area->version);
    if (geo.area->version < 0x64 || geo.area->version > 0x6a)
        return 0;
    for (i = 0; i < 8; i++)
        geo.area->zone_texts[i] = swap16(geo.area->zone_texts[i]);
    n = geo.area->rows * geo.area->cols;
    if (n <= 0 || n > 0x240)
        return 0;
    if (!chunk_copy(&p, 0x2050414dL, geo.area->cells, 0xd80L))
        return 0;
    if (!chunk_copy(&p, 0x52434e45L, event_table, 0x7d0L))
        return 0;
    if (!chunk_copy(&p, 0x47525453L, text_pool_block, 0x1c00L))
        return 0;
    for (i = 0, p = text_pool_block; i < 0x1c00; i++)
        if (*p++ != 0)
            break;
    if (i >= 0x1c00)
        n = txt_init(text_pool_block, 0x1c00);
    txt_touch(text_pool_block);
    return 1;
}

static char far chunk_copy(char far * far *pp, long tag, void far *dst,
                            long size)                           /* 36e3:1a37 */
{
    long odd;

    if (pp == 0 || *pp == 0)
        return 0;
    odd = size & 1;
    if (chunk_find(pp, tag) != size)
        return 0;
    mem_move(*pp, dst, (int)size);
    *pp += (int)size;
    if (odd)
        (*pp)++;
    return 1;
}

static long far chunk_find(char far * far *pp, long want)       /* 36e3:1ab8 */
{
    char odd;
    long id;
    long len;
    long left;
    long type;

    if (pp == 0 || *pp == 0)
        return -1;
    left = 0x7fffffffL;
    do {
        if ((id = *(long far *)*pp) == 0)
            return -1;
        *pp += 4;
        if ((len = *(long far *)*pp) == 0)
            return -1;
        len = swap32(len);
        *pp += 4;
        odd = len & 1;
        len += odd;
        if (id == want)
            return len - odd;
        if (id == 0x4d524f46L) {
            if ((type = *(long far *)*pp) == 0)
                return -1;
            *pp += 4;
            if (type != 0x444f4d41L) {
                left -= len + 8;
                if ((len -= 4) >= 0)
                    *pp += (int)len;
            } else if (want == 0x444f4d41L)
                *pp -= 4;
            else {
                if ((len -= 4) >= 0)
                    *pp += 4;
                left = len - 8;
            }
        } else {
            left -= len + 8;
            *pp += (int)len;
        }
    } while (left > 0);
    return -1;
}
