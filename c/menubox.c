/* CKIT.EXE overlay section 5, module 1ff3: the pop-up menu, a dialog item of
 * module 25e5's kind.  pop_proc is its handler: the item's val points at a
 * menu (a title, an array of 8-byte entries, a position and flags) and its
 * text at an optional view record that scrolls a long menu; width holds the
 * entry picked, from 1, and len the box's least width.  The item draws the
 * picked entry as its face (message 1); a press (message 3) draws the menu
 * (pop_draw), runs it (pop_run) until it returns an entry or 0, and draws
 * the face again.  A view record scrolls the entries under a '^' and a 'v'
 * arrow line (pop_scroll, pop_step); without one the whole menu is drawn and
 * run (plain_draw, plain_run).  pop_line draws one text line of a menu: an
 * entry, a title or an arrow.  pop_width sizes a menu to its widest entry.
 *
 * Section 5 (parent section 3): payload at EXE 0x0334e0, link segment 1c86.
 * A bare stored SEG:OFF in this file means section 5.  The module is stored
 * frame 1ff3 (load-based 2ff3), EXE 0x036bb9..0x03863e (6,789 bytes, 14
 * routines), from byte 9 of the paragraph -- 1ff3:0000..0008 is the tail of
 * module 1e8f, whose last retf is at 1ff3:0008 -- to its last retf at
 * 219b:000d; module 219b (menuops.c) starts at 219b:000e.  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.  <stdlib.h> is for abs, which Turbo C
 * compiles inline (pop_step), so the file is CRLF.
 *
 * Section 5's relocation table files 113 sites under 1ff3, all in this
 * range.  Six routines are vector targets: stubs 18de:18a5 (pop_hilite),
 * 18af (pop_scroll), 18b9 (pop_proc), 18c3 (pop_draw), 18cd (pop_run) and
 * 18d7 (pop_width).  Section 21 far-calls the other five stubs once each;
 * section 17 passes 18de:18b9 to ui_add as an op-8 item handler (EXE
 * 0x069d40).  Module 219b far-calls pop_run, pop_draw, pop_width and
 * pop_line.  Nothing else reaches 1ff3.  Turbo C far-calls a public function
 * of this module until it is defined; pop_face, pop_fit, pop_hit, pop_step,
 * pop_arrow, plain_run and plain_draw are near-called ahead of their
 * definitions, so they are static.
 *
 * _DATA is four literals, DGROUP 5ed0..5ee2; 5ee3 is a pad byte before
 * menuops.c's menu_hook (5ee4).  Below 5ed0, module 1e8f takes the
 * addresses of the literals at 5ec0 and 5ec3 (EXE 0x0368f7, 0x03697e),
 * module 1c86 and section 15 write byte 5eca and pass the addresses 5ecb
 * and 5ecd, and 5ecf is a pad byte nothing references.  _BSS is pop_attr, b390..b391, which only pop_proc writes and
 * nothing reads; module 1e8f references b386..b38e (EXE 0x035584..0x036a9d),
 * and nothing b38f.  Every name here is chosen from the code: the sharing
 * map pairs none of the 14 routines with DQK.
 */

#include <stdlib.h>

typedef int (far *ItemProc)();       /* (item, msg, a, b), as dialog.c's */
typedef int (far *ItemCb)(int id, ...);

struct Wid {                    /* one dialog item, 0x20 bytes, as dialog.c */
    ItemProc fn;                /* +00 handler                            */
    ItemCb cb;                  /* +04 called on activate                 */
    int far *val;               /* +08 here the menu                      */
    char far *text;             /* +0c here the view record, or 0         */
    int row;                    /* +10                                    */
    int col;                    /* +12                                    */
    int cursor;                 /* +14 pointer frame while over it        */
    int len;                    /* +16 here the least width               */
    int width;                  /* +18 here the entry picked, from 1      */
    int glyph;                  /* +1a                                    */
    char flags;                 /* +1c                                    */
    char key1;                  /* +1d hotkeys                            */
    char key2;                  /* +1e                                    */
    unsigned char color;        /* +1f                                    */
};

struct Entry {                  /* one menu entry, 8 bytes, as menuops.c  */
    char far *text;             /* +0 str_format format, or 0             */
    int val;                    /* +4 its argument                        */
    char key;                   /* +6 hotkey; A-Z means Ctrl+letter       */
    char flags;                 /* +7 1 greyed, 2 takes no place, 4 a
                                      0x7f mark, 0x81 not pickable        */
};

