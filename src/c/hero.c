/* CKIT.EXE resident module 13d0: the party member record.
 * Deriving a member's combat figures from ability scores and readied
 * items (to-hit, damage dice, armour class, movement and load), the
 * ability-score tables behind them, item names and the item list,
 * effects, hit-point loss, the character panel and name line, caster
 * and class levels, readiness to train and a class level's experience,
 * freeing and dropping a member, and redrawing the view for the current
 * view mode.
 *
 * EXE 0x006100..0x00835b, one Turbo C large-model _TEXT segment starting
 * at byte 0 of paragraph 13d0.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Frame 10d7's last retf is at 0x0060ff, and module 15f5 starts at
 * 0x00835b (15f5:000b).  Ghidra's listing labels 1c13 only as a candidate
 * and has no label at 1a72, whose body it leaves undecoded after 18fe's
 * jump table.  Nothing found calls item_make (1c13).
 *
 * _DATA is DGROUP 0e56..0ec4: 16 stored literals; -d stores " " as the
 * tail of " No\t " and "%s" as the tail of "%s%s".  0ec5 is pad.  _BSS is
 * worn_load and frame_flag at a7f4..a7f6.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and hero_NNNN only where in this module
 * the routine starts.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is, so every
 * routine called ahead of its definition carries a prototype.
 */

#include <stdlib.h>

struct SpellInfo {
    int kind;                       /* +0 */
    char pad_2[0x14];               /* +2   not read here */
};

struct Effect {
    unsigned char kind;             /* +0 */
    char pad_1[0x5];                /* +1   not read here */
    struct Effect far *next;        /* +6 */
};

struct Fight {
    unsigned char casting;          /* +0 */
    char pad_1[0x2];                /* +1   not read here */
    int initiative;                 /* +3 */
    char pad_5[0xa];                /* +5   not read here */
    unsigned char dying;            /* +f */
    char pad_10[0x4];               /* +10  not read here */
    char not_pc;                    /* +14 */
};

struct Item {
    struct Item far *next;          /* +0 */
    char pad_4[0x1];                /* +4   not read here */
    char name[0x23];                /* +5 */
    unsigned char type;             /* +28 */
    unsigned char name_ids[3];      /* +29 */
    unsigned weight;                /* +2c */
    unsigned value;                 /* +2e */
    char plus;                      /* +30 */
    char save_bonus;                /* +31 */
    char readied;                   /* +32 */
    unsigned char unid_mask;        /* +33 */
    char cursed;                    /* +34 */
    unsigned char count;            /* +35 */
    unsigned char charges;          /* +36 */
    unsigned char effect;           /* +37 */
    unsigned char special;          /* +38 */
    char pad_39[0x1];               /* +39  not read here */
    struct Item far *more;          /* +3a */
};

struct ItemType {
    unsigned char slot;             /* +0 */
    unsigned char hands;            /* +1 */
    char pad_2[0x4];                /* +2   not read here */
    unsigned char armor_bonus;      /* +6 */
    char pad_7[0x2];                /* +7   not read here */
    unsigned char dice_n;           /* +9 */
    unsigned char dice_s;           /* +a */
    char dmg;                       /* +b */
    char pad_c[0x2];                /* +c   not read here */
    unsigned char flags;            /* +e */
    char pad_f[0x1];                /* +f   not read here */
};

struct Hero {
    struct Hero far *next;          /* +0 */
    struct Effect far *effects;     /* +4 */
    struct Item far *items;         /* +8 */
    struct Item far *equipped[13];  /* +c */
    struct Fight far *fight;        /* +40 */
    long xp;                        /* +44 */
    char pad_48[0x4];               /* +48  not read here */
    unsigned coins[3];              /* +4c */
    char pad_52[0x4];               /* +52  not read here */
    unsigned encumbrance;           /* +56 */
    unsigned char race;             /* +58 */
    char pad_59[0x5];               /* +59  not read here */
    unsigned char status;           /* +5e */
    unsigned char side;             /* +5f */
    char name[0x11];                /* +60 */
    unsigned char str;              /* +71 */
    char pad_72[0x5];               /* +72  not read here */
    unsigned char dex;              /* +77 */
    char pad_78[0x1];               /* +78  not read here */
    unsigned char con;              /* +79 */
    char pad_7a[0x2];               /* +7a  not read here */
    unsigned char str_ex;           /* +7c */
    char pad_7d[0x2];               /* +7d  not read here */
    unsigned char to_hit_base;      /* +7f */
    char pad_80[0x1];               /* +80  not read here */
    unsigned char hp_max;           /* +81 */
    char pad_82[0x6];               /* +82  not read here */
    unsigned char move_base;        /* +88 */
    char pad_89[0x1];               /* +89  not read here */
    unsigned char dual_level;       /* +8a */
    char pad_8b[0x9];               /* +8b  not read here */
    unsigned char sweep_targets;    /* +94 */
    char pad_95[0x8];               /* +95  not read here */
    unsigned char level[7];         /* +9d */
    unsigned char old_level[7];     /* +a4 */
    char pad_ab[0x2];               /* +ab  not read here */
    unsigned char dice_n_base[2];   /* +ad */
    unsigned char dice_s_base[2];   /* +af */
    char dmg_base[2];               /* +b1 */
    unsigned char ac_base;          /* +b3 */
    char pad_b4[0x9];               /* +b4  not read here */
    unsigned char icon_slot;        /* +bd */
    char pad_be[0x3];               /* +be  not read here */
    unsigned char item_count;       /* +c1 */
    unsigned char hands;            /* +c2 */
    char save_bonus;                /* +c3 */
    char pad_c4[0x1];               /* +c4  not read here */
    char level_up;                  /* +c5 */
    unsigned char spells[0x8d];     /* +c6 */
    char pad_153[0x2b];             /* +153 not read here */
    char active;                    /* +17e */
    char pad_17f[0x1];              /* +17f not read here */
    unsigned char to_hit;           /* +180 */
    unsigned char ac;               /* +181 */
    unsigned char ac_rear;          /* +182 */
    char pad_183[0x2];              /* +183 not read here */
    unsigned char dice_n[2];        /* +185 */
    unsigned char dice_s[2];        /* +187 */
    char dmg[2];                    /* +189 */
    unsigned char hp;               /* +18b */
    unsigned char movement;         /* +18c */
};

