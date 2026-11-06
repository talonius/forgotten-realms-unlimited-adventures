/* CKIT.EXE resident module 22d2: glyphs changed in place in their library.
 * glyph_mirror mirrors a glyph left to right ("Mirror" in its error text):
 * when is_xvga returns nonzero it hands mem_swap plane blocks 0
 * and 3, then 1 and 2 (and 4 and 7, 5 and 6 when a type 1 glyph carries
 * twice the planes), then every row of every plane to one of four 00d7
 * routines, chosen by is_xvga and pixel_shift.  glyph_recolor
 * ("ClrTran") runs a glyph through a table clrtran_table fills from a
 * colour map.  A palette glyph (type 8) has its first entry rewritten when
 * rows bit 0 is set and its cycles' first entries when bit 1 is, and is
 * left alone when pixel_shift is nonzero and is_xvga returns 0; types
 * 0, 1 and 5 have their pixels rewritten, and any other type raises
 * error_box first.  glyph_palette_load ("TNPalette") hands a palette glyph's RGB
 * triples and colour cycles to pal_set.
 *
 * A palette glyph's header reads differently: rows holds flags (bit 0:
 * dy and dx give the first entry and the count; bit 1: width cycles follow
 * the RGB triples), and without bit 0 the palette starts at entry 0.
 *
 * EXE 0x01512d..0x01576e, one Turbo C large-model _TEXT segment starting at
 * byte 0xd of paragraph 22d2.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 21d7's last retf is at 0x01512c, and module 2336 starts at
 * 0x01576e (2336:000e) with no stub between.  No far call to clrtran_table
 * is found in the recovered listing; its caller is the push cs / near call
 * at 22d2:032f.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module gives an address is reused from there; the
 * rest are placeholders.
 */

struct Glyph {                  /* the header before a glyph's bits */
    int rows;                   /* +0 */
    int dy;                     /* +2 */
    int dx;                     /* +4 */
    unsigned char width;        /* +6 bytes per plane row */
    unsigned char type;         /* +7 low nibble: format */
};

struct CycSpec {                /* one colour cycle, as pal_set takes it */
    unsigned char flags;        /* +0 */
    unsigned char rate;         /* +1 */
    unsigned char first;        /* +2 first palette entry */
    unsigned char count;        /* +3 */
};

extern char far *far glyph_find(char far *lib, int n, struct Glyph far *hdr); /* 11d7:0076 */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_swap(void far *a, void far *b, int n);   /* 00d7:0e16 */
extern void far row_mirror1(char far *p, int n);                /* 00d7:1f37 */
extern void far row_mirror2(char far *p, int n);                /* 00d7:1f8f */
extern void far row_mirror4(char far *p, int n);                /* 00d7:1fd8 */
extern void far row_mirror8(char far *p, int n);                /* 00d7:2018 */
extern void far row_recolor(char far *p, int n, unsigned char far *tab); /* 00d7:2046 */
extern void far plane_recolor(char far *p, int a, int b,
                             unsigned char far *tab);            /* 00d7:206e */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern void far pal_set(int first, int count, unsigned char far *rgb,
                        int nspec, struct CycSpec far *spec);    /* 1157:00b3 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */

void far glyph_mirror(char far *lib, int n)                      /* 22d2:000d */
{
    struct Glyph hdr;
    char far *p;
    short planes;
    short k;
    register int w;
    register int size;

    if ((p = glyph_find(lib, n, &hdr)) == 0)
        return;
    w = hdr.width;
    if (!pixel_shift())
        w <<= 3;
    planes = video_planes();
    if ((hdr.type & 0xf) == 1) {
        if (is_xvga())
            planes += planes;
        else
            planes++;
    } else if ((hdr.type & 0xf) && (hdr.type & 0xf) != 5)
        error_box("Invalid type (%x) to Mirror", hdr.type);
    if (is_xvga()) {
        size = hdr.width * hdr.rows;
        mem_swap(p, p + size * 3, size);
        mem_swap(p + size, p + size * 2, size);
        if (video_planes() < planes) {
            mem_swap(p + size * 4, p + size * 7, size);
            mem_swap(p + size * 5, p + size * 6, size);
        }
    }
    while (--planes >= 0) {
        k = hdr.rows;
        while (--k >= 0) {
            if (is_xvga())
                row_mirror8(p, w);
            else
                switch (pixel_shift()) {
                case 3:
                    row_mirror1(p, w);
                    break;
                case 2:
                    row_mirror2(p, w);
                    break;
                case 1:
                    row_mirror4(p, w);
                    break;
                case 0:
                    row_mirror8(p, w);
                    break;
                }
            p = huge_add(p, (long)w);
        }
    }
}

