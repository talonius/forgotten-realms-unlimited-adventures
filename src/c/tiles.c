/* CKIT.EXE resident module 15f5: tile libraries, palettes and the screen
 * flush.  Opening a .tlb tile library into a slot, the resource handles
 * that name a slot and an index in it, glyph drawing through those
 * handles, the palette buffer and its upload, and the per-plane row
 * buffers.  When the set is not already in its slot, tile_open_file
 * first tries it as a loose file, design_dir joined to
 * "<name><bank><set>.tlb" with <name> the name's first four characters,
 * which grp_read reads from the open position to the end into the
 * cache; only when that fails does it open the library, the name without
 * its digit plus ".tlb".  glyph_size puts the corners glyph_bounds gives
 * through text_xy_to and returns their differences.  pal_black sets
 * palette entries first..last to black and calls pal_commit when
 * is_xvga() || !pixel_shift(), the test screen_flush and res_palette_load
 * also make.
 *
 * EXE 0x00835b..0x009092, one Turbo C large-model _TEXT segment starting at
 * byte b of paragraph 15f5.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 13d0's last retf is at 0x00835a, and module 16c9 starts at
 * 0x009092 (16c9:0002) with no stub between.  Nothing found calls
 * tile_open_set (040d), res_index_find or pal_fade (0859): no far call, far
 * jump, callback push or far-pointer dword in CKIT.EXE names them.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  A name another module here gives
 * an address is reused from there; the lib_ names are library.c's (module
 * 247e), pal_commit is palette.c's (module 2157), and tile_loader_identity is
 * glyph.c's name for this module's own lib_register callback.  A modNNNN_
 * name is a placeholder that says only which segment the callee lives in,
 * and tiles_NNNN only where in this module the routine starts.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is: glyph_cell
 * and glyph_at far-call screen_draw_begin, and screen_flush_mode_set far-calls
 * screen_flush, so those two carry prototypes.
 *
 * _DATA is 1c42..1c68: five initialized statics and four literals; 1c69
 * is pad.
 * _BSS follows the statics' names (Turbo C's symbol-table order), not their
 * declaration order; these names reproduce the shipped a7f8..ab73 layout.
 * The originals are not recoverable from it.  rows_saved's [5] is what fits
 * between a863 and resources at ab33; no code here bounds the plane count.
 *
 * 0x454c4954L is the tag "TILE" as four bytes.  Cell coordinates reach
 * draw_glyph, fill_rect and text_xy offset by 0x1f40; what that offset
 * selects in the callees is not established here.
 */

struct Res {                    /* a resource: a slot and an index in it  */
    int slot;                   /* +0  -1 when free                       */
    int index;                  /* +2                                     */
    char tail[2];               /* +4  not read here                      */
};

struct CycSpec;                 /* palette.c's colour cycle               */

extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far grp_pop(void);                               /* 1336:06c4 */
extern char far slot_open(char far *name, int slot);             /* 1336:025c */
extern void far slot_release(char far *name);                    /* 1336:0708 */
extern char far grp_read(int fd, long n);                     /* 1336:0797 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern long far slot_used(int slot);                             /* 1336:0921 */
extern char far lib_load(int fd, long pos, int slot);            /* 147e:006d */
extern int  far lib_items(void far *lib);                        /* 147e:0170 */
extern char far *far lib_item(void far *lib, int n);             /* 147e:01f9 */
extern long far lib_item_size(void far *lib, int n);             /* 147e:029d */
extern int  far lib_itype(void far *lib);                        /* 147e:0339 */
extern int  far lib_index_find(void far *lib, int id);           /* 147e:037c */
extern int  far lib_find(int fd, int id);                        /* 147e:0415 */
extern long far lib_find_item(int fd, int n);                    /* 147e:04c5 */
extern void far lib_create(char far *name, int slot, long tag); /* 157c:000a */
extern void far lib_insert(int slot, int n, long size);        /* 157c:0081 */
extern void far lib_resize(int slot, int n, long size);        /* 157c:037e */
extern char far lib_read_convert(int fd, long len, int slot, int n, long tag); /* 157c:04f1 */
extern void far glyph_mirror(char far *lib, int n);              /* 12d2:000d */
extern void far glyph_recolor(char far *lib, int n, unsigned char far *map); /* 12d2:02f5 */
extern void far glyph_palette_load(char far *lib, int n);              /* 12d2:0537 */
extern long far file_seek(int fd, long pos, int whence);      /* 00d7:2705 */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern void far blit_bytes(void far *p, int a, int b, int c);  /* 00d7:0286 */
extern void far grab_words(void far *p, int a, int b);         /* 00d7:09e4 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */
extern void far glyph_library_open(char mode, char far *name, int slot); /* 11d7:0031 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far glyph_bounds(int row, int col, int slot, int glyph, int far *top,
                             int far *left, int far *bottom, int far *right); /* 11d7:0bf0 */
