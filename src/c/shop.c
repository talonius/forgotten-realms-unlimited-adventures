/* CKIT.EXE overlay 10 module 2431: the shop and the vault.  Pricing the
 * shop's stock and buying from it, the shop menu, the vault menu, selling
 * or identifying one of a member's items for platinum, and three routines
 * that make treasure: a scroll, one item from the item table, and the items
 * a treasure mask names.
 *
 * EXE 0x04cb25..0x04dcde, one Turbo C large-model _TEXT segment starting at
 * 2431:0005 in RTLink section 10 (payload 0x4a550, link segment 21d4,
 * parent section 3), after postcmbt.c's final retf at 2431:0004.  Its own
 * final retf is 2431:11bd, where module 254c begins at 254c:000e (section
 * 10's table files the last 22ef fixup site at 0x04cb22, the first and
 * last 2431 sites at 0x04cb45 and 0x04dc49, and the first 254c site at
 * 0x04dcfb).  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are
 * defined in shipped order; bake consumes them in that order.  postcmbt.c
 * far-calls item_give; code outside the section reaches seven routines
 * through vector stubs: 18de:15c1 -> 05b4, 18de:1a8f -> 08e0,
 * 18de:1b43 -> 0b1a, 18de:1b4d -> 0de1, 18de:1733 -> 0fe1,
 * 18de:1a71 -> 1088, 18de:15cb -> 1141.
 *
 * A bare stored seg:off in this file means section 10.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and an ovlNN_ name which section and stored
 * segment.  A mod18de_ callee is an RTLink vector stub; its comment names
 * the routine the stub reaches.
 *
 * The first eight routines are DQK's shop.c relinked, with FRUA's records
 * and the changes the bytes show: a price is rate_price's long, which
 * takes the value, the shop's rate and the count, where DQK's shop_pick,
 * shop_buy and item_sell scale the value by the rate themselves; shop_pick
 * sets list_options_set(0, 0, 1) before its list; item_sell halves a price above
 * 1 and pays nothing for an item without value or a cursed one; the shop
 * menu offers Buy only while the shop has stock, prompts with anim 1, warns
 * a party leaving money behind with three text lines and a pause, and
 * redraws when it restores view mode 4 with picture_hides_roster set; the vault menu
 * does not redraw first and ends with pool_clear; money is platinum.
 * scroll_make is DQK eclcmds.c's scroll_make with FRUA's 18-byte record and
 * without its level roll and spell_fits test (byte_3 is always 0x28, ident
 * 4), and treasure_item_add the item-making half of its cmd_treasure.
 *
 * _DATA is DGROUP 7ad2..7e64: item_classes, public because section 26 reads
 * it (add bx,0x7ad2 at 0x0837d1, 0x083ec5, 0x083ede and 0x083f0b), then the
 * code's 30 literals, 28 distinct, stored as 27 -- -d stores each "buys"
 * format once and "" as the NUL of "%l".  _BSS is shop_redraw, one byte at
 * d20e.  pooled, which money.c and section 8 (0x042ff6) also set and
 * nothing reads, is extern here.
 */

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Node {
    struct Node far *next;      /* +0                                    */
    char key;                   /* +4                                    */
    char text[35];              /* +5                                    */
};

struct ItemRec {                /* an item's saved fields, 18 bytes       */
    unsigned char type;         /* +0                                     */
    unsigned char name1;        /* +1                                     */
    char name2;                 /* +2                                     */
    unsigned char name3;        /* +3                                     */
    int weight;                 /* +4                                     */
    int value;                  /* +6                                     */
    char plus;                  /* +8                                     */
    char pad_9;                 /* +9   not read here                     */
    char readied;               /* +a                                     */
    unsigned char unid_mask;    /* +b   bits 0-2: not yet identified      */
    char cursed;                /* +c                                     */
    unsigned char slots[4];     /* +d   [1..3] a scroll's spells          */
    char pad_11;                /* +11  not read here                     */
};

struct Item {                   /* an item, 0x3e bytes                    */
    struct Item far *next;      /* +0                                     */
    unsigned char in_use;       /* +4                                     */
    char name[0x23];            /* +5  a Node's text in the shop list     */
    unsigned char type;         /* +28 0x49: a bundle                     */
    char pad_29[5];             /* +29 not read here                      */
    unsigned value;             /* +2e                                    */
    char pad_30[3];             /* +30 not read here                      */
    unsigned char unid_mask;    /* +33 bits 0-2: not yet identified       */
    char cursed;                /* +34                                    */
    unsigned char count;        /* +35                                    */
    char pad_36[8];             /* +36 not read here                      */
};

