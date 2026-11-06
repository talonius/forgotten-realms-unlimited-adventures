/* CKIT.EXE resident module 247e: resource libraries.  Opening a .GLB or
 * .BTX library into a slot, the library header and its item index, finding
 * and reading one item straight from the file, converting a loaded library
 * through the converter registered for its signature, and expanding a packed
 * one: five packing methods.  Their decoders live in segment 10d7, reached
 * directly or through lz_expand and lzw_expand here; the text code tables
 * and the LZW dictionary they use are set up here, and 10d7 calls back into
 * lzw_prune.
 *
 * EXE 0x016be3..0x017bca, one Turbo C large-model _TEXT segment starting at
 * byte 3 of paragraph 247e.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A modNNNN_ name is a placeholder that says only which segment the
 * callee lives in, and fcache_NNNN names are fcache.c's; the other names are
 * the ones DQK's landed modules define.  lib_done, the empty routine
 * sys_shutdown calls, keeps sysutil.c's name.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is.  lib_init
 * calls lib_register and takes lib_convert's address through a segment
 * fixup, lib_load calls lib_convert, lib_find calls lib_find_item and
 * lib_expand calls lzw_reset and lzw_expand, all ahead of their definitions,
 * so those carry prototypes.  btx_text calls text_tables near ahead of its
 * definition, which only a static function allows.
 *
 * sig_tags, sig_count and sig_convert are public: module 257c reads them.
 * _BSS follows the names (Turbo C's symbol-table order), not the
 * declaration order; these names reproduce the shipped e66c..e697 layout.
 * sig_tags' [4] is what fits between e676 and sig_count at e686;
 * lib_register stops at three.
 *
 * 0x42494c48L is the library tag "HLIB" as four bytes.
 */

struct LibHdr {                 /* the 16 bytes at the front of a library */
    long tag;                   /* +0   "HLIB"                            */
    long size;                  /* +4   total bytes, header included      */
    int count;                  /* +8   items in the index                */
    char method;                /* +a   packing, 0 once expanded          */
    char itype;                 /* +b   nonzero: item 0 is not converted  */
    long sig;                   /* +c   selects the registered converter  */
};

struct Lzw {                    /* the LZW dictionary, codes 0..0x3ff     */
    char used[0x400];           /* +0     per code; 0 is free             */
    int word_400;               /* +400   cleared, not read in this file  */
    char suffix[0x400];         /* +402                                   */
    int prefix[0x400];          /* +802                                   */
    int next;                   /* +1002  0x100 after lzw_reset           */
};

extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern char far path_default_ext(char far *path, char far *ext, char force); /* 17a1:0264 */
extern long far file_size(int fd);                            /* 17a1:01e6 */
extern char far slot_open(char far *name, int slot);             /* 1336:025c */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern long far slot_used(int slot);                             /* 1336:0921 */
extern void far grp_pop(void);                               /* 1336:06c4 */
extern char far grp_read(int fd, long n);                     /* 1336:0797 */
extern void far slot_insert_delete(int slot, long pos, long delta);    /* 1336:0b35 */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern long far file_seek(int fd, long pos, int whence);      /* 00d7:2705 */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern char far *far packbits_expand(char far *in, char far *out, int n); /* 00d7:0a1c */
extern char far *far huff_expand(char far *in, char far *out, int n,
                                  void far *table);              /* 00d7:0cbb */
extern void far lzw_emit(char far * far *out, int code, struct Lzw far *t); /* 00d7:224b */
extern char far *far lzw_decode(unsigned w, int code, unsigned char far *in,
                                  int next, char far *end, struct Lzw far *t,
                                  char far * far *out);          /* 00d7:2286 */
extern char far *far lz_decode(char far *in, char far *out, int n); /* 00d7:233d */

void far lib_register(long tag, long (far *fn)(int slot, long off, long len));
long far lib_convert(int slot, long off, long size);
long far lib_find_item(int fd, int n);
void far lzw_reset(struct Lzw far *t);
char far *far lzw_expand(unsigned char far *in, char far *out, int n,
                         struct Lzw far *t);
static void text_tables(void);

static char far *dec_table;                                      /* e66c */
static int item_len;                                             /* e670 */
static void far *item_ptr;                                       /* e672 */
long sig_tags[4];                                                /* e676 */
int sig_count;                                                   /* e686 */
static int lib_target;                                           /* e688 */
static int item_index;                                           /* e68a */
long (far *sig_convert[3])(int slot, long off, long len);        /* e68c */

