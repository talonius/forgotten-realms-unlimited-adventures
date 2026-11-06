/* CKIT.EXE resident module 176b: the list box under menu.c's list_menu.
 * lbox_setup takes the box menu.c fills in, adds a dialog area for the
 * list with a click and a key handler, and either two arrow buttons (flags
 * 1) or a scroll bar through the item proc at 1e8f:0836 (flags 2), then
 * draws the list and selects its entry.  Entries come from the box's fetch
 * callback: fetch(n, &skip) returns entry n, fetch(-1, &skip) the one after
 * the last fetched, and a null pointer past the end; skip set means the
 * entry cannot be selected.  lbox_show scrolls the window by moving pixels
 * when the new top is less than a page away and redraws the rest;
 * lbox_select moves the highlight to an entry, scrolling it into view;
 * lbox_step moves the selection by a count and, when the target cannot be
 * selected, tries the entry one beyond it and then the one short of it.
 * The selected entry is drawn in colour 0xb, an unselectable one in 0xc
 * and the others in 7, all on 8.
 *
 * EXE 0x009abc..0x00a7a9, one Turbo C large-model _TEXT segment starting at
 * byte 0xc of paragraph 176b.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 16c9's last retf is at 0x009abb, and module 183a starts at
 * 0x00a7a9 (183a:0009) with no stub between.  All 43 MZ relocation sites
 * in the range have frame 076b; nothing outside the module stores a far
 * pointer to it but menu.c's one call to lbox_setup (0x00ca28).
 *
 * _DATA is 1ca0..1ca2, the "%s" literal; 1c9f ends module 16c9's " %s",
 * and 1ca3 is pad before module 183a's 1ca4.  _BSS is lbox_cur at ab9e,
 * between module 16c9's ab9d and module 183a's aba2; only this module
 * names it.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; scroll_proc's is the far pointer pushed as an item proc.  A name
 * another module here gives an address is reused from there; a modNNNN_
 * name is a placeholder that says only which segment the routine lives in.
 * lbox_setup keeps the name menu.c calls it by.  The sharing map pairs
 * the routines with Dark Queen of Krynn's frame 1b05 (1 exact, 5 shape, 2
 * modified, 1 FRUA-only), DQK's listbox.c.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is:
 * lbox_click and lbox_setup far-call lbox_select, which carries a
 * prototype.  lbox_key near-calls lbox_step ahead of its definition, which
 * only a static function allows.  lbox_key's `2e 3b 07` at 176b:06b3 is
 * Turbo C's sparse-switch value search, the census's cs:var marker.
 *
 * A coordinate from 0x1f40 up is a text cell: 0x1f40 + 4 * n is column or
 * row n, so (v - 0x1f40) >> 2 is the cell number status_printf and
 * text_clear take.
 */

#include <stdlib.h>

struct ListBox {                /* menu.c's list_menu frame; its names    */
    int row;                    /* +00 used as the row here               */
    int col;                    /* +02 used as the column here            */
    int rows;                   /* +04 entries shown at once              */
    int width;                  /* +06 width in cells                     */
    int flags;                  /* +08 1 arrow buttons, 2 a scroll bar    */
    int first;                  /* +0a first entry shown                  */
    int sel;                    /* +0c selected entry                     */
    int id_base;                /* +0e items count + 2, + 3: arrows, bar  */
    char far *(far *fetch)(int n, char far *skip); /* +10                 */
    char (far *done)(void);     /* +14                                    */
    void (far *on_select)(int sel); /* +18 told the selected entry            */
};

extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern void far win_scroll(int row1, int col1, int row2, int col2,
                           int drow, int dcol, int attr);        /* 10f8:0008 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern char far mouse_clicked(int far *x, int far *y);            /* 0f5e:0fa2 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern int  far scroll_proc();                                  /* 0e8f:0836 */

int far lbox_select(int n, char click, struct ListBox far *box);
static char lbox_step(int delta, struct ListBox far *box);

static struct ListBox far *lbox_cur;                             /* ab9e */

int far lbox_count(struct ListBox far *box)                      /* 176b:000c */
{
    char far *text;
    char skip;
    register int n;

    n = 0;
    text = (*box->fetch)(0, &skip);     /* 176b:001a mov ax,si: -O reuses n's 0 */
    while (text) {
        text = (*box->fetch)(-1, &skip);
        n++;
    }
    return n;
}

int far lbox_show(register int top, register int sel, char redraw,
                  struct ListBox far *box)                       /* 176b:0058 */
{
    int count;
    int offset;
    int i;
    int old_top;
    int old_sel;
    int row;
    int col;
    int width;
    int rows;
    int shift;
    int rc;
    char skip;
    char far *new_text;
    char far *old_text;
    char far *text;