struct Area {                   /* what geo.area points at                    */
    char pad_0[0x120];          /* +0    not read here                    */
    unsigned char pics[2];      /* +120  picture numbers; 0 is the stock  */
};                              /*       0x80 / 0x81                       */

struct Geo {                    /* the party's place and the view state   */
    char pad_0[8];              /* +0   not read here                     */
    struct Area far *area;      /* +8   d41c  the current area's record   */
};

struct Pic;                         /* picview.c's picture window state */

struct Event {                      /* 0x14 bytes */
    char pad_0[0x6];                /* +0   not read here */
    unsigned char x6;               /* +6 */
    char pad_7[0xd];                /* +7   not read here */
};

extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far msg_pause(void);                                 /* 06c9:0953 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool); /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool); /* 0a75:02c8 */
extern char far pool_lowbit(int value);                          /* 0a75:032f */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far icon_clear(unsigned char idx);                   /* 0cb5:004d */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far rec_free(void far * far *pp);                    /* 0d48:0162 */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_main(void);                                /* 0d65:00de */
extern void far pic_frame(int col, int row, char show, unsigned char n); /* 18de:103f */
extern void far pic_open(char far *name, unsigned char id, char unused, struct Pic far *p);
                                                                 /* 18de:1049 */
extern void far view_refresh(void);                              /* 18de:1053 */
extern void far party_info_display(void);                              /* 18de:105d */
extern void far party_display(void far *node);                    /* 18de:1067 */
extern void far party_rows(struct Hero far *hero, char c);     /* 18de:1071 */
extern void far big_redraw(void);                              /* 18de:107b */

extern long step_ec6[];                                          /* 0ec6 */
extern long level_xp_table[][12];                                       /* 13a4 */
extern unsigned char max_level[][7];                             /* 1c18 */
extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern char far *status_names[];                                 /* 39b0 */
extern char far *item_words[];                                   /* 3d03 */
extern char helpless_kinds[4];                                   /* 616d */
extern char byte_6723;                                           /* 6723 */
extern struct Geo geo;                                           /* d414 */
extern struct Event far *event_table;                               /* a7f0 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern char redrawn;                                             /* b3f3 */
extern struct ItemType far *item_table;                          /* b40a */
extern unsigned char text_row;                                   /* b41a */
extern int view_mode;                                            /* b421 */
extern char resting;                                             /* b4eb */
extern unsigned char attack_rate;                                /* b723 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char picture_hides_roster;                                           /* b761 */
extern struct Pool party_pool;                                    /* b76e */
extern unsigned char side_count[];                               /* b8d1 */
extern struct Item far *cur_item;                                /* bcfd */
extern struct Pool npc_pool;                                    /* bd01 */
extern char item_cast_active;                                           /* be69 */
extern struct Pool effect_pool;                                    /* c3c4 */
extern char shop_temple_frame;                                           /* c672 */
extern struct Pic pic;                                           /* ca22 */
extern struct Pool item_record_pool;                                    /* cb0e */
extern struct Pool combat_pool;                                  /* ce72 */
extern char panel_dirty;                                         /* d0fb */
extern struct Hero far *node_list;                               /* d129 */

void far hero_text(struct Hero far *h, char far *s, char row, char pause);
char far is_helpless(struct Hero far *h);
char far dex_defense(struct Hero far *h);
char far dex_missile(struct Hero far *h);
char far str_hit(struct Hero far *h);
char far str_damage(struct Hero far *h);
int  far str_carry(struct Hero far *h);
void far combat_text_clear(void);
void far character_name_show(struct Hero far *h, char col, char row, char poss);
char far find_effect(struct Hero far *h, unsigned char kind, struct Effect far * far *out);
unsigned char far first_level(struct Hero far *h);
unsigned char far class_level(struct Hero far *h, int cls);
long far level_xp_threshold(struct Hero far *h, unsigned char level, short cls);

