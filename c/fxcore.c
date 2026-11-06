/* v1.0 build (CKIT.EXE "Version 1.0 March 19,1993", EXE 0x06fec8; the
 * addresses below are v1.2's).  Differs from v1.2 in two places:
 * fx_disable lacks v1.2's "cursor_obj[0x1d] set and how 6/7/8 -> how 5"
 * rewrite, and fx_trigger_group's case 19 fires trigger 0xd9 where v1.2 fires
 * 0xe2.
 *
 * CKIT.EXE overlay 19 module 2e88: effects on a combatant and the rolls
 * behind them.  Adding, finding and removing effects, the triggers that
 * run an effect's handler, saving throws and attack rolls, the dice,
 * damage that is not a weapon's, recomputing a score an effect or an item
 * changes, raising a fallen member, and the patches on the combat map
 * running out.
 *
 * EXE 0x06ff6e..0x072618, one Turbo C large-model _TEXT segment starting at
 * byte e of paragraph 2e88, the second and last module of RTLink section 19
 * (payload 0x6eff0, link segment 2d91).  fxcore_unit_init is the lone retf
 * rest.c leaves at 0x06ff6e; no call, callback or vector found reaches it.
 * The eight zero bytes after the last retf pad the section to its end.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 19.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, fxcore_NNNN only where in this module the
 * routine starts, and ovl19_2e88_NNNN is rest.c's name for a routine here.
 * A mod18de_ callee is an RTLink vector stub; its comment names the
 * routine the stub reaches.
 *
 * _DATA is DGROUP 8fa4..90d0: hp_fx and 18 stored literals; -d stores
 * "lost a spell", "is Poisoned" and "is killed" once each.  No _BSS.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is, so every
 * routine called ahead of its definition carries a prototype.
 * fx_hurt_immune is near-called ahead of its definition, so it is static.
 */

struct Effect {                 /* 10 bytes, from effect_pool               */
    unsigned char kind;         /* +0                                     */
    char pad_1;                 /* +1  not read here                      */
    unsigned duration;          /* +2  0: does not run out                */
    unsigned char strength;     /* +4                                     */
    char on_remove;             /* +5  nonzero: run the handler on removal */
    struct Effect far *next;    /* +6                                     */
};

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x2e];           /* +4   not read here                     */
    char readied;               /* +32                                    */
    char pad_33[4];             /* +33  not read here                     */
    unsigned char effect;       /* +37                                    */
    unsigned char special;      /* +38  over 0x80: a special, code & 0x7f */
};

struct Fight {                  /* a combatant's state in a fight         */
    unsigned char casting;      /* +0   spell being cast                  */
    char can_cast;              /* +1                                     */
};

struct Stat {
    unsigned char base;
    unsigned char cur;
};

struct Hero {                   /* a party member or a monster            */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x34];           /* +c   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x16];          /* +44  not read here                     */
    unsigned char undead;       /* +5a  nonzero: patch 0x1d passes it by  */
    char pad_5b[3];             /* +5b  not read here                     */
    unsigned char status;       /* +5e                                    */
    unsigned char side;         /* +5f                                    */
    char pad_60[0x10];          /* +60  not read here                     */
    struct Stat stats[6];       /* +70  str int wis dex con cha           */
    unsigned char str_ex;       /* +7c                                    */
    unsigned char str_ex_base;  /* +7d                                    */
    char pad_7e[3];             /* +7e  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82;                /* +82  not read here                     */
    unsigned char saves[6];     /* +83                                    */
    unsigned char hit_dice;     /* +89                                    */
    unsigned char dual_level;   /* +8a                                    */
    char pad_8b[8];             /* +8b  not read here                     */
    unsigned char control;      /* +93                                    */
    char pad_94[9];             /* +94  not read here                     */
    unsigned char level[7];     /* +9d  one per class                     */
    unsigned char old_level[7]; /* +a4  one per class                     */
    char pad_ab[0xd];           /* +ab  not read here                     */
    unsigned char hp_base;      /* +b8                                    */
    char pad_b9[7];             /* +b9  not read here                     */
    unsigned char immunity;     /* +c0                                    */
    char pad_c1[2];             /* +c1  not read here                     */
    char save_bonus;            /* +c3                                    */
    unsigned char magic_resist; /* +c4  magic resistance, percent         */
    char pad_c5[0xb9];          /* +c5  not read here                     */
    char active;                /* +17e                                   */
    char pad_17f;               /* +17f not read here                     */
    unsigned char to_hit;       /* +180                                   */
    unsigned char ac;           /* +181                                   */
    char pad_182[9];            /* +182 not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct Patch {                  /* 0x21 bytes: a patch on the combat map  */
    char pad_0[4];              /* +0   not read here                     */
    struct Patch far *next;     /* +4                                     */
    unsigned char terrain[10];  /* +8   [1..9] the tile under it          */
    char active[10];            /* +12  [1..9] nonzero: on the map        */
    char x;                     /* +1c                                    */
    char y;                     /* +1d                                    */
    unsigned char strength;     /* +1e  rounds left; 0xff: dispelled      */
    unsigned char level;        /* +1f  not read here                     */
    unsigned char kind;         /* +20  the map cell it becomes           */
};

struct Map {
    char pad_0[9];              /* +0   not read here                     */
    unsigned char cell[25][50]; /* +9                                     */
};

struct Step {
    unsigned char who;          /* +0   index into heroes                 */
    unsigned char cost;         /* +1                                     */
    unsigned char dir;          /* +2                                     */
};

