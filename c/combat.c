/* CKIT.EXE overlay 00 module 1b41: the combat round loop.
 * combat_main sets a fight up, runs rounds until a side's count reaches
 * zero, the round counter reaches its limit or Esc ends a demo, and tears
 * it down again.
 * Each round orders the combatants by initiative and gives each a turn:
 * the computer runs its own through the vector stub 18de:1157, and a party
 * member's turn is the command menu -- moving and attacking by direction,
 * casting, turning, guarding, delaying, bandaging, the game speed.  After
 * the fight, party members past their race's age limit may die of it.
 *
 * EXE 0x01e2b0..0x01fa24, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 1b41, the first module of RTLink section 0 (payload
 * 0x1e2b0, link segment 1b41; a root section, the parent of 1 and 2).  Its
 * final retf is byte 3 of paragraph 1cb8, whose module starts at 1cb8:0004.
 * Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in
 * shipped order; bake consumes them in that order.  Section 3's 1b41 frame
 * holds other bytes.
 *
 * A bare seg:off in this file, stored or load-based, means section 0.  The
 * seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable; a definition's comment
 * and a side comment in a body give the load-based address, as the
 * resident modules' do.  Callee and variable names are placeholders chosen
 * from use; an ovl00_ name says which stored segment of section 0 the
 * callee lives in, and a mod18de_ callee is an RTLink vector stub whose
 * target is named beside it.
 *
 * combat_main is reached through the vector stub 18de:10df.  No call,
 * vector or relocation in sections 0 to 2 reaches combat_unit_init.  The
 * routines after combat_main are near-called ahead of their definitions,
 * so static.
 *
 * _DATA is max_age and ten stored literals, DGROUP 54f6..5573: -d stores
 * "%s%s" and "" inside "%s%s%s".  _BSS is empty.
 *
 * combat_order is indexed from 1 up to 0x48, and heroes and tile_info from 1;
 * the shipped code adds the address of element 0 (c52a, b8f1, 5ff1).
 * side_count is indexed by side.
 *
 * This is the v1.0 build (CKIT.EXE file offset 0x1e310; addresses above are
 * v1.2's).  It differs from v1.2 only in round_end: v1.0 lets a dying hero
 * bleed every round regardless of cursor_obj[0x1d].
 */

struct Pool;                    /* strutil.c's slot pool                  */
struct Res;                     /* tiles.c's resource handle              */
struct Effect;                  /* fxcore.c's; handled only by pointer    */
struct Item;                    /* cbaction.c's; only a null is passed here */

struct Aim {                    /* a spell's chosen target and cell       */
    struct Hero far *target;    /* +0                                     */
    int x;                      /* +4                                     */
    int y;                      /* +6                                     */
    char byte_8;                /* +8   not read here                     */
};

struct Patch {                  /* 0x21 bytes, from patch_pool             */
    char pad_0[4];              /* +0   not read here                     */
    struct Patch far *next;     /* +4                                     */
};

struct Fight {                  /* a combatant's state in a fight         */
    unsigned char casting;      /* +0   spell cast at the start of a turn */
    char can_cast;              /* +1                                     */
    char pad_2;                 /* +2   not read here                     */
    int initiative;             /* +3   0 or less: no turn this round     */
    char pad_5[2];              /* +5   not read here                     */
    unsigned char moves;        /* +7   half-moves left                   */
    char guarding;              /* +8                                     */
    char pad_9;                 /* +9   not read here                     */
    unsigned char facing;       /* +a                                     */
    struct Hero far *target;    /* +b                                     */
    unsigned char dying;        /* +f   rounds spent dying                */
    unsigned char attacks_taken; /* +10                                    */
    char pad_11;                /* +11  not read here                     */
    unsigned char turn_attempts; /* +12  below ai_level: may turn undead   */
    unsigned char attack_angles; /* +13                                    */
    char not_pc;                /* +14  1: no age check                   */
};

