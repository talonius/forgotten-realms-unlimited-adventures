/* CKIT.EXE overlay 00 module 1cb8: combat services around the icons.
 * The four facings of a moving icon, flying an icon along a line over the
 * combat map, flashing a hit or spell on a combatant, the two sides'
 * counts, the targets in range and the distance to one, ending a turn and
 * guarding, redrawing the combat view, the ranged-weapon tests, bandaging
 * the dying, scattering an icon around the view's centre, and a missile's
 * facing for its direction.
 *
 * EXE 0x01fa24..0x020a95, one Turbo C large-model _TEXT segment starting at
 * byte 4 of paragraph 1cb8, in RTLink section 0 (payload 0x1e2b0, link
 * segment 1b41; a root section, the parent of 1 and 2).  The first four
 * bytes of paragraph 1cb8 end module 1b41's last routine; the lone retf at
 * 1dbf:0005 after this module's last routine is nothing's target and is
 * left to module 1dbf.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 0.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and cbtutil_NNNN only where in this module
 * the routine starts.  A mod18de_ callee is an RTLink vector stub; its
 * comment names the routine the stub reaches.
 *
 * _DATA is sine and two literals, DGROUP 5574..563c.  The module has no
 * _BSS.
 *
 * heroes, near_ids and steps are indexed from 1; the shipped code adds the
 * address of element 0 (b8f1, c354, ca28).  side_count is indexed by side.
 */

#include <stdlib.h>

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28  index into item_table             */
};

struct ItemType {               /* 0x10 bytes an item type                */
    char pad_0[0xc];            /* +0   not read here                     */
    unsigned char range;        /* +c                                     */
    char pad_d;                 /* +d   not read here                     */
    unsigned char flags;        /* +e                                     */
    char pad_f;                 /* +f   not read here                     */
};

struct Fight {                  /* a combatant's state in a fight         */
    unsigned char casting;      /* +0                                     */
    char pad_1[2];              /* +1   not read here                     */
    int initiative;             /* +3                                     */
    char pad_5[2];              /* +5   not read here                     */
    unsigned char moves;        /* +7   half-moves left                   */
    char guarding;              /* +8                                     */
    char pad_9[6];              /* +9   not read here                     */
    unsigned char dying;        /* +f   bandaging clears it               */
    char pad_10;                /* +10  not read here                     */
    char panicked;              /* +11  nonzero: 0bc5 does not guard      */
    char pad_12[2];             /* +12  not read here                     */
    char not_pc;                /* +14  nonzero: 0cc8 passes over it      */
};

struct Hero {                   /* a combatant                            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[8];              /* +4   not read here                     */
    struct Item far *equipped[13]; /* +c   0: the weapon                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x1a];          /* +44  not read here                     */
    unsigned char status;       /* +5e  5: dying, 0cc8 sets 4; 9 uncounted */
    unsigned char side;         /* +5f  0 or 1                            */
    char pad_60[0x11e];         /* +60  not read here                     */
    char active;                /* +17e 0: not counted in a side          */
};

struct Map {
    char pad_0[2];              /* +0   not read here                     */
    int view_x;                 /* +2   the view's left column            */
    int view_y;                 /* +4   the view's top row                */
    char pad_6[2];              /* +6   not read here                     */
    char ignore_blocking;       /* +8   nonzero: terrain blocks nothing   */
};

struct LineWalk {               /* a line stepper, section 2's            */
    int x0;                     /* +0                                     */
    int y0;                     /* +2                                     */
    int x1;                     /* +4                                     */
    int y1;                     /* +6                                     */
    int err;                    /* +8   not read here                     */
    int dx;                     /* +a   not read here                     */
    int dy;                     /* +c   not read here                     */
    int x;                      /* +e   not read here                     */
    int y;                      /* +10  not read here                     */
    int sx;                     /* +12  not read here                     */
    int sy;                     /* +14  not read here                     */
    unsigned char steps;        /* +16  not read here                     */
    char dir;                   /* +17  direction of the last step        */
};

