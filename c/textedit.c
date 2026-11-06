/* CKIT.EXE overlay section 5, module 1c86: text fields, dialog items of
 * module 25e5's kind.  field_proc handles a one-line field: the item's
 * text is the line (at most 0x26 characters), its val a record holding a
 * copy of the line, the field's label and the cursor position.  box_proc
 * handles a box of six lines of 0x26 columns: the item's text is a struct
 * Box (0xeb bytes of text, the cursor, where each line starts and ends,
 * and which lines need drawing), its val a copy of it followed by the
 * box's label.  Both draw a frame and the text (message 1), take the focus
 * on a press (3) or when told (0x12), give it up on keys 0xd, 0xa, 0x1b,
 * 0x60 and 9 or when told (0x1a, 0x80), and pass every other key (5) to
 * edit_key; 0x1b and 0x60 restore the copy first, and 9 then gives the
 * focus to the next item that answers 0x80.  edit_key edits a line: keys
 * 0x106 and 0x102 move the cursor one place left and right, 0x107 and 0x105
 * to either end, 0x104 and 0x108 a line down and up in a box; 8 deletes
 * the character before the cursor, 0x10a the one under it, and 0x20..0x7e
 * are inserted.  In a box it then re-wraps the text (box_wrap) and, where
 * that fails, undoes the edit.  line_of and line_move find and move
 * between a box's lines; field_char, field_text, box_char and box_lines
 * draw.
 *
 * Section 5 (parent section 3): payload at EXE 0x0334e0, link segment 1c86.
 * A bare stored SEG:OFF in this file means section 5.  The module is stored
 * frame 1c86 (load-based 2c86), EXE 0x0334e0..0x03557e (8,350 bytes, 18
 * routines), from the section's first byte to its last retf at 1e8f:000d;
 * labels.c (module 1e8f) starts at 1e8f:000e.  One Turbo C large-model
 * _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d; functions are
 * defined in shipped order.  Definition comments give the load-based
 * address; extern comments the far pointer as stored in the shipped call,
 * or the DGROUP offset.
 *
 * Section 5's relocation table files 100 sites under 1c86, all in this
 * range (0x033590..0x03556b), the module's 100 far calls.  field_proc and
 * box_proc are the targets of vector stubs 18de:175b and 18de:1765, which
 * nothing far-calls: section 17 passes 18de:175b (EXE 0x06a02b) and
 * 18de:1765 (0x069dbc), and section 18 passes 18de:175b (0x06e387), to
 * ui_add (25e5:1293) as op-8 item handlers.  No far call in section 5
 * stores frame 1c86 (section 4's 26 are to its own roster.c), and no
 * section names 5 as its parent.  Every other routine is reached only by near calls from this
 * module.  Turbo C far-calls a public function of this module until it
 * is defined, so the fourteen near-called ahead of their definitions are
 * static; line_of and line_move are near-called only after theirs, and the
 * bytes do not decide static or public for them.
 *
 * FRUA's own code: the sharing map pairs none of the 18 routines with DQK
 * (all a-only), so every name here is chosen from the code.  tick_slot,
 * a byte other modules use as a counter, is box_wrap's loop index.
 *
 * _DATA is three stored literals, DGROUP 5e62..5e6b: "%c", "%s" and "%*s",
 * with "" on the NUL of "%c" (-d).  5e61 below it is a pad byte after
 * saveload.c's (section 4) last literal; labels.c's _DATA follows at 5e6c.
 * No _BSS.  field_released, focus_mouse_row and focus_mouse_col, which edit_begin and
 * edit_end use and section 15 uses the same way, lie past labels.c's _DATA
 * (5ec9 is a pad byte) and before menubox.c's at 5ed0: a module's _DATA is
 * one block, so they are not this module's, and they are extern here.
 *
 * This is the v1.0 build (CKIT.EXE "Version 1.0 March 19,1993"): EXE
 * 0x0334e0, 8,330 code bytes, byte-exact.  The addresses and claims above
 * are v1.2's.  v1.0 differs only in field_char and box_char, which lack
 * v1.2's "a NUL character draws as a space" (c == 0 -> ' ') check.
 */

