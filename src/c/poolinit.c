/* CKIT.EXE overlay 6 module 21fa: setting up the slot pools.
 * Six routines take a slot count and give one pool its capacity, its
 * record size and backing storage for that many records; one takes a
 * count it never reads and points node_pool at item_record_pool instead, and the
 * last takes nothing and copies the text at boots_word into debug_word.
 * Module 2209 calls them all.
 *
 * EXE 0x039001..0x0390fe, one Turbo C large-model _TEXT segment starting at
 * byte 1 of paragraph 21fa in RTLink section 6 (payload 0x38da0, link
 * segment 21d4).  Byte 0 of paragraph 21fa is config.c's final retf, and
 * the last 14 bytes of the range lie in paragraph 2209, whose module starts
 * at 2209:000e.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are
 * defined in shipped order; bake consumes them in that order.  Sections 7
 * to 10 hold other bytes at the 21d4 link segment.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; arena_alloc names a routine
 * of section 6's module 2209.  mem_alloc and arena_alloc both
 * return storage for the byte count they are passed.
 *
 * _DATA and _BSS are empty: every pool is another module's.
 */

#define POOL_INIT(pool, n, size, alloc)                                 \
    pool.capacity = n;                                                  \
    pool.stride = size;                                                 \
    pool.base = alloc(n * size)

struct Pool {                   /* strutil.c's slot pool                  */
    int capacity;               /* +0                                     */
    int stride;                 /* +2                                     */
    char far *base;             /* +4                                     */
    unsigned char bitmap[1];    /* +8                                     */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern void far *far arena_alloc(int n);                     /* 2209:0606 */

extern char far *item_words[];                                   /* 3d03 */
#define boots_word (item_words[45])                              /* "Boots" */
extern char debug_word[];                                          /* b3f4 */
extern struct Pool party_pool;                                    /* b76e */
extern struct Pool patch_pool;                                    /* bb90 */
extern struct Pool npc_pool;                                    /* bd01 */
extern struct Pool effect_pool;                                    /* c3c4 */
extern struct Pool item_record_pool;                                    /* cb0e */
extern void far *node_pool;                                      /* ce5d */
extern struct Pool combat_pool;                                  /* ce72 */

void far party_pool_init(int n)                                    /* 31fa:0001 */
{
    POOL_INIT(party_pool, n, 0x18e, mem_alloc);   /* hero.c's Hero */
}

void far npc_pool_init(int n)                                    /* 31fa:0025 */
{
    POOL_INIT(npc_pool, n, 0x18e, arena_alloc); /* hero.c's Hero */
}

void far item_pool_init(int n)                                    /* 31fa:0049 */
{
    POOL_INIT(item_record_pool, n, 0x3e, mem_alloc);    /* hero.c's Item */
}

void far item_pool_select(int n)            /* 31fa:006d */     /* n unread */
{
    node_pool = &item_record_pool;
}

void far effect_pool_init(int n)                                    /* 31fa:007c */
{
    POOL_INIT(effect_pool, n, 0xa, mem_alloc);     /* hero.c's Effect */
}

void far cloud_pool_init(int n)                                    /* 31fa:00a0 */
{
    POOL_INIT(patch_pool, n, 0x21, arena_alloc);
}

void far combat_pool_init(int n)                                    /* 31fa:00c4 */
{
    POOL_INIT(combat_pool, n, 0x18, arena_alloc);
}

void far debug_password_init(void)                                     /* 31fa:00e8 */
{
    str_copy(debug_word, boots_word);
}