struct Menu {
    char far *title;            /* +0                                     */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8 row the entries are placed from     */
    int col;                    /* +a                                     */
    char count;                 /* +c entries                             */
    char flags;                 /* +d 4 no title, among others (gates
                                      menu_key, sizes from entry 1, drops
                                      the shadow; see menuops.c); 8 see
                                      plain_draw; 0x10 five row units an
                                      entry, else four; 0x20 set with a
                                      view record: no title               */
    int glyph;                  /* +e end-cap glyph, bank << 7 | index    */
    int row;                    /* +10 the window's top row (from base)   */
    int width;                  /* +12 4-column cells                     */
};

struct View {                   /* a scrolled menu's window               */
    int above;                  /* +0 entries hidden above; < 0 blank rows */
    int below;                  /* +2 entries hidden below; < 0 blank rows */
    int count;                  /* +4 entries that take a place, in all   */
    int bottom;                 /* +6 the row below the last one shown    */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far screen_page_save(void);                              /* 0f5e:0291 */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern int  far pointer_mode_set(int mode);                          /* 0f5e:0a85 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern char far mouse_clicked(int far *x, int far *y);            /* 0f5e:0fa2 */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern void far mouse_move_to(int row, int col);                 /* 0f5e:10a3 */
extern void far mouse_fake(int button, char both);               /* 0f5e:10d5 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far text_offset_xy(int row, int col, int row2, int col2,
                             int far *out_row, int far *out_col); /* 107e:0565 */
extern void far win_scroll(int row1, int col1, int row2, int col2,
                           int drow, int dcol, int attr);        /* 10f8:0008 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far pointer_glyph_load(int icon, int frame);               /* 11d7:0cf9 */
extern int  far ui_item_proc(struct Wid far *w, int msg, int a, int b); /* 15e5:01c3 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern int  far menu_hit(struct Menu far *m, int x, int y, int far *pos); /* 219b:0028 */
extern void far menu_call_hook(void);                            /* 219b:013a */
extern int  far menu_key(struct Menu far *m, int key);           /* 219b:0184 */
extern int  far menu_query(struct Menu far *m, int item, int q); /* 219b:020f */

extern int scratch_word;                                            /* b41f */
extern struct Wid far *dl_items;                                 /* e6a3 */

static void pop_face(struct Wid far *p, struct Menu far *m, int attr);
static void pop_fit(struct Wid far *p, struct Menu far *m, struct View far *v);
int far pop_run(struct Menu far *m, int attr, struct View far *v);
int far pop_scroll(struct Menu far *m, struct View far *v, int delta, int attr, int off,
                   int pos, int far *last, int n);
static int pop_hit(struct Menu far *m, int x, int y, int far *pos, struct View far *v);
void far pop_draw(struct Menu far *m, int attr, struct View far *v);
int far pop_width(struct Menu far *m);
static void pop_step(struct Menu far *m, int far *cur, int dir, int row, int attr);
static void pop_arrow(struct Menu far *m, int row, int attr, char up);
static int plain_run(struct Menu far *m, int attr);
static void plain_draw(struct Menu far *m, int attr);
void far pop_line(int row, int col, int width, char far *text, int attr, int key,
                  int eflags, int glyph, int mflags);

static int pop_attr;                                             /* b390 */

/* 0x100 with the colour's two nibbles xored, or 0x118 when the high nibble
 * is 8: pop_run and plain_run fill the entry under the pointer with it, and
 * the one it left again with it. */
int far pop_hilite(register int attr)                            /* 2ff3:0009 */
{
    if ((attr & 0xf0) == 0x80)
        return 0x118;
    return (attr >> 4 ^ attr & 0xf) | 0x100;
}

int far pop_proc(struct Wid far *w, int msg, int a, int b)       /* 2ff3:0033 */
{
    char far *s;
    int x;
    int y;
    int r1;
    int c1;
    int r2;
    int c2;
    struct Wid far *p;
    struct Menu far *m;
    struct View far *v;
    register int rc;
    register int attr;

    if (w == 0)
        return 0;
    p = w;
    m = (struct Menu far *)p->val;
    v = (struct View far *)p->text;
    if (m == 0)
        return 0;
    rc = 0;
    switch (msg) {
    case 0:
        if (v)
            m->flags |= 0x20;
        if (p->width < 1 || menu_query(m, 0, 0x24) < p->width)
            p->width = 1;
        pop_fit(p, m, v);
        if (p->len == 0)
            p->len = pop_width(m);
        if (m->title)
            p->col = p->col + str_len(m->title) * 4 + 4;
        if (p->color == 0)
            p->color = 0x87;
        m->col = p->col;
        break;
    case 1:
        p->flags |= 0x80;
        if (p->flags & 2)
            break;
        if (m->title)
            text_print_at(p->row, p->col - str_len(m->title) * 4 - 4,
                          (p->flags & 1) ? 0x180 : p->color, m->title);
        attr = (p->flags & 1) ? 0x180 : 0x70;
        pop_attr = attr;
        if (pop_width(m) < p->len)
            m->width = p->len;
        text_xy(p->row, p->col, &r1, &c1);
        text_xy(p->row + ((m->flags & 0x10) ? 5 : 4), p->col + p->len * 4, &r2, &c2);
        if (m->flags & 0x10)
            r1 += (screen_rows() >= 300 ? 1 : 0) + 1;
        fill_rect(r1 - 2, c1 - 1, r2 + 1, c2 + 1, 0);
        fill_rect(r1 - 1, c1, r2 + 2, c2 + 2, 0);
        fill_rect(r1 - 1, c1, r2, c2, (attr & 0xf0) >> 4);
        pop_face(p, m, attr);
        break;
    case 2:
        if (p->flags & 3)
            rc = 0;
        else {
            text_xy_to(p->row, p->col, a, b, &x, &y);
            rc = x >= 0 && x < ((m->flags & 0x10) ? 5 : 4) && y >= 0 && y < m->width * 4;
        }
        break;
    case 3:
        screen_page_save();
        pop_draw(m, 0x70, v);
        while ((x = pop_run(m, 0x70, v)) < 0)
            ;
        if (x > 0) {
            p->width = x;
            rc = 1;
        } else if (v->above || v->below)
            pop_fit(p, m, v);
        draw_page_set(0);
        pop_face(p, m, 0x70);
        draw_page_set(1);
        p->flags |= 0x80;
        screen_page_show();
        break;
    case 4:
        pop_fit(p, m, v);
        if (p->cb)
            (*p->cb)(p - dl_items, p->width);
        p->flags &= 0x7f;
        break;
    case 5:
        if (!(m->flags & 4))
            break;
        x = menu_key(m, a);
        if (x > 0)
            p->width = x;
        rc = x > 0;
        break;
    case 0x24:
        p->width = a;
        pop_fit(p, m, v);
        p->flags &= 0x7f;
        break;
    case 0x27:
        s = *(char far * far *)&a;
        if (p->text == s)
            break;
        p->text = s;
        p->flags &= 0x7f;
        if (s) {
            p->flags |= 0x20;
            pop_fit(p, m, (struct View far *)s);
        }
        break;
    default:
        rc = ui_item_proc(p, msg, a, b);
    }
    return rc;
}

/* The item's face: the entry picked, drawn in the box at the item. */
static void pop_face(struct Wid far *p, struct Menu far *m, int attr) /* 2ff3:054d */
{
    char buf[42];
    struct Entry far *e;

    if (p == 0)
        return;
    if (m == 0)
        return;
    if ((e = m->items + p->width - 1) != 0 && e->text)
        str_format(buf, e->text, e->val);
    else
        buf[0] = 0;
    pop_line(p->row, p->col, m->width, buf, attr, e->key, e->flags, m->glyph, m->flags);
}

/* Places the menu so the entry picked lies at the item's row: without a
 * view record by moving base; with one by choosing the window, counting
 * the entries above and below it, and the rows it shows. */
static void pop_fit(struct Wid far *p, struct Menu far *m, struct View far *v) /* 2ff3:05fc */
{
    unsigned char step;
    struct Entry far *e;
    register int old;
    register int cur;

    if (v == 0) {
        p->flags &= 0xdf;
        m->base = p->row - (p->width - 1) * ((m->flags & 0x10) ? 5 : 4);
        return;
    }
    if (m->flags & 4) {
        old = p->width;
        p->width = 1;
    }
    step = (m->flags & 0x10) ? 5 : 4;
    v->above = p->width - 1 - (p->row - 8008) / step;
    v->below = m->count - p->width - (8092 - p->row) / step - 1;
    v->count = 0;
    for (scratch_word = p->width - 2, e = m->items + scratch_word; scratch_word >= 0; scratch_word--, e--)
        if (e->flags & 2)
            v->above--;
        else
            v->count++;
    cur = v->count++;
    for (scratch_word = p->width, e = m->items + scratch_word; m->count > scratch_word; scratch_word++, e++)
        if (e->flags & 2)
            v->below--;
        else
            v->count++;
    while (v->above < 0 && v->below < -v->above)
        v->above++;
    while (v->below < 0 && -v->below > v->above)
        v->below++;
    m->base = p->row - (cur - v->above) * step;
    v->bottom = (v->count - v->above - v->below) * step + m->base;
    if (m->flags & 4)
        p->width = old;
}

/* Runs a drawn menu until a button goes down: the entry under the pointer
 * is marked, keys 0x108 and 0x104 move the pointer one entry's rows, Enter
 * fakes a click, Esc or ` moves the pointer to 0,0 and fakes one, and the
 * window scrolls when the pointer reaches an arrow line.  Returns the entry
 * under the pointer, from 1, or 0; -1 when pop_hit gives -1 with no button. */
int far pop_run(struct Menu far *m, int attr, struct View far *v) /* 2ff3:0804 */
{
    char btn;
    int x;
    int y;
    short span;
    short step;
    short off;
    int pos;
    int last;
    short delta;
    int mx;
    int my;
    register int n;
    register int row;

    mouse_pos(&mx, &my);
    if (v == 0)
        return plain_run(m, attr);
    row = m->row;
    span = m->width * 4;
    step = (m->flags & 0x10) ? 5 : 4;
    off = 0;
    last = -1;
    do {
        btn = mouse_clicked(&x, &y);
        if (!pointer_mode_set(-1) && key_poll()) {
            n = key_get();
            if (n == '\r' || n == '\n')
                mouse_fake(1, 0);
            else if (n == 0x1b || n == '`') {
                mouse_move_to(0, 0);
                mouse_fake(1, 0);
            } else if (n == 0x108) {
                text_offset_xy(x, y, 8000 - ((m->flags & 0x10) ? 5 : 4), 0, &x, &y);
                if (x < 0)
                    x = 0;
                mouse_move_to(x, y);
            } else if (n == 0x104) {
                text_offset_xy(x, y, 8000 + ((m->flags & 0x10) ? 5 : 4), 0, &x, &y);
                if (x >= screen_rows())
                    x = screen_rows() - 1;
                mouse_move_to(x, y);
            }
        }
        n = pop_hit(m, x, y, &pos, v);
        if (pos != last && last >= 0) {
            text_xy(row + last, m->col + off, &x, &y);
            if (m->flags & 0x10) {
                if (screen_rows() >= 300)
                    x++;
                x++;
            }
            fill_rect(x, y, row + last + step, m->col + span - off, pop_hilite(attr));
        }
        delta = 0;
        if (pos >= 0) {
            if (pos == 0 && v->above > 0)
                delta = -step;
            else if (v->below > 0 && v->bottom == row + pos + step)
                delta = step;
            else if (pos != last) {
                text_xy(row + pos, m->col + off, &x, &y);
                if (m->flags & 0x10) {
                    if (screen_rows() >= 300)
                        x++;
                    x++;
                }
                fill_rect(x, y, row + pos + step, m->col + span - off, pop_hilite(attr));
            }
        }
        if (delta) {
            if (!btn)
                n = pop_scroll(m, v, delta, attr, off, pos, &last, n);
            else
                n = 0;
        } else if (pos != last)
            last = pos;
        pointer_glyph_load(0, n ? 1 : 2);
        menu_call_hook();
    } while (!btn && n >= 0);
    if (btn && n < 0)
        n = 0;
    if (!pointer_mode_set(-1))
        mouse_move_to(mx, my);
    return n;
}

/* Scrolls the window one entry's rows (delta < 0 toward the top): win_scroll
 * moves the rows, pop_arrow draws an arrow line that appears, pop_step draws
 * entry n and, at the list's end, moves n on; *last becomes pos, or -1.
 * Returns n. */
int far pop_scroll(struct Menu far *m, struct View far *v, register int delta,
                   register int attr, int off, int pos, int far *last, int n) /* 2ff3:0b60 */
{
    short r1;
    short r2;

    if (delta < 0) {
        r1 = m->row + pos - delta;
        r2 = (v->below < 0 ? -v->below - 1 : 1) * delta + v->bottom;
        v->above--;
        v->below++;
        if (v->below == 1)
            pop_arrow(m, v->bottom + delta, attr, 0);
    } else {
        r1 = m->row + (v->above < 0 ? -v->above - 1 : 1) * delta;
        r2 = m->row + pos;
        v->below--;
        v->above++;
        if (v->above == 1)
            pop_arrow(m, m->row, attr, 1);
    }
    win_scroll(r1, m->col + off, r2, m->col + m->width * 4 - off, delta + 8000, 0, attr >> 4 & 0xf);
    pop_step(m, &n, 0, delta > 0 ? r2 - delta : r1, attr);
    if (delta < 0 && v->above == 0) {
        pop_step(m, &n, delta, r1 + delta, (attr & 0xf0) >> 4 | (attr & 0xf) << 4);
        *last = pos;
    } else if (delta > 0 && v->below == 0) {
        pop_step(m, &n, delta, r2, (attr & 0xf0) >> 4 | (attr & 0xf) << 4);
        *last = pos;
    } else
        *last = -1;
    return n;
}

/* Which entry of a scrolled menu is under the screen point (x, y): its
 * number from 1, with its row offset in *pos; 0 and *pos -1 when none is,
 * -1 above the menu when flag 4 is set. */
static int pop_hit(struct Menu far *m, int x, int y, int far *pos, struct View far *v) /* 2ff3:0d19 */
{
    short span;
    short lim;
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
        x += v->above;
        lim = m->count - (v->below > 0 ? v->below : 0);
        if ((in = x >= 0 && x < lim) != 0)
            for (n = i = 0; n <= x && i < lim; i++)
                if (!(m->items[i].flags & 2))
                    n++;
        if (!in || n <= x || m->items[i - 1].flags & 0x81) {
            in = 0;
            n = -1;
        } else {
            n -= v->above + 1;
            if (m->flags & 0x10)
                n *= 5;
            else
                n <<= 2;
        }
    } else
        n = -1;
    *pos = n;
    if (m->flags & 4 && x < 0)
        return -1;
    return in ? i : 0;
}

