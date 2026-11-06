/* CKIT.EXE overlay 14 module 28aa: the character view.  The status page
 * (status, class, race, alignment, level, experience, the six abilities, the
 * money and the combat values), its menu of items, spells, trade, deposit or
 * drop, lay on hands and cure disease; the items menu (ready, use, trade,
 * deposit, drop, halve, join, sell and identify) with readying and an item's
 * readied effect; joining scroll bundles; trading, dropping and donating
 * money with the amount prompt and the carry limit; picking a party member;
 * and the hit points a class level adds or takes away.
 *
 * EXE 0x05dce0..0x061f1c, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 28aa, the first byte of RTLink section 14 (payload
 * 0x5dce0, link segment 28aa, parent section 13).  Its own final retf is
 * 28aa:423b; the lone retf at 28aa:423c (2ccd:000c) is left to module 2ccd,
 * whose code begins at 2ccd:000d.  Section 14's table files all 384 fixup
 * sites in the range under frame 28aa.  Built with Turbo C 2.01, tcc -ml -O
 * -Z -d.  Functions are defined in shipped order; bake consumes them in that
 * order.  Section 15 holds other bytes at link segment 28aa.
 *
 * A bare stored seg:off in this file means section 14.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  Section 14 has no child
 * section, so code outside the section reaches this module only through
 * vector stubs: 24 of the section's 31 vectors land on 24 of its routines.
 *
 * A PORT of DQK's charview.c (dqk 893e699).  The status page shows a human's
 * earlier class beside the current one where DQK shows deity, robe and
 * knightly order; readying is split in two, ready_check (28aa:189f, FRUA's
 * own) returning the error that item_equip_toggle reports; join_item (28aa:1ebe)
 * bundles scrolls (types 0x27, 0x28, 'I'), FRUA's own, and joins other like
 * items as DQK's does;
 * donate (28aa:326e) is FRUA's own; hp_floor (28aa:3a93) is rewritten.
 * use_item, input_amount and pick_hero each carry a switch whose values are
 * few and sparse, which Turbo C compiles to a search of a value table in the
 * code segment (cmp ax,[cs:bx]).
 *
 * _DATA is DGROUP 8908..8d59, literals only: 78 distinct, stored as 75 --
 * -d stores "" on the NUL of "Status:", " " on the tail of "STR " and "%s"
 * on the tail of "%s/%s".
 * 8904..8907 ("GEO") is another module's, and 8d5a starts another module's
 * literals.  _BSS is d422..d429: last_tick and view_hero, which only this
 * module's code addresses; d420 is addressed from sections 3, 5 and 13, and
 * d42a from sections 16 and 21, never from this module.
 */

#include <stdlib.h>

struct Pool;
struct Effect;

struct Item {                   /* 0x3e bytes                             */
    struct Item far *next;      /* +0                                     */
    char pad_4;                 /* +4                                     */
    char name[0x23];            /* +5                                     */
    unsigned char type;         /* +28  'I': a bundle, scrolls in more    */
    unsigned char name1;        /* +29                                    */
    char name2;                 /* +2a                                    */
    unsigned char name3;        /* +2b                                    */
    unsigned weight;            /* +2c                                    */
    unsigned value;             /* +2e  a bundle's is the sum of its own  */
    char plus;                  /* +30                                    */
    char save_bonus;            /* +31                                    */
    char readied;               /* +32                                    */
    unsigned char unid_mask;    /* +33                                    */
    char cursed;                /* +34                                    */
    unsigned char count;        /* +35  a bundle's: scrolls in more       */
    unsigned char charges;      /* +36  bit 7: a spell still to scribe    */
    unsigned char effect;       /* +37  bit 7: a spell still to scribe    */
    unsigned char special;      /* +38  bit 7: an effect when readied     */
    char pad_39;                /* +39                                    */
    struct Item far *more;      /* +3a                                    */
};

struct ItemType {               /* 0x10 bytes an item type                */
    unsigned char slot;         /* +0   index into Hero.equipped             */
    unsigned char hands;        /* +1                                     */
    char pad_2[0xb];            /* +2                                     */
    unsigned char classes;      /* +d   tested against Hero.classes       */
    char pad_e[2];              /* +e                                     */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0  2 magic-user                       */
    unsigned char level;        /* +2                                     */
    char pad_3[0xc];            /* +3                                     */
    int in_combat;              /* +f   nonzero: usable in a fight        */
    char pad_11[5];             /* +11                                    */
};

