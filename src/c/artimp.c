/* CKIT.EXE overlay section 24, module 2d91: importing and unimporting art.
 * import_screen is screen 16 of editloop.c's loop, which artpick.c's
 * gallery ("Import") runs with the pick in env->bits (kind mask << 8 |
 * number): it keeps bits 0..14, imports through art_import, and ends with
 * env->bits holding art_import's result, which art_screen tests (low
 * nibble set: label_import).  It returns the screen it was called with.
 *
 * art_import lists the LBM and PCX files of the relative "art" directory
 * (dir_first and dir_next, a node each from node_pool) in list_menu under
 * "Select Cancel", and returns 0 on Cancel, Esc or an empty list (with a
 * beep).  For a pick it loads the file into cache slot 0x18 and has
 * pcx.c (a name ending in X or x) or ilbm.c (any other) run pcx_show or
 * ilbm_show and fill a 0x100-entry RGB table and a table of colour
 * cycles, returning their count.  By the kind mask it then names a new
 * library (lib_create, tag "TILE", slot 0x18) and has glyphcut.c's
 * glyph_grab cut glyphs out of the picture into items: PIC<A..F>1nnn.tlb
 * (mask 1, one 88 x 88 glyph, palette entries 0x20 to 0xff), SPRI0nnn.tlb
 * (mask 2, three sprite frames, entries 0xb0 to 0xff), CPIC1nnn.tlb (mask
 * 4 or 8, two combat icon frames side by side, sized by "Shape:" Normal,
 * Tall, Wide or Big; no palette) or BIGP0nnn.tlb (mask 0x10 or 0x20, three
 * strips of 0x28 rows in one item behind an 8-byte header whose first
 * word is 0x78, entries 0x20 to 0xff).  With palette entries it adds a
 * palette glyph (type 8: the RGB triples, then the cycles).  Each item is
 * inserted as item 0.  It then creates the file (DOS function 3Ch) in the
 * design directory (design_dir), closes it, and passes 0, the path and slot
 * 0x18 to slot_save (module 2e44).  art_unimport (artpick.c's
 * label_unimport, through stub 18de:1f17) frees picview.c's picture
 * (masks 1 and 2) or big picture (0x10 and 0x20), calls slot_release on
 * the file name and deletes the file (DOS function 41h); it always
 * returns 1.
 *
 * Section 24 (parent section 20, which is empty and has no parent; no
 * section names 24 as its parent): payload at EXE 0x07b5b0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 24.  The module is stored frame 2d91 (load-based 3d91),
 * EXE 0x07b5b0..0x07c0ec (2,876 bytes, 3 routines), from the section's
 * first byte to its last retf at 2d91:0b3b; module 2e44 follows at
 * 2e44:000c (EXE 0x07c0ec).  One Turbo C large-model _TEXT segment, built
 * with Turbo C 2.01, tcc -ml -O -Z -d; functions are defined in shipped
 * order.  Definition comments give the load-based address; extern comments
 * the far pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 24's relocation table files 89 sites under 2d91, all in this
 * range (EXE 0x07b5da..0x07c0e1), all far calls: 3 through vector stubs
 * into section 3 and 9 into section 24's modules 2e44, 2e59, 2fb1 and 3039.
 * The next site, EXE 0x07c12c, is filed under 2e44.  Both of the section's
 * vectors land here: 18de:1be3 -> import_screen (editloop.c's
 * import_screen, called once from section 10) and 18de:1f17 ->
 * art_unimport (artpick.c's art_unimport, called once from section 18).
 * No relocated word in section 24 stores frame 2d91, and section 24 has no
 * descendants.  import_screen near-calls art_import ahead of its
 * definition, so art_import is static.
 *
 * FRUA's own code: the sharing map pairs 0000 and 0a06 a-only and 0082
 * partial with no partner, so every name here is chosen from the code.
 * Section 24's other modules are slotsave.c (2e44), glyphcut.c (2e59),
 * ilbm.c (2fb1) and pcx.c (3039); this module's externs take their
 * names.
 *
 * _DATA is DGROUP 91d4..9241: ten literals, "art" stored once for its
 * three uses and all four file-name formats shared with art_unimport.
 * Below it areatool.c's (section 23) _DATA ends at 91d3; the block from
 * 9242 (to about 92fb) is read only by section 25.  No _BSS: the only DGROUP addresses past a7f0 the
 * code names are menu_escaped and node_pool.
 *
 * Forced by the bytes, each against the module's three ranges: the chained
 * assignments' order (ok = sprite = 0, cut.col = cut.row = 0x10,
 * hdr.dy = hdr.dx = 0, first = ncol = 0; reversed, each bakes 2 of 3 or
 * 1 of 3); the (unsigned char) casts on list_menu's and menu_run's results
 * (without, 1 of 3 each); "if (head == 0) head = cur; else ..." (the
 * other order, 2 of 3); is_pcx as a bare comparison (with ? 1 : 0, 1 of
 * 3); the palette glyph's size as an int sum (a long one, 1 of 3);
 * Glyph.width unsigned char (char, 1 of 3); EdEnv.bits signed (as
 * unsigned the object makes no LXRSH@ call: the bake refuses its --sym).
 *
 * Not decided by the bytes (each bakes 3 of 3): both register or both
 * plain for n and ncol in either declaration order, or register n with
 * plain ncol (plain n with register ncol bakes 2 of 3); short or int for the stack
 * ints; size + off or off + size; pal + first * 3 or &pal[first * 3]; 8
 * or sizeof cut.hdr; Glyph.type char or unsigned char; static or public
 * for art_unimport.
 *
 * Shipped as written: a kind mask with none or several of the six bits
 * reaches the cutting loop with n unset; the four draw_glyph calls in the
 * combat icon case pass row and column 0x1f40; Cancel in "Shape:" returns
 * 0 after the picture has been loaded.
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   signed here: LXRSH@               */
    char shift;                 /* +4   not read here                     */
    int sel;                    /* +5   not read here                     */
};

