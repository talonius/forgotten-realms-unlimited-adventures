/* CKIT.EXE resident module 1d65: the screen frames.  Each layout clears a
 * text window and draws the frame's pieces with draw_glyph from slot 1.
 * frame_main adds glyphs 9 and 0x15 and the compass letter for
 * dir_names[geo.facing], whose first character alone picks the glyph.
 * frame_piece_refresh called with 0 redraws glyph 9 when a redraw is pending; with
 * 2 it clears the pending redraw, and with any other value it marks one.
 * frame_gen opens, draws and frees the gen picture.  frame_select opens the
 * frame library while none is open and, when is_xvga returns nonzero
 * or pixel_shift returns 0, hands slot 1 to glyph_palette_load.
 *
 * EXE 0x00fa57..0x00fe75, one Turbo C large-model _TEXT segment starting at
 * byte 7 of paragraph 1d65.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1d48's last retf is at 0x00fa56, and module 1da7 starts at
 * 0x00fe75 (1da7:0005) with no stub between.
 *
 * _DATA is 54e8..54f4, right after module 1d48's literal; 54f5 is pad.
 * _BSS is gen_res at adca, the module's only static there.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  A name another module
 * gives an address is reused from there; a modNNNN_ name is a placeholder
 * that says only which segment the callee lives in, and frame_NNNN only
 * where in this module the routine starts.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined: frame_main far-calls frame_piece_refresh, so that one carries a
 * prototype.
 */

struct Res {                    /* a resource: a slot and an index in it  */
    int slot;                   /* +0                                     */
    int index;                  /* +2                                     */
    char tail[2];               /* +4                                     */
};

struct Geo {                    /* DQK's geo record; only +2 is read here */
    char x;                     /* +0   d414, not read in this module     */
    char y;                     /* +1   d415, not read in this module     */
    unsigned char facing;       /* +2   d416, indexes dir_names           */
};

extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far frame_invalidate(void);                          /* 083a:0d3c */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far hero_frame_flag_set(char c);                            /* 03d0:0000 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp); /* 05f5:01de */
extern void far res_draw(int col, int row, struct Res far *r);   /* 05f5:05fd */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far glyph_library_open(char mode, char far *name, int slot); /* 11d7:0031 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far glyph_palette_load(char far *lib, int n);              /* 12d2:0537 */

extern struct Geo geo;                                           /* d414 */
extern char dir_names[8][3];                                     /* 5fb2 */

void far frame_piece_refresh(char c);

static char redraw_pending = 0;                                  /* 54e8 */
static int frame_cur = -1;                                       /* 54e9 */

static struct Res far *gen_res;                                  /* adca */

void far frame_rule(int row)                                     /* 1d65:0007 */
{
    draw_glyph(row * 4 + 0x1f40, 0x1f40, 1, 7);
}

void far frame_border(void)                                      /* 1d65:002a */
{
    screen_draw_begin(0);
    text_clear(1, 1, 0x26, 0x16);
    draw_glyph(0x1f40, 0x1f40, 1, 1);
    draw_glyph(0x1f40, 0x1f40, 1, 2);
    draw_glyph(0x1f40, 0x1f40, 1, 3);
    draw_glyph(0x1f40, 0x1f40, 1, 4);
    frame_invalidate();
}

void far frame_character_sheet(void)                                        /* 1d65:009e */
{
    frame_border();
    frame_rule(8);
    frame_rule(0x10);
    frame_rule(0x14);
    draw_glyph(0x1f40, 0x1f40, 1, 8);
    pal_select(0);
}

void far frame_main(void)                                        /* 1d65:00de */
{
    frame_border();
    frame_rule(0x10);
    frame_rule(0xc);
    frame_piece_refresh(2);
    draw_glyph(0x1f40, 0x1f40, 1, 9);
    draw_glyph(0x1f40, 0x1f40, 1, 0x15);
    switch (dir_names[geo.facing][0]) {
    case 'E':
        draw_glyph(0x1f40, 0x1f40, 1, 0x19);
        break;
    case 'N':
        draw_glyph(0x1f40, 0x1f40, 1, 0x16);
        break;
    case 'S':
        draw_glyph(0x1f40, 0x1f40, 1, 0x17);
        break;
    case 'W':
        draw_glyph(0x1f40, 0x1f40, 1, 0x18);
        break;
    }
    hero_frame_flag_set(1);
}

void far frame_piece_refresh(char c)                                      /* 1d65:01c6 */
{
    if (c)
        redraw_pending = c != 2;
    else if (redraw_pending) {
        redraw_pending = 0;
        screen_draw_begin(1);
        draw_glyph(0x1f40, 0x1f40, 1, 9);
    }
}

void far frame_lower_panel(void)                                        /* 1d65:020c */
{
    frame_border();
    frame_rule(0x10);
}

void far frame_lower_panel_palette(void)                                        /* 1d65:021a */
{
    frame_border();
    frame_rule(0x10);
    pal_select(0);
}

void far frame_right_panel(void)                                        /* 1d65:0233 */
{
    screen_draw_begin(0);
    text_clear(0x17, 1, 0x26, 0x15);
    text_clear(1, 1, 0x15, 0x15);
    draw_glyph(0x1f40, 0x1f40, 1, 1);
    draw_glyph(0x1f40, 0x1f40, 1, 2);
    draw_glyph(0x1f40, 0x1f40, 1, 3);
    draw_glyph(0x1f40, 0x1f40, 1, 5);
    clip_set(0x1f9d, 0x1f40, 0x1fa4, 0x1fe0);
    draw_glyph(0x1f40, 0x1f40, 1, 4);
    clip_reset();
    draw_glyph(0x1f40, 0x1f40, 1, 6);
    frame_invalidate();
}

void far frame_list_panel(void)                                        /* 1d65:0302 */
{
    frame_border();
    frame_rule(2);
    pal_select(0);
}

void far frame_gen(void)                                         /* 1d65:031b */
{
    set_open_mode('3');
    tile_open_file("gen", 1, 0, 0, &gen_res);
    screen_draw_begin(0);
    text_clear(1, 1, 0x26, 0xd);
    draw_glyph(0x1f40, 0x1f40, 1, 1);
    draw_glyph(0x1f40, 0x1f40, 1, 2);
    draw_glyph(0x1f40, 0x1f40, 1, 3);
    res_draw(0, 0, gen_res);
    pal_select(0);
    frame_invalidate();
    res_free(&gen_res);
}

void far frame_select(int n)                                       /* 1d65:03c9 */
{
    if (n != frame_cur || n == 0) {
        if (frame_cur < 0)
            glyph_library_open('3', "frame", 1);
        if (is_xvga() || !pixel_shift())
            glyph_palette_load(slot_base(1), 0);
        frame_cur = n;
    }
}
