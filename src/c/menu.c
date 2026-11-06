/* CKIT.EXE resident module 183a: menus.  Hotkey tables, the button hook,
 * menu text parsing and drawing, the key wait with its animation and
 * timeout, and the menu shapes built on them (prompts, lists, grids).
 * A port of Dark Queen of Krynn's menu.c.
 *
 * EXE 0x00a7a9..0x00cb5d, one Turbo C large-model _TEXT segment starting at
 * byte 9 of paragraph 183a.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is; those
 * carry prototypes below.  menu_split, menu_wait_key and menu_input are
 * static: they are near-called ahead of their definitions.
 *
 * _BSS follows the statics' names (Turbo C's symbol-table order), not their
 * declaration order; these names reproduce the shipped aba2..ad04 layout,
 * chosen with goldbox.bss_order.  The originals are not recoverable from it.
 *
 * A menu's fkeys flag selects key_filter, which also takes key codes
 * 0x100..0x108 and 0x140..0x159 (key_get's translated codes), over
 * key_filter_esc, which takes only Esc and backquote.
 */

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Node {
    struct Node far *next;      /* +0                                    */
    char key;                   /* +4                                    */
    char text[35];              /* +5                                    */
};

struct Wid {
    char pad_0[12];             /* +00 not read here                     */
    char far *text;             /* +0c                                   */
    int row;                    /* +10                                   */
    int col;                    /* +12                                   */
    char pad_14[4];             /* +14 not read here                     */
    int width;                  /* +18 in 4-column cells                 */
    int glyph;                  /* +1a not read here                     */
    char flags;                 /* +1c                                   */
};

struct ListBox {                /* list_menu's frame, handed to lbox_setup */
    int row;                    /* +00                                   */
    int col;                    /* +02                                   */
    int rows;                   /* +04                                   */
    int width;                  /* +06                                   */
    int flags;                  /* +08 2 = more entries than width       */
    int first;                  /* +0a                                   */
    int sel;                    /* +0c                                   */
    int id_base;                /* +0e                                   */
    char far *(far *fetch)(int n, char far *skip); /* +10                */
    char (far *done)(void);     /* +14                                   */
    void (far *on_select)(int sel); /* +18                               */
};

struct Pool;                    /* strutil.c's slot pool                 */

typedef int (far *HookFn)();          /* (w, msg, x, y), as dialog.c's ItemProc */

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far to_lower(int c);                             /* 00d7:25ab */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern char far *far substr(char far *s, int start, int len);    /* 0a75:0027 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far str_delete(char far *s, int pos, int n);         /* 0a75:01c4 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern char far key_pressed(void);                               /* 0da7:009a */
extern HookFn far ui_set_hook(int kind, HookFn fn);              /* 15e5:1236 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern int  far dl_item_count(void);                               /* 15e5:151e */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_run(int (far *idle)(void));              /* 15e5:1589 */
extern int  far ui_poll(void);                                   /* 15e5:15e6 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern long far ticks_now(void);                                 /* 06c9:096c */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far party_display(void far *node);                    /* 18de:1067 */
extern void far pic_frame(int col, int row, char show, unsigned char n); /* 18de:103f */
extern void far order_down(void);                              /* 18de:1099 */
extern void far party_key_select(unsigned char c);                   /* 18de:10a3 */
extern void far click_set(char c);                            /* 18de:10ad */
extern void far view_click(int n, int col, int row);           /* 18de:10b7 */
extern void far frame_piece_refresh(char c);                              /* 0d65:01c6 */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far lbox_setup(struct ListBox far *box, char redraw); /* 076b:08b6 */

extern struct HotKey hotkeys[20];                                /* b4c0 */
extern void far *node_pool;                                      /* ce5d */
extern char far *confirm_items;                                  /* 3c24 */
extern char far *continue_items;                                 /* 390c */
extern char confirm_anim;                                        /* b42a */
extern char menu_escaped;                                        /* ca2a */
extern struct Node far *node_list;                               /* d129 */
extern struct Node far *cur_node;                                /* b3dd */
extern int node_count;                                           /* b3e2 */
extern int frame_col;                                            /* c3a3 */
extern int frame_row;                                            /* c3a5 */
extern char anim_off;                                            /* cc78 */
extern char anim_first;                                          /* b758 */
extern unsigned char anim_last;                                  /* b759 */
extern unsigned char anim_frame;                                 /* b75a */
extern unsigned char anim_speed;                                 /* ca23 */
extern char tick_slot;                                           /* b3f2 */
extern int view_mode;                                            /* b421 */
extern char view_flag;                                           /* b40f */
extern char view_kind;                                           /* b40e */
extern long menu_timeout;                                        /* cfd2 */
extern char timeout_key;                                         /* cfd6 */

