/* CKIT.EXE resident module 13ad: a table of 100 records and the text pool
 * beside it.  Allocating both, counting the records in use, finding the first
 * free record's 1-based number, freeing a record by number, and adding, reading
 * and freeing text in a packtext.c pool by a 1-based slot; reading text back
 * replaces its first two carets with the current party member's name.
 *
 * EXE 0x005eda up to 0x006100, one Turbo C large-model _TEXT segment starting
 * at byte 0xa of paragraph 13ad.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Frame 10d7's code ends with the retf at 0x005ed9 (its routine far-called
 * as 00d7:2d35), and hero.c starts at 0x006100 (13d0:0000).  The 36 far
 * calls into this module are all in overlay sections 4, 6, 8, 17 and 22.
 *
 * No _DATA.  _BSS is the two far pointers at a7ec..a7f3, after overlay
 * code's a7e4..a7eb and before hero.c's a7f4; the image's DGROUP bytes end
 * at a7e2.  Overlay code reads both pointers, and hero.c reads event_table.
 * _BSS follows the names, not the declaration order: event_table is hero.c's
 * name, and text_pool_block is a name that sorts ahead of it.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  Callee and variable
 * names are placeholders; a modNNNN_ name says only which segment the callee
 * lives in, and textbank_NNNN only where in this module the routine starts.
 */

enum {
    CELLS     = 100,                                                /* records in event_table */
    BANK_SIZE = 0x1c00                                              /* bytes in text_pool_block */
};

struct Event {                                                      /* 0x14 bytes */
    char kind;                                                      /* +0 */
    char pad_1[0x13];                                               /* +1   not read here */
};

struct Hero {
    char pad_0[0x60];                                               /* +0   not read here */
    char name[0x11];                                                /* +60 */
};

extern void far *far mem_alloc(int n);                           /* 00d7:2669 */
extern int  far str_len(char far *s);                          /* 00d7:2577 */
extern void far mem_move(void far *src, void far *dst, int n);  /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);            /* 00d7:277b */
extern char far txt_init(char far *pool, int size);                 /* 0cf0:02b4 */
extern int  far txt_add(char far *pool, char far *s);               /* 0cf0:0363 */
extern void far txt_get(char far *pool, int slot, char far *out);   /* 0cf0:046e */
extern void far txt_free(char far *pool, int slot);                 /* 0cf0:04ca */

extern struct Hero far *cur_node;                                   /* b3dd */

char far *text_pool_block;                                          /* a7ec */
struct Event far *event_table;                                         /* a7f0 */

void far textbank_init(void)                                        /* 13ad:000a */
{
    event_table = mem_alloc(CELLS * sizeof(struct Event));
    text_pool_block = mem_alloc(BANK_SIZE);
    mem_fill(event_table, CELLS * sizeof(struct Event), 0);
    txt_init(text_pool_block, BANK_SIZE);
}

int far event_used_count(void)                                         /* 13ad:0058 */
{
    struct Event far *p;
    int i;
    int n;

    p = event_table;
    if (p == 0)
        return 0;
    i = n = 0;
    for (; i < CELLS; i++, p++)
        if (p->kind != 0)
            n++;
    return n;
}

int far event_free_slot(void)                                        /* 13ad:0097 */
{
    struct Event far *p;
    int i;

    p = event_table;
    if (p != 0) {
        for (i = 0; i < CELLS && p->kind != 0; i++, p++)
            ;
        if (i < CELLS)
            return ++i;
    }
    return 0;
}

int far event_release(int n)                                        /* 13ad:00d7 */
{
    struct Event far *p;

    p = event_table;
    if (p == 0 || n <= 0 || n > CELLS)
        return 0;
    (p + n - 1)->kind = 0;
    return 1;
}

int far bank_text_add(char far *pool, char far *s)                  /* 13ad:011a */
{
    int slot;

    if ((slot = txt_add(pool, s)) >= -1)
        slot++;
    return slot;
}

void far bank_text_get(char far *pool, int slot, char far *out)     /* 13ad:0141 */
{
    char far *p;
    int n;

    txt_get(pool, --slot, out);
    n = 0;
    for (p = out; *p != 0; p++)
        if (*p == '^' && ++n < 3) {
            mem_move(p + 1, p + str_len(cur_node->name), str_len(p));
            mem_move(cur_node->name, p, str_len(cur_node->name));
            p += str_len(cur_node->name) - 1;
        }
}

void far bank_text_free(char far *pool, int slot)                    /* 13ad:0217 */
{
    txt_free(pool, --slot);
}