static int worn_load;                                            /* a7f4 */
static char frame_flag;                                          /* a7f6 */

void far hero_frame_flag_set(char c)                                    /* 13d0:0000 */
{
    frame_flag = c;
}

void far weapon_stats(struct Hero far *h)                        /* 13d0:000b */
{
    struct Item far *w;
    unsigned char type;
    char bonus;

    w = h->equipped[0];
    if (w == 0)
        return;
    type = w->type;
    h->to_hit = h->to_hit_base;
    if (item_table[type].flags & 2)
        h->to_hit = h->to_hit + dex_missile(h);
    h->dmg[0] = item_table[type].dmg;
    if (item_table[type].flags & 4) {
        h->to_hit = h->to_hit + str_hit(h);
        h->dmg[0] = h->dmg[0] + str_damage(h);
    }
    bonus = w->plus;
    if (item_table[type].flags & 0x80 && h->equipped[12] != 0)
        bonus = h->equipped[12]->plus + bonus;
    if ((item_table[type].flags & 1) > 0 && h->equipped[11] != 0)
        bonus = h->equipped[11]->plus + bonus;
    h->dmg[0] = h->dmg[0] + bonus;
    if (h->race == 0) {
        if (w->type >= 0x15 && w->type <= 0x1b || w->type == 0x13 || w->type == 0x12)
            bonus++;
    }
    h->to_hit = h->to_hit + bonus;
    h->dice_n[0] = item_table[type].dice_n;
    h->dice_s[0] = item_table[type].dice_s;
}

void far armor_move(struct Hero far *h, struct Item far *it)     /* 13d0:01c4 */
{
    if (item_table[it->type].slot == 2) {
        if (it->weight < 0 || it->weight > 0x18f)
            h->movement = 6;
        else if (it->weight <= 0x96)
            h->movement = h->move_base;
        else
            h->movement = 9;
        if (it->plus != 0 && h->movement <= 9)
            h->movement = h->movement + 3;
    }
}

void far item_ac(struct Hero far *h, struct Item far *it, char far *m,
               char far *flag)                                   /* 13d0:0243 */
{
    unsigned char bonus;
    short kind;

    bonus = item_table[it->type].armor_bonus;
    if (!(bonus & 0x80))
        return;
    bonus = bonus & 0x7f;
    kind = item_table[it->type].slot;
    if (kind == 1)
        m[1] = it->plus + bonus;
    else if (bonus == 0) {
        if (kind == 9) {
            if (it->plus > m[3])
                m[3] = it->plus;
        } else if (kind == 7)
            m[4] = it->plus;
        else
            m[2] = m[2] + it->plus;
        h->save_bonus = h->save_bonus + it->save_bonus;
    } else if (it->plus + bonus > m[5]) {
        m[5] = it->plus + bonus;
        if (it->plus > 0 && kind == 2)
            *flag = 1;
    }
}

void far load_move(struct Hero far *h)                           /* 13d0:0361 */
{
    char lim;
    register int load;

    load = h->encumbrance - str_carry(h);
    if (load < 0)
        load = 0;
    if (load < 0 || load > 0x400)
        lim = 3;
    else if (load <= 0x200)
        lim = h->movement;
    else if (load <= 0x300)
        lim = 9;
    else
        lim = 6;
    if (h->movement > lim)
        h->movement = lim;
}

void far item_name(struct Hero far *h, struct Item far *it, char col, char row,
               char full, char show)                             /* 13d0:03d1 */
{
    unsigned char i;
    unsigned char mask;
    struct Effect far *fx;
    char found;
    char plural;
    struct Hero far *node;
    char buf[42];

    it->name[0] = 0;
    if (full) {
        if (it->readied)
            str_copy(it->name, " Yes\t");
        else
            str_copy(it->name, " No\t ");
    }
    found = 0;
    for (node = node_list; node != 0 && found == 0; node = node->next)
        if (find_effect(node, 5, &fx))
            found = 1;
    if (found && (it->plus > 0 || it->save_bonus > 0 || it->cursed != 0))
        str_cat(it->name, "* ");
    if (it->count > 0 && it->type != 0x49) {
        str_cat(it->name, byte_str(it->count));
        str_cat(it->name, " ");
    }
    mask = 0;
    for (i = 1; i <= 3; i++)
        if (it->name_ids[i - 1] != 0 && !((it->unid_mask >> (3 - i)) & 1))
            mask += 1 << (i - 1);
    plural = 0;
    for (i = 3; i >= 1; i--) {
        if (((mask >> (i - 1)) & 1) > 0) {
            if (it->type == 0x49 && i == 2) {
                str_copy(buf, msg_format("%d", it->count));
                str_copy(it->name, msg_format("%s%s", it->name, buf));
            } else
                str_copy(it->name, msg_format("%s%s", it->name, item_words[it->name_ids[i - 1]]));
            if (it->count >= 2 && !plural &&
                ((1 << (i - 1)) == mask ||
                 i == 1 && mask > 4 ||
                 i == 1 && it->type == 0x49 ||
                 i == 2 && !pool_lowbit(mask) ||
                 i == 3 && (it->type == 0x37 || it->type == 0x1e || it->type == 5 ||
                            it->type == 7 || it->type == 0xc) && it->name_ids[2] != 0x5d)) {
                str_cat(it->name, "s ");
                plural = 1;
            } else
                str_cat(it->name, " ");
        }
    }
    if (show)
        status_printf(col, row, 7, 0, it->name);
}

