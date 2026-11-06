/* CKIT.EXE resident module 1df0: the prompt lines.  A stack of up to five
 * lines (win_top indexes it) keeps each line's row, column and attribute;
 * win_vprint clears the current line from its column to its background and
 * prints the formatted text centred on the full line width.  win_prompt
 * waits for a key, beeping when prompt_beep is set.  win_yes_no beeps, then
 * answers y, or n, Esc or ` as no, and beeps again on any other key; a
 * mouse click, which mouse_press polls for with button mask 7, ends it,
 * yes inside the line and no outside.
 * Both draw an icon through pointer_glyph_load in two frames.
 *
 * EXE 0x01030f..0x01063d, one Turbo C large-model _TEXT segment starting at
 * byte f of paragraph 1df0.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1da7's last retf is at 0x01030e, and module 1e23 starts at
 * 0x01063d (1e23:000d) with no stub between.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module here gives an address is reused from there;
 * beep, the key_ routines and text_xy_to are DQK's definitions.  A modNNNN_
 * name is a placeholder that says only which segment the callee lives in,
 * and prompt_NNNN only where in this module the routine starts.
 *
 * The line stack, prompt_icon and prompt_frame are public: code outside
 * this module reads them (overlay section 3 code from 0x02d012 the line
 * stack, resident code at 0x01990d prompt_icon and prompt_frame).  No
 * operand outside this module names prompt_beep or the "%r" literal.
 */

extern void far text_offset_xy(int row, int col, int row2, int col2,
                             int far *out_row, int far *out_col); /* 107e:0565 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern void far beep(void);                                      /* 0da7:018c */
extern void far pointer_glyph_load(int icon, int frame);               /* 11d7:0cf9 */
extern void far key_wait(void);                                  /* 0da7:00f5 */
extern void far key_flush(void);                                 /* 0da7:00bd */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern int  far to_lower(int c);                             /* 00d7:25ab */
extern int  far mouse_press(char mask, int far *a, int far *b); /* 0f5e:0ee9 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */

int win_top = 0;                                                 /* 96aa */
int prompt_icon = -1;                                            /* 96ac */
int win_row[5] = { 0x1fa0 };                                     /* 96ae */
int win_col[5] = { 0 };                                          /* 96b8 */
int win_attr[5] = { 15 };                                        /* 96c2 */
static char prompt_beep = 1;                                     /* 96cc */

int prompt_frame;                                                /* d590 */

void far prompt_icon_set(int icon, int frame)                    /* 1df0:000f */
{
    prompt_icon = icon;
    prompt_frame = frame;
}

void far prompt_window_push(int row, int col)                           /* 1df0:0020 */
{
    if (win_top < 4)
        win_top++;
    win_row[win_top] = row;
    win_col[win_top] = col * 4;
    win_attr[win_top] = 15;
}

void far prompt_attr_set(int attr)                                   /* 1df0:005a */
{
    win_attr[win_top] = attr;
}

void far prompt_beep_set(char on)                                    /* 1df0:006c */
{
    prompt_beep = on;
}

void far prompt_window_pop(void)                                       /* 1df0:0077 */
{
    if (win_top > 0)
        win_top--;
}

void far win_vprint(char far *fmt, void far *args)               /* 1df0:0083 */
{
    int row2;
    int col2;
    char buf[80];

    text_offset_xy(win_row[win_top], 0x1fe0 - win_col[win_top], 0x1f44, 0,
                 &row2, &col2);
    fill_rect(win_row[win_top], win_col[win_top] + 0x1f40, row2, col2,
                 (win_attr[win_top] >> 4) & 0xf);
    str_format(buf, "%r", fmt, args);
    buf[0x28 - (win_col[win_top] >> 1)] = 0;
    text_print_at(win_row[win_top], 0x1f90 - str_len(buf) * 2,
                  win_attr[win_top], buf);
}

void far win_printf(char far *fmt, ...)                          /* 1df0:0165 */
{
    win_vprint(fmt, (void far *)(&fmt + 1));
}

void far win_text_printf(char far *fmt, ...)                     /* 1df0:017b */
{
    draw_page_set(1);
    win_vprint(fmt, (void far *)(&fmt + 1));
}

void far win_prompt(char far *fmt, ...)                          /* 1df0:019a */
{
    draw_page_set(1);
    win_vprint(fmt, (void far *)(&fmt + 1));
    if (prompt_beep)
        beep();
    if (prompt_icon >= 0)
        pointer_glyph_load(prompt_icon, prompt_frame + 1);
    key_wait();
    if (prompt_icon >= 0)
        pointer_glyph_load(prompt_icon, prompt_frame);
}

char far win_yes_no(char far *fmt, ...)                          /* 1df0:01f7 */
{
    short key;
    int x;
    int y;
    char click;
    char inside;

    draw_page_set(1);
    if (fmt)
        win_vprint(fmt, (void far *)(&fmt + 1));
    beep();
    key_flush();
    for (;;) {
        if (key_poll()) {
            key = key_get();
            key = to_lower(key);
            if (key == 0x1b || key == 0x60 || key == 'n') {
                key = 0;
                break;
            }
            if (key == 'y') {
                key = 1;
                break;
            }
            beep();
        }
        click = mouse_press(7, &x, &y);
        text_xy_to(win_row[win_top], win_col[win_top] + 0x1f40, x, y, &x, &y);
        inside = x >= 0 && x < 4 && y >= 0 && 0xa0 - win_col[win_top] > y;
        if (click) {
            key = inside;
            break;
        }
        if (prompt_icon >= 0)
            pointer_glyph_load(prompt_icon, (inside ? 0 : 1) + prompt_frame);
    }
    if (prompt_icon >= 0)
        pointer_glyph_load(prompt_icon, prompt_frame);
    return key;
}
