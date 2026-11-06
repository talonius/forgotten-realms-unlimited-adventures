/* CKIT.EXE overlay 02 module 2482: a computer-run combatant's turn.
 * Reading the keys the player may press meanwhile, choosing a temper,
 * checking morale, casting a spell, readying the best weapon, moving
 * toward the target direction by direction, and closing to attack.
 *
 * EXE 0x029710..0x02b21c, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 2482, the first module of RTLink section 2 (payload
 * 0x29710, link segment 2482; a child of section 0).  Its final retf is
 * byte b of paragraph 2632, where approach.c begins at byte c.  Built with
 * Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order;
 * bake consumes them in that order.  Section 1's 2482 frame holds other
 * bytes.
 *
 * A bare stored seg:off in this file means section 2.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; an ovl00_ or cbaction_, cbtutil_
 * or cbview_ callee is section 0's, reached by a direct far call because
 * section 0 is section 2's parent, and a mod18de_ callee is an RTLink
 * vector stub whose target is named beside it.
 *
 * ai_turn is reached through the vector stub 18de:1157 and turn_keys
 * through 18de:1161, combat.c's ai_turn and turn_keys; combat.c
 * calls the second with a null member.  ai_turn and ai_move far-call
 * turn_keys ahead of its definition.  The routines near-called ahead of
 * their definitions are static and prototyped; the others are static as
 * DQK's are, which the bytes do not decide.
 *
 * _DATA is dir_tries and seven literals, DGROUP 59b2..5a53.  _BSS is
 * b0f4..b21b: the spell list spell_pick builds with its cursor, the state
 * the move and weapon choices keep between calls, and junk, an out-value
 * find_effect fills that nothing reads.
 *
 * near_ids and heroes are indexed from 1; the shipped code adds the
 * address of element 0 (c354, b8f1).
 */

struct Effect;                  /* hero.c's; handled only by pointer      */

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28  index into item_table             */
    char pad_29[7];             /* +29  not read here                     */
    char plus;                  /* +30                                    */
    char pad_31[3];             /* +31  not read here                     */
    char cursed;                /* +34  nonzero: item_score gives 0       */
};

struct ItemType {               /* 0x10 bytes an item type                */
    unsigned char slot;         /* +0                                     */
    unsigned char hands;        /* +1                                     */
    char pad_2[3];              /* +2   not read here                     */
    unsigned char fire_rate;    /* +5                                     */
    char pad_6;                 /* +6   not read here                     */
    unsigned char weapon_class; /* +7                                     */
    char pad_8;                 /* +8   not read here                     */
    unsigned char dice_n;       /* +9                                     */
    unsigned char dice_s;       /* +a                                     */
    char dmg;                   /* +b                                     */
    unsigned char range;        /* +c                                     */
    unsigned char classes;      /* +d   tested against Hero.classes       */
    unsigned char flags;        /* +e                                     */
    char pad_f;                 /* +f   not read here                     */
};

struct Fight {                  /* a combatant's state in a fight         */
    unsigned char casting;      /* +0                                     */
    char can_cast;              /* +1                                     */
    char pad_2;                 /* +2   not read here                     */
    int initiative;             /* +3   turn_keys sets 20                 */
    char pad_5[2];              /* +5   not read here                     */
    unsigned char moves;        /* +7   half-moves left                   */
    char pad_8[2];              /* +8   not read here                     */
    unsigned char facing;       /* +a                                     */
    struct Hero far *target;    /* +b                                     */
    char pad_f[2];              /* +f   not read here                     */
    char panicked;              /* +11  nonzero: forced to flee           */
    unsigned char turn_attempts; /* +12                                    */
    char pad_13[2];             /* +13  not read here                     */
    char fleeing;               /* +15                                    */
    unsigned char temper;       /* +16  row of dir_tries                  */
};