typedef int (far *ItemProc)();       /* (item, msg, a, b), as dialog.c's */
typedef int (far *ItemCb)(int id, ...);

struct Wid {                    /* one dialog item, 0x20 bytes, as dialog.c */
    ItemProc fn;                /* +00 handler                            */
    ItemCb cb;                  /* +04 called on activate                 */
    int far *val;               /* +08 here the saved copy and the label  */
    char far *text;             /* +0c here the text, or the struct Box   */
    int row;                    /* +10                                    */
    int col;                    /* +12                                    */
    int cursor;                 /* +14 pointer frame while over it        */
    int len;                    /* +16 here the field's width, 1..0x26    */
    int width;                  /* +18                                    */
    int glyph;                  /* +1a bank<<10|index                     */
    char flags;                 /* +1c 1 drawn in 0x80, 2 not drawn (and
                                       neither is hit), 4 has the focus,
                                       0x80 drawn                         */
    char key1;                  /* +1d hotkeys                            */
    char key2;                  /* +1e                                    */
    unsigned char color;        /* +1f                                    */
};

struct FieldSave {              /* what a field's val points at           */
    char text[0x27];            /* +00 the line as it was given focus     */
    char label[0x25];           /* +27 drawn left of the field            */
    unsigned char pos;          /* +4c the cursor                         */
};

struct Line {                   /* one line of a box: text[start..end)    */
    unsigned char start;
    unsigned char end;          /* 0 an unused line                       */
};

struct Box {                    /* what a box's text points at, 0xfa      */
    char text[0xeb];            /* +00                                    */
    unsigned char pos;          /* +eb the cursor                         */
    struct Line lines[6];       /* +ec                                    */
    unsigned char from;         /* +f8 draw from this offset ...          */
    unsigned char count;        /* +f9 ... over this many lines           */
};

struct BoxSave {                /* what a box's val points at             */
    struct Box box;             /* +00 the box as it was given focus      */
    char label[1];              /* +fa drawn above the box                */
};

extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern char far mouse_clicked(int far *x, int far *y);            /* 0f5e:0fa2 */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern int  far ui_item_proc(struct Wid far *w, int msg, int a, int b); /* 15e5:01c3 */
extern int  far dl_item_count(void);                               /* 15e5:151e */
extern void far ui_send();                                       /* 15e5:18f0 */

extern char field_released;                                           /* 5eca */
extern int focus_mouse_row;                                            /* 5ecb */
extern int focus_mouse_col;                                            /* 5ecd */
extern char tick_slot;                                           /* b3f2 */
extern int scratch_word;                                            /* b41f */
extern struct Wid far *dl_items;                                 /* e6a3 */

static void edit_begin(struct Wid far *p, char line);
static void edit_end(struct Wid far *p, char line);
static unsigned char field_pos(int row, int col, char far *text);
static unsigned char box_pos(int row, int col, char far *text);
static void box_reset(char far *text);
static void field_char(int row, int col, unsigned char at, int len, int attr, char c);
static void field_text(int row, int col, unsigned char from, int attr, char far *text);
static void box_char(int row, int col, unsigned char pos, int attr, char far *text, char c);
static void box_lines(int row, int col, unsigned char from, int attr, char far *text);
static void box_clear(char far *text);
static char box_wrap(char far *text, unsigned char from, char grew);
static char is_close(char c);
static char is_open(char c);
static int edit_key(int key, unsigned char far *pos, char far *buf, int max, char multi);

