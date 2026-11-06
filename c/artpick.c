/* v1.0 build (CKIT.EXE "Version 1.0 March 19,1993"): this object is at EXE
 * 0x06b5a0, frame 39fa:0000, 12,308 code bytes; _DATA 8de0.  The comment
 * below describes v1.2.  v1.0 differs in two routines: show_pic's empty
 * picture (num 0) only clears its box (no pic_release call), and
 * show_walls calls wall_sets_free(1) only for num < 0xff, clears rows
 * row + 0x1c..row + 0x1e (v1.2: row..row + 0x20), and draws the five wall
 * pieces for every num, 0 and 0xff included (v1.2 skips both).
 *
 * CKIT.EXE overlay section 18, module 29f4: the design editor's art screens.
 * art_screen (screen 8) lists the labels of one art kind -- pictures,
 * sprites, combat icons, big pictures, the overland, dungeon and
 * wilderness art, wall sets and backdrops, by labels.c's tables -- in
 * list_menu, previewing the entry under the pointer (art_preview, or
 * art_browse in the plain list), and runs the command line built by
 * art_menu: picking an entry ("Select", "Replace" a backdrop slot), or in
 * gallery mode (art_mode 1, screens 1 and 0xb) "Leave View Rename [Import
 * Unimport]" and in monster mode (art_mode -1, screen 7) "Leave View
 * Rename Edit Default Copy".  It returns the
 * next screen and leaves the pick in the shared env bits (kind mask << 8 |
 * number).  rename_screen (screen 18) edits one label's text in a
 * textedit.c field and writes the table back; for a monster it also
 * renames section 25's record.  art_save and art_restore set up and put
 * back the area squares, the party place and cursor_obj[0x13] the sprite
 * and wall-set previews draw with; art_push keeps the open browser in the
 * DGROUP long exp_share for the list callbacks.
 *
 * Section 18 (parent section 15, itself a child of 13; no section names 18
 * as its parent): payload at EXE 0x06b620, link segment 29f4, which
 * sections 16 and 17 share.  A bare stored SEG:OFF in this file means
 * section 18.  The module is stored frame 29f4 (load-based 39f4), EXE
 * 0x06b620..0x06e63f (12,319 bytes, 41 routines), from the section's first
 * byte to its last retf at 29f4:301e; module 2cf5 follows at 2cf5:000f
 * (EXE 0x06e63f, the target of vector 18de:1f7b; 2cf5:0000..000e is this
 * module's rename_key).  One Turbo C large-model _TEXT segment, built with
 * Turbo C 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the far
 * pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 18's relocation table files 224 sites under 29f4, all in this
 * range (EXE 0x06b70d..0x06e5f1): 215 far calls and nine loads of the stub
 * segment 18de (eight for the label filters 18de:187d and 18de:1f0d, one
 * for textedit.c's field_proc stub 18de:175b, EXE 0x06e387).  The other
 * 55 are module 2cf5's, filed under 2cf5.  art_screen and rename_screen are the vector targets of stubs
 * 18de:1c51 and 18de:1bcf (editloop.c's screens 8 and 18, art_screen and
 * rename_screen); no far call in section 18 stores frame 29f4.  Every other
 * routine is near-called or passed with push cs ahead of its definition,
 * so static, except art_marks, which only art_browse and art_preview call,
 * after its definition: for it the bytes do not decide.  The module calls
 * view3d.c (section 13) directly (view_render, wall_sets_free,
 * geo_reset); module 2cf5, not this one, makes section 18's calls into
 * menubar.c, view3d.c's view_attrs and mapedit.c's stub 18de:1e3b and
 * reads mapedit.c's _BSS.
 *
 * FRUA's own code: the sharing map pairs 40 routines a-only and art_noop
 * (5 bytes) exact-weak, so every name here is chosen from the code.
 *
 * _DATA is DGROUP 8e3a..8f29: icon_glyphs (4 x 4), dung_glyphs and
 * wild_glyphs (5 x 13 each and a zero), and 20 literals stored as 14 (-d
 * stores "" on the NUL of "%s %d" and " %s", "%s", "s", "%s %s %s" and
 * "%s %s %s %s %s" as tails).  Only this module's code names 8e3a, 8e4a
 * and 8e8c.  Below it evedit.c's (section 17) _DATA ends at 8e39; above it
 * 8f2a..8f2e ("menu") is module 2cf5's, 8f2f a zero pad and rest.c's
 * (section 19) _DATA starts at 8f30.  No _BSS: every DGROUP address past
 * a7f0 the code names belongs to another module or a goldbox.toml sym, and
 * section 18 has no room in the _BSS order (mapedit.c d42a..d444, evedit.c
 * d446..d44f, rest.c d450).
 *
 * Forced by the bytes: exp_share is a long (as a struct ArtView far * it
 * bakes 10 of 41) and
 * env->bits a signed long (LXRSH@); the assignments inside the tests of
 * art_run, art_command, art_marks, art_title, art_heading and
 * rename_screen; the conditional expressions for art_title's columns (with
 * if/else -O merges the three column formulas); key = mode ? 0 : n - 1
 * (if/else merges the two key = 0); "switch (t & 0xf) { case 2: }" and
 * "switch (p->done & 0x3f) { case 2: }" in art_screen (an if keeps es:bx);
 * art_screen's "else (void)0;": an expression statement with no code
 * stands in the else arm while -O threads jumps and is dropped after, so
 * the jne keeps its target, a jmp to the tail cross-jumped into case 11's
 * (0; or prev; there do too; an empty else or a bare ; does not);
 * art_menu's goto labels (case 3's tail is the one kept) and verb = last
 * = 0; c->arg->plain = ++c->arg->plain % 2; col = 2; k = col; col <<= 2; in
 * show_sprite; gc's (len = ...) in show_backs; "c->arg->plain == 0" for
 * label_list's sixth argument where !c->arg->plain gives neg/sbb.
 */

struct Res;                     /* tiles.c's resource                     */

struct Node {                   /* a list_menu line, as menu.c's          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct Label {                  /* one label record, as labels.c's        */
    unsigned char num;          /* +0                                     */
    unsigned char kinds;        /* +1                                     */
    char text[12];              /* +2                                     */
};

struct Pic {                    /* picview.c's picture window state       */
    unsigned char frames;       /* +0                                     */
    unsigned char anim;         /* +1                                     */
    struct Res far *res;        /* +2                                     */
};

struct MapCell {                /* view3d.c's square of the area, 6 bytes */
    unsigned char wall[4];      /* +0                                     */
    unsigned char event;        /* +4                                     */
    unsigned char bits;         /* +5                                     */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0                                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    unsigned char sets[3];      /* +4                                     */
    unsigned char flags;        /* +7                                     */
    unsigned char back[4];      /* +8                                     */
    char pad_c[0x6a];           /* +c                                     */
    char name[16];              /* +76                                    */
    char pad_86[0x9c];          /* +86                                    */
    struct MapCell cells_head[3]; /* +122                                   */
};

struct Pos {                    /* a place and facing, 5 bytes            */
    char x;                     /* +0                                     */
    char y;                     /* +1                                     */
    unsigned char facing;       /* +2                                     */
    char facing_wall;           /* +3                                     */
    char event;                 /* +4                                     */
};