/* Draws a scrolled menu: its title, the window's entries with an arrow line
 * for each side that hides entries, and the fills around the box. */
void far pop_draw(struct Menu far *m, int attr, struct View far *v) /* 2ff3:0e7f */
{
    struct Entry far *e;
    short row;
    int col;
    int x2;
    int y2;
    short wid;
    short i;
    short skip;
    char buf[80];
    register int step;

    if (v == 0) {
        plain_draw(m, attr);
        return;
    }
    row = m->base;
    col = m->col;
    wid = m->width;
    step = (m->flags & 0x10) ? 5 : 4;
    m->row = row;
    pointer_hide();
    if (!(m->flags & 0x24))
        pop_line(row - step, col, wid, m->title, attr, 0, 0, m->glyph, m->flags | 2);
    i = 0;
    for (e = m->items; v->above > i; e++)
        if (!(e->flags & 2))
            i++;
    if (v->above < 0) {
        fill_rect(row, col, row - v->above * step, col + wid * 4, attr >> 4 & 0xf);
        row -= v->above * step;
    } else if (v->above > 0) {
        pop_arrow(m, row, attr, 1);
        row += step;
        i++;
    }
    skip = v->below > 0 ? v->below + 1 : 0;
    for (e = m->items + i; v->count - skip > i; i++, e++) {
        if (e->flags & 2)
            continue;
        if (e->text)
            str_format(buf, e->text, e->val);
        else
            buf[0] = 0;
        pop_line(row, col, wid, buf, attr, e->key, e->flags, m->glyph, m->flags);
        row += step;
    }
    if (v->below > 0) {
        pop_arrow(m, row, attr, 0);
        row += step;
    } else if (v->below < 0) {
        fill_rect(row, col, row - v->below * step, col + wid * 4, attr >> 4 & 0xf);
        row -= v->below * step;
    }
    text_xy(row, col + 1, &row, &col);
    text_xy(m->base + 1, m->col + wid * 4, &x2, &y2);
    if (m->flags & 0x10)
        row += (screen_rows() >= 300 ? 1 : 0) + 1;
    if (!(m->flags & 4)) {
        fill_rect(x2 - 3, col - 2, x2 - 2, y2, attr >> 4 & 0xf);
        fill_rect(x2 - 4, col - 3, x2 - 3, y2 + 1, 0);
    }
    fill_rect(x2 - 3, col - 3, row, col - 2, 0);
    fill_rect(row, col - 3, row + 1, y2 + 2, 0);
    fill_rect(row + 1, col, row + 2, y2 + 2, 0);
    fill_rect(x2 - 3, y2, row, y2 + 1, 0);
    fill_rect(x2, y2 + 1, row, y2 + 2, 0);
    pointer_show();
}