struct Hero {                   /* a combatant                            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[4];              /* +4   not read here                     */
    struct Item far *items;     /* +8                                     */
    struct Item far *weapon;    /* +c                                     */
    struct Item far *off_hand;  /* +10                                    */
    char pad_14[0x24];          /* +14  not read here                     */
    struct Item far *arrows;    /* +38                                    */
    struct Item far *bolts;     /* +3c                                    */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x15];          /* +44  not read here                     */
    unsigned char cls;          /* +59                                    */
    char pad_5a[4];             /* +5a  not read here                     */
    unsigned char status;       /* +5e                                    */
    unsigned char side;         /* +5f                                    */
    char pad_60[0x13];          /* +60  not read here                     */
    unsigned char intel;        /* +73                                    */
    char pad_74[0xd];           /* +74  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[0x11];          /* +82  not read here                     */
    unsigned char control;      /* +93  bits 0-6: morale when bit 7 set   */
    char pad_94[0x19];          /* +94  not read here                     */
    unsigned char dice_n_base_main; /* +ad                                    */
    char pad_ae;                /* +ae  not read here                     */
    unsigned char dice_s_base_main; /* +af                                    */
    char pad_b0;                /* +b0  not read here                     */
    char dmg_base_main;         /* +b1                                    */
    char pad_b2[5];             /* +b2  not read here                     */
    unsigned char class_mask;   /* +b7                                    */
    char pad_b8[8];             /* +b8  not read here                     */
    unsigned char immunity;     /* +c0                                    */
    char pad_c1;                /* +c1  not read here                     */
    unsigned char hands;        /* +c2  hands in use                      */
    char pad_c3[3];             /* +c3  not read here                     */
    unsigned char spells[0x8d]; /* +c6                                    */
    char pad_153[0x2b];         /* +153 not read here                     */
    char active;                /* +17e                                   */
    char quick_fight;           /* +17f nonzero: the computer plays it    */
    char pad_180[0xb];          /* +180 not read here                     */
    unsigned char hp;           /* +18b                                   */
};

struct Map {
    char pad_0[8];              /* +0   not read here                     */
    char ignore_blocking;       /* +8   nonzero: terrain blocks nothing   */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    char pad_1[3];              /* +1   not read here                     */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    char pad_0[0xf];            /* +0   not read here                     */
    int in_combat;              /* +f   0: never cast here                */
    char pad_11;                /* +11  not read here                     */
    unsigned char ai_priority;  /* +12                                    */
    char pad_13[3];             /* +13  not read here                     */
};

struct Geo {                    /* the party's place                      */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
};

extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern unsigned char far game_key_get(void);                        /* 0d48:0052 */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern char far pool_lowbit(int value);                          /* 0a75:032f */
extern char far screen_sync(void);                               /* 0a75:0362 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern void far hero_panel(struct Hero far *h);                   /* 03d0:0834 */
extern char far is_helpless(struct Hero far *h);                   /* 03d0:09f2 */
extern unsigned char far calc_move(struct Hero far *h);          /* 03d0:0a38 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern void far fx_trigger_group(unsigned char when, struct Hero far * far *hp);
                                /* 18de:1111, vector to overlay 19 2e88:0449 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far spell_cast(unsigned char n, char a, char b, char far *done);
                                /* 18de:1175, vector to overlay 12 2b69:14c8 */
extern void far fx_terrain_apply(struct Hero far *h, char may_save, char skip_1d);
                                /* 18de:11a7, vector to overlay 19 2e88:0faa */
extern void far fx_out_of_play(struct Hero far *h, int how, char far *msg);
                                /* 18de:12d3, vector to overlay 19 2e88:16ef */
extern void far item_equip_toggle(struct Item far *item);
                                /* 18de:135f, vector to overlay 14 28aa:19bf */
extern unsigned char far combat_targets_near(struct Hero far *h, unsigned char range, char same);
                                                                 /* overlay 0 1cb8:0971 */
extern unsigned char far get_distance(struct Hero far *h, struct Hero far *t);
                                                                 /* overlay 0 1cb8:0a99 */
extern char far combat_actions_end(struct Hero far *h);                /* overlay 0 1cb8:0b84 */
extern char far combat_guard(struct Hero far *h);                /* overlay 0 1cb8:0bc5 */
extern char far has_ranged_weapon(struct Hero far *h);                /* overlay 0 1cb8:0c32 */
extern char far has_throwable_weapon(struct Hero far *h);                /* overlay 0 1cb8:0c71 */
extern char far bandage_dying(char apply);                        /* overlay 0 1cb8:0cc8 */
extern char far ranged_ammo_find(struct Hero far *h, struct Item far * far *sel);
                                                                 /* overlay 0 1cb8:0fb5 */
