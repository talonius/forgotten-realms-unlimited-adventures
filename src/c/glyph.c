/* CKIT.EXE resident module 21d7: glyphs drawn from the tile libraries.
 * A glyph is an 8-byte header and its bits, found by index in a library
 * a slot holds.  The header's low type nibble picks the format: 9 is a
 * list of parts drawn in turn, 10 splits the glyph in two at split_rows,
 * 2, 3 and 7 are packed forms handed to decoders in frame 10d7, 0, 1 and
 * 5 are drawn plane by plane by blit_planes, which draws nothing for any
 * other type, and 8 is a palette, which pal_glyph uploads.  glyph_blit returns
 * early for a glyph wholly outside clip_top, clip_bottom, clip_left and
 * clip_right, and cuts the rest to them.  fill_pattern stores a colour's
 * color_map entry as one word when is_xvga() is set or pixel_shift()
 * is 0, and otherwise 16 words from the two dither tables; pointer_glyph_load
 * copies a glyph into glyph_buf and hands it to pointer_shape_set.  glyph_library_init
 * registers the "TILE" converter with the library layer.
 *
 * EXE 0x014178..0x01512d, one Turbo C large-model _TEXT segment starting at
 * byte 8 of paragraph 21d7.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 2157's last retf is at 0x014177, and module 22d2 starts at
 * 0x01512d (22d2:000d) with no stub between.  Ghidra's index misses
 * glyph_library_init, far-called from 1da7:0064 at 0x00fed4.
 *
 * _DATA is 9f04..a1b7: module 2157's last literal, read at 2157:03c4, ends
 * at 9f02 and 9f03 is the one pad byte alignment allows, so blank_9f04 is
 * this module's.  _BSS follows the statics' names (Turbo C's symbol-table
 * order), not their declaration order; these names reproduce the shipped
 * e08c..e293 layout, checked with goldbox.bss_order.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call or pushed pointer; a variable's is its DGROUP offset.  A name another module gives an
 * address is reused from there; a modNNNN_ name is a placeholder that says
 * only which segment the callee lives in.
 */

struct Glyph {                  /* the header before a glyph's bits */
    int rows;                   /* +0 */
    int dy;                     /* +2 subtracted from the row */
    int dx;                     /* +4 subtracted from the column */
    unsigned char width;        /* +6 bytes per plane row */
    unsigned char type;         /* +7 low nibble: format; 0x80 one plane; 0x40 to blit_packed */
};

struct Part {                   /* one part of a type 9 glyph */
    unsigned char glyph;        /* +0 */
    unsigned char count;        /* +1 the loop bound, re-read from each part */
    int dy;                     /* +2 */
    int dx;                     /* +4 */
};

struct Pointer {                /* a glyph copied for pointer_shape_set */
    char hot_row;               /* e08e */
    char hot_col;               /* e08f */
    char rows;                  /* e090 */
    char cols;                  /* e091 */
    unsigned char mask[256];    /* e092 */
    unsigned char image[4][64];  /* e192 */
};

extern void far lib_register(long tag,
                             long (far *fn)(int slot, long off, long len)); /* 147e:0023 */
extern void far lib_open(char mode, char far *name, int slot);   /* 147e:00d1 */
extern char far *far lib_item(void far *lib, int n);             /* 147e:01f9 */
extern long far tile_loader_identity(int slot, long off, long len);     /* 05f5:0d37 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern char far path_default_ext(char far *path, char far *ext, char force); /* 17a1:0264 */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far blit_words(void far *p, int a, int b, int c);  /* 00d7:0241 */
extern void far blit_bytes(void far *p, int a, int b, int c);  /* 00d7:0286 */
extern void far blit_masked_w(void far *p, void far *q, int a, int b, int c); /* 00d7:02ba */
extern void far blit_masked_b(void far *p, void far *q, int a, int b, int c); /* 00d7:030a */
extern void far rle_draw(int top, int rows, int left, int width,
                             void far *q, void far *p);          /* 00d7:057e */
extern char far *far packbits_expand(char far *in, char far *out, int n); /* 00d7:0a1c */
extern void far split_nibbles(char far *in, char far *out, int n); /* 00d7:0a5c */
extern void far nibbles_to_planar(char far *in, char far *out, int stride, int n); /* 00d7:0a80 */
extern char far *far runs_draw(int top, int bottom, int rows, int left,
                                  int width, char far *p);       /* 00d7:0db6 */