struct Node {                   /* a list_menu line                       */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct HotKey {
    unsigned char key;          /* +0  code the menu returns              */
    unsigned char ch;           /* +1  letter that selects it             */
};

struct Stat {
    unsigned char base;         /* +0                                     */
    unsigned char cur;          /* +1                                     */
};

struct Fight {                  /* combat state, Hero +40                 */
    char pad_0[2];              /* +0   not read here                     */
    char can_use_item;          /* +2   nonzero: may use items            */
};

struct Hero {                   /* a party member                         */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4                                     */
    struct Item far *items;     /* +8                                     */
    struct Item far *equipped[13]; /* +c   readied, by item_table slot       */
    struct Fight far *fight;    /* +40                                    */
    long xp;                    /* +44  experience                        */
    char pad_48[4];             /* +48                                    */
    unsigned coins[3];          /* +4c  platinum, gems, jewelry           */
    unsigned age;               /* +52                                    */
    char pad_54[2];             /* +54                                    */
    unsigned encumbrance;       /* +56                                    */
    unsigned char race;         /* +58  index into race_names, 5 human    */
    unsigned char cls;          /* +59  index into class_names            */
    char pad_5a[2];             /* +5a                                    */
    unsigned char gender;       /* +5c  index into sex_names              */
    unsigned char alignment;    /* +5d  index into align_names            */
    unsigned char status;       /* +5e  index into status_names           */
    char pad_5f;                /* +5f                                    */
    char name[0x10];            /* +60                                    */
    struct Stat stats[6];       /* +70  str int wis dex con cha           */
    unsigned char str_ex;       /* +7c  exceptional strength              */
    unsigned char str_ex_base;  /* +7d                                    */
    char pad_7e[2];             /* +7e                                    */
    unsigned char cures;        /* +80  cure disease left                 */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[8];             /* +82                                    */
    unsigned char dual_level;   /* +8a                                    */
    char pad_8b[8];             /* +8b                                    */
    unsigned char control;      /* +93  0x80 and up: not a party member   */
    char pad_94[9];             /* +94                                    */
    unsigned char level[7];     /* +9d  one per class                     */
    unsigned char old_level[7]; /* +a4  one per class                     */
    char pad_ab[0xc];           /* +ab                                    */
    unsigned char class_mask;   /* +b7  tested against ItemType.classes   */
    unsigned char hp_base;      /* +b8                                    */
    char pad_b9[8];             /* +b9                                    */
    unsigned char item_count;   /* +c1                                    */
    unsigned char hands;        /* +c2  hands in use                      */
    char pad_c3[3];             /* +c3                                    */
    unsigned char spells[0x8d]; /* +c6  bits 0-6: a memorized spell       */
    char pad_153[0x26];         /* +153                                   */
    unsigned char mage_slots_5; /* +179 level-5 mage spells allowed       */
    char pad_17a[4];            /* +17a                                   */
    char active;                /* +17e 0: can do nothing with spells     */
    char quick_fight;           /* +17f nonzero: fights on its own        */
    unsigned char to_hit;       /* +180                                   */
    unsigned char ac;           /* +181                                   */
    char pad_182[3];            /* +182                                   */
    unsigned char dice_n_main;  /* +185 damage dice                       */
    char pad_186;               /* +186                                   */
    unsigned char dice_s_main;  /* +187                                   */
    char pad_188;               /* +188                                   */
    char dmg_main;              /* +189                                   */
    char pad_18a;               /* +18a                                   */
    unsigned char hp;           /* +18b                                   */
    unsigned char movement;     /* +18c                                   */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern unsigned far mem_scan(void far *p, int n, char c);    /* 00d7:27b1 */
extern void far item_name(struct Hero far *h, struct Item far *it, char col,
                          char row, char full, char show);       /* 03d0:03d1 */
extern void far show_ac(struct Hero far *h, char col, char row); /* 03d0:06f9 */
extern void far show_hp(struct Hero far *h, char col, char row, char hilite,
                        char max);                               /* 03d0:0789 */
extern unsigned char far calc_move(struct Hero far *h);          /* 03d0:0a38 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern int  far str_carry(struct Hero far *h);                   /* 03d0:12ae */
extern void far item_remove(struct Hero far *h, struct Item far *it); /* 03d0:13c4 */
extern void far item_append(struct Hero far *h, struct Item far *src); /* 03d0:149c */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far view_redraw(void);                                 /* 03d0:1a72 */
extern char far con_hp_adj(struct Hero far *h, short arg);        /* 03d0:2045 */
extern char far old_class_abilities_available(struct Hero far *h);                   /* 03d0:2100 */
extern unsigned char far first_level(struct Hero far *h);        /* 03d0:212e */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far status_char(char col, char row, char attr, unsigned char c);
                                                                 /* 06c9:059a */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far str_pad_left(char far *s, unsigned char width);  /* 06c9:09ee */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far set_selected(struct Node far *p);                /* 083a:0317 */
extern struct Node far *far get_selected(void);                  /* 083a:0327 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far press_return_clear(void);                        /* 083a:053a */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far node_menu(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:197d */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern char far list_menu(char far *title, char far *text, char row, char col,
                          char row2, char col2, struct Node far *list,
                          char a, char b, char far *redraw_box, int far *top,
                          struct Node far * far *out);           /* 083a:20d1 */
extern void far int_to_str(int value, char far *buf);            /* 0a75:000d */
extern char far *far substr(char far *s, int start, int len);    /* 0a75:0027 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far str_to_long(char far *s, long far *out, int far *err); /* 0a75:009c */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern unsigned char far game_key_get(void);                        /* 0d48:0052 */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern char far *far int_str(int n);                             /* 0d48:019d */
extern char far *far ulong_str(unsigned long n);                 /* 0d48:01b6 */
extern void far frame_character_sheet(void);                                /* 0d65:009e */
extern void far frame_list_panel(void);                                /* 0d65:0302 */
extern void far beep(void);                                      /* 0da7:018c */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far party_display(void far *node);   /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far party_rows(struct Hero far *hero, char c);
                                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far party_key_select(unsigned char c);  /* 18de:10a3, vector to overlay 4 1c86:06b6 */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern char far combat_actions_end(struct Hero far *h); /* 18de:1139, vector to overlay 0 1cb8:0b84 */
extern void far combat_view_show(void);             /* 18de:1143, vector to overlay 0 1cb8:0c07 */
extern void far spell_cast(unsigned char n, char a, char b, char far *done);
                                                /* 18de:1175, vector to overlay 12 2b69:14c8 */
extern void far combat_palette_set(void);             /* 18de:1297, vector to overlay 0 22fa:00e5 */
extern unsigned char far spell_menu(unsigned char list, unsigned char type, int far *choice,
                                      char far *more);
                                                /* 18de:1305, vector to overlay 12 2b69:203e */
extern int  far pool_records(void);             /* 18de:1481, vector to overlay 3 1bfb:00db */
extern void far pool_set_records(int n);            /* 18de:148b, vector to overlay 3 1bfb:00d0 */
extern void far le_long_put(long n, unsigned char far *p);
                                                /* 18de:14e5, vector to overlay 4 1cff:048c */
extern long far le_long_get(unsigned char far *p);
                                                /* 18de:14f9, vector to overlay 4 1cff:04e9 */
extern void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove);
                                                /* 18de:19bd, vector to overlay 19 2e88:1648 */
extern char far item_casts(struct Item far *item);
                                                /* 18de:1acb, vector to overlay 12 26e3:4055 */
extern char far fx_heal(struct Hero far *hero, char a, char b);
                                                /* 18de:1af3, vector to overlay 19 2e88:224c */
extern void far fx_stat_recalc(struct Hero far *hero, short stat);
                                                /* 18de:1b07, vector to overlay 19 2e88:1a80 */
extern void far item_sell(struct Hero far *hero, struct Item far *item);
                                                /* 18de:1b43, vector to overlay 10 2431:0b1a */
extern void far item_identify(struct Hero far *hero, struct Item far *item, char far *redraw);
                                                /* 18de:1b4d, vector to overlay 10 2431:0de1 */
extern void far fx_dispatch(unsigned char kind, struct Hero far *h,
                             struct Effect far *fx, char removed);
                                                /* 18de:1c8d, vector to overlay 11 26e3:3831 */
extern void far item_spend(struct Hero far *hero, struct Item far *item,
                             char spell);       /* 18de:1cbf, vector to overlay 12 26e3:408c */

extern unsigned char constitution_hp_bonus[];        /* 1383  hit point bonus by CON    */
extern unsigned char mu_spell_row;         /* 13a2  the spell_slots_by_level row read     */
extern unsigned char spell_slots_by_level[][29][9];                          /* 14f4 */
extern struct SpellInfo spell_info[];                            /* 264a */
extern char far *class_names[];                                  /* 3914 */
extern char far *race_names[];                                   /* 395c */
extern char far *align_names[];                                  /* 3978 */
extern char far *sex_names[];                                    /* 399c */
extern char far *coin_names[3];                                  /* 39a4 */
extern char far *status_names[];                                 /* 39b0 */
extern char far *txt_lose_it;                                    /* 39e4 */
extern char far *how_much_text;                                  /* 3a0c */
extern char far *coin_title;                                     /* 3a24 */
extern char far *trade_to_text;                                  /* 3a28 */
extern char far *trade_amount_text;                              /* 3a2c */
extern char far *items_label;                                    /* 3a38 */
extern char far *ready_label;                                    /* 3a3c */
extern char far *your_text;                                      /* 3a40 */
extern char far *gone_text;                                      /* 3a44 */
extern char far *must_ready_text;                                /* 3a48 */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *pick_text;                                      /* 3bd8 */
extern char far *select_items;                                   /* 3c2c */
extern char far *view_items;                                     /* 3c54 */
extern char far *items_items;                                    /* 3c58 */
extern unsigned char disease_fx[];                               /* 6171 */
extern unsigned char class_bit[7];                               /* 6252 */
extern unsigned char hit_dice_level_limit[7];                                /* 6262 */
extern unsigned char class_hit_dice[7];                                /* 6269 */
extern unsigned char hit_die_sides[7];                                /* 6270 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern struct ItemType far *item_table;                          /* b40a */
extern unsigned char text_col;                                   /* b419 */
extern unsigned char text_row;                                   /* b41a */
extern int view_mode;                                            /* b421 */
extern char confirm_anim;                                        /* b42a */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char exploration_menu_context;                                           /* b4e8 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char fx_lock;                                             /* b8d0 */
extern long pool[3];                                             /* b8d6 */
extern struct Item far *pool_items;                              /* b8e2 */
extern struct Item far *item_cur;                                /* bb8c */
extern char byte_bcef;                                           /* bcef */
extern struct Item far *cur_item;                                /* bcfd */
extern char item_cast_active;                                           /* be69 */
extern char byte_c3c3;                                           /* c3c3 */
extern char item_handler_active;                                           /* c523 */
extern char menu_escaped;                                        /* ca2a */
extern char inventory_redraw;                                           /* cb08 */
extern struct Hero far *picker_start_character;                                /* cb0a */
extern struct Pool item_record_pool;                                    /* cb0e */
extern struct Hero far *selected_target;                               /* cc6f */
extern void far *node_pool;                                      /* ce5d */
extern char order_mode;                                           /* d126 */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */

static long last_tick;                                           /* d422 */
static struct Hero far *view_hero;                               /* d426 */

void far money_show(void);
void far combat_show(void);
void far stat_show(short stat, char active);
void far items_menu(char far *done);
void far item_equip_toggle(struct Item far *item);
static void far trade_item(struct Item far *item);
static void far halve_item(struct Item far *item);
void far join_item(struct Item far *item);
void far use_item(struct Item far *item, char far *done);
char far cant_carry(struct Hero far *hero, char mode, struct Item far *item);
void far pick_hero(char far *prompt, char a, char b, struct Hero far * far *out);
char far hp_bonus(struct Hero far *h, short cls);
static void far trade(void);
void far drop(char in_view);
char far can_heal(struct Hero far *h);
char far can_cure(struct Hero far *h);
void far heal(struct Hero far *h);
void far cure(struct Hero far *h);

void far char_status(void)                                       /* 38aa:0000 */
{
    short stat;
    short i;
    char buf[0x52];
    char sep;
    char dual;

    view_hero = cur_node;
    frame_character_sheet();
    character_name_show(view_hero, 1, 1, 0);
    status_printf(0x14, 1, 0xb, 0, "Status:");
    status_printf(0x1b, 1, 7, 0, status_names[view_hero->status]);
    str_copy(buf, sex_names[view_hero->gender]);
    status_printf(1, 3, 0xb, 0, buf);
    status_printf(8, 3, 0xb, 0, msg_format("%d years", view_hero->age));
    str_copy(buf, align_names[view_hero->alignment]);
    status_printf(1, 4, 0xb, 0, buf);
    str_copy(buf, race_names[view_hero->race]);
    status_printf(0x14, 4, 0xb, 0, buf);
    dual = 0;
    str_copy(buf, class_names[view_hero->cls]);
    if (view_hero->race == 5)
        for (i = 0; i <= 6; i++)
            if (view_hero->old_level[i] > 0) {
                dual = 1;
                if (first_level(view_hero) > view_hero->old_level[i]) {
                    str_copy(buf, msg_format("%s/%s", buf, class_names[i]));
                    i = 6;
                }
            }
    status_printf(1, 5, 0xb, 0, buf);
    status_printf(1, 7, 0xb, 0, "Level");
    str_copy(buf, "");
    if (dual) {
        for (i = 0; i <= 6; i++)
            if (view_hero->level[i] > 0) {
                str_copy(buf, msg_format("%s%d", buf, view_hero->level[i]));
                i = 6;
            }
        for (i = 0; i <= 6; i++)
            if (first_level(view_hero) > view_hero->old_level[i] && view_hero->old_level[i] > 0) {
                str_copy(buf, msg_format("%s/%s", buf, byte_str(view_hero->old_level[i])));
                i = 6;
            }
    } else {
        sep = 0;
        for (i = 0; i <= 6; i++)
            if (view_hero->level[i] > 0) {
                if (sep)
                    str_copy(buf, msg_format("%s/", buf));
                str_copy(buf, msg_format("%s%s", buf,
                                             byte_str(view_hero->level[i] + view_hero->old_level[i])));
                sep = 1;
            }
    }
    status_printf(7, 7, 0xb, 0, buf);
    status_printf(0x18, 6, 0xb, 0, "Experience:");
    str_copy(buf, ulong_str(view_hero->xp));
    status_printf((0x14 - str_length(buf)) / 2 + 0x14, 7, 7, 0, buf);
    for (stat = 0; stat <= 5; stat++) {
        switch (stat) {
        case 0:
            str_copy(buf, "STR ");
            break;
        case 1:
            str_copy(buf, "INT ");
            break;
        case 2:
            str_copy(buf, "WIS ");
            break;
        case 3:
            str_copy(buf, "DEX ");
            break;
        case 4:
            str_copy(buf, "CON ");
            break;
        case 5:
            str_copy(buf, "CHA ");
            break;
        }
        status_printf(1, stat + 9, 7, 0, buf);
        stat_show(stat, 0);
    }
    money_show();
    combat_show();
    if (view_hero->equipped[0])
        item_name(view_hero, view_hero->equipped[0], 1, 0x15, 0, 1);
    if (view_hero->equipped[2])
        item_name(view_hero, view_hero->equipped[2], 1, 0x16, 0, 1);
}

void far money_show(void)                                        /* 38aa:057a */
{
    char row;
    short i;
    char buf[0x2a];

    text_clear(0x14, 9, 0x26, 0xf);
    row = 9;
    for (i = 0; i <= 2; i++)
        if (cur_node->coins[i] > 0) {
            str_copy(buf, coin_names[i]);
            status_printf(0x14, row, 7, 0, buf);
            if (i == 0)
                str_copy(buf, ulong_str(cur_node->coins[i]));
            else
                str_copy(buf, int_str(cur_node->coins[i]));
            status_printf(0x26 - str_length(buf), row, 7, 0, buf);
            row++;
        }
}

void far combat_show(void)                                       /* 38aa:0679 */
{
    struct Hero far *h;
    char col;
    char buf[0x2a];

    h = cur_node;
    recalc_stats(h);
    status_printf(0x14, 3, 0xb, 0, "Hit Points ");
    text_clear(0x1f, 3, 0x25, 3);
    show_hp(h, 0x1f, 3, 0, 1);
    status_printf(1, 0x11, 0xb, 0, "Armor Class");
    if (h->ac >= 1 && h->ac <= 0x32)
        col = 0x10;
    else if (h->ac >= 0x33 && h->ac <= 0x3c)
        col = 0x11;
    else if (h->ac >= 0x3d && h->ac <= 0x45)
        col = 0x10;
    else
        col = 0xf;
    text_clear(0xf, 0x11, 0x11, 0x11);
    show_ac(h, col, 0x11);
    status_printf(0x14, 0x11, 0xb, 0, "Encumbrance");
    str_copy(buf, int_str(h->encumbrance));
    str_pad_left(buf, 5);
    status_printf(0x21, 0x11, 7, 0, buf);
    status_printf(1, 0x12, 0xb, 0, "THAC0\t ");
    int_to_str(0x3c - h->to_hit, buf);
    str_pad_left(buf, 2);
    status_printf(0x10, 0x12, 7, 0, buf);
    status_printf(0x14, 0x12, 0xb, 0, "Movement");
    calc_move(h);
    str_copy(buf, byte_str(h->movement));
    str_pad_left(buf, 3);
    status_printf(0x23, 0x12, 7, 0, buf);
    str_copy(buf, msg_format("%dd%s", h->dice_n_main, byte_str(h->dice_s_main)));
    if (h->dmg_main > 0)
        str_copy(buf, msg_format("%s+%s", buf, byte_str(h->dmg_main)));
    if (h->dmg_main < 0)
        str_copy(buf, msg_format("%s-%s", buf, byte_str(abs(h->dmg_main))));
    status_printf(1, 0x13, 0xb, 0, "Damage");
    str_pad_left(buf, 7);
    status_printf(0xb, 0x13, 7, 0, buf);
}

void far stat_show(short stat, char active)                      /* 38aa:0987 */
{
    char buf[0x2a];
    char col;
    char hl;

    if (active)
        hl = 0xb;
    else
        hl = 7;
    col = 5;
    text_clear(col, stat + 9, 0xb, stat + 9);
    if (cur_node->stats[stat].cur < 10)
        col++;
    status_printf(col, stat + 9, hl, 0, byte_str(cur_node->stats[stat].cur));
    if (stat == 0 && cur_node->stats[0].cur == 0x12 && cur_node->str_ex > 0) {
        str_copy(buf, byte_str(cur_node->str_ex));
        if (cur_node->str_ex < 10)
            str_copy(buf, msg_format("0%s", buf));
        if (cur_node->str_ex == 100)
            str_copy(buf, "00");
        status_printf(7, 9, hl, 0, msg_format("(%s)", buf));
    }
    if (cur_node->stats[stat].cur != cur_node->stats[stat].base
        || stat == 0 && cur_node->str_ex_base != cur_node->str_ex)
        status_printf(0xc, stat + 9, hl, 0, "*");
    else
        status_printf(0xc, stat + 9, hl, 0, " ");
}

void far char_view(char far *done)                               /* 38aa:0b21 */
{
    unsigned char ch;
    char has_money;
    char has_spells;
    unsigned char i;
    short j;
    int index;
    unsigned char options;
    char redraw;

    *done = 0;
    ch = 0xff;
    selected_target = cur_node;
    char_status();
    mem_fill(hotkeys, 0x28, 0xff);
    redraw = 1;
    while (ch != 7 && !*done) {
        options = 0;
        has_spells = 0;
        has_money = 0;
        for (i = 0; i <= 0x8c; i++)
            if ((view_hero->spells[i] & 0x7f) > 0)
                has_spells = 1;
        for (j = 0; j <= 2; j++)
            if (view_hero->coins[j] > 0)
                has_money = 1;
        if (view_hero->items)
            hotkey_add(0, &options);
        if (has_spells)
            hotkey_add(1, &options);
        if ((view_hero->control < 0x80 || view_hero->active == 0 || view_hero->status == 1) && has_money
            && view_mode != 5)
            hotkey_add(2, &options);
        if (has_money)
            if (view_mode == 10)
                hotkey_add(3, &options);
            else
                hotkey_add(4, &options);
        if (can_heal(cur_node) && view_mode != 10)
            hotkey_add(5, &options);
        if (can_cure(cur_node) && view_mode != 10)
            hotkey_add(6, &options);
        hotkey_add(7, &options);
        ui_mode = redraw;
        ch = menu_run("", view_items, 0, 0);
        if (menu_escaped && ch == 0x1b)
            ch = 7;
        redraw = ui_mode;
        index = -1;
        switch (ch) {
        case 0:
            items_menu(done);
            break;
        case 1:
            i = spell_menu(0, 0, &index, &has_spells);
            break;
        case 2:
            trade();
            break;
        case 3:
        case 4:
            drop(1);
            money_show();
            break;
        case 5:
            heal(cur_node);
            break;
        case 6:
            cure(cur_node);
            break;
        }
        if (!*done && ch < 3)
            char_status();
    }
    if (view_mode == 5)
        combat_palette_set();
    view_redraw();
}

char far item_ok(struct Item far *item)                          /* 38aa:0db5 */
{
    char ok;
    struct Item far *sub;

    ok = 0;
    if (item->readied)
        bottom_msg("Must be unreadied");
    else if (!item_casts(item))
        ok = 1;
    else if (item->charges & 0x80 || item->effect & 0x80 || item->special & 0x80) {
        character_name_show(cur_node, 1, 0x15, 0);
        text_col = str_length(cur_node->name) + 2;
        text_row = 0x15;
        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 0, " was going to scribe from that scroll");
        if (confirm("is it Okay to lose it? ", 0))
            ok = 1;
    } else if (item->type == 'I') {
        sub = item;
        ok = 0;
        while (sub->more) {     /* 28aa:0e90 tests item, never sub       */
            if (item->charges & 0x80 || item->effect & 0x80
                || item->special & 0x80) {
                character_name_show(cur_node, 1, 0x15, 0);
                text_col = str_length(cur_node->name) + 2;
                text_row = 0x15;
                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 0,
                          " was going to scribe from that bundle");
                if (confirm("is it Okay to lose it? ", 0))
                    ok = 0;     /* 28aa:0f14 a yes leaves ok 0 too      */
                goto done;
            }
            sub = sub->more;
        }
        ok = 1;
    } else
        ok = 1;
done:
    text_clear(1, 0x15, 0x26, 0x16);
    return ok;
}

