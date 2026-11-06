/* CKIT.EXE resident module 16c9: text in windows.  The status line, framed
 * on row 0x18 or, in view mode 5, filled on row 0x17; text word-wrapped
 * into a window that waits for a key when it fills; the framed prompts that
 * read a line of text or a number; the delays scaled by the setting at
 * cursor_obj + 0x12; and padding a string on the left.
 *
 * EXE 0x009092..0x009abc, one Turbo C large-model _TEXT segment starting at
 * byte 2 of paragraph 16c9.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 15f5's last retf is at 0x009091, and module 176b starts at
 * 0x009abc (176b:000c) with no stub between.
 *
 * _DATA is 1c6a..1c9f, the pause_ms table and six literals; 1c69 is pad
 * after module 15f5's literals, and 1ca0 is module 176b's.  _BSS is
 * ab74..ab9d, after module 15f5's a7f8..ab73 and before module 176b's
 * ab9e.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  A name another module
 * here gives an address is reused from there; box_draw and
 * press_return_clear are DQK's definitions.  A modNNNN_ name is a
 * placeholder that says only which segment the callee lives in, and
 * textwin_NNNN only where in this module the routine starts.  The
 * variables are DQK's names, which DQK's C declares but never defines.
 * Ghidra's index misses 001d, 0021 and 098a.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is: text_wrap
 * far-calls text_clear and status_msg far-calls msg_pause, so those carry
 * prototypes.  text_wrap calls wait_continue near ahead of its definition,
 * which only a static function allows.
 *
 * A coordinate from 0x1f40 up is a text cell: 0x1f40 + 4 * n is column or
 * row n.  Some calls pass 1 or 2 above that form; what the low bits select
 * is not established.
 */

extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern char far *far str_find(char far *s, char c);          /* 00d7:258e */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far prompt_window_push(int row, int col);                   /* 0df0:0020 */
extern void far prompt_attr_set(int attr);                           /* 0df0:005a */
extern void far prompt_beep_set(char on);                            /* 0df0:006c */
extern void far prompt_window_pop(void);                               /* 0df0:0077 */
extern char far line_edit(char far *prompt, char far *buf, int max);
                                 /* 18de:1085, vector to section 3 1c3e:0007 */
extern int  far num_edit(int lo, int hi, char far *prompt);
                                 /* 18de:108f, vector to section 3 1c3e:03c0 */
extern void far str_upr(char far *s);                       /* 00d7:25f5 */
extern void far press_return_clear(void);                        /* 083a:053a */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far clear_screen(int a);                             /* 0da7:029d */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */

extern char far *continue_items;                                 /* 390c */
extern unsigned char text_col;                                   /* b419 */
extern unsigned char text_row;                                   /* b41a */
extern int view_mode;                                            /* b421 */
extern char text_slow;                                           /* b431 */
extern unsigned char far *cursor_obj;                            /* b75b */

void far text_clear(char x1, char y1, char x2, char y2);
void far msg_pause(void);
static void wait_continue(void);

static unsigned pause_ms[10] = {                                 /* 1c6a */
    10, 100, 250, 500, 1000, 1500, 2000, 3000, 4000, 5000
};

static char input_ok;                                            /* ab74 */
static char input_buf[41];      /* ab75, up to 176b's ab9e; needs >= 0x24 */

void far text_fill(int row1, int col1, int row2, int col2, int attr) /* 16c9:0002 */
{
    fill_rect(row1, col1, row2, col2, attr);
}

char far input_result(void)                                      /* 16c9:001d */
{
    return input_ok;
}

void far input_done(void)                                        /* 16c9:0021 */
{
    prompt_window_push(0x1f9e, 1);
    prompt_attr_set(0x8f);
    prompt_beep_set(1);
}

