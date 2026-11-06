/* CKIT.EXE overlay section 5, module 219b: helpers for the menus of module
 * 1ff3 (menubox.c, the pop-up menu) -- the hit test and the hotkey search
 * over a menu's entries, the query that reads a menu's or an entry's
 * fields, an idle hook, a menu line drawn at a text-relative row, and thin
 * wrappers over menubox.c's pop_draw, pop_run and pop_width.
 *
 * Section 5 (parent section 3): payload at EXE 0x0334e0, link segment
 * 1c86.  A bare stored SEG:OFF in this file means section 5.  The module is
 * stored frame 219b (load-based 319b), EXE 0x03863e..0x0389b1 (883 bytes,
 * 11 routines), from byte e of the paragraph -- 219b:0000..000d is the tail
 * of module 1ff3, whose last retf is at 219b:000d -- to its last retf; 15
 * zero bytes follow to the section's end (EXE 0x0389c0).  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call (section 5's frame 1ff3 for menubox.c's pop_ callees), or
 * the DGROUP offset.  struct View is menubox.c's scrolled-menu window; it
 * is left incomplete here, since only null pointers to it are passed.
 *
 * Section 5's relocation table files 8 sites under 219b, all in this range:
 * the far calls to 1ff3 (4), 107e (2), 00d7 and 0da7 (stored).  Eight routines are
 * vector targets (stubs 18de:18e1..1927, all far-called from section 15
 * and two also from section 21); module 1ff3 far-calls 0028, 013a (twice),
 * 0184 and 020f.  Nothing found reaches 0174 or 0203.
 *
 * _DATA is menu_hook alone, DGROUP 5ee4..5ee7 (zero), after module 1ff3's
 * "\x7f" at 5ee1 and a pad byte, and before section 6's config.c (5ee8).
 * No _BSS.  Every name here is chosen from the code (FRUA's own module; the
 * sharing map's one shape pairing, 0174 with DQK's set_selected, is not a
 * partner: FRUA's menu.c defines set_selected at 183a:0317, which stores to
 * ad01/ad03; this routine stores to 5ee4/5ee6).
 */

struct Entry {                  /* one menu entry, 8 bytes                */
    char far *text;             /* +0 not read here (menubox.c's text)    */
    int val;                    /* +4 query 0x2a                          */
    char key;                   /* +6 hotkey; A-Z means Ctrl+letter       */
    char flags;                 /* +7 2 takes no place; 0x81 not pickable */
};

