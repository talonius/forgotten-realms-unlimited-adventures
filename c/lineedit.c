/* CKIT.EXE overlay 03 module 1c3e: editing a line of text on the prompt line.
 * line_edit prints an optional prompt on the current prompt line, then
 * reads keys into a buffer up to max characters with a cursor that blinks
 * on the timer tick: Enter or a line feed accepts a non-empty line, Esc or
 * ` abandons it, Backspace (or key 0x106) removes the last character, and
 * any other key from ' ' to '~' is appended; everything else beeps, and so
 * does a mouse click.  Text already in the buffer is offered as a default
 * when the byte after its NUL is 1: the first edit key replaces it.
 * num_edit reads a number between lo and hi through line_edit, as wide as
 * hi's digits and one wider when lo is negative, and asks again until one
 * is in range.
 *
 * EXE 0x02cfc7..0x02d435, one Turbo C large-model _TEXT segment starting at
 * byte 7 of paragraph 1c3e, the last module of RTLink section 3 (payload
 * 0x2bff0, link segment 1b41; a root section, the parent of 4 to 10): the
 * first 7 bytes of paragraph 1c3e are module 1bfb's tail, and eleven zero
 * bytes follow to the section's end at 0x2d440.  Built with Turbo C 2.01,
 * tcc -ml -O -Z -d.  Functions are defined in shipped order; bake consumes
 * them in that order.
 *
 * A bare stored seg:off in this file means section 3.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  A name another module
 * gives an address is reused from there; the rest are placeholders chosen
 * from use.  Both routines are reached only through the vector stubs
 * 18de:1085 and 18de:108f, which textwin.c calls line_edit and
 * num_edit.
 *
 * The census marker, 2e 3b 07 at 1c3e:022a, is Turbo C's sparse-switch
 * value search for line_edit's switch on the key.
 *
 * _DATA is five literals stored as four at 969c..96a9, " " on the tail of
 * "%s ".
 * _BSS is empty: the line stack is prompt.c's.
 */

#include <stdlib.h>

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern void far text_offset_xy(int row, int col, int row2, int col2,
                             int far *out_row, int far *out_col); /* 107e:0565 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern int  far mouse_press(char mask, int far *x, int far *y); /* 0f5e:0ee9 */
extern void far beep(void);                                      /* 0da7:018c */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern int  far str_to_num(char far *s);                      /* 18c9:0008 */

extern int win_top;                                              /* 96aa */
extern int win_row[5];                                           /* 96ae */
extern int win_col[5];                                           /* 96b8 */
extern int win_attr[5];                                          /* 96c2 */

char far line_edit(char far *prompt, char far *buf, int max, int col) /* 2c3e:0007 */
{
    long now;
    int row2;
    int mx;
    int col2;
    short key;
    short step;
    char far *end;
    char fg;
    char bg;
    char going;
    char ok;
    char fresh;
    register int n;
    register int x;

    pointer_hide();
    fresh = *buf && buf[str_len(buf) + 1] == 1;  /* a default, replaced by the first edit key */
    end = buf;
    n = 0;
    draw_page_set(1);
    fg = win_attr[win_top] & 0xf;
    bg = (win_attr[win_top] >> 4) & 0xf;
    step = (win_attr[win_top] & 0x200) ? 2 : 4;
    text_offset_xy(win_row[win_top], 0x1fe0 - win_col[win_top], 0x1f44, 0,
                 &row2, &col2);
    if (prompt) {
        fill_rect(win_row[win_top], win_col[win_top] + 0x1f40, row2, col2, bg);
        text_print_at(win_row[win_top], win_col[win_top] + 0x1f40,
                      win_attr[win_top], "%s", prompt);
    }
    x = win_col[win_top] + step + 0x1f40;
    if (prompt)
        x += str_len(prompt) * step;
    else
        x += col;
    if (fresh) {
        text_print_at(win_row[win_top], x, win_attr[win_top], "%s ", buf);
        n = -1;
    } else
        *end = 0;
    going = 1;
    while (going) {
        if (((now = timer_ticks()) & 0x1f) == 0)
            fill_rect(win_row[win_top], n * step + x, row2, n * step + x + step,
                      (now & 0x20) ? fg : bg);
        if (mouse_press(7, &mx, &col2))    /* the mouse y reuses col2's slot, as shipped */
            beep();
        if (!key_poll())
            continue;
        key = key_get();
        switch (key) {
        case 0x1b:
        case 0x60:
            pointer_show();
            going = ok = 0;
            break;
        case '\n':
        case '\r':
            if (fresh) {
                n = str_len(buf);
                fresh = 0;
            }
            if (n == 0) {
                beep();
                break;
            }
            pointer_show();
            going = 0;
            ok = 1;
            break;
        case 8:
        case 0x106:
            if (fresh) {
                text_print_at(win_row[win_top], x - step, win_attr[win_top],
                              "%* ", str_len(buf) + 1);
                *end = 0;
                n = 0;
                fresh = 0;
            }
            if (n > 0) {
                text_print_at(win_row[win_top], n * step + x, win_attr[win_top], " ");
                n--;
                end--;
                *end = 0;
            }
            break;
        default:
            if (key < 0x20 || key > 0x7e || n >= max) {
                beep();
                break;
            }
            if (fresh) {
                text_print_at(win_row[win_top], x - step, win_attr[win_top],
                              "%* ", str_len(buf) + 1);
                n = 0;
                fresh = 0;
            }
            *end = key;
            end++;
            *end = 0;
            n++;
        }
        text_print_at(win_row[win_top], x, win_attr[win_top], "%s ", buf);
    }
    return ok;
}

int far num_edit(int lo, int hi, char far *fmt, int col)         /* 2c3e:03c0 */
{
    char text[80];
    char digits[8];
    register int n;
    register int width;

    width = lo < 0 ? 2 : 1;
    for (n = -abs(hi); n <= -10; n /= 10)
        width++;
    if (fmt)
        str_format(text, "%r", fmt, (void far *)&col);
    for (;;) {
        digits[0] = 0;
        if (!line_edit(fmt ? text : (char far *)0, digits, width, col))
            return lo - 1;
        n = str_to_num(digits);
        if (n >= lo && n <= hi)
            return n;
        beep();
    }
}
