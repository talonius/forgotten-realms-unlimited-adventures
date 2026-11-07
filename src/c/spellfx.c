/* CKIT.EXE overlay 12 module 26e3: spell effects.  A handler for each filled
 * spell_fn id -- spells, potions, wands and other magic items -- called
 * through spell_fn[id] once the targets are chosen: it rolls the dice,
 * applies the effect to targets[1..target_count] or to the caster, and
 * reports it with one of the effect messages.  Also the lightning bolt's
 * bouncing path, the meteor swarm's steps, the patches of stinking cloud,
 * cloudkill and blade barrier, an item giving up a stored spell, and the
 * "Casts a Spell" announcement.
 *
 * EXE 0x054eb0..0x05971c, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 26e3, the first module of RTLink section 12 (payload
 * 0x54eb0, link segment 26e3, which sections 11 and 13 also use).  Its last
 * retf is byte 0xb of paragraph 2b69; module 2b69 starts at 2b69:000c.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 12.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and ovl12_2b69_NNNN names a routine of
 * section 12's module 2b69.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  The handlers keep the
 * names Dark Queen of Krynn's spellfx.c gives them.
 *
 * spellfx_init (vector stub 18de:194f) fills spell_fn, 89 entries itself
 * and 44 through spell_fn_fill, and sets hit_proc, fx_lock, item_cast_active and
 * inventory_redraw.  Module 2b69 makes the calls (0x5b89b).  Ids 65, 95, 97 and 99
 * of 1 to 137 are never filled.  Module 2b69 also far-calls item_casts and
 * cast_announce; vector stubs reach item_casts (18de:1acb), item_spend
 * (1cbf), cast_announce (1cc9), spell_remove_curse (1afd) and bolt_run
 * (1cb5).  Nothing found calls spellfx_unit_init.
 *
 * _DATA is diag_dx, diag_dy and 63 stored literals, DGROUP 82e6..8655; -d
 * stores "" as the tail of "is %s".  _BSS is fx_pool and fx_cost, d216..d217.
 *
 * targets is indexed from 1.  heroes, recs, combatants, steps and
 * tile_info are indexed from 1 too; the shipped code adds the address of
 * element 0 (b8f1, ce0b, cc76, ca28, 5ff1).
 */

struct Effect {                 /* an effect on a party member            */
    unsigned char kind;         /* +0                                     */
    char pad_1[3];              /* +1   not read here                     */
    unsigned char strength;     /* +4   level in bits 0-4; 0xff: kept     */
    char pad_5;                 /* +5   not read here                     */
    struct Effect far *next;    /* +6                                     */
};

struct Item {                   /* 0x3e bytes, a item_record_pool record         */
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28                                    */
    unsigned char name1;        /* +29                                    */
    char pad_2a[8];             /* +2a  not read here                     */
    char readied;               /* +32  cleared by remove curse           */
    char pad_33;                /* +33  not read here                     */
    char cursed;                /* +34                                    */
    unsigned char slots[4];     /* +35  [1..3] spell ids in bits 0-6      */
    char pad_39;                /* +39  not read here                     */
    struct Item far *more;      /* +3a                                    */
};

struct Fight {                  /* a combatant's state in a fight         */
    char pad_0[0xa];            /* +0   not read here                     */
    unsigned char facing;       /* +a                                     */
    struct Hero far *target;    /* +b                                     */
    char pad_f[2];              /* +f   not read here                     */
    char panicked;              /* +11                                    */
};

struct Hero {                   /* a party member or a monster            */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x34];           /* +c   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    long xp;                    /* +44                                    */
    long xp_old;                /* +48                                    */
    char pad_4c[6];             /* +4c  not read here                     */
    unsigned age;               /* +52                                    */
    char pad_54[4];             /* +54  not read here                     */
    unsigned char race;         /* +58                                    */
    unsigned char cls;          /* +59                                    */
    char pad_5a[4];             /* +5a  not read here                     */
    unsigned char status;       /* +5e  0: conscious                      */
    unsigned char side;         /* +5f  the side side_affect keeps        */
    char name[0x11];            /* +60                                    */
    unsigned char str;          /* +71                                    */
    char pad_72[6];             /* +72  not read here                     */
    unsigned char con_base;     /* +78  raise dead needs it nonzero       */
    char pad_79[3];             /* +79  not read here                     */
    unsigned char str_ex;       /* +7c                                    */
    char pad_7d[4];             /* +7d  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    unsigned char icon_dim;     /* +82                                    */
    char pad_83[4];             /* +83  not read here                     */
    unsigned char save_spell;   /* +87                                    */
    char pad_88;                /* +88  not read here                     */
    unsigned char hit_dice;     /* +89                                    */
    char pad_8a[9];             /* +8a  not read here                     */
    unsigned char control;      /* +93                                    */
    char pad_94[2];             /* +94  not read here                     */
    unsigned char level_max[7]; /* +96  one per class: restoration's goal */
    unsigned char level[7];     /* +9d  one per class                     */
    char pad_a4[0x12];          /* +a4  not read here                     */
    unsigned char hp_max_old;   /* +b6                                    */
    char pad_b7;                /* +b7  not read here                     */
    unsigned char hp_base;      /* +b8                                    */
    char pad_b9[6];             /* +b9  not read here                     */
    unsigned char type_flags;   /* +bf                                    */
    unsigned char immunity;     /* +c0                                    */
    char pad_c1[4];             /* +c1  not read here                     */
    char level_up;              /* +c5                                    */
    unsigned char spells[0x8d]; /* +c6  feeblemind clears those with bit 7 */
    char pad_153[0x2b];         /* +153 not read here                     */
    char active;                /* +17e                                   */
    char quick_fight;           /* +17f                                   */
    char pad_180;               /* +180 not read here                     */
    unsigned char ac;           /* +181                                   */
    char pad_182[9];            /* +182 not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct Patch {                  /* 0x21 bytes: a patch on the combat map  */
    struct Hero far *caster;    /* +0                                     */
    struct Patch far *next;     /* +4                                     */
    unsigned char terrain[10];  /* +8   [1..name1] the terrain under it   */
    char active[10];            /* +12  [1..name1] nonzero: on the map    */
    char x;                     /* +1c                                    */
    char y;                     /* +1d                                    */
    unsigned char strength;     /* +1e  dispel magic sets 0xff            */
    unsigned char level;        /* +1f  the caster's level                */
    unsigned char kind;         /* +20  the map cell it becomes           */
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
    char dir;                   /* +17  not read here                     */
};

struct Step {
    unsigned char who;          /* +0   index into heroes                 */
    unsigned char cost;         /* +1   not read here                     */
    unsigned char dir;          /* +2   not read here                     */
};

struct Fallen {                 /* 9 bytes: a fallen combatant            */
    struct Hero far *hero;      /* +0                                     */
    int x;                      /* +4                                     */
    int y;                      /* +6                                     */
    char terrain;               /* +8   the terrain under it              */
};

struct Combatant {
    int x;                      /* +0                                     */
    int y;                      /* +2                                     */
    char number;                /* +4   not read here                     */
    unsigned char size;         /* +5                                     */
};

struct Map {
    char pad_0[9];              /* +0   not read here                     */
    unsigned char cell[25][50]; /* +9   by row                            */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    char pad_1[3];              /* +1   not read here                     */
};

struct SpellInfo {              /* 0x16 bytes a spell, spelltab.c's       */
    char pad_0[0xa];            /* +0   not read here                     */
    int save_effect;            /* +a                                     */
    int save_type;              /* +c   not read here                     */
    unsigned char effect;       /* +e                                     */
    char pad_f[7];              /* +f   not read here                     */
};

struct Pool {                   /* strutil.c's slot pool                  */
    int capacity;               /* +0                                     */
    int stride;                 /* +2                                     */
    char far *base;             /* +4                                     */
    unsigned char bitmap[1];    /* +8                                     */
};

extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern char far is_helpless(struct Hero far *h);                   /* 03d0:09f2 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far item_remove(struct Hero far *h, struct Item far *it); /* 03d0:13c4 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far heal_msg(struct Hero far *h);                   /* 03d0:1878 */
extern unsigned char far caster_level(unsigned char spell, char cap); /* 03d0:18fe */
extern char far can_level_up(struct Hero far *h);                   /* 03d0:1f71 */
extern char far con_hp_adj(struct Hero far *h, short arg);        /* 03d0:2045 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern long far level_xp_threshold(struct Hero far *h, unsigned char level, short cls);
                                                                 /* 03d0:21cc */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far msg_pause(void);                                 /* 06c9:0953 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw_top(void);                                 /* 083a:03ea */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far fx_disable(struct Hero far *h, int how, char far *msg);
                                /* 18de:10e9, vector to overlay 19 2e88:000f */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
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
extern void far fx_terrain_apply(struct Hero far *h, char may_save, char skip_1d);
                                /* 18de:11a7, vector to overlay 19 2e88:0faa */
extern void far fx_clouds_tick(char now);
                                /* 18de:11b1, vector to overlay 19 2e88:2461 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame, char erase);
                                /* 18de:11cf, vector to overlay 0 22fa:09a2 */
extern void far combat_view_scroll(char x, char y, unsigned char margin, unsigned char dir);
                                /* 18de:11d9, vector to overlay 0 22fa:0847 */
extern char far hero_place(struct Hero far *h, unsigned char x, unsigned char y,
                             char moving);
                                /* 18de:11e3, vector to overlay 0 22fa:1053 */
extern void far tile_update(char x, char y, unsigned char who);
                                /* 18de:11ed, vector to overlay 0 22fa:04d5 */
extern unsigned char far combatant_slot(struct Hero far *h);
                                /* 18de:11f7, vector to overlay 0 22fa:0b44 */