struct Geo {                    /* view3d.c's geo record                  */
    struct Pos pos;             /* +0                                     */
    char attr_5;                /* +5                                     */
    char attr_6;                /* +6                                     */
    char show_map;              /* +7                                     */
    struct Area far *area;      /* +8                                     */
    char attr_c;                /* +c                                     */
    char attr_d;                /* +d                                     */
};

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   signed here: LXRSH@               */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct FieldSave {              /* a text field's val, textedit.c's       */
    char text[0x27];            /* +00                                    */
    char label[0x25];           /* +27                                    */
    unsigned char pos;          /* +4c                                    */
};

struct ArtSel {                 /* the screens' argument, editloop.c's
                                   a93, 0x12 bytes                        */
    int next;                   /* +0   the screen to go to               */
    int scr;                    /* +2   the screen that called            */
    int done;                   /* +4   ends art_run; > 0 takes the pick  */
    unsigned char kind;         /* +6   the art kind (labels.c's 0..14)   */
    unsigned char mask;         /* +7   its kind_mask bits                */
    unsigned char num;          /* +8   the entry's number                */
    unsigned char sub;          /* +9   num's top two bits; picks the
                                        label filter on screen 21         */
    unsigned char plain;        /* +a   nonzero: the plain list, no
                                        preview (command 1 toggles it)    */
    unsigned char shown;        /* +b   the kind bit the heading shows    */
    int sel;                    /* +c   the slot being replaced, or < 0   */
    int top;                    /* +e   the list position                 */
    unsigned char first;        /* +10  the lowest number listed          */
    char refresh;               /* +11                                    */
};

struct ArtView {                /* art_run's state, 0x62 bytes            */
    struct ArtSel far *arg;     /* +0                                     */
    char title[0x32];           /* +4   the command line                  */
    unsigned char nkeys;        /* +36  its commands                      */
    char redraw;                /* +37  list_menu's redraw flag           */
    struct Node far *list;      /* +38                                    */
    struct Node far *cur;       /* +3c                                    */
    unsigned char far *tab;     /* +40  the label table, labels_load's    */
    int sets;                   /* +44  geo.area->sets[0], saved          */
    long saved;                 /* +46  exp_share, saved                  */
    struct MapCell cells[3];    /* +4a  geo.area's first squares, saved   */
    unsigned char cursor_13;    /* +5c  cursor_obj[0x13], saved           */
    struct Pos pos;             /* +5d  geo.pos, saved                    */
};

extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far glyph_at(int col, int row, unsigned char glyph, char unused,
                         struct Res far *r);                     /* 05f5:0598 */
extern void far glyph_draw(int col, int row, int glyph, struct Res far *r); /* 05f5:0629 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern void far pal_black(int first, int last);                  /* 05f5:0ce2 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far list_alloc(int n, struct Node far * far *head);  /* 083a:0575 */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern int  far menu_line_key(int key);                              /* 083a:1e2d */
extern void far menu_line_show(char far *title, char far *text, char fkeys); /* 083a:1e9a */
extern void far list_options_set(void (far *extra)(int sel), char anim, char done); /* 083a:20b5 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far pic_load(char far *name, unsigned char set, unsigned char mode,
                         char n);                                /* 0cb5:0007 */
extern void far icon_clear(unsigned char idx);                   /* 0cb5:004d */
extern void far icon_load(char far *name, unsigned char set, unsigned char idx); /* 0cb5:0093 */
extern void far icon_draw(int col, int row, unsigned char kind,
                          unsigned char bank, unsigned char idx); /* 0cb5:0299 */
extern void far pic_free(void);                                  /* 0cb5:0352 */
extern void far icon_size(unsigned char idx, unsigned char bank, int far *rows,
                          int far *cols);                        /* 0cb5:035f */
extern void far txt_pack(char far *src, char far *dst);          /* 0cf0:00ab */
extern void far txt_unpack(char far *src, char far *dst, int n); /* 0cf0:01c2 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_piece_refresh(char c);                              /* 0d65:01c6 */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far frame_list_panel(void);                                /* 0d65:0302 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far clip_set(int row1, int col1, int row2, int col2); /* 107e:05dd */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_poll(void);                                   /* 15e5:15e6 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern int  far ui_query(int id, int what);                      /* 15e5:1936 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far pic_frame(int col, int row, char show, unsigned char n);
                                /* 18de:103f, vector to overlay 3 1bbc:000b */
extern void far pic_open(char far *name, unsigned char id, char anim,
                             struct Pic far *p);
                                /* 18de:1049, vector to overlay 3 1bbc:0077 */
extern void far mode_change(int n);
                                /* 18de:10cb, vector to overlay 3 1b41:02fe */
extern void far big_open(unsigned char id);
                                /* 18de:146d, vector to overlay 3 1bbc:0334 */
extern int  far field_proc();
                                /* 18de:175b, vector to overlay 5 1c86:0000 */
extern void far mod18de_1783(void);
                                /* 18de:1783, vector to overlay 5 1e8f:134f */
extern unsigned char far kind_mask(unsigned char kind, int dir);
                                /* 18de:1797, vector to overlay 5 1e8f:0f1b */
extern int  far label_count(unsigned char kind, unsigned char mask, unsigned char far *lo,
                             unsigned char far *hi, unsigned char far *cur, int far *pos,
                             char (far *filter)(unsigned char));
                                /* 18de:17a1, vector to overlay 5 1e8f:07f1 */
extern int  far label_list(struct Node far *list, unsigned char kind, unsigned char mask,
                             unsigned char first, char wide, char unused,
                             char (far *filter)(unsigned char));
                                /* 18de:17ab, vector to overlay 5 1e8f:099d */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern unsigned char far mask_kind(unsigned char m, int far *dir);
                                /* 18de:17c9, vector to overlay 5 1e8f:0f9e */
extern void far piece_box(int row, int col, int n, int k);
                                /* 18de:17dd, vector to overlay 5 1e8f:1576 */
extern void far area_label(int v, char far *out);
                                /* 18de:17e7, vector to overlay 5 1e8f:0eb5 */
extern void far back_draw(unsigned char col, unsigned char row, unsigned char n,
                             char own, char raw);
                                /* 18de:17fb, vector to overlay 5 1e8f:14c3 */
extern void far msg_box(char far *fmt, ...);
                                /* 18de:1841, vector to overlay 5 1e8f:1359 */
extern int  far labels_save(unsigned char far *buf, int entry);
                                /* 18de:1855, vector to overlay 5 1e8f:015f */
extern int  far labels_load(unsigned char far * far *out, int entry);
                                /* 18de:185f, vector to overlay 5 1e8f:0021 */
extern int  far label_entry(unsigned char kind);
                                /* 18de:1869, vector to overlay 5 1e8f:065d */
extern int  far label_nth(int idx, unsigned char kind, unsigned char mask,
                             unsigned char first, struct Label far * far *out,
                             char (far *filter)(unsigned char));
                                /* 18de:1873, vector to overlay 5 1e8f:0d05 */
extern char far monst_listed(unsigned char n);
                                /* 18de:187d, vector to overlay 25 2e62:0849 */
extern char far monst_unlisted(unsigned char n);
                                /* 18de:1f0d, vector to overlay 25 2e62:086d */
extern char far art_unimport(unsigned char num, unsigned char mask);
                                /* 18de:1f17, vector to overlay 24 2d91:0a06 */
extern char far monst_default(unsigned char num, char far *buf);
                                /* 18de:1f21, vector to overlay 25 2e62:0798 */
extern void far monst_save(unsigned char num);
                                /* 18de:1f2b, vector to overlay 25 2e62:0698 */