struct Hero {                   /* a combatant                            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[0x3c];           /* +4   not read here                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0xe];           /* +44  not read here                     */
    unsigned age;               /* +52  years                             */
    char pad_54[4];             /* +54  not read here                     */
    unsigned char race;         /* +58  index into max_age                */
    char pad_59[3];             /* +59  not read here                     */
    unsigned char gender;       /* +5c  0: he                             */
    char pad_5d;                /* +5d  not read here                     */
    unsigned char status;       /* +5e  5 dying, 6 dead                   */
    unsigned char side;         /* +5f                                    */
    char name[0x11];            /* +60                                    */
    char pad_71[0x22];          /* +71  not read here                     */
    unsigned char control;      /* +93  bit 7: never player-run           */
    char pad_94[0x2d];          /* +94  not read here                     */
    unsigned char item_count;   /* +c1  items carried                     */
    char pad_c2[4];             /* +c2  not read here                     */
    unsigned char spells[0x8d]; /* +c6                                    */
    char pad_153[0x2b];         /* +153 not read here                     */
    char active;                /* +17e                                   */
    char quick_fight;           /* +17f nonzero: 18de:1157 plays it       */
};

struct Map {
    char pad_0[2];              /* +0   not read here                     */
    int view_x;                 /* +2   the view's left column            */
    int view_y;                 /* +4   the view's top row                */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    char pad_1[3];              /* +1   not read here                     */
};

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far hero_panel(struct Hero far *h);                   /* 03d0:0834 */
extern char far is_helpless(struct Hero far *h);                   /* 03d0:09f2 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far continue_prompt(char a, char b, char far *items); /* 06c9:0932 */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern void far box_draw_both(void);                             /* 083a:0400 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far set_menu_kind(int kind);                         /* 083a:13f2 */
extern char far grid_choose(char far *title, char far *text, char fkeys, char anim,
                            unsigned char rows, unsigned char cols); /* 083a:156f */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern char far pool_lowbit(int value);                          /* 0a75:032f */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern void far icon_clear(unsigned char idx);                   /* 0cb5:004d */
extern void far pic_free(void);                                  /* 0cb5:0352 */
extern void far esc_set(char on);                              /* 0d48:0008 */
extern char far esc_get(void);                                 /* 0d48:0013 */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far input_poll(void);                                /* 1157:05c0 */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far fx_disable(struct Hero far *h, int how, char far *msg);
                                /* 18de:10e9, vector to overlay 19 2e88:000f */
extern void far camp_spell_targets(unsigned char spell, char mode, char far *result);
                                /* 18de:10f3, vector to overlay 12 2b69:13cb */
extern void far fx_hp_effects_remove(struct Hero far *h);
                                /* 18de:10fd, vector to overlay 19 2e88:17ec */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern void far fx_trigger_group(unsigned char when, struct Hero far * far *hp);
                                /* 18de:1111, vector to overlay 19 2e88:0449 */
extern void far combat_setup(void);
                                /* 18de:1125, vector to overlay 1 2482:1c4d */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far char_view(char far *done);
                                /* 18de:114d, vector to overlay 14 28aa:0b21 */
extern void far ai_turn(struct Hero far *h);
                                /* 18de:1157, vector to overlay 2 2482:0000 */
extern void far turn_keys(struct Hero far *h);
                                /* 18de:1161, vector to overlay 2 2482:0fd0 */
extern void far items_menu(char far *done);
                                /* 18de:116b, vector to overlay 14 28aa:0f56 */
extern void far spell_cast(unsigned char n, char a, char b, char far *done);
                                /* 18de:1175, vector to overlay 12 2b69:14c8 */
extern void far fx_terrain_apply(struct Hero far *h, char may_save, char skip_1d);
                                /* 18de:11a7, vector to overlay 19 2e88:0faa */
extern void far fx_clouds_tick(char now);
                                /* 18de:11b1, vector to overlay 19 2e88:2461 */
extern void far clock_advance(unsigned char n, short unit);
                                /* 18de:11bb, vector to overlay 19 2d91:02bf */
