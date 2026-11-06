/* v1.0 build (CKIT.EXE "Version 1.0 March 19,1993", EXE 0x062c00): the
 * same module but a disabled field (flags & 1) draws its label and frame
 * in attribute 0x80 where v1.2 uses 0x180.  The addresses below are
 * v1.2's.
 *
 * CKIT.EXE overlay section 15, module 28aa: number fields, dialog items of
 * module 25e5's kind.  num_proc handles a field holding a long: the item's
 * text is its label, drawn left of the field, and its val a struct NumSave
 * (the largest and least value allowed, the value, a copy of it, where the
 * value is copied out, and a '-' typed while the value is 0).  Like
 * textedit.c's field_proc (section 5) it draws a frame and the number
 * (message 1), takes the focus on a press (3) or when told (0x12), gives
 * it up (when the value is in range) on keys 0xd, 0xa, 0x1b, 0x60 and 9
 * or when told (0x1a, 0x80), and
 * passes every other key (5) to num_key; 0x1b and 0x60 swap the value with
 * the copy first, and 9 then gives the focus to the next item that answers
 * 0x80.  num_key edits the value: 0x108 and 0x104 add and take 1, 0x107 and
 * 0x105 set the least and largest value, 0x106 multiplies by 10, 0x102, 8
 * and 0x10a divide by 10; a digit (or the shifted digit on its key) is
 * appended and '-' changes the sign, either refused with "Valid numbers:
 * %ld - %ld" when the result is out of range.  num_end checks the value
 * when the focus goes, num_digits counts a value's characters and num_draw
 * draws it.
 *
 * Section 15 (parent section 13; the parent of sections 16, 17 and 18):
 * payload at EXE 0x062c80, link segment 28aa (section 14's too, whose
 * charview.c has the same frame).  A bare stored SEG:OFF in this file
 * means section 15.  The module is stored frame 28aa (load-based 38aa),
 * EXE 0x062c80..0x063b77 (3,831 bytes, 6 routines), from the section's
 * first byte to its last retf at 2999:0006; module 2999 starts at
 * 2999:0007.  One Turbo C large-model _TEXT segment, built with Turbo C
 * 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the
 * far pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 15's relocation table files 48 sites under 28aa, all in this
 * range, the module's 48 far calls; the other 26 are filed under 2999.  No
 * far call in sections 15 to 18 stores frame 28aa, and no vector targets
 * it: num_proc is reached only as a handler, which section 17 passes to
 * ui_add (25e5:1293) as an op-8 item, pushing 28aa:0000 at EXE 0x069ef7.
 * The other five routines are near-called ahead of their definitions, so
 * they are static.
 *
 * FRUA's own code: the sharing map pairs none of the 6 routines with DQK
 * (all a-only), so every name here is chosen from the code.  Forced by the
 * bytes: num_digits's n = ... ? 2 : 1 (an if/else threads the jump to the
 * loop test), case 0x2a's if-block (an early break changes which of the
 * two p->flags &= 0x7f tails -O keeps), and num_key's rc a plain int (a
 * register rc takes si and leaves key di).
 *
 * _DATA is six stored literals, DGROUP 8d5a..8d8f ("%1ld", "Valid numbers:
 * %ld - %ld" and ")!@#$%^&*(" are each written more than once and stored
 * once, -d), right after charview.c's (section 14) last literal at 8d59;
 * section 16 reads 8d90..8da1 as two byte tables ([bx+0x8d90],
 * [bx+0x8d99]), and its literals start at 8da2.  No _BSS.  field_released, focus_mouse_row and focus_mouse_col,
 * which num_begin and num_end use as textedit.c's edit_begin and edit_end
 * do, lie between labels.c's and menubox.c's data (section 5), so they
 * are extern here.
 */

typedef int (far *ItemProc)();       /* (item, msg, a, b), as dialog.c's */
typedef int (far *ItemCb)(int id, ...);

