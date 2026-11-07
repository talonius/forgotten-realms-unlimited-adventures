/* CKIT.EXE overlay 07 module 21d4: creating a party member.  The pick
 * screen for race, gender, class and alignment, the ability rolls and
 * their racial, age and class limits, training the new member, the name,
 * the icon grid, and the starting levels, spells and items.
 *
 * EXE 0x03b900..0x03f9de, one Turbo C large-model _TEXT segment starting
 * at byte 0 of paragraph 21d4, the first module of RTLink section 7
 * (payload 0x3b900, link segment 21d4, parent section 3; sections 6 and 8
 * to 10 have the same link segment).  Its final retf is byte d of
 * paragraph 25e1, where the next module begins at 25e1:000e (section 7's
 * table files every fixup site before it under frame 21d4, and every one
 * after it under 25e1).  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 7.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, an ovlNN_ name which section and stored
 * segment, and a mod18de_ callee is an RTLink vector stub whose target is
 * named beside it.  Code outside the section reaches three of these
 * routines through vector stubs: 18de:19b3 -> 31bb, 18de:19a9 -> 0f94,
 * 18de:1963 -> 334c; hero_create also far-calls 334c.  Nothing reaches
 * create_unit_init.
 *
 * This is DQK's create.c relinked, with FRUA's record layouts and the
 * changes the bytes show: FRUA's races, classes and icon table; no deity
 * pick; three good-evil choices in the alignment loops where DQK has two;
 * race_start without DQK's byte_c6 store; gender_set's cast to enum Sex,
 * which DQK lacks (without it gender_set is a byte longer, add al,0xff
 * for dec ax); training through section 7's
 * module 25e1; mu_learn setting fixed spell bits by magic-user level and
 * by spell slot; and starting items chosen by class group and start_gear
 * in gear_start, one gear_add call an item.  hero_create takes a
 * destination record: given one, it takes create_xp from 18de:19f9 rather
 * than start_xp, skips the icon screen and the save prompt, and copies
 * the new member there instead of giving items and saving it.
 * align_default and gear_start are static: each is near-called ahead of
 * its definition.  gear_add is static too; nothing outside the module can
 * reach it, so the bytes do not decide.
 *
 * _DATA is align_base and 51 stored literals, DGROUP 70d8..72ac; -d stores
 * "" as the NUL of "DUNGCOM1", "Fighter" as the tail of "Cleric/Fighter",
 * and each literal used more than once once.  _BSS is 27 statics, DGROUP
 * d1c8..d201, in DQK's names and order less DQK's cur_deity, patron_row
 * and deity_blessing; spare_word, unused_byte, deity_nodes and patron_box
 * are referenced by nothing and exist only to hold the layout.
 */

struct Res;                     /* tiles.c's                              */

struct Stat {
    unsigned char base;         /* +0                                     */
    unsigned char cur;          /* +1                                     */
};

struct Item {
    struct Item far *next;      /* +0                                     */
    unsigned char in_use;       /* +4                                     */
    char pad_5[0x23];           /* +5   not read here                     */
    unsigned char type;         /* +28  first of 0x12 bytes from item_templates */
    char pad_29[9];             /* +29  not read here                     */
    char readied;               /* +32                                    */
    unsigned char unid_mask;    /* +33  low 3 bits: not yet identified    */
    char pad_34;                /* +34  not read here                     */
    unsigned char count;        /* +35                                    */
    char pad_36[2];             /* +36  not read here                     */
    unsigned char special;      /* +38  bit 7: gear_add hands it to 1747  */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0  0 cleric, 2 magic-user, as used    */
    unsigned char level;        /* +2                                     */
    char pad_3[0x13];           /* +3   not read here                     */
};

struct Hero {                   /* a party member, 0x18e bytes            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    struct Item far *equipped[13]; /* +c                                     */
    char pad_40[4];             /* +40  not read here                     */
    long xp;                    /* +44                                    */
    char pad_48[4];             /* +48  not read here                     */
    unsigned coins[3];          /* +4c                                    */
    unsigned age;               /* +52  years                             */
    char pad_54[4];             /* +54  not read here                     */
    unsigned char race;         /* +58                                    */
    unsigned char cls;          /* +59                                    */
    char pad_5a[2];             /* +5a  not read here                     */
    unsigned char gender;       /* +5c                                    */
    unsigned char alignment;    /* +5d  (law - 1) * 3 + good - 1          */
    unsigned char status;       /* +5e                                    */
    char pad_5f;                /* +5f  not read here                     */
    char name[0x10];            /* +60                                    */
    struct Stat stats[6];       /* +70  str, int, wis, dex, con, cha      */
    unsigned char str_ex;       /* +7c  exceptional strength              */
    unsigned char str_ex_base;  /* +7d                                    */
    char pad_7e;                /* +7e  not read here                     */
    unsigned char to_hit_base;  /* +7f  best of attack_scores_by_level over classes     */
    unsigned char cures;        /* +80                                    */
    unsigned char hp_max;       /* +81                                    */
    unsigned char icon_dim;     /* +82                                    */
    char pad_83[5];             /* +83  not read here                     */
    unsigned char move_base;    /* +88                                    */
    unsigned char hit_dice;     /* +89                                    */
    char pad_8a[9];             /* +8a  not read here                     */
    unsigned char control;      /* +93  bit 7: not a player character     */
    char pad_94[9];             /* +94  not read here                     */
    unsigned char level[7];     /* +9d  one per class; 0: not that class  */
    unsigned char old_level[7];  /* +a4                                   */
    unsigned char attack_rate;  /* +ab                                    */
    char pad_ac;                /* +ac  not read here                     */
    unsigned char dice_n_base_main; /* +ad                                    */
    char pad_ae;                /* +ae  not read here                     */
    unsigned char dice_s_base_main; /* +af                                    */
    char pad_b0[3];             /* +b0  not read here                     */
    unsigned char ac_base;      /* +b3                                    */
    char pad_b4[3];             /* +b4  not read here                     */
    unsigned char class_mask;   /* +b7  sum of class_mask_bits bits              */
    unsigned char hp_base;      /* +b8                                    */
    char pad_b9[3];             /* +b9  not read here                     */
    unsigned char icon_body;    /* +bc  0..0x30, the icon grid's choice   */
    unsigned char icon_slot;    /* +bd  icon slot                         */
    char pad_be;                /* +be  not read here                     */
    unsigned char type_flags;   /* +bf                                    */
    unsigned char immunity;     /* +c0                                    */
    unsigned char item_count;   /* +c1                                    */
    char pad_c2[0x91];          /* +c2  not read here                     */
    unsigned char known[0x10];  /* +153 one bit per spell, from 1         */
    unsigned char slots[3][9];  /* +163                                   */
    char active;                /* +17e                                   */
    char pad_17f[0xc];          /* +17f not read here                     */
    unsigned char hp;           /* +18b                                   */
    char pad_18c[2];            /* +18c not read here                     */
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

struct AgeLimits {              /* 0xa bytes a race                       */
    unsigned age[5];            /* +0                                     */
};

struct RaceClasses {            /* 0x1b bytes a race                      */
    unsigned char count;        /* +0                                     */
    int classes[13];            /* +1                                     */
};

struct ClassAligns {            /* 0x15 bytes a class                     */
    unsigned char count;        /* +0                                     */
    int aligns[10];             /* +1                                     */
};

enum Sex { SEX_MALE, SEX_FEMALE };    /* a cast to it keeps gender_set's dec ax  */
struct Choices {                /* the pick screen's radio groups         */
    int race;                   /* +0  from 1                             */
    unsigned law;               /* +2  from 1: lawful, neutral, chaotic   */
    unsigned good;              /* +4  from 1: good, neutral, evil        */
    int cls;                    /* +6  from 1                             */
    int gender;                 /* +8  from 1                             */
};

struct AgeRoll {                /* 4 bytes a class                        */
    int base;                   /* +0                                     */
    unsigned char dice;         /* +2                                     */
    unsigned char sides;        /* +3                                     */
};

struct ClassMins {              /* 6 bytes a class                        */
    unsigned char stat_min[6];  /* +0  str, int, wis, dex, con, cha       */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern unsigned char far first_level(struct Hero far *h);        /* 03d0:212e */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp); /* 05f5:01de */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far pal_select(struct Res far *r);                   /* 05f5:06b3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern char far input_result(void);                              /* 06c9:001d */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern char far *far input_text(char far *prompt, unsigned char fg, unsigned char bg,
                                unsigned char max);              /* 06c9:05bc */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far frame_invalidate(void);                          /* 083a:0d3c */
extern void far set_extra_row(char c);                           /* 083a:23ae */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far icon_clear(unsigned char idx);                   /* 0cb5:004d */
extern void far icon_load(char far *name, unsigned char set, unsigned char idx);
                                                                 /* 0cb5:0093 */
extern void far icon_draw(int col, int row, unsigned char kind,
                          unsigned char bank, unsigned char idx); /* 0cb5:0299 */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far input_pending_clear(void);                                 /* 0d48:010a */
extern void far *far rec_alloc(void);                            /* 0d48:013c */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_right_panel(void);                                /* 0d65:0233 */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern int  far pointer_mode_set(int mode);                          /* 0f5e:0a85 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far mouse_move_to(int row, int col);                 /* 0f5e:10a3 */
extern void far mouse_fake(int button, char both);               /* 0f5e:10d5 */
extern void far text_xy_to(int row, int col, int x, int y,
                           int far *out_row, int far *out_col);  /* 107e:04f8 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_run(int (far *idle)(void));              /* 15e5:1589 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far name_clean(char far *name);            /* overlay 3 1b41:003f */
extern void far train_hero(void);                           /* 25e1:06b9 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far character_icon_load(char on);
                                /* 18de:16c5, vector to overlay 4 1ffa:04aa */
extern void far character_file_save(struct Hero far *hero, char far *file);
                                /* 18de:16cf, vector to overlay 4 1ffa:0c2c */
extern void far saving_throws(struct Hero far *hero);
                                /* 18de:173d, vector to overlay 14 2ccd:0a0f */
extern void far item_effect(struct Item far *item, char on);
                                /* 18de:1747, vector to overlay 14 28aa:16f0 */
extern char far ready_check(struct Hero far *hero, struct Item far *item,
                             unsigned char far *slot);
                                /* 18de:1751, vector to overlay 14 28aa:189f */
extern void far modify_hero(void);
                                /* 18de:1959, vector to overlay 6 22c2:12dc */
extern void far char_status(void);
                                /* 18de:198b, vector to overlay 14 28aa:0000 */
extern void far stat_show(int stat, char active);
                                /* 18de:1995, vector to overlay 14 28aa:0987 */
extern void far combat_show(void);
                                /* 18de:199f, vector to overlay 14 28aa:0679 */
extern void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove);
                                /* 18de:19bd, vector to overlay 19 2e88:1648 */