    count = lbox_count(box);
    rows = box->rows;
    top = int_max(0, int_min(int_max(count, rows) - rows, top));
    sel = int_max(0, int_min(count - 1, sel));
    old_top = box->first;
    old_sel = box->sel;
    old_text = (*box->fetch)(old_sel, &skip);
    new_text = (*box->fetch)(sel, &skip);
    if (skip) {
        sel = old_sel;
        new_text = old_text;
        rc = -1;
    } else
        rc = 1;
    if (!redraw && old_top == top && old_sel == sel)
        return 0;
    row = box->row + 0x1f40;
    col = box->col + 0x1f40;
    width = box->width;
    if (top != old_top && abs(top - old_top) < rows && !redraw) {
        if (top > old_top) {
            shift = top - old_top;
            if (old_sel != sel && old_top + shift <= old_sel && old_top + rows > old_sel)
                status_printf((col - 0x1f40) >> 2, ((row - 0x1f40) >> 2) + (old_sel - old_top),
                              7, 8, "%s", old_text);
            win_scroll(row, col, row + rows * 4, col + width * 4, shift * 4 + 0x1f40, 0x1f40,
                       is_xvga() || !pixel_shift() ? 0x17 : 8);
            if (old_sel != sel && sel >= top && top + rows - shift > sel)
                status_printf((col - 0x1f40) >> 2, ((row - 0x1f40) >> 2) + (sel - top),
                              0xb, 8, "%s", new_text);
            offset = (rows - shift) * 4;
            i = old_top + rows;
        } else {
            shift = old_top - top;
            if (old_sel != sel && old_sel >= old_top && old_top + rows - shift > old_sel)
                status_printf((col - 0x1f40) >> 2, ((row - 0x1f40) >> 2) + (old_sel - old_top),
                              7, 8, "%s", old_text);
            win_scroll(row, col, row + rows * 4, col + width * 4, 0x1f40 - shift * 4, 0x1f40,
                       is_xvga() || !pixel_shift() ? 0x17 : 8);
            if (old_sel != sel && top + shift <= sel && top + rows > sel)
                status_printf((col - 0x1f40) >> 2, ((row - 0x1f40) >> 2) + (sel - top),
                              0xb, 8, "%s", new_text);
            offset = 0;
            i = top;
            rows = shift;
        }
    } else {
        offset = 0;
        i = top;
    }
    text = (*box->fetch)(i, &skip);
    do {
        text_clear((col - 0x1f40) >> 2, (row + offset - 0x1f40) >> 2,
                   ((col - 0x1f40) >> 2) + width - 1, (row + offset - 0x1f40) >> 2);
        if (sel != i)
            status_printf((col - 0x1f40) >> 2, (row + offset - 0x1f40) >> 2,
                          skip ? 0xc : 7, 8, "%s", text);
        else
            status_printf((col - 0x1f40) >> 2, (row + offset - 0x1f40) >> 2,
                          0xb, 8, "%s", text);
        offset += 4;
        i++;
    } while (top + rows > i && (text = (*box->fetch)(-1, &skip)) != 0);
    text_clear((col - 0x1f40) >> 2, (row + offset - 0x1f40) >> 2,
               ((col - 0x1f40) >> 2) + width - 1, ((row - 0x1f40) >> 2) + rows - 1);
    box->sel = sel;
    if (box->flags == 2)
        ui_mark(box->id_base + 2, 0x20, top);
    else
        box->first = top;
    return rc;
}

void far lbox_arrow(int id)                                      /* 176b:050c */
{
    struct ListBox far *box;
    register int page;

    box = lbox_cur;
    page = box->rows - 1;
    switch (id - box->id_base) {
    case 2:
        lbox_show(box->first - page, box->sel, 0, box);
        break;
    case 3:
        lbox_show(box->first + page, box->sel, 0, box);
        break;
    }
}

void far lbox_click(int id, int x, int y)                        /* 176b:057c */
{
    int row;
    int col;
    struct ListBox far *box;
    register int n;
    register int rc;

    box = lbox_cur;
    text_xy_to(box->row + 0x1f40, box->col + 0x1f40, x, y, &row, &col);
    n = box->first + (row >> 2);
    rc = lbox_select(n, 1, box);
    if (rc == 1 && box->done) {
        if ((*box->done)())
            ui_mark(id, 0x14);
    } else
        while (!mouse_clicked(&x, &y)) {
            text_xy_to(box->row + 0x1f40, box->col + 0x1f40, x, y, &row, &col);
            n = box->first + (row >> 2);
            lbox_select(n, 0, box);
        }
}