struct Fallen {                 /* 9 bytes: a fallen combatant            */
    struct Hero far *hero;      /* +0                                     */
    int x;                      /* +4                                     */
    int y;                      /* +6                                     */
    char terrain;               /* +8   not read here                     */
};

struct Combatant {
    int x;                      /* +0   not read here                     */
    int y;                      /* +2   not read here                     */
    char number;                /* +4   not read here                     */
    unsigned char size;         /* +5   0: off the map                    */
};

struct Pool;

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far hero_panel(struct Hero far *h);                   /* 03d0:0834 */
extern void far spell_mark(struct Hero far *h, unsigned char spell); /* 03d0:1381 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause); /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far hero_damage(struct Hero far *h, unsigned short dmg); /* 03d0:1792 */
extern unsigned char far caster_level(unsigned char spell, char cap); /* 03d0:18fe */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far msg_pause(void);                                 /* 06c9:0953 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool); /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool); /* 0a75:02c8 */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far party_display(void far *node);   /* 18de:1067, vector to overlay 4 1c86:023a */
extern void far combat_sides_count(void);             /* 18de:111b, vector to overlay 0 1cb8:091f */
extern char far combat_actions_end(struct Hero far *h); /* 18de:1139, vector to overlay 0 1cb8:0b84 */
extern char far combatant_x(struct Hero far *h); /* 18de:117f, vector to overlay 0 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h); /* 18de:1189, vector to overlay 0 22fa:0ae4 */
extern void far view_center(struct Hero far *h, unsigned char margin, char cursor);
                                /* 18de:1193, vector to overlay 0 22fa:1286 */
extern char far combatant_visible(struct Hero far *h, char whole);
                                /* 18de:119d, vector to overlay 0 22fa:0627 */
extern void far combat_direction_probe(struct Hero far *h, unsigned char dir, unsigned char far *who,
                             unsigned char far *cell, unsigned char far *kind);
                                /* 18de:11c5, vector to overlay 0 22fa:0b9c */
extern void far combat_view_scroll(char x, char y, unsigned char margin, unsigned char dir);
                                /* 18de:11d9, vector to overlay 0 22fa:0847 */
extern char far hero_place(struct Hero far *h, unsigned char x, unsigned char y,
                             char moving);
                                /* 18de:11e3, vector to overlay 0 22fa:1053 */
extern void far tile_update(char x, char y, unsigned char who);
                                /* 18de:11ed, vector to overlay 0 22fa:04d5 */
extern unsigned char far combatant_slot(struct Hero far *h);
                                /* 18de:11f7, vector to overlay 0 22fa:0b44 */
extern void far hit_flash(struct Hero far *h, char a, char far *msg);
                                /* 18de:123d, vector to overlay 0 1cb8:0730 */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern unsigned char far combatant_size(struct Hero far *h);
                                /* 18de:128d, vector to overlay 0 22fa:0b14 */
extern void far hero_fall(struct Hero far *h); /* 18de:12b5, vector to overlay 0 22fa:0d0a */
extern void far combat_grid_build(void);             /* 18de:12c9, vector to overlay 0 22fa:0331 */
extern void far fx_dispatch(unsigned char kind, struct Hero far *h,
                             struct Effect far *fx, char removed);
                                /* 18de:1c8d, vector to overlay 11 26e3:3831 */

extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern unsigned char patch_dir[];                                /* 6199 */
extern unsigned char hit_dice_level_limit[];                                 /* 6262 */
extern struct Hero far *cur_node;                                /* b3dd */
extern unsigned char text_row;                                   /* b41a */
extern int view_mode;                                            /* b421 */
extern int damage;                                               /* b42f */
extern char effect_id;                                           /* b720 */
extern char save_succeeded;                                           /* b721 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern unsigned char save_bonus;                                 /* b8d3 */
extern struct Pool patch_pool;                                    /* bb90 */
extern unsigned char damage_dice_count;                                 /* be60 */
extern unsigned char current_spell;                                  /* be64 */
extern struct Pool effect_pool;                                    /* c3c4 */
extern int element_mask;                                         /* c64e */
extern struct Map far *cur_map;                                  /* c66b */
extern char mirror_image_bypass;                                           /* ca08 */
extern struct Patch far *patch_list;                             /* ca0a */
extern unsigned char step_count;                                 /* ca10 */
extern char attack_roll;                                           /* ca1d */
extern int current_save_type;                                            /* ca1f */
extern struct Step steps[];                                      /* ca2b */
extern char move_flag;                                           /* cc79 */
extern struct Combatant combatants[];                            /* cc7c */
extern struct Fallen recs[8];                                    /* ce14 */
extern unsigned char rec_count;                                  /* cfd7 */
extern char panel_dirty;                                         /* d0fb */
extern char crit_hit;                                            /* d0fd */
extern struct Hero far *node_list;                               /* d129 */

void far fx_trigger_group(unsigned char when, struct Hero far * far *hp);
void far fx_remove(struct Hero far *h, unsigned char kind, struct Effect far *fx);
char far fx_saving_throw(struct Hero far *h, int type, char adj);
int  far fx_roll_dice(unsigned char n, unsigned char sides);
int  far fx_roll_damage(unsigned char n, unsigned char sides);
void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                     unsigned char strength, char on_remove);
void far fx_invisibility_remove(struct Hero far *h);
void far fx_hp_effects_remove(struct Hero far *h);
void far fx_stat_recalc(struct Hero far *h, short stat);
void far fx_damage(struct Hero far *h, unsigned char dmg, int reduce, char saved);
void far fx_add_message(struct Hero far *h, unsigned char kind, int dur,
                     unsigned char strength, char on_remove, int save_negates,
                     char saved, char far *msg);
