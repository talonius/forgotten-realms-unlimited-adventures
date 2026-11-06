/* CKIT.EXE resident module 2855: scanning a string against a character set,
 * and a case-blind prefix test.  str_break steps over the characters of s
 * that are not in set and str_skip over those that are, each returning where
 * it stopped (the NUL when it runs off the end); set membership is
 * str_find's, whose result points at set's NUL when the character is not
 * there.  str_match says whether the string at begins with s, comparing
 * each pair lowered by to_lower.
 *
 * EXE 0x01a952..0x01aa01, one Turbo C large-model _TEXT segment starting at
 * byte 2 of paragraph 2855.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 281e's last retf is at 0x01a951, and module 2860 starts at
 * 0x01aa01 (2860:0001) with no stub between.  The module names no DGROUP
 * address, so it has no _DATA and no _BSS.  Nothing found calls str_break
 * (0002) or str_skip (0036): no far call, far jump, callback push or
 * far-pointer dword in CKIT.EXE names them.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  str_match returns a char, set in al alone; both shipped callers
 * (0x00cbcb, 0x032923) test al alone.
 */

extern char far *far str_find(char far *s, char c);          /* 00d7:258e */
extern int  far to_lower(int c);                             /* 00d7:25ab */

char far *far str_break(char far *s, char far *set)              /* 2855:0002 */
{
    while (*s && !*str_find(set, *s))
        s++;
    return s;
}

char far *far str_skip(char far *s, char far *set)               /* 2855:0036 */
{
    while (*s && *str_find(set, *s))
        s++;
    return s;
}

char far str_match(char far *s, char far *at)                    /* 2855:006a */
{
    for (; *s; s++, at++)
        if (!*at || to_lower(*s) != to_lower(*at))
            return 0;
    return 1;
}
