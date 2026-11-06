/* CKIT.EXE resident module 1cf0: the packed text pool.
 *
 * EXE 0x00f305 up to 0x00f888, one Turbo C large-model _TEXT segment starting at
 * byte 5 of paragraph 1cf0.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A pool is one far block: a 6-byte header (capacity, a word set to -1, bytes
 * used), a length byte for each of POOL_SLOTS slots, and the text from
 * POOL_TEXT on.  Text is stored six bits a character, four characters in three
 * bytes, 0x60..0x7f folded down by 0x20.  A slot's length byte counts packed bytes;
 * 0 marks a free slot, and txt_offset skips a length of 0xff.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  Callee names are placeholders; a modNNNN_ name says only which
 * segment the callee lives in.
 *
 * _BSS follows the statics' names (Turbo C's symbol-table order), not their
 * declaration order; these names reproduce the shipped ad94..ada5 layout.
 */

enum {
    POOL_HDR   = 6,                     /* header bytes before the length table */
    POOL_SLOTS = 0x190,                 /* length-table entries */
    POOL_TEXT  = POOL_HDR + POOL_SLOTS  /* 0x196, where the text starts */
};

struct PoolHdr {
    int cap;                            /* +0 text bytes available */
    int mark;                           /* +2 set to -1 by txt_init; nothing here tests it */
    int used;                           /* +4 text bytes in use */
};

extern int  far str_len(char far *s);                          /* 00d7:2577 */
extern void far mem_move(void far *src, void far *dst, int n);  /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);            /* 00d7:277b */

static char txt_dirty = 0;                                          /* 54de, only ever set to 0; never read */
static char far *txt_pool = 0;                                      /* 54df */

static char far *txt_data;                                          /* ad94 */
static unsigned char far *txt_lens;                                 /* ad98 */
static char txt_unused[6];                                          /* ad9c, no reference found */
static struct PoolHdr far *txt_header;                              /* ada2 */

int far txt_offset(int slot)                                        /* 1cf0:0005 */
{
    register int i;
    register int ofs;

    for (i = ofs = 0; i < slot; i++)
        if (txt_lens[i] != 0xff)
            ofs = ofs + txt_lens[i];
    return ofs;
}

char far txt_char6(char far *s, int i)                              /* 1cf0:002f */
{
    char c;

    c = s[i];
    if (c > 0x5f)
        c = c - 0x20;
    c = c & 0x3f;
    return c;
}

void far txt_select(char far *pool)                                 /* 1cf0:005e */
{
    if (txt_pool != pool) {
        txt_dirty = 0;
        txt_header = (struct PoolHdr far *)(txt_pool = pool);
        txt_lens = (unsigned char far *)txt_pool + POOL_HDR;
        txt_data = txt_pool + POOL_TEXT;
    }
}

void far txt_pack(char far *src, char far *dst)                     /* 1cf0:00ab */
{
    char done;
    char c;
    register int i;
    register int n;

    c = 0;
    n = i = 0;
    done = 0;
    do {
        dst[n] = txt_char6(src, i) << 2;
        if (src[i] == 0)
            done = 1;
        else {
            i++;
            c = txt_char6(src, i) << 2;
            c = (c & 0xff) >> 6;
            dst[n] += c & 0xff;
            n++;
            dst[n] = (txt_char6(src, i) & 0xff) << 4;
            if (src[i] == 0)
                done = 1;
            else {
                i++;
                c = txt_char6(src, i);
                dst[n] += ((c << 2) & 0xff) >> 4;
                n++;
                dst[n] = txt_char6(src, i) << 6;
                if (src[i] == 0)
                    done = 1;
                else {
                    i++;
                    dst[n] += ((txt_char6(src, i) << 2) & 0xff) >> 2 & 0xff;
                    n++;
                    if (src[i] == 0)
                        done = 1;
                    else
                        i++;
                }
            }
        }
    } while (!done && n < 0xff);
}