struct Step {
    unsigned char who;          /* +0   index into heroes                 */
    unsigned char cost;         /* +1                                     */
    unsigned char dir;          /* +2   not read here                     */
};

struct Res;

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far glyph_cell(int col, int row, int glyph, char unused,
                           struct Res far *r);                   /* 05f5:0564 */
extern void far res_glyph_mirror(struct Res far *r, unsigned char n);  /* 05f5:05dd */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_copy(struct Res far *src, unsigned char n, char map,
                         struct Res far *dst, unsigned char m);  /* 05f5:0904 */
extern void far rows_save(int row, int col, int height, int width); /* 05f5:0af4 */
extern void far rows_restore(int row, int col, int height, int width); /* 05f5:0bc3 */
extern void far text_pause(void);                                /* 06c9:093c */
extern char far pool_lowbit(int value);                          /* 0a75:032f */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far frame_right_panel(void);                                /* 0d65:0233 */
extern int  far random_int(int n);                               /* 0da7:022d */
extern char far line_step(struct LineWalk far *l);
                                /* 18de:1265, vector to overlay 2 2632:020c */
extern void far line_init(struct LineWalk far *l);
                                /* 18de:126f, vector to overlay 2 2632:01a0 */
extern int  far int_sign(int v);
                                /* 18de:1279, vector to overlay 2 2632:000d */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern void far combat_palette_set(void);                               /* 22fa:00e5 */
extern char far in_combat_view(char x, char y);                     /* 22fa:05f8 */
extern char far combatant_visible(struct Hero far *h, char whole);     /* 22fa:0627 */
extern void far combat_view_scroll(char x, char y, unsigned char margin,
                            unsigned char dir);                  /* 22fa:0847 */
extern char far combatant_x(struct Hero far *h);                 /* 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h);                 /* 22fa:0ae4 */
extern unsigned char far combatant_size(struct Hero far *h);        /* 22fa:0b14 */
extern unsigned char far combatant_slot(struct Hero far *h);        /* 22fa:0b44 */

extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern struct ItemType far *item_table;                          /* b40a */
extern int view_mode;                                            /* b421 */
extern char grid_x[];                                            /* b437 */
extern char grid_y[];                                            /* b47b */
extern struct Res far *icon_res;                                 /* b71c */
extern unsigned char far *cursor_obj;                            /* b75b */
extern unsigned char side_count[];                               /* b8d1 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern unsigned char near_ids[];                                 /* c355 */
extern char view_cx;                                             /* c39f */
extern char view_cy;                                             /* c3bc */
extern struct Map far *cur_map;                                  /* c66b */
extern unsigned char step_count;                                 /* ca10 */
extern struct Step steps[];                                      /* ca2b */
extern struct Hero far *node_list;                               /* d129 */

static char sine[180] = {       /* sin of 2-degree steps, times 100       */
       0,    3,    7,   10,   14,   17,   21,   24,   28,   31,
      34,   37,   41,   44,   47,   50,   53,   56,   59,   62,
      64,   67,   69,   72,   74,   77,   79,   81,   83,   85,
      87,   88,   90,   91,   93,   94,   95,   96,   97,   98,
      98,   99,   99,  100,  100,  100,  100,  100,   99,   99,
      98,   98,   97,   96,   95,   94,   93,   91,   90,   88,
      87,   85,   83,   81,   79,   77,   74,   72,   69,   67,
      64,   62,   59,   56,   53,   50,   47,   44,   41,   37,
      34,   31,   28,   24,   21,   17,   14,   10,    7,    3,
       0,   -3,   -7,  -10,  -14,  -17,  -21,  -24,  -28,  -31,
     -34,  -37,  -41,  -44,  -47,  -50,  -53,  -56,  -59,  -62,
     -64,  -67,  -69,  -72,  -74,  -77,  -79,  -81,  -83,  -85,
     -87,  -88,  -90,  -91,  -93,  -94,  -95,  -96,  -97,  -98,
     -98,  -99,  -99, -100, -100, -100, -100, -100,  -99,  -99,
     -98,  -98,  -97,  -96,  -95,  -94,  -93,  -91,  -90,  -88,
     -87,  -85,  -83,  -81,  -79,  -77,  -74,  -72,  -69,  -67,
     -64,  -62,  -59,  -56,  -53,  -50,  -47,  -44,  -41,  -37,
     -34,  -31,  -28,  -24,  -21,  -17,  -14,  -10,   -7,   -3
};

