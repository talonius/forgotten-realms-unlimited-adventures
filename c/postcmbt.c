/* CKIT.EXE overlay 10 module 22ef: after a fight.  Settling the combat --
 * who fled, who fell, who is revived -- the experience from the monsters,
 * their money and the treasure's worth, the summary box, the treasure
 * menu with taking coins and items, and the combatants' clean-up.
 *
 * EXE 0x04b70f..0x04cb25, one Turbo C large-model _TEXT segment starting at
 * 22ef:000f in RTLink section 10 (payload 0x4a550, link segment 21d4,
 * parent section 3), after money.c's final retf at 22ef:000e.  Its own
 * final retf is 22ef:1424, byte 4 of paragraph 2431, where the next module
 * begins at 2431:0005 (section 10's table files the last 22ef fixup site at
 * 0x04cb22 and the first 2431 site at 0x04cb45).  Built with Turbo C 2.01, tcc
 * -ml -O -Z -d.  Functions are defined in shipped order; bake consumes them
 * in that order.  Code outside the section reaches post_combat through the
 * vector stub 18de:1a2b.
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
 * This is DQK's postcmbt.c relinked, with FRUA's record layouts and the
 * changes the bytes show: a combatant's state, side and class are bytes;
 * loot_exp counts a monster's experience before the flag that withholds its
 * money and items; only a member who is not a monster keeps the party
 * alive; three spells detect treasure; a lost fight takes one line and
 * clears chain_allowed; and combat_results clears 0x11 effects, not 0x12.
 * DQK's drops_return has no FRUA counterpart, and post_combat does not
 * call it.
 * loot_exp's pool[0] * 2 is Turbo C's inline long shift, shl ax,1;
 * rcl dx,1.
 *
 * _DATA is DGROUP 7a5c..7ad1: the code's ten literals, eight distinct,
 * stored as seven -- -d stores the three "Take:" once and "Items Exit" as
 * the tail of "Money Items Exit".  _BSS is three chars, fled, killed and
 * has_items, at d20a..d20c.
 *
 * This is the v1.0 build (CKIT.EXE of March 19, 1993): EXE 0x04b6af..0x04cab7,
 * stored 32f1:000f in section 10, _DATA at 7a08, _BSS at d1ac.  v1.0 still has
 * two of DQK's forms that v1.2 changed: loot_exp adds a monster's money to the
 * pool before the cursor_obj[0x1e] flag (the flag withholds only its items),
 * and any combatant able or in state 3, 4 or 5 -- monsters included -- keeps
 * the party alive.
 */

struct Stat {
    unsigned char base;         /* +0                                     */
    unsigned char cur;          /* +1                                     */
};

struct Fight {                  /* a combatant's state in a fight         */
    char pad_0[0x14];           /* +0   not read here                     */
    char not_pc;                /* +14  1: a monster, not the party's     */
};

struct Item {                   /* an item, 0x3e bytes                    */
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28                                    */
    unsigned char name1;        /* +29                                    */
    char name2;                 /* +2a                                    */
    unsigned char name3;        /* +2b                                    */
    unsigned weight;            /* +2c                                    */
    char pad_2e[2];             /* +2e  not read here                     */
    char plus;                  /* +30                                    */
    char save_bonus;            /* +31                                    */
    char readied;               /* +32                                    */
    unsigned char unid_mask;    /* +33                                    */
    char cursed;                /* +34                                    */
    unsigned char count;        /* +35                                    */
    unsigned char charges;      /* +36                                    */
    unsigned char effect;       /* +37                                    */
    unsigned char special;      /* +38                                    */
    char pad_39[5];             /* +39  not read here                     */
};

