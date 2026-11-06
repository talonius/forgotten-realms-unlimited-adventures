/* CKIT.EXE overlay 11 module 2a6a: effect handlers in combat.
 * The handlers effects.c's fx_install and fx_install_2 store into
 * fx_handler with segment 2a6a, and one helper effects.c far-calls,
 * fx_poison_attack, a poison attack: effect 0x37 on the attacker's target,
 * and death if it takes.  The handlers play snakes, charm, silence,
 * nausea, dispelling, confusion and berserk rages, turning to stone, lost
 * intellect and memorized spells, fear, beheading, the level drains,
 * extra damage by the target's flags, and the monsters' breath weapons
 * and gaze.  Every handler takes the member, the effect record and
 * whether the effect is being removed.
 *
 * EXE 0x053364..0x05463c, one Turbo C large-model _TEXT segment starting at
 * byte 4 of paragraph 2a6a, the last module of RTLink section 11 (payload
 * 0x4faf0, link segment 26e3, which sections 12 and 13 also use).  The
 * first 4 bytes of paragraph 2a6a are the tail of module 26e3 (effects.c),
 * and four zero bytes follow to the section's end at 0x054640.  Built with
 * Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order;
 * bake consumes them in that order.
 *
 * Every routine after the lone retf at 2a6a:0004 is reached as a far
 * pointer from effects.c's installers or as a direct far call from
 * effects.c, so all are public; nothing in the section near-calls them.
 * fx_confusion's roll of 1..100 compiles to a 100-entry jump table, and
 * fx_breath_weapon's switch on four sparse effect kinds to Turbo C's value
 * search through a table in the code segment (2a6a:1060); with any other
 * kind it leaves lo, hi and name unset, as shipped.
 *
 * A PORT of Dark Queen of Krynn's fxcombat.c (overlay 9 module 30cc).
 * FRUA's module has no counterpart of DQK's 0075, 0bc0, 0c28, 0d07, 0d36,
 * 0ded and 0e24; 0071 is its own; 0bbc repeats 0ad5's body byte for byte
 * for slot 121, where DQK stores 0ad9 in both slots 108 and 121.  Hero +5f
 * (side) is a byte here where DQK's (+66) is a word.
 *
 * A bare stored seg:off in this file means section 11.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are the ones the landed modules give each address, placeholders
 * included; the handlers keep the names effects.c gives them.  A
 * mod18de_ callee is an RTLink vector stub; its comment names the routine
 * the stub reaches.
 *
 * _DATA is DGROUP 81e2..82e4, the literals; -d stores "is %s" and "goes
 * berserk" once each.  The module has no _BSS.
 */

enum Side { SIDE_A, SIDE_B };  /* the (enum Side) casts keep mov ah,0 at 2a6a:08f0, 0aa4, 0aca */

struct Effect {                 /* 10 bytes                               */
    unsigned char kind;         /* +0                                     */
    char pad_1;                 /* +1   not read here                     */
    unsigned duration;          /* +2   not read here                     */
    unsigned char strength;     /* +4                                     */
    char pad_5[5];              /* +5   not read here                     */
};

struct Fight {                  /* a combatant's state in a fight         */
    unsigned char casting;      /* +0   spell being cast                  */
    char can_cast;              /* +1                                     */
    char can_use_item;          /* +2                                     */
    int initiative;             /* +3                                     */
    char pad_5[2];              /* +5   not read here                     */
    unsigned char moves;        /* +7                                     */
    char pad_8[3];              /* +8   not read here                     */
    struct Hero far *target;    /* +b                                     */
    char pad_f[2];              /* +f   not read here                     */
    char panicked;              /* +11                                    */
};

struct Step {
    unsigned char who;          /* +0   index into heroes                 */
    unsigned char cost;         /* +1   not read here                     */
    unsigned char dir;          /* +2   not read here                     */
};