extern void far combat_sides_count(void);                              /* 1cb8:091f */
extern char far combat_actions_end(struct Hero far *h);                /* 1cb8:0b84 */
extern char far combat_guard(struct Hero far *h);                /* 1cb8:0bc5 */
extern void far combat_view_show(void);                              /* 1cb8:0c07 */
extern char far has_ranged_weapon(struct Hero far *h);                /* 1cb8:0c32 */
extern char far has_throwable_weapon(struct Hero far *h);                /* 1cb8:0c71 */
extern char far bandage_dying(char apply);                        /* 1cb8:0cc8 */
extern void far combat_round_begin(struct Hero far *h);             /* 1dbf:0006 */
extern void far combat_move_step(struct Hero far *h, unsigned char dir); /* 1dbf:06b2 */
extern void far opportunity_attacks(struct Hero far *h, unsigned char dir); /* 1dbf:08d8 */
extern char far combat_flee(struct Hero far *h);             /* 1dbf:0d02 */
extern void far attacks_left_recalc(struct Hero far *h);             /* 1dbf:0db4 */
extern void far turn_undead(struct Hero far *h);             /* 1dbf:0fec */
extern void far attack_register(struct Hero far *h, struct Hero far *t); /* 1dbf:170a */
extern void far combat_attack(struct Hero far *h, struct Hero far *t, char c,
                                struct Item far *missile, char far *done); /* 1dbf:178d */
extern void far combat_spell_targets(unsigned char spell, char mode, char far *result);
                                                                 /* 1dbf:1ddb */
extern void far combat_cast_begin(unsigned char spell, char n, char far *found);
                                                                 /* 1dbf:23a6 */
extern void far hostile_hp_percent_update(void);                           /* 1dbf:2840 */
extern char far attack_confirm(struct Hero far *h, struct Hero far *t); /* 1dbf:2942 */
extern void far combat_aim(struct Hero far *h, char a, char b, char c, char d,
                                char far *done, struct Aim far *aim); /* 1dbf:3634 */
extern char far gods_intervene(char n);                         /* 1dbf:3de3 */
extern void far combat_leadership_apply(void);                           /* 1dbf:4bbf */
extern char far combat_sweep_attack(struct Hero far *h, struct Hero far *t); /* 1dbf:50d1 */
extern void far cell_redraw(char x, char y);                     /* 22fa:025e */
extern void far tile_update(char x, char y, unsigned char who);  /* 22fa:04d5 */
extern char far combatant_visible(struct Hero far *h, char whole);     /* 22fa:0627 */
extern void far combat_view_scroll(char x, char y, unsigned char margin,
                            unsigned char dir);                  /* 22fa:0847 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame,
                            char erase);                         /* 22fa:09a2 */
extern char far combatant_x(struct Hero far *h);                 /* 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h);                 /* 22fa:0ae4 */
extern unsigned char far combatant_slot(struct Hero far *h);        /* 22fa:0b44 */
extern void far combat_direction_probe(struct Hero far *h, unsigned char dir, unsigned char far *who,
                            unsigned char far *cell, unsigned char far *kind);
                                                                 /* 22fa:0b9c */
extern char far hero_place(struct Hero far *h, unsigned char x, unsigned char y,
                           char moving);                         /* 22fa:1053 */
extern void far view_center(struct Hero far *h, unsigned char margin, char cursor);
                                                                 /* 22fa:1286 */

extern char far *continue_items;                                 /* 390c */
extern char far *magic_on_text;                                  /* 3b38 */
extern char far *magic_off_text;                                 /* 3b3c */
extern char far *flee_text;                                      /* 3b40 */
extern char far *cant_go_text;                                   /* 3b44 */
extern char far *speed_title;                                    /* 3c00 */
extern char far *continue_text;                                  /* 3c0c */
extern char far *speed_items;                                    /* 3c78 */
extern char far *command_items;                                  /* 3c84 */
extern char far *move_items;                                     /* 3c88 */
extern char far *old_text;                                       /* 3efb */
extern char far *strain_text;                                    /* 3eff */
extern char far *breath_text;                                    /* 3f03 */
extern char far *he_text;                                        /* 3f07 */
extern char far *she_text;                                       /* 3f0b */
extern char far *age_death_text;                                 /* 3f0f */
extern char far *no_witness_text;                                /* 3f13 */
extern char far *one_witness_text;                               /* 3f17 */
extern char far *many_witness_text;                              /* 3f1b */
extern char far *search_body_text;                               /* 3f1f */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern char turn_done;                                           /* b414 */
extern unsigned char combat_round;                                  /* b418 */
extern int view_mode;                                            /* b421 */
extern char esc_check;                                           /* b423 */
extern char text_slow;                                           /* b431 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char magic_on;                                            /* b728 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern unsigned char side_count[];                               /* b8d1 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern struct Pool patch_pool;                                    /* bb90 */
extern unsigned char combat_round_limit;                                  /* bcf1 */
extern char year_rolled;                                         /* c354 */
extern unsigned char order_idx;                                  /* c3a0 */
extern struct Hero far *combat_order[0x49];                      /* c52a */
extern struct Map far *cur_map;                                  /* c66b */
extern struct Patch far *patch_list;                             /* ca0a */
extern unsigned char ai_level;                                   /* ca21 */
extern char menu_escaped;                                        /* ca2a */
extern char move_flag;                                           /* cc79 */
extern char test_mode;                                           /* cc7a */
extern struct Res far *pic_res;                                  /* cfd8 */
extern char panel_dirty;                                         /* d0fb */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern void (far *hit_proc)(unsigned char spell, char mode, char far *result); /* d131 */