static void fx_hurt_immune(struct Hero far *h);

static unsigned char hp_fx[18] = {                               /* 8fa4 */
    0x03, 0x03, 0x0b, 0x15, 0x17, 0x1b, 0x1e, 0x1f, 0x23,
    0x34, 0x4d, 0x6a, 0x6b, 0x6f, 0xdd, 0xde, 0xb2, 0x76
};

/* The translated Pascal unit's empty initialization block (DKK fxengine.pas). */
void far fxcore_unit_init(void)                                       /* 3e88:000e */
{
}

void far fx_disable(struct Hero far *h, int how, char far *msg) /* 3e88:000f */
{
    hero_text(h, msg, 10, 0);
    switch (h->status) {
    case 6:
    case 7:
    case 8:
        break;
    default:
        h->status = how;
        h->active = 0;
        h->hp = 0;
        fx_hp_effects_remove(h);
        fx_trigger_group(0xd, &h);
        if (h->active == 0)
            hero_fall(h);
        text_pause();
        combat_text_clear();
        if (view_mode != 5)
            party_display(cur_node);
    }
}

void far fx_remove(struct Hero far *h, unsigned char kind,
                         struct Effect far *fx)                  /* 3e88:00d8 */
{
    struct Effect far *prev;
    struct Effect far *node;

    node = fx;
    if (node == 0) {
        node = h->effects;
        while (node != 0 && node->kind != kind)
            node = node->next;
    }
    if (node == 0)
        return;
    if (node->on_remove != 0)
        fx_dispatch(kind, h, node, 1);
    if (h->effects == node)
        h->effects = node->next;
    else {
        prev = h->effects;
        while (prev != 0 && prev->next != node)
            prev = prev->next;
        if (prev != 0)
            prev->next = node->next;
    }
    pool_free(node, 10, &effect_pool);
    if (kind == 0xe)
        fx_stat_recalc(h, 5);
    if (kind == 0xc || kind == 0x26)
        fx_stat_recalc(h, 0);
    if (kind == 0x44) {
        fx_stat_recalc(h, 1);
        fx_stat_recalc(h, 2);
    }
}

void fx_resist(struct Hero far *h)                               /* 3e88:0248 */
{
    unsigned char level;
    int resist;
    int chance;

    resist = h->magic_resist;
    if (resist != 0 && (effect_id != 0 || ((char)element_mask & 8))) { /* test byte */
        level = caster_level(current_spell, 0);
        chance = (11 - level) * 5;
        chance += resist;
        if (fx_roll_dice(1, 100) <= chance) {
            damage = 0;
            if (effect_id != 0x5b && effect_id != 0x52)
                effect_id = 0;
        }
    }
}

void fx_trigger(struct Hero far * far *hp, unsigned char kind)   /* 3e88:02c4 */
{
    unsigned char saved[0xcc];
    struct Hero far *p;
    struct Effect far *fx;
    char found;
    unsigned char i;
    unsigned char range;

    found = 0;
    p = node_list;
    if (find_effect(*hp, kind, &fx))
        found = 1;
    else if (kind == 0x15 || kind == 0x2d || kind == 0x2e || kind == 0x31) {
        mem_move5(steps, saved, 0xcc);
        while (p != 0 && !found) {
            if (find_effect(p, kind, &fx)) {
                if (view_mode == 5) {
                    if (kind == 0x31)
                        range = 6;
                    else
                        range = 1;
                    plan_steps(combatant_x(p), combatant_y(p), range, 0xff,
                                 combatant_size(p));
                    for (i = 1; i <= step_count; i++)
                        if (combatant_slot(*hp) == (steps - 1)[i].who)
                            found = 1;
                } else
                    found = 1;
            }
            p = p->next;
        }
        mem_move5(saved, steps, 0xcc);
    }
    if (found)
        fx_dispatch(kind, *hp, fx, 0);
}

