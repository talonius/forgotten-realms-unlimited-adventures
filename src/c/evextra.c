/* CKIT.EXE overlay section 26, module 2fc3: the event editor's helpers
 * that evedit.c (section 17) reaches through vector stubs: its pickers,
 * its checks and its page extras.
 *
 * npc_spells edits the spells an NPC's event record has memorized (the
 * record read as a party member and made cur_node meanwhile): "Memorize
 * Forget Leave" over "%d Spells Memorized", the nonzero bytes among the
 * 0x8d at +c6..+152.  Memorize runs spellcst.c's spell menu (stub
 * 18de:1305) with list 12, type 6 when byte +18d is below 0x70, and
 * otherwise, while camp.c's show_slots returns nonzero, with list 1, type
 * 2, a pick passing only when camp.c's slots_left allows it; each pick
 * goes into the free byte nearest +152.  Forget (list 0, type 1) clears
 * the byte nearest +152 that holds the pick.  kind_pick lists item_kind_names's
 * item kinds ("None" first only when none is set) under "Please Select a
 * Item Kind:" and returns the list position.  item_pick asks kind_pick
 * for a kind, has shop.c's treasure_mask_add (stub 18de:15cb) fill the item pool
 * from that row of item_classes, puts a "No Item" entry at its head and
 * returns item_classes[kind][pos - 1] for list position pos, 0 for "No
 * Item".  var_name names variable n: keys 0..7, items 8..19 (with full,
 * or when it has one, key_item_names's name), quests 20 on; var_pick lists the
 * 64.  The pickers return -1 on Cancel.  edit_check checks the event
 * record for code 1 (bytes 2 and 3 multiplied, at most 0x240), 2 (the six
 * monster groups at +8..+13, count & 0x1f and kind, 50 in all, a kind
 * large_monsters lists counting double) and 3 (no one group above 50 so
 * counted), or shows the message evedit.c asks for (0x100 text area full,
 * 0x101 button text too long by n, 0x102 button n wrong); it returns 0
 * when it showed a message, 1 when not.  page_extra draws what a page's
 * field kind 0x21 names: three of the record's six icons (+9, +b, +d for
 * 1; +f, +11, +13 for 2, as evedit.c's icons 8..13) on glyph 0x17 cells;
 * for 3 to 6 the items of the item_classes row in the high nibble of
 * byte +8, +b, +e or +11, an empty slot i marking the page's field i
 * 0x11; for 7 byte +89; for 8, when byte +58 is 5, "Old Class:" with the
 * first class whose level (+a4..+aa) is nonzero and below first_level's.
 *
 * Section 26 (parent section 20, which is empty and has no parent; no
 * section names 26 as its parent): payload at EXE 0x080f10, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 26.  The module is stored frame 2fc3 (load-based 3fc3),
 * EXE 0x083232..0x084040 (3,598 bytes, 7 routines), from 2fc3:0002, the
 * byte after module 2e34's last retf (2e34:18f1), to the retf at
 * 2fc3:0e0f, the section's last byte.  One Turbo C large-model _TEXT
 * segment, built with Turbo C 2.01, tcc -ml -O -Z -d; functions are
 * defined in shipped order.  Definition comments give the load-based
 * address; extern comments the far pointer as stored in the shipped call,
 * or the DGROUP offset.
 *
 * Section 26's relocation table files 87 sites under 2fc3, all in this
 * range (EXE 0x083258..0x084027) and all far calls.  No relocated word in
 * section 26 stores 2fc3, so nothing in the section far-calls the module.
 * Seven of the section's twelve vectors land here, one on each routine:
 * 18de:1ebd -> npc_spells, 1edb -> kind_pick, 1ed1 -> item_pick, 1a99 ->
 * var_name, 1ec7 -> var_pick, 1ee5 -> edit_check, 1eb3 -> page_extra.
 * Section 17 (evedit.c) far-calls each stub (1ee5 at four sites, 1a99 at
 * one); section 8 (move.c) also calls 1a99.  item_pick near-calls
 * kind_pick and var_pick var_name, after their definitions.  Every
 * routine is public, as a vector target.
 *
 * FRUA's own code: the sharing map pairs all 7 routines a-only, so every
 * name here is chosen from the code.
 *
 * _DATA is DGROUP 9454..9688: 38 literals, 32 distinct, stored as 29 (-d
 * stores "Select Cancel" and "Too many monsters!" once, and "Memorized",
 * "%s" and "%2d" as tails of "%d Spells Memorized", "Key %d - %s" and
 * "Item %2d").  Below it edpick.c's _DATA ends at 9452 and 9453 is a zero
 * pad; above it 9689 is a zero pad and 968a starts sysutil.c's.  No
 * instruction outside this range names 9454..9689.  One object: every
 * routine is in frame 2fc3, and edit_check and page_extra use tails of
 * var_name's literals.  No _BSS: every other DGROUP operand is another
 * module's or a goldbox.toml sym.
 *
 * Forced by the bytes, each re-baked against its alternative over the 7
 * ranges: npc_spells' "{ (void)0; continue; }" (a bare continue compiles
 * a je to the loop test and a jmp to the store, the shipped jne/jmp
 * swapped: 6 of 7; artpick.c's lever) and test_mode = 5 ahead of the
 * loop (6); item_pick's "(kind = kind_pick(0, 0)) < 0" and its nested
 * ternary return, jointly (either alone lets -O merge the two "return
 * -1"s: 2 of 7); mask[1] = mask[2] = 0xff (6); kind_pick's "none ? 0 : 1"
 * (!none: 1) and p = p->next in the for increment (6); edit_check's
 * line1[0] = line2[0] = 0 (6) and "if (title) { ... } return 1;" (5);
 * var_name's item branch with the copy first (3); page_extra's k = 6
 * where a break would leave the loop (6), "> 0" (!= 0: 6) and register i
 * and j (6).  Not decided by the bytes: item_classes[kind][top - 1] or
 * *(... + top - 1), a ternary or if/else in kind_pick's return, j += 2 in
 * the body or ahead of i++ in the for increment (after it: 6), var_name's key branch either way round,
 * register or plain ints in npc_spells and edit_check, int or short k and
 * row in page_extra.
 *
 * Shipped as written: edit_check's code 3 tests total, which nothing sets
 * when no monster group has a kind; the "Item Kind" title says "a".
 */

