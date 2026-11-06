/* CKIT.EXE overlay 00 module 22fa: the combat map's view.
 * Each combatant's cells and slot, its cell and icon size, the seven-by-
 * seven window over the 50-by-25 combat map -- scrolling it to keep a cell
 * in view, drawing the terrain, the cursor and the icons in it -- probing
 * the cells a combatant would step into, placing a combatant, a combatant
 * falling, the terrain under the patches and the fallen, and the direction
 * from one cell to another.
 *
 * EXE 0x025e4a..0x0276c0, one Turbo C large-model _TEXT segment starting at
 * byte a of paragraph 22fa, the last module of RTLink section 0 (payload
 * 0x1e2b0, link segment 1b41; a root section, the parent of 1 and 2).  The
 * first ten bytes of paragraph 22fa end the routine at 1dbf:52a2, whose
 * fixups carry frame 1dbf; the module's last retf is the section's last
 * byte.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined
 * in shipped order; bake consumes them in that order.  Nothing found
 * reaches cbview_unit_init.
 *
 * A bare stored seg:off in this file means section 0.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and cbview_NNNN only where in this module
 * the routine starts.
 *
 * The module has no _DATA and no _BSS; every table it reads is extern.
 * <stdlib.h> is for abs, which Turbo C compiles inline.
 *
 * combatants, heroes, recs and tile_info are indexed from 1; the shipped
 * code adds the address of element 0 (cc76, b8f1, ce0b, 5ff1).  grid_x,
 * grid_y and patch_dir are indexed from 1 at their own addresses.
 *
 * Directions run 0 (north) clockwise to 7 (north-west), as dir_dx and
 * dir_dy step them; 8 is no direction.  A view cell is 12 pixels on a side,
 * and 0x1f44 is the coordinate glyph_at and the clip rectangle take for the
 * view's top-left corner; icon_draw takes a view cell and adds it itself.
 */

#include <stdlib.h>

struct IconCell {               /* one cell of an icon, from its corner   */
    char dx;                    /* +0   -1: no more cells                 */
    char dy;                    /* +1                                     */
};

struct Map {
    char pad_0[2];              /* +0   not read here                     */
    int view_x;                 /* +2   the view's left column            */
    int view_y;                 /* +4   the view's top row                */
    char cursor_on;             /* +6   nonzero: draw the cursor          */
    char cursor_size;           /* +7   the cursor's icon size            */
    char ignore_blocking;       /* +8   not read here                     */
    unsigned char cell[25][50]; /* +9   tile_info index from 1, by row    */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    char pad_1[2];              /* +1   not read here                     */
    char kind;                  /* +3   glyph                             */
};

struct Fight {                  /* a combatant's state in a fight         */
    char pad_0[3];              /* +0   not read here                     */
    int initiative;             /* +3                                     */
    char pad_5[2];              /* +5   not read here                     */
    unsigned char moves;        /* +7   half-moves left                   */
    char guarding;              /* +8                                     */
    char pad_9;                 /* +9   not read here                     */
    unsigned char facing;       /* +a   facing                            */
    char pad_b[9];              /* +b   not read here                     */
    char not_pc;                /* +14                                    */
};

struct Hero {                   /* a combatant                            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[0x3c];           /* +4   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x1a];          /* +44  not read here                     */
    unsigned char status;       /* +5e                                    */
    char pad_5f[0x23];          /* +5f  not read here                     */
    unsigned char icon_dim;     /* +82  bits 0-6: icon size               */
    char pad_83[0x3a];          /* +83  not read here                     */
    unsigned char icon_slot;    /* +bd                                    */
    char pad_be[0xc0];          /* +be  not read here                     */
    char active;                /* +17e                                   */
};

struct Combatant {
    int x;                      /* +0                                     */
    int y;                      /* +2                                     */
    char number;                /* +4   not read here                     */
    unsigned char size;         /* +5   icon size; 0: off the map         */
};

struct Fallen {                 /* 9 bytes: a fallen combatant            */
    struct Hero far *hero;      /* +0                                     */
    int x;                      /* +4                                     */
    int y;                      /* +6                                     */
    char terrain;               /* +8   the terrain under it              */
};