int far lbox_key(int id, int key)                                /* 176b:0667 */
{
    struct ListBox far *box;
    int first;
    register int page;

    box = lbox_cur;
    page = box->rows - 1;
    while (key_poll()) {
        if (key == 0x152)
            sound_toggle();
        else if (key == 0x153)
            music_toggle();
        key = key_get();
    }
    switch (key) {
    case 0x152:
        sound_toggle();
        return 0;
    case 0x153:
        music_toggle();
        return 0;
    case 0x107:
    case 0x108:
        first = box->first;
        if (lbox_step(-1, box) && first == 0)
            lbox_step(lbox_count(box) - 1, box);
        break;
    case 0x104:
    case 0x105:
        if (lbox_step(1, box))
            lbox_step(-box->sel, box);
        break;
    case 0x101:
        if (box->sel == box->first)
            page = box->rows - 1;
        else
            page = box->sel - box->first;
        lbox_step(-page, box);
        break;
    case 0x103:
        if (box->rows + box->first - 1 == box->sel)
            page = box->rows - 1;
        else
            page = box->rows + box->first - 1 - box->sel;
        lbox_step(page, box);
        break;
    default:
        return 0;
    }
    return 1;
}

void far lbox_bar(int id, int pos)                               /* 176b:07e4 */
{
    struct ListBox far *box;
    register int page;

    box = lbox_cur;
    page = box->rows - 1;
    switch (pos) {
    case -2:
        lbox_show(box->first - page, box->sel, 1, box);
        break;
    case -1:
        lbox_show(box->first - 1, box->sel, 1, box);
        break;
    default:
        lbox_show(box->first, box->sel, 1, box);
        break;
    case 1:
        lbox_show(box->first + 1, box->sel, 1, box);
        break;
    case 2:
        lbox_show(box->first + page, box->sel, 1, box);
        break;
    }
}

void far lbox_setup(struct ListBox far *box, char redraw)      /* 176b:08b6 */
{
    int row;
    int width;
    int first;
    int sel;
    int count;
    int glyph;
    int span;
    register int rows;
    register int col;

    lbox_cur = box;
    row = box->row + 0x1f40;
    col = box->col + 0x1f40;
    rows = box->rows;
    first = box->first;
    width = box->width;
    sel = box->sel;
    count = lbox_count(box);
    ui_add(5, row, col, rows * 4, width * 4, 0x22, lbox_click, 7, lbox_key, 0);
    col += width * 4;
    switch (box->flags) {
    case 1:
        ui_add(1, row + 1, col, (char far *)0, 0x22, lbox_arrow, 0x25, 0xc, 0x12,
               1, row + rows * 4 - 4, col, (char far *)0, 0x22, lbox_arrow, 0x25, 0xe, 0x12,
               0);
        break;
    case 2:
        if (rows < 7) {
            glyph = 0x14;
            span = 0xa;
        } else if (rows < 9) {
            glyph = 0x13;
            span = 0x10;
        } else if (rows < 0xd) {
            glyph = 0x12;
            span = 0x1a;
        } else {
            glyph = 0x11;
            span = 0x37;
        }
        ui_add(8, scroll_proc, 0x28, row + 5, col, 0x2a, span, 0x24, count - (rows - 1),
               0x23, &box->first, 0x25, glyph + 0x400, 0x22, lbox_bar, 0);
        break;
    }
    lbox_show(first, sel, redraw, box);
    lbox_select(sel, !redraw, box);
}

int far lbox_select(register int n, char click, struct ListBox far *box) /* 176b:0a7c */
{
    int first;
    int rows;
    int row;
    int col;
    int at;
    int rc;
    int count;
    char skip;
    char far *text;
    register int sel;

    sel = box->sel;
    first = box->first;
    rows = box->rows;
    row = box->row + 0x1f40;
    col = box->col + 0x1f40;
    count = lbox_count(box);
    n = int_max(0, int_min(count - 1, n));
    if (n < first)
        rc = lbox_show(n, n, 0, box);
    else if (first + rows <= n)
        rc = lbox_show(n - rows + 1, n, 0, box);
    else {
        text = (*box->fetch)(n, &skip);
        if (skip)
            return -1;
        rc = n == sel;
        if (!click && rc) {
            if (box->on_select)
                (*box->on_select)(box->sel);
            return 1;
        }
        if (sel >= first && sel - first < rows && sel < count) {
            at = (sel - first) * 4 + row;
            status_printf((col - 0x1f40) >> 2, (at - 0x1f40) >> 2, 7, 8, "%s",
                          (*box->fetch)(sel, &skip));
        }
        at = (n - first) * 4 + row;
        status_printf((col - 0x1f40) >> 2, (at - 0x1f40) >> 2, 0xb, 8, "%s", text);
        box->sel = n;
    }
    if (rc >= 0 && box->on_select)
        (*box->on_select)(box->sel);
    return rc;
}

static char lbox_step(register int delta, struct ListBox far *box) /* 176b:0c36 */
{
    int old;
    register int n;

    old = box->sel;
    n = box->sel + delta;
    n = int_max(0, int_min(lbox_count(box) - 1, n));
    delta = n - box->sel;
    if (lbox_select(box->sel + delta, 0, box) < 0) {
        if (delta > 0)
            delta++;
        else
            delta--;
        if (lbox_select(box->sel + delta, 0, box) < 0) {
            if (delta > 0)
                delta -= 2;
            else
                delta += 2;
            lbox_select(box->sel + delta, 0, box);
        }
    }
    return box->sel == old;
}