void far icon_facing(unsigned char cell, unsigned char half, unsigned char row,
                     char mirror)                                /* 2cb8:0004 */
{
    res_copy(icon_res, half * 0x26 + cell, 0, icon_res, row + 0x4d);
    if (mirror)
        res_glyph_mirror(icon_res, row + 0x4d);
}

void far icon_facings(unsigned char cell)                        /* 2cb8:0052 */
{
    icon_facing(cell, 0, 0, 0);
    icon_facing(cell, 0, 1, 1);
    icon_facing(cell, 1, 2, 1);
    icon_facing(cell, 1, 3, 0);
}

int far view_row(int y)                                          /* 2cb8:009f */
{
    return y * 4 + 0x1f44;
}

int far view_col(int x)                                          /* 2cb8:00ae */
{
    return x * 4 + 0x1f44;
}

void far tile_blit(int x, int y, unsigned char glyph, char mode,
                   struct Res far *r)                            /* 2cb8:00bd */
{
    glyph_cell(x, y, glyph, mode, r);
}

void far icon_fly(int x0, int y0, int x1, int y1, unsigned char frames,
                  unsigned char pace)                            /* 2cb8:00de */
{
    unsigned char path[0x94];
    struct LineWalk walk;
    unsigned char path_idx;
    unsigned char path_count;
    unsigned char frame_idx;
    char last_pass;
    char done;
    int cam_x;
    int cam_y;
    int acc_x;
    int acc_y;
    int diff_x;
    int diff_y;
    int step_x;
    int step_y;
    int over_x;
    int over_y;
    register int px;
    register int py;

    if (x0 < 0)
        x0 = 0;
    else if (x0 > 0x31)
        x0 = 0x31;
    if (y0 < 0)
        y0 = 0;
    else if (y0 > 0x18)
        y0 = 0x18;
    if (x1 < 0)
        x1 = 0;
    else if (x1 > 0x31)
        x1 = 0x31;
    if (y1 < 0)
        y1 = 0;
    else if (y1 > 0x18)
        y1 = 0x18;
    mem_fill(path, sizeof path, 8);
    mem_fill(&walk, sizeof walk, 0);
    path_idx = 0;
    frame_idx = 0;
    walk.x0 = x0 * 3;
    walk.y0 = y0 * 3;
    walk.x1 = x1 * 3;
    walk.y1 = y1 * 3;
    line_init(&walk);
    do {
        done = !line_step(&walk);
        path[path_idx] = walk.dir;
        path_idx++;
    } while (!done);
    if (path_idx < 4)
        return;
    path_count = path_idx - 2;
    diff_x = x1 - x0;
    diff_y = y1 - y0;
    if (!in_combat_view(x0 - cur_map->view_x, y0 - cur_map->view_y)
        || !in_combat_view(x1 - cur_map->view_x, y1 - cur_map->view_y)) {
        if (abs(diff_x) <= 6 && abs(diff_y) <= 6) {
            last_pass = 1;
            cam_x = x0 + diff_x / 2;
            cam_y = y0 + diff_y / 2;
        } else {
            last_pass = 0;
            cam_x = cur_map->view_x + 3;
            cam_y = cur_map->view_y + 3;
        }
    } else {
        last_pass = 1;
        cam_x = cur_map->view_x + 3;
        cam_y = cur_map->view_y + 3;
    }
    combat_view_scroll(cam_x, cam_y, 0xff, 8);
    path_idx = 0;
    acc_x = 0;
    acc_y = 0;
    do {
        px = (x0 - cur_map->view_x) * 3 + acc_x;
        py = (y0 - cur_map->view_y) * 3 + acc_y;
        done = 0;
        do {
            step_x = dir_dx[path[path_idx]];
            step_y = dir_dy[path[path_idx]];
            px += step_x;
            py += step_y;
            if (pace > 0 || px % 3 == 0 || py % 3 == 0) {
                screen_flush_mode_set(2);
                rows_save(view_row(py), view_col(px), 12, 12);
                tile_blit(px, py, frame_idx + 0x4d, 5, icon_res);
                delay(pace * 2);
                rows_restore(view_row(py), view_col(px), 12, 12);
                frame_idx++;
                if (frame_idx >= frames)
                    frame_idx = 0;
                screen_flush_mode_set(0);
            }
            path_idx++;
            if (px < 0 || px > 0x12 || py < 0 || py > 0x12)
                done = 1;
            if (!done && path_idx < path_count) {
                acc_x += step_x;
                acc_y += step_y;
                if (abs(acc_x) == 3) {
                    x0 += int_sign(acc_x);
                    cam_x += int_sign(acc_x);
                    acc_x = 0;
                }
                if (abs(acc_y) == 3) {
                    y0 += int_sign(acc_y);
                    cam_y += int_sign(acc_y);
                    acc_y = 0;
                }
            }
        } while (path_idx < path_count && !done);
        if (path_idx < path_count) {
            over_x = 0;
            over_y = 0;
            acc_y = 0;
            acc_x = 0;
            x0 = x1;
            y0 = y1;
            if (x1 + 3 > 0x31)
                over_x = x1 - 0x31;
            else if (x1 < 3)
                over_x = 3 - x1;
            if (y1 + 3 > 0x18)
                over_y = y1 - 0x18;
            else if (y1 < 3)
                over_y = 3 - y1;
            cam_x = x1 + over_x;
            cam_y = y1 + over_y;
            combat_view_scroll(cam_x, cam_y, 0xff, 8);
            px = (x1 - cur_map->view_x) * 3;
            py = (y1 - cur_map->view_y) * 3;
            path_idx = path_count;
            done = 0;
            do {
                step_x = -dir_dx[path[path_idx]];
                step_y = -dir_dy[path[path_idx]];
                px += step_x;
                py += step_y;
                if (px > 0x12)
                    x0 = cur_map->view_x + 6;
                else if (px < 0)
                    x0 = cur_map->view_x;
                if (py > 0x12)
                    y0 = cur_map->view_y + 6;
                else if (py < 0)
                    y0 = cur_map->view_y;
                if (px < 0 || px > 0x12 || py < 0 || py > 0x12)
                    done = 1;
                if (!done) {
                    acc_x += step_x;
                    acc_y += step_y;
                    if (abs(acc_x) == 3) {
                        x0 += int_sign(acc_x);
                        cam_x += int_sign(acc_x);
                        acc_x = 0;
                    }
                    if (abs(acc_y) == 3) {
                        y0 += int_sign(acc_y);
                        cam_y += int_sign(acc_y);
                        acc_y = 0;
                    }
                    path_idx--;
                }
            } while (!done);
        } else {
            last_pass = 1;
            if (!in_combat_view(x1 - cur_map->view_x, y1 - cur_map->view_y))
                combat_view_scroll(x1, y1, 3, 8);
            px = (x1 - cur_map->view_x) * 3;
            py = (y1 - cur_map->view_y) * 3;
            rows_save(view_row(py), view_col(px), 12, 12);
            tile_blit(px, py, frame_idx + 0x4d, 5, icon_res);
            if (pace > 0) {
                screen_flush();
                delay(pace * 2);
                rows_restore(view_row(py), view_col(px), 12, 12);
            }
        }
    } while (!last_pass);
    screen_flush();
}

