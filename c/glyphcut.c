/* v1.0 build (CKIT.EXE "Version 1.0 March 19,1993"): this object is at EXE
 * 0x07c116, frame 3e5d:0006, 5,507 code bytes; _DATA a69c, _BSS d4ce.  The
 * comment below describes v1.2.  The only difference from v1.2: trn_load
 * calls path_default_ext without a prototype in scope, so its force
 * argument 1 is pushed as an int (mov ax,1), one byte longer.
 *
 * CKIT.EXE overlay section 24, module 2e59: cutting a glyph out of the screen.
 * glyph_grab copies the cut's 8-byte glyph header to the start of the
 * scratch block db_buffer returns, ORs flags into its type, adjusts the
 * type and the row width by video mode (is_xvga, pixel_shift), and
 * reads the rectangle at (row, col) of the screen into the block after the
 * header in the form the type names: 0 and 5 the planes as they are; 1 the
 * planes and a transparency mask, built by mask_row and cut out of the
 * planes by mask_apply; 2 the planes run-length packed (grab_big,
 * rle_pack; with four planes on display 3, first turned into packed 4-bit
 * pixels by planes4_to_pixels); 3 runs of four kinds -- transparent (a count
 * byte, omitted for a row's trailing run), partly masked (| 0x40, then mask
 * and data), repeated (| 0x80) and literal (| 0xc0) -- (grab_packed,
 * pack_row, run_len); 7 runs of zero and non-zero mask
 * bytes (grab_bytes); 8 a palette glyph, its entries and colour cycles
 * from ilbm.c's ilbm_cmap and ilbm_crng (slot 8) and a TRN table
 * (trn_load); 9 rec's six-byte records.  Other types call console_beep and give 0.
 * It returns the size of what it wrote, header included ("Tile too
 * large %l/%d" leaves 8; type 2 gives 0 once its output passes 0x3c00).  The cut's key byte is the transparent colour;
 * a frame byte other than 0 and n + 1 sets bit 8 of the key, and
 * mask_row then XORs the picture with the line it reads after
 * draw_page_set(0) instead of comparing pixels with the key.  trn_load
 * reads 0x100 bytes of the file named by trn_name, default extension
 * TRN, and packs n of them from skip as nibbles; no instruction in the
 * image but trn_load's names trn_name, so as far as the bytes show the
 * name stays empty.
 *
 * Section 24 (parent section 20, which is empty and has no parent; no
 * section names 24 as its parent): payload at EXE 0x07b5b0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 24.  The module is stored frame 2e59 (load-based 3e59),
 * EXE 0x07c230..0x07d7b2 (5,506 bytes, 10 routines), from byte 0 of the
 * frame, right after slotsave.c's last retf at 2e44:014f, to its last
 * retf at 2e59:1581; module 2fb1 starts at 2fb1:0002 (EXE 0x07d7b2, push
 * bp) with no pad between.  One Turbo C large-model _TEXT segment, built
 * with Turbo C 2.01, tcc -ml -O -Z -d; functions are defined in shipped
 * order.  Definition comments give the load-based address; extern
 * comments the far pointer as stored in the shipped call, or the DGROUP
 * offset.
 *
 * Section 24's relocation table files 128 sites under 2e59, all in this
 * range (EXE 0x07c23c..0x07d7a8), all far calls: 34 into frame 00d7, 7
 * to error_box, 51 into 0f5e, 32 into 107e, 1 to path_default_ext and 3
 * to ilbm.c (module 2fb1).  The next site, EXE 0x07d7ce, is filed under
 * 2fb1.  The only far calls to 2e59 in the image are artimp.c's two to
 * glyph_grab (EXE 0x07bc6b and 0x07bd5d); no vector lands here, and
 * section 24 has no descendants.  The other nine routines are reached
 * only by near calls after their definitions, so static is by choice: the
 * bytes do not decide it.
 *
 * FRUA's own code: the sharing map pairs all ten routines a-only, so
 * every name here is chosen from the code.
 *
 * _DATA is DGROUP a6f6..a78e: seven literals, "Tile too large %l/%d"
 * stored once for its two uses.  Below it a6f2..a6f5 is the long
 * fcache.c declares as far_fallback (resident code reads it), above
 * dirfind.c's a6ee..a6f1; a78f is a pad byte, and a790 up is read by
 * module 2fb1.  _BSS is trn_name at d52c, after areatool.c's (section 23)
 * fill_rec at d528..d52b; d57c up is section 25's, so the 0x50 is the
 * spacing, not a bound the bytes give.
 *
 * Forced by the bytes, each against the module's ten ranges: rle_pack's
 * dst and start declared register (Turbo C keeps far pointers in memory,
 * but the return's difference then loads start into dx, sub ax,dx; sbb
 * dx,dx; with neither or only one, rle_pack is a byte longer and 5 of 10
 * bake); rle_pack's dst a char pointer (unsigned: run - 1 is add al,0xff,
 * 9 of 10); the assignment inside trn_load's open test (two statements,
 * 8 of 10); case 8's conditional expression (an if-else chain merges the
 * two 0x100 arms, 9 of 10); grab_bytes' register w beside a plain x (both
 * register or both plain put w in di, 9 of 10); wb <<= 3 (*= 8, 9 of 10);
 * 1 | flags (flags | 1, 9 of 10); 0x3ff8u (signed, 9 of 10); mask_row
 * defined old-style with a char inv, as its callers push an int (a
 * prototype, 6 of 10; int inv, 9 of 10); size = size + 8 in case 9 (+=,
 * 9 of 10).
 *
 * Not decided by the bytes (each bakes 10 of 10): short or int w2 and
 * mode; if ((w2 = w) & 1) or two statements; static or public for the
 * nine module-local routines; *dst++ = c << 4 | *p++ or three statements
 * in trn_load; *dst++ = c or *dst = c; dst++ in mask_row; a[stride * i +
 * 1] or (a + stride * i)[1]; cur += n = pack_row(...) or two statements.
 */

