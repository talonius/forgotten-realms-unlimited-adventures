/* CKIT.EXE resident module 25e5: dialogs ("DL" in its error texts).
 * A dialog is an array of 0x20-byte items in one far block; each item has
 * a handler that takes messages -- draw, hit test, press, activate, key,
 * set a field -- and falls back on ui_item_proc for the ones it does not
 * take.  ui_add builds items from a tag list, ui_poll runs one round of
 * mouse and keyboard input over them, and ui_mark and ui_send post a
 * message to one item or a range.
 *
 * EXE 0x01825a..0x019c2e, one Turbo C large-model _TEXT segment starting at
 * byte a of paragraph 25e5.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * It starts the byte after libedit.c's last retf (257c:0699 is 25e5:0009)
 * and ends at sfxslots.c's first byte (2782:000e).
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  Callee names are
 * placeholders chosen from use; a modNNNN_ name says only which segment the
 * callee lives in.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is.  radio_proc
 * is static: group_proc near-calls it ahead of its definition.  ui_send is
 * declared without a prototype: its nine callers (three here) pass three
 * words, and it reads five.  area_proc is declared without one too: it
 * reads a fifth word, which ui_poll passes with messages 3 and 4, and calls
 * itself with four.
 *
 * -O cross-jumping keeps the LAST of two identical tails and points the
 * first at it.  Where the shipped code kept the first, the source says so:
 * ui_item_proc reaches its shared tails by goto, and ui_add puts three case
 * labels inside the item-type branches whose code they share.
 *
 * _BSS follows the statics' names (Turbo C's symbol-table order), not their
 * declaration order; these names reproduce the shipped e69c..e6ad layout,
 * checked with goldbox.bss_order.  dl_items is public: resident code at
 * 0x011183 and 0x0114d1 and overlay code read it.
 */

typedef int (far *ItemProc)();       /* (item, msg, a, b), and n for 3, 4 */
typedef int (far *ItemCb)(int id, ...);

struct Wid {                    /* one dialog item, 0x20 bytes            */
    ItemProc fn;                /* +00 handler                            */
    ItemCb cb;                  /* +04 called on activate                 */
    int far *val;               /* +08 value a check or group writes      */
    char far *text;             /* +0c                                    */
    int row;                    /* +10                                    */
    int col;                    /* +12                                    */
    int cursor;                 /* +14 pointer frame while over it        */
    int len;                    /* +16 text argument; group's count       */
    int width;                  /* +18 4-column cells; group's selection  */
    int glyph;                  /* +1a bank<<10|index; < -1 framed, -1 text */
    char flags;                 /* +1c                                    */
    char key1;                  /* +1d hotkeys                            */
    char key2;                  /* +1e                                    */
    unsigned char color;        /* +1f                                    */
};

struct Pointer;                /* glyph.c's copied glyph                 */

extern int  far to_lower(int c);                             /* 00d7:25ab */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern void far pointer_shape_set(struct Pointer far *g);            /* 0f5e:1189 */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far text_offset_xy(int row, int col, int row2, int col2,
                             int far *out_row, int far *out_col); /* 107e:0565 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far glyph_bounds(int row, int col, int slot, int glyph, int far *top,
                             int far *left, int far *bottom, int far *right); /* 11d7:0bf0 */
extern void far pointer_glyph_load(int icon, int frame);               /* 11d7:0cf9 */
extern int  far inp_press(char op, int far *x, int far *y);   /* 178d:000b */
extern char far inp_clicked(int far *x, int far *y);            /* 178d:0026 */
extern void far inp_pos(int far *x, int far *y);            /* 178d:003e */
extern char far kbd_waiting(void);                              /* 178d:005c */
extern int  far kbd_read(void);                              /* 178d:0062 */
extern void far con_printf(char far *fmt, ...);                  /* 1860:0001 */

extern int prompt_icon;                                          /* 96ac */
extern int prompt_frame;                                         /* d590 */

int far button_proc(struct Wid far *w, int msg, int a, int b);
int far group_proc(struct Wid far *w, int msg, int a, int b);
static int radio_proc(struct Wid far *w, int msg, int a, int b);
int far check_proc(struct Wid far *w, int msg, int a, int b);
int far area_proc();
int far label_proc(struct Wid far *w, int msg, int a, int b);
int far key_proc(struct Wid far *w, int msg, int a, int b);
void far ui_send();
int far ui_poll(void);