static void combatant_turn(struct Hero far *h);
static void command_loop(struct Hero far *h);
static void command_menu(struct Hero far *h, unsigned char far *key);
static void round_end(char far *done);
static void move_hero(struct Hero far *h, unsigned char key, char far *done);
static void strike(struct Hero far *h, struct Hero far *t, char far *done);
static void combat_speed_menu(void);
static void clear_target(struct Hero far *h);

static unsigned max_age[6] = { 1600, 325, 525, 525, 199, 120 }; /* by race */

/* The translated Pascal unit's empty initialization block (DKK combat.pas). */
void far combat_unit_init(void)                                       /* 2b41:0000 */
{
}

void far combat_cleanup(void)                                    /* 2b41:0001 */
{
    struct Patch far *nxt;
    struct Hero far *cur;
    struct Effect far *eff;
    unsigned char i;

    while (patch_list != 0) {
        nxt = patch_list->next;
        pool_free(patch_list, 0x21, &patch_pool);
        patch_list = nxt;
    }
    cur = node_list;
    while (cur != 0) {
        fx_remove(cur, 0xb0, 0);
        if (find_effect(cur, 0xb, &eff))
            if (cur->status == 0)
                if (side_count[1] > 1)
                    cur->status = 3;
                else
                    cur->status = 0;
        fx_hp_effects_remove(cur);
        cur = cur->next;
    }
    pic_free();
    for (i = 0; i <= 0x16; i++)
        icon_clear(i + 0xf);
    for (i = 8; i <= 0xe; i++)
        icon_clear(i);
    for (i = 0x4c; i < 0x51; i++)
        icon_clear(i);
    res_free(&pic_res);
    hit_proc = camp_spell_targets;
}

void far combat_aging(void)                                      /* 2b41:012c */
{
    struct Hero far *cur;
    struct Hero far *p;
    struct Hero far *witness;
    char is_old;
    char is_warning;
    unsigned char row;
    unsigned char count;
    char pronoun[6];
    char message[42];

    cur = node_list;
    while (cur != 0) {
        if (cur->status < 6 && cur->fight->not_pc != 1) {
            is_old = cur->age > max_age[cur->race];
            if (!is_old)
                is_warning = cur->age > max_age[cur->race] - 10;
            if (is_warning)                 /* unset when is_old: 2b41:01a7 */
                hero_text(cur, old_text, 10, 1);
            if (is_old) {
                row = 0xe;
                text_clear(0x17, row, 0x26, 0x15);
                text_wrap(0x17, row, 0x26, 0x15, 0xb, 0, 1,
                          msg_format("%s%s%s", strain_text, cur->name, ".\t"));
                continue_prompt(0, 0, continue_items);
                text_clear(0x17, row, 0x26, 0x15);
                if (cur->gender == 0)
                    str_copy(pronoun, he_text);
                else
                    str_copy(pronoun, she_text);
                text_wrap(0x17, row, 0x26, 0x15, 0xb, 0, 1,
                          msg_format("%s%s", pronoun, breath_text));
                continue_prompt(0, 0, continue_items);
                text_clear(0x17, row, 0x26, 0x15);
                fx_disable(cur, 8, age_death_text);
                p = node_list;
                count = 0;
                while (p != 0) {
                    if (p->active == 1 && p->status == 0 && p->side != 1) {
                        count++;
                        witness = p;
                    }
                    p = p->next;
                }
                switch (count) {
                case 0:
                    str_copy(message, no_witness_text);
                    break;
                case 1:
                    str_copy(message, msg_format("%s%s", witness->name, one_witness_text));
                    break;
                default:
                    str_copy(message, msg_format("%s%s%s", many_witness_text,
                                                     witness->name, search_body_text));
                }
                text_clear(0x17, row, 0x26, 0x15);
                text_wrap(0x17, row, 0x26, 0x15, 0xb, 0, 1, message);
                continue_prompt(0, 0, continue_items);
            }
        }
        cur = cur->next;
    }
    year_rolled = 0;
}