struct Patch {                  /* a patch of changed terrain             */
    char pad_0[4];              /* +0   not read here                     */
    struct Patch far *next;     /* +4                                     */
    unsigned char terrain[10];  /* +8   by patch_dir index, from 1        */
    char active[10];            /* +12  from 1                            */
    char x;                     /* +1c                                    */
    char y;                     /* +1d                                    */
};

struct Res;

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far glyph_at(int col, int row, unsigned char glyph, char unused,
                         struct Res far *r);                     /* 05f5:0598 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far icon_draw(int col, int row, unsigned char kind,
                          unsigned char bank, unsigned char idx); /* 0cb5:0299 */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */

extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern struct IconCell icon_cells[6][6];                         /* 6125 */
extern unsigned char patch_dir[];                                /* 6199 */
extern int view_mode;                                            /* b421 */
extern char grid_x[];                                            /* b437 */
extern char grid_y[];                                            /* b47b */
extern struct Res far *icon_res;                                 /* b71c */
extern struct Hero far *heroes[];                                /* b8f5 */
extern char cell_who[25][50];                                    /* be6a */
extern struct Map far *cur_map;                                  /* c66b */
extern struct Patch far *patch_list;                             /* ca0a */
extern unsigned char combatant_count;                            /* cc73 */
extern char move_flag;                                           /* cc79 */
extern struct Combatant combatants[];                            /* cc7c */
extern struct Fallen recs[8];                                    /* ce14 */
extern unsigned char rec_count;                                  /* cfd7 */
extern struct Res far *pic_res;                                  /* cfd8 */

void far cell_get(char x, char y, unsigned char far *who, unsigned char far *kind);
void far tile_update(char x, char y, unsigned char who);
char far in_combat_view(char x, char y);
char far combatant_visible(struct Hero far *h, char whole);
char far combatant_x(struct Hero far *h);
char far combatant_y(struct Hero far *h);
unsigned char far combatant_slot(struct Hero far *h);
void far view_clip(void);
char far combat_terrain_under(char x, char y);
void far cell_draw(char vx, char vy, char x, char y);

/* The translated Pascal unit's empty initialization block (DKK cbtmap.pas). */
void far cbview_unit_init(void)                                       /* 32fa:000a */
{
}

char far icon_cell_delta(unsigned char size, unsigned char n,
                         char far *dx, char far *dy)             /* 32fa:000b */
{
    char ok;

    ok = 0;
    if (size != 0) {
        *dx = icon_cells[size][n].dx;
        *dy = icon_cells[size][n].dy;
        if (*dx >= 0)
            ok = 1;
    }
    return ok;
}

void far grid_update(void)                                       /* 32fa:0078 */
{
    unsigned char i;

    for (i = 1; i <= combatant_count; i++) {
        grid_x[i] = (combatants - 1)[i].x - cur_map->view_x;
        grid_y[i] = (combatants - 1)[i].y - cur_map->view_y;
    }
}

void far combat_palette_set(void)                                       /* 32fa:00e5 */
{
    pal_select(pic_res);
}

void far cursor_draw(char x, char y)                             /* 32fa:00f5 */
{
    char vx, vy, dx, dy;
    unsigned char i, who, kind;

    vx = x - cur_map->view_x;
    vy = y - cur_map->view_y;
    view_clip();
    for (i = 0; i <= 5; i++)
        if (icon_cell_delta(cur_map->cursor_size, i, &dx, &dy))
            if (in_combat_view(vx + dx, vy + dy)) {
                cell_draw(vx + dx, vy + dy, x + dx, y + dy);
                if (cur_map->cursor_on)
                    icon_draw(vx + dx, vy + dy, 0, 0, 0x1e);
            }
    cell_get(x, y, &who, &kind);
    if (who > 0)
        if (combatant_visible((heroes - 1)[who], 0))
            icon_draw(grid_x[who], grid_y[who], (heroes - 1)[who]->fight->facing, 0,
                      (heroes - 1)[who]->icon_slot);
    clip_reset();
}

