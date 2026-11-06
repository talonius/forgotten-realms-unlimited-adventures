/* CKIT.EXE resident module 1e8f: the scroll bar, a dialog item of module
 * 25e5's kind.  scroll_proc is its handler: it draws the bar (the track
 * glyph and an arrow at each end), hit-tests it, and on a press repeats an
 * arrow or a page step while the button is held, or drags the thumb.  The
 * item's val points at the position, width holds the count of positions
 * and len the track's length; flags bit 0x40 makes the bar horizontal.
 * thumb_pos maps a position to the thumb's pixel coordinate, thumb_move
 * erases and redraws the thumb, and scroll_fix clamps the position and
 * moves the thumb to it.  An arrow or page step calls the item's cb with
 * the item's index and the step (-1 or 1 for an arrow, -2 or 2 for a page),
 * a drag that moved the thumb with the index and 0.  Messages it does not
 * take go to dialog.c's default handler, ui_item_proc.
 *
 * EXE 0x010cf9..0x0119e6, one Turbo C large-model _TEXT segment starting at
 * byte 9 of paragraph 1e8f.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * diskfile.c's last retf is at 0x010cf8.  scroll_proc runs past paragraph
 * 1f5e: its retf is 1f5e:0005, and module 1f5e starts at 1f5e:0006.  All 56
 * MZ relocation sites in the range have frame 0e8f.  A port of DQK's
 * scroll.c (DQK module 21e9); the vertical thumb here is glyph 0x10.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  A name another module
 * here gives an address is reused from there; a modNNNN_ name is a
 * placeholder that says only which segment the routine lives in.
 * scroll_proc keeps the name listbox.c pushes it by.  No far call in
 * CKIT.EXE reaches the module's first five routines; listbox.c's
 * lbox_setup passes scroll_proc as an item handler (mov ax,0e8f at
 * 0x00a4e0).  The module has no _DATA and no _BSS; it reads dialog.c's
 * dl_items.
 */

typedef int (far *ItemProc)();       /* (item, msg, a, b), as dialog.c's */
typedef int (far *ItemCb)(int id, ...);

struct Wid {                    /* one dialog item, 0x20 bytes, as dialog.c */
    ItemProc fn;                /* +00 handler                            */
    ItemCb cb;                  /* +04 called on a step or a drag         */
    int far *val;               /* +08 the bar's position                 */
    char far *text;             /* +0c                                    */
    int row;                    /* +10                                    */
    int col;                    /* +12                                    */
    int cursor;                 /* +14 pointer frame while over it        */
    int len;                    /* +16 the track's length                 */
    int width;                  /* +18 the count of positions             */
    int glyph;                  /* +1a bank << 10 | index                 */
    char flags;                 /* +1c                                    */
    char key1;                  /* +1d hotkeys                            */
    char key2;                  /* +1e                                    */
    unsigned char color;        /* +1f                                    */
};

extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern int  far screen_cols(void);                              /* 0f5e:05d4 */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far text_offset_xy(int row, int col, int row2, int col2,
                             int far *out_row, int far *out_col); /* 107e:0565 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far glyph_bounds(int row, int col, int slot, int glyph, int far *top,
                             int far *left, int far *bottom, int far *right); /* 11d7:0bf0 */
extern int  far ui_item_proc(struct Wid far *w, int msg, int a, int b); /* 15e5:01c3 */
extern char far inp_clicked(int far *x, int far *y);            /* 178d:0026 */

extern struct Wid far *dl_items;                                 /* e6a3 */

int far thumb_pos(struct Wid far *w, int v)                      /* 1e8f:0009 */
{
    long t;
    int lo;
    int x;
    int hi;
    int y;

    text_xy(w->row, w->col, &lo, &x);
    if (w->flags & 0x40) {
        text_offset_xy(lo, x, 0, w->len + 0x1f40, &y, &hi);
        lo = x;
    } else
        text_offset_xy(lo, x, w->len + 0x1f40, 0, &hi, &y);
    if (w->width > 1) {
        t = hi - lo;
        t *= v;
        t /= w->width - 1;
        t += lo;
        return t;
    }
    return lo;
}