void far fx_trigger_group(unsigned char when, struct Hero far * far *hp) /* 3e88:0449 */
{
    switch (when) {
    case 1:
        fx_trigger(hp, 0x19); fx_trigger(hp, 0x45); fx_trigger(hp, 0x25);
        break;
    case 2:
        fx_trigger(hp, 0x56); fx_trigger(hp, 0x65); fx_trigger(hp, 0x66);
        fx_trigger(hp, 0x79); fx_trigger(hp, 0x41); fx_trigger(hp, 0x73);
        fx_trigger(hp, 0x20); fx_trigger(hp, 0xe0); fx_trigger(hp, 0xaa);
        fx_trigger(hp, 0xb0); fx_trigger(hp, 0xdc); fx_trigger(hp, 0x84);
        break;
    case 3:
        fx_trigger(hp, 0x40); fx_trigger(hp, 0x56); fx_trigger(hp, 0x65);
        fx_trigger(hp, 0x66); fx_trigger(hp, 0x79); fx_trigger(hp, 0x41);
        fx_trigger(hp, 0x20); fx_trigger(hp, 0x68); fx_trigger(hp, 0x6c);
        fx_trigger(hp, 0xaa); fx_trigger(hp, 0xb0); fx_trigger(hp, 0x84);
        fx_trigger(hp, 0x58);
        break;
    case 4:
        fx_trigger(hp, 0x1d); fx_trigger(hp, 0x69); fx_trigger(hp, 0x78);
        fx_trigger(hp, 0x4b); fx_trigger(hp, 0x74); fx_trigger(hp, 0xba);
        break;
    case 5:
        fx_trigger(hp, 0x1c); fx_trigger(hp, 0x29); fx_trigger(hp, 0x3c);
        fx_trigger(hp, 0x42); fx_trigger(hp, 0x43); fx_trigger(hp, 0x50);
        fx_trigger(hp, 0x51); fx_trigger(hp, 0x70); fx_trigger(hp, 0x52);
        fx_trigger(hp, 0x67); fx_trigger(hp, 0x7d); fx_trigger(hp, 0xd9);
        fx_trigger(hp, 0xa8); fx_trigger(hp, 0xa9); fx_trigger(hp, 0x9f);
        fx_trigger(hp, 0xae); fx_trigger(hp, 0xaf); fx_trigger(hp, 0xc1);
        break;
    case 6:
        fx_resist(*hp);
        fx_trigger(hp, 0x0a); fx_trigger(hp, 0x11); fx_trigger(hp, 0x14);
        fx_trigger(hp, 0x1c); fx_trigger(hp, 0x06); fx_trigger(hp, 0x28);
        fx_trigger(hp, 0x32); fx_trigger(hp, 0x36); fx_trigger(hp, 0x3d);
        fx_trigger(hp, 0x49); fx_trigger(hp, 0x4f); fx_trigger(hp, 0x5d);
        fx_trigger(hp, 0x62); fx_trigger(hp, 0x3f); fx_trigger(hp, 0x39);
        fx_trigger(hp, 0xd9); fx_trigger(hp, 0xa8); fx_trigger(hp, 0xad);
        fx_trigger(hp, 0xae); fx_trigger(hp, 0x77); fx_trigger(hp, 0xdb);
        fx_hurt_immune(*hp);
        break;
    case 7:
        fx_trigger(hp, 0x03); fx_trigger(hp, 0x1b); fx_trigger(hp, 0x1f);
        fx_trigger(hp, 0x34); fx_trigger(hp, 0x35); fx_trigger(hp, 0x6a);
        fx_trigger(hp, 0x33); fx_trigger(hp, 0x78);
        break;
    case 8:
        fx_trigger(hp, 0x59); fx_trigger(hp, 0x64); fx_trigger(hp, 0xab);
        fx_trigger(hp, 0xc3);
        break;
    case 9:
        fx_resist(*hp);
        fx_trigger(hp, 0x12); fx_trigger(hp, 0x1c); fx_trigger(hp, 0x28);
        fx_trigger(hp, 0x39); fx_trigger(hp, 0x3f); fx_trigger(hp, 0x4c);
        fx_trigger(hp, 0x4f); fx_trigger(hp, 0x5c); fx_trigger(hp, 0x5f);
        fx_trigger(hp, 0x7b); fx_trigger(hp, 0x9e); fx_trigger(hp, 0x5d);
        fx_trigger(hp, 0x8d); fx_trigger(hp, 0x8f);
        break;
    case 10:
        fx_trigger(hp, 0x01); fx_trigger(hp, 0x02); fx_trigger(hp, 0x07);
        fx_trigger(hp, 0x1a); fx_trigger(hp, 0x21); fx_trigger(hp, 0x24);
        fx_trigger(hp, 0x31); fx_trigger(hp, 0x78); fx_trigger(hp, 0x94);
        break;
    case 11:
        fx_trigger(hp, 0x04); fx_trigger(hp, 0x08); fx_trigger(hp, 0x09);
        fx_trigger(hp, 0x0d); fx_trigger(hp, 0x11); fx_trigger(hp, 0x1e);
        fx_trigger(hp, 0x21); fx_trigger(hp, 0x2d); fx_trigger(hp, 0x2e);
        fx_trigger(hp, 0x76); fx_trigger(hp, 0x47); fx_trigger(hp, 0x7a);
        break;
    case 12:
        fx_trigger(hp, 0x08); fx_trigger(hp, 0x09); fx_trigger(hp, 0x0a);
        fx_trigger(hp, 0x0d); fx_trigger(hp, 0x11); fx_trigger(hp, 0x14);
        fx_trigger(hp, 0x21); fx_trigger(hp, 0x24); fx_trigger(hp, 0x2d);
        fx_trigger(hp, 0x2e); fx_trigger(hp, 0x31); fx_trigger(hp, 0x32);
        fx_trigger(hp, 0x36); fx_trigger(hp, 0x3d); fx_trigger(hp, 0x5e);
        fx_trigger(hp, 0x61); fx_trigger(hp, 0x7e); fx_trigger(hp, 0x8f);
        break;
    case 13:
        fx_trigger(hp, 0xb3); fx_trigger(hp, 0xd8);
        break;
    case 14:
        fx_trigger(hp, 0x46); fx_trigger(hp, 0x53); fx_trigger(hp, 0x54);
        fx_trigger(hp, 0x55); fx_trigger(hp, 0x5a); fx_trigger(hp, 0x5b);
        fx_trigger(hp, 0x60); fx_trigger(hp, 0x3a); fx_trigger(hp, 0x75);
        fx_trigger(hp, 0x72); fx_trigger(hp, 0xb3); fx_trigger(hp, 0xcb);
        fx_trigger(hp, 0xdf); fx_trigger(hp, 0x7c);
        break;
    case 15:
        fx_trigger(hp, 0x38); fx_trigger(hp, 0x0b); fx_trigger(hp, 0x15);
        fx_trigger(hp, 0x1e); fx_trigger(hp, 0x44); fx_trigger(hp, 0x52);
        fx_trigger(hp, 0xb1); fx_trigger(hp, 0x89);
        break;
    case 16:
        fx_trigger(hp, 0x19); fx_trigger(hp, 0x45); fx_trigger(hp, 0x25);
        fx_trigger(hp, 0x2f); fx_trigger(hp, 0x30); fx_trigger(hp, 0x59);
        break;
    case 17:
        fx_trigger(hp, 0x01); fx_trigger(hp, 0x0b);
        break;
    case 18:
        fx_trigger(hp, 0x4a); fx_trigger(hp, 0x27); fx_trigger(hp, 0x2a);
        fx_trigger(hp, 0x63); fx_trigger(hp, 0xb2); fx_trigger(hp, 0xdd);
        break;
    case 19:
        fx_trigger(hp, 0x0b); fx_trigger(hp, 0x17); fx_trigger(hp, 0xd9);
        break;
    case 20:
        fx_trigger(hp, 0x32); fx_trigger(hp, 0x36);
        break;
    case 21:
        fx_trigger(hp, 0x23);
        break;
    case 24:
        fx_trigger(hp, 0x64);
        break;
    }
}

