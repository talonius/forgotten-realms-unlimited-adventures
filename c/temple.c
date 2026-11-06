/* CKIT.EXE overlay 10 module 254c: the temple.  Pricing a service from the
 * temple's rate, paying for it from the member's platinum or the pool, the
 * heal menu's services -- the cures, healing points, raising the dead,
 * resurrection, removing a curse, stone to flesh and restoring lost levels
 * -- fixing the whole party at once, and the temple menu.
 *
 * EXE 0x04dcde..0x04efe2, one Turbo C large-model _TEXT segment starting at
 * 254c:000e in RTLink section 10 (payload 0x4a550, link segment 21d4,
 * parent section 3), after module 2431's final retf at 254c:000d.  Its own
 * final retf is 254c:1311, byte 1 of paragraph 267d, where the next module
 * begins at 267d:0002.  Section 10's table files all 102 fixup sites in the
 * range under frame 254c.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 10.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and an ovlNN_ name which section and stored
 * segment.  A mod18de_ callee is an RTLink vector stub; its comment names
 * the routine the stub reaches.  Code outside the section reaches
 * temple_shop through the vector stub 18de:1a3f and rate_price through
 * 18de:1a7b; module 2431 far-calls rate_price three times.
 *
 * DQK's temple.c reworked: every service takes the temple's event record
 * and charges through temple_pay at the event's rate (ev[5]); the heal
 * menu lists only the services the temple's level (ev[4]) offers, and adds
 * resurrection (temple_resurrect); the menu adds the money commands.
 * rate_price and temple_pay are FRUA's own; temple_resurrect is
 * raise_dead reworked (the map pairs it partial with DQK's raise_dead).  heal_menu
 * counts map[12], one entry past the twelve: it reads the high word of
 * list, zeroed just before, so one more node is counted than is filled.
 *
 * _DATA is DGROUP 7e66..7ff7: rate_div, rate_mul, the template of
 * heal_menu's map, and the literals, 26 distinct stored as 25 -- -d stores
 * "%s" on the tail of "%s%s%ld%s".  7e65 is the zero pad after module
 * 2431's _DATA (7ad2..7e64), and 7ff8 starts another module's literals.
 * The module has no _BSS.
 */

struct Effect;

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x30];           /* +4   not read here                     */
    char cursed;                /* +34                                    */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x40];           /* +c   not read here                     */
    unsigned platinum;          /* +4c  platinum                          */
    char pad_4e[0xa];           /* +4e  not read here                     */
    unsigned char race;         /* +58                                    */
    char pad_59[5];             /* +59  not read here                     */
    unsigned char status;       /* +5e  1 or 6 dead, 7 stone, 8 gone      */
    char pad_5f;                /* +5f  not read here                     */
    char name[0x18];            /* +60                                    */
    unsigned char con_base;     /* +78                                    */
    char pad_79[8];             /* +79  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[0x14];          /* +82  not read here                     */
    unsigned char level_max[7]; /* +96  one per class                     */
    unsigned char level[7];     /* +9d  one per class                     */
    char pad_a4[0xda];          /* +a4  not read here                     */
    char active;                /* +17e                                   */
    char pad_17f[0xc];          /* +17f not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct Node {
    struct Node far *next;      /* +0                                    */
    char key;                   /* +4                                    */
    char text[35];              /* +5                                    */
};

struct Heal {                   /* a heal-menu service                    */
    int spell;                  /* +0  spell_names index                  */
    int level;                  /* +2  offered when the temple's level    */
};                              /*     is at least this                   */

extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far press_return_wait(char anim);                            /* 083a:054a */
extern void far list_alloc(int n, struct Node far * far *head);  /* 083a:0575 */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far node_prompt(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1a07 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern char far *far int_str(int n);                             /* 0d48:019d */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far party_display(void far *node);   /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);  /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far char_view(char far *done);   /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far spell_run(unsigned char spell);
                                                /* 18de:1ae9, vector to overlay 12 2b69:217d */
extern char far fx_heal(struct Hero far *hero, char a, char b);
                                                /* 18de:1af3, vector to overlay 19 2e88:224c */
extern void far spell_remove_curse(void);             /* 18de:1afd, vector to overlay 12 26e3:137e */
extern void far fx_stat_recalc(struct Hero far *hero, short stat);
                                                /* 18de:1b07, vector to overlay 19 2e88:1a80 */