struct NewItem {                /* an item as treasure_item_add builds it         */
    struct Item far *next;      /* +0                                     */
    char in_use;                /* +4                                     */
    char pad_5[0x23];           /* +5   not read here                     */
    struct ItemRec rec;         /* +28                                    */
    struct Item far *more;      /* +3a                                    */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x40];           /* +c   not read here                     */
    unsigned coins[3];          /* +4c  platinum, gems, jewelry           */
    char pad_52[0xe];           /* +52  not read here                     */
    char name[0x11];            /* +60                                    */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0  2 magic-user                       */
    char pad_2[0x14];           /* +2  not read here                      */
};

struct Pool;

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far item_name(struct Hero far *h, struct Item far *it, char col,
                          char row, char full, char show);       /* 03d0:03d1 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far item_remove(struct Hero far *h, struct Item far *it); /* 03d0:13c4 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far str_pad_left(char far *s, unsigned char width);  /* 06c9:09ee */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far press_return_clear(void);                        /* 083a:053a */
extern void far set_menu_kind(int kind);                         /* 083a:13f2 */
extern char far node_prompt(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1a07 */
extern void far list_options_set(void (far *extra)(int sel), char anim, char done); /* 083a:20b5 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far *far rec_alloc(void);                            /* 0d48:013c */
extern void far frame_border(void);                              /* 0d65:002a */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far party_display(void far *node);      /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);  /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far char_view(char far *done);   /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far items_menu(char far *done);   /* 18de:116b, vector to overlay 14 28aa:0f56 */
extern void far vault_save_current(void);             /* 18de:1701, vector to overlay 4 1ffa:1d1b */
extern void far vault_load_current(void);             /* 18de:170b, vector to overlay 4 1ffa:1cd7 */
extern int  far party_scrolls(void);             /* 18de:171f, vector to overlay 14 28aa:1e59 */
extern void far join_item(struct Item far *item);
                                                /* 18de:1b57, vector to overlay 14 28aa:1ebe */
extern char far cant_carry(struct Hero far *hero, char n, struct Item far *item);
                                                /* 18de:1b61, vector to overlay 14 28aa:34dc */
extern void far drop(char n);           /* 18de:1b6b, vector to overlay 14 28aa:2ee1 */
extern void far pool_clear(void);                                /* overlay 3 1bfb:0002 */
extern char far too_heavy(struct Hero far *hero, long amount, long far *room);
                                                                 /* 21d4:0000 */
extern void far pool_money(void);                                /* 21d4:00f6 */
extern void far share_money(void);                               /* 21d4:01ee */
extern void far take_menu(void);                                 /* 21d4:0701 */
extern void far pool_check(char far *has_coins, char far *has_items); /* 21d4:09ab */
extern void far appraise(char far *done);                        /* 21d4:09fb */
extern void far loot_take(char far *has_coins, char far *has_items); /* 22ef:0d13 */
extern long far rate_price(long value, unsigned char rate, unsigned count); /* 254c:0063 */

extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern char far *not_enough_text;                                /* 3b50 */
extern char far *leave_text1;                                    /* 3b54 */
extern char far *leave_text2;                                    /* 3b58 */
extern char far *leave_text3;                                    /* 3b5c */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *items_title;                                    /* 3c14 */
extern char far *buy_items;                                      /* 3c94 */
extern char far *shop_items;                                     /* 3c98 */
extern char spaces[];                   /* 3c9c  34 spaces            */
extern char far *vault_items;                                    /* 3cd7 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern int view_mode;                                            /* b421 */
extern char pooled;                                              /* b424 */
extern int saved_view_mode;                                            /* b433 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char picture_hides_roster;                                           /* b761 */
extern long pool[3];                                             /* b8d6 */
extern struct Item far *pool_items;                              /* b8e2 */
extern char shop_temple_frame;                                           /* c672 */
extern char treasure_has_items;                                           /* ca15 */
extern char menu_escaped;                                        /* ca2a */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char has_coins;                                           /* cc6d */
extern char vault_slot;                                           /* cc6e */
extern struct ItemRec far *item_templates;                             /* d122 */
extern char ui_mode;                                             /* d128 */