void far thumb_move(struct Wid far *w, int from, int to)         /* 1e8f:00e5 */
{
    int row;
    int col;
    int lim_row;
    int lim;

    if (to == from)
        return;
    if (from >= 0) {
        if (w->flags & 0x40) {
            row = w->row;
            col = from;
            text_xy(0, w->col, &lim_row, &lim);
            if (screen_cols() < 600) {
                if (col <= lim)
                    col = (col & ~7) + 4;
                else
                    col = ((col - 5) & ~7) + 4;
            } else {
                col &= ~1;
                if (col <= lim)
                    col = (col & ~0xf) + 8;
                else
                    col = ((col - 10) & ~0xf) + 8;
            }
            draw_glyph(row, col, 0, 0x3e);
        } else {
            row = from;
            text_offset_xy(from, 0x1f40, 0x1f44, 0, &row, &col);
            clip_set(from, 0x1f40, row, 0x1fe0);
            draw_glyph(w->row, w->col, w->glyph >> 10, w->glyph & 0x3ff);
            clip_reset();
        }
    }
    if (w->flags & 0x40) {
        if (screen_cols() < 600)
            draw_glyph(w->row, to, 0, (to & 7) + 0x36);
        else
            draw_glyph(w->row, to & ~1, 0, (to >> 1 & 7) + 0x36);
    } else
        draw_glyph(to, w->col, w->glyph ? w->glyph >> 10 : 0, 0x10);
}

void far scroll_fix(struct Wid far *w, register int old)         /* 1e8f:0283 */
{
    register int pos;

    pos = *w->val;
    if (pos >= w->width)
        *w->val = pos = w->width - 1;
    if (pos < 0)
        *w->val = pos = 0;
    if (pos != old)
        thumb_move(w, old >= 0 ? thumb_pos(w, old) : -1, thumb_pos(w, pos));
}

/* An arrow (glyph >= 0) is drawn at row, col; a page step (glyph < 0)
 * passes the two ends of the track region it covers instead. */
char far arrow_track(struct Wid far *w, register int row, int col,
                     register int glyph, int step)               /* 1e8f:02fc */
{
    long next;
    int pos;
    int x;
    int y;
    int top;
    int left;
    int bottom;
    int right;
    char was;
    char hit;

    was = 1;
    if (glyph >= 0) {
        draw_glyph(row, col, 0, glyph + 1);
        glyph_bounds(row, col, 0, glyph, &top, &left, &bottom, &right);
    } else if (w->flags & 0x40) {
        text_xy(w->row, row, &top, &left);
        text_xy(w->row + 6, col, &bottom, &right);
    } else {
        text_xy(row, w->col, &top, &left);
        text_xy(col, w->col + 6, &bottom, &right);
    }
    next = timer_ticks();
    x = top;
    y = left;
    do {
        hit = top <= x && x < bottom && left <= y && y < right ? 1 : 0;
        if (was != hit) {
            was = hit;
            if (glyph >= 0)
                draw_glyph(row, col, 0, (was ? 1 : 0) + glyph);
        }
        if (was && timer_ticks() >= next) {
            next = timer_ticks() + 7;
            pos = *w->val;
            if (w->cb)
                (*w->cb)(w - dl_items, step);
            scroll_fix(w, pos);
            if (glyph < 0) {
                pos = thumb_pos(w, *w->val);
                if (w->flags & 0x40) {
                    if (step < 0)
                        right = pos;
                    else
                        text_offset_xy(0, pos, 0, 0x1f44, &pos, &left);
                } else {
                    if (step < 0)
                        bottom = pos;
                    else
                        text_offset_xy(pos, 0, 0x1f44, 0, &top, &pos);
                }
            }
        }
    } while (!inp_clicked(&x, &y));
    if (was && glyph >= 0)
        draw_glyph(row, col, 0, glyph);
    return was;
}