struct Hero {                   /* a combatant, party member or monster   */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x34];           /* +c   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    long xp;                    /* +44                                    */
    char pad_48[4];             /* +48  not read here                     */
    unsigned coins[3];          /* +4c  platinum, gems, jewelry           */
    char pad_52[2];             /* +52  not read here                     */
    unsigned xp_value;          /* +54  a monster's experience            */
    char pad_56[3];             /* +56  not read here                     */
    unsigned char cls;          /* +59                                    */
    char pad_5a[4];             /* +5a  not read here                     */
    unsigned char status;       /* +5e                                    */
    unsigned char side;         /* +5f  1: the monsters'                  */
    char pad_60[0x10];          /* +60  not read here                     */
    struct Stat stats[4];       /* +70  strength, intelligence, wisdom,   */
                                /*      dexterity                         */
    char pad_78[0x1b];          /* +78  not read here                     */
    unsigned char control;      /* +93  bit 7: not a player character     */
    char pad_94[0x31];          /* +94  not read here                     */
    char level_up;              /* +c5                                    */
    unsigned char spells[0x8d]; /* +c6                                    */
    char pad_153[0x2b];         /* +153 not read here                     */
    char active;                /* +17e                                   */
    char pad_17f[0xc];          /* +17f not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Effect;
struct Pool;

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far item_name(struct Hero far *h, struct Item far *it, char col, char row,
                          char full, char show);                 /* 03d0:03d1 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern void far hero_drop(char keep, char icon);                 /* 03d0:1e69 */
extern char far can_level_up(struct Hero far *h);                   /* 03d0:1f71 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far continue_prompt(char a, char b, char far *items); /* 06c9:0932 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far set_aborted(char c);                             /* 083a:032f */
extern char far get_aborted(void);                               /* 083a:033a */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far set_menu_kind(int kind);                         /* 083a:13f2 */
extern char far node_prompt(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1a07 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim); /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Item far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Item far * far *out);           /* 083a:20d1 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far rec_free(void far * far *pp);                    /* 0d48:0162 */
extern void far frame_border(void);                              /* 0d65:002a */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far party_display(void far *node);   /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);  /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern void far char_view(char far *done);   /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far spell_cast(unsigned char n, char a, char b, char far *done);
                                                /* 18de:1175, vector to overlay 12 2b69:14c8 */
extern void far pool_clear(void);                                /* overlay 3 1bfb:0002 */
extern void far pool_set_records(int n);                          /* overlay 3 1bfb:00d0 */
extern int  far pool_records(void);                           /* overlay 3 1bfb:00db */
extern void far pool_money(void);                                /* 21d4:00f6 */
extern void far share_money(void);                               /* 21d4:01ee */
extern void far take_menu(void);                                 /* 21d4:0701 */
extern void far pool_check(char far *has_coins, char far *has_items); /* 21d4:09ab */
extern void far item_give(struct Item far *item, char far *failed); /* 2431:01eb */

extern char far *continue_items;                                 /* 390c */
extern char far *fled_text;                                      /* 3b60 */
extern char far *lost_text;                                      /* 3b64 */
extern char far *treasure_text;                                  /* 3b74 */
extern char far *no_treasure_text;                               /* 3b78 */
extern char far *receives_text;                                  /* 3b7c */
extern char far *xp_text;                                        /* 3b80 */
extern char far *left_text;                                      /* 3b84 */
extern char far *claim_text;                                     /* 3b88 */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *items_title;                                    /* 3c14 */
extern char far *take_items;                                     /* 3cbf */
extern char far *treasure_items;                                 /* 3cc7 */
extern unsigned char fx_clear[0x11];                             /* 6177 */
extern char chain_allowed;                                           /* 671d */
extern struct Hero far *cur_node;                                /* b3dd */
extern char next_monster_group;                                           /* b3e1 */
extern unsigned char text_col;                                   /* b419 */
extern unsigned char text_row;                                   /* b41a */
extern long exp_share;                                           /* b41b */
extern int view_mode;                                            /* b421 */
extern char esc_check;                                           /* b423 */
extern char text_slow;                                           /* b431 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char combat_won;                                          /* b4ea */
extern unsigned char down_count;                                 /* b737 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern long pool[3];                                             /* b8d6 */
extern struct Item far *pool_items;                              /* b8e2 */
extern char combat_outcome[4];                                         /* b8ee */
extern char adventure_exit;                                           /* bcf6 */
extern char monsters_queued;                                           /* c3c1 */
extern struct Item far *dropped_item_boundary;                               /* ca16 */
extern unsigned char ai_level;                                   /* ca21 */
extern char menu_escaped;                                        /* ca2a */
extern char has_coins;                                           /* cc6d */
extern struct Pool combat_pool;                                  /* ce72 */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern char encounter_npc_count;                                           /* d135 */