void far frame_invalidate(void);
char far step_mode(void);
char far menu_run(char far *title, char far *text, char fkeys, char anim);
void far set_menu_kind(int kind);
void far set_item_count(int n);
char far extra_row(void);
static void far menu_split(char far *s);
static int far menu_wait_key(char anim);
static char far menu_input(int rc, char far * far *items, char far *esc);

static int menu_kind = 2;                                        /* 1ca4 */
static char timed_out = 0;                                       /* 1ca6 */
static char aborted = 0;                                         /* 1ca7 */
static char picked = 0;                                          /* 1ca8 */
static char space_hit = 0;                                       /* 1ca9 */
static char has_default = 0;                                     /* 1caa */
static char redraw = 1;                                          /* 1cab */
static int list_top = 0;                                         /* 1cac */
static void (far *list_extra)(int sel) = 0;                      /* 1cae list_options_set sets these three */
static char list_anim = 0;                                       /* 1cb2 */
static char list_done_on = 1;                                    /* 1cb3 */

static int num;                                                  /* aba2 items in the menu */
static unsigned char width;                                      /* aba4 */
static struct Node far *cursor;                                  /* aba5 */
static unsigned char height;                                     /* aba9 */
static char text_buf[80];                                        /* abaa */
static int last_num;                                             /* abfa */
static int raw_key;                                              /* abfc */
static char show_extra;                                          /* abfe */
static char full_frame;                                          /* abff */
static char last_text[80];                                       /* ac00 */
static HookFn saved_hook;                                        /* ac50 */
static int click_row;                                            /* ac54 */
static unsigned char def_index;                                  /* ac56 */
static struct Node far *list_first;                              /* ac57 */
static int click_column;                                         /* ac5b */
static char last_prompt[80];                                     /* ac5d */
static char prompt_text[81];                                     /* acad */
static char stepping_mode;                                       /* acfe */
static int widgets_owned;                                        /* acff menu_line_show sets, menu_line_key subtracts */
static struct Node far *selected_node;                           /* ad01 */

void far hotkey_add(char key, unsigned char far *count)          /* 183a:0009 */
{
    if (*count == 0)
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
    hotkeys[*count].key = key;
    (*count)++;
}

void far hotkey_reset(unsigned char n)                           /* 183a:004b */
{
    unsigned char i;

    mem_fill(hotkeys, sizeof hotkeys, 0xff);
    for (i = 0; i <= n; i++)
        hotkeys[i].key = i;
}

int far button_hook(struct Wid far *w, int msg, int x, int y)    /* 183a:0092 */
{
    short top;
    short left;
    short bottom;
    short right;
    register int hi;
    register int attr;

    if (msg == 1) {
        w->flags |= 0x80;
        if (w->flags & 2)
            return 0;
        if (w->flags & 9)
            hi = 3;
        else
            hi = 0;
        text_xy(w->row, 0x1f40, &top, &left);
        if (!(w->flags & 0x20))
            top--;
        draw_glyph(top, w->col, 1, hi + 10);
        for (left = 0; left < w->width; left++)
            draw_glyph(top, w->col + left * 4, 1, hi + 11);
        draw_glyph(top, w->col + (left - 1) * 4, 1, hi + 12);
        if (hi)
            hi = 1;
        left = w->col;
        text_xy(w->row, left, &top, &left);
        if (!(w->flags & 0x20))
            top--;
        if (w->flags & 1)
            attr = 0x80;
        else if (w->flags & 0x40) {
            if (hi)
                attr = 0x8b;
            else
                attr = 0x8f;
        } else {
            if (hi)
                attr = 0x83;
            else
                attr = 0x87;
        }
        if (hi && screen_rows() >= 300)
            hi = 2;
        text_print_at(top + 1, left, attr, w->text);
        if (!(w->flags & 0x41) && (w->flags & 0x20))
            text_print_at(top + 1, left, (hi ? 0xb : 0xf) + 0x80, "%c", *w->text);
        return 0;
    } else if (msg == 2) {
        if (w->flags & 3)
            return 0;
        text_xy(w->row - 1, w->col - 2, &top, &left);
        text_xy(w->row + 5, w->col + w->width * 4 + 2, &bottom, &right);
        return top <= x && x < bottom && left <= y && y < right;
    }
    return (*saved_hook)(w, msg, x, y);
}

void far button_hook_install(void)                               /* 183a:02ef */
{
    saved_hook = ui_set_hook(1, button_hook);
}

char far get_picked(void)                                        /* 183a:0308 */
{
    return picked;
}

void far set_picked(char c)                                      /* 183a:030c */
{
    picked = c;
}

void far set_selected(struct Node far *p)                        /* 183a:0317 */
{
    selected_node = p;
}

struct Node far *far get_selected(void)                          /* 183a:0327 */
{
    return selected_node;
}