extern void far combat_move_step(struct Hero far *h, unsigned char dir); /* overlay 0 1dbf:06b2 */
extern void far opportunity_attacks(struct Hero far *h, unsigned char dir); /* overlay 0 1dbf:08d8 */
extern char far combat_flee(struct Hero far *h);             /* overlay 0 1dbf:0d02 */
extern void far attacks_left_recalc(struct Hero far *h);             /* overlay 0 1dbf:0db4 */
extern char far combat_target_allowed(struct Hero far *h, struct Hero far *t, char any);
                                                                 /* overlay 0 1dbf:0f05 */
extern void far turn_undead(struct Hero far *h);             /* overlay 0 1dbf:0fec */
extern char far turn_target_find(struct Hero far *h, struct Hero far * far *target,
                              int far *level);                   /* overlay 0 1dbf:1229 */
extern void far attack_register(struct Hero far *h, struct Hero far *t); /* overlay 0 1dbf:170a */
extern void far combat_attack(struct Hero far *h, struct Hero far *t, char mode,
                                struct Item far *missile, char far *done);
                                                                 /* overlay 0 1dbf:178d */
extern void far combat_cast_begin(unsigned char spell, char mode, char far *done);
                                                                 /* overlay 0 1dbf:23a6 */
extern unsigned char far fastest_enemy_speed(struct Hero far *h);      /* overlay 0 1dbf:28c5 */
extern char far combat_target_pick(struct Hero far *h, char range, char any, char fresh,
                              char flag);                        /* overlay 0 1dbf:3bf5 */
extern char far gods_intervene(char skip_check);                /* overlay 0 1dbf:3de3 */
extern char far spell_target_rating(struct Hero far *h, unsigned char spell); /* overlay 0 1dbf:4530 */
extern char far combat_sweep_attack(struct Hero far *h, struct Hero far *t); /* overlay 0 1dbf:50d1 */
extern char far combatant_visible(struct Hero far *h, char whole);     /* overlay 0 22fa:0627 */
extern void far combat_view_scroll(char x, char y, unsigned char margin, unsigned char dir);
                                                                 /* overlay 0 22fa:0847 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame, char erase);
                                                                 /* overlay 0 22fa:09a2 */
extern char far combatant_x(struct Hero far *h);                 /* overlay 0 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h);                 /* overlay 0 22fa:0ae4 */
extern void far combat_direction_probe(struct Hero far *h, unsigned char dir, unsigned char far *who,
                            unsigned char far *cell, unsigned char far *kind);
                                                                 /* overlay 0 22fa:0b9c */
extern unsigned char far combatant_direction(struct Hero far *from, struct Hero far *to);
                                                                 /* overlay 0 22fa:166c */
extern char far line_clear(char x, char y, char far *tx, char far *ty,
                           unsigned short far *cost);            /* 2632:0325 */

extern struct SpellInfo spell_info[];                            /* 264a */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern struct ItemType far *item_table;                          /* b40a */
extern char turn_done;                                           /* b414 */
extern char magic_on;                                            /* b728 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Hero far *heroes[];                                /* b8f5 */
extern unsigned char near_ids[];                                 /* c355 */
extern char can_reach;                                           /* c399 */
extern struct Map far *cur_map;                                  /* c66b */
extern unsigned char morale;                                     /* ca11 */
extern unsigned char ai_level;                                   /* ca21 */
extern char move_flag;                                           /* cc79 */
extern unsigned char morale_base;                                /* d0fc */
extern struct Hero far *node_list;                               /* d129 */
extern struct Geo geo;                                           /* d414 */

char far turn_keys(struct Hero far *h);
static char try_turn_undead(struct Hero far *h);
static char ai_no_action(struct Hero far *h);
static char try_spell(struct Hero far *h);
static char ai_attack(struct Hero far *h);
static char turn_end(struct Hero far *h);
static char morale_check(struct Hero far *h);
static void arm_best(struct Hero far *h, struct Hero far *target);
static unsigned char spell_pick(struct Hero far *h, struct Effect far * far *spare);