/* A one-line text field. */
int far field_proc(struct Wid far *w, int msg, int a, int b)     /* 2c86:0000 */
{
    int x;
    int y;
    int r1;
    int c1;
    int r2;
    int c2;
    int attr;
    int old;
    struct Wid far *p;
    struct FieldSave far *e;
    register int rc;

    if (w == 0)
        return 0;
    p = w;
    e = (struct FieldSave far *)p->val;
    rc = 0;
    switch (msg) {
    case 0:
        if (p->text) {
            e->pos = 0;
            if (e)
                mem_move(p->text, e, 0x27);
        }
        p->len = int_max(1, int_min(p->len, 0x26));
        p->col &= 0xfffe;
        if (e && e->label)
            p->col = int_max((0x50 - p->len) * 4, str_len(e->label) * 4 + p->col + 4);
        if (p->color == 0)
            p->color = 0x87;
        break;
    case 1:
        if (p->flags & 0x80)
            break;
        p->flags |= 0x80;
        if (p->flags & 2)
            break;
        if (p->flags & 4)
            scratch_word++;
        else if (p->flags & 1)
            scratch_word = 2;
        if (p->glyph)
            draw_glyph(p->row, p->col, p->glyph >> 10, (p->glyph & 0x3ff) + scratch_word);
        if (p->text == 0)
            break;
        if (e && e->label)
            text_print_at(p->row, p->col - str_len(e->label) * 4 - 4,
                          (p->flags & 1) ? 0x80 : p->color, e->label);
        if (p->flags & 1)
            attr = 0x80;
        else if (p->flags & 4)
            attr = 0xf0;
        else
            attr = 0x70;
        text_xy(p->row, p->col, &r1, &c1);
        text_xy(p->row + 4, p->col + p->len * 4, &r2, &c2);
        fill_rect(r1 - 2, c1 - 1, r2 + 1, c2 + 4, 0);
        fill_rect(r1 - 1, c1, r2, c2 + 3, (attr & 0xf0) >> 4);
        field_text(p->row, p->col, 0, attr, p->text);
        if (p->flags & 4)
            field_char(p->row, p->col, e->pos, p->len, (attr & 0xf0) >> 4 | (attr & 0xf) << 4,
                       p->text[e->pos]);
        break;
    case 2:
        if (p->flags & 3)
            rc = 0;
        else {
            text_xy_to(p->row, p->col, a, b, &x, &y);
            rc = x >= 0 && x < 4 && y >= 0 && y < p->len * 4 + 2;
        }
        break;
    case 3:
        while (!mouse_clicked(&x, &y))
            ;
        if (e == 0)
            break;
        if (!field_proc(w, 2, x, y))
            break;
        e->pos = field_pos(p->row, p->col, p->text);
        edit_begin(p, 1);
        rc = 1;
        break;
    case 0x12:
        if (p->flags & 4)
            break;
        edit_begin(p, 1);
        break;
    case 0x80:
        rc = 1;
    case 0x1a:
        if (!(p->flags & 4))
            break;
        edit_end(p, 1);
        break;
    case 5:
        if (!(p->flags & 4))
            break;
        if (p->text == 0)
            break;
        if (a == 0x60 || a == 0x1b) {
            if (e) {
                mem_move(e, p->text, 0x27);
                if (str_len(p->text) < e->pos)
                    e->pos = str_len(p->text);
            }
            edit_end(p, 1);
            rc = 1;
        } else if (a == 0xd || a == 0xa) {
            edit_end(p, 1);
            rc = 1;
        } else if (a == 9) {
            edit_end(p, 1);
            x = p - dl_items;
            do {
                if (dl_item_count() - 1 <= x)
                    x = 0;
                else
                    x++;
            } while (!(*dl_items[x].fn)(&dl_items[x], 0x80));
            (*dl_items[x].fn)(&dl_items[x], 0x12);
            rc = 1;
        } else {
            old = e->pos;
            rc = edit_key(a, &e->pos, p->text, p->len - 1, 0);
            if (e->pos != old || rc > 0) {
                if (rc > 0) {
                    field_text(p->row, p->col, int_min(old, e->pos), 0xf0, p->text);
                    if (e->pos <= old) {
                        old = int_max(old, str_len(p->text));
                        if (p->len >= old)
                            field_char(p->row, p->col, old, p->len, 0xf0, ' ');
                    }
                } else
                    field_char(p->row, p->col, old, p->len, 0xf0, p->text[old]);
                field_char(p->row, p->col, e->pos, p->len, 0xf, p->text[e->pos]);
                p->flags |= 0x80;
            }
            rc = rc != 0;
        }
        break;
    case 0x2a:
        a = int_max(1, int_min(a, 0x26));
        if (p->len != a)
            p->flags &= 0x7f;
        p->len = a;
        break;
    case 0x24:
        if (p->width == a)
            break;
        p->flags &= 0x7f;
        p->width = a;
        break;
    case 0x28:
        rc = ui_item_proc(p, msg, a, b);
        p->col &= 0xfffe;
        break;
    case 4:
    case 0x13:
    case 0x1b:
        break;
    default:
        rc = ui_item_proc(p, msg, a, b);
    }
    return rc;
}

