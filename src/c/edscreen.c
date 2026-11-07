/* CKIT.EXE overlay section 18, module 2cf5: the area editor's screen.
 * ed_defaults puts the editor's settings back: the view's attributes, the
 * "Undo" text, the tool and the view as ed_mode's nibbles hold them
 * (ed_mode_get unpacks what mapedit.c's ed_mode_set packs), walls
 * blocking, the seven paint values cleared, no ed_state, and a facing of
 * 0 made 8 (mapedit.c's north).  ed_start and ed_enter start from it; ed_enter also sizes
 * the map window (ed_window) and puts the party's place in the settings.
 * ed_open and shown_clear reset the map editor record: the pointer flag
 * (hover = !f5, f6 = 0), erase, and the eight values the status panel
 * shows.  ed_frame draws the
 * screen's frame for the 3D view (0) or the map (1), with or without the
 * menu glyph.  ed_layout lays the screen out: the dialog items
 * (ed_items: the menu bar, the key filter, the view's areas, the hotkeys
 * and the command line), the "Undo" mark, and the menu bar (ed_menus: the
 * File, Map and Utilities menus, the view and the tool marked, and while
 * labels.c's cur_area (cur_area_get) is at most 4, the 3D view,
 * Backdrop placement and Display access not pickable and "Block
 * placement" for "Wall placement").
 *
 * Section 18 (parent section 15, itself a child of 13; no section names 18
 * as its parent): payload at EXE 0x06b620, link segment 29f4, which
 * sections 16 and 17 share.  A bare stored SEG:OFF in this file means
 * section 18.  The module is stored frame 2cf5 (load-based 3cf5), EXE
 * 0x06e63f..0x06ed69 (1,834 bytes, 11 routines), from 2cf5:000f, the byte
 * after module 29f4's last retf (2cf5:0000..000e is that module's), to
 * the section's last retf; seven zero bytes follow to the section's end
 * (0x06ed70).  One Turbo C large-model _TEXT segment, built with Turbo C
 * 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the far
 * pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 18's relocation table files 55 sites under 2cf5, all in this
 * range (EXE 0x06e64b..0x06ed2b): 52 far calls and three segment pushes
 * (18de for the handlers 18de:1e45 and 18de:1d55, 2999 for section 15's
 * bar_proc).  Nine of the section's eleven vectors land here, stubs
 * 18de:1f3f..1f8f, which section 21 far-calls (every one) and section 23
 * (1f3f, 1f53, 1f71, 1f85, 1f8f); ed_defaults and ed_menus are reached
 * only by this module's far calls.  The module far-calls six of its
 * routines ahead of their definitions (ed_defaults twice), so those are
 * public; the other five are vector targets.  No far call outside section
 * 18 stores frame 2cf5.  It calls section 13's view_attrs and section
 * 15's menubar.c directly, and mapedit.c (section 16), labels.c (section
 * 5) and section 21 through vector stubs.
 *
 * FRUA's own code: the sharing map pairs ten routines a-only and ed_items
 * partial (top partner DQK menu.c's menu_bar, a name FRUA's menu.c gives
 * 183a:13fd), so every name here is chosen from the code.
 *
 * _DATA is DGROUP 8f2a..8f2e, menu_glyph, the glyph file's name (the same
 * text as two literals, stored once by -d, bakes the same); only this
 * module's code names 8f2a.  Below it module 29f4's _DATA ends at 8f29;
 * 8f2f is a zero pad and rest.c's (section 19) _DATA starts at 8f30.  No
 * _BSS: the module reads and writes mapedit.c's _BSS
 * (section 16) and view3d.c's geo.  The menus and entries at 007c..0147
 * and the texts at 024c and 0404 are no landed module's; their names are
 * placeholders.
 *
 * Forced by the bytes: EdState's view is unsigned char (as char, the
 * calls that pass it widen it with cbw, not mov ah,0); ed_mode's high
 * nibble is read through an unsigned char cast (char gives sar, not shr);
 * ed_start's argument is unsigned char and the stub's parameter wider
 * (a char argument widens with cbw; a char stub parameter pushes the
 * word unchanged); ed_frame's switch with
 * case 1 first (case 0 first, or an if chain, moves the code).
 */

