/* CKIT.EXE resident module 2157: the VGA palette and its colour cycles.
 * pal_target holds the palette the game asked for and pal_live the one last
 * handed to 00d7:20f6; a bit per entry in pal_stale marks a pal_target entry
 * not yet copied across.  Up to 12 colour cycles rotate a run of pal_live
 * entries each time timer_ticks's tick count passes their next tick.
 *
 * pal_set takes a run of RGB triples, stored shifted right by 2, and a list
 * of cycles.  An entry that changes is written to pal_target and marked
 * stale; when anything changed, the running cycles that overlap the run are
 * freed, their entries marked stale, and the list is started in free slots.
 * pal_commit saves the cycle slots pal_set touched into cyc_last, then
 * copies the stale entries to pal_live and hands the span they cover to
 * 00d7:20f6.  input_poll steps every cycle that is due and hands on the
 * entries it rotated; strutil.c's wait loop is one of its 11 far callers,
 * so "input_poll" is a placeholder.
 *
 * EXE 0x01397d..0x014178, one Turbo C large-model _TEXT segment starting at
 * byte 0xd of paragraph 2157.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * The routine at 2157:0009, far-called only as 113b:01c9, is module 213b's
 * and ends with the retf at 0x01397c; module 21d7 starts at 0x014178
 * (21d7:0008) with no stub between.  Ghidra's index misses pal_init,
 * far-called from overlay code at 0x039260.
 *
 * _DATA is the one literal at 9eee..9f02.  _BSS follows the statics' names
 * (Turbo C's symbol-table order), not their declaration order; these names
 * reproduce the shipped d99e..e08a layout, checked with goldbox.bss_order.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  A name another module gives an
 * address is reused from there; a modNNNN_ name is a placeholder that says
 * only which segment the callee lives in.
 */

struct CycSpec {                    /* one cycle as pal_set is given it */
    unsigned char flags;            /* bit 0: rotate towards the first entry */
    unsigned char rate;             /* ticks between steps */
    unsigned char first;            /* first palette entry */
    unsigned char count;            /* entries in the run */
};

struct Cycle {
    long next;                      /* tick of the next step; 0x7fffffffL when free */
    struct CycSpec spec;
};

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far dac_load(int first, int count, int wait,
                             void far *rgb);                     /* 00d7:20f6 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern int  far random_int(int n);                               /* 0da7:022d */
extern long far timer_ticks(void);                              /* 107e:00c0 */

static char pal_on;                         /* d99e, input_poll steps cycles while set */
static unsigned char pal_live[256][3];      /* d99f */
static unsigned char pal_stale[256 / 8];    /* dc9f */
static struct Cycle cyc_last[12];           /* dcbf */
static struct Cycle cyc_slots[12];          /* dd1f */
static char cyc_touched[12];                /* dd7f */
static unsigned char pal_target[256][3];    /* dd8b */

void far pal_init(void)                                          /* 2157:000d */
{
    register int i;

    mem_fill(pal_live, sizeof(pal_live), 0);
    mem_fill(pal_target, sizeof(pal_target), 0);
    mem_fill(pal_stale, sizeof(pal_stale), 0xff);
    mem_fill(cyc_last, sizeof(cyc_last), 0);
    for (i = 0; i < 12; i++)
        cyc_last[i].next = 0x7fffffffL;
    mem_move(cyc_last, cyc_slots, sizeof(cyc_slots));
    mem_fill(cyc_touched, sizeof(cyc_touched), 0);
    pal_on = 1;
}

