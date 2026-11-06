/* CKIT.EXE overlay 07 module 25e1: changing a human's class and training.
 * change_class lets a conscious human who has not changed class before
 * pick a class his race may take and his abilities and alignment qualify
 * for (15 in the current class's minimums of 9 or more, 17 in the new
 * one's), keeps his current level in old_level and restarts him at level 1
 * in the new class.  train_hero (training) raises cur_node a level
 * in each class the experience pays for and the race's limits by class,
 * level and strength or intelligence allow, provided the hall at
 * cursor_obj + 0x30 teaches at least one of his classes and one of those
 * being raised; free_train waives the fee, the experience, the hall and
 * the race's limits.
 *
 * EXE 0x03f9de..0x0409ec, one Turbo C large-model _TEXT segment starting
 * at byte e of paragraph 25e1 (load-based 35e1), the last module of RTLink
 * section 7 (payload 0x3b900, link segment 21d4, parent section 3; sections
 * 6 and 8 to 10 have the same link segment).  The first e bytes of
 * paragraph 25e1 are the tail of create.c (module 21d4); four zero bytes
 * follow to the section's end at 0x409f0.  Section 7's table files all 76
 * fixup sites in the range under frame 25e1.  Built with Turbo C 2.01,
 * tcc -ml -O -Z -d.  Functions are defined in shipped order; bake consumes
 * them in that order.
 *
 * A bare stored seg:off in this file means section 7.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address.  Callee and variable names are placeholders chosen
 * from use; a modNNNN_ name says only which segment the callee lives in,
 * and a mod18de_ callee is an RTLink vector stub whose target is named
 * beside it.  Section 7 has no child section, so code outside it reaches
 * this module only through vector stubs: 18de:19db -> 06b9, 1a03 -> 061d
 * and 1a0d -> 0154 (party.c's names); create.c far-calls 06b9.  Within the
 * module, change_ok far-calls human_class, and change_class far-calls
 * human_old_class and (twice) human_class, each ahead of its definition; change_class near-calls
 * change_ok after it, so whether change_ok is static the bytes do not say.
 *
 * FRUA's own code: the sharing map classes all five routines a-only.
 * train_hero takes its name from its DQK lineage (create.c's placeholder
 * for it was ovl07_25e1_06b9).  It follows DQK's train_hero (train.c,
 * dqk 893e699, overlay 5 module 2aa7) in its locals and much of its order,
 * but limits levels by a switch on race where DQK reads max_level (-O
 * cross-jumps the cases' identical tails, and case 1's first test must end
 * in break, or the rest of the case sit in its else: without either the
 * module bakes 4 of 5), charges train_cost platinum, runs its
 * best-class pass twice (the second rereads each class's level and drops
 * the experience test), sets no fixed spell bits, and when old_class_abilities_available holds
 * gives effects 0x69 and 8 to a member with old_level[4] or old_level[3]
 * who lacks them.  change_ok's cls must be short (int bakes 4 of 5), the
 * coins unsigned (int changes the size), and the fee taken with -= (the
 * expanded form bakes 4 of 5).
 *
 * _DATA is 16 stored literals, DGROUP 72ae..73e0; -d stores the list
 * title "" as the NUL of the first.  The module has no _BSS.
 */

struct Pool;                    /* strutil.c's slot pool                  */
struct Effect;

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28  index into item_table             */
    char pad_29[9];             /* +29  not read here                     */
    char readied;               /* +32                                    */
    char pad_33;                /* +33  not read here                     */
    char cursed;                /* +34                                    */
};

struct ItemType {               /* 0x10 bytes an item type                */
    char pad_0[0xd];            /* +0   not read here                     */
    unsigned char classes;      /* +d   tested against Hero.classes       */
    char pad_e[2];              /* +e   not read here                     */
};