void far show_ac(struct Hero far *h, char col, char row)         /* 13d0:06f9 */
{
    char buf[0x52];
    register int ac;

    ac = h->ac - 0x3c;
    str_copy(buf, byte_str(abs(h->ac - 0x3c)));              /* Turbo C's inline abs: cwd, xor, sub */
    str_cat(buf, " ");
    if (ac > 0)
        str_copy(buf, msg_format("-%s", buf));
    status_printf(col, row, 7, 0, buf);
}

void far show_hp(struct Hero far *h, char col, char row, char hilite,
               char max)                                         /* 13d0:0789 */
{
    char color;
    char len;
    char buf[42];

    if (h->hp < h->hp_max)
        color = 0xb;
    else
        color = 7;
    if (hilite)
        color = 0xb;
    str_copy(buf, byte_str(h->hp));
    status_printf(col, row, color, 0, buf);
    if (max) {
        len = str_length(buf);
        status_printf(col + len, row, 7, 0, msg_format("/%d", h->hp_max));
    }
}

void far hero_panel(struct Hero far *h)                           /* 13d0:0834 */
{
    unsigned char row;

    if (!panel_dirty)
        return;
    panel_dirty = 0;
    text_clear(0x17, 1, 0x26, 0x15);
    row = 1;
    hero_text(h, " ", row, 0);
    row++;
    status_printf(0x17, row + 1, 7, 0, "Hitpoints");
    show_hp(h, 0x21, row + 1, 0, 0);
    row += 2;
    status_printf(0x17, row + 1, 7, 0, "AC");
    show_ac(h, 0x1a, row + 1);
    text_row = row + 1;
    if (h->equipped[0] != 0) {
        row += 2;
        item_name(h, h->equipped[0], 0, 0, 0, 0);
        text_wrap(0x17, row + 1, 0x26, row + 3, 7, 0, 1, h->equipped[0]->name);
    }
    row = text_row + 1;
    if (h->active == 0)
        status_printf(0x17, row + 1, 0xb, 0, status_names[h->status]);
    else if (is_helpless(h))
        status_printf(0x17, row + 1, 0xb, 0, "(Helpless)");
    else if (h->fight->casting > 0)
        status_printf(0x17, row + 1, 0xb, 0, "(Casting)");
}

char far is_helpless(struct Hero far *h)                           /* 13d0:09f2 */
{
    char res;
    unsigned char i;
    struct Effect far *fx;

    res = 0;
    for (i = 0; i < 4; i++)
        if (find_effect(h, helpless_kinds[i], &fx))
            res = 1;
    return res;
}

unsigned char far calc_move(struct Hero far *h)                  /* 13d0:0a38 */
{
    unsigned char ret;
    struct Item far *it;
    struct Effect far *fx;

    h->movement = h->move_base;
    for (it = h->items; it != 0; it = it->next)
        if (it->readied)
            armor_move(h, it);
    load_move(h);
    if (find_effect(h, 0x27, &fx))
        h->movement = h->movement * 2;
    if (find_effect(h, 0x2a, &fx))
        h->movement = h->movement / 2;
    if (find_effect(h, 0x4a, &fx))
        h->movement = h->movement * 2;
    attack_rate = h->movement;
    if (h->side == 0) {
        if ((char)cursor_obj[0x1c] + attack_rate < 1) /* (char): cbw */
            attack_rate = 1;
        else
            attack_rate = (char)cursor_obj[0x1c] + attack_rate;
    }
    attack_rate = attack_rate * 2;
    if (find_effect(h, 0xb2, &fx))
        attack_rate = 0;
    ret = attack_rate;
    return ret;
}