void far items_menu(char far *done)                              /* 38aa:0f56 */
{
    struct Item far *item;
    struct Item far *sel;
    struct Hero far *h;
    struct Item far *picked;
    int index;
    unsigned char count;
    unsigned char options;
    unsigned char ch;
    char redraw;
    char need_redraw;
    char redraw_box;
    char anim;
    register int n;

    h = cur_node;
    sel = h->items;
    index = 0;
    redraw_box = 1;
    need_redraw = 1;
    ch = 0xff;
    redraw = 1;
    anim = confirm_anim;
    confirm_anim = 0;
    while (ch != 9 && !*done && h->item_count > 0) {
        count = h->item_count;
        if (h->items) {
            mem_fill(hotkeys, 0x28, 0xff);
            options = 0;
            hotkey_add(0, &options);
            if (h->active && cursor_obj[2] == 0
                && (view_mode == 2 || view_mode == 3 || view_mode == 4
                    || view_mode == 5 && h->fight->can_use_item != 0))
                hotkey_add(1, &options);
            if ((h->control < 0x80 || h->active == 0 || h->status == 1) && view_mode != 5)
                hotkey_add(2, &options);
            if (view_mode != 10)
                hotkey_add(4, &options);
            else if (h->control < 0x80 || h->active == 0 || h->status == 1)
                hotkey_add(3, &options);
            if (h->item_count < 0x10)
                hotkey_add(5, &options);
            hotkey_add(6, &options);
            if ((h->control < 0x80 || h->active == 0 || h->status == 1) && view_mode == 1
                && cursor_obj[0x14] != 1)
                hotkey_add(7, &options);
            if (view_mode == 1 && cursor_obj[0x14] != 1)
                hotkey_add(8, &options);
            hotkey_add(9, &options);
            for (item = h->items; item; item = item->next)
                item_name(h, item, 0, 0, 1, 0);
            if (need_redraw || inventory_redraw) {
                frame_list_panel();
                character_name_show(h, 1, 1, 1);
                status_printf(str_length(h->name) + 4, 1, 7, 0, items_label);
                status_printf(1, 3, 0xb, 0, ready_label);
                redraw_box = 1;
                need_redraw = 0;
                inventory_redraw = 0;
            }
            ui_mode = redraw;
            picked = sel;
            ch = list_menu(empty_text, items_items, 1, 5, 0x26, 0x16,
                           (struct Node far *)h->items, 1, 8, &redraw_box, &index,
                           (struct Node far * far *)&picked);
            sel = picked;
            redraw = ui_mode;
            if (menu_escaped && ch == 0x1b)
                ch = 9;
            if (sel)
                switch (ch) {
                case 0:
                    item_equip_toggle(sel);
                    break;
                case 1:
                    if (sel->readied == 0) {
                        bottom_msg(must_ready_text);
                        ch = 0xff;
                    } else if (item_casts(sel)
                               || sel->effect > 0 && sel->special < 0x80) {
                        use_item(sel, done);
                        if (view_mode != 5)
                            *done = 0;
                        if (!*done)
                            need_redraw = 1;
                    }
                    break;
                case 2:
                    if (item_ok(sel))
                        trade_item(sel);
                    else
                        ch = 0xff;
                    need_redraw = 1;
                    break;
                case 3:
                    if (item_ok(sel)) {
                        if (sel->type == 'I')
                            n = sel->count + 1;
                        else
                            n = 1;
                        if (pool_records() + n <= 0x32 || view_mode != 10) {
                            for (cur_item = pool_items; cur_item && cur_item->next;
                                 cur_item = cur_item->next)
                                ;
                            item_name(h, sel, 0, 0, 0, 0);
                            if (view_mode == 10) {
                                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                                          msg_format("Your %swill be left in the vault",
                                                     sel->name));
                                pool_set_records(pool_records() + n);
                                text_pause();
                                if (!pool_items) {
                                    pool_alloc(&pool_items, 0x3e, &item_record_pool);
                                    cur_item = pool_items;
                                } else {
                                    pool_alloc(&cur_item->next, 0x3e, &item_record_pool);
                                    cur_item = cur_item->next;
                                }
                                *cur_item = *sel;
                                cur_item->next = 0;
                                if (sel->type == 'I')
                                    sel->more = 0;
                                item_remove(h, sel);
                                redraw_box = 1;
                            } else {
                                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                                          msg_format("Your %swill be gone forever",
                                                     sel->name));
                                if (confirm("Drop It? ", 1)) {
                                    item_remove(cur_node, sel);
                                    redraw_box = 1;
                                }
                            }
                            text_clear(1, 0x15, 0x26, 0x16);
                        } else if (view_mode == 10)
                            if (sel->type == 'I')
                                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                                          "Scroll Bundle won't fit into Vault!");
                            else
                                text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                                          "Vault is full!");
                    } else
                        ch = 0xff;
                    break;
                case 4:
                    if (item_ok(sel)) {
                        item_name(h, sel, 0, 0, 0, 0);
                        text_wrap(1, 0x15, 0x26, 0x16, 0xb, 0, 1,
                                  msg_format("%s%s%s", your_text, sel->name, gone_text));
                        if (confirm(txt_lose_it, 0)) {
                            item_remove(cur_node, sel);
                            redraw_box = 1;
                        }
                        text_clear(1, 0x15, 0x26, 0x16);
                    } else
                        ch = 0xff;
                    break;
                case 5:
                    halve_item(sel);
                    break;
                case 6:
                    join_item(sel);
                    break;
                case 7:
                    if (item_ok(sel))
                        item_sell(h, sel);
                    else
                        ch = 0xff;
                    break;
                case 8:
                    item_identify(h, sel, &redraw_box);
                    break;
                }
            recalc_stats(h);
        }
        if (h->item_count != count)
            redraw_box = 1;
    }
    confirm_anim = anim;
}