extern void far icon_scatter(unsigned char radius, unsigned char cell, unsigned char sound);
                                /* 18de:120b, vector to overlay 0 1cb8:0d53 */
extern void far icon_fly(int x0, int y0, int x1, int y1, unsigned char frames, unsigned char pace);
                                /* 18de:121f, vector to overlay 0 1cb8:00de */
extern unsigned char far combat_targets_near(struct Hero far *h, unsigned char range, char same);
                                /* 18de:1229, vector to overlay 0 1cb8:0971 */
extern void far hit_flash(struct Hero far *h, char a, char far *msg);
                                /* 18de:123d, vector to overlay 0 1cb8:0730 */
extern int  far enemy_team(struct Hero far *h);
                                /* 18de:1247, vector to overlay 0 1cb8:08fc */
extern void far missile_facing(unsigned char dir, unsigned char cell);
                                /* 18de:1251, vector to overlay 0 1cb8:0f2c */
extern void far icon_facings(unsigned char cell);
                                /* 18de:125b, vector to overlay 0 1cb8:0052 */
extern char far line_step(struct LineWalk far *l);
                                /* 18de:1265, vector to overlay 2 2632:020c */
extern void far line_init(struct LineWalk far *l);
                                /* 18de:126f, vector to overlay 2 2632:01a0 */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern int  far fx_roll_damage(unsigned char n, unsigned char sides);
                                /* 18de:12ab, vector to overlay 19 2e88:1624 */
extern char far dir_ok(char x0, char y0, char x1, char y1, unsigned char who);
                                /* 18de:12bf, vector to overlay 2 2632:048c */
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
extern unsigned char far combat_terrain_under(char x, char y);
                                /* 18de:1337, vector to overlay 0 22fa:130b */
extern void far class_update(struct Hero far *hero);
                                /* 18de:1729, vector to overlay 14 2ccd:05c5 */
extern void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove);
                                /* 18de:19bd, vector to overlay 19 2e88:1648 */
extern char far fx_heal(struct Hero far *hero, char a, char b);
                                /* 18de:1af3, vector to overlay 19 2e88:224c */
extern void far fx_stat_recalc(struct Hero far *hero, short stat);
                                /* 18de:1b07, vector to overlay 19 2e88:1a80 */
extern void far fx_dispatch(unsigned char kind, struct Hero far *h,
                             struct Effect far *fx, char removed);
                                /* 18de:1c8d, vector to overlay 11 26e3:3831 */
extern void far fx_add_message(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove, int save_negates,
                             char saved, char far *msg);
                                /* 18de:1c97, vector to overlay 19 2e88:2181 */
extern char far fx_revive(struct Hero far *h, unsigned char hp, char far *msg);
                                /* 18de:1cab, vector to overlay 19 2e88:230c */
extern char far fx_strength_upgrade(struct Hero far *h, unsigned char base, unsigned char ex,
                             unsigned char far *out);
                                /* 18de:1cd3, vector to overlay 19 2e88:190c */
extern char far fx_cure(struct Hero far *h, unsigned char kind);
                                /* 18de:1cdd, vector to overlay 19 2e88:184b */
extern int  far spell_duration(unsigned char spell);            /* 2b69:10d7 */
extern void far spell_damage_apply(unsigned char spell, char level, char b, unsigned char dmg, int d,
                                char far *msg);
                                                                 /* 2b69:11b5 */
extern void far camp_spell_targets(unsigned char spell, char mode, char far *ok);
                                                                 /* 2b69:13cb */
extern void far spell_path_targets(char x0, char y0, char x1, char y1, unsigned char width,
                                unsigned char len);
                                                                 /* 2b69:1a61 */
extern void far spell_effect_apply(char n, char far *msg);          /* 2b69:1e6f */

extern char far *spell_name_table[];                             /* 2438 */
#define spell_names (spell_name_table - 1)    /* spell numbers start at 1 */
extern struct SpellInfo spell_info_table[];                      /* 2660 */
#define spell_info (spell_info_table - 1)     /* spell numbers start at 1 */
extern char far *txt_abort_spell;                                /* 39fc */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *txt_flame_type;                                 /* 3bd0 */
extern char far *txt_hot_cold;                                   /* 3c44 */
extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern unsigned char patch_dir[];                                /* 6199 */
extern unsigned char hit_dice_level_limit[];                                 /* 6262 */
extern char far *fx_names[];                                     /* 6379 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern int view_mode;                                            /* b421 */
extern int damage;                                               /* b42f */
extern void (far *spell_fn[])(void);                             /* b4f0 */
extern char effect_id;                                           /* b720 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern char fx_lock;                                             /* b8d0 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern struct Pool patch_pool;                                    /* bb90 */
extern unsigned char current_spell;                                  /* be64 */
extern char item_cast_active;                                           /* be69 */
extern char view_cx;                                             /* c39f */
extern char view_cy;                                             /* c3bc */
extern char item_handler_active;                                           /* c523 */
extern int element_mask;                                         /* c64e */
extern struct Map far *cur_map;                                  /* c66b */
extern char mirror_image_bypass;                                           /* ca08 */
extern struct Patch far *patch_list;                             /* ca0a */
extern unsigned char step_count;                                 /* ca10 */
extern struct Step steps[];                                      /* ca2b */
extern char inventory_redraw;                                           /* cb08 */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char move_flag;                                           /* cc79 */
extern struct Combatant combatants[];                            /* cc7c */
extern struct Fallen recs[8];                                    /* ce14 */
extern unsigned char rec_count;                                  /* cfd7 */
extern struct Hero far *targets[];                               /* cfe3 */
extern struct Hero far *node_list;                               /* d129 */
extern void (far *hit_proc)(unsigned char spell, char mode, char far *result); /* d131 */
extern unsigned char target_count;                               /* d137 */

static int diag_dx[4] = { -1, 1, -1, 1 };                        /* 82e6 */
static int diag_dy[4] = { -1, -1, 1, 1 };                        /* 82ee */

static unsigned char fx_pool;                                    /* d216 */
static unsigned char fx_cost;                                    /* d217 */

/* The translated Pascal unit's empty initialization block (DKK magic.pas). */
void far spellfx_unit_init(void)                                   /* 36e3:0000 */
{
}

void far side_affect(int id, char far *msg)                      /* 36e3:0001 */
{
    unsigned char i;

    mirror_image_bypass = 1;
    for (i = 1; i <= target_count; i++) {
        if (targets[i]->side != id ||
            (current_spell == 1 && view_mode == 5 && combat_targets_near(targets[i], 1, 0) > 0))
            targets[i] = 0;
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, msg);
}

void far spell_bless(void)                                       /* 36e3:00a4 */
{
    side_affect(cur_node->side, msg_format("is %s", fx_names[1]));
}

void far spell_curse(void)                                       /* 36e3:00ce */
{
    side_affect(enemy_team(cur_node), msg_format("is %s", fx_names[2]));
}

void far spell_cure_light(void)                                  /* 36e3:00fd */
{
    if (target_count != 0) {
        if (fx_heal(targets[1], fx_roll_dice(1, 8), 0))
            heal_msg(targets[1]);
    }
}

void far spell_cause_light(void)                                 /* 36e3:0139 */
{
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(1, 8), 8, "");
}

void far fx_affected(void)                                       /* 36e3:0163 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is affected");
}

void far fx_protected(void)                                      /* 36e3:0181 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is protected");
}

void far spell_resist_cold(void)                                 /* 36e3:019f */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is cold-resistant");
}

void far spell_burning_hands(void)                               /* 36e3:01bd */
{
    if (cursor_obj[0x3c])
        bottom_msg("That has no effect underwater!");
    else
        spell_damage_apply(current_spell, 0, 0, caster_level(current_spell, 0), 9, "");
}

void far spell_charm_person(void)                                /* 36e3:0201 */
{
    struct Effect far *fx;
    struct Hero far *h;

    h = targets[1];
    if (h != 0) {
        if ((h->race == 6 && !(h->type_flags & 2)) || h->icon_dim > 1) {
            hero_text(h, "is unaffected", 10, 1);
        } else {
            spell_damage_apply(current_spell, caster_level(current_spell, 1) + (cur_node->side << 7),
                            1, 0, 0, "is charmed");
            if (find_effect(h, 0xb, &fx))
                fx_dispatch(0xb, h, fx, 0);
        }
    }
}

void far spell_enlarge(void)                                     /* 36e3:02bf */
{
    char n;
    struct Hero far *h;
    struct Effect far *fx;

    h = targets[1];
    fx_pool = 0x12;
    fx_cost = 0;
    switch (caster_level(current_spell, 0)) {
    case 1: fx_cost = 0; break;
    case 2: fx_cost = 1; break;
    case 3: fx_cost = 0x33; break;
    case 4: fx_cost = 0x4c; break;
    case 5: fx_cost = 0x5b; break;
    case 6: fx_cost = 0x64; break;
    case 7: fx_pool = 0x13; break;
    case 8: fx_pool = 0x14; break;
    case 9: fx_pool = 0x15; break;
    default: fx_pool = 0x16; break;
    }
    if (fx_strength_upgrade(h, fx_pool, fx_cost, &n) && !find_effect(h, 0x39, &fx) &&
        !find_effect(h, 0x3f, &fx)) {
        hero_text(h, "is stronger", 10, 1);
        fx_add(h, 0xc, spell_duration(current_spell), n, 1);
        fx_stat_recalc(h, 0);
    } else {
        hero_text(h, "is unaffected", 10, 1);
    }
}