struct Entry {                  /* one menu entry, 8 bytes, as menubox.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6                                     */
    char flags;                 /* +7   4 a mark, 0x81 not pickable       */
};

struct Menu {                   /* a pop-up menu, 0x14 bytes, menubox.c's */
    char far *title;            /* +0                                     */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8   row the entries are placed from   */
    int col;                    /* +a                                     */
    char count;                 /* +c                                     */
    char flags;                 /* +d                                     */
    int glyph;                  /* +e                                     */
    int row;                    /* +10                                    */
    int width;                  /* +12                                    */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0                                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
};

struct Pos {                    /* a place and facing, 5 bytes            */
    char x;                     /* +0                                     */
    char y;                     /* +1                                     */
    unsigned char facing;       /* +2                                     */
    char facing_wall;           /* +3                                     */
    char event;                 /* +4                                     */
};

struct Geo {                    /* view3d.c's geo record                  */
    struct Pos pos;             /* +0   d414..d418                        */
    char attr_5;                /* +5                                     */
    char attr_6;                /* +6                                     */
    char show_map;              /* +7                                     */
    struct Area far *area;      /* +8   d41c                              */
    char attr_c;                /* +c                                     */
    char attr_d;                /* +d                                     */
};

struct EdState {                /* the editor's settings, mapedit.c's     */
    int screen;                 /* +0                                     */
    int prev;                   /* +2                                     */
    unsigned char view;         /* +4   0 the 3D view, 1 the map          */
    unsigned char tool;         /* +5   0 wall 1 backdrop 2 zone 3 event  */
    int f6;                     /* +6                                     */
    char f8;                    /* +8                                     */
    char move;                  /* +9   0 free movement, else walls block */
    unsigned char paint[7];     /* +a   side_a .. zone, +10               */
    char pad_11[0x1b];          /* +11                                    */
    struct Pos pos;             /* +2c  the party marker in the map view  */
};

struct Wid;                     /* a dialog item, as dialog.c's; only passed on */