static ItemProc dl_procs[7] = {                                  /* a5f6 */
    button_proc, group_proc, radio_proc, check_proc,
    area_proc, label_proc, key_proc
};

static int dl_used;                                              /* e69c */
static char dl_drawn;                                            /* e69e */
static long next_rep;                                            /* e69f */
struct Wid far *dl_items;                                        /* e6a3 */
static struct Wid far *item_pool;                                /* e6a7 */
static char dl_running;                                          /* e6ab */
static int max_items;                                            /* e6ac */

char far key_match(int hot, int key)                             /* 25e5:000a */
{
    if (hot == 0)
        return 0;
    if (hot < 0x20)
        return hot + 0x100 == key;
    switch (hot) {
    case '*':
        return 1;
    case '#':
        return key == '`' || key == 0x1b;
    case '@':
        return key == '\r' || key == '\n';
    }
    return to_lower(hot) == to_lower(key);
}

void far item_draw(struct Wid far *w, int base)                  /* 25e5:0091 */
{
    int x;
    int y;
    register int attr;
    register int k;

    w->flags |= 0x80;
    if (w->flags & 2)
        return;
    if (w->flags & 1)
        k = 2;
    else {
        k = (w->flags & 8) ? 3 : 0;
        if (w->flags & 4)
            k++;
    }
    if (w->glyph >= 0)
        draw_glyph(w->row, w->col, w->glyph ? w->glyph >> 10 : 0,
                   (w->glyph ? w->glyph & 0x3ff : base) + k);
    if (w->text == 0)
        return;
    if (w->flags & 1) {
        if (w->color) {
            if ((w->color & 0xf0) == 0x90)
                attr = 0x98;
            else
                attr = (w->color & 0xf0) | 7;
        } else
            attr = 0x17;
    } else {
        if (w->color)
            attr = w->color;
        else
            attr = 0xf0;
    }
    text_offset_xy(w->row, w->col, 0, 0x1f46, &x, &y);
    text_print_at(x, y, attr, w->text);
}

int far ui_item_proc(struct Wid far *w, int msg, int a, int b)    /* 25e5:01c3 */
{
    struct Wid far *p;
    int x;
    int y;
    char old;
    char far *s;
    char was;
    char up;
    char hit;

    p = w;
    switch (msg) {
    case 1:
        p->flags |= 0x80;
        break;
    case 3:
        was = 1;
        (*p->fn)(p, 0x13);
        (*p->fn)(p, 1);
        (*p->fn)(p, 7);
        do {
            up = inp_clicked(&x, &y);
            hit = (*p->fn)(p, 2, x, y) ? 1 : 0;
            if (was != hit) {
                was = hit;
                (*p->fn)(p, was ? 0x13 : 0x1b);
                (*p->fn)(p, 1);
            }
            if (was && !up)
                (*p->fn)(p, 7);
        } while (!up);
        (*p->fn)(p, 0x1b);
        return was;
    case 5:
        if (p->flags & 3)
            break;
        return key_match(p->key1, a) || key_match(p->key2, a);
    case 4:
        if (p->cb == 0)
            break;
        (*p->cb)(p - dl_items);
        break;
    case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15:
    case 0x16:
        old = p->flags;
        p->flags |= 1 << (msg - 0x10);
    changed:                            /* 25e5:03d4, reached from 0x18.. */
        if (p->flags == old)
            break;
    dirty:                              /* 25e5:03e3 */
        p->flags &= 0x7f;
        break;
    case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d:
    case 0x1e:
        old = p->flags;
        p->flags &= ~(1 << (msg - 0x18));
        goto changed;
    case 0x20:
        p->key1 = a;
        break;
    case 0x21:
        p->key2 = a;
        break;
    case 0x22:
        p->cb = *(ItemCb far *)&a;
        break;
    case 0x23:
        p->val = *(int far * far *)&a;
        break;
    case 0x24:
        p->width = a;
        break;
    case 0x25:
        if (p->glyph == a)
            break;
        p->glyph = a;
        goto dirty;
    case 0x26:
        if (p->color == a)
            break;
        p->color = a;
        goto dirty;
    case 0x27:
        s = *(char far * far *)&a;
        if (p->text == s)
            break;
        p->text = s;
        goto dirty;
    case 0x28:
        if (p->row == a && p->col == b)
            break;
        p->row = a;
        p->col = b;
        goto dirty;
    case 0x29:
        p->cursor = a;
        break;
    case 0x2a:
        p->len = a;
        break;
    case 0x2b:
        p->fn = dl_procs[a - 1];
        break;
    case 0x2c:
        p->fn = *(ItemProc far *)&a;
        break;
    }
    return 0;
}

