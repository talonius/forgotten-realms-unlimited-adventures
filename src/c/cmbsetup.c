/* CKIT.EXE overlay 01 module 2482: setting up a fight.
 * Loading the combat icons and picture, building the combat map -- in a
 * dungeon from the walls around the party's square, in the wilderness from
 * the terrain's river, woods and scatter -- readying each party member's
 * combat record, and placing every combatant in its side's formation.
 *
 * EXE 0x027790..0x0294f7, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 2482, the only module of RTLink section 1 (payload
 * 0x27790, link segment 2482; a child of section 0).  Nine zero bytes follow
 * to the section's end at 0x29500.  Built with Turbo C 2.01, tcc -ml -O -Z
 * -d.  Functions are defined in shipped order; bake consumes them in that
 * order.  Section 2's 2482 frame holds other bytes.
 *
 * A bare stored seg:off in this file means section 1, or section 0 for a
 * callee in frames 1cb8, 1dbf and 22fa.  The seg:off beside each extern is
 * the far pointer as stored in the shipped call, or the DGROUP offset of an
 * extern variable; a definition's comment gives the load-based address, as
 * the resident modules' do.  Callee and variable names are placeholders
 * chosen from use; an ovl00_ name says which stored segment of section 0
 * the callee lives in, and a mod18de_ callee is an RTLink vector stub whose
 * target is named beside it.
 *
 * combat_setup is reached through the vector stub 18de:1125, combat.c's
 * combat_setup; every other routine is near-called only, and scatter_tile,
 * called ahead of its definition, is the one whose static the bytes prove.
 *
 * _DATA is DGROUP 5908..59b1: a mask byte, the direction and formation
 * tables, the terrain table terrain_flags copies to its frame, and three
 * literals.  _BSS is aec2..b0f3: the block and wall state of the dungeon
 * map, the two sides' anchors and formations, and the loop counters placing
 * shares.  Nothing found in the image references unused, dummy_long or
 * unused_byte; they keep the shipped layout.  Turbo C lays _BSS out by the
 * statics' names, and these names give the shipped order.
 *
 * combatants, heroes, recs and tile_info are indexed from 1; the shipped
 * code adds the address of element 0 (cc76, b8f1, ce0b, 5ff1).
 *
 * Directions run 0 (north) clockwise to 7 (north-west), as dir_dx and
 * dir_dy step them.  The dungeon map is 13 by 5 blocks of 6 by 5 cells
 * around the party's square; a formation is 6 rows of 11 cells.
 */

struct Map {
    char pad_0[2];              /* +0   not read here                     */
    int view_x;                 /* +2   the view's left column            */
    int view_y;                 /* +4   the view's top row                */
    char cursor_on;             /* +6                                     */
    char cursor_size;           /* +7                                     */
    char ignore_blocking;       /* +8   nonzero: terrain blocks nothing   */
    unsigned char cell[25][50]; /* +9   tile_info index from 1, by row    */
};

struct Area {                   /* what geo.area points at                    */
    char pad_0[2];              /* +0   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[8];              /* +4   not read here                     */
    unsigned char combat_sets[0xfc]; /* +c by combat_outdoors; no bound is read    */
    unsigned char zone_wild_sets[1]; /* +108 by what 18de:1355 returns         */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    char pad_1[2];              /* +1   not read here                     */
    char kind;                  /* +3   0x25: open ground                 */
};

struct Fight {                  /* a combatant's state in a fight         */
    char pad_0[0xa];            /* +0   not read here                     */
    unsigned char facing;       /* +a                                     */
    char pad_b[7];              /* +b   not read here                     */
    unsigned char turn_attempts; /* +12                                    */
    char pad_13;                /* +13  not read here                     */
    char not_pc;                /* +14                                    */
    char pad_15[2];             /* +15  not read here                     */
    char byte_17;               /* +17                                    */
};

struct Hero {                   /* a combatant                            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[0x3c];           /* +4   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x1a];          /* +44  not read here                     */
    unsigned char status;       /* +5e  7, 8, 9: see place_all            */
    unsigned char side;         /* +5f                                    */
    char pad_60[0x22];          /* +60  not read here                     */
    unsigned char icon_dim;     /* +82  bits 0-2: size                    */
    char pad_83[0x10];          /* +83  not read here                     */
    unsigned char control;      /* +93                                    */
    char pad_94[0xea];          /* +94  not read here                     */
    char active;                /* +17e                                   */
};