struct Node {
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct Stat {
    unsigned char base;         /* +0                                     */
    unsigned char cur;          /* +1                                     */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x38];           /* +c   not read here                     */
    long xp;                    /* +44                                    */
    char pad_48[4];             /* +48  not read here                     */
    unsigned coins[3];          /* +4c  platinum, gems, jewelry           */
    char pad_52[6];             /* +52  not read here                     */
    unsigned char race;         /* +58  5 human                           */
    unsigned char cls;          /* +59                                    */
    char pad_5a[3];             /* +5a  not read here                     */
    unsigned char alignment;    /* +5d                                    */
    unsigned char status;       /* +5e                                    */
    char pad_5f;                /* +5f  not read here                     */
    char name[0x10];            /* +60                                    */
    struct Stat stats[6];       /* +70  str int wis dex con cha           */
    char pad_7c[0xd];           /* +7c  not read here                     */
    unsigned char hit_dice;     /* +89                                    */
    unsigned char dual_level;   /* +8a                                    */
    unsigned char thief[8];     /* +8b                                    */
    char pad_93[3];             /* +93  not read here                     */
    unsigned char level_max[7]; /* +96                                    */
    unsigned char level[7];     /* +9d  one per class; 0: not that class  */
    unsigned char old_level[7]; /* +a4  one per class                     */
    unsigned char attack_rate;  /* +ab                                    */
    char pad_ac[0xb];           /* +ac  not read here                     */
    unsigned char class_mask;   /* +b7  sum of class_mask_bits bits              */
    char pad_b8[0xd];           /* +b8  not read here                     */
    char level_up;              /* +c5                                    */
    unsigned char spells[0x8d]; /* +c6                                    */
    unsigned char known[0x10];  /* +153 one bit per spell, from 1         */
    unsigned char slots[3][9];  /* +163 per spell level, by kind          */
};

struct RaceClasses {            /* 0x1b bytes a race                      */
    unsigned char count;        /* +0                                     */
    int classes[13];            /* +1                                     */
};

struct ClassMins {              /* 6 bytes a class                        */
    unsigned char stat_min[6];  /* +0  str, int, wis, dex, con, cha       */
};

struct ClassAligns {            /* 0x15 bytes a class                     */
    unsigned char count;        /* +0                                     */
    int aligns[10];             /* +1                                     */
};

extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern char far can_level_up(struct Hero far *h);                   /* 03d0:1f71 */
extern char far old_class_abilities_available(struct Hero far *h);                   /* 03d0:2100 */
extern unsigned char far first_level(struct Hero far *h);        /* 03d0:212e */
extern long far level_xp_threshold(struct Hero far *h, unsigned char level, short cls);
                                                                 /* 03d0:21cc */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far status_msg(char far *fmt, char fg, char bg, ...); /* 06c9:08f7 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern char far extra_row(void);                                 /* 083a:23b9 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern char far *far ulong_str(unsigned long n);                 /* 0d48:01b6 */
extern unsigned char far spell_menu(unsigned char list, unsigned char type,
                                      int far *choice, char far *more);
                                /* 18de:1305, vector to overlay 12 2b69:203e */
extern void far item_equip_toggle(struct Item far *item);
                                /* 18de:135f, vector to overlay 14 28aa:19bf */
extern void far class_update(struct Hero far *hero);
                                /* 18de:1729, vector to overlay 14 2ccd:05c5 */
extern void far saving_throws(struct Hero far *hero);
                                /* 18de:173d, vector to overlay 14 2ccd:0a0f */
extern void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove);
                                /* 18de:19bd, vector to overlay 19 2e88:1648 */
extern void far cleric_slots(struct Hero far *h);
                                /* 18de:19c7, vector to overlay 14 2ccd:000d */
extern void far hp_adjust(struct Hero far *hero, char classes, char count, char c);
                                /* 18de:19e5, vector to overlay 14 28aa:3cf1 */
extern void far thief_skills(struct Hero far *h);
                                /* 18de:19ef, vector to overlay 14 2ccd:0898 */
extern char far is_human(struct Hero far *hero);
                                /* 18de:1a17, vector to overlay 14 2ccd:0c1f */
extern void far mu_slot_cap(struct Hero far *h);
                                /* 18de:1a21, vector to overlay 14 2ccd:0155 */