static unsigned char dir_tries[7][6] = { /* by temper: turns from the heading */
    { 0, 8, 7, 6, 1, 2 },
    { 0, 8, 7, 6, 1, 2 },
    { 0, 8, 1, 2, 7, 6 },
    { 0, 7, 1, 8, 6, 2 },
    { 0, 1, 7, 8, 2, 6 },
    { 0, 8, 7, 6, 5, 4 },
    { 0, 8, 1, 2, 3, 4 }
};

static int at;
static unsigned char spl;
static struct Effect far *junk;
static unsigned sorted[0x8d];
static unsigned char old_dir;
static unsigned char retries;
static unsigned char standstill;
static char foe_halves_a;
static unsigned char need_bonus;
static char foe_resists_b;
static unsigned char missile_needs;

void far ai_turn(struct Hero far *h)                             /* 3482:0000 */
{
    unsigned char temper;
    char done;

    if (h->fight->target == 0)
        combat_target_pick(h, 0xff, 1, 0, 0);
    done = turn_keys(h);
    box_draw();
    combat_text_clear();
    if (!h->active)
        done = combat_actions_end(h);
    temper = h->fight->temper;
    if (temper == 0 || temper > 4 || fx_roll_dice(1, 4) == 1) {
        temper = fx_roll_dice(1, 8);
        if (temper == 8)
            temper = fx_roll_dice(1, 2) + 4;
        else
            temper = fx_roll_dice(1, 4);
    }
    h->fight->temper = temper;
    if (!done)
        done = morale_check(h);
    if (h->fight->fleeing && !h->fight->panicked)
        hero_text(h, "flees in panic", 10, 1);
    if (done)
        return;
    if (ai_no_action(h)) {
        done = combat_actions_end(h);
    } else if (h->fight->casting > 0) {
        spell_cast(h->fight->casting, 1, 1, &done);
        done = combat_actions_end(h);
    } else if (try_turn_undead(h)) {
        turn_done = combat_actions_end(h);
    } else if (try_spell(h)) {
        done = 1;
    } else {
        arm_best(h, h->fight->target);
        done = turn_keys(h);
        while (!done) {
            if (combat_target_pick(h, 0xff, 1, 0, 0) && h->fight->initiative > 0 && h->active)
                done = ai_attack(h);
            else
                done = turn_end(h);
        }
    }
}

static char try_turn_undead(struct Hero far *h)                      /* 3482:0240 */
{
    char r;
    struct Hero far *spot;
    int n;

    r = 0;
    if (!h->fight->fleeing) {
        n = 0xd;
        if (h->fight->turn_attempts < ai_level
            && (class_level(h, 0) > 0 || class_level(h, 3) > 2)
            && turn_target_find(h, &spot, &n)) {
            r = 1;
            turn_undead(h);
        }
    }
    return r;
}

static char ai_no_action(struct Hero far *h)                      /* 3482:02cd */
{
    char r;

    r = 0;
    return r;
}

static char try_spell(struct Hero far *h)                        /* 3482:02de */
{
    char r;
    struct Effect far *spare;
    char found;
    unsigned char spell;
    unsigned char sum;

    r = 0;
    if (!h->fight->fleeing && !cursor_obj[2] && h->fight->can_cast
        && (h->control >= 0x7f || magic_on)) {
        found = 0;
        spell = spell_pick(h, &spare);
        sum = class_level(h, 2) + class_level(h, 3) + class_level(h, 4)
              + class_level(h, 1);
        sum = sum * 4;
        if (spell && fx_roll_dice(1, 100) > sum
            && fx_roll_dice(1, 5) <= spell_info[spell].ai_priority)
            combat_cast_begin(spell, 1, &found);
        r = found;
    }
    return r;
}