/* The menu's width in 4-column cells: its widest entry (and its title
 * unless flag 4 or 0x20), two more for a Ctrl hotkey, two with an end-cap
 * glyph, and two; stored in m->width. */
int far pop_width(struct Menu far *m)                            /* 2ff3:121a */
{
    struct Entry far *e;
    short len;
    char buf[80];
    register int w;
    register int i;

    if (m->flags & 4 || m->flags & 0x20)
        w = 0;
    else
        w = str_len(m->title);
    for (e = m->items, i = 0; m->count > i; i++, e++) {
        if (e->flags & 2)
            continue;
        str_format(buf, e->text, e->val);
        len = str_len(buf);
        if (e->key && e->key >= 'A' && e->key <= 'Z')
            len += 2;
        w = int_max(w, len);
    }
    if (m->glyph)
        w += 2;
    w += 2;
    m->width = w;
    return w;
}

/* Moves *cur one entry in dir's direction, skipping entries that take no
 * place (flag 2), if there is one, and draws entry *cur at row. */
static void pop_step(struct Menu far *m, int far *cur, register int dir, int row,
                     int attr)                                   /* 2ff3:12e0 */
{
    char buf[80];
    short lim;
    struct Entry far *e;
    register int i;

    if (dir) {
        dir = dir / abs(dir);
        i = *cur + dir;
        lim = dir > 0 ? m->count : 1;
        for (e = m->items + i - 1; i != lim && e->flags & 2; e += dir, i += dir)
            ;
        if (!(e->flags & 2))
            *cur = i;
    }
    e = m->items + *cur - 1;
    if (e->text)
        str_format(buf, e->text, e->val);
    else
        buf[0] = 0;
    pop_line(row, m->col, m->width, buf, attr, e->key, e->flags, m->glyph, m->flags);
}