extern void far screen_page_save(void);                              /* 0f5e:0291 */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern void far video_plane_set(int plane);                         /* 0f5e:05ef */
extern void far video_address_set(int row, int col);                  /* 0f5e:0708 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far pointer_hide_rect(int row1, int col1, int row2, int col2); /* 0f5e:0e12 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far palette_load(unsigned char far *pal);            /* 107e:03ce */
extern void far palette_page_load(unsigned char far *pal, char flag); /* 107e:0480 */
extern void far pal_set(int first, int count, unsigned char far *rgb,
                        int nspec, struct CycSpec far *spec);    /* 1157:00b3 */
extern void far pal_commit(void);                              /* 1157:0457 */

extern char design_dir[];                                          /* 5a54 */

void far screen_draw_begin(char on);
void far screen_flush(void);

static char open_mode = '1';                                     /* 1c42 */
static char text_on = 0;                                         /* 1c43 */
static char flush_mode = 0;                                      /* 1c44 */
static char pal_dirty = 0;                                       /* 1c45 */
static long cur_used = -1L;                                      /* 1c46 */

static char pal_buf[96];                                         /* a7f8 */
static struct Res far *cur_res;                                  /* a858 */
static char far *open_name;                                      /* a85c */
static char lib_open;                                            /* a860 */
static int tile_bank;                                            /* a861 */
static char rows_saved[5][144];                                  /* a863 */
static struct Res resources[10];                                 /* ab33 */
static int tile_slot;                                            /* ab6f */
static int tile_set_num;                                         /* ab71 */
static char tile_load_mode;                                      /* ab73 */

void far res_free(struct Res far * far *pp)                      /* 15f5:000b */
{
    if (*pp != 0) {
        if ((*pp)->slot >= 0)
            slot_close((*pp)->slot);
        (*pp)->slot = -1;
        *pp = 0;
    }
}

char far tile_load(int fd, char far *path)                       /* 15f5:004d */
{
    int n;
    long pos;

    if ((n = lib_find(fd, tile_set_num)) <= 0 &&
        (n = lib_find(fd, tile_bank * 100 + tile_set_num)) <= 0)
        return 0;
    if ((pos = lib_find_item(fd, n)) < 0)
        return 0;
    if (tile_load_mode) {
        grp_pop();
        lib_create(open_name, tile_slot, 0x454c4954L);
        lib_insert(tile_slot, 0, 0L);
        if (lib_read_convert(fd, pos, tile_slot, 0, 0x454c4954L)) {
            if (tile_load_mode != 2)
                return 1;
            lib_insert(tile_slot, 1, 0L);
            if (file_seek(fd, 0L, 0) >= 0 &&
                (n = lib_find(fd, tile_set_num + 0x80)) > 0 &&
                (pos = lib_find_item(fd, n)) >= 0 &&
                lib_read_convert(fd, pos, tile_slot, 1, 0x454c4954L))
                return 1;
        }
        slot_close(tile_slot);
        slot_release(open_name);
        return 0;
    }
    if (!lib_load(fd, pos, tile_slot))
        return 0;
    return 1;
}

void far set_open_mode(char mode)                                   /* 15f5:01d3 */
{
    open_mode = mode;
}

void far tile_open_file(char far *name, unsigned char set, char kind, char depth,
                        struct Res far * far *pp)                /* 15f5:01de */
{
    short bank;
    char buf[80];
    char tmp[14];
    char path[80];
    int i;
    int n;

    if (*pp != 0)
        res_free(pp);
    for (i = 0; i < 10 && resources[i].slot >= 0; i++)
        ;
    *pp = &resources[i];
    (*pp)->slot = i + 2;
    (*pp)->index = set;
    n = str_len(name) - 1;
    if (name[n] >= '0' && name[n] <= '9') {
        (*pp)->index = 0;
        bank = name[n] - '0';
        str_copy(tmp, name);
        tmp[n] = 0;
        str_format(buf, "%s.tlb", tmp);
        str_copy(tmp, name);
        tmp[4] = 0;
        str_format(tmp, "%s%d%03d.tlb", tmp, bank, set);
        if (!slot_open(tmp, i + 2)) {
            lib_open = 1;
            path[0] = 0;
            path_append_name(path, design_dir);
            path_append_name(path, tmp);
            if ((n = dos_open(0x3d00, path)) >= 0) {
                if (grp_read(n, -1L)) {
                    dos_call(0x3e00, 0, n);
                    return;
                }
                dos_call(0x3e00, 0, n);
            }
            open_name = tmp;
            tile_set_num = set;
            tile_slot = i + 2;
            tile_bank = bank;
            tile_load_mode = kind == 3;
            if (tile_load_mode && depth == 15)
                tile_load_mode = 2;
            file_open(open_mode, buf, 0, tile_load);
        } else
            lib_open = 0;
    } else
        glyph_library_open(open_mode, name, i + 2);
}