static char step_ok(struct Hero far *h, unsigned char i, unsigned char turn,
                    char far *blocked)                           /* 3482:0410 */
{
    char r;
    struct Effect far *spare;
    unsigned char rel;
    unsigned char dir;
    unsigned char who;
    unsigned char cell;
    unsigned char cost;
    char ok;
    unsigned char kind;

    *blocked = 0;
    ok = 0;
    rel = dir_tries[h->fight->temper][i];
    dir = (turn + rel) % 8;
    combat_direction_probe(h, dir, &who, &cell, &kind);
    if (cell == 0)
        *blocked = 1;
    else if ((tile_info - 1)[cell].move_cost == 0xff) {
        r = 0;
        return r;
    } else {
        if (pool_lowbit(dir))
            cost = (tile_info - 1)[cell].move_cost * 3;
        else
            cost = (tile_info - 1)[cell].move_cost * 2;
        if (!who && h->fight->moves > cost) {
            switch (kind) {
            case 0x1d:
                if (!find_effect(h, 0x1e, &spare) && !find_effect(h, 0x8f, &spare)
                    && !find_effect(h, 0x3f, &spare) && !find_effect(h, 0x39, &spare)
                    && !h->fight->panicked && !(h->immunity & 0x18))
                    cost = h->fight->moves + 1;
                break;
            case 0x1c:
                if (find_effect(h, 0x39, &spare) || find_effect(h, 0x8f, &spare)
                    || (h->immunity & 0x10))
                    break;
            case 0x1b:
                if (!h->fight->panicked)
                    cost = h->fight->moves + 1;
                break;
            }
            if (h->fight->moves >= cost)
                ok = 1;
        }
    }
    r = ok;
    return r;
}

static void ai_move(struct Hero far *h)                          /* 3482:063a */
{
    char blocked;
    char done;
    unsigned char dir;
    unsigned char dir2;
    unsigned char i;

    done = 0;
    if (h->fight->moves / 2 < 10)
        status_printf(0, 0x18, 7, 0, msg_format("Move/Attack, Move Left = %s ",
                                                byte_str(h->fight->moves / 2)));
    else
        status_printf(0, 0x18, 7, 0, msg_format("Move/Attack, Move Left = %s",
                                                byte_str(h->fight->moves / 2)));
    if (turn_keys(h))
        return;
    if (h->fight->moves / 2 > 0 && h->fight->initiative > 0
        && (!(h->control & 0x80)
            || (h->control & 0x80) && fx_roll_dice(1, 100) + morale >= morale_base
            || h->side == 1)
        && (h->fight->fleeing || h->cls != 5)) {
        if (!h->fight->fleeing)
            dir = combatant_direction(h, h->fight->target);
        else {
            h->fight->temper = fx_roll_dice(1, 2);
            dir = geo.facing - (geo.facing + 2) % 4 / 2 + 1;
            if (h->side == 0)
                dir = dir + 4;
            dir = dir % 8;
        }
        blocked = 0;
        done = 0;
        i = 1;
        while (i < 6 && !done && !step_ok(h, i, dir, &blocked)) {
            if (h->fight->fleeing && blocked)
                done = combat_flee(h);
            else
                i++;
        }
        if (done) {
            h->fight->moves = 0;
            h->fight->fleeing = 0;
            done = combat_actions_end(h);
        } else {
            if (i < 6 && h->fight->temper < 7)
                dir2 = (dir_tries[h->fight->temper][i] + dir) % 8;
            if (i == 6 || (dir2 + 4) % 8 == old_dir) {
                retries++;
                h->fight->temper = h->fight->temper % 6 + 1;
                if (retries > 1) {
                    h->fight->target = 0;
                    if (retries > 2) {
                        h->fight->moves = 0;
                        done = 1;
                    } else if (!combat_target_pick(h, 0xff, 1, 0, 0))
                        done = turn_end(h);
                }
            }
            if (i < 6)
                old_dir = dir2;
            else
                done = 1;
        }
        if (done)
            return;
        move_flag = can_reach || combatant_visible(h, 0) || h->side == 0;
        combatant_facing_set(h, dir2, 0, 0);
        opportunity_attacks(h, h->fight->facing);
        if (!h->active) {
            done = combat_actions_end(h);
            return;
        }
        if (h->fight->moves > 0)
            combat_move_step(h, h->fight->facing);
        if (!h->active || is_helpless(h))
            done = combat_actions_end(h);
        fx_terrain_apply(h, 0, 0);
    } else {
        if (h->fight->moves / 2 == 0 && !h->fight->fleeing)
            done = try_spell(h);
        if (!done)
            done = turn_end(h);
    }
}