void far pal_set(int first, int count, unsigned char far *rgb,
                 int nspec, struct CycSpec far *spec)            /* 2157:00b3 */
{
    struct CycSpec far *s;
    short n;
    unsigned char far *p;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    char changed;
    register int i;
    register int j;

    s = spec;
    changed = 0;
    for (i = j = 0; i < 12 && !changed; i++) {
        if (cyc_slots[i].next == 0x7fffffffL)
            continue;
        if (cyc_slots[i].spec.first >= first + count)
            continue;
        if (cyc_slots[i].spec.first + cyc_slots[i].spec.count <= first)
            continue;
        /* && where || is meant: one matching field counts the slot unchanged, as shipped */
        /* s[j] is not bounded by nspec, as shipped */
        if (cyc_slots[i].spec.flags != s[j].flags &&
            cyc_slots[i].spec.rate != s[j].rate &&
            cyc_slots[i].spec.first != s[j].first &&
            cyc_slots[i].spec.count != s[j].count)
            changed = 1;
        j++;
    }
    if (j != nspec)
        changed = 1;
    p = pal_target[first];
    for (i = count, j = first; --i >= 0; j++) {
        r = *rgb >> 2;
        rgb++;
        g = *rgb >> 2;
        rgb++;
        b = *rgb >> 2;
        rgb++;
        if (p[0] != r || p[1] != g || p[2] != b) {
            changed = 1;
            pal_stale[j >> 3] |= 1 << (j & 7);
            *p = r;
            p++;
            *p = g;
            p++;
            *p = b;
            p++;
        } else
            p += 3;
    }
    if (!changed)
        return;
    for (i = 0; i < 12; i++) {
        if (cyc_slots[i].next == 0x7fffffffL)
            continue;
        if (cyc_slots[i].spec.first >= first + count)
            continue;
        if (cyc_slots[i].spec.first + cyc_slots[i].spec.count <= first)
            continue;
        j = cyc_slots[i].spec.first;
        n = cyc_slots[i].spec.count;
        while (--n >= 0) {
            pal_stale[j >> 3] |= 1 << (j & 7);
            j++;
        }
        cyc_slots[i].next = 0x7fffffffL;
        cyc_touched[i] = 1;
    }
    for (i = j = 0; i < nspec; i++, s++) {
        while (cyc_slots[j].next != 0x7fffffffL && j < 12) /* reads [12] first, as shipped */
            j++;
        if (j >= 12) {
            error_box("Out of color ranges!");
            return;
        }
        cyc_slots[j].next = timer_ticks() + random_int(8) + s->rate - 3;
        mem_move(s, &cyc_slots[j].spec, sizeof(struct CycSpec));
        cyc_touched[j] = 1;
    }
}

void far pal_commit(void)                                      /* 2157:0457 */
{
    short hi;
    register int i;
    register int lo;

    lo = 0xff;
    hi = 0;
    pal_on = 0;
    for (i = 0; i < 12; i++) {
        if (cyc_touched[i]) {
            mem_move(&cyc_slots[i], &cyc_last[i], sizeof(struct Cycle));
            cyc_touched[i] = 0;
        }
    }
    i = 0;
    while (i < 0x100) {
        if (pal_stale[i >> 3]) {
            if (pal_stale[i >> 3] == 0xff) {
                if (lo > i)
                    lo = i;
                if (i + 7 > hi)
                    hi = i + 7;
                mem_move(pal_target[i], pal_live[i], 8 * 3);
                pal_stale[i >> 3] = 0;
            } else {
                do {
                    if (pal_stale[i >> 3] & (1 << (i & 7))) {
                        mem_move(pal_target[i], pal_live[i], 3);
                        if (lo > i)
                            lo = i;
                        if (hi < i)
                            hi = i;
                    }
                    i++;
                } while (i & 7);
                pal_stale[(i - 8) >> 3] = 0;
                continue;
            }
        }
        i += 8;
    }
    if (lo <= hi)
        dac_load(lo, hi - lo + 1, 1, pal_live[lo]);
    pal_on = 1;
}

void far input_poll(void)                                        /* 2157:05c0 */
{
    struct Cycle far *c;
    long now;
    short k;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    char wait;
    register int i;
    register int j;

    if (!pal_on)
        return;
    wait = 1;
    now = timer_ticks();
    c = cyc_slots;
    for (k = 0; k < 12; k++, c++) {
        if (c->next > now)
            continue;
        i = c->spec.first;
        j = c->spec.count - 1;
        while (c->next <= now) {
            if (c->spec.flags & 1) {
                r = pal_live[i][0];
                g = pal_live[i][1];
                b = pal_live[i][2];
                mem_move(pal_live[i + 1], pal_live[i], j * 3);
                pal_live[i + j][0] = r;
                pal_live[i + j][1] = g;
                pal_live[i + j][2] = b;
            } else {
                r = pal_live[i + j][0];
                g = pal_live[i + j][1];
                b = pal_live[i + j][2];
                mem_move(pal_live[i], pal_live[i + 1], j * 3);
                pal_live[i][0] = r;
                pal_live[i][1] = g;
                pal_live[i][2] = b;
            }
            c->next += c->spec.rate;
        }
        dac_load(i, j + 1, wait, pal_live[i]);
        wait = 0;
    }
}
