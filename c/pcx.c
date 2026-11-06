/* CKIT.EXE overlay section 24, module 3039: reading a PCX picture, the
 * PCX counterpart of ilbm.c.  This is the v1.0 build: its header-check error box
 * reads "Can't read PCX, bad header!" where v1.2's reads "not 256 color!".  The picture is a file loaded whole into a
 * cache slot (slot_base gives its first byte, slot_used its length).  For
 * section 24's art import (artimp.c), which runs this module for a file
 * name ending in X or x and ilbm.c's for any other:
 *
 * pcx_show copies the 0x4a-byte head of the 0x80-byte PCX header and
 * draws the picture only when it is RLE-encoded (encoding 1), eight bits
 * a pixel and one plane, with at most 0x500 bytes a row; otherwise it
 * shows an error box and returns.  It clears the screen and draws the
 * picture row by row, up to the screen's height (screen_rows), from the
 * top-left corner, each row unpacked by pcx_unpack.  In video mode 15
 * (is_xvga) pcx_unchain deals a 320-pixel row out into four 0x50-byte
 * runs of every fourth pixel and each run goes to its own plane
 * (video_plane_set, blit_bytes); otherwise, after an error box whenever
 * pixel_shift is not 0, the row goes out as it is.  pointer_hide and
 * pointer_show bracket the drawing, as in ilbm.c.
 *
 * pcx_cmap copies the palette to rgb, n RGB triples at most, and returns
 * how many it copied: 0x100 at most from the 256-colour palette at the
 * file's end when the byte before it is 0x0c (tried at 0x301 bytes from
 * the end, then, for a file one byte longer, at 0x302), otherwise 0x10 at
 * most from the header's 16-colour palette at offset 0x10.  pcx_cmap_at
 * stores that palette's offset in the file and returns its entry count
 * (0x100 or 0x10); its second test asks for more than 0x80 bytes before
 * the palette where pcx_cmap's asks for 0x81, as shipped.  pcx_crng
 * returns 0: a PCX file has no colour cycles.
 *
 * Section 24 (parent section 20, which is empty and has no parent; no
 * section names 24 as its parent): payload at EXE 0x07b5b0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 24.  The module is stored frame 3039 (load-based 4039),
 * EXE 0x07e038..0x07e498 (1,120 bytes, 5 routines), from byte 8 of the
 * frame, right after ilbm.c's last retf at 2fb1:0887, to its last retf at
 * 3039:0467; eight zero bytes follow to the section's end (EXE 0x07e4a0).
 * One Turbo C large-model _TEXT segment, built with Turbo C 2.01, tcc -ml
 * -O -Z -d; functions are defined in shipped order.  Definition comments
 * give the load-based address; extern comments the far pointer as stored
 * in the shipped call, or the DGROUP offset.
 *
 * Section 24's relocation table files 40 sites under 3039, all in this
 * range (EXE 0x07e0a9..0x07e490), all far calls: 24 into frame 00d7, 3
 * into 0da7 (clear_screen, error_box), 8 into 0f5e and 5 into 1336
 * (slot_base, slot_used); no site after them in the section.  The only
 * far calls to 3039 in the image are
 * artimp.c's to pcx_show, pcx_cmap and pcx_crng (EXE 0x07b8e4, 0x07b8f8,
 * 0x07b909); no vector lands here, and section 24 has no descendants.
 * pcx_unchain is reached only by pcx_show's near call after its
 * definition, so static is by choice: the bytes do not decide it.
 * Nothing in the image calls pcx_cmap_at.
 *
 * FRUA's own code: the sharing map pairs all five routines a-only, so
 * every name here is chosen from the code.  pcx_unchain is byte for byte
 * ilbm.c's row_unchain, a second copy in this object; one name keeps one
 * address, so it is named apart.  pcx_unpack is the last of the four
 * hand-assembly routines in frame 10d7 (src/asm/pixels.asm; ilbm.c calls
 * the first two): a PCX RLE decoder that unpacks
 * from in to out until at least n bytes are written (a byte of 0xc0 or
 * more is a run, its low six bits the count, of the next byte; any other
 * a literal) and returns the far pointer past what it read.
 *
 * _DATA is DGROUP a7ae..a7e2: two literals, ending on the image's last
 * byte (EXE 0x08f842).  Below it ilbm.c's _DATA ends at a7ad.  No _BSS:
 * a7ae and a7cd are the only DGROUP addresses the code names.
 *
 * Forced by the bytes, each against the module's five ranges: the locals'
 * declaration order in pcx_cmap and pcx_cmap_at (mark and mark_prev
 * reversed, 4 of 5 each; pos before p, 4 of 5) and in pcx_show (rowbytes
 * before w and h, 4 of 5); w computed before h (4 of 5); xmax - xmin + 1
 * (1 + xmax - xmin, 4 of 5); PcxHdr.planes a signed char, for the cbw
 * (unsigned char, 4 of 5); pcx_crng's two parameters, for its frame
 * (none, 3 of 5) and its int result (char, 4 of 5).
 *
 * Not decided by the bytes (each bakes 5 of 5): register or plain n in
 * pcx_cmap, and its int or char result; char or unsigned char for the
 * marks, encoding and bpp; int or unsigned for line_bytes and pcx_crng's
 * result; long or unsigned long for pcx_cmap_at's offset; - 0x301 or +
 * -0x301L; pcx_cmap's 0x100 tail written twice or reached by a goto;
 * pcx_cmap_at's else-if or two ifs, its last return inside an else or
 * after it; the header test as || or a negated &&; line[16][0x50] or a
 * flat line[0x500] indexed i * 0x50; short or int for the stack ints; the
 * order of the register declarations i and row; planes * line_bytes in
 * either order; sizeof hdr or 0x4a; static or public for pcx_unchain.
 */