extern char far *far monst_load(unsigned char num);
                                /* 18de:1f35, vector to overlay 25 2e62:05a6 */
extern void far view_render(int col, int row, char x, char y, unsigned char dir);
                                                    /* overlay 13 26e3:0d4e */
extern void far wall_sets_free(char reset);         /* overlay 13 26e3:1721 */
extern void far geo_reset(char unused);           /* overlay 13 26e3:1805 */

extern char far *backdrop_text;                          /* 0158  "Backdrop"     */
extern char far *art_names[];                       /* 01b4  "No art", ...  */
extern char far *monster_editor_text;                          /* 01f8  "Monster Editor" */
extern char far *image_text;                          /* 0334  "Image"        */
extern char far *monster_text;                          /* 0338  "Monster"      */
extern char far *replace_text;                          /* 035c  "Replace"      */
extern char far *edit_text;                          /* 0360  "Edit"         */
extern char far *to_text;                          /* 0374  "to"           */
extern char far *with_text;                          /* 0380  "with"         */
extern char far *for_text;                          /* 0384  "for"          */
extern char far *from_text;                          /* 0388  "from"         */
extern char far *view_text;                          /* 03b8  "View"         */
extern char far *select_word;                          /* 03bc  "Select"       */
extern char far *leave_text;                          /* 03c0  "Leave"        */
extern char far *cancel_text;                          /* 03c8  "Cancel"       */
extern char far *name_text;                          /* 03cc  "Name"         */
extern char far *rename_text;                          /* 03d0  "Rename"       */
extern char far *import_text;                          /* 03d4  "Import"       */
extern char far *unimport_text;                          /* 03d8  "Unimport"     */
extern char far *copy_text;                          /* 03dc  "Copy"         */
extern char far *gallery_text;                          /* 03e4  "Gallery"      */
extern char far *loaded_text;                          /* 03ec  "Loaded"       */
extern char far *unavailable_text;                          /* 03f0  "That item is not available." */
extern char far *design_default_text;                          /* 0414  "A Game Design" */
extern char far *day_text;                          /* 0438  "Day"          */
extern char far *night_text;                          /* 043c  "Night"        */
extern char far *combat_word;                          /* 0440  "Combat"       */
extern char far *slot_text;                          /* 0444  "Slot"         */
extern char far *art_word;                          /* 0448  "Art"          */
extern char far *default_text;                          /* 044c  "Default"      */
extern char far *empty_text;                                     /* 3bc0 */
extern int hook_key;                                            /* b384 */
extern char tick_slot;                                           /* b3f2 */
extern long exp_share;                                           /* b41b */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char game_rec[];                                          /* ba08 */
extern struct Res far *big_res;                                  /* bcf7 */
extern struct Pic pic;                                           /* ca22 */
extern char menu_escaped;                                        /* ca2a */
extern struct Res far *wall_set_res[3];                              /* caf8 */
extern struct Res far *pic_res;                                  /* cfd8 */
extern struct Geo geo;                                           /* d414 */

static void far art_reset(struct ArtSel far *p);
static void far art_run(struct ArtSel far *arg);
static char far art_mode(int scr);
static void far art_command(struct ArtView far *c, int key);
static void far art_take(struct ArtView far *c);
static char far art_list(struct ArtView far *c);
static unsigned char far art_menu(struct ArtView far *c);
static char far art_count(struct ArtView far *c, int far *n);
static void far art_noop(struct ArtView far *c);
static void far art_marks(struct ArtView far *c, int n);
static void far key_state(struct ArtView far *c, unsigned char id, char on);
static void far art_title(struct ArtView far *c);
static void far art_heading(struct ArtView far *c, unsigned char kind);
static void far art_frame(struct ArtSel far *p);
static void far art_push(struct ArtView far *c, char on);
static void far art_browse(int n);
static void far art_preview(int n);
static void far preview_mark(int col);
static void far show_pic(int num);
static void far show_sprite(int num);
static void far sprite_frame(int col, int row, unsigned char n);
static void far show_icons(int num);
static void far icon_ground(int row, int col, int set);
static void far show_big(int num);
static void far show_area(int num, char wild);
static void far dung_tiles(int row, int col, unsigned char step);
static void far wild_tiles(int row, int col, unsigned char step);
static void far tile_grid(unsigned char far *p, int row, int col, unsigned char step);
static void far show_walls(int num);
static void far show_backs(int num, int scr);
static void far art_save(struct ArtView far *c);
static void far art_restore(struct ArtView far *c);
static int  far art_label(struct Label far * far *out, struct ArtSel far *p, int n);
static void far area_blank(unsigned char sets);
static void far label_import(struct ArtSel far *p);
static char far label_rename(struct ArtView far *c);
static char far label_unimport(struct ArtView far *c);
static char far back_paired(int n);
static int  far rename_key(int id, int key);

static unsigned char icon_glyphs[16] = {                         /* 8e3a */
    0x0b, 0x0f, 0x02, 0x0e, 0x17, 0x17, 0x05, 0x02,
    0x17, 0x17, 0x17, 0x05, 0x17, 0x17, 0x17, 0x17
};

static unsigned char dung_glyphs[66] = {                         /* 8e4a */
    0x13, 0x12, 0x17, 0x05, 0x04, 0x03, 0x06, 0x12, 0x17, 0x17, 0x01, 0x17, 0x17,
    0x02, 0x16, 0x17, 0x17, 0x05, 0x08, 0x0b, 0x18, 0x17, 0x17, 0x19, 0x06, 0x06,
    0x05, 0x0c, 0x0a, 0x0e, 0x17, 0x09, 0x17, 0x17, 0x17, 0x17, 0x0d, 0x0b, 0x0b,
    0x01, 0x0d, 0x0f, 0x0c, 0x06, 0x12, 0x17, 0x10, 0x12, 0x17, 0x13, 0x06, 0x06,
    0x15, 0x17, 0x17, 0x0d, 0x0b, 0x18, 0x17, 0x11, 0x18, 0x17, 0x02, 0x07, 0x0b
};

static unsigned char wild_glyphs[66] = {                         /* 8e8c */
    0x01, 0x14, 0x23, 0x1b, 0x18, 0x18, 0x1a, 0x1f, 0x0d, 0x10, 0x11, 0x22, 0x08,
    0x0a, 0x04, 0x02, 0x12, 0x17, 0x18, 0x18, 0x1a, 0x1f, 0x0c, 0x03, 0x11, 0x26,
    0x12, 0x08, 0x06, 0x0b, 0x22, 0x1b, 0x1c, 0x1d, 0x1e, 0x20, 0x07, 0x05, 0x26,
    0x25, 0x21, 0x26, 0x0e, 0x13, 0x15, 0x16, 0x18, 0x18, 0x19, 0x15, 0x09, 0x10,
    0x24, 0x10, 0x12, 0x0d, 0x25, 0x0f, 0x1f, 0x1b, 0x18, 0x18, 0x19, 0x12, 0x0c
};