extern void far cleric_slots(struct Hero far *h);
                                /* 18de:19c7, vector to overlay 14 2ccd:000d */
extern void far money_show(void);
                                /* 18de:19d1, vector to overlay 14 28aa:057a */
extern void far hp_adjust(struct Hero far *hero, char classes, char count, char c);
                                /* 18de:19e5, vector to overlay 14 28aa:3cf1 */
extern void far thief_skills(struct Hero far *h);
                                /* 18de:19ef, vector to overlay 14 2ccd:0898 */
extern long far xp_input(void);
                                /* 18de:19f9, vector to overlay 25 2e62:0b3d */

extern struct RaceLimits race_limits[];                          /* 0fd2 */
extern struct RaceClasses race_classes[];                        /* 1032 */
extern struct AgeRoll start_ages[][7];                           /* 10d4 */
extern struct AgeLimits age_limits[];                            /* 117c */
extern struct ClassMins class_mins[];                            /* 11b8 */
extern struct ClassAligns class_aligns[];                        /* 121e */
extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern char far *class_names[];                                  /* 3914 */
extern char far *race_names[];                                   /* 395c */
extern char far *sex_names[];                                    /* 399c */
extern char far *ready_text;                                     /* 3ac4 */
extern unsigned char attack_scores_by_level[][22];                             /* 61b1 */
extern unsigned char class_mask_bits[];                                 /* 624b */
extern unsigned char bit_mask[8];                                /* 6711 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern long create_xp;                                           /* b42b */
extern char training_done;                                           /* b722 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern long start_xp;                                           /* ba28 */
extern long start_platinum;                                           /* ba2c */
extern long start_gems;                                           /* ba30 */
extern long start_jewelry;                                           /* ba34 */
extern unsigned char start_gear;                                  /* ba3a */
extern char free_train;                                          /* c524 */
extern char far *item_templates;                                       /* d122 */

static unsigned char align_base = 0;                             /* 70d8 */

static int key_y;                                                /* d1c8 */
static int key_col;                                              /* d1ca */
static char align_ok;                                            /* d1cc */
static int icon_sel;                                             /* d1cd */
static int title_hue;                                            /* d1cf */
static struct Hero far *new_hero;                                /* d1d1 */
static unsigned char law_try;                                    /* d1d5 */
static unsigned char good_scan;                                  /* d1d6 */
static int spare_word;                                           /* d1d7 */
static struct Hero far *work_hero;                               /* d1d9 */
static int list_attr;                                            /* d1dd */
static struct Hero far *char_before;                             /* d1df */
static char icon_closed;                                         /* d1e3 */
static int icon_left;                                            /* d1e4 */
static char unused_byte;                                         /* d1e6 */
static unsigned char align_total;                                /* d1e7 */
static int icon_row_no;                                          /* d1e8 */
static int stat_button;                                          /* d1ea */
static void far *deity_nodes;                                    /* d1ec */
static char patron_box;                                          /* d1f0 */
static char moral_matched;                                       /* d1f1 */
static char order_matched;                                       /* d1f2 */
static char icon_redraw;                                         /* d1f3 */
static unsigned char portrait_orig;                              /* d1f4 */
static int byte_c5_kept;                                         /* d1f5 */
static char icon_cancelled;                                      /* d1f7 */
static struct Choices create_choices;                            /* d1f8 */

static void align_default(void);
void far mu_learn(struct Hero far *h);
static void gear_start(struct Hero far *h);

/* The translated Pascal unit's empty initialization block (DKK chargen.pas). */
void far create_unit_init(void)                                       /* 31d4:0000 */
{
}

