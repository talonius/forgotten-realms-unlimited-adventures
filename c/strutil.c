/* CKIT.EXE resident module 1a75: string, number and message helpers.
 * A 1-based substr, insert and delete, a 0-based str_index, a right-to-left
 * number parser, a slot pool with one bit per slot, the message table, and a
 * tick-based delay.
 *
 * EXE 0x00cb5d..0x00d02d, one Turbo C large-model _TEXT segment starting at
 * byte d of paragraph 1a75.  Built with Turbo C 2.01, tcc -ml -O -Z -d: -d
 * merges duplicate strings, which is what puts msg_text's "" on the NUL of
 * "%d" (DGROUP 0x1ce4 = 0x1ce2 + 2).  Functions are defined in shipped order;
 * bake consumes them in that order.  Module 183a's last routine ends at
 * 0x00cb5c: 0x00cb59 is far-called as 083a:23b9, never as 0a75:0009.  Module
 * 1ac2 starts at 0x00d02d (1ac2:000d) with no stub between.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module here gives an address is reused from there;
 * screen_flush and input_poll are DQK's definitions.  A modNNNN_ name is a
 * placeholder that says only which segment the callee lives in.
 *
 * The statics' names fix their DGROUP order: Turbo C 2.01 emits _BSS in its
 * symbol-table order, which follows the name and not the declaration order.
 * These names reproduce the shipped 0xad06 num, 0xad08 ptr, 0xad0c table,
 * 0xad10 buffer; the originals are not recoverable from it.
 */

extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern char far str_match(char far *s, char far *at);            /* 1855:006a */
extern char far *far str_find(char far *s, char c);          /* 00d7:258e */
extern int  far text_format(char far *fmt, void far *args,
                             void (far *put)(char), char far *codes, ...); /* 1867:011a */
extern int  far random_int(int n);                               /* 0da7:022d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern void far input_poll(void);                                /* 1157:05c0 */

void far int_to_str(int value, char far *buf)                    /* 1a75:000d */
{
    str_format(buf, "%d", value);
}

static char string_buf[82];              /* ad10; sound.c _BSS starts ad62 */

char far *far substr(char far *s, int start, int len)            /* 1a75:0027 */
{
    mem_move(s + start - 1, string_buf, len);
    string_buf[len] = 0;
    return string_buf;
}

int far str_length(char far *s)                                  /* 1a75:0054 */
{
    return str_len(s);
}

int far str_index(char far *s, char far *t)                      /* 1a75:0066 */
{
    register int i;

    for (i = 0; *t; t++, i++)
        if (str_match(s, t))
            return i;
    return 0;
}

void far str_to_long(char far *s, long far *out, int far *err)   /* 1a75:009c */
{
    long digit;
    unsigned short place;
    unsigned short k;
    char far *p;

    *out = 0;
    digit = place = 0;
    for (p = str_find(s, 0) - 1; p >= s; place++, p--) {
        if (*p >= '0' && *p <= '9') {
            digit = *p - '0';
            for (k = place; k > 0; k--)
                digit *= 10;
            *out += digit;
        } else {
            *err = str_len(s) - place - 1;
            return;
        }
    }
    *err = 0;
}

void far str_insert(char far *s, char far *t, int pos)           /* 1a75:0164 */
{
    register int len;
    char far *at;

    len = str_len(s);
    at = t + pos - 1;
    mem_move(at, at + len, str_len(at) + 1);
    mem_move(s, at, len);
}

void far str_delete(char far *s, register int pos, int n)        /* 1a75:01c4 */
{
    mem_move(s + pos + n - 1, s + pos - 1, str_len(s + pos + n - 1) + 1);
}

struct Pool {
    int capacity;               /* +0  slots in the pool                */
    int stride;                 /* +2  bytes per slot                   */
    char far *base;             /* +4  backing storage                  */
    unsigned char bitmap[1];    /* +8  one bit per slot, 1 = occupied   */
};

void far pool_alloc(void far * far *out, int unused, struct Pool far *pool) /* 1a75:0203 */
{
    unsigned char bit;
    unsigned char row;
    unsigned char byte;
    register int slot;

    for (row = 0; (byte = pool->bitmap[row]) == 0xff && row < pool->capacity - 1; row++)
        ;
    for (bit = 0; bit < 8 && (byte & (1 << bit)); bit++)
        ;
    slot = row * 8 + bit;
    if (pool->capacity <= slot) {
        *out = 0;
    } else {
        pool->bitmap[row] |= 1 << bit;
        *out = pool->base + pool->stride * slot;
    }
}

void far pool_free(void far *ptr, int unused, struct Pool far *pool) /* 1a75:02c8 */
{
    unsigned char lo;
    unsigned char hi;
    register int slot;

    slot = ((char far *)ptr - pool->base) / pool->stride;
    if (slot >= 0 && pool->capacity > slot) {
        hi = slot >> 3;
        lo = slot - hi * 8;
        pool->bitmap[hi] &= ~(1 << lo);
    }
}

char far pool_lowbit(int value)                                  /* 1a75:032f */
{
    return value & 1;
}

void far mem_move5(void far *src, void far *dst, unsigned n)     /* 1a75:0339 */
{
    mem_move(src, dst, n);
}

int far random_n(int a)                                          /* 1a75:0354 */
{
    return random_int(a);
}

void far screen_sync(void)                                       /* 1a75:0362 */
{
    screen_flush();
    key_poll();
}

unsigned char far hi_byte(int w)                                 /* 1a75:036d */
{
    return (w >> 8) & 0xff;
}

unsigned char far lo_byte(int w)                                 /* 1a75:037b */
{
    return w & 0xff;
}

static int msg_num;                     /* ad06 */
static char far *msg_ptr;               /* ad08 */
static char far * far *msg_table;       /* ad0c */

void far msg_set_table(int count, char far * far *table)         /* 1a75:0385 */
{
    msg_num = count;
    msg_table = table;
}

char far *far msg_text(register int i)                           /* 1a75:039b */
{
    if (msg_table && i >= 0 && i < msg_num && msg_table[i])
        return msg_table[i];
    return "";
}

void far msg_putc(char c)                                        /* 1a75:03e7 */
{
    *msg_ptr++ = c;
}

char far *far msg_format(char far *fmt, ...)                     /* 1a75:03fa */
{
    msg_ptr = string_buf;
    text_format(fmt, (void far *)(&fmt + 1), msg_putc, 0L);
    *msg_ptr = 0;
    return string_buf;
}

void far delay(unsigned ms)                                      /* 1a75:0431 */
{
    long until;
    register int rem;

    if (ms == 0)
        return;
    screen_flush();
    until = ms;
    until *= 72;
    rem = until % 1000;
    until /= 1000;
    if (random_int(1000) < rem)
        until++;
    until += timer_ticks();
    while (timer_ticks() < until) {
        if (is_xvga() || !pixel_shift())
            input_poll();
    }
}