void far tile_open_set(char bank, char far *name, int set, int slot) /* 15f5:040d */
{
    char tmp[80];
    char buf[80];

    str_format(buf, "%s.tlb", name);
    str_format(tmp, "%s%d", name, set);
    if (!slot_open(tmp, slot)) {
        lib_open = 1;
        open_name = tmp;
        tile_set_num = set;
        tile_slot = slot;
        tile_bank = bank - '0';
        tile_load_mode = 0;
        file_open(bank, buf, 0, tile_load);
    } else
        lib_open = 0;
}

char far tile_opened(void)                                        /* 15f5:04b0 */
{
    return lib_open;
}

void far res_alloc(struct Res far * far *pp, unsigned n)         /* 15f5:04b4 */
{
    int i;

    if (*pp != 0)
        res_free(pp);
    for (i = 0; i < 10 && resources[i].slot >= 0; i++)
        ;
    *pp = &resources[i];
    i += 2;
    (*pp)->slot = i;
    (*pp)->index = -1;
    lib_create("activ", i, 0x454c4954L);
    while (n-- > 0)
        lib_insert(i, 0, 0L);
}

char far res_item_count(struct Res far *r)                           /* 15f5:054a */
{
    return lib_items(slot_base(r->slot));
}

void far glyph_cell(int col, int row, int glyph, char unused,
                    struct Res far *r)                           /* 15f5:0564 */
{
    screen_draw_begin(1);
    draw_glyph(row * 4 + 0x1f44, col * 4 + 0x1f44, r->slot, glyph);
}

void far glyph_at(int col, int row, unsigned char glyph, char unused,
                  struct Res far *r)                             /* 15f5:0598 */
{
    screen_draw_begin(1);
    draw_glyph(row, col, r->slot, glyph);
}

void far glyph_put(int col, int row, unsigned char glyph, char unused,
                   struct Res far *r)                            /* 15f5:05bf */
{
    draw_glyph(row, col, r->slot, glyph);
}

void far res_glyph_mirror(struct Res far *r, unsigned char n)          /* 15f5:05dd */
{
    glyph_mirror(slot_base(r->slot), n);
}

void far res_draw(int col, int row, struct Res far *r)           /* 15f5:05fd */
{
    draw_glyph(row * 4 + 0x1f40, col * 4 + 0x1f40, r->slot, r->index);
}

void far glyph_draw(int col, int row, int glyph, struct Res far *r) /* 15f5:0629 */
{
    draw_glyph(row * 4 + 0x1f40, col * 4 + 0x1f40, r->slot, glyph);
}

int far res_index_find(struct Res far *r, int n)                     /* 15f5:0654 */
{
    return lib_index_find(slot_base(r->slot), n);
}

void far screen_draw_begin(char on)                                     /* 15f5:0671 */
{
    if (!text_on && flush_mode != 2) {
        if (on)
            screen_page_save();
        draw_page_set(0);
        text_on = 1;
    }
}

void far screen_flush_mode_set(char mode)                                   /* 15f5:069d */
{
    if (mode == 2)
        screen_flush();
    flush_mode = mode;
}

void far pal_select(struct Res far *r)                           /* 15f5:06b3 */
{
    long used;

    if (r != 0)
        used = slot_used(r->slot);
    else
        used = -1L;
    if (cur_res != r || used != cur_used) {
        pal_dirty = 1;
        cur_res = r;
        cur_used = used;
        if (r != 0)
            huge_move(lib_item(slot_base(r->slot), 0) + 8, pal_buf, 0x60);
    }
}

void far screen_flush(void)                                      /* 15f5:0756 */
{
    char pal[96];

    if (flush_mode == 0) {
        if (pal_dirty && cur_res != 0)          /* slot 0, not cur_res's; */
            huge_move(lib_item(slot_base(0), 0) + 8, pal, 0x60); /* never read */
        if (text_on) {
            screen_page_show();
            draw_page_set(1);
            text_on = 0;
        }
        if (is_xvga() || !pixel_shift())
            pal_commit();
        pal_dirty = 0;
    }
}

