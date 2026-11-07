/* CKIT.EXE overlay 9 module 21d4: making camp.  The camp menu and its
 * submenus: casting, memorizing and scribing spells and the slots they
 * use, the spell effects on each party member, resting, the party order,
 * game speed, icon and difficulty level, and fixing the party by resting
 * while its healing spells are cast.
 *
 * EXE 0x045fd0..0x048d6a, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 21d4, the first module of RTLink section 9 (payload
 * 0x45fd0, link segment 21d4, parent section 3).  Its final retf is byte 9
 * of paragraph 24ad, where the next module begins at 24ad:000a (section 9's
 * table files every fixup site before it under frame 21d4, and every one
 * after it under 24ad).  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 9.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and an ovlNN_ name which section and stored
 * segment.  A mod18de_ callee is an RTLink vector stub; its comment names
 * the routine the stub reaches.  Code outside the section reaches six of
 * these routines through vector stubs: 18de:1aa3 -> 0581, 18de:1aad ->
 * 0634, 18de:1ab7 -> 08e8, 18de:1ac1 -> 02fe, 18de:1099 -> 164f,
 * 18de:1571 -> 2ab9.
 *
 * This is DQK's camp.c relinked, with FRUA's record layouts and changes in
 * the routines the bytes show: no special-class spells, three spell kinds,
 * a check that a member has a first-level spell slot, and a camp menu with
 * Save, Done and a flag that trades Load and Save for Done.  fix_needs and
 * fix_cures step three of their counter pointers where they mean to count,
 * as shipped (side comments there).
 *
 * _DATA is 28 stored literals, DGROUP 762c..7851; -d stores "" as the NUL
 * of "cannot cast spells in this area" and " " as the tail of "is in no
 * condition to ".  The module declares no _BSS.
 */

struct Effect {                 /* an effect on a party member            */
    unsigned char kind;         /* +0  index into fx_names                */
    char pad_1[5];              /* +1  not read here                      */
    struct Effect far *next;    /* +6                                     */
};

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4  not read here                      */
    unsigned char type;         /* +28 'I': spells are in more's chain    */
    char pad_29[0xc];           /* +29 not read here                      */
    unsigned char slots[4];     /* +35 [0] 'I' chain length; [1..3] spell */
    char pad_39[1];             /* +39 not read here                      */
    struct Item far *more;      /* +3a                                    */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x4c];           /* +c   not read here                     */
    unsigned char race;         /* +58                                    */
    char pad_59[5];             /* +59  not read here                     */
    unsigned char status;       /* +5e  6 dead, 7 stone                   */
    char pad_5f;                /* +5f  not read here                     */
    char name[0x11];            /* +60                                    */
    char pad_71[2];             /* +71  not read here                     */
    unsigned char intel;        /* +73  bounds magic-user spell levels    */
    char pad_74;                /* +74  not read here                     */
    unsigned char wis;          /* +75  bounds cleric spell levels        */
    char pad_76[3];             /* +76  not read here                     */
    unsigned char con;          /* +79  raising the dead lowers it        */
    char pad_7a[4];             /* +7a  not read here                     */
    char study;                 /* +7e  prep_time's base                  */
    char pad_7f[2];             /* +7f  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[8];             /* +82  not read here                     */
    unsigned char dual_level;   /* +8a                                    */
    char pad_8b[8];             /* +8b  not read here                     */
    unsigned char control;      /* +93  bit 7 bars the icon editor        */
    char pad_94[2];             /* +94  not read here                     */
    unsigned char level_max[7]; /* +96  one per class                     */
    unsigned char level[7];     /* +9d  one per class                     */
    char pad_a4[0x14];          /* +a4  not read here                     */
    unsigned char hp_base;      /* +b8                                    */
    char pad_b9[0xd];           /* +b9  not read here                     */
    unsigned char spells[0x8d]; /* +c6  bit 7: still to memorize          */
    unsigned char known[0xf];   /* +153 one bit per spell, from 1         */
    unsigned char slots_by_level[3][9]; /* +162 stride 9: level 9 is the next row */
    char pad_17d;               /* +17d read as slots[2][9]               */
    char active;                /* +17e 0: can do nothing with spells     */
    char pad_17f[0xc];          /* +17f not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0  0 cleric, 1 druid, 2 magic-user    */
    unsigned char level;        /* +2  from 1                             */
    char pad_3[0x13];           /* +3  not read here                      */
};

struct Pool;                    /* strutil.c's slot pool                 */

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Node {
    struct Node far *next;      /* +0                                    */
    char key;                   /* +4                                    */
    char text[35];              /* +5                                    */
};

extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far node_prompt(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1a07 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim); /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far set_step_mode(char c);                           /* 083a:239f */
extern char far step_mode(void);                                 /* 083a:23aa */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool); /* 0a75:0203 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_main(void);                                /* 0d65:00de */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far frame_lower_panel_palette(void);                                /* 0d65:021a */
extern void far pic_release(void);                           /* 1bbc:02b4 */
extern void far party_drop(void);                                /* 24ad:104a */
extern void far party_info_display(void);             /* 18de:105d, vector to overlay 4 1c86:0406 */
extern void far party_display(void far *node);   /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);  /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides); /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far char_view(char far *done);   /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far spell_cast(unsigned char n, char a, char b, char far *done);
                                                /* 18de:1175, vector to overlay 12 2b69:14c8 */
extern unsigned char far spell_menu(unsigned char list, unsigned char type, int far *choice,
                                      char far *more);
                                                /* 18de:1305, vector to overlay 12 2b69:203e */
extern void far pending_load_set(char n);           /* 18de:141d, vector to overlay 4 1cff:08e5 */
extern void far game_save_menu(void);             /* 18de:16bb, vector to overlay 4 1ffa:1a85 */
extern void far icon_customize(char n);           /* 18de:19a9, vector to overlay 7 21d4:0f94 */
extern char far item_casts(struct Item far *item);
                                                /* 18de:1acb, vector to overlay 12 26e3:4055 */