void far spell_reduce(void)                                      /* 36e3:040f */
{
    struct Effect far *fx;
    struct Hero far *h;

    h = targets[1];
    if (h != 0) {
        if (target_count > 0 && !fx_saving_throw(h, 4, 0) && find_effect(h, 0xc, &fx)) {
            fx_remove(h, 0xc, 0);
            fx_stat_recalc(h, 0);
            hero_text(h, "has been reduced", 10, 1);
        }
    }
}

void far spell_friends(void)                                     /* 36e3:04a4 */
{
    spell_damage_apply(current_spell, fx_roll_dice(2, 4), 1, 0, 0, "is friendly");
    fx_stat_recalc(cur_node, 5);
}

void far spell_magic_missile(void)                               /* 36e3:04e1 */
{
    unsigned char n;

    n = caster_level(current_spell, 0) + 1;
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(n / 2, 4) + n / 2, 8, "");
}

void far spell_shield(void)                                      /* 36e3:0541 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is shielded");
}

void far spell_shocking_grasp(void)                              /* 36e3:055f */
{
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(1, 8) + caster_level(current_spell, 0), 0xc, "");
}

void far spell_sleep(void)                                       /* 36e3:059b */
{
    unsigned char i;
    struct Effect far *fx;

    mirror_image_bypass = 1;
    fx_pool = fx_roll_dice(4, 4);
    for (i = 1; i <= target_count; i++) {
        switch (targets[i]->hit_dice) {
        case 0:
        case 1: fx_cost = 1; break;
        case 2: fx_cost = 2; break;
        case 3: fx_cost = 4; break;
        case 4: fx_cost = 6; break;
        case 5:
            if (targets[i]->race == 6) {
                fx_cost = 10;
                break;
            }
        default:
            fx_cost = 0x14;
        }
        if (targets[i]->status != 1 && !find_effect(targets[i], 0x35, &fx) &&
            fx_pool >= fx_cost)
            fx_pool -= fx_cost;
        else
            targets[i] = 0;
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, "falls asleep");
}

void far spell_hold(void)                                        /* 36e3:06c1 */
{
    char n;

    switch (target_count) {
    case 1:
        if (current_spell == 0x17)
            n = -2;
        else
            n = -3;
        break;
    case 2:
        n = -1;
        break;
    case 3:
    case 4:
        n = 0;
        break;
    }                                           /* n is unset for other counts */
    spell_effect_apply(n, "is held");
}

void far spell_resist_fire(void)                                 /* 36e3:0714 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is fire resistant");
}

void far spell_silence(void)                                     /* 36e3:0732 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is silenced");
}

void far spell_slow_poison(void)                                 /* 36e3:0750 */
{
    struct Effect far *fx;
    struct Hero far *h;

    h = targets[1];
    if (h->status == 1) {
        targets[1] = 0;
    } else if (find_effect(h, 0x37, &fx)) {
        if (h->hp == 0)
            h->hp = 1;
        h->status = 0;
        h->active = 1;
        spell_damage_apply(current_spell, -1, 1, 0, 0, "is affected");
        fx_add(h, 0xf, 10, -1, 1);
    }
}

void far spell_snake_charm(void)                                 /* 36e3:07ea */
{
    struct Hero far *h;
    struct Effect far *fx;

    fx_pool = cur_node->hp;
    target_count = 0;
    for (h = node_list; h != 0; h = h->next) {
        if ((h->type_flags & 0x10) && h->active && !find_effect(h, 0x33, &fx) &&
            h->hp <= fx_pool) {
            fx_pool = fx_pool - h->hp;
            target_count++;
            targets[target_count] = h;
        }
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, msg_format("is %s", fx_names[0xb]));
}

void far spell_spirit_hammer(void)                               /* 36e3:08ba */
{
    spell_damage_apply(current_spell, 0, 1, 0, 0, "");
    fx_dispatch(0x17, targets[1], 0, 0);
}

void far spell_invisibility(void)                                /* 36e3:08f2 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is invisible");
}

void far spell_knock(void)                                       /* 36e3:0910 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "Knock-Knock");
}

void far spell_mirror_image(void)                                /* 36e3:092e */
{
    char n;

    n = fx_roll_dice(1, 4) << 5;
    n = caster_level(current_spell, 1) | n;
    spell_damage_apply(current_spell, n, 0, 0, 0, "is duplicated");
}

void far spell_enfeeble(void)                                    /* 36e3:097d */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is weakened");
}

void far spell_cloud(void)                                       /* 36e3:099b */
{
    short x;
    short y;
    struct Patch far *c;
    unsigned char dir;
    unsigned char count;
    unsigned char tile;
    unsigned char k;
    unsigned char i;
    unsigned char who;
    int seen[9];

    mem_zero(seen, 0x12);
    pool_alloc((void far * far *)&c, 0x21, &patch_pool);
    mem_zero(c, 0x21);
    c->x = view_cx;
    c->y = view_cy;
    c->caster = cur_node;
    c->level = caster_level(current_spell, 1);
    c->strength = c->level;
    switch (current_spell) {
    case 0x22:
        count = 4;
        c->kind = 0x1d;
        break;
    case 0x5b:
        count = 9;
        c->kind = 0x1c;
        break;
    case 0x65:
        count = 4;
        c->kind = 0x1b;
        c->strength = c->strength * 3;
        break;
    }
    for (i = 1; i <= count; i++) {
        dir = patch_dir[i];
        x = c->x + dir_dx[dir];
        y = c->y + dir_dy[dir];
        if (x >= 0 && x <= 0x31 && y >= 0 && y <= 0x18) {
            cell_get(x, y, &who, &tile);
            for (k = 1; k <= i - 1; k++)
                if (seen[k - 1] == who)
                    who = 0;
            seen[i - 1] = who;
            tile = combat_terrain_under(x, y);
            c->active[i] = tile > 0 && (tile_info - 1)[tile].move_cost < 0xff;
            c->terrain[i] = tile;
            if (c->active[i])
                cur_map->cell[y][x] = c->kind;
        } else {
            c->active[i] = 0;
        }
    }
    c->next = patch_list;
    patch_list = c;
    combat_view_scroll(view_cx, view_cy, -1, 8);
    for (i = 1; i <= count; i++)
        if (seen[i - 1] > 0)
            fx_terrain_apply((heroes - 1)[seen[i - 1]], 1, 0);
}

void far spell_strength(void)                                    /* 36e3:0c5a */
{
    struct Hero far *h;
    unsigned char pct;
    unsigned char roll;
    unsigned char str;
    char out;

    pct = 0;
    h = targets[1];
    if (h->level[5] > 0)
        roll = fx_roll_dice(1, 4);
    if (h->level[0] > 0 || h->level[6] > 0)
        roll = fx_roll_dice(1, 6);
    if (h->level[2] > 0 || h->level[3] > 0 || h->level[1] > 0 || h->level[4] > 0)
        roll = fx_roll_dice(1, 8);
    str = h->str + roll;                        /* roll unset if no test passed */
    if (str > 0x12) {
        if (h->level[2] > 0 || h->level[3] > 0 || h->level[1] > 0 || h->level[4] > 0) {
            pct = h->str_ex + (str - 0x12) * 10;
            if (pct > 0x64)
                pct = 0x64;
        }
        str = 0x12;
    }
    if (fx_strength_upgrade(h, str, pct, &out)) {
        out = roll + 0x64;
        fx_add(h, 0x26, spell_duration(current_spell), out, 1);
        fx_stat_recalc(h, 0);
    }
}

char far disease_cure(void)                                      /* 36e3:0d96 */
{
    char done;

    done = 0;
    fx_lock = 1;
    if (fx_cure(targets[1], 0x22))
        done = 1;
    if (fx_cure(targets[1], 0x2b)) {
        done = 1;
        fx_remove(targets[1], 0x2c, 0);
        fx_remove(targets[1], 0x1f, 0);
        fx_stat_recalc(targets[1], 0);
    }
    if (fx_cure(targets[1], 0xd7)) {
        fx_remove(targets[1], 0xac, 0);
        done = 1;
    }
    fx_lock = 0;
    return done;
}

void far spell_heal(void)                                        /* 36e3:0e5a */
{
    struct Hero far *h;
    unsigned char n;

    h = targets[1];
    if (disease_cure())
        hero_text(h, "is cured", 10, 0);
    if (fx_cure(h, 0x21))
        hero_text(h, "is no longer blind", 10, 0);
    if (fx_cure(h, 0x44)) {
        hero_text(h, "regains intellect", 10, 0);
        fx_stat_recalc(h, 1);
        fx_stat_recalc(h, 2);
    }
    n = fx_roll_dice(1, 4);
    if (h->hp + n > h->hp_max)
        n = 0;
    else
        n = h->hp_max - h->hp - n;
    if (fx_heal(h, n, 0))
        hero_text(h, "is Healed", 10, 0);
}

void far spell_cure_blindness(void)                              /* 36e3:0f85 */
{
    if (fx_cure(targets[1], 0x21))
        hit_flash(targets[1], 1, "can see");
}

void far spell_cause_blindness(void)                             /* 36e3:0fb5 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is blind");
}

void far spell_cure_disease(void)                                /* 36e3:0fd3 */
{
    if (disease_cure())
        hero_text(targets[1], "is cured", 10, 0);
}

void far spell_cause_disease(void)                               /* 36e3:0ff7 */
{
    struct Effect far *fx;

    spell_damage_apply(current_spell, 0, 1, 0, 0, msg_format("is %s", fx_names[0x22]));
    if (find_effect(targets[1], 0x22, &fx))
        fx_dispatch(0x22, targets[1], fx, 0);
}