unsigned char item_classes[16][20] = {           /* 7ad2 item ids by class */
    { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
      0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14 },
    { 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
      0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
      0x2a, 0x2b, 0x2c, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
      0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45 },
    { 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
      0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63 },
    { 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
      0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81 },
    { 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
      0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f },
    { 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
      0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd },
    { 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
      0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d },
    { 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
      0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9 },
    { 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
      0x2d, 0x2e, 0x2f, 0x30, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd },
    { 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
      0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xe9, 0xde, 0x00, 0x00 },
    { 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
      0xea, 0xeb, 0xec, 0xed, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static char shop_redraw;                                         /* d20e */

void far shop_pick(char far *key, struct Item far * far *pick,
                   struct Item far * far *cursor, int far *top)  /* 3431:0005 */
{
    struct Item far *head;
    struct Item far *p;
    char num[17];
    char line[35];
    unsigned char len;
    unsigned char i;
    long price;

    p = pool_items;
    while (p) {
        str_copy(line, spaces);
        price = rate_price(p->value, cursor_obj[0x28], p->count);
        str_format(num, "%l", price);
        len = str_len(num);
        for (i = 0; i < len; i++)
            line[0x22 - len + i] = num[i];
        len = str_len(p->name);
        for (i = 0; i < len; i++)
            line[i] = p->name[i];
        str_copy(p->name, line);
        p = p->next;
    }
    head = pool_items;
    ui_mode = 0;
    hotkey_reset(1);
    list_options_set(0, 0, 1);
    *key = list_menu(items_title, buy_items, 1, 1, 0x26, 0x12,
                     (struct Node far *)head, 1, 0, &shop_redraw, top,
                     (struct Node far * far *)cursor);
    *pick = *cursor;
    p = pool_items;
    while (p) {
        item_name(0, p, 0, 0, 0, 0);
        p = p->next;
    }
}

void far item_give(struct Item far *item, char far *failed)     /* 3431:01eb */
{
    struct Item far *p;
    struct Item far *q;

    if (item->type == 0x49 && party_scrolls() + item->count > 0x78) {
        bottom_msg("Too many Bundles!");
        *failed = 1;
    } else if (cant_carry(cur_node, 1, item)) {
        bottom_msg("OverLoaded");
        *failed = 1;
    } else {
        *failed = 0;
        p = cur_node->items;
        if (!p) {
            p = rec_alloc();
            *p = *item;
            p->next = 0;
            cur_node->items = p;
        } else {
            while (p->next)
                p = p->next;
            q = rec_alloc();
            *q = *item;
            q->next = 0;
            p->next = q;
        }
        if (item->count > 0)
            join_item(q);            /* q unset when items was empty: 2431:0307 */
        recalc_stats(cur_node);
    }
}

void far funds_show(void)                                        /* 3431:0327 */
{
    char s[12];

    str_copy(s, msg_format("%u", cur_node->coins[0]));
    str_pad_left(s, 10);
    status_printf(1, 0x14, 0xb, 0, msg_format("Personal funds:%s", s));
    str_copy(s, msg_format("%ld", pool[0]));
    str_pad_left(s, 10);
    status_printf(1, 0x15, 0xb, 0, msg_format("Pooled funds:  %s", s));
}

void far shop_buy(void)                                          /* 3431:03e9 */
{
    struct Item far *pick;
    char key;
    char done;
    unsigned short price;
    long steel;
    long pooled_steel;
    char failed;
    struct Item far *cursor;
    int top;

    pick = 0;
    cursor = pool_items;
    top = 0;
    frame_border();
    shop_redraw = 1;
    do {
        funds_show();
        shop_pick(&key, &pick, &cursor, &top);
        if (key == 1 || key == 0x1b && menu_escaped) {
            done = 1;
        } else {
            done = 0;
            price = rate_price(pick->value, cursor_obj[0x28], pick->count);
            steel = cur_node->coins[0];
            pooled_steel = pool[0];
            if (price <= steel) {
                item_give(pick, &failed);
                if (!failed) {
                    cur_node->coins[0] -= price;
                    if (pick->count > 0)
                        bottom_msg(msg_format("%s buys %s ", cur_node->name,
                                             pick->name));
                    else
                        bottom_msg(msg_format("%s buys a %s ", cur_node->name,
                                             pick->name));
                }
            } else if (price <= pooled_steel + steel) {
                item_give(pick, &failed);
                if (!failed) {
                    cur_node->coins[0] = 0;
                    pool[0] -= price - steel;
                    if (pick->count > 0)
                        bottom_msg(msg_format("%s buys %s ", cur_node->name,
                                             pick->name));
                    else
                        bottom_msg(msg_format("%s buys a %s ", cur_node->name,
                                             pick->name));
                }
            } else
                bottom_msg(not_enough_text);
        }
    } while (!done);
}

void far shop_menu(void)                                         /* 3431:05b4 */
{
    char viewed;
    char done;
    char has_coins;
    char has_items;
    unsigned char key;
    unsigned char count;
    unsigned char i;
    char state;
    char appraised;
    struct Item far *p;

    saved_view_mode = view_mode;
    view_mode = 1;
    shop_temple_frame = 0;
    view_redraw();
    shop_temple_frame = 1;
    party_display(cur_node);
    mem_zero(pool, sizeof pool);
    pooled = 0;
    done = 0;
    p = pool_items;
    while (p) {
        item_name(0, p, 0, 0, 0, 0);
        p = p->next;
    }
    state = 1;
    do {
        pool_check(&has_coins, &has_items);
        if (has_coins) {
            count = 0;
            for (i = 0; i <= 6; i++)
                if (i != 0 || pool_items)
                    hotkey_add(i, &count);
        } else {
            count = 0;
            for (i = 0; i <= 6; i++)
                if (i != 2 && i != 4 && (i != 0 || pool_items))
                    hotkey_add(i, &count);
        }
        ui_mode = state;
        key = node_prompt(empty_text, shop_items, 1, 1);
        state = ui_mode;
        if (menu_escaped) {
            switch (key) {
            case 0x84:
            case 0x85:
            case 0x87:
            case 0x88:
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
                key = 0xff;
                break;
            case 0x1b:
                key = 6;
            }
        }
        switch (key) {
        case 0:
            shop_buy();
            break;
        case 1:
            char_view(&viewed);
            break;
        case 2:
            take_menu();
            break;
        case 3:
            pool_money();
            break;
        case 4:
            share_money();
            break;
        case 5:
            appraise(&appraised);
            break;
        case 6:
            pool_check(&has_coins, &has_items);
            if (has_coins) {
                combat_text_clear();
                text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, leave_text1);
                text_wrap(1, 0x12, 0x26, 0x16, 3, 0, 0, leave_text2);
                press_return_clear();
                text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, leave_text3);
                if (confirm(empty_text, 1))
                    text_clear(1, 0x11, 0x26, 0x16);
                else
                    done = 1;
            } else
                done = 1;
            break;
        }
        switch (key) {
        case 0:
        case 2:
        case 5:
            view_redraw();
            break;
        case 3:                         /* appraise's flag, as shipped: 2431:08b1 */
            if (appraised)
                view_redraw();
            break;
        }
    } while (!done);
    view_mode = saved_view_mode;
    if (view_mode == 4 && picture_hides_roster) {
        picture_hides_roster = 0;
        view_redraw();
    }
}

