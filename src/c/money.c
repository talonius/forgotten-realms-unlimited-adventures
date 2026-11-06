/* CKIT.EXE overlay 10 module 21d4: the party's money.  Pooling the members'
 * platinum, gems and jewelry, sharing the pool out again, taking coins from
 * it by menu, and appraising gems and jewelry for sale or keeping.
 *
 * EXE 0x04a550..0x04b70f, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 21d4, the first module of RTLink section 10 (payload
 * 0x4a550, link segment 21d4, parent section 3).  Its final retf is byte 0xe
 * of paragraph 22ef, where the next module begins at 22ef:000f (section 10's
 * table files every fixup site before it under frame 21d4, and the next
 * module's under 22ef).  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 10.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  Code outside the section
 * reaches four of these routines through vector stubs: 18de:1a67 -> 00f6,
 * 18de:1a53 -> 01ee, 18de:1a5d -> 0701, 18de:1a49 -> 09ab.
 *
 * This is DQK's money.c relinked, with FRUA's record layouts and the changes
 * the bytes show: the first coin is platinum; give_platinum, share_money
 * and take_menu carry amounts as long where DQK's carry unsigned; and
 * appraise's value is unsigned where DQK's is signed.
 *
 * _DATA is seven literals, DGROUP 79fa..7a5a; -d stores the "%s%s" format
 * as the tail of "%s%s%s".  It declares no _BSS; pooled is extern here.
 *
 * A member carries platinum, gems and jewelry as coins[0..2]; pool[] holds
 * the party's pooled amounts in the same order.
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

struct Pool;                    /* strutil.c's slot pool                 */

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4  not read here                      */
    unsigned char type;         /* +28                                    */
    unsigned char name1;        /* +29                                    */
    char name2;                 /* +2a                                    */
    unsigned char name3;        /* +2b 0x7a gem, 0x7b jewelry             */
    unsigned weight;            /* +2c                                    */
    unsigned value;             /* +2e                                    */
    char pad_30[2];             /* +30 not read here                      */
    char readied;               /* +32                                    */
    unsigned char unid_mask;    /* +33                                    */
    char pad_34[0xa];           /* +34 not read here                      */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x40];           /* +c   not read here                     */
    unsigned coins[3];          /* +4c  platinum, gems, jewelry           */
    char pad_52[4];             /* +52  not read here                     */
    unsigned encumbrance;       /* +56  held under carry_limit's limit   */
    char pad_58[0x3b];          /* +58  not read here                     */
    unsigned char control;      /* +93  0 or 0xb3: pooled and counted     */
    char pad_94[0x2d];          /* +94  not read here                     */
    unsigned char item_count;   /* +c1                                    */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim); /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far int_to_str(int value, char far *buf);            /* 0a75:000d */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool); /* 0a75:0203 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern void far *far rec_alloc(void);                            /* 0d48:013c */
extern void far frame_border(void);                              /* 0d65:002a */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides); /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far weight_add(struct Hero far *hero, int n);
                                /* 18de:1b11, vector to overlay 14 28aa:2a48 */
extern void far weight_sub(struct Hero far *hero, int n);
                                /* 18de:1b1b, vector to overlay 14 28aa:2a18 */
extern int  far carry_limit(struct Hero far *hero);
                                /* 18de:1b25, vector to overlay 14 28aa:2a2b */
extern long far input_amount(long most, char far *prompt, char c);
                                /* 18de:1b2f, vector to overlay 14 28aa:27e7 */
extern int  far coin_kind(char far *text, char far *buf);
                                /* 18de:1b39, vector to overlay 14 28aa:273a */

extern char far *coin_names[3];                                  /* 39a4 */
extern char far *how_many_text;                                  /* 3a04 */
extern char far *take_text;                                      /* 3a08 */
extern char far *how_much_text;                                  /* 3a0c */
extern char far *gems_text;                                      /* 3a10 */
extern char far *jewelry_text;                                   /* 3a14 */
extern char far *collection_text;                                /* 3a18 */
extern char far *gem_value_text;                                 /* 3a1c */
extern char far *jewel_value_text;                               /* 3a20 */
extern char far *coin_title;                                     /* 3a24 */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *appraise_title;                                 /* 3bd4 */
extern char far *select_items;                                   /* 3c2c */
extern char far *appraise_items;                                 /* 3c48 */
extern char far *sell_items;                                     /* 3c4c */
extern struct Hero far *cur_node;                                /* b3dd */
extern char pooled;                                              /* b424 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern long pool[3];                                             /* b8d6 */
extern struct Item far *pool_items;                              /* b8e2 */
extern char menu_escaped;                                        /* ca2a */
extern void far *node_pool;                                      /* ce5d */
extern struct Hero far *node_list;                               /* d129 */