void far lib_init(void)                                          /* 247e:0003 */
{
    sig_count = 0;
    lib_register(0x42494c48L, lib_convert);
}

void far lib_done(void)                                      /* 247e:0022 */
{
}

void far lib_register(long tag, long (far *fn)(int slot, long off, long len)) /* 247e:0023 */
{
    if (sig_count < 3) {
        sig_tags[sig_count] = tag;
        sig_convert[sig_count] = fn;
        sig_count++;
    } else
        error_box("Too many library signatures");
}

char far lib_load(int fd, long pos, int slot)                    /* 247e:006d */
{
    if (grp_read(fd, pos) && lib_convert(slot, 0L, slot_used(slot)) >= 0)
        return 1;
    grp_pop();
    return 0;
}

char far lib_load_cb(int fd, char far *path)                     /* 247e:00b4 */
{
    return lib_load(fd, file_size(fd), lib_target);
}

void far lib_open(char mode, char far *name, register int slot)  /* 247e:00d1 */
{
    struct LibHdr hdr;
    char path[80];

    str_copy(path, name);
    path_default_ext(path, "GLB", 0);
    if (!slot_open(path, slot)) {
        lib_target = slot;
        file_open(mode, path, 0, lib_load_cb);
    }
    huge_move(slot_base(slot), &hdr, 16);
    if (hdr.tag != 0x42494c48L)
        error_box("LBLoad: Bad Lib: '%s'", path);
}

int far lib_items(void far *lib)                                 /* 247e:0170 */
{
    struct LibHdr hdr;

    huge_move(lib, &hdr, 16);
    if (hdr.tag != 0x42494c48L) {
        error_box("LBCount: Invalid Library File");
        return 0;
    }
    return hdr.count;
}

long far lib_sig(void far *lib)                                  /* 247e:01b2 */
{
    struct LibHdr hdr;

    huge_move(lib, &hdr, 16);
    if (hdr.tag != 0x42494c48L) {
        error_box("LBType: Invalid Library File");
        return 0L;
    }
    return hdr.sig;
}

char far *far lib_item(void far *lib, register int n)            /* 247e:01f9 */
{
    long off;
    struct LibHdr hdr;

    huge_move(lib, &hdr, 16);
    if (hdr.tag != 0x42494c48L) {
        error_box("LBIBase: Invalid Library File");
        return 0;
    }
    if (n < 0 || n >= hdr.count) {
        error_box("Invalid item (%d/%d)", n, hdr.count);
        return 0;
    }
    huge_move(huge_add(lib, n * sizeof(long) + sizeof hdr), &off, 4);
    return huge_add(lib, off);
}

long far lib_item_size(void far *lib, register int n)            /* 247e:029d */
{
    long off[2];
    struct LibHdr hdr;

    huge_move(lib, &hdr, 16);
    if (hdr.tag != 0x42494c48L) {
        error_box("LBISize: Invalid Library File");
        return 0L;
    }
    if (n < 0 || n >= hdr.count) {
        error_box("Invalid item (%d/%d)", n, hdr.count);
        return 0L;
    }
    huge_move(huge_add(lib, n * sizeof(long) + sizeof hdr), off, 8);
    return off[1] - off[0];
}

int far lib_itype(void far *lib)                                 /* 247e:0339 */
{
    struct LibHdr hdr;

    huge_move(lib, &hdr, 16);
    if (hdr.tag != 0x42494c48L) {
        error_box("LBIndxType: Invalid Library File");
        return 0;
    }
    return hdr.itype;
}

int far lib_index_find(void far *lib, int id)                    /* 247e:037c */
{
    int ent[2];
    char far *p;
    short n;

    p = lib_item(lib, 0);
    huge_move(p, &n, 2);
    p = huge_add(p, 2L);
    while (--n >= 0) {
        huge_move(p, ent, 4);
        p = huge_add(p, 4L);
        if (ent[0] == id)
            return ent[1];
    }
    return -1;
}

int far lib_find(register int fd, int id)                        /* 247e:0415 */
{
    int ent[2];
    long pos;
    short n;

    pos = file_seek(fd, 0L, 1);
    if (lib_find_item(fd, 0) <= 0 || dos_io(0x3f00, &n, fd, 2) != 2)
        return -1;
    while (--n >= 0) {
        if (dos_io(0x3f00, ent, fd, 4) != 4)
            return -1;
        if (ent[0] == id) {
            file_seek(fd, pos, 0);
            return ent[1];
        }
    }
    file_seek(fd, pos, 0);
    return 0;
}

