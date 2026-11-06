/* v1.0 BUILD (CKIT.EXE 1.0, March 19 1993), file offset 0x1b0fa (28cf:000a),
 * 319 code bytes, the same source as v1.2; only its frame moved, so the
 * switch table's paragraph-relative operands differ.  The addresses below
 * are v1.2's.
 *
 * CKIT.EXE resident module 28c9: str_to_num, reading a number whose radix
 * is named by a letter after its digits.  A trailing B, O, D or H (either
 * case) selects binary, octal, decimal or hexadecimal and is not read as a
 * digit; with any other last character the whole string is decimal.  Digits
 * valid in the radix accumulate; a '-' met before the number ends negates
 * the result (a second does not cancel it); other characters up to ' ' are
 * skipped; a digit the radix does not allow, any other character above ' '
 * and any byte from 0x80 end the number.
 *
 * EXE 0x01b098..0x01b1d7, one Turbo C large-model _TEXT segment starting at
 * byte 8 of paragraph 28c9: textfmt.c's last retf is the byte before, and
 * nine zero bytes after the range pad to the RTLink manager at 0x01b1e0
 * (28de:0000).  Built with Turbo C 2.01, tcc -ml -O -Z -d.  The switch's
 * value table sits at 28c9:0046 and is addressed from the paragraph, which
 * is why the routine is an object of its own.  Its one far caller is overlay
 * section 3 (18c9:0008 at 0x02d411, a section relocation).  The module names
 * no DGROUP address, so it has no _DATA and no _BSS.  The code is DQK's
 * numparse.c token for token.
 *
 * The switch on the radix letter is Turbo C's sparse-switch value search
 * through the code segment (2e 3b 07 at 28c9:0037).
 */

int far str_to_num(char far *s)                                 /* 28c9:0008 */
{
    char c;
    char far *p;
    char far *end;
    char neg;
    register unsigned radix;
    register unsigned n;

    for (end = s; *end; end++)
        ;
    end--;
    switch (*end) {
    case 'H': case 'h':
        radix = 16;
        break;
    case 'O': case 'o':
        radix = 8;
        break;
    case 'D': case 'd':
        radix = 10;
        break;
    case 'B': case 'b':
        radix = 2;
        break;
    default:
        radix = 10;
        end++;
    }
    n = 0;
    neg = 0;
    for (p = s; p < end; ) {
        c = *p++;
        if (c >= '0' && c <= '9' && c < radix + '0')
            n = radix * n + c - '0';
        else if (c >= 'A' && c < radix + 'A' - 10)
            n = radix * n + c - ('A' - 10);
        else if (c >= 'a' && c < radix + 'a' - 10)
            n = radix * n + c - ('a' - 10);
        else if (c == '-')
            neg = 1;
        else if ((unsigned char)c > ' ')
            break;
    }
    return neg ? -n : n;
}