void far fx_terrain_apply(struct Hero far *h, char may_save, char skip_1d) /* 3e88:0faa */
{
    unsigned char cell;
    unsigned char who;
    struct Effect far *fx;
    struct Effect far *fx2;
    char saved;
    struct Hero far *node;
    unsigned char kind;

    if (h->active == 0)
        return;
    mirror_image_bypass = 1;
    combat_direction_probe(h, 8, &who, &cell, &kind);
    if (kind != 0 && (kind != 0x1d || skip_1d == 0)) {
        if (!combatant_visible(h, 1) && move_flag)
            combat_view_scroll(combatant_x(h), combatant_y(h), 0xff, 8);
        element_mask = 8;
        switch (kind) {
        case 0x1d:
            current_spell = 0x22;
            if (h->undead != 0)
                break;
            if (find_effect(h, 0x1f, &fx))
                break;
            if (find_effect(h, 0x8f, &fx))
                break;
            if (h->immunity & 0x18)
                break;
            saved = fx_saving_throw(h, 0, 0);
            if (saved) {
                node = cur_node;
                fx_add_message(h, 0x1e, fx_roll_dice(1, 4) + 1, 0xff, 0, 0, saved,
                            "starts to cough");
                fx2 = 0;
                if (find_effect(h, 0x1e, &fx2)) {
                    fx_dispatch(0x1e, h, fx, 0);     /* fx, not fx2 */
                    if (h->fight->casting > 0) {
                        hero_text(h, "lost a spell", 12, 1);
                        spell_mark(h, h->fight->casting);
                        combat_actions_end(h);
                    }
                }
                cur_node = node;
            } else {
                node = cur_node;
                fx_add_message(h, 0x1f, fx_roll_dice(1, 4) + 1, 0xff, 0, 0, saved,
                            "chokes and gags from nausea");
                if (find_effect(h, 0x1f, &fx))
                    fx_dispatch(0x1f, h, fx, 0);
                cur_node = node;
            }
            break;
        case 0x1c:
            current_spell = 0x5b;
            if (h->active == 0)
                break;
            if (h->immunity & 0x10)
                break;
            switch (h->hit_dice) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                hero_text(h, "is Poisoned", 10, 0);
                text_pause();
                fx_add(h, 0x37, 0, 0xff, 0);
                fx_disable(h, 6, "is killed");
                break;
            case 5:
                if (!fx_saving_throw(h, 0, -4)) {
                    hero_text(h, "is Poisoned", 10, 0);
                    text_pause();
                    fx_add(h, 0x37, 0, 0xff, 0);
                    fx_disable(h, 6, "is killed");
                }
                break;
            case 6:
                if (!fx_saving_throw(h, 0, 0)) {
                    hero_text(h, "is Poisoned", 10, 0);
                    text_pause();
                    fx_add(h, 0x37, 0, 0xff, 0);
                    fx_disable(h, 6, "is killed");
                }
                break;
            default:
                element_mask = 0x88;
                fx_damage(h, fx_roll_damage(1, 10), 0, 0);
            }
            break;
        case 0x1b:
            current_spell = 0x65;
            damage = 0xff;
            fx_trigger_group(9, &h);
            if (damage == 0)
                break;
            if (h->active == 0)
                break;
            if (may_save == 0 || !fx_saving_throw(h, 4, 0)) {
                hero_text(h, "is cut by knives", 10, 0);
                text_pause();
                fx_damage(h, fx_roll_damage(8, 8), 0, 0);
            }
            break;
        }
    }
    mirror_image_bypass = 0;
}

char far fx_attack_roll_no_attacker(struct Hero far *h, unsigned char need)     /* 3e88:13ac */
{
    char hit;

    hit = 0;
    attack_roll = fx_roll_dice(1, 20);
    if (attack_roll > 1) {
        if (attack_roll > 16)
            crit_hit = 1;
        else
            crit_hit = 0;
        if (attack_roll == 20)
            attack_roll = 100;
        fx_trigger_group(0x10, &h);
        if (attack_roll >= 0) {
            if (attack_roll + need > h->ac)
                hit = 1;
            crit_hit = crit_hit && hit;
        }
    }
    return hit;
}