static char fled;                                                /* d20a */
static char killed;                                              /* d20b */
static char has_items;                                           /* d20c */

void far post_reset(void);

void far loot_join(struct Item far *item)                        /* 32ef:000f */
{
    struct Item far *q;
    struct Item far *p;
    struct Item far *next;
    struct Item far *dest;
    unsigned char count;

    dest = item;
    count = item->count;
    p = pool_items;
    while (p) {
        next = p->next;
        if (p != dest && p->count > 0 && p->name1 == dest->name1
            && p->name2 == dest->name2 && p->name3 == dest->name3
            && p->type == dest->type && p->plus == dest->plus
            && p->save_bonus == dest->save_bonus && p->cursed == dest->cursed
            && p->weight == dest->weight
            && p->charges == p->charges && p->charges < 2  /* p with p, as shipped */
            && p->effect == p->effect && p->special == p->special) {
            if (p->count + count <= 0xff) {
                count += p->count;
                q = pool_items;
                if (p == q)
                    pool_items = p->next;
                else
                    while (q && q->next != p)
                        q = q->next;
                q->next = p->next;
                rec_free(&p);
            } else {
                dest->count = 0xff;
                p->count = p->count - (0xff - count);
                count = p->count;
                dest = p;
            }
        }
        p = next;
    }
    dest->count = count;
}

void far loot_exp(long far *share)                               /* 32ef:01ff */
{
    struct Hero far *p;
    short i;
    long total;
    struct Item far *q;
    struct Item far *it;
    unsigned char n;

    total = 0;
    p = node_list;
    n = 0;
    while (p) {
        it = p->items;
        if (p->side == 1 && p->status != 0 && p->status != 3 && p->status != 9) {
            killed = 1;
            if (p->fight->not_pc == 1) {
                for (i = 0; i <= 2; i++)
                    pool[i] += p->coins[i];
                total += p->xp_value;
                if (!cursor_obj[0x1e]) {
                    if (it && it->type != 0x69) {
                        q = pool_items;
                        pool_items = it;
                        for (;;) {
                            n++;
                            it->readied = 0;
                            if (!it->next)
                                break;
                            if (pool_items->count > 0 && it->unid_mask)
                                loot_join(pool_items);
                            it = it->next;
                        }
                        it->next = q;
                        p->items = 0;
                    }
                }
            }
        }
        p = p->next;
    }
    total += pool[0] * 2;
    total += pool[1] * 250;
    total += pool[2] * 2200;
    q = pool_items;
    while (q && q != dropped_item_boundary) {
        if (q->plus > 0)
            total += q->plus * 400;
        q = q->next;
    }
    if (cursor_obj[0x20] <= down_count)
        down_count--;
    *share = total / (cursor_obj[0x20] - down_count);
}

void far exp_award(long exp)                                     /* 32ef:042b */
{
    struct Hero far *p;
    long share;

    if (cursor_obj[0x1f])
        return;
    p = node_list;
    while (p) {
        if (p->active && p->status != 1) {
            share = exp;
            switch (p->cls) {
            case 0:
                if (p->stats[2].cur > 0xe)
                    share = exp / 10 + exp;
                break;
            case 2:
                if (p->stats[0].cur > 0xf)
                    share = exp / 10 + exp;
                break;
            case 3:
                if (p->stats[0].cur > 0xf && p->stats[2].cur > 0xf)
                    share = exp / 10 + exp;
                break;
            case 4:
                if (p->stats[0].cur > 0xf && p->stats[1].cur > 0xf
                    && p->stats[2].cur > 0xf)
                    share = exp / 10 + exp;
                break;
            case 5:
                if (p->stats[1].cur > 0xf)
                    share = exp / 10 + exp;
                break;
            case 6:
                if (p->stats[3].cur > 0xf)
                    share = exp / 10 + exp;
                break;
            case 8: case 10: case 11: case 12: case 13: case 14: case 16:
                share = exp / 2;
                break;
            case 9: case 15:
                share = exp / 3;
                break;
            }
            p->xp += share;
            p->level_up = can_level_up(p);
        }
        p = p->next;
    }
}