void far cell_redraw(char x, char y)                             /* 32fa:025e */
{
    char vx, vy;
    unsigned char who, kind;

    vx = x - cur_map->view_x;
    vy = y - cur_map->view_y;
    view_clip();
    tile_update(vx, vy, 0);
    cell_get(x, y, &who, &kind);
    if (who > 0)
        if (combatant_visible((heroes - 1)[who], 0))
            icon_draw(grid_x[who], grid_y[who], (heroes - 1)[who]->fight->facing, 0,
                      (heroes - 1)[who]->icon_slot);
    clip_reset();
}

void far combat_grid_build(void)                                       /* 32fa:0331 */
{
    unsigned char i, j;
    char dx, dy;

    mem_zero(cell_who, sizeof cell_who);
    for (i = 1; i <= combatant_count; i++) {
        if ((combatants - 1)[i].size > 0) {
            for (j = 0; j <= 5; j++)
                if (icon_cell_delta((combatants - 1)[i].size, j, &dx, &dy))
                    cell_who[(combatants - 1)[i].y + dy][(combatants - 1)[i].x + dx] = i;
            grid_x[i] = (combatants - 1)[i].x - cur_map->view_x;
            grid_y[i] = (combatants - 1)[i].y - cur_map->view_y;
        }
    }
}

void far cell_get(char x, char y, unsigned char far *who,
                  unsigned char far *kind)                       /* 32fa:0469 */
{
    if (x >= 0 && x <= 49 && y >= 0 && y <= 24) {
        *kind = cur_map->cell[y][x];
        *who = cell_who[y][x];
    } else {
        *who = 0;
        *kind = 0;
    }
}

void far tile_update(char x, char y, unsigned char who)          /* 32fa:04d5 */
{
    char mx, my, dx, dy;
    unsigned char i, kind, size;

    if (who == 0) {
        mx = cur_map->view_x + x;
        my = cur_map->view_y + y;
        cell_get(mx, my, &who, &kind);
    }
    if (who > 0) {
        x = grid_x[who];
        y = grid_y[who];
        mx = cur_map->view_x + x;
        my = cur_map->view_y + y;
        size = (combatants - 1)[who].size;
        for (i = 0; i <= 5; i++)
            if (icon_cell_delta(size, i, &dx, &dy))
                if (in_combat_view(x + dx, y + dy))
                    cell_draw(x + dx, y + dy, mx + dx, my + dy);
    } else if (in_combat_view(x, y))
        cell_draw(x, y, mx, my);
}

char far in_combat_view(char x, char y)                             /* 32fa:05f8 */
{
    char ok;

    if (x < 0 || x > 6 || y < 0 || y > 6)
        ok = 0;
    else
        ok = 1;
    return ok;
}

char far combatant_visible(struct Hero far *h, char whole)             /* 32fa:0627 */
{
    char seen;
    unsigned char i, slot;
    char dx, dy;

    seen = 1;
    slot = combatant_slot(h);
    if ((combatants - 1)[slot].size == 0)
        seen = 0;
    else
        for (i = 0; i <= 5; i++) {
            if (icon_cell_delta((combatants - 1)[slot].size, i, &dx, &dy)) {
                if (!in_combat_view(grid_x[slot] + dx, grid_y[slot] + dy)) {
                    seen = 0;
                    if (whole)
                        return seen;
                } else {
                    seen = 1;
                    if (!whole)
                        return seen;
                }
            }
        }
    return seen;
}

char far view_build(char x, char y, unsigned char margin)        /* 32fa:06e7 */
{
    char moved;
    unsigned char m;
    char i, j, cx, cy, x_lo, x_hi, y_lo, y_hi;

    moved = 0;
    cx = cur_map->view_x + 3;
    cy = cur_map->view_y + 3;
    m = margin;
    if (margin == 0xff)
        m = 0;
    x_lo = cx - m;
    x_hi = cx + m;
    y_lo = cy - m;
    y_hi = cy + m;
    if (margin == 0xff || x < x_lo || x > x_hi || y < y_lo || y > y_hi) {
        if (x < x_lo)
            while (x < cx && cx > 3)
                cx--;
        else if (x > x_hi)
            while (x > cx && cx < 46)
                cx++;
        if (y < y_lo)
            while (y < cy && cy > 3)
                cy--;
        else if (y > y_hi)
            while (y > cy && cy < 21)
                cy++;
        cur_map->view_x = cx - 3;
        cur_map->view_y = cy - 3;
        cy = cur_map->view_y;
        for (j = 0; j <= 6; j++) {
            cx = cur_map->view_x;
            for (i = 0; i <= 6; i++) {
                cell_draw(i, j, cx, cy);
                cx++;
            }
            cy++;
        }
        grid_update();
        moved = 1;
    }
    return moved;
}