struct Combatant {
    int x;                      /* +0                                     */
    int y;                      /* +2                                     */
    char number;                /* +4                                     */
    unsigned char size;         /* +5   0: skipped                        */
};

struct Fallen {                 /* 9 bytes: a fallen combatant            */
    struct Hero far *hero;      /* +0                                     */
    int x;                      /* +4                                     */
    int y;                      /* +6                                     */
    char terrain;               /* +8   the terrain under it              */
};

struct Geo {                    /* the party's place                      */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
    char pad_3[5];              /* +3   not read here                     */
    struct Area far *area;      /* +8   d41c  the current area's record   */
};

struct Res;
struct Pool;
struct Patch;
struct Item;

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far hero_drop(char keep, char icon);                 /* 03d0:1e69 */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp); /* 05f5:01de */
extern void far res_copy(struct Res far *src, unsigned char n, char map,
                         struct Res far *dst, unsigned char m);  /* 05f5:0904 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern char far pool_lowbit(int value);                          /* 0a75:032f */
extern void far pic_load(char far *name, unsigned char set, unsigned char mode,
                         char n);                                /* 0cb5:0007 */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far input_press_discard(void);                                 /* 0d48:0110 */
extern void far fx_trigger_group(unsigned char when, struct Hero far * far *hp);
                                /* 18de:1111, vector to overlay 19 2e88:0449 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern char far wall_side(char x, char y, char dir);
                                /* 18de:1341, vector to overlay 13 26e3:09f3 */
extern char far wall_passable(char x, char y, char dir);
                                /* 18de:134b, vector to overlay 13 26e3:08f0 */
extern unsigned char far cell_zone_at(char x, char y);
                                /* 18de:1355, vector to overlay 13 26e3:0b87 */
extern void far combat_sides_count(void);                              /* 1cb8:091f */
extern void far combat_view_show(void);                              /* 1cb8:0c07 */
extern void far hostile_hp_percent_update(void);                           /* 1dbf:2840 */
extern void far combat_grid_build(void);                               /* 22fa:0331 */
extern char far combatant_x(struct Hero far *h);                 /* 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h);                 /* 22fa:0ae4 */
extern void far combat_direction_probe(struct Hero far *h, unsigned char dir, unsigned char far *who,
                            unsigned char far *cell, unsigned char far *kind);
                                                                 /* 22fa:0b9c */
extern char far combat_terrain_under(char x, char y);                     /* 22fa:130b */

extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern char combat_outdoors;                                           /* 6722 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char view_flag;                                           /* b40f */
extern unsigned char combat_round;                                  /* b418 */
extern int view_mode;                                            /* b421 */
extern char text_slow;                                           /* b431 */
extern char exploration_menu_context;                                           /* b4e8 */
extern struct Res far *icon_res;                                 /* b71c */
extern char magic_on;                                            /* b728 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern unsigned char side_count[];                               /* b8d1 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern unsigned char combat_round_limit;                                  /* bcf1 */
extern unsigned char current_spell;                                  /* be64 */
extern char cell_who[25][50];                                    /* be6a */
extern struct Map far *cur_map;                                  /* c66b */
extern struct Patch far *patch_list;                             /* ca0a */
extern struct Item far *dropped_item_boundary;                               /* ca16 */
extern unsigned char enemy_quarter_mask;                                  /* ca1a */
extern char attack_roll;                                           /* ca1d */
extern unsigned char combatant_count;                            /* cc73 */
extern char flag_cc74;                                           /* cc74 */
extern char flag_cc75;                                           /* cc75 */
extern struct Combatant combatants[];                            /* cc7c */
extern struct Fallen recs[8];                                    /* ce14 */
extern struct Pool combat_pool;                                  /* ce72 */
extern unsigned char rec_count;                                  /* cfd7 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Geo geo;                                           /* d414 */