extern char far spell_list_build(unsigned char list); /* 18de:1ad5, vector to overlay 12 2b69:0b13 */
extern char far camp_rest(char interactive); /* 18de:1adf, vector to overlay 19 2d91:0d37 */
extern void far spell_run(unsigned char spell); /* 18de:1ae9, vector to overlay 12 2b69:217d */
extern char far fx_heal(struct Hero far *hero, char a, char b);
                                                /* 18de:1af3, vector to overlay 19 2e88:224c */
extern void far spell_remove_curse(void);             /* 18de:1afd, vector to overlay 12 26e3:137e */
extern void far fx_stat_recalc(struct Hero far *hero, short stat);
                                                /* 18de:1b07, vector to overlay 19 2e88:1a80 */

extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern char far *exit_text;                                      /* 3a70 */
extern char far *selected_text;                                  /* 3b20 */
extern char far *speed_text;                                     /* 3b24 */
extern char far *camp_text;                                      /* 3b28 */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *order_title;                                    /* 3bfc */
extern char far *speed_title;                                    /* 3c00 */
extern char far *order_items;                                    /* 3c74 */
extern char far *speed_items;                                    /* 3c78 */
extern char far *exit_items;                                     /* 3c80 */
extern char far *camp_menus[4];         /* 3ce3  camp, magic, alter, level */
extern char far *fx_names[];                                     /* 6379 */
extern unsigned char bit_mask[8];                                /* 6711 */
extern char in_service_event;                                           /* 671c */
extern char quit_requested;                                           /* 6725 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern char view_kind;                                           /* b40e */
extern char view_kind_saved;                                           /* b417 */
extern int view_mode;                                            /* b421 */
extern char game_saved;                                           /* b425 */
extern int saved_view_mode;                                            /* b433 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern unsigned rest_time[7];                                    /* b729 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char picture_hides_roster;                                           /* b761 */
extern char fx_lock;                                             /* b8d0 */
extern char door_tries[];                                         /* ba04 */
extern char adventure_exit;                                           /* bcf6 */
extern unsigned char current_spell;                                  /* be64 */
extern char no_study;                                            /* be68 */
extern unsigned heal_ticks;                                      /* c34e */
extern char pic_name[];                                          /* c3a7 */
extern char pic_saved[];                                         /* c656 */
extern unsigned char bonus_slots;                                /* ca09 */
extern unsigned encounter_ticks;                                 /* ca1b */
extern char pick_special;                                        /* ca29 */
extern char menu_escaped;                                        /* ca2a */
extern char test_mode;                                           /* cc7a */
extern void far *node_pool;                                      /* ce5d */
extern struct Hero far *spell_target;                            /* cfe7 */
extern char order_mode;                                           /* d126 */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */

/* The translated Pascal unit's empty initialization block (DKK camp.pas). */
void far camp_unit_init(void)                                         /* 31d4:0000 */
{
}

int far prep_time(struct Hero far *hero)                         /* 31d4:0001 */
{
    short r;
    unsigned char max_mem;
    unsigned char max_scroll;
    struct Item far *sub;
    struct Item far *item;
    unsigned char i;
    unsigned char level;
    unsigned char base;
    int mem;
    int scroll;

    max_mem = 0;
    max_scroll = 0;
    scroll = mem = 0;
    for (i = 0; i <= 0x8c; i++)
        if ((hero->spells[i] & 0x80) && hero->active) {
            level = spell_info[hero->spells[i] & 0x7f].level;
            if (level > max_mem)
                max_mem = level;
            mem += level;
        }
    for (item = hero->items; item; item = item->next)
        if (item_casts(item)) {
            if (item->type == 'I')
                sub = item->more;
            else
                sub = item;
            do {
                for (i = 1; i <= 3; i++)
                    if (sub->slots[i] & 0x80) {
                        level = spell_info[sub->slots[i] & 0x7f].level;
                        if (level > max_scroll)
                            max_scroll = level;
                        scroll += level;
                    }
                sub = sub->more;
            } while (item->type == 'I' && sub);
        }
    base = 0;
    if (mem > 0 || scroll > 0)
        base = 4;
    if (max_mem > 2 || max_scroll > 2)
        base = 6;
    hero->study = base;
    r = base * 60 + mem * 15 + scroll * 15;
    return r;
}

void far memorize_reset(struct Hero far *hero)                   /* 31d4:01ab */
{
    unsigned char i;

    for (i = 0; i <= 0x8c; i++)
        if (hero->spells[i] & 0x80)
            hero->spells[i] = 0;
    hero->study = 0;
}

void far scribe_reset(struct Hero far *hero)                     /* 31d4:01ee */
{
    unsigned char i;
    struct Item far *sub;
    struct Item far *item;

    for (item = hero->items; item; item = item->next)
        if (item_casts(item)) {
            if (item->type == 'I') {
                for (sub = item->more, tick_slot = 1; item->slots[0] >= tick_slot;
                     sub = sub->more, tick_slot++)
                    for (i = 1; i <= 3; i++)
                        sub->slots[i] = sub->slots[i] & 0x7f;
            } else
                for (i = 1; i <= 3; i++)
                    item->slots[i] = item->slots[i] & 0x7f;
        }
}

void far scribe_reset_all(void)                                  /* 31d4:02c8 */
{
    struct Hero far *h;

    for (h = node_list; h; h = h->next)
        scribe_reset(h);
}

unsigned char far slots_left(unsigned char level, short kind,
                             unsigned char far *used)            /* 31d4:02fe */
{
    char r;
    unsigned char count;
    unsigned char i;

    count = 0;
    for (i = 0; i <= 0x8c; i++)
        if (cur_node->spells[i] > 0
            && spell_info[cur_node->spells[i] & 0x7f].level == level
            && spell_info[cur_node->spells[i] & 0x7f].kind == kind)
            count++;
    *used = count;
    if (cur_node->slots_by_level[kind][level] < count)
        count = cur_node->slots_by_level[kind][level];
    r = cur_node->slots_by_level[kind][level] - count;
    if (kind == 2) {
        if (cur_node->intel < 0xc && level > 6)
            r = 0;
        if (cur_node->intel < 0xe && level > 7)
            r = 0;
        if (cur_node->intel < 0x10 && level > 8)
            r = 0;
        if (cur_node->intel < 0x12 && level > 9)
            r = 0;
    } else if (kind == 0) {
        if (cur_node->wis < 0x11 && level > 6)
            r = 0;
        if (cur_node->wis < 0x12 && level > 7)
            r = 0;
    }
    return r;
}