void far pal_load(struct Res far *r)                             /* 15f5:07da */
{
    unsigned char pal[96];

    if (r != 0)
        huge_move(lib_item(slot_base(r->slot), r->index) + 8, pal, 0x60);
    else
        mem_fill(pal, 0x60, 0);
    palette_load(pal);
    mem_move(pal, pal_buf, 0x60);
    pal_dirty = 1;
}

void far pal_fade(char fade, struct Res far *r)                  /* 15f5:0859 */
{
    unsigned char pal[96];

    if (fade) {
        pal_load(0);
        screen_page_show();
        pal_load(r);
    } else {
        huge_move(lib_item(slot_base(r->slot), r->index) + 8, pal, 0x60);
        palette_page_load(pal, 1);
        mem_move(pal, pal_buf, 0x60);
        pal_dirty = 1;
    }
}

void far res_glyph_recolor(unsigned char far *map, int n, struct Res far *r) /* 15f5:08e1 */
{
    glyph_recolor(slot_base(r->slot), n, map);
}

void far res_copy(struct Res far *src, unsigned char n, char map,
                  struct Res far *dst, unsigned char m)          /* 15f5:0904 */
{
    int i;
    long size;

    if (src == 0) {
        lib_resize(dst->slot, m, 0L);
        return;
    }
    if (map)
        i = lib_index_find(slot_base(src->slot), n);
    else
        i = n;
    size = lib_item_size(slot_base(src->slot), i);
    if (m < 0x4c || lib_item_size(slot_base(dst->slot), m) < size)
        lib_resize(dst->slot, m, size);
    huge_move(lib_item(slot_base(src->slot), i),
                 lib_item(slot_base(dst->slot), m), size);
}

void far glyph_size(struct Res far *r, int glyph, int far *rows, int far *cols) /* 15f5:0a07 */
{
    int top;
    int left;
    int bottom;
    int right;

    glyph_bounds(0x1f40, 0x1f40, r->slot, glyph, &top, &left, &bottom, &right);
    text_xy_to(0x1f40, 0x1f40, top, left, &top, &left);
    text_xy_to(0x1f40, 0x1f40, bottom, right, &bottom, &right);
    *rows = bottom - top;
    *cols = right - left;
}

void far res_reset(void)                                         /* 15f5:0a8d */
{
    int i;

    for (i = 0; i < 10; i++)
        resources[i].slot = -1;
}

void far cell_fill(int col, int row, int width, unsigned height,
                   unsigned char attr)                           /* 15f5:0aae */
{
    fill_rect(((row + 8) >> 1) + 0x1f40, col * 4 + 0x1f44,
                 ((row + height + 8) >> 1) + 0x1f40, (col + width) * 4 + 0x1f44, attr);
}

void far rows_save(int row, int col, int height, int width)      /* 15f5:0af4 */
{
    int i;
    short h;

    text_xy(row, col, &row, &col);
    if (pixel_shift() == 3)
        col &= 0xfff0;
    text_xy(height + 0x1f40, width + 0x1f40, &h, &width);
    pointer_hide_rect(row, col, row + h, col + width);
    width = (width >> 3) << (3 - pixel_shift());
    if (width & 1)
        width++;
    video_address_set(row, col);
    for (i = 0; i < video_planes(); i++) {
        video_plane_set(i);
        grab_words(rows_saved[i], h, width);
    }
    pointer_show();
}

void far rows_restore(int row, int col, int height, int width)   /* 15f5:0bc3 */
{
    int i;
    short h;

    text_xy(row, col, &row, &col);
    if (pixel_shift() == 3)
        col &= 0xfff0;
    text_xy(height + 0x1f40, width + 0x1f40, &h, &width);
    pointer_hide_rect(row, col, row + h, col + width);
    width = (width >> 3) << (3 - pixel_shift());
    if (width & 1)
        width++;
    video_address_set(row, col);
    for (i = 0; i < video_planes(); i++) {
        video_plane_set(i);
        blit_bytes(rows_saved[i], h, width, 0);
    }
    pointer_show();
}

void far res_palette_load(struct Res far *r)                           /* 15f5:0c95 */
{
    if (is_xvga() || !pixel_shift())
        glyph_palette_load(slot_base(r->slot), lib_itype(slot_base(r->slot)) != 0);
}

void far pal_black(int first, int last)                          /* 15f5:0ce2 */
{
    unsigned char pal[0x300];

    if (is_xvga() || !pixel_shift()) {
        mem_fill(pal, 0x300, 0);
        pal_set(first, last - first + 1, pal, 0, 0);
        pal_commit();
    }
}

long far tile_loader_identity(int slot, long off, long len)              /* 15f5:0d37 */
{
    return len;
}