void far txt_unpack(char far *src, char far *dst, int n)            /* 1cf0:01c2 */
{
    char cur;
    char prev;
    char out;
    short ofs;
    register int phase;
    register int j;

    out = 0;
    j = 0;
    phase = 1;
    ofs = -1;
    cur = 0;
    prev = 0;
    for (; j < n; j = j + 1) {
        if (phase < 4) {
            ofs = ofs + 1;
            prev = cur;
            cur = src[ofs];
        }
        switch (phase) {
        case 1:
            out = ((cur & 0xff) >> 2) & 0x3f;
            break;
        case 2:
            out = (((cur & 0xff) >> 4) + (prev << 4)) & 0x3f;
            break;
        case 3:
            out = (((cur & 0xff) >> 6) + (prev << 2)) & 0x3f;
            break;
        case 4:
            out = cur & 0x3f;
        }
        if (phase < 4)
            phase = phase + 1;
        else
            phase = 1;
        if (out != 0 && out <= 0x1f)
            out = out + 0x40;
        *dst = out;
        dst = dst + 1;
    }
    *dst = 0;
}

char far txt_init(char far *pool, int size)                         /* 1cf0:02b4 */
{
    char ok;
    register int i;

    ok = 1;
    txt_header = (struct PoolHdr far *)pool;
    txt_lens = (unsigned char far *)pool + POOL_HDR;
    txt_data = pool + POOL_TEXT;
    mem_fill(txt_data, size - POOL_TEXT, 0);
    txt_header->cap = 0;
    txt_header->mark = -1;
    txt_header->used = 0;
    for (i = 0; i < POOL_SLOTS; i++)
        txt_lens[i] = 0;
    txt_header->cap = size - POOL_TEXT;
    return ok;
}

void far txt_touch(char far *pool)                                  /* 1cf0:0338 */
{
    txt_select(pool);
    txt_header->cap = txt_header->cap;                              /* each field read and stored back */
    txt_header->mark = txt_header->mark;
    txt_header->used = txt_header->used;
}

int far txt_add(char far *pool, char far *s)                        /* 1cf0:0363 */
{
    short k;
    short rc;
    short len;
    short size;
    register int slot;
    register int ofs;

    ofs = 0;
    txt_select(pool);
    len = str_len(s) + 1;
    slot = -3;
    for (k = 0; k < POOL_SLOTS; k++)
        if (txt_lens[k] == 0) {
            slot = k;
            break;
        }
    size = (len * 3 + 3) / 4;
    if (slot == -3)
        rc = -3;
    else if (len > 0xff || txt_header->used + size > txt_header->cap)
        rc = -2;
    else if (len == 0)
        rc = -1;
    else {
        ofs = txt_offset(slot);
        mem_move(txt_data + ofs, txt_data + ofs + size, txt_header->used - ofs);
        txt_pack(s, txt_data + ofs);
        txt_lens[slot] = size;
        txt_header->used = txt_header->used + size;
        rc = slot;
        txt_dirty = 0;
    }
    return rc;
}

void far txt_get(char far *pool, register int slot, char far *out)  /* 1cf0:046e */
{
    txt_select(pool);
    if (slot < 0 || slot >= POOL_SLOTS)
        *out = 0;
    else
        txt_unpack(txt_data + txt_offset(slot), out, txt_lens[slot] * 4 / 3);
}

void far txt_free(char far *pool, register int slot)                /* 1cf0:04ca */
{
    register int size;

    txt_select(pool);
    if (slot >= 0 && slot < POOL_SLOTS) {
        size = txt_lens[slot];
        mem_move(txt_data + txt_offset(slot) + size, txt_data + txt_offset(slot),
                     txt_header->used - (txt_offset(slot) + size));
        txt_lens[slot] = 0;
        txt_header->used -= size;
    }
}

int far txt_usage(char far *pool, int far *cap)                     /* 1cf0:0548 */
{
    register int used;

    txt_select(pool);
    used = txt_header->used * 4 / 3;
    *cap = txt_header->cap * 4 / 3;
    return used;
}
