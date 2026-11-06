/* CKIT.EXE overlay 03 module 1bfb: saving and loading the pool.  Emptying
 * the pool, its coins zeroed and its items freed; writing the pooled coins
 * and items to a file and reading them back; and the pool's record count,
 * which pool_load sets and section 10 lowers as items leave.
 * An item whose type is 'I' carries count more items on its sub chain,
 * and each is written as its own record; pool_save pads the records out
 * to 50 from item_templates.
 *
 * EXE 0x02cb92..0x02cfc7, one Turbo C large-model _TEXT segment starting at
 * byte 2 of paragraph 1bfb in RTLink section 3 (payload 0x2bff0, link
 * segment 1b41; a root section, the parent of 4 to 10).  The first 2 bytes
 * of paragraph 1bfb are the tail of module 1bbc, and the last 7 bytes of
 * the range lie in paragraph 1c3e, whose module starts at 1c3e:0007.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 3.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use.  Three routines are also reached
 * through the vector stubs 18de:1477 (pool_clear), 1481 (pool_records)
 * and 148b (pool_set_records); section 4 far-calls pool_clear, and section
 * 10 far-calls pool_clear, pool_set_records and pool_records.  Section
 * 4's module 1ffa passes pool_load (0x32f70) and pool_save (0x32faf) as
 * file callbacks.
 *
 * This is DQK's poolfile.c relinked, with FRUA's 0x3e-byte item and
 * 0x12-byte record; pool_save keeps each record write's count in got.
 *
 * _DATA is empty.  _BSS is records at b252.
 *
 * In v1.0 the module starts 5 bytes earlier, at 2c01:0007 (EXE 0x02cbf7),
 * with poolfile_argument_noop; v1.2 dropped it.  Ghidra finds it only as
 * a scan candidate, and nothing in the recovered listing calls it.  Its
 * frame says it took arguments (a no-argument empty is a lone retf); how
 * many is not in the bytes, so it takes one.
 */

struct ItemRec {                /* an item as a file holds it, 0x12 bytes */
    unsigned char type;         /* +0  'I' carries count more items       */
    char pad_1[0x0c];           /* +1  not read here                      */
    unsigned char count;        /* +d                                     */
    char pad_e[4];              /* +e  not read here                      */
};

struct Item {                   /* an item, a item_record_pool record            */
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4  not read here                      */
    struct ItemRec rec;         /* +28                                    */
    struct Item far *more;      /* +3a                                    */
};

struct ItemFileHdr {            /* what precedes the item records         */
    int tag;                    /* +0  written as -1                      */
    unsigned count;             /* +2  items on pool_items                */
};

struct Pool;                    /* strutil.c's; only its address is passed */

extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */

extern char tick_slot;                                           /* b3f2 */
extern long pool[3];                                             /* b8d6 */
extern struct Item far *pool_items;                              /* b8e2 */
extern struct Item far *cur_item;                                /* bcfd */
extern char treasure_has_items;                                           /* ca15 */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char far *item_templates;                                       /* d122 */

static int records;                                              /* b252 */

void far poolfile_argument_noop(int unused)                      /* 2c01:0007 */
{
}

void far pool_clear(void)                                        /* 2bfb:0002 */
{
    struct Item far *sub;
    struct Item far *p;

    mem_zero(pool, sizeof pool);
    for (cur_item = pool_items; cur_item != 0; ) {
        if (cur_item->rec.type == 'I') {
            sub = cur_item->more;
            for (tick_slot = 1; tick_slot <= cur_item->rec.count; tick_slot++)
                if (sub != 0) {
                    p = sub;
                    sub = sub->more;
                    pool_free(p, sizeof(struct Item), &item_record_pool);
                }
        }
        p = cur_item;
        cur_item = cur_item->next;
        pool_free(p, sizeof(struct Item), &item_record_pool);
    }
    pool_items = 0;
}

void far pool_set_records(int n)                                  /* 2bfb:00d0 */
{
    records = n;
}

int far pool_records(void)                                    /* 2bfb:00db */
{
    return records;
}

char far pool_save(int fd)                                       /* 2bfb:00df */
{
    struct Item far *p;
    struct Item far *sub;
    int got;
    struct ItemFileHdr hdr;
    register int n;

    if (dos_io(0x4000, pool, fd, sizeof pool) != sizeof pool)
        return 0;
    hdr.count = 0;
    for (p = pool_items; p != 0; p = p->next)
        hdr.count++;
    hdr.tag = -1;
    n = 0;
    if (dos_io(0x4000, &hdr, fd, sizeof hdr) != sizeof hdr)
        return 0;
    for (p = pool_items; p != 0; p = p->next) {
        if ((got = dos_io(0x4000, &p->rec, fd, sizeof(struct ItemRec)))
            != sizeof(struct ItemRec))
            return 0;
        n++;
        if (p->rec.type == 'I') {
            sub = p;
            for (tick_slot = 1; tick_slot <= p->rec.count; tick_slot++) {
                sub = sub->more;
                if ((got = dos_io(0x4000, &sub->rec, fd, sizeof(struct ItemRec)))
                    != sizeof(struct ItemRec))
                    return 0;
                n++;
            }
        }
    }
    if (n >= 50 || dos_io(0x4000, item_templates, fd, (50 - n) * sizeof(struct ItemRec))
        == (50 - n) * sizeof(struct ItemRec)) {
        pool_clear();
        return 1;
    }
    return 0;
}

char far pool_load(int fd)                                       /* 2bfb:023f */
{
    struct Item far *p;
    struct Item far *sub;
    struct Item rec;
    struct ItemFileHdr hdr;
    char err;
    register int n;

    pool_clear();
    n = 0;
    if (dos_io(0x3f00, pool, fd, sizeof pool) != sizeof pool)
        return 0;
    treasure_has_items = 0;
    if (dos_io(0x3f00, &hdr, fd, sizeof hdr) != sizeof hdr)
        return 0;
    for (err = 0; hdr.count > 0 && !err; hdr.count--) {
        mem_fill(&rec, sizeof rec, 0);
        if (dos_io(0x3f00, &rec.rec, fd, sizeof(struct ItemRec))
            != sizeof(struct ItemRec))
            err = 1;
        if (!err) {
            if (pool_items != 0) {
                pool_alloc(&p->next, sizeof(struct Item), &item_record_pool);
                p = p->next;
            } else {
                pool_alloc(&pool_items, sizeof(struct Item), &item_record_pool);
                p = pool_items;
            }
            mem_move(&rec, p, sizeof rec);
            treasure_has_items = 1;
            n++;
            if (p->rec.type == 'I') {
                sub = p;
                for (tick_slot = 1; tick_slot <= p->rec.count && !err;
                     tick_slot++) {
                    mem_fill(&rec, sizeof rec, 0);
                    if (dos_io(0x3f00, &rec.rec, fd, sizeof(struct ItemRec))
                        != sizeof(struct ItemRec))
                        err = 1;
                    if (!err) {
                        pool_alloc(&sub->more, sizeof(struct Item), &item_record_pool);
                        sub = sub->more;
                        mem_move(&rec, sub, sizeof rec);
                    } else
                        p->rec.count = tick_slot - 1;
                }
                n += p->rec.count;
            }
        }
    }
    if (err)
        pool_clear();
    else
        pool_set_records(n);
    return !err;
}