char far spell_ok(unsigned char activity)                        /* 31d4:0476 */
{
    char r;
    char msg[42];

    msg[0] = 0;
    if (activity == 1 && cursor_obj[2] > 0)
        str_copy(msg, "cannot cast spells in this area");
    else if (cur_node->status == 1 || cur_node->active == 0) {
        str_copy(msg, "is in no condition to ");
        switch (activity) {
        case 1:
            str_cat(msg, "cast any spells");
            break;
        case 2:
            str_cat(msg, "memorize spells");
            break;
        case 3:
            str_cat(msg, "scribe any scrolls");
            break;
        }
    }
    if (msg[0] == 0 && cur_node->slots_by_level[0][1] == 0 && cur_node->slots_by_level[1][1] == 0
        && cur_node->slots_by_level[2][1] == 0)
        str_copy(msg, "Cannot do magic");
    if (msg[0] == 0)
        r = 1;
    else {
        r = 0;
        hero_text(cur_node, msg, 10, 1);
    }
    return r;
}

void far cast_menu(void)                                         /* 31d4:0581 */
{
    int index;
    char not_empty;
    char redraw;
    char done;
    unsigned char spell;

    redraw = 0;
    index = -1;
    ui_mode = 1;
    if (spell_ok(1))
        do {
            spell = spell_menu(0, 1, &index, &not_empty);
            if (spell != 0) {
                redraw = 1;
                text_clear(1, 0x11, 0x26, 0x16);
                spell_cast(spell, 0, 1, &done);
            } else if (not_empty)
                redraw = 1;
            else
                hero_text(cur_node, "has no spells memorized", 10, 1);
        } while (spell != 0);
    if (redraw)
        view_redraw();
}

unsigned char far show_slots(void)                               /* 31d4:0634 */
{
    char label[42];
    char slot[10][3];
    char n;
    unsigned char avail[3];
    short kind;
    unsigned char level;
    unsigned char column;
    unsigned char row;
    unsigned char c;
    unsigned char any;

    bonus_slots = 0;
    any = 0;
    for (kind = 0; kind <= 2; kind++) {
        avail[kind] = 0;
        for (level = 1; level <= 9; level++) {
            slot[level - 1][kind] = slots_left(level, kind, &c);
            if (cur_node->slots_by_level[kind][level] == 0)
                slot[level - 1][kind] = -1;
            else {
                avail[kind] = 1;
                if (slot[level - 1][kind] != 0)
                    any = 1;
            }
        }
    }
    if (any) {
        text_clear(1, 0x11, 0x26, 0x16);
        row = 1;
        for (kind = 0; kind <= 2; kind++)
            if (avail[kind]) {
                switch (kind) {
                case 0:
                    str_copy(label, "    Cleric :");
                    break;
                case 1:
                    str_copy(label, "     Druid :");
                    break;
                case 2:
                    str_copy(label, "Magic-User :");
                    break;
                }
                status_printf(1, row + 0x11, 7, 0, label);
                column = 0xd;
                for (level = 1; level <= 9; level++) {
                    n = slot[level - 1][kind];
                    if (n >= 0)
                        status_printf(column + 1, row + 0x11, 7, 0, "%d", n);
                    column += 3;
                }
                row++;
            }
    }
    return any;
}

void far study_rest(char far *interrupted)                        /* 31d4:081e */
{
    struct Hero far *h;
    unsigned short total;
    unsigned short time;

    total = 0;
    for (h = node_list; h; h = h->next) {
        time = prep_time(h);
        if (total < time)
            total = time;
    }
    rest_time[4] = total / 1440;
    total %= 1440;
    rest_time[3] = total / 60;
    total %= 60;
    rest_time[2] = total / 10;
    rest_time[1] = total % 10;
    encounter_ticks = 0;
    *interrupted = camp_rest(1);
    if (*interrupted)
        view_mode = saved_view_mode;
    mem_fill(rest_time, sizeof rest_time, 0);
    party_info_display();
}

void far memorized_sort(void)                                    /* 31d4:08e8 */
{
    unsigned char i;
    unsigned char j;
    unsigned char temp;

    for (i = 0; i <= 0x8b; i++)
        for (j = i + 1; j <= 0x8c; j++)
            if (cur_node->spells[i] == 0
                || (cur_node->spells[i] & 0x7f) > (cur_node->spells[j] & 0x7f)
                   && cur_node->spells[j] != 0) {
                temp = cur_node->spells[i];
                cur_node->spells[i] = cur_node->spells[j];
                cur_node->spells[j] = temp;
            }
}

void far memorize_menu(void)                                     /* 31d4:09b7 */
{
    char done;
    char not_empty;
    char redraw;
    unsigned char spell;
    unsigned char i;
    unsigned char used;
    int index;

    if (spell_ok(2)) {
        done = 0;
        index = -1;
        ui_mode = 1;
        not_empty = spell_list_build(5);
        if (not_empty) {
            spell = spell_menu(5, 8, &index, &not_empty);
            if (spell == 0)
                memorize_reset(cur_node);
            else
                done = 1;
        } else {
            spell = spell_menu(5, 5, &index, &not_empty);
            used = 1;
            redraw = 0;
        }
        index = -1;
        redraw = 1;
        frame_lower_panel_palette();
        while (!done) {
            done = !show_slots();
            if (done)
                hero_text(cur_node, "cannot memorize any more spells", 10, 1);
            else {
                spell = spell_menu(1, 2, &index, &not_empty);
                redraw = 1;
                if (spell == 0)
                    done = 1;
                else if (slots_left(spell_info[spell].level, spell_info[spell].kind, &used) > 0
                         || spell_info[spell].kind == 2 && bonus_slots > 0) {
                    pick_special = 0;
                    for (i = 0; cur_node->spells[i] != 0; i++)
                        ;
                    cur_node->spells[i] = spell + 0x80;
                    memorized_sort();
                }
            }
        }
        if (index != -1) {
            index = -1;
            spell = spell_menu(5, 8, &index, &not_empty);
            if (not_empty && spell == 0)
                memorize_reset(cur_node);
        }
        if (redraw)
            view_redraw();
    }
}