extern char far donate(int a, int b, int c);
                                                /* 18de:1b75, vector to overlay 14 28aa:326e */
extern void far pool_money(void);                                /* 21d4:00f6 */
extern void far share_money(void);                               /* 21d4:01ee */
extern void far take_menu(void);                                 /* 21d4:0701 */
extern void far pool_check(char far *has_coins, char far *has_items); /* 21d4:09ab */
extern void far appraise(char far *done);                        /* 21d4:09fb */

extern char far *spell_names[];                                  /* 2434 */
extern char far *cure_anyway_text;                               /* 3b8c */
extern char far *not_blind_text;                                 /* 3b98 */
extern char far *cannot_raise_text;                              /* 3ba0 */
extern char far *beyond_help_text;                               /* 3ba4 */
extern char far *not_dead_text;                                  /* 3ba8 */
extern char far *not_cursed_text;                                /* 3bb0 */
extern char far *not_stoned_text;                                /* 3bb4 */
extern char far *no_lost_text;                                   /* 3bb8 */
extern char far *help_you_text;                                  /* 3bbc */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *heal_items;                                     /* 3ccb */
extern char far *temple_items;                                   /* 3cd3 */
extern unsigned char disease_fx[];      /* 6171  [1..6] are read here */
extern struct Hero far *cur_node;                                /* b3dd */
extern char confirm_anim;                                        /* b42a */
extern char fx_lock;                                             /* b8d0 */
extern long pool[3];                                             /* b8d6 */
extern unsigned char current_spell;                                  /* be64 */
extern char menu_escaped;                                        /* ca2a */
extern struct Hero far *spell_target;                            /* cfe7 */
extern struct Hero far *node_list;                               /* d129 */

static unsigned char rate_div[8] = { 100, 50, 20, 10, 5, 4, 3, 2 }; /* 7e66 */
static unsigned char rate_mul[8] = { 2, 3, 4, 5, 10, 20, 50, 100 }; /* 7e6e */

char far confirm_anyway(char far *msg)                           /* 354c:000e */
{
    char c;
    char anim;

    hero_text(cur_node, msg, 0, 0);
    anim = confirm_anim;
    confirm_anim = 0;
    c = confirm(cure_anyway_text, 0);
    confirm_anim = anim;
    combat_text_clear();
    return c;
}

long far rate_price(long price, unsigned char rate, unsigned count)
                                                                 /* 354c:0063 */
{
    long cost;

    if (price < 1)
        price = 1;
    if (count > 1)
        price = price * count;
    if (rate == 0)
        cost = 0;
    else if (rate <= 8) {
        cost = price / rate_div[rate - 1];
        if (cost < 1)
            cost = 1;
    }
    else if (rate == 9) {
        cost = price * 2 / 3;
        if (cost < 1)
            cost = 1;
    } else if (rate == 10)
        cost = price;
    else if (rate == 11)
        cost = price * 3 / 2;
    else
        cost = price * rate_mul[rate - 12];
    return cost;
}

char far temple_pay(char far *what, long price, unsigned char rate) /* 354c:0183 */
{
    char r;
    char buf[255];
    char ok;
    long coins;
    long pooled;
    long cost;
    char anim;

    cost = rate_price(price, rate, 1);
    if (cost != 0) {
        str_format(buf, "%s%s%ld%s", what, " will only cost ", cost, " platinum pieces.");
        text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, buf);
        anim = confirm_anim;
        confirm_anim = 0;
        ok = confirm("pay for cure", 1);
        confirm_anim = anim;
        if (ok == 1) {
            coins = cur_node->platinum;
            if (cost <= coins)
                cur_node->platinum -= cost;
            else {
                pooled = pool[0];
                if (cost <= pooled)
                    pool[0] -= cost;
                else {
                    bottom_msg("Not enough money.");
                    combat_text_clear();
                    ok = 0;
                }
            }
        } else
            combat_text_clear();
    } else
        ok = 1;
    if (ok == 1) {
        combat_text_clear();
        if (str_cmp("Fix", what))
            hero_text(cur_node, "is cured.", 0, 1);
        else {
            str_format(buf, "the whole party is healed.");
            text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, buf);
            press_return_wait(0);
            combat_text_clear();
        }
    }
    r = ok;
    return r;
}