extern struct RaceClasses race_classes[];                        /* 1032 */
extern struct ClassMins class_mins[];                            /* 11b8 */
extern struct ClassAligns class_aligns[];                        /* 121e */
extern char far *class_names[];                                  /* 3914 */
extern unsigned char class_bit[7];                               /* 6252 */
extern long train_cost;                                          /* 671e */
extern unsigned char bit_mask[8];                                /* 6711 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern struct ItemType far *item_table;                          /* b40a */
extern char training_done;                                           /* b722 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char train_costs_waived;                                           /* c39a */
extern char free_train;                                          /* c524 */
extern char menu_escaped;                                        /* ca2a */
extern void far *node_pool;                                      /* ce5d */

int far human_old_class(struct Hero far *h);
int far human_class(struct Hero far *h);

char far change_ok(short cls, struct Hero far *h)                /* 35e1:000e */
{
    char ok;
    short i;
    unsigned char j;

    ok = human_class(h) != cls;
    for (i = 0; i <= 5 && (class_mins[human_class(h)].stat_min[i] < 9 ||
                           h->stats[i].base > 14); i++)
        ;
    ok = ok && i > 5;
    for (i = 0; i <= 5 && (class_mins[cls].stat_min[i] < 9 || h->stats[i].base > 16); i++)
        ;
    ok = ok && i > 5;
    for (j = 0; class_aligns[cls].count > j && class_aligns[cls].aligns[j] != h->alignment; j++)
        ;
    ok = ok && class_aligns[cls].count > j;
    return ok;
}

int far change_class(struct Hero far *h)                         /* 35e1:0154 */
{
    char result;
    short i;
    char choice;
    unsigned char count;
    struct Node far *head;
    struct Node far *cur;
    int top;
    char mode;
    char redraw;
    struct Item far *item;

    result = 0x11;
    if (h->status != 0) {
        status_msg("only conscious humans may change", 0xb, 0);
        return 0x11;
    }
    if (!is_human(h) || human_old_class(h) != 0x11) {
        status_msg("%s%s", 0xb, 0, h->name, " doesn't qualify.");
        return 0x11;
    }
    count = race_classes[h->race].count;
    pool_alloc((void far * far *)&head, sizeof(struct Node), node_pool);
    cur = head;
    cur->next = 0;
    cur->key = 1;
    str_copy(cur->text, "Pick New Class");
    top = 1;
    for (tick_slot = 1; tick_slot <= count; tick_slot++) {
        i = race_classes[h->race].classes[tick_slot - 1];
        if (change_ok(i, h)) {
            pool_alloc((void far * far *)cur, sizeof(struct Node), node_pool);
            cur = cur->next;
            cur->next = 0;
            cur->key = 0;
            str_copy(cur->text, class_names[i]);
            top++;
        }
    }
    if (top == 1) {
        status_msg("%s%s", 0xb, 8, h->name, " doesn't qualify.");
        list_free(&head);
        return 0x11;
    }
    cur = head;
    top = 0;
    mode = 1;
    redraw = 1;
    do {
        hotkey_reset(1);
        choice = list_menu("", "Select Exit", 1, 2, 0x26, 0x16, head, mode, 0,
                           &redraw, &top, &cur);
        if (menu_escaped && choice == 0x1b || choice == 1)
            return 0x11;
    } while (choice != 0);
    h->xp = 0;
    h->level_up = 0;
    h->attack_rate = 2;
    for (i = 0; i <= 6 && !str_same(class_names[i], cur->text); i++)
        ;
    list_free(&head);
    h->old_level[human_class(h)] = first_level(h);
    h->dual_level = h->hit_dice;
    h->hit_dice = 1;
    h->level[human_class(h)] = 0;
    h->level[i] = 1;
    mem_fill(h->slots, 0x1b, 0);
    mem_zero(h->level_max, 7);
    mem_zero(h->spells, 0x8d);
    switch (i) {
    case 0:
        h->slots[0][0] = 1;
        break;
    case 5:
        h->slots[2][0] = 1;
        h->known[1] = h->known[1] | bit_mask[2];
        h->known[2] = h->known[2] | bit_mask[1];
        h->known[1] = h->known[1] | bit_mask[3];
        h->known[2] = h->known[2] | bit_mask[4];
        break;
    }
    h->cls = i;
    box_draw();
    result = i;
    status_msg("%s%s%s.", 0xf, 0, h->name, " becomes a ", class_names[i]);
    mem_fill(h->spells, 0x8d, 0);
    class_update(h);
    cleric_slots(h);
    mu_slot_cap(h);
    saving_throws(h);
    if (h->level[6] > 0)
        thief_skills(h);
    else
        mem_fill(h->thief, 8, 0);
    for (item = h->items; item != 0; item = item->next)
        if (!(item_table[item->type].classes & h->class_mask) && !item->cursed && item->readied)
            item_equip_toggle(item);
    return result;
}