char far thumb_drag(struct Wid far *w, int off)                  /* 1e8f:0579 */
{
    long t;
    int top;
    int left;
    int bottom;
    int right;
    int pos;
    int skip;
    int lo;
    int hi;
    int x;
    int y;
    char up;
    register int last;
    register int orig;

    if (w->flags & 0x40) {
        text_xy(w->row - 8, w->col - 0x14, &top, &left);
        text_xy(w->row + 0xe, w->col + w->len + 0x14, &bottom, &right);
        text_xy(0, w->col, &skip, &lo);
        text_xy(0, w->col + w->len, &skip, &hi);
        text_xy(w->row, 0, &skip, &pos);    /* pos is set again before any read */
    } else {
        text_xy(w->row - 0x14, w->col - 6, &top, &left);
        text_xy(w->row + w->len + 0x14, w->col + 0xa, &bottom, &right);
        text_xy(w->row, 0, &lo, &skip);
        text_xy(w->row + w->len, 0, &hi, &skip);
    }
    orig = last = thumb_pos(w, *w->val);
    do {
        up = inp_clicked(&x, &y);
        if (x < top || x >= bottom || y < left || y >= right)
            pos = orig;
        else {
            pos = (w->flags & 0x40) ? y : x;
            pos = int_max(lo, int_min(hi, pos + off));
        }
        if (pos != last) {
            thumb_move(w, last, pos);
            last = pos;
        }
    } while (!up);
    if (pos != orig) {
        t = w->width - 1;
        t *= pos - lo;
        t += (hi - lo) >> 1;
        t /= hi - lo;
        *w->val = t;
        if (w->cb)
            (*w->cb)(w - dl_items, 0);
        if ((last = thumb_pos(w, *w->val)) != pos)
            thumb_move(w, pos, last);
        return 1;
    }
    return 0;
}

int far scroll_proc(struct Wid far *w, int msg, int a, int b)   /* 1e8f:0836 */
{
    struct Wid far *p;
    int u;
    int v;
    int z;
    char horiz;
    register int n;

    p = w;
    horiz = (p->flags & 0x40) ? 1 : 0;
    switch (msg) {      /* 2e 3b 07 at 0863: Turbo C's sparse-switch value search */
    case 0:
        if (*p->val < 0)
            *p->val = 0;
        else if (*p->val >= p->width)
            *p->val = p->width;         /* scroll_fix clamps to width - 1 */
        break;
    case 1:
        p->flags |= 0x80;
        draw_glyph(p->row, p->col, p->glyph >> 10, p->glyph & 0x3ff);
        if (horiz) {
            draw_glyph(p->row, p->col - 6, 0, 0x17);
            draw_glyph(p->row, p->col + p->len + 2, 0, 0x15);
        } else {
            draw_glyph(p->row - 5, p->col, 0, 0xc);
            draw_glyph(p->row + p->len + 5, p->col, 0, 0xe);
        }
        scroll_fix(p, -1);
        break;
    case 2:
        text_xy_to(p->row, p->col, a, b, &u, &v);
        if (horiz)
            return u >= 0 && u < 4 && v >= -6 && p->len + 10 > v;
        return u >= -6 && p->len + 10 > u && v >= 0 && v < 4;
    case 3:
        text_xy_to(p->row, p->col, a, b, &u, &v);
        if (horiz) {
            if (v < 0)
                return arrow_track(p, p->row, p->col - 6, 0x17, -1);
            if (p->len + 4 < v)
                return arrow_track(p, p->row, p->col + p->len + 2, 0x15, 1);
            n = thumb_pos(p, *p->val);
            text_offset_xy(0, n, 0, 0x1f44, &u, &z);
            a = b;
            u = p->col;
        } else {
            if (u < 0)
                return arrow_track(p, p->row - 5, p->col, 0xc, -1);
            if (p->len + 4 < u)
                return arrow_track(p, p->row + p->len + 5, p->col, 0xe, 1);
            n = thumb_pos(p, *p->val);
            text_offset_xy(n, 0, 0x1f44, 0, &z, &v);
            u = p->row;
        }
        if (a < n)
            return arrow_track(p, u, n, -1, -2);
        if (a >= z)
            return arrow_track(p, z, p->len + u + 4, -1, 2);
        return thumb_drag(p, n - a);
    case 4:
    case 5:
        break;
    case 0x20:
        u = *p->val;
        if (a == u)
            break;
        *p->val = a;
    moved:                              /* 1e8f:0c16; 0x23 jumps here from 0c56 */
        scroll_fix(p, u);
        break;
    case 0x23:
        u = *p->val;
        p->val = *(int far * far *)&a;
        if (*p->val == u)
            break;
        goto moved;
    case 0x24:
        if (a < 0)
            a = 0;
        if (p->width == a)
            break;
        u = thumb_pos(p, *p->val);
        p->width = a;
        if (*p->val >= a)
            *p->val = a - 1;
        thumb_move(p, u, thumb_pos(p, *p->val));
        break;
    default:
        return ui_item_proc(p, msg, a, b);
    }
    return 0;
}
