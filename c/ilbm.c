/* CKIT.EXE overlay section 24, module 2fb1: reading an IFF ILBM or PBM picture.
 * The picture is a file loaded whole into a cache slot (slot_base gives
 * its first byte).  ilbm_find walks the IFF chunks from a pointer: each
 * chunk is a tag and a big-endian length (swap32), padded to even;
 * it steps into a FORM and over any other chunk, and returns the length
 * of the skip-th chunk with the tag asked for, the pointer left on its
 * data, or -1L when the chunks run out.  ilbm_long reads one raw long and
 * steps the pointer.  For section 24's art import (artimp.c, which runs
 * module 3039 instead for a PCX file) and its glyph cutter (glyphcut.c):
 *
 * ilbm_show clears the screen and draws the picture's BODY row by row,
 * up to the screen's height (screen_rows), from the top-left corner.
 * BMHD must be 0x14 bytes, the FORM type ILBM (rows of planes, each
 * (width + 7) / 8 bytes rounded up to even, a mask plane after them when
 * masking bit 0 is set) or PBM (rows of one byte a pixel, width rounded
 * up to even).  Every row is unpacked with packbits_expand, whatever BMHD's
 * compression byte says.  A PBM row is first split into eight planes
 * (pixels_to_planes).  In video mode 15 (is_xvga) an ILBM row is joined
 * back into pixels (planes_to_pixels), row_unchain deals a 320-pixel row out
 * into four 0x50-byte runs of every fourth pixel, and each run goes to
 * its own plane (video_plane_set, blit_bytes).  Otherwise, with more than
 * one screen plane (video_planes), the picture's first planes go out as
 * they are; with one, by pixel_shift: 3 the first plane, 2 an error box
 * for every row, 1 through planes4_to_pixels, 0 the pixels.  pointer_hide and
 * pointer_show bracket the drawing, as in glyphcut.c.
 *
 * ilbm_cmap copies up to n RGB triples of the CMAP chunk to rgb and
 * returns the chunk's entry count (0 without one); when none of the first
 * n bytes at rgb has a low nibble set, each of those n bytes becomes
 * c | c >> 4 (n bytes, not the 3n copied, as shipped).  ilbm_cmap_at
 * stores the CMAP data's offset from the slot's base (huge_diff) and
 * returns its entry count.  ilbm_crng fills one CycSpec per CRNG chunk
 * whose low entry is below its high one and whose rate is not 0 (flags
 * bit 0 from the CRNG flags' bit 1, a rate rounded from a constant over
 * the CRNG rate, the first entry and the count), and returns how many it
 * filled.
 *
 * Section 24 (parent section 20, which is empty and has no parent; no
 * section names 24 as its parent): payload at EXE 0x07b5b0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 24.  The module is stored frame 2fb1 (load-based 3fb1),
 * EXE 0x07d7b2..0x07e038 (2,182 bytes, 7 routines), from byte 2 of the
 * frame, right after glyphcut.c's last retf at 2e59:1581, to its last
 * retf at 2fb1:0887; module 3039 starts at 3039:0008 (EXE 0x07e038, push
 * bp) with no pad between.  One Turbo C large-model _TEXT segment, built
 * with Turbo C 2.01, tcc -ml -O -Z -d; functions are defined in shipped
 * order.  Definition comments give the load-based address; extern
 * comments the far pointer as stored in the shipped call, or the DGROUP
 * offset.
 *
 * Section 24's relocation table files 71 sites under 2fb1, all in this
 * range (EXE 0x07d7ce..0x07e030), all far calls: 43 into frame 00d7, 2
 * into 0da7 (clear_screen, error_box), 18 into 0f5e and 8 into 1336
 * (slot_base, huge_diff).  The next site, EXE 0x07e0a9, is filed under
 * 3039.  The only far calls to 2fb1 in the image are artimp.c's to
 * ilbm_show, ilbm_cmap and ilbm_crng (EXE 0x07b91b, 0x07b92f, 0x07b940)
 * and glyphcut.c's two to ilbm_cmap and one to ilbm_crng (EXE 0x07d695,
 * 0x07d6b6, 0x07d72c); no vector lands here, and section 24 has no
 * descendants.  ilbm_long, ilbm_find and row_unchain are reached only by
 * near calls after their definitions, so static is by choice: the bytes
 * do not decide it.  Nothing in the image calls ilbm_cmap_at.
 *
 * FRUA's own code: the sharing map pairs all seven routines a-only, so
 * every name here is chosen from the code.  planes_to_pixels and
 * pixels_to_planes are two of the four hand-assembly routines in frame
 * 10d7 (src/asm/pixels.asm): 2bd3 joins n groups of eight plane bytes, stride apart,
 * into 8n pixel bytes, and 2c49 splits pixels into planes the same way.
 *
 * _DATA is DGROUP a790..a7ad: one literal.  Below it glyphcut.c's _DATA
 * ends at a78e and a78f is a pad byte; a7ae up is read by module 3039.
 * No _BSS: a790 is the only DGROUP address the code names.
 *
 * Forced by the bytes, each against the module's seven ranges: the
 * register parameter n in ilbm_cmap (plain, 6 of 7); *q = *q | *q >> 4
 * (|=, 3 of 7); ilbm_cmap's if (len > 0) block ahead of return 0 (an
 * early return, 3 of 7); (int)len / 3 in ilbm_cmap_at (a long division,
 * 4 of 7); ilbm_find's locals declared tag, len, left (reversed, 6 of 7)
 * and its do-while (while, 1 of 7); count = i = 0 (i = count = 0 or two
 * statements, 6 of 7); Crng's flags read as a byte (an int tested with
 * 0x200, 5 of 7); Crng.rate unsigned (int, 5 of 7); Bmhd.w unsigned (int,
 * 6 of 7); the
 * switch's cases in the order 3, 2, 1, 0 (case 3 last, 6 of 7).
 *
 * Not decided by the bytes (each bakes 7 of 7): line[8][0x50] or a flat
 * line[0x280] indexed i * 0x50; short or int for the stack ints; register or plain k and w in row_unchain; *q++ = *p++ or
 * three statements; acc |= *q++ or two statements; the assignments inside
 * the form, rowbytes and pbm_w tests or separate statements; else return
 * last or first; >> 1 or / 2; len - 4 or len + -4; char or unsigned char
 * for is_pbm and the row buffers; long or unsigned long for ilbm_long,
 * the tag, id and ilbm_cmap_at's offset; int or char for ilbm_crng's
 * result; 0x14, 0x2d0, 8 and 4 or their sizeof; static or public for the
 * three helpers.
 */