/* A box of six lines. */
int far box_proc(struct Wid far *w, int msg, int a, int b)       /* 2c86:07a0 */
{
    int x;
    int y;
    int r1;
    int c1;
    int r2;
    int c2;
    int attr;
    int old;
    struct Wid far *p;
    struct Box far *t;
    struct BoxSave far *bak;
    register int rc;

    if (w == 0)
        return 0;
    p = w;
    t = (struct Box far *)p->text;
    bak = (struct BoxSave far *)p->val;
    rc = 0;
    switch (msg) {
    case 0:
        if (p->text) {
            box_clear((char far *)t);
            box_wrap(p->text, 0xff, 0);
            if (bak)
                mem_move(p->text, bak, 0xfa);
        }
        p->col &= 0xfffe;
        if (p->color == 0)
            p->color = 0x87;
        break;
    case 1:
        if (p->flags & 0x80)
            break;
        p->flags |= 0x80;
        if (p->flags & 2)
            break;
        if (p->flags & 4)
            scratch_word++;
        else if (p->flags & 1)
            scratch_word = 2;
        if (p->glyph)
            draw_glyph(p->row, p->col, p->glyph >> 10, (p->glyph & 0x3ff) + scratch_word);
        if (p->text == 0)
            break;
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        if (bak && bak->label)
            text_print_at(p->row - 5, p->col, (p->flags & 1) ? 0x80 : p->color, bak->label);
        if (p->flags & 1)
            attr = 0x80;
        else if (p->flags & 4)
            attr = 0xf0;
        else
            attr = 0x70;
        text_xy(p->row, p->col, &r1, &c1);
        text_xy(p->row + 0x18, p->col + 0x98, &r2, &c2);
        fill_rect(r1 - 2, c1 - 1, r2 + 1, c2, 0);
        fill_rect(r1 - 1, c1, r2, c2, (attr & 0xf0) >> 4);
        box_reset(p->text);
        box_lines(p->row, p->col, 0, attr, p->text);
        if (p->flags & 4)
            box_char(p->row, p->col, t->pos, (attr & 0xf0) >> 4 | (attr & 0xf) << 4, p->text, 0);
        screen_flush_mode_set(0);
        screen_flush();
        break;
    case 2:
        if (p->flags & 3)
            rc = 0;
        else {
            text_xy_to(p->row, p->col, a, b, &x, &y);
            rc = x >= -1 && x <= 0x18 && y >= 0 && y < 0x98;
        }
        break;
    case 3:
        while (!mouse_clicked(&x, &y))
            ;
        if (t == 0)
            break;
        if (!box_proc(w, 2, x, y))
            break;
        t->pos = box_pos(p->row, p->col, p->text);
        edit_begin(p, 0);
        rc = 1;
        break;
    case 0x12:
        if (p->flags & 4)
            break;
        edit_begin(p, 0);
        break;
    case 0x80:
        rc = 1;
    case 0x1a:
        if (!(p->flags & 4))
            break;
        edit_end(p, 0);
        break;
    case 5:
        if (!(p->flags & 4))
            break;
        if (p->text == 0)
            break;
        if (a == 0x60 || a == 0x1b) {
            if (bak)
                mem_move(bak, p->text, 0xfa);
            edit_end(p, 0);
            rc = 1;
        } else if (a == 0xd || a == 0xa) {
            edit_end(p, 0);
            rc = 1;
        } else if (a == 9) {
            edit_end(p, 0);
            x = p - dl_items;
            do {
                if (dl_item_count() - 1 <= x)
                    x = 0;
                else
                    x++;
            } while (!(*dl_items[x].fn)(&dl_items[x], 0x80));
            (*dl_items[x].fn)(&dl_items[x], 0x12);
            rc = 1;
        } else {
            old = t->pos;
            rc = edit_key(a, &t->pos, p->text, 0xe9, 1);
            if (t->pos != old || rc > 0) {
                if (rc > 0)
                    box_lines(p->row, p->col, int_min(old, t->pos), 0xf0, p->text);
                else
                    box_char(p->row, p->col, old, 0xf0, p->text, 0);
                box_char(p->row, p->col, t->pos, 0xf, p->text, 0);
                p->flags |= 0x80;
            }
            rc = rc != 0;
        }
        break;
    case 0x24:
        if (p->width == a)
            break;
        p->flags &= 0x7f;
        p->width = a;
        break;
    case 0x28:
        rc = ui_item_proc(p, msg, a, b);
        p->col &= 0xfffe;
        break;
    case 4:
    case 0x13:
    case 0x1b:
    case 0x2a:
        break;
    default:
        rc = ui_item_proc(p, msg, a, b);
    }
    return rc;
}