struct Wid {                    /* one dialog item, 0x20 bytes, as dialog.c */
    ItemProc fn;                /* +00 handler                            */
    ItemCb cb;                  /* +04 called on activate                 */
    int far *val;               /* +08 here a struct NumSave              */
    char far *text;             /* +0c here the label                     */
    int row;                    /* +10                                    */
    int col;                    /* +12                                    */
    int cursor;                 /* +14 pointer frame while over it        */
    int len;                    /* +16 here the field's width in digits   */
    int width;                  /* +18                                    */
    int glyph;                  /* +1a bank<<10|index                     */
    char flags;                 /* +1c 1 drawn in 0x80, 2 not drawn (and
                                       neither is hit), 4 has the focus,
                                       8 the next key starts a new value,
                                       0x20 cancelled (Esc or 0x60),
                                       0x40 changed, 0x80 drawn           */
    char key1;                  /* +1d hotkeys                            */
    char key2;                  /* +1e                                    */
    unsigned char color;        /* +1f                                    */
};

struct NumSave {                /* what a number field's val points at    */
    long upper;                 /* +00 the largest value allowed          */
    long lower;                 /* +04 the least                          */
    long value;                 /* +08                                    */
    long saved;                 /* +0c the value as it was given focus    */
    long far *out;              /* +10 the value is copied here           */
    char neg;                   /* +14 a '-' typed while the value is 0   */
};

extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern unsigned far mem_scan(void far *p, int n, char c);    /* 00d7:27b1 */
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
extern void far ui_send(int first, int last, int msg, ...);      /* 15e5:18f0 */
extern void far msg_box(char far *fmt, ...);  /* 18de:1841, vector to overlay 5 1e8f:1359 */

extern char field_released;                                           /* 5eca */
extern int focus_mouse_row;                                            /* 5ecb */
extern int focus_mouse_col;                                            /* 5ecd */
extern int scratch_word;                                            /* b41f */
extern struct Wid far *dl_items;                                 /* e6a3 */

static void num_begin(struct Wid far *p);
static void num_end(struct Wid far *p);
static int num_digits(long v);
static void num_draw(int row, int col, int attr, int len, long v, char neg);
static int num_key(int key, long max, long min, long far *v, char far *neg);