static unsigned char mask_5908 = 2;
static unsigned char side_dirs[4][4] = { /* by quarter: 8, then three directions */
    { 8, 4, 6, 2 },
    { 8, 6, 4, 0 },
    { 8, 0, 6, 2 },
    { 8, 2, 0, 4 }
};
static unsigned char ring_dirs[4][4] = { /* by quarter and ring: 2 * base */
    { 0, 0, 2, 6 },
    { 2, 2, 0, 4 },
    { 4, 4, 2, 6 },
    { 6, 6, 4, 0 }
};
static unsigned char quarter_dir[4] = { 7, 2, 3, 6 };
static char corner_x[2][4] = {
    { 5, 4, 5, 6 },
    { 3, 8, 7, 2 }
};
static char corner_y[2][4] = {
    { 3, 2, 2, 3 },
    { 0, 2, 5, 3 }
};
static char spans[5][6][2] = {  /* by form_type and row: first, last column */
    { { 1, 0 }, { 1, 0 }, { 1, 0 }, { 2, 9 }, { 3, 10 }, { 4, 10 } },
    { { 0, 2 }, { 0, 3 }, { 1, 4 }, { 2, 5 }, { 3, 6 }, { 4, 7 } },
    { { 0, 6 }, { 0, 7 }, { 1, 8 }, { 1, 0 }, { 1, 0 }, { 1, 0 } },
    { { 3, 6 }, { 4, 7 }, { 5, 8 }, { 6, 9 }, { 7, 10 }, { 8, 10 } },
    { { 0, 6 }, { 0, 7 }, { 1, 8 }, { 2, 9 }, { 3, 10 }, { 4, 10 } }
};

static unsigned char num;
static char gx;
static char gy;
static char unused;
static char blk_x;
static char blk_y;
static unsigned char terrain;
static unsigned char form_type;
static char start_x[2];
static int east_wall;
static char start_y[2];
static long dummy_long;
static int west_wall;
static int active_side;
static int north_wall;
static int south_wall;
static char unused_byte;
static char side_facing[2];
static unsigned char forms_facing;
static unsigned char start_facing;
static unsigned char ring_widths[2];
static char formation_grid[2][4][6][11];
static unsigned char enemy_quarter;

static void cell_put(unsigned char dx, unsigned char dy,
                     unsigned char tile)                         /* 3482:0000 */
{
    char x;
    char y;

    x = blk_x * 6 + blk_y * 5 + dx + 0x15;
    y = blk_y * 5 + dy + 0xa;
    if (x >= 0 && x <= 0x31 && y >= 0 && y <= 0x18)
        cur_map->cell[y][x] = tile + 1;
}

static int wall_at(char x, char y, char dir)                     /* 3482:0072 */
{
    if (x >= 0 && x <= geo.area->cols - 1 && y >= 0 && y <= geo.area->rows - 1) {
        if (wall_passable(x, y, dir)) {
            if (wall_side(x, y, dir))
                return 3;
            return 0;
        }
        return 1;
    }
    if (y == geo.y && (dir == 2 || dir == 6))
        return 0;
    return 1;
}

static int wall_between(char x, char y, unsigned char dir)       /* 3482:00f2 */
{
    short here;
    short there;
    unsigned char back;

    back = (dir + 4) % 8;
    here = wall_at(x, y, dir);
    there = wall_at(x + dir_dx[dir], y + dir_dy[dir], back);
    return here | there;
}

static void block_floor(void)                                    /* 3482:0151 */
{
    unsigned char col;
    unsigned char row;

    for (row = 2; row <= 4; row++)
        for (col = 0; col <= 5; col++)
            cell_put(col, row, 0x16);
    if (west_wall == 1) {
        for (row = 2; row <= 4; row++) {
            cell_put(row - 1, row, 4);
            cell_put(row, row, 3);
            cell_put(row + 1, row, 0xd);
        }
    } else if (west_wall == 3) {
        cell_put(1, 2, 8);
        cell_put(5, 4, 0);
    }
}

static void block_north(void)                                    /* 3482:01fe */
{
    if (north_wall == 1) {
        cell_put(3, 0, 5);
        cell_put(4, 0, 5);
        cell_put(3, 1, 0xa);
        cell_put(4, 1, 0xa);
    } else {
        cell_put(3, 0, 0x16);
        cell_put(4, 0, 0x16);
        cell_put(3, 1, 0x16);
        cell_put(4, 1, 0x16);
    }
}

