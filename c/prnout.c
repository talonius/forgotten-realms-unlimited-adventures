/* v1.0 build (CKIT.EXE "Version 1.0 March 19,1993"): this object is at EXE
 * 0x07f08a, 754 code bytes; _DATA at a62c, _BSS at e650.  The comment
 * below describes v1.2.  v1.0 differs: prn_eject ends a page with
 * prn_skip(0x42 - line_no) empty lines instead of a form feed; prn_line
 * ends a line with LF only (no CR), copies '\n' and '\f' through, and
 * puts the page number at 0x49 - title length with "%s%( %)Page %02d"
 * (no leading space); prn_center's buffer is 0x58 bytes, not 0x5c.
 *
 * CKIT.EXE overlay section 25, frame 2d91, second object: output to the
 * printer, a line at a time, with pages.  prn_open starts a job (the
 * printer's printer_begin, counters cleared, a header flag and a title
 * kept); prn_line formats a line with str_format's "%r" (a null format
 * prints an empty line), sends it through printer.c's printer_putc while
 * prn_ready says the printer can take it, ends it with CR LF and counts
 * it; after 0x42 lines it counts a new page (no form feed is sent).  When
 * the header flag is set, the first line of every page after the first is
 * preceded by two empty lines, the title and "Page n", and three empty
 * lines.  prn_eject ends a started page with a form feed, prn_close ejects
 * and calls printer_end, prn_center moves a formatted line right by
 * (0x56 - its length) / 2 spaces within 0x50 characters, prn_skip prints
 * n empty lines, and prn_indent prints a line at a given column, ejecting
 * first from line 0x38 on.  prn_ready answers 1 when printer.c's
 * printer_ready does, retrying for 0x2d0 ticks of timer_ticks; then it
 * shows "The printer is not ready!", waits for a key or the printer, and
 * when the printer is still not ready gives up (stop) and answers 0 until
 * the next prn_open.  A line longer than 0x50 characters is reported in
 * four error boxes, which cut it with NULs, so only its first 0x1e
 * characters are printed.
 *
 * Section 25 (parent section 20, which is empty and has no parent; no
 * section names 25 as its parent): payload at EXE 0x07e7c0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 25.  This object is EXE 0x07f1ba..0x07f4d0 (790 bytes, 8
 * routines), 2d91:09fa..0d0f: from the byte after mapprint.c's last retf
 * (2d91:09f9) to the retf before module 2e62 (2e62:0000).  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.
 *
 * Two objects share frame 2d91.  Each Turbo C object has one _DATA and one
 * _BSS block; mapprint.c's lie in the overlay sections' run of DGROUP
 * (9242, d57c), while these literals lie at a686..a6dc, between the
 * resident dialog.c's _DATA (a5f6..a685) and pathfind.c's (a6de), and
 * these statics at e6ae..e6b7, between dialog.c's _BSS (e69c..e6ad) and
 * sfxslots.c's (e6b8): no instruction outside this range names a686..a6dc
 * or e6ae..e6b7.  Section 24's slotsave.c, glyphcut.c, ilbm.c and pcx.c
 * also keep their _DATA among the resident modules'.  Yet this
 * code's 33 relocation sites are filed under 2d91, as mapprint.c's 80
 * are, and mapprint.c's 21 far calls into it store 2d91:09fa..0ce7, so
 * the link put both objects' code in one segment; the bytes do not say
 * how.  Baked as one file, the two give 8 of 17 ranges.
 *
 * Section 25's relocation table files 33 sites in this range under 2d91
 * (EXE 0x07f1c0..0x07f476), all far calls: ten to printer.c (all of that
 * module's callers), three to this object's own prn_eject and prn_skip
 * ahead of their definitions, the rest to the resident code.  No vector
 * lands here.  mapprint.c far-calls every routine but prn_ready (21
 * calls); prn_ready is reached only by near calls from prn_line after its
 * definition, so the bytes do not decide static or public for it.
 *
 * FRUA's own code: the sharing map pairs 7 routines a-only and prn_close
 * (0a22) shape-weak, so every name here is chosen from the code.  The
 * formatting directives "%(", "%)" and "%*" are str_format's and are not
 * examined here.  prn_line's copy loop does not step past a '\n' or '\f':
 * it calls prn_ready again on the same character until that fails, as
 * shipped.
 *
 * _DATA is DGROUP a686..a6dc, seven literals ("%r" at a6b1, a tail of "%*
 * %r" at a6d7, is stored first, so not merged); a6dd is read and written
 * by resident code at EXE 0x003328 and 0x00332c.  _BSS
 * is DGROUP e6ae..e6b7: stop, pages, banner, line_no and doc_title, names
 * chosen with goldbox.bss_order to give the shipped order.
 *
 * Forced by the bytes: printer_putc's int parameter (printer.c's char gives
 * mov al and no cbw: 2 of 8 ranges; unprototyped also bakes 8 of 8);
 * prn_ready's char return (int: 3 of 8) and its "if (!printer_ready())"
 * tail (the if-return-1 order: 7 of 8); "++line_no >= 0x42" (two
 * statements: 4 of 8); "line_no = pages = 0" (reversed: 7 of 8); "--n >=
 * 0" in prn_skip (n-- > 0: 6 of 8).  Not decided by the bytes, each 8 of
 * 8: prn_open's header char or int; register or plain pad; prn_line's copy
 * loop as a for or as a while with continue.
 */

extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern char far key_pressed(void);                               /* 0da7:009a */
extern void far key_flush(void);                                 /* 0da7:00bd */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far win_text_printf(char far *fmt, ...);             /* 0df0:017b */
extern void far screen_page_save(void);                              /* 0f5e:0291 */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far printer_begin(void);                              /* 17d0:0003 */
extern void far printer_putc(int c);                             /* 17d0:0004 */
extern int  far printer_ready(void);                              /* 17d0:0019 */
extern void far printer_end(void);                              /* 17d0:0038 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */

void far prn_eject(void);
void far prn_skip(int n);

static char stop;               /* e6ae  prn_ready gave up                */
static int pages;               /* e6af  pages finished                   */
static char banner;             /* e6b1  print the title on each page     */
static int line_no;             /* e6b2  lines on this page               */
static char far *doc_title;     /* e6b4                                   */

void far prn_open(char header, char far *title)                  /* 3d91:09fa */
{
    printer_begin();
    line_no = pages = 0;
    banner = header;
    doc_title = title;
    stop = 0;
}

void far prn_close(void)                                         /* 3d91:0a22 */
{
    prn_eject();
    printer_end();
}

void far prn_eject(void)                                         /* 3d91:0a2d */
{
    if (line_no)
        prn_skip(0x42 - line_no);
}

char far prn_ready(void)                                         /* 3d91:0a49 */
{
    long t;

    if (stop)
        return 0;
    if (printer_ready())
        return 1;
    t = timer_ticks() + 0x2d0;
    while (timer_ticks() <= t)
        if (printer_ready())
            return 1;
    screen_page_save();
    draw_page_set(1);
    win_text_printf("The printer is not ready!");
    while (!key_pressed() && !printer_ready())
        ;
    key_flush();
    screen_page_show();
    if (!printer_ready()) {
        stop = 1;
        return 0;
    }
    return 1;
}

void far prn_line(char far *fmt, ...)                            /* 3d91:0ae0 */
{
    char buf[0x52];
    char far *p;

    if (!line_no && pages && banner) {
        line_no++;
        prn_skip(2);
        prn_line("%s%( %)Page %02d", doc_title, 0x49 - str_len(doc_title), pages);
        prn_skip(3);
        line_no--;
    }
    if (fmt) {
        str_format(buf, "%r", fmt, (void far *)(&fmt + 1));
        if (str_len(buf) > 0x50) {
            error_box("String to long on print %d", str_len(buf));
            buf[0x1e] = 0;
            error_box("'%s'", buf);
            buf[0x3c] = 0;
            error_box("'%s'", buf + 0x1f);
            error_box("'%s'", buf + 0x3d);
        }
        for (p = buf; *p && prn_ready(); )
            printer_putc(*p++);
    }
    if (prn_ready())
        printer_putc('\n');
    if (++line_no >= 0x42) {
        line_no = 0;
        pages++;
    }
}

void far prn_center(char far *fmt, ...)                          /* 3d91:0c50 */
{
    char buf[0x58];
    int pad;

    str_format(buf, "%r", fmt, (void far *)(&fmt + 1));
    pad = (0x56 - str_len(buf)) / 2;
    mem_move(buf, buf + pad, 0x50 - pad);
    mem_fill(buf, pad, ' ');
    prn_line("%s", buf);
}

void far prn_skip(int n)                                         /* 3d91:0cd1 */
{
    while (--n >= 0)
        prn_line(0);
}

void far prn_indent(int col, char far *fmt, ...)                 /* 3d91:0ce7 */
{
    if (line_no >= 0x38)
        prn_eject();
    prn_line("%* %r", col, fmt, (void far *)(&fmt + 1));
}
