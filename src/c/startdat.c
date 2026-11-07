/* CKIT.EXE data-only module, first in DGROUP: the start-up's data, DGROUP
 * 0000..002b.  startup.asm stores the PSP's segment in _psp and its stack
 * pointer in stack_top's low word; heap_brk is the runtime's heap break, which
 * start_main (cmdargs.c) takes argv from and 10d7:2669 allocates from, set to
 * bss_end (e746), where the C modules' _BSS ends (fmtout.c's str_out is the last
 * variable).  joy_last_ail is lowlevel.asm's.  Dark Queen of Krynn's startdat.c,
 * the same five variables in the same 44 bytes.
 *
 * No routines and no literals; its empty _TEXT sits at startup.asm's start.
 */

extern char far *bss_end[];                     /* e746 */

char null_area[32] = { 0 };                     /* 0000  zeros a stray null write lands on */
unsigned _psp = 0;                              /* 0020  the PSP's segment */
char far *far *heap_brk = bss_end;              /* 0022 */
void far *stack_top = null_area;                /* 0026  startup.asm stores SP in its low word */
int joy_last_ail = 0;                           /* 002a */