void far recalc_stats(struct Hero far *h)                           /* 13d0:0b88 */
{
    unsigned char i;
    short n;
    struct Item far *it;
    char flag;
    char mods[6];
    short j;
    short k;
    unsigned short w;
    unsigned char kind;

    for (j = 0; j <= 0xc; j++)
        h->equipped[j] = 0;
    h->item_count = 0;
    it = h->items;
    h->hands = 0;
    h->encumbrance = 0;
    worn_load = 0;
    for (; it != 0; it = it->next) {
        h->item_count++;
        w = it->weight;
        if (it->count > 0)
            w = it->count * w;
        h->encumbrance += w;
        if (it->readied) {
            worn_load += w;
            kind = item_table[it->type].slot;
            switch (kind) {
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
                h->equipped[kind] = it;
                break;
            case 9:
                if (h->equipped[9] != 0) {
                    if (h->equipped[10] == 0)
                        h->equipped[10] = it;
                } else
                    h->equipped[9] = it;
                break;
            }
            if (it->type == 0x1e)
                h->equipped[11] = it;
            if (it->type == 0xc || it->type == 0x4a)
                h->equipped[12] = it;
            h->hands = h->hands + item_table[it->type].hands;
        }
    }
    for (k = 0; k <= 2; k++)
        h->encumbrance = h->encumbrance + h->coins[k];
    for (i = 1; i <= 2; i++) {
        h->dice_n[i - 1] = h->dice_n_base[i - 1];
        h->dice_s[i - 1] = h->dice_s_base[i - 1];
        h->dmg[i - 1] = h->dmg_base[i - 1];
    }
    mem_zero(mods, 6);
    flag = 0;
    h->save_bonus = 0;
    h->ac = h->ac_base;
    h->movement = h->move_base;
    h->to_hit = h->to_hit_base;
    mods[0] = dex_defense(h);
    if (h->equipped[0] == 0) {
        h->to_hit = h->to_hit + str_hit(h);
        h->dmg[0] = h->dmg[0] + str_damage(h);
    }
    weapon_stats(h);
    for (it = h->items; it != 0; it = it->next)
        if (it->readied) {
            armor_move(h, it);
            item_ac(h, it, mods, &flag);
        }
    if (flag) {
        if (mods[3] > 0)
            mods[3] = 0;
        if (mods[4] > 0 && (h->equipped[7] == 0 || h->equipped[7]->type != 0x3a))
            mods[4] = 0;
    }
    h->movement = calc_move(h) / 2;
    if (h->ac > mods[5])
        mods[5] = h->ac;
    h->ac = 0;
    for (n = 0; n <= 5; n++)
        h->ac = h->ac + mods[n];
    h->ac_rear = mods[5] + mods[2] + mods[4] + mods[3] - 2;
    h->sweep_targets = 1;
    if (h->race != 6) {
        h->sweep_targets = int_max(h->sweep_targets, class_level(h, 2));
        h->sweep_targets = int_max(h->sweep_targets, class_level(h, 4));
        h->sweep_targets = int_max(h->sweep_targets, class_level(h, 3));
    }
}

char far dex_defense(struct Hero far *h)                         /* 13d0:0ff9 */
{
    char r;
    unsigned char dex;

    dex = h->dex;
    if (dex >= 1 && dex <= 3)
        r = -4;
    else if (dex >= 4 && dex <= 6)
        r = dex - 7;
    else if (dex >= 15 && dex <= 18)
        r = dex - 14;
    else if (dex >= 0x13 && dex <= 0x14)
        r = 4;
    else if (dex >= 0x15 && dex <= 0x17)
        r = 5;
    else if (dex >= 0x18 && dex <= 0x19)
        r = 6;
    else
        r = 0;
    return r;
}

char far dex_missile(struct Hero far *h)                         /* 13d0:1089 */
{
    char r;
    unsigned char dex;

    dex = h->dex;
    if (dex < 0 || dex > 0x19)                /* unsigned < 0: emitted, never true */
        r = 0;
    else if (dex <= 2)
        r = -4;
    else if (dex <= 5)
        r = dex - 6;
    else if (dex <= 15)
        r = 0;
    else if (dex <= 0x12)
        r = dex - 15;
    else if (dex <= 0x14)
        r = 3;
    else if (dex <= 0x17)
        r = 4;
    else
        r = 5;
    return r;
}

unsigned char far str_rank(struct Hero far *h)                  /* 13d0:1100 */
{
    unsigned char r;
    unsigned char str;
    unsigned char ext;

    str = h->str;
    if (str >= 0 && str <= 0x11)
        r = h->str;
    else if (str == 0x12) {
        ext = h->str_ex;
        if (ext >= 0) {
            if (ext == 0)
                r = 0x12;
            else if (ext <= 0x32)
                r = 0x13;
            else if (ext <= 0x4b)
                r = 0x14;
            else if (ext <= 0x5a)
                r = 0x15;
            else if (ext <= 0x63)
                r = 0x16;
            else if (ext == 0x64)
                r = 0x17;
        }
    } else if (str >= 0x13 && str <= 0x19)
        r = h->str + 5;
    return r;                                 /* unset past str 0x19 or str_ex 0x64, as shipped */
}

char far str_hit(struct Hero far *h)                             /* 13d0:11a3 */
{
    char r;
    unsigned char s;

    r = 0;
    s = str_rank(h);
    if (s > 0) {
        if (s <= 3)
            r = -3;
        else if (s <= 5)
            r = -2;
        else if (s <= 7)
            r = -1;
        else if (s > 0x10) {
            if (s <= 0x13)
                r = 1;
            else if (s <= 0x16)
                r = 2;
            else if (s <= 0x19)
                r = 3;
            else if (s <= 0x1b)
                r = 4;
            else if (s <= 0x1e)
                r = s - 0x17;
        }
    }
    return r;
}