int far art_screen(int prev, struct EdEnv far *env, struct ArtSel far *arg) /* 39f4:0000 */
{
    struct ArtSel far *p;
    register int t;

    if (arg == 0)
        return 0;
    p = arg;
    p->next = prev;
    switch (prev) {
    case 10:
    case 16:
    case 18:
        p->next = p->scr;
        t = p->done;
        p->done = 0;
        if (env->bits & 0xf) {
            if (prev == 16)
                label_import(p);
            else
                (void)0;        /* the else arm's jmp the jne keeps */
        }
        if (p->done == 0)
            art_run(p);
        break;
    case 11:
        t = p->done;
        p->done = 0;
        if (env->bits & 0xf) {
            switch (t & 0xf) {
            case 2:
                p->kind = (env->bits & 0xff0) >> 4;
                p->sel = -1;
                p->mask = kind_mask(p->kind, p->sel);
                if (p->kind == 9)
                    p->mask |= 0x10;
                p->first = env->shift ? 0 : 1;
                p->num = 0;
                env->bits &= 0xffff0000L;
                env->bits |= p->mask << 8 | p->num;
                p->mask &= 0x3f;
                art_reset(p);
            }
        } else if (t == 2) {
            p->done = 1;
            p->next = 1;
        }
        if (p->done == 0)
            art_run(p);
        break;
    case 1:
        geo_reset(cur_area_get());
        p->kind = 0;
        p->done = 2;
        p->next = 0xb;
        break;
    case 21:
        if (p->done)
            p->next = 7;
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
        p->mask = (env->bits & 0xff00) >> 8;
        p->kind = mask_kind(p->mask, &p->sel);
        p->mask &= 0x3f;
        p->first = env->shift ? 0 : 1;
        p->num = p->first;
        if (p->sel > 0)
            p->sel = env->bits & 0xff;
        else
            p->num = env->bits & 0xff;
        art_reset(p);
        art_run(p);
        break;
    default:
        p->next = p->scr;
    }
    env->bits &= 0xffff0000L;
    switch (p->next) {
    case 1:
        break;
    case 10:
        t = p->done & 0x3f;
        env->bits |= t;
        break;
    case 11:
        switch (p->done & 0x3f) {
        case 2:
            env->bits |= p->done | p->kind << 4;
        }
        break;
    default:
        if (p->kind >= 6)
            p->mask |= 0x40;
        if (!(p->done & 0x80))
            p->mask |= 0x80;
        env->bits |= p->mask << 8 | p->num & 0xff;
    }
    return p->next;
}

static void far art_reset(struct ArtSel far *p)                  /* 39f4:0380 */
{
    p->scr = p->next;
    p->refresh = 1;
    if (p->scr != 0x15)
        p->sub = 0xff;
    p->shown = 0xff;
}

static void far art_run(struct ArtSel far *arg)                  /* 39f4:03a8 */
{
    char mode;
    char alloc;
    struct ArtView c;
    register int key;

    if (arg->mask == 0) {
        arg->done = -1;
        arg->next = arg->scr;
    }
    c.arg = arg;
    c.arg->done = 0;
    art_save(&c);
    art_push(&c, 1);
    alloc = art_list(&c);
    if ((mode = art_mode(arg->scr)) != 0 && c.arg->done)
        c.arg->next = 1;
    art_noop(&c);
    art_title(&c);
    art_heading(&c, c.arg->kind);
    while (c.arg->done == 0) {
        if (arg->plain == 0) {
            list_options_set(art_preview, c.arg->kind == 1, !mode);
            key = (unsigned char)list_menu(empty_text, c.title, 1, 0x11, 0x14, 0x17, c.list, 1, 0,
                                           &c.redraw, &c.arg->top, &c.cur);
        } else {
            list_options_set(art_browse, 0, !mode);
            key = (unsigned char)list_menu(empty_text, c.title, 2, 4, 0x26, 0x16, c.list, 1, 0,
                                           &c.redraw, &c.arg->top, &c.cur);
        }
        if (menu_escaped) {
            if (key == 0xd && !mode)
                key = 0;
            else if (key == 0x1b || key == 0x60)
                key = mode ? 0 : c.nkeys - 1;
        }
        if (key == 0 && c.arg->sel >= 0 && c.arg->top == c.arg->sel) {
            msg_box(unavailable_text, 0);
            c.redraw = 1;
            key = -1;
        }
        if (key >= 0 && c.nkeys > key)
            art_command(&c, key);
    }
    if (alloc)
        list_free(&c.list);
    art_push(&c, 0);
    art_restore(&c);
    if (c.arg->done > 0 && (art_mode(c.arg->next) && c.arg->next != 0x12 || c.arg->next < 9
                          || c.arg->next == 0x10 || c.arg->next == 0x15))
        art_take(&c);
}

static char far art_mode(int scr)                                /* 39f4:0624 */
{
    if (scr == 0xb || scr == 1)
        return 1;
    if (scr == 7)
        return -1;
    return 0;
}

static void far art_command(struct ArtView far *c, int key)      /* 39f4:0647 */
{
    char mode;

    if ((mode = art_mode(c->arg->scr)) < 0) {
        if (key == 0) {
            key = 2;
            mode = 0;
        } else if (key == 3)
            key = 0;
    } else if (mode == 0 && c->nkeys == 2 && key)
        key = 2;
    switch (key) {
    case 0:
        if (c->arg->next == 0xb)
            c->arg->done = 2;
        else
            c->arg->done = 1;
        break;
    case 1:
        c->arg->plain = ++c->arg->plain % 2;
        art_title(c);
        c->arg->shown = 0xff;
        art_heading(c, c->arg->kind);
        c->redraw = 1;
        break;
    default:
        if (mode)
            switch (key) {
            case 2:
                c->arg->done = 1;
                c->arg->next = 0x12;
                break;
            case 3:
                c->arg->done = 1;
                c->arg->next = 0x10;
                break;
            case 4:
                if (mode < 0) {
                    label_rename(c);
                    c->redraw = 1;
                } else
                    label_unimport(c);
                break;
            case 5:
                if (mode < 0) {
                    c->arg->done = 1;
                    c->arg->next = 0x15;
                }
                break;
            }
        else if (key == 2) {
            c->arg->num = 0;
            c->arg->done = -1;
            c->arg->mask &= 0x7f;
        }
    }
}

static void far art_take(struct ArtView far *c)                  /* 39f4:07eb */
{
    struct Label far *lab;

    c->arg->num = art_label(&lab, c->arg, c->arg->top);
    if (lab) {
        c->arg->mask &= lab->kinds & 0x3f;
        if (c->arg->kind >= 6)
            c->arg->mask |= 0x40;
        c->arg->kind = mask_kind(c->arg->mask, 0);
    }
    if (c->arg->kind == 9 && back_paired(c->arg->num))
        c->arg->num &= 0xfe;
}

static char far art_list(struct ArtView far *c)                  /* 39f4:08a1 */
{
    int n;
    register int i;

    c->tab = 0;
    if (!art_menu(c) || !art_count(c, &n)) {
        c->arg->done = 1;
        return 0;
    }
    c->list = 0;
    list_alloc(n, &c->list);
    if ((c->cur = c->list) == 0) {
        c->arg->done = 1;
        return 1;
    }
    if (c->arg->kind == 9 && c->arg->scr == 2) {
        c->tab = 0;
        for (i = 1; c->cur; i++) {
            str_format(c->cur->text, "%s %d", backdrop_text, i);
            c->cur->key = 0;
            c->cur = c->cur->next;
        }
    } else if (c->tab)
        label_list(c->list, c->arg->kind, c->arg->mask, c->arg->first, 0, c->arg->plain == 0,
                     c->arg->scr == 0x15 ? (c->arg->sub ? monst_listed : monst_unlisted) : 0);
    else
        c->list = 0;
    if (c->list) {
        c->redraw = 1;
        for (c->cur = c->list; c->cur && c->cur->key; c->cur = c->cur->next)
            ;
        for (i = 0; c->arg->top > i && c->cur; c->cur = c->cur->next)
            if (c->cur->key == 0)
                i++;
        if (c->cur == 0)
            c->cur = c->list;
    } else
        c->arg->done = 1;
    return 2;
}