void far vault_menu(void)                                        /* 3431:08e0 */
{
    unsigned char k;
    char leave;
    char state;
    unsigned char count;
    unsigned char key;
    char done;

    view_mode = 10;
    shop_temple_frame = 1;
    party_display(cur_node);
    pool_clear();
    vault_load_current();
    pooled = 0;
    leave = 0;
    state = 1;
    do {
        mem_zero(hotkeys, sizeof hotkeys);
        count = 0;
        hotkey_add(0, &count);
        pool_check(&has_coins, &treasure_has_items);
        if (has_coins || treasure_has_items)
            hotkey_add(1, &count);
        hotkey_add(2, &count);
        if (cur_node->coins[0] > 0 || cur_node->coins[1] > 0 || cur_node->coins[2] > 0)
            hotkey_add(3, &count);
        if (cur_node->items)
            hotkey_add(4, &count);
        hotkey_add(5, &count);
        ui_mode = state;
        set_menu_kind(2);
        key = node_prompt("", vault_items, 1, 0);
        state = ui_mode;
        if (menu_escaped && key == 0x1b) {
            menu_escaped = 0;
            key = 5;
        }
        done = 0;
        if (!menu_escaped) {
            switch (key) {
            case 0:
                char_view(&done);
                break;
            case 1:
                loot_take(&has_coins, &treasure_has_items);
                break;
            case 2:
                pool_money();
                break;
            case 3:
                drop(0);
                text_clear(1, 0x11, 0x26, 0x16);
                break;
            case 4:
                items_menu(&done);
                view_redraw();
                break;
            case 5:
                leave = 1;
                break;
            }
        } else {
            k = key;
            if (k == 0x84 || k == 0x88 || k == 0x87 || k == 0x85) {
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
            }
        }
    } while (!leave);
    vault_slot = 0x54;
    vault_save_current();
    pool_clear();
}