void far item_effect(struct Item far *item, char on)             /* 38aa:16f0 */
{
    unsigned char id;
    struct Hero far *h;
    unsigned char level;
    unsigned char k;
    unsigned char cnt;

    cnt = 0;
    h = cur_node;
    id = item->special & 0x7f;
    switch (id) {
    case 0:
        item_handler_active = 1;
        fx_dispatch(item->effect, h, (struct Effect far *)item, !on);
        break;
    case 1:
        level = class_level(h, 5);
        if (level > 0x1d)
            level = 0x1d;
        else if (level < 1)
            level = 1;
        k = spell_slots_by_level[mu_spell_row][level - 1][4];
        if (on)
            h->mage_slots_5 = k + k;
        else {
            h->mage_slots_5 = k;
            cnt = 0;
            for (k = 0; k <= 0x8c; k++)
                if (h->spells[k] && spell_info[h->spells[k]].kind == 2
                    && spell_info[h->spells[k]].level == 5) {
                    cnt++;
                    if (h->mage_slots_5 < cnt)
                        h->spells[k] = 0;
                }
        }
        break;
    case 3:
    case 5:
        fx_stat_recalc(h, 0);
        break;
    case 9:
        if (!on)
            fx_remove(h, 0x17, 0);
        break;
    }
}