void far set_aborted(char c)                                     /* 183a:032f */
{
    aborted = c;
}

char far get_aborted(void)                                       /* 183a:033a */
{
    return aborted;
}

struct Node far *far list_nth(int n, struct Node far *head)      /* 183a:033e */
{
    struct Node far *p;
    register int i;

    i = 0;
    for (p = head; p && i != n; i++)
        p = p->next;
    if (i == n)
        return p;
    return 0;
}

int far list_count(struct Node far *head)                        /* 183a:0382 */
{
    struct Node far *p;
    register int n;

    n = 0;
    for (p = head; p; p = p->next)
        n++;
    return n;
}

void far box_draw(void)                                          /* 183a:03b2 */
{
    clip_set(0x1f9d, 0x1f40, 0x1fa4, 0x1fe0);
    draw_glyph(0x1f40, 0x1f40, 1, 4);
    clip_reset();
    frame_invalidate();
}

void far box_draw_top(void)                                         /* 183a:03ea */
{
    draw_glyph(0x1f40, 0x1f40, 1, 6);
}

void far box_draw_both(void)                                     /* 183a:0400 */
{
    clip_set(0x1f9d, 0x1f40, 0x1fa4, 0x1fe0);
    draw_glyph(0x1f40, 0x1f40, 1, 4);
    clip_reset();
    draw_glyph(0x1f40, 0x1f40, 1, 6);
    frame_invalidate();
}

int far confirm(char far *prompt)                                /* 183a:044d */
{
    unsigned char key;

    hotkey_reset(1);
    do {
        key = menu_run(prompt, confirm_items, 1, confirm_anim);
        if (menu_escaped) {
            if (key == 13)
                key = 0;
            else if (key == 0x1b || key == '`')
                key = 1;
            else
                key = 2;
        }
    } while (key < 0 || key > 1);
    return !key;
}

void far press_return(void)                                      /* 183a:04be */
{
    input_pending_clear();
    screen_draw_begin(1);
    box_draw();
    status_printf(7, 0x18, 0, 7, "Press        to continue.");
    frame_invalidate();
    dl_begin();
    ui_add(1, 0x1f9e, 0x1f78, "Return", 0x24, 6, 0x20, 0x40, 0x14, 0x16, 0x15, 0);
    dl_draw(1);
    screen_flush();
}

void far press_return_clear(void)                                /* 183a:053a */
{
    press_return();
    ui_run(0);
}

void far press_return_wait(char anim)                                    /* 183a:054a */
{
    press_return();
    set_menu_kind(2);
    menu_input(menu_wait_key(anim), 0, &menu_escaped);
}

void far list_alloc(int n, struct Node far * far *head)          /* 183a:0575 */
{
    struct Node far *p;
    register int i;

    pool_alloc((void far * far *)head, 40, node_pool);
    if (*head == 0)
        return;
    (*head)->next = 0;
    (*head)->key = 0;
    (*head)->text[0] = 0;
    p = *head;
    for (i = 1; i < n; i++) {
        pool_alloc((void far * far *)p, 40, node_pool);
        if (p->next == 0)
            return;
        p = p->next;
        p->next = 0;
        p->key = 0;
        p->text[0] = 0;
    }
}

void far list_free(struct Node far * far *head)                  /* 183a:0629 */
{
    struct Node far *p;
    struct Node far *q;

    p = *head;
    while (p) {
        q = p;
        p = p->next;
        if (q)
            pool_free(q, 40, node_pool);
    }
}

int far key_filter(int id, register int key)                     /* 183a:067d */
{
    while (key_poll()) {
        if (key == 0x152)
            sound_toggle();
        else if (key == 0x153)
            music_toggle();
        key = key_get();
    }
    if ((key >= 0x100 && key <= 0x108) || key == 0x1b || key == '`' ||
        (key >= 0x140 && key <= 0x159)) {
        if (key == 0x152)
            sound_toggle();
        if (key == 0x153)
            music_toggle();
        raw_key = key;
        return 1;
    }
    return 0;
}

int far key_filter_esc(int id, register int key)                 /* 183a:06f4 */
{
    while (key_poll()) {
        if (key == 0x152)
            sound_toggle();
        else if (key == 0x153)
            music_toggle();
        key = key_get();
    }
    if (key == 0x1b || key == '`') {
        raw_key = key;
        return 1;
    }
    if (key == 0x152)
        sound_toggle();
    else if (key == 0x153)
        music_toggle();
    return 0;
}

int far menu_parse(char far *s, char far * far *items)           /* 183a:0755 */
{
    char far *p;
    short count;
    char done;
    char at;

    has_default = 0;
    count = 0;
    for (p = s; *p == ' '; p++)
        ;
    if (*p) {
        done = 0;
        do {
            items[count] = p;
            at = 0;
            do {
                p++;
                if (*p == '@')
                    at = 1;
            } while (*p && (*p < 'A' || *p > 'Z'));
            if (at) {
                at = 0;
                def_index = count;
                has_default = 1;
            }
            if (*p == 0)
                done = 1;
            else
                p[-1] = 0;
            count++;
        } while (!done);
    } else
        items[0] = 0;
    return count;
}