int far button_proc(struct Wid far *w, int msg, int a, int b)    /* 25e5:0537 */
{
    int x1;
    int y1;
    int x2;
    int y2;
    register int k;

    switch (msg) {
    case 1:
        w->flags |= 0x80;
        if (w->flags & 2)
            break;
        k = (w->flags & 9) ? 1 : 0;
        if (w->glyph >= 0)
            draw_glyph(w->row, w->col, w->glyph ? w->glyph >> 10 : 0,
                       (w->glyph ? w->glyph & 0x3ff : 0xe) + k);
        else if (w->glyph < -1) {
            x2 = -w->glyph;
            y2 = x2 >> 10;
            x2 &= 0x3ff;
            if (k)
                k = 4;
            draw_glyph(w->row, w->col, y2, x2 + k);
            for (y1 = 0; y1 < w->width; y1++)
                draw_glyph(w->row, w->col + y1 * 4, y2, x2 + k + (y1 & 1) + 1);
            draw_glyph(w->row, w->col + (y1 - 1) * 4, y2, x2 + k + 3);
            if (k)
                k = 1;
        }
        if (w->text == 0)
            break;
        if ((w->flags & 0x40) || w->glyph < 0)
            y1 = w->col;
        else {
            glyph_bounds(w->row, w->col, w->glyph ? w->glyph >> 10 : 0,
                         w->glyph ? w->glyph & 0x3ff : 0xe, &x1, &y1, &x2, &y2);
            text_xy_to(0x1f40, 0x1f40, x1, y1, &x1, &y1);
            text_xy_to(x1 + 0x1f40, y1 + 0x1f40, x2, y2, &x2, &y2);
            text_offset_xy(0, y1 + 0x1f40, 0, (y2 >> 1) + 0x1f40 - str_len(w->text) * 2,
                         &x1, &y1);
        }
        text_xy(w->row, y1, &x1, &y1);
        if (w->flags & 1) {
            if (w->color) {
                if (w->len)
                    y2 = w->len;
                else if ((w->color & 0xf0) == 0x90)
                    y2 = 0x98;
                else
                    y2 = (w->color & 0xf0) | 7;
            } else
                y2 = 0x17;
        } else {
            if (w->color)
                y2 = w->color;
            else
                y2 = 0xf0;
        }
        if (k && screen_rows() >= 300)
            k = 2;
        text_print_at(x1 + k, y1, y2, w->text);
        if (!(w->flags & 0x20))
            break;
        if (w->flags & 1)
            break;
        text_print_at(x1 + k, y1, (w->color & 0xf0) | 4, "%c", *w->text);
        break;
    case 2:
        if (w->flags & 3)
            break;
        if (w->glyph >= 0)
            glyph_bounds(w->row, w->col, w->glyph ? w->glyph >> 10 : 0,
                         w->glyph ? w->glyph & 0x3ff : 0xe, &x1, &y1, &x2, &y2);
        else if (w->glyph == -1) {
            text_xy(w->row, w->col, &x1, &y1);
            text_offset_xy(x1, y1, 0x1f44, str_len(w->text) * 4 + 0x1f40, &x2, &y2);
        } else {
            text_xy(w->row - 1, w->col - 2, &x1, &y1);
            text_xy(w->row + 5, w->col + w->width * 4 + 2, &x2, &y2);
        }
        return x1 <= a && a < x2 && y1 <= b && b < y2;
    case 3:
        if (w->flags & 4)
            next_rep = timer_ticks();
        if ((x1 = ui_item_proc(w, msg, a, b)) == 0)
            next_rep = 0;
        return x1;
    case 7:
        if (next_rep == 0)
            break;
        if (timer_ticks() < next_rep)
            break;
        next_rep = timer_ticks() + 12;
        if (w->cb)
            (*w->cb)(w - dl_items);
        break;
    case 4:
        if (next_rep) {
            next_rep = 0;
            break;
        }
    default:
        return ui_item_proc(w, msg, a, b);
    }
    return 0;
}