void far icon_default(void)                                      /* 31d4:0001 */
{
    if (cur_node->gender == 0) {
        if (cur_node->race == 0 || cur_node->race == 1) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 8;
                break;
            case 3:
            case 4:
            case 10:
                cur_node->icon_body = 9;
                break;
            case 6:
                cur_node->icon_body = 0x13;
                break;
            case 0:
            case 11:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x17;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x1a;
                else
                    cur_node->icon_body = 0x2d;
                break;
            case 5:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x1b;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x1c;
                else
                    cur_node->icon_body = 0x1d;
                break;
            case 1:
            case 7:
                cur_node->icon_body = 0;
                break;
            }
        } else if (cur_node->race == 5) {
            switch (cur_node->cls) {
            case 1:
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0;
                break;
            case 3:
                cur_node->icon_body = 3;
                break;
            case 4:
            case 10:
                cur_node->icon_body = 0x10;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x11;
                break;
            case 0:
            case 11:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x17;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x1a;
                else
                    cur_node->icon_body = 0x2d;
                break;
            case 5:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x1b;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x1c;
                else
                    cur_node->icon_body = 0x1d;
                break;
            case 7:
                cur_node->icon_body = 1;
                break;
            }
        } else if (cur_node->race == 2) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0x22;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x2f;
                break;
            }
        } else if (cur_node->race == 3) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0x29;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x2a;
                break;
            }
        } else if (cur_node->race == 4) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0x25;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x26;
                break;
            }
        }
    } else {
        if (cur_node->race == 0 || cur_node->race == 1) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0xf;
                break;
            case 3:
            case 4:
            case 10:
                cur_node->icon_body = 0xe;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x15;
                break;
            case 0:
            case 11:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x18;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x19;
                else
                    cur_node->icon_body = 0x2e;
                break;
            case 5:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x1f;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x20;
                else
                    cur_node->icon_body = 0x21;
                break;
            case 1:
            case 7:
                cur_node->icon_body = 0;
                break;
            }
        } else if (cur_node->race == 5) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0xa;
                break;
            case 1:
            case 3:
                cur_node->icon_body = 0xc;
                break;
            case 4:
            case 10:
                cur_node->icon_body = 0x24;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x14;
                break;
            case 0:
            case 11:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x18;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x19;
                else
                    cur_node->icon_body = 0x2e;
                break;
            case 5:
                if (cur_node->alignment % 3 == 0)
                    cur_node->icon_body = 0x1f;
                else if (cur_node->alignment % 3 == 1)
                    cur_node->icon_body = 0x20;
                else
                    cur_node->icon_body = 0x21;
                break;
            case 7:
                cur_node->icon_body = 0;
                break;
            }
        } else if (cur_node->race == 2) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0x23;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x30;
                break;
            }
        } else if (cur_node->race == 3) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0x2b;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x2c;
                break;
            }
        } else if (cur_node->race == 4) {
            switch (cur_node->cls) {
            case 2:
            case 8:
            case 9:
            case 13:
            case 14:
            case 15:
                cur_node->icon_body = 0x27;
                break;
            case 6:
            case 12:
            case 16:
                cur_node->icon_body = 0x28;
                break;
            }
        }
    }
}

void far icon_pair(unsigned char col, unsigned char row)         /* 31d4:0629 */
{
    col = col + 7;
    row = row - 1;
    icon_draw(col, row - 1000, 0, 0, 0x1e);
    icon_draw(col + 3, row - 1000, 0, 0, 0x1f);
    icon_draw(col, row - 1000, 0, 0, 8);
    icon_draw(col + 3, row - 1000, 0, 1, 8);
    screen_flush();
}

void far icon_click(int id, int x, int y)                        /* 31d4:06c5 */
{
    int row;
    int col;
    short pos;

    screen_draw_begin(1);
    text_xy_to(0x1f44, 0x1f44, x, y, &row, &col);
    icon_row_no = row / 12;
    icon_left = col / 12;
    pos = icon_row_no * 7 + icon_left;
    cur_node->icon_body = icon_sel;
    if (pos != icon_sel) {
        icon_draw(icon_sel % 7, icon_sel / 7, 0, 0, 0x1e);
        character_icon_load(1);
        icon_draw(icon_sel % 7, icon_sel / 7, 0, 0, 8);
        cur_node->icon_body = pos;
        icon_draw(icon_left, icon_row_no, 0, 0, 0x1f);
        screen_flush();
        character_icon_load(1);
        icon_draw(icon_left, icon_row_no, 0, 0, 8);
        screen_flush();
    }
    ui_mark(id, 0x14);
    icon_sel = pos;
    screen_flush();
}

void far icon_refresh(void)                                      /* 31d4:07e8 */
{
    character_icon_load(1);
}

void far icon_done_cb(void)                                      /* 31d4:07f2 */
{
    icon_closed = 1;
}

void far icon_exit_cb(void)                                      /* 31d4:07f8 */
{
    icon_cancelled = 1;
    icon_closed = 1;
}

void far icon_screen(void)                                       /* 31d4:0803 */
{
    char buf[0x50];
    unsigned char saved;
    struct Res far *res;

    saved = cur_node->icon_body;
    icon_sel = saved;
    cur_node->icon_body = 0;
    if (icon_redraw) {
        screen_draw_begin(0);
        text_clear(1, 1, 0x26, 0x16);
        frame_right_panel();
        pal_select(0);
        screen_flush();
        set_open_mode(0x32);
        res = 0;
        tile_open_file("DUNGCOM1", 1, 0, 0, &res);
        res_palette_load(res);
        res_free(&res);
        for (tick_slot = 0; tick_slot <= 0x30; tick_slot++) {
            screen_draw_begin(1);
            if (tick_slot == icon_sel)
                icon_draw(tick_slot % 7, tick_slot / 7, 0, 0, 0x1f);
            else
                icon_draw(tick_slot % 7, tick_slot / 7, 0, 0, 0x1e);
            character_icon_load(1);
            icon_draw(tick_slot % 7, tick_slot / 7, 0, 0, 8);
            screen_flush();
            cur_node->icon_body++;
            if (cur_node->icon_body > 0x30)
                cur_node->icon_body = 0;
        }
        icon_redraw = 0;
    }
    cur_node->icon_body = saved;
    status_printf(0x17, 8, 7, 8, "%s", cur_node->name);
    if (cur_node->control & 0x80)
        status_printf(str_len(cur_node->name) + 3, 0x17, 7, 8, "(NPC)");
    str_copy(buf, sex_names[cur_node->gender]);
    status_printf(0x17, 9, 7, 8, buf);
    str_copy(buf, race_names[cur_node->race]);
    status_printf(0x17, 0xa, 7, 8, buf);
    switch (cur_node->cls) {
    case 9:
        str_copy(buf, "Cleric/Fighter");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Magic-User");
        status_printf(0x17, 0xd, 7, 8, buf);
        break;
    case 10:
        str_copy(buf, "Cleric");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Ranger");
        status_printf(0x17, 0xd, 7, 8, buf);
        break;
    case 11:
        str_copy(buf, "Cleric");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Magic-User");
        status_printf(0x17, 0xd, 7, 8, buf);
        break;
    case 12:
        str_copy(buf, "Cleric");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Thief");
        status_printf(0x17, 0xd, 7, 8, buf);
        break;
    case 13:
        str_copy(buf, "Fighter");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Magic-User");
        status_printf(0x17, 0xd, 7, 8, buf);
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 14:
        str_copy(buf, class_names[cur_node->cls]);
        status_printf(0x17, 0xc, 7, 8, buf);
        break;
    case 15:
        str_copy(buf, "Fighter");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Magic-User");
        status_printf(0x17, 0xd, 7, 8, buf);
        str_copy(buf, "Thief");
        status_printf(0x17, 0xe, 7, 8, buf);
        break;
    case 16:
        str_copy(buf, "Magic-User");
        status_printf(0x17, 0xc, 7, 8, buf);
        str_copy(buf, "Thief");
        status_printf(0x17, 0xd, 7, 8, buf);
        break;
    }
    text_print_at(0x1f48, 0x1f9e, list_attr, ready_text);
}

int far icon_key(int id, int key)                                /* 31d4:0c7e */
{
    short pos;
    register int moved;

    pos = icon_sel;
    key = (key & 0x7f) + 0x80;
    switch (key) {
    case 0x86:                          /* leaves moved unset, as shipped */
        if (pos % 7)
            pos = pos - 1;
        else
            pos = pos + 6;
        break;
    case 0x88:
        if (pos / 7)
            pos = pos - 7;
        else
            pos = pos + 42;
        moved = 1;
        break;
    case 0x82:
        if (pos % 7 != 6)
            pos = pos + 1;
        else
            pos = pos - 6;
        moved = 1;
        break;
    case 0x84:
        if (pos / 7 != 6)
            pos = pos + 7;
        else
            pos = pos - 42;
        moved = 1;
        break;
    default:
        moved = 0;
        break;
    }
    cur_node->icon_body = icon_sel;
    icon_draw(icon_sel % 7, icon_sel / 7, 0, 0, 0x1e);
    character_icon_load(1);
    icon_draw(icon_sel % 7, icon_sel / 7, 0, 0, 8);
    cur_node->icon_body = pos;
    icon_draw(pos % 7, pos / 7, 0, 0, 0x1f);
    character_icon_load(1);
    icon_draw(pos % 7, pos / 7, 0, 0, 8);
    ui_mark(id, 0x14);
    icon_sel = pos;
    screen_flush();
    return moved;
}