void far cure_blind(unsigned char far *ev)                       /* 354c:0337 */
{
    struct Effect far *eff;
    char s[41];
    char c;

    c = 1;
    if (!find_effect(cur_node, 0x21, &eff))
        c = confirm_anyway(not_blind_text);
    if (c == 1) {
        if (temple_pay("Cure Blindness", 200L, ev[5]))
            fx_remove(cur_node, 0x21, 0);
        hero_text(cur_node, s, 10, 1);   /* s is never set, as shipped */
    }
}

void far cure_disease(unsigned char far *ev)                     /* 354c:03c9 */
{
    struct Effect far *eff;
    char s[41];
    unsigned char i;
    char found;
    char c;

    found = 0;
    for (i = 1; i <= 6; i++)
        if (find_effect(cur_node, disease_fx[i], &eff)) {
            found = 1;
            i = 6;
        }
    if (!found)
        c = confirm_anyway("is not Diseased");
    else
        c = 1;
    if (c == 1 && temple_pay("Cure Disease", 200L, ev[5])) {
        fx_lock = 1;
        for (i = 1; i <= 6; i++)
            fx_remove(cur_node, disease_fx[i], 0);
        fx_stat_recalc(cur_node, 0);
        fx_lock = 0;
        str_copy(s, "is cured");
        hero_text(cur_node, s, 0, 1);
    }
}

void far heal_points(unsigned char kind, unsigned char far *ev)  /* 354c:04cc */
{
    unsigned char hp;

    switch (kind) {
    case 1:
        if (temple_pay("Cure Light Wounds", 20L, ev[5])) {
            hp = fx_roll_dice(1, 8);
            fx_heal(cur_node, hp, 0);
        }
        break;
    case 2:
        if (temple_pay("Cure Serious Wounds", 70L, ev[5])) {
            hp = fx_roll_dice(2, 8) + 1;
            fx_heal(cur_node, hp, 0);
        }
        break;
    case 3:
        if (temple_pay("Cure Critical Wounds", 120L, ev[5])) {
            hp = fx_roll_dice(3, 8) + 3;
            fx_heal(cur_node, hp, 0);
        }
        break;
    case 4:
        if (temple_pay("Heal", 1000L, ev[5])) {
            hp = fx_roll_dice(1, 4);
            if (cur_node->hp + hp > cur_node->hp_max)
                hp = 0;
            else
                hp = cur_node->hp_max - cur_node->hp - hp;
            fx_heal(cur_node, hp, 0);
            fx_remove(cur_node, 0x21, 0);
            fx_remove(cur_node, 0x44, 0);
            fx_stat_recalc(cur_node, 1);
            fx_stat_recalc(cur_node, 2);
        }
        break;
    }
}

void far raise_dead(unsigned char far *ev)                       /* 354c:0682 */
{
    char c;
    char dead;
    struct Hero far *hero;
    char cannot;
    char gone;

    hero = cur_node;
    dead = 0;
    cannot = 0;
    c = 1;
    if (hero->status == 6 || hero->status == 1)
        dead = 1;
    gone = hero->status == 8;
    if (hero->race < 1)
        cannot = 1;
    if (!dead || cannot) {
        if (cannot)
            c = confirm_anyway(cannot_raise_text);
        else if (gone)
            c = confirm_anyway(beyond_help_text);
        else
            c = confirm_anyway(not_dead_text);
    }
    if (c == 1 && temple_pay("Raise Dead", 1100L, ev[5]) && dead && !cannot && !gone) {
        spell_target = hero;
        current_spell = 0x4b;
        spell_run(0x4b);
    }
}

void far cure_poison(unsigned char far *ev)                      /* 354c:0775 */
{
    struct Effect far *eff;
    char c;
    char found;

    found = 0;
    c = 1;
    if (find_effect(cur_node, 0x37, &eff))
        found = 1;
    if (!found)
        c = confirm_anyway("is not poisoned.");
    if (c == 1 && temple_pay("Neutralize Poison", 200L, ev[5]) && found
        && find_effect(cur_node, 0x37, &eff)) {
        if (cur_node->hp == 0)
            cur_node->hp = 1;
        fx_lock = 1;
        fx_remove(cur_node, 0x37, 0);
        fx_remove(cur_node, 0x16, 0);
        fx_remove(cur_node, 0x0f, 0);
        fx_lock = 0;
        cur_node->active = 1;
        cur_node->status = 0;
    }
}