int far human_old_class(struct Hero far *h)                      /* 35e1:061d */
{
    short r;
    short i;

    r = 0x11;
    if (h->race == 5) {
        for (i = 0; i < 6 && h->old_level[i] == 0; i++)
            ;
        if (h->old_level[i] > 0)
            r = i;
    }
    return r;
}

int far human_class(struct Hero far *h)                          /* 35e1:066b */
{
    short r;
    short i;

    r = 0x11;
    if (h->race == 5) {
        for (i = 0; i < 6 && h->level[i] == 0; i++)
            ;
        if (h->level[i] > 0)
            r = i;
    }
    return r;
}

void far train_hero(void)                                   /* 35e1:06b9 */
{
    unsigned char blocked;
    long cap;
    long next;
    unsigned char train;
    unsigned char has;
    unsigned char count;
    short i;
    unsigned char row;
    unsigned char was5;
    unsigned char lvl;
    unsigned char pick;
    int choice;
    char more;
    long best_xp;
    short best;
    unsigned char hall;
    struct Effect far *fx;

    if (cur_node->status != 0) {
        status_msg("we only train conscious people", 0xb, 0);
        return;
    }
    if (cur_node->coins[0] < train_cost && !free_train && !extra_row() && !train_costs_waived) {
        status_msg(msg_format("Training costs %s platinum.", ulong_str(train_cost)), 0xb, 0);
        return;
    }
    train = 0;
    has = 0;
    cap = 0;
    hall = cursor_obj[0x30];
    cap = 0;
    for (i = 0; i <= 6; i++) {
        if (cur_node->level[i] > 0) {
            has += class_bit[i];
            blocked = 0;
            lvl = cur_node->level[i];
            switch (cur_node->race) {
            case 2:
                if (i == 2 && (lvl == 9 || lvl == 8 && cur_node->stats[0].base == 17 ||
                               lvl == 7 && cur_node->stats[0].base < 17))
                    blocked = 1;
                break;
            case 0:
                if (i == 2 && (lvl == 7 || lvl == 6 && cur_node->stats[0].base == 17 ||
                               lvl == 5 && cur_node->stats[0].base < 17))
                    blocked = 1;
                if (i == 5 && (lvl == 11 || lvl == 9 && cur_node->stats[1].base < 17 ||
                               lvl == 10 && cur_node->stats[1].base == 17))
                    blocked = 1;
                break;
            case 3:
                if (i == 2 && (lvl == 6 || lvl == 5 && cur_node->stats[0].base < 18))
                    blocked = 1;
                if (i == 0 && lvl >= 7)
                    blocked = 1;
                break;
            case 1:
                if (i == 0 && lvl == 5) {
                    blocked = 1;
                    break;
                }
                if ((i == 2 || i == 4) &&
                    (lvl == 8 || lvl == 7 && cur_node->stats[0].base == 17 ||
                     lvl == 6 && cur_node->stats[0].base < 17))
                    blocked = 1;
                if (i == 5 && (lvl == 8 || lvl == 7 && cur_node->stats[1].base == 17 ||
                               lvl == 6 && cur_node->stats[1].base < 17))
                    blocked = 1;
                break;
            case 4:
                if (i == 2 && (lvl == 6 || lvl == 5 && cur_node->stats[0].base == 17 ||
                               lvl == 4 && cur_node->stats[0].base < 17))
                    blocked = 1;
                break;
            }
            if (lvl >= 40)
                blocked = 1;
            if ((blocked || level_xp_threshold(cur_node, lvl + 1, i) > cur_node->xp) && !free_train)
                continue;
            train += class_bit[i];
            next = level_xp_threshold(cur_node, lvl + 2, i);
            if (cur_node->xp >= next && next > cap)
                cap = next - 1;
        }
    }
    if (!extra_row()) {
        best_xp = 0;
        next = 0;
        for (i = 0; i <= 6; i++) {
            if ((char)(class_bit[i] & train)) {
                if (level_xp_threshold(cur_node, lvl + 1, i) > best_xp) {
                    best_xp = level_xp_threshold(cur_node, lvl + 1, i);
                    best = i;
                }
            }
        }
        if (best_xp > 0) {
            train = class_bit[best] | train;
            next = level_xp_threshold(cur_node, lvl + 2, best);
            if (next > 0 && cur_node->xp >= next && next > cap)
                cap = next - 1;
        }
    }
    if (!extra_row()) {
        best_xp = 0;
        next = 0;
        for (i = 0; i <= 6; i++) {
            lvl = cur_node->level[i];
            if ((char)(class_bit[i] & train)) {
                if (level_xp_threshold(cur_node, lvl + 1, i) > best_xp) {
                    best_xp = level_xp_threshold(cur_node, lvl + 1, i);
                    best = i;
                }
            }
        }
        if (best_xp > 0) {
            train = class_bit[best] | train;
            lvl = cur_node->level[best];
            next = level_xp_threshold(cur_node, lvl + 2, best);
            if (next > 0 && next > cap)
                cap = next - 1;
        }
    }
    if (cap > 0 && !extra_row() && cur_node->xp > cap)
        cur_node->xp = cap;
    if (!free_train) {
        if (!(char)(has & hall) && !extra_row()) {
            status_msg("we don't train that class here", 0xb, 0);
            return;
        }
        if (!(train & hall)) {
            training_done = 1;
            cur_node->level_up = can_level_up(cur_node);
            if (extra_row())
                return;
            status_msg("Not Enough Experience", 0xb, 0);
            return;
        }
    }
    if (!extra_row()) {
        text_clear(1, 1, 0x26, 0x16);
        row = 4;
        character_name_show(cur_node, 4, row, 0);
        status_printf(str_length(cur_node->name) + 4, row, 7, 0, " will become:");
        for (i = 0; i <= 6; i++) {
            if (cur_node->level[i] > 0 && (class_bit[i] & train)) {
                row++;
                if (row == 5)
                    status_printf(6, row, 7, 0,
                                  msg_format("    a level %s %s",
                                             byte_str(cur_node->level[i] + 1),
                                             class_names[i]));
                else
                    status_printf(6, row, 7, 0,
                                  msg_format("and a level %s %s",
                                             byte_str(cur_node->level[i] + 1),
                                             class_names[i]));
            }
        }
        if (!confirm("Do you wish to train? ", 1))
            return;
        bottom_msg("Congratulations...");
        if (!free_train && !train_costs_waived)
            cur_node->coins[0] -= train_cost;
    }
    count = 0;
    was5 = cur_node->level[5];
    cur_node->class_mask = 0;
    for (i = 0; i <= 6; i++) {
        if (cur_node->level[i] > 0) {
            count++;
            if (class_bit[i] & train)
                cur_node->level[i]++;
        }
    }
    class_update(cur_node);
    if (!extra_row()) {
        if (cur_node->level[5] > was5 || cur_node->level[4] > 8) {
            choice = -1;
            do {
                pick = spell_menu(4, 4, &choice, &more);
            } while (pick <= 0 && more != 0);
            if (pick > 0)
                cur_node->known[(pick - 1) / 8] =
                    cur_node->known[(pick - 1) / 8] | bit_mask[(pick - 1) % 8];
        }
        if (old_class_abilities_available(cur_node)) {
            if (cur_node->old_level[4] > 0 && !find_effect(cur_node, 0x69, &fx))
                fx_add(cur_node, 0x69, 0, 0xff, 0);
            if (cur_node->old_level[3] > 0 && !find_effect(cur_node, 8, &fx))
                fx_add(cur_node, 8, 0, 0xff, 0);
        }
    }
    if (cur_node->hit_dice > cur_node->dual_level)
        hp_adjust(cur_node, train, count, 1);
    cur_node->level_up = can_level_up(cur_node);
}