void far combat_view_scroll(char x, char y, unsigned char margin,
                         unsigned char dir)                      /* 32fa:0847 */
{
    struct Hero far *h;
    unsigned char i;
    char tx, ty;

    screen_draw_begin(1);
    tx = x + dir_dx[dir];
    ty = y + dir_dy[dir];
    if (view_build(tx, ty, margin)) {
        view_clip();
        for (i = 1; i <= combatant_count; i++) {
            h = (heroes - 1)[i];
            if (h != 0)
                if (h->active != 0)
                    if ((combatants - 1)[i].size > 0)
                        if (combatant_visible(h, 0))
                            icon_draw(grid_x[i], grid_y[i], h->fight->facing, 0,
                                      h->icon_slot);
        }
        clip_reset();
    }
    cell_redraw(x, y);
    if (!in_combat_view(tx - cur_map->view_x, ty - cur_map->view_y)) {
        if (tx > 49)
            tx = 49;
        if (tx < 0)
            tx = 0;
        if (ty > 24)
            ty = 24;
        if (ty < 0)
            ty = 0;
    }
    cursor_draw(tx, ty);
    screen_flush();
}

void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame,
                         char erase)                             /* 32fa:09a2 */
{
    unsigned char slot;

    slot = combatant_slot(h);
    if (!combatant_visible(h, 1) && move_flag)
        combat_view_scroll(combatant_x(h), combatant_y(h), 3, 8);
    if (h->fight->facing / 4 != dir / 4 || frame != 0 || erase != 0)
        if (move_flag)
            tile_update(0, 0, slot);
    h->fight->facing = dir;
    if (erase == 0)
        if (combatant_visible(h, 0))
            if (move_flag) {
                view_clip();
                icon_draw(grid_x[slot], grid_y[slot], dir, frame, h->icon_slot);
                clip_reset();
                screen_flush();
            }
}

char far combatant_x(struct Hero far *h)                         /* 32fa:0ab5 */
{
    char x;

    x = (combatants - 1)[combatant_slot(h)].x;
    return x;
}

char far combatant_y(struct Hero far *h)                         /* 32fa:0ae4 */
{
    char y;

    y = (combatants - 1)[combatant_slot(h)].y;
    return y;
}

unsigned char far combatant_size(struct Hero far *h)                /* 32fa:0b14 */
{
    unsigned char size;

    size = (combatants - 1)[combatant_slot(h)].size;
    return size;
}

unsigned char far combatant_slot(struct Hero far *h)                /* 32fa:0b44 */
{
    unsigned char slot;
    char found;
    unsigned char i;

    found = 0;
    slot = 0;
    i = 0;
    do {
        i++;
        if ((heroes - 1)[i] == h)
            found = 1;
    } while (!found && i != combatant_count);
    if (found)
        slot = i;
    return slot;
}

void far combat_direction_probe(struct Hero far *h, unsigned char dir, unsigned char far *who,
                         unsigned char far *cell, unsigned char far *kind) /* 32fa:0b9c */
{
    unsigned char i, slot, c, t, best;
    char nx, ny, x, y, dx, dy;

    *who = 0;
    *cell = 23;
    best = 1;
    *kind = 0;
    slot = combatant_slot(h);
    x = (combatants - 1)[slot].x;
    y = (combatants - 1)[slot].y;
    for (i = 0; i <= 5; i++) {
        if (icon_cell_delta((combatants - 1)[slot].size, i, &dx, &dy)) {
            nx = x + dx + dir_dx[dir];
            ny = y + dy + dir_dy[dir];
            cell_get(nx, ny, &c, &t);
            if (c == slot)
                c = 0;
            if (c > 0)
                *who = c;
            if (t == 0x1b || t == 0x1c || t == 0x1d)
                *kind = t;
            if (t == 0 || *cell == 0)
                *cell = 0;
            else if ((tile_info - 1)[t].move_cost >= best) {
                best = (tile_info - 1)[t].move_cost;
                *cell = t;
            }
        }
    }
}