/* The '^' or 'v' line that says entries are hidden above or below. */
static void pop_arrow(struct Menu far *m, int row, int attr, char up) /* 2ff3:13f0 */
{
    char buf[2];

    str_format(buf, "%c", up ? '^' : 'v');
    pop_line(row, m->col, m->width, buf, attr, 0, 0, m->glyph, m->flags | 2);
}

/* pop_run for a menu without a view record: no key handling (Enter, Esc,
 * backquote, 0x108, 0x104), and pop_run returns its result directly, so
 * pop_run's pointer restore is skipped. */
static int plain_run(struct Menu far *m, int attr)               /* 2ff3:144a */
{
    short row;
    short span;
    int pos;
    int x;
    int y;
    short off;
    char btn;
    register int n;
    register int last;

    row = m->row;
    span = m->width * 4;
    last = -1;
    off = 0;
    do {
        btn = mouse_clicked(&x, &y);
        n = menu_hit(m, x, y, &pos);
        if (pos != last) {
            if (pos >= 0) {
                text_xy(row + pos, m->col + off, &x, &y);
                if (m->flags & 0x10) {
                    if (screen_rows() >= 300)
                        x++;
                    x++;
                }
                fill_rect(x, y, row + pos + ((m->flags & 0x10) ? 5 : 4),
                             m->col + span - off, pop_hilite(attr));
            }
            if (last >= 0) {
                text_xy(row + last, m->col + off, &x, &y);
                if (m->flags & 0x10) {
                    if (screen_rows() >= 300)
                        x++;
                    x++;
                }
                fill_rect(x, y, row + last + ((m->flags & 0x10) ? 5 : 4),
                             m->col + span - off, pop_hilite(attr));
            }
            last = pos;
        }
        pointer_glyph_load(0, n ? 1 : 2);
        menu_call_hook();
    } while (!btn && n >= 0);
    if (btn && n < 0)
        n = 0;
    return n;
}