void far item_sell(struct Hero far *hero, struct Item far *item) /* 3431:0b1a */
{
    long price;
    unsigned short amount;
    long room;
    char ask[42];

    item_name(hero, item, 0, 0, 0, 0);
    price = rate_price(item->value, cursor_obj[0x28], item->count);
    if (price > 1)
        price = price >> 1;
    if (item->value == 0 || item->cursed)
        price = 0;
    if (price > 0) {
        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                  msg_format("I'll give you %ld platinum pieces for your %s", price,
                             item->name));
        str_copy(ask, "Will you sell ");
        if (item->count < 2 || item->type == 0x49)
            str_copy(ask, msg_format("%sit? ", ask));
        else
            str_copy(ask, msg_format("%sthem? ", ask));
    } else if (item->count < 2) {
        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                  msg_format("Your %sis worthless to me.", item->name));
        str_copy(ask, "Shall I dispose of it for you? ");
    } else {
        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                  msg_format("Your %sare worthless.", item->name));
        str_copy(ask, "Would you like to discard them? ");
    }
    if (confirm(ask, 1)) {
        if (price > 0)
            bottom_msg("Sold!");
        else if (item->count < 2)
            bottom_msg("I will give it to one of my sons.");
        else
            bottom_msg("Very well.\tI'll take them.");
        item_remove(cur_node, item);
        amount = price;
        if (too_heavy(cur_node, amount, &room)) {
            bottom_msg("Overloaded.\tMoney will be put in pool.");
            cur_node->coins[0] += (unsigned short)room;
            price -= room;
            pool[0] += price;
        } else
            cur_node->coins[0] += price;
    }
    text_clear(1, 0x15, 0x26, 0x16);
}

void far item_identify(struct Hero far *hero, struct Item far *item,
                       char far *redraw)                         /* 3431:0de1 */
{
    long steel;
    long pooled_steel;
    char paid;

    if (item->type == 0x49) {
        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                  "Fool! You must remove them from the bundle first!");
    } else {
        paid = 0;
        item_name(hero, item, 0, 0, 0, 0);
        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                  msg_format("For 20 platinum pieces I'll identify your %s",
                             item->name));
        if (confirm("Is It a Deal? ", 1)) {
            pooled_steel = pool[0];
            if ((steel = cur_node->coins[0]) >= 20) {
                paid = 1;
                cur_node->coins[0] -= 20;
            } else if (pooled_steel + steel >= 20) {
                paid = 1;
                cur_node->coins[0] = 0;
                pool[0] -= 20 - steel;
            } else {
                bottom_msg("Not Enough Money");
                text_pause();
            }
        }
        if (paid) {
            if (!(item->unid_mask & 7)) {
                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                          msg_format("I can't tell anything new about your %s",
                                     item->name));
            } else {
                item->unid_mask = item->unid_mask & 0xf8;
                item_name(hero, item, 0, 0, 0, 0);
                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                          msg_format("It looks like some sort of %s", item->name));
                *redraw = 1;
            }
        }
        if (paid)
            text_pause();
        text_clear(1, 0x15, 0x26, 0x16);
    }
}

void far scroll_make(struct ItemRec far *rec)                      /* 3431:0fe1 */
{
    unsigned char spell;
    char ok;

    mem_zero(rec, 0x12);
    rec->type = 0x27;
    rec->name2 = 0x27;
    rec->name1 = 0x66;
    rec->plus = 1;
    rec->weight = 1;
    rec->value = 3000;
    rec->cursed = 0;
    rec->name3 = 0x28;
    rec->unid_mask = 4;
    for (tick_slot = 1; tick_slot <= 3; tick_slot++) {
        ok = 0;
        do {
            spell = fx_roll_dice(1, 0x7e);
            if (spell_info[spell].kind == 2)
                ok = 1;
        } while (!ok);
        rec->slots[tick_slot] = spell;
    }
}

void far treasure_item_add(int id, char known)                           /* 3431:1088 */
{
    struct NewItem item;
    struct Item far *prev;

    if (id == 0)
        return;
    if (id == 0xff)
        scroll_make(&item.rec);
    else
        mem_move5(&item_templates[id], &item.rec, 0x12);
    if (item.rec.type == 0xff)
        return;
    if (known)
        item.rec.unid_mask &= 0xf8;
    item.more = 0;
    item.next = 0;
    item.in_use = 0;
    prev = pool_items;
    pool_alloc((void far * far *)&pool_items, 0x3e, &item_record_pool);
    *(struct NewItem far *)pool_items = item;
    pool_items->next = prev;
}

char far treasure_mask_add(unsigned char far *mask)                      /* 3431:1141 */
{
    register int i;
    register int row;
    short k;
    char found;

    found = 0;
    row = (*mask >> 4) & 0xf;
    for (i = 0x13; i >= 0; i--) {
        k = 2 - (i >> 3);
        if (mask[k] & (1 << (i & 7))) {
            treasure_item_add(item_classes[row][i], 1);
            found = 1;
        }
    }
    return found;
}