void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                       unsigned char bg, char far *fmt, ...)     /* 16c9:0044 */
{
    short at_row;
    short top;
    short len;
    short left;
    short right;
    short bottom;
    short junk;
    char buf[82];

    str_format(buf, "%r", fmt, (void far *)(&fmt + 1));
    screen_flush();
    if (bg == 0) {
        if (row >= 0x17) {
            bg = 8;
            if (fg == 8)
                fg = 15;
            else if (fg == 7)
                fg = 15;
        } else
            bg = 8;
    }
    if (row == 0x18) {
        text_xy(0x1f9e, 0x1f40, &at_row, &top);
        at_row++;
        col++;
        text_xy(at_row, 0, &top, &len);
        text_xy(0x1fa2, col * 4 + 0x1f40, &bottom, &left);
        bottom++;
        len = str_len(buf);
        text_xy(0, (col + len) * 4 + 0x1f40, &junk, &right);
        fill_rect(top - 2, left - 1, top - 1, right + 1, 0);
        text_fill(top - 1, left, top, right, bg);
        fill_rect(top - 1, left - 1, bottom, left, 0);
        fill_rect(top - 1, right, bottom, right + 1, 0);
        fill_rect(bottom, left - 1, bottom + 1, right + 1, 0);
    } else if (view_mode == 5 && row == 0x17) {
        at_row = 0x1f99;
        col++;
        text_xy(at_row, col * 4 + 0x1f40, &bottom, &left);
        len = str_len(buf);
        text_fill(bottom - 1, col * 4 + 0x1f40, bottom, (col + len) * 4 + 0x1f40, bg);
    } else
        at_row = row * 4 + 0x1f40;
    text_print_at(at_row, col * 4 + 0x1f40, (bg << 4) + fg, "%s", buf);
}

void far text_repeat(unsigned char col, unsigned char row, unsigned char fg,
                     unsigned char bg, unsigned char n, unsigned char c,
                     char unused)                                /* 16c9:0281 */
{
    register int i;
    register int y;
    short x;

    y = row * 4 + 0x1f40;
    x = col * 4 + 0x1f40;
    if (bg == 0)
        bg = 8;
    for (i = 0; i < n; i++)
        text_print_at(y, i * 4 + x, (bg << 4) + fg, "%c", c);
}

char far is_break(char c)                                        /* 16c9:02f4 */
{
    return *str_find("()[]{}-.,?!\":;", c);
}

void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                   unsigned char y2, unsigned char fg, unsigned char bg,
                   char clear, char far *text)                   /* 16c9:030f */
{
    register int end;
    register int at;
    short last;

    screen_flush();
    if (bg == 0)
        bg = 8;
    if (x1 > 0x27 || y1 > 0x18 || x2 > 0x27 || y2 > 0x27) /* y2: 0x27, shipped */
        return;
    if (text_col < x1 || text_col > x2 + 1 || text_row < y1 || text_row > y2) {
        text_col = x1;
        text_row = y1;
    }
    if (clear) {
        text_clear(x1, y1, x2, y2);
        text_col = x1;
        text_row = y1;
    }
    at = 0;
    last = str_length(text) - 1;
    if (last < 0)
        return;
    do {
        for (end = at; end < last && is_break(text[end]); end++)
            ;
        for (; end < last && !is_break(text[end]) && text[end] != ' '; end++)
            ;
        for (; end < last && is_break(text[end]); end++)
            ;
        if (end < last && text[end] != ' ')
            end--;
        if (end - at + text_col > x2) {
            if (end - at + text_col == x2 + 1 && text[end] == ' ') {
                end--;
                for (; at <= end; at++, text_col++) {
                    text_repeat(text_col, text_row, fg, bg, 1, text[at], 1);
                    if (text_slow)
                        delay(cursor_obj[0x12] * 3);
                }
            }
            text_col = x1;
            text_row++;
            for (; at <= last && text[at] == ' '; at++)
                ;
            if (text_row > y2 && at < last) {
                text_col = x1;
                text_row = y1;
                box_draw();
                wait_continue();
                text_clear(x1, y1, x2, y2);
                for (; at <= end; at++, text_col++) {
                    text_repeat(text_col, text_row, fg, bg, 1, text[at], 1);
                    if (text_slow)
                        delay(cursor_obj[0x12] * 3);
                }
            }
        } else
            for (; at <= end; at++, text_col++) {
                text_repeat(text_col, text_row, fg, bg, 1, text[at], 1);
                if (text_slow)
                    delay(cursor_obj[0x12] * 3);
            }
    } while (at <= last);
}

void far status_char(char col, char row, char attr, unsigned char c) /* 16c9:059a */
{
    status_printf(col, row, attr, attr, "%( %)", c);
}