void far scribe_menu(void)                                       /* 31d4:0ba2 */
{
    char done;
    char not_empty;
    char redraw;
    unsigned char spell;
    unsigned char j;
    unsigned char k;
    int index;
    struct Item far *item;
    struct Item far *sub;
    char found;

    if (spell_ok(3)) {
        done = 0;
        index = -1;
        not_empty = spell_list_build(6);
        if (not_empty) {
            redraw = 1;
            spell = spell_menu(6, 9, &index, &not_empty);
            if (spell == 0)
                scribe_reset(cur_node);
            else
                done = 1;
        } else
            redraw = 0;
        index = -1;
        while (!done) {
            spell = spell_menu(3, 3, &index, &not_empty);
            if (spell == 0) {
                done = 1;
                if (!not_empty)
                    hero_text(cur_node, "has no copyable scrolls", 10, 1);
                else
                    redraw = 1;
            } else {
                redraw = 1;
                if (cur_node->known[(spell - 1) / 8] & bit_mask[(spell - 1) % 8]) {
                    hero_text(cur_node, "already knows that spell", 10, 1);
                    index = -1;
                } else {
                    item = cur_node->items;
                    found = 0;
                    while (item && !found) {
                        if (item_casts(item)) {
                            if (item->type == 'I') {
                                for (sub = item->more, k = 1; item->slots[0] >= k;
                                     sub = sub->more, k++)
                                    for (j = 1; j <= 3; j++)
                                        if (!found && (sub->slots[j] & 0x80)
                                            && (sub->slots[j] & 0x7f) == spell) {
                                            hero_text(cur_node, "already scribed that spell",
                                                      10, 1);
                                            index = -1;
                                            found = 1;
                                        }
                            } else
                                for (j = 1; j <= 3; j++)
                                    if ((item->slots[j] & 0x80)
                                        && (item->slots[j] & 0x7f) == spell) {
                                        hero_text(cur_node, "is already scribing that spell",
                                                  10, 1);
                                        found = 1;
                                        index = -1;
                                    }
                        }
                        item = item->next;
                    }
                    if (!found) {
                        if (cur_node->slots_by_level[spell_info[spell].kind][spell_info[spell].level] > 0) {
                            item = cur_node->items;
                            while (item && !found) {
                                if (item->type == 'I') {
                                    for (sub = item->more, k = 1; item->slots[0] >= k; k++) {
                                        j = 1;
                                        do {
                                            if (sub->slots[j] == spell) {
                                                sub->slots[j] = sub->slots[j] | 0x80;
                                                found = 1;
                                            }
                                            j++;
                                        } while (j <= 3 && !found);
                                        sub = sub->more;
                                        if (found)
                                            k = item->slots[0];
                                    }
                                } else if (item_casts(item)) {
                                    j = 1;
                                    do {
                                        if (item->slots[j] == spell) {
                                            item->slots[j] = item->slots[j] | 0x80;
                                            found = 1;
                                        }
                                        j++;
                                    } while (j <= 3 && !found);
                                }
                                item = item->next;
                            }
                        } else {
                            hero_text(cur_node, "can not scribe that spell.", 10, 1);
                            index = -1;
                        }
                    }
                }
            }
        }
        if (index != -1) {
            index = -1;
            spell = spell_menu(6, 9, &index, &not_empty);
            if (spell == 0)
                scribe_reset(cur_node);
        }
        if (redraw)
            view_redraw();
    }
}

void far fx_line_add(char title, char far *text,
                     struct Node far * far *cur)                 /* 31d4:0fe2 */
{
    pool_alloc((void far * far *)*cur, sizeof(struct Node), node_pool);
    *cur = (*cur)->next;
    (*cur)->next = 0;
    str_copy((*cur)->text, text);
    (*cur)->key = title;
}

void far effects_menu(void)                                      /* 31d4:1052 */
{
    struct Node far *head;
    struct Node far *cur;
    struct Hero far *h;
    unsigned char count;
    char redraw;
    char show;
    char text[42];
    int index;
    unsigned char id;
    struct Effect far *fx;

    pool_alloc((void far * far *)&head, sizeof(struct Node), node_pool);
    cur = head;
    cur->key = 1;
    cur->next = 0;
    h = node_list;
    str_copy(cur->text, h->name);
    while (h) {
        count = 0;
        if (h != node_list)
            fx_line_add(1, h->name, &cur);
        for (fx = h->effects; fx; fx = fx->next) {
            show = 1;
            id = fx->kind;
            if (id == 1 || id == 2 || id == 4 || id == 5 || id == 6 || id == 8
                || id == 9 || id == 0xa || id == 0xc || id == 0xd || id == 0xe
                || id == 0xf || id == 0x10 || id == 0x11 || id == 0x13 || id == 0x14
                || id == 0x15 || id == 0x17 || id == 0x18 || id == 0x19 || id == 0x1c
                || id == 0x1d || id == 0x20 || id == 0x21 || id == 0x22 || id == 0x23
                || id == 0x24 || id == 0x25 || id == 0x26 || id == 0x27 || id == 0x29
                || id == 0x2a || id == 0x2d || id == 0x2e || id == 0x31 || id == 0x32
                || id == 0x36 || id == 0x37 || id == 0x38 || id == 0x39 || id == 0x3d
                || id == 0x3e || id == 0x3f || id == 0x44 || id == 0x45 || id == 0x47
                || id == 0x4a || id == 0x4b || id == 0x4e || id == 0x59 || id == 0x5c
                || id == 0x5d || id == 0x62 || id == 0x73 || id == 0x77 || id == 0x79
                || id == 0x7a || id == 0x7b || id == 0x7e || id == 0xac) {
                str_copy(text, fx_names[fx->kind]);
                if (text[0] == 0)
                    show = 0;
            } else
                show = 0;
            if (show) {
                count++;
                fx_line_add(0, msg_format(" %s", text), &cur);
            }
        }
        if (count == 0)
            fx_line_add(0, " <No Spell Effects>", &cur);
        fx_line_add(0, " ", &cur);
        h = h->next;
    }
    redraw = 1;
    index = 0;
    frame_border();
    hotkey_reset(1);
    list_menu(empty_text, exit_items, 1, 4, 0x26, 0x16, head, 1, 0xff,
              &redraw, &index, &cur);
    list_free(&head);
    view_redraw();
}