char far icon_menu(void)                                         /* 31d4:0e1f */
{
    portrait_orig = cur_node->icon_body;
    icon_closed = 0;
    icon_load("COMSPR", 0xf, 0x1e);
    icon_load("COMSPR", 0x10, 0x1f);
    list_attr = 0x87;
    set_extra_row(1);
    icon_screen();
    icon_refresh();
    icon_pair(1, 2);
    if (icon_redraw) {
        icon_row_no = portrait_orig / 7;
        icon_left = portrait_orig % 7;
        icon_sel = portrait_orig;
    }
    frame_invalidate();
    dl_begin();
    ui_add(5, 0x1f44, 0x1f44, 0x54, 0x54, 0x22, icon_click, 0);
    ui_add(1, 0x1f9e, 0x1f44, "Done", 0x14, 0x20, 'D', 0x24, 4, 0x22, icon_done_cb, 0x15,
           1, 0x1f9e, 0x1f58, "Exit", 0x14, 0x20, 'E', 0x21, 0x23, 0x24, 4, 0x22, icon_exit_cb,
           0x15,
           7, icon_key, 0);
    dl_draw(1);
    screen_flush();
    ui_run(0);
    dl_end();
    set_extra_row(0);
    icon_clear(8);
    icon_clear(0x1e);
    icon_clear(0x1f);
    return icon_closed;
}

char far icon_customize(char redraw)                              /* 31d4:0f94 */
{
    struct Hero save;
    struct Hero copy;

    byte_c5_kept = cur_node->icon_slot;
    cur_node->icon_slot = 8;
    char_before = &save;
    mem_move(cur_node, char_before, sizeof(struct Hero));
    work_hero = &copy;
    mem_move(cur_node, work_hero, sizeof(struct Hero));
    icon_cancelled = 0;
    icon_redraw = 1;
    while (!icon_menu())
        ;
    if (icon_cancelled)
        mem_move(char_before, cur_node, sizeof(struct Hero));
    cur_node->icon_slot = byte_c5_kept;
    if (redraw)
        character_icon_load(1);
    return !icon_cancelled;
}

void far class_start(struct Hero far *h)                         /* 31d4:105f */
{
    switch (h->cls) {
    case 0:
    case 2:
    case 5:
    case 6:
        h->level[h->cls] = 1;
        break;
    case 3:
        h->cures = 1;
        h->level[3] = 1;
        fx_add(h, 8, 0, 0xff, 0);
        break;
    case 4:
        h->level[4] = 1;
        fx_add(h, 0x69, 0, 0xff, 0);
        break;
    case 8:
        h->level[0] = 1;
        h->level[2] = 1;
        h->xp = create_xp / 2;
        break;
    case 9:
        h->level[0] = 1;
        h->level[2] = 1;
        h->level[5] = 1;
        h->xp = create_xp / 3;
        break;
    case 10:
        h->level[0] = 1;
        h->level[4] = 1;
        fx_add(h, 0x69, 0, 0xff, 0);
        h->xp = create_xp / 2;
        break;
    case 11:
        h->level[0] = 1;
        h->level[5] = 1;
        h->xp = create_xp / 2;
        break;
    case 12:
        h->level[0] = 1;
        h->level[6] = 1;
        h->xp = create_xp / 2;
        break;
    case 13:
        h->level[2] = 1;
        h->level[5] = 1;
        h->xp = create_xp / 2;
        break;
    case 14:
        h->level[2] = 1;
        h->level[6] = 1;
        h->xp = create_xp / 2;
        break;
    case 15:
        h->level[2] = 1;
        h->level[5] = 1;
        h->level[6] = 1;
        h->xp = create_xp / 3;
        break;
    case 16:
        h->level[5] = 1;
        h->level[6] = 1;
        h->xp = create_xp / 2;
        break;
    }
}

void far stat_adjust(short stat, struct Hero far *h)             /* 31d4:11ee */
{
    switch (stat) {
    case 0:
        if (h->stats[stat].cur > 0 && h->age > age_limits[h->race].age[1])
            h->stats[stat].cur++;
        if (h->stats[stat].cur > 0 && h->age > age_limits[h->race].age[2])
            h->stats[stat].cur--;
        if (h->stats[stat].cur > 0 && h->age > age_limits[h->race].age[3])
            h->stats[stat].cur -= 2;
        if (h->stats[stat].cur > 0 && h->age > age_limits[h->race].age[4])
            h->stats[stat].cur--;
        if (h->stats[stat].cur < race_limits[h->race].str_min[h->gender])
            h->stats[stat].cur = race_limits[h->race].str_min[h->gender];
        if (h->stats[stat].cur > race_limits[h->race].str_max[h->gender])
            h->stats[stat].cur = race_limits[h->race].str_max[h->gender];
        if (h->stats[stat].cur < class_mins[h->cls].stat_min[stat])
            h->stats[stat].cur = class_mins[h->cls].stat_min[stat];
        if (h->stats[0].cur == 18 &&
            (h->level[2] > 0 || h->level[4] > 0 || h->level[3] > 0 || h->level[1] > 0)) {
            h->str_ex = random_n(100) + 1;
            if (h->str_ex > race_limits[h->race].exc_max[h->gender])
                h->str_ex = race_limits[h->race].exc_max[h->gender];
        }
        break;
    case 1:
        if (h->age > age_limits[h->race].age[2])
            h->stats[stat].cur++;
        if (h->age > age_limits[h->race].age[4])
            h->stats[stat].cur++;
        if (h->stats[stat].cur < race_limits[h->race].int_min)
            h->stats[stat].cur = race_limits[h->race].int_min;
        if (h->stats[stat].cur > race_limits[h->race].int_max)
            h->stats[stat].cur = race_limits[h->race].int_max;
        if (h->stats[stat].cur < class_mins[h->cls].stat_min[stat])
            h->stats[stat].cur = class_mins[h->cls].stat_min[stat];
        break;
    case 2:
        if (h->age <= age_limits[h->race].age[1])
            h->stats[stat].cur--;
        if (h->age > age_limits[h->race].age[2])
            h->stats[stat].cur++;
        if (h->age > age_limits[h->race].age[3])
            h->stats[stat].cur++;
        if (h->age > age_limits[h->race].age[4])
            h->stats[stat].cur++;
        if (h->stats[stat].cur < race_limits[h->race].wis_min)
            h->stats[stat].cur = race_limits[h->race].wis_min;
        if (h->stats[stat].cur > race_limits[h->race].wis_max)
            h->stats[stat].cur = race_limits[h->race].wis_max;
        if (h->stats[stat].cur < class_mins[h->cls].stat_min[stat])
            h->stats[stat].cur = class_mins[h->cls].stat_min[stat];
        if (h->stats[stat].cur < 13 && h->cls <= 8 && h->cls >= 12) /* never true */
            h->stats[stat].cur = 13;
        break;
    case 3:
        if (h->age > age_limits[h->race].age[3])
            h->stats[stat].cur -= 2;
        if (h->age > age_limits[h->race].age[4])
            h->stats[stat].cur--;
        if (h->stats[stat].cur < race_limits[h->race].dex_min)
            h->stats[stat].cur = race_limits[h->race].dex_min;
        if (h->stats[stat].cur > race_limits[h->race].dex_max)
            h->stats[stat].cur = race_limits[h->race].dex_max;
        if (h->stats[stat].cur < class_mins[h->cls].stat_min[stat])
            h->stats[stat].cur = class_mins[h->cls].stat_min[stat];
        break;
    case 4:
        if (h->age > age_limits[h->race].age[2])
            h->stats[stat].cur--;
        if (h->age > age_limits[h->race].age[3])
            h->stats[stat].cur--;
        if (h->age > age_limits[h->race].age[4])
            h->stats[stat].cur--;
        if (h->stats[stat].cur < race_limits[h->race].con_min)
            h->stats[stat].cur = race_limits[h->race].con_min;
        if (h->stats[stat].cur > race_limits[h->race].con_max)
            h->stats[stat].cur = race_limits[h->race].con_max;
        if (h->stats[stat].cur < class_mins[h->cls].stat_min[stat])
            h->stats[stat].cur = class_mins[h->cls].stat_min[stat];
        break;
    case 5:
        if (h->stats[stat].cur < race_limits[h->race].cha_min)
            h->stats[stat].cur = race_limits[h->race].cha_min;
        if (h->stats[stat].cur > race_limits[h->race].cha_max)
            h->stats[stat].cur = race_limits[h->race].cha_max;
        if (h->stats[stat].cur < class_mins[h->cls].stat_min[stat])
            h->stats[stat].cur = class_mins[h->cls].stat_min[stat];
        break;
    }
}

