/* CKIT.EXE resident module 2793: the C side of start-up.  The start-up code
 * at 1000:0000 copies the command tail onto the stack, puts the program's
 * path and a space ahead of it under DOS 3 or later ("C " under DOS 2; DOS
 * 1 exits), zeroes DGROUP a7e4..e749, and far-calls start_main with that
 * string (1000:00b9).  start_main splits it into words at the heap break, a
 * word being a quoted run or a run of characters above ' ' (a signed
 * compare, so a byte of 0x80 or more ends a word), cuts each with a NUL,
 * ends the table with a null pointer, moves the heap break past the table,
 * and calls 28de:13d7 with the word count and the table.  28de:13d7 is an
 * RTLink vector stub (call near; jmp far into overlay section 3), and argc,
 * argv is the shape a main takes.
 *
 * EXE 0x019d3b..0x019e19, one Turbo C large-model _TEXT segment starting at
 * byte 0xb of paragraph 2793.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Module 278d's last routine ends at 0x019d3a (its retf); the routines at
 * 2793:0002 and 2793:0008 before it are 278d's, far-called only as
 * 178d:0062 and 178d:0068.  Module 27a1 starts at 0x019e19 (27a1:0009) with
 * no stub between.  Its one far caller, at 0x0024b9, stores 1793:000b, and
 * both MZ relocation sites inside it have frame 1793.
 *
 * The seg:off beside the extern function is the far pointer as stored in
 * the shipped call; heap_brk's is its DGROUP offset.  The callee's name is
 * a placeholder.
 *
 * This module has no _DATA or _BSS.  heap_brk (0022) is the runtime's heap
 * break, initialized to DGROUP:e746; 10d7:2669 allocates from its offset
 * word.
 */

extern int  far game_main(int argc, char far *far *argv);     /* 18de:13d7 */

extern char far *far *heap_brk;                                  /* 0022 */

void far start_main(char far *s)                                 /* 2793:000b */
{
    char far *far *p;
    char far *far *argv;

    p = heap_brk;
    while (*s) {
        if (*s == '"') {
            s++;
            *p = s;
            p++;
            while (*s) {
                if (*s == '"')
                    goto cut;
                s++;
            }
        } else if (*s > ' ') {
            *p = s;
            p++;
            do
                s++;
            while (*s > ' ');
        } else {
cut:
            *s = 0;
            s++;
        }
    }
    *p = 0;
    argv = heap_brk;
    heap_brk = p + 1;
    game_main(p - argv, argv);
}
