/* CKIT.EXE overlay 02 module 2632: lines over the map and closing in on a cell.
 * Stepping a line cell by cell, testing whether a line stays clear of the
 * terrain and within a cost, whether a cell lies ahead of another in a
 * given direction, and planning, for each combatant, the cheapest clear
 * approach to a target's cells.
 *
 * EXE 0x02b21c..0x02bd1c, one Turbo C large-model _TEXT segment starting at
 * byte c of paragraph 2632, the last module of RTLink section 2 (payload
 * 0x29710, link segment 2482; a child of section 0).  The first 12 bytes of
 * paragraph 2632 are the tail of module 2482; four zero bytes follow to the
 * section's end at 0x2bd20.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 2.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  icon_cell_delta is section
 * 0's, reached by a direct far call because section 0 is section 2's
 * parent.  Six routines are also reached from other sections through the
 * vector stubs 18de:1265, 126f, 1279, 1283, 12bf and 130f, the mod18de_
 * names of cbtutil.c, spellfx.c and others.
 *
 * _DATA is dir_of, DGROUP 5fca..5fd2.  The module has no _BSS.
 *
 * tile_info, steps and combatants are indexed from 1.  The shipped code
 * adds the address of element 0 (5ff1, ca28, cc76), which falls on other
 * variables; (steps - 1)[i] folds to that displacement, where
 * steps[i - 1] computes i - 1 at run time.
 *
 * Directions run 0 (north) clockwise to 7 (north-west), as dir_dx and
 * dir_dy step them; 8 is no direction.  A line's steps count 2 for a
 * straight step and 3 for a diagonal one.
 */

#include <stdlib.h>

struct LineWalk {
    int x0;                     /* +0                                     */
    int y0;                     /* +2                                     */
    int x1;                     /* +4                                     */
    int y1;                     /* +6                                     */
    int err;                    /* +8                                     */
    int dx;                     /* +a   |x1 - x0|                         */
    int dy;                     /* +c   |y1 - y0|                         */
    int x;                      /* +e                                     */
    int y;                      /* +10                                    */
    int sx;                     /* +12  -1, 0 or 1                        */
    int sy;                     /* +14                                    */
    unsigned char steps;        /* +16                                    */
    char dir;                   /* +17  direction of the last step        */
};

struct Map {
    char pad_0[8];              /* +0   not read here                     */
    char ignore_blocking;       /* +8   nonzero: terrain blocks nothing   */
    unsigned char cell[25][50]; /* +9   tile_info index from 1, by row    */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   not read here                     */
    unsigned char height;       /* +1                                     */
    unsigned char block;        /* +2   a line lower than this is stopped */
    char kind;                  /* +3   not read here                     */
};

struct Step {
    unsigned char who;          /* +0   index into combatants             */
    unsigned char cost;         /* +1                                     */
    unsigned char dir;          /* +2                                     */
};

struct Combatant {
    int x;                      /* +0                                     */
    int y;                      /* +2                                     */
    char number;                /* +4   not read here                     */
    unsigned char size;         /* +5   0: skipped                        */
};

extern char far icon_cell_delta(unsigned char size, unsigned char n,
                         char far *dx, char far *dy); /* overlay 0 22fa:000b */

extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern struct Map far *cur_map;                                  /* c66b */
extern unsigned char step_count;                                 /* ca10 */
extern struct Step steps[];                                      /* ca2b */
extern unsigned char combatant_count;                            /* cc73 */
extern struct Combatant combatants[];                            /* cc7c */

char dir_of[3][3] = {           /* [sy + 1][sx + 1]                       */
    { 7, 0, 1 },
    { 6, 8, 2 },
    { 5, 4, 3 }
};

/* The translated Pascal unit's empty initialization block (DKK pathing.pas). */
void far approach_unit_init(void)                                     /* 3632:000c */
{
}

int far int_sign(int v)                                          /* 3632:000d */
{
    int r;

    if (v < 0)
        r = -1;
    else if (v > 0)
        r = 1;
    else
        r = 0;
    return r;
}

