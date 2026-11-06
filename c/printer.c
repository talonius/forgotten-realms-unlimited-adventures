/* CKIT.EXE resident module 27d0: the printer.  printer_putc writes one
 * character to it through int 21h function 05h, and printer_ready returns 1
 * when int 21h ax=4407h (IOCTL output status) on handle 4, stdprn, gives a
 * positive result through 00d7:2697, 0 otherwise.  printer_begin and
 * printer_end are empty.
 *
 * EXE 0x01a103..0x01a139, one Turbo C large-model _TEXT segment starting at
 * byte 3 of paragraph 27d0.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 27a1 ends with the retf at 0x01a102; module 27d3 starts at
 * 0x01a139 (27d3:0009).  Both MZ relocation sites inside the range are the
 * far calls to 00d7:2697, with frame 17d0.
 *
 * Every routine is far-called only from overlay section 25, ten sites in
 * all: 0003 at 0x7f1bd, 0038 at 0x7f1e7, 0004 at four sites and 0019 at
 * four.  Those callers test all of ax after 0019.  The module has no _DATA
 * and no _BSS.
 *
 * The seg:off beside the extern is the far pointer as stored in the shipped
 * call; its placeholder name is the one the landed modules give it.
 */

extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */

void far printer_begin(void)                                      /* 27d0:0003 */
{
}

void far printer_putc(char c)                                    /* 27d0:0004 */
{
    dos_call(0x0500, c);
}

int far printer_ready(void)                                       /* 27d0:0019 */
{
    return dos_call(0x4407, 0, 4) > 0;
}

void far printer_end(void)                                      /* 27d0:0038 */
{
}