struct CycSpec {                /* one colour cycle, as pal_set takes it  */
    unsigned char flags;        /* +0                                     */
    unsigned char rate;         /* +1                                     */
    unsigned char first;        /* +2                                     */
    unsigned char count;        /* +3                                     */
};

struct PcxHdr {                 /* the first 0x4a bytes of a PCX header   */
    char maker;                 /* +0   0x0a, not read here               */
    char version;               /* +1   not read here                     */
    char encoding;              /* +2   1: RLE                            */
    char bpp;                   /* +3   bits a pixel a plane              */
    int xmin;                   /* +4                                     */
    int ymin;                   /* +6                                     */
    int xmax;                   /* +8                                     */
    int ymax;                   /* +a                                     */
    int hdpi;                   /* +c   not read here                     */
    int vdpi;                   /* +e   not read here                     */
    unsigned char colormap[0x30]; /* +10  the 16-colour palette          */
    char reserved;              /* +40  not read here                     */
    char planes;                /* +41                                    */
    int line_bytes;             /* +42  bytes a plane row                 */
    int pal_info;               /* +44  not read here                     */
    int h_size;                 /* +46  not read here                     */
    int v_size;                 /* +48  not read here                     */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far blit_bytes(void far *p, int a, int b, int c);  /* 00d7:0286 */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern char far *far pcx_unpack(char far *in, char far *out, int n); /* 00d7:2d35 */
extern void far clear_screen(int a);                             /* 0da7:029d */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern int  far screen_cols(void);                              /* 0f5e:05d4 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far video_plane_set(int plane);                         /* 0f5e:05ef */
extern void far video_address_set(int row, int col);                  /* 0f5e:0708 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern long far slot_used(int slot);                             /* 1336:0921 */

static void far pcx_unchain(int rows, int w, unsigned char far *dst) /* 4039:0008 */
{
    short quarter;
    short n;
    unsigned char buf[0x140];
    unsigned char far *p;
    unsigned char far *q;
    register int k;

    quarter = w >> 2;
    while (--rows >= 0) {
        q = buf;
        for (k = 0; k < 4; k++) {
            p = dst + k;
            n = quarter;
            while (--n >= 0) {
                *q++ = *p++;
                p += 3;
            }
        }
        mem_move(buf, dst, w);
        dst += w;
    }
}

int far pcx_cmap(int slot, register int n, unsigned char far *rgb) /* 4039:008c */
{
    char far *p;
    long pos;
    char mark;
    char mark_prev;

    p = slot_base(slot);
    pos = slot_used(slot) - 0x301;
    huge_move(huge_add(p, pos), &mark, 1);
    huge_move(huge_add(p, pos - 1), &mark_prev, 1);
    if (pos > 0x80 && mark == 0xc) {
        p = huge_add(p, pos + 1);
        n = int_min(n, 0x100);
    } else if (pos > 0x81 && mark_prev == 0xc) {
        p = huge_add(p, pos);
        n = int_min(n, 0x100);
    } else {
        p = huge_add(p, 0x10L);
        n = int_min(n, 0x10);
    }
    huge_move(p, rgb, n * 3);
    return n;
}

int far pcx_cmap_at(int slot, long far *off)                     /* 4039:01d2 */
{
    char far *p;
    long pos;
    char mark;
    char mark_prev;

    p = slot_base(slot);
    pos = slot_used(slot) - 0x301;
    huge_move(huge_add(p, pos), &mark, 1);
    huge_move(huge_add(p, pos - 1), &mark_prev, 1);
    if (pos > 0x80 && mark == 0xc) {
        *off = pos + 1;
        return 0x100;
    } else if (pos > 0x80 && mark_prev == 0xc) {
        *off = pos;
        return 0x100;
    }
    *off = 0x10;
    return 0x10;
}

int far pcx_crng(int slot, struct CycSpec far *cyc)              /* 4039:02bc */
{
    return 0;
}

void far pcx_show(int slot)                                      /* 4039:02c3 */
{
    struct PcxHdr hdr;
    char far *p;
    short w;
    short h;
    short rowbytes;
    unsigned char line[16][0x50];
    register int i;
    register int row;

    p = slot_base(slot);
    huge_move(p, &hdr, sizeof hdr);
    w = hdr.xmax - hdr.xmin + 1;
    h = hdr.ymax - hdr.ymin + 1;
    rowbytes = hdr.planes * hdr.line_bytes;
    if (hdr.encoding != 1 || hdr.bpp != 8 || rowbytes > 0x500 || hdr.planes != 1) {
        error_box("Can't read PCX, bad header!");
        return;
    }
    clear_screen(0);
    p = huge_add(p, 0x80L);
    pointer_hide();
    for (row = 0; row < int_min(h, screen_rows()); row++) {
        p = huge_add(pcx_unpack(p, line[0], rowbytes), 0L);
        video_address_set(row, 0);
        if (is_xvga()) {
            pcx_unchain(1, 0x140, line[0]);
            for (i = 0; i < video_planes(); i++) {
                video_plane_set(i);
                blit_bytes(line[i], 1, 0x50, 0);
            }
        } else {
            if (pixel_shift())
                error_box("PCX only on 256 color");
            blit_bytes(line[0], 1, int_min(screen_cols(), w), 0);
        }
    }
    pointer_show();
}