static void block_nw(char far *px, char far *py)                 /* 3482:0288 */
{
    unsigned char k;
    char ul;
    char ll;
    char ur;
    char lr;
    char open;

    if (!wall_between(*px, *py - 1, 6) && !wall_between(*px - 1, *py, 0))
        open = 1;
    else
        open = 0;
    ul = 0;
    for (k = 1; k <= 4; k++) {
        switch (k) {
        case 1:
            switch (north_wall) {
            case 0:
                switch (west_wall) {
                case 0:
                    ul = 0x16;
                    break;
                case 1:
                    if (open)
                        ul = 0;
                    else
                        ul = 0xd;
                    break;
                case 3:
                    ul = 0xd;
                    break;
                }
                break;
            case 1:
            case 3:
                if (west_wall == 0) {
                    if (open)
                        ul = 0xf;
                    else
                        ul = 5;
                } else if (open)
                    ul = 0x12;
                else
                    ul = 2;
                break;
            }
            break;
        case 2:
            switch (north_wall) {
            case 0:
                ur = 0x16;
                break;
            case 3:
                ur = 0x11;
                break;
            case 1:
                ur = 5;
                break;
            }
            break;
        case 3:
            switch (west_wall) {
            case 0:
                if (north_wall == 0)
                    ll = 0x16;
                else if (open)
                    ll = 0x10;
                else
                    ll = 0xa;
                break;
            case 3:
                if (open)
                    ll = 0x14;
                else
                    ll = 7;
                break;
            case 1:
                if (open)
                    ll = 1;
                else
                    ll = 3;
                break;
            }
            break;
        case 4:
            switch (west_wall) {
            case 0:
            case 3:
                switch (north_wall) {
                case 0:
                    lr = 0x16;
                    break;
                case 3:
                    lr = 0x17;
                    break;
                case 1:
                    lr = 0xa;
                    break;
                }
                break;
            case 1:
                switch (north_wall) {
                case 0:
                    lr = 0xd;
                    break;
                case 3:
                    lr = 0x15;
                    break;
                case 1:
                    lr = 6;
                    break;
                }
                break;
            }
            break;
        }
    }
    cell_put(1, 0, ul);
    cell_put(2, 0, ur);
    cell_put(1, 1, ll);
    cell_put(2, 1, lr);
}

static void block_ne(char far *px, char far *py)                 /* 3482:04a1 */
{
    unsigned char k;
    char ul;
    char ll;
    char ur;
    char lr;
    char open;
    short up;
    short right;

    up = wall_between(*px, *py - 1, 2);
    right = wall_between(*px + 1, *py, 0);
    if (up == 0 && right == 0)
        open = 1;
    else
        open = 0;
    for (k = 1; k <= 4; k++) {
        switch (k) {
        case 1:
            switch (north_wall) {
            case 0:
                if (up == 1)
                    ul = 4;
                else
                    ul = 0x16;
                break;
            case 3:
                ul = 0xf;
                break;
            case 1:
                ul = 5;
                break;
            }
            break;
        case 2:
            if (north_wall == 0) {
                switch (up) {
                case 0:
                    ur = 0x16;
                    break;
                case 3:
                    if (east_wall == 0 && right != 0)
                        ur = 0x18;
                    else if (east_wall == 0)
                        ur = 7;
                    else
                        ur = 1;
                    break;
                case 1:
                    if (east_wall == 0) {
                        if (right != 0)
                            ur = 0xb;
                        else
                            ur = 7;
                    } else
                        ur = 3;
                    break;
                }
            } else if (east_wall != 0)
                ur = 9;
            else if (right != 0)
                ur = 5;
            else if (open)
                ur = 0x11;
            else
                ur = 0x13;
            break;
        case 3:
            switch (north_wall) {
            case 0:
                ll = 0x16;
                break;
            case 3:
                ll = 0x10;
                break;
            case 1:
                ll = 0xa;
                break;
            }
            break;
        case 4:
            if (north_wall == 0) {
                if (up == 0)
                    lr = 0x16;
                else if (east_wall != 0)
                    lr = 4;
                else if (right == 0)
                    lr = 8;
                else
                    lr = 0xc;
            } else if (east_wall != 0)
                lr = 0xe;
            else if (right == 0)
                lr = 0x17;
            else
                lr = 0xa;
            break;
        }
    }
    cell_put(5, 0, ul);
    cell_put(6, 0, ur);
    cell_put(5, 1, ll);
    cell_put(6, 1, lr);
}

static void dungeon_map(void)                                    /* 3482:06aa */
{
    char x;
    char y;

    mem_fill(cur_map->cell, 0x4e2, 0x16);
    for (blk_y = -2; blk_y <= 2; blk_y++)
        for (blk_x = -6; blk_x <= 6; blk_x++) {
            x = geo.x + blk_x;
            y = geo.y + blk_y;
            west_wall = wall_between(x, y, 6);
            north_wall = wall_between(x, y, 0);
            east_wall = wall_between(x, y, 2);
            south_wall = wall_between(x, y, 4);
            block_floor();
            block_north();
            block_nw(&x, &y);
            block_ne(&x, &y);
        }
}