void far order_combatants(void)                                  /* 2b41:044c */
{
    char more;
    struct Hero far *cur;
    struct Hero far *saved;
    struct Hero far *nxt;
    char cur_init;
    char slot_init;
    unsigned char i;
    unsigned char j;

    cur = node_list;
    mem_fill(combat_order, 0x124, 0);
    while (cur != 0) {
        i = 1;
        more = 1;
        cur_init = cur->fight->initiative;
        while (i < 0x41 && more) {
            if (combat_order[i] != 0)
                slot_init = combat_order[i]->fight->initiative;
            else
                slot_init = -1;
            if (cur_init > slot_init
                    || (cur_init == slot_init && fx_roll_dice(1, 2) == 1))
                more = 0;
            else
                i++;
        }
        saved = combat_order[i];
        combat_order[i] = cur;
        if (saved != 0) {
            j = i;
            more = 1;
            while (j < 0x48 && more) {
                j++;
                nxt = combat_order[j];
                combat_order[j] = saved;
                saved = nxt;
                if (nxt == 0)
                    more = 0;
            }
        }
        cur = cur->next;
    }
}

void far combat_main(void)                                       /* 2b41:05a2 */
{
    char finished;
    char done;
    struct Hero far *cur;
    int key;

    finished = 0;
    view_mode = 5;
    hit_proc = combat_spell_targets;
    combat_leadership_apply();
    combat_setup();
    done = 0;
    if (side_count[0] == 0 || side_count[1] == 0)
        done = 1;
    cur = node_list;
    while (cur != 0) {
        fx_trigger_group(0x18, &cur);
        cur = cur->next;
    }
    while (!done) {
        combat_sides_count();
        cur = node_list;
        while (cur != 0) {
            combat_round_begin(cur);
            cur = cur->next;
        }
        cursor_obj[0x2e] = 0;
        order_combatants();
        cur = combat_order[1];
        order_idx = 1;
        while (cur != 0 && !done && (!esc_check || !esc_get())) {
            combatant_turn(cur);
            order_idx++;
            if (order_idx > 0x48)
                cur = 0;
            else
                cur = combat_order[order_idx];
            combat_sides_count();
            if (side_count[0] == 0 || side_count[1] == 0 || combat_round >= combat_round_limit)
                if (!finished) {
                    done = 1;
                    finished = 1;
                }
            if (esc_check)
                if (key_poll()) {
                    key = key_get();
                    if (key == 0x152)
                        sound_toggle();
                    if (key == 0x153)
                        music_toggle();
                    if (key == 0x1b || key == 0x60)
                        esc_set(1);
                }
            if (is_xvga() || !pixel_shift())
                input_poll();
        }
        round_end(&done);
        if (esc_check && esc_get())
            done = 1;
    }
    if (year_rolled)
        combat_aging();
    combat_cleanup();
    text_slow = 1;
}

static void combatant_turn(struct Hero far *h)                   /* 2b41:0781 */
{
    h->fight->attacks_taken = 0;
    h->fight->attack_angles = 0;
    h->fight->guarding = 0;
    fx_trigger_group(7, &h);
    if (h->fight->initiative > 0) {
        if (h->fight->initiative == 0x14)
            h->fight->initiative = 0x13;
        cur_node = h;
        move_flag = h->side == 0 || combatant_visible(h, 0);
        view_center(h, 2, 1);
        recalc_stats(h);
        panel_dirty = 1;
        hero_panel(h);
        fx_trigger_group(0xf, &h);
        if (h->fight->casting == 0)
            fx_trigger_group(0x15, &h);
        if (h->fight->initiative > 0)
            if (h->quick_fight)
                ai_turn(h);
            else
                command_loop(h);
        cell_redraw(combatant_x(h), combatant_y(h));
    }
}