char far dispel_roll(unsigned char level, unsigned char far *caster)
                                                                 /* 36e3:1068 */
{
    char ok;

    if (*caster > level)
        fx_pool = (*caster - level) * 5 + 0x32;
    else if (*caster < level)
        fx_pool = 0x32 - (fx_cost - *caster) * 2;     /* fx_cost, not level: as shipped */
    else
        fx_pool = 0x32;
    ok = fx_roll_dice(1, 100) <= fx_pool;
    return ok;
}

void far spell_dispel_magic(void)                                /* 36e3:10dd */
{
    unsigned char i;
    unsigned char lvl;
    struct Hero far *h;
    char any;
    struct Effect far *fx;
    struct Effect far *next;
    char x;
    char y;
    unsigned char tile;
    unsigned char count;
    struct Patch far *c;

    mirror_image_bypass = 1;
    lvl = caster_level(current_spell, 0);
    for (i = 1; i <= target_count; i++) {
        any = 0;
        h = targets[i];
        for (fx = h->effects; fx != 0; fx = next) {
            next = fx->next;
            if (fx->strength < 0xff) {
                fx_cost = fx->strength & 0x1f;
                if (lvl > fx_cost)
                    fx_pool = (lvl - fx_cost) * 5 + 0x32;
                else if (lvl < fx_cost)
                    fx_pool = 0x32 - (fx_cost - lvl) * 2;
                else
                    fx_pool = 0x32;
                if (fx_roll_dice(1, 100) <= fx_pool) {
                    fx_remove(h, fx->kind, fx);
                    any = 1;
                }
            }
        }
        if (any)
            hit_flash(h, 1, "is affected");
    }
    if (view_mode == 5) {
        for (tick_slot = 0; tick_slot <= 8; tick_slot++) {
            x = dir_dx[tick_slot] + view_cx;
            y = dir_dy[tick_slot] + view_cy;
            cell_get(x, y, &count, &tile);
            if (tile == 0x1d || tile == 0x1c) {
                c = patch_list;
                if (tile == 0x1d)
                    count = 4;
                else
                    count = 9;
                for (; c != 0; c = c->next) {
                    for (i = 1; i <= count; i++)
                        if (c->x + dir_dx[patch_dir[i]] == x &&
                            c->y + dir_dy[patch_dir[i]] == y &&
                            dispel_roll(c->level, &lvl))
                            c->strength = 0xff;
                }
                combat_view_scroll(view_cx, view_cy, 3, 8);
            }
        }
        fx_clouds_tick(1);
    }
}

void far spell_prayer(void)                                      /* 36e3:1346 */
{
    spell_damage_apply(current_spell, caster_level(current_spell, 1) + (cur_node->side << 7),
                    0, 0, 0, "is praying");
}

void far spell_remove_curse(void)                                /* 36e3:137e */
{
    struct Item far *it;
    char found;
    short k;

    if (fx_cure(targets[1], 0x2b)) {
        hit_flash(targets[1], 1, "is un-cursed");
    } else {
        found = 0;
        for (it = targets[1]->items; it != 0 && !found; it = it->next) {
            if (it->cursed && it->readied) {
                found = 1;
                it->readied = 0;
                if (it->slots[3] & 0x80) {
                    item_handler_active = 1;
                    fx_dispatch(0x7f, targets[1], (struct Effect far *)it, 1);
                    for (k = 0; k <= 5; k++)
                        fx_stat_recalc(targets[1], k);
                }
            }
        }
        if (found)
            hit_flash(targets[1], 1, "has an item un-cursed");
    }
}

void far spell_bestow_curse(void)                                /* 36e3:146c */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "has been cursed!");
}

void far spell_blink(void)                                       /* 36e3:148a */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is blinking");
}

void far spell_fireball(void)                                    /* 36e3:14a8 */
{
    unsigned char dice;
    char bonus;
    unsigned char i;

    if (cursor_obj[0x3c] && current_spell != 0x33) {
        bottom_msg("That has no effect underwater!");
    } else {
        mirror_image_bypass = 1;
        if (current_spell == 0x85)
            dice = fx_roll_dice(1, 3) * 2 + 1;
        else
            dice = caster_level(current_spell, 0);
        if (cursor_obj[0x3c]) {
            plan_steps(view_cx, view_cy, 2, -1, 1);
            for (i = 1; i <= step_count; i++)
                targets[i] = (heroes - 1)[(steps - 1)[i].who];
            target_count = step_count;
        }
        combat_view_scroll(view_cx, view_cy, 0, 8);
        if (current_spell == 0x73)
            bonus = dice;
        else
            bonus = 0;
        spell_damage_apply(current_spell, 0, 0, fx_roll_damage(dice, 6) + bonus, 9, "");
    }
}

void far side_apply(char kind, int id, char far *msg)            /* 36e3:15ce */
{
    unsigned char left;
    unsigned char i;

    mirror_image_bypass = 1;
    left = caster_level(current_spell, 0);
    for (i = 1; i <= target_count; i++) {
        if (targets[i]->side != id || left <= 0) {
            targets[i] = 0;
        } else {
            effect_id = kind;
            element_mask = 8;
            fx_trigger_group(9, &targets[i]);
            if (effect_id == kind) {
                left--;
                if (fx_cure(targets[i], kind))
                    targets[i] = 0;
            }
        }
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, msg);
    for (i = 1; i <= target_count; i++)
        if (targets[i] != 0)
            fx_trigger_group(0x12, &targets[i]);
}

void far spell_haste(void)                                       /* 36e3:16eb */
{
    side_apply(0x2a, cur_node->side, "is Hasted");
}

void far bolt_hit(int x, int y, int dmg);

void far bolt_run(unsigned range, int dmg, char bounce)          /* 36e3:1706 */
{
    unsigned char spent;
    unsigned char who_was;
    unsigned char tile;
    unsigned char who;
    char dir;
    char px;
    char py;
    short prev_x;
    short prev_y;
    short tx;
    short ty;
    struct LineWalk back;
    struct LineWalk line;
    char moved;
    char stopped;
    register int cx;
    register int cy;

    icon_facings(0x14);
    cx = combatant_x(cur_node);
    cy = combatant_y(cur_node);
    tx = view_cx;
    ty = view_cy;
    bolt_hit(tx, ty, dmg);
    cell_get(tx, ty, &who_was, &tile);
    range = range * 2;
    mirror_image_bypass = 1;
    spent = 0;
    dir = 1;
    while (range > 0) {
        mem_zero(&line, 0x18);
        line.x0 = tx;
        line.y0 = ty;
        line.x1 = (tx - cx) * dir * range + tx;
        line.y1 = (ty - cy) * dir * range + ty;
        line_init(&line);
        stopped = 0;
        do {
            px = line.x;
            py = line.y;
            if (line.x0 != line.x1 || line.y0 != line.y1) {
                do {
                    prev_x = line.x;
                    prev_y = line.y;
                    moved = line_step(&line);
                    cell_get(line.x, line.y, &who, &tile);
                } while (moved && (who <= 0 || who == who_was) && tile != 0 &&
                         (tile_info - 1)[tile].move_cost <= 1 && line.steps < range);
            }
            if (tile == 0)
                range = 0;
            if (line.x < 0 || line.x > 0x31 || line.y < 0 || line.y > 0x18) {
                stopped = 1;
                line.x = prev_x;
                line.y = prev_y;
            }
            icon_fly(px, py, line.x, line.y, 4, 0x32);
            if (who_was != who)
                bolt_hit(line.x, line.y, dmg);
            who_was = who;
            if ((tile_info - 1)[tile].move_cost > 1 && cursor_obj[0x22] == 1 && !stopped) {
                tx = line.x;
                ty = line.y;
                dir = -1;
                mem_zero(&back, 0x18);
                back.x0 = tx;
                back.y0 = ty;
                back.x1 = cx;
                back.y1 = cy;
                line_init(&back);
                while (line_step(&back))
                    ;
                if (bounce && back.steps <= 8)
                    line.steps = line.steps * 2;
                bounce = 0;
            }
            spent = line.steps - spent;
            if (spent < 2)
                spent = 2;
            if (spent < range)
                range -= spent;
            else
                range = 0;
            spent = line.steps;
        } while (range != 0 &&
                 ((tile_info - 1)[tile].move_cost <= 1 || cursor_obj[0x22] != 1));
    }
    mirror_image_bypass = 0;
}

void far bolt_hit(int x, int y, int dmg)                         /* 36e3:19e8 */
{
    unsigned char who;
    unsigned char tile;
    struct Hero far *h;

    cell_get(x, y, &who, &tile);
    if (who > 0) {
        h = (heroes - 1)[who];
        fx_damage(h, dmg, 2, fx_saving_throw(h, 4, 0));
        icon_facings(0x14);
    }
}

void far spell_lightning_bolt(void)                              /* 36e3:1a57 */
{
    short dmg;

    if (cursor_obj[0x3c]) {
        spell_fireball();
    } else {
        element_mask = 0xc;
        dmg = fx_roll_damage(caster_level(current_spell, 0), 6);
        bolt_run(7, dmg, 1);
    }
}

void far spell_slow(void)                                        /* 36e3:1aa5 */
{
    side_apply(0x27, enemy_team(cur_node), "is Slowed");
}

void far spell_harm(void)                                        /* 36e3:1ac5 */
{
    struct Hero far *h;

    h = targets[1];
    element_mask = 8;
    fx_add_message(h, 0x22, 0, 0, 0, 0, 0, "");
    hero_text(h, msg_format("is %s", fx_names[0x22]), 10, 1);
    if (h->hp > 4)
        fx_damage(h, h->hp - fx_roll_dice(1, 4), 0, 0);
}

void far potion_speed(void)                                      /* 36e3:1b62 */
{
    if (!fx_cure(targets[1], 0x2a))
        spell_damage_apply(current_spell, 0, 0, 0, 0, "is Speedy");
}