void far combat_results(void)                                    /* 32ef:0599 */
{
    struct Hero far *p;
    char done;
    unsigned char i;
    char alive;

    down_count = 0;
    adventure_exit = 1;
    fled = 0;
    p = node_list;
    while (p && p->fight->not_pc != 1) {
        if (p->status == 3)
            fled = 1;
        p = p->next;
    }
    p = node_list;
    alive = 0;
    while (p && !alive) {
        if (p->active || p->status == 4 || p->status == 3 || p->status == 5)
            alive = 1;
        p = p->next;
    }
    if (cursor_obj[0x1d] && alive)
        adventure_exit = 0;
    combat_won = 0;
    p = node_list;
    if (!esc_check) {
        while (p && p->fight->not_pc != 1) {
            if ((p->status == 3 || p->status == 1 || p->status == 0)
                && p->side == 0 && !(p->control & 0x80))
                adventure_exit = 0;
            if (p->status == 1 || p->status == 0) {
                combat_won = 1;
                fled = 0;
            }
            if (!p->active || p->status == 1)
                down_count++;
            for (i = 0; i < 0x11; i++)
                fx_remove(p, fx_clear[i], 0);
            p = p->next;
        }
        if (combat_won) {
            loot_exp(&exp_share);
            exp_award(exp_share);
        }
        p = node_list;
        if (!adventure_exit) {
            while (p && p->fight->not_pc != 1) {
                if (!fled) {
                    switch (p->status) {
                    case 3:
                        p->status = 0;
                        p->active = 1;
                        break;
                    case 5:
                        if (cursor_obj[0x1d]) {
                            p->status = 0;
                            p->active = 1;
                            p->hp = 1;
                        } else
                            p->status = 4;
                        break;
                    case 4:
                        if (p->hp > 0) {
                            p->status = 0;
                            p->active = 1;
                        } else if (cursor_obj[0x1d]) {
                            p->status = 0;
                            p->active = 1;
                            p->hp = 1;
                        }
                        break;
                    }
                    p = p->next;
                } else if (p->status == 3) {
                    p->status = 0;
                    p->active = 1;
                    p = p->next;
                } else {
                    cur_node = p;
                    p = p->next;
                    hero_drop(0, 1);
                }
            }
        } else {
            p = node_list;
            done = 0;
            while (p && !done) {
                if (p->fight->not_pc != 1) {
                    cur_node = p;
                    hero_drop(0, 1);
                } else
                    done = 1;
                p = p->next;
            }
            cursor_obj[0x20] = 0;
        }
    } else {
        p = node_list;
        while (p) {
            if (p->active && p->status == 0 && p->side != 1) {
                combat_won = 1;
                loot_exp(&exp_share);
                exp_award(exp_share);
            }
            p = p->next;
        }
        p = node_list;
        while (p) {
            if (p->status == 0 || p->status == 1)
                p->active = 1;
            if (p->status == 5)
                p->status = 4;
            p = p->next;
        }
    }
}

void far combat_summary(long exp)                                /* 32ef:0931 */
{
    char num[12];
    char msg[82];
    struct Item far *p;
    struct Item far *q;

    frame_border();
    if (killed) {
        if (fled) {
            status_printf(1, 3, 7, 0, fled_text);
            exp = 0;
            p = pool_items;
            while (p) {
                q = p->next;
                rec_free(&p);
                p = q;
            }
            mem_zero(pool, sizeof pool);
        } else if (!combat_won && cursor_obj[0x1d]) {
            status_printf(1, 3, 7, 0, lost_text);
            exp = 0;
        } else
            status_printf(1, 3, 7, 0, "The party has won.");
    }
    str_format(num, "%l", exp);
    str_copy(msg, msg_format("%s%ld", receives_text, exp));
    if (!cursor_obj[0x1f]) {
        status_printf(1, 5, 7, 0, msg);
        status_printf(1, 7, 7, 0, xp_text);
    }
    has_coins = 0;
    has_items = 0;
    pool_check(&has_coins, &has_items);
    if (has_coins || has_items)
        status_printf(1, 10, 7, 0, treasure_text);
    else
        status_printf(1, 10, 7, 0, no_treasure_text);
    menu_run(empty_text, continue_items, 1, 0);
}