struct MapEd {                  /* the map editor, mapedit.c's            */
    struct EdState far *st;     /* +0                                     */
    char erase;                 /* +4                                     */
    char pointer_mode;          /* +5   the mouse pointer's state         */
    int button;                 /* +6                                     */
    struct Menu far *menus[5];  /* +8   three put on the bar here         */
    char nmenus;                /* +1c  how many                          */
    char hover;                 /* +1d  the mouse is over the map         */
    unsigned char shown[8];     /* +1e  what the status panel shows       */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far menu_line_show(char far *title, char far *text, char fkeys); /* 083a:1e9a */
extern void far frame_main(void);                                /* 0d65:00de */
extern void far frame_right_panel(void);                                /* 0d65:0233 */
extern void far prompt_icon_set(int icon, int frame);            /* 0df0:000f */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern int  far pointer_mode_set(int mode);                          /* 0f5e:0a85 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far glyph_library_open(char mode, char far *name, int slot); /* 11d7:0031 */
extern void far draw_glyph(int row, int col, int n, int glyph);  /* 11d7:0bd1 */
extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern void far map_button_cb(struct MapEd far *m, int key, int v);
                                /* 18de:1d55, vector to overlay 16 29f4:144b */
extern int  far ed_item_base(struct EdState far *e);
                                /* 18de:1e3b, vector to overlay 16 29f4:3774 */
extern int  far map_key_hook(int a, int key);
                                /* 18de:1e45, vector to overlay 16 29f4:13f5 */
extern void far items_mark(struct MapEd far *m);
                                /* 18de:1f99, vector to overlay 21 2d91:186b */
extern void far area_start(struct EdState far *e, int a);
                                /* 18de:1fad, vector to overlay 21 2d91:298e */
extern void far view_attrs(char a, char b, char c, char d);     /* overlay 13 26e3:0572 */
extern void far bar_clear(void);                                /* overlay 15 2999:0007 */
extern void far bar_add(struct Menu far *m, char unused);       /* overlay 15 2999:0012 */
extern void far bar_draw(void);                                 /* overlay 15 2999:0373 */
extern int  far bar_proc(struct Wid far *w, int msg, int a, int b); /* overlay 15 2999:0500 */

extern struct Menu file_menu;                       /* 007c  "File"            */
extern char far *place_names[2];                    /* 0090  "Wall placement", ... */
extern struct Entry map_items[9];                   /* 0098  "3-D view", ...   */
extern struct Menu map_menu;                        /* 00e0  "Map"             */
extern struct Entry util_items[8];                  /* 00f4  "Display access", ... */
extern struct Menu util_menu;                       /* 0134  "Utilities"       */
extern char far *undo_names[];                        /* 024c  "Undo", "Un*do"   */
extern char far *ed_bar_items;                          /* 0404  "Select Left Place Right Undo  Mark" */
extern char far *empty_text;                                     /* 3bc0 */
extern char tick_slot;                                           /* b3f2 */
extern char ed_mode;                                           /* d136 */
extern struct Geo geo;                                           /* d414 */
extern struct EdState far *ed_state;                             /* d42c */
extern int ed_ypos;                                              /* d432 */
extern unsigned char ed_rows;                                    /* d434 */
extern unsigned char ed_cols;                                    /* d435 */
extern char edit_undo[6];                                        /* d436 */
extern int ed_pix_x;                                             /* d43c */
extern int edit_width;                                           /* d43e */
extern int edit_height;                                          /* d442 */

static char menu_glyph[] = "menu";                              /* 8f2a */

void far ed_defaults(struct EdState far *e);
void far shown_clear(struct MapEd far *m);
void far ed_menus(struct MapEd far *m);
void far ed_items(int view);
void far ed_window(char on, int view);
void far ed_mode_get(unsigned char far *tool, unsigned char far *view);

/* Section 21's 2d91:298e takes the settings and a; what a means is not
 * read here. */
void far ed_start(struct EdState far *e, unsigned char a)        /* 3cf5:000f */
{
    ed_defaults(e);
    area_start(e, a);
}

void far ed_enter(struct EdState far *e)                         /* 3cf5:0034 */
{
    ed_defaults(e);
    ed_window(1, e->view);
    e->pos = geo.pos;
}

void far ed_defaults(struct EdState far *e)                      /* 3cf5:006f */
{
    set_open_mode(0x32);
    view_attrs(0xf, 0, 0, 8);
    str_copy(edit_undo, undo_names[0]);
    ed_mode_get(&e->tool, &e->view);
    e->move = 1;
    for (tick_slot = 0; tick_slot < 7; tick_slot++)
        e->paint[tick_slot] = 0;
    ed_state = 0;
    if (geo.pos.facing == 0)
        geo.pos.facing = 8;
}

void far ed_open(struct MapEd far *m)                            /* 3cf5:00fe */
{
    m->pointer_mode = pointer_mode_set(-1);
    m->hover = !m->pointer_mode;
    m->erase = 0;
    m->button = 0;
    shown_clear(m);
}

void far shown_clear(struct MapEd far *m)                        /* 3cf5:0139 */
{
    unsigned char far *p;
    int i;

    p = m->shown;
    for (i = 0; i < 8; i++)
        *p++ = 0xff;
}

void far ed_frame(int view, char menu)                           /* 3cf5:0165 */
{
    unsigned char facing;
    int row;
    int col;

    switch (view) {
    case 1:
        frame_right_panel();
        if (menu) {
            glyph_library_open(0x33, menu_glyph, 0x15);
            draw_glyph(0x1f40, 0x1f40, 0x15, 2);
            slot_close(0x15);
        } else {
            text_xy_to(0, 0, -8, 0xa8, &row, &col);
            fill_rect(row + 0x1f48, 0x1f44, 0x1f48, col + 0x1f44, 8);
        }
        break;
    case 0:
        facing = geo.pos.facing;
        geo.pos.facing = geo.pos.facing % 8;
        frame_main();
        geo.pos.facing = facing;
        if (menu) {
            glyph_library_open(0x33, menu_glyph, 0x15);
            draw_glyph(0x1f40, 0x1f40, 0x15, 1);
            slot_close(0x15);
        }
        break;
    }
    screen_flush();
}

/* With no mouse pointer shown, the party's place is taken back from the
 * settings. */
void far ed_layout(struct MapEd far *m)                          /* 3cf5:025b */
{
    ed_items(m->st->view);
    ui_mark(ed_item_base(m->st) + 4, 0x27, (char far *)edit_undo);
    ed_menus(m);
    prompt_icon_set(0, 1);
    if (pointer_mode_set(-1) == 0)
        geo.pos = m->st->pos;
    bar_draw();
}

void far ed_menus(struct MapEd far *m)                           /* 3cf5:02de */
{
    int i;

    bar_clear();
    i = 0;
    bar_add(m->menus[i++] = &file_menu, 0);
    bar_add(m->menus[i++] = &map_menu, 0);
    bar_add(m->menus[i++] = &util_menu, 0);
    m->nmenus = i;
    file_menu.base++;
    map_menu.base++;
    util_menu.base++;
    items_mark(m);
    for (i = 0; i < 4; i++)
        map_items[i + 3].flags = m->st->tool == i ? 4 : 0;
    if (cur_area_get() <= 4) {
        map_items[0].flags = 0x81;
        map_items[1].flags = 4;
        map_items[3].text = place_names[1];
        map_items[4].flags = 0x81;
        util_items[0].flags = 0x81;
    } else {
        if (m->st->view == 0) {
            map_items[0].flags = 4;
            map_items[1].flags = 0;
        } else {
            map_items[0].flags = 0;
            map_items[1].flags = 4;
        }
        util_items[0].flags = 0;
        map_items[3].text = place_names[0];
    }
}

void far ed_items(int view)                                      /* 3cf5:043e */
{
    int h;
    int w;
    int n;

    if (pointer_mode_set(-1) == 0)
        n = 5;
    else
        n = 6;
    dl_begin();
    ui_add(8, bar_proc, 0x14, 0);
    ui_add(7, map_key_hook, 0x14, 0);
    if (view == 0)
        ui_add(5, 0x1f44, 0x1f54, 0x2c, 0x1c, 0x29, 0xb, 0x14,
               5, 0x1f70, 0x1f40, 9, 0x44, 0x29, 0x17, 0x14,
               5, 0x1f44, 0x1f70, 0x2c, 0x10, 0x29, 0x15, 0x14,
               5, 0x1f44, 0x1f40, 0x2c, 0x14, 0x29, 0x16, 0x14, 0);
    else {
        text_xy_to(0x1f40, 0x1f40, ed_rows * 8, ed_cols * 8, &h, &w);
        ui_add(5, 0x1f48, 0x1f44, h, w, 0x22, map_button_cb, 0x29, -1, 0x14, 0);
        ui_add(5, 0x1f44, w + 0x1f44, h + 8, 4, 0x29, 5, 0x14, 0);
        ui_add(5, h + 0x1f48, 0x1f44, 4, w, 0x29, 7, 0x14, 0);
        ui_add(5, 0x1f44, 0x1f40, h + 8, 4, 0x29, 9, 0x14, 0);
        ui_add(5, 0x1f44, 0x1f44, 4, w, 0x29, 0xb, 0x14, 0);
    }
    hotkey_reset(n - 1);
    menu_line_show(empty_text, ed_bar_items, 0);
}

/* With on, the map window's rows and columns shown and its pixel place
 * for the view; then its pixel size, at most the area's. */
void far ed_window(char on, int view)                            /* 3cf5:066f */
{
    if (on) {
        if (view == 0) {
            ed_rows = 9;
            ed_cols = 9;
            text_xy(0x1f48, 0x1f84, &ed_ypos, &ed_pix_x);
        } else {
            ed_rows = 0x14;
            ed_cols = 0x15;
            text_xy(0x1f48, 0x1f44, &ed_ypos, &ed_pix_x);
        }
    }
    edit_height = int_min(geo.area->rows, ed_rows) * 8;
    edit_width = int_min(geo.area->cols, ed_cols) * 8;
}

/* The tool from ed_mode's low nibble, the view from its high one. */
void far ed_mode_get(unsigned char far *tool, unsigned char far *view) /* 3cf5:070a */
{
    if (tool)
        *tool = ed_mode & 0xf;
    if (view)
        *view = (unsigned char)ed_mode >> 4 & 0xf;
}