char far ready_check(struct Hero far *h, struct Item far *item, unsigned char far *slot)
                                                                 /* 38aa:189f */
{
    char err;

    err = 0;
    if (h->hands + item_table[item->type].hands > 2)
        err = 3;
    *slot = item_table[item->type].slot;
    switch (*slot) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        if (h->equipped[*slot])
            err = 2;
        break;
    case 9:
        if (h->equipped[10])
            err = 2;
        break;
    }
    if (item->type == 0x1e && h->equipped[11]) {
        err = 2;
        *slot = 0xb;
    }
    if ((item->type == 0xc || item->type == 0x4a) && h->equipped[12]) {
        err = 2;
        *slot = 0xc;
    }
    if (!(item_table[item->type].classes & h->class_mask))
        err = 1;
    return err;
}

void far item_equip_toggle(struct Item far *item)                    /* 38aa:19bf */
{
    char special;
    unsigned char err;
    unsigned char slot;
    struct Hero far *h;

    special = item->special & 0x80;
    h = cur_node;
    if (item->readied) {
        if (item->cursed)
            bottom_msg("It's Cursed!");
        else {
            item->readied = 0;
            if (special)
                item_effect(item, 0);
        }
    } else {
        err = ready_check(h, item, &slot);
        switch (err) {
        case 0:
            item->readied = 1;
            if (special)
                item_effect(item, 1);
            break;
        case 1:
            bottom_msg("Wrong Class");
            break;
        case 2:
            item_name(h, h->equipped[slot], 0, 0, 0, 0);
            bottom_msg(msg_format("already using %s", h->equipped[slot]->name));
            break;
        case 3:
            if (view_mode != 5 || h->quick_fight == 0)
                bottom_msg("Your hands are full!");
            break;
        case 4:
            bottom_msg("Wrong Magical Order");
            break;
        case 5:
            bottom_msg("Usable Only By Kender");
            break;
        case 6:
            bottom_msg("Thief Level Too Low");
            break;
        }
    }
}

static void far trade_item(struct Item far *item)                /* 38aa:1b3c */
{
    struct Hero far *target;

    target = selected_target;
    view_redraw();
    pick_hero("Trade with whom?", 1, 1, &target);
    if (target) {
        selected_target = target;
        if (cant_carry(target, 0, item))
            bottom_msg("Overloaded");
        else {
            item_append(target, item);
            if (item->type == 'I')
                item->more = 0;
            item_remove(cur_node, item);
            recalc_stats(target);
        }
    }
}

static void far halve_item(struct Item far *item)                /* 38aa:1bf7 */
{
    struct Item far *new;
    struct Item far *last;
    unsigned char half;
    unsigned char rest;

    half = item->count / 2;
    if (half > 0) {
        if (item->type == 'I') {
            rest = item->count - half;
            pool_alloc(&new, 0x3e, &item_record_pool);
            *new = *item;
            if (half == 1)
                mem_move(new->more, new, 0x3e);
            else {
                new->count = half;
                new->weight = half;
                new->name2 = half;
                last = new;
                new->value = 0;
                for (tick_slot = 1; tick_slot <= half; tick_slot++) {
                    last = last->more;
                    new->value += last->value;
                }
            }
            new->readied = 0;
            new->next = item->next;
            if (rest == 1) {
                if (half == 1)
                    mem_move(item->more->more, item, 0x3e);
                else
                    mem_move(item->more->more->more, item, 0x3e);
                item->more = 0;
            } else {
                item->count = rest;
                item->weight = rest;
                item->name2 = rest;
                for (tick_slot = 1; tick_slot <= half; tick_slot++) {
                    item->value -= item->more->value;
                    item->more = item->more->more;
                }
            }
            if (half == 1)
                new->more = 0;
            else
                last->more = 0;
        } else {
            rest = item->count - half;
            pool_alloc(&new, 0x3e, &item_record_pool);
            mem_move5(item, new, 0x3e);
            new->count = half;
            new->readied = 0;
            new->next = item->next;
            item->count = rest;
        }
        item->next = new;
    } else
        bottom_msg("Can't halve that");
}

int far party_scrolls(void)                                      /* 38aa:1e59 */
{
    struct Item far *it;
    struct Hero far *h;
    register int n;

    n = 0;
    for (h = node_list; h; h = h->next)
        for (it = h->items; it; it = it->next)
            if (it->type == 'I')
                n += it->count;
    return n;
}

void far join_item(struct Item far *item)                        /* 38aa:1ebe */
{
    struct Item far *tail;
    struct Item far *cur;
    struct Item far *next;
    struct Item far *dst;
    unsigned char qty;
    unsigned char loose;
    unsigned char k;

    dst = item;
    if (dst->type == 0x27 || dst->type == 0x28 || dst->type == 'I') {
        if (dst->count >= 10)
            bottom_msg(msg_format("Bundles are limited to %d scrolls.", 10));
        else {
            qty = 0;
            for (cur = cur_node->items; cur; cur = cur->next)
                if (cur->type == 'I' && cur->count < 10 || cur->type == 0x27 || cur->type == 0x28)
                    qty++;
            loose = 0;
            for (cur = cur_node->items; cur; cur = cur->next)
                if (cur->type == 0x27 || cur->type == 0x28)
                    loose++;
            if (party_scrolls() + loose > 0x78)
                bottom_msg("Too many Bundles!");
            else if (qty < 2)
                bottom_msg("There are no similar items to join with.");
            else {
                if (dst->type != 'I') {
                    pool_alloc(&dst->more, 0x3e, &item_record_pool);
                    tail = dst->more;
                    mem_move(dst, tail, 0x3e);
                    tail->more = 0;
                    dst->readied = 0;
                    dst->name3 = 0x4d;
                    dst->name2 = 1;
                    dst->name1 = 0x27;
                    dst->charges = 0;
                    dst->effect = 0;
                    dst->special = 0;
                    dst->count = 1;
                    dst->type = 'I';
                    dst->unid_mask = 0;
                } else {
                    tail = dst;
                    for (k = 1; k <= dst->count; k++)
                        tail = tail->more;
                    tail->more = 0;
                }
                for (cur = cur_node->items; cur && dst->count < 10; cur = next) {
                    next = cur->next;
                    if (cur != dst
                        && (cur->type == 'I' && cur->count < 10 && cur->count + dst->count <= 10
                            || cur->type == 0x27 || cur->type == 0x28)) {
                        pool_alloc(&tail->more, 0x3e, &item_record_pool);
                        tail = tail->more;
                        if (cur->type == 'I') {
                            mem_move(cur->more, tail, 0x3e);
                            dst->value += cur->value;
                            dst->count += cur->count;
                            for (k = 1; cur->count - 1 >= k; k++)
                                tail = tail->more;
                        } else {
                            mem_move(cur, tail, 0x3e);
                            dst->count++;
                            dst->value += tail->value;
                            tail->more = 0;
                        }
                        dst->name2 = dst->count;
                        dst->weight = dst->count;
                        cur->more = 0;
                        item_remove(cur_node, cur);
                    }
                }
            }
        }
    } else if (dst->count == 0)
        bottom_msg("That cannot be joined with other items.");
    else {
        qty = dst->count;
        cur = cur_node->items;
        while (cur) {
            next = cur->next;
            if (cur != dst && cur->count > 0 && cur->name1 == dst->name1
                && cur->name2 == dst->name2 && cur->name3 == dst->name3
                && cur->unid_mask == dst->unid_mask && cur->type == dst->type
                && cur->plus == dst->plus && cur->save_bonus == dst->save_bonus
                && cur->cursed == dst->cursed && cur->weight == dst->weight
                && cur->charges == cur->charges && cur->charges < 2
                && cur->effect == cur->effect && cur->special == cur->special)
                                /* 28aa:2333 the last three against cur */
                if (cur->count + qty <= 0xff) {
                    qty += cur->count;
                    item_remove(cur_node, cur);
                } else {
                    dst->count = 0xff;
                    cur->count = cur->count - (0xff - qty);
                    qty = cur->count;
                    dst = cur;
                    item_name(cur_node, dst, 0, 0, 0, 0);
                }
            cur = next;
        }
        dst->count = qty;
    }
}