char far too_heavy(struct Hero far *hero, long amount, long far *room)
                                                                 /* 31d4:0000 */
{
    char over;
    unsigned short most;

    most = carry_limit(hero);
    if (hero->encumbrance + amount > most) {
        over = 1;
        if (hero->encumbrance <= most)
            *room = most - hero->encumbrance;
        else
            *room = 0;
    } else {
        over = 0;
        *room = 0;
    }
    return over;
}

void far give_platinum(long amount)                              /* 31d4:007a */
{
    long room;

    if (too_heavy(cur_node, amount, &room)) {
        bottom_msg("Overloaded. Money will be put in Pool.");
        cur_node->coins[0] += room;
        weight_add(cur_node, room);
        pool[0] += amount - room;
    } else {
        cur_node->coins[0] += amount;
        weight_add(cur_node, amount);
    }
}

void far pool_money(void)                                        /* 31d4:00f6 */
{
    struct Hero far *p;
    short i;

    p = node_list;
    pooled = 1;
    while (p) {
        if (p->control == 0 || p->control == 0xb3) {
            for (i = 0; i <= 2; i++) {
                pool[i] += p->coins[i];
                weight_sub(p, p->coins[i]);
            }
            mem_fill(p->coins, sizeof p->coins, 0);
        }
        p = p->next;
    }
}

unsigned char far party_count(void)                              /* 31d4:01a1 */
{
    char r;
    struct Hero far *p;
    unsigned char n;

    p = node_list;
    n = 0;
    while (p) {
        if (p->control == 0 || p->control == 0xb3)
            n++;
        p = p->next;
    }
    r = n;
    return r;
}

void far share_money(void)                                       /* 31d4:01ee */
{
    struct Hero far *p;
    unsigned char n;
    long room;
    long share[3];
    long rest[3];
    short i;

    p = node_list;
    n = party_count();
    mem_fill(share, 4, 0);          /* 4 of 12 bytes; the loop sets all */
    mem_fill(rest, 4, 0);
    for (i = 0; i <= 2; i++) {
        if (pool[i] > 0) {
            share[i] = pool[i] / n;
            rest[i] = pool[i] % n;
        } else {
            share[i] = 0;
            rest[i] = 0;
        }
    }
    while (p) {
        if (p->control < 0x80) {
            for (i = 2; i >= 0; i--) {
                if (!too_heavy(p, share[i], &room)) {
                    p->coins[i] = share[i] + p->coins[i];
                    weight_add(p, share[i]);
                    if (rest[i] > 0) {
                        if (!too_heavy(p, 1L, &room)) {
                            p->coins[i] = p->coins[i] + 1;
                            weight_add(p, 1);
                            rest[i]--;
                        }
                    }
                } else {
                    p->coins[i] = p->coins[i] + room;
                    rest[i] = rest[i] + share[i] - room;
                    weight_add(p, room);
                }
            }
        }
        p = p->next;
    }
    for (i = 2; i >= 0; i--) {
        if (rest[i] > 0) {
            p = node_list;
            while (p) {
                room = carry_limit(p) - p->encumbrance;
                if (room > 0) {
                    if (rest[i] > room) {
                        p->coins[i] = p->coins[i] + room;
                        weight_add(p, room);
                        rest[i] = rest[i] - room;
                    } else {
                        p->coins[i] = rest[i] + p->coins[i];
                        weight_add(p, rest[i]);
                        rest[i] = 0;
                    }
                }
                p = p->next;
            }
        }
    }
    pooled = 0;
    for (i = 0; i <= 2; i++) {
        pool[i] = rest[i];
        if (pool[i])
            pooled = 1;
    }
}

void far pool_take(struct Hero far *hero, long amount, short kind)
                                                                 /* 31d4:063e */
{
    long room;

    if (too_heavy(hero, amount, &room)) {
        bottom_msg("Overloaded");
        text_pause();
        input_pending_clear();
    } else {
        if (pool[kind] < amount)
            amount = pool[kind];
        pool[kind] = pool[kind] - amount;
        hero->coins[kind] = hero->coins[kind] + amount;
        weight_add(hero, amount);
    }
}

