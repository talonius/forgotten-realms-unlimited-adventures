/* CKIT.EXE resident module 1cb5: pictures and icons.  Loading a picture
 * library into pic_res and freeing it; loading, clearing, drawing and
 * measuring an icon through the resource icon_res.  icon_load fills
 * entries idx and idx + 0x26; icon_clear clears idx, and idx + 0x26 for
 * idx < 0x26; icon_draw draws entry bank * 0x26 + idx, through the scratch
 * entry 0x4c when kind is above 3; icon_size gives that entry's glyph_size
 * divided by 12.  A name starting "CH" or "CB" is cut with
 * substr(name, 1, len - 1).
 * The one reference to pic_mode found in the image is pic_load's store.
 *
 * EXE 0x00ef56..0x00f305, one Turbo C large-model _TEXT segment starting at
 * byte 6 of paragraph 1cb5.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * The lone retf at 0x00ef56 (charpic_unit_init) is claimed here: module 1c8e's
 * last routine ends at 0x00ef55, and no far call, near call, stored far
 * pointer or relocation found in the image reaches the byte.  Module 1cf0
 * starts at 0x00f305 (1cf0:0005) with no stub between.  The other six
 * routines are far-called as 0cb5:OOOO, and all 29 MZ relocation sites in
 * the range are filed under frame 0cb5.
 *
 * _DATA is 38d2..38e7: pic_mode and four literals.  There is no _BSS.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  A name another module
 * here gives an address is reused from there: the tiles_ names, res_free,
 * tile_open_file, glyph_at, res_copy and glyph_size are tiles.c's (module
 * 15f5), extra_row is menu.c's (module 183a).  A modNNNN_ name is a
 * placeholder that says only which segment the callee lives in.
 *
 * glyph_at's fourth argument is pushed as a byte here (mov al,1), and
 * extra_row's result is tested as a byte (or al,al).  All six shipped calls
 * to pic_load push five words (add sp,0xa); its body reads the first three,
 * the first a far pointer, and never n.
 */

struct Res {                    /* tiles.c's resource; only passed here   */
    int slot;                   /* +0                                     */
    int index;                  /* +2                                     */
    char tail[2];               /* +4                                     */
};

extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp); /* 05f5:01de */
extern void far glyph_at(int col, int row, unsigned char glyph, char unused,
                         struct Res far *r);                     /* 05f5:0598 */
extern void far res_glyph_mirror(struct Res far *r, unsigned char n);  /* 05f5:05dd */
extern void far res_copy(struct Res far *src, unsigned char n, char map,
                         struct Res far *dst, unsigned char m);  /* 05f5:0904 */
extern void far glyph_size(struct Res far *r, int glyph, int far *rows,
                           int far *cols);                       /* 05f5:0a07 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern char far *far substr(char far *s, int start, int len);    /* 0a75:0027 */
extern char far extra_row(void);                                 /* 083a:23b9 */
extern void far mode_change(int n);    /* 18de:10cb, RTLink vector into section 3 */

extern struct Res far *icon_res;                                 /* b71c */
extern struct Res far *pic_res;                                  /* cfd8 */

static int pic_mode = 0x7f;                                      /* 38d2 */

/* The translated Pascal unit's empty initialization block (DKK maptiles.pas). */
void far charpic_unit_init(void)                                      /* 1cb5:0006 */
{
}

void far pic_load(char far *name, unsigned char set,
                  unsigned char mode, char n)                    /* 1cb5:0007 */
{
    set_open_mode('2');
    if (mode)
        pic_mode = mode;
    tile_open_file(name, set, 0, 0, &pic_res);
    res_palette_load(pic_res);
}

void far icon_clear(unsigned char idx)                           /* 1cb5:004d */
{
    res_copy(0, 0, 0, icon_res, idx);
    if (idx < 0x26)
        res_copy(0, 0, 0, icon_res, idx + 0x26);
}

void far icon_load(char far *name, unsigned char set,
                   unsigned char idx)                            /* 1cb5:0093 */
{
    struct Res far *r;
    char buf[80];

    if (name[0] == 'C' && (name[1] == 'H' || name[1] == 'B'))
        name = substr(name, 1, str_len(name) - 1);
    r = 0;
    set_open_mode(str_same(name, "CPIC") || str_same(name, "COMSPR") ? '2' : '3');
    str_copy(buf, name);
    str_cat(buf, "1");
    if (!str_same(name, "CBODY") || !extra_row()) {
        tile_open_file(buf, set, 3, name[0] == 'C' && name[1] == 'H' ? 0 : 15, &r);
        res_copy(r, 0, 0, icon_res, idx);
        res_copy(r, name[0] == 'C' && name[1] == 'H' ? 0 : 1, 0, icon_res, idx + 0x26);
    } else {
        mode_change(1);
        tile_open_file(name, 0xff, 1, 0, &r);
        res_copy(r, set, 1, icon_res, idx);
        if (name[0] == 'C' && name[1] == 'H')
            res_copy(r, set, 1, icon_res, idx + 0x26);
        else
            res_copy(r, set + 0x80, 1, icon_res, idx + 0x26);
    }
    res_free(&r);
}

void far icon_draw(register int col, register int row, unsigned char kind,
                   unsigned char bank, unsigned char idx)        /* 1cb5:0299 */
{
    col = (col + col + col) * 4 + 0x1f44; /* add,add,shl,shl; col * 12 is a mul */
    if (row < -900)
        row += 1000;
    row = (row + row + row) * 4 + 0x1f44;
    if (kind > 3) {
        res_copy(icon_res, bank * 0x26 + idx, 0, icon_res, 0x4c);
        res_glyph_mirror(icon_res, 0x4c);
        glyph_at(col, row, 0x4c, 1, icon_res);
    } else
        glyph_at(col, row, bank * 0x26 + idx, 1, icon_res);
}

void far pic_free(void)                                          /* 1cb5:0352 */
{
    res_free(&pic_res);
}

void far icon_size(unsigned char idx, unsigned char bank, int far *rows,
                   int far *cols)                                /* 1cb5:035f */
{
    glyph_size(icon_res, bank * 0x26 + idx, rows, cols);
    *rows /= 12;
    *cols /= 12;
}