/* Takes the focus: every item is told 0x80 (give up yours), the text is
 * copied to val, the pointer's place is kept in focus_mouse_row/5ecd, and
 * field_released (which edit_end sets to 1) is cleared; two text_xy results are
 * never read, as shipped. */
static void edit_begin(struct Wid far *p, char line)              /* 2c86:0e05 */
{
    int r1;
    int c1;
    int r2;
    int c2;
    int far *save;

    save = p->val;
    ui_send(0, dl_item_count() - 1, 0x80);
    if (line) {
        if (save)
            mem_move(p->text, save, 0x27);
    } else if (save)
        mem_move(p->text, save, 0xfa);
    p->flags |= 4;
    p->flags &= 0x7f;
    text_xy(p->row, p->col, &r1, &c1);
    if (line)
        text_xy(p->row + 4, p->col + p->len * 4, &r2, &c2);
    else
        text_xy(p->row + 0x18, p->col + 0x98, &r2, &c2);
    mouse_pos(&focus_mouse_row, &focus_mouse_col);
    field_released = 0;
}

/* Gives up the focus. */
static void edit_end(struct Wid far *p, char line)                /* 2c86:0f1b */
{
    if (!line)
        box_reset(p->text);
    p->flags &= 0x7b;
    field_released = 1;
}

/* The character of a field under the pointer. */
static unsigned char field_pos(int row, int col, char far *text) /* 2c86:0f44 */
{
    int x;
    int y;

    mouse_pos(&x, &y);
    text_xy_to(row, col, x, y, &row, &col);
    return int_min(str_len(text), col / 4);
}

/* The character of a box under the pointer. */
static unsigned char box_pos(int row, int col, char far *text)   /* 2c86:0f9d */
{
    int x;
    int y;
    int len;
    struct Box far *b;

    mouse_pos(&x, &y);
    text_xy_to(row, col, x, y, &row, &col);
    row = row / 4;
    col = col / 4;
    b = (struct Box far *)text;
    len = str_len(text);
    if (b->lines[row].end) {
        if (b->lines[row].end == len)
            return int_min(b->lines[row].start + col,
                                int_min(b->lines[row].end, b->lines[row].start + 0x25));
        return int_min(b->lines[row].start + col, b->lines[row].end - 1);
    }
    return len;
}

