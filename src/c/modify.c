/* CKIT.EXE overlay 06 module 22c2: modifying a party member.
 *
 * EXE 0x039c87..0x03b2e9, one Turbo C large-model _TEXT segment starting at
 * byte 7 of paragraph 22c2 in RTLink section 6 (payload 0x38da0, link
 * segment 21d4, parent section 3; sections 7 to 10 have the same link
 * segment).  The first 7 bytes of paragraph 22c2 are the tail of module
 * 2294, and section 6's table files every fixup site from 0x039c87 on
 * under frame 22c2; module 2428 begins at 2428:0009, after this module's
 * last retf.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are
 * defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and a mod18de_ callee is an RTLink vector
 * stub whose target is named beside it.
 *
 * _DATA is field and five literals, DGROUP 7070..70d7.  _BSS is nine
 * statics, DGROUP d192..d1c7, in DQK's names and order.
 *
 * click_set is reached through vector stub 18de:10ad and modify_hero
 * through 18de:1959; show_field is far-called from inside the module
 * ahead of its definition.  modify_hero edits cur_node in place: the six
 * abilities, the name and the maximum hit points, one field (field, 0..7)
 * at a time from the "Modify:" menu.  Only a member whose experience is
 * create_xp, half of it or a third of it may be changed.  Keep recomputes
 * what depends on the abilities; Exit puts back what was saved on entry.
 * This is DQK's modify.c with FRUA's Hero record (byte race, class and
 * gender).  hp_minimum gives class 4 alone the extra hit point below the
 * class limit and gives class 1 class 6's term above it; modify_keep
 * counts class 4's full limit above it; edit_cha has no race 3 class 3
 * case; and modify_hero has no HALIBUT test.
 */

struct Stat {
    unsigned char base;         /* +0                                     */
    unsigned char cur;          /* +1                                     */
};

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x2e];           /* +4   not read here                     */
    char readied;               /* +32                                    */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x38];           /* +c   not read here                     */
    long xp;                    /* +44                                    */
    char pad_48[0x10];          /* +48  not read here                     */
    unsigned char race;         /* +58  row of race_limits                */
    unsigned char cls;          /* +59  row of class_mins                 */
    char pad_5a[2];             /* +5a  not read here                     */
    unsigned char gender;       /* +5c  column of the strength limits     */
    char pad_5d[3];             /* +5d  not read here                     */
    char name[0x10];            /* +60  at most 15 characters             */
    struct Stat stats[6];       /* +70  str, int, wis, dex, con, cha      */
    unsigned char str_ex;       /* +7c  exceptional strength              */
    unsigned char str_ex_base;  /* +7d                                    */
    char pad_7e[3];             /* +7e  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[0x1b];          /* +82  not read here                     */
    unsigned char level[7];     /* +9d  one per class; 0: not that class  */
    char pad_a4[0x14];          /* +a4  not read here                     */
    unsigned char hp_base;      /* +b8                                    */
    char pad_b9[0x9a];          /* +b9  not read here                     */
    unsigned char known[0x10];  /* +153 one bit per spell, from 1         */
    unsigned char slots[4][9];  /* +163                                   */
    char pad_187[4];            /* +187 not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct RaceLimits {             /* 0x10 bytes a race                      */
    unsigned char str_min[2];   /* +0  by gender                          */
    unsigned char str_max[2];   /* +2                                     */
    unsigned char exc_max[2];   /* +4                                     */
    unsigned char int_min;      /* +6                                     */
    unsigned char int_max;      /* +7                                     */
    unsigned char wis_min;      /* +8                                     */
    unsigned char wis_max;      /* +9                                     */
    unsigned char dex_min;      /* +a                                     */
    unsigned char dex_max;      /* +b                                     */
    unsigned char con_min;      /* +c                                     */
    unsigned char con_max;      /* +d                                     */
    unsigned char cha_min;      /* +e                                     */
    unsigned char cha_max;      /* +f                                     */
};

