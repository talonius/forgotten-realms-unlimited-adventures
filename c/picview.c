/* CKIT.EXE overlay 03 module 1bbc: the picture window and the big picture.
 * Opening a picture from the PIC tile libraries or by name, drawing it and
 * its animation frames at a cell position, and closing it; opening,
 * drawing and closing a bigpic library picture, which big_redraw reopens
 * from big_last when it has been closed.
 *
 * EXE 0x02c7ab..0x02cb92, one Turbo C large-model _TEXT segment starting at
 * byte b of paragraph 1bbc in RTLink section 3 (payload 0x2bff0, link
 * segment 1b41; a root section, the parent of 4 to 10).  The first b bytes
 * of paragraph 1bbc are the tail of module 1b88, and the last 2 bytes of
 * the range lie in paragraph 1bfb, whose module starts at 1bfb:0002.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 3.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use.  Six routines are also reached
 * from other sections through the vector stubs 18de:103f (pic_frame), 1049
 * (pic_open), 107b (big_redraw), 13b9 (bigpic_release), 13c3
 * (pic_release) and 146d (big_open); hero.c and menu.c name three of
 * them pic_frame, pic_open and big_redraw.  Sections 4, 8 and 9
 * far-call all seven routines directly; pic_show is reached only by section
 * 8's far call at file 0x418e2.  pic_open calls pic_release ahead of
 * its definition, so that one is prototyped.
 *
 * _DATA is five literals stored as four, DGROUP 5acc..5ae5: -d stores ""
 * on the terminator of "PIC" (5acf).  _BSS is big_last at b250.
 */

struct Res {                    /* tiles.c's resource; only passed here   */
    int slot;
    int index;
    char tail[2];
};

struct Pic {                    /* a picture window's state               */
    unsigned char frames;       /* +0  res_item_count's count, less 1 for PIC */
    unsigned char anim;         /* +1  1 for a PIC library picture        */
    struct Res far *res;        /* +2                                     */
};

extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp); /* 05f5:01de */
extern char far res_item_count(struct Res far *r);                   /* 05f5:054a */
extern void far glyph_draw(int col, int row, int glyph, struct Res far *r); /* 05f5:0629 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern void far pal_black(int first, int last);                  /* 05f5:0ce2 */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far frame_piece_refresh(char c);                              /* 0d65:01c6 */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far int_to_str(int value, char far *buf);            /* 0a75:000d */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far pal_commit(void);                              /* 1157:0457 */
extern void far mode_change(int mode);                           /* 1b41:02fe */

extern char view_kind;                                           /* b40e */
extern char view_flag;                                           /* b40f */
extern char byte_b757;                                           /* b757 */
extern char anim_first;                                          /* b758 */
extern unsigned char anim_last;                                  /* b759 */
extern unsigned char anim_frame;                                 /* b75a */
extern struct Res far *big_res;                                  /* bcf7 */
extern char pic_name[];                                          /* c3a7 */
extern struct Pic pic;                                           /* ca22 */
extern unsigned char pic_disk;                                   /* caf7 */

void far pic_release(void);

static char big_last;                                            /* b250 */

void far pic_frame(int col, int row, char show, unsigned char n) /* 2bbc:000b */
{
    if (pic.res != 0) {
        if (show) {
            screen_draw_begin(1);
            if (n > 0)
                glyph_draw(col, row, 1, pic.res);
            glyph_draw(col, row, n + 1, pic.res);
        }
        res_palette_load(pic.res);
        screen_flush();
    }
}

void far pic_open(char far *name, unsigned char id, char unused,
                  struct Pic far *p)                             /* 2bbc:0077 */
{
    char disk[2];

    mode_change(str_same(name, "PIC") ? 2 : 0);
    if (str_same(name, pic_name) && id == view_kind || id == 0xff)
        return;
    pic_release();
    str_copy(pic_name, name);
    view_kind = id;
    int_to_str(pic_disk, disk);
    set_open_mode('2');
    if (str_same(name, "PIC"))
        tile_open_file(msg_format("PIC%c1",
                                  id < 0x4c ? 'A' : id < 0x8a ? 'B' : id < 0xa4 ? 'C'
                                  : id < 0xc1 ? 'D' : id < 0xe3 ? 'E' : 'F'),
                       id, 0, 0, &p->res);
    else
        tile_open_file(msg_format("%s%s", name, disk), id, 0, 0, &p->res);
    if (str_same(name, "PIC"))
        pal_black(0x20, 0xff);
    else {
        res_palette_load(p->res);
        pal_commit();
    }
    p->frames = res_item_count(p->res);
    if (str_same(name, "PIC")) {
        p->frames--;
        p->anim = 1;
    } else
        p->anim = 0;
    anim_last = p->frames - 1;
    anim_first = 0;
    if (str_same(name, "PIC")) {
        if (id == 0x39)
            anim_first = 4;
        else if (id == 0x94)
            anim_first = 5;
        else if (id == 0x97)
            anim_first = 6;
        else if (id == 0xad)
            anim_first = 1;
        else if (id == 0xb4)
            anim_first = 6;
    }
    byte_b757 = 0;
    anim_frame = 0;
    frame_piece_refresh(0);
}

void far pic_release(void)                                   /* 2bbc:02b4 */
{
    res_free(&pic.res);
    mem_zero(&pic, sizeof pic);
    str_copy(pic_name, "");
    view_kind = -1;
}

void far pic_show(unsigned char n)                               /* 2bbc:02e9 */
{
    if (pic.res != 0) {
        glyph_draw(3, 3, n, pic.res);
        pal_select(pic.res);
        res_palette_load(pic.res);
        screen_flush();
    }
}

void far big_open(unsigned char id)                              /* 2bbc:0334 */
{
    mode_change(3);
    if (view_flag != id) {
        set_open_mode('2');
        tile_open_file(msg_format("bigpi%c%d", id < 0xf8 ? 'c' : 'x', pic_disk),
                       id, 0, 0, &big_res);
        pal_black(0x20, 0xff);
        big_last = view_flag = id;
    }
}

void far big_redraw(void)                                        /* 2bbc:03a5 */
{
    if (view_flag == -1)
        big_open(big_last);
    frame_lower_panel();
    glyph_draw(1, 1, 1, big_res);
    res_palette_load(big_res);
}

void far bigpic_release(void)                                      /* 2bbc:03e0 */
{
    res_free(&big_res);
    view_flag = -1;
}