void far menu_draw_items(int col, int n, char far * far *items)  /* 183a:0820 */
{
    short len;
    short start;
    register int i;
    register int attr;

    start = col;
    for (i = 0; i < n; i++) {
        len = str_len(items[i]);
        if (has_default) {
            if (def_index >= i)
                attr = 0x1f99;
            else
                attr = 0x1f9f;
        } else
            attr = 0x1f9e;
        ui_add(1, attr, col, items[i], 0x24, len, 0x20, to_upper(*items[i]),
               0x21, to_lower(*items[i]), 0x14, 0x15, 0);
        if (has_default && def_index == i)
            col = start;
        else
            col += (len + 1) * 4;
    }
}

void far menu_show(int n, char far * far *items, char far *title,
                   char fkeys, char show)                        /* 183a:090e */
{
    char buf[42];
    register int len;
    register int col;

    menu_escaped = 0;
    if (*title) {
        str_copy(buf, title);
        len = str_len(buf);
        if (title[len - 1] == ' ' || title[len - 1] == '@') {
            if (title[len - 2] == ':')
                buf[--len] = 0;
        } else if (title[len - 1] != ':') {
            str_cat(buf, " ");
            len++;
        }
        if (show)
            status_printf(0, 0x18, 0, 7, "%s", buf);
        col = len * 4 + 0x1f44;
    } else
        col = 0x1f43;
    if (show) {
        ui_add(7, fkeys ? key_filter : key_filter_esc, 0x14, 0);
        if (**items)
            menu_draw_items(col, n, items);
    }
}

void far grid_hit(int id, int x, int y)                          /* 183a:0a16 */
{
    short col;
    short row;

    text_xy_to(0x1f44, 0x1f44, x, y, &col, &row);
    click_column = col / 12;
    click_row = row / 12;
}

void far node_hit(int id, int x, int y)                          /* 183a:0a52 */
{
    short col;
    short row;
    struct Node far *p;
    char found;
    register int i;
    register int target;

    picked = 0;
    text_xy_to(frame_row, frame_col, x, y, &col, &row);
    target = col / 4;
    i = 0;
    p = node_list;
    found = 0;
    while (found != 1) {
        if (p == cur_node)
            found = 1;
        else {
            p = p->next;
            i++;
        }
    }
    if (i == target && (menu_kind == 13 || menu_kind == 9)) {
        ui_mark(id, 0x14);
        picked = 1;
    }
    while (i != target) {
        if (!step_mode())
            party_key_select(0x85);
        else
            order_down();
        i = (i + 1) % node_count;
    }
    if (!picked) {
        if (menu_kind == 12)
            set_aborted(1);
        party_display(cur_node);
    }
}

void far item_hit(int id, int x, int y)                          /* 183a:0b40 */
{
    short col;
    short row;
    struct Node far *p;
    char found;
    register int i;
    register int target;

    text_xy_to(frame_row, frame_col, x, y, &col, &row);
    target = col / 4;
    i = 0;
    p = node_list;
    found = 0;
    while (found != 1) {
        if (get_selected() == p)
            found = 1;
        else {
            p = p->next;
            i++;
        }
    }
    if (i == target && (menu_kind == 13 || menu_kind == 7)) {
        ui_mark(id, 0x14);
        picked = 1;
    }
    while (i != target) {
        set_selected(get_selected()->next);
        if (!get_selected())
            set_selected(node_list);
        i = (i + 1) % node_count;
    }
    party_display(get_selected());
}

void far menu_frame(register int row, register int col)          /* 183a:0c1f */
{
    struct Node far *p;

    p = node_list;
    for (node_count = 0; p && node_count < 9; node_count++)
        p = p->next;
    if (!full_frame) {
        if (last_num != node_count)
            ui_mark(num, 0x2a, node_count * 4);
    } else if (menu_kind == 7) {
        ui_add(5, row, col, node_count * 4, 0x1fe0 - (0xa0 - col), 0x22, item_hit, 0);
    } else if (menu_kind == 9 || menu_kind == 12) {
        ui_add(5, row, col, node_count * 4, 0x1fe0 - (0xa0 - col), 0x22, node_hit, 0x14, 0);
    } else {
        ui_add(5, row, col, node_count * 4, 0x1fe0 - (0xa0 - col), 0x22, node_hit, 0);
    }
    last_num = node_count;
}

void far frame_invalidate(void)                                  /* 183a:0d3c */
{
    redraw = full_frame = 1;
}