long far lib_find_item(register int fd, register int n)          /* 247e:04c5 */
{
    struct LibHdr hdr;
    long start;
    long off[2];
    long pos;

    start = file_seek(fd, 0L, 1);
    if (dos_io(0x3f00, &hdr, fd, 16) != 16)
        return -1L;
    if (hdr.tag != 0x42494c48L || hdr.method != 0) {
        error_box("Invalid Library File (%d)", hdr.method);
        return -1L;
    }
    if (n < 0 || n >= hdr.count) {
        error_box("Invalid item (%d/%d)", n, hdr.count);
        return 0L;
    }
    pos = n * sizeof(long) + start + sizeof hdr;
    if (file_seek(fd, pos, 0) != pos || dos_io(0x3f00, off, fd, 8) != 8 ||
        file_seek(fd, start + off[0], 0) < 0)
        return -1L;
    return off[1] - off[0];
}

void far btx_open(char mode, char far *name, int slot)           /* 247e:05de */
{
    char path[80];

    str_copy(path, name);
    path_default_ext(path, "BTX", 0);
    lib_open(mode, path, slot);
}

void far btx_text(int slot, int n, char far *dst)                /* 247e:0622 */
{
    char far *lib;
    int len;

    lib = slot_base(slot);
    len = lib_item_size(lib, n);
    lib = lib_item(lib, n);
    huge_move(lib, dst, len);
    if (*dst & 0x80) {
        len = (dst[0] & 0x7f) << 8 | dst[1] & 0xff;
        text_tables();
        huff_expand(lib + 2, dst, len, db_buffer());
    }
    dst[len] = 0;
}

char far item_read_cb(int fd, char far *path)                    /* 247e:06be */
{
    return (item_len = lib_find_item(fd, item_index)) > 0 &&
           dos_io(0x3f00, item_ptr, fd, item_len) == item_len;
}

int far lib_read_item(char mode, char far *name, int n, void far *buf) /* 247e:06fb */
{
    char path[80];

    item_index = n;
    item_ptr = buf;
    str_copy(path, name);
    path_default_ext(path, "GLB", 0);
    file_open(mode, path, 0, item_read_cb);
    return item_len;
}

char far table_read_cb(int fd, char far *path)                   /* 247e:0759 */
{
    int len;

    if (dec_table == 0) {               /* shipped: a second call   */
        len = file_size(fd);         /* reads with len unset     */
        if ((dec_table = mem_alloc(len)) == 0) {
            error_box("Out of memory on LBDecode");
            return 0;
        }
    }
    return dos_io(0x3f00, dec_table, fd, len) == len;
}

void far table_load(void)                                        /* 247e:07b9 */
{
    file_open('A', "table.dec", 0, table_read_cb);
}

void far make_table(char far *out, ...)                          /* 247e:07d2 */
{
    void *ap;
    int bits;
    int count;
    char far *s;
    char **sp;
    int n;

    n = 0;
    ap = ...;                           /* stdarg.h's va_start and  */
    for (;;) {
        bits = *((int *)ap)++;          /* va_arg, written out      */
        if (bits == 0)
            break;
        sp = ap;
        s = *sp++;
        ap = sp;
        for (; *s != 0; s++) {
            count = 1 << (8 - bits);
            while (--count >= 0) {
                *out = *s;
                out[0x100] = bits;
                n++;
                out++;
            }
        }
    }
    out[0x100] = 0x87;
    out++;
    n++;
    out[0x100] = 0x87;
    out++;
    n++;
    out += 0x100;
    for (count = 0; count < 0x100; count++, out++) {
        *out = count;
        out[0x100] = 8;
    }
    if (n != 0x100)
        error_box("MakeTable count %d/256", n);
}

static void text_tables(void)                                    /* 247e:08af */
{
    make_table(db_buffer(), 3, " e", 4, "t", 5, "oansirhlu", 6, "dmcywf.pg,",
               8, "\n\rbIvETANkSOR'LCD-GHWMPUBFYx?qKVj01)(z43!%X5287Z*9/&6J#^+;Q:\"\t", 0);
}

char far *far lz_expand(char far *in, char far *out, int n)      /* 247e:08ee */
{
    out = (char far *)((long)out + 0xf8008000L); /* seg-800, off+8000 for off < 8000 */
    return lz_decode(in, out, n);
}