int far group_proc(struct Wid far *w, int msg, register int a, int b) /* 25e5:0aa9 */
{
    register int i;

    switch (msg) {
    case 0:
        if (w->val)
            i = *w->val;
        else
            i = 1;
        group_proc(w, 4, i, 0);
        break;
    case 4:
        if (w->width != a) {
            if (w->width)
                radio_proc(w + w->width, 0x1a, 0, 0);
            w->width = a;
            if (w->val)
                *w->val = a;
        }
        if (a)
            radio_proc(w + a, 0x12, 0, 0);
        if (w->cb == 0)
            break;
        (*w->cb)(w - dl_items, a);
        break;
    case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15:
    case 0x16: case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c:
    case 0x1d: case 0x1e:
        for (i = 1; i <= w->len; i++)
            radio_proc(w + i, msg, 0, 0);
    default:
        return ui_item_proc(w, msg, a, b);
    }
    return 0;
}

static int radio_proc(struct Wid far *w, int msg, register int a, int b) /* 25e5:0c1d */
{
    struct Wid far *p;
    int x1;
    int y1;
    int x2;
    int y2;
    register int i;

    switch (msg) {
    case 1:
        item_draw(w, 0x10);
        break;
    case 2:
        if (w->flags & 3)
            break;
        if (w->glyph) {
            glyph_bounds(w->row, w->col, w->glyph >> 10, w->glyph & 0x3ff,
                         &x1, &y1, &x2, &y2);
            if (x1 <= a && a < x2 && y1 <= b && b < y2)
                return 1;
        }
        text_xy(w->row, w->col, &x1, &y1);
        if (a < x1)
            break;
        if (b < y1)
            break;
        i = w->text ? str_len(w->text) : 0;
        text_offset_xy(x1, y1, 0x1f44, (w->len + i) * 4 + 0x1f48, &x1, &y1);
        return a < x1 && b < y1;
    case 4:
        for (i = 0, p = w; p->fn != dl_procs[1]; i++, p--)
            ;
        group_proc(p, 4, i, 0);
        if (w->cb)
            (*w->cb)(w - dl_items);
        break;
    default:
        return ui_item_proc(w, msg, a, b);
    }
    return 0;
}

int far check_proc(struct Wid far *w, int msg, int a, int b)     /* 25e5:0de1 */
{
    switch (msg) {      /* 2e 3b 07 at 0ded: Turbo C's sparse-switch value search */
    case 0:
        if (w->val == 0)
            break;
        check_proc(w, *w->val ? 0x12 : 0x1a, 0, 0);
        break;
    case 1:
        item_draw(w, 0x15);
        break;
    case 2:
        return radio_proc(w, 2, a, b);
    case 4:
        w->flags ^= 4;
        w->flags &= 0x7f;
        if (w->val)
            *w->val = (w->flags & 4) ? 1 : 0;
        if (w->cb == 0)
            break;
        (*w->cb)(w - dl_items);
        break;
    case 0x12:
        w->flags |= 4;
        w->flags &= 0x7f;
        if (w->val)
            *w->val = 1;
        break;
    case 0x1a:
        w->flags &= 0x7b;
        if (w->val)
            *w->val = 0;
        break;
    default:
        return ui_item_proc(w, msg, a, b);
    }
    return 0;
}

int far area_proc(w, msg, a, b, btn)                             /* 25e5:0f33 */
struct Wid far *w;
int msg;
register int a;
int b;
int btn;
{
    int x;
    int y;
    int r;
    char was;
    char hit;
    register int cur;

    switch (msg) {
    case 2:
        if (w->flags & 3)
            break;
        text_xy_to(w->row, w->col, a, b, &x, &y);
        if (x < 0 || x >= w->len || y < 0 || y >= w->width)
            break;
        if ((w->flags & 0x40) && w->cb && !(*w->cb)(-2, a, b))
            break;
        return 1;
    case 3:
        if (!(w->flags & 0x40))
            return 1;
        was = 0;
        x = a;
        y = b;
        cur = -1;
        do {
            if (w->cb) {
                r = (*w->cb)(-2, x, y);
                if (cur < 0)
                    cur = r;
                hit = r == cur ? 1 : 0;
            } else
                hit = area_proc(w, 2, x, y) ? 1 : 0;
            if (was != hit) {
                if (w->cb)
                    (*w->cb)(-3, cur, was);
                was = hit;
            }
        } while (!inp_clicked(&x, &y));
        if (was && w->cb)
            (*w->cb)(-4, cur);
        return was;
    case 4:
        if (w->cb)
            (*w->cb)(w - dl_items, a, b, btn);
        break;
    default:
        return ui_item_proc(w, msg, a, b);
    }
    return 0;
}