void far magic_menu(char far *interrupted)                       /* 31d4:1411 */
{
    unsigned char k;
    unsigned char count;
    unsigned char choice;
    char saved;

    choice = 0xff;
    saved = 1;
    while (*interrupted == 0 && choice != 5 && (!menu_escaped || choice != 0x1b)) {
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        count = 0;
        for (k = 0; k <= 5; k++)
            if (k != 4 || cursor_obj[0x2c] < 0x64)
                hotkey_add(k, &count);
        ui_mode = saved;
        choice = node_prompt("", camp_menus[1], 1, 1);
        saved = ui_mode;
        if (menu_escaped) {
            party_rows(cur_node, 0);
            party_key_select(choice);
            party_rows(cur_node, 1);
        } else
            switch (choice) {
            case 0:
                cast_menu();
                break;
            case 1:
                memorize_menu();
                break;
            case 2:
                scribe_menu();
                break;
            case 3:
                effects_menu();
                break;
            case 4:
                study_rest(interrupted);
                break;
            }
    }
}

void far order_up(void)                                          /* 31d4:1534 */
{
    struct Hero far *prev;
    struct Hero far *h;
    struct Hero far *after;

    h = node_list;
    prev = 0;
    if (h == cur_node)
        while (h->next)
            h = h->next;
    else
        while (h->next != cur_node) {
            prev = h;
            h = h->next;
        }
    after = cur_node->next;
    if (node_list == cur_node) {
        if (after)
            node_list = after;
        h->next = cur_node;
        cur_node->next = 0;
    } else {
        h->next = after;
        cur_node->next = h;
        if (h == node_list || prev == 0)
            node_list = cur_node;
        else
            prev->next = cur_node;
    }
}

void far order_down(void)                                        /* 31d4:164f */
{
    struct Hero far *h;
    struct Hero far *after;

    h = node_list;
    if (h != cur_node)
        while (h->next != cur_node)
            h = h->next;
    after = cur_node->next;
    if (after == 0) {
        cur_node->next = node_list;
        node_list = cur_node;
        h->next = after;
    } else {
        cur_node->next = after->next;
        if (node_list == cur_node)
            node_list = after;
        if (h != cur_node)
            h->next = after;
        after->next = cur_node;
    }
}

void far order_menu(void)                                        /* 31d4:173c */
{
    unsigned char choice;
    char saved;

    set_step_mode(0);
    choice = 0xff;
    saved = 1;
    while (choice != 2 && (!menu_escaped || choice != 0x1b)) {
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        if (step_mode())
            hotkeys[0].key = 1;
        else
            hotkeys[0].key = 0;
        hotkeys[1].key = 2;
        ui_mode = saved;
        choice = node_prompt(order_title, order_items, 1, 1);
        saved = ui_mode;
        if (menu_escaped) {
            if (!step_mode()) {
                party_rows(cur_node, 0);
                party_key_select(choice);
                party_rows(cur_node, 1);
            } else {
                switch (choice) {
                case 0x87:
                case 0x88:
                    order_up();
                    break;
                case 0x84:
                case 0x85:
                    order_down();
                    break;
                }
                party_display(cur_node);
            }
        } else if (choice < 2 || menu_escaped && choice == 0x1b) {
            set_step_mode(!step_mode());
            if (step_mode())
                hero_text(cur_node, selected_text, 10, 0);
            else
                combat_text_clear();
        }
    }
    set_step_mode(0);
    order_mode = 0;
}

void far level_menu(void)                                        /* 31d4:18ae */
{
    unsigned char choice;
    unsigned char value;

    value = cursor_obj[0x27];
    ui_mode = value;
    hotkey_reset(4);
    choice = menu_run(empty_text, camp_menus[3], 0, 1);
    if (menu_escaped) {
        party_rows(cur_node, 0);
        party_key_select(choice);
        party_rows(cur_node, 1);
    } else if (choice < 5)
        value = choice + 1;
    cursor_obj[0x27] = value;
}

void far speed_menu(void)                                        /* 31d4:1944 */
{
    unsigned char count;
    unsigned char choice;
    char saved;

    saved = 1;
    do {
        status_printf(1, 0x12, 7, 0,
                      msg_format("%s%s", speed_text, byte_str(cursor_obj[0x12])));
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        count = 0;
        if (cursor_obj[0x12] > 0)
            hotkey_add(0, &count);
        if (cursor_obj[0x12] < 9)
            hotkey_add(1, &count);
        hotkey_add(2, &count);
        ui_mode = saved;
        choice = menu_run(speed_title, speed_items, 1, 1);
        saved = ui_mode;
        if (menu_escaped)
            switch (choice) {
            case 0x1b:
                choice = 2;
                break;
            case 0x84:
                if (cursor_obj[0x12] > 0)
                    cursor_obj[0x12]--;
                break;
            case 0x88:
                if (cursor_obj[0x12] < 9)
                    cursor_obj[0x12]++;
                break;
            }
        else
            switch (choice) {
            case 0:
                if (cursor_obj[0x12] > 0)
                    cursor_obj[0x12]--;
                break;
            case 1:
                if (cursor_obj[0x12] < 9)
                    cursor_obj[0x12]++;
                break;
            }
    } while (choice != 2);
    combat_text_clear();
}