char far fx_attack_roll(struct Hero far *h, struct Hero far *t,
                     unsigned char need)                         /* 3e88:143d */
{
    char hit;
    char r;
    char adj;

    hit = 0;
    r = 0;
    fx_invisibility_remove(h);
    attack_roll = fx_roll_dice(1, 20);
    if (attack_roll > 1) {
        if (attack_roll > 16)
            crit_hit = 1;
        else
            crit_hit = 0;
        if (attack_roll == 20)
            attack_roll = 100;
        fx_trigger_group(10, &h);
        fx_trigger_group(0x10, &t);
        if (h->side == 0) {
            adj = cursor_obj[0x2f];
            switch (cursor_obj[0x27]) {
            case 1:
                adj += 2;
                break;
            case 2:
                adj++;
                break;
            case 4:
                adj--;
                break;
            case 5:
                adj -= 2;
                break;
            }
        } else
            adj = cursor_obj[1];
        if (attack_roll >= 0 && h->to_hit + attack_roll + adj >= need) {
            hit = 1;
            r = 1;
        }
        crit_hit = crit_hit && hit;
    }
    return r;
}

char far fx_saving_throw(struct Hero far *h, int type, char adj)     /* 3e88:1552 */
{
    char r;
    struct Effect far *fx;

    save_succeeded = 1;
    save_bonus = fx_roll_dice(1, 20);
    if (find_effect(h, 0x76, &fx))
        save_bonus = 1;
    switch (save_bonus) {
    case 1:
        save_succeeded = 0;
        break;
    default:
        save_bonus = h->save_bonus + save_bonus + adj;
        current_save_type = type;
        fx_trigger_group(0xc, &h);
        if (h->saves[type] <= save_bonus)
            save_succeeded = 1;
        else
            save_succeeded = 0;
        break;
    case 20:
        save_succeeded = 1;
        break;
    }
    r = save_succeeded;
    return r;
}

int far fx_roll_dice(unsigned char n, unsigned char sides)    /* 3e88:15ee */
{
    unsigned char i;
    int total;

    total = 0;
    if (n > 0)
        for (i = 1; i <= n; i++)
            total += random_n(sides) + 1;
    return total;
}

int far fx_roll_damage(unsigned char n, unsigned char sides)        /* 3e88:1624 */
{
    unsigned char r;

    damage_dice_count = n;
    r = fx_roll_dice(n, sides);
    return r;
}

void far fx_add(struct Hero far *h, unsigned char kind, int dur,
                     unsigned char strength, char on_remove)     /* 3e88:1648 */
{
    struct Effect far *fx;
    struct Effect far *e;

    if (kind == 0)
        return;
    pool_alloc((void far * far *)&fx, 10, &effect_pool);
    if (h->effects == 0)
        h->effects = fx;
    else {
        e = h->effects;
        while (e->next != 0)
            e = e->next;
        e->next = fx;
    }
    fx->next = 0;
    fx->kind = kind;
    fx->duration = dur;
    fx->on_remove = on_remove;
    fx->strength = strength;
}

void far fx_out_of_play(struct Hero far *h, int how, char far *msg) /* 3e88:16ef */
{
    unsigned char slot;

    if (h->active == 0)
        return;
    slot = combatant_slot(h);
    view_center(h, 3, 0);
    hero_text(h, msg, 10, 1);
    h->active = 0;
    h->status = how;
    if (h->status != 3)
        h->hp = 0;
    tile_update(0, 0, slot);
    screen_flush();
    (combatants - 1)[slot].size = 0;
    combat_grid_build();
    if (combat_actions_end(h))
        fx_hp_effects_remove(h);
}

void far fx_invisibility_remove(struct Hero far *h)                         /* 3e88:17b0 */
{
    struct Effect far *fx;

    while (find_effect(h, 0x19, &fx))
        fx_remove(h, 0x19, fx);
}

void far fx_hp_effects_remove(struct Hero far *h)                         /* 3e88:17ec */
{
    unsigned char i;
    struct Effect far *fx;

    for (i = 1; i <= 18; i++)
        fx_remove(h, (hp_fx - 1)[i], 0);
    if (find_effect(h, 0x4d, &fx) && h->control == 0xb3)
        h->side = 0;
}

char far fx_cure(struct Hero far *h, unsigned char kind)     /* 3e88:184b */
{
    char r;
    struct Effect far *fx;

    r = 0;
    if (find_effect(h, kind, &fx)) {
        hero_text(h, "is Cured", 10, 1);
        fx_remove(h, kind, fx);
        r = 1;
    }
    return r;
}

unsigned char fx_str_code(unsigned char base, unsigned char ex)  /* 3e88:18a9 */
{
    unsigned char r;

    r = base + 100;
    if (base == 18)
        r = ex + 1;
    return r;
}

void fx_str_decode(struct Effect far *fx, unsigned char far *base,
                   unsigned char far *ex)                        /* 3e88:18cc */
{
    *ex = 0;
    *base = fx->strength & 0x7f;
    if (*base <= 101) {
        *ex = *base - 1;
        *base = 18;
    } else
        *base = *base - 100;
}

char far fx_strength_upgrade(struct Hero far *h, unsigned char base, unsigned char ex,
                     unsigned char far *out)                     /* 3e88:190c */
{
    char r;

    if (h->stats[0].base < base || (base == 18 && h->str_ex_base < ex)) {
        r = 1;
        *out = fx_str_code(base, ex);
    } else
        r = 0;
    return r;
}

void fx_keep_best(unsigned char far *best_ex, unsigned char far *cand_ex,
                  unsigned char far *best, unsigned char far *cand) /* 3e88:1950 */
{
    if (*cand > *best || (*cand == 18 && *cand_ex > *best_ex)) {
        *best = *cand;
        *best_ex = *cand_ex;
    }
}