/* A number field. */
int far num_proc(struct Wid far *w, int msg, int a, int b)       /* 38aa:0000 */
{
    int x;
    int y;
    int r1;
    int c1;
    int r2;
    int c2;
    int attr;
    long t;
    struct Wid far *p;
    struct NumSave far *e;
    register int rc;

    if (w == 0)
        return 0;
    p = w;
    e = (struct NumSave far *)p->val;
    if (e == 0)
        return 0;
    rc = 0;
    switch (msg) {
    case 0:
        e->saved = e->value;
        p->flags &= 0x9f;
        if (e->upper < e->lower) {
            t = e->upper;
            e->upper = e->lower;
            e->lower = t;
        } else if (e->upper == e->lower) {
            e->upper = 0x7fffffffL;
            e->lower = 0x80000001L;
        }
        if (e->lower == 0x80000000L)
            e->lower++;
        if (e->value < e->lower)
            e->value = e->lower;
        if (e->value > e->upper)
            e->value = e->upper;
        if (e->out)
            *e->out = e->value;
        e->neg = 0;
        if (p->len <= 0)
            p->len = int_max(1, int_max(num_digits(e->upper), num_digits(e->lower)));
        p->col &= 0xfffe;
        if (p->text)
            p->col = int_max((0x50 - int_max(num_digits(e->upper), num_digits(e->lower))) * 4,
                                  str_len(p->text) * 4 + p->col + 4);
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
            scratch_word = 1;
        else if (p->flags & 1)
            scratch_word = 2;
        else
            scratch_word = 0;
        if (p->glyph)
            draw_glyph(p->row, p->col, p->glyph >> 10, (p->glyph & 0x3ff) + scratch_word);
        if (p->text)
            text_print_at(p->row, p->col - str_len(p->text) * 4 - 4,
                          (p->flags & 1) ? 0x80 : p->color, p->text);
        if (p->flags & 1)
            attr = 0x80;
        else if (p->flags & 4)
            attr = 0xf0;
        else
            attr = 0x70;
        text_xy(p->row, p->col, &r1, &c1);
        text_xy(p->row + 4, p->col + p->len * 4, &r2, &c2);
        fill_rect(r1 - 2, c1 - 1, r2 + 1, c2 + 1, 0);
        fill_rect(r1 - 1, c1, r2, c2, (attr & 0xf0) >> 4);
        if (p->flags & 8)
            attr = 0xf;
        num_draw(p->row, p->col, attr, p->len, e->value, e->neg);
        if ((p->flags & 0xc) != 4)
            break;
        text_print_at(p->row, p->col + (p->len - 1) * 4, 0xf, "%1ld",
                      e->value % 10 * (e->value < 0 ? -1 : 1));
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
        if (!num_proc(p, 2, x, y))
            break;
        num_begin(p);
        rc = 1;
        break;
    case 0x12:
        if (p->flags & 4)
            break;
        num_begin(p);
        break;
    case 0x80:
        rc = 1;
    case 0x1a:
        if (!(p->flags & 4))
            break;
        num_end(p);
        break;
    case 5:
        if (!(p->flags & 4))
            break;
        if (a == 0x60 || a == 0x1b) {
            t = e->value;
            e->value = e->saved;
            e->saved = t;
            p->flags |= 0x60;
            if (e->out)
                *e->out = e->value;
            num_end(p);
            rc = 1;
        } else if (a == 0xd || a == 0xa) {
            num_end(p);
            rc = 1;
        } else if (a == 9) {
            num_end(p);
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
            if (p->flags & 8) {
                e->value = 0;
                p->flags &= 0x77;
            }
            rc = num_key(a, e->upper, e->lower, &e->value, &e->neg);
            if (rc > 0) {
                num_draw(p->row, p->col, 0xf0, p->len, e->value, e->neg);
                text_print_at(p->row, p->col + (p->len - 1) * 4, 0xf, "%1ld",
                              e->value % 10 * (e->value < 0 ? -1 : 1));
                p->flags |= 0x80;
            }
            rc = rc != 0;
        }
        break;
    case 0x24:
        if (a == e->value)
            break;
        e->value = a;
        if (e->out)
            *e->out = e->value;
        p->flags &= 0x7f;
        break;
    case 0x2a:
        a = int_max(1, a);
        if (p->len != a) {
            p->len = a;
            p->flags &= 0x7f;
        }
        break;
    case 0x28:
        rc = ui_item_proc(p, msg, a, b);
        p->col &= 0xfffe;
        break;
    case 4:
    case 0x13:
    case 0x15:
    case 0x16:
    case 0x1b:
        break;
    default:
        rc = ui_item_proc(p, msg, a, b);
    }
    if (e->value != e->saved)
        p->flags |= 0x40;
    return rc;
}

/* Takes the focus: every item is told 0x80 (give up yours), the value is
 * copied, and the pointer's place is kept in focus_mouse_row/5ecd.  Taking it
 * afresh sets flag 8, so the first key starts a new value. */
static void num_begin(struct Wid far *p)                          /* 38aa:088a */
{
    char flags;
    struct NumSave far *e;

    e = (struct NumSave far *)p->val;
    flags = p->flags;
    ui_send(0, dl_item_count() - 1, 0x80);
    if (e)
        e->saved = e->value;
    if (!(flags & 4))
        p->flags |= 8;
    else
        p->flags &= 0xf7;
    p->flags |= 4;
    p->flags &= 0x1f;
    mouse_pos(&focus_mouse_row, &focus_mouse_col);
    field_released = 0;
}

/* Gives up the focus.  A value out of range is refused with a message and
 * the copy put back, and the field keeps flag 4. */