static unsigned char terrain_flags(void)                         /* 3482:0786 */
{
    unsigned char flags[32] = {
        0x00, 0x18, 0x11, 0x15, 0x01, 0x01, 0x60, 0x14,
        0x08, 0x01, 0x00, 0x21, 0x71, 0x09, 0x06, 0x04,
        0x01, 0x09, 0x09, 0x08, 0x59, 0x00, 0x11, 0x11,
        0x00, 0x00, 0x01, 0x11, 0x00, 0x00, 0x20, 0x20
    };

    return flags[terrain];
}

static void bank_put(char x, char y, unsigned char far *run)     /* 3482:07b1 */
{
    cur_map->cell[y][x] = 0x3a;
    if (x < 0x31) {
        cur_map->cell[y][x + 1] = fx_roll_dice(1, 2) + 0x3a;
        if (x + 1 < 0x31)
            cur_map->cell[y][x + 2] = fx_roll_dice(1, 2) + 0x3a;
        if (x + 2 < 0x31)
            cur_map->cell[y][x + 3] = 0x3d;
    }
    *run = 1;
}

static void wild_river(void)                                     /* 3482:0863 */
{
    unsigned char run;
    char col;
    char row;

    if (!cursor_obj[0x33])
        return;
    col = 0x22 - fx_roll_dice(5, 4);
    while ((col + 2) % 7 > 0)
        col--;
    run = 1;
    for (row = 0; row <= 0x18; row++) {
        if (col <= 0x31) {
            cur_map->cell[row][col] = 0x34;
            if (col < 0x31) {
                if (fx_roll_dice(1, 0x14) <= run)
                    bank_put(col + 1, row, &run);
                else {
                    run++;
                    cur_map->cell[row][col + 1] = fx_roll_dice(1, 2) + 0x34;
                    if (col + 1 < 0x31)
                        cur_map->cell[row][col + 2] = 0x37;
                    if (col + 2 < 0x31)
                        cur_map->cell[row][col + 3] = 0x37;
                    if (col + 3 < 0x31)
                        cur_map->cell[row][col + 4] = fx_roll_dice(1, 2) + 0x37;
                }
            }
            if (col + 4 < 0x31)
                cur_map->cell[row][col + 5] = 0x34;
            col++;
        }
    }
}

static void wild_woods(void)                                     /* 3482:09ea */
{
    char col;
    char row;
    char chance;

    if (terrain_flags() & 0x80)
        return;
    chance = 5;
    if ((char)(terrain_flags() & mask_5908))
        chance -= 3;
    if (terrain_flags() & 4)
        chance -= 3;
    if (terrain_flags() & 0x40)
        chance += 2;
    if (terrain_flags() & 8)
        chance += 5;
    if (chance < 0)
        chance = 1;
    for (col = 0; col <= 0x31; col++)
        for (row = 1; row <= 0x18; row++)
            if ((tile_info - 1)[cur_map->cell[row][col]].kind == 0x25
                && (tile_info - 1)[cur_map->cell[row - 1][col]].kind == 0x25
                && fx_roll_dice(1, 100) <= chance) {
                if (fx_roll_dice(1, 100) <= chance)
                    cur_map->cell[row][col] = fx_roll_dice(1, 2) + 0x29;
                else {
                    cur_map->cell[row - 1][col] = fx_roll_dice(1, 5) + 0x1f;
                    cur_map->cell[row][col] = fx_roll_dice(1, 5) + 0x24;
                }
            }
}

static void scatter_tile(char x, char y, unsigned char p1, unsigned char p2,
                         unsigned char p3, unsigned char p4,
                         unsigned char p5);

static void wild_scatter(void)                                   /* 3482:0b7c */
{
    char col;
    char row;
    int n;

    n = 0x32;
    if (terrain_flags() & 0x10)
        n += 10;
    if (terrain_flags() & 0x20)
        n += 0x1e;
    if (terrain_flags() & 0x40)
        n += 0x14;
    if (terrain_flags() & 4)
        n -= 10;
    if ((char)(terrain_flags() & mask_5908))
        n -= 0x14;
    if (terrain_flags() & 0x80)
        n -= 0x32;
    for (col = 0; col <= 0x31; col++)
        for (row = 0; row <= 0x18; row++)
            if ((tile_info - 1)[cur_map->cell[row][col]].kind == 0x25) {
                if (n >= -0x1e && n <= 9)
                    scatter_tile(col, row, 0, 0, 0, 0x1e, 0xf);
                if (n >= 10 && n <= 0x1d)
                    scatter_tile(col, row, 0, 1, 5, 0x14, 0xa);
                if (n >= 0x1e && n <= 0x45)
                    scatter_tile(col, row, 0, 2, 5, 10, 5);
                if (n >= 0x46 && n <= 0x59)
                    scatter_tile(col, row, 10, 2, 10, 10, 1);
                if (n >= 0x5a && n <= 0x6e)
                    scatter_tile(col, row, 0xf, 5, 0xf, 10, 1);
            }
}