void far hero_fall(struct Hero far *h)                           /* 32fa:0d0a */
{
    unsigned char slot, k, frame;
    char dx, dy, cell_x, cell_y;

    if (view_mode != 5) {
        sfx_play(6);
        text_pause();
        return;
    }
    for (k = 1; k < 9 && (recs - 1)[k].hero != h; k++)
        ;
    if (k < 9)
        return;
    slot = combatant_slot(h);
    cell_x = combatant_x(h);
    cell_y = combatant_y(h);
    if (!combatant_visible(h, 1))
        combat_view_scroll(cell_x, cell_y, 3, 8);
    tile_update(0, 0, slot);
    sfx_play(6);
    view_clip();
    for (frame = 0; frame <= 8; frame++) {
        for (k = 0; k <= 5; k++)
            if (icon_cell_delta((combatants - 1)[slot].size, k, &dx, &dy))
                if (in_combat_view(grid_x[slot] + dx, grid_y[slot] + dy))
                    icon_draw(grid_x[slot] + dx, grid_y[slot] + dy, 3, frame & 1, 0x18);
        screen_flush();
        delay(10);
    }
    clip_reset();
    if (h->fight->not_pc == 0 && h->status != 8) {
        frame = combat_terrain_under(cell_x, cell_y);
        rec_count++;
        (recs - 1)[rec_count].terrain = frame;
        if (cur_map->cell[cell_y][cell_x] != 0x1d && cur_map->cell[cell_y][cell_x] != 0x1c) {
            if (h->status == 7)
                cur_map->cell[cell_y][cell_x] = 0x1e;
            else
                cur_map->cell[cell_y][cell_x] = 0x1f;
        }
        (recs - 1)[rec_count].hero = h;
        (recs - 1)[rec_count].x = cell_x;
        (recs - 1)[rec_count].y = cell_y;
    }
    text_pause();
    tile_update(0, 0, slot);
    (combatants - 1)[combatant_slot(h)].size = 0;
    combat_grid_build();
    combat_view_scroll(cur_map->view_x + 3, cur_map->view_y + 3, 3, 8);
    h->fight->initiative = 0;
    h->fight->moves = 0;
    h->fight->guarding = 0;
}

char far hero_place(struct Hero far *h, unsigned char x, unsigned char y,
                    char moving)                                 /* 32fa:1053 */
{
    char ok;
    unsigned char slot, i, who, t;
    char found;
    unsigned char kind;

    if (view_mode == 5) {
        ok = 0;
        slot = combatant_slot(h);
        (combatants - 1)[slot].size = h->icon_dim & 0x7f;
        (combatants - 1)[slot].x = x;
        (combatants - 1)[slot].y = y;
        combat_direction_probe(h, 8, &who, &t, &kind);
        if (who != 0 || t == 0 || (tile_info - 1)[t].move_cost == 0xff)
            (combatants - 1)[slot].size = 0;
        else {
            ok = 1;
            if (moving)
                if (h->fight->not_pc == 0) {
                    for (i = 1; i <= rec_count; i++)
                        if ((recs - 1)[i].hero == h) {
                            if ((recs - 1)[i].terrain != 0x1f)
                                t = (recs - 1)[i].terrain;
                            mem_fill(&(recs - 1)[i], sizeof(struct Fallen), 0);
                        }
                    found = 0;
                    for (i = 1; i <= rec_count; i++)
                        if ((recs - 1)[i].hero != 0 && (recs - 1)[i].x == x
                            && (recs - 1)[i].y == y)
                            found = 1;
                    if (!found)
                        cur_map->cell[y][x] = t;
                }
            combat_grid_build();
        }
    } else
        ok = 1;
    return ok;
}