char far str_damage(struct Hero far *h)                           /* 13d0:1234 */
{
    char r;
    unsigned char s;

    r = 0;
    s = str_rank(h);
    if (s > 0) {
        if (s <= 2)
            r = -2;
        else if (s <= 5)
            r = -1;
        else if (s > 0xf) {
            if (s == 0x10)
                r = 1;
            else if (s <= 0x13)
                r = s - 0x10;
            else if (s <= 0x1d)
                r = s - 0x11;
            else if (s == 0x1e)
                r = 0xe;
        }
    }
    return r;
}

int far str_carry(struct Hero far *h)                            /* 13d0:12ae */
{
    unsigned char s;
    register int r;

    s = str_rank(h);
    if (s > 0) {
        if (s <= 3)
            r = -350;
        else if (s <= 5)
            r = -250;
        else if (s <= 7)
            r = -150;
        else if (s <= 0xb)
            r = 0;
        else if (s <= 0xd)
            r = 100;
        else if (s <= 0xf)
            r = 200;
        else if (s <= 0x10)
            r = 350;
        else if (s <= 0x15)
            r = (s - 0x11) * 250 + 500;
        else if (s <= 0x1a)
            r = (s - 0x16) * 1000 + 2000;
        else if (s <= 0x1b)
            r = 7500;
        else if (s <= 0x1e)
            r = (s - 0x1c) * 3000 + 9000;
        else
            r = 0;
    }
    return r;                                 /* unset for s == 0, as shipped */
}

void far spell_mark(struct Hero far *h, unsigned char spell)     /* 13d0:1381 */
{
    unsigned char i;

    for (i = 0; i <= 0x8c; i++)
        if (h->spells[i] == spell) {
            h->spells[i] = spell | 0x80;
            break;
        }
}

void far item_remove(struct Hero far *h, struct Item far *it)      /* 13d0:13c4 */
{
    struct Item far *p;

    if (it->type == 0x49)
        while (it->more != 0) {
            p = it->more;
            it->more = p->more;
            rec_free((void far * far *)&p);
        }
    p = h->items;
    if (p == it)
        h->items = it->next;
    else
        while (p != 0 && p->next != it)
            p = p->next;
    p->next = it->next;                       /* p is 0 when it is not listed, as shipped */
    it->next = 0;
    rec_free((void far * far *)&it);
}

void far item_append(struct Hero far *h, struct Item far *src)     /* 13d0:149c */
{
    struct Item far *p;
    struct Item far *n;

    pool_alloc((void far * far *)&n, 0x3e, &item_record_pool);
    mem_move5(src, n, 0x3e);
    n->next = 0;
    p = h->items;
    if (p == 0)
        h->items = n;
    else {
        while (p->next != 0)
            p = p->next;
        p->next = n;
    }
}

void far bottom_msg(char far *msg)                                /* 13d0:152e */
{
    box_draw();
    status_printf(0, 0x18, 0xf, 8, "%s", msg);
    msg_pause();
    box_draw();
}

void far hero_text(struct Hero far *h, char far *s, char row, char pause) /* 13d0:1560 */
{
    char top;
    char none;

    if (h == 0)
        none = 1;
    else
        none = 0;
    if (view_mode == 5) {
        text_clear(0x17, row, 0x26, 0x15);
        if (!none)
            character_name_show(h, 0x17, row, 0);
        text_wrap(0x17, row + 1, 0x26, 0x15, 7, 0, 1, s);
    } else {
        if (resting)
            top = 0x12;
        else
            top = 0x11;
        text_clear(1, top, 0x26, 0x16);
        if (!none)
            character_name_show(h, 1, top + 1, 0);
        text_wrap(1, top + 2, 0x26, 0x16, 7, 0, 1, s);
    }
    if (pause) {
        msg_pause();
        combat_text_clear();
    }
}

void far combat_text_clear(void)                                         /* 13d0:1657 */
{
    if (view_mode == 5)
        text_clear(0x17, 0xa, 0x26, 0x15);
    else
        text_clear(1, 0x11, 0x26, 0x16);
}

void far character_name_show(struct Hero far *h, char col, char row, char poss) /* 13d0:1689 */
{
    char color;

    if (h->active == 0)
        color = 0;
    else if (h->side == 1 && view_mode == 5)
        color = 2;
    else if (h->level_up != 0)
        color = 0xd;
    else
        color = 7;
    status_printf(col, row, color, 0, "%s", h->name);
    if (poss)
        status_printf(col + str_length(h->name), row, color, 0, "'s");
}