void far spell_cure_serious(void)                                /* 36e3:1b97 */
{
    if (target_count != 0) {
        if (fx_heal(targets[1], fx_roll_dice(2, 8) + 1, 0))
            heal_msg(targets[1]);
    }
}

void far potion_frost(void)                                      /* 36e3:1bd5 */
{
    char n;

    if (fx_strength_upgrade(targets[1], 0x15, 0, &n))
        hero_text(targets[1], "is stronger", 10, 1);
    fx_add(targets[1], 0x71, fx_roll_dice(1, 4) * 10 + 0x28, n, 1);
    fx_stat_recalc(targets[1], 0);
}

void far javelin_lightning(void)                                 /* 36e3:1c5b */
{
    element_mask = 0xc;
    bolt_run(3, 0x14, 0);
}

void far wand_paralysis(void)                                    /* 36e3:1c74 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is paralyzed");
}

void far potion_healing(void)                                    /* 36e3:1c92 */
{
    if (fx_heal(targets[1], fx_roll_dice(2, 4) + 2, 0))
        hit_flash(targets[1], 1, "is Healed");
}

void far elixir_youth(void)                                      /* 36e3:1cd2 */
{
    struct Hero far *h;

    h = cur_node;
    h->age = h->age - (fx_roll_dice(1, 4) + 1);
    if (h->age < 0x12) {
        h->age = 0x12;
        hero_text(h, "is now 18 years old", 10, 1);
    } else {
        hero_text(h, "becomes younger", 10, 1);
    }
}

void far wand_missiles(void)                                     /* 36e3:1d40 */
{
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(2, 4) + 2, 8, "");
}

void far spell_cause_serious(void)                               /* 36e3:1d6c */
{
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(2, 8) + 1, 8, "");
}

void far spell_neutralize(void)                                  /* 36e3:1d98 */
{
    struct Hero far *h;
    struct Effect far *fx;

    h = targets[1];
    if (h->status == 1) {
        targets[1] = 0;
    } else if (find_effect(h, 0x37, &fx)) {
        if (h->hp == 0)
            h->hp = 1;
        fx_lock = 1;
        fx_remove(h, 0x37, 0);
        fx_remove(h, 0x16, 0);
        fx_remove(h, 0xf, 0);
        fx_lock = 0;
        hero_text(h, "is unpoisoned", 10, 1);
        h->active = 1;
        h->status = 0;
    } else {
        hero_text(h, "is unaffected", 10, 1);
    }
}

void far spell_poison(void)                                      /* 36e3:1e7b */
{
    struct Effect far *fx;
    struct Hero far *t;

    if (cur_node->fight != 0) {
        spell_damage_apply(current_spell, 0, 0, 0, 8, "");
        t = cur_node->fight->target;
        effect_id = 0x40;
        fx_trigger_group(9, &t);
        fx = 0;
        if (effect_id == 0x40)
            fx_dispatch(0x40, cur_node, fx, 0);
    }
}

void far spell_sticks_snakes(void)                               /* 36e3:1f05 */
{
    struct Effect far *fx;

    if (targets[1]->hit_dice < 6) {
        spell_damage_apply(current_spell, caster_level(current_spell, 1), 0, 0, 8, "");
        if (find_effect(targets[1], 3, &fx))
            fx_dispatch(3, targets[1], fx, 0);
    } else if (!is_helpless(targets[1])) {
        hero_text(targets[1], "smashes them flat", 10, 1);
    }
}

void far spell_cure_critical(void)                               /* 36e3:1fad */
{
    if (target_count != 0) {
        if (fx_heal(targets[1], fx_roll_dice(3, 8) + 3, 0))
            heal_msg(targets[1]);
    }
}

void far spell_cause_critical(void)                              /* 36e3:1feb */
{
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(3, 8) + 3, 8, "");
}

void far spell_dispel_evil(void)                                 /* 36e3:2017 */
{
    fx_add_message(cur_node, 4, spell_duration(0x49), 0, 0, 0, 0, "");
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is affected");
}

void far spell_flame_strike(void)                                /* 36e3:2063 */
{
    if (cursor_obj[0x3c]) {
        bottom_msg("That has no effect underwater!");
    } else {
        mirror_image_bypass = 1;
        spell_damage_apply(current_spell, 0, 0, fx_roll_damage(6, 8), 9, "");
    }
}

void far spell_raise_dead(void)                                  /* 36e3:20ab */
{
    struct Hero far *h;

    h = targets[1];
    if ((h->status == 6 || h->status == 1) && h->con_base > 0 && h->race != 0) {
        fx_lock = 1;
        fx_remove(h, 0x37, 0);
        fx_lock = 0;
        h->status = 0;
        h->active = 1;
        h->con_base--;
        fx_stat_recalc(h, 4);
        h->hp = 1;
        hero_text(h, "is raised", 10, 1);
    }
}

void far spell_slay_living(void)                                 /* 36e3:213d */
{
    struct Hero far *h;

    h = targets[1];
    element_mask = 0x40;
    damage = 0x43;
    fx_trigger_group(9, &h);
    if (damage != 0) {
        if (!fx_saving_throw(h, 4, 0) && !(h->immunity & 8)) {
            fx_disable(h, 6, "is slain");
        } else {
            element_mask = 8;
            fx_damage(h, fx_roll_damage(2, 8) + 1, 0, 0);
        }
    } else {
        hero_text(h, "is unaffected", 10, 1);
    }
}

void far spell_entangle(void)                                    /* 36e3:21f4 */
{
    struct Hero far *h;
    unsigned char i;
    char save;

    mirror_image_bypass = 1;
    if (cursor_obj[0x22] == 0) {
        for (i = 1; i <= target_count; i++) {
            if (targets[i] != 0) {
                h = targets[i];
                save = fx_saving_throw(h, 4, 0);
                fx_add_message(h, 0x1b, spell_duration(0x4e), 0, 0, 1, save, "is entangled");
            }
        }
    }
    mirror_image_bypass = 0;
}

void far spell_faerie_fire(void)                                 /* 36e3:2295 */
{
    spell_effect_apply(0, "is highlighted");
}

void far spell_invis_animals(void)                               /* 36e3:22a6 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is invisible");
}

void far spell_charm_monsters(void)                              /* 36e3:22c4 */
{
    unsigned char i;
    struct Effect far *fx;

    spell_effect_apply(0, "is charmed");
    for (i = 1; i <= target_count; i++)
        if (find_effect(targets[i], 0xb, &fx))
            fx_dispatch(0xb, targets[i], fx, 0);
}

void far spell_confusion(void)                                   /* 36e3:233e */
{
    unsigned char i;
    char save;
    struct Hero far *h;
    unsigned char n;

    mirror_image_bypass = 1;
    n = fx_roll_dice(2, 8);
    if (target_count > n)
        target_count = n;
    for (i = 1; i <= target_count; i++) {
        if (targets[i] != 0) {
            h = targets[i];
            save = fx_saving_throw(h, 4, 0);
            fx_add_message(h, 0x23, spell_duration(0x52), 0, 0, 1, save, "is confused");
        }
    }
    mirror_image_bypass = 0;
}

void far spell_dimension_door(void)                              /* 36e3:23ef */
{
    struct Hero far *h;

    h = cur_node;
    tile_update(0, 0, combatant_slot(h));
    hero_place(h, view_cx, view_cy, 0);
    combat_view_scroll(combatant_x(h), combatant_y(h), 0, 8);
    hero_text(h, "teleports", 10, 1);
}

void far spell_fear(void)                                        /* 36e3:2479 */
{
    struct Hero far *caster;
    struct Hero far *t;
    unsigned char i;
    char save;
    struct Effect far *fx;

    mirror_image_bypass = 1;
    if (view_mode == 5) {
        caster = cur_node;
        spell_path_targets(combatant_x(caster), combatant_y(caster), view_cx, view_cy, 3, 6);
        for (i = 1; i <= target_count; i++) {
            t = targets[i];
            save = fx_saving_throw(t, 4, 0);
            if (save == 0) {
                fx_add_message(t, 0x6f, spell_duration(0x54), 0, 1, 1, save, "flees in terror");
                if (find_effect(t, 0x6f, &fx)) {
                    t->fight->panicked = 1;
                    t->quick_fight = 1;
                    if (!(t->control & 0x80))
                        t->control = 0xb3;
                    t->fight->target = 0;
                }
            } else {
                hero_text(t, "is unaffected", 10, 1);
            }
        }
        mirror_image_bypass = 0;
    }
}

void far spell_fire_shield(void)                                 /* 36e3:25b3 */
{
    unsigned char choice;
    char done;

    done = 0;
    do {
        if (view_mode == 5 && cur_node->quick_fight != 0) {
            choice = fx_roll_dice(1, 2) - 1;
        } else {
            hotkey_reset(1);
            choice = menu_run(txt_flame_type, txt_hot_cold, 0, 0);
        }
        switch (choice) {
        case 0:
            if (cursor_obj[0x3c]) {
                bottom_msg("That has no effect underwater!");
                return;
            }
            fx_add_message(cur_node, 0x32, spell_duration(0x55), 0, 0, 0, 0, "is Protected");
            fx_add_message(cur_node, 0x70, spell_duration(0x55), 0, 0, 0, 0, "");
            done = 1;
            break;
        case 1:
            fx_add_message(cur_node, 0x36, spell_duration(0x55), 0, 0, 0, 0, "is Protected");
            fx_add_message(cur_node, 0x70, spell_duration(0x55), 0, 0, 0, 0, "");
            done = 1;
            break;
        default:
            done = confirm(txt_abort_spell, 1);
        }
    } while (!done);
}