int far label_proc(struct Wid far *w, int msg, int a, int b)     /* 25e5:1118 */
{
    switch (msg) {
    case 1:
        w->flags |= 0x80;
        if (!(w->flags & 2))
            text_print_at(w->row, w->col, w->color ? w->color : 0xf0, w->text, w->len);
        break;
    default:
        return ui_item_proc(w, msg, a, b);
    }
    return 0;
}

int far key_proc(struct Wid far *w, int msg, int a, int b)       /* 25e5:1182 */
{
    if (msg == 5 && w->cb && !(w->flags & 3))
        return (*w->cb)(w - dl_items, a);
    if (msg == 4)
        return 0;
    return ui_item_proc(w, msg, a, b);
}

void far dl_init(int max)                                        /* 25e5:11ea */
{
    max_items = max;
    if ((item_pool = mem_alloc(max * sizeof(struct Wid))) == 0) {
        con_printf("Insufficient memory in DLInit\n");
        dos_call(0x4c01);
    }
    dl_running = dl_drawn = 0;
    next_rep = 0;
}

/* Empty mirror of dl_init, called first on the exit path. */
void far dl_done(void)                                       /* 25e5:1235 */
{
}

ItemProc far ui_set_hook(int kind, ItemProc fn)                  /* 25e5:1236 */
{
    ItemProc old;

    old = dl_procs[kind - 1];
    if (fn)
        dl_procs[kind - 1] = fn;
    return old;
}

void far dl_begin(void)                                       /* 25e5:1276 */
{
    dl_items = item_pool;
    dl_used = 0;
    dl_running = 1;
    dl_drawn = 0;
}

void far ui_add(int op, ...)                                     /* 25e5:1293 */
{
    struct Wid far *w;
    int far *ap;

    ap = &op;
    w = dl_items + dl_used - 1;
    for (;;) {
        switch (op = *ap++) {
        case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
            if (dl_used >= max_items) {
                error_box("To many dialog items (%d/%d)", dl_used, max_items);
                return;
            }
            dl_used++;
            w++;
            mem_fill(w, sizeof(struct Wid), 0);
            if (op == 8) {
                w->fn = *((ItemProc far *)ap)++;
                break;
            }
            w->fn = dl_procs[op - 1];
            if (op == 2) {
        case 0x2a:
                w->len = *ap++;
                break;
            }
            if (op == 7) {
        case 0x22:
                w->cb = *((ItemCb far *)ap)++;
                break;
            }
            w->row = *ap++;
            w->col = *ap++;
            if (op == 5) {
                w->len = *ap++;
        case 0x24:
                w->width = *ap++;
            } else
                w->text = *((char far * far *)ap)++;
            break;
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15:
        case 0x16:
            w->flags |= 1 << (op - 0x10);
            break;
        case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d:
        case 0x1e:
            w->flags &= ~(1 << (op - 0x10));    /* -0x10 as shipped: clears nothing */
            break;
        case 0x20:
            w->key1 = *ap++;
            break;
        case 0x21:
            w->key2 = *ap++;
            break;
        case 0x23:
            w->val = *((int far * far *)ap)++;
            break;
        case 0x25:
            w->glyph = *ap++;
            break;
        case 0x26:
            w->color = *ap++;
            break;
        case 0x27:
            w->text = *((char far * far *)ap)++;
            break;
        case 0x28:
            w->row = *ap++;
            w->col = *ap++;
            break;
        case 0x29:
            w->cursor = *ap++;
            break;
        case 0:
            return;
        }
    }
}