static void command_loop(struct Hero far *h)                     /* 2b41:08c5 */
{
    unsigned char key;
    char done;
    struct Hero far *p;
    struct Aim aim;
    unsigned char spell;

    if (h->active) {
        turn_keys(0);
        if (h->fight->casting > 0) {
            spell = h->fight->casting;
            h->fight->casting = 0;
            spell_cast(spell, 0, 1, &done);
            done = combat_actions_end(h);
        } else {
            done = 0;
            while (!done) {
                ui_mode = 1;
                command_menu(h, &key);
                if (!menu_escaped) {
                    switch (key) {
                    case 0:
                        move_hero(h, key, &done);
                        break;
                    case 1:
                        combat_aim(h, 0xff, 1, 0, 1, &done, &aim);
                        break;
                    case 2:
                        ui_mode = 2;
                        items_menu(&done);
                        attacks_left_recalc(h);
                        if (!done)
                            combat_view_show();
                        break;
                    case 3:
                        combat_cast_begin(0, 0, &done);
                        break;
                    case 4:
                        if (class_level(h, 0) > 0 || class_level(h, 3) > 2)
                            if (h->fight->turn_attempts < ai_level) {
                                turn_undead(h);
                                done = combat_actions_end(h);
                            }
                        break;
                    case 5:
                        done = combat_guard(h);
                        break;
                    case 6:
                        clear_target(h);
                        box_draw();
                        turn_keys(0);
                        delay(200);
                        done = 1;
                        ai_turn(h);
                        break;
                    case 7:
                        h->fight->initiative = 1;
                        tick_slot = order_idx;
                        p = combat_order[tick_slot + 1];
                        while (p != 0 && tick_slot < 0x48) {
                            combat_order[tick_slot] = p;
                            tick_slot++;
                            p = combat_order[tick_slot + 1];
                        }
                        combat_order[tick_slot] = h;
                        order_idx--;
                        done = 1;
                        break;
                    case 8:
                        done = bandage_dying(1);
                        done = combat_actions_end(h);
                        break;
                    case 9:
                        char_view(&done);
                        attacks_left_recalc(h);
                        if (!done)
                            combat_view_show();
                        break;
                    case 10:
                        combat_speed_menu();
                        break;
                    case 11:
                        done = combat_actions_end(h);
                        break;
                    case 12:
                        gods_intervene(1);
                        view_center(h, 3, 0);
                        done = 1;
                        break;
                    }
                } else {
                    switch (key) {
                    case 0x81:
                    case 0x82:
                    case 0x83:
                    case 0x84:
                    case 0x85:
                    case 0x86:
                    case 0x87:
                    case 0x88:
                        move_hero(h, key, &done);
                        break;
                    case ' ':
                        p = node_list;
                        while (p != 0) {
                            if (p->control < 0x80)
                                p->quick_fight = 0;
                            p = p->next;
                        }
                        break;
                    case 0xcc:
                        magic_on = !magic_on;
                        if (magic_on)
                            bottom_msg(magic_on_text);
                        else
                            bottom_msg(magic_off_text);
                        break;
                    case 0xc0:
                    case 0xd0:
                        order_idx--;
                        h->fight->initiative = 0x14;
                        p = node_list;
                        while (p != 0) {
                            clear_target(p);
                            p = p->next;
                        }
                        box_draw();
                        delay(200);
                        done = 1;
                        break;
                    case 0xd7:
                        if (gods_intervene(0)) {
                            view_center(h, 3, 0);
                            done = 1;
                        } else
                            bottom_msg("That doesn't work");
                        break;
                    }
                }
                if (!done) {
                    view_center(h, 2, 1);
                    panel_dirty = 1;
                    hero_panel(h);
                }
            }
        }
    } else
        done = combat_actions_end(h);
}