void far take_menu(void)                                         /* 31d4:0701 */
{
    short i;
    short kind;
    struct Node far *list;
    struct Node far *cur;
    struct Node far *first;
    int top;
    char buf[50];
    long amount;
    char done;
    char redraw;
    char key;

    frame_border();
    top = 0;
    do {
        redraw = 1;
        list = 0;
        cur = 0;
        for (i = 0; i <= 2; i++) {
            if (pool[i] > 0) {
                if (!list) {
                    pool_alloc(&list, sizeof(struct Node), node_pool);
                    list->next = 0;
                } else {
                    cur = list;
                    pool_alloc(&list, sizeof(struct Node), node_pool);
                    list->next = cur;
                }
                str_format(buf, "%ld", pool[i]);
                str_copy(list->text, msg_format("%s %s", coin_names[i], buf));
                list->key = 0;
            }
        }
        first = cur = list;
        hotkey_reset(1);
        key = list_menu(coin_title, select_items, 2, 2, 0x12, 8, list, 1, 0,
                        &redraw, &top, &cur);
        if (!cur || key == 1 || menu_escaped && key == 0x1b) {
            done = 1;
        } else {
            done = 0;
            kind = coin_kind(cur->text, buf);
            if (kind == 1)
                str_copy(buf, msg_format("%s%s%s", how_many_text, buf, take_text));
            else
                str_copy(buf, msg_format("%s%s%s", how_much_text, buf, take_text));
            amount = input_amount(pool[kind], buf, 7);
            pool_take(cur_node, amount, kind);
            list_free(&first);
            pooled = 0;
            done = 1;
            for (i = 0; i <= 2; i++) {
                if (pool[i] > 0) {
                    pooled = 1;
                    done = 0;
                }
            }
        }
    } while (!done);
}

void far pool_check(char far *has_coins, char far *has_items)   /* 31d4:09ab */
{
    short i;

    *has_coins = 0;
    *has_items = 0;
    for (i = 0; i <= 2; i++)
        if (pool[i])
            *has_coins = 1;
    if (pool_items)
        *has_items = 1;
}