static unsigned char far art_menu(struct ArtView far *c)         /* 39f4:0b2d */
{
    char far *verb;
    char far *last;
    char mode;

    verb = last = 0;
    c->nkeys = 0;
    if ((mode = art_mode(c->arg->scr)) != 0) {
        verb = leave_text;
        if (mode > 0 && c->arg->kind >= 6)
            c->nkeys = 3;
        else if (mode < 0)
            c->nkeys = 6;
        else
            c->nkeys = 5;
        last = empty_text;
    } else
        switch (c->arg->scr) {
        case 2:
            if (c->arg->kind == 9 && c->arg->sel >= 0) {
                verb = replace_text;
                c->nkeys = 2;
                goto cancel;
            }
            goto select;
        case 3:
        case 4:
        case 0x15:
            if (c->arg->kind == 9 || c->arg->kind == 8) {
                c->arg->sub = (c->arg->num & 0xc0) >> 6;
                c->arg->num &= 0x3f;
            }
        select:
            verb = select_word;
        three:
            c->nkeys = 3;
        cancel:
            last = cancel_text;
            break;
        default:
            if (c->arg->scr < 0x16) {
                if (verb)
                    goto three;
                goto select;
            }
        }
    if (c->nkeys) {
        hotkey_reset(c->nkeys - 1);
        if (c->nkeys == 2)
            str_format(c->title, "%s %s", verb, last);
        else if (mode) {
            if (c->arg->scr == 7)
                str_format(c->title, "%s %s %s %s %s %s", verb, view_text, rename_text, edit_text,
                           default_text, copy_text);
            else if (c->nkeys == 3)
                str_format(c->title, "%s %s %s", verb, view_text, rename_text);
            else
                str_format(c->title, "%s %s %s %s %s", verb, view_text, rename_text, import_text,
                           unimport_text);
        } else
            str_format(c->title, "%s %s %s", verb, view_text, last);
    }
    return c->nkeys;
}

static char far art_count(struct ArtView far *c, int far *n)     /* 39f4:0ddc */
{
    *n = 0;
    switch (c->arg->kind) {
    case 9:
        if (c->arg->scr == 2) {
            c->arg->first = 0;
            *n = 4;
            break;
        }
    case 6:
    case 7:
    case 8:
        if (c->arg->refresh) {
            if (c->arg->num == 0)
                c->arg->num = 0xff;
            if (c->arg->sel == 0)
                c->arg->sel = 0xff;
            if (c->arg->sub && c->arg->sub != 0xff)
                c->arg->first = 0;
        }
    }
    if (c->arg->refresh && c->arg->kind == 9 && c->arg->scr == 2) {
        c->arg->num = int_max(c->arg->num, c->arg->first);
        c->arg->top = c->arg->num - c->arg->first;
        if (c->arg->sel == c->arg->num)
            c->arg->top = (c->arg->top + 1) % (*n + 1);
    } else {
        if (labels_load(&c->tab, c->arg->kind < 6 ? 1 : 2) == 0)
            return 0;
        *n = label_count(c->arg->kind, c->arg->mask, &c->arg->first, 0, &c->arg->num,
                          c->arg->refresh ? &c->arg->top : 0,
                          c->arg->scr == 0x15 ? (c->arg->sub ? monst_listed : monst_unlisted) : 0);
    }
    c->arg->refresh = 0;
    return *n > 0;
}

static void far art_noop(struct ArtView far *c)                  /* 39f4:1031 */
{
}

static void far art_marks(struct ArtView far *c, int n)          /* 39f4:1036 */
{
    char mode;
    struct Label far *lab;

    if ((mode = art_mode(c->arg->scr)) != 0 && c->arg->kind < 6) {
        art_label(&lab, c->arg, (c->tab ? 0 : c->arg->first) + n);
        if (mode > 0) {
            if (lab && lab->kinds & 0x80)
                key_state(c, 5, 0);
            else
                key_state(c, 5, 1);
            if (lab->num > 0x7f)
                key_state(c, 3, 0);
            else
                key_state(c, 3, 1);
        } else {
            if (lab && lab->kinds & 0x40)
                key_state(c, 5, 0);
            else
                key_state(c, 5, 1);
        }
    }
}

static void far key_state(struct ArtView far *c, unsigned char id, char on) /* 39f4:114d */
{
    if (c->nkeys >= id) {
        if (on > 0) {
            if (ui_query(id, 0x11) == 0 && ui_query(id, 0x10) != 0)
                return;
            ui_mark(id, 0x19);
            ui_mark(id, 0x10);
        } else if (on < 0) {
            if (ui_query(id, 0x11))
                return;
            ui_mark(id, 0x11);
        } else {
            if (ui_query(id, 0x11) == 0 && ui_query(id, 0x10) == 0)
                return;
            ui_mark(id, 0x19);
            ui_mark(id, 0x18);
        }
        c->redraw = 1;
    }
}

static void far art_title(struct ArtView far *c)                 /* 39f4:123b */
{
    char buf[39];
    char flag;
    register int col;

    art_frame(c->arg);
    if ((flag = c->arg->plain == 0) != 0) {
        if (art_mode(c->arg->scr) < 0) {
            str_format(buf, "%s", monster_editor_text);
            col = flag ? 0x1fa0 : (0x26 - str_len(buf)) / 2 * 4 + 0x1f44;
        } else {
            str_format(buf, "%s %s", art_word, gallery_text);
            col = flag ? 0x1fa4 : (0x26 - str_len(buf)) / 2 * 4 + 0x1f44;
        }
        text_print_at(0x1f84, col, 0x8c, buf);
        if (flag && geo.area->version) {
            mem_move(geo.area->name, buf, 0x10);
            buf[16] = 0;
            if (buf[0] == 0)
                area_label(cur_area_get(), buf);
        } else if (game_rec[0])
            str_format(buf, game_rec);
        else
            str_format(buf, design_default_text);
        col = flag ? 0x1f9c : (0x26 - str_len(buf)) / 2 * 4 + 0x1f44;
        if (flag) {
            buf[16] = 0;
            text_print_at(0x1f88, col, 0x87, buf);
        } else {
            text_print_at(0x1f88, col, 0x87, buf);
            if (geo.area->version) {
                mem_move(geo.area->name, buf, 0x10);
                buf[16] = 0;
                if (buf[0] == 0)
                    area_label(cur_area_get(), buf);
                text_print_at(0x1f8c, (0x26 - str_len(buf)) / 2 * 4 + 0x1f44, 0x87, "%s",
                              buf);
            }
        }
    }
}