void far spell_fumble(void)                                      /* 36e3:26ee */
{
    struct Hero far *h;
    struct Effect far *fx;

    h = targets[1];
    element_mask = 0x40;
    if (!fx_saving_throw(h, 4, 0)) {
        fx_add_message(h, 0x1b, spell_duration(0x56), 0, 0, 0, 0, "is clumsy");
        if (find_effect(h, 0x1b, &fx))
            fx_dispatch(0x1b, h, 0, 0);
    } else {
        fx_add_message(h, 0x2a, spell_duration(0x56), 0, 0, 0, 0, "is slowed");
        if (find_effect(h, 0x2a, &fx))
            fx_dispatch(0x2a, h, 0, 0);
    }
}

void far spell_ice_storm(void)                                   /* 36e3:27dd */
{
    mirror_image_bypass = 1;
    if (cursor_obj[0x3c])
        spell_damage_apply(current_spell, 0, 0, fx_roll_damage(1, 10), 8, "");
    else
        spell_damage_apply(current_spell, 0, 0, fx_roll_damage(3, 10), 8, "");
}

void far spell_minor_globe(void)                                 /* 36e3:2842 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is protected");
}

void far spell_barkskin(void)                                    /* 36e3:2860 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is toughened");
}

void far spell_cone_of_cold(void)                                /* 36e3:287e */
{
    struct Hero far *h;
    unsigned char r;
    unsigned char dice;

    mirror_image_bypass = 1;
    h = cur_node;
    dice = caster_level(current_spell, 0);
    r = (caster_level(current_spell, 0) + 1) / 2;
    if (r < 1)
        r = 1;
    spell_path_targets(combatant_x(h), combatant_y(h), view_cx, view_cy, 2, r);
    spell_damage_apply(current_spell, 0, 0, fx_roll_damage(dice, 4) + dice, 10, "");
}

void far spell_feeblemind(void)                                  /* 36e3:292a */
{
    unsigned char old;
    unsigned char i;
    struct Hero far *h;
    struct Effect far *fx;

    h = targets[1];
    old = h->save_spell;
    switch (h->cls) {
    case 0:
        h->save_spell--;
        break;
    case 5:
        h->save_spell += 4;
        break;
    default:
        h->save_spell += 2;
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, "");
    if (find_effect(h, 0x44, &fx)) {
        for (i = 0; i <= 0x8c; i++)
            if (h->spells[i] & 0x80)
                h->spells[i] = 0;
        fx_dispatch(0x44, h, 0, 0);
    }
    h->save_spell = old;
}

void far scroll_protection(void)                                 /* 36e3:2a00 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "");
}

void far spell_charm_mammal(void)                                /* 36e3:2a1e */
{
    struct Effect far *fx;
    struct Hero far *h;

    h = targets[1];
    if (h != 0) {
        if (h->race < 6 || (h->type_flags & 2)) {
            spell_damage_apply(current_spell, caster_level(current_spell, 1) + (cur_node->side << 7),
                            1, 0, 0, msg_format("is %s", fx_names[0xb]));
            if (find_effect(h, 0xb, &fx))
                fx_dispatch(0xb, h, fx, 0);
        } else {
            hero_text(h, msg_format("is not %s", fx_names[0xb]), 10, 1);
        }
    }
}

void far potion_invisibility(void)                               /* 36e3:2af8 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "");
}

void far spell_cure_light_98(void)                               /* 36e3:2b16 */
{
    if (target_count != 0) {
        if (fx_heal(targets[1], fx_roll_dice(1, 8), 0))
            heal_msg(targets[1]);
    }
}

void far potion_extra_healing(void)                              /* 36e3:2b52 */
{
    if (fx_heal(targets[1], fx_roll_dice(2, 4) + 2, 0))
        hit_flash(targets[1], 1, "is Healed");
}

void far spell_bestow_curse_100(void)                            /* 36e3:2b92 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "");
}

void far spell_restoration(void)                                 /* 36e3:2bb0 */
{
    short best;
    short k;
    long best_xp;
    struct Hero far *h;
    unsigned char diff;
    unsigned char gain;
    unsigned char total;
    char cut;
    unsigned char count;

    total = 0;
    h = targets[1];
    best = 0x11;
    best_xp = 0x7fffffffL;
    count = 0;
    for (k = 0; k <= 6; k++)
        if (h->level_max[k] > 0)
            count++;
    for (k = 0; k <= 6; k++) {
        if (h->level[k] < h->level_max[k]) {
            diff = h->level_max[k] - h->level[k];
            total += diff;
            if (level_xp_threshold(h, h->level[k] + 1, k) <= best_xp) {
                best_xp = level_xp_threshold(h, h->level[k] + 1, k);
                best = k;
            }
        }
    }
    if (best != 0x11) {
        if (h->hp_max < h->hp_max_old) {
            gain = (h->hp_max_old - h->hp_max) / total;
            if (h->level[best] + 1 < hit_dice_level_limit[best])
                cut = con_hp_adj(h, best) / count;
            else
                cut = 0;
            h->hp += gain;
            h->hp_base += gain - cut;
            h->hp_max += gain;
        }
        h->level[best]++;
        class_update(h);
        if (total == 1 && h->xp < h->xp_old)
            h->xp = h->xp_old;
        else
            h->xp = best_xp;
    }
    h->level_up = can_level_up(h);
}

void far spell_energy_drain(void)                                /* 36e3:2da0 */
{
    cur_node->fight->target = targets[1];
    fx_dispatch(0x65, cur_node, 0, 0);
}

void far spell_destruction(void)                                 /* 36e3:2dd2 */
{
    struct Hero far *h;

    h = targets[1];
    element_mask = 0x48;
    damage = 0xff;
    fx_trigger_group(9, &h);
    if (!(h->immunity & 8) && damage != 0)
        fx_disable(h, 8, "is destroyed");
    else
        hero_text(h, "is unaffected", 10, 1);
}

void far spell_resurrection(void)                                /* 36e3:2e44 */
{
    struct Hero far *h;

    h = targets[1];
    if (h->status == 6 && h->con_base > 0 && h->race > 0) {
        fx_remove(h, 0x37, 0);
        h->status = 0;
        h->active = 1;
        h->hp = h->hp_max;
        hero_text(h, "is resurrected", 10, 1);
    }
}

void far spell_hold_animal(void)                                 /* 36e3:2eaf */
{
    char n;

    switch (target_count) {
    case 1: n = -4; break;
    case 2: n = -2; break;
    case 3: n = -1; break;
    default: n = 0;
    }
    spell_effect_apply(n, "is held");
}

void far spell_prot_fire(void)                                   /* 36e3:2ef5 */
{
    struct Hero far *h;

    h = targets[1];
    if (h == 0)
        return;
    if (class_level(h, 4) > 0)
        fx_add_message(h, 0x77, 0, caster_level(0x6d, 0), 0, 0, 0, "is Protected");
    else
        fx_add_message(h, 0x14, spell_duration(0x6d), class_level(h, 4), 0, 0, 0,
                     "is Protected");
}

void far spell_death(void)                                       /* 36e3:2f95 */
{
    struct Hero far *h;
    unsigned char i;

    mirror_image_bypass = 1;
    element_mask = 0x40;
    fx_pool = fx_roll_dice(4, 0x14) * 2;
    for (i = 1; i <= target_count; i++) {
        h = targets[i];
        switch (h->hit_dice) {
        case 0: fx_cost = 1; break;
        case 1: fx_cost = 2; break;
        case 2:
        case 3: fx_cost = 8; break;
        case 6:
            if (h->hp_max > 0x33) {
                fx_cost = 0x14;
                break;
            }
        case 4:
        case 5: fx_cost = 0x10; break;
        case 7:
        case 8: fx_cost = 0x14; break;
        default: fx_cost = 0xa2;
        }
        damage = 0x64;
        fx_trigger_group(9, &h);
        if (!(h->immunity & 8) && fx_pool >= fx_cost && damage != 0) {
            fx_pool -= fx_cost;
            fx_disable(h, 6, "drops dead");
        } else {
            targets[i] = 0;
        }
    }
}

void far spell_disintegrate(void)                                /* 36e3:30a7 */
{
    char save;
    struct Hero far *h;

    element_mask = 0x48;
    h = targets[1];
    damage = 0xff;
    fx_trigger_group(9, &h);
    if (damage != 0) {
        save = fx_saving_throw(h, 4, 0);
        if (save == 0) {
            fx_disable(h, 6, "disintegrates");
            while (h->items != 0)
                item_remove(h, h->items);
            return;
        }
    }
    hero_text(h, "is unaffected", 10, 1);
}

void far spell_globe(void)                                       /* 36e3:3154 */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is protected");
}

void far spell_stone_to_flesh(void)                              /* 36e3:3172 */
{
    struct Hero far *h;
    unsigned char i;
    unsigned char k;

    h = targets[1];
    if (h->status == 7) {
        h->status = 0;
        h->active = 1;
        h->hp = 1;
        hero_text(h, "is no longer a statue", 10, 0);
        if (view_mode == 5) {
            for (i = 1; (recs - 1)[i].hero != h; i++)
                ;
            cur_map->cell[(recs - 1)[i].y][(recs - 1)[i].x] = (recs - 1)[i].terrain;
            for (k = i; k <= rec_count - 1; k++)
                (recs - 1)[k] = (recs - 1)[k + 1];
            rec_count--;
            if (hero_place(h, combatant_x(h), combatant_y(h), 1))
                combat_view_scroll(view_cx, view_cy, 0, 8);
        }
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, "");
}