extern void far keyed_blit(void far *p, int a, int b, int c, char d); /* 00d7:212e */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern int  far screen_cols(void);                              /* 0f5e:05d4 */
extern void far video_plane_set(int plane);                         /* 0f5e:05ef */
extern void far video_address_set(int row, int col);                  /* 0f5e:0708 */
extern char far draw_page_visible(void);                              /* 0f5e:03d2 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far pointer_hide_rect(int row1, int col1, int row2, int col2); /* 0f5e:0e12 */
extern void far pointer_shape_set(struct Pointer far *g);             /* 0f5e:1189 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far palette_load(unsigned char far *pal);            /* 107e:03ce */
extern void far palette_page_load(unsigned char far *pal, char flag); /* 107e:0480 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */

extern int video_mode;                                           /* d61b */
extern int clip_top;                                             /* d988 */
extern int clip_left;                                            /* d98d */
extern int clip_right;                                           /* d98f */
extern int clip_bottom;                                          /* d993 */

static char blank_9f04[4] = {0};             /* 9f04, no reference found; type a guess */
static unsigned char dith4[96][4] = {                            /* 9f08, up to dith8 */
    {0x00, 0x00, 0x00, 0x00}, {0x80, 0x00, 0x00, 0x00}, {0x80, 0x00, 0x08, 0x00}, {0x80, 0x04, 0x20, 0x00},
    {0x88, 0x00, 0x22, 0x00}, {0x88, 0x00, 0x88, 0x00}, {0x80, 0x08, 0x80, 0x08}, {0x82, 0x28, 0x82, 0x00},
    {0x4a, 0x00, 0xa4, 0x00}, {0xa8, 0x00, 0x45, 0x00}, {0xaa, 0x00, 0xaa, 0x00}, {0xaa, 0x00, 0x55, 0x00},
    {0x88, 0x22, 0x44, 0x11}, {0x88, 0x22, 0x88, 0x22}, {0x88, 0x88, 0x22, 0x22}, {0x24, 0x90, 0x24, 0x90},
    {0x82, 0x28, 0x82, 0x28}, {0xa2, 0x44, 0x2a, 0x11}, {0x88, 0x22, 0x88, 0x55}, {0x88, 0xaa, 0x88, 0x22},
    {0x8a, 0x22, 0xa8, 0x22}, {0xa2, 0xa0, 0x2a, 0x0a}, {0xa8, 0xa2, 0x8a, 0x2a}, {0x94, 0x52, 0x49, 0x25},
    {0xaa, 0x11, 0xaa, 0x44}, {0xaa, 0x22, 0xaa, 0x88}, {0xaa, 0x15, 0xaa, 0x51}, {0xaa, 0x25, 0xaa, 0x52},
    {0xaa, 0x2a, 0xaa, 0xa2}, {0xaa, 0x4a, 0xaa, 0xa4}, {0xaa, 0x33, 0xcc, 0x55}, {0xaa, 0x55, 0xaa, 0x55},
    {0xaa, 0xaa, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa}, {0xaa, 0x55, 0xaa, 0xaa}, {0xda, 0xaa, 0xad, 0xaa},
    {0xba, 0xaa, 0xab, 0xaa}, {0xb5, 0xaa, 0x5b, 0xaa}, {0x75, 0xaa, 0x57, 0xaa}, {0xee, 0xaa, 0xbb, 0xaa},
    {0xaa, 0x77, 0xaa, 0xdd}, {0x5b, 0x6d, 0xb5, 0xd6}, {0xab, 0xae, 0xba, 0xea}, {0xbb, 0xea, 0xbb, 0xae},
    {0xbb, 0xee, 0xaa, 0xee}, {0x55, 0xee, 0xbb, 0xee}, {0x5d, 0xbb, 0xd5, 0xee}, {0xeb, 0xbe, 0xeb, 0xbe},
    {0x6f, 0xbd, 0xf6, 0xdb}, {0xbb, 0xbb, 0xee, 0xee}, {0xbb, 0xee, 0xbb, 0xee}, {0x77, 0xdd, 0xbb, 0xee},
    {0xff, 0x55, 0xff, 0xaa}, {0xff, 0xaa, 0xff, 0xaa}, {0xff, 0x5d, 0xff, 0xba}, {0xff, 0xea, 0xff, 0x5d},
    {0xff, 0xbe, 0xeb, 0xbe}, {0xef, 0xfe, 0xef, 0xfe}, {0xff, 0xee, 0xff, 0xee}, {0xff, 0xbb, 0xff, 0xee},
    {0xff, 0xfb, 0xdf, 0xfe}, {0xff, 0xef, 0xff, 0xfe}, {0xff, 0xff, 0xff, 0xfe}, {0xff, 0xff, 0xff, 0xff},
    {0x55, 0x00, 0x55, 0x00}, {0x00, 0xaa, 0x00, 0xaa}, {0x00, 0x55, 0x00, 0x55}, {0xff, 0x00, 0xff, 0x00},
    {0x55, 0xaa, 0x55, 0xaa}, {0x00, 0xff, 0x00, 0xff}, {0x55, 0x55, 0x55, 0x55}, {0xaa, 0xff, 0xaa, 0xff},
    {0x55, 0xff, 0x55, 0xff}, {0xbb, 0xff, 0xee, 0xff}, {0xaa, 0x00, 0xaa, 0x00}, {0xaa, 0x00, 0x55, 0x00},
    {0x88, 0x22, 0x44, 0x11}, {0x88, 0x22, 0x88, 0x22}, {0x88, 0x88, 0x22, 0x22}, {0x24, 0x90, 0x24, 0x90},
    {0x82, 0x28, 0x82, 0x28}, {0xa2, 0x44, 0x2a, 0x11}, {0x88, 0x22, 0x88, 0x55}, {0x88, 0xaa, 0x88, 0x22},
    {0x8a, 0x22, 0xa8, 0x22}, {0xa2, 0xa0, 0x2a, 0x0a}, {0xa8, 0xa2, 0x8a, 0x2a}, {0x94, 0x52, 0x49, 0x25},
    {0xaa, 0x11, 0xaa, 0x44}, {0xaa, 0x22, 0xaa, 0x88}, {0xaa, 0x15, 0xaa, 0x51}, {0xaa, 0x25, 0xaa, 0x52},
    {0xaa, 0x2a, 0xaa, 0xa2}, {0xaa, 0x4a, 0xaa, 0xa4}, {0xaa, 0x33, 0xcc, 0x55}, {0xaa, 0x55, 0xaa, 0x55},
};
static unsigned char dith8[32][8] = {                            /* a088 */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0x11, 0x11, 0x40, 0x04, 0x22, 0x22, 0x44, 0x11}, {0x99, 0x99, 0x25, 0x52, 0x33, 0x33, 0x55, 0x55},
    {0xbb, 0xbb, 0xf5, 0x5f, 0xaa, 0xaa, 0x77, 0xdd}, {0x22, 0x22, 0x11, 0x00, 0x11, 0x11, 0x01, 0x10},
    {0x77, 0x77, 0xd5, 0x5d, 0x99, 0x99, 0x77, 0x11}, {0x4c, 0xc4, 0xaa, 0x45, 0x44, 0x44, 0xaa, 0xaa},
    {0xcc, 0xcc, 0xda, 0xad, 0xcc, 0xcc, 0xbb, 0xee}, {0xdd, 0xdd, 0xee, 0xff, 0xee, 0xee, 0xee, 0xff},
    {0xaa, 0xaa, 0x18, 0xc2, 0x88, 0x88, 0x0c, 0xc0}, {0x5a, 0xa5, 0x5f, 0xea, 0x77, 0x77, 0x33, 0xcc},
    {0x88, 0x88, 0x88, 0x22, 0x44, 0x44, 0x20, 0x02}, {0x44, 0x44, 0xaa, 0x55, 0x66, 0x66, 0xee, 0x99},
    {0x45, 0x54, 0xa0, 0x0a, 0x55, 0x55, 0x88, 0x22}, {0xee, 0xee, 0x77, 0xdd, 0xcc, 0xcc, 0xff, 0xff},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}, {0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa},
    {0x14, 0x41, 0x14, 0x41, 0x14, 0x41, 0x14, 0x41}, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};