void far race_start(struct Hero far *h)                          /* 31d4:1b97 */
{
    switch (h->race) {
    case 2:
        fx_add(h, 0x2f, 0, 0xff, 0);
        fx_add(h, 0x1a, 0, 0xff, 0);
        fx_add(h, 0x61, 0, 0xff, 0);
        break;
    case 3:
        fx_add(h, 0x30, 0, 0xff, 0);
        fx_add(h, 7, 0, 0xff, 0);
        fx_add(h, 0x61, 0, 0xff, 0);
        break;
    case 4:
        fx_add(h, 0x61, 0, 0xff, 0);
        break;
    case 0:
        fx_add(h, 0x5f, 0, 0xff, 0);
        break;
    case 1:
        fx_add(h, 0x12, 0, 0xff, 0);
        break;
    }
}

char far name_input(struct Hero far *h)                          /* 31d4:1c75 */
{
    char buf[0x10];

    do {
        str_copy(h->name, input_text("Character name:", 0, 0, 0xf));
        str_copy(buf, h->name);
        name_clean(buf);
    } while (buf[0] == 0 && input_result() || buf[0] >= '0' && buf[0] <= '9');
    return input_result();
}

int far stats_menu(void)                                         /* 31d4:1cee */
{
    frame_invalidate();
    dl_begin();
    ui_add(1, 0x1f9e, 0x1f94, "Done", 0x14, 0x20, 'D', 0x24, 4, 0x15,
           1, 0x1f9e, 0x1f44, "Reroll Stats", 0x14, 0x20, 'R', 0x24, 0xc, 0x15,
           1, 0x1f9e, 0x1f78, "Modify", 0x14, 0x20, 'M', 0x24, 6, 0x15,
           1, 0x1f9e, 0x1fa8, "Exit", 0x14, 0x20, 'E', 0x21, 0x23, 0x24, 4, 0x15,
           0);
    stat_button = ui_run(0);
    dl_end();
    return stat_button;
}

void far stats_roll(struct Hero far *h)                          /* 31d4:1dc6 */
{
    short i;
    unsigned char k;
    unsigned char roll;
    char count;
    unsigned char saved;
    char any;
    char buf[0x2a];
    short j;
    char train_saved;

    for (j = 0; j <= 6; j++)
        if (cur_node->level[j] > 0)
            cur_node->level[j] = 1;
    h->str_ex = 0;
    for (i = 0; i <= 5; i++) {
        h->stats[i].cur = 0;
        for (k = 1; k <= 6; k++) {
            roll = fx_roll_dice(3, 6) + 1;
            switch (h->race) {
            case 2:
                switch (i) {
                case 4:
                    roll++;
                    break;
                case 5:
                    roll--;
                    break;
                }
                break;
            case 0:
                switch (i) {
                case 3:
                    roll++;
                    break;
                case 4:
                    roll--;
                    break;
                }
                break;
            case 4:
                switch (i) {
                case 0:
                    roll--;
                    break;
                case 3:
                    roll++;
                    break;
                }
                break;
            }
            if (h->stats[i].cur < roll)
                h->stats[i].cur = roll;
        }
        stat_adjust(i, h);
        h->stats[i].base = h->stats[i].cur;
        if (i == 0)
            h->str_ex_base = h->str_ex;
        stat_show(i, 0);
    }
    h->hp = h->hp_max;
    h->attack_rate = 2;
    h->dice_n_base_main = 1;
    h->dice_s_base_main = 2;
    h->move_base = 0xc;
    count = 0;
    mem_fill(h->slots, 0x1b, 0);
    for (j = 0; j <= 6; j++) {
        if (h->level[j] > 0) {
            switch (j) {
            case 0:
                h->slots[0][0] = 2;
                cleric_slots(h);
                for (tick_slot = 1; tick_slot <= 0x7e; tick_slot++)
                    if (spell_info[tick_slot].kind == 0 && spell_info[tick_slot].level == 1)
                        h->known[(tick_slot - 1) / 8] =
                            h->known[(tick_slot - 1) / 8] | bit_mask[(tick_slot - 1) % 8];
                break;
            }
            count++;
        }
    }
    h->coins[0] = start_platinum;
    h->coins[2] = start_jewelry;
    h->coins[1] = start_gems;
    h->hp_max = 0;
    h->hp_base = 0;
    h->hp = 0;
    hp_adjust(h, 0xff, count, 1);
    saved = cursor_obj[0x30];
    cursor_obj[0x30] = 0xff;
    training_done = 0;
    set_extra_row(1);
    train_saved = free_train;
    free_train = 0;
    do
        train_hero();
    while (!training_done);
    free_train = train_saved;
    cursor_obj[0x30] = saved;
    set_extra_row(0);
    any = 0;
    buf[0] = 0;
    for (j = 0; j <= 6; j++) {
        if (cur_node->level[j] > 0 ||
            first_level(cur_node) > cur_node->old_level[j] && cur_node->old_level[j] > 0) {
            if (any)
                str_cat(buf, "/");
            str_cat(buf, byte_str(cur_node->level[j] + cur_node->old_level[j]));
            any = 1;
        }
    }
    str_cat(buf, " ");
    status_printf(7, 7, 0xb, 0, buf);
    combat_show();
    money_show();
}

char far hero_finish(struct Hero far *h)                         /* 31d4:2176 */
{
    short i;

    if (h->cls <= 6)
        h->age = fx_roll_dice(start_ages[h->race][h->cls].dice,
                              start_ages[h->race][h->cls].sides) +
                 start_ages[h->race][h->cls].base + 10;
    else
        switch (h->cls) {
        case 14:
            h->age = start_ages[h->race][2].sides * start_ages[h->race][2].dice +
                     start_ages[h->race][2].base;
            break;
        case 13:
        case 15:
        case 16:
            h->age = start_ages[h->race][5].sides * start_ages[h->race][5].dice +
                     start_ages[h->race][5].base;
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            h->age = start_ages[h->race][0].sides * start_ages[h->race][0].dice +
                     start_ages[h->race][0].base;
            break;
        }
    cur_node = h;
    char_status();
    stats_roll(h);
    do {
        stats_menu();
        if (stat_button == 2) {
            set_extra_row(1);
            modify_hero();
            set_extra_row(0);
            char_status();
        }
        if (stat_button == 1) {
            stats_roll(h);
            input_pending_clear();
        }
    } while (stat_button != 3 && stat_button != 0);
    if (stat_button == 3)
        return 0;
    char_status();
    for (i = 0; i <= 5; i++)
        h->stats[i].base = h->stats[i].cur;
    h->str_ex_base = h->str_ex;
    return 1;
}

void far hero_setup(void)                                        /* 31d4:243e */
{
    register int i;

    race_start(new_hero);
    new_hero->xp = create_xp;
    new_hero->hit_dice = 1;
    class_start(new_hero);
    if (new_hero->level[6] > 0)
        thief_skills(new_hero);
    new_hero->class_mask = 0;
    new_hero->to_hit_base = 0;
    for (i = 0; i <= 6; i++) {
        if (new_hero->level[i] > 0) {
            if (attack_scores_by_level[i][new_hero->level[i]] > new_hero->to_hit_base)
                new_hero->to_hit_base = attack_scores_by_level[i][new_hero->level[i]];
            new_hero->class_mask += class_mask_bits[i];
        }
    }
    saving_throws(new_hero);
    cur_node = new_hero;
}

