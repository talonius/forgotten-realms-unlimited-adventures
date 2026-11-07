/* CKIT.EXE overlay section 26, module 2e34: three of the design editor's
 * screens and the editor's heading.  Each screen reads its request from
 * env->bits, clears part of it and leaves its answer there, and returns
 * the screen it was called with.
 *
 * ask_screen (screen 10) asks question k = bits & 0x3f of ask_questions's
 * eleven: 0 ("Save changes to current module?") with "Save Don't save
 * Cancel", 1 to 5 and 7 with "Ok Cancel", 6 ("Delete %d chained events?",
 * the count ((bits & 0xff00) >> 8) + 1) with "Ok Cancel", 8 to 10 with
 * "Ok" alone, or "Ok Cancel" when bits & 0x300; 11 and up ask nothing.
 * ask_box shows the question and its answers as menu_line_show's command line,
 * polls the dialog until an answer 0..which is picked (an escaped Enter takes
 * the first, any other escaped key the last; with "Ok" alone ask_key also
 * ends it on Enter or Esc), and stores n - pick in the low byte: Save 2,
 * Don't save 1, Ok 1 (alone 0), Cancel 0.
 *
 * pick_screen (screen 11) lists one kind of thing, kind = bits & 0xf
 * (pick_kind_names: adventure modules, event types, art types, entry points, the
 * game design, map zones, special effects, reserved), from position num =
 * bits 4..9, in list_menu, and stores 1 for a pick or 0 in the low nibble
 * and the position & 0xff in bits 4..11.  Modules (verb module_verbs[bits
 * 10..12]: "Open", "Save (write to)", "Open (copy from)", "Place",
 * "Select"), event types and the kinds 6 and 7 are labels.c's label lists
 * (stubs 18de:17a1 and 17ab), whose pick is mapped to a label number
 * through 18de:1873 for event types and kinds 6 and 7; art types are
 * art_names with "Edit Leave"; entry points are the area's eight spots
 * ("Place which Entry Point at x, y?"); map zones are the area's zone
 * names ("My zone #%d" when empty), with zone_draw drawing the eight
 * zone swatches through list_options_set and env->bits 8..9 picking "Select" or
 * "Replace which Map Zone?".  The game design (kind 4) is design.c's
 * design_pick, whose result goes to the low nibble.  ed_header heads the
 * screen with screen_titles's screen name (index scr), the design's name
 * (game_rec, used as a format, or "A Game Design") and, unless brief, the
 * area's name or label.
 *
 * wall_screen (screen 12) picks a wall slot (0..15: one above a grid of
 * five by three, slot_pos) and an obstruction (16 list positions down the
 * right, wall_pos, mapped to wall_kind_names's sixteen kinds by order[1..16]),
 * "Please Select a wall and obstruction:" (bits & 0xf == 1) or "Replace
 * which wall and obstruction?" (2), from slot bits 4..7 and kind bits
 * 8..11, and stores 1 for a pick or 0 in the low nibble and the slot and
 * kind back in bits 4..11.  Clicks pick a slot (dialog ids 0..15) or a
 * list position (0x11..0x20).  The keys 0x81..0x88 move in the grid; with
 * the pointer state (vidmouse.c's 0a85) 0, 0x82 and 0x86 switch between
 * the grid and the list and the others move in the one in use.  List
 * position 8 shows position 7's name with "secret" (wall_kind_names[3] + 7)
 * beside it.
 *
 * Section 26 (parent section 20, which is empty and has no parent; no
 * section names 26 as its parent): payload at EXE 0x080f10, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 26.  The module is stored frame 2e34 (load-based 3e34),
 * EXE 0x08194e..0x083232 (6,372 bytes, 12 routines), from 2e34:000e, the
 * byte after design.c's last retf (2e34:000d), to the retf at 2e34:18f1;
 * module 2fc3 starts at 2fc3:0002 (EXE 0x083232, the target of vector
 * 18de:1ebd).  One Turbo C large-model _TEXT segment, built with Turbo C
 * 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the far
 * pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 26's relocation table files 125 sites under 2e34, all in this
 * range (EXE 0x0819a1..0x083203) and all far calls: 14 through vector
 * stubs, one to design.c's design_pick (2d91:0000, the only relocated word
 * in section 26 that stores 2d91) and one to this module's ed_header ahead
 * of its definition.  Four of the section's twelve vectors land here:
 * 18de:1c1f -> ask_screen, 18de:1c15 -> pick_screen and 18de:1c0b ->
 * wall_screen (editloop.c's screens 10, 11 and 12, each called once from
 * section 10) and 18de:2007 -> ed_header (evchain.c's ed_header, called
 * once from section 22).  design.c calls ask_screen directly (2e34:000e,
 * EXE 0x081878).  The other eight routines are near-called, or their
 * address is pushed with push cs or stored with cs, ahead of their
 * definitions, so they are static.
 *
 * FRUA's own code: the sharing map pairs all 12 routines a-only, so every
 * name here is chosen from the code.
 *
 * _DATA is DGROUP 93fa..9452: 26 literals, 12 distinct, stored as 8 (-d
 * stores each repeat once, and " %s", "%s %s", "%s" and "" as tails of
 * "%s %s %s").  Below it 93f9 ends design.c's _DATA; 9453 is a zero pad
 * and 9454 module 2fc3's.  No instruction outside this range names
 * 93fa..9452 (EXE 0x039133's "mov ax,0x9400" is the low word of a long
 * constant).  One object: pick_screen, ed_header and wall_screen use tails
 * of ask_screen's "%s %s %s", and zone_draw is tied in by pick_screen's cs
 * store.  No _BSS: every other DGROUP operand is another
 * module's or a goldbox.toml sym.
 *
 * Forced by the bytes, each re-baked against its alternative over the 12
 * ranges: n = label_list(...) + lo (n += lo as its own statement lets
 * -O merge the three tails: 1 of 12); in pick_screen's and wall_screen's
 * last stores, |= or a parenthesized = bits | (...) against the
 * unparenthesized = bits | a | b (1 and 4 of 12); the if/else stores of 0 and
 * n, 0 and 1, 0xc and 0xe (ternaries: 1 of 12 each); the assignments
 * inside the tests (k = menu_line_key, lab = cur_area_get: 1 and 3 of 12);
 * n = keys[0] = 0 (0 of 12) and by_id = invert = 0 (11 of 12) chained;
 * row += 4 in zone_draw's for increment (11); lo > 0 ? 1 : 0 or (lo > 0)
 * against lo != 0 (11) and
 * lo = num ? 1 : 0 (1); case 0x88's and 0x84's "else if (in_list)" ahead
 * of the grid step, which keeps -O's merged copies of the grid steps in
 * cases 0x86 and 0x82 ("else if (!in_list)": 11 of 12; the list test
 * first or a ternary: 4 of 12); wall_screen's
 * result built by explicit masks (a bit-field union: 4 of 12); order[]
 * 17 bytes indexed from 1 (from 0: 11 of 12); unsigned char for
 * wall_screen's cur (pick_screen's cur bakes either way), lo,
 * lab and the slot_pos and wall_pos numbers; ask_key returning int.  Not
 * decided by the bytes: |= or = bits | in ask_box and for design_pick's
 * result, max = num ? num : 0xff or if/else, int or short for num and
 * unavail, register or plain for pick_screen's n and max, and cases 0x88
 * and 0x84 as written or as a nested if/else or with early breaks.
 *
 * Shipped as written: pick_screen's default case lists "Item %d" only for
 * kind < 8, which never holds there (kind 4 returns first, 8..15 fail the
 * test); invert is never set; unavail is 0 or -1, and top is position +
 * lo after list_menu.
 */