void far remove_curse(unsigned char far *ev)                     /* 354c:087b */
{
    char cursed;
    struct Item far *item;
    struct Effect far *eff;
    char c;

    c = 1;
    cursed = 0;
    item = cur_node->items;
    while ((item != 0) & !cursed) {
        if (item->cursed != 0)
            cursed = 1;
        item = item->next;
    }
    if (!cursed && !find_effect(cur_node, 0x24, &eff))
        c = confirm_anyway(not_cursed_text);
    else
        c = 1;
    if (c == 1 && temple_pay("Remove Curse", 700L, ev[5])) {
        spell_target = cur_node;
        spell_remove_curse();
    }
}

void far stone_flesh(unsigned char far *ev)                      /* 354c:0945 */
{
    char c;

    c = 1;
    if (cur_node->status != 7)
        c = confirm_anyway(not_stoned_text);
    if (c == 1 && temple_pay("Stone to Flesh", 400L, ev[5]) && cur_node->status == 7) {
        cur_node->status = 0;
        cur_node->active = 1;
        cur_node->hp = 1;
    }
}

void far restore_levels(unsigned char far *ev)                   /* 354c:09b0 */
{
    char c;
    char lower;
    short i;

    lower = 0;
    c = 1;
    for (i = 0; i <= 6; i++)
        if (cur_node->level[i] < cur_node->level_max[i])
            lower = 1;
    if (!lower)
        c = confirm_anyway(no_lost_text);
    if (c == 1 && temple_pay("Restore", 1500L, ev[5])) {
        spell_target = cur_node;
        current_spell = 0x66;
        spell_run(0x66);
    }
}

void far temple_resurrect(unsigned char far *ev)                      /* 354c:0a46 */
{
    char c;
    char dead;
    struct Hero far *hero;
    char cannot;
    char gone;

    gone = 0;
    dead = 0;
    cannot = 0;
    c = 1;
    hero = cur_node;
    if (hero->status == 6 || hero->status == 1)
        dead = 1;
    gone = hero->status == 8 || cur_node->con_base == 0;
    if (hero->race < 1)
        cannot = 1;
    if (!dead || cannot) {
        if (cannot)
            c = confirm_anyway(cannot_raise_text);
        else if (gone)
            c = confirm_anyway(beyond_help_text);
        else
            c = confirm_anyway(not_dead_text);
    }
    if (c == 1 && temple_pay("Resurrection", 2000L, ev[5])) {
        spell_target = cur_node;
        current_spell = 0x69;
        spell_run(0x69);
    }
}

void far heal_menu(unsigned char far *ev)                        /* 354c:0b37 */
{
    struct Heal map[12] = {
        { 0x03, 1 }, { 0x25, 3 }, { 0x27, 3 }, { 0x2b, 3 }, { 0x43, 4 }, { 0x3a, 4 },
        { 0x47, 5 }, { 0x4b, 5 }, { 0x24, 6 }, { 0x71, 6 }, { 0x66, 7 }, { 0x69, 7 }
    };
    struct Node far *list;
    struct Node far *sel;
    char redraw;
    unsigned char i;
    int idx;
    char done;
    unsigned char c;
    char num[13];
    unsigned char len;
    char color;
    unsigned char count;

    done = 0;
    list = 0;
    sel = 0;
    count = 0;
    for (i = 0; i <= 12; i++)
        if (map[i].level <= ev[4])
            count++;
    list_alloc(count - 1, &list);
    sel = list;
    for (i = 1; list != 0 && i <= count; i++)
        if (map[i - 1].level <= ev[4]) {
            str_copy(list->text, spell_names[map[i - 1].spell]);
            list->key = 0;
            list = list->next;
        }
    list = sel;
    box_draw();
    redraw = 1;
    frame_lower_panel();
    idx = 0;
    do {
        text_clear(1, 0x13, 0x26, 0x13);
        status_printf(1, 0x13, 0x0b, 0, "%s", cur_node->name);
        str_copy(num, int_str(cur_node->hp));
        len = str_length(num);
        if (cur_node->hp != cur_node->hp_max)
            color = 0x0b;
        else
            color = 7;
        status_printf(0x12, 0x13, color, 0, num);
        str_format(num, "/%s", int_str(cur_node->hp_max));
        status_printf(len + 0x12, 0x13, 7, 0, num);
        status_printf(1, 1, 0x0b, 0, "%s%s", cur_node->name, help_you_text);
        hotkey_reset(1);
        c = list_menu(empty_text, heal_items, 2, 3, 0x26, 0x0f, list, 1, 0,
                      &redraw, &idx, &sel);
        if (menu_escaped) {
            if (c == 0x1b)
                done = 1;
        } else
            switch (c) {
            case 0:
                switch (idx) {
                case 0:
                    heal_points(1, ev);
                    break;
                case 1:
                    cure_blind(ev);
                    break;
                case 2:
                    cure_disease(ev);
                    break;
                case 3:
                    remove_curse(ev);
                    break;
                case 4:
                    cure_poison(ev);
                    break;
                case 5:
                    heal_points(2, ev);
                    break;
                case 6:
                    heal_points(3, ev);
                    break;
                case 7:
                    raise_dead(ev);
                    break;
                case 8:
                    heal_points(4, ev);
                    break;
                case 9:
                    stone_flesh(ev);
                    break;
                case 10:
                    restore_levels(ev);
                    break;
                case 11:
                    temple_resurrect(ev);
                    break;
                case 12:
                    done = 1;
                    break;
                }
                break;
            case 1:
                done = 1;
                break;
            }
    } while (!done);
    list_free(&list);
    view_redraw();
    party_display(cur_node);
}

