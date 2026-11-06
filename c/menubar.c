/* CKIT.EXE overlay section 15, module 2999: the menu bar -- up to 12 pop-up
 * menus of module 1ff3's kind (menubox.c, section 5) side by side on one
 * text row, each under its title.  bar_add places a menu after the one
 * before it (its title's width is four columns a character plus 8) and
 * bar_draw draws the titles; bar_proc is the bar's dialog item handler,
 * which tests a point against the bar's rows (message 2), tracks the
 * pointer over the titles (3, bar_track) and looks a key up as a hotkey of
 * every menu (5, bar_key); bar_take hands over the last result, the menu's
 * number from 1 in the high byte and its entry from 1 in the low byte.
 * bar_query reads a field of the bar, a menu or an entry; bar_set and
 * bar_set_range pass entries on to module 219b's empty setter.  Every call
 * into a menu goes through a vector stub to section 5's module 219b
 * (menuops.c).
 *
 * Section 15 (parent section 13; the parent of sections 16, 17 and 18):
 * payload at EXE 0x062c80, link segment 28aa (section 14's too).  A bare
 * stored SEG:OFF in this file means section 15.  The module is stored frame
 * 2999 (load-based 3999), EXE 0x063b77..0x064116 (1,439 bytes, 10
 * routines), from byte 7 of the paragraph -- 2999:0000..0006 is the tail of
 * module 28aa (numfield.c), whose last retf is at 2999:0006 -- to its last
 * retf; 10 zero bytes follow to the section's end (EXE 0x064120).  One
 * Turbo C large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z
 * -d; functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.
 *
 * Section 15's relocation table files 26 sites under 2999, all in this
 * range: the module's 26 far calls.  Its four vectors land here: stubs
 * 18de:1fa3 (bar_draw), 1fc1 (bar_take), 1fdf (bar_track) and 1fe9
 * (bar_query), each far-called once from section 21.  Section 18 far-calls
 * bar_clear once, bar_add three times and bar_draw once, and passes
 * 2999:0500 (bar_proc) to ui_add (25e5:1293) as an op-8 item handler (EXE
 * 0x06ea97).  bar_track is a vector target, so public.  bar_proc
 * near-calls bar_key, and bar_set_range bar_set, each after its
 * definition, so the bytes do not say whether those two are static.
 * Nothing found reaches bar_set_range, and so nothing reaches bar_set.
 *
 * _DATA is two literals, DGROUP 9758..977c (the second is written twice
 * and stored once, -d), right after resident diskfile.c's _DATA
 * (96d0..9757); 977d, zero, is a pad byte before dispinit.c's (section 6,
 * 977e).  _BSS is nbars, bar_flags, bar_menus[12] and bar_result, DGROUP
 * d59e..d601, after diskfile.c's _BSS (d592..d59c, then d59d, which nothing
 * found references) and before vidmouse.c's (d602); names chosen with
 * goldbox.bss_order.  No goldbox.toml sym, other module's placement or
 * recovered listing outside this range names d59e..d601.  FRUA's own code:
 * the sharing map pairs none of the 10 routines with DQK (all a-only), so
 * every name here is chosen from the code.
 */

struct Entry {                  /* one menu entry, 8 bytes, as menuops.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6 hotkey                              */
    char flags;                 /* +7                                     */
};

struct Menu {                   /* a pop-up menu, as menubox.c            */
    char far *title;            /* +0                                     */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8 row the entries are placed from     */
    int col;                    /* +a                                     */
    char count;                 /* +c entries                             */
    char flags;                 /* +d 1 passed to menu_text as the title's
                                      entry flags; 4 set by bar_add
                                      (menubox.c: no title, among others);
                                      0x10 five row units an entry, else
                                      four; 0x20 see bar_track            */
    int glyph;                  /* +e end-cap glyph                       */
    int row;                    /* +10                                    */
    int width;                  /* +12 4-column cells                     */
};