void far clrtran_table(unsigned char far *map, unsigned char far *tab) /* 22d2:01f0 */
{
    register int i;

    switch (is_xvga() ? 0 : pixel_shift()) {
    case 3:
        mem_move(map, tab, 16);
        break;
    case 2:
        for (i = 0; i < 0x100; i++)
            tab[i] = map[i & 3] | map[(i & 0xc) >> 2] << 2 |
                     map[(i & 0x30) >> 4] << 4 | map[(i & 0xc0) >> 6] << 6;
        break;
    case 1:
        for (i = 0; i < 0x100; i++)
            tab[i] = map[i & 0xf] | map[(i & 0xf0) >> 4] << 4;
        break;
    case 0:
        mem_move(map, tab, 0x100);
        break;
    }
}

void far glyph_recolor(char far *lib, int n, unsigned char far *map) /* 22d2:02f5 */
{
    struct Glyph hdr;
    struct CycSpec cyc;
    char far *p;
    unsigned char tab[0x100];
    register int i;

    if ((p = glyph_find(lib, n, &hdr)) == 0)
        return;
    clrtran_table(map, tab);
    if ((hdr.type & 0xf) == 8) {
        if (pixel_shift() && !is_xvga())
            return;
        if (hdr.rows & 1) {
            hdr.dy = tab[hdr.dy];
            huge_move(&hdr, huge_add(p, -8L), 8);
            p = huge_add(p, hdr.dx * 3L);
        } else
            p = huge_add(p, 0x300L);
        if (!(hdr.rows & 2))
            return;
        for (i = 0; i < hdr.width; i++) {
            huge_move(p, &cyc, 4);
            cyc.first = tab[cyc.first];
            huge_move(&cyc, p, 4);
            p = huge_add(p, 4L);
        }
        return;
    }
    i = hdr.width * hdr.rows;
    if (!pixel_shift())
        i <<= 3;
    if (is_xvga())
        i <<= 2;
    if ((hdr.type & 0xf) == 1)
        p = huge_add(p, (long)i);
    else if ((hdr.type & 0xf) && (hdr.type & 0xf) != 5)
        error_box("Invalid type (%x) to ClrTran", hdr.type);
    if (video_planes() == 4 && !is_xvga())
        plane_recolor(p, i, i, tab);
    else
        row_recolor(p, i, tab);
}

void far glyph_palette_load(char far *lib, int n)                      /* 22d2:0537 */
{
    struct Glyph hdr;
    struct CycSpec cyc[16];
    char far *p;
    unsigned char rgb[0x300];
    register int first;
    register int count;

    if ((p = glyph_find(lib, n, &hdr)) == 0)
        return;
    if ((hdr.type & 0xf) != 8) {
        error_box("Invalid TNPalette call %lx %d", lib, n);
        return;
    }
    if (hdr.rows & 1) {
        first = hdr.dy;
        count = hdr.dx;
    } else {
        first = 0;
        count = is_xvga() ? 0x100 : pixel_shift() ? 0x10 : 0x100;
    }
    if (!(hdr.rows & 2))
        hdr.width = 0;
    huge_move(p, rgb, count * 3);
    p = huge_add(p, (long)(count * 3));
    huge_move(p, cyc, hdr.width * 4); /* width is not bounded by 16, as shipped */
    pal_set(first, count, rgb, hdr.width, cyc);
}