void far pick_exit(void)                                         /* 31d4:2539 */
{
    icon_cancelled = 1;
}

void far align_set(void)                                         /* 31d4:253f */
{
    new_hero->alignment = (create_choices.law - 1) * 3 + create_choices.good - 1;
}

void far align_fix(void)                                         /* 31d4:2557 */
{
    char found;

    align_total = class_aligns[new_hero->cls + align_base].count;
    align_ok = 0;
    for (tick_slot = 0; tick_slot < align_total && !align_ok; tick_slot++) {
        new_hero->alignment = (create_choices.law - 1) * 3 + create_choices.good - 1;
        if (class_aligns[new_hero->cls + align_base].aligns[tick_slot] == new_hero->alignment)
            align_ok = 1;
    }
    if (align_ok)
        return;
    found = 0;
    for (create_choices.good = 1; create_choices.good <= 3 && !found; create_choices.good++) {
        new_hero->alignment = (create_choices.law - 1) * 3 + create_choices.good - 1;
        for (tick_slot = 0; tick_slot < align_total && !found; tick_slot++)
            if (class_aligns[new_hero->cls + align_base].aligns[tick_slot] == new_hero->alignment)
                found = 1;
    }
    create_choices.good--;
    if (!found)
        align_default();
}

void far align_mark(void)                                        /* 31d4:269d */
{
    char hit;
    unsigned char k;
    unsigned char n;

    n = class_aligns[new_hero->cls + align_base].count;
    for (k = 1; k <= 3; k++) {
        hit = 0;
        for (tick_slot = 0; tick_slot < n && !hit; tick_slot++) {
            new_hero->alignment = (create_choices.law - 1) * 3 + (k - 1);
            if (class_aligns[new_hero->cls + align_base].aligns[tick_slot] == new_hero->alignment)
                hit = 1;
        }
        if (hit)
            ui_mark(k + 0x20, 0x18);
        else
            ui_mark(k + 0x20, 0x10);
    }
}

static void align_default(void)                                  /* 31d4:277b */
{
    switch (create_choices.cls) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 14:
        create_choices.law = 1;
        create_choices.good = 1;
        break;
    case 7:
    case 13:
    case 15:
    case 16:
    case 17:
        create_choices.law = 2;
        create_choices.good = 2;
        break;
    }
}

void far race_set(void)                                          /* 31d4:27ca */
{
    unsigned char n;
    unsigned char first;
    unsigned char c;
    char cur_bad;
    char hit;

    new_hero->race = create_choices.race - 1;
    n = race_classes[new_hero->race].count;
    first = 0xff;
    hit = 0;
    cur_bad = 0;
    for (c = 0; c <= 16; c++) {
        for (tick_slot = 0; tick_slot < n && !hit; tick_slot++) {
            if (race_classes[new_hero->race].classes[tick_slot] == c) {
                if (first == 0xff)
                    first = c;
                hit = 1;
            }
        }
        if (!hit) {
            if (c == create_choices.cls - 1)
                cur_bad = 1;
            ui_mark(c + 0xb, 0x10);
        } else
            ui_mark(c + 0xb, 0x18);
        hit = 0;
    }
    if (cur_bad)
        ui_mark(0xa, 4, first + 1);
    align_base = 0;
}

void far align_update(void)                                      /* 31d4:28d6 */
{
    align_mark();
    align_fix();
    ui_mark(0x20, 4, create_choices.good);
    align_set();
}

void far good_set(void)                                          /* 31d4:28f7 */
{
    align_set();
}

void far gender_set(void)                                        /* 31d4:28fc */
{
    new_hero->gender = (enum Sex)(create_choices.gender - 1);
}

void far class_set(void)                                         /* 31d4:2909 */
{
    new_hero->cls = create_choices.cls - 1;
    switch (create_choices.cls - 1) {
    case 1:
    case 3:
        ui_mark(0x1d, 0x18);
        ui_mark(0x1e, 0x10);
        ui_mark(0x1f, 0x10);
        break;
    case 0:
    case 2:
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
        ui_mark(0x1d, 0x18);
        ui_mark(0x1e, 0x18);
        ui_mark(0x1f, 0x18);
        break;
    }
    align_total = class_aligns[new_hero->cls + align_base].count;
    moral_matched = 0;
    for (good_scan = 1; good_scan <= 3 && !moral_matched; good_scan++)
        for (tick_slot = 0; tick_slot < align_total && !moral_matched; tick_slot++) {
            new_hero->alignment = (create_choices.law - 1) * 3 + (good_scan - 1);
            if (class_aligns[new_hero->cls + align_base].aligns[tick_slot] == new_hero->alignment)
                moral_matched = 1;
        }
    good_scan--;
    if (moral_matched)
        align_fix();
    else {
        order_matched = 0;
        for (law_try = 1; law_try <= 3 && !order_matched; law_try++)
            for (tick_slot = 0; tick_slot < align_total && !order_matched; tick_slot++) {
                new_hero->alignment = (law_try - 1) * 3 + create_choices.good - 1;
                if (class_aligns[new_hero->cls + align_base].aligns[tick_slot] ==
                    new_hero->alignment)
                    order_matched = 1;
            }
        law_try--;
        if (order_matched)
            create_choices.law = law_try;
        else
            align_default();
    }
    align_mark();
    ui_mark(0x1c, 4, create_choices.law);
    ui_mark(0x20, 4, create_choices.good);
}

void far pick_titles(void)                                       /* 31d4:2b42 */
{
    frame_border();
    text_print_at(0x1f46, 0x1f46, title_hue, "PICK RACE");
    text_print_at(0x1f68, 0x1f46, title_hue, "PICK ALIGNMENT");
    text_print_at(0x1f8c, 0x1f46, title_hue, "PICK GENDER");
    text_print_at(0x1f4c, 0x1f84, title_hue, "PICK CLASS");
}

int far pick_arrows(int id, int key)                             /* 31d4:2ba9 */
{
    if (id < 0) {
        key_y = 0;
        key_col = 0;
        mouse_move_to(0x1f4d, 0x1f48);
ignore:
        return 0;
    }
    if (key == 0x106 || key == 0x102)
        key_col = 1 - key_col;
    else if (key == 0x108 && key_y != 0)
        key_y--;
    else if (key == 0x104)
        key_y++;
    else if (key != 0xd)
        goto ignore;
    if (key_col) {
        if (key_y > 0xf)
            key_y = 0xf;
    } else if (key_y == 7)
        key_y = 0xa;
    else if (key_y == 9)
        key_y = 6;
    else if (key_y == 0xf)
        key_y = 0x12;
    else if (key_y == 0x11)
        key_y = 0xe;
    else if (key_y > 0x13)
        key_y = 0x13;
    mouse_move_to(key_y * 4 + 0x1f4d, (key_col ? 0x1f86 : 0x1f46) + 2);
    if (key == 0xd)
        mouse_fake(1, 1);
    return 1;
}