struct ClassMins {              /* 6 bytes a class                        */
    unsigned char stat_min[6];  /* +0  str, int, wis, dex, con, cha       */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0  2 magic-user                       */
    char pad_2[0x14];           /* +2  not read here                      */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far show_hp(struct Hero far *h, char col, char row, char hilite, char max);
                                                                 /* 03d0:0789 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern char far con_hp_adj(struct Hero far *h, short arg);        /* 03d0:2045 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far status_char(char col, char row, char attr, unsigned char c); /* 06c9:059a */
extern void far status_msg(char far *fmt, char fg, char bg, ...); /* 06c9:08f7 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far menu_bar(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:13fd */
extern char far extra_row(void);                                 /* 083a:23b9 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern char far screen_sync(void);                               /* 0a75:0362 */
extern unsigned char far game_key_get(void);                        /* 0d48:0052 */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr);
                                                                 /* 0f5e:0040 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far item_equip_toggle(struct Item far *item);
                                /* 18de:135f, vector to overlay 14 28aa:19bf */
extern void far mu_learn(struct Hero far *hero);
                                /* 18de:1963, vector to overlay 7 21d4:334c */
extern void far spell_slots(struct Hero far *hero);
                                /* 18de:196d, vector to overlay 14 2ccd:019a */
extern char far hp_bonus(struct Hero far *hero, int cls);
                                /* 18de:1977, vector to overlay 14 28aa:3f31 */
extern unsigned char far hp_floor(struct Hero far *hero);
                                /* 18de:1981, vector to overlay 14 28aa:3a93 */
extern void far char_status(void);
                                /* 18de:198b, vector to overlay 14 28aa:0000 */
extern void far stat_show(int stat, char active);
                                /* 18de:1995, vector to overlay 14 28aa:0987 */
extern void far combat_show(void);
                                /* 18de:199f, vector to overlay 14 28aa:0679 */

extern struct RaceLimits race_limits[];                          /* 0fd2 */
extern struct ClassMins class_mins[];                            /* 11b8 */
extern struct SpellInfo spell_info[];                            /* 264a */
extern char far *cant_modify_text;                               /* 3ab0 */
extern unsigned char hit_dice_level_limit[];                                 /* 6262 */
extern unsigned char bit_mask[8];                                /* 6711 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern long create_xp;                                           /* b42b */
extern struct Hero far *node_list;                               /* d129 */

static unsigned char field = 0xff;                               /* 7070 */

static char picked;                                              /* d192 */
static unsigned char hp_save;                                    /* d193 */
static char dirty;                                               /* d194 */
static char extended;                                            /* d195 */
static char finished;                                            /* d196 */
static char old_name[0x29];                                      /* d197 */
static unsigned char exc_saved;                                  /* d1c0 */
static unsigned char name_col;                                   /* d1c1 */
static unsigned char stats_saved[6];                             /* d1c2 */

void far show_field(char active, unsigned char f);
static void mu_forget(void);

void far set_field(char f)                                       /* 32c2:0007 */
{
    if (f == 6 && field != 6) {
        pointer_hide();
        box_draw();
        status_printf(0, 0x18, 0xf, 8, "Edit Name: Press return when done.");
        show_field(0, 6);
    } else if (field == 6 && f != 6) {
        input_pending_clear();
        pointer_show();
    }
    field = f;
}

void far click_set(char c)                                       /* 32c2:0067 */
{
    picked = c;
}

char far click_get(void)                                         /* 32c2:0072 */
{
    return picked;
}