void far alter_menu(void)                                        /* 31d4:1a7b */
{
    unsigned char choice;
    char saved;

    choice = 0xff;
    saved = 1;
    ui_mode = 1;
    while (choice != 5) {
        hotkey_reset(5);
        choice = node_prompt("", camp_menus[2], 1, 1);
        saved = ui_mode;
        if (menu_escaped) {
            if (choice == 0x1b)
                choice = 5;
            else {
                party_rows(cur_node, 0);
                party_key_select(choice);
                party_rows(cur_node, 1);
            }
        } else {
            switch (choice) {
            case 0:
                order_menu();
                break;
            case 1:
                if (cursor_obj[0x20] > 1)
                    party_drop();
                break;
            case 2:
                speed_menu();
                break;
            case 3:
                if (!(cur_node->control & 0x80)) {
                    icon_customize(1);
                    pic_release();
                    view_redraw();
                }
                break;
            case 4:
                level_menu();
                break;
            }
            ui_mode = saved;
        }
    }
}

unsigned char far levels_lost(struct Hero far *h)                /* 31d4:1b76 */
{
    char r;
    unsigned char sum;
    short i;

    sum = 0;
    for (i = 0; i <= 6; i++)
        if (h->level[i] < h->level_max[i])
            sum += h->level_max[i] - h->level[i];
    r = sum;
    return r;
}

/* The counters are pointers, as DKK's var parameters were; two of the seven
 * are moved rather than counted: restore_need at 21d4:1c29 and raise_need at
 * 1c3d step the pointer, as shipped, so the caller's counts stay 0. */
void far fix_needs(unsigned far *raise_need, unsigned far *curse_need,
                   unsigned far *sight_need, unsigned far *heal_need,
                   unsigned far *detox_need, unsigned far *flesh_need,
                   unsigned far *restore_need)                   /* 31d4:1bce */
{
    struct Hero far *h;
    struct Effect far *fx;

    *restore_need = 0;
    *raise_need = 0;
    *curse_need = 0;
    *sight_need = 0;
    *heal_need = 0;
    *detox_need = 0;
    *flesh_need = 0;
    for (h = node_list; h; h = h->next) {
        restore_need += levels_lost(h);
        if (h->status == 6 && h->race > 0)
            raise_need++;
        if (h->status == 7)
            (*flesh_need)++;
        if (find_effect(h, 0x24, &fx))
            (*curse_need)++;
        if (find_effect(h, 0x37, &fx))
            (*detox_need)++;
        if (find_effect(h, 0x44, &fx))
            (*heal_need)++;
        else if (find_effect(h, 0x21, &fx))
            (*sight_need)++;
    }
}

char far hero_ok(struct Hero far *h)                             /* 31d4:1cee */
{
    char r;

    r = h->status == 0;
    return r;
}

void far fix_spells(unsigned far *total, unsigned far *resurrect,
                    unsigned far *sight, unsigned far *flesh,
                    unsigned far *curse, unsigned far *raise,
                    unsigned far *detox, unsigned far *heal)     /* 31d4:1d0f */
{
    unsigned short score;
    struct Hero far *h;

    score = 0;
    for (h = node_list; h; h = h->next)
        if (hero_ok(h)) {
            if (h->known[0] & bit_mask[2])
                score += h->slots_by_level[0][1] * 9;
            if (h->known[7] & bit_mask[1])
                score += h->slots_by_level[0][4] * 0x11;
            if (h->known[8] & bit_mask[6])
                score += h->slots_by_level[0][5] * 0x1b;
            if (h->known[4] & bit_mask[3]) {
                score += h->slots_by_level[0][6] * 0x32;
                *heal += h->slots_by_level[0][6];
            }
            if (h->known[8] & bit_mask[2])
                *detox += h->slots_by_level[0][4];
            if (h->known[9] & bit_mask[2])
                *raise += h->slots_by_level[0][5];
            if (h->known[5] & bit_mask[2])
                *curse += h->slots_by_level[0][3];
            if (h->known[0xb] & bit_mask[0])
                *curse += h->slots_by_level[2][3];
            if (h->known[0xe] & bit_mask[0])
                *flesh += h->slots_by_level[2][6];
            if (h->known[4] & bit_mask[4])
                *sight += h->slots_by_level[0][3];
            if (h->known[0xd] & bit_mask[0])
                *resurrect += h->slots_by_level[0][6];
        }
    *total += score;
}

void far heal_roll(unsigned far *total, unsigned far *n4, unsigned far *n3,
                   unsigned far *n2, unsigned far *n1)           /* 31d4:1e91 */
{
    unsigned short score;
    unsigned char i;

    score = 0;
    for (i = 1; i <= *n1; i++)
        score += fx_roll_dice(1, 8);
    for (i = 1; i <= *n2; i++)
        score += fx_roll_dice(2, 8) + 1;
    for (i = 1; i <= *n3; i++)
        score += fx_roll_dice(3, 8) + 3;
    for (i = 1; i <= *n4; i++)
        score += fx_roll_dice(4, 8) + 3;
    *total += score;
}

unsigned far party_wounds(void)                                  /* 31d4:1f48 */
{
    unsigned short r;
    unsigned short sum;
    struct Hero far *h;

    sum = 0;
    for (h = node_list; h; h = h->next)
        sum += h->hp_max - h->hp;
    r = sum;
    return r;
}