static void command_menu(struct Hero far *h, unsigned char far *key) /* 2b41:0ce2 */
{
    unsigned char count;
    unsigned char i;
    char flag;
    char cy;
    char cx;

    count = 0;
    hotkey_add(1, &count);
    if (h->item_count > 0)
        hotkey_add(2, &count);
    flag = 0;
    for (i = 0; i <= 0x8c; i++)
        if ((h->spells[i] & 0x7f) > 0)
            flag = 1;
    if (flag && h->fight->can_cast && !cursor_obj[2])
        hotkey_add(3, &count);
    if ((class_level(h, 0) > 0 || class_level(h, 3) > 2) && h->fight->turn_attempts < ai_level)
        hotkey_add(4, &count);
    if (!has_ranged_weapon(h) || has_throwable_weapon(h))
        hotkey_add(5, &count);
    hotkey_add(6, &count);
    hotkey_add(7, &count);
    if (bandage_dying(0))
        hotkey_add(8, &count);
    for (i = 9; i <= 0xb; i++)
        hotkey_add(i, &count);
    if (test_mode)
        hotkey_add(0xc, &count);
    turn_done = 0;
    do {
        cy = combatant_y(h) - cur_map->view_y;
        cx = combatant_x(h) - cur_map->view_x;
        if (h->fight->moves > 0)
            set_menu_kind(4);
        else
            set_menu_kind(0xb);
        *key = grid_choose("", command_items, 1, 0, cy, cx);
        if (menu_escaped) {
            if (*key == 0xc0 || *key == 0xd0 || *key == 0xcc || *key == 0xd7
                    || *key == 0x12 || *key == 0x88 || *key == 0x84 || *key == 0x86
                    || *key == 0x82 || *key == 0x83 || *key == 0x81 || *key == 0x87
                    || *key == 0x85)
                turn_done = 1;
            else
                *key = 0xff;
        }
        if (*key <= 0xc)
            turn_done = 1;
    } while (!turn_done);
    box_draw_both();
}

static void round_end(char far *done)                            /* 2b41:0f6b */
{
    struct Hero far *p;
    unsigned char n;

    clock_advance(1, 1);
    fx_clouds_tick(0);
    combat_round++;
    hostile_hp_percent_update();
    p = node_list;
    n = 1;
    while (p != 0) {
        fx_trigger_group(0x13, &p);
        fx_terrain_apply(p, 0, 1);
        if (p->status == 5) {
            p->fight->dying++;
            if (p->fight->dying > 9)
                p->status = 6;
        }
        n++;
        p = p->next;
    }
    if (bandage_dying(0))
        bottom_msg("A Comrade is Dying");
    combat_sides_count();
    combat_view_scroll(cur_map->view_x + 3, cur_map->view_y + 3, 0xff, 8);
    if (side_count[0] == 0 || side_count[1] == 0 || combat_round >= combat_round_limit)
        *done = 1;
    if (side_count[0] > 0 && side_count[1] == 0 && !esc_check)
        if (confirm(continue_text, 0))
            *done = 0;
}