void far spell_flesh_to_stone(void)                              /* 36e3:32fb */
{
    char save;
    struct Hero far *h;

    element_mask = 0x40;
    h = targets[1];
    damage = 0xff;
    fx_trigger_group(9, &h);
    if (damage != 0) {
        save = fx_saving_throw(h, 4, 0);
        if (save == 0)
            fx_disable(h, 7, "is turned to stone");
    }
}

void far spell_mass_invis(void)                                  /* 36e3:3367 */
{
    unsigned char i;

    mirror_image_bypass = 1;
    for (i = 1; i <= target_count; i++) {
        if (targets[i]->side != cur_node->side ||
            (view_mode == 5 && combat_targets_near(targets[i], 1, 0) > 0))
            targets[i] = 0;
    }
    spell_damage_apply(current_spell, 0, 0, 0, 0, msg_format("is %s", fx_names[0x19]));
}

void far spell_word_stun(void)                                   /* 36e3:3417 */
{
    unsigned char n;
    struct Hero far *h;

    mirror_image_bypass = 1;
    h = targets[1];
    switch (h->hp) {
        case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
        case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
        case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
        case 26: case 27: case 28: case 29: case 30:
        n = fx_roll_dice(4, 4);
        break;
        case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38:
        case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46:
        case 47: case 48: case 49: case 50: case 51: case 52: case 53: case 54:
        case 55: case 56: case 57: case 58: case 59: case 60:
        n = fx_roll_dice(2, 4);
        break;
        case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68:
        case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76:
        case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84:
        case 85: case 86: case 87: case 88: case 89: case 90:
        n = fx_roll_dice(1, 4);
        break;
    default:
        n = 0;
    }
    if (n != 0)
        fx_add_message(h, spell_info[0x75].effect, n, caster_level(0x75, 1), 0,
                       spell_info[0x75].save_effect, 0, msg_format("is %s", fx_names[0x6a]));
}

void far spell_fire_touch(void)                                  /* 36e3:3584 */
{
    if (cursor_obj[0x3c])
        bottom_msg("That has no effect underwater!");
    else
        spell_damage_apply(current_spell, 0, 0, 0, 0, "is covered in flame");
}

void far spell_iron_skin(void)                                   /* 36e3:35bb */
{
    spell_damage_apply(current_spell, 0, 0, 0, 0, "is protected");
}

void far spell_mass_charm(void)                                  /* 36e3:35d9 */
{
    struct Effect far *fx;
    struct Hero far *h;
    char done;
    unsigned char i;
    unsigned char lvl;
    unsigned char used;
    unsigned char cap;

    mirror_image_bypass = 1;
    done = 0;
    used = 0;
    i = 1;
    cap = caster_level(current_spell, 0) * 2;
    for (; i <= target_count && !done; i++) {
        h = targets[i];
        if (h == 0)
            continue;
        if (!(h->immunity & 2) || h->side == cur_node->side) {
            hero_text(h, "is unaffected", 10, 1);
            continue;
        }
        lvl = h->hit_dice;
        if (lvl + used > cap)
            continue;
        used += lvl;
        done = used == cap;
        fx_add_message(h, 0xb, spell_duration(current_spell),
                     caster_level(current_spell, 1) + (cur_node->side << 7), 1, 1,
                     fx_saving_throw(h, 4, -2), "is charmed");
        if (find_effect(h, 0xb, &fx))
            fx_dispatch(0xb, h, fx, 0);
    }
}

void far spell_dance(void)                                       /* 36e3:373a */
{
    struct Hero far *h;

    h = targets[1];
    recalc_stats(h);
    fx_trigger_group(0xb, &h);
    if (fx_attack_roll(cur_node, h, h->ac)) {
        fx_add_message(h, 0x1b, spell_duration(0x79), caster_level(0x79, 1), 0, 0, 0,
                     empty_text);
        fx_add_message(h, 0x76, spell_duration(0x79), caster_level(0x79, 1), 0, 0, 0,
                     "begins to dance wildly");
    }
}

void far spell_word_blind(void)                                  /* 36e3:37fb */
{
    struct Hero far *h;
    unsigned char n;
    unsigned char i;
    unsigned char sum;

    mirror_image_bypass = 1;
    for (i = 1, sum = 0; i <= target_count && sum <= 0x64; i++) {
        h = targets[i];
        if (h->hp_max <= 0x64 && h->hp_max + sum <= 0x64)
            sum += h->hp_max;
    }
    n = fx_roll_dice(1, 4) + 1;                 /* never read: as shipped */
    if (sum < 0x33)
        n = n * 10;
    for (i = 1, sum = 0; i <= target_count && sum <= 0x64; i++) {
        h = targets[i];
        if (h->hp_max <= 0x64 && h->hp_max + sum <= 0x64) {
            sum += h->hp_max;
            fx_add_message(h, 0x21, spell_duration(current_spell), 0, 0, 0, 0, "is blinded");
        }
    }
}

char far swarm_step(unsigned char mode, unsigned char dir, char far *cy, char far *cx,
                 char far *y0, char far *y1, char far *x0, char far *x1, unsigned far *power)
                                                                 /* 36e3:3904 */
{
    char r;
    char blocked;

    blocked = 0;
    *power = 0xff;
    switch (mode) {
    case 1:
        *x1 = diag_dx[dir - 1] + *x0;
        *y1 = diag_dy[dir - 1] + *y0;
        break;
    case 3:
        *x1 = diag_dx[dir - 1] + *x0;
        *y1 = *y0;
        break;
    case 2:
    case 4:
        *x1 = *x0;
    }
    if (*x1 < 0 || *x1 > 0x31 || *y1 < 0 || *y1 > 0x18)
        blocked = 1;
    if (blocked)
        r = 0;
    else
        r = line_clear(*cx, *cy, x1, y1, power);
    *power = *power / 2;
    return r;
}

void far spell_meteor_swarm(void)                                /* 36e3:3a07 */
{
    struct Hero far *h;
    char tx;
    char ty;
    char cx;
    char cy;
    char x;
    char y;
    unsigned char who;
    unsigned char before;
    unsigned char n;
    unsigned char k;
    unsigned power;
    char r;

    if (cursor_obj[0x3c]) {
        bottom_msg("That has no effect underwater!");
        return;
    }
    mirror_image_bypass = 1;
    element_mask = 9;
    h = cur_node;
    cx = combatant_x(h);
    cy = combatant_y(h);
    tx = view_cx;
    ty = view_cy;
    for (n = 1; n <= 4; n++) {
        mem_zero(targets, 4);               /* targets[0] only: as shipped */
        k = 1;
        do {
            r = swarm_step(k, n, &cy, &cx, &ty, &y, &tx, &x, &power);
            k++;
        } while (k <= 4 && r == 0);
        for (who = 0; !dir_ok(cx, cy, x, y, who); who++)
            ;
        move_flag = 1;
        combatant_facing_set(h, who, 1, 0);
        icon_facings(0x20);
        missile_facing(who, 0x20);
        icon_fly(cx, cy, x, y, 1, 0x1e);
        view_cx = x;
        view_cy = y;
        icon_scatter(1, 0x19, 0xc);
        spell_path_targets(cx, cy, x, y, 1, power);
        before = target_count;
        plan_steps(x, y, 1, -1, 1);
        for (k = 1; k <= step_count; k++) {
            target_count++;
            targets[target_count] = (heroes - 1)[(steps - 1)[k].who];
        }
        for (k = 1; k <= target_count; k++) {
            if (targets[k] == 0)
                continue;
            if (targets[k]->active == 0)
                continue;
            if (k <= before)
                fx_damage(targets[k], fx_roll_damage(10, 4), 0, 0);
            else
                fx_damage(targets[k], fx_roll_damage(10, 4), 2, fx_saving_throw(targets[k], 4, 0));
        }
        if (combatant_visible(h, 0)) {
            combatant_facing_set(h, h->fight->facing, 1, 1);
            combatant_facing_set(h, h->fight->facing, 0, 0);
        }
    }
}

void far spell_word_kill(void)                                   /* 36e3:3d14 */
{
    unsigned char i;

    mirror_image_bypass = 1;
    if (target_count > 0) {
        if (targets[1] != 0 && targets[1]->hp_max <= 0x3c && targets[1]->hp_max > 10) {
            damage = 0xff;
            fx_trigger_group(9, &targets[1]);
            if (damage != 0)
                fx_disable(targets[1], 6, "is slain");
        } else {
            for (i = 1; i <= target_count; i++) {
                if (targets[i] != 0 && targets[i]->hp_max <= 10) {
                    damage = 0xff;
                    fx_trigger_group(9, &targets[i]);
                    if (damage != 0)
                        fx_disable(targets[i], 6, "is slain");
                }
            }
        }
    }
}

void far spell_summoning(void)                                   /* 36e3:3e12 */
{
    char cx;
    char cy;
    char x;
    char y;
    char dir;
    struct Hero far *q;
    struct Hero far *p;
    char done;
    char ok;
    unsigned char placed;
    unsigned char tile;
    unsigned char who;
    unsigned char slot;
    unsigned char want;

    q = p = node_list;
    done = 0;
    dir = 0;
    placed = 0;
    if (cursor_obj[0x22])
        want = 0x17;
    else
        want = 0x45;
    while (p != 0 && p->status != 9)
        p = p->next;
    do {
        if (dir > 7) {
            if (q == 0 || q->next == 0 || q->status == 9) {
                q = cur_node;
                done = 1;
            } else {
                q = q->next;
            }
            dir = 0;
        }
        if (p == 0 || placed == 2) {
            done = 1;
        } else {
            p->side = cur_node->side;
            cx = combatant_x(q);
            cy = combatant_y(q);
            ok = 0;
            do {
                x = dir_dx[dir] + cx;
                y = dir_dy[dir] + cy;
                if (x <= 0x31 && x >= 0 && y <= 0x18 && y >= 0) {  /* off map: loops forever, as shipped */
                    cell_get(x, y, &who, &tile);
                    if (who == 0 && tile == want) {
                        slot = combatant_slot(p);
                        (combatants - 1)[slot].size = p->icon_dim & 7;
                        (combatants - 1)[slot].x = x;
                        (combatants - 1)[slot].y = y;
                        ok = fx_revive(p, p->hp_max, "gates in");
                        if (ok) {
                            placed++;
                            p = p->next;
                            if (p == 0 || p->status != 9)
                                done = 1;
                        }
                    }
                    dir++;
                }
            } while (!ok && dir <= 7);
        }
    } while (!done);
    if (placed == 0)
        hero_text(cur_node, "summons is not answered", 10, 1);
}