struct Node {                   /* a list_menu line, as menu.c's          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct Spot {                   /* a place the area keeps, 4 bytes        */
    unsigned char y;            /* +0                                     */
    unsigned char x;            /* +1                                     */
    unsigned char facing;       /* +2                                     */
    unsigned char which;        /* +3   not read here                     */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0   0: no area loaded                 */
    unsigned char rows;         /* +2   not read here                     */
    unsigned char cols;         /* +3   not read here                     */
    char pad_4[0xa];            /* +4   not read here                     */
    struct Spot spots[8];       /* +e   the entry points                  */
    char pad_2e[0x48];          /* +2e  not read here                     */
    char name[16];              /* +76  the area's name                   */
    char zone_names[8][16];     /* +86  by zone 0..7                      */
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

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   a screen's result; signed here    */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
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
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far frame_list_panel(void);                                /* 0d65:0302 */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern int  far pointer_mode_set(int mode);                          /* 0f5e:0a85 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far mouse_move_to(int row, int col);                 /* 0f5e:10a3 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_poll(void);                                   /* 15e5:15e6 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
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
extern void far piece_box(int row, int col, int n, int k);
                                /* 18de:17dd, vector to overlay 5 1e8f:1576 */
extern void far area_label(int v, char far *out);
                                /* 18de:17e7, vector to overlay 5 1e8f:0eb5 */
extern void far msg_box(char far *fmt, ...);
                                /* 18de:1841, vector to overlay 5 1e8f:1359 */
extern int  far label_nth(int idx, unsigned char kind, unsigned char mask,
                             unsigned char first, void far * far *out,
                             char (far *filter)(unsigned char));
                                /* 18de:1873, vector to overlay 5 1e8f:0d05 */
extern char far area_overland(void);
                                /* 18de:1e63, vector to overlay 16 29f4:378b */
extern char far design_pick(void);                               /* 2d91:0000 */

extern char far *tool_names[];                               /* 0154  "Wall", ...     */
#define zone_text (tool_names[2])                             /* "Zone"                */
extern char far *wall_kind_names[];                        /* 016c  "Open", ...    */
extern char far *art_names[];                       /* 01b4  "No art", ...  */
extern char far *screen_titles[];                        /* 01dc  "", "AD&D Unlimited Adventures", ... */
extern char far *entry_point_text;                          /* 0208  "Entry Point"  */
extern char far *module_verbs[];                        /* 0280  "Open", "Save (write to)", ... */
extern char far *ask_questions[];                        /* 0294  "Save changes to current module?", ... */
extern char far *pick_kind_names[];                        /* 02c0  "Adventure Module", ... */
extern char far *compass_names[];                        /* 02e0  "North", ...   */
extern char far *wall_text;                          /* 034c  "Wall"         */
extern char far *place_text;                          /* 0358  "Place"        */
extern char far *replace_text;                          /* 035c  "Replace"      */
extern char far *edit_text;                          /* 0360  "Edit"         */
extern char far *a_text;                          /* 0364  "a"            */
extern char far *an_text;                          /* 0368  "an"           */
extern char far *at_text;                          /* 0370  "at"           */
extern char far *wall_obstruction_text;                          /* 0390  "wall and obstruction" */
extern char far *obstruction_text;                          /* 0394  "Obstruction"  */
extern char far *which_text;                          /* 0398  "which"        */
extern char far *please_text;                          /* 03a4  "Please"       */
extern char far *save_text;                          /* 03a8  "Save"         */
extern char far *dont_save_text;                          /* 03b0  "Don't save"   */
extern char far *select_word;                          /* 03bc  "Select"       */
extern char far *leave_text;                          /* 03c0  "Leave"        */
extern char far *ok_text;                          /* 03c4  "Ok"           */
extern char far *cancel_text;                          /* 03c8  "Cancel"       */
extern char far *unavailable_text;                          /* 03f0  "That item is not available." */
extern char far *entry_word;                          /* 0400  "Entry"        */
extern char far *design_default_text;                          /* 0414  "A Game Design" */
extern char far *zone_default_fmt;                          /* 0420  "My zone #%d"  */
extern char far *nowhere_text;                          /* 0424  "Nowhere"      */
extern char far *empty_text;                                     /* 3bc0 */
extern int hook_key;                                            /* b384 */
extern char tick_slot;                                           /* b3f2 */
extern char game_rec[];                                          /* ba08 */
extern char menu_escaped;                                        /* ca2a */
extern struct Geo geo;                                           /* d414 */

static void far ask_box(struct EdEnv far *env, char far *quest, char far *keys, int n);
static void far zone_draw(int cur);
void far ed_header(int row, int col, int w, int scr, int gap, char brief);
static void far wall_pos(unsigned char n, int far *row, int far *col);
static void far wall_swatch(unsigned char n, unsigned char far *order);
static void far slot_pos(unsigned char n, int far *row, int far *col);
static void far slot_draw(unsigned char n);
static void far slot_frame(unsigned char n, int attr);
static int  far ask_key(int id, int key);

int far ask_screen(int prev, struct EdEnv far *env, void far *arg) /* 3e34:000e */
{
    char quest[0x28];
    char keys[0x2e];
    register int k;
    register int n;

    n = keys[0] = 0;
    k = env->bits & 0x3f;
    switch (k) {
    case 0:
        str_copy(quest, ask_questions[k]);
        str_format(keys, "%s %s %s", save_text, dont_save_text, cancel_text);
        n = 2;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 7:
        str_copy(quest, ask_questions[k]);
        str_format(keys, "%s %s", ok_text, cancel_text);
        n = 1;
        break;
    case 6:
        str_format(quest, ask_questions[k], (int)((env->bits & 0xff00) >> 8) + 1);
        str_format(keys, "%s %s", ok_text, cancel_text);
        n = 1;
        break;
    default:
        if (k < 0xb) {
            str_copy(quest, ask_questions[k]);
            if (env->bits & 0x300) {
                str_format(keys, "%s %s", ok_text, cancel_text);
                n = 1;
            } else
                str_format(keys, "%s", ok_text);
        } else
            quest[0] = 0;
    }
    if (quest[0])
        ask_box(env, quest, keys, n);
    return prev;
}

static void far ask_box(struct EdEnv far *env, char far *quest, char far *keys, int n)
                                                                 /* 3e34:018d */
{
    register int k;
    register int key;

    if (quest) {
        screen_draw_begin(1);
        screen_flush_mode_set(1);
        frame_border();
        text_print_at(0x1f62, (0x26 - str_len(quest)) * 4 / 2 + 0x1f44, 0x8f, quest);
        dl_begin();
        hotkey_reset(n);
        menu_line_show(empty_text, keys, 0);
        if (n == 0)
            ui_add(7, ask_key, 0x14, 0);
        dl_draw(1);
        screen_flush_mode_set(0);
        screen_flush();
        do {
            while ((key = ui_poll()) < 0)
                input_poll();
            if ((k = menu_line_key(key)) >= 0) {
                if (menu_escaped) {
                    if (k == 0xd)
                        k = 0;
                    else
                        k = n;
                    menu_escaped = 0;
                }
            } else if (n == 0 && (hook_key == 0xd || hook_key == 0x1b))
                k = 0;
        } while (k < 0 || k > n);
        dl_end();
        env->bits &= 0xffffff00L;
        env->bits |= n - k;
    }
}

int far pick_screen(int prev, struct EdEnv far *env, void far *arg) /* 3e34:02e8 */
{
    int kind;
    int num;
    int unavail;
    char far *verb;
    char title[0x32];
    char cmd[0x32];
    unsigned char lo;
    unsigned char hi;
    unsigned char cur;
    char redraw;
    char by_id;
    char invert;
    char brief;
    int top;
    struct Node far *node;
    struct Node far *list;
    void (far *extra)(int);
    register int n;
    register int max;

    extra = 0;
    kind = env->bits & 0xf;
    num = (env->bits & 0x3f0) >> 4;
    by_id = invert = 0;
    brief = 0;
    cmd[0] = 0;
    if (kind == 4) {
        kind = design_pick();
        env->bits &= 0xfffff000L;
        env->bits |= kind & 0xfL;
        return prev;
    }
    if (kind == 3) {
        redraw = area_overland();
        str_format(title, "%s %s %s %s %d, %d%s?", place_text, which_text,
                   redraw ? entry_point_text : entry_word, at_text, geo.pos.x, geo.pos.y,
                   redraw ? "" : msg_format(" %s", compass_names[(geo.pos.facing & 6) >> 1]));
    } else
        str_format(title, "%s %s %s %s:", please_text, select_word, kind < 4 ? an_text : a_text,
                   pick_kind_names[kind]);
    unavail = 0;
    top = 0;
    lo = 1;
    list = 0;
    switch (kind) {
    case 0:
        hi = 0xff;
        max = 0x28;
        kind = (env->bits & 0x1c00) >> 10;
        if (kind == 2 || kind == 1) {
            if (num != 0 && num < 4)
                hi = 4;
            else
                lo = 5;
            if (lo == num)
                top++;
            if (kind == 2)
                brief = 1;
        } else if (kind == 4) {
            lo = num ? 1 : 0;
            unavail = -1;
            top = num - (lo > 0 ? 1 : 0);
        } else if (kind == 0)
            top = num - 1;
        verb = module_verbs[kind];
        by_id = 1;
        max = label_count(0xb, 1, &lo, &hi, 0, &kind, 0);
        list_alloc(max, &list);
        node = list;
        if (lo == 0) {
            str_format(node->text, nowhere_text);
            node->key = 0;
            node = node->next;
        }
        for (n = int_max(lo, 1); node; n++) {
            area_label(n, node->text);
            node->key = 0;
            node = node->next;
        }
        n = label_list(list, 0xb, 1, lo, 1, 0, 0) + lo;
        break;
    case 1:
        by_id = 1;
        max = label_count(0xa, 0x60, &lo, 0, 0, &top, 0);
        verb = select_word;
        list_alloc(max, &list);
        n = label_list(list, 0xa, 0x60, lo, 0, 0, 0) + lo;
        break;
    case 6:
    case 7:
        by_id = 1;
        num = (env->bits & 0xff0) >> 4;
        lo = 0;
        unavail = -1;
        top = num - (lo > 0 ? 1 : 0);
        cur = num;
        if (kind == 6)
            num = 0xc;
        else
            num = 0xe;
        max = label_count(num, 1, &lo, 0, &cur, &top, 0);
        verb = select_word;
        list_alloc(max, &list);
        n = label_list(list, num, 1, lo, 0, 0, 0) + lo;
        break;
    case 2:
        max = 9;
        if (num != 0 && num <= max)
            top = num - lo;
        str_format(cmd, "%s %s", edit_text, leave_text);
        list_alloc(max, &list);
        node = list;
        for (n = lo; node; n++) {
            str_format(node->text, "%s", art_names[n]);
            node->key = 0;
            node = node->next;
        }
        break;
    case 5:
        extra = zone_draw;
        max = 8;
        lo = 0;
        n = (num & 0x30) >> 4;
        num &= 0xf;
        if (num != 0 && num <= max)
            top = num - lo;
        switch (n) {
        case 1:
            verb = select_word;
            break;
        case 2:
            verb = replace_text;
            str_format(title, "%s %s %s?", replace_text, which_text, pick_kind_names[kind]);
            break;
        }
        list_alloc(max, &list);
        node = list;
        for (n = lo; node; n++) {
            mem_move(geo.area->zone_names[n], node->text, 0x10);
            node->text[0x10] = 0;
            if (node->text[0] == 0)
                str_format(node->text, zone_default_fmt, n + 1);
            node->key = 0;
            node = node->next;
        }
        break;
    case 3:
        max = 8;
        lo = 0;
        if (num != 0 && num < max)
            top = num;
        verb = module_verbs[3];
        list_alloc(max, &list);
        node = list;
        for (n = lo; node; n++) {
            str_format(node->text, "%s %d: %d,%d %s", entry_word, n + 1, geo.area->spots[n].x,
                       geo.area->spots[n].y,
                       redraw ? "" : compass_names[(geo.area->spots[n].facing & 6) >> 1]);
            node->key = 0;
            node = node->next;
        }
        break;
    default:
        if (kind < 8) {
            max = num ? num : 0xff;
            verb = select_word;
            list_alloc(max, &list);
            node = list;
            for (n = lo; node; n++) {
                str_format(node->text, "Item %d", n);
                node->key = 0;
                node = node->next;
            }
        } else
            title[0] = 0;
    }
    kind = 1;
    if (list && n - lo == max) {
        redraw = 1;
        for (node = list, n = 0; n < top && node; n++)
            node = node->next;
        if (!node)
            node = list;
        hotkey_reset(1);
        if (cmd[0] == 0)
            str_format(cmd, "%s %s", verb, cancel_text);
        frame_lower_panel();
        screen_flush();
        text_print_at(0x1f48, (0x26 - str_len(title)) * 4 / 2 + 0x1f44, 0x8f, title);
        ed_header(0x1f88, 0x1f44, 0x26, prev == 0x15 ? 7 : prev, 1, brief);
        do {
            if (extra)
                list_options_set(extra, 0, 1);
            kind = (unsigned char)list_menu(empty_text, cmd, 2, 4, 0x26, 0xf, list, 1, 0,
                                            &redraw, &top, &node);
            top += lo;
            if (menu_escaped) {
                if (kind == 0xd)
                    kind = 0;
                else
                    kind = 1;
            }
            if (invert)
                kind = 1 - kind;
            if (kind == 0 && unavail != 0 && top == unavail) {
                msg_box(unavailable_text, 0);
                top -= lo;
                redraw = 1;
                kind = -1;
            }
        } while (kind < 0 || kind > 1);
        if (by_id) {
            switch ((int)env->bits & 0xf) {
            case 1:
                top = label_nth(top - lo, 0xa, 0x60, lo, 0, 0);
                break;
            case 6:
            case 7:
                if ((env->bits & 0xf) == 6)
                    num = 0xc;
                else
                    num = 0xe;
                top = label_nth(top - lo, num, 1, lo, 0, 0);
                break;
            }
        }
        env->bits &= 0xfffff000L;
        env->bits |= 1 - kind & 0xfL | (top & 0xff) << 4;
    }
    list_free(&list);
    return prev;
}

static void far zone_draw(int cur)                               /* 3e34:0d75 */
{
    register int row;
    register int col;

    row = 0x1f50;
    col = 0x1f8c;
    for (tick_slot = 0; tick_slot < 8; tick_slot++, row += 4) {
        fill_rect(row, col, row + 4, col + 4, tick_slot);
        text_print_at(row, col + 6, tick_slot == cur ? 0x8b : 0x87, "%s %d", zone_text,
                      tick_slot + 1);
    }
}

void far ed_header(int row, int col, int w, int scr, int gap, char brief) /* 3e34:0de8 */
{
    char buf[0x27];
    unsigned char lab;

    gap = int_max(0, gap);
    if (scr < 0xa) {
        str_format(buf, "%s", screen_titles[scr]);
        text_print_at(row, (w - str_len(buf)) / 2 * 4 + col, 0x8c, buf);
    }
    if (game_rec[0])
        str_format(buf, game_rec);
    else
        str_format(buf, design_default_text);
    row += gap * 4;
    text_print_at(row + 4, (w - str_len(buf)) / 2 * 4 + col, 0x87, "%s", buf);
    if (!brief && (lab = cur_area_get()) != 0 && geo.area->version != 0) {
        mem_move(geo.area->name, buf, 0x10);
        buf[0x10] = 0;
        if (buf[0] == 0)
            area_label(lab, buf);
        text_print_at(row + 8, (w - str_len(buf)) / 2 * 4 + col, 0x87, "%s", buf);
    }
}

int far wall_screen(int prev, struct EdEnv far *env, void far *arg) /* 3e34:0f71 */
{
    int key;
    int sel;
    unsigned char i;
    unsigned char cur;
    char in_list;
    char pointer;
    char title[0x2a];
    char keys[0x14];
    int row;
    int col;
    unsigned char hi;
    unsigned char order[0x11];
    char far *name;
    register int k;

    pointer = pointer_mode_set(-1);
    switch ((int)env->bits & 0xf) {
    case 1:
        str_format(title, "%s %s %s %s:", please_text, select_word, a_text, wall_obstruction_text);
        str_format(keys, "%s %s", select_word, cancel_text);
        break;
    case 2:
        str_format(title, "%s %s %s?", replace_text, which_text, wall_obstruction_text);
        str_format(keys, "%s %s", replace_text, cancel_text);
        break;
    }
    i = (env->bits & 0xff0) >> 4;
    cur = i & 0xf;
    hi = i >> 4 & 0xf;
    for (i = 0; i < 2; i++)
        order[i + 1] = i;
    for (i = 2; i < 4; i++)
        order[i + 1] = i + 0xc;
    for (i = 4; i < 0x10; i++)
        order[i + 1] = i - 2;
    for (i = 1, sel = 0; i <= 0x10 && sel == 0; i++)
        if (order[i] == hi)
            sel = i;
    dl_begin();
    ui_add(5, 0x1f4e, 0x1f66, 0x10, 0xc, 0x14, 0);
    for (i = 1; i < 0x10; i++) {
        slot_pos(i, &row, &col);
        ui_add(5, row, col, 0x10, 0xc, 0x14, 0);
    }
    ui_add(2, 0x10, 0x23, &sel, 0);
    for (i = 1; i <= 0x10; i++) {
        name = wall_kind_names[order[i == 8 ? i - 1 : i]];
        wall_pos(i, &row, &col);
        ui_add(3, row, col, name, 0x26, 0x87, 0x2a, int_max(0, 0xf - str_len(name)),
               0x14, 0);
    }
    ui_add(6, 0x1f44, (0x26 - str_len(title)) * 4 / 2 + 0x1f46, title, 0x26, 0x8f, 0);
    hotkey_reset(2);
    menu_line_show(empty_text, keys, 1);
    ui_add(7, ask_key, 0x14, 0);
    frame_list_panel();
    for (i = 0; i < 0x10; i++)
        slot_draw(i);
    slot_frame(cur, 0xf);
    for (i = 1; i <= 0x10; i++)
        wall_swatch(i, order);
    text_print_at(0x1f4e, 0x1fa0, 0x8c, "%ss:", obstruction_text);
    text_print_at(0x1f4e, 0x1f4a, 0x8c, "%ss:", wall_text);
    wall_pos(8, &row, &col);
    text_print_at(row + 4, col + 0xa, 0x87, wall_kind_names[3] + 7);
    screen_flush();
    if (!pointer) {
        in_list = 0;
        pointer_show();
    }
    do {
        while ((key = ui_poll()) < 0)
            input_poll();
        if ((k = menu_line_key(key)) >= 0) {
            if (menu_escaped) {
                if (k == 0xd)
                    k = 0;
                else if (k == 0x1b || k == 0x60)
                    k = 1;
                else {
                    key = -1;
                    switch (k) {
                    case 0x88:
                        if (pointer)
                            key = cur ? (cur > 5 ? cur - 5 : 0) : 0xd;
                        else if (in_list)
                            key = (sel + 0xe) % 0x10 + 1;
                        else
                            key = (cur + 0xf) % 0x10;
                        break;
                    case 0x81:
                        if (pointer || !in_list)
                            key = cur ? (cur > 5 ? cur - 5 : 0) : 0xd;
                        else
                            key = sel > 1 ? (sel > 9 ? 9 : 1) : 9;
                        break;
                    case 0x84:
                        if (pointer)
                            key = cur ? (cur < 0xb ? cur + 5 : 0) : 3;
                        else if (in_list)
                            key = sel % 0x10 + 1;
                        else
                            key = (cur + 1) % 0x10;
                        break;
                    case 0x83:
                        if (pointer || !in_list)
                            key = cur ? (cur < 0xb ? cur + 5 : 0) : 3;
                        else
                            key = sel < 0x10 ? (sel < 8 ? 8 : 0x10) : 8;
                        break;
                    case 0x86:
                        if (pointer)
                            key = (cur + 0xf) % 0x10;
                        else
                            key = (in_list = 1 - in_list) ? sel : cur;
                        break;
                    case 0x82:
                        if (pointer)
                            key = (cur + 1) % 0x10;
                        else
                            key = (in_list = 1 - in_list) ? sel : cur;
                        break;
                    case 0x87:
                        key = pointer || !in_list ? 0 : 1;
                        break;
                    case 0x85:
                        key = pointer || !in_list ? 0xf : 0x10;
                        break;
                    default:
                        beep();
                    }
                    if (key >= 0) {
                        if (pointer || !in_list) {
                            slot_frame(cur, 8);
                            cur = key;
                            slot_frame(cur, 0xf);
                        } else {
                            ui_mark(0x10, 4, key);
                            hi = order[sel];
                            wall_pos(sel, &row, &col);
                            text_xy(row, col + 0x40, &row, &col);
                            mouse_move_to(row + 4, col + 1);
                        }
                    }
                    k = -1;
                }
                menu_escaped = 0;
            }
        } else if (key < 0x10) {
            if (cur != key) {
                slot_frame(cur, 8);
                cur = key;
                slot_frame(cur, 0xf);
            }
        } else if (key > 0x10 && key <= 0x20)
            hi = order[sel];
        else if (hook_key == 0xd)
            k = 0;
        else if (hook_key == 0x1b)
            k = 1;
    } while (k < 0);
    if (!pointer)
        pointer_hide();
    dl_end();
    key = 0;
    key = key & 0xfff0 | cur & 0xf;
    key = key & 0xff0f | (hi & 0xf) << 4;
    env->bits &= 0xfffff000L;
    env->bits |= key << 4 | 1 - k;
    return prev;
}

static void far wall_pos(unsigned char n, int far *row, int far *col) /* 3e34:16df */
{
    if (n <= 8)
        n--;
    *row = n * 4 + 0x1f54;
    *col = 0x1f9a;
}

static void far wall_swatch(unsigned char n, unsigned char far *order) /* 3e34:1707 */
{
    int row;
    int col;

    wall_pos(n, &row, &col);
    fill_rect(row, col + 0x3c, row + 4, col + 0x40, order[n]);
}

static void far slot_pos(unsigned char n, int far *row, int far *col) /* 3e34:1755 */
{
    if (n--) {
        *row = (n / 5 + 1) * 0x14 + 0x1f4e;
        *col = n % 5 * 0xe + 0x1f4a;
    } else {
        *row = 0x1f4e;
        *col = 0x1f66;
    }
}

static void far slot_draw(unsigned char n)                       /* 3e34:17ab */
{
    int row;
    int col;
    register int r;

    slot_pos(n, &row, &col);
    r = row + 0x10;
    fill_rect(r - 4, col + 4, r, col + 8, n);
    piece_box(row, col, n, 3);
}

static void far slot_frame(unsigned char n, int attr)            /* 3e34:180c */
{
    int row;
    int col;
    register int r;
    register int c;

    slot_pos(n, &row, &col);
    r = row + 0x10;
    c = col + 0xc;
    fill_rect(row - 2, col - 2, row, c + 2, attr);
    fill_rect(row, c, r, c + 2, attr);
    fill_rect(r, col - 2, r + 2, c + 2, attr);
    fill_rect(row, col - 2, r, col, attr);
    if (pointer_mode_set(-1) == 0 && attr == 0xf)
        mouse_move_to(r - 2, c - 2);
}

static int far ask_key(int id, int key)                          /* 3e34:18cd */
{
    if (key == 0xd || key == 0x1b) {
        hook_key = key;
        return 1;
    }
    hook_key = 0;
    return 0;
}