void far heal_all(unsigned char far *ev)                         /* 354c:0ef7 */
{
    unsigned char i;
    short k;
    struct Hero far *hero;
    short j;
    char lower;
    struct Effect far *eff;

    if (!temple_pay("Fix", 0L, ev[5]))
        return;
    hero = node_list;
    while (hero != 0) {
        fx_remove(hero, 0x44, 0);
        fx_remove(hero, 0x21, 0);
        for (i = 1; i <= 6; i++)
            fx_remove(hero, disease_fx[i], 0);
        if (find_effect(hero, 0x37, &eff)) {
            if (hero->hp == 0)
                hero->hp = 1;
            fx_lock = 1;
            fx_remove(hero, 0x37, 0);
            fx_remove(hero, 0x16, 0);
            fx_remove(hero, 0x0f, 0);
            fx_lock = 0;
            hero->active = 1;
            hero->status = 0;
        }
        if (hero->status == 6 && hero->race != 0) {
            spell_target = hero;
            current_spell = 0x69;
            spell_run(0x69);
        }
        if (hero->status == 7) {
            hero->status = 0;
            hero->active = 1;
        }
        do {
            lower = 0;
            for (j = 0; j <= 6; j++)
                if (hero->level[j] < hero->level_max[j])
                    lower = 1;
            if (lower) {
                spell_target = hero;
                current_spell = 0x66;
                spell_run(0x66);
            }
        } while (lower);
        for (k = 0; k <= 5; k++)
            fx_stat_recalc(hero, k);
        if (hero->status != 8 && hero->status != 6)
            hero->hp = hero->hp_max;
        if (hero->status == 4) {
            hero->status = 0;
            hero->active = 1;
        }
        hero = hero->next;
    }
}

char far temple_shop(unsigned char far *ev, char sub, int a, int b, int c)
                                                                 /* 354c:10fe */
{
    unsigned char key;
    char done;
    unsigned char count;
    char appraised;
    char has_coins;
    char has_items;
    char r;

    r = 0;
    done = 0;
    do {
        pool_check(&has_coins, &has_items);
        count = 0;
        if (!sub)
            hotkey_add(0, &count);
        hotkey_add(1, &count);
        if (!sub && ev[4] >= 7 && ev[5] == 0)
            hotkey_add(2, &count);
        if (sub || ev[5] != 0) {
            hotkey_add(3, &count);
            hotkey_add(4, &count);
        }
        if (has_coins) {
            hotkey_add(5, &count);
            hotkey_add(6, &count);
        }
        if (sub)
            hotkey_add(7, &count);
        hotkey_add(8, &count);
        key = node_prompt(empty_text, temple_items, 1, 0);
        if (menu_escaped) {
            if (key == 0x1b)
                key = 3;
            else {
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
                key = 0xff;
            }
        }
        switch (key) {
        case 0:
            heal_menu(ev);
            break;
        case 1:
            char_view(&menu_escaped);
            break;
        case 2:
            heal_all(ev);
            party_display(cur_node);
            break;
        case 3:
            if (!menu_escaped)
                pool_money();
            break;
        case 4:
            appraise(&appraised);
            view_redraw();
            break;
        case 5:
            take_menu();
            view_redraw();
            break;
        case 6:
            share_money();
            break;
        case 7:
            if (donate(a, b, c))
                r = 1;
            break;
        case 8:
            done = 1;
            break;
        }
    } while (!done);
    return r;
}