struct Bar {                    /* one menu on the bar, 8 bytes           */
    struct Menu far *menu;      /* +0                                     */
    int col;                    /* +4 its title's left column             */
    int width;                  /* +6 its title's width, in columns       */
};

struct Wid;                     /* a dialog item, as dialog.c's; only passed on */

extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far screen_page_save(void);                              /* 0f5e:0291 */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern char far mouse_clicked(int far *x, int far *y);            /* 0f5e:0fa2 */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern int  far ui_item_proc(struct Wid far *w, int msg, int a, int b); /* 15e5:01c3 */
extern int  far menu_query(struct Menu far *m, int item, int q);
                                       /* 18de:18e1, vector to overlay 5 219b:020f */
extern int  far menu_key(struct Menu far *m, int key);
                                       /* 18de:18eb, vector to overlay 5 219b:0184 */
extern void far menu_call_hook(void);    /* 18de:18f5, vector to overlay 5 219b:013a */
extern void far menu_text(int row, int col, int width, char far *text, int key,
                             int eflags, int glyph, int mflags);
                                       /* 18de:18ff, vector to overlay 5 219b:0324 */
extern void far menu_box_draw(struct Menu far *m);
                                       /* 18de:1909, vector to overlay 5 219b:000e */
extern int  far menu_box_run(struct Menu far *m);
                                       /* 18de:1913, vector to overlay 5 219b:0148 */
extern int  far menu_box_width(struct Menu far *m);
                                       /* 18de:191d, vector to overlay 5 219b:0162 */
extern void far menu_item_set_stub(struct Menu far *m, int a, int b, int c, int d);
                                       /* 18de:1927, vector to overlay 5 219b:020a */

static char nbars;                                               /* d59e */
static char bar_flags;          /* d59f 0x10, 0x20: set by a menu with that flag */
static struct Bar bar_menus[12];                                 /* d5a0 */
static int bar_result;                                           /* d600 */

void far bar_clear(void)                                         /* 3999:0007 */
{
    nbars = 0;
    bar_flags = 0;
}

/* Puts m on the bar after the last menu, at text column 8004 for the
 * first.  Section 18's three calls pass a second argument, always 0, which
 * the code does not read. */
void far bar_add(struct Menu far *m, char unused)                /* 3999:0012 */
{
    register int col;

    if (nbars >= 12) {
        error_box("Too many menus");
        return;
    }
    if (m->flags & 0x10)
        bar_flags |= 0x10;
    if (m->flags & 0x20)
        bar_flags |= 0x20;
    m->flags |= 4;
    bar_menus[nbars].menu = m;
    if (nbars != 0)
        col = bar_menus[nbars - 1].col + bar_menus[nbars - 1].width + 4;
    else
        col = 8004;
    bar_menus[nbars].col = col;
    m->base = 8004;
    if (bar_flags & 0x10)
        m->base++;
    m->col = col;
    bar_menus[nbars].width = str_len(m->title) * 4 + 8;
    nbars++;
}

/* The last menu first: the first whose hotkey is key gives (its number
 * from 1) << 8 plus the entry's; 0 for none. */
int far bar_key(int key)                                         /* 3999:0116 */
{
    register int i;
    register int r;

    i = nbars;
    while (--i >= 0)
        if ((r = menu_key(bar_menus[i].menu, key)) > 0)
            return ((i + 1) << 8) + r;
    return 0;
}

/* Tracks the pointer from screen point (x, y): over a title the menu under
 * it is drawn (its column at most 0x1fe0 less four times its width, and
 * a rectangle filled over the title unless bar flag 0x20 is set)
 * and run.  A run giving an entry n gives (the menu's number from 1) << 8
 * plus n; a run giving 0 gives 0, and one giving less than 0 goes on
 * tracking; with no menu open, 0 once mouse_clicked answers nonzero. */