void far hit_flash(struct Hero far *h, char effect, char far *msg) /* 2cb8:0730 */
{
    char gx;
    char gy;
    unsigned char cell;
    unsigned char frame;
    unsigned char rep;
    unsigned char reps;

    if (view_mode == 5) {
        if (effect)
            cell = 0x17;
        else
            cell = 0x19;
        icon_facings(cell);
        if (!combatant_visible(h, 1))
            combat_view_scroll(combatant_x(h), combatant_y(h), 3, 8);
        if (effect)
            sfx_play(5);
        else
            sfx_play(4);
        hero_text(h, msg, 10, 0);
        if (effect)
            reps = cursor_obj[0x12];
        else
            reps = 0;
        gx = grid_x[combatant_slot(h)] * 3;
        gy = grid_y[combatant_slot(h)] * 3;
        for (rep = 0; rep <= reps; rep++)
            for (frame = 0; frame <= 3; frame++) {
                screen_flush_mode_set(2);
                rows_save(view_row(gy), view_col(gx), 12, 12);
                tile_blit(gx, gy, frame + 0x4d, 5, icon_res);
                delay(70);
                rows_restore(view_row(gy), view_col(gx), 12, 12);
                screen_flush_mode_set(0);
            }
        screen_flush();
        if (reps == 0)
            text_pause();
    } else
        hero_text(h, msg, 10, 1);
}

