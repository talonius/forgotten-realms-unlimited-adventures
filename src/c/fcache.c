/* CKIT.EXE resident module 2336: the far-memory file cache.  One far block
 * holds up to 48 file groups packed end to end, each holding bytes read
 * from a file, keyed by its name; a slot (0..47) names a group.  Closing
 * a slot leaves its group cached, and a later open of the same name
 * reuses it.
 * When room runs out a group no slot names is evicted: the newest one when
 * random_int(3) is nonzero, else the least recently opened one.  Bytes
 * can be inserted into and deleted from a group in place, moving the
 * groups above it.  The error texts call the cache FC.
 *
 * EXE 0x01576e..0x0167b0, one Turbo C large-model _TEXT segment starting at
 * byte e of paragraph 2336.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 22d2's last retf is at 0x01576d.  The two retf bytes at
 * 0x0167b0..0x0167b1 (243b:0000, 0001) are left to module 243b: nothing
 * far-calls them.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  A modNNNN_ or fcache_NNNN name
 * is a placeholder: modNNNN_ says only which segment the callee lives in.
 * file_cache_init and file_cache_done, sys_init's and sys_shutdown's slots,
 * are DQK's names.  far_fallback, video_mode and mem_top_seg are
 * DQK's names for variables its C never defines.  The other names are the
 * ones landed modules here or in DQK define.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is.
 * file_cache_done calls slot_release, load_cb calls grp_read, and
 * slot_used, grp_extend, huge_shift and huge_diff are each called ahead
 * of their definitions, so those carry prototypes.  grp_remove, grp_evict
 * and slot_check are called near ahead of their definitions, which only
 * a static function allows.
 *
 * slot_of and grp_ptrs are public: overlay 24 (3e3e:007c..018b) reads
 * both.  _BSS follows the variables' names (Turbo C's symbol-table order),
 * not their declaration order; these names reproduce the shipped
 * e294..e667 layout, checked with goldbox.bss_order.
 */

extern int  far dos_io(int ax, void far *buf, int bx, int cx);  /* 00d7:26e9 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far *far far_alloc(long n);                       /* 00d7:27f5 */
extern void far far_free(void far *p);                       /* 00d7:2830 */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern unsigned far mem_scan(void far *p, int n, char c);    /* 00d7:27b1 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern long far file_seek(int fd, long pos, int whence);      /* 00d7:2705 */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern int  far random_int(int n);                               /* 0da7:022d */
extern int  far is_tandy(void);   /* 18de:2025, RTLink vector into section 6 */
extern void far path_base_copy(char far *name, char far *out);     /* 17d3:0334 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */

extern long far_fallback;                                        /* a6f2 */
extern int video_mode;                                           /* d61b */
extern unsigned mem_top_seg;                                     /* d721 */

void far slot_release(char far *name);
long far slot_used(int slot);
char far grp_extend(long n);
char far grp_read(int fd, long n);
void far huge_shift(char far *src, char far *dst, long n);
long far huge_diff(char far *a, char far *b);
static void grp_remove(int n);
static char grp_evict(char for_open);
static void slot_check(int slot);

static char mru[48];            /* e294  groups, most recently opened first */
static char names[48][14];      /* e2c4  each group's file name           */
char slot_of[48];               /* e564  each slot's group, -1 when none   */
static int kill_mem;            /* e594  evictions to make room            */
char far *grp_ptrs[49];         /* e596  group starts; [n_groups] is free  */
static int n_groups;            /* e65a                                    */
static long least_free;         /* e65c  the least room not held by a slot */
static char far *far_limit;     /* e660  the end of the block              */
static int peak_grps;           /* e664  the most groups held at once      */
static int open_kills;          /* e666  evictions to free a group entry   */

long far file_read(int fd, char far *buf, long n)                /* 2336:000e */
{
    long total;
    short ds;
    int got;
    int chunk;

    total = 0;
    while (n > 0) {
        chunk = n > 0x4000 ? 0x4000 : n;
        ds = _DS;
        _DS = (unsigned)((unsigned long)buf >> 16);
        got = dos_io(0x3f00, buf, fd, chunk);
        _DS = ds;
        if (got < 0)
            n = total = got;
        else if (got == 0)
            n = 0;
        else {
            buf = huge_add(buf, (long)got);
            total += got;
            n -= got;
        }
    }
    return total;
}