static char ai_attack(struct Hero far *h)                        /* 3482:0ac8 */
{
    char r;
    char done;
    char going;
    unsigned char range;
    unsigned short cost;
    unsigned char count;
    unsigned char pick;
    struct Item far *aim;
    struct Hero far *target;
    char tx;
    char ty;
    unsigned char cnt;

    old_dir = 8;
    retries = 0;
    standstill = 0;
    cnt = 0;
    going = 1;
    done = 0;
    fx_trigger_group(0xe, &h);
    if (h->side == 0 && bandage_dying(1))
        h->fight->initiative = 0;
    if (h->fight->initiative == 0)
        going = 0;
    while (!done && going) {
        if (h->fight->fleeing) {
            while (h->fight->moves > 0 && h->fight->initiative > 0
                   && h->fight->initiative < 20)
                ai_move(h);
        }
        if (h->fight->initiative == 0 || h->fight->initiative == 20)
            going = 0;
        else
            done = 0;
        if (!done && going) {
            cnt++;
            if (cnt > 20) {
                done = 1;
                going = !turn_end(h);
            }
            can_reach = 0;
            range = 1;
            if (h->weapon != 0)
                range = item_table[h->weapon->type].range;
            if (range == 0 || range == 0xff)
                range = 1;
            target = h->fight->target;
            if (!target->active || target->side == 0)
                target = 0;
            if (target != 0 && combat_target_allowed(h, target, 0)) {
                tx = combatant_x(target);
                ty = combatant_y(target);
                cost = range;
                cur_map->ignore_blocking = 0;
                if (line_clear(combatant_x(h), combatant_y(h), &tx, &ty, &cost)
                    && cost / 2 <= range)
                    can_reach = 1;
            }
            if (!can_reach) {
                count = combat_targets_near(h, range, 0);
                if (count == 0) {
                    if (combat_target_pick(h, 0xff, 0, 0, 0)) {
                        ai_move(h);
                        if (h->fight->casting > 0)
                            going = 0;
                    } else
                        done = turn_end(h);
                } else {
                    pick = fx_roll_dice(1, count);
                    target = (heroes - 1)[(near_ids - 1)[pick]];
                    if (has_ranged_weapon(h) && !has_throwable_weapon(h)
                        && combat_targets_near(h, 1, 0) > 0) {
                        arm_best(h, h->fight->target);
                        done = 1;
                    } else if (get_distance(h, target) == 1
                               || combat_target_allowed(h, target, 0))
                        can_reach = 1;
                }
            }
            if (can_reach)
                combat_view_scroll(combatant_x(h), combatant_y(h), 2,
                            combatant_direction(h, target));
            if (can_reach) {
                if (combat_sweep_attack(h, target))
                    done = combat_actions_end(h);
                else {
                    attack_register(h, target);
                    aim = 0;
                    if (has_ranged_weapon(h)) {
                        can_reach = ranged_ammo_find(h, &aim);
                        if (has_throwable_weapon(h) && get_distance(h, target) == 1)
                            aim = 0;
                    }
                    combat_attack(h, target, 0, aim, &done);
                    if (done)
                        going = 0;
                    else if (!target->active)
                        done = 1;
                }
            }
        }
    }
    r = !going;
    return r;
}

static char turn_end(struct Hero far *h)                         /* 3482:0f6c */
{
    char r;

    combat_text_clear();
    if (is_helpless(h) || has_ranged_weapon(h) || h->fight->initiative == 0)
        r = combat_actions_end(h);
    else
        r = combat_guard(h);
    return r;
}

char far turn_keys(struct Hero far *h)                           /* 3482:0fd0 */
{
    char r;
    struct Hero far *p;
    unsigned char key;

    r = 0;
    if (screen_sync()) {
        key = game_key_get();
        switch (key) {
        case 0xcc:
            magic_on = !magic_on;
            if (magic_on)
                bottom_msg("Magic On");
            else
                bottom_msg("Magic Off");
            break;
        case 0x20:
            for (p = node_list; p != 0; p = p->next)
                if (p->control < 0x80 && p->status != 1)
                    p->quick_fight = 0;
            if (h != 0 && !h->quick_fight) {
                h->fight->initiative = 20;
                r = 1;
            }
            break;
        case 0xd7:
            gods_intervene(0);
            break;
        }
    }
    return r;
}