static unsigned char color_map[16] = {                           /* a188 */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static int pat_ofs;                                              /* e08c, written only */
static struct Pointer glyph_buf;                                 /* e08e */
static int split_rows;                                           /* e292 */

void far glyph_library_init(void)                                        /* 21d7:0008 */
{
    lib_register(0x454c4954L, tile_loader_identity);
    split_rows = 8;
    pat_ofs = (video_mode - 1) * 2;
}

void far glyph_library_done(void)                                        /* 21d7:0030 */
{
}

void far glyph_library_open(char mode, char far *name, int slot)         /* 21d7:0031 */
{
    char path[80];

    str_copy(path, name);
    path_default_ext(path, "TLB", 1);
    lib_open(mode, path, slot);
}

char far *far glyph_find(char far *lib, int n, struct Glyph far *hdr) /* 21d7:0076 */
{
    char far *p;

    if (n >= 0)
        p = lib_item(lib, n);
    else
        p = lib;
    huge_move(p, hdr, 8);
    return huge_add(p, 8L);
}

char far *far glyph_words(char far *p, int far *a, int far *b)  /* 21d7:00d5 */
{
    int w[2];

    huge_move(p, w, 4);
    *a = w[0];
    *b = w[1];
    return huge_add(p, 4L);
}

void far color_map_set(char far *map)                            /* 21d7:011d */
{
    mem_move(map, color_map, 16);
}

void far fill_pattern(int color, int far *out)                   /* 21d7:0138 */
{
    short v;
    short mask;
    register int i;
    register int ofs;

    if (video_mode <= 4) {
        ofs = (video_mode - 1) * 2;
        mask = 1;
    } else {
        ofs = 8;
        mask = 3;
    }
    color &= 0xf;
    if (is_xvga() || !pixel_shift()) {
        *out = color_map[color];
        return;
    }
    for (i = 0; i < 16; i++) {
        if (ofs == 8)
            v = dith4[color_map[color] & 0x7f][i & mask]; /* rows past 95 read dith8 */
        else
            v = dith8[color_map[color] & 0x1f][(i & mask) + ofs];
        *out = v << 8 | v;
        out++;
    }
}

void far blit_planes(char far *p, short row, short col, int stride, short rows,
                     short width, short planes, short skip,
                     unsigned char type)                         /* 21d7:0205 */
{
    char far *q;
    register int i;

    switch (type) {
    case 0:
        video_address_set(row, col);
        if (!(width & 1)) {
            for (i = 0; i < planes; i++) {
                video_plane_set(i);
                blit_words(p, rows, width, skip);
                p += stride;
            }
        } else {
            for (i = 0; i < planes; i++) {
                video_plane_set(i);
                blit_bytes(p, rows, width, skip);
                p += stride;
            }
        }
        break;
    case 1:
        video_address_set(row, col);
        if (is_xvga()) {
            q = p;
            p += stride * planes;
            for (i = 0; i < planes; i++) {
                video_plane_set(i);
                blit_masked_w(p, q, rows, width, skip);
                p += stride;
                q += stride;
            }
        } else if (!(width & 1)) {
            q = p;
            for (i = 0; i < planes; i++) {
                p += stride;
                video_plane_set(i);
                blit_masked_w(p, q, rows, width, skip);
            }
        } else {
            q = p;
            for (i = 0; i < planes; i++) {
                p += stride;
                video_plane_set(i);
                blit_masked_b(p, q, rows, width, skip);
            }
        }
        break;
    case 5:
        video_address_set(row, col);
        for (i = 0; i < planes; i++) {
            video_plane_set(i);
            keyed_blit(p, rows, width, skip, 0xff);
            p += stride;
        }
        break;
    }
}

void far blit_runs(int row, int col, int rows, int width, int planes, int top,
                   int left, int bottom, int right, char far *p) /* 21d7:03bb */
{
    register int i;

    video_address_set(row, col);
    for (i = 0; i < planes; i++) {
        video_plane_set(i);
        p = runs_draw(top, bottom, rows, left, width - left - right, p);
    }
}

void far blit_packed(short row, int col, short rows, short width, short planes,
                     short top, short left, int right, char far *p,
                     char flag)                                  /* 21d7:040a */
{
    short k;
    char line[0x140];
    char buf[8][40];
    register int n;
    register int m;

    n = width * planes;
    if (video_mode == 14 && !flag)
        n >>= 1;
    if (video_planes() != planes && flag)
        n <<= 2;
    m = width - left - right;
    while (--top >= 0)
        p = packbits_expand(p, line, n);
    if (video_mode == 15) {
        flag = 1;
        planes = 1;
    }
    if (video_mode == 14 && !flag) {
        while (--rows >= 0) {
            p = packbits_expand(p, line, n);
            video_address_set(row++, col);
            split_nibbles(line, buf[0], n);
            blit_words(buf[0] + left, 1, m, 0);
        }
    } else if (planes == 4) {
        while (--rows >= 0) {
            p = packbits_expand(p, line, n);
            video_address_set(row++, col);
            nibbles_to_planar(line, buf[0], 40, n);
            for (k = 0; k < planes; k++) {
                video_plane_set(k);
                blit_bytes(buf[k] + left, 1, m, 0);
            }
        }
    } else if (video_planes() == 4 && flag) {
        if (!(m & 1) && !(left & 1) && !(width & 1)) {
            while (--rows >= 0) {
                p = packbits_expand(p, line, n);
                video_address_set(row++, col);
                video_plane_set(0);
                blit_words(line + left, 1, m, 0);
                video_plane_set(1);
                blit_words(line + (width + left), 1, m, 0);
                video_plane_set(2);
                blit_words(line + (width + width + left), 1, m, 0);
                video_plane_set(3);
                blit_words(line + (width + width + width + left), 1, m, 0);
            }
        } else {
            while (--rows >= 0) {
                p = packbits_expand(p, line, n);
                video_address_set(row++, col);
                for (k = 0; k < 4; k++) {
                    video_plane_set(k);
                    blit_bytes(line + (width * k + left), 1, m, 0);
                }
            }
        }
    } else if (!(m & 1) && !(left & 1)) {
        video_plane_set(0);
        while (--rows >= 0) {
            p = packbits_expand(p, line, n);
            video_address_set(row++, col);
            blit_words(line + left, 1, m, 0);
        }
    } else {
        video_plane_set(0);
        while (--rows >= 0) {
            p = packbits_expand(p, line, n);
            video_address_set(row++, col);
            blit_bytes(line + left, 1, m, 0);
        }
    }
}

void far glyph_blit(char far *p, register int rows, short width, short row,
                    short col, unsigned char type)               /* 21d7:07cb */
{
    char far *q;
    short left;
    short bottom;
    short right;
    short k;
    short planes;
    short size;
    short off;
    char flag;
    register int cut;

    if (row >= clip_bottom)
        return;
    if (row + rows <= clip_top)
        return;
    if (col >= clip_right)
        return;
    if ((width << pixel_shift()) + col <= clip_left)
        return;
    planes = (type & 0x80) ? 1 : video_planes();
    flag = type & 0x40;
    type &= 0xf;
    if (type == 10) {
        cut = split_rows;
        switch (planes) {
        case 1:
            size = width * 2 * (rows - cut);
            k = 0x80;
            break;
        case 4:
            k = 0;
            size = (rows - cut) * width * 5;
            break;
        }                               /* other counts leave size and k unset, as shipped */
        if (rows > cut)
            glyph_blit(p, rows - cut, width, row, col, k + 1);
        glyph_blit(p + size, cut, width, row + rows - cut, col, k);
        return;
    }
    size = rows * width;
    if (row < clip_top)
        cut = clip_top - row;
    else
        cut = 0;
    if (row + rows > clip_bottom) {
        bottom = rows - (clip_bottom - row);
        rows = clip_bottom - row;
    } else
        bottom = 0;
    if (col < clip_left)
        left = (clip_left - col) >> pixel_shift();
    else
        left = 0;
    if ((width << pixel_shift()) + col > clip_right)
        right = ((width << pixel_shift()) + col - clip_right) >> pixel_shift();
    else
        right = 0;
    row += cut;
    col = int_max(col, clip_left);
    rows -= cut;
    if (type == 3) {
        q = glyph_words(p, &off, &size);
        p = q + off;
        for (k = 0; k < planes; k++) {
            video_plane_set(k);
            video_address_set(row, col);
            rle_draw(cut, rows, left, width - left - right, q, p);
            p += size;
        }
    } else if (type == 2)
        blit_packed(row, col, rows, width, planes, cut, left, right, p, flag);
    else if (type == 7)
        blit_runs(row, col, rows, width, planes, cut, left, bottom, right, p);
    else {
        p += cut * width + left;
        right += left;
        width -= right;
        if (rows > 0 && width > 0)
            blit_planes(p, row, col, size, rows, width, planes, right, type);
    }
}

void far glyph_draw_at(int row, int col, char far *lib, int glyph) /* 21d7:0a9d */
{
    struct Glyph hdr;
    struct Part part;
    char far *p;
    register int n;
    register int i;

    text_xy(row, col, &row, &col);
    if ((p = glyph_find(lib, glyph, &hdr)) == 0)
        return;
    row -= hdr.dy;
    col -= hdr.dx;
    if ((hdr.type & 0xf) == 9) {
        for (n = 1, i = 0; i < n; i++) {
            huge_move(p, &part, 6);
            p = huge_add(p, 6L);
            glyph_draw_at(row + part.dy, col + part.dx, lib, part.glyph);
            n = part.count;
        }
        return;
    }
    n = hdr.width;
    if (!pixel_shift())
        n <<= 3;
    if (draw_page_visible())
        pointer_hide_rect(row, col, row + hdr.rows, (n << pixel_shift()) + col);
    glyph_blit(p, hdr.rows, n, row, col, hdr.type);
    if (draw_page_visible())
        pointer_show();
}

void far draw_glyph(int row, int col, int n, int glyph)          /* 21d7:0bd1 */
{
    glyph_draw_at(row, col, slot_base(n), glyph);
}

void far glyph_bounds(int row, int col, int slot, int glyph, int far *top,
                      int far *left, int far *bottom, int far *right) /* 21d7:0bf0 */
{
    struct Glyph hdr;
    register int r;
    register int b;

    if (glyph_find(slot_base(slot), glyph, &hdr) == 0)
        return;
    text_xy(row, col, &row, &col);
    col -= hdr.dx;
    row -= hdr.dy;
    b = row + hdr.rows;
    if (!pixel_shift())
        r = (hdr.width << 3) + col;
    else
        r = (hdr.width << pixel_shift()) + col;
    *top = int_max(row, clip_top);
    *left = int_max(col, clip_left);
    *bottom = int_min(b, clip_bottom);
    *right = int_min(r, clip_right);
    if (*top >= *bottom || *left >= *right)
        *top = *bottom = *left = *right = 0;
}

void far pointer_glyph_load(int slot, int n)                           /* 21d7:0cf9 */
{
    struct Glyph hdr;
    char far *p;
    short size;
    unsigned char far *src;
    unsigned char far *dst;
    register int i;
    register int count;

    if ((p = glyph_find(slot_base(slot), n, &hdr)) == 0)
        return;
    if (video_mode == 14)
        hdr.width <<= 3;
    count = hdr.width * hdr.rows;
    size = count;
    hdr.width <<= pixel_shift();
    if (video_planes() * size > 0x100) {
        hdr.rows = int_min(hdr.rows, video_planes() > 1 ? 16 : 24);
        hdr.width = int_min(hdr.width, screen_cols() > 400 ? 32 : 16);
        count = 0x100 / video_planes();
    }
    mem_fill(&glyph_buf, sizeof(glyph_buf), 0);
    glyph_buf.hot_row = hdr.dy;
    glyph_buf.hot_col = hdr.dx;
    glyph_buf.rows = hdr.rows;
    glyph_buf.cols = hdr.width;
    huge_move(p, glyph_buf.mask, (is_xvga() ? video_planes() : 1) * count);
    p = huge_add(p, (long)size * (is_xvga() ? video_planes() : 1));
    for (i = 0; i < int_min(video_planes(), 4); i++) {
        if (i == 0 || !(hdr.type & 0xe0) || (hdr.type & 0xf0) >= 0xd0)
            huge_move(p, glyph_buf.image[i], count);
        else
            mem_fill(glyph_buf.image[i], count, 0);
        p = huge_add(p, (long)size);
    }
    if ((hdr.type & 0xf) == 5) {
        src = glyph_buf.mask;
        dst = glyph_buf.image[0];
        for (i = 0; i < 0x100; i++) {
            if (*src == 0xff) {
                *dst++ = 0;
                src++;
            } else {
                *dst++ = *src;
                *src++ = 0;
            }
        }
    }
    pointer_shape_set(&glyph_buf);
}

void far pal_glyph(int slot, int n, char fade, char flag)        /* 21d7:0f28 */
{
    struct Glyph hdr;
    char far *p;
    unsigned char pal[0x300];

    if ((p = glyph_find(slot_base(slot), n, &hdr)) != 0) {
        if ((hdr.type & 0xf) != 8)
            error_box("Invalid TPalette call %d %d", slot, n);
        else {
            huge_move(p, pal, 0x300);
            if (fade)
                palette_page_load(pal, flag);
            else
                palette_load(pal);
        }
    }
}