unsigned char far hp_minimum(struct Hero far *hero)              /* 32c2:0076 */
{
    unsigned char count;
    short total;
    unsigned char result;
    short i;
    unsigned char over;
    register int bonus;
    register int limit;

    bonus = 0;
    count = 0;
    total = 0;
    for (i = 0; i <= 6; i++) {
        if (hero->level[i] > 0) {
            limit = hit_dice_level_limit[i];
            if (hero->level[i] < limit) {
                total += hero->level[i];
                count++;
                if (i == 4)
                    total++;
                bonus += hp_bonus(hero, i) * hero->level[i];
            } else {
                count++;
                over = hero->level[i] - limit + 1;
                switch (i) {
                case 0:
                    total += over * 2 + 9;
                    break;
                case 2:
                case 3:
                    total += over * 3 + 9;
                    break;
                case 4:
                    total += over * 2 + 0xb;
                    break;
                case 5:
                    total += over + 0xb;
                    break;
                case 1:
                case 6:
                    total += over * 2 + 0xa;
                    break;
                }
                bonus += hp_bonus(hero, i) * (limit - 1);
            }
        }
    }
    if (bonus > 0)
        total = (total + bonus) / count;
    else
        total = total / count;
    result = total;
    return result;
}

void far modify_undo(void)                                       /* 32c2:01d8 */
{
    short i;

    for (i = 0; i <= 5; i++)
        cur_node->stats[i].cur = stats_saved[i];
    cur_node->str_ex = exc_saved;
    cur_node->hp_max = hp_save;
    cur_node->hp = cur_node->hp_max;
    str_copy(cur_node->name, old_name);
    recalc_stats(cur_node);
    finished = 1;
}

void far modify_keep(void)                                       /* 32c2:0250 */
{
    short j;
    short i;

    spell_slots(cur_node);
    if (cur_node->level[5] > 0) {
        mu_forget();
        mu_learn(cur_node);
    }
    hp_save = 0;
    tick_slot = 0;
    for (i = 0; i <= 6; i++) {
        if (cur_node->level[i] > 0) {
            if ((int)cur_node->level[i] < hit_dice_level_limit[i]) {    /* (int): cmp ax,dx, not a byte cmp */
                if (i == 4)
                    hp_save += con_hp_adj(cur_node, i) * (cur_node->level[i] + 1);
                else
                    hp_save += con_hp_adj(cur_node, i) * cur_node->level[i];
            } else if (i == 4)
                hp_save += con_hp_adj(cur_node, i) * hit_dice_level_limit[i];
            else
                hp_save += con_hp_adj(cur_node, i) * (hit_dice_level_limit[i] - 1);
            tick_slot++;
        }
    }
    hp_save = hp_save / tick_slot;
    cur_node->hp_base = cur_node->hp_max - hp_save;
    for (j = 0; j <= 5; j++)
        cur_node->stats[j].base = cur_node->stats[j].cur;
    cur_node->str_ex_base = cur_node->str_ex;
    finished = 1;
}

void far edit_str(unsigned char key)                             /* 32c2:03e2 */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        set_field(1);
        break;
    case 1:
    case 0x87:
    case 0x88:
        set_field(7);
        break;
    case 2:
    case 0x82:
        cur_node->stats[0].cur++;
        if (cur_node->stats[0].cur > race_limits[cur_node->race].str_max[cur_node->gender]) {
            if (cur_node->stats[0].cur > 18 &&
                (cur_node->level[2] > 0 || cur_node->level[3] > 0 ||
                 cur_node->level[4] > 0 || cur_node->level[1] > 0) &&
                race_limits[cur_node->race].exc_max[cur_node->gender] > cur_node->str_ex)
                cur_node->str_ex++;
            cur_node->stats[0].cur = race_limits[cur_node->race].str_max[cur_node->gender];
        }
        break;
    case 3:
    case 0x86:
        cur_node->stats[0].cur--;
        if (cur_node->str_ex > 0) {
            cur_node->str_ex--;
            cur_node->stats[0].cur++;
        } else if (cur_node->stats[0].cur < race_limits[cur_node->race].str_min[cur_node->gender])
            cur_node->stats[0].cur = race_limits[cur_node->race].str_min[cur_node->gender];
        if (cur_node->stats[0].cur < class_mins[cur_node->cls].stat_min[0])
            cur_node->stats[0].cur = class_mins[cur_node->cls].stat_min[0];
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 0);
}