void far view_center(struct Hero far *h, unsigned char margin, char cursor) /* 32fa:1286 */
{
    cur_map->cursor_on = cursor;
    cur_map->cursor_size = (combatants - 1)[combatant_slot(h)].size;
    if (move_flag)
        combat_view_scroll(combatant_x(h), combatant_y(h), margin, 8);
    cur_map->cursor_on = 0;
    cur_map->cursor_size = 1;
}

void far view_clip(void)                                         /* 32fa:12f8 */
{
    clip_set(0x1f44, 0x1f44, 0x1f98, 0x1f98);
}

char far combat_terrain_under(char x, char y)                             /* 32fa:130b */
{
    char under;
    unsigned char t, i;
    struct Patch far *p;

    t = cur_map->cell[y][x];
    if (t == 0x1d || t == 0x1c || t == 0x1b) {
        for (p = patch_list; p != 0; p = p->next)
            for (i = 1; i <= 9; i++)
                if (p->active[i] != 0 && p->x + dir_dx[patch_dir[i]] == x
                    && p->y + dir_dy[patch_dir[i]] == y)
                    t = p->terrain[i];
    } else if (t == 0x1f) {
        for (i = 1; i <= rec_count; i++)
            if ((recs - 1)[i].x == x && (recs - 1)[i].y == y)
                t = (recs - 1)[i].terrain;
    }
    under = t;
    return under;
}

unsigned char far dir_between(int x1, int y1, int x2, int y2)    /* 32fa:146b */
{
    unsigned char dir, d;
    char found;
    int adx, ady;

    adx = abs(x2 - x1);
    ady = abs(y2 - y1);
    d = 7;
    found = 0;
    while (!found) {
        switch (d) {
        case 1:
            found = y2 < y1 && x2 > x1 && adx * 618 / 256 >= ady && adx * 106 / 256 <= ady;
            break;
        case 3:
            found = y2 > y1 && x2 > x1 && adx * 618 / 256 >= ady && adx * 106 / 256 <= ady;
            break;
        case 5:
            found = y2 > y1 && x2 < x1 && adx * 618 / 256 >= ady && adx * 106 / 256 <= ady;
            break;
        case 7:
            found = y2 < y1 && x2 < x1 && adx * 618 / 256 >= ady && adx * 106 / 256 <= ady;
            break;
        case 0:
            found = y2 <= y1 && adx * 618 / 256 <= ady;
            break;
        case 2:
            found = x2 >= x1 && adx * 106 / 256 >= ady;
            break;
        case 4:
            found = y2 >= y1 && adx * 618 / 256 <= ady;
            break;
        case 6:
            found = x2 <= x1 && adx * 106 / 256 >= ady;
            break;
        }
        if (!found) {
            d++;
            if (d >= 8)
                d -= 8;
        }
    }
    dir = d;
    return dir;
}

unsigned char far combatant_direction(struct Hero far *from, struct Hero far *to) /* 32fa:166c */
{
    unsigned char dir;

    dir = dir_between(combatant_x(from), combatant_y(from),
                      combatant_x(to), combatant_y(to));
    return dir;
}

void far cell_draw(char vx, char vy, char x, char y)             /* 32fa:16bb */
{
    unsigned char t, base, top, i;
    int row, col;

    col = (vx + vx + vx) * 4 + 0x1f44;  /* add,add,shl,shl; vx * 12 is a mul */
    row = (vy + vy + vy) * 4 + 0x1f44;
    t = cur_map->cell[y][x];
    if (t > 0x4c || t == 0)
        t = 0x15;
    base = (tile_info - 1)[t].kind;
    if (t == 0x1d || t == 0x1c || t == 0x1b || t == 0x1f || t == 0x1e) {
        t = combat_terrain_under(x, y);
        top = (tile_info - 1)[t].kind;
        switch (t) {
        case 0x1e:
        case 0x1f:
            for (i = 1; i <= rec_count; i++)
                if ((recs - 1)[i].x == x && (recs - 1)[i].y == y)
                    t = (recs - 1)[i].terrain;
        }
        if (t > 0x4c || t == 0)
            t = 0x15;
        top = (tile_info - 1)[t].kind;
        glyph_at(col, row, top + 1, 0, pic_res);
        glyph_at(col, row, base + 0xf, 1, icon_res);
    } else
        glyph_at(col, row, base + 1, 0, pic_res);
}