/* pop_draw for a menu without a view record: every entry, and flag 8
 * moves the box down by the rows of the entries that take no place. */
static void plain_draw(struct Menu far *m, int attr)             /* 2ff3:15eb */
{
    struct Entry far *e;
    short row;
    int col;
    int x2;
    int y2;
    short k;
    short step;
    char buf[80];
    register int i;
    register int wid;

    row = m->base;
    col = m->col;
    wid = m->width;
    step = (m->flags & 0x10) ? 5 : 4;
    if (m->flags & 8) {
        k = 0;
        for (i = 0, e = m->items; m->count > i; i++, e++)
            if (e->flags & 2)
                k++;
        row += k * 4;
        if (m->flags & 0x10)
            row += k;
    }
    m->row = row;
    pointer_hide();
    if (!(m->flags & 4))
        pop_line(row - step, col, wid, m->title, attr, 0, 0, m->glyph, m->flags | 2);
    for (i = 0, e = m->items; m->count > i; i++, e++) {
        if (e->flags & 2)
            continue;
        if (e->text)
            str_format(buf, e->text, e->val);
        else
            buf[0] = 0;
        pop_line(row, col, wid, buf, attr, e->key, e->flags, m->glyph, m->flags);
        row += step;
    }
    text_xy(row, col + 1, &row, &col);
    text_xy(m->base + 1, m->col + wid * 4, &x2, &y2);
    if (m->flags & 0x10)
        row += (screen_rows() >= 300 ? 1 : 0) + 1;
    fill_rect(row, col, row + 1, y2 + 1, 0);
    fill_rect(x2, y2, row, y2 + 1, 0);
    pointer_show();
}

