/* CKIT.EXE resident module 257c: editing a resource library in its slot.
 * Starting an empty library, inserting, deleting and resizing one item in
 * place -- the header's count and size and every later index entry move
 * with it, and the file cache moves the bytes -- and reading an item from
 * an open file into the slot and converting it through the converter
 * registered for its signature.
 *
 * EXE 0x017bca..0x01825a, one Turbo C large-model _TEXT segment starting at
 * byte a of paragraph 257c.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A modNNNN_ name is a placeholder that says only which segment the
 * callee lives in; the other names are the ones DQK's landed modules
 * define.  lib_delete (0209) has no caller found.
 *
 * sig_tags, sig_count and sig_convert belong to module 247e, which registers
 * the converters.  This module has no _BSS of its own.
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

extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern long far file_read(int fd, char far *buf, long n);        /* 1336:000e */
extern char far slot_open(char far *name, int slot);             /* 1336:025c */
extern char far grp_append(void far *src, int n);                /* 1336:03bd */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far slot_insert_delete(int slot, long pos, long delta);    /* 1336:0b35 */
extern long far huge_diff(char far *a, char far *b);             /* 1336:1001 */
extern int  far lib_items(void far *lib);                        /* 147e:0170 */
extern char far *far lib_item(void far *lib, int n);             /* 147e:01f9 */
extern long far lib_item_size(void far *lib, int n);             /* 147e:029d */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */

extern long sig_tags[4];                                         /* e676 */
extern int sig_count;                                            /* e686 */
extern long (far *sig_convert[3])(int slot, long off, long len); /* e68c */

void far lib_create(char far *name, int slot, long tag)        /* 257c:000a */
{
    struct LibHdr hdr;
    long end;

    if (!slot_open(name, slot)) {
        hdr.tag = 0x42494c48L;
        hdr.size = end = 0x14L;         /* the header and one entry */
        hdr.count = 0;
        hdr.method = 0;
        hdr.itype = 0;
        hdr.sig = tag;
        grp_append(&hdr, 16);
        grp_append(&end, 4);
    }
}

void far lib_insert(int slot, int n, long size)                /* 257c:0081 */
{
    struct LibHdr hdr;
    char far *p;
    long off;
    long pos;
    short last;
    int i;

    p = slot_base(slot);
    if (n < 0 || lib_items(p) < n)
        error_box("LBInsert invalid item (%d)", n);
    if (size < 0)
        error_box("LBInsert invalid size (%l)", size);
    huge_move(p, &hdr, 16);
    last = ++hdr.count;
    hdr.size += size + 4;
    huge_move(&hdr, p, 16);
    slot_insert_delete(slot, (n + 1) * sizeof(long) + sizeof hdr, 4L);
    p = huge_add(slot_base(slot), 16L);
    for (i = 0; i <= last; i++) {
        if (i != n + 1) {
            huge_move(p, &off, 4);
            off += 4;
        }
        if (i == n)
            pos = off;
        else if (i > n)
            off += size;
        huge_move(&off, p, 4);
        p = huge_add(p, 4L);
    }
    slot_insert_delete(slot, pos, size);
}

void far lib_delete(int slot, int n)                             /* 257c:0209 */
{
    struct LibHdr hdr;
    char far *p;
    long size;
    long off;
    long pos;
    short last;
    int i;

    p = slot_base(slot);
    if (n < 0 || lib_items(p) <= n)
        error_box("LBDelete invalid item (%d)", n);
    size = lib_item_size(p, n);
    huge_move(p, &hdr, 16);
    last = hdr.count--;
    hdr.size -= size + 4;
    huge_move(&hdr, p, 16);
    p = huge_add(p, 16L);
    for (i = 0; i <= last; i++) {
        huge_move(p, &off, 4);
        off -= 4;
        if (i == n)
            pos = off;
        else if (i > n)
            off -= size;
        huge_move(&off, p, 4);
        p = huge_add(p, 4L);
    }
    slot_insert_delete(slot, n * sizeof(long) + sizeof hdr, -4L);
    slot_insert_delete(slot, pos, -size);
}

void far lib_resize(int slot, int n, long size)                /* 257c:037e */
{
    struct LibHdr hdr;
    char far *p;
    long old;
    long off;
    long pos;
    long delta;
    short last;
    int i;

    p = slot_base(slot);
    if (n < 0 || lib_items(p) <= n)
        error_box("LBResize invalid item (%d)", n);
    old = lib_item_size(p, n);
    if ((delta = size - old) != 0) {
        huge_move(p, &hdr, 16);
        last = hdr.count;
        hdr.size += delta;
        huge_move(&hdr, p, 16);
        p = huge_add(p, 16L);
        for (i = 0; i <= last; i++) {
            if (i > n) {
                huge_move(p, &off, 4);
                if (i == n + 1) {
                    pos = off;
                    if (delta < 0)
                        pos += delta;
                }
                off += delta;
                huge_move(&off, p, 4);
            }
            p = huge_add(p, 4L);
        }
        slot_insert_delete(slot, pos, delta);
    }
}

char far lib_read_convert(int fd, long len, int slot, int n, long tag) /* 257c:04f1 */
{
    char far *p;
    long got;
    long off;
    register int i;

    /* shipped: 037e takes the new size, so this is right only for an empty
     * item; both calls found, in frame 15f5, insert item n empty first. */
    lib_resize(slot, n, len - lib_item_size(slot_base(slot), n));
    if (file_read(fd, lib_item(slot_base(slot), n), len) != len)
        return 0;
    for (i = 0; sig_tags[i] != tag && i < sig_count; i++)
        ;
    if (i < sig_count) {
        if ((got = (*sig_convert[i])(slot, huge_diff(lib_item(slot_base(slot), n),
                                                     slot_base(slot)), len)) < 0)
            return 0;
        if (got != len) {
            p = huge_add(slot_base(slot), (n + 1) * sizeof(long) + 16L);
            for (i = n + 1; i <= lib_items(slot_base(slot)); i++) {
                huge_move(p, &off, 4);
                off += got - len;
                huge_move(&off, p, 4);
                p = huge_add(p, 4L);
            }
        }
    }
    return 1;
}