/* Marks the whole box to be drawn. */
static void box_reset(char far *text)                            /* 2c86:10af */
{
    struct Box far *b;

    if ((b = (struct Box far *)text) != 0) {
        b->from = 0;
        b->count = 6;
    }
}

/* Sets *line to the first line that does not end before offset pos.  1
 * when pos is that line's end and the line is full (0x26 columns) or the
 * next line, in use, starts there; then, with next, *line is the line after it. */
static char line_of(struct Box far *b, unsigned char pos, int far *line, char next) /* 2c86:10da */
{
    for (*line = 0; *line < 6 && b->lines[*line].end && b->lines[*line].end < pos; (*line)++)
        ;
    if (b->lines[*line].end == pos && *line < 5 &&
        (b->lines[*line + 1].start == pos && b->lines[*line + 1].end ||
         pos - b->lines[*line].start >= 0x26)) {
        if (next)
            (*line)++;
        return 1;
    }
    return 0;
}

/* Moves the cursor one line up or down, keeping its column. */
static char line_move(char far *text, unsigned char far *pos, char up) /* 2c86:119e */
{
    int n;
    struct Box far *b;

    b = (struct Box far *)text;
    line_of(b, *pos, &n, 1);
    if (up && n || !up && n < 5 &&
        (b->lines[n + 1].end || b->lines[n].end - b->lines[n].start >= 0x26)) {
        *pos = int_max(0, int_min(*pos - b->lines[n].start, 0x25));
        n += up ? -1 : 1;
        if (b->lines[n].end) {
            *pos += b->lines[n].start;
            *pos = int_min(*pos, b->lines[n].end - (text[b->lines[n].end] ? 1 : 0));
        } else
            *pos = b->lines[n].start;
        return 1;
    }
    return 0;
}

/* One character of a field, or the cursor bar past its end. */
static void field_char(register int row, int col, unsigned char at, int len, int attr, char c) /* 2c86:1306 */
{
    if (at < len)
        text_print_at(row, col + at * 4, attr, "%c", c);
    else {
        col += at * 4;
        text_xy(row, col, &attr, &col);
        fill_rect(row, col + 1, row + 4, col + 2, 0x10f);
    }
}

/* A field's text from offset from on. */
static void field_text(int row, int col, unsigned char from, int attr, char far *text) /* 2c86:1389 */
{
    text_print_at(row, col + from * 4, attr, "%s", text + from);
}

/* One character of a box. */
static void box_char(int row, int col, unsigned char pos, int attr, char far *text, char c) /* 2c86:13bc */
{
    int n;
    struct Box far *b;

    b = (struct Box far *)text;
    line_of(b, pos, &n, 1);
    pos = int_max(0, int_min(pos - b->lines[n].start, 0x25));
    if (c == 0)
        c = *(text + b->lines[n].start + pos);
    text_print_at(row + n * 4, col + pos * 4, attr, "%c", c);
}

/* The box's lines from offset from on, each padded to 0x26 columns; a
 * line's text is passed to text_print_at as the format, as shipped. */
static void box_lines(int row, int col, unsigned char from, int attr, char far *text) /* 2c86:1477 */
{
    int n;
    char c;
    struct Box far *b;

    b = (struct Box far *)text;
    from = int_min(from, b->from);
    line_of(b, from, &n, 1);
    from = int_max(0, int_min(from - b->lines[n].start, 0x25));
    for (; n < int_min(b->count, 6); n++) {
        if (b->lines[n].end) {
            c = text[b->lines[n].end];
            text[b->lines[n].end] = 0;
            text_print_at(row + n * 4, col + from * 4, attr, text + b->lines[n].start + from);
        }
        from = int_max(b->lines[n].end, b->lines[n].start) - b->lines[n].start;
        if (from < 0x26)
            text_print_at(row + n * 4, col + from * 4, attr, "%*s", 0x26 - from, "");
        if (b->lines[n].end)
            text[b->lines[n].end] = c;
        from = 0;
    }
    b->from = 0;
    b->count = 6;
}