struct Hero {                   /* a party member or a monster            */
    char pad_0[0x40];           /* +0   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x1b];          /* +44  not read here                     */
    unsigned char side;         /* +5f                                    */
    char pad_60[0x13];          /* +60  not read here                     */
    unsigned char intel;        /* +73                                    */
    char pad_74;                /* +74  not read here                     */
    unsigned char wis;          /* +75                                    */
    char pad_76[0xb];           /* +76  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[0x11];          /* +82  not read here                     */
    unsigned char control;      /* +93                                    */
    char pad_94[0x2b];          /* +94  not read here                     */
    unsigned char type_flags;   /* +bf                                    */
    unsigned char immunity;     /* +c0                                    */
    char pad_c1[5];             /* +c1  not read here                     */
    unsigned char spells[0x8d]; /* +c6  bit 7: still to memorize          */
    char pad_153[0x2c];         /* +153 not read here                     */
    char quick_fight;           /* +17f                                   */
    char pad_180;               /* +180 not read here                     */
    unsigned char ac;           /* +181                                   */
    unsigned char ac_rear;      /* +182                                   */
    unsigned char attacks_main; /* +183                                   */
    unsigned char attacks_off;  /* +184                                   */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far hero_panel(struct Hero far *h);                   /* 03d0:0834 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern char far str_damage(struct Hero far *h);                   /* 03d0:1234 */
extern void far spell_mark(struct Hero far *h, unsigned char spell); /* 03d0:1381 */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far text_pause(void);                                /* 06c9:093c */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far fx_disable(struct Hero far *h, int how, char far *msg);
                                /* 18de:10e9, vector to overlay 19 2e88:000f */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern void far combat_sides_count(void);  /* 18de:111b, vector to overlay 0 1cb8:091f */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern char far combat_actions_end(struct Hero far *h); /* 18de:1139, vector to overlay 0 1cb8:0b84 */
extern char far combatant_x(struct Hero far *h); /* 18de:117f, vector to overlay 0 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h); /* 18de:1189, vector to overlay 0 22fa:0ae4 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame, char erase);
                                /* 18de:11cf, vector to overlay 0 22fa:09a2 */
extern void far icon_fly(int x0, int y0, int x1, int y1, char a, char b);
                                /* 18de:121f, vector to overlay 0 1cb8:00de */
extern void far hit_flash(struct Hero far *h, char a, char far *msg);
                                /* 18de:123d, vector to overlay 0 1cb8:0730 */
extern int  far enemy_team(struct Hero far *h);
                                /* 18de:1247, vector to overlay 0 1cb8:08fc */
extern void far icon_facings(char n);
                                /* 18de:125b, vector to overlay 0 1cb8:0052 */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern unsigned char far combatant_size(struct Hero far *h);
                                /* 18de:128d, vector to overlay 0 22fa:0b14 */
extern int  far fx_roll_damage(unsigned char n, unsigned char sides);
                                /* 18de:12ab, vector to overlay 19 2e88:1624 */
extern unsigned char far combatant_direction(struct Hero far *h, struct Hero far *t);
                                /* 18de:12e7, vector to overlay 0 22fa:166c */
extern char far fx_saving_throw(struct Hero far *h, int type, char adj);
                                /* 18de:1323, vector to overlay 19 2e88:1552 */
extern void far fx_damage(struct Hero far *h, unsigned char dmg, int reduce,
                             char saved);
                                /* 18de:132d, vector to overlay 19 2e88:1ea2 */
extern void far spell_run(char n);
                                /* 18de:1ae9, vector to overlay 12 2b69:217d */
extern void far fx_add_message(struct Hero far *h, unsigned char kind, int dur,
                             unsigned char strength, char on_remove, int save_negates,
                             char saved, char far *msg);
                                /* 18de:1c97, vector to overlay 19 2e88:2181 */
extern void far spell_path_targets(char x, char y, char tx, char ty, char a, char b);
                                /* 18de:1ca1, vector to overlay 12 2b69:1a61 */
extern void far bolt_run(unsigned range, int dmg, char bounce);
                                /* 18de:1cb5, vector to overlay 12 26e3:1706 */
extern void far fx_end_actions(struct Hero far *h, struct Effect far *fx, char removed);
                                                                 /* 26e3:010d */
extern void far fx_drain(struct Hero far *h, unsigned char count); /* 26e3:17b4 */
extern void far fx_dispatch(unsigned char id, struct Hero far *h, struct Effect far *fx,
                            char removed);                       /* 26e3:3831 */

extern char far *fx_names[];                                     /* 6379 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char turn_done;                                           /* b414 */
extern unsigned char combat_round;                                  /* b418 */
extern int view_mode;                                            /* b421 */
extern unsigned char move_rate;                                  /* b427 */
extern int damage;                                               /* b42f */
extern char refused;                                             /* b432 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern unsigned char movement_stops_attack_rate;                                  /* be61 */
extern unsigned char current_spell;                                  /* be64 */
extern struct Hero far *fx_target;                               /* c350 */
extern char view_cx;                                             /* c39f */
extern char view_cy;                                             /* c3bc */
extern int element_mask;                                         /* c64e */
extern char mirror_image_bypass;                                           /* ca08 */
extern unsigned char morale;                                     /* ca11 */
extern char attack_roll;                                           /* ca1d */
extern struct Step steps[];                                      /* ca2b */
extern struct Hero far *targets[];                               /* cfe3 */
extern char crit_hit;                                            /* d0fd */
extern void (far *hit_proc)(unsigned char spell, char mode, char far *result); /* d131 */
extern unsigned char target_count;                               /* d137 */

/* The translated Pascal unit's empty initialization block (DKK effhndlr.pas). */
void far fxcombat_unit_init(void)                                   /* 3a6a:0004 */
{
}

void far fx_snakes(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0005 */
{
    unsigned char n;

    n = h->attacks_main + h->attacks_off;
    if (fx->strength > n)
        fx->strength -= n;
    else
        fx_remove(h, 3, 0);
    hit_flash(h, 1, "is fighting with snakes");
    combat_text_clear();
    combat_actions_end(h);
}

void far fx_attack_bonus_vs_immunity4(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0071 */
{
    if (h->fight != 0 && !removed) {
        fx_target = h->fight->target;
        if (fx_target->immunity & 4)
            attack_roll++;
    }
}

void far fx_charm(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:00a5 */
{
    if (h->fight != 0) {
        if (removed) {
            h->side = (fx->strength & 0x40) / 0x40;
            if (h->control == 0xb3) {
                h->control = 0;
                h->quick_fight = 0;
            }
        } else {
            if (!(fx->strength & 0x20)) {
                fx->strength = fx->strength + (h->side << 6) + 0x20;
                h->side = fx->strength / 0x80;
                h->quick_fight = 1;
                if (!(h->control & 0x80))
                    h->control = 0xb3;
                h->fight->target = 0;
                combat_sides_count();
            }
            morale = 100;
        }
    }
}

void far fx_silence(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0157 */
{
    if (h->fight != 0 && !removed) {
        if (h->fight->can_use_item)
            hero_text(h, msg_format("is %s", fx_names[21]), 10, 1);
        if (h->fight->casting > 0)
            spell_mark(h, h->fight->casting);
        h->fight->casting = 0;
        h->fight->can_use_item = 0;
        h->fight->can_cast = 0;
    }
}

void far fx_attack_bonus_vs_type80(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:01f2 */
{
    if (h->fight != 0 && !removed) {
        fx_target = h->fight->target;
        if (fx_target->type_flags & 0x80)
            attack_roll++;
    }
}

void far fx_nausea(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0226 */
{
    if (h->fight != 0 && view_mode == 5) {
        if (h->fight->can_use_item)
            hero_text(h, "is coughing", 10, 1);
        h->fight->can_use_item = 0;
        h->fight->can_cast = 0;
        recalc_stats(h);
        h->ac -= 2;
        h->ac_rear -= 2;
        if (h == cur_node)
            hero_panel(h);
    }
}

void far fx_dispel_evil_attack(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:02b7 */
{
    if (h->fight != 0 && !removed) {
        fx_target = h->fight->target;
        if ((char)(fx_target->type_flags & 1) & !fx_saving_throw(fx_target, 4, 0)) { /* &, as shipped */
            fx_disable(fx_target, 8, "is dispelled");
            fx_remove(h, 4, 0);
            fx_remove(h, 0x20, 0);
        } else
            hero_text(fx_target, "resists dispel evil", 10, 1);
    }
}

void far fx_confusion(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:036a */
{
    unsigned char roll;
    char saved;

    if (h->fight != 0 && !removed) {
        roll = fx_roll_dice(1, 100);
        switch (roll) {
        case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
        case 10:
            fx_remove(h, 0x23, 0);
            h->fight->panicked = 1;
            h->quick_fight = 1;
            if (!(h->control & 0x80))
                h->control = 0xb3;
            h->fight->target = 0;
            fx_add_message(h, 0x6f, 10, 0, 1, 1, 0, "runs away");
            break;
        case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
        case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
        case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34:
        case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42:
        case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50:
        case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58:
        case 59: case 60:
            hit_flash(h, 1, "is confused");
            combat_text_clear();
            fx_end_actions(h, fx, 0);
            break;
        case 61: case 62: case 63: case 64: case 65: case 66: case 67: case 68:
        case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76:
        case 77: case 78: case 79: case 80:
            fx_add_message(h, 0x4d, 1, h->side, 1, 1, 0, "goes berserk");
            fx_dispatch(0x4d, h, 0, 0);
            break;
        case 81: case 82: case 83: case 84: case 85: case 86: case 87: case 88:
        case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96:
        case 97: case 98: case 99: case 100:
            hit_flash(h, 1, "is enraged");
            combat_text_clear();
            break;
        }
        saved = fx_saving_throw(h, 4, 0xfe);
        if (saved)
            fx_remove(h, 0x23, 0);
    }
}

void far fx_blink(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:05b7 */
{
    if (h->fight != 0 && !removed && h->fight->initiative == 0) {
        refused = 1;
        attack_roll = 0xff;
    }
}

void far fx_poison_attack(struct Hero far *h, char save)          /* 3a6a:05e4 */
{
    struct Effect far *fx;
    char saved;

    if (h->fight != 0) {
        fx_target = h->fight->target;
        saved = fx_saving_throw(fx_target, 0, save);
        fx_add_message(fx_target, 0x37, 0, 0xff, 0, 1, saved, msg_format("is %s", fx_names[0x37]));
        text_pause();
        if (find_effect(fx_target, 0x37, &fx))
            fx_disable(fx_target, 6, "is killed");
    }
}

void far fx_petrify_attack(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0698 */
{
    if (h->fight != 0 && !removed) {
        fx_target = h->fight->target;
        if (!fx_saving_throw(fx_target, 1, 0))
            fx_disable(fx_target, 7, "is turned to stone");
    }
}

void far fx_feeblemind(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:06ef */
{
    unsigned char i;

    if (h->fight != 0 && !removed) {
        h->intel = 3;
        h->wis = 3;
        hero_text(h, "loses intellect", 10, 1);
        if (view_mode == 5) {
            h->fight->can_cast = 0;
            if (h->fight->casting > 0) {
                hero_text(h, "lost a spell", 12, 1);
                spell_mark(h, h->fight->casting);
                h->fight->casting = 0;
            }
        }
        for (i = 0; i <= 0x8c; i++)
            if (h->spells[i] & 0x80)
                h->spells[i] = 0;
    }
}

void far fx_damage_vs_immunity1(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:07c1 */
{
    if (h->fight != 0 && !removed) {
        fx_target = h->fight->target;
        if (fx_target->immunity & 1) {
            damage = fx_roll_damage(6, 6) + str_damage(h);
            attack_roll++;
        }
    }
}

void far fx_berserk(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0818 */
{
    if (h->fight != 0) {
        if (!removed) {
            h->quick_fight = 1;
            if (!(h->control & 0x80) || h->control == 0xb3)
                h->control = 0xb3;
            else
                h->control = 0xb2;
            if (view_mode == 5) {
                h->fight->target = 0;
                plan_steps(combatant_x(h), combatant_y(h), 0xff, 0xff, combatant_size(h));
                h->fight->target = (heroes - 1)[(steps - 1)[2].who];
                h->fight->can_cast = 0;
                h->side = (enum Side)(h->fight->target->side ^ 1);
                hero_text(h, "goes berserk", 10, 1);
            }
        } else {
            if (h->control == 0xb3) {
                h->control = 0;
                h->quick_fight = 0;
            }
            h->side = 0;
        }
    }
}

void far fx_drain_one_level(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0935 */
{
    if (view_mode == 5)
        fx_drain(h->fight->target, 1);
}

void far fx_drain_two_levels(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:095a */
{
    if (view_mode == 5)
        fx_drain(h->fight->target, 2);
}

void far fx_fire_damage_4d6(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:097f */
{
    struct Hero far *t;

    if (view_mode == 5) {
        t = h->fight->target;
        if (t != 0) {
            element_mask = 9;
            fx_damage(t, fx_roll_damage(4, 6), 2, fx_saving_throw(t, 4, 0));
        }
    }
}

void far fx_berserk_creature(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:09e2 */
{
    if (h->fight != 0) {
        if (!removed) {
            h->quick_fight = 1;
            if (!(h->control & 0x80) || h->control == 0xb3)
                h->control = 0xb3;
            else
                h->control = 0xb2;
            h->fight->target = 0;
            plan_steps(combatant_x(h), combatant_y(h), 0xff, 0xff, combatant_size(h));
            h->fight->target = (heroes - 1)[(steps - 1)[2].who];
            h->side = (enum Side)(h->fight->target->side ^ 1);
        } else {
            if (h->control == 0xb3)
                h->control = 0;
            h->side = (enum Side)fx->strength;
        }
    }
}

void far fx_fire_damage_2d6(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0ad5 */
{
    struct Hero far *t;

    if (view_mode == 5) {
        t = h->fight->target;
        if (t != 0) {
            element_mask = 9;
            fx_damage(t, fx_roll_damage(2, 6), 2, fx_saving_throw(t, 4, 0));
        }
    }
}

void far fx_fear_remove(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0b38 */
{
    if (h->fight != 0 && removed) {
        if (h->control == 0xb3) {
            h->control = 0;
            h->quick_fight = 0;
        }
        h->fight->panicked = 0;
    }
}

void far fx_vorpal(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0b70 */
{
    struct Hero far *t;

    if (view_mode == 5 && !removed && crit_hit) {
        t = h->fight->target;
        if (!(t->immunity & 0x20))
            fx_disable(t, 6, "is beheaded");
    }
}

void far fx_fire_damage_2d6_alt(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0bbc */
{
    struct Hero far *t;

    if (view_mode == 5) {
        t = h->fight->target;
        if (t != 0) {
            element_mask = 9;
            fx_damage(t, fx_roll_damage(2, 6), 2, fx_saving_throw(t, 4, 0));
        }
    }
}

void far fx_movement_stop(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0c1f */
{
    if (h->fight != 0) {
        h->fight->moves = 0;
        if (movement_stops_attack_rate)
            move_rate = 0;
    }
}

void far fx_poison_steam_breath(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0c46 */
{
    char x;
    char y;
    char ok;
    char saved;
    struct Hero far *t;
    unsigned char i;
    char dir;

    mirror_image_bypass = 1;
    if (!combat_round)
        fx->strength = 3;
    if (fx->strength > 0) {
        x = combatant_x(h);
        y = combatant_y(h);
        (*hit_proc)(0x82, 1, &ok);
        if (ok) {
            spell_path_targets(x, y, view_cx, view_cy, 0xff, 3);
            if (target_count > 0) {
                element_mask = 0x20;
                hero_text(h, "breathes poison", 10, 1);
                dir = combatant_direction(h, targets[1]);
                hero_panel(h);
                combatant_facing_set(h, dir, 1, 0);
                icon_facings(0x13);
                icon_fly(combatant_x(h), combatant_y(h),
                             combatant_x(targets[1]), combatant_y(targets[1]), 1, 0x1e);
                for (i = 1; i <= target_count; i++)
                    if (targets[i] != 0) {
                        t = targets[i];
                        saved = fx_saving_throw(t, 3, 0);
                        fx_damage(t, h->hp_max, 2, saved);
                    }
                fx->strength--;
                combat_actions_end(h);
            }
        }
    }
    mirror_image_bypass = 0;
}

void far fx_lightning_breath(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0e0e */
{
    char x;
    char y;
    char dir;

    if (!combat_round || fx->strength > 3)
        fx->strength = 3;
    if (fx->strength > 0) {
        element_mask = 0x24;
        x = combatant_x(h);
        y = combatant_y(h);
        (*hit_proc)(0x33, 1, &turn_done);
        if (turn_done) {
            hero_text(h, "breathes lightning", 10, 1);
            dir = combatant_direction(h, targets[1]);
            hero_panel(h);
            combatant_facing_set(h, dir, 1, 0);
            sfx_play(9);
            icon_facings(0x14);
            icon_fly(x, y, view_cx, view_cy, 4, 0x32);
            bolt_run(9, h->hp_max, 0);
            fx->strength--;
            combat_actions_end(h);
        }
    }
}

void far fx_spell_gaze(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:0f39 */
{
    char ok;
    char dir;

    (*hit_proc)(10, 1, &ok);
    if (ok) {
        mirror_image_bypass = 1;
        element_mask = 0x10;
        hero_text(h, "gazes", 10, 1);
        dir = combatant_direction(h, targets[1]);
        combatant_facing_set(h, dir, 1, 0);
        icon_facings(0x13);
        icon_fly(combatant_x(h), combatant_y(h), view_cx, view_cy, 4, 0x32);
        current_spell = 10;
        spell_run(10);
        mirror_image_bypass = 0;
    }
}

void far fx_breath_weapon(struct Hero far *h, struct Effect far *fx, char removed)
                                                                 /* 3a6a:1004 */
{
    char x;
    char y;
    unsigned char i;
    struct Hero far *t;
    char saved;
    char lo;
    char hi;
    char name[21];
    char dir;
    int dmg;

    mirror_image_bypass = 1;
    if (!combat_round || fx->strength > 3)
        fx->strength = 3;
    if (fx->strength > 0) {
        dmg = h->hp_max;
        switch (fx->kind) {
        case 0x53: case 0x54:
            element_mask = 0x21;
            lo = 3;
            hi = 9;
            str_copy(name, "Fire");
            if (fx->kind == 0x53)
                dmg = fx_roll_damage(24, 10) + 12;
            break;
        case 0x55:
            element_mask = 0x22;
            lo = 3;
            hi = 7;
            str_copy(name, "cold");
            break;
        case 0x60:
            element_mask = 0x20;
            lo = 1;
            hi = 6;
            str_copy(name, "acid");
            break;
        }
        x = combatant_x(h);
        y = combatant_y(h);
        (*hit_proc)(0x17, 1, &turn_done);
        if (turn_done) {
            spell_path_targets(x, y, view_cx, view_cy, lo, hi);
            for (i = 1; i <= target_count; i++)
                if (enemy_team(h) != targets[i]->side)
                    turn_done = 0;
            if (target_count > 0 && turn_done) {
                hero_text(h, msg_format("breathes %s", name), 10, 1);
                dir = combatant_direction(h, targets[1]);
                hero_panel(h);
                combatant_facing_set(h, dir, 1, 0);
                icon_facings(0x13);
                icon_fly(combatant_x(h), combatant_y(h),
                             combatant_x(targets[1]), combatant_y(targets[1]), 1, 0x1e);
                for (i = 1; i <= target_count; i++)
                    if (targets[i] != 0) {
                        t = targets[i];
                        saved = fx_saving_throw(t, 3, 0);
                        fx_damage(t, dmg, 2, saved);
                    }
                fx->strength--;
                combat_actions_end(h);
            }
        }
    }
    mirror_image_bypass = 0;
}
