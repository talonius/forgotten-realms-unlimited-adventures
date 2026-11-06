/* CKIT.EXE resident module 2860: formatted output through text_format, to
 * the console and into a string.  con_printf hands each character to the
 * runtime's 00d7:247d, which writes it through DOS (int 21h function 2, a
 * '\n' as CR LF); str_format points str_out at buf,
 * lets str_putc store each character there, ends the string with a NUL and
 * returns its length.  Neither passes text_format a code table.
 *
 * EXE 0x01aa01..0x01aa74, one Turbo C large-model _TEXT segment starting at
 * byte 1 of paragraph 2860.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 2855's last retf is at 0x01aa00, and module 2867 starts at
 * 0x01aa74 (2867:0004) with no stub between.  The one static, str_out, is
 * the module's _BSS at DGROUP e742, the last _BSS variable: DGROUP word
 * 0022, the runtime's initial heap break, holds e746, where it ends.  No
 * other code names it.
 * Nothing far-calls str_putc (0024): str_format passes it as a callback.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call or callback push.  text_format lies inside frame 2867, whose module
 * is not yet written; its prototype is the shape of these calls.
 */

extern int  far text_format(char far *fmt, void far *args,
                             void (far *put)(char), char far *codes, ...); /* 1867:011a */
extern void far con_putc(char c);                            /* 00d7:247d */

static char far *str_out;                                        /* e742 */

void far con_printf(char far *fmt, ...)                          /* 2860:0001 */
{
    text_format(fmt, (void far *)(&fmt + 1), con_putc, 0L);
}

void far str_putc(char c)                                        /* 2860:0024 */
{
    *str_out++ = c;
}

int far str_format(char far *buf, char far *fmt, ...)            /* 2860:0037 */
{
    str_out = buf;
    text_format(fmt, (void far *)(&fmt + 1), str_putc, 0L);
    *str_out = 0;
    return str_out - buf;
}