void far heal_clock(unsigned far *n4, unsigned far *n3, unsigned far *n2,
                    unsigned far *n1, unsigned far *score)       /* 31d4:1f90 */
{
    struct Hero far *h;
    unsigned short maximum;
    unsigned short time1;
    unsigned short time2;
    unsigned short time3;
    unsigned short time4;
    unsigned short time5;
    unsigned short threshold;
    unsigned short value;
    unsigned short divisor;

    h = node_list;
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    time5 = 0;
    *n1 = 0;
    *n2 = 0;
    *n3 = 0;
    *n4 = 0;
    maximum = 0;
    while (h) {
        threshold = 0;
        if (hero_ok(h) && h->wis > 3) {
            value = h->slots_by_level[0][1];
            *n1 += value;
            time1 = value * 15;
            value = h->slots_by_level[1][2];
            *n1 += value;
            time2 = value * 30;
            *n2 += h->slots_by_level[0][4];
            time3 = h->slots_by_level[0][4] * 60;
            *n3 += h->slots_by_level[0][5];
            time4 = h->slots_by_level[0][5] * 75;
            *n4 += h->slots_by_level[0][6];
            time5 = h->slots_by_level[0][6] * 90;
        }
        threshold += time1 + time2 + time3 + time4 + time5;
        if (maximum < threshold)
            maximum = threshold;
        h = h->next;
    }
    if (party_wounds() < *score) {
        divisor = *score / party_wounds();
        maximum = maximum / divisor;
    }
    rest_time[3] = maximum / 60;
    rest_time[2] = (maximum - 60 * rest_time[3]) / 10;
    rest_time[1] = maximum % 10;
}

void far cure_clock(unsigned far *curse_need, unsigned far *sight_need,
                    unsigned far *detox_need, unsigned far *flesh_need,
                    unsigned far *heal_need, unsigned far *raise_need,
                    unsigned far *restore_need, unsigned far *sight,
                    unsigned far *curse, unsigned far *detox,
                    unsigned far *raise, unsigned far *flesh,
                    unsigned far *heal, unsigned far *resurrect,
                    unsigned far *restore)                       /* 31d4:212a */
{
    struct Hero far *h;
    unsigned short score;

    for (h = node_list; h; h = h->next)
        if (hero_ok(h)) {
            *restore += h->slots_by_level[0][7];
            *resurrect += h->slots_by_level[0][7];
            *heal += h->slots_by_level[0][6];
            *flesh += h->slots_by_level[2][6];
            *raise += h->slots_by_level[0][5];
            *detox += h->slots_by_level[0][4];
            *curse += h->slots_by_level[2][4];
            *curse += h->slots_by_level[0][3];
            *sight += h->slots_by_level[0][3];
        }
    score = 0;
    score = score + int_min(*restore, *restore_need) * 0x69;
    score = score + int_min(*resurrect, *raise_need) * 0x69;
    score = score + int_min(*heal, *heal_need) * 0x5a;
    score = score + int_min(*flesh, *flesh_need) * 0x5a;
    if (*resurrect == 0) {
        score = score + int_min(*raise, *raise_need) * 0x4b;
        *raise = 0;
    }
    score = score + int_min(*detox, *detox_need) * 0x3c;
    score = score + int_min(*sight, *sight_need) * 0x2d;
    score = score + int_min(*curse, *curse_need) * 0x2d;
    rest_time[3] = score / 60;
    rest_time[2] = (score - 60 * rest_time[3]) / 10;
    rest_time[1] = score % 10;
}

void far heal_party(unsigned far *total)                         /* 31d4:232b */
{
    struct Hero far *h;
    unsigned short amount;

    for (h = node_list; h; h = h->next)
        if (h->hp_max > h->hp) {
            amount = h->hp_max - h->hp;
            if (*total < amount)
                amount = *total;
            if (amount < 1)
                amount = 0;
            if (amount > 0)
                if (fx_heal(h, amount, 0))
                    if (*total >= amount)
                        *total -= amount;
        }
}

/* restore is stepped as a pointer at 21d4:23f8, as shipped, not counted
 * down.  The blindness cure removes effect 0x21 from cur_node rather than
 * h and never counts sight down, both as DKK's applyRecovery does. */
void far fix_cures(unsigned far *sight, unsigned far *curse,
                   unsigned far *raise, unsigned far *detox,
                   unsigned far *flesh, unsigned far *heal,
                   unsigned far *restore, unsigned far *resurrect) /* 31d4:23c6 */
{
    struct Hero far *h;
    struct Effect far *fx;
    char changed;
    unsigned char damage;
    unsigned char factor;
    short index;

    for (h = node_list; h; h = h->next) {
        changed = 0;
        while (levels_lost(h) > 0 && *restore > 0) {
            spell_target = h;
            current_spell = 0x66;
            spell_run(0x66);
            restore--;
        }
        if (*heal > 0 && find_effect(h, 0x44, &fx)) {
            fx_remove(h, 0x44, 0);
            if (find_effect(h, 0x21, &fx))
                fx_remove(h, 0x21, 0);
            changed = 1;
            hero_text(h, "is healed", 10, 1);
            (*heal)--;
            fx_stat_recalc(h, 1);
            fx_stat_recalc(h, 2);
        }
        if (*flesh > 0 && h->status == 7) {
            h->status = 0;
            h->active = 1;
            h->hp = 1;
            hero_text(h, "is changed back to flesh", 10, 1);
            (*flesh)--;
        }
        if (*detox > 0 && find_effect(h, 0x37, &fx)) {
            if (h->hp == 0)
                h->hp = 1;
            fx_lock = 1;
            fx_remove(h, 0x37, 0);
            fx_remove(h, 0x16, 0);
            fx_remove(h, 0xf, 0);
            fx_lock = 0;
            if (h->hp) {
                h->active = 1;
                h->status = 0;
            }
            hero_text(h, "is detoxified", 10, 1);
            (*detox)--;
        }
        if ((*raise > 0 || *resurrect > 0) && h->status == 6 && h->race > 0) {
            fx_lock = 1;
            fx_remove(h, 0x37, 0);
            fx_lock = 0;
            h->hp = 1;
            h->status = 0;
            h->active = 1;
            if (*resurrect > 0) {
                *raise = 0;
                hero_text(h, "has been resurrected", 10, 1);
                (*resurrect)--;
            } else {
                if (h->con > 0 && *raise > 0)
                    h->con--;
                if (h->hp_max > h->hp_base)
                    damage = h->hp_max - h->hp_base;
                else
                    damage = 0;
                factor = 0;
                if (h->con >= 0xe) {
                    for (index = 0; index <= 6; index++)
                        if (h->level[index] > 0)
                            if (index == 2 || index == 3 || index == 4 || index == 1)
                                factor += (h->con - 14) * h->level[index];
                            else if (h->con > 0xf)
                                factor += h->level[index] * 2;
                            else
                                factor += h->level[index];
                    if (factor > 0)
                        damage = damage / factor;
                    if (h->con < 0x11 || h->level[2] > 0 || h->level[3] > 0
                        || h->level[2] > h->dual_level)
                        h->hp_max -= damage;
                }
                hero_text(h, "has been raised from the dead", 10, 1);
                (*raise)--;
            }
        }
        if (*curse > 0 && find_effect(h, 0x24, &fx)) {
            spell_target = h;
            spell_remove_curse();
            hero_text(h, "is relieved of a curse", 10, 1);
            (*curse)--;
        }
        if (!changed && *sight > 0 && find_effect(h, 0x21, &fx)) {
            fx_remove(cur_node, 0x21, 0);
            hero_text(cur_node, "regains sight", 10, 1);
        }
    }
}