int far menu_setup(char far *text, char far * far *items, char far *title,
                   char far *fkeys)                              /* 183a:0d45 */
{
    register int n;

    menu_split(text);
    if (str_cmp(last_text, prompt_text) || str_cmp(last_prompt, title))
        redraw = 1;
    else if (menu_kind == 8)
        redraw = 0;
    str_copy(text_buf, prompt_text);
    str_copy(last_text, prompt_text);
    str_copy(last_prompt, title);
    n = menu_parse(text_buf, items);
    set_item_count(n);
    if (redraw) {
        dl_begin();
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        box_draw();
    }
    menu_show(n, items, title, *fkeys, redraw);
    if (redraw) {
        dl_draw(1);
        screen_flush_mode_set(0);
        screen_flush();
    }
    full_frame = redraw;
    redraw = 0;
    return n;
}

void far set_item_count(int n)                                   /* 183a:0e56 */
{
    num = n;
}

int far get_item_count(void)                                     /* 183a:0e61 */
{
    return num;
}

static void far menu_split(char far *s)                          /* 183a:0e65 */
{
    unsigned char k;
    unsigned char start;
    unsigned char i;
    unsigned char n;
    unsigned char len;
    char buf[82];

    len = str_length(s);
    str_copy(prompt_text, "");
    n = 0;
    k = 0;
    i = 0;
    while (i < len) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            if (hotkeys[n].key == k) {
                hotkeys[n].ch = s[i];
                start = i;
                do
                    i++;
                while ((s[i] < 'A' || s[i] > 'Z') && i < len);
                str_copy(buf, substr(s, start + 1, i - start));
                str_copy(prompt_text, msg_format("%s%s", prompt_text, buf));
                n++;
            } else
                i++;
            k++;
        } else
            i++;
    }
    len = str_length(prompt_text);
    if (prompt_text[len - 1] == ' ' || prompt_text[len - 1] == '@')
        str_delete(prompt_text, len, 1);
}

static int far menu_wait_key(char anim)                          /* 183a:0fe7 */
{
    long tick[5];
    long flash_at;
    long flash_end;
    long frame_at;
    char key;
    register int rc;

    timed_out = 0;
    if (anim_off)
        anim = 0;
    if (menu_kind == 2 || menu_kind == 7 || menu_kind == 13 || menu_kind == 12) {
        for (tick_slot = 1; tick_slot <= 4; tick_slot++)
            tick[tick_slot] = ticks_now();
        frame_at = ticks_now();
        flash_at = ticks_now() + 30;
        flash_end = flash_at + 50;
    }
    do {
        if (is_xvga() || !pixel_shift())
            input_poll();
        key = key_pressed();
        rc = ui_poll();
        if (!key && (menu_kind == 2 || menu_kind == 7 || menu_kind == 13 || menu_kind == 12)) {
            if (view_mode == 3 && view_flag == 'y' && view_kind != 'P' && ticks_now() >= flash_at)
                flash_at = flash_end + 30;
            if (anim && anim_speed > 0 && anim_last >= 1) {
                pic_frame(3, 3, anim, anim_frame);
                if (anim_first)
                    frame_piece_refresh(1);
                if (ticks_now() - frame_at >= 50) {
                    if (++anim_frame > anim_last)
                        anim_frame = anim_first;
                    frame_at = ticks_now();
                }
            } else
                frame_at = ticks_now();
            if (menu_timeout > 0 && ticks_now() - frame_at >= menu_timeout) {
                timed_out = 1;
                break;
            }
            if (view_mode == 3 && view_flag == 'y' && view_kind != 'P' && ticks_now() >= flash_end)
                flash_end = flash_at + 50;
        }
    } while (rc < 0);
    if (get_aborted())
        rc = 0;
    return rc;
}

static char far menu_input(int rc, char far * far *items, char far *esc) /* 183a:11e5 */
{
    char key;
    unsigned char j;
    char c;
    register int n;

    if (timed_out)
        return timeout_key;
    n = get_item_count();
    if (menu_kind == 1)
        n++;
    if ((menu_kind == 4 || menu_kind == 1) && rc > n && n + 8 >= rc) {
        menu_escaped = 1;
        switch (rc - n - 1) {
        case 0: key = 0x86; break;
        case 1: key = 0x88; break;
        case 2: key = 0x82; break;
        case 3: key = 0x84; break;
        case 4: key = 0x87; break;
        case 5: key = 0x81; break;
        case 6: key = 0x83; break;
        case 7: key = 0x85; break;
        }
    } else if (menu_kind == 4 && n + 9 == rc) {
        menu_escaped = 1;
        key = ' ';
    } else if (menu_kind == 5 && rc > n && rc != 0xff) {
        view_click(rc - n, click_column, click_row);
        key = 0x1b;
    } else if (menu_kind == 10 && rc > n && rc != 0xff) {
        set_item_count(get_item_count() + 1);
        rc = rc - get_item_count();
        key = rc;
        click_set(1);
    } else if ((menu_kind == 7 && picked) || (menu_kind == 13 && picked) ||
               (menu_kind == 12 && get_aborted())) {
        key = 'S';
    } else {
        if (rc == 0) {
            if (raw_key > 0xff)
                key = (raw_key & 0x7f) + 0x80;
            else {
                key = raw_key;
                if (raw_key < 0x20)
                    menu_escaped = 1;
            }
        } else {
            j = 0;
            c = *items[rc - 1];
            while (j < 20 && hotkeys[j].ch != c)
                j++;
            if (j < 20)
                key = hotkeys[j].key;
            else
                key = 0xff;
        }
        if (key == '`')
            key = 0x1b;
        if ((unsigned char)key > 20)
            menu_escaped = 1;
    }
    menu_kind = 2;
    return key;
}