static void scatter_tile(char x, char y, unsigned char p1, unsigned char p2,
                         unsigned char p3, unsigned char p4,
                         unsigned char p5)                       /* 3482:0ce6 */
{
    unsigned char roll;

    roll = fx_roll_dice(1, 100);
    if (roll <= p1)
        cur_map->cell[y][x] = fx_roll_dice(1, 2) + 0x31;
    else if (roll <= p1 + p2 + 1)
        cur_map->cell[y][x] = 0x2c;
    else if (roll <= p1 + p2 + 1)       /* the bound above again: 3482:0d64 */
        cur_map->cell[y][x] = fx_roll_dice(1, 5) + 0x2c;
    else if (roll <= p1 + p2 + p3 + p4)
        cur_map->cell[y][x] = 0x44;
    else if (roll <= p1 + p2 + p3 + p4 + p5)
        cur_map->cell[y][x] = fx_roll_dice(1, 6) + 0x3d;
}

static void wild_map(void)                                       /* 3482:0e3e */
{
    mem_fill(cur_map->cell, 0x4e2, 0x45);
    terrain = cursor_obj[0x1a2];
    wild_river();
    wild_woods();
    wild_scatter();
}

static void map_build(void)                                      /* 3482:0e73 */
{
    unsigned char set;
    struct Res far *res;
    short i;

    set = 1;
    res = 0;
    set_open_mode('2');
    tile_open_file("COMSPR", 0xff, 1, 0, &res);
    res_palette_load(res);
    for (i = 0xf; i <= 0x25; i++) {
        res_copy(res, i - 0xf, 1, icon_res, i);
        res_copy(res, i + 0x71, 1, icon_res, i + 0x26);
    }
    res_free(&res);
    if (cursor_obj[0x22]) {
        set = geo.area->combat_sets[combat_outdoors];
        if (set == 0)
            set = 1;
        if (combat_outdoors) {
            cursor_obj[0x22] = 0;
            cursor_obj[0x24] = 1;
            pic_load("WildCom1", set, 0, 0x27);
        } else
            pic_load("DungCom1", set, 0, 0x19);
    } else {
        set = geo.area->zone_wild_sets[cell_zone_at(cursor_obj[0x25], cursor_obj[0x26])];
        if (set == 0)
            set = 1;
        pic_load("WildCom1", set, 0, 0x27);
    }
    cur_map->cursor_on = 0;
    cur_map->cursor_size = 1;
    cur_map->ignore_blocking = 0;
    if (cursor_obj[0x22])
        dungeon_map();
    else
        wild_map();
}

static void party_ready(void)                                    /* 3482:0fe1 */
{
    struct Hero far *h;
    unsigned char count;
    unsigned char mor;

    h = node_list;
    count = 0;
    while (h != 0) {
        recalc_stats(h);
        count++;
        pool_alloc((void far * far *)&h->fight, sizeof(struct Fight), &combat_pool);
        mem_fill(h->fight, sizeof(struct Fight), 0);
        if (cursor_obj[0x20] < count)
            h->fight->not_pc = 1;
        h->fight->facing = quarter_dir[geo.facing / 2];
        if (h->side == 1)
            h->fight->facing = (h->fight->facing + 4) % 8;
        mor = (h->control & 0x7f) * 2;
        if (h->side == 0 && h->fight->not_pc == 1 && (mor == 0 || mor > 0x66))
            h->control = cursor_obj[0x20] * 4 + 0x80;
        h->fight->byte_17 = 1;
        h = h->next;
    }
}

static char outside(char x, char y)                              /* 3482:1105 */
{
    if (x >= 0 && x < 11 || y >= 0 && y < 6)
        return 0;
    return 1;
}

static char spot_try(unsigned char who, char x, char y, char bx, char by,
                     unsigned char ring)                         /* 3482:1128 */
{
    unsigned char blocked;
    unsigned char cell;
    unsigned char kind;

    if (x < 0 || y < 0 || x > 10 || y > 5)
        return 0;
    if (formation_grid[active_side][ring][y][x] == 0)
        return 0;
    (combatants - 1)[who].x = bx * 6 + x + by * 5 + 0x16;
    (combatants - 1)[who].y = by * 5 + y + 0xa;
    combat_direction_probe((heroes - 1)[who], 8, &blocked, &cell, &kind);
    if (!blocked && cell > 0 && (tile_info - 1)[cell].move_cost < 0xff) {
        formation_grid[active_side][ring][y][x] = 0;
        return 1;
    }
    return 0;
}