void far fix_menu(char far *interrupted)                         /* 31d4:284d */
{
    unsigned total;
    unsigned n1;
    unsigned n2;
    unsigned n3;
    unsigned saved[7];
    unsigned resurrect;
    unsigned raise;
    unsigned raise_need;
    unsigned curse;
    unsigned curse_need;
    unsigned sight;
    unsigned sight_need;
    unsigned heal;
    unsigned heal_need;
    unsigned detox;
    unsigned detox_need;
    unsigned flesh;
    unsigned flesh_need;
    unsigned restore;
    unsigned restore_need;
    unsigned n4;

    if (cursor_obj[2] == 0) {
        raise = 0;
        curse = 0;
        sight = 0;
        heal = 0;
        detox = 0;
        flesh = 0;
        resurrect = 0;
        restore = 0;
        *interrupted = 0;
        total = 0;
        if (party_wounds()) {
            fix_spells(&total, &resurrect, &sight, &flesh, &curse, &raise, &detox, &heal);
            mem_move(rest_time, saved, sizeof saved);
            heal_clock(&n4, &n3, &n2, &n1, &total);
            no_study = 1;
            *interrupted = camp_rest(0);
            no_study = 0;
            if (!*interrupted) {
                heal_roll(&total, &n4, &n3, &n2, &n1);
                fix_cures(&sight, &curse, &raise, &detox, &flesh, &heal, &restore,
                          &resurrect);
                heal_party(&total);
                party_display(cur_node);
                party_info_display();
                mem_move(saved, rest_time, sizeof saved);
                mem_move(rest_time, saved, sizeof saved);
                fix_needs(&raise_need, &curse_need, &sight_need, &heal_need, &detox_need,
                          &flesh_need, &restore_need);
                cure_clock(&curse_need, &sight_need, &detox_need, &flesh_need,
                           &heal_need, &raise_need, &restore_need, &sight, &curse,
                           &detox, &raise, &flesh, &heal, &resurrect, &restore);
                no_study = 1;
                *interrupted = camp_rest(0);
                no_study = 0;
                if (!*interrupted) {
                    fix_cures(&sight, &curse, &raise, &detox, &flesh, &heal, &restore,
                              &resurrect);
                    party_display(cur_node);
                    party_info_display();
                    mem_move(saved, rest_time, sizeof saved);
                }
            }
        }
    }
}

void far camp_menu(char far *interrupted)                        /* 31d4:2ab9 */
{
    unsigned char choice;
    char done;
    char saved;
    unsigned char count;

    picture_hides_roster = 0;
    saved_view_mode = view_mode;
    view_mode = 2;
    mem_fill(door_tries, 4, 1);
    heal_ticks = 0;
    mem_fill(rest_time, sizeof rest_time, 0);
    encounter_ticks = 0;
    str_copy(pic_saved, pic_name);
    view_kind_saved = view_kind;
    if (in_service_event == 0) {
        view_redraw();
        party_display(cur_node);
        text_clear(1, 0x11, 0x26, 0x16);
        status_printf(1, 0x12, 7, 0, camp_text);
    }
    scribe_reset_all();
    *interrupted = 0;
    choice = 0xff;
    saved = 1;
    while ((*interrupted == 0 || in_service_event) && choice != 8) {
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        count = 0;
        for (tick_slot = 0; tick_slot <= 8; tick_slot++)
            if (tick_slot != 2 && tick_slot != 4 || cursor_obj[0x2c] < 0x64) {
                if (test_mode) {
                    if (tick_slot == 5 || tick_slot == 6)
                        continue;
                } else if (tick_slot == 7)
                    continue;
                hotkey_add(tick_slot, &count);
            }
        ui_mode = saved;
        choice = node_prompt(empty_text, camp_menus[0], 1, 1);
        saved = ui_mode;
        if (menu_escaped) {
            if (choice == 0x1b)
                choice = 8;
            else {
                party_rows(cur_node, 0);
                party_key_select(choice);
                party_rows(cur_node, 1);
            }
        } else {
            switch (choice) {
            case 0:
                char_view(&done);
                break;
            case 1:
                magic_menu(interrupted);
                break;
            case 2:
                study_rest(interrupted);
                break;
            case 3:
                alter_menu();
                break;
            case 4:
                fix_menu(interrupted);
                break;
            case 5:
                if (cur_node == 0 || game_saved
                    || confirm("Game not saved. Load anyway? ", 0)) {
                    pending_load_set(1);
                    adventure_exit = 1;
                    return;
                }
                break;
            case 6:
                game_save_menu();
                if (confirm(exit_text, 0)) {
                    quit_requested = 1;
                    adventure_exit = 1;
                    choice = 8;
                }
                break;
            case 7:
                adventure_exit = 1;
                choice = 8;
                break;
            }
            if (choice != 8)
                box_draw();
        }
    }
    scribe_reset_all();
    view_mode = saved_view_mode;
    if (view_mode == 3) {
        frame_lower_panel();
        view_redraw();
    } else {
        picture_hides_roster = 0;
        frame_main();
        party_display(cur_node);
        party_info_display();
    }
    party_info_display();
    combat_text_clear();
    box_draw();
}