struct CycSpec {                /* one colour cycle, as pal_set takes it  */
    unsigned char flags;        /* +0   bit 0: CRNG flag bit 1            */
    unsigned char rate;         /* +1                                     */
    unsigned char first;        /* +2                                     */
    unsigned char count;        /* +3                                     */
};

struct Crng {                   /* an ILBM CRNG chunk, big-endian words   */
    int pad;                    /* +0   not read here                     */
    unsigned rate;              /* +2                                     */
    char flags_hi;              /* +4   the flags word's high byte        */
    unsigned char flags;        /* +5   its low byte: bit 1 tested        */
    unsigned char low;          /* +6                                     */
    unsigned char high;         /* +7                                     */
};

struct Bmhd {                   /* an ILBM BMHD chunk, big-endian words   */
    unsigned w;                 /* +0   width in pixels                   */
    int h;                      /* +2   height in rows                    */
    int x;                      /* +4   swapped, not read                 */
    int y;                      /* +6   swapped, not read                 */
    unsigned char planes;       /* +8                                     */
    unsigned char masking;      /* +9   bit 0: one more plane per row     */
    unsigned char compression;  /* +a   not read here                     */
    unsigned char pad;          /* +b                                     */
    int transparent;            /* +c   swapped, not read                 */
    unsigned char x_aspect;     /* +e                                     */
    unsigned char y_aspect;     /* +f                                     */
    int page_w;                 /* +10  swapped, not read                 */
    int page_h;                 /* +12  swapped, not read                 */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far blit_bytes(void far *p, int a, int b, int c);  /* 00d7:0286 */
extern void far planes4_to_pixels(void far *src, int stride, void far *dst, int n);
                                                                 /* 00d7:07ef */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern int  far swap16(int v);                             /* 00d7:098b */
extern long far swap32(long v);                            /* 00d7:0994 */
extern char far *far packbits_expand(char far *in, char far *out, int n); /* 00d7:0a1c */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far planes_to_pixels(int n, unsigned char far *planes, int stride,
                             unsigned char far *pixels);         /* 00d7:2bd3 */
extern void far pixels_to_planes(int n, unsigned char far *pixels, int stride,
                             unsigned char far *planes);         /* 00d7:2c49 */
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
extern long far huge_diff(char far *a, char far *b);             /* 1336:1001 */

static long far ilbm_long(char far * far *pp)                    /* 3fb1:0002 */
{
    long v;

    huge_move(*pp, &v, 4);
    *pp = huge_add(*pp, 4L);
    return v;
}

static long far ilbm_find(char far * far *pp, long id, int skip) /* 3fb1:0050 */
{
    long tag;
    long len;
    long left;

    left = 0x7fffffffL;
    do {
        tag = ilbm_long(pp);
        len = swap32(ilbm_long(pp));
        if (len & 1)
            len++;
        if (tag == id) {
            if (skip <= 0)
                return len;
            skip--;
        }
        if (tag == 0x4d524f46L) {                                /* "FORM" */
            left = len - 4;
            ilbm_long(pp);
        } else {
            left -= len + 8;
            *pp = huge_add(*pp, len);
        }
    } while (left > 0);
    return -1L;
}

static void far row_unchain(int rows, int w, unsigned char far *dst) /* 3fb1:0141 */
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

int far ilbm_cmap(int slot, register int n, unsigned char far *rgb) /* 3fb1:01c5 */
{
    char far *p;
    long len;
    short count;
    unsigned char acc;
    unsigned char far *q;
    register int i;

    p = slot_base(slot);
    len = ilbm_find(&p, 0x50414d43L, 0);                         /* "CMAP" */
    if (len > 0) {
        count = len / 3;
        n = int_min(count, n);
        huge_move(p, rgb, n * 3);
        q = rgb;
        i = n;
        acc = 0;
        while (--i >= 0)
            acc |= *q++;
        if (!(acc & 0xf)) {
            q = rgb;
            i = n;
            while (--i >= 0) {
                *q = *q | *q >> 4;
                q++;
            }
        }
        return count;
    }
    return 0;
}

int far ilbm_cmap_at(int slot, long far *off)                    /* 3fb1:02a0 */
{
    char far *p;
    long len;

    p = slot_base(slot);
    len = ilbm_find(&p, 0x50414d43L, 0);                         /* "CMAP" */
    if (len > 0) {
        *off = huge_diff(p, slot_base(slot));
        return (int)len / 3;
    }
    return 0;
}

int far ilbm_crng(int slot, struct CycSpec far *cyc)             /* 3fb1:0310 */
{
    struct Crng crng;
    char far *p;
    register int count;
    register int i;

    count = i = 0;
    for (;;) {
        p = slot_base(slot);
        if (ilbm_find(&p, 0x474e5243L, i) < 0)                   /* "CRNG" */
            return count;
        huge_move(p, &crng, 8);
        crng.rate = swap16(crng.rate);
        if (crng.low < crng.high && crng.rate != 0) {
            cyc->flags = 0;
            if (crng.flags & 2)
                cyc->flags |= 1;
            cyc->rate = ((crng.rate >> 1) + 0x48cd) / crng.rate;
            cyc->first = crng.low;
            cyc->count = crng.high - crng.low + 1;
            cyc++;
            count++;
        }
        i++;
    }
}

void far ilbm_show(int slot)                                     /* 3fb1:03c6 */
{
    struct Bmhd bmhd;
    char far *p;
    long form;
    short planes;
    short rowbytes;
    short pbm_w;
    unsigned char line[8][0x50];
    unsigned char plane[9][0x50];
    char is_pbm;
    register int i;
    register int row;

    mem_fill(plane, 0x2d0, 0);
    p = slot_base(slot);
    if (ilbm_find(&p, 0x44484d42L, 0) != 0x14)                   /* "BMHD" */
        return;
    huge_move(p, &bmhd, 0x14);
    bmhd.w = swap16(bmhd.w);
    bmhd.h = swap16(bmhd.h);
    bmhd.x = swap16(bmhd.x);
    bmhd.y = swap16(bmhd.y);
    bmhd.transparent = swap16(bmhd.transparent);
    bmhd.page_w = swap16(bmhd.page_w);
    bmhd.page_h = swap16(bmhd.page_h);
    planes = bmhd.planes;
    if (bmhd.masking & 1)
        planes++;
    if ((rowbytes = (bmhd.w + 7) >> 3) & 1)
        rowbytes++;
    clear_screen(0);
    p = slot_base(slot);
    if (ilbm_find(&p, 0x4d524f46L, 0) < 0)                       /* "FORM" */
        return;
    if ((form = ilbm_long(&p)) == 0x4d424c49L)                   /* "ILBM" */
        is_pbm = 0;
    else if (form == 0x204d4250L) {                              /* "PBM " */
        is_pbm = 1;
        if ((pbm_w = bmhd.w) & 1)
            pbm_w++;
    } else
        return;
    p = slot_base(slot);
    if (ilbm_find(&p, 0x59444f42L, 0) < 0)                       /* "BODY" */
        return;
    pointer_hide();
    for (row = 0; row < int_min(bmhd.h, screen_rows()); row++) {
        if (is_pbm) {
            p = huge_add(packbits_expand(p, line[0], pbm_w), 0L);
            pixels_to_planes(int_min(screen_cols(), bmhd.w) >> 3, line[0], 0x50, plane[0]);
        } else
            for (i = 0; i < planes; i++)
                p = huge_add(packbits_expand(p, plane[i], rowbytes), 0L);
        video_address_set(row, 0);
        if (is_xvga()) {
            if (!is_pbm)
                planes_to_pixels(int_min(screen_cols(), bmhd.w) >> 3, plane[0], 0x50,
                             line[0]);
            row_unchain(1, 0x140, line[0]);
            for (i = 0; i < video_planes(); i++) {
                video_plane_set(i);
                blit_bytes(line[i], 1, 0x50, 0);
            }
        } else if (video_planes() > 1)
            for (i = 0; i < int_min(video_planes(), bmhd.planes); i++) {
                video_plane_set(i);
                blit_bytes(plane[i], 1, int_min(screen_cols(), bmhd.w) >> 3, 0);
            }
        else
            switch (pixel_shift()) {
            case 3:
                blit_bytes(plane[0], 1, int_min(screen_cols(), bmhd.w) >> 3, 0);
                break;
            case 2:
                error_box("Don't know how to display CGA");
                break;
            case 1:
                planes4_to_pixels(plane[0], 0x50, line[0],
                             int_min(screen_cols(), bmhd.w) >> 3);
                blit_bytes(line[0], 1, int_min(screen_cols(), bmhd.w) >> 1, 0);
                break;
            case 0:
                if (!is_pbm)
                    planes_to_pixels(int_min(screen_cols(), bmhd.w + 7) >> 3, plane[0],
                                 0x50, line[0]);
                blit_bytes(line[0], 1, int_min(screen_cols(), bmhd.w), 0);
                break;
            }
    }
    pointer_show();
}