void far steps_sort(void)                                        /* 3632:002e */
{
    unsigned char i;
    unsigned char j;
    unsigned char dir_j;
    unsigned char dir_i;
    struct Step tmp;

    if (step_count > 1) {
        for (i = 1; i <= step_count - 1; i++) {
            for (j = i + 1; j <= step_count; j++) {
                dir_i = (steps - 1)[i].dir;
                dir_j = (steps - 1)[j].dir;
                if ((steps - 1)[j].cost < (steps - 1)[i].cost
                    || (steps - 1)[j].cost == (steps - 1)[i].cost && dir_j < dir_i
                       && dir_j % 2 <= dir_i % 2) {
                    tmp = (steps - 1)[i];
                    (steps - 1)[i] = (steps - 1)[j];
                    (steps - 1)[j] = tmp;
                }
            }
        }
    }
}

void far line_init(struct LineWalk far *l)                       /* 3632:01a0 */
{
    l->x = l->x0;
    l->y = l->y0;
    l->dx = abs(l->x1 - l->x0);
    l->dy = abs(l->y1 - l->y0);
    l->sx = int_sign(l->x1 - l->x0);
    l->sy = int_sign(l->y1 - l->y0);
    l->err = 0;
    l->steps = 0;
}

char far line_step(struct LineWalk far *l)                       /* 3632:020c */
{
    char moved;
    char xs;
    char ys;

    moved = 0;
    xs = 1;
    ys = 1;
    if (l->dx >= l->dy) {
        if (l->x != l->x1) {
            l->x += l->sx;
            xs = l->sx + 1;
            l->err += l->dy;
            l->err += l->dy;
            l->steps++;
            l->steps++;
            if (l->err >= l->dx) {
                l->err -= l->dx;
                l->err -= l->dx;
                l->y += l->sy;
                ys = l->sy + 1;
                l->steps++;
            }
            moved = 1;
        }
    } else if (l->y != l->y1) {
        l->y += l->sy;
        ys = l->sy + 1;
        l->err += l->dx;
        l->err += l->dx;
        l->steps++;
        l->steps++;
        if (l->err >= l->dy) {
            l->err -= l->dy;
            l->err -= l->dy;
            l->x += l->sx;
            xs = l->sx + 1;
            l->steps++;
        }
        moved = 1;
    }
    l->dir = dir_of[ys][xs];
    return moved;
}

char far line_clear(char x, char y, char far *tx, char far *ty,
                    unsigned short far *cost)                    /* 3632:0325 */
{
    char ok;
    struct LineWalk path;
    struct LineWalk rise;
    char done;
    unsigned short limit;

    limit = *cost * 2 + 1;
    path.x0 = x;
    path.y0 = y;
    path.x1 = *tx;
    path.y1 = *ty;
    line_init(&path);
    rise.x0 = 0;
    rise.y0 = (tile_info - 1)[cur_map->cell[y][x]].height;
    if (path.dx > path.dy)
        rise.x1 = path.dx;
    else
        rise.x1 = path.dy;
    rise.y1 = (tile_info - 1)[cur_map->cell[y][x]].height;   /* start cell again: 2632:03ac */
    line_init(&rise);
    done = 0;
    do {
        if (!cur_map->ignore_blocking
            && (tile_info - 1)[cur_map->cell[path.y][path.x]].block > rise.y
            || path.steps > limit) {
            *tx = path.x;
            *ty = path.y;
            *cost = path.steps;
            ok = 0;
            return ok;
        }
        line_step(&rise);
        done = !line_step(&path);
    } while (!done);
    *cost = path.steps;
    ok = 1;
    return ok;
}

