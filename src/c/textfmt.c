/* CKIT.EXE resident module 2867: text_format, the formatter, and the three
 * helpers it calls near.  text_format hands each character of fmt to put; a
 * '%' escape reads an optional decimal count (a leading 0 pads numbers with
 * '0') and a letter, with v, the int at the argument pointer, as its operand.
 * d u x X o print v, ld lu lx lo a long, s a string, w the two bytes of v; c
 * puts v's low byte, or with a count above 1 count - 2 NULs and then v's high
 * and low bytes; r formats another string with its own arguments; z puts count
 * NULs, and any other letter is put count times.  g moves the argument pointer
 * to the argument its number names; ( ) repeat the text between them v times,
 * and [ ; : ] keep alternative v (from 0) of those between them, a %: starting
 * the fallback.  + - * / # \ & | ^ < > combine v with the count, ~ ignores
 * it, and = sets v to the number given (0 when there is none); each reads the
 * next escape at once.  A * with no number is printf's width: v becomes the
 * count, the next int becomes v, and the next letter is dispatched at once.
 * The k-th letter of codes calls instead the k-th far handler passed after
 * codes.  Its four far callers (1867:011a at 0x00cf6b, 0x010200, 0x01aa1b and
 * 0x01aa59) are strutil.c, sysutil.c and fmtout.c; sysutil.c's is the one
 * that passes a code table.
 *
 * EXE 0x01aa74..0x01b098, one Turbo C large-model _TEXT segment starting at
 * byte 4 of paragraph 2867.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * fmtout.c's last retf is at 0x01aa73.  The routine at 0x01b098 is another
 * module, 28c9: its switch's value table is addressed as 28c9:0046, and it
 * is far-called as 18c9:0008.  The module names no DGROUP address, so it has
 * no _DATA and no _BSS.  The code is DQK's textfmt.c token for token, with
 * the formatter under the name the landed files call it; only the placement
 * and the runtime's addresses differ.
 *
 * The switch on the escape letter (2e 3b 07 at 2867:01fc) and the one on the
 * letter after %l (at 2867:0390) are Turbo C's sparse-switch value search
 * through the code segment, with their value and jump tables at 2867:020c and
 * 2867:039f; a CB byte in the first jump table (2867:0254) is data, not a
 * retf.  The long division and remainder in put_ulong call the runtime's
 * LUDIV@ (00d7:2a4b) and LUMOD@ (00d7:2a55).  put_uint and put_ulong are
 * defined old-style, so their callers promote the char zero with cbw;
 * code_index's prototyped char c is pushed as a word.
 */

union argp {
    int       far *i;
    long      far *l;
    char far *far *s;
    void far *far *p;
};

typedef void (far *CodeFn)(int v, int n, void (far *put)(int),
                           union argp far *ap, char far *far *fmt);

static int near pascal code_index(char far *codes, char c)       /* 2867:0004 */
{
    register int i;

    for (i = 1; *codes; i++)
        if (*codes++ == c)
            return i;
    return 0;
}

static void near pascal put_uint(value, width, zero, radix, put) /* 2867:0030 */
register unsigned value;
short width;
char zero;
register unsigned radix;
void (far *put)(int);
{
    if (value >= radix)
        put_uint(value / radix, width - 1, zero, radix, put);
    else
        while (--width > 0)
            put(zero ? '0' : ' ');
    value = value % radix;
    put((value > 9 ? '7' : '0') + value);
}

static void near pascal put_ulong(value, width, zero, radix, put) /* 2867:0096 */
unsigned long value;
short width;
char zero;
register unsigned radix;
void (far *put)(int);
{
    if (radix <= value)
        put_ulong(value / radix, width - 1, zero, radix, put);
    else
        while (--width > 0)
            put(zero ? '0' : ' ');
    radix = value % radix;
    put((radix > 9 ? '7' : '0') + radix);
}