void far file_cache_init(int need_kb, int want_kb)                  /* 2336:00cd */
{
    char far *p;
    long size;

    n_groups = 0;
    mem_fill(slot_of, sizeof slot_of, -1);
    mem_fill(names, sizeof names, 0);
    size = (long)want_kb << 10;
    if ((p = far_alloc(size)) == 0)
        if ((size = far_fallback) == 0 || (p = far_alloc(size)) == 0)
            error_box("Not enough far memory available!");
    far_limit = huge_add(p, size);
    if (is_tandy()) {
        if (video_mode <= 4) {
            if (((long)far_limit & 0x0f000000L) == 0) {
                far_limit -= 0;             /* sub word [e660],0 (2336:0195) */
                size -= 0x4000;
            }
        } else if (video_mode == 12) {
            far_limit = (char far *)((unsigned long)mem_top_seg << 16);
            size = huge_diff(far_limit, p);
        }
    }
    grp_ptrs[0] = p;
    if (p == 0)
        size = 0;
    if ((long)need_kb << 10 > size)
        error_box("Out of FAR Memory! %l/%l", size, (long)need_kb << 10);
    least_free = size;
    peak_grps = open_kills = kill_mem = 0;
}

void far file_cache_done(void)                                      /* 2336:0241 */
{
    slot_release(0);
    far_free(grp_ptrs[0]);
}

char far slot_open(char far *name, register int slot)            /* 2336:025c */
{
    short i;
    char buf[80];

    path_base_copy(name, buf);
    buf[13] = 0;
    if (slot < 0 || slot >= 48)
        error_box("Invalid group (%d)", slot);
    if (!(slot_of[slot] & 0x80))
        error_box("Group %d in use for '%s'", slot, names[slot_of[slot]]);
    for (i = 0; i < n_groups; i++)
        if (str_same(names[i], buf)) {
            slot_of[slot] = i;
            mem_move(mru, mru + 1, mem_scan(mru, n_groups, i));
            mru[0] = i;
            return 1;
        }
    if (n_groups >= 48 && !grp_evict(1)) {
        error_box("FCSetup: too many file groups");
        return 1;
    }
    str_copy(names[n_groups], buf);
    slot_of[slot] = n_groups;
    mem_move(mru, mru + 1, n_groups);
    mru[0] = n_groups;
    n_groups++;
    grp_ptrs[n_groups] = grp_ptrs[n_groups - 1];
    peak_grps = int_max(peak_grps, n_groups);
    return 0;
}

char far grp_append(void far *src, int n)                        /* 2336:03bd */
{
    char far *p;
    long used;

    used = huge_diff(grp_ptrs[n_groups], grp_ptrs[n_groups - 1]);
    if (!grp_extend((long)n))
        return 0;
    p = huge_add(grp_ptrs[n_groups - 1], used);
    huge_move(src, p, n + 1);
    return 1;
}

void far mem_report(long need)                                   /* 2336:0449 */
{
    long total;
    char seen[48];
    int i;
    int row;

    draw_page_set(1);
    row = 0;
    total = 0;
    for (i = 0; i < 48; i++)
        seen[i] = 0;
    for (i = 0; i < 48; i++)
        if (!(slot_of[i] & 0x80)) {
            text_print_at(row * 4 + 0x1f40, 0x1f40, 15, "Grp %2d/%2d Size %5l %s",
                          i, slot_of[i], slot_used(i), names[slot_of[i]]);
            row++;
            seen[slot_of[i]] = 1;
            total += slot_used(i);
        }
    for (i = 0; i < n_groups; i++)
        if (!seen[i]) {
            text_print_at(row * 4 + 0x1f40, 0x1f40, 15, "Grp ##/%2d Size %5l %s",
                          i, huge_diff(grp_ptrs[i + 1], grp_ptrs[i]), names[i]);
            row++;
        }
    for (i = 0; i < 48; i++)
        text_print_at(i * 4 + 0x1f40, 0x1fcc, 15, "%2x %2x",
                      slot_of[i] & 0xff, mru[i] & 0xff);
    text_print_at(0x1f9c, 0x1f40, 15, "%d/%d grps %l size %d cnt %d mem",
                  peak_grps, 48, slot_used(-2) - least_free, open_kills, kill_mem);
    error_box("%d Need %l+%l/%l", n_groups, need, total,
              huge_diff(far_limit, grp_ptrs[0]));
}

