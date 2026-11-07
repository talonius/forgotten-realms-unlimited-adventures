/* CKIT.EXE overlay 11 module 26e3: the effect handlers.
 * Every effect a party member or monster carries has an id; fx_install
 * and fx_install_2 fill fx_handler with one far handler per id, and
 * fx_dispatch calls the handler for an id with the member, the effect
 * record and whether it is being removed -- or, while item_handler_active is set,
 * the item handler in slot 0x7f.  The handlers adjust the current roll
 * (save_bonus, attack_roll), damage and element mask, add and remove
 * effects, drain levels, and play the monsters' special attacks: gazes,
 * breath, acid, engulfing.  fx_paralyze, fx_cancel, fx_rearm,
 * fx_cancel_if, fx_improve_rolls, fx_avoid, fx_weapon, fx_need_plus,
 * fx_forget_spells and fx_drain are helpers.
 *
 * EXE 0x04faf0..0x053364, one Turbo C large-model _TEXT segment starting
 * at byte 0 of paragraph 26e3, the first module of RTLink section 11
 * (payload 0x4faf0, link segment 26e3, which sections 12 and 13 also
 * use).  Its last retf is byte 3 of paragraph 2a6a; the lone retf after
 * it, 2a6a:0004, is left to module 2a6a.  Built with Turbo C 2.01, tcc
 * -ml -O -Z -d.  Functions are defined in shipped order; bake consumes
 * them in that order.
 *
 * One file: the installers store a handler already defined here as
 * mov [m+2],cs, the form Turbo C gives only a function defined earlier in
 * the same file, and those stores and the near calls tie 26e3:0000 to
 * 26e3:3831.  fx_install near-calls fx_install_2 ahead of its
 * definition, so fx_install_2 is static.  Vector stub 18de:1945 reaches
 * fx_install and 18de:1c8d fx_dispatch; module 2a6a far-calls
 * fx_end_actions, fx_drain and fx_dispatch.  29 stores put module 2a6a's
 * handlers in the table and one puts vector stub 18de:12a1.
 *
 * A bare stored seg:off in this file means section 11.  The seg:off
 * beside each extern is the far pointer as stored in the shipped call,
 * or the DGROUP offset of an extern variable; a definition's comment
 * gives the load-based address, as the resident modules' do.  Callee and
 * variable names are placeholders chosen from use; a modNNNN_ name says
 * only which segment the callee lives in, effects_NNNN only where in this
 * module the routine starts, and ovl11_2a6a_NNNN names a routine of
 * section 11's module 2a6a.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  A handler keeps the
 * name Dark Queen of Krynn's effects.c gives the handler in the same
 * slot, except where that name encodes a DQK address.
 *
 * _DATA is DGROUP 7ff8..81e1, 34 stored literals; -d stores "is
 * paralyzed", "gazes...", "reflects it!" and "%s%s" once each, and "" on
 * the NUL of "is paralyzed" (8004).  _BSS is fx_handler at c674..ca07.
 * The dispatcher indexes from c670, so the table is used as
 * (fx_handler - 1)[id], and fx_install's first loop also writes slot 0,
 * c670..c673, bytes this module does not own: section 6 stores c670
 * (0x03946d, 0x039604), sections 4 and 8 use c671, and hero.c and
 * sections 8 and 10 use c672.
 *
 * heroes and steps are indexed from 1; the shipped code adds the address
 * of element 0 (b8f1, ca28).  targets[1] is DGROUP cfe7.
 */

struct Effect {                 /* 10 bytes, from effect_pool               */
    unsigned char kind;         /* +0                                     */
    char pad_1;                 /* +1   not read here                     */
    unsigned duration;          /* +2                                     */
    unsigned char strength;     /* +4                                     */
    char on_remove;             /* +5                                     */
    struct Effect far *next;    /* +6   not read here                     */
};

struct Item {                   /* 0x3e bytes, a item_record_pool record         */
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28                                    */
    char pad_29;                /* +29  not read here                     */
    char name2;                 /* +2a                                    */
    unsigned char name3;        /* +2b                                    */
    char pad_2c[4];             /* +2c  not read here                     */
    char plus;                  /* +30                                    */
    char pad_31;                /* +31  not read here                     */
    char readied;               /* +32                                    */
    char pad_33;                /* +33  not read here                     */
    char cursed;                /* +34                                    */
    char pad_35[2];             /* +35  not read here                     */
    unsigned char effect;       /* +37                                    */
    unsigned char special;      /* +38                                    */
    char pad_39[5];             /* +39  not read here                     */
};

struct Pool;

struct Fight {                  /* a combatant's state in a fight         */
    char pad_0;                 /* +0   not read here                     */
    char can_cast;              /* +1                                     */
    char pad_2[5];              /* +2   not read here                     */
    unsigned char moves;        /* +7                                     */
    char pad_8[3];              /* +8   not read here                     */
    struct Hero far *target;    /* +b                                     */
};

struct Hero {                   /* a party member or a monster            */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    struct Item far *weapon;    /* +c                                     */
    char pad_10[0x30];          /* +10  not read here                     */
    struct Fight far *fight;    /* +40                                    */
    long xp;                    /* +44                                    */
    long xp_old;                /* +48                                    */
    char pad_4c[6];             /* +4c  not read here                     */
    unsigned age;               /* +52  years                             */
    char pad_54[4];             /* +54  not read here                     */
    unsigned char race;         /* +58                                    */
    char pad_59[4];             /* +59  not read here                     */
    unsigned char alignment;    /* +5d  0..8; 2, 5, 8 evil, 0, 3, 6 good  */
    unsigned char status;       /* +5e                                    */
    unsigned char side;         /* +5f                                    */
    char name[0x11];            /* +60                                    */
    unsigned char str;          /* +71                                    */
    char pad_72[7];             /* +72  not read here                     */
    unsigned char con;          /* +79                                    */
    char pad_7a;                /* +7a  not read here                     */
    unsigned char cha;          /* +7b                                    */
    char pad_7c[4];             /* +7c  not read here                     */
    unsigned char cures;        /* +80                                    */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[7];             /* +82  not read here                     */
    unsigned char hit_dice;     /* +89                                    */
    unsigned char dual_level;   /* +8a                                    */
    char pad_8b[0xb];           /* +8b  not read here                     */
    unsigned char level_max[7]; /* +96  one per class                     */
    unsigned char level[7];     /* +9d  one per class                     */
    char pad_a4[0x12];          /* +a4  not read here                     */
    unsigned char hp_max_old;   /* +b6                                    */
    char pad_b7[8];             /* +b7  not read here                     */
    unsigned char type_flags;   /* +bf                                    */
    char pad_c0;                /* +c0  not read here                     */
    unsigned char item_count;   /* +c1                                    */
    char pad_c2[4];             /* +c2  not read here                     */
    unsigned char spells[0x8d]; /* +c6  bit 7: still to memorize          */
    char pad_153[0xf];          /* +153 not read here                     */
    unsigned char slots_by_level[3][9]; /* +162 by kind and spell level           */
    char pad_17d;               /* +17d not read here                     */
    char active;                /* +17e                                   */
    char pad_17f[2];            /* +17f not read here                     */
    unsigned char ac;           /* +181                                   */
    unsigned char ac_rear;      /* +182                                   */
    unsigned char attacks_main; /* +183                                   */
    unsigned char attacks_off;  /* +184                                   */
    unsigned char dice_n_main;  /* +185                                   */
    char pad_186;               /* +186 not read here                     */
    unsigned char dice_s_main;  /* +187                                   */
    char pad_188[3];            /* +188 not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct Step {
    unsigned char who;          /* +0   index into heroes                 */
    unsigned char cost;         /* +1   not read here                     */
    unsigned char dir;          /* +2   not read here                     */
};