char far find_effect(struct Hero far *h, unsigned char kind, struct Effect far * far *out) /* 13d0:1726 */
{
    char ret;
    char found;

    found = 0;
    *out = h->effects;
    while (*out != 0 && found == 0) {
        if ((*out)->kind == kind)
            found = 1;
        else
            *out = (*out)->next;
    }
    ret = found;
    return ret;
}

void far hero_damage(struct Hero far *h, unsigned short dmg)     /* 13d0:1792 */
{
    unsigned short over;
    unsigned char hp;

    over = 0;
    hp = 0;
    if (h->hp >= dmg)
        hp = h->hp - dmg;
    else
        over = dmg - h->hp;
    if (cursor_obj[0x1d] == 0 && (over > 9 || hp == 0 && h->status == 1))
        h->status = 6;
    else if (over > 0) {
        h->status = 5;
        if (view_mode == 5)
            h->fight->dying = over;
    } else if (hp == 0)
        h->status = 4;
    if (h->status != 0 && h->status != 1) {
        h->active = 0;
        h->hp = 0;
        if (view_mode == 5) {
            side_count[h->side]--;
            h->fight->initiative = 0;
        }
    } else
        h->hp = hp;
}

void far heal_msg(struct Hero far *h)                           /* 13d0:1878 */
{
    char buf[42];

    if (h->hp == h->hp_max)
        str_copy(buf, "is fully healed");
    else
        str_copy(buf, "is partially healed");
    hero_text(h, buf, 10, 1);
    if (view_mode != 5)
        party_rows(h, h == cur_node);
}

unsigned char far caster_level(unsigned char spell, char cap)       /* 13d0:18fe */
{
    signed char ret;
    signed char r;
    signed char r2;

    if (class_level(cur_node, 0) == 0 && class_level(cur_node, 5) == 0 &&
        class_level(cur_node, 4) < 8 && class_level(cur_node, 1) < 9)
        r = 6;
    else
        switch (spell_info[spell].kind) {     /* r unset for a kind below 0, 3 or past 4, as shipped */
        case 0:
            r = class_level(cur_node, 0);
            r2 = class_level(cur_node, 3);
            if (r < r2)
                r = r2;
            break;
        case 1:
            r = class_level(cur_node, 4) - 7;
            if (r < 0)
                r = 0;
            break;
        case 2:
            r = class_level(cur_node, 5);
            r2 = class_level(cur_node, 4) - 8;
            if (r < r2)
                r = r2;
            break;
        case 4:
            r = 0xc;
            break;
        }
    if (item_cast_active && spell_info[spell].kind != 4)
        r = 6;
    if (cap && r > 0x1f)
        r = 0x1f;
    ret = r;
    return ret;
}

void far view_redraw(void)                                         /* 13d0:1a72 */
{
    redrawn = 1;
    if (cursor_obj[0x85] == 0 || event_table[cursor_obj[0x85] - 1].x6 < 0xf0 || byte_6723 == 0)
        switch (view_mode) {
        case 0:
            frame_border();
            break;
        case 1: case 10:
            if (picture_hides_roster)
                big_redraw();
            else {
                if (shop_temple_frame)
                    frame_main();
            redraw_pic:
                pic_frame(3, 3, 1, 0);
            redraw_node:
                party_display(cur_node);
                party_info_display();
            }
            break;
        case 2:
            frame_main();
            if (geo.area->pics[0] == 0)
                pic_open("PIC", 0x80, 0, &pic);
            else
                pic_open("PIC", geo.area->pics[0], 0, &pic);
            goto redraw_pic;                  /* shipped jumps back into case 1 (1aef) */
        case 4:
            frame_main();
            view_refresh();
            goto redraw_node;                 /* shipped jumps back into case 1 (1b02) */
        case 3:
            if (cursor_obj[0x24] == 1)
                view_refresh();
            break;
        case 6:
            frame_main();
            if (geo.area->pics[1] == 0)
                pic_open("PIC", 0x81, 0, &pic);
            else
                pic_open("PIC", geo.area->pics[1], 0, &pic);
            pic_open("PIC", 0x81, 0, &pic);
            pic_frame(3, 3, 1, 0);
            party_display(cur_node);
            break;
        }
    else
        big_redraw();
}

struct Item far *far item_make(unsigned char type, char w1, char w2, char w3,
                               char plus, char plus2, char ready, char hide,
                               char x34, int weight, char count, int x2e,
                               char x36, char x37, char x38)     /* 13d0:1c13 */
{
    struct Item far *ret;
    struct Item far *it;

    pool_alloc((void far * far *)&it, 0x3e, &item_record_pool);
    mem_fill(it, 0x3e, 0);
    it->type = type;
    it->name_ids[0] = w1;
    it->name_ids[1] = w2;
    it->name_ids[2] = w3;
    it->plus = plus;
    it->save_bonus = plus2;
    it->readied = ready;
    it->unid_mask = hide;
    it->cursed = x34;
    it->weight = weight;
    it->count = count;
    it->value = x2e;
    it->charges = x36;
    it->effect = x37;
    it->special = x38;
    ret = it;
    return ret;
}

