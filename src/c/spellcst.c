/* CKIT.EXE overlay 12 module 2b69: choosing and casting spells.  The spell
 * lists -- memorized, in the grimoire, to memorize or scribe, the spells an
 * item holds, and the spells a member's classes could know -- built into a
 * menu with a level heading above each run, the spell menu itself, and
 * casting: whether a spell can be cast here, choosing its targets, rolling
 * each target's save and handing the effect on, the targets along a line or
 * cone, and calling the spell's handler through spell_fn.
 *
 * EXE 0x05971c..0x05b8a1, one Turbo C large-model _TEXT segment starting at
 * byte 0xc of paragraph 2b69, the second and last module of RTLink section
 * 12 (payload 0x54eb0, link segment 26e3, which sections 11 and 13 also
 * use).  Its first 12 bytes are spellfx.c's tail; after its last retf the
 * section holds 15 zero bytes to its initialized end, 0x5b8b0.  Built with
 * Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order;
 * bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 12.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a mod18de_ callee is an RTLink
 * vector stub, and its comment names the routine the stub reaches.  The
 * ovl12_2b69_ functions keep the names spellfx.c calls them by.  Vector
 * stubs reach spell_list_build (18de:1ad5), spell_range (12fb),
 * camp_spell_targets (10f3), spell_cast (1175), spell_path_targets (1ca1),
 * spell_menu (1305) and spell_run (1ae9); spellfx.c far-calls 10d7, 11b5,
 * 1a61 and 1e6f, and its spellfx_init stores 13cb in hit_proc.
 *
 * This is Dark Queen of Krynn's spellcst.c relinked, with FRUA's record
 * layouts and the changes the bytes show: no turn_checks and no
 * spell_memorized; spell_usable returns 1 at once when test_mode is 5, has
 * no case 3, and tests other classes and levels in cases 0 to 2;
 * spell_choose resets the hot keys in each case and has a type 6; items_add
 * makes no class test on an item's flags; list 1 needs a slot at the
 * spell's kind and level; list 4 walks spell_info for the magic-user spells
 * a member does not know at a level it has a slot for, and list 12 is new,
 * shown "in Grimoire"; spell_cast ends with box_draw_top; and spell_damage_apply
 * and spell_effect_apply make no effect-0x96 test, nor 11b5 a status-10 test.
 *
 * _DATA is level_names, the four step tables and 15 stored literals, DGROUP
 * 8656..8898; -d stores "" as the NUL of "Exit".  _BSS is list_head,
 * item_ptrs, spell_ids, item_count and spell_counts, d218..d412.
 *
 * targets is indexed from 1; hits, heroes and steps are too, and the
 * shipped code adds the address of element 0.  spell_path_targets's scans use
 * the global tick_slot as their loop counter.
 */

struct Item {                   /* an item a party member carries         */
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28  0x49: spells are in more's chain  */
    char pad_29[0xa];           /* +29  not read here                     */
    unsigned char unid_mask;    /* +33  bits 0-2: not castable            */
    char pad_34;                /* +34  not read here                     */
    unsigned char slots[4];     /* +35  [1..3] spell ids; bit 7: marked   */
    char pad_39;                /* +39  not read here                     */
    struct Item far *more;      /* +3a                                    */
};

struct ItemType {               /* 0x10 bytes an item type                */
    unsigned char slot;         /* +0   item_add tests 0xc                */
    char pad_1[0xf];            /* +1   not read here                     */
};

struct Fight {                  /* a combatant's state in a fight         */
    char pad_0[0xa];            /* +0   not read here                     */
    unsigned char facing;       /* +a                                     */
};

struct Hero {                   /* a party member or a monster            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[8];              /* +c   not read here                     */
    void far *armor;            /* +14                                    */
    char pad_18[0x28];          /* +18  not read here                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x14];          /* +44  not read here                     */
    unsigned char race;         /* +58                                    */
    char pad_59[6];             /* +59  not read here                     */
    unsigned char side;         /* +5f                                    */
    char name[0x11];            /* +60                                    */
    char pad_71[2];             /* +71  not read here                     */
    unsigned char intel;        /* +73                                    */
    char pad_74;                /* +74  not read here                     */
    unsigned char wis;          /* +75                                    */
    char pad_76[0x27];          /* +76  not read here                     */
    unsigned char level[7];     /* +9d  one per class                     */
    char pad_a4[0x1b];          /* +a4  not read here                     */
    unsigned char type_flags;   /* +bf                                    */
    char pad_c0[6];             /* +c0  not read here                     */
    unsigned char spells[0x8d]; /* +c6  bit 7: still to memorize          */
    unsigned char known[0xf];   /* +153 one bit per spell, from 1         */
    unsigned char slots_by_level[3][9]; /* +162 stride 9: level 9 is the next row */
    char pad_17d[4];            /* +17d not read here                     */
    unsigned char ac;           /* +181                                   */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0   0 cleric, 1 druid, 2 magic-user   */
    unsigned char level;        /* +2                                     */
    unsigned char range;        /* +3                                     */
    unsigned char range_per_level; /* +4                                     */
    unsigned char duration;     /* +5                                     */
    unsigned char duration_per_level; /* +6                                     */
    unsigned char targeting;    /* +7                                     */
    int target;                 /* +8   1 caster, 2 one chosen, 3 party   */
    int save_effect;            /* +a   0: no saving throw                */
    int save_type;              /* +c                                     */
    unsigned char effect;       /* +e                                     */
    char pad_f[7];              /* +f   not read here                     */
};