void far edit_int(unsigned char key)                             /* 32c2:05ef */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        set_field(2);
        break;
    case 1:
    case 0x87:
    case 0x88:
        set_field(0);
        break;
    case 2:
    case 0x82:
        cur_node->stats[1].cur++;
        if (cur_node->stats[1].cur > race_limits[cur_node->race].int_max)
            cur_node->stats[1].cur = race_limits[cur_node->race].int_max;
        break;
    case 3:
    case 0x86:
        cur_node->stats[1].cur--;
        if (cur_node->stats[1].cur < race_limits[cur_node->race].int_min)
            cur_node->stats[1].cur = race_limits[cur_node->race].int_min;
        if (cur_node->stats[1].cur < class_mins[cur_node->cls].stat_min[1])
            cur_node->stats[1].cur = class_mins[cur_node->cls].stat_min[1];
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 1);
}

void far edit_wis(unsigned char key)                             /* 32c2:074c */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        set_field(3);
        break;
    case 1:
    case 0x87:
    case 0x88:
        set_field(1);
        break;
    case 2:
    case 0x82:
        cur_node->stats[2].cur++;
        if (cur_node->stats[2].cur > race_limits[cur_node->race].wis_max)
            cur_node->stats[2].cur = race_limits[cur_node->race].wis_max;
        if (cur_node->slots[0][0] > 0)
            cur_node->slots[0][0] = 1;
        break;
    case 3:
    case 0x86:
        cur_node->stats[2].cur--;
        if (cur_node->stats[2].cur < race_limits[cur_node->race].wis_min)
            cur_node->stats[2].cur = race_limits[cur_node->race].wis_min;
        if (cur_node->stats[2].cur < class_mins[cur_node->cls].stat_min[2])
            cur_node->stats[2].cur = class_mins[cur_node->cls].stat_min[2];
        if (cur_node->slots[0][0] > 0)
            cur_node->slots[0][0] = 1;
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 2);
}

void far edit_dex(unsigned char key)                             /* 32c2:08bf */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        set_field(4);
        break;
    case 1:
    case 0x87:
    case 0x88:
        set_field(2);
        break;
    case 2:
    case 0x82:
        cur_node->stats[3].cur++;
        if (cur_node->stats[3].cur > race_limits[cur_node->race].dex_max)
            cur_node->stats[3].cur = race_limits[cur_node->race].dex_max;
        break;
    case 3:
    case 0x86:
        cur_node->stats[3].cur--;
        if (cur_node->stats[3].cur < race_limits[cur_node->race].dex_min)
            cur_node->stats[3].cur = race_limits[cur_node->race].dex_min;
        if (cur_node->stats[3].cur < class_mins[cur_node->cls].stat_min[3])
            cur_node->stats[3].cur = class_mins[cur_node->cls].stat_min[3];
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 3);
}

void far edit_con(unsigned char key)                             /* 32c2:0a1c */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        set_field(5);
        break;
    case 1:
    case 0x87:
    case 0x88:
        set_field(3);
        break;
    case 2:
    case 0x82:
        cur_node->stats[4].cur++;
        if (cur_node->stats[4].cur > race_limits[cur_node->race].con_max)
            cur_node->stats[4].cur = race_limits[cur_node->race].con_max;
        if (hp_minimum(cur_node) > cur_node->hp_max)
            cur_node->hp_max = hp_minimum(cur_node);
        cur_node->hp = cur_node->hp_max;
        show_field(0, 4);               /* row 4; the decrement redraws row 7 */
        break;
    case 3:
    case 0x86:
        cur_node->stats[4].cur--;
        if (cur_node->stats[4].cur < race_limits[cur_node->race].con_min)
            cur_node->stats[4].cur = race_limits[cur_node->race].con_min;
        if (cur_node->stats[4].cur < class_mins[cur_node->cls].stat_min[4])
            cur_node->stats[4].cur = class_mins[cur_node->cls].stat_min[4];
        if (hp_floor(cur_node) < cur_node->hp_max)
            cur_node->hp_max = hp_floor(cur_node);
        cur_node->hp = cur_node->hp_max;
        show_field(0, 7);
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 4);
}