static void far art_heading(struct ArtView far *c, unsigned char kind) /* 39f4:149c */
{
    char buf[40];
    char far *s;
    char plain;
    char copy;
    char mode;
    register int col;

    mode = art_mode(c->arg->scr);
    copy = c->arg->scr == 0x15;
    if ((plain = c->arg->plain == 0) != 0)
        text_print_at(0x1f8c, 0x1f9c, 0x8b, "%s %s", mode ? view_text : select_word,
                      copy ? "" : image_text);
    str_copy(buf, mode >= 0 && !copy ? art_names[kind] : monster_text);
    if (c->arg->scr == 2)
        str_cat(buf, msg_format(" %s", slot_text));
    else if (mode && kind)
        str_cat(buf, kind == 7 ? "es" : "s");
    if (plain)
        text_print_at(0x1f90, 0x1f9c, 0x87, "%*s", 0x10, buf);
    else if (copy || mode < 0)
        s = msg_format("%s %s", select_word, buf);
    else
        s = msg_format("%s %s: %s", select_word, image_text, buf);
    if (c->arg->scr == 2 && c->arg->sel >= 0) {
        if (plain) {
            text_print_at(0x1f94, 0x1f9c, 0x87, "%s %s", to_text, replace_text);
            text_print_at(0x1f98, 0x1f9c, 0x87, "%s %d", with_text, c->arg->sel + 1);
        } else
            str_format(buf, "%s %s", to_text, replace_text);
    } else if (c->arg->scr == 3 && kind >= 6) {
        if (kind >= 8)
            str_format(buf, "%s", loaded_text);
        else
            str_format(buf, "%s %s", for_text, combat_word);
        if (plain)
            text_print_at(0x1f94, 0x1f9c, 0x87, buf);
    } else if (copy) {
        str_format(buf, "%s %s %s", to_text, copy_text, from_text);
        if (plain)
            text_print_at(0x1f94, 0x1f9c, 0x87, buf);
    } else
        buf[0] = 0;
    if (!plain) {
        if (buf[0]) {
            str_cat(s, " ");
            str_cat(s, buf);
        }
        str_copy(buf, s);
        col = (0x26 - str_len(buf)) / 2 * 4 + 0x1f44;
        text_print_at(0x1f44, 0x1f44, 0x8f, "%*s", 0x26, "");
        text_print_at(0x1f44, col, 0x8f, buf);
    }
}

static void far art_frame(struct ArtSel far *p)                  /* 39f4:183c */
{
    if (p->plain == 0) {
        frame_lower_panel();
        clip_set(0x1f82, 0x1f40, 0x1f9d, 0x1fe0);
        draw_glyph(0x1f60, 0x1f4c, 1, 8);
        clip_reset();
    } else
        frame_list_panel();
    screen_flush();
}

static void far art_push(struct ArtView far *c, char on)         /* 39f4:188f */
{
    if (on) {
        c->saved = exp_share;
        exp_share = (long)c;
    } else
        exp_share = c->saved;
}

static void far art_browse(int n)                                /* 39f4:18cc */
{
    struct ArtView far *c;
    unsigned char bits;
    unsigned char bit;
    short i;
    struct Label far *lab;
    register int num;

    if ((c = (struct ArtView far *)exp_share) != 0) {
        art_marks(c, n);
        num = art_label(&lab, c->arg, (c->tab ? 0 : c->arg->first) + n);
        bits = c->arg->mask;
        if (c->tab && (lab && (bits &= lab->kinds) || !num || num == 0xff && c->arg->kind >= 6)) {
            if (c->arg->kind < 6) {
                for (i = 0; i < 6; i++, bits >>= 1)
                    if (bits & 1) {
                        if (c->arg->shown != (bit = 1 << i))
                            art_heading(c, mask_kind(bit, 0));
                        i = 0x63;
                        c->arg->shown = bit;
                    }
            } else {
                for (i = 0; i < 5; i++, bits >>= 1)
                    if (bits & 1) {
                        if (c->arg->shown != (bit = 1 << i | 0x40))
                            art_heading(c, mask_kind(bit, 0));
                        i = 0x63;
                        c->arg->shown = bit;
                    }
            }
        }
    }
}

static void far art_preview(int n)                               /* 39f4:1a58 */
{
    struct ArtView far *c;
    unsigned char bits;
    unsigned char bit;
    short i;
    struct Label far *lab;
    register int num;

    if ((c = (struct ArtView far *)exp_share) != 0) {
        art_marks(c, n);
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        num = art_label(&lab, c->arg, (c->tab ? 0 : c->arg->first) + n);
        bits = c->arg->mask;
        if (c->tab && (lab && (bits &= lab->kinds) || !num || num == 0xff && c->arg->kind >= 6)) {
            if (c->arg->kind < 6) {
                for (i = 0; i < 6; i++, bits >>= 1)
                    if (bits & 1) {
                        if (c->arg->shown != (bit = 1 << i)) {
                            if (c->arg->shown == 1) {
                                clip_set(0x1f40, 0x1f40, 0x1f82, 0x1fe0);
                                frame_lower_panel();
                                clip_reset();
                                frame_piece_refresh(2);
                                screen_flush();
                            }
                            fill_rect(0x1f44, 0x1f44, 0x1f80, 0x1fdc, 8);
                            art_heading(c, mask_kind(bit, 0));
                        }
                        switch (i) {
                        case 0:
                            preview_mark(0);
                            show_pic(num);
                            frame_piece_refresh(0);
                            i = 0x63;
                            break;
                        case 1:
                            if (c->arg->shown != bit) {
                                mod18de_1783();
                                pal_black(0x20, 0xff);
                            }
                            show_sprite(num);
                            if (c->arg->shown != bit && (is_xvga() || pixel_shift() == 0))
                                res_palette_load(wall_set_res[0]);
                            i = 0x63;
                            break;
                        case 2:
                        case 3:
                            show_icons(num);
                            i = 0x63;
                            break;
                        case 4:
                        case 5:
                            show_big(num);
                            i = 0x63;
                            break;
                        }
                        c->arg->shown = bit;
                    }
            } else {
                for (i = 0; i < 5; i++, bits >>= 1)
                    if (bits & 1) {
                        if (c->arg->shown != (bit = 1 << i | 0x40)) {
                            fill_rect(0x1f44, 0x1f44, 0x1f80, 0x1fdc, 8);
                            art_heading(c, mask_kind(bit, 0));
                        }
                        switch (i) {
                        case 0:
                        case 1:
                            show_area(num, i == 1);
                            i = 0x63;
                            break;
                        case 2:
                            show_walls(num);
                            i = 0x63;
                            break;
                        case 3:
                        case 4:
                            preview_mark(0);
                            show_backs(num, c->arg->scr);
                            frame_piece_refresh(0);
                            i = 0x63;
                            break;
                        }
                        c->arg->shown = bit;
                    }
            }
        } else if (c->tab == 0 && c->arg->kind == 9 && c->arg->scr == 2) {
            preview_mark(0);
            show_backs(num, c->arg->scr);
            frame_piece_refresh(0);
        }
        screen_flush_mode_set(0);
        screen_flush();
    }
}

static void far preview_mark(int col)                            /* 39f4:1dc8 */
{
    if (col == 0)
        frame_piece_refresh(2);
    draw_glyph(0x1f40, col + 0x1f40, 1, 9);
}

static void far show_pic(int num)                                /* 39f4:1df6 */
{
    register int row;
    register int col;

    if (num == 0) {
        col = row = 0x1f4c;
        fill_rect(row, col, row + 0x2c, col + 0x2c, 0);
    } else {
        pic_open("PIC", num, 0, &pic);
        pic_frame(3, 3, 1, 0);
    }
}