char far mem_room(long need)                                     /* 2336:05f9 */
{
    for (;;) {
        if (huge_diff(far_limit, grp_ptrs[n_groups]) >= need)
            return 1;
        if (!grp_evict(0))
            break;
    }
    error_box("Out of FAR memory!");
    mem_report(need);
    return 0;
}

char far grp_extend(long n)                                      /* 2336:0653 */
{
    if (mem_room(n)) {
        grp_ptrs[n_groups] = huge_add(grp_ptrs[n_groups], n);
        if (slot_used(-1) < least_free)
            least_free = slot_used(-1);
        return 1;
    }
    return 0;
}

void far grp_pop(void)                                      /* 2336:06c4 */
{
    int i;

    n_groups--;
    for (i = 0; i < 48; i++)
        if (slot_of[i] == n_groups)
            slot_of[i] = -1;
    mem_move(mru + 1, mru, n_groups);
}

void far slot_close(int slot)                                  /* 2336:06fb */
{
    slot_of[slot] = -1;
}

void far slot_release(char far *name)                            /* 2336:0708 */
{
    char buf[80];
    int i;

    if (name == 0) {
        n_groups = 0;
        mem_fill(slot_of, sizeof slot_of, -1);
    } else {
        path_base_copy(name, buf);
        buf[13] = 0;
        for (i = 0; i < n_groups; i++)
            if (str_same(names[i], buf))
                grp_remove(i--);
    }
}

char far load_cb(int fd, char far *path)                         /* 2336:0781 */
{
    return grp_read(fd, -1L);
}

char far grp_read(register int fd, long n)                   /* 2336:0797 */
{
    long pos;

    if (n < 0) {
        if ((pos = file_seek(fd, 0L, 1)) < 0 || (n = file_seek(fd, 0L, 2)) < 0 ||
            file_seek(fd, pos, 0) < 0)
            return 0;
        n -= pos;
    }
    pos = huge_diff(grp_ptrs[n_groups], grp_ptrs[n_groups - 1]);
    if (!grp_extend(n + (n & 1 ? 1 : 0)))
        return 0;
    return file_read(fd, huge_add(grp_ptrs[n_groups - 1], pos), n) == n;
}

void far slot_load(char mode, char far *name, int slot)          /* 2336:08ce */
{
    if (!slot_open(name, slot))
        file_open(mode, name, 0, load_cb);
}

void far *far slot_base(int slot)                                /* 2336:08fe */
{
    slot_check(slot);
    return grp_ptrs[slot_of[slot]];
}

long far slot_used(int slot)                                     /* 2336:0921 */
{
    long n;

    if (slot < 0) {
        n = huge_diff(far_limit, grp_ptrs[0]);
        if (slot != -2)
            for (slot = 0; slot < n_groups; slot++)
                if (mem_scan(slot_of, 48, slot) < 48)
                    n -= huge_diff(grp_ptrs[slot + 1], grp_ptrs[slot]);
        return n;
    }
    if (slot >= 48 || slot_of[slot] & 0x80)
        return 0L;
    return huge_diff(grp_ptrs[slot_of[slot] + 1], grp_ptrs[slot_of[slot]]);
}