struct ItemType {               /* 0x10 bytes an item type                */
    unsigned char slot;         /* +0                                     */
    char pad_1[6];              /* +1   not read here                     */
    unsigned char weapon_class; /* +7                                     */
    char pad_8[8];              /* +8   not read here                     */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    int kind;                   /* +0                                     */
    unsigned char level;        /* +2                                     */
    char pad_3[7];              /* +3   not read here                     */
    int save_effect;            /* +a                                     */
    char pad_c[7];              /* +c   not read here                     */
    int hostile;                /* +13                                    */
    char pad_15;                /* +15  not read here                     */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far hero_panel(struct Hero far *h);                   /* 03d0:0834 */
extern unsigned char far calc_move(struct Hero far *h);          /* 03d0:0a38 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern char far str_damage(struct Hero far *h);                   /* 03d0:1234 */
extern void far item_remove(struct Hero far *h, struct Item far *it); /* 03d0:13c4 */
extern void far item_append(struct Hero far *h, struct Item far *src); /* 03d0:149c */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far heal_msg(struct Hero far *h);                   /* 03d0:1878 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern long far level_xp_threshold(struct Hero far *h, unsigned char level, short cls);
                                                                 /* 03d0:21cc */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far party_rows(struct Hero far *hero, char c);
                                /* 18de:1071, vector to overlay 4 1c86:0064 */
extern void far fx_disable(struct Hero far *h, int how, char far *msg);
                                /* 18de:10e9, vector to overlay 19 2e88:000f */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern char far combat_actions_end(struct Hero far *h); /* 18de:1139, vector to overlay 0 1cb8:0b84 */
extern char far combatant_x(struct Hero far *h); /* 18de:117f, vector to overlay 0 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h); /* 18de:1189, vector to overlay 0 22fa:0ae4 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame, char erase);
                                /* 18de:11cf, vector to overlay 0 22fa:09a2 */
extern unsigned char far combatant_slot(struct Hero far *h);
                                /* 18de:11f7, vector to overlay 0 22fa:0b44 */
extern void far combat_attack(struct Hero far *h, struct Hero far *t, char c,
                             struct Item far *missile, char far *done);
                                /* 18de:1201, vector to overlay 0 1dbf:178d */
extern char far ranged_ammo_find(struct Hero far *h, struct Item far * far *w);
                                /* 18de:1215, vector to overlay 0 1cb8:0fb5 */
extern void far icon_fly(int x0, int y0, int x1, int y1, unsigned char frames, unsigned char pace);
                                /* 18de:121f, vector to overlay 0 1cb8:00de */
extern unsigned char far get_distance(struct Hero far *h, struct Hero far *t);
                                /* 18de:1233, vector to overlay 0 1cb8:0a99 */
extern void far hit_flash(struct Hero far *h, char a, char far *msg);
                                /* 18de:123d, vector to overlay 0 1cb8:0730 */
extern int  far enemy_team(struct Hero far *h);
                                /* 18de:1247, vector to overlay 0 1cb8:08fc */
extern void far icon_facings(unsigned char cell);
                                /* 18de:125b, vector to overlay 0 1cb8:0052 */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern unsigned char far combatant_size(struct Hero far *h);
                                /* 18de:128d, vector to overlay 0 22fa:0b14 */
extern void far eye_rays(struct Hero far *h, struct Effect far *fx, char removed);
                                /* 18de:12a1, vector to overlay 0 1dbf:4dbe */
extern int  far fx_roll_damage(unsigned char n, unsigned char sides);
                                /* 18de:12ab, vector to overlay 19 2e88:1624 */
extern unsigned char far combatant_direction(struct Hero far *h, struct Hero far *t);
                                /* 18de:12e7, vector to overlay 0 22fa:166c */
extern char far fx_saving_throw(struct Hero far *h, int type, char adj);
                                /* 18de:1323, vector to overlay 19 2e88:1552 */
extern void far fx_damage(struct Hero far *h, unsigned char dmg, int reduce,
                             char saved);
                                /* 18de:132d, vector to overlay 19 2e88:1ea2 */
extern void far item_equip_toggle(struct Item far *item);
                                /* 18de:135f, vector to overlay 14 28aa:19bf */
extern void far class_update(struct Hero far *hero);
                                /* 18de:1729, vector to overlay 14 2ccd:05c5 */
extern void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove);
                                /* 18de:19bd, vector to overlay 19 2e88:1648 */
extern void far hp_adjust(struct Hero far *hero, char classes, char count, char c);
                                /* 18de:19e5, vector to overlay 14 28aa:3cf1 */
extern char far fx_heal(struct Hero far *hero, char a, char b);
                                /* 18de:1af3, vector to overlay 19 2e88:224c */
extern void far fx_add_message(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove, int save_negates,
                             char saved, char far *msg);
                                /* 18de:1c97, vector to overlay 19 2e88:2181 */
extern void far spell_path_targets(char x, char y, char tx, char ty, unsigned char width,
                             unsigned char len);
                                /* 18de:1ca1, vector to overlay 12 2b69:1a61 */
extern char far fx_revive(struct Hero far *h, unsigned char hp, char far *msg);
                                /* 18de:1cab, vector to overlay 19 2e88:230c */