static void far show_sprite(int num)                             /* 39f4:1e55 */
{
    int k;
    unsigned char step;
    register int col;
    register int row;

    col = 2;
    k = col;
    col <<= 2;
    col += 0x1f40;
    row = 0x1f4c;
    step = 0x30;
    if (num) {
        pic_open("SPRIT", num, 1, &pic);
        pal_black(0xb0, 0xff);
    }
    for (tick_slot = 0; tick_slot < 3; tick_slot++, col += step, k += step >> 2) {
        fill_rect(row, col, row + 0x2c, col + 0x2c, 0);
        if (num) {
            back_draw(k - 1, 2, 9, 0, 1);
            view_render(col, row, geo.pos.x, geo.pos.y, geo.pos.facing);
            sprite_frame(k, 3, tick_slot + 1);
        }
    }
}

static void far sprite_frame(int col, int row, unsigned char n)  /* 39f4:1f30 */
{
    if (pic.res) {
        glyph_draw(col, row, n, pic.res);
        res_palette_load(pic.res);
    }
}

static void far show_icons(int num)                              /* 39f4:1f68 */
{
    short i;
    int rows;
    int cols;
    register int col;
    register int row;

    mode_change(4);
    row = 0x1f4c;
    col = 0x1f5a;
    for (i = 0; i < 2; i++, col += 0x3c) {
        if (num == 0)
            fill_rect(row, col, row + 0x30, col + 0x30, 0);
        else {
            icon_ground(row, col, 2);
            icon_load("CPIC", num, 8);
            icon_size(8, i, &rows, &cols);
            icon_draw(i * 5 + 3, 4 - rows, 0, i, 8);
            icon_clear(8);
        }
    }
}

static void far icon_ground(int row, int col, int set)           /* 39f4:2023 */
{
    unsigned char j;
    unsigned char i;
    unsigned char step;
    unsigned char far *p;
    register int r;
    register int c;

    step = 0xc;
    pic_load("DungCom1", set, 0, 0);
    pal_select(pic_res);
    for (i = 0, r = row, p = icon_glyphs; i < 4; i++, r += step)
        for (j = 0, c = col; j < 4; j++, c += step)
            glyph_at(c, r, *p++, 0, pic_res);
    pic_free();
}

static void far show_big(int num)                                /* 39f4:20b8 */
{
    if (num == 0)
        fill_rect(0x1f44, 0x1f44, 0x1f80, 0x1fdc, 0);
    else {
        big_open(num);
        glyph_draw(1, 1, 1, big_res);
        res_palette_load(big_res);
    }
}

static void far show_area(int num, char wild)                    /* 39f4:2109 */
{
    int row2;
    int col2;
    unsigned char step;
    register int row;
    register int col;

    mode_change(4);
    col = row = 0x1f44;
    row2 = 0x1f80;
    col2 = 0x1fdc;
    step = 0xc;
    clip_set(row, col, row2, col2);
    if (num == 0xff)
        fill_rect(row, col, row2, col2, 0);
    else if (wild) {
        pic_load("WildCom1", num, 0, 0);
        pal_select(pic_res);
        wild_tiles(row, col, step);
    } else {
        pic_load("DungCom1", num, 0, 0);
        pal_select(pic_res);
        dung_tiles(row, col, step);
    }
    pic_free();
    clip_reset();
}

static void far dung_tiles(int row, int col, unsigned char step) /* 39f4:21d4 */
{
    tile_grid(dung_glyphs, row, col, step);
}

static void far wild_tiles(int row, int col, unsigned char step) /* 39f4:21ed */
{
    tile_grid(wild_glyphs, row, col, step);
}

static void far tile_grid(unsigned char far *p, int row, int col, unsigned char step) /* 39f4:2206 */
{
    unsigned char j;
    unsigned char i;
    register int r;
    register int c;

    for (i = 0, r = row; i < 5; i++, r += step)
        for (j = 0, c = col; j < 0xd; j++, c += step)
            glyph_at(c, r, *p++, 0, pic_res);
}

static void far show_walls(int num)                              /* 39f4:2265 */
{
    unsigned char k;
    register int col;
    register int row;

    row = 0x1f4e;
    col = 0x1f46;
    geo.area->sets[0] = num;
    if (num < 0xff)
        wall_sets_free(1);
    fill_rect(row + 0x1c, col, row + 0x1e, col + 0x96, 8);
    for (k = 1; k <= 5; k++, col += 0x1e)
        piece_box(row, col, k, 6);
}

static void far show_backs(int num, int scr)                     /* 39f4:22df */
{
    unsigned char i;
    unsigned char j;
    unsigned char count;
    unsigned char len;
    int gr;
    int gc;
    int step;
    char buf[10];
    register int col;
    register int row;

    mode_change(0);
    count = !art_mode(scr) + 1;
    if (scr == 2)
        num = geo.area->back[num];
    col = row = 0x1f4c;
    step = 0x44;
    for (i = 0; i < count; i++, num++, col += step) {
        if (i == 0 || back_paired(num)) {
            if (i)
                preview_mark(step);
            fill_rect(row, col, row + 0x2c, col + 0x2c, 0);
            back_draw(i % 2 * 0x11 + 2, 2, num, i, 1);
            if (back_paired(num)) {
                str_format(buf, "%s %c", num & 1 ? night_text : day_text, (num - 0x20) / 2 + 'A');
                gr = row + 0x2f;
                gc = col + 0x16 - (len = str_len(buf)) * 4 / 2;
                draw_glyph(gr, gc, 1, 0xd);
                for (j = 0; j < len; j++)
                    draw_glyph(gr, j * 4 + gc, 1, 0xe);
                draw_glyph(gr, (j - 1) * 4 + gc, 1, 0xf);
                text_xy(gr, gc, &gr, &gc);
                text_print_at(gr + 1, gc, 0x87, buf);
            }
        } else if (i)
            fill_rect(row - 8, col - 8, row + 0x34, col + 0x3a, 8);
    }
}

static void far art_save(struct ArtView far *c)                  /* 39f4:24e4 */
{
    unsigned char m;
    unsigned char k;
    unsigned char i;
    unsigned char far *q;
    unsigned char bits;

    bits = c->arg->mask;
    if (c->arg->kind < 6) {
        for (i = 0; i < 6; i++, bits >>= 1)
            if (bits & 1)
                switch (i) {
                case 1:
                    if (geo.area->version == 0)
                        area_blank(5);
                    c->cursor_13 = cursor_obj[0x13];
                    cursor_obj[0x13] = 5;
                    c->pos = geo.pos;
                    geo.pos.x = geo.pos.y = 0;
                    geo.pos.facing = 2;
                    for (m = 0; m < 3; m++) {
                        c->cells[m] = geo.area->cells_head[m];
                        mem_fill(&geo.area->cells_head[m], 6, 0);
                    }
                    for (m = 0; m < 3; m++) {
                        for (k = 0; k < 3; k += 2) {
                            q = &geo.area->cells_head[m].wall[k];
                            if (k == 0 && (m == 0 || m == 2)) {
                                *q = *q & 0xf0 | 4;
                                *q = *q & 0xf | 0;
                            } else if (k == 2 && m == 1) {
                                *q = *q & 0xf0 | 3;
                                *q = *q & 0xf | 0;
                            } else {
                                *q = *q & 0xf0 | 1;
                                *q = *q & 0xf | 0xe0;
                            }
                        }
                        if (m == 2) {
                            q = &geo.area->cells_head[m].wall[1];
                            *q = *q & 0xf0 | 4;
                            *q = *q & 0xf | 0;
                        }
                        q = &geo.area->cells_head[m].bits;
                        *q = *q & 0xfc | 1;
                    }
                    c->sets = geo.area->sets[0];
                    geo.area->sets[0] = 1;
                    mod18de_1783();
                }
    } else {
        for (i = 0; i < 5; i++, bits >>= 1)
            if (bits & 1)
                switch (i) {
                case 2:
                    if (geo.area->version == 0)
                        area_blank(5);
                    c->cursor_13 = cursor_obj[0x13];
                    cursor_obj[0x13] = 5;
                    c->sets = geo.area->sets[0];
                }
    }
}