void far appraise(char far *done)                                /* 31d4:09fb */
{
    char text1[42];
    char text2[42];
    unsigned char key;
    unsigned char roll;
    long room;
    unsigned short value;
    struct Item far *p;
    struct Item far *item;
    char leave;
    char no_keep;
    unsigned char count;

    *done = 1;
    if (cur_node->coins[1] == 0 && cur_node->coins[2] == 0) {
        bottom_msg("No Gems or Jewelry");
        *done = 0;
        return;
    }
    do {
        if (cur_node->coins[1] == 0 && cur_node->coins[2] == 0) {
            leave = 1;
        } else {
            leave = 0;
            int_to_str(cur_node->coins[1], text1);
            int_to_str(cur_node->coins[2], text2);
            str_copy(text1, msg_format("%s%s", gems_text, text1));
            str_copy(text2, msg_format("%s%s", jewelry_text, text2));
            frame_border();
            character_name_show(cur_node, 1, 1, 0);
            status_printf(1, 7, 0xb, 0, collection_text);
            status_printf(1, 9, 0xb, 0, text1);
            status_printf(1, 0xa, 0xb, 0, text2);
            mem_fill(hotkeys, sizeof hotkeys, 0xff);
            count = 0;
            if (cur_node->coins[1])
                hotkey_add(0, &count);
            if (cur_node->coins[2])
                hotkey_add(1, &count);
            hotkey_add(2, &count);
            key = menu_run(appraise_title, appraise_items, 1, 0);
            if (menu_escaped && key == 0x1b)
                key = 2;
            switch (key) {
            case 0:
                if (cur_node->coins[1]) {
                    cur_node->coins[1]--;
                    roll = fx_roll_dice(1, 100);
                    switch (roll) {
                    case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                    case 8: case 9: case 10: case 11: case 12: case 13:
                    case 14: case 15: case 16: case 17: case 18: case 19:
                    case 20: case 21: case 22: case 23: case 24: case 25:
                        value = 5;
                        break;
                    case 26: case 27: case 28: case 29: case 30: case 31:
                    case 32: case 33: case 34: case 35: case 36: case 37:
                    case 38: case 39: case 40: case 41: case 42: case 43:
                    case 44: case 45: case 46: case 47: case 48: case 49:
                    case 50:
                        value = 25;
                        break;
                    case 51: case 52: case 53: case 54: case 55: case 56:
                    case 57: case 58: case 59: case 60: case 61: case 62:
                    case 63: case 64: case 65: case 66: case 67: case 68:
                    case 69: case 70:
                        value = 50;
                        break;
                    case 71: case 72: case 73: case 74: case 75: case 76:
                    case 77: case 78: case 79: case 80: case 81: case 82:
                    case 83: case 84: case 85: case 86: case 87: case 88:
                    case 89: case 90:
                        value = 250;
                        break;
                    case 91: case 92: case 93: case 94: case 95: case 96:
                    case 97: case 98: case 99:
                        value = 500;
                        break;
                    case 100:
                        value = 2500;
                        break;
                    }
                    int_to_str(value, text1);
                    str_copy(text1, msg_format("%s%s%s", gem_value_text, text1,
                                                   " platinum."));
                    status_printf(1, 0xc, 0xb, 0, text1);
                    mem_fill(hotkeys, sizeof hotkeys, 0xff);
                    hotkeys[0].key = 0;
                    if (too_heavy(cur_node, 1L, &room) || cur_node->item_count >= 0x10) {
                        no_keep = 1;
                    } else {
                        hotkeys[1].key = 1;
                        no_keep = 0;
                    }
                    key = menu_run(empty_text, sell_items, 0, 0);
                    box_draw();
                    if (key == 1 && !no_keep) {
                        item = rec_alloc();
                        mem_fill(item, sizeof(struct Item), 0);
                        item->next = 0;
                        item->readied = 0;
                        item->name3 = 0x7a;
                        item->name2 = 0;
                        item->name1 = 0;
                        item->type = 0x2f;
                        item->value = value * 2;
                        item->weight = 1;
                        item->unid_mask = 0;
                        p = cur_node->items;
                        if (!p) {
                            cur_node->items = item;
                        } else {
                            while (p->next)
                                p = p->next;
                            p->next = item;
                        }
                    } else
                        give_platinum(value);
                }
                break;
            case 1:
                if (cur_node->coins[2]) {
                    cur_node->coins[2]--;
                    roll = fx_roll_dice(1, 100);
                    switch (roll) {
                    case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                    case 8: case 9: case 10:
                        value = random_n(450) + 50;
                        break;
                    case 11: case 12: case 13: case 14: case 15: case 16:
                    case 17: case 18: case 19: case 20:
                        value = random_n(500) + 100;
                        break;
                    case 21: case 22: case 23: case 24: case 25: case 26:
                    case 27: case 28: case 29: case 30: case 31: case 32:
                    case 33: case 34: case 35: case 36: case 37: case 38:
                    case 39: case 40:
                        value = random_n(750) + 150;
                        break;
                    case 41: case 42: case 43: case 44: case 45: case 46:
                    case 47: case 48: case 49: case 50:
                        value = random_n(1500) + 250;
                        break;
                    case 51: case 52: case 53: case 54: case 55: case 56:
                    case 57: case 58: case 59: case 60: case 61: case 62:
                    case 63: case 64: case 65: case 66: case 67: case 68:
                    case 69: case 70:
                        value = random_n(2500) + 500;
                        break;
                    case 71: case 72: case 73: case 74: case 75: case 76:
                    case 77: case 78: case 79: case 80: case 81: case 82:
                    case 83: case 84: case 85: case 86: case 87: case 88:
                    case 89: case 90:
                        value = random_n(3000) + 1000;
                        break;
                    case 91: case 92: case 93: case 94: case 95: case 96:
                    case 97: case 98: case 99: case 100:
                        value = random_n(5000) + 1000;
                        break;
                    }
                    int_to_str(value, text2);
                    str_copy(text1, msg_format("%s%s%s", jewel_value_text, text2,
                                                   " platinum."));
                    status_printf(1, 0xc, 0xb, 0, text1);
                    mem_fill(hotkeys, sizeof hotkeys, 0xff);
                    hotkeys[0].key = 0;
                    if (too_heavy(cur_node, 1L, &room) || cur_node->item_count >= 0x10) {
                        no_keep = 1;
                    } else {
                        hotkeys[1].key = 1;
                        no_keep = 0;
                    }
                    key = menu_run(empty_text, sell_items, 0, 0);
                    box_draw();
                    if (key == 1 && !no_keep) {
                        item = rec_alloc();
                        mem_fill(item, sizeof(struct Item), 0);
                        item->next = 0;
                        item->readied = 0;
                        item->name3 = 0x7b;
                        item->name2 = 0;
                        item->name1 = 0;
                        item->type = 0x2f;
                        item->value = value * 2;
                        item->weight = 1;
                        item->unid_mask = 0;
                        p = cur_node->items;
                        if (!p) {
                            p = item;   /* not cur_node->items: 21d4:116a */
                        } else {
                            while (p->next)
                                p = p->next;
                            p->next = item;
                        }
                    } else
                        give_platinum(value);
                }
                break;
            case 2:
                leave = 1;
                break;
            }
            recalc_stats(cur_node);
        }
    } while (!leave);
}