void far text_format(char far *fmt, void far *args, void (far *put)(int), char far *codes, ...) /* 2867:011a */
{
    char c;
    char zero;
    char far *s;
    char far *loop_start;
    short k;                            /* handler index, then radix */
    union argp ap;
    short count;
    short loops;
    int far *mark;
    long far *lp;
    register int v;
    register int n;

    ap.i = args;
    loops = 0;
    while ((c = *fmt++) != 0) {
        if (c == '%') {
            v = *ap.i;
again:
            c = *fmt++;
            if (c >= '0' && c <= '9') {
                zero = c == '0';
                n = c - '0';
                while ((c = *fmt++) >= '0' && c <= '9')
                    n = n * 10 + c - '0';
                count = n;
            } else {
                zero = n = 0;
                count = 1;
            }
            if (codes && (k = code_index(codes, c)) != 0) {
                ((CodeFn far *)&codes)[k](v, n, put, &ap, &fmt);
                continue;
            }
dispatch:
            switch (c) {
            case 'd':
                if (v < 0) {
                    put('-');
                    n--;
                    v = -v;
                }
            case 'u':
                put_uint(v, n, zero, 10, put);
                ap.i++;
                break;
            case 's':
                s = *ap.s++;
                if (s)
                    while (*s) {
                        put(*s++);
                        n--;
                    }
                while (n-- > 0)
                    put(' ');
                break;
            case 'c':
                if (n > 1) {
                    while (n > 2) {
                        put(0);
                        n--;
                    }
                    put(v >> 8);
                }
                put(v);
                ap.i++;
                break;
            case 'w':
                while (n > 2) {
                    put(0);
                    n--;
                }
                put(v);
                put(v >> 8);
                ap.i++;
                break;
            case 'X':
            case 'x':
                put_uint(v, n, zero, 16, put);
                ap.i++;
                break;
            case 'o':
                put_uint(v, n, zero, 8, put);
                ap.i++;
                break;
            case 'l':
                lp = ap.l;
                k = 11;
                switch (*fmt++) {
                case 'd':
                    break;
                case 'u':
                    k = 10;
                    break;
                case 'x':
                    k = 16;
                    break;
                case 'o':
                    k = 8;
                    break;
                default:
                    fmt--;
                }
                if ((k & 1) && *lp < 0) {
                    put('-');
                    n--;
                    *lp = -*lp;
                }
                put_ulong(*lp++, n, zero, k & ~1, put);
                ap.l = lp;
                break;
            case 'r':
                s = *ap.s++;
                text_format(s, *ap.p++, put, codes, (&codes)[1]); /* first handler only */
                break;
            case '+':
                v += count;
                goto again;
            case '>':
                v >>= count;
                goto again;
            case '<':
                v <<= count;
                goto again;
            case '(':
                loops = v;
                loop_start = fmt;
                ap.i++;
                if (v)
                    break;
                while (*fmt && (*fmt++ != '%' || *fmt != ')'))
                    ;
                if (*fmt)
                    fmt++;
                break;
            case ')':
                if (--loops > 0)
                    fmt = loop_start;
                break;
            case '[':
                mark = ap.i + 1;
                while (*fmt && v > 0)
                    if (*fmt++ == '%')
                        switch (*fmt++) {
                        case ':':
                            goto next;
                        case ';':
                            v--;
                            break;
                        case ']':
close:
                            ap.i = mark;
                            goto next;
                        }
                break;
            case ']':
                goto close;
            case ':':
            case ';':
                while (*fmt && (*fmt++ != '%' || *fmt != ']'))
                    ;
                if (*fmt)
                    fmt++;
                goto close;
            case 'g':
                ap.i = (int far *)args + n;
                break;
            case '&':
                v &= count;
                goto again;
            case '*':
                if (n == 0) {
                    n = v;
                    count = n;
                    v = *++ap.i;
                    c = *fmt++;
                    goto dispatch;
                }
                v *= count;
                goto again;
            case '-':
                v -= count;
                goto again;
            case '^':
                v ^= count;
                goto again;
            case '/':
                v /= count;
                goto again;
            case '#':
                v %= count;
                goto again;
            case '|':
                v |= count;
                goto again;
            case '~':
                v = ~v;
                goto again;
            case '\\':
                v = (unsigned)v / count;
                goto again;
            case '=':
                v = n;
                goto again;
            case 'z':
                c = 0;
                goto repeat;
            case 0:
                fmt--;
                c = '%';
            default:
repeat:
                while (--count >= 0)
                    put(c);
            }
        } else
            put(c);
next:   ;
    }
}