static void num_end(struct Wid far *p)                            /* 38aa:0918 */
{
    struct NumSave far *e;

    e = (struct NumSave far *)p->val;
    if (e) {
        if (e->value >= e->lower && e->value <= e->upper) {
            if (e->out)
                *e->out = e->value;
            p->flags &= 0x73;
        } else {
            msg_box("Valid numbers: %ld - %ld", e->lower, e->upper);
            e->value = e->saved;
            if (e->out)
                *e->out = e->value;
            p->flags &= 0x7f;
            return;
        }
    }
    field_released = 1;
}

/* The characters v takes: its digits and a '-'. */
static int num_digits(long v)                                     /* 38aa:09ef */
{
    register int n;

    n = v < 0 ? 2 : 1;
    while ((v = v / 10) != 0)
        n++;
    return n;
}

/* A value in a field len digits wide; one too wide loses its sign and is
 * cut to its last len digits, drawn with a width of 0 (len is counted down
 * to 0 first, as shipped).  A '-' typed while the value is 0 is drawn. */
static void num_draw(int row, int col, int attr, register int len, long v, char neg) /* 38aa:0a2d */
{
    int n;
    long m;

    n = num_digits(v);
    if (n > len) {
        if (v < 0)
            v = v * -1;
        m = 1;
        for (; len; len--)
            m = m * 10;
        text_print_at(row, col, attr, "%0*ld", len, v % m);
    } else
        text_print_at(row, col, attr, "%*ld", len, v);
    if (v == 0 && neg)
        text_print_at(row, col, attr, "-");
}

/* One key for the value *v, kept between min and max.  1 the value
 * changed, 0 a key it does not take, -1 otherwise (a change refused). */
static int num_key(register int key, long max, long min, long far *v, char far *neg) /* 38aa:0b09 */
{
    long t;
    int rc;

    if (v == 0)
        return 0;
    rc = -1;
    switch (key) {
    case 0x108:
        if (*v >= max)
            break;
        (*v)++;
        rc = 1;
        break;
    case 0x104:
        if (*v <= min)
            break;
        (*v)--;
        rc = 1;
        break;
    case 0x106:
        if (num_digits(*v) >= 11)
            break;
        if (*v * 10 > max)
            break;
        if (*v * 10 < min)
            break;
        *v = *v * 10;
        rc = 1;
        break;
    case 0x102:
        if (*v == 0)
            break;
        t = *v / 10;
        if (t < min)
            break;
        *v = t;
        rc = 1;
        break;
    case 0x107:
        if (*v == min)
            break;
        *v = min;
        rc = 1;
        break;
    case 0x105:
        if (*v == max)
            break;
        *v = max;
        rc = 1;
        break;
    case 8:
    case 0x10a:
        if (*v == 0)
            break;
        *v = *v / 10;
        rc = 1;
        break;
    default:
        if (mem_scan(")!@#$%^&*(", 10, key) < 10)
            key = mem_scan(")!@#$%^&*(", 10, key) + '0';
        if (key >= '0' && key <= '9') {
            if (num_digits(*v) < 11) {
                key -= '0';
                if ((max - key) / 10 >= *v) {
                    if (*v < 0 || *v == 0 && *neg) {
                        t = *v * 10 - key;
                        *neg = 0;
                    } else
                        t = *v * 10 + key;
                    if ((min > 0 || t >= min) && t <= max) {
                        *v = t;
                        rc = 1;
                    }
                }
            }
            if (rc != 1)
                msg_box("Valid numbers: %ld - %ld", min, max);
        } else if (key == '-') {
            if (*v == 0 && min < 0)
                *neg = !*neg;
            t = *v * -1;
            if (t >= min && t <= max) {
                *v = t;
                rc = 1;
            } else
                msg_box("Valid numbers: %ld - %ld", min, max);
        } else
            rc = 0;
    }
    return rc;
}