extern void far fx_snakes(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0005 */
extern void far fx_attack_bonus_vs_immunity4(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0071 */
extern void far fx_charm(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:00a5 */
extern void far fx_silence(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0157 */
extern void far fx_attack_bonus_vs_type80(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:01f2 */
extern void far fx_nausea(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0226 */
extern void far fx_dispel_evil_attack(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:02b7 */
extern void far fx_confusion(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:036a */
extern void far fx_blink(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:05b7 */
extern void far fx_poison_attack(struct Hero far *h, char c);     /* 2a6a:05e4 */
extern void far fx_petrify_attack(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0698 */
extern void far fx_feeblemind(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:06ef */
extern void far fx_damage_vs_immunity1(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:07c1 */
extern void far fx_berserk(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0818 */
extern void far fx_drain_one_level(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0935 */
extern void far fx_drain_two_levels(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:095a */
extern void far fx_fire_damage_4d6(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:097f */
extern void far fx_berserk_creature(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:09e2 */
extern void far fx_fire_damage_2d6(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0ad5 */
extern void far fx_fear_remove(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0b38 */
extern void far fx_vorpal(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0b70 */
extern void far fx_fire_damage_2d6_alt(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0bbc */
extern void far fx_movement_stop(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0c1f */
extern void far fx_poison_steam_breath(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0c46 */
extern void far fx_lightning_breath(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0e0e */
extern void far fx_spell_gaze(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:0f39 */
extern void far fx_breath_weapon(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 2a6a:1004 */

extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern char far *game_text[];                                    /* 38e8 */
extern char far *txt_gains;                                      /* 39d4 */
extern char far *item_words[];                                   /* 3d03 */
extern unsigned char class_bit[7];                               /* 6252 */
extern char far *fx_names[];                                     /* 6379 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern struct ItemType far *item_table;                          /* b40a */
extern char turn_done;                                           /* b414 */
extern unsigned char combat_round;                                  /* b418 */
extern int view_mode;                                            /* b421 */
extern unsigned char move_rate;                                  /* b427 */
extern int damage;                                               /* b42f */
extern char refused;                                             /* b432 */
extern char effect_id;                                           /* b720 */
extern char save_succeeded;                                           /* b721 */
extern unsigned char attack_rate;                                /* b723 */
extern char fx_lock;                                             /* b8d0 */
extern unsigned char side_count[];                               /* b8d1 */
extern unsigned char save_bonus;                                 /* b8d3 */
extern unsigned char attacks_used;                                  /* b8e8 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern unsigned char damage_dice_count;                                 /* be60 */
extern unsigned char movement_stops_attack_rate;                                  /* be61 */
extern unsigned char current_spell;                                  /* be64 */
extern char hits_left;                                           /* c34c */
extern struct Hero far *fx_target;                               /* c350 */
extern char year_rolled;                                         /* c354 */
extern char view_cx;                                             /* c39f */
extern char view_cy;                                             /* c3bc */
extern char item_handler_active;                                           /* c523 */
extern int element_mask;                                         /* c64e */
extern char mirror_image_bypass;                                           /* ca08 */
extern unsigned char bonus_slots;                                /* ca09 */
extern unsigned char step_count;                                 /* ca10 */
extern unsigned char morale;                                     /* ca11 */
extern char attack_roll;                                           /* ca1d */
extern int current_save_type;                                            /* ca1f */
extern struct Step steps[];                                      /* ca2b */
extern struct Pool item_record_pool;                                    /* cb0e */
extern struct Hero far *targets[];                               /* cfe3 */
extern struct Hero far *node_list;                               /* d129 */
extern void (far *hit_proc)(unsigned char spell, char mode, char far *result); /* d131 */
extern unsigned char target_count;                               /* d137 */

void far fx_dispatch(unsigned char id, struct Hero far *h, struct Effect far *fx, char removed);
static void far fx_install_2(void);

static void (far *fx_handler[229])(struct Hero far *h, struct Effect far *fx, char removed);
                                /* by effect id from 1: (fx_handler - 1)[id] */

void far fx_paralyze(struct Hero far *h, int unused, char save) /* 36e3:0000 */
{
    char saved;
    long node;

    fx_target = h->fight->target;
    saved = fx_saving_throw(fx_target, 0, save);
    if (find_effect(h, 0x58, &node))
        fx_add_message(fx_target, 0x34, 10, 0xc, 0, 1, saved, "is paralyzed");
    else
        fx_add_message(fx_target, 0x34, 10, 0xc, 0, 1, saved, "is paralyzed by poison");
}

void far fx_cancel(char id)                                      /* 36e3:00a1 */
{
    if (id == 0 || effect_id == id) {
        damage = 0;
        effect_id = 0;
    }
}

char far fx_rearm(struct Hero far *h, char id, char strength, int minutes) /* 36e3:00bf */
{
    char ok;

    if (fx_lock)
        ok = 0;
    else {
        ok = 1;
        fx_add(h, id, minutes, strength, 1);
    }
    return ok;
}

void far fx_cancel_if(int mask)                                  /* 36e3:00f7 */
{
    if ((char)element_mask & (char)mask)
        fx_cancel(0);
}

void far fx_end_actions(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:010d */
{
    combat_actions_end(h);
}

void far fx_improve_rolls(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:011f */
{
    if (!removed) {
        save_bonus++;
        attack_roll++;
    }
}

void far fx_bless(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0132 */
{
    if (!removed) {
        morale += 5;
        attack_roll++;
    }
}

void far fx_curse(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0146 */
{
    if (!removed) {
        if (morale < 5)
            morale = 0;
        else
            morale -= 5;
        attack_roll--;
    }
}

void far fx_dispel_evil(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0168 */
{
    if (!removed && (cur_node->type_flags & 1))
        attack_roll -= 7;
}

void far fx_elec_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0184 */
{
    if (!removed)
        fx_cancel_if(4);
}

void far fx_prot_evil(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0198 */
{
    if (!removed) {
        if (cur_node->alignment == 2 || cur_node->alignment == 5 || cur_node->alignment == 8) {
            save_bonus += 2;
            attack_roll -= 2;
        }
    }
}

void far fx_prot_good(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:01c6 */
{
    if (!removed) {
        if (cur_node->alignment == 0 || cur_node->alignment == 3 || cur_node->alignment == 6) {
            save_bonus += 2;
            attack_roll -= 2;
        }
    }
}

void far fx_resist_cold(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:01f4 */
{
    if (!removed && (element_mask & 2)) {
        damage = damage / 2;
        save_bonus += 3;
    }
}

void far fx_enlarged(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0218 */
{
}

void far fx_barkskin(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:021d */
{
    if (!removed) {
        if (!(element_mask & 8))
            save_bonus++;
        attack_roll--;
    }
}

void far fx_friendly(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0238 */
{
}

void far fx_slow_poison(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:023d */
{
    if (fx_rearm(h, 0x1a, fx->strength, 10) && h->hp > 1) {
        element_mask = 0;
        fx_damage(h, 1, 0, 0);
        if (view_mode != 5)
            party_rows(h, h == cur_node);
    }
}

void far fx_read_magic(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:02b4 */
{
}

void far fx_shielded(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:02b9 */
{
    if (!removed) {
        if (h->ac < 0x39)
            h->ac = 0x39;
        save_bonus++;
        if (current_spell == 0xf)
            damage = 0;
    }
}

void far fx_half_elf(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:02e6 */
{
    if (!removed) {
        if (fx_roll_dice(1, 100) <= 30) {
            fx_cancel(0xb);
            fx_cancel(0x35);
        }
    }
}

void far fx_resist_fire(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0313 */
{
    if (!removed && (element_mask & 1)) {
        damage = damage / 2;
        save_bonus += 3;
    }
}

void far fx_slow_poison_end(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0337 */
{
    long node;

    if (!removed) {
        if (find_effect(h, 0x37, &node))
            fx_disable(h, 6, "dies from poison");
        fx_lock = 1;
        fx_remove(h, 0xf, 0L);
        fx_lock = 0;
    }
}

void far fx_spirit_hammer(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0397 */
{
    struct Item far *node;
    char found;
    struct Hero far *saved;

    node = h->items;
    found = 0;
    while (node != 0 && !found)
        if (node->type == 6 && node->name3 == 0x79)
            found = 1;
        else
            node = node->next;
    if (removed && node != 0)
        item_remove(h, node);
    if (!removed && !found && h->item_count < 0x10) {
        pool_alloc(&node, 0x3e, &item_record_pool);
        mem_zero(node, 0x3e);
        node->type = 6;
        node->name2 = 6;
        node->name3 = 0x79;
        node->plus = 1;
        node->effect = 0x17;
        node->special = 0x89;
        item_append(h, node);
        pool_free(node, 0x3e, &item_record_pool);
        node = h->items;
        found = 0;
        while (node != 0 && !found)
            if (node->name3 == 0x79 && node->type == 6)
                found = 1;
            else
                node = node->next;
        saved = cur_node;
        cur_node = h;
        if (node != 0 && h->weapon == 0) {
            item_equip_toggle(node);
            node->readied = 1;
        }
        cur_node = saved;
        hero_text(h, "Gains an item", 10, 1);
    }
    recalc_stats(h);
}

void far fx_invisible(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:054a */
{
    long node;

    if (!removed) {
        if (!find_effect(cur_node, 0x18, &node)) {
            refused = 1;
            attack_roll -= 4;
        }
    }
}

void far fx_mirror_image(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0580 */
{
    unsigned char n;

    if (!removed && view_mode == 5 && fx_roll_dice(1, fx->strength / 0x20 + 1) > 1
        && (!mirror_image_bypass || (current_spell > 0 && spell_info[current_spell].hostile == 0))) {
        fx_cancel(0);
        hero_text(h, "lost an image", 10, 1);
        n = fx->strength >> 5;
        n--;
        if (n == 0)
            fx_remove(h, 0x1c, 0L);
        else {
            n = n << 5;
            fx->strength = fx->strength & 0x1f | n;
        }
    }
}

void far fx_weakened(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0654 */
{
    if (!removed)
        damage = damage - damage / 4;
}

void far fx_blind(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0672 */
{
    if (!removed) {
        attack_roll -= 4;
        h->ac -= 4;
        h->ac_rear -= 4;
        save_bonus -= 4;
    }
}

void far fx_diseased(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0696 */
{
    if (!removed) {
        fx_dispatch(0x2b, h, fx, removed);
        fx_dispatch(0x2c, h, fx, removed);
    }
}

void far fx_accursed(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:06d3 */
{
    if (!removed) {
        attack_roll -= 4;
        save_bonus -= 4;
    }
}

void far fx_hastened(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:06e8 */
{
    if (!removed) {
        if (!(fx->strength & 0x20)) {
            fx->strength = fx->strength + 0x20;
            hero_text(h, "ages", 10, 1);
            h->age++;
            year_rolled = 1;
        }
        attack_rate *= 2;
        move_rate *= 2;
    }
}

void far fx_block_lower_spell_levels(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:073b */
{
    if (!removed && current_spell != 0 && spell_info[current_spell].level < 8)
        fx_cancel(0);
}

void far fx_avoid(struct Hero far *h, unsigned char chance)      /* 36e3:076e */
{
    if (cur_node->weapon != 0 && get_distance(cur_node, h) > 1
        && fx_roll_dice(1, 100) <= chance) {
        hero_text(h, "Avoids it", 10, 1);
        damage = 0;
        attack_roll = -1;
        hits_left--;
    }
}

struct Item far *far fx_weapon(struct Hero far *h)               /* 36e3:07d4 */
{
    struct Item far *ret;
    struct Item far *w;
    char ok;

    w = 0;
    if (h->weapon != 0) {
        ok = ranged_ammo_find(h, &w);
        if (!ok || w == 0)
            w = h->weapon;
    }
    ret = w;
    return ret;
}

void far fx_missile_prot(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0832 */
{
    if (!removed && fx_weapon(cur_node) != 0 && fx_weapon(cur_node)->plus == 0)
        fx_avoid(h, 100);
}

void far fx_slowed(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0877 */
{
    if (!removed) {
        attack_rate /= 2;
        move_rate /= 2;
    }
}

void far fx_periodic_strength_drain(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:089b */
{
    if (fx_rearm(h, 0x2b, fx->strength, 0x3c)) {
        if (h->str > 3) {
            hero_text(h, "is weakened", 10, 1);
            h->str--;
        } else
            fx_add(h, 0x1f, 0, 0xff, 0);
    }
}

void far fx_periodic_hp_drain(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0901 */
{
    if (fx_rearm(h, 0x2c, fx->strength, 10)) {
        if (h->hp > 1) {
            element_mask = 0;
            fx_damage(h, 1, 0, 0);
            if (view_mode != 5)
                party_rows(h, h == cur_node);
        } else
            fx_add(h, 0x1f, 0, 0xff, 0);
    }
}

void far fx_attack_penalty_type_4(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0993 */
{
    if (!removed && (cur_node->type_flags & 4))
        attack_roll -= 4;
}

void far fx_attack_penalty_type_20(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:09af */
{
    if (!removed && (cur_node->type_flags & 0x20))
        attack_roll -= 4;
}

void far fx_prayer(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:09cb */
{
    short side;

    if (!removed) {
        side = (fx->strength & 0x80) / 0x80;
        if (h->side == side)
            fx_improve_rolls(h, fx, removed);
        else {
            attack_roll--;
            save_bonus--;
        }
    }
}

void far fx_fire_shield_hot(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0a1c */
{
    if (!removed) {
        if (element_mask & 2) {
            save_bonus += 2;
            if (save_succeeded)
                damage = 0;
            else
                damage = damage / 2;
        } else if (((element_mask & 1) != 0) & !save_succeeded)
            damage *= 2;
    }
}

void far fx_fire_shield_cold(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0a78 */
{
    if (!removed) {
        if (element_mask & 1) {
            save_bonus += 2;
            if (save_succeeded)
                damage = 0;
            else
                damage = damage / 2;
        } else if ((element_mask & 2) && !save_succeeded)
            damage *= 2;
    }
}

void far fx_poisoned(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0ac4 */
{
}

void far fx_invis_ring(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0ac9 */
{
    if (removed) {
        if (view_mode == 5)
            fx_remove(h, 0x19, 0L);
    } else
        fx_add(h, 0x19, 1, 0xc, 0);
}

void far fx_major_globe(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0b0b */
{
    if (!removed && current_spell > 0 && (int)spell_info[current_spell].level <= 4)
        fx_cancel(0);
}

void far fx_stone_gaze(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0b42 */
{
    long node;

    if (!removed) {
        (*hit_proc)(0x82, 1, &turn_done);
        if (enemy_team(h) != targets[1]->side)
            turn_done = 0;
        if (turn_done) {
            fx_target = targets[1];
            hero_text(h, "gazes...", 10, 0);
            icon_facings(0x14);
            icon_fly(combatant_x(h), combatant_y(h),
                         combatant_x(fx_target), combatant_y(fx_target), 4, 0x2d);
            if (find_effect(h, 0x3b, &node) && find_effect(fx_target, 0x48, &node)) {
                hero_text(fx_target, "reflects it!", 0xc, 0);
                icon_fly(combatant_x(fx_target), combatant_y(fx_target),
                             combatant_x(h), combatant_y(h), 4, 0x2d);
                fx_target = h;
            }
            if (!fx_saving_throw(fx_target, 1, 0) && !find_effect(fx_target, 0x7e, &node)
                && !find_effect(fx_target, 0x4f, &node))
                fx_disable(fx_target, 7, "is turned to stone");
        }
    }
}

void far fx_noop_slot_59(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0d26 */
{
}

void far fx_need_plus(unsigned char n)                           /* 36e3:0d2b */
{
    unsigned char level;
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0) {
        if (w->plus < n)
            damage = 0;
    } else if (cur_node->race == 6) {
        level = (n - 1) * 2 + 5;
        if (cur_node->hit_dice < level)
            damage = 0;
    } else
        damage = 0;
}

void far fx_need_plus_3(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0d82 */
{
    fx_need_plus(3);
}

void far fx_fire_ring(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:0d8f */
{
    unsigned char i;

    if (!removed && ((char)element_mask & 1) && damage > 0) {
        for (i = 1; i <= damage_dice_count; i++) {
            damage -= 2;
            if (damage_dice_count > damage)
                damage = damage_dice_count;
        }
        save_bonus += 4;
        if (!(element_mask & 8))
            fx_cancel(0);
    }
}

void far fx_periodic_regeneration(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0dec */
{
    if (fx_rearm(h, 0x3e, fx->strength, 0x3c) && fx_heal(h, 1, 1))
        heal_msg(h);
}

void far fx_minor_globe(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0e34 */
{
    if (!removed && current_spell > 0 && (int)spell_info[current_spell].level <= 3)
        fx_cancel(0);
}

void far fx_poison_attack_normal(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0e6b */
{
    if (!removed)
        fx_poison_attack(h, 0);
}

void far fx_halve_low_class_weapon_dmg(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0e86 */
{
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0 && (char)(item_table[w->type].weapon_class <= 1))
        damage = damage / 2;
}

void far fx_halve_high_bit_weapon_damage(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0ed9 */
{
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0 && (item_table[w->type].weapon_class & 0x80))
        damage = damage / 2;
}

void far fx_animal_invisibility(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0f21 */
{
    long node;

    if (!removed && (h->type_flags & 0x45) && !find_effect(h, 0x18, &node))
        attack_roll -= 4;
}

void far fx_confusing_gaze(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:0f59 */
{
    long node;
    char ok;

    if (!removed) {
        (*hit_proc)(0x44, 1, &ok);
        fx_target = targets[1];
        if (ok && get_distance(h, fx_target) < 2) {
            element_mask = 0x10;
            hero_text(h, "gazes...", 10, 0);
            icon_facings(0x14);
            icon_fly(combatant_x(h), combatant_y(h),
                         combatant_x(fx_target), combatant_y(fx_target), 4, 0x2d);
            if (!fx_saving_throw(fx_target, 4, 0) && !find_effect(fx_target, 0x7e, &node))
                fx_add_message(fx_target, 0x23, fx_roll_dice(1, 10) + 2, h->side, 1, 1, 0,
                             "stares blankly");
        }
    }
}

void far fx_highlighted(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1094 */
{
    if (!removed) {
        h->ac -= 2;
        h->ac_rear -= 2;
    }
}

void far fx_noop_slot_72(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:10ae */
{
}

void far fx_breath_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:10b3 */
{
    if (!removed && (element_mask & 0x20) > 0)
        fx_cancel(0);
}

void far fx_magic_boots(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:10ce */
{
    if (!removed)
        attack_rate *= 2;
}

void far fx_disease_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:10e1 */
{
    if (!removed)
        fx_cancel(0x22);
}

void far fx_magic_stone(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:10f4 */
{
    if (!removed)
        save_bonus++;
}

void far fx_healed_by_fire(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1103 */
{
    unsigned short hp;

    if (((char)element_mask & 1) && ((char)element_mask & 8)) {
        hero_text(h, "Is Healed.", 10, 1);
        hp = h->hp + damage;
        if (h->hp_max < hp)
            h->hp = h->hp_max;
        else
            h->hp = hp;
    } else if ((char)element_mask & 4) {
        fx_add(h, 0x2a, 3, 0xff, 0);
        hero_text(h, msg_format("is %s", fx_names[42]), 10, 1);
    }
    fx_cancel(0);
}

void far fx_halve_weapon_below_plus_3(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:11bf */
{
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0 && (char)((char)w->plus < 3))
        damage = damage / 2;
}

void far fx_halve_weapon_below_plus_4(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1202 */
{
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0 && (char)((char)w->plus < 4))
        damage = damage / 2;
}

void far fx_slain_by_type(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1245 */
{
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0 && w->type == 0x4a)
        fx_disable(h, 6, "is slain");
}

void far fx_poison_attack_penalized(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1288 */
{
    if (!removed)
        fx_poison_attack(h, 0xfe);
}

void far fx_adjacent_paralysis(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:12a3 */
{
    if (!removed && get_distance(h, h->fight->target) < 2)
        fx_paralyze(h, 100, 0);
}

void far fx_magic_cloak(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:12e1 */
{
    if (!removed) {
        if (!combat_round && !attack_roll)
            fx->strength = fx->strength & 0xf;
        else if (!(fx->strength & 0x10)) {
            attack_roll = -1;
            fx->strength = fx->strength | 0x10;
        }
    }
}

void far fx_fear_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1322 */
{
    if (!removed)
        fx_cancel(0x6f);
}

void far fx_fire_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1335 */
{
    fx_cancel_if(1);
}

void far fx_constitution_save_bonus(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1343 */
{
    char bonus;

    if (!removed && (current_save_type == 4 || current_save_type == 2 || current_save_type == 0)) {
        switch (h->con) {
        case 4: case 5: case 6:
            bonus = 1;
            break;
        case 7: case 8: case 9: case 10:
            bonus = 2;
            break;
        case 11: case 12: case 13:
            bonus = 3;
            break;
        case 14: case 15: case 16: case 17:
            bonus = 4;
            break;
        case 18: case 19: case 20:
            bonus = 5;
            break;
        }
        save_bonus = save_bonus + bonus;
    }
}

void far fx_resist_charm_sleep(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:13cc */
{
    if (!removed && fx_roll_dice(1, 100) <= 90) {
        fx_cancel(0xb);
        fx_cancel(0x35);
    }
}

void far fx_constitution_magic_saves(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:13f9 */
{
    char bonus;

    if (!removed && (current_save_type == 4 || current_save_type == 2)) {
        switch (h->con) {
        case 4: case 5: case 6:
            bonus = 1;
            break;
        case 7: case 8: case 9: case 10:
            bonus = 2;
            break;
        case 11: case 12: case 13:
            bonus = 3;
            break;
        case 14: case 15: case 16: case 17:
            bonus = 4;
            break;
        case 18: case 19: case 20:
            bonus = 5;
            break;
        }
        save_bonus = save_bonus + bonus;
    }
}

void far fx_cold_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1478 */
{
    if (!removed && ((char)element_mask & 2))
        fx_cancel(0);
}

void far fx_random_secondary_attacks(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1492 */
{
    if (fx_roll_dice(1, 4) == 1)
        h->attacks_off = 8;
    else
        h->attacks_off = 4;
}

void far fx_terrify(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:14bd */
{
    long node;
    struct Hero far *m;

    mirror_image_bypass = 1;
    node = 0;
    for (m = node_list; m != 0; m = m->next)
        if (m->side != h->side && m->status == 0 && m->hit_dice < 6
            && !find_effect(m, 0x78, &node) && !find_effect(m, 0x5c, &node))
            fx_add_message(m, 0x78, fx_roll_dice(4, 6), 0, 1, 1, fx_saving_throw(m, 4, 0),
                         "is terrified");
    mirror_image_bypass = 0;
}

void far fx_forget_spells(struct Hero far *h)                    /* 36e3:159d */
{
    int count[3][9];
    unsigned char spell;
    unsigned char i;
    unsigned short kind;
    unsigned char level;

    mem_zero(count, 0x36);
    for (i = 0; i <= 0x8c; i++)
        if (h->spells[i] != 0 && !(h->spells[i] & 0x80)) {
            spell = h->spells[i] & 0x7f;
            kind = spell_info[spell].kind;
            level = spell_info[spell].level;
            if (count[kind][level - 1] >= h->slots_by_level[kind][level]) {
                if (h->slots_by_level[kind][level] == 0 || !bonus_slots || kind != 2)
                    h->spells[i] = 0;
                else
                    bonus_slots--;
            } else
                count[kind][level - 1]++;
        }
    for (i = 0; i <= 0x8c; i++)
        if (h->spells[i] & 0x80) {
            spell = h->spells[i] & 0x7f;
            kind = spell_info[spell].kind;
            level = spell_info[spell].level;
            if (count[kind][level - 1] >= h->slots_by_level[kind][level])
                h->spells[i] = 0;
            else
                count[kind][level - 1]++;
        }
}

void far fx_drain(struct Hero far *h, unsigned char count)      /* 36e3:17b4 */
{
    unsigned char cand;
    unsigned char best_level;
    short idx;
    short best_idx;
    long upper;
    long lower;
    unsigned char pass;
    unsigned char active;
    char any;

    if (h != 0) {
        hero_text(h, "gets drained", 10, 1);
        text_pause();
        for (pass = 1; pass <= count; pass++) {
            active = 0;
            best_level = 0;
            for (idx = 0; idx <= 6; idx++) {
                cand = h->level[idx];
                if (cand > 0 || h->level_max[idx] > 0)
                    active++;
                if (cand > best_level) {
                    best_level = cand;
                    best_idx = idx;
                } else if (cand == best_level && best_level != 0
                           && level_xp_threshold(h, cand, best_idx) > level_xp_threshold(h, best_level, idx)) {
                    best_level = cand;
                    best_idx = idx;
                }
            }
            if (h->level_max[best_idx] <= h->level[best_idx])
                h->level_max[best_idx] = h->level[best_idx];
            if (h->xp_old <= h->xp)
                h->xp_old = h->xp;
            if (h->hp_max_old < h->hp_max)
                h->hp_max_old = h->hp_max;
            switch (best_level) {
            case 1:
                lower = 0;
                upper = 0;
                break;
            case 2:
                lower = 0;
                upper = level_xp_threshold(h, best_level, best_idx);
                break;
            default:
                lower = level_xp_threshold(h, best_level - 1, best_idx);
                upper = level_xp_threshold(h, best_level, best_idx);
            }
            h->xp = lower + (upper - lower) / 2;
            if (h->level[best_idx] > 0)
                h->level[best_idx]--;
            if (h->level[best_idx] >= h->dual_level)
                hp_adjust(h, class_bit[best_idx], active, 0);
            class_update(h);
            fx_forget_spells(h);
        }
        any = 0;
        for (idx = 0; idx <= 6; idx++)
            if (h->level[idx] > 0)
                any = 1;
        if (!any || h->hp_max <= 0)
            fx_disable(h, 8, "dies from draining");
        else if (h->hp <= 0)
            fx_disable(h, 4, "collapses from draining");
    }
}

void far fx_need_plus_1(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1a6c */
{
    fx_need_plus(1);
}

void far fx_level_damage_vs_type_8(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1a79 */
{
    if (!removed) {
        fx_target = h->fight->target;
        if (get_distance(h, fx_target) <= 1 && (fx_target->type_flags & 8))
            damage += class_level(h, 4);
    }
}

void far fx_stunned(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:1ad0 */
{
    combat_actions_end(h);
}

void far fx_restore_cure_allowance(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1ae2 */
{
    if (removed)
        h->cures = class_level(h, 3) / 5 + 1;
}

void far fx_adjacent_double_zap(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1b10 */
{
    short saved_damage;
    short saved_spell;
    short saved_mask;

    if (!removed && get_distance(cur_node, h) < 2) {
        saved_damage = damage;
        damage *= 2;
        if (damage > 0) {
            saved_mask = element_mask;
            element_mask = 8;
            saved_spell = current_spell;
            current_spell = 0x55;
            hero_text(cur_node, "gets zapped", 10, 1);
            fx_damage(cur_node, damage, 0, 0);
            damage = saved_damage;
            element_mask = saved_mask;
            current_spell = saved_spell;
        }
    }
}

void far fx_giant_str(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:1bac */
{
    if (!removed)
        hero_text(h, msg_format("%s%s", txt_gains, fx_names[113]), 10, 1);
}

void far fx_paralyze_gaze(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1be9 */
{
    char saved;
    long node;

    if (!removed) {
        (*hit_proc)(10, 1, &turn_done);
        if (enemy_team(h) != targets[1]->side)
            turn_done = 0;
        if (turn_done) {
            element_mask = 0x10;
            fx_target = targets[1];
            hero_text(h, "gazes...", 10, 0);
            icon_facings(0x14);
            icon_fly(combatant_x(h), combatant_y(h),
                         combatant_x(fx_target), combatant_y(fx_target), 4, 0x2d);
            if (find_effect(h, 0x3b, &node) && find_effect(fx_target, 0x48, &node)) {
                hero_text(fx_target, "reflects it!", 0xc, 0);
                icon_fly(combatant_x(fx_target), combatant_y(fx_target),
                             combatant_x(h), combatant_y(h), 4, 0x2d);
                fx_target = h;
            }
            saved = fx_saving_throw(fx_target, 1, 3) || find_effect(fx_target, 0x7e, &node);
            fx_add_message(fx_target, 0x34, 10, 0xc, 0, 1, saved, "is paralyzed");
        }
    }
}

void far fx_vary_primary_attack_damage(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:1dd1 */
{
    char text[0x28];
    char bonus;

    str_copy(text, "");
    bonus = str_damage(h);
    switch (attacks_used) {
    case 1:
        damage = fx_roll_dice(1, 10) + bonus + 4;
        str_copy(text, msg_format("%s%s", item_words[20], item_words[114]));
        break;
    case 2:
        damage = fx_roll_dice(2, 4) + bonus + 4;
        str_copy(text, msg_format("%s%s", item_words[16], item_words[114]));
        break;
    case 3:
        damage = fx_roll_dice(1, 8) + bonus + 4;
        str_copy(text, msg_format("%s%s", item_words[18], item_words[114]));
        break;
    case 4:
        damage = fx_roll_dice(1, 8) + bonus + 4;
        str_copy(text, msg_format("%s%s", item_words[13], item_words[114]));
        break;
    case 5:
        damage = fx_roll_dice(1, 4) + bonus + 4;
        str_copy(text, msg_format("%s%s", item_words[4], item_words[114]));
        break;
    case 6:
        damage = fx_roll_dice(1, 6) + bonus + 4;
        str_copy(text, msg_format("%s%s", item_words[14], item_words[114]));
        break;
    }
    text_wrap(0x17, 7, 0x26, 9, 7, 0, 1, text);
}

void far fx_armor_class_minus_four(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2000 */
{
    h->ac -= 4;
    h->ac_rear -= 4;
}

void far fx_druid_fire(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2014 */
{
    unsigned short left;

    left = 6;
    if ((char)element_mask & 1) {
        left = fx->strength * 12;
        if ((int)left > damage) {
            left -= damage;
            damage = 0;
            fx->strength = left / 12;
            if (fx->strength == 0)
                fx_remove(h, 0x77, 0L);
        } else {
            damage -= left;
            fx_remove(h, 0x77, 0L);
        }
    }
}

void far fx_reduce_damage_and_attack(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2086 */
{
    if (h->hit_dice < 3)
        combat_actions_end(h);
    if (damage > 2)
        damage -= 2;
    else
        damage = 0;
    attack_roll -= 2;
}

void far fx_iron_skin(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:20ba */
{
    h->ac += 4;
    h->ac_rear += 4;
}

void far fx_mind_blank(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:20ce */
{
    fx_cancel(0xb);
    fx_cancel(0x23);
    fx_cancel(0x34);
    fx_cancel(0x44);
    fx_cancel(0x6f);
}

void far fx_need_plus_2(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:20fb */
{
    fx_need_plus(2);
}

void far fx_save_bonus_mask_16(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2108 */
{
    if ((char)element_mask & 0x10)
        save_bonus = 0x14;
}

void far fx_paralyze_unconditional(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2119 */
{
    fx_paralyze(h, 100, 0);
}

void far fx_adjacent_save_aura(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2131 */
{
    struct Hero far *m;
    long node;

    hero_text(h, game_text[2], 10, 1);
    plan_steps(combatant_x(h), combatant_y(h), 1, 0xff, combatant_size(h));
    for (tick_slot = 2; (unsigned char)tick_slot <= step_count; tick_slot++) {
        m = (heroes - 1)[(steps - 1)[tick_slot].who];
        if (m->side != h->side && !fx_saving_throw(m, 4, 0) && !find_effect(m, 0x76, &node)) {
            hit_flash(m, 1, game_text[1]);
            fx_add(h, 0x94, 1, 0xff, 0);
        }
    }
}

void far fx_charm_sleep_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2242 */
{
    fx_cancel(0xb);
    fx_cancel(0x35);
}

void far fx_poison_hold_immune(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2257 */
{
    fx_cancel(0x37);
    fx_cancel(0x34);
    if (current_save_type == 0)
        save_bonus = 100;
}

void far fx_attack_penalty_two(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2278 */
{
    attack_roll -= 2;
}

void far fx_cancel_hold(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2282 */
{
    fx_cancel(0x34);
}

void far fx_weapon_type36_damage(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:228f */
{
    struct Item far *w;

    w = cur_node->weapon;
    if (w != 0 && w->type == 0x36)
        damage = fx_roll_damage(1, 6) + 1;
}

void far fx_add_damage_dice_count(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:22c6 */
{
    if ((element_mask & 1) && (element_mask & 8))
        damage += damage_dice_count;
}

void far fx_halve_magic_weapon_damage(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:22e4 */
{
    struct Item far *w;

    w = fx_weapon(cur_node);
    if (w != 0 && (char)w->plus > 0)
        damage = damage / 2;
}

void far fx_disease_touch(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:231c */
{
    long node;

    fx_target = h->fight->target;
    if (!find_effect(fx_target, 0x32, &node) && !find_effect(fx_target, 0xd7, &node)) {
        fx_add(fx_target, 0xd7, 0x3840, 0xff, 1);
        fx_add(fx_target, 0xac, 0xa8c0, (fx_roll_dice(1, 6) << 5) + 0x1f, 1);
        hero_text(fx_target, "is Diseased", 10, 1);
    }
}

void far fx_fear_aura(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:23d7 */
{
    struct Hero far *m;
    char bonus;
    char saved;

    for (m = node_list; m != 0; m = m->next) {
        fx_remove(m, 0xab, 0L);
        if (m->active && enemy_team(h) == m->side) {
            bonus = 0;
            if (side_count[m->side] >= 6)
                bonus++;
            if (m->race == 5)
                bonus += 2;
            saved = fx_saving_throw(m, 0, bonus);
            fx_add_message(m, 0x34, fx_roll_dice(1, 4), 0xc, 0, 1, saved, "is paralyzed with fear");
        }
    }
}

void far fx_rotting(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:24ae */
{
    if (!fx_lock) {
        hero_text(h, "Rots...", 10, 1);
        if (h->cha > 3)
            h->cha -= 2;
        if (h->cha < 3)
            h->cha = 3;
        if (fx->strength / 0x20 > 1) {
            fx->strength -= 0x20;
            fx_rearm(h, 0xac, fx->strength, 0xa8c0);
        } else
            fx_disable(h, 8, "Dies rots away");
    }
}

void far fx_heal_eight_on_mask_4(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2542 */
{
    if ((char)element_mask & 4) {
        fx_cancel(0);
        fx_roll_dice(1, 8);             /* the roll is dropped: 36e3:255a */
        h->hp += 8;
    }
}

void far fx_save_against_mask_1_or_2(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:256c */
{
    if (((char)element_mask & 1) || ((char)element_mask & 2)) {
        if (fx_saving_throw(h, 4, 0) && spell_info[current_spell].save_effect != 0)
            damage = 0;
        else
            damage = damage / 2;
    }
}

void far fx_halve_damage(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:25c4 */
{
    damage = damage / 2;
}

void far fx_engulf(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:25d5 */
{
    long node;
    struct Hero far *t;

    t = h->fight->target;
    if (hits_left == 2 && t->active && !find_effect(t, 0xb2, &node)
        && !find_effect(t, 0xb1, &node)) {
        t = h->fight->target;
        hero_text(h, msg_format("engulfs %s", t->name), 0xc, 1);
        fx_add(t, 0xb2, 0, combatant_slot(t), 0);
        fx_dispatch(0xb2, t, 0L, 0);
        fx_add(t, 0xb1, 0, fx_roll_dice(2, 4), 0);
        fx_add(h, 0xb3, 0, combatant_slot(t), 1);
    }
}

void far fx_suffocate(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2702 */
{
    if (fx->strength == 0)
        fx_disable(h, 6, "Suffocates");
    else
        fx->strength--;
}

void far fx_attack_linked_character(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2730 */
{
    fx_target = (heroes - 1)[fx->strength];
    if (fx_target == 0)
        fx_remove(h, 0xb3, fx);
    else if (removed || !h->active || !fx_target->active) {
        fx_remove(fx_target, 0xb2, 0L);
        fx_target->fight->moves = calc_move(fx_target);
        fx_remove(fx_target, 0xb1, 0L);
        if (!removed || !h->active) {
            fx->on_remove = 0;
            fx_remove(h, 0xb3, fx);
        }
    } else {
        h->attacks_main = 2;
        h->attacks_off = 0;
        h->dice_n_main = 2;
        h->dice_s_main = 8;
        combat_attack(h, fx_target, 1, 0L, &turn_done);
        turn_done = combat_actions_end(h);
        if (!fx_target->active) {
            fx_remove(h, 0xb3, 0L);
            fx_remove(fx_target, 0xb2, 0L);
            fx_remove(fx_target, 0xb1, 0L);
        }
    }
}

void far fx_armor_acid(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:288d */
{
    int floor;
    struct Item far *it;
    char hit;

    fx_target = h->fight->target;
    it = fx_target->items;
    hit = 0;
    for (; it != 0; it = it->next)
        if (it->readied && item_table[it->type].slot == 2) {
            hit = 1;
            if (it->type == 0x23 || it->type == 0x24 || it->type == 0x26)
                floor = -1;
            else
                floor = 0;
            if ((char)it->plus > floor) {
                it->plus--;
                hero_text(h, msg_format("is Disolving %s's armor!", fx_target->name), 10, 1);
            } else {
                hero_text(h, msg_format("Dissolves %s's armor!", fx_target->name), 10, 1);
                item_remove(fx_target, it);
            }
        }
    if (hit)
        damage = 0;
}

void far fx_need_plus_any(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:29af */
{
    struct Item far *w;

    fx_need_plus(0xff);
    w = fx_weapon(cur_node);
    if (w == 0 && !(element_mask & 8))
        damage = 0;
}

void far fx_grant_gorgon_breath(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:29e7 */
{
    long node;
    struct Hero far *m;

    for (m = node_list; m != 0; m = m->next)
        if (find_effect(m, 0xc3, &node) && !find_effect(h, 0xcb, &node))
            fx_add(h, 0xcb, 0xff, 4, 0);
}

void far fx_gorgon_breath(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2a60 */
{
    char x;
    char y;
    unsigned char i;
    struct Hero far *t;
    char saved;
    char size;
    char reach;
    char dir;

    if (fx->strength != 0) {
        fx->strength--;
        mirror_image_bypass = 1;
        element_mask = 0;
        size = 2;
        reach = 6;
        x = combatant_x(h);
        y = combatant_y(h);
        (*hit_proc)(0x17, 1, &turn_done);
        if (turn_done) {
            spell_path_targets(x, y, view_cx, view_cy, size, reach);
            for (i = 1; i <= target_count; i++)
                if (enemy_team(h) != targets[i]->side)
                    turn_done = 0;
            if (target_count > 0 && turn_done) {
                hero_text(h, "breathes", 10, 1);
                dir = combatant_direction(h, targets[1]);
                hero_panel(h);
                combatant_facing_set(h, dir, 1, 0);
                icon_facings(0x13);
                icon_fly(combatant_x(h), combatant_y(h),
                             combatant_x(targets[1]), combatant_y(targets[1]), 1, 0x1e);
                for (i = 1; i <= target_count; i++)
                    if (targets[i] != 0) {
                        t = targets[i];
                        saved = fx_saving_throw(t, 1, 0);
                        if (!saved)
                            fx_disable(t, 7, "is petrified by the gorgon's breath");
                    }
                combat_actions_end(h);
            }
        }
        mirror_image_bypass = 0;
    }
}

void far fx_rot_touch(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:2c65 */
{
    if (fx_rearm(h, 0xd7, fx->strength, 0x3840) && fx_heal(h, 1, 1))
        heal_msg(h);
}

void far fx_revive_unless_fire(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2cad */
{
    if (!((char)element_mask & 1) && !((char)element_mask & 0)) /* test byte [c64e],0: 2cb7 */
        fx_add(h, 0xda, fx_roll_dice(3, 6), 0xff, 1);
}

void far fx_regen_delay(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2ce4 */
{
    fx_add(h, 0xe2, 0, 0xff, 0);
}

void far fx_regenerate_three(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2d02 */
{
    h->hp += 3;
    if (h->hp > h->hp_max)
        h->hp = h->hp_max;
}

void far fx_regen_trigger(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2d26 */
{
    long node;

    if (!find_effect(h, 0xe2, &node) && !find_effect(h, 0xe1, &node))
        fx_add(h, 0xe1, 3, 0xff, 1);
}

void far fx_rise_again(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2d7f */
{
    if (!fx_revive(h, h->hp_max, "stands up and grins"))
        fx_rearm(h, 0xda, fx->strength, 1);
}

void far fx_fire_minus_one_per_die(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2dbc */
{
    unsigned char i;

    if ((char)element_mask & 1)
        for (i = 1; i <= damage_dice_count; i++) {
            damage--;
            if (damage_dice_count > damage)
                damage = damage_dice_count;
        }
}

void far fx_bear_hug(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2df6 */
{
    if (attack_roll >= 0x12) {
        fx_target = h->fight->target;
        hero_text(h, msg_format("hugs %s", fx_target->name), 0xc, 1);
        fx_add(fx_target, 0xdd, 0, combatant_slot(fx_target), 0);
        fx_dispatch(0xdd, fx_target, 0L, 0);
        fx_add(h, 0xde, 0, combatant_slot(fx_target), 1);
    }
}

void far fx_hugged(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2eac */
{
    if (h->fight != 0) {
        h->fight->moves = 0;
        if (movement_stops_attack_rate)
            move_rate = 0;
    }
}

void far fx_hug_attack(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2ed3 */
{
    char done;

    fx_target = (heroes - 1)[fx->strength];
    if (removed || !h->active || !fx_target->active) {
        fx_remove(fx_target, 0xdd, 0L);
        if (!removed) {
            fx->on_remove = 0;
            fx_remove(h, 0xde, fx);
        }
    } else {
        h->attacks_main = 1;
        h->attacks_off = 0;
        h->dice_n_main = 2;
        h->dice_s_main = 8;
        combat_attack(h, fx_target, 1, 0L, &done);
        done = combat_actions_end(h);
        if (!fx_target->active) {
            fx_remove(h, 0xde, 0L);
            fx_remove(fx_target, 0xdd, 0L);
        }
    }
}

void far fx_fire_breath_single(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:2fd8 */
{
    char ok;
    char saved;
    char dir;

    if (h->fight != 0 && !removed) {
        element_mask = 0x21;
        (*hit_proc)(0x44, 1, &ok);
        fx_target = targets[1];
        if (ok) {
            dir = combatant_direction(h, fx_target);
            combatant_facing_set(h, dir, 1, 0);
            hero_text(h, "Breathes Fire", 10, 1);
            icon_facings(0x19);
            icon_fly(combatant_x(h), combatant_y(h),
                         combatant_x(fx_target), combatant_y(fx_target), 1, 0x1e);
            saved = fx_saving_throw(fx_target, 3, 0);
            fx_damage(fx_target, 7, 2, saved);
        }
    }
}

void far fx_ghoul_paralysis(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:30f9 */
{
    if (h->fight->target->race != 0)
        fx_paralyze(h, 0x3f, 0);
}

void far fx_none(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:3123 */
{
}

void far fx_item(struct Hero far *h, struct Effect far *fx, char removed) /* 36e3:3128 */
{
    struct Item far *item;

    item_handler_active = 0;
    item = (struct Item far *)fx;
    if (removed)
        fx_remove(h, item->effect, 0L);
    else
        fx_add(h, item->effect, 0, 0xff, 1);
}

void far fx_install(void)                                        /* 36e3:317e */
{
    int i;

    for (i = 0; i <= 0xe5; i++)
        (fx_handler - 1)[i] = fx_none;
    (fx_handler - 1)[1] = fx_bless;
    (fx_handler - 1)[2] = fx_curse;
    (fx_handler - 1)[3] = fx_snakes;
    (fx_handler - 1)[4] = fx_dispel_evil;
    (fx_handler - 1)[5] = fx_none;
    (fx_handler - 1)[6] = fx_elec_immune;
    (fx_handler - 1)[7] = fx_attack_bonus_vs_immunity4;
    (fx_handler - 1)[8] = fx_prot_evil;
    (fx_handler - 1)[9] = fx_prot_good;
    (fx_handler - 1)[10] = fx_resist_cold;
    (fx_handler - 1)[11] = fx_charm;
    (fx_handler - 1)[12] = fx_enlarged;
    (fx_handler - 1)[13] = fx_barkskin;
    (fx_handler - 1)[14] = fx_friendly;
    (fx_handler - 1)[15] = fx_slow_poison;
    (fx_handler - 1)[16] = fx_read_magic;
    (fx_handler - 1)[17] = fx_shielded;
    (fx_handler - 1)[18] = fx_half_elf;
    (fx_handler - 1)[19] = fx_none;
    (fx_handler - 1)[20] = fx_resist_fire;
    (fx_handler - 1)[21] = fx_silence;
    (fx_handler - 1)[22] = fx_slow_poison_end;
    (fx_handler - 1)[23] = fx_spirit_hammer;
    (fx_handler - 1)[24] = fx_none;
    (fx_handler - 1)[25] = fx_invisible;
    (fx_handler - 1)[26] = fx_attack_bonus_vs_type80;
    (fx_handler - 1)[27] = fx_end_actions;
    (fx_handler - 1)[28] = fx_mirror_image;
    (fx_handler - 1)[29] = fx_weakened;
    (fx_handler - 1)[30] = fx_nausea;
    (fx_handler - 1)[31] = fx_end_actions;
    (fx_handler - 1)[32] = fx_dispel_evil_attack;
    (fx_handler - 1)[33] = fx_blind;
    (fx_handler - 1)[34] = fx_diseased;
    (fx_handler - 1)[35] = fx_confusion;
    (fx_handler - 1)[36] = fx_accursed;
    (fx_handler - 1)[37] = fx_blink;
    (fx_handler - 1)[38] = fx_enlarged;
    (fx_handler - 1)[39] = fx_hastened;
    (fx_handler - 1)[40] = fx_block_lower_spell_levels;
    (fx_handler - 1)[41] = fx_missile_prot;
    (fx_handler - 1)[42] = fx_slowed;
    (fx_handler - 1)[43] = fx_periodic_strength_drain;
    (fx_handler - 1)[44] = fx_periodic_hp_drain;
    (fx_handler - 1)[45] = fx_prot_evil;
    (fx_handler - 1)[46] = fx_prot_good;
    (fx_handler - 1)[47] = fx_attack_penalty_type_4;
    (fx_handler - 1)[48] = fx_attack_penalty_type_20;
    (fx_handler - 1)[49] = fx_prayer;
    (fx_handler - 1)[50] = fx_fire_shield_hot;
    (fx_handler - 1)[51] = fx_end_actions;
    (fx_handler - 1)[52] = fx_end_actions;
    (fx_handler - 1)[53] = fx_end_actions;
    (fx_handler - 1)[54] = fx_fire_shield_cold;
    (fx_handler - 1)[55] = fx_poisoned;
    (fx_handler - 1)[56] = fx_invis_ring;
    (fx_handler - 1)[57] = fx_major_globe;
    (fx_handler - 1)[58] = fx_stone_gaze;
    (fx_handler - 1)[59] = fx_noop_slot_59;
    (fx_handler - 1)[60] = fx_need_plus_3;
    (fx_handler - 1)[61] = fx_fire_ring;
    (fx_handler - 1)[62] = fx_periodic_regeneration;
    (fx_handler - 1)[63] = fx_minor_globe;
    (fx_handler - 1)[64] = fx_poison_attack_normal;
    (fx_handler - 1)[65] = fx_petrify_attack;
    (fx_handler - 1)[66] = fx_halve_low_class_weapon_dmg;
    (fx_handler - 1)[67] = fx_halve_high_bit_weapon_damage;
    (fx_handler - 1)[68] = fx_feeblemind;
    (fx_handler - 1)[69] = fx_animal_invisibility;
    (fx_handler - 1)[70] = fx_confusing_gaze;
    (fx_handler - 1)[71] = fx_highlighted;
    (fx_handler - 1)[72] = fx_noop_slot_72;
    (fx_handler - 1)[73] = fx_breath_immune;
    (fx_handler - 1)[74] = fx_magic_boots;
    (fx_handler - 1)[75] = fx_damage_vs_immunity1;
    (fx_handler - 1)[76] = fx_disease_immune;
    (fx_handler - 1)[77] = fx_berserk;
    (fx_handler - 1)[78] = fx_magic_stone;
    (fx_handler - 1)[79] = fx_healed_by_fire;
    (fx_handler - 1)[80] = fx_halve_weapon_below_plus_3;
    (fx_handler - 1)[81] = fx_halve_weapon_below_plus_4;
    (fx_handler - 1)[82] = fx_slain_by_type;
    (fx_handler - 1)[83] = fx_breath_weapon;
    (fx_handler - 1)[84] = fx_breath_weapon;
    (fx_handler - 1)[85] = fx_breath_weapon;
    (fx_handler - 1)[86] = fx_poison_attack_penalized;
    (fx_handler - 1)[87] = fx_none;
    (fx_handler - 1)[88] = fx_adjacent_paralysis;
    (fx_handler - 1)[89] = fx_magic_cloak;
    (fx_handler - 1)[90] = fx_poison_steam_breath;
    (fx_handler - 1)[91] = fx_lightning_breath;
    (fx_handler - 1)[92] = fx_fear_immune;
    (fx_handler - 1)[93] = fx_fire_immune;
    (fx_handler - 1)[94] = fx_constitution_save_bonus;
    (fx_handler - 1)[95] = fx_resist_charm_sleep;
    (fx_handler - 1)[96] = fx_breath_weapon;
    (fx_handler - 1)[97] = fx_constitution_magic_saves;
    (fx_handler - 1)[98] = fx_cold_immune;
    (fx_handler - 1)[99] = fx_random_secondary_attacks;
    (fx_handler - 1)[100] = fx_terrify;
    (fx_handler - 1)[101] = fx_drain_one_level;
    (fx_handler - 1)[102] = fx_drain_two_levels;
    (fx_handler - 1)[103] = fx_need_plus_1;
    (fx_handler - 1)[104] = fx_fire_damage_4d6;
    (fx_handler - 1)[105] = fx_level_damage_vs_type_8;
    (fx_handler - 1)[106] = fx_stunned;
    (fx_handler - 1)[107] = fx_berserk_creature;
    (fx_handler - 1)[108] = fx_fire_damage_2d6;
    (fx_handler - 1)[109] = fx_none;
    (fx_handler - 1)[110] = fx_restore_cure_allowance;
    (fx_handler - 1)[111] = fx_fear_remove;
    (fx_handler - 1)[112] = fx_adjacent_double_zap;
    (fx_handler - 1)[113] = fx_giant_str;
    (fx_handler - 1)[114] = fx_paralyze_gaze;
    (fx_handler - 1)[115] = fx_vorpal;
    (fx_handler - 1)[116] = fx_vary_primary_attack_damage;
    (fx_handler - 1)[117] = eye_rays;
    (fx_handler - 1)[118] = fx_armor_class_minus_four;
    (fx_handler - 1)[119] = fx_druid_fire;
    (fx_handler - 1)[120] = fx_reduce_damage_and_attack;
    (fx_handler - 1)[121] = fx_fire_damage_2d6_alt;
    (fx_handler - 1)[122] = fx_iron_skin;
    (fx_handler - 1)[123] = fx_mind_blank;
    (fx_handler - 1)[124] = fx_spell_gaze;
    (fx_handler - 1)[125] = fx_need_plus_2;
    (fx_handler - 1)[126] = fx_save_bonus_mask_16;
    (fx_handler - 1)[127] = fx_item;
    fx_install_2();
}

static void far fx_install_2(void)                               /* 36e3:36d0 */
{
    (fx_handler - 1)[132] = fx_paralyze_unconditional;
    (fx_handler - 1)[137] = fx_adjacent_save_aura;
    (fx_handler - 1)[141] = fx_charm_sleep_immune;
    (fx_handler - 1)[143] = fx_poison_hold_immune;
    (fx_handler - 1)[148] = fx_attack_penalty_two;
    (fx_handler - 1)[158] = fx_cancel_hold;
    (fx_handler - 1)[159] = fx_weapon_type36_damage;
    (fx_handler - 1)[168] = fx_add_damage_dice_count;
    (fx_handler - 1)[169] = fx_halve_magic_weapon_damage;
    (fx_handler - 1)[170] = fx_disease_touch;
    (fx_handler - 1)[171] = fx_fear_aura;
    (fx_handler - 1)[172] = fx_rotting;
    (fx_handler - 1)[173] = fx_heal_eight_on_mask_4;
    (fx_handler - 1)[174] = fx_save_against_mask_1_or_2;
    (fx_handler - 1)[175] = fx_halve_damage;
    (fx_handler - 1)[176] = fx_engulf;
    (fx_handler - 1)[177] = fx_suffocate;
    (fx_handler - 1)[178] = fx_movement_stop;
    (fx_handler - 1)[179] = fx_attack_linked_character;
    (fx_handler - 1)[186] = fx_armor_acid;
    (fx_handler - 1)[193] = fx_need_plus_any;
    (fx_handler - 1)[195] = fx_grant_gorgon_breath;
    (fx_handler - 1)[203] = fx_gorgon_breath;
    (fx_handler - 1)[215] = fx_rot_touch;
    (fx_handler - 1)[216] = fx_revive_unless_fire;
    (fx_handler - 1)[217] = fx_regen_trigger;
    (fx_handler - 1)[218] = fx_rise_again;
    (fx_handler - 1)[219] = fx_fire_minus_one_per_die;
    (fx_handler - 1)[220] = fx_bear_hug;
    (fx_handler - 1)[221] = fx_hugged;
    (fx_handler - 1)[222] = fx_hug_attack;
    (fx_handler - 1)[223] = fx_fire_breath_single;
    (fx_handler - 1)[224] = fx_ghoul_paralysis;
    (fx_handler - 1)[225] = fx_regen_delay;
    (fx_handler - 1)[226] = fx_regenerate_three;
}

void far fx_dispatch(unsigned char id, struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 36e3:3831 */
{
    if (item_handler_active)
        (*(fx_handler - 1)[0x7f])(h, fx, removed);      /* fx_item */
    else
        (*(fx_handler - 1)[id])(h, fx, removed);
}