static char morale_check(struct Hero far *h)                     /* 3482:10a4 */
{
    char r;
    unsigned char x;

    r = 0;
    h->fight->fleeing = 0;
    if (h->fight->panicked) {
        h->fight->fleeing = 1;
        hero_text(h, "is forced to flee", 10, 1);
    } else if (h->control & 0x80) {
        morale = (h->control & 0x7f) * 2;
        fx_trigger_group(0x11, &h);
        if (100 - h->hp * 100 / h->hp_max > morale || !morale) {
            morale = morale_base;
            fx_trigger_group(0x11, &h);
            if (100 - cursor_obj[0x1b] > morale || !morale || h->side == 0) {
                x = fastest_enemy_speed(h);
                if (calc_move(h) / 2 >= x)
                    h->fight->fleeing = 1;
                else if (h->intel > 5) {
                    fx_out_of_play(h, 4, "Surrenders");
                    r = combat_actions_end(h);
                }
            }
        }
    }
    return r;
}

static unsigned char item_score(struct Hero far *h,
                                struct Item far *item)           /* 3482:11fe */
{
    unsigned char r;
    unsigned char score;
    struct ItemType info;

    info = item_table[item->type];
    score = info.dice_s * info.dice_n;
    if (item->plus > 0)
        score = score + item->plus * 8;
    if (info.dmg > 0)
        score = score + info.dmg * 2;
    if ((info.flags & 8) > 0)
        score = score + (info.fire_rate - 1) * 2;
    if (info.hands <= 1)
        score = score + 3;
    if (h->hands + info.hands >= 3)
        score = 0;
    if (item->cursed)
        score = 0;
    if (info.weapon_class <= 1 && foe_halves_a || (info.weapon_class & 0x80) && foe_resists_b)
        score = score / 2;
    if ((info.flags & 8) || (info.flags & 0x10)) {
        if (item->plus < missile_needs)
            score = 0;
    } else if (item->plus < need_bonus)
        score = 0;
    r = score;
    return r;
}