static void far art_restore(struct ArtView far *c)               /* 39f4:276d */
{
    unsigned char m;
    unsigned char i;
    unsigned char bits;

    bits = c->arg->mask;
    if (c->arg->kind < 6) {
        for (i = 0; i < 6; i++, bits >>= 1)
            if (bits & 1)
                switch (i) {
                case 1:
                    geo.pos = c->pos;
                    cursor_obj[0x13] = c->cursor_13;
                    for (m = 0; m < 3; m++)
                        geo.area->cells_head[m] = c->cells[m];
                    geo.area->sets[0] = c->sets;
                    mod18de_1783();
                }
    } else {
        for (i = 0; i < 5; i++, bits >>= 1)
            if (bits & 1)
                switch (i) {
                case 2:
                    cursor_obj[0x13] = c->cursor_13;
                    geo.area->sets[0] = c->sets;
                    mod18de_1783();
                }
    }
}

static int far art_label(struct Label far * far *out, struct ArtSel far *p, int n) /* 39f4:2881 */
{
    if (p->kind == 9 && p->scr == 2) {
        if (out)
            *out = 0;
        return n;
    }
    if (p->first == 0 && n == 0) {
        if (out)
            *out = 0;
        return p->kind >= 6 ? 0xff : 0;
    }
    return label_nth(n, p->kind, p->mask, p->first, out,
                        p->scr == 0x15 ? (p->sub ? monst_listed : monst_unlisted) : 0);
}

static void far area_blank(unsigned char sets)                   /* 39f4:2936 */
{
    register int i;

    if (geo.area->version == 0) {
        mem_fill(geo.area, 0x122, 0);
        if (sets && sets <= 4) {
            geo.area->rows = 0xf;
            geo.area->cols = 0x26;
        } else {
            geo.area->rows = 0xb;
            geo.area->cols = 0xb;
        }
        for (i = 0; i < 3; i++)
            geo.area->sets[i] = 1;
    }
}

static void far label_import(struct ArtSel far *p)               /* 39f4:299a */
{
    struct Label far *lab;

    art_label(&lab, p, p->top);
    if (lab) {
        lab->kinds |= 0x80;
        labels_save(0, label_entry(mask_kind(lab->kinds, 0)));
    }
}

static char far label_rename(struct ArtView far *c)              /* 39f4:29f0 */
{
    char rc;
    char buf[16];
    struct Label far *lab;

    rc = 0;
    art_label(&lab, c->arg, c->arg->top);
    if (lab && monst_default(lab->num, buf)) {
        txt_pack(buf, lab->text);
        lab->kinds &= 0xbf;
        if (labels_save(0, label_entry(mask_kind(lab->kinds, 0)))) {
            rc = 1;
            label_list(c->list, c->arg->kind, c->arg->mask, c->arg->first, 0, c->arg->plain == 0,
                         c->arg->scr == 0x15 ? (c->arg->sub ? monst_listed : monst_unlisted) : 0);
        }
    }
    return rc;
}

static char far label_unimport(struct ArtView far *c)            /* 39f4:2b12 */
{
    char rc;
    struct Label far *lab;

    rc = 0;
    art_label(&lab, c->arg, c->arg->top);
    if (lab && art_unimport(lab->num, c->arg->mask & lab->kinds & 0x3f)) {
        lab->kinds &= 0x7f;
        if (labels_save(0, label_entry(mask_kind(lab->kinds, 0))))
            rc = 1;
    }
    return rc;
}

static char far back_paired(int n)                               /* 39f4:2ba3 */
{
    return n >= 0x20 && n < 0xff;
}

int far rename_screen(int prev, struct EdEnv far *env, struct ArtSel far *arg) /* 39f4:2bbc */
{
    char is_mon;
    char marked;
    char armed;
    char title[26];
    char text[40];
    char name[16];
    int nkeys;
    struct FieldSave field;
    struct ArtSel far *p;
    struct Label far *lab;
    char far *rec;
    register int rc;
    register int key;

    if (arg == 0)
        return 0;
    p = arg;
    is_mon = art_mode(p->scr) < 0;
    art_label(&lab, p, p->top);
    if (lab == 0)
        env->bits &= 0xfffffff0L;
    else {
        txt_unpack(lab->text, name, 0xf);
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        frame_border();
        str_format(title, "%s %s", rename_text, is_mon ? monster_text : image_text);
        text_print_at(0x1f48, (0x26 - str_len(title)) * 4 / 2 + 0x1f44, 0x8c, title);
        marked = armed = 0;
        nkeys = 1;
        str_format(title, "%s %s", rename_text, cancel_text);
        dl_begin();
        ui_add(7, rename_key, 0x14, 0);
        str_copy(text, name);
        field.label[0] = 0;
        ui_add(8, field_proc, 0x27, text, 0x26, 0x70, 0x23, &field, 0x28, 0x1f68, 0x1f78, 0x2a,
               0xf, 0x14, 0);
        hotkey_reset(nkeys);
        menu_line_show(empty_text, title, 0);
        if (marked)
            ui_mark(4, 0x10);
        text_print_at(0x1f68, 0x1f64, 0x87, "%s:", name_text);
        dl_draw(1);
        screen_flush_mode_set(0);
        screen_flush();
        do {
            while ((key = ui_poll()) < 0)
                input_poll();
            if ((rc = menu_line_key(key)) >= 0) {
                if (menu_escaped) {
                    if (rc == 0xd)
                        rc = 0;
                    else
                        rc = nkeys;
                    menu_escaped = 0;
                }
            } else if (key == 0) {
                if (hook_key == 9)
                    ui_mark(1, 0x12);
                else if (ui_query(1, 0x12)) {
                    if (hook_key == 0x1b && armed && !marked) {
                        marked = 1;
                        ui_mark(4, 0x10);
                    }
                    ui_mark(1, 5, hook_key);
                } else if (hook_key == 0x1b)
                    rc = nkeys;
                else
                    rc = 0;
            }
            if (marked && str_cmp(text, field.text)) {
                marked = 0;
                ui_mark(4, 0x18);
            }
        } while (rc && rc != nkeys);
        dl_end();
        env->bits &= 0xffff0000L;
        if (rc == 0) {
            if (ui_query(rc, 0x12))
                ui_mark(rc, 0x1a);
            if (nkeys > 1)
                art_label(&lab, p, p->top);
            if (str_cmp(name, text)) {
                txt_pack(text, lab->text);
                labels_save(0, label_entry(p->kind));
                if (is_mon && (rec = monst_load(lab->num)) != 0) {
                    str_copy(rec + 0x60, text);
                    monst_save(lab->num);
                }
            }
            env->bits |= 1;
        }
    }
    return prev;
}

static int far rename_key(int id, int key)                       /* 39f4:2ff5 */
{
    if (key == 9 || key == 0xd || key == 0x1b) {
        hook_key = key;
        return 1;
    }
    hook_key = 0;
    return 0;
}