struct Node {                   /* a list_menu line, as menu.c's          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct Glyph {                  /* the header before a glyph's bits       */
    int rows;                   /* +0   type 8: flags                     */
    int dy;                     /* +2   type 8: the first entry           */
    int dx;                     /* +4   type 8: the entry count           */
    unsigned char width;        /* +6   bytes per plane row; type 8: the
                                        colour cycles                     */
    unsigned char type;         /* +7                                     */
};

struct CycSpec {                /* one colour cycle, as pal_set takes it  */
    unsigned char flags;        /* +0                                     */
    unsigned char rate;         /* +1                                     */
    unsigned char first;        /* +2                                     */
    unsigned char count;        /* +3                                     */
};

struct Cut {                    /* where glyph_grab cuts a glyph, 0x10 bytes */
    int row;                    /* +0   the top pixel row                 */
    int col;                    /* +2   the left pixel column             */
    struct Glyph hdr;           /* +4                                     */
    char byte_c;                /* +c   not set here                      */
    unsigned char key;          /* +d                                     */
    unsigned char count;        /* +e   not set here                      */
    unsigned char frame;        /* +f                                     */
};

extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern int  far dos_open(int ax, char far *name, ...);       /* 00d7:26bc */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far pool_alloc(void far * far *out, int unused, void far *pool); /* 0a75:0203 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far beep(void);                                      /* 0da7:018c */
extern char far dir_first(char disk, char far *name, char far *ext,
                          char files, char dirs);                /* 0e23:04d5 */
extern char far *far dir_next(char far *is_dir);                 /* 0e23:0608 */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern void far pal_set(int first, int count, unsigned char far *rgb,
                        int nspec, struct CycSpec far *spec);    /* 1157:00b3 */
extern void far pal_commit(void);                              /* 1157:0457 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far slot_release(char far *name);                    /* 1336:0708 */
extern void far slot_load(char mode, char far *name, int slot);  /* 1336:08ce */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern char far *far lib_item(void far *lib, int n);             /* 147e:01f9 */
extern void far lib_create(char far *name, int slot, long tag); /* 157c:000a */
extern void far lib_insert(int slot, int n, long size);        /* 157c:0081 */
extern void far lib_resize(int slot, int n, long size);        /* 157c:037e */
extern void far path_in_dir(char far *path, char far *dir, char force); /* 17d3:0355 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far mode_change(int n);
                                /* 18de:10cb, vector to overlay 3 1b41:02fe */
extern void far bigpic_release(void);
                                /* 18de:13b9, vector to overlay 3 1bbc:03e0 */
extern void far pic_release(void);
                                /* 18de:13c3, vector to overlay 3 1bbc:02b4 */
extern void far slot_save(char disk, char far *path, int slot);  /* 2e44:00f8 */
extern int  far glyph_grab(struct Cut far *cut, void far *rec, int flags, int n); /* 2e59:0e4f */
extern int  far ilbm_cmap(int slot, int n, unsigned char far *rgb); /* 2fb1:01c5 */
extern int  far ilbm_crng(int slot, struct CycSpec far *cyc);    /* 2fb1:0310 */
extern void far ilbm_show(int slot);                             /* 2fb1:03c6 */
extern int  far pcx_cmap(int slot, int n, unsigned char far *rgb); /* 3039:008c */
extern int  far pcx_crng(int slot, struct CycSpec far *cyc);     /* 3039:02bc */
extern void far pcx_show(int slot);                              /* 3039:02c3 */