void far edit_cha(unsigned char key)                             /* 32c2:0c08 */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        if (!extra_row())
            set_field(6);
        else
            set_field(7);
        break;
    case 1:
    case 0x87:
    case 0x88:
        set_field(4);
        break;
    case 2:
    case 0x82:
        cur_node->stats[5].cur++;
        if (cur_node->stats[5].cur > race_limits[cur_node->race].cha_max)
            cur_node->stats[5].cur = race_limits[cur_node->race].cha_max;
        break;
    case 3:
    case 0x86:
        cur_node->stats[5].cur--;
        if (cur_node->stats[5].cur < race_limits[cur_node->race].cha_min)
            cur_node->stats[5].cur = race_limits[cur_node->race].cha_min;
        if (cur_node->stats[5].cur < class_mins[cur_node->cls].stat_min[5])
            cur_node->stats[5].cur = class_mins[cur_node->cls].stat_min[5];
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 5);
}

void far edit_name(void)                                         /* 32c2:0d79 */
{
    short key;
    struct Hero far *p;
    char found;
    register int k;

    extended = 0;
    dirty = 1;
    do {
        if ((timer_ticks() & 0x1f) == 0) {
            fill_rect(0x1f44, name_col * 4 + 0x1f44, 0x1f48, name_col * 4 + 0x1f48, 0x10f);
            while ((timer_ticks() & 0x1f) == 0)
                ;
        }
    } while (!screen_sync());
    key = game_key_get();
    if (key & 0x80)
        extended = 1;
    else
        extended = 0;
    if (key == 0x60)
        key = 0x1b;
    show_field(1, field);
    k = key;
    if (k >= 0x20 && k <= 0x7a)
        k = -1;
    switch (k) {
    case 0x8a:
        if (str_length(cur_node->name) > 0 &&
            str_length(cur_node->name) - 1 >= name_col) {
            if (str_length(cur_node->name) - 1 == name_col)
                cur_node->name[name_col] = 0;
            else if (str_length(cur_node->name))
                for (tick_slot = name_col + 1; str_length(cur_node->name) >= tick_slot;
                     tick_slot++)
                    cur_node->name[tick_slot - 1] = cur_node->name[tick_slot];
        }
        break;
    case 0xd:
    case 0x84:
    case 0x85:
        p = node_list;
        found = 0;
        while (p) {
            if (str_same(p->name, cur_node->name) && p != cur_node)
                found = 1;
            p = p->next;
        }
        if (found)
            status_msg("Already a %s in party!", 0xb, 0, cur_node->name);
        else {
            show_field(0, field);
            set_field(7);
        }
        break;
    case 0x87:
    case 0x88:
        p = node_list;
        found = 0;
        while (p) {
            if (str_same(p->name, cur_node->name) && p != cur_node)
                found = 1;
            p = p->next;
        }
        if (found)
            status_msg("Already a %s in party!", 0xb, 0, cur_node->name);
        else {
            show_field(0, field);
            set_field(5);
        }
        break;
    case 0x86:
        if (name_col == 0)
            name_col = str_length(cur_node->name);
        else
            name_col--;
        break;
    case 0x82:
        if (str_length(cur_node->name) <= name_col)
            name_col = 0;
        else
            name_col++;
        break;
    case 8:
        if (name_col > 0) {
            name_col--;
            if (str_length(cur_node->name) - 1 > name_col)
                for (tick_slot = name_col + 1; str_length(cur_node->name) >= tick_slot;
                     tick_slot++)
                    cur_node->name[tick_slot - 1] = cur_node->name[tick_slot];
            else if (str_length(cur_node->name) - 1 == name_col)
                cur_node->name[name_col] = 0;
        }
        break;
    case -1:
        if (str_length(cur_node->name) < 0xf) {
            for (tick_slot = str_length(cur_node->name) + 1; tick_slot > name_col; tick_slot--)
                cur_node->name[tick_slot] = cur_node->name[tick_slot - 1];
            cur_node->name[name_col] = key;
            cur_node->name[0xf] = 0;
            name_col++;
            if (name_col > 0xf)
                name_col = 0xf;
        }
        break;
    }
}