char far *far input_text(char far *prompt, unsigned char fg, unsigned char bg,
                         unsigned char max)                      /* 16c9:05bc */
{
    short at_row;
    short top;
    short len;
    short left;
    short right;
    short bottom;
    short junk;
    short col;

    if (bg == 0)
        bg = 8;
    if (fg == bg)
        fg = 0;
    max = int_min(0x23, max);
    input_buf[0] = input_buf[1] = 0;
    text_xy(0x1f9e, 0x1f40, &at_row, &col);
    at_row++;
    col = 0;
    col++;
    text_xy(at_row, 0, &top, &len);
    text_xy(0x1fa2, col * 4 + 0x1f40, &bottom, &left);
    bottom++;
    len = 0x26;
    text_xy(0, (col + len) * 4 + 0x1f40, &junk, &right);
    fill_rect(top - 2, left - 1, top - 1, right + 1, 0);
    text_fill(top - 1, left, top, right, bg);
    fill_rect(top - 1, left - 1, bottom, left, 0);
    fill_rect(top - 1, right, bottom, right + 1, 0);
    fill_rect(bottom, left - 1, bottom + 1, right + 1, 0);
    prompt_window_push(at_row, 1);
    prompt_attr_set((bg << 4) + fg);
    if (line_edit(prompt, input_buf, max))
        input_ok = 1;
    else
        input_ok = 0;
    prompt_window_pop();
    str_upr(input_buf);
    return input_buf;
}

int far input_number(char far *prompt, unsigned char fg, unsigned char bg) /* 16c9:0774 */
{
    short at_row;
    short top;
    short len;
    short left;
    short right;
    short bottom;
    short junk;
    short col;
    register int n;

    if (bg == 0)
        bg = 8;
    if (fg == bg)
        fg = 0;
    text_xy(0x1f9e, 0x1f40, &at_row, &col);
    at_row++;
    col = 0;
    col++;
    text_xy(at_row, 0, &top, &len);
    text_xy(0x1fa2, col * 4 + 0x1f40, &bottom, &left);
    bottom++;
    len = 0x26;
    text_xy(0, (col + len) * 4 + 0x1f40, &junk, &right);
    fill_rect(top - 2, left - 1, top - 1, right + 1, 0);
    text_fill(top - 1, left, top, right, bg);
    fill_rect(top - 1, left - 1, bottom, left, 0);
    fill_rect(top - 1, right, bottom, right + 1, 0);
    fill_rect(bottom, left - 1, bottom + 1, right + 1, 0);
    prompt_window_push(at_row, 1);
    prompt_attr_set((bg << 4) + fg);
    while ((n = num_edit(0, 0x7fff, prompt)) < 0)
        ;
    prompt_window_pop();
    return n;
}

void far status_msg(char far *fmt, char fg, char bg, ...)        /* 16c9:08f7 */
{
    box_draw();
    status_printf(0, 0x18, fg, bg, "%r", fmt, (void far *)(&bg + 2)); /* a char argument fills a word */
    screen_flush();
    msg_pause();
    box_draw();
}

void far continue_prompt(char a, char b, char far *items)        /* 16c9:0932 */
{
    press_return_clear();
}

void far text_pause(void)                                        /* 16c9:093c */
{
    delay(cursor_obj[0x12] * 100);
}

void far msg_pause(void)                                         /* 16c9:0953 */
{
    delay(pause_ms[cursor_obj[0x12]]);
}

long far ticks_now(void)                                         /* 16c9:096c */
{
    return timer_ticks() * 100 / 72;
}

void far clear_black(void)                                       /* 16c9:098a */
{
    clear_screen(0);
}

void far text_clear(char x1, char y1, char x2, char y2)          /* 16c9:0994 */
{
    text_fill(y1 * 4 + 0x1f40, x1 * 4 + 0x1f40, y2 * 4 + 0x1f44, x2 * 4 + 0x1f44, 8);
}

static void wait_continue(void)                                  /* 16c9:09d3 */
{
    input_pending_clear();
    continue_prompt(0, 0xb, continue_items);
}

void far str_pad_left(char far *s, unsigned char width)          /* 16c9:09ee */
{
    while (str_length(s) < width)
        str_copy(s, msg_format(" %s", s));
}