void fx_con_bonus(short cls, unsigned char lvl, unsigned char far *sum,
                  unsigned char far *con)                        /* 3e88:1992 */
{
    if (hit_dice_level_limit[cls] <= lvl)
        lvl = hit_dice_level_limit[cls] - 1;
    if (cls == 4 || cls == 1)
        lvl++;
    if (cls == 2 || cls == 3 || cls == 4 || cls == 1) {
        switch (*con) {
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            *sum += (*con - 14) * lvl;
            break;
        case 20:
            *sum += lvl * 5;
            break;
        case 21:
        case 22:
        case 23:
            *sum += lvl * 6;
            break;
        case 24:
        case 25:
            *sum += lvl * 7;
            break;
        }
    } else if (*con > 15)
        *sum += lvl * 2;
    else if (*con == 15)
        *sum += lvl;
}

void far fx_stat_recalc(struct Hero far *h, short stat)             /* 3e88:1a80 */
{
    unsigned char best;
    unsigned char cand;
    unsigned char best_ex;
    unsigned char cand_ex;
    struct Effect far *fx;
    unsigned char old_max;
    unsigned char sum;
    unsigned char lvl;
    short i;
    struct Item far *it;
    unsigned char code;
    unsigned char classes;

    sum = 0;
    cand = 0;
    cand_ex = 0;
    it = h->items;
    best = h->stats[stat].base;
    best_ex = h->str_ex_base;
    while (it != 0) {
        if (it->special > 0x80 && it->readied != 0) {
            code = it->special & 0x7f;
            if (stat == 0) {
                switch (code) {
                case 3:
                    cand = 18;
                    cand_ex = 100;
                    break;
                case 5:
                    cand = it->effect + 18;
                    if (cand == 18)
                        cand_ex = 100;
                    break;
                }
                fx_keep_best(&best_ex, &cand_ex, &best, &cand);
            }
        }
        it = it->next;
    }
    switch (stat) {
    case 0:
        if (find_effect(h, 0x26, &fx)) {
            fx_str_decode(fx, &cand, &cand_ex);
            if (best <= 18 && best_ex < 100) {
                cand = best + cand;
                if (cand > 18) {
                    if (h->level[2] > 0 || h->level[3] > 0 || h->level[4] > 0
                        || h->level[1] > 0) {
                        cand_ex = h->str_ex + (cand - 18) * 10;
                        if (cand_ex > 100)
                            cand_ex = 100;
                    }
                    cand = 18;
                }
            }
            fx_keep_best(&best_ex, &cand_ex, &best, &cand);
        }
        if (find_effect(h, 0x71, &fx)) {
            fx_str_decode(fx, &cand, &cand_ex);
            fx_keep_best(&best_ex, &cand_ex, &best, &cand);
        }
        if (find_effect(h, 0xc, &fx)) {
            fx_str_decode(fx, &cand, &cand_ex);
            fx_keep_best(&best_ex, &cand_ex, &best, &cand);
        }
        h->stats[0].cur = best;
        h->str_ex = best_ex;
        break;
    case 4:
        classes = 0;
        old_max = h->hp_max;
        h->hp_max = h->hp_base;
        for (i = 0; i <= 6; i++) {
            lvl = h->old_level[i];
            if (lvl > 0)
                fx_con_bonus(i, lvl, &sum, &best);
            lvl = h->level[i];
            if (lvl > 0)
                classes++;
            if (hit_dice_level_limit[i] < lvl)
                lvl = hit_dice_level_limit[i];
            if ((int)h->dual_level < lvl) { /* a signed word compare */
                lvl -= h->dual_level;
                fx_con_bonus(i, lvl, &sum, &best);
            }
        }
        sum = sum / classes;
        h->hp_max += sum;
        if (h->hp_max > old_max && h->hp != 0)
            h->hp += h->hp_max - old_max;
        if (h->hp_max < old_max && h->hp != 0) {
            if (h->hp > old_max - h->hp_max)
                h->hp -= old_max - h->hp_max;
            else
                h->hp = 0;
        }
        h->stats[4].cur = best;
        if (h->stats[4].cur >= 20)
            fx_add(h, 0x3e, 60, 0xff, 1);
        else
            fx_remove(h, 0x3e, 0);
        break;
    case 5:
        if (find_effect(h, 0xe, &fx))
            best += fx->strength;
        h->stats[5].cur = best;
        break;
    case 1:
    case 2:
        if (find_effect(h, 0x44, &fx))
            best = 3;
        h->stats[stat].cur = best;
        break;
    }
}