int far dl_item_count(void)                                        /* 25e5:151e */
{
    return dl_used;
}

void far dl_draw(char all)                                   /* 25e5:1522 */
{
    struct Wid far *w;
    register int i;

    if (!dl_drawn) {
        ui_send(0, dl_used - 1, 0);
        dl_drawn = 1;
    }
    for (i = 0, w = dl_items; i < dl_used; i++, w++)
        if (all || !(w->flags & 0x80))
            (*w->fn)(w, 1);
}

int far ui_run(int (far *idle)(void))                       /* 25e5:1589 */
{
    register int rc;

    if (!dl_running)
        error_box("not in dialog in DLDialog");
    else if (!dl_drawn) {
        ui_send(0, dl_used - 1, 0);
        dl_drawn = 1;
    }
    while ((rc = ui_poll()) < 0 && (idle == 0 || (rc = (*idle)()) < 0))
        ;
    return rc;
}

int far ui_poll(void)                                            /* 25e5:15e6 */
{
    struct Wid far *w;
    int x;
    int y;
    register int i;
    register int n;

    draw_page_set(1);
    dl_draw(0);
    n = inp_press(7, &x, &y);
    for (i = 0, w = dl_items; i < dl_used && !(*w->fn)(w, 2, x, y); i++, w++)
        ;
    if (n) {
        if (i < dl_used) {
            if ((*w->fn)(w, 3, x, y, n)) {
                (*w->fn)(w, 4, x, y, n);
                if (w->flags & 0x10)
                    return i;
                w->flags &= 0x7f;
            }
        } else
            beep();
    }
    if (i < dl_used) {
        if (w->cursor)
            n = w->cursor;
        else
            n = prompt_icon < 0 ? 1 : prompt_frame;
    } else
        n = prompt_icon < 0 ? 2 : prompt_frame + 1;
    if (n >= 0)
        pointer_glyph_load(prompt_icon < 0 ? 0 : prompt_icon, n);
    else
        pointer_shape_set(0);
    if (!kbd_waiting())
        return -1;
    n = kbd_read();
    for (i = 0, w = dl_items; i < dl_used && !(*w->fn)(w, 5, n); i++, w++)
        ;
    if (i < dl_used) {
        (*w->fn)(w, 0x13);
        (*w->fn)(w, 1);
        (*w->fn)(w, 4, -1);
        (*w->fn)(w, 0x1b);
        if (w->flags & 0x10)
            return i;
    } else
        beep();
    return -1;
}

int far ui_track(void)                                           /* 25e5:17be */
{
    struct Wid far *w;
    int x;
    int y;
    register int i;
    register int n;

    inp_pos(&x, &y);
    for (i = 0, w = dl_items; i < dl_used && !(*w->fn)(w, 2, x, y); i++, w++)
        ;
    if (i < dl_used) {
        if (w->cursor)
            n = w->cursor;
        else
            n = prompt_icon < 0 ? 1 : prompt_frame;
    } else {
        n = prompt_icon < 0 ? 2 : prompt_frame + 1;
        i = -1;
    }
    if (n >= 0)
        pointer_glyph_load(prompt_icon < 0 ? 0 : prompt_icon, n);
    else
        pointer_shape_set(0);
    return i;
}

void far dl_end(void)                                       /* 25e5:187a */
{
    ui_send(0, dl_used - 1, 6);
    dl_running = 0;
}

void far ui_mark(int id, int msg, int a, int b)                  /* 25e5:1894 */
{
    if (!dl_running)
        return;
    if (id < 0 || id >= dl_used)
        error_box("DLItem index %d/%d invalid", id, dl_used);
    else
        (*dl_items[id].fn)(&dl_items[id], msg, a, b);
}

void far ui_send(int first, int last, int msg, int a, int b)     /* 25e5:18f0 */
{
    if (!dl_running)
        return;
    for (; first <= last; first++)
        (*dl_items[first].fn)(&dl_items[first], msg, a, b);
}

int far ui_query(int id, int what)                               /* 25e5:1936 */
{
    switch (what) {
    case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15:
    case 0x16:
        return (char)(dl_items[id].flags & (1 << (what - 0x10)) ? 1 : 0);
    case 0x24:
        return dl_items[id].width;
    case 0x2a:
        return dl_items[id].len;
    }
    return 0;
}