static void move_hero(struct Hero far *h, unsigned char key, char far *done) /* 2b41:10a5 */
{
    unsigned char start_moves;
    char start_facing;
    char cell_x;
    char cell_y;
    unsigned char dir;
    unsigned char foe;
    unsigned char terrain;
    unsigned char ch;
    unsigned char kind;
    unsigned char cost;
    char prompt[40];
    int col;
    int row;

    start_moves = h->fight->moves;
    start_facing = h->fight->facing;
    cell_x = combatant_x(h);
    cell_y = combatant_y(h);
    ch = key;
    *done = 0;
    dir = 8;
    while (!*done && ch != 0 && ch != 1 && ch != 0xd) {
        if (ch == 0xff) {
            str_format(prompt, "%s%2d", "Move/Attack, Move Left = ", h->fight->moves / 2);
            row = combatant_y(h) - cur_map->view_y;
            col = combatant_x(h) - cur_map->view_x;
            set_menu_kind(4);
            hotkey_reset(1);
            ch = grid_choose(prompt, move_items, 1, 0, row, col);
        }
        dir = 8;
        if (menu_escaped) {
            switch (ch) {
            case 0x1b:
                h->fight->moves = start_moves;
                tile_update(0, 0, combatant_slot(h));
                *done = !hero_place(h, cell_x, cell_y, 0);
                combat_view_scroll(combatant_x(h), combatant_y(h), 0, 8);
                h->fight->facing = start_facing;
                dir = 8;
                ch = 0;
                menu_escaped = 0;
                break;
            case 0x88:
                dir = 0;
                break;
            case 0x81:
                dir = 1;
                break;
            case 0x82:
                dir = 2;
                break;
            case 0x83:
                dir = 3;
                break;
            case 0x84:
                dir = 4;
                break;
            case 0x85:
                dir = 5;
                break;
            case 0x86:
                dir = 6;
                break;
            case 0x87:
                dir = 7;
                break;
            default:
                dir = 8;
            }
        } else if (ch == 0) {
            h->fight->moves = start_moves;
            tile_update(0, 0, combatant_slot(h));
            *done = !hero_place(h, cell_x, cell_y, 0);
            combat_view_scroll(combatant_x(h), combatant_y(h), 0, 8);
            h->fight->facing = start_facing;
            dir = 8;
        }
        if (dir < 8) {
            combatant_facing_set(h, dir, 0, 0);
            combat_direction_probe(h, dir, &foe, &terrain, &kind);
            if (foe > 0)
                strike(h, (heroes - 1)[foe], done);
            else if (terrain == 0) {
                if (confirm(flee_text, 0))
                    *done = combat_flee(h);
                else {
                    *done = 0;
                    ch = 0xff;
                }
            } else {
                if (pool_lowbit(dir))
                    cost = (tile_info - 1)[terrain].move_cost * 3;
                else
                    cost = (tile_info - 1)[terrain].move_cost * 2;
                if ((tile_info - 1)[terrain].move_cost == 0xff)
                    cost = 0xff;
                if (h->fight->moves < cost)
                    bottom_msg(cant_go_text);
                else {
                    opportunity_attacks(h, dir);
                    if (h->active) {
                        if (h->fight->moves > 0)
                            combat_move_step(h, dir);
                        if (!h->active)
                            *done = combat_actions_end(h);
                        fx_terrain_apply(h, 1, 0);
                        if (is_helpless(h) || !h->active)
                            *done = combat_actions_end(h);
                    } else
                        *done = combat_actions_end(h);
                }
            }
        }
        if (menu_escaped)
            ch = 0xff;
    }
    if (h->fight->moves < 2)
        h->fight->moves = 0;
}

static void strike(struct Hero far *h, struct Hero far *t, char far *done) /* 2b41:155a */
{
    if (has_ranged_weapon(h) && !has_throwable_weapon(h))
        bottom_msg("Not with that weapon");
    else if (attack_confirm(h, t)) {
        h->fight->target = t;
        if (combat_sweep_attack(h, t))
            *done = 1;
        else {
            attack_register(h, t);
            combat_attack(h, t, 0, 0L, done);
        }
    }
}

static void combat_speed_menu(void)                                    /* 2b41:160b */
{
    char query[42];
    unsigned char count;
    unsigned char choice;

    choice = 0xff;
    while (choice != 2) {
        str_copy(query, msg_format("%s%s%s%s", speed_title, "(",
                                       byte_str(cursor_obj[0x12]), ") "));
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        count = 0;
        if (cursor_obj[0x12] > 0)
            hotkey_add(0, &count);
        if (cursor_obj[0x12] < 9)
            hotkey_add(1, &count);
        hotkey_add(2, &count);
        choice = menu_run(query, speed_items, 0, 0);
        if (menu_escaped)
            switch (choice) {       /* 2b41:16e5: a char is never 0x104 or 0x108 */
            case 0x1b:
                choice = 2;
                break;
            case 0x108:
                choice = 1;
                break;
            case 0x104:
                choice = 0;
                break;
            }
        switch (choice) {
        case 0:
            cursor_obj[0x12]--;
            break;
        case 1:
            cursor_obj[0x12]++;
            break;
        }
    }
}

static void clear_target(struct Hero far *h)                     /* 2b41:1730 */
{
    h->quick_fight = 1;
    if (h->fight->target != 0 && h->fight->target->side == h->side)
        h->fight->target = 0;
}