void far loot_pick(char far *key, struct Item far * far *pick,
                   struct Item far * far *cur, int far *top)     /* 32ef:0b16 */
{
    struct Item far *head;
    char redraw;
    struct Item far *p;

    redraw = 1;
    p = pool_items;
    while (p) {
        item_name(0, p, 0, 0, 0, 0);
        p = p->next;
    }
    head = pool_items;
    hotkey_reset(1);
    *key = list_menu(items_title, take_items, 1, 1, 0x26, 0x16, head, 1, 0,
                     &redraw, top, cur);
    *pick = *cur;
}

void far loot_items(void)                                        /* 32ef:0bd0 */
{
    struct Item far *pick;
    struct Item far *p;
    char key;
    char done;
    struct Item far *cur;
    int top;
    char failed;

    pick = 0;
    cur = pool_items;
    top = 0;
    frame_border();
    ui_mode = 1;
    do {
        loot_pick(&key, &pick, &cur, &top);
        if (key)
            done = 1;
        else {
            done = 0;
            item_give(pick, &failed);
            if (!failed) {
                p = pool_items;
                if (view_mode == 10) {
                    if (pick->type == 0x49)
                        pool_set_records(pool_records() - (pick->count + 1));
                    else
                        pool_set_records(pool_records() - 1);
                }
                if (p == pick)
                    pool_items = pick->next;
                else {
                    while (p->next != pick)
                        p = p->next;
                    p->next = pick->next;
                }
                pick->next = 0;
                if (pick)
                    rec_free(&pick);
                if (!pool_items)
                    done = 1;
            }
        }
    } while (!done);
    view_redraw();
}

void far loot_take(char far *coins, char far *items)             /* 32ef:0d13 */
{
    unsigned char key;
    char done;

    ui_mode = 1;
    done = 0;
    if (*coins) {
        if (*items) {
            do {
                if (*coins && *items) {
                    hotkey_reset(2);
                    key = node_prompt("Take:", "Money Items Exit", 1, 1);
                } else if (*coins) {
                    hotkey_reset(1);
                    key = node_prompt("Take:", "Money Exit", 1, 1);
                    if (!menu_escaped && key == 1)
                        key = 2;
                } else {
                    hotkey_reset(1);
                    key = node_prompt("Take:", "Items Exit", 1, 1);
                    if (!menu_escaped && key < 2)
                        key++;
                }
                if (menu_escaped) {
                    switch (key) {
                    case 0x1b:
                        done = 1;
                        break;
                    default:
                        party_rows(cur_node, 0);
                        party_key_select(key);
                        party_rows(cur_node, 1);
                    }
                } else {
                    switch (key) {
                    case 0:
                        take_menu();
                        view_redraw();
                        break;
                    case 1:
                        loot_items();
                        break;
                    case 2:
                        done = 1;
                        break;
                    default:
                        party_rows(cur_node, 0);
                        party_key_select(key);
                        party_rows(cur_node, 1);
                    }
                }
                pool_check(coins, items);
                if (!*items)
                    done = 1;
                party_display(cur_node);
            } while (!done);
        } else {
            take_menu();
            view_redraw();
        }
    } else
        loot_items();
}