/* One line of a menu at (row, col), width cells wide (0: the text's own):
 * its background, end-cap glyphs, the text -- centred under mflags 2; one
 * starting '(' printed through "%(%c%1g%)" with its second character -- a
 * "^X" hotkey at the right, and the 0x7f mark under eflags 4.  eflags 1
 * greys it; mflags 0x10 makes it five row units high. */
void far pop_line(int row, register int col, int width, char far *text, int attr, int key,
                  int eflags, int glyph, int mflags)            /* 2ff3:17e2 */
{
    int pc;
    int pr;
    short span;
    register int r = row;

    if (width == 0)
        width = str_len(text) + (glyph ? 2 : 0) + (key && key >= 'A' && key <= 'Z' ? 2 : 0) + 2;
    span = width * 4;
    if (mflags & 0x10) {
        r++;
        text_xy(row, col, &pr, &pc);
        if (screen_rows() >= 300)
            pr++;
        pr++;
        fill_rect(pr, col, row + 5, col + span, (attr & 0xf0) >> 4);
        text_xy(row + 5, col, &pr, &pc);
        pc = pr + 1;
        if (screen_rows() >= 300)
            pc++;
        fill_rect(pr, col, pc, col + span, pixel_shift() == 3 && video_planes() == 1 ? 0 : 8);
    } else
        fill_rect(row, col, row + 4, col + span, (attr & 0xf0) >> 4);
    if (glyph) {
        draw_glyph(r, col, glyph >> 7, glyph & 0x7f);
        draw_glyph(r, col + span - 4, glyph >> 7, (glyph & 0x7f) + 1);
    }
    if (eflags & 1)
        attr = (attr & 0xf0) | 7 | 0x100u;
    if (*text == '(') {
        if (glyph)
            text_print_at(r, col + 4, attr, "%(%c%1g%)", width - 2, text[1]);
        else
            text_print_at(r, col, attr, "%(%c%1g%)", width, text[1]);
    } else if (mflags & 2)
        text_print_at(r, col + (width - str_len(text)) * 2, attr, text);
    else
        text_print_at(r, col + (glyph ? 4 : 0) + 4, attr, text);
    if (key && key >= 'A' && key <= 'Z')
        text_print_at(r, col + span - 8 - (glyph ? 4 : 0), attr, "^%c", key | 0x40);
    if (eflags & 4)
        text_print_at(r, col + (glyph ? 4 : 0), attr, "\x7f");
}