int far bar_track(int x, int y)                                  /* 3999:015f */
{
    struct Menu far *m;
    int left;
    int r;
    register int i;
    register int cur;

    cur = -1;
    draw_page_set(1);
    screen_page_save();
    for (;;) {
        text_xy_to(8000, 8000, x, y, &x, &y);
        if ((bar_flags & 0x10 ? 5 : 4) > x) {
            for (i = 0; nbars > i; i++) {
                left = bar_menus[i].col - 8000;
                if (y >= left && bar_menus[i].width + left >= y)
                    break;
            }
            if (nbars <= i)
                i = -1;
            if (cur != i) {
                if (cur >= 0) {
                    screen_page_show();
                    screen_page_save();
                }
                if (i >= 0) {
                    m = bar_menus[i].menu;
                    m->col = int_min(bar_menus[i].col, 0x1fe0 - menu_box_width(m) * 4);
                    r = 0;
                    if (!(bar_flags & 0x20))
                        fill_rect(bar_flags & 0x10 ? (screen_rows() >= 300 ? 2 : 1) : 8000,
                                     bar_menus[i].col + r, (bar_flags & 0x10 ? 5 : 4) + 8000,
                                     bar_menus[i].col + bar_menus[i].width - r, 0x10f);
                    menu_box_draw(m);
                }
                cur = i;
            }
        }
        if (cur >= 0) {
            if ((r = menu_box_run(m)) >= 0) {
                screen_page_show();
                screen_page_save();
                if (r == 0)
                    return 0;
                return ((cur + 1) << 8) + r;
            }
            mouse_pos(&x, &y);
        } else {
            if (mouse_clicked(&x, &y))
                return 0;
            menu_call_hook();
        }
    }
}

/* Draws every menu's title on the bar. */
void far bar_draw(void)                                          /* 3999:0373 */
{
    struct Menu far *m;
    register int i;

    for (i = 0; nbars > i; i++) {
        m = bar_menus[i].menu;
        menu_text(8000, bar_menus[i].col, bar_menus[i].width >> 2, m->title, 0,
                     m->flags & 1, m->glyph, m->flags | 2);
    }
}

/* Menu n (from 1), entry item: passed on to 219b:020a, which does nothing. */
void far bar_set(int n, int item, int a, int b, int c)           /* 3999:03f8 */
{
    register int i;

    i = n - 1;
    if (i < 0 || nbars <= i)
        error_box("Invalid menu num (%d)", i);
    menu_item_set_stub(bar_menus[i].menu, item, a, b, c);
}

void far bar_set_range(int n, int first, int last, int a, int b, int c) /* 3999:0446 */
{
    register int i;

    for (i = first; i <= last; i++)
        bar_set(n, i, a, b, c);
}

/* Menu n (from 1): n 0 gives the number of menus; an item from 0 (0 the
 * menu itself) asks menu_query; a negative item gives the title's column
 * (q 0x28) or width (0x2d), and 0 for any other q. */
int far bar_query(int n, int item, int q)                        /* 3999:046c */
{
    register int i;

    i = n - 1;
    if (i < -1 || nbars <= i)
        error_box("Invalid menu num (%d)", i);
    if (i < 0)
        return nbars;
    if (item >= 0)
        return menu_query(bar_menus[i].menu, item, q);
    switch (q) {
    case 0x28:
        return bar_menus[i].col;
    case 0x2d:
        return bar_menus[i].width;
    }
    return 0;
}

/* The bar's dialog item handler. */
int far bar_proc(struct Wid far *w, int msg, int a, int b)       /* 3999:0500 */
{
    int row;
    int col;

    switch (msg) {
    case 2:
        text_xy_to(8000, 8000, a, b, &row, &col);
        return (bar_flags & 0x10 ? 5 : 4) > row;
    case 3:
        bar_result = bar_track(a, b);
        return bar_result > 0;
    case 5:
        bar_result = bar_key(a);
        return bar_result > 0;
    default:
        return ui_item_proc(w, msg, a, b);
    }
}

/* The last result, cleared. */
int far bar_take(void)                                           /* 3999:0597 */
{
    register int r;

    r = bar_result;
    bar_result = 0;
    return r;
}