void far loot_menu(void)                                         /* 32ef:0ea5 */
{
    unsigned char i;
    unsigned char count;
    unsigned char key;
    char done;
    char changed;
    unsigned char spell;
    char detect;
    char saved;

    done = 0;
    view_redraw();
    saved = 1;
    do {
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        pool_check(&has_coins, &has_items);
        i = 0;
        detect = 0;
        if (has_items)
            while (i <= 0x8c && !detect) {
                if ((cur_node->spells[i] == 5 || cur_node->spells[i] == 0xb
                     || cur_node->spells[i] == 0x4d)
                    && cur_node->active) {
                    detect = 1;
                    spell = cur_node->spells[i];
                }
                i++;
            }
        count = 0;
        for (i = 0; i <= 5; i++) {
            switch (i) {
            case 1:
                if (has_coins || has_items)
                    hotkey_add(1, &count);
                break;
            case 3:
                if (has_coins)
                    hotkey_add(3, &count);
                break;
            case 4:
                if (detect)
                    hotkey_add(4, &count);
                break;
            default:
                hotkey_add(i, &count);
            }
        }
        ui_mode = saved;
        set_menu_kind(0xc);
        key = node_prompt(empty_text, treasure_items, 1, 1);
        saved = ui_mode;
        if (menu_escaped) {
            switch (key) {
            case 0x1b:
                key = 5;
                break;
            case 0x84:
            case 0x88:
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
                key = 0xff;
                break;
            }
        }
        if (!get_aborted()) {
            switch (key) {
            case 0:
                char_view(&changed);
                break;
            case 1:
                loot_take(&has_coins, &has_items);
                break;
            case 2:
                pool_money();
                break;
            case 3:
                share_money();
                break;
            case 4:
                spell_cast(spell, 0, 0, &changed);
                break;
            case 5:
                pool_check(&has_coins, &has_items);
                if (has_coins || has_items) {
                    text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, left_text);
                    text_wrap(1, 0x11, 0x26, 0x16, 0xb, 0, 0, claim_text);
                    if (confirm(empty_text, 0))
                        text_clear(1, 0x11, 0x26, 0x16);
                    else
                        done = 1;
                } else
                    done = 1;
                break;
            }
        } else
            set_aborted(0);
    } while (!done);
    sfx_play(0xff);
}

void far fight_cleanup(void)                                    /* 32ef:11a9 */
{
    struct Hero far *p;
    struct Hero far *next;

    cursor_obj[0x15] = 0;
    p = node_list;
    while (p) {
        if (p->fight->not_pc == 1 || p->side == 1) {
            killed = 1;
            if (!p->active)
                cursor_obj[0x15]++;
            next = p->next;
            cur_node = p;
            hero_drop(p->fight->not_pc == 1, 1);
            p = next;
        } else {
            if (p->fight) {
                pool_free(p->fight, 0x18, &combat_pool);
                p->fight = 0;
            }
            p = p->next;
        }
    }
    cur_node = node_list;
}

void far post_combat(void)                                       /* 32ef:1290 */
{
    struct Item far *p;
    struct Item far *q;
    struct Hero far *h;

    cursor_obj[0x2e] = 0;
    mem_zero(combat_outcome, sizeof combat_outcome);
    killed = 0;
    if (!esc_check)
        combat_results();
    view_mode = 6;
    fight_cleanup();
    if (!esc_check) {
        h = node_list;
        combat_outcome[1] = 1;
        while (h) {
            recalc_stats(h);
            h = h->next;
        }
        if (!adventure_exit) {
            if (fled) {
                p = pool_items;
                while (p) {
                    q = p;
                    p = p->next;
                    rec_free(&q);
                }
                pool_items = 0;
            } else {
                combat_outcome[0] = 1;
                combat_outcome[1] = 0;
            }
            if (!esc_check) {
                combat_summary(exp_share);
                if (has_coins || has_items)
                    loot_menu();
            }
            pool_clear();
        } else {
            frame_border();
            text_col = 2;
            text_row = 6;
            text_wrap(2, 5, 0x25, 0x16, 7, 0, 1,
                      "The monsters rejoice, for the party has been destroyed!");
            chain_allowed = 0;
            continue_prompt(0, 0, continue_items);
        }
        text_slow = 1;
        cursor_obj[1] = 0;
        cursor_obj[0x2f] = 0;
        cursor_obj[0x1c] = 0;
        cursor_obj[0x1e] = 0;
        cursor_obj[0x1d] = 0;
        cursor_obj[0x1f] = 0;
    }
    post_reset();
}

void far post_reset(void)                                        /* 32ef:140b */
{
    ai_level = 0;
    encounter_npc_count = 0;
    monsters_queued = 0;
    next_monster_group = 8;
    pool_clear();
}