struct Hero;                    /* hero.c's party member                  */
struct Res;                     /* tiles.c's resource                     */
struct Pool;                    /* strutil.c's slot pool                  */

struct Node {                   /* a list_menu line, as menu.c's          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct ItemRec {                /* an item's saved fields, 18 bytes, shop.c's */
    char pad_0[0xb];            /* +0   not read here                     */
    unsigned char unid_mask;    /* +b   bits 0-2: not yet identified      */
    char pad_c[6];              /* +c   not read here                     */
};

struct Item {                   /* an item, 0x3e bytes                    */
    struct Item far *next;      /* +0                                     */
    unsigned char in_use;       /* +4   not read here                     */
    char name[0x23];            /* +5                                     */
    struct ItemRec rec;         /* +28                                    */
    char pad_3a[4];             /* +3a  not read here                     */
};

struct SpellInfo {              /* 0x16 bytes a spell, camp.c's           */
    int kind;                   /* +0                                     */
    unsigned char level;        /* +2                                     */
    char pad_3[0x13];           /* +3   not read here                     */
};

struct Field {                  /* one field of the page, 0x11 bytes, evedit.c's */
    char pad_0[0xd];            /* +0   not read here                     */
    char id;                    /* +d   its dialog item                   */
    char pad_e[3];              /* +e   not read here                     */
};

struct Page {                   /* evedit.c's page, as far as read here   */
    int count;                  /* +0                                     */
    struct Field fields[30];    /* +2                                     */
};