void far lib_expand(register int slot, long off, long size, long full, int method) /* 247e:0918 */
{
    char far *in;
    char far *out;
    long delta;
    long left;
    short n;
    register int chunk;

    left = full - 16;
    delta = full - size;
    out = huge_add(slot_base(slot), off + 16);
    huge_move(out, &n, 2);
    delta += n;
    slot_insert_delete(slot, off + 16, delta);
    out = huge_add(slot_base(slot), off + 16);
    in = huge_add(out, delta + 2);
    if (method == 3)
        lzw_reset(db_buffer());
    else if (method == 4)
        text_tables();
    while (left > 0) {
        chunk = left > 0x2000 ? 0x2000 : left;
        switch (method) {
        case 1:
            in = huge_add(packbits_expand(in, out, chunk), 0L);
            break;
        case 2:
            in = huge_add(huff_expand(in, out, chunk, dec_table), 0L);
            break;
        case 3:
            in = huge_add(lzw_expand(in, out, chunk, db_buffer()), 0L);
            break;
        case 4:
            in = huge_add(huff_expand(in, out, chunk, db_buffer()), 0L);
            break;
        case 5:
            in = huge_add(lz_expand(in, out, chunk), 0L);
            break;
        }
        out = huge_add(out, chunk);
        left -= chunk;
    }
    slot_insert_delete(slot, off + full, -(long)n);
}

long far lib_convert(register int slot, long off, long size)     /* 247e:0b98 */
{
    struct LibHdr hdr;
    char far *p;
    long prev;
    long cur;
    long got;
    long adj;
    long (far *fn)(int slot, long off, long len);
    register int i;

    p = huge_add(slot_base(slot), off);
    huge_move(p, &hdr, 16);
    if (hdr.tag != 0x42494c48L)
        error_box("_LBConvert: Bad Lib Header");
    if (hdr.method != 0) {
        lib_expand(slot, off, size, hdr.size, hdr.method);
        size = hdr.size;
        hdr.method = 0;
        p = huge_add(slot_base(slot), off);
    }
    if (hdr.size & 1)
        hdr.size++;
    if (hdr.tag != 0x42494c48L || hdr.size != size) {
        error_box("Invalid library header");
        return -1L;
    }
    for (i = 0; sig_tags[i] != hdr.sig && i < sig_count; i++)
        ;
    if (i < sig_count) {
        fn = sig_convert[i];
        p = huge_add(p, 16L);
        huge_move(p, &cur, 4);
        adj = 0;
        i = 0;
        while (i < hdr.count) {
            prev = cur;
            p += 4;
            huge_move(p, &cur, 4);
            cur += adj;
            if (i != 0 || hdr.itype == 0) {
                got = (*fn)(slot, off + prev, cur - prev);
                if (got < 0)
                    return 0L;
                adj += got - cur + prev;
                cur = prev + got;
            }
            p = huge_add(slot_base(slot), ++i * sizeof(long) + off + sizeof hdr);
            huge_move(&cur, p, 4);
        }
        hdr.size += adj;
    }
    huge_move(&hdr, huge_add(slot_base(slot), off), 16);
    return hdr.size;
}

int far lzw_prune(int keep, struct Lzw far *t, int low)          /* 247e:0e35 */
{
    int i;
    int n;
    char far *p;

    n = 0;
    do {
        for (i = 0x100, p = t->used + i; i < 0x400; i++, p++) {
            if (*p < 0 && i != keep) {
                *p = 0;
                n++;
                if (i < low)
                    low = i;
            } else if (*p > 0)
                *p = -1;
        }
        for (i = 0x100, p = t->used + i; i < 0x400; i++, p++)
            if (*p != 0)
                t->used[t->prefix[i]] = 1;
    } while (n < 0x55);
    return low;
}

void far lzw_reset(struct Lzw far *t)                            /* 247e:0ece */
{
    short i;

    mem_fill(t->used, 0x100, -1);
    mem_fill(t->used + 0x100, 0x300, 0);
    mem_fill(&t->word_400, 2, 0);
    for (i = 0; i < 0x100; i++)
        t->suffix[i] = i;
    mem_fill(t->prefix, 0x200, -1);
    t->next = 0x100;
}

char far *far lzw_expand(unsigned char far *in, char far *out, int n, struct Lzw far *t) /* 247e:0f66 */
{
    unsigned short w;
    char far *end;
    int code;

    end = out + n;
    w = *in << 8;
    in++;
    w |= *in;
    in++;
    code = w >> 6;
    lzw_emit(&out, code, t);
    return in = lzw_decode(w, code, in, t->next, end, t, &out);
}