struct Menu {
    char far *title;            /* +0 not read here (menubox.c's title)   */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8 not read here (menubox.c's base)    */
    int col;                    /* +a                                     */
    char count;                 /* +c entries                             */
    char flags;                 /* +d 4 no Esc and no beep; 0x10 five
                                      text_xy row units an entry, else four */
    int glyph;                  /* +e not read here (menubox.c's glyph)   */
    int row;                    /* +10                                    */
    int width;                  /* +12 4-column cells                     */
};

struct View;

extern int  far to_lower(int c);                             /* 00d7:25ab */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern int  far pop_run(struct Menu far *m, int attr, struct View far *v); /* 1ff3:0804 */
extern void far pop_draw(struct Menu far *m, int attr, struct View far *v); /* 1ff3:0e7f */
extern int  far pop_width(struct Menu far *m);                   /* 1ff3:121a */
extern void far pop_line(int row, int col, int width, char far *text, int attr, int key,
                         int eflags, int glyph, int mflags);     /* 1ff3:17e2 */

static void (far *menu_hook)(void) = 0;                          /* 5ee4 */

void far menu_box_draw(struct Menu far *m)                        /* 319b:000e */
{
    pop_draw(m, 0x8f, (struct View far *)0);
}

/* Which entry is under the screen point (x, y): its number from 1, with
 * its offset from the menu's row in *pos (text_xy row units, txthigh_dots
 * / 2 each); 0 and *pos -1 when none is, -1 when the point is above the
 * menu. */
int far menu_hit(struct Menu far *m, int x, int y, int far *pos) /* 319b:0028 */
{
    short span;
    char in;
    register int n;
    register int i;

    span = m->width * 4;
    text_xy_to(m->row, m->col, x, y, &x, &y);
    in = x >= 0 && y >= 0 && y < span;
    if (in) {
        if (m->flags & 0x10)
            x = x / 5;
        else
            x >>= 2;
        for (i = n = 0; n <= x && m->count > i; i++)
            if (!(m->items[i].flags & 2))
                n++;
        if (n <= x || m->items[i - 1].flags & 0x81) {
            in = 0;
            n = -1;
        } else {
            n--;
            if (m->flags & 0x10)
                n *= 5;
            else
                n <<= 2;
        }
    } else
        n = -1;
    *pos = n;
    if (x < 0)
        return -1;
    return in ? i : 0;
}

void far menu_call_hook(void)                                    /* 319b:013a */
{
    if (menu_hook)
        (*menu_hook)();
}

int far menu_box_run(struct Menu far *m)                         /* 319b:0148 */
{
    return pop_run(m, 0x8f, (struct View far *)0);
}

int far menu_box_width(struct Menu far *m)                         /* 319b:0162 */
{
    return pop_width(m);
}

void far menu_set_hook(void (far *fn)(void))                     /* 319b:0174 */
{
    menu_hook = fn;
}

/* The entry whose hotkey equals key lowered by to_lower, numbered
 * from 1; 0 for Esc or ` unless flag 4 is set, -1 (with a beep unless
 * flag 4 is set) for no entry. */
int far menu_key(struct Menu far *m, int key)                    /* 319b:0184 */
{
    register int c;
    register int i;

    if (!(m->flags & 4) && (key == 0x1b || key == 0x60))
        return 0;
    for (i = 0; m->count > i; i++) {
        c = m->items[i].key;
        if (c >= 'A' && c <= 'Z')
            c &= 0x1f;
        if (c && to_lower(key) == c)
            return i + 1;
    }
    if (!(m->flags & 4))
        beep();
    return -1;
}

int far menuops_0203(struct Menu far *m)                         /* 319b:0203 */
{
    return 0;
}

void far menu_item_set_stub(struct Menu far *m, int a, int b, int c, int d) /* 319b:020a */
{
}

/* A field of the menu (item < 1) or of entry item (from 1); 0 for any
 * other q.  q 0x10 up tests flag bit q - 0x10; the menu's case 2 shares that
 * body and always gives 0 (1 << 0xf2 is 0), as shipped. */
int far menu_query(struct Menu far *m, int item, int q)          /* 319b:020f */
{
    struct Entry far *e;

    if (item < 1) {
        switch (q) {
        case 0x02:
        case 0x10:
        case 0x11:
        case 0x13:
            return (m->flags & (1 << (q - 0x10))) != 0;
        case 0x28:
            return m->col;
        case 0x2d:
            return m->width * 4;
        case 0x24:
            return m->count;
        }
    } else {
        e = m->items + item - 1;
        switch (q) {
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x17:
            return (e->flags & (1 << (q - 0x10))) != 0;
        case 0x20:
            return e->key;
        case 0x2a:
            return e->val;
        }
    }
    return 0;
}

/* Draws a menu line through pop_line.  Row 8000 is replaced by
 * text_xy's position for row 8001 (one txthigh_dots / 2 unit) plus one, and
 * then clears flag 0x10. */
void far menu_text(int row, int col, int width, char far *text, int key, int eflags,
                   int glyph, int mflags)                        /* 319b:0324 */
{
    int y;

    if (row == 8000) {
        row = 8001;
        text_xy(row, col, &row, &y);
        row++;
        mflags &= ~0x10;
    }
    pop_line(row, col, width, text, 0x8f, key, eflags, glyph, mflags);
}