void far use_item(struct Item far *item, char far *done)          /* 38aa:23fc */
{
    unsigned char spell;
    int index;
    char not_empty;

    index = -1;
    item_cast_active = 0;
    spell = 0;
    if (item_casts(item)) {
        item_cur = item;
        if (item->type == 'I')
            spell = spell_menu(7, 1, &index, &not_empty);
        else
            spell = spell_menu(2, 1, &index, &not_empty);
    } else if (item->effect > 0 && item->special < 0x80) {
        item_cast_active = 1;
        spell = item->effect & 0x7f;
    }
    if (spell == 0)
        *done = 0;
    else {
        if (view_mode == 5 && cur_node->quick_fight == 0)
            combat_view_show();
        if (item_cast_active) {
            hero_text(cur_node, "uses an item", 0xa, 0);
            if (view_mode == 5) {
                status_printf(0, 0x17, 7, 0, "Item:");
                item_name(cur_node, item, 5, 0x17, 0, 1);
            } else
                item_name(cur_node, item, 1, 0x16, 0, 1);
            text_pause();
            combat_text_clear();
        }
        item_cast_active = 1;
        if (item_casts(item)) {
            combat_text_clear();
            if (class_level(cur_node, 5) > 0 || class_level(cur_node, 0) > 0
                || class_level(cur_node, 6) > 9 && fx_roll_dice(1, 100) <= 0x4b)
                spell_cast(spell, cur_node->quick_fight, 0, done);
            else
                hero_text(cur_node, "oops!", text_row + 1, 1);
        } else {
            switch (spell) {
            case 0x39:
                spell = 0x7f;
                break;
            case 0x3b:
                spell = 0x80;
                break;
            case 0x3c:
                spell = 0x81;
                break;
            case 0x3d:
                spell = 0x82;
                break;
            case 0x3e:
                spell = 0x83;
                break;
            case 0x3f:
                spell = 0x84;
                break;
            case 0x40:
                spell = 0x85;
                break;
            case 0x41:
                spell = 0x86;
                break;
            case 0x5f:
                spell = 0x87;
                break;
            case 0x61:
                spell = 0x88;
                break;
            case 0x63:
                spell = 0x89;
                break;
            }
            spell_cast(spell, cur_node->quick_fight, 0, done);
        }
        item_cast_active = 0;
        if (view_mode == 5 && spell_info[spell].in_combat != 0)
            *done = combat_actions_end(cur_node);
    }
    if (*done)
        if (item_casts(item))
            item_spend(cur_node, item, spell);
        else if (item->charges > 0)
            if (item->count > 1)
                item->count--;
            else {
                item->charges--;
                if (item->charges == 0)
                    item_remove(cur_node, item);
            }
}

int far coin_kind(char far *text, char far *name)                 /* 38aa:273a */
{
    short kind;
    unsigned char i;

    for (i = 1; text[i - 1] == ' '; i++)
        ;
    switch (text[i - 1]) {
    case 'G':
        kind = 1;
        str_copy(name, "Gems ");
        break;
    case 'P':
    case 'S':
        str_copy(name, "Platinum ");
        kind = 0;
        break;
    case 'J':
        str_copy(name, "Jewelry ");
        kind = 2;
        break;
    }
    return kind;
}

long far input_amount(long most, char far *prompt, char color)   /* 38aa:27e7 */
{
    long result;
    char col;
    char plen;
    char most_text[8];
    char text[7];
    unsigned char ch;
    long val;
    int err;
    long value;

    box_draw();
    status_printf(0, 0x18, color, 0, prompt);
    plen = str_len(prompt);
    col = plen;
    str_format(most_text, "%ld", most);
    text[0] = 0;
    screen_flush();
    do {
        ch = game_key_get();
        switch (ch) {
        default:                /* a shifted digit key counts as its digit */
            ch = mem_scan(")!@#$%^&*(", 10, ch) + '0';
            if (ch < '0' || ch > '9')
                break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (str_len(text) < 6) {
                str_format(text, "%s%c", text, ch);
                str_to_long(text, &val, &err);
                if (val <= most)
                    col++;
                else {
                    str_copy(text, most_text);
                    col = str_len(most_text) + plen;
                }
            } else
                beep();
            status_printf(plen, 0x18, 0xb, 0, text);
            break;
        case 8:
            if (str_len(text) > 0) {
                str_copy(text, substr(text, 1, str_len(text) - 1));
                col--;
                status_char(col, 0x18, 0, 1);
            }
            break;
        case 0xd:
        case 0x1b:
        case 0x60:
            break;
        }
    } while (ch != 0xd && ch != 0x1b && ch != 0x60);
    box_draw();
    if (ch == 0x1b || ch == 0x60)
        value = 0;
    else
        str_to_long(text, &value, &err);
    result = value;
    return result;
}

void far weight_sub(struct Hero far *h, int n)                   /* 38aa:2a18 */
{
    h->encumbrance = h->encumbrance - n;
}

unsigned far carry_limit(struct Hero far *h)                     /* 38aa:2a2b */
{
    unsigned short limit;

    limit = str_carry(h) + 1500;
    return limit;
}

void far weight_add(struct Hero far *h, int n)                   /* 38aa:2a48 */
{
    h->encumbrance = h->encumbrance + n;
}

void far money_trade(struct Hero far * far *giver, struct Hero far * far *taker,
                     long amount, short kind)                    /* 38aa:2a5b */
{
    if (carry_limit(*taker) >= (*taker)->encumbrance + amount) {
        (*giver)->coins[kind] = (*giver)->coins[kind] - amount;
        weight_sub(*giver, amount);
        (*taker)->coins[kind] = (*taker)->coins[kind] + amount;
        weight_add(*taker, amount);
    } else {
        bottom_msg("Overloaded");
        text_pause();
        input_pending_clear();
    }
}

static void far trade(void)                                      /* 38aa:2b19 */
{
    struct Hero far *target;
    short coin;
    short kind;
    unsigned char column;
    unsigned char pad;
    struct Node far *cur;
    struct Node far *prev;
    struct Node far *list;
    int index;
    char num_text[0x18];
    char line[0x18];
    char desc[0x29];
    char key;
    unsigned short amount;
    char exit_outer;
    char exit_inner;
    char redraw;
    register int count;

    do {
        target = selected_target;
        view_redraw();
        pick_hero(trade_to_text, 1, 1, &target);
        if (!target)
            exit_outer = 1;
        else {
            char_status();
            do {
                money_show();
                selected_target = target;
                cur = 0;
                prev = 0;
                list = 0;
                count = 0;
                for (coin = 2; coin >= 0; coin--)
                    if (cur_node->coins[coin] != 0) {
                        count++;
                        if (!cur) {
                            pool_alloc(&cur, 0x28, node_pool);
                            cur->next = 0;
                        } else {
                            prev = cur;
                            pool_alloc(&cur, 0x28, node_pool);
                            cur->next = prev;
                        }
                        str_copy(num_text, int_str(cur_node->coins[coin]));
                        pad = 0x12 - str_len(coin_names[coin]) - str_len(num_text);
                        str_copy(line, "");
                        for (column = 1; column <= pad; column++)
                            str_copy(line, msg_format("%s ", line));
                        str_copy(cur->text, msg_format("%s%s%s", coin_names[coin], line,
                                                           num_text, 0L));
                                /* 28aa:2ca2 a fifth argument, unread   */
                        cur->key = 0;
                    }
                list = cur;
                index = 0;
                redraw = 1;
                hotkey_reset(1);
                key = list_menu(coin_title, select_items, 0x14, 9, 0x26, 0xf, cur, 1, 1,
                                &redraw, &index, &prev);
                if (key == 1 || menu_escaped && key == 0x1b)
                    prev = 0;
                if (!prev)
                    exit_inner = 1;
                else {
                    str_copy(desc, "");
                    kind = coin_kind(prev->text, desc);
                    str_copy(desc, msg_format("%s%s%s", how_much_text, desc,
                                                  trade_amount_text));
                    amount = input_amount(cur_node->coins[kind], desc, 7);
                    money_trade(&cur_node, &target, amount, kind);
                    exit_inner = 1;
                    exit_outer = 1;
                    for (coin = 0; coin <= 2; coin++)
                        if (cur_node->coins[coin] > 0) {
                            exit_outer = 0;
                            exit_inner = 0;
                        }
                }
                if (list)
                    list_free(&list);
            } while (!exit_inner);
        }
    } while (!exit_outer);
}