struct LineWalk {               /* a line stepper, section 2's            */
    int x0;                     /* +0                                     */
    int y0;                     /* +2                                     */
    int x1;                     /* +4                                     */
    int y1;                     /* +6                                     */
    int err;                    /* +8   not read here                     */
    int dx;                     /* +a   not read here                     */
    int dy;                     /* +c   not read here                     */
    int x;                      /* +e                                     */
    int y;                      /* +10                                    */
    int sx;                     /* +12  not read here                     */
    int sy;                     /* +14  not read here                     */
    unsigned char steps;        /* +16                                    */
    char dir;                   /* +17                                    */
};

struct Step {
    unsigned char who;          /* +0   index into heroes                 */
    unsigned char cost;         /* +1   not read here                     */
    unsigned char dir;          /* +2   not read here                     */
};

struct Effect;                  /* hero.c's                               */

struct Pool;                    /* strutil.c's                            */

struct Node {
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far spell_mark(struct Hero far *h, unsigned char spell); /* 03d0:1381 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern unsigned char far caster_level(unsigned char spell, char cap); /* 03d0:18fe */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw_top(void);                                 /* 083a:03ea */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_list_panel(void);                                /* 0d65:0302 */
extern void far fx_trigger_group(unsigned char when, struct Hero far * far *hp);
                                /* 18de:1111, vector to overlay 19 2e88:0449 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern char far combatant_x(struct Hero far *h);
                                /* 18de:117f, vector to overlay 0 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h);
                                /* 18de:1189, vector to overlay 0 22fa:0ae4 */
extern char far combatant_visible(struct Hero far *h, char whole);
                                /* 18de:119d, vector to overlay 0 22fa:0627 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame, char erase);
                                /* 18de:11cf, vector to overlay 0 22fa:09a2 */
extern void far icon_scatter(char a, char b, char c);
                                /* 18de:120b, vector to overlay 0 1cb8:0d53 */
extern void far icon_fly(int x0, int y0, int x1, int y1, char a, char b);
                                /* 18de:121f, vector to overlay 0 1cb8:00de */
extern void far missile_facing(char who, char n);
                                /* 18de:1251, vector to overlay 0 1cb8:0f2c */
extern void far icon_facings(char n);
                                /* 18de:125b, vector to overlay 0 1cb8:0052 */
extern char far line_step(struct LineWalk far *l);
                                /* 18de:1265, vector to overlay 2 2632:020c */
extern void far line_init(struct LineWalk far *l);
                                /* 18de:126f, vector to overlay 2 2632:01a0 */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern char far dir_ok(char x0, char y0, char x1, char y1, unsigned char who);
                                /* 18de:12bf, vector to overlay 2 2632:048c */
extern void far fx_invisibility_remove(struct Hero far *h);
                                /* 18de:12dd, vector to overlay 19 2e88:17b0 */
extern char far fx_attack_roll(struct Hero far *h, struct Hero far *t, unsigned char need);
                                /* 18de:12f1, vector to overlay 19 2e88:143d */
extern char far line_clear(char x0, char y0, char far *x1, char far *y1,
                             unsigned short far *power);
                                /* 18de:130f, vector to overlay 2 2632:0325 */
extern void far cell_get(char x, char y, unsigned char far *who,
                             unsigned char far *kind);
                                /* 18de:1319, vector to overlay 0 22fa:0469 */
extern char far fx_saving_throw(struct Hero far *h, int type, char adj);
                                /* 18de:1323, vector to overlay 19 2e88:1552 */
extern void far fx_damage(struct Hero far *h, unsigned char dmg, int reduce,
                             char saved);
                                /* 18de:132d, vector to overlay 19 2e88:1ea2 */
extern void far pick_hero(char far *prompt, char a, char b, struct Hero far * far *out);
                                /* 18de:1a85, vector to overlay 14 28aa:3fb9 */
extern void far fx_add_message(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove, int save_negates,
                             char saved, char far *msg);
                                /* 18de:1c97, vector to overlay 19 2e88:2181 */
extern char far item_casts(struct Item far *it);                 /* 26e3:4055 */
extern void far cast_announce(struct Hero far *h, char far *text, unsigned char spell);
                                                                 /* 26e3:4235 */
void far spell_run(unsigned char spell);

extern char far *spell_names[];                                  /* 2434 */
extern struct SpellInfo spell_info[];                            /* 264a */
extern char far *txt_lose_it;                                    /* 39e4 */
extern char far *txt_cant_cast_here;                             /* 39e8 */
extern char far *txt_that_item;                                  /* 39ec */
extern char far *txt_combat_only;                                /* 39f0 */
extern char far *txt_use_it;                                     /* 39f4 */
extern char far *txt_casts;                                      /* 39f8 */
extern char far *txt_abort_spell;                                /* 39fc */
extern char far *txt_spell_aborted;                              /* 3a00 */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *txt_choose_spell;                               /* 3bcc */
extern char far *txt_spells_ok;                                  /* 3bf4 */
extern char far *txt_scribe_ok;                                  /* 3bf8 */
extern char far *txt_cast_exit;                                  /* 3c30 */
extern char far *txt_memorize_exit;                              /* 3c34 */
extern char far *txt_keep_exit;                                  /* 3c38 */
extern char far *txt_scribe_exit;                                /* 3c3c */
extern char far *txt_learn_exit;                                 /* 3c40 */
extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern unsigned char bit_mask[8];                                /* 6711 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern struct ItemType far *item_table;                          /* b40a */
extern int view_mode;                                            /* b421 */
extern void (far *spell_fn[])(void);                             /* b4f0 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Hero far *heroes[];                                /* b8f5 */
extern struct Item far *item_cur;                                /* bb8c */
extern unsigned char current_spell;                                  /* be64 */
extern char item_cast_active;                                           /* be69 */
extern char view_cx;                                             /* c39f */
extern char view_cy;                                             /* c3bc */
extern int element_mask;                                         /* c64e */
extern char mirror_image_bypass;                                           /* ca08 */
extern unsigned char step_count;                                 /* ca10 */
extern char pick_special;                                        /* ca29 */
extern struct Step steps[];                                      /* ca2b */
extern struct Hero far *selected_target;                               /* cc6f */
extern char move_flag;                                           /* cc79 */
extern char test_mode;                                           /* cc7a */
extern void far *node_pool;                                      /* ce5d */
extern struct Hero far *targets[];                               /* cfe3 */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern void (far *hit_proc)(unsigned char spell, char mode, char far *result); /* d131 */
extern unsigned char target_count;                               /* d137 */

static char level_names[10][41] = {                              /* 8656 */
    "Special", "1st Level", "2nd Level", "3rd Level", "4th Level",
    "5th Level", "6th Level", "7th Level", "8th Level", "9th Level"
};
static char near_dx[8] = { -1, 0, 0, 1, 1, 0, 0, -1 };           /* 87f0 */
static char near_dy[8] = { 0, -1, -1, 0, 0, 1, 1, 0 };           /* 87f8 */
static char far_dx[8] = { 1, 1, 0, 0, -1, -1, 0, 0 };            /* 8800 */
static char far_dy[8] = { 0, 0, 1, 1, 0, 0, -1, -1 };            /* 8808 */

static struct Node far *list_head;                               /* d218 */
static struct Item far *item_ptrs[51];                           /* d21c */
static unsigned char spell_ids[0x93];                            /* d2e8 */
static unsigned char item_count;                                 /* d37b */
static unsigned char spell_counts[0x97];                         /* d37c */

char far spell_usable(struct Hero far *h, unsigned char spell, char flag)
                                                                 /* 3b69:000c */
{
    char r;

    if (test_mode == 5)
        return 1;
    spell = spell & 0x7f;
    r = 0;
    switch (spell_info[spell].kind) {
    case 0:
        r = (char)(h->wis > 8) && (class_level(h, 0) > 0 || class_level(h, 3) > 8);
        break;
    case 1:
        r = (char)(h->wis > 8) && class_level(h, 4) > 0;
        break;
    case 2:
        if (h->intel > 8) {
            if (class_level(h, 5) > 0 && !(h->race == 5 && h->armor != 0 && view_mode == 5))
                r = 1;
            if (class_level(h, 4) > 8)
                r = 1;
            if (flag && class_level(h, 6) > 10)
                r = 1;
        }
        break;
    case 4:
        r = 0;
        break;
    }
    return r;
}

char far spell_choose(unsigned char type, int far *index)       /* 3b69:0166 */
{
    char result;
    struct Node far *picked;
    struct Node far *node;
    char title[42];
    char text[42];
    short count;
    char rows;
    char key;
    char can_exit;
    char redraw;
    char fkeys;

    str_copy(title, txt_choose_spell);
    rows = 0x16;
    fkeys = 0;
    redraw = 0;
    ui_mode = 1;
    switch (type) {
    case 1:
        hotkey_reset(1);
        str_copy(text, txt_cast_exit);
        break;
    case 2:
        hotkey_reset(1);
        str_copy(text, txt_memorize_exit);
        break;
    case 3:
        hotkey_reset(1);
        str_copy(text, txt_scribe_exit);
        break;
    case 4:
        hotkey_reset(0);
        str_copy(text, txt_learn_exit);
        break;
    case 6:
        hotkey_reset(1);
        str_copy(text, txt_learn_exit);
        break;
    case 8:
        hotkey_reset(1);
        str_copy(title, txt_spells_ok);
        str_copy(text, txt_keep_exit);
        break;
    case 9:
        hotkey_reset(1);
        str_copy(title, txt_scribe_ok);
        str_copy(text, txt_keep_exit);
        break;
    default:
        fkeys = -1;
        hotkey_reset(0);
        str_copy(title, empty_text);
        str_copy(text, "Exit");
        break;
    }
    if (type == 2) {
        rows = 0xf;
        redraw = pick_special;
    } else
        rows = 0x16;
    can_exit = type != 4;
    if (*index < 0) {
        redraw = 1;
        *index = 0;
    }
    if (type == 4 || type == 1)
        redraw = 1;
    picked = 0;
    do
        key = list_menu(title, text, 1, 5, 0x26, rows, list_head, can_exit, fkeys,
                        &redraw, index, &picked);
    while (key != 0 && key != 1 && key != 0x1b);
    node = list_head;
    count = 0;
    while (node != picked) {
        if (node->key == 0)
            count++;
        node = node->next;
    }
    if (key != 0)
        result = 0;
    else
        result = spell_ids[count];
    if (type == 3)
        item_cur = item_ptrs[count];
    list_free(&list_head);
    return result;
}

void far list_add(unsigned char spell)                           /* 3b69:03f2 */
{
    struct Node far *node;
    unsigned short count;
    unsigned char level;

    node = list_head;
    if (list_head == 0) {
        pool_alloc(&node, 0x28, node_pool);
        list_head = node;
        level = 0;
        count = 0;
    } else {
        count = 1;
        while (node->next != 0) {
            if (node->key == 0)
                count++;
            node = node->next;
        }
        level = spell_info[spell_ids[count - 1]].level;
        pool_alloc(node, 0x28, node_pool);
        node = node->next;
    }
    node->next = 0;
    if (spell_info[spell & 0x7f].level != level) {
        str_copy(node->text, level_names[spell_info[spell & 0x7f].level]);
        node->key = 1;
        pool_alloc(node, 0x28, node_pool);
        node = node->next;
        node->next = 0;
    }
    if (spell & 0x80)
        str_copy(node->text, " *");
    else
        str_copy(node->text, "\t");
    str_copy(node->text, msg_format("%s%s", node->text, spell_names[spell & 0x7f]));
    node->key = 0;
    spell_ids[count] = spell & 0x7f;
}

void far list_add_count(unsigned char spell)                     /* 3b69:05d8 */
{
    unsigned char saved;
    unsigned char swap;
    struct Node far *node;
    struct Node far *scan;
    struct Node far *added;
    unsigned short i;
    char found;
    unsigned char level;
    register int pos;

    node = list_head;
    found = 0;
    level = spell_info[spell & 0x7f].level;
    if (list_head == 0) {
        mem_zero(spell_counts, 0x97);
        pool_alloc(&node, 0x28, node_pool);
        node->next = 0;
        list_head = node;
        pos = 0;
        spell_counts[pos] = 1;
    } else {
        pos = 0;
        while (node != 0 && !found) {
            if (node->key == 0) {
                if (spell_info[spell_ids[pos]].level <= level && spell_ids[pos] != (spell & 0x7f)) {
                    pos++;
                    node = node->next;
                } else
                    found = 1;
            } else {
                node = node->next;
                pos++;
            }
        }
        if (spell_ids[pos] != (spell & 0x7f)) {
            scan = list_head;
            if (scan != node) {
                while (scan->next != node)
                    scan = scan->next;
                pool_alloc(&added, 0x28, node_pool);
                added->next = scan->next;
                scan->next = added;
            } else {
                pool_alloc(&added, 0x28, node_pool);
                added->next = node;
                list_head = added;
            }
            node = added;
            saved = 1;
            for (i = pos; i <= 0x8c; i++) {
                swap = spell_counts[i];
                spell_counts[i] = saved;
                saved = swap;
            }
        } else
            spell_counts[pos]++;
    }
    if (spell & 0x80)
        str_copy(node->text, " *");
    else
        str_copy(node->text, "\t");
    str_copy(node->text, msg_format("%s%s", node->text, spell_names[spell & 0x7f]));
    node->key = 0;
    if (spell_counts[pos] > 1)
        str_copy(node->text, msg_format("%s (%s)", node->text, byte_str(spell_counts[pos])));
    if (spell_ids[pos] != (spell & 0x7f)) {
        saved = spell_ids[pos];
        spell_ids[pos] = spell & 0x7f;
        for (i = pos + 1; i <= 0x86; i++) {
            swap = spell_ids[i];
            spell_ids[i] = saved;
            saved = swap;
        }
    }
}

void far item_add(char marked, char check)                       /* 3b69:08d7 */
{
    unsigned char i;
    struct Effect far *fx;

    if (check && item_table[item_cur->type].slot == 0xc)
        return;
    if (find_effect(cur_node, 0x10, &fx)
        || class_level(cur_node, 0) > 0 && item_table[item_cur->type].slot == 0xc)
        item_cur->unid_mask = item_cur->unid_mask & 0xf8;
    if ((item_cur->unid_mask & 7) != 0)
        return;
    for (i = 1; i <= 3; i++) {
        if (marked && item_cur->slots[i] > 0x80 || !marked && item_cur->slots[i] > 0) {
            if (spell_usable(cur_node, item_cur->slots[i], 1)) {
                list_add(item_cur->slots[i]);
                item_ptrs[item_count] = item_cur;
                item_count++;
            }
        }
    }
}

void far bundle_add(char marked)                                 /* 3b69:0a08 */
{
    register int i;

    for (i = 0; i <= 0x32; i++)
        item_ptrs[i] = 0;
    item_count = 0;
    item_cur = item_cur->more;
    while (item_cur != 0) {
        item_add(marked, 0);
        item_cur = item_cur->more;
    }
}

void far items_add(char marked, char check)                      /* 3b69:0a56 */
{
    struct Item far *bundle;
    register int i;

    for (i = 0; i <= 0x32; i++)
        item_ptrs[i] = 0;
    item_count = 0;
    item_cur = cur_node->items;
    while (item_cur != 0) {
        if (item_casts(item_cur)) {
            if (item_cur->type == 0x49) {
                bundle = item_cur;
                item_cur = item_cur->more;
                while (item_cur != 0) {
                    item_add(marked, check);
                    item_cur = item_cur->more;
                }
                item_cur = bundle;
            } else
                item_add(marked, check);
        }
        item_cur = item_cur->next;
    }
}

char far spell_list_build(unsigned char list)                    /* 3b69:0b13 */
{
    char result;
    unsigned char spell;
    unsigned short i;
    struct Node far *cur;
    struct Node far *node;
    unsigned char prev;
    unsigned char level;
    char headers;

    headers = 1;
    result = 0;
    list_head = 0;
    mem_zero(spell_ids, 0x93);
    switch (list) {
    case 0:
        for (i = 0; i <= 0x8c; i++) {
            if (cur_node->spells[i] > 0 && spell_usable(cur_node, cur_node->spells[i] & 0x7f, 0)
                && cur_node->spells[i] < 0x80)
                list_add_count(cur_node->spells[i]);
        }
        break;
    case 5:
        for (i = 0; i <= 0x8c; i++) {
            if ((cur_node->spells[i] & 0x80)
                && spell_usable(cur_node, cur_node->spells[i] & 0x7f, 0))
                list_add_count(cur_node->spells[i]);
        }
        break;
    case 1:
        for (i = 1; i <= 0x7e; i++) {
            if ((cur_node->known[(i - 1) >> 3] & bit_mask[(i - 1) & 7])
                && cur_node->slots_by_level[spell_info[i].kind][spell_info[i].level] > 0
                && spell_usable(cur_node, i, 0))
                list_add_count(i);
        }
        break;
    case 2:
        item_count = 0;
        item_add(0, 0);
        headers = 0;
        break;
    case 7:
        bundle_add(0);
        headers = 0;
        break;
    case 3:
        items_add(0, 1);
        headers = 0;
        break;
    case 6:
        items_add(1, 1);
        headers = 0;
        break;
    case 4:
        for (spell = 1; spell <= 0x7e; spell++) {
            level = spell_info[spell].level;
            if (cur_node->slots_by_level[2][level] > 0 && spell_info[spell].kind == 2
                && !(cur_node->known[(spell - 1) / 8] & bit_mask[(spell - 1) % 8]))
                list_add_count(spell);
        }
        break;
    case 12:
        for (spell = 1; spell <= 0x7e; spell++) {
            switch (spell) {
            case 0x39:
            case 0x3b:
            case 0x3c:
            case 0x3d:
            case 0x3e:
            case 0x3f:
            case 0x40:
            case 0x41:
            case 0x61:
            case 0x63:
            case 0x7e:
                break;
            default:
                if (spell_info[spell].kind == 2 && (cur_node->level[5] != 0 || cur_node->level[4] != 0)
                    || spell_info[spell].kind == 0 && (cur_node->level[0] != 0 || cur_node->level[3] != 0)
                    || spell_info[spell].kind == 1 && cur_node->level[4] != 0)
                    list_add_count(spell);
                break;
            }
        }
        break;
    }
    if (list_head != 0) {
        if (headers) {
            i = 0;
            level = spell_info[spell_ids[i]].level;
            pool_alloc(&node, 0x28, node_pool);
            node->next = list_head;
            list_head = node;
            str_copy(list_head->text, level_names[spell_info[spell_ids[i]].level]);
            list_head->key = 1;
            cur = list_head;
            do {
                prev = level;
                if (spell_ids[i] != 0)
                    level = spell_info[spell_ids[i]].level;
                if (prev < level) {
                    pool_alloc(&node, 0x28, node_pool);
                    node->next = cur->next;
                    cur->next = node;
                    cur = cur->next;
                    str_copy(cur->text, level_names[level]);
                    cur->key = 1;
                }
                i++;
                cur = cur->next;
            } while (cur != 0);
        }
        result = 1;
    }
    return result;
}

unsigned char far spell_range(unsigned char spell)               /* 3b69:0ff0 */
{
    char r;
    unsigned char n;

    r = 4;
    if (item_cast_active == 0)
        n = caster_level(spell, 0) * spell_info[spell].range_per_level + spell_info[spell].range;
    else
        n = spell_info[spell].range_per_level * 6 + spell_info[spell].range;
    if (spell == 0x7d)
        n = n / 4;
    if (n == 0 && spell_info[spell].targeting != 0)
        n = 1;
    if (n == 0xff)
        n = 1;
    r = n;
    return r;
}

int far spell_duration(unsigned char spell)                     /* 3b69:10d7 */
{
    short r;

    switch (spell) {
    case 0x1a:
        r = 0xec4;
        break;
    case 0x7f:
    case 0x82:
        r = fx_roll_dice(5, 4);
        break;
    case 0x80:
        r = fx_roll_dice(1, 4) * 10 + 0x28;
        break;
    case 0x7a:
        r = 0x5a0;
        break;
    case 0x79:
        r = fx_roll_dice(1, 4) + 1;
        break;
    default:
        r = caster_level(spell, 0) * spell_info[spell].duration_per_level + spell_info[spell].duration;
        break;
    }
    return r;
}

void far spell_damage_apply(unsigned char spell, char level, char b, unsigned char dmg, int d,
                         char far *msg)                          /* 3b69:11b5 */
{
    char lvl;
    unsigned char i;
    char saved;

    if (dmg == 0)
        element_mask = 0;
    else
        element_mask = d;
    if (target_count == 0)
        return;
    if (level == 0)
        lvl = caster_level(spell, 1);
    else
        lvl = level;
    for (i = 1; i <= target_count; i++) {
        if (targets[i] != 0) {
            if (spell_info[spell].save_effect == 0)
                saved = 0;
            else
                saved = fx_saving_throw(targets[i], spell_info[spell].save_type, 0);
            if (spell_info[current_spell].range == 0xff) {
                recalc_stats(targets[i]);
                fx_trigger_group(0xb, &targets[i]);
                if (!fx_attack_roll(cur_node, targets[i], targets[i]->ac)) {
                    dmg = 0;
                    saved = 1;
                }
            }
            if (dmg > 0)
                fx_damage(targets[i], dmg, spell_info[spell].save_effect, saved);
            if (spell_info[spell].effect > 0)
                fx_add_message(targets[i], spell_info[spell].effect, spell_duration(spell), lvl, b,
                             spell_info[spell].save_effect, saved, msg);
        }
    }
    element_mask = 0;
}

void far camp_spell_targets(unsigned char spell, char mode, char far *ok)
                                                                 /* 3b69:13cb */
{
    targets[1] = cur_node;
    target_count = 1;
    *ok = 1;
    switch (spell_info[spell].target) {
    case 1:
        break;
    case 2:
        view_redraw();
        if (selected_target != 0)
            targets[1] = selected_target;
        pick_hero("Cast Spell on whom", 1, 1, &targets[1]);
        selected_target = targets[1];
        if (targets[1] == 0) {
            target_count = 0;
            *ok = 0;
        }
        break;
    case 3:
        targets[target_count] = node_list;
        while (targets[target_count]->next != 0) {
            targets[target_count + 1] = targets[target_count]->next;
            target_count++;
        }
        break;
    default:
        *ok = 0;
        break;
    }
}

void far spell_cast(unsigned char spell, char quiet, char announce, char far *done)
                                                                 /* 3b69:14c8 */
{
    char again;
    char x;
    char y;
    char n;
    struct Hero far *caster;
    char kind;

    caster = cur_node;
    again = 1;
    if (view_mode != 5 && spell_info[spell].target == 0) {
        if (item_cast_active == 0) {
            status_printf(1, 0x13, 7, 0, spell_names[spell]);
            status_printf(1, 0x14, 7, 0, txt_cant_cast_here);
            if (confirm(txt_lose_it, 1))
                spell_mark(caster, spell);
        } else {
            status_printf(1, 0x13, 7, 0, txt_that_item);
            status_printf(1, 0x14, 7, 0, txt_combat_only);
            if (confirm(txt_use_it, 1))
                *done = 1;
        }
        announce = 0;
        again = 0;
    }
    if (announce && item_cast_active == 0)
        cast_announce(caster, txt_casts, spell);
    while (again) {
        hit_proc(spell, quiet, done);
        if (*done) {
            again = 0;
            if (view_mode == 5 && spell != 0x7c) {
                if (spell == 0x2f || spell == 0x73 || spell == 0x4a || spell == 0x85)
                    icon_facings(0x20);
                else
                    icon_facings(0x13);
                x = combatant_x(caster);
                y = combatant_y(caster);
                for (n = 0; !dir_ok(x, y, view_cx, view_cy, n); n++)
                    ;
                if (spell == 0x2f || spell == 0x73 || spell == 0x4a || spell == 0x85) {
                    kind = 1;
                    missile_facing(n, 0x20);
                } else
                    kind = 4;
                move_flag = 1;
                combatant_facing_set(caster, n, 1, 0);
                switch (spell) {
                case 0x2f:
                    sfx_play(0xc);
                    break;
                case 0x33:
                    sfx_play(9);
                    break;
                default:
                    sfx_play(3);
                    break;
                }
                icon_fly(x, y, view_cx, view_cy, kind, 0x1e);
                switch (spell) {
                case 0x22:
                    icon_scatter(1, 0x1b, 0xc);
                    break;
                case 0x5b:
                    icon_scatter(1, 0x1c, 0xc);
                    break;
                case 0x2f:
                case 0x73:
                case 0x85:
                    if (cursor_obj[0x22] == 0 || spell == 0x85)
                        icon_scatter(2, 0x19, 0xc);
                    else
                        icon_scatter(3, 0x19, 0xc);
                    break;
                case 0x57:
                    if (cursor_obj[0x3c])
                        icon_scatter(3, 0x1a, 0xc);
                    else
                        icon_scatter(2, 0x1a, 0xc);
                    break;
                case 0x33:
                    if (cursor_obj[0x3c])
                        icon_scatter(2, 0x14, 9);
                    break;
                }
                if (combatant_visible(caster, 0)) {
                    combatant_facing_set(caster, caster->fight->facing, 1, 1);
                    combatant_facing_set(caster, caster->fight->facing, 0, 0);
                }
            }
            fx_invisibility_remove(caster);
            if (item_cast_active == 0 && spell > 0)
                spell_mark(caster, spell);
            current_spell = spell;
            spell_run(current_spell);
            current_spell = 0;
            mirror_image_bypass = 0;
        } else if (view_mode == 5) {
            if (quiet || confirm(txt_abort_spell, 1)) {
                bottom_msg(txt_spell_aborted);
                if (item_cast_active == 0 && spell > 0)
                    spell_mark(caster, spell);
                again = 0;
            }
        } else
            again = 0;
    }
    combat_text_clear();
    if (view_mode == 5)
        box_draw_top();
}

void far path_init(char x0, char y0, char x1, char y1, struct LineWalk far *l)
                                                                 /* 3b69:193d */
{
    l->x0 = x0;
    l->y0 = y0;
    l->x1 = x1;
    l->y1 = y1;
    line_init(l);
}

void far path_clamp(char far *x, char far *y)                   /* 3b69:196f */
{
    if (*x > 0x31)
        *x = 0x31;
    else if (*x < 0)
        *x = 0;
    if (*y > 0x18)
        *y = 0x18;
    else if (*y < 0)
        *y = 0;
}

void far path_collect(unsigned char far *hits, unsigned char far *tile, unsigned char far *who,
                      struct LineWalk far *l, char far *end)     /* 3b69:19ac */
{
    unsigned char i;
    char found;