char far item_casts(struct Item far *it)                         /* 36e3:4055 */
{
    char r;

    if (it != 0 && (it->type == 0x27 || it->type == 0x28 || it->type == 0x49))
        r = 1;
    else
        r = 0;
    return r;
}

void far item_spend(struct Hero far *h, struct Item far *it, unsigned char spell)
                                                                 /* 36e3:408c */
{
    unsigned char want;
    unsigned char k;
    unsigned char slot;
    struct Item far *prev;
    struct Item far *first;
    char found;

    want = spell;
    slot = 0;
    if (it->type == 0x49) {
        first = it;
        found = 0;
        while (it->more != 0 && !found) {
            prev = it;
            it = it->more;
            for (k = 1; k <= 3; k++) {
                if ((it->slots[k] & 0x7f) == want) {
                    slot = k;
                    found = 1;
                }
            }
        }
        if (found) {
            it->slots[slot] = 0;
            it->name1--;
            if (it->name1 < 0x64) {
                prev->more = it->more;
                first->slots[0]--;
                if (first->slots[0] == 1) {
                    prev = first->next;
                    mem_move(first->more, first, sizeof(struct Item));
                    first->more = 0;
                    first->next = prev;
                }
                if (it != 0)
                    pool_free(it, sizeof(struct Item), &item_record_pool);
            }
        }
    } else {
        for (k = 1; k <= 3; k++)
            if ((it->slots[k] & 0x7f) == want)
                slot = k;
        if (slot != 0) {
            it->slots[slot] = 0;
            it->name1--;
            if (it->name1 < 0x64)
                item_remove(h, it);
        }
    }
}

void far cast_announce(struct Hero far *h, char far *text, unsigned char spell)
                                                                 /* 36e3:4235 */
{
    if (view_mode == 5) {
        hero_text(h, "Casts a Spell", 10, 1);
        box_draw_top();
        status_printf(0, 0x17, 7, 0, msg_format("Spell:%s", spell_names[spell]));
    } else {
        text_clear(1, 0x12, 0x26, 0x16);
        character_name_show(h, 1, 0x13, 0);
        status_printf(str_length(h->name) + 2, 0x13, 7, 0, text);
        status_printf(1, 0x14, 7, 0, spell_names[spell]);
        msg_pause();
        combat_text_clear();
    }
}

void far spell_fn_fill(void)                                     /* 36e3:4319 */
{
    spell_fn[1] = spell_bless;
    spell_fn[2] = spell_curse;
    spell_fn[3] = spell_cure_light;
    spell_fn[4] = spell_cause_light;
    spell_fn[5] = fx_affected;
    spell_fn[6] = fx_protected;
    spell_fn[7] = fx_protected;
    spell_fn[8] = spell_resist_cold;
    spell_fn[9] = spell_burning_hands;
    spell_fn[10] = spell_charm_person;
    spell_fn[11] = fx_affected;
    spell_fn[12] = spell_enlarge;
    spell_fn[13] = spell_reduce;
    spell_fn[14] = spell_friends;
    spell_fn[15] = spell_magic_missile;
    spell_fn[16] = fx_protected;
    spell_fn[17] = fx_protected;
    spell_fn[18] = fx_affected;
    spell_fn[19] = spell_shield;
    spell_fn[20] = spell_shocking_grasp;
    spell_fn[21] = spell_sleep;
    spell_fn[22] = fx_affected;
    spell_fn[23] = spell_hold;
    spell_fn[24] = spell_resist_fire;
    spell_fn[25] = spell_silence;
    spell_fn[26] = spell_slow_poison;
    spell_fn[27] = spell_snake_charm;
    spell_fn[28] = spell_spirit_hammer;
    spell_fn[29] = fx_affected;
    spell_fn[30] = spell_invisibility;
    spell_fn[31] = spell_knock;
    spell_fn[32] = spell_mirror_image;
    spell_fn[33] = spell_enfeeble;
    spell_fn[34] = spell_cloud;
    spell_fn[35] = spell_strength;
    spell_fn[36] = spell_heal;
    spell_fn[37] = spell_cure_blindness;
    spell_fn[38] = spell_cause_blindness;
    spell_fn[39] = spell_cure_disease;
    spell_fn[40] = spell_cause_disease;
    spell_fn[41] = spell_dispel_magic;
    spell_fn[42] = spell_prayer;
    spell_fn[43] = spell_remove_curse;
    spell_fn[44] = spell_bestow_curse;
}

void far spellfx_init(void)                                      /* 36e3:44d2 */
{
    hit_proc = camp_spell_targets;
    fx_lock = 0;
    item_cast_active = 0;
    inventory_redraw = 1;
    spell_fn[45] = spell_blink;
    spell_fn[46] = spell_dispel_magic;
    spell_fn[47] = spell_fireball;
    spell_fn[48] = spell_haste;
    spell_fn[49] = spell_hold;
    spell_fn[50] = spell_invisibility;
    spell_fn[51] = spell_lightning_bolt;
    spell_fn[52] = fx_protected;
    spell_fn[53] = fx_protected;
    spell_fn[54] = fx_protected;
    spell_fn[55] = spell_slow;
    spell_fn[56] = spell_harm;
    spell_fn[127] = potion_speed;
    spell_fn[58] = spell_cure_serious;
    spell_fn[128] = potion_frost;
    spell_fn[129] = javelin_lightning;
    spell_fn[130] = wand_paralysis;
    spell_fn[131] = potion_healing;
    spell_fn[132] = elixir_youth;
    spell_fn[133] = spell_fireball;
    spell_fn[134] = wand_missiles;
    spell_fn[66] = spell_cause_serious;
    spell_fn[67] = spell_neutralize;
    spell_fn[68] = spell_poison;
    spell_fn[69] = fx_protected;
    spell_fn[70] = spell_sticks_snakes;
    spell_fn[71] = spell_cure_critical;
    spell_fn[72] = spell_cause_critical;
    spell_fn[73] = spell_dispel_evil;
    spell_fn[74] = spell_flame_strike;
    spell_fn[75] = spell_raise_dead;
    spell_fn[76] = spell_slay_living;
    spell_fn[77] = fx_affected;
    spell_fn[78] = spell_entangle;
    spell_fn[79] = spell_faerie_fire;
    spell_fn[80] = spell_invis_animals;
    spell_fn[81] = spell_charm_monsters;
    spell_fn[82] = spell_confusion;
    spell_fn[83] = spell_dimension_door;
    spell_fn[84] = spell_fear;
    spell_fn[85] = spell_fire_shield;
    spell_fn[86] = spell_fumble;
    spell_fn[87] = spell_ice_storm;
    spell_fn[88] = spell_minor_globe;
    spell_fn[89] = spell_remove_curse;
    spell_fn[90] = spell_barkskin;
    spell_fn[91] = spell_cloud;
    spell_fn[92] = spell_cone_of_cold;
    spell_fn[93] = spell_feeblemind;
    spell_fn[94] = spell_hold;
    spell_fn[135] = scroll_protection;
    spell_fn[96] = spell_charm_mammal;
    spell_fn[136] = potion_invisibility;
    spell_fn[98] = spell_cure_light_98;
    spell_fn[137] = potion_extra_healing;
    spell_fn[100] = spell_bestow_curse_100;
    spell_fn[101] = spell_cloud;
    spell_fn[102] = spell_restoration;
    spell_fn[103] = spell_energy_drain;
    spell_fn[104] = spell_destruction;
    spell_fn[105] = spell_resurrection;
    spell_fn[106] = spell_cure_disease;
    spell_fn[107] = spell_neutralize;
    spell_fn[108] = spell_hold_animal;
    spell_fn[109] = spell_prot_fire;
    spell_fn[110] = spell_death;
    spell_fn[111] = spell_disintegrate;
    spell_fn[112] = spell_globe;
    spell_fn[113] = spell_stone_to_flesh;
    spell_fn[114] = spell_flesh_to_stone;
    spell_fn[115] = spell_fireball;
    spell_fn[116] = spell_mass_invis;
    spell_fn[117] = spell_word_stun;
    spell_fn[118] = spell_fire_touch;
    spell_fn[119] = spell_iron_skin;
    spell_fn[120] = spell_mass_charm;
    spell_fn[121] = spell_dance;
    spell_fn[122] = fx_protected;
    spell_fn[123] = spell_word_blind;
    spell_fn[124] = spell_meteor_swarm;
    spell_fn[125] = spell_word_kill;
    spell_fn[126] = spell_summoning;
    spell_fn[57] = fx_protected;
    spell_fn[59] = spell_silence;
    spell_fn[60] = fx_affected;
    spell_fn[61] = spell_remove_curse;
    spell_fn[62] = spell_bless;
    spell_fn[63] = spell_charm_person;
    spell_fn[64] = spell_burning_hands;
    spell_fn_fill();
}