char far hero_pick(void)                                          /* 31d4:2cae */
{
    icon_cancelled = 0;
    list_attr = 0x87;
    title_hue = 0x8c;
    pick_titles();
    screen_flush();
    create_choices.race = 6;
    create_choices.law = 1;
    create_choices.good = 1;
    create_choices.gender = 1;
    create_choices.cls = 3;
    new_hero->race = 5;
    new_hero->alignment = 0;
    new_hero->gender = 0;
    new_hero->cls = 2;
    frame_invalidate();
    dl_begin();
    ui_add(2, 6, 0x22, race_set, 0x23, &create_choices.race,
           3, 0x1f4c, 0x1f46, "ELF", 0x26, list_attr,
           3, 0x1f50, 0x1f46, "HALF-ELF", 0x26, list_attr,
           3, 0x1f54, 0x1f46, "DWARF", 0x26, list_attr,
           3, 0x1f58, 0x1f46, "GNOME", 0x26, list_attr,
           3, 0x1f5c, 0x1f46, "HALFLING", 0x26, list_attr,
           3, 0x1f60, 0x1f46, "HUMAN", 0x26, list_attr,
           0);
    ui_add(2, 2, 0x22, gender_set, 0x23, &create_choices.gender,
           3, 0x1f92, 0x1f46, "MALE", 0x26, list_attr,
           3, 0x1f96, 0x1f46, "FEMALE", 0x26, list_attr,
           0);
    ui_add(2, 0x11, 0x22, class_set, 0x23, &create_choices.cls,
           3, 0x1f52, 0x1f86, "CLERIC", 0x26, list_attr,
           3, 0x1f56, 0x1f86, "DRUID", 0x26, list_attr, 0x11,
           3, 0x1f56, 0x1f86, "FIGHTER", 0x26, list_attr,
           3, 0x1f5a, 0x1f86, "PALADIN", 0x26, list_attr,
           3, 0x1f5e, 0x1f86, "RANGER", 0x26, list_attr,
           0);
    ui_add(3, 0x1f62, 0x1f86, "MAGIC-USER", 0x26, list_attr,
           3, 0x1f66, 0x1f86, "THIEF", 0x26, list_attr,
           3, 0x1f6a, 0x1f86, "MONK", 0x26, list_attr, 0x11,
           3, 0x1f6a, 0x1f86, "CLERIC/FIGHTER", 0x26, list_attr,
           3, 0x1f6e, 0x1f86, "CLERIC/FIGHTER/M-U", 0x26, list_attr,
           3, 0x1f72, 0x1f86, "CLERIC/RANGER", 0x26, list_attr,
           0);
    ui_add(3, 0x1f76, 0x1f86, "CLERIC/MAGIC-USER", 0x26, list_attr,
           3, 0x1f76, 0x1f86, "CLERIC/THIEF", 0x26, list_attr, 0x11,
           3, 0x1f7a, 0x1f86, "FIGHTER/MAGIC-USER", 0x26, list_attr,
           3, 0x1f7e, 0x1f86, "FIGHTER/THIEF", 0x26, list_attr,
           3, 0x1f82, 0x1f86, "FIGHTER/M-U/THIEF", 0x26, list_attr,
           3, 0x1f86, 0x1f86, "MAGIC-USER/THIEF", 0x26, list_attr,
           0);
    ui_add(2, 3, 0x22, align_update, 0x23, &create_choices.law,
           3, 0x1f6e, 0x1f46, "LAWFUL", 0x26, list_attr,
           3, 0x1f72, 0x1f46, "NEUTRAL", 0x26, list_attr,
           3, 0x1f76, 0x1f46, "CHAOTIC", 0x26, list_attr,
           2, 3, 0x22, good_set, 0x23, &create_choices.good,
           3, 0x1f7c, 0x1f46, "GOOD", 0x26, list_attr,
           3, 0x1f80, 0x1f46, "NEUTRAL", 0x26, list_attr,
           3, 0x1f84, 0x1f46, "EVIL", 0x26, list_attr,
           0);
    ui_add(1, 0x1f9e, 0x1f44, "Done", 0x14, 0x20, 'D', 0x24, 4, 0x22, hero_setup, 0x15,
           1, 0x1f9e, 0x1f58, "Exit", 0x14, 0x20, 'E', 0x21, 0x23, 0x24, 4, 0x22, pick_exit,
           0x15,
           0);
    if (!pointer_mode_set(-1)) {
        ui_add(7, pick_arrows, 0);
        pick_arrows(-1, 0);
        pointer_show();
    }
    dl_draw(1);
    ui_run(0);
    dl_end();
    if (!pointer_mode_set(-1))
        pointer_hide();
    return !icon_cancelled;
}

char far hero_create(struct Hero far *out)                       /* 31d4:31bb */
{
    struct Hero hero;
    struct Hero far *saved;
    char buf[0x28];
    char ok;

    ok = 0;
    new_hero = &hero;
    mem_fill(new_hero, sizeof(struct Hero), 0);
    new_hero->ac_base = 0x32;
    new_hero->to_hit_base = 0x28;
    new_hero->status = 0;
    new_hero->active = 1;
    new_hero->icon_dim = 1;
    new_hero->icon_slot = 8;
    list_attr = 0x87;
    saved = cur_node;
    if (out)
        create_xp = xp_input();
    else
        create_xp = start_xp;
    if (hero_pick() && hero_finish(new_hero) && name_input(new_hero)) {
        icon_default();
        if (out || icon_customize(0)) {
            new_hero->type_flags = 2;
            new_hero->immunity = 2;
            str_copy(buf, msg_format("Save %s?", new_hero->name));
            if (out || confirm(buf, 1)) {
                mu_learn(new_hero);
                if (out)
                    *out = *new_hero;
                else {
                    gear_start(new_hero);
                    character_file_save(new_hero, "");
                }
                ok = 1;
            }
        }
    }
    cur_node = saved;
    return ok;
}

void far mu_learn(struct Hero far *h)                            /* 31d4:334c */
{
    unsigned char i;

    if (h->level[5] >= 1) {
        h->known[1] = h->known[1] | bit_mask[2];
        h->known[2] = h->known[2] | bit_mask[1];
        h->known[2] = h->known[2] | bit_mask[2];
        h->known[2] = h->known[2] | bit_mask[4];
    }
    if (h->level[5] >= 2)
        h->known[1] = h->known[1] | bit_mask[6];
    if (h->level[5] >= 3) {
        h->known[4] = h->known[4] | bit_mask[1];
        h->known[1] = h->known[1] | bit_mask[1];
    }
    if (h->level[5] >= 4)
        h->known[3] = h->known[3] | bit_mask[6];
    if (h->level[5] >= 5)
        h->known[5] = h->known[5] | bit_mask[6];
    if (h->level[5] >= 6) {
        h->known[1] = h->known[1] | bit_mask[3];
        h->known[1] = h->known[1] | bit_mask[6];
        h->known[3] = h->known[3] | bit_mask[5];
        h->known[3] = h->known[3] | bit_mask[6];
        h->known[4] = h->known[4] | bit_mask[1];
        h->known[5] = h->known[5] | bit_mask[6];
        h->known[6] = h->known[6] | bit_mask[2];
        h->known[6] = h->known[6] | bit_mask[5];
    }
    if (h->level[5] >= 7) {
        h->known[1] = h->known[1] | bit_mask[5];
        h->known[1] = h->known[1] | bit_mask[7];
        h->known[3] = h->known[3] | bit_mask[4];
        h->known[4] = h->known[4] | bit_mask[2];
        h->known[5] = h->known[5] | bit_mask[4];
        h->known[5] = h->known[5] | bit_mask[7];
        h->known[0xa] = h->known[0xa] | bit_mask[0];
    }
    if (h->level[5] >= 8) {
        h->known[1] = h->known[1] | bit_mask[1];
        h->known[1] = h->known[1] | bit_mask[0];
        h->known[3] = h->known[3] | bit_mask[7];
        h->known[6] = h->known[6] | bit_mask[6];
        h->known[0xa] = h->known[0xa] | bit_mask[4];
    }
    if (h->level[5] >= 9) {
        h->known[6] = h->known[6] | bit_mask[1];
        h->known[6] = h->known[6] | bit_mask[3];
        h->known[0xa] = h->known[0xa] | bit_mask[1];
        h->known[0xa] = h->known[0xa] | bit_mask[7];
        h->known[0xb] = h->known[0xb] | bit_mask[5];
    }
    if (h->slots[2][3] > 0) {
        h->known[0xa] = h->known[0xa] | bit_mask[1];
        h->known[0xa] = h->known[0xa] | bit_mask[3];
        h->known[0xa] = h->known[0xa] | bit_mask[7];
        h->known[0xa] = h->known[0xa] | bit_mask[0];
        h->known[0xa] = h->known[0xa] | bit_mask[2];
        h->known[0xa] = h->known[0xa] | bit_mask[4];
        h->known[0xa] = h->known[0xa] | bit_mask[6];
    }
    if (h->slots[2][4] > 0) {
        h->known[0xb] = h->known[0xb] | bit_mask[2];
        h->known[0xb] = h->known[0xb] | bit_mask[3];
        h->known[0xe] = h->known[0xe] | bit_mask[5];
        h->known[0xe] = h->known[0xe] | bit_mask[6];
    }
    if (h->slots[2][5] > 0) {
        h->known[0xe] = h->known[0xe] | bit_mask[0];
        h->known[0xe] = h->known[0xe] | bit_mask[1];
        h->known[0xd] = h->known[0xd] | bit_mask[7];
        h->known[0xd] = h->known[0xd] | bit_mask[6];
    }
    if (h->slots[2][6] > 0) {
        h->known[0xe] = h->known[0xe] | bit_mask[2];
        h->known[0xe] = h->known[0xe] | bit_mask[4];
    }
    if (h->slots[2][7] > 0)
        h->known[0xf] = h->known[0xf] | bit_mask[0];
    if (h->slots[2][8] > 0)
        for (i = 1; i <= 0x7e; i++)
            if (spell_info[i].kind == 2)
                h->known[(i - 1) / 8] = h->known[(i - 1) / 8] | bit_mask[(i - 1) % 8];
}