void far edit_hp(unsigned char key)                              /* 32c2:11d1 */
{
    switch (key) {
    case 0:
    case 0x84:
    case 0x85:
        set_field(0);
        break;
    case 1:
    case 0x87:
    case 0x88:
        if (extra_row())
            set_field(5);
        else
            set_field(6);
        break;
    case 2:
    case 0x82:
        cur_node->hp_max++;
        if (hp_floor(cur_node) < cur_node->hp_max)
            cur_node->hp_max = hp_floor(cur_node);
        cur_node->hp = cur_node->hp_max;
        break;
    case 3:
    case 0x86:
        cur_node->hp_max--;
        if (hp_minimum(cur_node) > cur_node->hp_max)
            cur_node->hp_max = hp_minimum(cur_node);
        cur_node->hp = cur_node->hp_max;
        break;
    case 4:
        modify_keep();
        break;
    case 5:
        modify_undo();
        break;
    }
    show_field(0, 7);
}

void far modify_hero(void)                                       /* 32c2:12dc */
{
    struct Item far *it;
    short i;
    char key;

    if (cur_node->xp != create_xp && cur_node->xp != create_xp / 2 &&
        cur_node->xp != create_xp / 3) {
        status_msg("%s%s", 0xb, 0, cur_node->name, cant_modify_text);
        return;
    }
    for (it = cur_node->items; it; it = it->next)
        if (it->readied)
            item_equip_toggle(it);
    char_status();
    for (i = 0; i <= 5; i++)
        stats_saved[i] = cur_node->stats[i].cur = cur_node->stats[i].base;
    exc_saved = cur_node->str_ex;
    hp_save = cur_node->hp_max;
    str_copy(old_name, cur_node->name);
    name_col = 0;
    if (!extra_row())
        show_field(0, 6);
    set_field(0);
    show_field(1, field);
    finished = 0;
    dirty = 1;
    finished = 0;
    while (!finished) {
        if (field != 6) {
            click_set(0);
            hotkey_reset(5);
            key = menu_bar("Modify:", "Next Previous Add Sub Keep Exit", 1, 0);
            if (click_get()) {
                show_field(0, field);
                set_field(key);
            }
        }
        if (!click_get()) {
            switch (field) {
            case 0:
                edit_str(key);
                break;
            case 1:
                edit_int(key);
                break;
            case 2:
                edit_wis(key);
                break;
            case 3:
                edit_dex(key);
                break;
            case 4:
                edit_con(key);
                break;
            case 5:
                edit_cha(key);
                break;
            case 6:
                edit_name();
                break;
            case 7:
                edit_hp(key);
                break;
            }
        } else
            click_set(0);
        combat_show();
        show_field(1, field);
    }
}

void far show_field(char active, unsigned char f)                /* 32c2:153e */
{
    switch (f) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        stat_show(f, active);
        break;
    case 7:
        show_hp(cur_node, 0x1f, 3, active, 0);
        break;
    case 6:
        if (active) {
            status_char(1, 1, 8, 0x10);
            status_printf(1, 1, 0xb, 8, cur_node->name);
        } else
            status_printf(1, 1, 7, 8, cur_node->name);
        break;
    }
}

static void mu_forget(void)                                      /* 32c2:15f1 */
{
    unsigned char i;

    for (i = 1; i <= 0x7e; i++)
        if (spell_info[i].kind == 2)
            cur_node->known[i / 8] = cur_node->known[i / 8] & ~bit_mask[i % 8];
                                        /* clears spell i + 1's bit, where the learner sets spell i's: as shipped */
}