int far enemy_team(struct Hero far *h)                           /* 2cb8:08fc */
{
    short side;

    if (h->side == 0)
        side = 1;
    else
        side = 0;
    return side;
}

void far combat_sides_count(void)                                      /* 2cb8:091f */
{
    struct Hero far *h;

    side_count[0] = 0;
    side_count[1] = 0;
    for (h = node_list; h != 0; h = h->next)
        if (h->active != 0 && h->status != 9)
            side_count[h->side]++;
}

unsigned char far combat_targets_near(struct Hero far *h, unsigned char range,
                               char same)                        /* 2cb8:0971 */
{
    unsigned char n;
    short side;
    unsigned char count;
    unsigned char matching;
    unsigned char i;

    plan_steps(combatant_x(h), combatant_y(h), range, 0xff, combatant_size(h));
    count = step_count;
    matching = 0;
    if (same)
        side = h->side;
    else
        side = enemy_team(h);
    if (count > 0) {
        for (i = 1; i <= count; i++)
            if ((heroes - 1)[(steps - 1)[i].who]->side == side) {
                matching++;
                mem_move5(&(steps - 1)[i], &(steps - 1)[matching], 3);
            }
        count = matching;
        step_count = matching;
    }
    n = count;
    for (i = 1; i <= count; i++)
        (near_ids - 1)[i] = (steps - 1)[i].who;
    return n;
}

unsigned char far get_distance(struct Hero far *h, struct Hero far *t) /* 2cb8:0a99 */
{
    unsigned char dist;
    struct Step saved[255];
    unsigned char i;

    mem_move5(steps - 1, saved, 4); /* 4 of the list's 765 bytes, as shipped */
    cur_map->ignore_blocking = 1;
    plan_steps(combatant_x(h), combatant_y(h), 0xff, 0xff, combatant_size(h));
    cur_map->ignore_blocking = 0;
    i = 1;
    while ((heroes - 1)[(steps - 1)[i].who] != t && i < step_count)
        i++;
    dist = (steps - 1)[i].cost / 2;
    mem_move5(saved, steps - 1, 4);
    return dist;
}

char far combat_actions_end(struct Hero far *h)                        /* 2cb8:0b84 */
{
    char r;

    h->fight->initiative = 0;
    h->fight->casting = 0;
    h->fight->guarding = 0;
    h->fight->moves = 0;
    r = 1;
    return r;
}

char far combat_guard(struct Hero far *h)                        /* 2cb8:0bc5 */
{
    char r;

    r = combat_actions_end(h);
    if (h->fight->panicked == 0) {
        h->fight->guarding = 1;
        bottom_msg("Guarding");
    }
    return r;
}

void far combat_view_show(void)                                      /* 2cb8:0c07 */
{
    frame_right_panel();
    combat_palette_set();
    combat_view_scroll(cur_map->view_x + 3, cur_map->view_y + 3, 0xff, 8);
}