/* Zeroes the struct Box from its text's end to +f9: the cursor, the lines
 * and the draw range with it. */
static void box_clear(char far *text)                            /* 2c86:1659 */
{
    register int n;

    n = str_len(text);
    mem_fill(text + n, 0xfa - n, 0);
}

/* Wraps the box's text into its lines again after a change at offset
 * from (grew: the cursor moved on); 0 when it no longer fits in six lines,
 * with the lines put back.  all is always 0: from is unsigned, so the
 * 0xff box_proc passes is not < 0 and its branch is dead, as shipped. */
static char box_wrap(char far *text, unsigned char from, char grew) /* 2c86:168a */
{
    int w;
    int n;
    int len;
    char far *s;
    char changed;
    char all;
    struct Box far *t;
    char saved[14];
    register int col;
    register int last;

    t = (struct Box far *)text;
    mem_move(t->lines, saved, 0xe);
    s = text;
    len = str_len(text);
    if ((all = from < 0) != 0) {
        last = 0;
        n = last;
        from = last;
        mem_fill(t->lines, 0xe, 0);
    } else {
        changed = line_of(t, from, &n, 0);
        for (tick_slot = n + 1; tick_slot < 6; tick_slot++) {
            if (tick_slot > n && t->lines[tick_slot].start == (grew ? -1 : 1) + len)
                t->lines[tick_slot].start = len;
            if (t->lines[tick_slot].end == (grew ? -1 : 1) + len)
                t->lines[tick_slot].end = len;
        }
        if (changed)
            n++;
        last = n;
        if (!grew && t->lines[last].end <= from)
            last++;
        if (n)
            n--;
    }
    t->from = from;
    s = text + t->lines[n].start;
    col = 0;
    w = s - text;
    changed = 1;
    while (*s && n < 6 && (changed || n <= last)) {
        while (*s && col < 0x26) {
            while (*s && is_open(*s) && col < 0x26) {
                s++;
                col++;
            }
            while (*s && *s != ' ' && !is_close(*s) && !is_open(*s) && col < 0x26) {
                s++;
                col++;
            }
            while (*s && is_close(*s) && col < 0x26) {
                s++;
                col++;
            }
            while (*s && *s == ' ' && col <= 0x26) {
                w = t->lines[n].start + col;
                col++;
                s++;
            }
            if (col < 0x26 || *s == 0)
                w = int_min(col, 0x26) + t->lines[n].start;
        }
        if (t->lines[n].start == w)
            w += col;
        if ((changed = t->lines[n].end != w) != 0) {
            if (t->from >= w || t->lines[n].end < t->from)
                t->from = int_min(t->from, int_max(0, int_min(w - 1, t->lines[n].end)));
            t->lines[n].end = w;
        }
        s = text + t->lines[n].end;
        if (++n < 6) {
            while (*s && *s == ' ' && col >= 0x26)
                s++;
            w = s - text;
            col = 0;
            if (t->lines[n].start != w) {
                t->lines[n].start = w;
                changed = 1;
            }
        } else if (col >= 0x26) {
            w = s - text;
            while (*s && *s == ' ')
                s++;
            if (*s == 0)
                text[w] = 0;
        }
    }
    t->count = n;
    if (*s == 0) {
        w = s - text;
        for (; n < 6; n++) {
            if (t->lines[n].end) {
                t->lines[n].end = 0;
                t->count = n + 1;
            }
            if (t->lines[n].start != w ||
                !grew && n && from - t->lines[n - 1].start == 0x25) {
                t->lines[n].start = w;
                t->count = n + 1;
            }
        }
    } else if (n == 6) {
        if (all)
            *s = 0;
        else {
            mem_move(saved, t->lines, 0xe);
            return 0;
        }
    }
    return 1;
}