static char place_one(unsigned char who)                         /* 3482:1277 */
{
    char placed;
    char first;
    char fail;
    char off;
    char blocked;
    char r;
    unsigned char state;
    unsigned char dir;
    unsigned char base;
    unsigned char k;
    char ddx;
    char ddy;
    char px;
    char py;
    unsigned char n;
    unsigned char step;
    char ox;
    char oy;
    unsigned char tries;
    unsigned char ring;
    char x0;
    char y0;
    char x;
    char y;

    first = 1;
    fail = 0;
    state = 1;
    n = 0;
    ring = 0;
    ox = start_x[active_side];
    oy = start_y[active_side];
    do {
        base = ring_dirs[side_facing[active_side]][ring] / 2;
        switch (state) {
        case 1:
            ddx = dir_dx[quarter_dir[(base + 2) % 4]];
            ddy = dir_dy[quarter_dir[(base + 2) % 4]];
            x0 = corner_x[ring > 0][base] + ddx * n;
            y0 = corner_y[ring > 0][base] + ddy * n;
            x = x0;
            y = y0;
            step = 1;
            state = 2;
            tries = 1;
            break;
        case 2:
            ddx = dir_dx[quarter_dir[(base + 1) % 4]];
            ddy = dir_dy[quarter_dir[(base + 1) % 4]];
            x = ddx * step + x0;
            y = ddy * step + y0;
            state = 3;
            tries++;
            break;
        case 3:
            ddx = dir_dx[quarter_dir[(base + 3) % 4]];
            ddy = dir_dy[quarter_dir[(base + 3) % 4]];
            x = ddx * step + x0;
            y = ddy * step + y0;
            state = 2;
            step++;
            tries++;
            break;
        }
        if (x < 0 || y < 0 || x > 10 || y > 5)
            off = 1;
        else
            off = 0;
        if (state > 1 && (off && !outside(x, y)
                          || first && ring_widths[active_side] <= tries
                          || !first && tries > 0xb)) {
            n++;
            if (active_side == 0 && pool_lowbit(side_facing[0]) && ring == 0 && n == 1) {
                px = start_x[active_side] + geo.x;
                py = start_y[active_side] + geo.y;
                blocked = 0;
                for (k = 1; k <= 3; k++) {
                    dir = side_dirs[side_facing[active_side]][k];
                    if (view_mode == 3 || wall_between(px, py, dir) != 1)
                        blocked = 1;
                }
                if (blocked)
                    n++;
            }
            state = 1;
            first = 0;
        }
        if (off && outside(x, y)) {
            placed = 0;
            state = 0;
            while (ring < 3 && state != 1) {
                ring++;
                px = start_x[active_side] + geo.x;
                py = start_y[active_side] + geo.y;
                dir = side_dirs[side_facing[active_side]][ring];
                if (view_mode == 3 || wall_between(px, py, dir) != 1) {
                    ox = start_x[active_side] + dir_dx[dir];
                    oy = start_y[active_side] + dir_dy[dir];
                    n = 0;
                    state = 1;
                }
            }
            if (state != 1)
                fail = 1;
        }
        if (!off)
            placed = spot_try(who, x, y, ox, oy, ring);
    } while (!placed && !fail);
    if (fail)
        r = 0;
    else
        r = 1;
    return r;
}

static void formation_set(void)                                  /* 3482:169c */
{
    unsigned char ring;
    unsigned char gap;
    short s;

    if (!cursor_obj[0x37] && geo.facing != start_facing)
        gap = 1;
    else
        gap = cursor_obj[0x37];
    if (geo.facing == forms_facing)
        return;
    forms_facing = geo.facing;
    side_facing[0] = geo.facing / 2;
    start_x[1] = dir_dx[geo.facing] * gap + start_x[0];
    start_y[1] = dir_dy[geo.facing] * gap + start_y[0];
    side_facing[1] = (geo.facing + 4) % 8 / 2;
    for (s = 0; s <= 1; s++)
        for (ring = 0; ring <= 3; ring++) {
            if (ring == 1)
                form_type = 4;
            else
                form_type = side_facing[s];
            for (gy = 0; gy <= 5; gy++)
                for (gx = 0; gx <= 10; gx++)
                    if (spans[form_type][gy][0] > gx || spans[form_type][gy][1] < gx)
                        formation_grid[s][ring][gy][gx] = 0;
                    else
                        formation_grid[s][ring][gy][gx] = 1;
        }
}