extern char far *select_word;                                       /* 03bc "Select" */
extern char far *cancel_text;                                       /* 03c8 "Cancel" */
extern char far *empty_text;                                     /* 3bc0 */
extern char design_dir[0x22];                                      /* 5a54 */
extern char menu_escaped;                                        /* ca2a */
extern void far *node_pool;                                      /* ce5d */

static char far art_import(unsigned char num, unsigned char mask);

int far import_screen(int prev, struct EdEnv far *env, void far *arg) /* 3d91:0000 */
{
    char ok;

    env->bits &= 0x7fff;
    ok = art_import(env->bits & 0xff, env->bits >> 8 & 0xff);
    env->bits &= 0xffff0000L;
    env->bits |= ok;
    return prev;
}

static char far art_import(unsigned char num, unsigned char mask) /* 3d91:0082 */
{
    struct Cut cut;
    int far *buf;
    char far *item;
    long off;
    short first;
    short size;
    short choice;
    short ncyc;
    unsigned char pal[0x300];
    char name[0x20];
    char path[0x50];
    char sprite;
    char ok;
    struct Node far *head;
    struct Node far *cur;
    struct CycSpec cyc[16];
    short key;
    char far *found;
    char is_dir;
    char is_pcx;
    char redraw;
    register int n;
    register int ncol;

    ok = sprite = 0;
    head = 0;
    if (dir_first(0, "art", "LBM", 1, 0)) {
        while ((found = dir_next(&is_dir)) != 0) {
            cur = head;
            pool_alloc((void far * far *)&head, sizeof(struct Node), node_pool);
            if (head == 0)
                head = cur;
            else
                head->next = cur;
            head->key = 0;
            str_copy(head->text, found);
        }
    }
    if (dir_first(0, "art", "PCX", 1, 0)) {
        while ((found = dir_next(&is_dir)) != 0) {
            cur = head;
            pool_alloc((void far * far *)&head, sizeof(struct Node), node_pool);
            if (head == 0)
                head = cur;
            else
                head->next = cur;
            head->key = 0;
            str_copy(head->text, found);
        }
    }
    if (head) {
        frame_border();
        cur = head;
        choice = 0;
        redraw = 1;
        str_format(name, "%s %s", select_word, cancel_text);
        hotkey_reset(1);
        key = (unsigned char)list_menu(empty_text, name, 1, 2, 0x26, 0x16, head, 1, 0,
                                       &redraw, &choice, &cur);
        if (menu_escaped && key == 0x1b)
            key = 1;
        if (key == 0) {
            ok = 1;
            str_copy(name, cur->text);
        }
        list_free(&head);
    } else
        beep();
    if (ok) {
        mode_change(4);
        path_in_dir(name, "art", 1);
        is_pcx = to_upper(name[str_len(name) - 1]) == 'X';
        slot_load(0, name, 0x18);
        draw_page_set(1);
        if (is_pcx) {
            pcx_show(0x18);
            pcx_cmap(0x18, 0x100, pal);
            ncyc = pcx_crng(0x18, cyc);
        } else {
            ilbm_show(0x18);
            ilbm_cmap(0x18, 0x100, pal);
            ncyc = ilbm_crng(0x18, cyc);
        }
        slot_close(0x18);
        slot_release(name);
        cut.hdr.dy = cut.hdr.dx = 0;
        cut.key = 0xff;
        cut.frame = 0;
        switch (mask) {
        case 1:
            str_format(name, "PIC%c1%03d.tlb", num < 0x4c ? 'A' : num < 0x8a ? 'B' :
                       num < 0xa4 ? 'C' : num < 0xc1 ? 'D' : num < 0xe3 ? 'E' : 'F', num);
            first = 0x20;
            ncol = 0xe0;
            cut.col = cut.row = 0x10;
            cut.hdr.rows = 0x58;
            cut.hdr.width = 0xb;
            cut.hdr.type = 0;
            n = 1;
            break;
        case 2:
            str_format(name, "SPRI0%03d.tlb", num);
            first = 0xb0;
            ncol = 0x50;
            cut.hdr.type = 5;
            cut.row = 0x22;
            cut.col = 0xe0;
            cut.hdr.rows = 0x23;
            cut.hdr.width = 5;
            cut.hdr.dy = -0x1a;
            cut.hdr.dx = -0x18;
            n = 3;
            sprite = 1;
            break;
        case 4:
        case 8:
            draw_glyph(0x1f40, 0x1f40, 1, 1);
            draw_glyph(0x1f40, 0x1f40, 1, 2);
            draw_glyph(0x1f40, 0x1f40, 1, 3);
            draw_glyph(0x1f40, 0x1f40, 1, 4);
            hotkey_reset(3);
            choice = (unsigned char)menu_run("Shape:", "Normal Tall Wide Big Cancel", 0, 0);
            if (choice == 4)
                return 0;
            str_format(name, "CPIC1%03d.tlb", num);
            first = ncol = 0;
            cut.col = choice & 2 ? 0x40 : 0xc0;
            cut.row = choice & 1 ? 0x16 : 0x54;
            cut.hdr.rows = choice & 1 ? 0x30 : 0x18;
            cut.hdr.width = (choice & 2 ? 0x30 : 0x18) >> 3;
            cut.hdr.type = 5;
            n = 2;
            break;
        case 0x10:
        case 0x20:
            str_format(name, "BIGP0%03d.tlb", num);
            first = 0x20;
            ncol = 0xe0;
            cut.row = 1;
            cut.col = 8;
            cut.hdr.rows = 0x78;
            cut.hdr.width = 0x26;
            cut.hdr.type = 2;
            n = 1;
            break;
        }
        if (is_xvga() || !pixel_shift()) {
            pal_set(first, ncol, pal + first * 3, 0, 0);
            pal_commit();
        }
        slot_release(name);
        lib_create(name, 0x18, 0x454c4954L);
        while (--n >= 0) {
            if (cut.hdr.rows == 0x78) {
                off = 8;
                lib_insert(0x18, 0, off);
                for (n = 0; n < 3; n++) {
                    cut.hdr.rows = 0x28;
                    size = glyph_grab(&cut, 0, 0x10, 1) - 8;
                    lib_resize(0x18, 0, size + off);
                    if (off == 8) {
                        buf = db_buffer();
                        *buf = 0x78;
                        huge_move(db_buffer(), lib_item(slot_base(0x18), 0), size + 8);
                    } else
                        huge_move((char far *)db_buffer() + 8,
                                     huge_add(lib_item(slot_base(0x18), 0), off), size);
                    off += size;
                    cut.row += 0x28;
                }
                n = -1;
            } else {
                size = glyph_grab(&cut, 0, 0x10, 1);
                lib_insert(0x18, 0, size);
                huge_move(db_buffer(), lib_item(slot_base(0x18), 0), size);
            }
            if (sprite) {
                if (n == 2) {
                    cut.row = 0x18;
                    cut.col = 0x78;
                    cut.hdr.rows = 0x35;
                    cut.hdr.width = 7;
                    cut.hdr.dy = -0x10;
                    cut.hdr.dx = -0x10;
                } else if (n == 1) {
                    cut.row = 0x10;
                    cut.col = 0x10;
                    cut.hdr.rows = 0x50;
                    cut.hdr.width = 9;
                    cut.hdr.dy = cut.hdr.dx = -8;
                }
            } else
                cut.col -= cut.hdr.width * 8 + 8;
        }
        if (ncol) {
            lib_insert(0x18, 0, ncol * 3 + ncyc * 4 + 8);
            cut.hdr.rows = 1;
            if (ncyc)
                cut.hdr.rows |= 2;
            cut.hdr.dy = first;
            cut.hdr.dx = ncol;
            cut.hdr.width = ncyc;
            cut.hdr.type = 8;
            item = lib_item(slot_base(0x18), 0);
            huge_move(&cut.hdr, item, 8);
            item = huge_add(item, 8L);
            huge_move(pal + first * 3, item, ncol * 3);
            if (ncyc) {
                item = huge_add(item, ncol * 3);
                huge_move(cyc, item, ncyc * 4);
            }
        }
        path[0] = 0;
        path_append_name(path, design_dir);
        path_append_name(path, name);
        if ((n = dos_open(0x3c00, path, 0, 0)) >= 0) {
            dos_call(0x3e00, 0, n);
            slot_save(0, path, 0x18);
        }
        slot_close(0x18);
    }
    return ok;
}

char far art_unimport(unsigned char num, unsigned char mask)     /* 3d91:0a06 */
{
    char name[0x50];
    char path[0x50];

    switch (mask) {
    case 1:
        str_format(name, "PIC%c1%03d.tlb", num < 0x4c ? 'A' : num < 0x8a ? 'B' :
                   num < 0xa4 ? 'C' : num < 0xc1 ? 'D' : num < 0xe3 ? 'E' : 'F', num);
        pic_release();
        break;
    case 2:
        str_format(name, "SPRI0%03d.tlb", num);
        pic_release();
        break;
    case 4:
    case 8:
        str_format(name, "CPIC1%03d.tlb", num);
        break;
    case 0x10:
    case 0x20:
        str_format(name, "BIGP0%03d.tlb", num);
        bigpic_release();
        break;
    }
    slot_release(name);
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, name);
    dos_open(0x4100, path);
    return 1;
}