void far money_drop(struct Hero far *h, long amount, short kind) /* 38aa:2e75 */
{
    h->coins[kind] = h->coins[kind] - amount;
    weight_sub(h, amount);
    if (view_mode == 6 || view_mode == 1 || view_mode == 10)
        pool[kind] = pool[kind] + amount;
}

void far drop(char in_view)                                      /* 38aa:2ee1 */
{
    short coin;
    short kind;
    unsigned char pad;
    unsigned char column;
    int index;
    struct Node far *cur;
    struct Node far *prev;
    struct Node far *list;
    char exit_flag;
    char redraw;
    char num_text[0x12];
    char line[0x12];
    char desc[0x29];
    char key;
    unsigned short amount;
    unsigned char left;
    unsigned char right;
    unsigned char top;
    unsigned char bottom;
    register int count;

    do {
        if (in_view)
            money_show();
        cur = 0;
        prev = 0;
        list = 0;
        count = 0;
        for (coin = 2; coin >= 0; coin--)
            if (cur_node->coins[coin] != 0) {
                count++;
                if (!cur) {
                    pool_alloc(&cur, 0x28, node_pool);
                    cur->next = 0;
                } else {
                    prev = cur;
                    pool_alloc(&cur, 0x28, node_pool);
                    cur->next = prev;
                }
                str_copy(num_text, int_str(cur_node->coins[coin]));
                pad = 0x12 - str_length(coin_names[coin]) - str_length(num_text);
                str_copy(line, "");
                for (column = 1; column <= pad; column++)
                    str_copy(line, msg_format("%s ", line));
                str_copy(cur->text, msg_format("%s%s%s", coin_names[coin], line, num_text));
                cur->key = 0;
            }
        list = cur;
        index = 0;
        redraw = 1;
        hotkey_reset(1);
        if (in_view) {
            left = 0x14;
            right = 0x26;
            top = 9;
            bottom = 0xf;
        } else {
            left = 1;
            right = 0x26;
            top = 0x11;
            bottom = 0x16;
        }
        key = list_menu(coin_title, select_items, left, top, right, bottom, cur, 1, 1,
                        &redraw, &index, &prev);
        if (!prev || key == 1 || menu_escaped && key == 0x1b)
            exit_flag = 1;
        else {
            desc[0] = 0;
            kind = coin_kind(prev->text, desc);
            if (view_mode == 10)
                if (str_length(desc) > 7)
                    str_copy(desc, msg_format("How much %sdo you deposit? ", desc));
                else if (kind == 1)
                    str_copy(desc, msg_format("How many %swill you deposit? ", desc));
                else
                    str_copy(desc, msg_format("How much %swill you deposit? ", desc));
            else
                str_copy(desc, msg_format("How much %swill you drop? ", desc));
            amount = input_amount(cur_node->coins[kind], desc, 7);
            money_drop(cur_node, amount, kind);
            exit_flag = 1;
            for (coin = 0; coin <= 2; coin++)
                if (cur_node->coins[coin] > 0)
                    exit_flag = 0;
        }
        if (list)
            list_free(&list);
    } while (!exit_flag);
}

char far donate(long goal, char count)                          /* 38aa:326e */
{
    unsigned char pad;
    unsigned char column;
    char done;
    char num_text[0x12];
    char line[0x12];
    char prompt[0x2a];
    long amount;
    long avail;
    char text[0x24];
    long total;
    char reached;

    reached = 0;
    amount = 0;
    do {
        avail = pool[0];
        if (cur_node->coins[0] != 0 || avail != 0) {
            if (cur_node->coins[0] != 0)
                str_copy(num_text, int_str(cur_node->coins[0]));
            else
                str_copy(num_text, ulong_str(avail));
            pad = 0x12 - str_length(coin_names[0]) - str_length(num_text);
            str_copy(line, "");
            for (column = 1; column <= pad; column++)
                str_copy(line, msg_format("%s ", line));
            str_copy(text, msg_format("%s%s%s", coin_names[0], line, num_text));
        }
        if (cur_node->coins[0] != 0 || avail != 0) {
            text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, text);
            prompt[0] = 0;
            str_copy(prompt, "How much PLATINUM do you donate?");
            if (cur_node->coins[0] != 0) {
                amount = input_amount(cur_node->coins[0], prompt, 7);
                cur_node->coins[0] -= amount;
            } else {
                amount = input_amount(avail, prompt, 7);
                pool[0] -= amount;
            }
            if (amount != 0) {
                text_wrap(1, 0x11, 0x26, 0x16, 7, 0, 1, "Thank you for your generous donation.");
                press_return_clear();
                total = le_long_get(cursor_obj + 0x3f);
                total += amount;
                if (total >= goal && count) {
                    reached = 1;
                    total = 0;
                }
                le_long_put(total, cursor_obj + 0x3f);
            }
        }
        done = 1;
        combat_text_clear();
    } while (!done && !reached);
    return reached;
}

char far cant_carry(struct Hero far *hero, char mode, struct Item far *item) /* 38aa:34dc */
{
    char r;
    char cannot;
    unsigned short weight;
    struct Item far *cur;
    char dup;

    recalc_stats(hero);
    cannot = 0;
    dup = 0;
    cur = hero->items;
    if (item->count > 0 && mode)
        while (!dup && cur) {
            dup = cur->name1 == item->name1 && cur->name2 == item->name2
                  && cur->name3 == item->name3 && cur->type == item->type
                  && cur->plus == item->plus && cur->save_bonus == item->save_bonus
                  && cur->cursed == item->cursed && cur->weight == item->weight
                  && cur->charges == item->charges && cur->charges < 2
                  && cur->effect == item->effect && cur->special == item->special;
            cur = cur->next;
        }
    if (hero->item_count > 0xf && !dup)
        cannot = 1;
    weight = item->weight;
    if (item->count > 0)
        weight = weight * item->count;
    if (hero->encumbrance + weight > str_carry(hero) + 1500)
        cannot = 1;
    r = cannot;
    return r;
}

char far can_heal(struct Hero far *h)                            /* 38aa:366b */
{
    char r;
    struct Effect far *found;

    r = (h->cls == 3 || h->old_level[3] > 0 && old_class_abilities_available(h)) && view_mode != 5
        && h->status == 0 && !find_effect(h, 0x8c, &found);
    return r;
}

char far can_cure(struct Hero far *h)                            /* 38aa:36cc */
{
    char r;

    r = (h->cls == 3 || h->old_level[3] > 0 && old_class_abilities_available(h)) && view_mode != 5
        && h->status == 0 && h->cures > 0;
    return r;
}

void far heal(struct Hero far *h)                                /* 38aa:371d */
{
    struct Hero far *target;

    view_redraw();
    target = node_list;
    pick_hero("Heal whom? ", 1, 0, &target);
    if (target) {
        if (fx_heal(target, class_level(h, 3) * 2, 0)) {
            party_rows(target, 1);
            bottom_msg(msg_format("%s%s", target->name, " feels better"));
        } else
            bottom_msg(msg_format("%s is unaffected", target->name));
        fx_add(h, 0x8c, 0x5a0, 0, 0);
    }
    char_status();
}

void far cure(struct Hero far *h)                                /* 38aa:37fc */
{
    struct Hero far *target;
    char diseased;
    char go;
    struct Effect far *found;

    view_redraw();
    target = node_list;
    pick_hero("Cure whom? ", 1, 0, &target);
    if (target) {
        diseased = 0;
        for (tick_slot = 0; tick_slot < 6; tick_slot++)
            if (find_effect(target, disease_fx[tick_slot], &found))
                diseased = 1;
        go = 1;
        if (!diseased) {
            hero_text(target, "is not diseased", 0, 0);
            go = confirm("cure anyway: ", 0);
            combat_text_clear();
        }
        if (go == 1) {
            fx_lock = 1;
            for (tick_slot = 0; tick_slot < 6; tick_slot++)
                fx_remove(target, disease_fx[tick_slot], 0);
            fx_lock = 0;
            if (h->cures > 0)
                h->cures--;
            if (!find_effect(h, 0x6e, &found))
                fx_add(h, 0x6e, 0x2760, 0, 1);
            if (diseased)
                bottom_msg(msg_format("%s is cured", target->name));
            else
                bottom_msg(msg_format("%s cures the healthy", h->name));
        }
    }
    char_status();
}