static void arm_best(struct Hero far *h, struct Hero far *target) /* 3482:130e */
{
    struct Item far *missile;
    struct Item far *melee;
    struct Item far *ammo;
    struct Item far *it;
    struct Item far *hand;
    unsigned char best_missile;
    unsigned char best_melee;
    unsigned char best_ammo;
    unsigned char score;
    unsigned char type;
    unsigned char flags;
    char ok;
    char changed;
    char both;
    char two;

    need_bonus = 0;
    if (target != 0) {
        if (find_effect(target, 0x7d, &junk))
            need_bonus = 2;
        if (find_effect(target, 0x3c, &junk))
            need_bonus = 3;
        if (find_effect(target, 0x50, &junk) || find_effect(target, 0x51, &junk)
            || find_effect(target, 0x67, &junk))
            need_bonus = 1;
        missile_needs = need_bonus;
        if (!missile_needs && find_effect(target, 0x29, &junk))
            missile_needs = 1;
        foe_halves_a = find_effect(target, 0x42, &junk);
        foe_resists_b = find_effect(target, 0x43, &junk);
    } else {
        missile_needs = 0;
        foe_resists_b = 0;
        foe_halves_a = 0;
    }
    if (h->weapon != 0)
        h->hands = h->hands - item_table[h->weapon->type].hands;
    if (h->off_hand != 0)
        h->hands = h->hands - item_table[h->off_hand->type].hands;
    missile = 0;
    melee = 0;
    ammo = 0;
    best_missile = 1;
    best_melee = h->dice_s_base_main * h->dice_n_base_main;
    if (h->dmg_base_main > 0)
        best_melee = best_melee + h->dmg_base_main * 2;
    best_ammo = 0;
    for (it = h->items; it != 0; it = it->next) {
        type = it->type;
        switch (item_table[type].slot) {
        case 0:
            if (item_table[type].classes & h->class_mask) {
                score = item_score(h, it);
                if (((item_table[type].flags & 8) || (item_table[type].flags & 0x10))
                    && score > best_missile) {
                    missile = it;
                    best_missile = score;
                }
                if (!(item_table[type].flags & 8) && score > best_melee) {
                    melee = it;
                    best_melee = score;
                }
            }
            break;
        }
        if ((int)item_table[type].slot == 1 && (item_table[type].classes & h->class_mask) > 0) {
            if (it->plus >= 0)
                score = it->plus + 1;
            else
                score = 0;
            if (score > best_ammo) {
                ammo = it;
                best_ammo = score;
            }
        }
    }
    if (missile != 0 && item_table[missile->type].range > 1
        && (item_table[missile->type].flags & 0x14) == 0x14)
        both = 1;
    else
        both = 0;
    two = 0;
    hand = 0;
    flags = 0;
    if (missile != 0) {
        flags = item_table[missile->type].flags;
        if (flags & 0x10)
            hand = missile;
        if (flags & 8) {
            if (flags & 1)
                hand = h->arrows;
            if (flags & 0x80)
                hand = h->bolts;
        }
    }
    if (hand != 0 || flags == 0xa)
        two = 1;
    if (missile != 0 && best_melee / 2 < best_missile && two
        && (both || !combat_targets_near(h, 1, 0)))
        it = missile;
    else
        it = melee;
    changed = 0;
    ok = 1;
    if (h->weapon != 0 && (h->weapon == it || h->weapon->cursed))
        ok = 0;
    if (ok) {
        if (h->weapon != 0)
            item_equip_toggle(h->weapon);
        recalc_stats(h);
        if (h->off_hand != 0 && !h->off_hand->cursed)
            h->hands = h->hands - item_table[h->off_hand->type].hands;
        if (it != 0)
            item_equip_toggle(it);
        changed = 1;
    }
    recalc_stats(h);
    attacks_left_recalc(h);
    ok = 1;
    if (h->off_hand != 0 && (h->off_hand == ammo || h->off_hand->cursed))
        ok = 0;
    if (h->hands > 2) {
        if (h->off_hand == 0 || h->off_hand->cursed)
            item_equip_toggle(it);
        else
            item_equip_toggle(h->off_hand);
        changed = 1;
    } else if (h->hands < 2 && ok) {
        if (h->off_hand != 0)
            item_equip_toggle(h->off_hand);
        recalc_stats(h);
        if (ammo != 0)
            item_equip_toggle(ammo);
        changed = 1;
    }
    recalc_stats(h);
    if (changed)
        hero_panel(h);
}

static void spell_next(void)                                     /* 3482:191d */
{
    char found;

    found = 0;
    do {
        if (sorted[at] == 0) {
            found = 1;
            spl = 0;
        } else if (sorted[at] != spl) {
            spl = sorted[at];
            found = 1;
        } else
            at++;
    } while (!found);
}

static void spells_sort(unsigned far *list, struct Hero far *h)  /* 3482:1974 */
{
    unsigned char spell;
    unsigned char tmp;
    unsigned char i;
    unsigned char j;
    char placed;

    for (i = 0; i <= 0x8c; i++) {
        spell = h->spells[i];
        if (spell > 0 && spell < 0x80 && spell_info[spell].in_combat != 0) {
            placed = 0;
            j = 0;
            do {
                if (list[j] == 0) {
                    list[j] = spell;
                    placed = 1;
                } else if (spell_info[list[j]].ai_priority > spell_info[spell].ai_priority)
                    j++;
                else {
                    do {
                        tmp = list[j];
                        list[j] = spell;
                        spell = tmp;
                        j++;
                    } while (spell != 0 && j <= 0x8c);
                    placed = 1;
                }
            } while (!placed);
        }
    }
}

static unsigned char spell_pick(struct Hero far *h,
                                struct Effect far * far *spare)  /* 3482:1a8b */
{
    unsigned char r;
    char found;

    r = 0;
    mem_zero(sorted, sizeof sorted);
    spells_sort(sorted, h);
    found = 0;
    spl = 0;
    at = 0;
    do {
        spell_next();
        if (spl)
            found = spell_target_rating(h, spl) > 0;
    } while (!found && spl);
    r = spl;
    return r;
}