void far set_menu_kind(int kind)                                 /* 183a:13f2 */
{
    menu_kind = kind;
}

char far menu_bar(char far *title, char far *text, char fkeys, char anim) /* 183a:13fd */
{
    char far *items[20];
    char key;

    set_item_count(menu_setup(text, items, title, &fkeys));
    if (full_frame) {
        ui_add(5, 0x1f64, 0x1f40, 4, 0x50, 0x14,
               5, 0x1f68, 0x1f40, 4, 0x50, 0x14,
               5, 0x1f6c, 0x1f40, 4, 0x50, 0x14,
               5, 0x1f70, 0x1f40, 4, 0x50, 0x14,
               5, 0x1f74, 0x1f40, 4, 0x50, 0x14,
               5, 0x1f78, 0x1f40, 4, 0x50, 0x14,
               5, 0x1f44, 0x1f40, 4, 0x50, 0x14, 0);
        if (extra_row())
            ui_add(0x11, 0);
        ui_add(5, 0x1f4c, 0x1f90, 4, 0x50, 0x14, 0);
    }
    set_menu_kind(10);
    key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    return key;
}

int far space_cb(int id, int key)                                /* 183a:154a */
{
    if (key == ' ') {
        space_hit = 1;
        ui_mark(id, 0x14);
        return 1;
    }
    return 0;
}

char far grid_choose(char far *title, char far *text, char fkeys, char anim,
                     unsigned char rows, unsigned char cols)     /* 183a:156f */
{
    char far *items[20];
    char key;

    if (height != rows || width != cols) {
        frame_invalidate();
        height = rows;
        width = cols;
    }
    set_item_count(menu_setup(text, items, title, &fkeys));
    if (full_frame) {
        ui_add(5, rows * 12 + 0x1f44, 0x1f40, 0xc, cols * 12 + 4, 0x29, 9, 0x14,
               5, 0x1f40, cols * 12 + 0x1f44, rows * 12 + 2, 0xc, 0x29, 0xb, 0x14,
               5, rows * 12 + 0x1f44, (cols + 1) * 12 + 0x1f42, 0xc, (6 - cols) * 12 + 6, 0x29, 5, 0x14,
               5, (rows + 1) * 12 + 0x1f46, cols * 12 + 0x1f42, (6 - rows) * 12 + 4, 0xc, 0x29, 7, 0x14,
               0);
    }
    if (full_frame) {
        ui_add(5, 0x1f40, 0x1f40, rows * 12 + 4, cols * 12 + 4, 0x29, 0xa, 0x14,
               5, 0x1f40, (cols + 1) * 12 + 0x1f44, rows * 12 + 4, (6 - cols) * 12 + 4, 0x29, 4, 0x14,
               5, (rows + 1) * 12 + 0x1f44, (cols + 1) * 12 + 0x1f44, (6 - rows) * 12 + 4,
               (6 - cols) * 12 + 4, 0x29, 6, 0x14,
               5, (rows + 1) * 12 + 0x1f44, 0x1f40, (6 - rows) * 12 + 4, cols * 12 + 4, 0x29, 8, 0x14,
               0);
    }
    if (full_frame)
        ui_add(7, space_cb, 0);
    set_menu_kind(4);
    key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    if (space_hit) {
        space_hit = 0;
        key = ' ';
    }
    return key;
}

char far grid_pick(char far *title, char far *text, char fkeys, char anim) /* 183a:1842 */
{
    char far *items[20];
    char key;

    set_item_count(menu_setup(text, items, title, &fkeys));
    if (full_frame) {
        ui_add(5, 0x1f44, 0x1f40, 0x54, 4, 0x29, 9, 0x14, 0x22, grid_hit,
               5, 0x1f40, 0x1f44, 4, 0x54, 0x29, 0xb, 0x14, 0x22, grid_hit,
               5, 0x1f44, 0x1f98, 0x54, 8, 0x29, 5, 0x14, 0x22, grid_hit,
               5, 0x1f98, 0x1f44, 0xc, 0x54, 0x29, 7, 0x14, 0x22, grid_hit,
               0);
    }
    if (full_frame)
        ui_add(5, 0x1f44, 0x1f44, 0x54, 0x54, 0x29, 0x18, 0x14, 0x22, grid_hit, 0);
    key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    return key;
}