/* Punctuation a line may end on. */
static char is_close(char c)                                     /* 2c86:1ba0 */
{
    return c == '.' || c == ',' || c == '?' || c == '!' || c == ':' || c == '-' ||
           c == ';' || c == ')' || c == ']' || c == '}' || c == '"';
}

/* Punctuation a word may start with. */
static char is_open(char c)                                      /* 2c86:1bee */
{
    return c == '(' || c == '[' || c == '{' || c == '"';
}

/* One key for a line whose last index is max (max + 1 characters) at
 * buf, cursor *pos; multi for a box.  1 the text changed, 0 a key it does not take; -1 otherwise
 * (a move, an insert refused with a beep, or a change a box could not take
 * and undid). */
static int edit_key(register int key, unsigned char far *pos, char far *buf, register int max, char multi) /* 2c86:1c12 */
{
    int rc;
    int i;
    int start;
    int lim;
    int len;
    char c;
    struct Box far *b;

    if (buf == 0 || pos == 0 || max == 0)
        return 0;
    b = (struct Box far *)buf;
    start = *pos;
    len = str_len(buf);
    if (multi && b->lines[5].end)
        lim = int_min(max, b->lines[5].start + 0x25);
    else
        lim = max;
    rc = -1;
    switch (key) {
    case 0x106:
    case 0x107:
        if (*pos == 0)
            break;
        *pos -= key == 0x106 ? 1 : *pos;
        if (!multi || key != 0x106)
            break;
        line_of(b, *pos, &i, 1);
        if (b->lines[i].start <= *pos)
            break;
        if (i == 0)
            break;
        i--;
        if (b->lines[i].end > *pos)
            break;
        *pos = int_min(b->lines[i].end, b->lines[i].start + 0x25);
        break;
    case 0x102:
    case 0x105:
        if (int_min(len, lim + 1) <= *pos)
            break;
        *pos += key == 0x102 ? 1 : int_min(len, lim + 1) - *pos;
        if (!multi || key != 0x102)
            break;
        line_of(b, *pos, &i, 1);
        if (b->lines[i].start <= *pos)
            break;
        *pos = b->lines[i].start;
        break;
    case 0x104:
    case 0x108:
        if (!multi)
            break;
        line_move(buf, pos, key == 0x108);
        break;
    case 8:
        if (*pos <= 0)
            break;
        (*pos)--;
    case 0x10a:
        if (*pos >= len)
            break;
        c = buf[*pos];
        for (i = *pos; int_min(len, max) > i; i++)
            buf[i] = buf[i + 1];
        if (len > 0) {
            len--;
            buf[len] = 0;
        }
        rc = 1;
        break;
    default:
        if (key >= 0x20 && key <= 0x7e) {
            if (!multi && len > max && buf[max] != ' ')
                beep();
            else {
                for (i = int_min(len, max); *pos < i; i--)
                    buf[i] = buf[i - 1];
                if (len <= max) {
                    len++;
                    buf[len] = 0;
                }
                buf[*pos] = key;
                if (*pos <= lim)
                    (*pos)++;
                rc = 1;
            }
        } else
            rc = 0;
    }
    if (multi && rc > 0) {
        if (!box_wrap(buf, *pos, *pos > start)) {
            beep();
            lim = key;
            if (key == 8 || key == 0x10a)
                key = c;
            else
                key = 8;
            edit_key(key, pos, buf, max, 1);
            *pos = start;
            return -1;
        }
        line_of(b, *pos, &i, 1);
        if (b->lines[i].start <= *pos && b->lines[i].end <= *pos && i < 5 &&
            b->lines[i + 1].start > *pos)
            *pos = int_min(b->lines[i].end, b->lines[i].start + 0x25);
    }
    return rc;
}