void far fx_damage(struct Hero far *h, unsigned char dmg, int reduce,
                     char saved)                                 /* 3e88:1ea2 */
{
    char msg[0x52];

    damage = dmg;
    fx_trigger_group(6, &h);
    if (saved) {
        switch (reduce) {
        case 1:
            damage = 0;
            break;
        case 2:
            damage = damage / 2;
            break;
        }
    } else
        fx_trigger_group(0x14, &h);
    if (damage > 0 && h->active != 0) {
        if (damage != 1)
            str_format(msg, "takes %d%s", damage, " points of damage ");
        else
            str_copy(msg, "takes 1 point of damage ");
        switch (element_mask & ~8) {
        case 1:
            str_copy(msg, msg_format("%sfrom Fire", msg));
            break;
        case 2:
            str_copy(msg, msg_format("%sfrom Cold", msg));
            break;
        case 4:
            str_copy(msg, msg_format("%sfrom Electricity", msg));
            break;
        default:
            if ((element_mask & 8) == element_mask)
                str_copy(msg, msg_format("%sfrom Magic", msg));
        }
        panel_dirty = 1;
        if (view_mode != 5)
            panel_dirty = 0;
        hero_panel(h);
        hit_flash(h, 0, msg);
        hero_damage(h, damage);
        if (view_mode == 5) {
            h->fight->can_cast = 0;
            if (h->fight->casting > 0) {
                hero_text(h, "lost a spell", 12, 1);
                spell_mark(h, h->fight->casting);
                combat_actions_end(h);
            }
        }
        if (h->active == 0) {
            str_copy(msg, "Goes Down");
            if ((h->immunity & 8) == 0) {
                if (h->status == 5)
                    str_copy(msg, msg_format("%s and is Dying", msg));
                if (h->status == 6 || h->status == 8 || h->status == 7)
                    str_copy(msg, "is killed");
            }
            hero_text(h, msg, text_row + 1, 0);
            if (view_mode == 5) {
                fx_hp_effects_remove(h);
                fx_trigger_group(0xd, &h);
                if (h->active == 0 && h->status != 10)
                    hero_fall(h);
                else
                    msg_pause();
            } else
                msg_pause();
        }
        combat_text_clear();
    }
}

void far fx_add_message(struct Hero far *h, unsigned char kind, int dur,
                     unsigned char strength, char on_remove, int save_negates,
                     char saved, char far *msg)                  /* 3e88:2181 */
{
    struct Effect far *fx;

    effect_id = kind;
    fx_trigger_group(9, &h);
    if (effect_id == 0 || (saved != 0 && save_negates == 1))
        hero_text(h, "is Unaffected", 10, 1);
    else {
        if (find_effect(h, effect_id, &fx) && fx->duration != 0)
            fx_remove(h, kind, fx);
        fx_add(h, kind, dur, strength, on_remove);
        if (msg != 0 && *msg != 0) {
            hit_flash(h, 1, msg);
            combat_text_clear();
        }
    }
}

char far fx_heal(struct Hero far *h, char n, char show)  /* 3e88:224c */
{
    struct Effect far *fx;
    char r;

    r = 0;
    if (h->status == 0 || h->status == 1 || h->status == 4 || h->status == 5) {
        if (h->hp < h->hp_max && (show != 0 || !find_effect(h, 0xd7, &fx))) {
            h->hp = h->hp + n;
            if (h->hp > h->hp_max)
                h->hp = h->hp_max;
            if (h->active == 0) {
                if (h->status == 5)
                    h->status = 4;
                if (h->status == 4 && view_mode != 5) {
                    h->status = 0;
                    h->active = 1;
                }
            }
            r = 1;
        }
    }
    return r;
}

char far fx_revive(struct Hero far *h, unsigned char hp, char far *msg) /* 3e88:230c */
{
    char r;
    char was;

    if (hero_place(h, combatant_x(h), combatant_y(h), 1)) {
        h->status = 0;
        h->active = 1;
        h->hp = hp;
        if (view_mode == 5) {
            was = move_flag;
            move_flag = 1;
            view_center(h, 3, 0);
            move_flag = was;
        }
        hit_flash(h, 1, msg);
        combat_sides_count();
        r = 1;
    } else
        r = 0;
    return r;
}

void fx_cloud_draw(void)                                         /* 3e88:23b0 */
{
    struct Patch far *c;
    unsigned char i;

    for (c = patch_list; c != 0; c = c->next)
        for (i = 1; i <= 9; i++)
            if (c->active[i] != 0)
                cur_map->cell[c->y + dir_dy[patch_dir[i]]][c->x + dir_dx[patch_dir[i]]]
                    = c->kind;
}

void far fx_clouds_tick(char now)                                   /* 3e88:2461 */
{
    struct Patch far *prev;
    struct Patch far *c;
    struct Patch far *next;
    char removed;
    unsigned char i;
    char x;
    char y;
    unsigned char q;
    char taken;
    char due;

    prev = 0;
    c = patch_list;
    removed = 0;
    while (c != 0) {
        next = c->next;
        if (now)
            due = c->strength == 0xff;
        else
            due = c->strength <= 1;
        if (due) {
            for (i = 1; i <= 9; i++) {
                if (c->active[i] != 0) {
                    x = c->x + dir_dx[patch_dir[i]];
                    y = c->y + dir_dy[patch_dir[i]];
                    taken = 0;
                    for (q = 1; q <= rec_count; q++)
                        if ((recs - 1)[q].hero != 0 && (recs - 1)[q].x == x
                            && (recs - 1)[q].y == y)
                            taken = 1;
                    if (taken)
                        cur_map->cell[y][x] = 0x1f;
                    else
                        cur_map->cell[y][x] = c->terrain[i];
                }
            }
            removed = 1;
            if (prev == 0)
                patch_list = next;
            else
                prev->next = next;
            pool_free(c, 0x21, &patch_pool);
        } else {
            prev = c;
            if (!now)
                c->strength--;
        }
        c = next;
    }
    if (removed) {
        fx_cloud_draw();
        bottom_msg("The air clears a little...");
    }
}

static void fx_hurt_immune(struct Hero far *h)                   /* 3e88:266d */
{
    char immune;

    immune = 0;
    if (((char)element_mask & 0x80) && (h->immunity & 0x10))
        immune = 1;
    if (((char)element_mask & 0x40) && (h->immunity & 8))
        immune = 1;
    if (immune) {
        damage = 0;
        effect_id = 0;
    }
}