char far node_menu(char far *title, char far *text, char fkeys, char anim) /* 183a:197d */
{
    char far *items[20];
    char key;

    set_menu_kind(7);
    set_item_count(menu_setup(text, items, title, &fkeys));
    if (view_mode == 0) {
        frame_row = 0x1f50;
        frame_col = 0x1f44;
    } else {
        frame_row = 0x1f50;
        frame_col = 0x1f84;
    }
    menu_frame(frame_row, frame_col);
    picked = 0;
    key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    return key;
}

char far node_prompt(char far *title, char far *text, char fkeys, char anim) /* 183a:1a07 */
{
    char key;
    char far *items[20];

    frame_row = 0x1f50;
    frame_col = 0x1f84;
    if (str_same(text, continue_items)) {
        set_item_count(1);
        set_menu_kind(2);
        press_return();
        menu_frame(frame_row, frame_col);
        menu_input(menu_wait_key(anim), 0, &menu_escaped);
        menu_escaped = 0;
        key = 13;
    } else {
        if (menu_kind != 12 && menu_kind != 13)
            set_menu_kind(2);
        set_item_count(menu_setup(text, items, title, &fkeys));
        menu_frame(frame_row, frame_col);
        key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    }
    return key;
}

char far node_dir_menu(char far *title, char far *text, char fkeys, char anim) /* 183a:1ae7 */
{
    char far *items[20];
    char key;

    frame_row = 0x1f50;
    frame_col = 0x1f84;
    set_menu_kind(1);
    set_item_count(menu_setup(text, items, title, &fkeys));
    menu_frame(frame_row, frame_col);
    if (full_frame) {
        ui_add(5, 0x1f40, 0x1f40, 0x32, 0x14, 0x29, 0x16, 0x14,
               5, 0x1f40, 0x1f54, 0x32, 0x1c, 0x29, 0xb, 0x14,
               5, 0x1f40, 0x1f70, 0x32, 0x14, 0x29, 0x15, 0x14,
               5, 0x1f72, 0x1f40, 0x1e, 0x44, 0x29, 0x17, 0x14,
               0);
    }
    key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    return key;
}

char far grid_small(char far *title, char far *text, char fkeys, char anim,
                    unsigned char rows, unsigned char cols)      /* 183a:1be9 */
{
    char far *items[20];
    char key;

    frame_invalidate();
    rows *= 4;
    cols *= 4;
    set_menu_kind(4);
    set_item_count(menu_setup(text, items, title, &fkeys));
    if (full_frame) {
        ui_add(5, rows + 0x1f42, 0x1f3f, 6, cols + 3, 0x29, 9, 0x14,
               5, 0x1f3f, cols + 0x1f42, rows + 3, 6, 0x29, 0xb, 0x14,
               5, rows + 0x1f42, cols + 0x1f48, 6, 0xff - cols, 0x29, 5, 0x14,
               5, rows + 0x1f48, cols + 0x1f42, 0xff - rows, 6, 0x29, 7, 0x14,
               0);
    }
    if (full_frame) {
        ui_add(5, 0x1f3f, 0x1f3f, rows + 3, cols + 3, 0x29, 0xa, 0x14,
               5, 0x1f3f, cols + 0x1f48, rows + 3, 0xff - cols, 0x29, 4, 0x14,
               5, rows + 0x1f48, cols + 0x1f48, 0xff - rows, 0xff - cols, 0x29, 6, 0x14,
               5, rows + 0x1f48, 0x1f3f, 0xff - rows, cols + 3, 0x29, 8, 0x14,
               0);
    }
    if (full_frame)
        ui_add(5, rows + 0x1f42, cols + 0x1f42, 6, 6, 0x29, 3, 0);
    key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    return key;
}

int far menu_line_key(int key)                                       /* 183a:1e2d */
{
    char far *items[20];
    register int n;

    str_copy(text_buf, prompt_text);
    n = menu_parse(text_buf, items);
    if (n == 0)
        return 13;
    if (key >= widgets_owned && widgets_owned + n >= key)
        return (unsigned char)menu_input(key - widgets_owned, items, &menu_escaped);
    return -1;
}