char far has_ranged_weapon(struct Hero far *h)                        /* 2cb8:0c32 */
{
    char r;

    if (h->equipped[0] != 0 && item_table[h->equipped[0]->type].range > 1)
        r = 1;
    else
        r = 0;
    return r;
}

char far has_throwable_weapon(struct Hero far *h)                        /* 2cb8:0c71 */
{
    char r;

    r = has_ranged_weapon(h)
        && (char)((item_table[h->equipped[0]->type].flags & 0x14) == 0x14);
    return r;
}

char far bandage_dying(char apply)                                /* 2cb8:0cc8 */
{
    char r;
    struct Hero far *h;
    char found;

    found = 0;
    for (h = node_list; h != 0; h = h->next)
        if (h->fight->not_pc == 0 && h->side == 0 && h->status == 5) {
            found = 1;
            if (apply) {
                h->status = 4;
                h->fight->dying = 0;
                hero_text(h, "is bandaged", 10, 1);
                apply = 0;
            }
        }
    r = found;
    return r;
}

void far icon_scatter(unsigned char radius, unsigned char cell,
                      unsigned char sound)                       /* 2cb8:0d53 */
{
    unsigned char pick;
    unsigned char max_ring;
    unsigned char ring;
    int point;
    char saved_x;
    char saved_y;
    char draw_x;
    char draw_y;
    char off_y;
    char off_x;
    register int dist;
    register int angle;

    combat_view_scroll(view_cx, view_cy, 0xff, 8);
    saved_x = view_cx;
    saved_y = view_cy;
    sfx_play(sound);
    icon_facing(cell, 0, 0, 0);
    icon_facing(cell, 0, 1, 1);
    icon_facing(cell, 1, 2, 0);
    icon_facing(cell, 1, 3, 1);
    off_x = 0;
    off_y = 0;
    if (saved_x < 3)
        off_x = (saved_x - 3) * 3;
    else if (saved_x > 0x2e)
        off_x = (saved_x - 0x2e) * 3;
    if (saved_y < 3)
        off_y = (saved_y - 3) * 3;
    else if (saved_y > 0x15)
        off_y = (saved_y - 0x15) * 3;
    max_ring = radius * 3 + 1;
    for (ring = 1; ring <= max_ring; ring++) {
        dist = ring;
        for (point = ring * 2 - 2; point >= 0; point--) {
            angle = random_int(180);
            draw_x = (sine[(angle + 45) % 180] * dist + 50) / 100 + off_x;
            draw_y = (sine[angle] * dist + 50) / 100 + off_y;
            if (draw_x >= -9 && draw_x <= 9 && draw_y >= -9 && draw_y <= 9) {
                pick = random_n(4);
                tile_blit(draw_x + 9, draw_y + 9, pick + 0x4d, 4, icon_res);
            }
        }
        screen_flush();
    }
    combat_view_scroll(view_cx, view_cy, 0xff, 8);
}

void far missile_facing(unsigned char dir, unsigned char cell)   /* 2cb8:0f2c */
{
    if (pool_lowbit(dir)) {
        if (dir == 3 || dir == 5)
            icon_facing(cell + 1, 1, 0, dir == 5);
        else
            icon_facing(cell + 1, 0, 0, dir == 7);
    } else
        icon_facing(dir % 4 + cell, dir / 4, 0, 0);
}

char far ranged_ammo_find(struct Hero far *h, struct Item far * far *sel) /* 2cb8:0fb5 */
{
    char r;
    struct Item far *w;
    unsigned char flags;

    r = 0;
    w = h->equipped[0];
    *sel = 0;
    flags = 0;
    if (w != 0) {
        flags = item_table[w->type].flags;
        if (flags & 0x10)
            *sel = w;
        if (flags & 8) {
            if (flags & 1)
                *sel = h->equipped[11];
            if (flags & 0x80)
                *sel = h->equipped[12];
        }
    }
    if (*sel != 0 || flags == 10 || flags == 12)
        r = 1;
    return r;
}