char far pick_char(void)                                         /* 38aa:3982 */
{
    char r;
    unsigned char key;
    char saved;

    saved = byte_c3c3;
    byte_c3c3 = 1;
    do {
        hotkey_reset(1);
        key = menu_run(pick_text, select_items, 1, 0);
        if (menu_escaped)
            if (key == 0x1b) {
                key = 1;
                menu_escaped = 0;
            } else {
                party_rows(cur_node, 0);
                party_key_select(key);
                party_rows(cur_node, 1);
            }
    } while (menu_escaped || key <= 1);
    party_rows(cur_node, 1);
    r = key == 0;
    byte_c3c3 = saved;
    return r;
}

void far ticks_start(void)                                       /* 38aa:3a44 */
{
    last_tick = timer_ticks();
}

char far ticks_quick(void)                                       /* 38aa:3a51 */
{
    long now;
    char r;

    now = timer_ticks();
    if (now - last_tick < 0x30)
        r = 1;
    else
        r = 0;
    last_tick = now;
    return r;
}

unsigned char far hp_floor(struct Hero far *h)                   /* 38aa:3a93 */
{
    unsigned char cnt;
    unsigned char r;
    unsigned short sum;
    char mod;
    short i;
    unsigned char excess;
    unsigned char lvl;
    char counted;
    register int base;

    cnt = 0;
    sum = 0;
    for (i = 0; i <= 6; i++)
        if (h->level[i] > 0 && h->level[i] > h->dual_level || h->old_level[i] > 0) {
            mod = con_hp_adj(h, i);
            if (h->old_level[i] > 0) {
                lvl = h->old_level[i];
                counted = 1;
            } else {
                cnt++;
                lvl = h->level[i];
                counted = h->dual_level == 0;
            }
            if (hit_dice_level_limit[i] > lvl) {
                if (!counted && h->dual_level < lvl) {
                    lvl = lvl - h->dual_level;
                    counted = 1;
                } else if (i == 4)
                    lvl++;
                if (counted)
                    sum += (hit_die_sides[i] + mod) * lvl;
            } else {
                if (!counted && h->dual_level < lvl) {
                    counted = 1;
                    if (h->dual_level >= hit_dice_level_limit[i] - 1) {
                        excess = lvl - h->dual_level;
                        base = 0;
                    } else {
                        base = (hit_dice_level_limit[i] - h->dual_level - 1) * (hit_die_sides[i] + mod);
                        excess = lvl - hit_dice_level_limit[i] + 1;
                    }
                } else {
                    excess = lvl - hit_dice_level_limit[i] + 1;
                    base = (hit_die_sides[i] + mod) * (hit_dice_level_limit[i] - 1);
                }
                if (i == 4)
                    base += hit_die_sides[i] + mod;
                if (counted)
                    switch (i) {
                    case 0:
                    case 4:
                    case 6:
                        sum += excess * 2 + base;
                        break;
                    case 2:
                    case 3:
                        sum += excess * 3 + base;
                        break;
                    case 5:
                        sum += excess + base;
                        break;
                    }
            }
        }
    if (cnt > 0)
        sum = sum / cnt;
    else if (h->dual_level == 0)
        sum = 0;
    r = sum;
    return r;
}

void far hp_adjust(struct Hero far *h, char mask, unsigned char divisor, char apply)
                                                                 /* 38aa:3cf1 */
{
    unsigned char roll1;
    unsigned char roll2;
    unsigned char total;
    unsigned char lvl;
    unsigned char secondary;
    short i;
    unsigned char amount;
    unsigned char floor;

    total = 0;
    secondary = 0;
    for (i = 0; i <= 6; i++)
        if (h->level[i] > 0 && (class_bit[i] & mask) > 0)
            if ((int)h->level[i] < hit_dice_level_limit[i]) {   /* 28aa:3d3d an int compare */
                if (h->level[i] > 1)
                    lvl = 1;
                else
                    lvl = class_hit_dice[i];
                roll1 = fx_roll_dice(lvl, hit_die_sides[i]);
                roll2 = fx_roll_dice(lvl, hit_die_sides[i]);
                if (roll2 > roll1)
                    roll1 = roll2;
                total += roll1;
                secondary += hp_bonus(h, i) * lvl;
            } else
                switch (i) {
                case 2:
                case 3:
                    total = 3;
                    break;
                case 0:
                case 4:
                case 6:
                    total = 2;
                    break;
                case 5:
                    total = 1;
                    break;
                }
    if (divisor > 0) {
        total = total / divisor;
        if (total < 1)
            total = 1;
        secondary = secondary / divisor;
        roll1 = h->hp_max - h->hp;
        if (apply) {
            h->hp_base += total;
            h->hp_max += total + secondary;
            if (h->hp_max < roll1)
                h->hp = 0;
            else
                h->hp = h->hp_max - roll1;
        } else {
            amount = total + secondary;
            floor = hp_floor(h);
            if (h->hp_max - amount > floor) {
                amount = h->hp_max - floor;
                total = amount - secondary;
            }
            if (h->hp_max < amount)
                h->hp_max = 0;
            else
                h->hp_max -= amount;
            if (h->hp_base > total)
                h->hp_base -= total;
            else
                h->hp_base = 0;
            if (h->hp_max < roll1)
                h->hp = 0;
            else
                h->hp = h->hp_max - roll1;
        }
    } else {
        h->hp = 0;
        h->hp_base = 0;
        h->hp_max = 0;
    }
}

char far hp_bonus(struct Hero far *h, short cls)                 /* 38aa:3f31 */
{
    char r;
    char bonus;

    bonus = 0;
    bonus += constitution_hp_bonus[h->stats[4].cur];
    if (cls == 2 || cls == 4 || cls == 3 || cls == 1)
        switch (h->stats[4].cur) {
        case 0x11:
            bonus++;
            break;
        case 0x12:
            bonus += 2;
            break;
        case 0x13:
        case 0x14:
            bonus += 3;
            break;
        case 0x15:
        case 0x16:
        case 0x17:
            bonus += 4;
            break;
        case 0x18:
        case 0x19:
            bonus += 5;
            break;
        }
    r = bonus;
    return r;
}

void far pick_hero(char far *prompt, char a, char b, struct Hero far * far *out)
                                                                 /* 38aa:3fb9 */
{
    unsigned char key;
    char mode;
    struct Hero far *cur;
    char saved;
    char saved_context;
    char text[0x2a];
    register int sel;

    set_selected((struct Node far *)*out);
    saved = byte_c3c3;
    order_mode = b;
    picker_start_character = cur_node;
    key = 0xff;
    saved_context = exploration_menu_context;
    exploration_menu_context = 0;
    while (key >= 2 && key != 0x1b) {
        party_display(*out);
        if (view_mode == 2 || view_mode == 6)
            mode = 1;
        else
            mode = 0;
        if (view_mode == 0)
            byte_c3c3 = 1;      /* 28aa:402b overwritten at once        */
        byte_c3c3 = saved;
        cur = *out;
        set_selected((struct Node far *)*out);
        if (a)
            hotkey_reset(1);
        else
            hotkey_reset(0);
        if (*prompt)
            str_copy(text, msg_format("%s%s", prompt, " "));
        else
            str_copy(text, msg_format("%s", prompt));
        key = node_menu(text, select_items, 1, mode);
        cur = (struct Hero far *)get_selected();
        if (menu_escaped) {
            sel = key;
            switch (sel) {
            case 0x1b:
                if (a)
                    cur = 0;
                break;
            case 0xd:
                key = 0;
                break;
            case 0x84:
            case 0x85:
                cur = cur->next;
                if (!cur)
                    cur = node_list;
                break;
            case 0x87:
            case 0x88:
                if (byte_bcef)
                    if (order_mode)
                        cur = picker_start_character;
                    else
                        cur = cur_node;
                else {
                    cur = node_list;
                    if (*out != node_list)
                        while (cur->next != *out)
                            cur = cur->next;
                    else
                        while (cur->next)
                            cur = cur->next;
                }
                break;
            }
        } else if (a && key == 1)
            cur = 0;
        *out = cur;
        set_selected((struct Node far *)*out);
    }
    exploration_menu_context = saved_context;
    order_mode = 0;
}