void far menu_line_show(char far *title, char far *text, char fkeys)  /* 183a:1e9a */
{
    char far *items[20];
    register int n;

    widgets_owned = dl_item_count();
    if (str_same(text, continue_items)) {
        status_printf(7, 0x18, 0, 7, "Press        to continue.");
        ui_add(1, 0x1f9e, 0x1f78, "Return", 0x24, 6, 0x20, 0x40, 0x14, 0x16, 0x15, 0);
        frame_invalidate();
        set_menu_kind(2);
        menu_escaped = 0;
    } else {
        menu_split(text);
        redraw = 1;
        str_copy(text_buf, prompt_text);
        str_copy(last_text, "");
        str_copy(last_prompt, "");
        n = menu_parse(text_buf, items);
        set_item_count(n);
        menu_show(n, items, title, fkeys, redraw);
        full_frame = redraw;
        redraw = 0;
    }
}

char far menu_run(char far *title, char far *text, char fkeys, char anim) /* 183a:1fb3 */
{
    char far *items[20];
    char key;

    if (str_same(text, continue_items)) {
        press_return_wait(anim);
        key = 13;
        menu_escaped = 0;
    } else {
        menu_setup(text, items, title, &fkeys);
        key = menu_input(menu_wait_key(anim), items, &menu_escaped);
    }
    return key;
}

char far *far list_next(int n, char far *key)                    /* 183a:2027 */
{
    register int i;

    if (n < 0) {
        if (!cursor->next)
            return 0;
        cursor = cursor->next;
    } else {
        cursor = list_first;
        for (i = 0; i < n; i++) {
            if (!cursor)
                return 0;
            cursor = cursor->next;
        }
    }
    if (!cursor)
        return 0;
    *key = cursor->key;
    return cursor->text;
}

char far list_done(void)                                         /* 183a:20a3 */
{
    ui_mark(1, 4);
    return 1;
}

void far list_options_set(void (far *extra)(int sel), char anim, char done)        /* 183a:20b5 */
{
    list_extra = extra;
    list_anim = anim;
    list_done_on = done;
}

char far list_menu(char far *title, char far *text, char row, char col,
                   char row2, char col2, struct Node far *list, void far *unused,
                   char far *redraw_box, int far *top,
                   struct Node far * far *out)                   /* 183a:20d1 */
{
    short rc;
    char c;
    char key;
    struct ListBox box;
    char far *items[20];
    struct Node far *p;
    char fkeys;                 /* never set: menu_setup reads a stale byte */
    char result;
    unsigned char j;
    int i;
    register int n;

    if (!list) {
        top = 0;                /* the pointers, not *top and *out: 183a:20e1 */
        out = 0;
        return 0x1b;
    }
    n = menu_setup(text, items, title, &fkeys);
    if (*top < list_top) {
        list_top = *top;
        *redraw_box = 1;
    }
    box.first = list_top;
    p = list_first = list;
    for (i = 0; p; p = p->next, i++)
        if (*top == i && p->key)
            (*top)++;
    box.row = col * 4;
    box.col = row * 4;
    box.rows = col2 - col;
    box.width = row2 - row;
    if (col2 - col < i)
        box.flags = 2;
    else
        box.flags = 0;
    box.sel = *top;
    box.id_base = n + 1;
    box.fetch = list_next;
    box.done = list_done_on ? list_done : 0;
    box.on_select = list_extra;
    list_extra = 0;
    if (box.flags) {
        if (row2 == '&')
            box.width--;
    } else if (row2 != '&')
        box.width++;
    frame_invalidate();
    dl_begin();
    screen_draw_begin(1);
    screen_flush_mode_set(1);
    box_draw();
    menu_show(n, items, title, 0, 1);
    dl_draw(1);
    screen_flush_mode_set(0);
    screen_flush();
    lbox_setup(&box, *redraw_box);
    *redraw_box = 0;
    if (list_done_on)
        ui_mark(1, 0x21, 0x40);
    list_done_on = 1;
    rc = menu_wait_key(list_anim);
    list_anim = 0;
    if (timed_out)
        return timeout_key;
    dl_end();
    if (rc == 0) {
        c = 0x1b;
        menu_escaped = 1;
    } else if (rc > n)
        c = *items[0];
    else
        c = *items[rc - 1];
    for (j = 0; j < 20 && hotkeys[j].ch != c; j++)
        ;
    if (j < 20)
        result = hotkeys[j].key;
    else if (c != 0x1b)
        result = 0xff;
    else
        result = 0x1b;
    *top = box.sel;
    list_next(*top, &key);
    *out = cursor;
    list_top = box.first;
    return result;
}

void far set_step_mode(char c)                                   /* 183a:239f */
{
    stepping_mode = c;
}

char far step_mode(void)                                         /* 183a:23aa */
{
    return stepping_mode;
}

void far set_extra_row(char c)                                   /* 183a:23ae */
{
    show_extra = c;
}

char far extra_row(void)                                         /* 183a:23b9 */
{
    return show_extra;
}
