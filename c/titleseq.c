/* CKIT.EXE overlay 06 module 226c: the title pictures.
 * title_show draws title screens 2 to 6 from the title1 library in turn,
 * waiting up to 1050 ticks on each; a space moves on, Esc leaves at once,
 * and any other key ends the sequence early.  title_pic draws one screen
 * over the first picture of the library's first set.
 *
 * EXE 0x03972b..0x0399a5, one Turbo C large-model _TEXT segment starting at
 * byte b of paragraph 226c in RTLink section 6 (payload 0x38da0, link
 * segment 21d4; a child of section 3).  The first b bytes of paragraph
 * 226c are the tail of module 2209; the lone retf at 2294:0005 after this
 * module's last routine is reached by no vector and no far call, and is
 * left to module 2294 by the run's rule; the bytes do not decide it.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  A modSSSS_OOOO or
 * <module>_OOOO callee name is a placeholder; mode_change is section 3's,
 * reached by a direct far call because section 3 is section 6's parent.
 * title_show is reached only through the vector stub 18de:1445, which
 * section 3's gamemain.c calls title_show; no vector and no far call
 * reaches titleseq_unit_init.
 *
 * _DATA is two literals, DGROUP 6bc0..6bc7.  _BSS is empty.
 */

struct Res;                     /* tiles.c's resource handle              */

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp); /* 05f5:01de */
extern void far glyph_draw(int col, int row, int glyph, struct Res far *r); /* 05f5:0629 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern long far ticks_now(void);                                 /* 06c9:096c */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern char far key_pressed(void);                               /* 0da7:009a */
extern void far key_flush(void);                                 /* 0da7:00bd */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far glyph_palette_load(char far *lib, int n);              /* 12d2:0537 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far mode_change(int mode);                           /* 1b41:02fe */

/* The translated Pascal unit's empty initialization block (DKK intro.pas). */
void far titleseq_unit_init(void)                                     /* 326c:000b */
{
}

char far title_wait(unsigned ticks)                              /* 326c:000c */
{
    char c;
    long now;
    long until;
    int key;

    input_pending_clear();
    now = ticks_now();
    until = ticks + now;
    key = ' ';
    for (;;) {
        while (!key_pressed() && ticks_now() < until) {
            if (is_xvga() || !pixel_shift())
                input_poll();
        }
        if (key_poll()) {
            key = key_get();
            if (key == 0x152) {
                sound_toggle();
                continue;
            }
            if (key == 0x153) {
                music_toggle();
                continue;
            }
        }
        break;
    }
    if (ticks_now() >= until)
        c = ' ';
    else
        c = key;
    key_flush();
    return c;
}

void far title_pic(char far *name, unsigned char n, char unused1,
                   char unused2)                                 /* 326c:00bf */
{
    struct Res far *res;
    char file[12];

    res = 0;
    str_copy(file, name);
    str_cat(file, "1");
    tile_open_file(file, 1, 1, 0, &res);
    screen_draw_begin(0);
    glyph_draw(0, 0, 1, res);
    res_palette_load(res);
    res_free(&res);
    tile_open_file(file, n, 1, 0, &res);
    if (n == 4)
        clip_set(0x35, 0x1f40, 0x1fa4, 0x1fe0);
    glyph_draw(0, 0, 1, res);
    clip_reset();
    glyph_draw(0, 0, 2, res);
    res_palette_load(res);
    screen_flush();
    res_free(&res);
}

void far title_done(void)                                        /* 326c:01d3 */
{
    if (is_xvga() || !pixel_shift())
        glyph_palette_load(slot_base(0), 0);
    pointer_show();
}

void far title_show(void)                                        /* 326c:01ff */
{
    short i;
    char key;

    mode_change(5);
    draw_page_set(0);
    pal_select(0);
    sfx_play(32);
    set_open_mode('1');
    pointer_hide();
    for (i = 2; i <= 6; i++) {
        title_pic("title", i, 1, 1);
        if ((key = title_wait(1050)) == 0x1b) {
            pointer_show();
            return;
        }
        if (key != ' ')
            break;
    }
    title_done();
}