    do {
        *end = !line_step(l);
        if (*end == 0) {
            cell_get(l->x, l->y, who, tile);
            i = 1;
            found = 0;
            if (*who > 0) {
                do {
                    if (hits[i - 1] == *who)
                        found = 1;
                    else if (hits[i - 1] == 0) {
                        hits[i - 1] = *who;
                        found = 1;
                    } else
                        i++;
                } while (!found);
            }
        }
    } while (*end == 0);
}

void far spell_path_targets(char x0, char y0, char x1, char y1, unsigned char width,
                         unsigned char len)                      /* 3b69:1a61 */
{
    char nx;
    char ny;
    char fx;
    char fy;
    struct LineWalk line;
    char done;
    unsigned char dist;
    unsigned char idx;
    unsigned char last;
    unsigned char dirs[50];
    unsigned char hits[30];
    unsigned char who;
    unsigned char tile;
    unsigned char count;
    unsigned char out;
    unsigned char dir;
    unsigned range;
    char found;

    idx = 0;
    path_init(x0, y0, x1, y1, &line);
    do {
        done = !line_step(&line);
        if (done == 0) {
            dirs[idx] = line.dir;
            idx++;
        }
    } while (done == 0);
    last = idx - 1;
    idx = 0;
    len = len * 2;
    dist = line.steps;
    done = 0;
    while (dist < len && done == 0) {
        if (x1 < 0x31 && x1 > 0 && y1 < 0x18 && y1 > 0) {
            switch (dirs[idx]) {
            case 0:
            case 2:
            case 4:
            case 6:
                dist += 2;
                break;
            case 1:
            case 3:
            case 5:
            case 7:
                dist += 3;
                break;
            }
            x1 += dir_dx[dirs[idx]];
            y1 += dir_dy[dirs[idx]];
            if (idx == last)
                idx = 0;
            else
                idx++;
        } else
            done = 1;
    }
    path_clamp(&x1, &y1);
    range = len;
    line_clear(x0, y0, &x1, &y1, &range);
    path_init(x0, y0, x1, y1, &line);
    mem_zero(hits, 0x1e);
    count = 1;
    do {
        done = !line_step(&line);
        if (done == 0) {
            cell_get(line.x, line.y, &who, &tile);
            if (who > 0) {
                if (count > 1) {
                    for (tick_slot = 1; tick_slot <= count - 1; tick_slot++)
                        if ((hits - 1)[tick_slot] == who)
                            found = 1;
                } else
                    found = 0;          /* only here: once set, it stays set */
                if (!found) {
                    (hits - 1)[count] = who;
                    count++;
                }
                dir = line.dir;
            }
        }
    } while (done == 0);
    if (width > 1 && width != 0xff) {
        nx = near_dx[dir] + x1;         /* dir is unset if nothing was hit */
        ny = near_dy[dir] + y1;
        path_clamp(&nx, &ny);
        fx = far_dx[dir] + x1;
        fy = far_dy[dir] + y1;
        path_clamp(&fx, &fy);
        path_init(x0, y0, fx, fy, &line);
        path_collect(hits, &tile, &who, &line, &done);
        if (width > 2) {
            path_init(x0, y0, nx, ny, &line);
            path_collect(hits, &tile, &who, &line, &done);
        }
    }
    if (width == 0xff) {
        plan_steps(x1, y1, 3, 0xff, 1);
        for (tick_slot = 1; tick_slot <= step_count; tick_slot++)
            (hits - 1)[tick_slot] = (steps - 1)[tick_slot].who;
    }
    count = 1;
    out = 1;
    target_count = 0;
    if ((hits - 1)[1] != 0) {
        do {
            if ((heroes - 1)[(hits - 1)[count]] != cur_node) {
                target_count++;
                targets[out] = (heroes - 1)[(hits - 1)[count]];
                out++;
            }
            count++;
        } while ((hits - 1)[count] != 0);
    }
}

void far spell_effect_apply(char n, char far *msg)                  /* 3b69:1e6f */
{
    struct Hero far *cur;
    char saved;
    unsigned char i;
    short pred;
    char level;
    char extra;

    extra = 0;
    for (i = target_count; i >= 1; i--) {
        if (targets[i] != 0) {
            cur = targets[i];
            if (i < target_count) {
                sfx_play(3);
                icon_facings(0x13);
                icon_fly(combatant_x(cur_node), combatant_y(cur_node), combatant_x(cur),
                             combatant_y(cur), 4, 0x1e);
            }
            if ((current_spell == 0x4f || current_spell == 0x51) && i == target_count) {
                saved = 1;
                pred = 1;
            } else {
                saved = fx_saving_throw(cur, spell_info[current_spell].save_type, n);
                pred = spell_info[current_spell].save_effect;
            }
            if (cur->race == 6 && (current_spell == 0x17 || current_spell == 0x31))
                saved = 1;
            if (!(cur->type_flags & 0x40) && current_spell == 0x6c)
                saved = 1;
            if (current_spell == 0x51) {
                level = caster_level(current_spell, 1) + (cur_node->side << 7);
                extra = 1;
            } else
                level = caster_level(current_spell, 1);
            fx_add_message(cur, spell_info[current_spell].effect, spell_duration(current_spell), level,
                         extra, pred, saved, msg);
        }
    }
}

unsigned char far spell_menu(unsigned char list, unsigned char type, int far *choice,
                             char far *more)                     /* 3b69:203e */
{
    char result;
    char far *label;

    switch (list) {
    case 0:
        label = "in Memory";
        break;
    case 1:
    case 12:
        label = "in Grimoire";
        break;
    case 2:
        label = "on Scroll";
        break;
    case 7:
        label = "in Bundle";
        break;
    case 3:
        label = "on Scrolls";
        break;
    case 4:
        label = "to Choose";
        break;
    case 5:
        label = "to Memorize";
        break;
    case 6:
        label = "to Scribe";
        break;
    default:
        label = "";
        break;
    }
    *more = spell_list_build(list);
    if (*more) {
        if (*choice < 0 || type == 1) {
            if (view_mode != 5) {
                if (type != 2)
                    frame_list_panel();
            } else
                frame_border();
        }
        character_name_show(cur_node, 1, 1, 1);
        status_printf(str_length(cur_node->name) + 4, 1, 7, 0, msg_format("Spells %s", label));
        result = spell_choose(type, choice);
    } else
        result = 0;
    return result;
}

void far spell_run(unsigned char spell)                          /* 3b69:217d */
{
    (*spell_fn[spell])();
}