struct Glyph {                  /* the 8-byte header before a glyph's bits */
    int rows;                   /* +0   type 8: flags                     */
    int dy;                     /* +2   type 8: the first entry           */
    int dx;                     /* +4   type 8: the entry count           */
    unsigned char width;        /* +6   bytes per plane row; type 8: the
                                        colour cycles                     */
    unsigned char type;         /* +7                                     */
};

struct Cut {                    /* where glyph_grab cuts, 0x10 bytes      */
    int row;                    /* +0   the top pixel row                 */
    int col;                    /* +2   the left pixel column             */
    struct Glyph hdr;           /* +4                                     */
    char byte_c;                /* +c   not read here                     */
    unsigned char key;          /* +d   the transparent colour            */
    unsigned char count;        /* +e   type 9: the record count          */
    unsigned char frame;        /* +f   not 0 nor n + 1: key bit 8 set    */
};

struct GlyphRec {               /* a type-9 record, 6 bytes               */
    char byte_0;                /* +0                                     */
    unsigned char count;        /* +1                                     */
    char pad[4];                /* +2                                     */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far planes4_to_pixels(void far *src, int stride, void far *dst, int n);
                                                                 /* 00d7:07ef */
extern void far grab_words(void far *p, int a, int b);         /* 00d7:09e4 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern int  far dos_open(int ax, char far *name, ...);       /* 00d7:26bc */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far video_plane_set(int plane);                         /* 0f5e:05ef */
extern void far video_address_set(int row, int col);                  /* 0f5e:0708 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern void far console_beep(void);                              /* 107e:02c5 */
extern char far path_default_ext();  /* 17a1:0264; v1.0: no prototype, so the 1 is pushed as an int */
extern int  far ilbm_cmap();       /* no prototype: called (8, 0) and (8, n, rgb) */
                                                                 /* 2fb1:01c5 */
extern int  far ilbm_crng(int slot, void far *cyc);              /* 2fb1:0310 */

static char trn_name[0x50];                                      /* d52c */

static void far mask_row(src, w, dst, stride, inv, planes, key, row) /* 3e59:0000 */
unsigned char far *src;
int w;
unsigned char far *dst;
int stride;
char inv;
int planes;
int key;
int row;
{
    short w2;
    short mode;
    unsigned char c;
    unsigned char bit;
    unsigned char buf[4][0x50];
    register int x;
    register int p;

    if (is_xvga())
        mode = 0;
    else
        mode = pixel_shift();
    if (key & 0x100) {
        w2 = w;
        if (w2 & 1)
            w2++;
        draw_page_set(0);
        video_address_set(row, 0);
        for (p = 0; p < planes; p++) {
            video_plane_set(p);
            grab_words(buf[p], 1, w2);
        }
        draw_page_set(1);
        for (x = 0; x < w; x++)
            for (p = 0; p < planes; p++) {
                c = buf[p][x] ^ src[p * stride + x];
                if (is_xvga())
                    buf[p][x] = c;
                else if (p)
                    buf[0][x] |= c;
                else
                    buf[0][x] = c;
            }
        switch (mode) {
        case 0:
            for (x = 0; x < w; x++)
                for (p = 0; p < planes; p++)
                    if (buf[p][x])
                        buf[p][x] = 0xff;
            break;
        case 1:
            for (x = 0; x < w; x++) {
                if (buf[0][x] & 0xf)
                    buf[0][x] |= 0xf;
                if (buf[0][x] & 0xf0)
                    buf[0][x] |= 0xf0;
            }
            break;
        case 2:
            for (x = 0; x < w; x++) {
                if (buf[0][x] & 3)
                    buf[0][x] |= 3;
                if (buf[0][x] & 0xc)
                    buf[0][x] |= 0xc;
                if (buf[0][x] & 0x30)
                    buf[0][x] |= 0x30;
                if (buf[0][x] & 0xc0)
                    buf[0][x] |= 0xc0;
            }
            break;
        }
        if (inv)
            for (x = 0; x < w; x++) {
                buf[0][x] ^= 0xff;
                if (is_xvga())
                    for (p = 1; p < planes; p++)
                        buf[p][x] ^= 0xff;
            }
        mem_move(buf, dst, w);
        if (is_xvga())
            for (p = 1; p < planes; p++)
                mem_move(buf[p], dst + w * p, w);
    } else {
        if (is_xvga())
            w *= planes;
        while (--w >= 0) {
            switch (mode) {
            case 0:
                c = *src == key ? 0xff : 0;
                break;
            case 1:
                if ((*src & 0xf) == key)
                    c = 0xf;
                else
                    c = 0;
                if ((*src >> 4 & 0xf) == key)
                    c |= 0xf0;
                break;
            case 2:
                if ((*src & 3) == key)
                    c = 3;
                else
                    c = 0;
                if ((*src >> 2 & 3) == key)
                    c |= 0xc;
                if ((*src >> 4 & 3) == key)
                    c |= 0x30;
                if ((*src >> 6 & 3) == key)
                    c |= 0xc0;
                break;
            case 3:
                c = 0xff;
                bit = 1;
                for (p = 0; p < planes; p++) {
                    if (key & bit)
                        c &= src[stride * p];
                    else
                        c &= ~src[stride * p];
                    bit <<= 1;
                }
                break;
            }
            if (!inv)
                c ^= 0xff;
            *dst++ = c;
            src++;
        }
    }
}

static void far mask_apply(unsigned char far *mask, unsigned char far *bits, int n,
                           int planes)                           /* 3e59:03ce */
{
    unsigned char far *q;
    unsigned char far *p;
    register int i;

    p = bits;
    q = mask;
    while (--planes >= 0) {
        if (!is_xvga())
            q = mask;
        for (i = n; --i >= 0; ) {
            *p &= ~*q;
            q++;
            p++;
        }
    }
}

static int far run_len(unsigned char far *a, unsigned char far *mask, int stride,
                       int planes)                               /* 3e59:0421 */
{
    char same;
    register int i;
    register int n;

    n = 0;
    same = 1;
    while (same) {
        n++;
        if (*mask++ != 0xff)
            same = 0;
        for (i = 0; same && i < planes; i++)
            same = a[stride * i] == a[stride * i + 1];
        a++;
    }
    return n;
}

static int far pack_row(int n, unsigned char far *mask, unsigned char far *data,
                        unsigned char far * far *out, unsigned char far *dst,
                        int vplanes, int planes, int stride)    /* 3e59:048c */
{
    unsigned char c;
    unsigned char k;
    unsigned char far *mark;
    unsigned char far *q;
    unsigned char far *o;
    register int run;
    register int lim;

    o = *out;
    q = dst;
    while (n > 0) {
        c = *mask++;
        run = 1;
        lim = int_min(n, 0x3f);
        if (c == 0) {
            for (; run < lim && *mask == 0; run++)
                mask++;
            if (run < n)
                *o++ = run;
            data += run;
            n -= run;
        } else if (c != 0xff) {
            mark = o++;
            *o++ = c;
            while (run < lim && (c = *mask) != 0 && c != 0xff) {
                *o++ = c;
                mask++;
                run++;
            }
            *mark = run | 0x40;
            for (k = 0; k < planes; k++)
                mem_move(data + k * stride, q + k * 0xc00, run);
            q += run;
            data += run;
            n -= run;
        } else {
            run = run_len(data, mask, stride, vplanes);
            run = int_min(lim, run);
            if (run >= 2) {
                for (k = 0; k < planes; k++)
                    mem_move(data + k * stride, q + k * 0xc00, 2);
                *o++ = run | 0x80;
                q++;
                mask += run - 1;
                n -= run;
                data += run;
            } else {
                mark = data;
                for (; run < lim && *mask == 0xff &&
                       run_len(mark++, mask, stride, vplanes) < 2; run++)
                    mask++;
                for (k = 0; k < planes; k++)
                    mem_move(data + k * stride, q + k * 0xc00, run);
                n -= run;
                q += run;
                *o++ = run | 0xc0;
                data += run;
            }
        }
    }
    *o++ = 0;
    *out = o;
    return q - dst;
}

static int far grab_packed(int row, int col, int rows, int width, int planes,
                           int key)                              /* 3e59:06f1 */
{
    int a;
    int b;
    int r;
    int n;
    unsigned char far *o;
    unsigned char far *cur;
    unsigned char far *start;
    unsigned char data[0x142];
    unsigned char mask[0x140];
    register int i;
    register int w;

    w = width + 1 & 0xfffe;
    o = (unsigned char far *)db_buffer() + 0xc;
    start = cur = o + (4 - planes) * 0x600 + 0x800;
    for (r = row; r < row + rows; r++) {
        for (i = 0; i < video_planes(); i++) {
            video_plane_set(i);
            video_address_set(r, col);
            grab_words(data + w * i, 1, w);
        }
        mask_row(data, width, mask, w, 0, video_planes(), key, r - row);
        if ((unsigned char far *)db_buffer() + 0x4000 < cur + (planes - 1) * 0xc00 + w) {
            error_box("Tile too large (packed)");
            break;
        }
        n = pack_row(width, mask, data, &o, cur, video_planes(), planes, w);
        cur += n;
    }
    a = o - ((unsigned char far *)db_buffer() + 0xc);
    b = cur - start;
    if (a & 1) {
        a++;
        *o = 0;
    }
    if (b & 1) {
        b++;
        for (i = 0; i < planes; i++)
            cur[i * 0xc00] = 0;
    }
    mem_move(&a, (unsigned char far *)db_buffer() + 8, 4);
    o = (unsigned char far *)db_buffer() + a + 0xc;
    for (i = 0; i < planes; i++) {
        mem_move(start + i * 0xc00, o, b);
        o += b;
    }
    return o - (unsigned char far *)db_buffer();
}

static int far rle_pack(unsigned char far *src, register char far *dst, int n) /* 3e59:0908 */
{
    unsigned char far *q;
    unsigned char c;
    register char far *start;
    register int run;
    register int left;

    left = n;
    start = dst;
    while (left > 0) {
        if (left > 1 && src[1] == src[0]) {
            run = 0;
            c = *src++;
            while (run < left - 1 && *src == c && run < 0x7f) {
                run++;
                src++;
            }
            *dst++ = -run;
            *dst++ = c;
            left -= run + 1;
        } else {
            run = 0;
            q = src;
            while (run < left && run < 0x80 &&
                   !(run < left - 2 && q[1] == q[0] && q[2] == q[0])) {
                q++;
                run++;
            }
            *dst++ = run - 1;
            left -= run;
            while (--run >= 0) {
                *dst = *src;
                src++;
                dst++;
            }
        }
    }
    return dst - start;
}

static int far grab_big(int row, int col, int rows, int width, int planes) /* 3e59:09ee */
{
    unsigned char far *o;
    unsigned char data[0x142];
    unsigned char packed[0x142];
    int r;
    int w;
    int n;
    register int i;
    register int wb;

    wb = width;
    w = wb + 1 & 0xfffe;
    o = (unsigned char far *)db_buffer() + 8;
    for (r = row; r < row + rows; r++) {
        for (i = 0; i < planes; i++) {
            video_plane_set(i);
            video_address_set(r, col);
            grab_words(data + w * i, 1, w);
        }
        if (planes == 4) {
            if (pixel_shift() == 3)
                planes4_to_pixels(data, w, packed, wb);
            else
                for (i = 0; i < video_planes(); i++)
                    mem_move(data + w * i, packed + wb * i, wb);
        } else if (planes == 1)
            mem_move(data, packed, wb);
        else
            error_box("Big packed tile for %d planes", planes);
        o += rle_pack(packed, o, wb * planes);
        if (o - (unsigned char far *)db_buffer() > 0x3c00)
            return 0;
    }
    n = o - (unsigned char far *)db_buffer();
    return n;
}

static int far grab_bytes(int row, int col, int rows, int width, int planes,
                          int key)                               /* 3e59:0b4e */
{
    int r;
    int k;
    int pl;
    int cnt;
    unsigned char far *o;
    unsigned char data[0x142];
    unsigned char mask[0x140];
    unsigned char far *p;
    unsigned char far *q;
    register int w;
    int x;

    w = width + 1 & 0xfffe;
    o = (unsigned char far *)db_buffer() + 8;
    for (pl = 0; pl < planes; pl++) {
        for (r = row; r < row + rows; r++) {
            for (k = 0; k < video_planes(); k++) {
                video_plane_set(k);
                video_address_set(r, col);
                grab_words(data + w * k, 1, w);
            }
            mask_row(data, width, mask, w, 1, video_planes(), key, r - row);
            if ((unsigned char far *)db_buffer() + 0x4000 < o + w) {
                error_box("Tile too large (byte packed)");
                break;
            }
            p = data + w * pl;
            q = mask + w * pl;
            for (x = 0; x < width; ) {
                if (*q) {
                    for (cnt = 0; x < width && *q++ && cnt < 0x7f; cnt++, x++)
                        p++;
                    q--;
                    if (x >= width)
                        break;
                    *o++ = -cnt;
                }
                for (cnt = 0; x < width && !*q++ && cnt < 0x7f; cnt++, x++)
                    ;
                q--;
                if (cnt) {
                    *o++ = cnt;
                    mem_move(p, o, cnt);
                    p += cnt;
                    o += cnt;
                }
            }
            *o++ = 0;
        }
    }
    return o - (unsigned char far *)db_buffer();
}

static void far trn_load(unsigned char far *dst, int skip, int n) /* 3e59:0d4f */
{
    unsigned char buf[0x100];
    unsigned char far *p;
    unsigned char c;
    char path[0x50];
    register int i;
    register int fd;

    str_copy(path, trn_name);
    path_default_ext(path, "TRN", 1);
    if ((fd = dos_open(0x3d00, path)) >= 0) {
        if (dos_io(0x3f00, buf, fd, 0x100) != 0x100)
            error_box("Read error on '%s'", path);
        dos_call(0x3e00, 0, fd);
        p = buf + skip;
        for (i = 0; i < n; i++)
            if (i & 1) {
                *dst = c << 4 | *p;
                p++;
                dst++;
            } else
                c = *p++;
        if (i & 1)
            *dst++ = c << 4;
    } else
        error_box("Can't open '%s' for input", path);
}

int far glyph_grab(struct Cut far *cut, struct GlyphRec far *rec, int flags, int n) /* 3e59:0e4f */
{
    struct Glyph far *hdr;
    int r;
    int k;
    int w;
    int msize;
    int cnt;
    int planes;
    int key;
    unsigned char far *p;
    unsigned char line[0x140];
    register int size;
    register int wb;

    if (flags & 0x80)
        planes = 1;
    else
        planes = video_planes();
    mem_move(&cut->hdr, db_buffer(), 8);
    hdr = db_buffer();
    hdr->type = hdr->type & 0xf | flags;
    if ((hdr->type & 0xf) == 5 && pixel_shift() && !is_xvga())
        hdr->type = 1 | flags;
    if ((hdr->type & 0xf) == 7 && pixel_shift() && !is_xvga())
        hdr->type = 3 | flags;
    if ((hdr->type & 0xf) == 3 && is_xvga())
        hdr->type = 7 | flags;
    if (pixel_shift())
        hdr->width <<= 3 - pixel_shift();
    wb = hdr->width;
    if (!pixel_shift())
        wb <<= 3;
    key = cut->key;
    if (cut->frame && cut->frame - 1 != n)
        key |= 0x100;
    pointer_hide();
    switch (hdr->type & 0xf) {
    case 0:
    case 5:
        size = cut->hdr.rows * wb;
        if (0x3ff8u / planes < size) {
            error_box("Tile too large %l/%d", (long)size * planes + 8, 0x4000);
            size = 8;
            break;
        }
        if (wb & 1) {
            w = wb;
            if (w & 1)
                w++;
            p = (unsigned char far *)db_buffer() + 8;
            for (k = 0; k < planes; k++) {
                video_plane_set(k);
                for (r = 0; r < cut->hdr.rows; r++) {
                    video_address_set(cut->row + r, cut->col);
                    grab_words(line, 1, w);
                    mem_move(line, p, wb);
                    p += wb;
                }
            }
        } else {
            video_address_set(cut->row, cut->col);
            for (k = 0; k < planes; k++) {
                video_plane_set(k);
                grab_words((unsigned char far *)db_buffer() + k * size + 8,
                             cut->hdr.rows, wb);
            }
        }
        size = size * planes + 8;
        break;
    case 1:
        size = cut->hdr.rows * wb;
        msize = size;
        if (is_xvga())
            msize = video_planes() * msize;
        if (0x3ff8u / ((is_xvga() ? planes : 1) + planes) < size) {
            error_box("Tile too large %l/%d", (long)size * planes + msize + 8, 0x4000);
            size = 8;
            break;
        }
        if (wb & 1) {
            w = wb;
            if (w & 1)
                w++;
            p = (unsigned char far *)db_buffer() + msize + 8;
            for (k = 0; k < video_planes(); k++) {
                video_plane_set(k);
                for (r = 0; r < cut->hdr.rows; r++) {
                    video_address_set(cut->row + r, cut->col);
                    grab_words(line, 1, w);
                    mem_move(line, p, wb);
                    p += wb;
                }
            }
        } else {
            video_address_set(cut->row, cut->col);
            for (k = 0; k < video_planes(); k++) {
                video_plane_set(k);
                grab_words((unsigned char far *)db_buffer() + msize + k * size + 8,
                             cut->hdr.rows, wb);
            }
        }
        if (cut->frame)
            for (r = 0; r < cut->hdr.rows; r++)
                mask_row((unsigned char far *)db_buffer() + msize + r * wb + 8, wb,
                         (unsigned char far *)db_buffer() + r * wb + 8, size, 1,
                         video_planes(), key, r);
        else
            mask_row((unsigned char far *)db_buffer() + msize + 8, size,
                     (unsigned char far *)db_buffer() + 8, size, 1, video_planes(),
                     key, 0);
        mask_apply((unsigned char far *)db_buffer() + 8,
                   (unsigned char far *)db_buffer() + msize + 8, size, planes);
        size = size * planes + msize + 8;
        break;
    case 2:
        size = grab_big(cut->row, cut->col, cut->hdr.rows, wb, planes);
        break;
    case 3:
        size = grab_packed(cut->row, cut->col, cut->hdr.rows, wb, planes, key);
        break;
    case 7:
        size = grab_bytes(cut->row, cut->col, cut->hdr.rows, wb, planes, key);
        break;
    case 9:
        cnt = cut->count;
        for (size = 0; size < cnt; size++)
            rec[size].count = cnt;
        size = cnt * 6;
        mem_move(rec, (unsigned char far *)db_buffer() + 8, size);
        size = size + 8;
        break;
    case 8:
        size = is_xvga() ? 0x100 : pixel_shift() ? 0x10 : 0x100;
        size = int_min(size, ilbm_cmap(8, 0));
        ilbm_cmap(8, size, (unsigned char far *)db_buffer() + 8);
        if (cut->hdr.rows & 1) {
            mem_move((unsigned char far *)db_buffer() + cut->hdr.dy * 3 + 8,
                         (unsigned char far *)db_buffer() + 8, cut->hdr.dx * 3);
            size = cut->hdr.dx;
        }
        size = size * 3 + 8;
        if (cut->hdr.rows & 2) {
            hdr->width = ilbm_crng(8, (unsigned char far *)db_buffer() + size);
            size += hdr->width * 4;
        }
        if (cut->hdr.rows & 4)
            if (cut->hdr.rows & 1) {
                trn_load((unsigned char far *)db_buffer() + size, cut->hdr.dy,
                         cut->hdr.dx);
                size += cut->hdr.dx + 1 >> 1;
            } else {
                trn_load((unsigned char far *)db_buffer() + size, 0, 0x100);
                size += 0x80;
            }
        break;
    default:
        console_beep();
        size = 0;
    }
    pointer_show();
    return size;
}