struct EvRec {                  /* evedit.c's event record, as far as read
                                   here: for an NPC a party member's
                                   record                                 */
    unsigned char data[0x1c2];  /* +0                                     */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern unsigned far mem_scan(void far *p, int n, char c);    /* 00d7:27b1 */
extern void far item_name(void far *h, struct Item far *it, char col,
                          char row, char full, char show);       /* 03d0:03d1 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern unsigned char far first_level(struct Hero far *h);        /* 03d0:212e */
extern void far glyph_at(int col, int row, unsigned char glyph, char unused,
                         struct Res far *r);                     /* 05f5:0598 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far press_return_clear(void);                        /* 083a:053a */
extern void far list_alloc(int n, struct Node far * far *head);  /* 083a:0575 */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern void far icon_draw(int col, int row, unsigned char kind,
                          unsigned char bank, unsigned char idx); /* 0cb5:0299 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_lower_panel_palette(void);                                /* 0d65:021a */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern unsigned char far spell_menu(unsigned char list, unsigned char type, int far *choice,
                                      char far *more);
                                /* 18de:1305, vector to overlay 12 2b69:203e */
extern void far pool_clear(void);
                                /* 18de:1477, vector to overlay 3 1bfb:0002 */
extern char far treasure_mask_add(unsigned char far *mask);
                                /* 18de:15cb, vector to overlay 10 2431:1141 */
extern void far scroll_make(struct ItemRec far *rec);
                                /* 18de:1733, vector to overlay 10 2431:0fe1 */
extern unsigned char far show_slots(void);
                                /* 18de:1aad, vector to overlay 9 21d4:0634 */
extern void far memorized_sort(void);
                                /* 18de:1ab7, vector to overlay 9 21d4:08e8 */
extern unsigned char far slots_left(unsigned char level, short kind,
                                      unsigned char far *used);
                                /* 18de:1ac1, vector to overlay 9 21d4:02fe */

extern char far *item_kind_names[];                        /* 02f0  "None", "+0 Weapons A", ... */
extern char far *a_text;                          /* 0364  "a"            */
extern char far *an_text;                          /* 0368  "an"           */
extern char far *please_text;                          /* 03a4  "Please"       */
extern char far *memorize_items;                          /* 03ac  "Memorize Forget Leave" */
extern char far *select_word;                          /* 03bc  "Select"       */
extern struct SpellInfo spell_info[];                            /* 264a */
extern char far *class_names[];                                  /* 3914 */
extern char far *empty_text;                                     /* 3bc0 */
extern unsigned char item_classes[16][20];                       /* 7ad2 */
extern unsigned char large_monsters[28];                              /* 8de6 */
extern struct Hero far *cur_node;                                /* b3dd */
extern struct Item far *pool_items;                              /* b8e2 */
extern char key_item_names[][16];                                      /* ba3c */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char test_mode;                                           /* cc7a */
extern struct Res far *pic_res;                                  /* cfd8 */
extern struct ItemRec far *item_templates;                             /* d122 */
extern struct Page far *ed_page;                                 /* d446 */
extern struct EvRec far *ed_ev;                                  /* d44a */

/* Memorize and forget the spells of the NPC record being edited. */
void far npc_spells(void)                                        /* 3fc3:0002 */
{
    struct Hero far *save;
    int choice;
    int spell;
    char more;
    unsigned char far *q;
    unsigned char count;
    unsigned char used;
    int n;
    int pick;

    save = cur_node;
    cur_node = (struct Hero far *)ed_ev;
    test_mode = 5;
    for (;;) {
        frame_border();
        n = 0;
        for (q = ed_ev->data + 0x152; ed_ev->data + 0xc6 <= q; q--)
            if (*q)
                n++;
        text_print_at(0x1f58, 0x1f58, 0x87, "%d Spells Memorized", n);
        count = 0;
        if (n < 0x8c)
            hotkey_add(0, &count);
        if (n)
            hotkey_add(1, &count);
        hotkey_add(2, &count);
        pick = (unsigned char)menu_run(empty_text, memorize_items, 0, 1);
        choice = -1;
        switch (pick) {
        case 0:
            if (ed_ev->data[0x18d] >= 0x70)
                frame_lower_panel_palette();
            while (n < 0x8c) {
                if (ed_ev->data[0x18d] >= 0x70) {
                    if (!show_slots())
                        break;
                    spell = spell_menu(1, 2, &choice, &more);
                    if (spell && !slots_left(spell_info[spell].level, spell_info[spell].kind, &used)) {
                        (void)0;
                        continue;
                    }
                } else
                    spell = spell_menu(0xc, 6, &choice, &more);
                if (spell == 0)
                    break;
                for (q = ed_ev->data + 0x152; ed_ev->data + 0xc6 <= q; q--)
                    if (*q == 0) {
                        *q = spell;
                        bottom_msg("Memorized");
                        n++;
                        break;
                    }
            }
            break;
        case 1:
            while (n > 0) {
                spell = spell_menu(0, 1, &choice, &more);
                if (spell == 0)
                    break;
                for (q = ed_ev->data + 0x152; ed_ev->data + 0xc6 <= q; q--)
                    if (*q == spell) {
                        *q = 0;
                        n--;
                        break;
                    }
            }
            break;
        default:
            cur_node = save;
            test_mode = 0;
            return;
        }
        memorized_sort();
    }
}

/* An item kind from item_kind_names ("None" too when none), starting at cur. */
int far kind_pick(int cur, char none)                            /* 3fc3:025f */
{
    struct Node far *p;
    struct Node far *list;
    int i;
    char buf[40];
    char redraw;
    int k;

    list = 0;
    list_alloc(none ? 0x11 : 0x10, &list);
    p = list;
    for (i = none ? 0 : 1; i < 0x11; i++, p = p->next) {
        p->key = 0;
        str_copy(p->text, item_kind_names[i]);
    }
    frame_border();
    str_format(buf, "%s %s %s Item Kind:", please_text, select_word, a_text);
    text_print_at(0x1f48, (0x28 - str_len(buf)) * 2 + 0x1f40, 0x8f, buf);
    p = list;
    for (i = 0; i < cur; i++)
        p = p->next;
    redraw = 1;
    hotkey_reset(1);
    k = (unsigned char)list_menu(empty_text, "Select Cancel", 2, 4, 0x26, 0x16, list, 1, 0,
                                 &redraw, &i, &p);
    list_free(&list);
    return k ? -1 : i;
}

/* An item id of a kind kind_pick asks for, or 0 for "No Item". */
int far item_pick(int cur)                                       /* 3fc3:03d3 */
{
    struct Item far *it;
    struct Node far *sel;
    struct Node far *head;
    int top;
    char buf[40];
    unsigned char mask[3];
    char redraw;
    int kind;
    int k;

    if ((kind = kind_pick(0, 0)) < 0)
        return -1;
    mask[0] = kind << 4 | 0xf;
    mask[1] = mask[2] = 0xff;
    pool_clear();
    treasure_mask_add(mask);
    it = pool_items;
    pool_alloc((void far * far *)&pool_items, 0x3e, &item_record_pool);
    mem_fill(pool_items, 0x3e, 0);
    pool_items->next = it;
    str_copy(pool_items->name, "No Item");
    head = (struct Node far *)pool_items;
    frame_border();
    str_format(buf, "%s %s %s Item:", please_text, select_word, an_text);
    text_print_at(0x1f48, (0x28 - str_len(buf)) * 2 + 0x1f40, 0x8f, buf);
    for (it = pool_items->next; it; it = it->next) {
        it->rec.unid_mask &= 0xf8;
        item_name(0, it, 0, 0, 0, 0);
    }
    sel = head;
    top = cur;
    redraw = 1;
    hotkey_reset(1);
    k = (unsigned char)list_menu(empty_text, "Select Cancel", 2, 4, 0x26, 0x16, head, 1, 0,
                                 &redraw, &top, &sel);
    pool_clear();
    return k ? -1 : top ? item_classes[kind][top - 1] : 0;
}

/* Variable n's name: a key (0..7), an item (8..19), a quest (20 on);
 * full adds the key's or item's name from key_item_names. */
void far var_name(char far *out, int n, char full)               /* 3fc3:05b8 */
{
    if (n < 8) {
        if (full)
            str_format(out, "Key %d - %s", n + 1, key_item_names[n]);
        else if (key_item_names[n][0] == 0)
            str_format(out, "Key %d", n + 1);
        else
            str_copy(out, key_item_names[n]);
    } else if (n < 0x14) {
        if (full)
            str_format(out, "Item %2d - %s", n - 7, key_item_names[n]);
        else if (key_item_names[n][0])
            str_copy(out, key_item_names[n]);
        else
            str_format(out, "Item %2d", n - 7);
    } else
        str_format(out, "Quest %2d", n - 0x13);
}

/* One of the 64 variables, starting at cur. */
int far var_pick(int cur)                                        /* 3fc3:06b0 */
{
    struct Node far *p;
    struct Node far *list;
    int i;
    char buf[40];
    char redraw;
    int k;

    list_alloc(0x40, &list);
    p = list;
    for (i = 0; i < 0x40; i++, p = p->next)
        var_name(p->text, i, 1);
    frame_border();
    str_format(buf, "%s %s %s Variable:", please_text, select_word, a_text);
    text_print_at(0x1f48, (0x28 - str_len(buf)) * 2 + 0x1f40, 0x8f, buf);
    p = list;
    i = cur;
    redraw = 1;
    hotkey_reset(1);
    k = (unsigned char)list_menu(empty_text, "Select Cancel", 2, 4, 0x26, 0x16, list, 1, 0,
                                 &redraw, &i, &p);
    list_free(&list);
    return k ? -1 : i;
}

/* Check code on the event record, or show the message evedit.c names:
 * 1 when all is well, 0 after a message. */
char far edit_check(int code, int n)                             /* 3fc3:07da */
{
    char far *title;
    char line1[40];
    char line2[40];
    int i;
    int total;

    title = 0;
    line1[0] = line2[0] = 0;
    switch (code) {
    case 1:
        if (ed_ev->data[2] * ed_ev->data[3] > 0x240) {
            title = "Map Area too large!";
            str_format(line1, "Try width of %d or", 0x240 / ed_ev->data[2]);
            str_format(line2, "height of %d.", 0x240 / ed_ev->data[3]);
        }
        break;
    case 2:
        total = 0;
        for (i = 8; i <= 0x12; i += 2)
            if (ed_ev->data[i + 1]) {
                total += ed_ev->data[i] & 0x1f;
                if (mem_scan(large_monsters, 0x1c, ed_ev->data[i + 1]) < 0x1c)
                    total += ed_ev->data[i] & 0x1f;
            }
        if (total > 0x32) {
            title = "Too many monsters!";
            str_copy(line1, "There is a limit of 50 monsters.");
            str_copy(line2, "Some monsters count double.");
        }
        break;
    case 3:
        for (i = 8; i <= 0x12; i += 2)
            if (ed_ev->data[i + 1]) {
                total = ed_ev->data[i] & 0x1f;
                if (mem_scan(large_monsters, 0x1c, ed_ev->data[i + 1]) < 0x1c)
                    total += ed_ev->data[i] & 0x1f;
                if (total > 0x32)
                    break;
            }
        if (total > 0x32) {
            title = "Too many monsters!";
            str_copy(line1, "There is a limit of 25 monsters");
            str_copy(line2, "when they count double.");
        }
        break;
    case 0x100:
        title = "Text area is full!";
        str_copy(line1, "There is too much text");
        str_copy(line2, "in the Module.");
        break;
    case 0x101:
        title = "Button text too long!";
        str_copy(line1, "Try making a label shorter.");
        str_format(line2, "Too long by %d characters.", n);
        break;
    case 0x102:
        title = "Buttons must start with letters!";
        str_copy(line1, "And each must be unique.");
        str_format(line2, "Button %d is wrong.", n);
        break;
    }
    if (title) {
        frame_border();
        text_print_at(0x1f50, 0x1f90 - str_len(title) * 2, 0x8b, "%s", title);
        if (line1[0])
            text_print_at(0x1f58, 0x1f90 - str_len(line1) * 2, 0x87, "%s", line1);
        if (line2[0])
            text_print_at(0x1f5e, 0x1f90 - str_len(line2) * 2, 0x87, "%s", line2);
        press_return_clear();
        return 0;
    }
    return 1;
}

/* What a page's field kind 0x21 asks for: the combat icons (1, 2), a
 * treasure row's items (3..6), a level (7), a human's old class (8). */
void far page_extra(int what)                                    /* 3fc3:0af9 */
{
    struct Item item;
    int k;
    int row;
    register int i;
    register int j;

    switch (what) {
    case 1:
    case 2:
        if (pic_res == 0)
            break;
        j = what == 1 ? 9 : 0xf;
        for (i = 0; i < 3; i++) {
            glyph_at(0x1fb4, i * 0x18 + 0x1f50, 0x17, 0, pic_res);
            glyph_at(0x1fc0, i * 0x18 + 0x1f50, 0x17, 0, pic_res);
            glyph_at(0x1fcc, i * 0x18 + 0x1f50, 0x17, 0, pic_res);
            glyph_at(0x1fb4, i * 0x18 + 0x1f5c, 0x17, 0, pic_res);
            glyph_at(0x1fc0, i * 0x18 + 0x1f5c, 0x17, 0, pic_res);
            glyph_at(0x1fcc, i * 0x18 + 0x1f5c, 0x17, 0, pic_res);
            if (ed_ev->data[j])
                icon_draw(10, i * 2 + 1, 0, 0, (j - 9) / 2 + 8);
            j += 2;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        j = (what - 3) * 3 + 8;
        row = ed_ev->data[j] >> 4 & 0xf;
        for (i = 0; i < 0x14; i++)
            if (item_classes[row][i]) {
                if (item_classes[row][i] == 0xff)
                    scroll_make(&item.rec);
                else
                    mem_move5(&item_templates[item_classes[row][i]], &item.rec, 0x12);
                item.rec.unid_mask &= 0xf8;
                item_name(0, &item, 0, 0, 0, 0);
                text_print_at(i * 4 + 0x1f4c, 0x1f50, 0x87, "%s", item.name);
            } else
                ui_mark(ed_page->fields[i].id, 0x11);
        break;
    case 7:
        text_print_at(0x1f71, 0x1fc0, 0x87, "%2d", ed_ev->data[0x89]);
        break;
    case 8:
        if (ed_ev->data[0x58] != 5)
            break;
        for (k = 0; k <= 6; k++)
            if (ed_ev->data[k + 0xa4] > 0
                && first_level((struct Hero far *)ed_ev) > ed_ev->data[k + 0xa4]) {
                text_print_at(0x1f6a, 0x1f48, 0x180, "Old Class:   %s", class_names[k]);
                k = 6;
            }
        break;
    }
}