static void place_all(void)                                      /* 3482:184d */
{
    struct Hero far *prev;
    struct Hero far *h;
    unsigned char kept;
    unsigned char dropped;
    unsigned char sides;
    unsigned char dir;
    char ok;

    start_facing = geo.facing;
    forms_facing = 0xff;
    enemy_quarter = 0;
    combat_sides_count();
    dropped = 0;
    kept = 0;
    for (num = 1; num <= 0x44; num++)
        (combatants - 1)[num].size = 0;
    mem_fill(cell_who, 0x4e2, 0);
    start_x[0] = 0;
    start_y[0] = 0;
    ring_widths[0] = (side_count[0] + 1) / 2;
    sides = 0;
    if (enemy_quarter_mask & 0xf) {
        if (enemy_quarter_mask & 1)
            sides++;
        if (enemy_quarter_mask & 2)
            sides++;
        if (enemy_quarter_mask & 4)
            sides++;
        if (enemy_quarter_mask & 8)
            sides++;
    } else
        sides++;
    sides = sides * 2;
    ring_widths[1] = (side_count[1] + sides - 1) / sides;
    formation_set();
    num = 1;
    combatant_count = 1;
    h = prev = node_list;
    while (h != 0) {
        input_press_discard();
        (heroes - 1)[num] = h;
        active_side = h->side;
        (combatants - 1)[num].number = num;
        (combatants - 1)[num].size = h->icon_dim & 7;
        if (enemy_quarter_mask & 0xf) {
            if (h->side == 0)
                geo.facing = start_facing;
            else {
                do
                    enemy_quarter = (enemy_quarter + 1) & 3;
                while (!(enemy_quarter_mask & 1 << enemy_quarter));
                geo.facing = enemy_quarter * 2;
            }
            formation_set();
        }
        dir = geo.facing;
        if (h->side == 1)
            dir = (dir + 4) % 8;
        h->fight->facing = quarter_dir[dir / 2];
        ok = place_one(num);
        if (ok) {
            prev = h;
            if (!h->active) {
                (combatants - 1)[num].size = 0;
                if (!h->fight->not_pc && h->status != 8) {
                    gx = (combatants - 1)[num].x;
                    gy = (combatants - 1)[num].y;
                    rec_count++;
                    (recs - 1)[rec_count].terrain = combat_terrain_under(gx, gy);
                    cur_map->cell[gy][gx] = h->status == 7 ? 0x1e : 0x1f;
                    (recs - 1)[rec_count].hero = h;
                    (recs - 1)[rec_count].x = gx;
                    (recs - 1)[rec_count].y = gy;
                }
            }
            if (h->status == 9) {
                h->active = 0;
                (combatants - 1)[num].size = 0;
            }
            combat_grid_build();
            num++;
            combatant_count++;
            kept++;
        } else {
            (combatants - 1)[num].size = 0;
            if (h->fight->not_pc == 1) {
                dropped++;
                (heroes - 1)[num] = 0;
                cur_node = h;
                hero_drop(1, 0);
                h = prev;
            } else {
                prev = h;
                num++;
                combatant_count++;
            }
        }
        h = h->next;
    }
    prev->next = 0;
    geo.facing = start_facing;
}

void far combat_setup(void)                                      /* 3482:1c4d */
{
    struct Hero far *h;

    text_slow = 0;
    view_flag = 0xff;
    magic_on = 0;
    combat_round = 0;
    combat_round_limit = 0xf;
    attack_roll = 0;
    rec_count = 0;
    patch_list = 0;
    dropped_item_boundary = 0;
    mem_zero(recs, sizeof recs);
    cursor_obj[0x36] = 0;
    map_build();
    input_press_discard();
    party_ready();
    input_press_discard();
    place_all();
    input_press_discard();
    cur_map->view_x = combatant_x(node_list) - 3;
    cur_map->view_y = combatant_y(node_list) - 3;
    combat_view_show();
    for (h = node_list; h != 0; h = h->next) {
        h->fight->turn_attempts = 0;
        fx_trigger_group(8, &h);
    }
    hostile_hp_percent_update();
    flag_cc75 = 0;
    flag_cc74 = 0;
    view_mode = 5;
    exploration_menu_context = 0;
    h = node_list;
    current_spell = 0;
}