void far hero_free(struct Hero far * far *pp)                    /* 13d0:1cbf */
{
    struct Item far *a;
    struct Item far *b;
    struct Effect far *e2;
    struct Effect far *e;
    char k;

    k = 0;
    if ((*pp)->fight != 0) {
        k = (*pp)->fight->not_pc;
        pool_free((*pp)->fight, 0x18, &combat_pool);
    }
    cur_item = (*pp)->items;
    while (cur_item != 0) {
        if (cur_item->type == 0x49) {
            a = cur_item->more;
            for (tick_slot = 1; cur_item->count >= (unsigned char)tick_slot; tick_slot++)
                if (a != 0) {
                    b = a;
                    a = a->more;
                    pool_free(b, 0x3e, &item_record_pool);
                }
        }
        b = cur_item;
        cur_item = cur_item->next;
        pool_free(b, 0x3e, &item_record_pool);
    }
    (*pp)->items = 0;
    e = (*pp)->effects;
    while (e != 0) {
        e2 = e;
        e = e->next;
        pool_free(e2, 0xa, &effect_pool);
    }
    if (k == 1 || (*pp)->side == 1)
        pool_free(*pp, 0x18e, &npc_pool);
    else
        pool_free(*pp, 0x18e, &party_pool);
    *pp = 0;
}

void far hero_drop(char keep, char icon)                         /* 13d0:1e69 */
{
    struct Hero far *p;

    if (node_list == 0)
        return;
    p = node_list;
    if (p != cur_node) {
        while (p->next != cur_node && p->next != 0)
            p = p->next;
        if (p->next != 0) {
            if (icon)
                icon_clear(cur_node->icon_slot);
            p->next = cur_node->next;
            hero_free(&cur_node);
            cur_node = p;
            if (!keep)
                cursor_obj[0x20]--;
        }
    } else {                                  /* -O cross-jumps the first arm's tail into this arm */
        if (icon)
            icon_clear(cur_node->icon_slot);
        node_list = cur_node->next;
        p = cur_node->next;
        hero_free(&cur_node);
        cur_node = p;
        if (!keep)
            cursor_obj[0x20]--;
    }
    if (cur_node == 0)
        cur_node = node_list;
}

char far can_level_up(struct Hero far *h)                           /* 13d0:1f71 */
{
    char capped;
    char ready;
    short i;
    unsigned char lvl;
    long need;

    capped = 0;
    ready = 0;
    for (i = 0; i <= 6 && ready == 0; i++)
        if (h->level[i] > 0 && h->level[i] < 0x28) {
            lvl = h->level[i];
            need = level_xp_threshold(h, lvl + 1, i);
            capped = 0;
            if (max_level[h->race][i] <= lvl)
                capped = 1;
            if (h->xp >= need && need > 0 && !capped)
                ready = 1;
        }
    return ready;
}

/* `2e ff a7 65 20` is Turbo C's dense switch, jmp cs:[bx + table]. */
char far con_hp_adj(struct Hero far *h, short arg)                /* 13d0:2045 */
{
    char t;
    char r;

    switch (h->con) {
    case 3:
        r = -2;
        break;
    case 4: case 5: case 6:
        r = -1;
        break;
    case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14:
        r = 0;
        break;
    case 15:
        r = 1;
        break;
    case 16:
        r = 2;
        break;
    case 17: case 18: case 19:
        if (arg == 2 || arg == 4 || arg == 3 || arg == 1)
            r = h->con - 14;
        else
            r = 2;
        break;
    }
    t = r;                                    /* unset outside con 3..19, as shipped */
    return t;
}

char far chance(unsigned char pct)                               /* 13d0:20d6 */
{
    char r;

    r = (unsigned)random_n(100) < pct;
    return r;
}

char far old_class_abilities_available(struct Hero far *h)                           /* 13d0:2100 */
{
    char r;

    r = first_level(h) > h->dual_level;
    return r;
}

unsigned char far first_level(struct Hero far *h)                /* 13d0:212e */
{
    unsigned char r;
    short i;

    if (h->race != 5)
        r = 0;
    else {
        for (i = 0; i < 6 && h->level[i] == 0; i++)
            ;
        r = h->level[i];
    }
    return r;
}

unsigned char far class_level(struct Hero far *h, int cls)         /* 13d0:2177 */
{
    unsigned char r;
    unsigned char lvl;
    unsigned char prev;

    lvl = h->level[cls];
    if (old_class_abilities_available(h))
        prev = h->old_level[cls];
    else
        prev = 0;
    if (lvl > prev)
        r = lvl;
    else
        r = prev;
    return r;
}

long far level_xp_threshold(struct Hero far *h, unsigned char level, short cls) /* 13d0:21cc */
{
    long xp;

    xp = 0x7fffffffL;
    if (level < 41)
        if (level < 12)
            xp = level_xp_table[cls][level];
        else
            xp = (level - 11) * step_ec6[cls] + level_xp_table[cls][11];
    return xp;
}