char far slot_reserve(register int slot, long n)                 /* 2336:0a04 */
{
    if (slot_used(slot) < -n)
        n = -slot_used(slot);
    if (n != 0) {
        if (n > 0 && !mem_room(n))
            return 0;
        slot = slot_of[slot] + 1;
        if (slot < n_groups)
            huge_shift(grp_ptrs[slot], huge_add(grp_ptrs[slot], n),
                       huge_diff(grp_ptrs[n_groups], grp_ptrs[slot]));
        for (; slot <= n_groups; slot++)
            grp_ptrs[slot] = huge_add(grp_ptrs[slot], n);
    }
    if (slot_used(-1) < least_free)
        least_free = slot_used(-1);
    return 1;
}

void far slot_insert_delete(register int slot, long pos, long delta)   /* 2336:0b35 */
{
    char far *p;
    long used;

    used = slot_used(slot);
    if (pos < 0 || pos > used) {
        error_box("FCInsDel: Invalid offset %l", pos);
        return;
    }
    if (delta < 0) {
        if (pos - delta > used) {
            error_box("FCInsDel: bad length %l", delta);
            return;
        }
    } else if (!mem_room(delta))
        return;
    if (delta < 0) {
        p = huge_add(slot_base(slot), pos);
        huge_shift(huge_add(p, -delta), p, used - pos + delta);
    }
    slot_reserve(slot, delta);
    if (delta > 0) {
        p = huge_add(slot_base(slot), pos);
        huge_shift(p, huge_add(p, delta), used - pos);
    }
}

void far huge_shift(char far *src, char far *dst, long n)        /* 2336:0caa */
{
    long d;
    unsigned chunk;

    d = huge_diff(src, dst);
    if (d < 0) {
        src = huge_add(src, n);
        dst = huge_add(dst, n);
    }
    while (n > 0) {
        chunk = n > 0xffdcL ? 0xffdc : n;
        if (d < 0) {
            src = huge_add(src, -(long)chunk);
            dst = huge_add(dst, -(long)chunk);
        }
        huge_move(src, dst, chunk);
        n -= chunk;
        if (d >= 0) {
            src = huge_add(src, (long)chunk);
            dst = huge_add(dst, (long)chunk);
        }
    }
}

static void grp_remove(int n)                                    /* 2336:0df2 */
{
    long size;
    int i;

    for (i = 0; i < 48; i++) {
        if (slot_of[i] == n)
            slot_of[i] = -1;
        else if (slot_of[i] > n)
            slot_of[i]--;
        if (i < n_groups) {
            if (mru[i] == n)
                mem_move(&mru[i + 1], &mru[i], n_groups - i - 1);
            if (mru[i] > n)
                mru[i]--;
        }
    }
    n_groups--;
    for (i = n; i < n_groups; i++) {
        size = huge_diff(grp_ptrs[i + 2], grp_ptrs[i + 1]);
        huge_shift(grp_ptrs[i + 1], grp_ptrs[i], size);
        grp_ptrs[i + 1] = huge_add(grp_ptrs[i], size);
        str_copy(names[i], names[i + 1]);
    }
    names[n_groups][0] = 0;
}

static char grp_evict(char for_open)                             /* 2336:0f29 */
{
    int i;

    if (for_open)
        open_kills++;
    else
        kill_mem++;
    if (random_int(3)) {
        for (i = n_groups - 1; i >= 0; i--)
            if (mem_scan(slot_of, 48, i) >= 48) {
                grp_remove(i);
                return 1;
            }
    } else {
        for (i = n_groups - 1; i >= 0; i--)
            if (mem_scan(slot_of, 48, mru[i]) >= 48) {
                grp_remove(mru[i]);
                return 1;
            }
    }
    return 0;
}

static void slot_check(register int slot)                        /* 2336:0fcb */
{
    if (slot < 0 || slot >= 48)
        error_box("Invalid group number (%d)", slot);
    if (slot_of[slot] & 0x80)
        error_box("Refrencing a non-loaded group (%d)", slot); /* sic */
}

long far huge_diff(char far *a, char far *b)                     /* 2336:1001 */
{
    return ((((long)a & ~0xffffL) - ((long)b & ~0xffffL)) >> 12) +
           ((long)a & 0xffffL) - ((long)b & 0xffffL);
}