static void gear_add(struct Hero far *h, unsigned char id)       /* 31d4:3740 */
{
    struct Hero far *saved;
    struct Item far *item;
    struct Item far *prev;
    unsigned char slot;

    item = rec_alloc();
    mem_zero(item, 4);
    if (id != 0 && item_templates[id * 0x12] != -1) {
        h->item_count++;
        mem_move5(item_templates + id * 0x12, &item->type, 0x12);
        item->next = 0;
        item->in_use = 0;
        if (h->items == 0)
            h->items = item;
        else {
            prev = h->items;
            h->items = item;
            h->items->next = prev;
        }
        h->items->unid_mask = h->items->unid_mask & 0xf8;
        if (id == 0x50 || id == 0x56 || id == 0x5b)
            item->count = 0x28;
        h->items->readied = 0;
        if (id != 0x52 && id != 0x53 && id != 0x54 && id != 0x56 && id != 0x5d) {
            if (!ready_check(h, item, &slot)) {
                item->readied = 1;
                h->equipped[slot] = item;
                if (item->special & 0x80) {
                    saved = cur_node;
                    cur_node = h;
                    item_effect(item, 1);
                    cur_node = saved;
                }
            }
        }
    }
}

static void gear_start(struct Hero far *h)                       /* 31d4:3907 */
{
    h->item_count = 0;
    if (h->level[2] > 0 || h->level[4] > 0 || h->level[3] > 0) {
        switch (start_gear) {
        case 1:
            gear_add(h, 0x17);
            gear_add(h, 0x29);
            break;
        case 2:
            gear_add(h, 0x1b);
            gear_add(h, 0x2a);
            gear_add(h, 0x2b);
            break;
        case 3:
            gear_add(h, 0x1c);
            gear_add(h, 0x24);
            gear_add(h, 0x2b);
            gear_add(h, 6);
            gear_add(h, 1);
            break;
        case 4:
            gear_add(h, 0x1e);
            gear_add(h, 0x28);
            gear_add(h, 5);
            gear_add(h, 1);
            break;
        case 5:
            gear_add(h, 0x42);
            gear_add(h, 0x4b);
            gear_add(h, 0x35);
            gear_add(h, 0x32);
            gear_add(h, 0x4e);
            gear_add(h, 0x49);
            break;
        case 6:
            gear_add(h, 0x60);
            gear_add(h, 0x69);
            gear_add(h, 0x6c);
            gear_add(h, 0x50);
            gear_add(h, 0x53);
            gear_add(h, 0x67);
            break;
        case 7:
            gear_add(h, 0x7e);
            gear_add(h, 0x87);
            gear_add(h, 0x6e);
            gear_add(h, 0x71);
            gear_add(h, 0x8a);
            gear_add(h, 0x85);
            break;
        case 8:
            gear_add(h, 0x9c);
            gear_add(h, 0xa5);
            gear_add(h, 0xa8);
            gear_add(h, 0xa3);
            gear_add(h, 0x8c);
            gear_add(h, 0x8f);
            break;
        }
    } else if (h->level[0] > 0) {
        switch (start_gear) {
        case 1:
            gear_add(h, 8);
            gear_add(h, 0x26);
            break;
        case 2:
            gear_add(h, 0x10);
            gear_add(h, 0x29);
            gear_add(h, 0x2b);
            break;
        case 3:
            gear_add(h, 0x10);
            gear_add(h, 0x25);
            gear_add(h, 0x2b);
            gear_add(h, 0xe);
            break;
        case 4:
            gear_add(h, 0xc);
            gear_add(h, 0x19);
            gear_add(h, 0x28);
            gear_add(h, 0x2b);
            break;
        case 5:
            gear_add(h, 0x3c);
            gear_add(h, 0x47);
            gear_add(h, 0x4e);
            gear_add(h, 0x41);
            break;
        case 6:
            gear_add(h, 0x5a);
            gear_add(h, 0x65);
            gear_add(h, 0x58);
            gear_add(h, 0x6c);
            gear_add(h, 0x67);
            break;
        case 7:
            gear_add(h, 0x78);
            gear_add(h, 0x83);
            gear_add(h, 0x7d);
            gear_add(h, 0x8a);
            gear_add(h, 0x85);
            break;
        case 8:
            gear_add(h, 0x93);
            gear_add(h, 0xa1);
            gear_add(h, 0xa8);
            gear_add(h, 0x9b);
            gear_add(h, 0xa3);
            break;
        }
    } else if (h->level[6] > 0) {
        switch (start_gear) {
        case 1:
            gear_add(h, 8);
            break;
        case 2:
            gear_add(h, 0x1d);
            gear_add(h, 0x27);
            break;
        case 3:
            gear_add(h, 0x1b);
            gear_add(h, 0x27);
            gear_add(h, 0x16);
            break;
        case 4:
            gear_add(h, 0x1c);
            gear_add(h, 4);
            gear_add(h, 1);
            gear_add(h, 0x2c);
            break;
        case 5:
            gear_add(h, 0x42);
            gear_add(h, 0x4f);
            gear_add(h, 0x34);
            gear_add(h, 0x32);
            break;
        case 6:
            gear_add(h, 0x60);
            gear_add(h, 0x5d);
            gear_add(h, 0x6d);
            break;
        case 7:
            gear_add(h, 0x7e);
            gear_add(h, 0x7b);
            gear_add(h, 0x8b);
            break;
        case 8:
            gear_add(h, 0x9c);
            gear_add(h, 0x99);
            gear_add(h, 0xd4);
            gear_add(h, 0xa0);
            break;
        }
    } else {
        switch (start_gear) {
        case 1:
            gear_add(h, 0xb);
            break;
        case 2:
            gear_add(h, 0xb);
            gear_add(h, 0xa);
            break;
        case 3:
            gear_add(h, 0xb);
            gear_add(h, 0x18);
            gear_add(h, 0x23);
            break;
        case 4:
            gear_add(h, 0xb);
            gear_add(h, 0x18);
            gear_add(h, 0x22);
            gear_add(h, 0x23);
            break;
        case 5:
            gear_add(h, 0x37);
            gear_add(h, 0x38);
            gear_add(h, 0xd2);
            gear_add(h, 0x46);
            break;
        case 6:
            gear_add(h, 0x5e);
            gear_add(h, 0x56);
            gear_add(h, 0xd3);
            gear_add(h, 0x64);
            break;
        case 7:
            gear_add(h, 0x73);
            gear_add(h, 0x74);
            gear_add(h, 0xd4);
            gear_add(h, 0x82);
            break;
        case 8:
            gear_add(h, 0x9a);
            gear_add(h, 0x2f);
            gear_add(h, 0x2f);
            gear_add(h, 0xd4);
            gear_add(h, 0xa0);
            break;
        }
    }
    recalc_stats(h);
}