char far dir_ok(char x, char y, char x2, char y2, unsigned char dir) /* 3632:048c */
{
    char ok;
    char nx;
    char ny;

    if (x < 0 || x > 0x31 || y < 0 || y > 0x18
        || x2 < 0 || x2 > 0x31 || y2 < 0 || y2 > 0x18) {
        ok = 0;
    } else {
        if (dir == 0xff)
            dir = 8;
        nx = dir_dx[dir] + x;
        ny = dir_dy[dir] + y;
        if (x == x2 && y == y2)
            ok = 1;
        else if (nx == x2 && ny == y2)
            ok = 1;
        else switch (dir) {
        case 0:
            ok = x2 >= nx && nx - x2 + ny >= y2
                 || x2 <= nx && x2 - nx + ny >= y2;
            break;
        case 1:
            ok = x2 >= nx && nx - x2 + ny >= y2
                 || nx + ny - y2 <= x2 && y2 <= ny;
            break;
        case 2:
            ok = nx + ny - y2 <= x2 && y2 <= ny
                 || nx + y2 - ny <= x2 && y2 >= ny;
            break;
        case 3:
            ok = nx + y2 - ny <= x2 && y2 >= ny
                 || x2 >= nx && x2 - nx + ny <= y2;
            break;
        case 4:
            ok = x2 >= nx && x2 - nx + ny <= y2
                 || x2 <= nx && nx - x2 + ny <= y2;
            break;
        case 5:
            ok = x2 <= nx && nx - x2 + ny <= y2
                 || nx + ny - y2 >= x2 && y2 >= ny;
            break;
        case 6:
            ok = nx + ny - y2 >= x2 && y2 >= ny
                 || nx + y2 - ny >= x2 && y2 <= ny;
            break;
        case 7:
            ok = nx + y2 - ny >= x2 && y2 <= ny
                 || x2 <= nx && x2 - nx + ny >= y2;
            break;
        case 8:
            ok = 1;
            break;
        }
    }
    return ok;
}

void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                    unsigned char size)                          /* 3632:07d4 */
{
    unsigned char k;
    unsigned char d;
    unsigned char i;
    unsigned char j;
    unsigned char best_i;
    unsigned char best_j;
    char found;
    char tx;
    char ty;
    char dx;
    char dy;
    char to_x[6];
    char to_y[6];
    char from_x[6];
    char from_y[6];
    unsigned short cost;
    unsigned short best;

    for (i = 0; i <= 5; i++) {
        if (icon_cell_delta(size, i, &dx, &dy)) {
            to_x[i] = x + dx;
            to_y[i] = y + dy;
        } else
            to_x[i] = -1;
    }
    step_count = 0;
    for (k = 1; k <= combatant_count; k++) {
        if ((combatants - 1)[k].size > 0) {
            found = 0;
            best = 0xff;
            for (j = 0; j <= 5; j++) {
                if (icon_cell_delta((combatants - 1)[k].size, j, &dx, &dy)) {
                    from_x[j] = (combatants - 1)[k].x + dx;
                    from_y[j] = (combatants - 1)[k].y + dy;
                } else
                    from_x[j] = -1;
            }
            for (j = 0; j <= 5; j++) {
                if (from_x[j] >= 0) {
                    for (i = 0; i <= 5; i++) {
                        if (to_x[i] >= 0
                            && dir_ok(to_x[i], to_y[i], from_x[j], from_y[j], dir)) {
                            tx = from_x[j];
                            ty = from_y[j];
                            cost = budget;
                            if (line_clear(to_x[i], to_y[i], &tx, &ty, &cost)) {
                                found = 1;
                                if (cost < best) {
                                    best = cost;
                                    best_i = i;
                                    best_j = j;
                                }
                            }
                        }
                    }
                }
            }
            if (found) {
                step_count++;
                (steps - 1)[step_count].who = k;
                (steps - 1)[step_count].cost = best;
                d = 0;
                if (dir < 8)
                    d = dir;
                else
                    while (!dir_ok(to_x[best_i], to_y[best_i],
                                   from_x[best_j], from_y[best_j], d))
                        d++;
                (steps - 1)[step_count].dir = d;
            }
        }
    }
    steps_sort();
}
