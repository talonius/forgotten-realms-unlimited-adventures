/* CKIT.EXE overlay 00 module 1dbf: combat actions.
 * A combatant's turn set up, moving a step and the guards who strike as it
 * passes, fleeing, turning undead, melee and missile attacks with their
 * reports, sweeping several foes, casting and choosing a spell's targets
 * (one, several, or an area scored cell by cell), the targeting menu and
 * its map cursor with the mouse, an effect handler that fires rays and
 * spells by range, morale_base, and the leadership pass that can hand
 * computer-run party members to the player.
 *
 * EXE 0x020a95..0x025e4a, one Turbo C large-model _TEXT segment starting at
 * byte 5 of paragraph 1dbf, in RTLink section 0 (payload 0x1e2b0, link
 * segment 1b41; a root section, the parent of 1 and 2).  The first five
 * bytes of paragraph 1dbf end cbtutil.c's module 1cb8; this module's first
 * routine is the lone retf at 1dbf:0005, which nothing reaches, left here by
 * the run's convention.  cbview.c (module 22fa) starts after the last retf,
 * at 22fa:000a.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are
 * defined in shipped order; bake consumes them in that order.
 *
 * One object: section 0 files all 387 fixup sites in the range under frame
 * 1dbf.  Fourteen routines are far-called ahead of their definitions and
 * rank_target is near-called ahead of its own, so it is static.
 *
 * A bare stored seg:off in this file means section 0.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and cbaction_NNNN only where in this module
 * the routine starts.  The ovl00_1dbf_NNNN names defined here are the
 * placeholders landed combat.c already calls, and combat.c stores
 * combat_spell_targets as hit_proc.  A mod18de_ callee is an RTLink vector stub;
 * its comment names the routine the stub reaches.  combat_attack,
 * view_click and eye_rays are also reached through the stubs 18de:1201,
 * 18de:10b7 and 18de:12a1.
 *
 * _DATA is area_widths, turn_table and 35 stored literals, DGROUP
 * 563e..5907; -d stores "" on the NUL of "-Backstabs-".  _BSS is 20
 * statics, DGROUP adce..aec1: their names were chosen to fit Turbo C's name
 * order, not recovered, and dummy_int, unused_int and unused_buffer exist
 * only to hold the layout.  <stdlib.h> is for abs, which Turbo C compiles
 * inline.
 *
 * heroes, near_ids, steps, combatants, recs, tile_info and a combatant's
 * attacks, dice_n, dice_s and dmg are indexed from 1, as are hits_left and
 * attacks_used by hand in melee_attack; the shipped code adds the address of
 * element 0.
 */

#include <stdlib.h>

struct Res;

struct Fight {                  /* a combatant's state in a fight         */
    unsigned char casting;      /* +0   spell being cast                  */
    char can_cast;              /* +1                                     */
    char can_use_item;          /* +2                                     */
    int initiative;             /* +3   initiative                        */
    unsigned char hand;         /* +5                                     */
    unsigned char sweep_targets; /* +6   foes one sweep may strike         */
    unsigned char moves;        /* +7   half-moves left                   */
    char guarding;              /* +8                                     */
    char attacked;              /* +9                                     */
    unsigned char facing;       /* +a   facing                            */
    struct Hero far *target;    /* +b                                     */
    unsigned char dying;        /* +f   not read here                     */
    unsigned char attacks_taken; /* +10  raised by each attack on it       */
    char panicked;              /* +11                                    */
    unsigned char turn_attempts; /* +12  turn-undead attempts              */
    unsigned char attack_angles; /* +13  facing turns toward attackers, %8 */
};

struct Hero {                   /* a combatant                            */
    struct Hero far *next;      /* +0                                     */
    char pad_4[8];              /* +4   not read here                     */
    struct Item far *equipped[13]; /* +c   0: the weapon                     */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x14];          /* +44  not read here                     */
    unsigned char race;         /* +58                                    */
    char pad_59;                /* +59  not read here                     */
    unsigned char undead;       /* +5a  row of turn_table, from 1; 0: not */
    char pad_5b[3];             /* +5b  not read here                     */
    unsigned char status;       /* +5e  5 dying, 6 dead                   */
    unsigned char side;         /* +5f                                    */
    char pad_60[0x13];          /* +60  not read here                     */
    unsigned char intel;        /* +73                                    */
    char pad_74[7];             /* +74  not read here                     */
    unsigned char cha;          /* +7b                                    */
    char pad_7c[5];             /* +7c  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    unsigned char icon_dim;     /* +82                                    */
    char pad_83[6];             /* +83  not read here                     */
    unsigned char hit_dice;     /* +89                                    */
    char pad_8a[9];             /* +8a  not read here                     */
    unsigned char control;      /* +93  bit 7: computer-run until led     */
    unsigned char sweep_targets; /* +94  copied to fight->sweep_targets           */
    char pad_95[0x16];          /* +95  not read here                     */
    unsigned char attack_rate;  /* +ab  attacks[0] at the start of a turn */
    unsigned char attack_rate_off; /* +ac  copied to move_rate               */
    char pad_ad[0x12];          /* +ad  not read here                     */
    unsigned char type_flags;   /* +bf                                    */
    unsigned char immunity;     /* +c0                                    */
    char pad_c1[3];             /* +c1  not read here                     */
    unsigned char magic_resist; /* +c4                                    */
    char pad_c5;                /* +c5  not read here                     */
    unsigned char spells[0x8d]; /* +c6                                    */
    char pad_153[0x2b];         /* +153 not read here                     */
    char active;                /* +17e                                   */
    char quick_fight;           /* +17f nonzero: the computer plays it    */
    char pad_180;               /* +180 not read here                     */
    unsigned char ac_front_rear[2]; /* +181 [1]: struck from behind           */
    unsigned char attacks[2];   /* +183 per hand, from 1                  */
    unsigned char dice_n[2];    /* +185 from 1                            */
    unsigned char dice_s[2];    /* +187 from 1                            */
    char dmg[2];                /* +189 from 1                            */
    unsigned char hp;           /* +18b                                   */
};

struct Item {
    struct Item far *next;      /* +0                                     */
    char pad_4[0x24];           /* +4   not read here                     */
    unsigned char type;         /* +28  index into item_table             */
    char pad_29[9];             /* +29  not read here                     */
    char readied;               /* +32                                    */
    char pad_33[2];             /* +33  not read here                     */
    unsigned char count;        /* +35  count in the stack                */
    char pad_36;                /* +36  not read here                     */
    unsigned char effect;       /* +37                                    */
    char pad_38[6];             /* +38  not read here                     */
};

struct ItemType {               /* 0x10 bytes an item type                */
    char pad_0[2];              /* +0   not read here                     */
    unsigned char dice_n_large; /* +2   melee_attack copies it to dice_n  */
    unsigned char dice_s_large; /* +3   melee_attack copies it to dice_s  */
    char dmg_large;             /* +4                                     */
    unsigned char fire_rate;    /* +5                                     */
    char pad_6[5];              /* +6   not read here                     */
    char dmg;                   /* +b                                     */
    unsigned char range;        /* +c                                     */
    char pad_d[3];              /* +d   not read here                     */
};

struct SpellInfo {              /* 0x16 bytes a spell                     */
    char pad_0[2];              /* +0   not read here                     */
    unsigned char level;        /* +2                                     */
    char pad_3[4];              /* +3   not read here                     */
    unsigned char targeting;    /* +7   targeting: low 4 bits the kind    */
    char pad_8[6];              /* +8   not read here                     */
    unsigned char effect;       /* +e   effect that blocks it; 0: none    */
    int in_combat;              /* +f   0: camp only                      */
    unsigned char casting_time; /* +11  casting time; a third delays it   */
    char pad_12;                /* +12  not read here                     */
    int hostile;                /* +13  0: cast on the caster's own side  */
    char pad_15;                /* +15  not read here                     */
};

struct Aim {                    /* a spell's chosen target and cell       */
    struct Hero far *target;    /* +0                                     */
    int x;                      /* +4                                     */
    int y;                      /* +6                                     */
    char byte_8;                /* +8   not read here                     */
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

struct MapBox {                 /* a rectangle of combat-map cells        */
    char x0;                    /* +0                                     */
    char y0;                    /* +1                                     */
    char x1;                    /* +2                                     */
    char y1;                    /* +3                                     */
};

struct HotKey {
    unsigned char key;          /* +0  code the menu returns             */
    unsigned char ch;           /* +1  letter that selects it            */
};

struct Pool;
struct Effect;

struct Map {
    char pad_0[2];              /* +0   not read here                     */
    int view_x;                 /* +2   the view's left column            */
    int view_y;                 /* +4   the view's top row                */
    char cursor_on;             /* +6   nonzero: draw the cursor          */
    char cursor_size;           /* +7   the cursor's icon size            */
    char ignore_blocking;       /* +8   nonzero: terrain blocks nothing   */
    unsigned char cell[25][50]; /* +9   tile_info index from 1, by row    */
};

struct TileInfo {
    unsigned char move_cost;    /* +0   0xff: impassable                  */
    char pad_1[3];              /* +1   not read here                     */
};

struct Combatant {
    int x;                      /* +0                                     */
    int y;                      /* +2                                     */
    char number;                /* +4   not read here                     */
    unsigned char size;         /* +5                                     */
};

extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far hero_panel(struct Hero far *h);                   /* 03d0:0834 */
extern char far is_helpless(struct Hero far *h);                   /* 03d0:09f2 */
extern unsigned char far calc_move(struct Hero far *h);          /* 03d0:0a38 */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern char far dex_missile(struct Hero far *h);                 /* 03d0:1089 */
extern void far spell_mark(struct Hero far *h, unsigned char spell); /* 03d0:1381 */
extern void far item_remove(struct Hero far *h, struct Item far *it); /* 03d0:13c4 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern void far hero_text(struct Hero far *h, char far *s, char row, char pause);
                                                                 /* 03d0:1560 */
extern void far combat_text_clear(void);                                 /* 03d0:1657 */
extern void far character_name_show(struct Hero far *h, char col, char row, char poss);
                                                                 /* 03d0:1689 */
extern char far find_effect(struct Hero far *h, unsigned char kind,
                          struct Effect far * far *out);         /* 03d0:1726 */
extern void far hero_damage(struct Hero far *h, unsigned short dmg); /* 03d0:1792 */
extern unsigned char far caster_level(unsigned char spell, char cap); /* 03d0:18fe */
extern char far chance(unsigned char pct);                       /* 03d0:20d6 */
extern unsigned char far class_level(struct Hero far *h, int cls); /* 03d0:2177 */
extern void far status_printf(unsigned char col, unsigned char row,
                              unsigned char fg, unsigned char bg,
                              char far *fmt, ...);               /* 06c9:0044 */
extern void far text_wrap(unsigned char x1, unsigned char y1, unsigned char x2,
                          unsigned char y2, unsigned char fg, unsigned char bg,
                          char clear, char far *text);           /* 06c9:030f */
extern void far text_pause(void);                                /* 06c9:093c */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern void far box_draw_top(void);                                 /* 083a:03ea */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far set_menu_kind(int kind);                         /* 083a:13f2 */
extern char far grid_pick(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1842 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern char far pool_lowbit(int value);                          /* 0a75:032f */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern char far *far msg_text(int i);                            /* 0a75:039b */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far delay(unsigned ms);                              /* 0a75:0431 */
extern void far sfx_play(unsigned short n);                   /* 0c8e:000e */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern void far game_exit(void);                                 /* 0d48:002f */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern void far fx_disable(struct Hero far *h, int how, char far *msg);
                                /* 18de:10e9, vector to overlay 19 2e88:000f */
extern void far fx_hp_effects_remove(struct Hero far *h);
                                /* 18de:10fd, vector to overlay 19 2e88:17ec */
extern void far fx_remove(struct Hero far *hero, unsigned char kind,
                             struct Effect far *fx);
                                /* 18de:1107, vector to overlay 19 2e88:00d8 */
extern void far fx_trigger_group(unsigned char when, struct Hero far * far *hp);
                                /* 18de:1111, vector to overlay 19 2e88:0449 */
extern int  far fx_roll_dice(unsigned char n, unsigned char sides);
                                /* 18de:112f, vector to overlay 19 2e88:15ee */
extern void far spell_cast(unsigned char n, char a, char b, char far *done);
                                /* 18de:1175, vector to overlay 12 2b69:14c8 */
extern void far plan_steps(char x, char y, unsigned short budget, unsigned char dir,
                             unsigned char size);
                                /* 18de:1283, vector to overlay 2 2632:07d4 */
extern int  far fx_roll_damage(unsigned char n, unsigned char sides);
                                /* 18de:12ab, vector to overlay 19 2e88:1624 */
extern char far dir_ok(char x0, char y0, char x1, char y1, unsigned char who);
                                /* 18de:12bf, vector to overlay 2 2632:048c */
extern void far fx_out_of_play(struct Hero far *h, int how, char far *msg);
                                /* 18de:12d3, vector to overlay 19 2e88:16ef */
extern void far fx_invisibility_remove(struct Hero far *h);
                                /* 18de:12dd, vector to overlay 19 2e88:17b0 */
extern char far fx_attack_roll(struct Hero far *h, struct Hero far *t, unsigned char need);
                                /* 18de:12f1, vector to overlay 19 2e88:143d */
extern unsigned char far spell_range(unsigned char spell);
                                /* 18de:12fb, vector to overlay 12 2b69:0ff0 */
extern unsigned char far spell_menu(unsigned char list, unsigned char type,
                                      int far *choice, char far *more);
                                /* 18de:1305, vector to overlay 12 2b69:203e */
extern char far line_clear(char x0, char y0, char far *x1, char far *y1,
                             unsigned short far *power);
                                /* 18de:130f, vector to overlay 2 2632:0325 */
extern char far fx_saving_throw(struct Hero far *h, int type, char adj);
                                /* 18de:1323, vector to overlay 19 2e88:1552 */
extern void far fx_damage(struct Hero far *h, unsigned char dmg, int reduce,
                             char saved);
                                /* 18de:132d, vector to overlay 19 2e88:1ea2 */
extern void far icon_facing(unsigned char cell, unsigned char half, unsigned char row,
                            char mirror);                        /* 1cb8:0004 */
extern void far icon_facings(unsigned char cell);                /* 1cb8:0052 */
extern void far icon_fly(int x0, int y0, int x1, int y1, unsigned char frames,
                         unsigned char pace);                    /* 1cb8:00de */
extern int  far enemy_team(struct Hero far *h);                  /* 1cb8:08fc */
extern void far combat_sides_count(void);                              /* 1cb8:091f */
extern unsigned char far combat_targets_near(struct Hero far *h, unsigned char range,
                                      char same);                /* 1cb8:0971 */
extern unsigned char far get_distance(struct Hero far *h, struct Hero far *t);
                                                                 /* 1cb8:0a99 */
extern char far combat_actions_end(struct Hero far *h);                /* 1cb8:0b84 */
extern void far combat_view_show(void);                              /* 1cb8:0c07 */
extern char far has_ranged_weapon(struct Hero far *h);                /* 1cb8:0c32 */
extern char far has_throwable_weapon(struct Hero far *h);                /* 1cb8:0c71 */
extern void far missile_facing(unsigned char dir, unsigned char cell); /* 1cb8:0f2c */
extern char far ranged_ammo_find(struct Hero far *h, struct Item far * far *sel);
                                                                 /* 1cb8:0fb5 */
extern void far cell_redraw(char x, char y);                     /* 22fa:025e */
extern void far combat_grid_build(void);                               /* 22fa:0331 */
extern void far cell_get(char x, char y, unsigned char far *who,
                         unsigned char far *kind);               /* 22fa:0469 */
extern void far tile_update(char x, char y, unsigned char who);  /* 22fa:04d5 */
extern char far in_combat_view(char x, char y);                     /* 22fa:05f8 */
extern char far combatant_visible(struct Hero far *h, char whole);     /* 22fa:0627 */
extern void far combat_view_scroll(char x, char y, unsigned char margin,
                            unsigned char dir);                  /* 22fa:0847 */
extern void far combatant_facing_set(struct Hero far *h, unsigned char dir, char frame,
                            char erase);                         /* 22fa:09a2 */
extern char far combatant_x(struct Hero far *h);                 /* 22fa:0ab5 */
extern char far combatant_y(struct Hero far *h);                 /* 22fa:0ae4 */
extern unsigned char far combatant_size(struct Hero far *h);        /* 22fa:0b14 */
extern unsigned char far combatant_slot(struct Hero far *h);        /* 22fa:0b44 */
extern void far combat_direction_probe(struct Hero far *h, unsigned char dir, unsigned char far *who,
                            unsigned char far *cell, unsigned char far *kind);
                                                                 /* 22fa:0b9c */
extern void far hero_fall(struct Hero far *h);                   /* 22fa:0d0a */
extern void far view_center(struct Hero far *h, unsigned char margin, char cursor);
                                                                 /* 22fa:1286 */
extern unsigned char far combatant_direction(struct Hero far *from, struct Hero far *to);
                                                                 /* 22fa:166c */

extern struct SpellInfo spell_info[];                            /* 264a */
extern char far *ally_prompt;                                    /* 3b30 */
extern char far *range_text;                                     /* 3b34 */
extern char far *blank_text;                                     /* 3c04 */
extern char far *aim_items;                                      /* 3c8c */
extern char far *target_items;                                   /* 3c90 */
extern char dir_dx[9];                                           /* 5fe3 */
extern char dir_dy[9];                                           /* 5fec */
extern struct TileInfo tile_info[];                              /* 5ff5 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern char debug_word[];                                          /* b3f4 */
extern struct ItemType far *item_table;                          /* b40a */
extern char turn_done;                                           /* b414 */
extern unsigned char combat_round;                                  /* b418 */
extern unsigned char text_row;                                   /* b41a */
extern unsigned char move_rate;                                  /* b427 */
extern int damage;                                               /* b42f */
extern char refused;                                             /* b432 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern char same_team_attack_approved;                                           /* b4e9 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Item far *pool_items;                              /* b8e2 */
extern unsigned char attacks_used[2];                               /* b8e8 */
extern struct Hero far *heroes[];                                /* b8f5 */
extern unsigned char combat_round_limit;                                  /* bcf1 */
extern char movement_stops_attack_rate;                                           /* be61 */
extern unsigned char current_spell;                                  /* be64 */
extern char hits_left[2];                                        /* c34c */
extern unsigned char near_ids[];                                 /* c355 */
extern char view_cx;                                             /* c39f */
extern unsigned char order_idx;                                  /* c3a0 */
extern char combat_action_done;                                           /* c3a1 */
extern char view_cy;                                             /* c3bc */
extern struct Hero far *combat_order[0x49];                      /* c52a */
extern int element_mask;                                         /* c64e */
extern struct Map far *cur_map;                                  /* c66b */
extern char mirror_image_bypass;                                           /* ca08 */
extern unsigned char step_count;                                 /* ca10 */
extern struct Item far *dropped_item_boundary;                               /* ca16 */
extern char menu_escaped;                                        /* ca2a */
extern struct Step steps[];                                      /* ca2b */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char move_flag;                                           /* cc79 */
extern struct Combatant combatants[];                            /* cc7c */
extern struct Fallen recs[8];                                    /* ce14 */
extern int (far *target_scores)[37];                                  /* ce61 */
extern unsigned char rec_count;                                  /* cfd7 */
extern struct Hero far *targets[];                               /* cfe3 */
extern char panel_dirty;                                         /* d0fb */
extern unsigned char morale_base;                                /* d0fc */
extern char ui_mode;                                             /* d128 */
extern struct Hero far *node_list;                               /* d129 */
extern unsigned char target_count;                               /* d137 */

static int target_menu_choice;                                   /* adce */
static char target_step_direction;                               /* add0 */
static struct Step reachable_target_list[69];                    /* add1 */
static unsigned char reach;                                      /* aea0 */
static unsigned char cur_pick;                                   /* aea1 */
static unsigned char pick_cnt;                                   /* aea2 */
static struct Hero far *saved_aim;                               /* aea3 */
static int next_key;                                             /* aea7 */
static int dummy_int;                                            /* aea9 */
static int target_x;                                             /* aeab */
static int target_y;                                             /* aead */
static int unused_int;                                           /* aeaf */
static char fly_last_x;                                          /* aeb1 */
static char fly_next_x;                                          /* aeb2 */
static char fly_last_y;                                          /* aeb3 */
static char fly_next_y;                                          /* aeb4 */
static struct Hero far *target_hero;                             /* aeb5 */
static char click_accept;                                        /* aeb9 */
static char moved_by_click;                                      /* aeba */
static char unused_buffer[7];                                    /* aebb */

static int area_widths[7][3] = {   /* by row offset -3..3            */
    { -3, -3,  1 },
    { -3,  1,  2 },
    {  1,  2,  3 },
    {  1,  2,  3 },
    {  1,  2,  3 },
    { -3,  1,  2 },
    { -3, -3,  1 }
};

static char turn_table[13][11] = { /* turning undead: kind, level band */
    { 99, 10,  7,  4,  1,  1,  0,  0, -1, -1, -1 },
    { 99, 13, 10,  7,  1,  1,  0,  0,  0, -1, -1 },
    { 99, 16, 13, 10,  4,  1,  1,  0,  0,  0, -1 },
    { 99, 19, 16, 13,  7,  4,  1,  1,  0,  0, -1 },
    { 99, 20, 19, 16, 10,  7,  4,  1,  1,  0,  0 },
    { 99, 99, 20, 19, 13, 10,  7,  4,  1,  1,  0 },
    { 99, 99, 99, 20, 16, 13, 10,  7,  4,  1,  0 },
    { 99, 99, 99, 99, 20, 16, 13, 10,  7,  4,  1 },
    { 99, 99, 99, 99, 99, 20, 16, 13, 10,  7,  1 },
    { 99, 99, 99, 99, 99, 99, 20, 16, 13, 10,  4 },
    { 99, 99, 99, 99, 99, 99, 99, 20, 16, 13, 10 },
    { 99, 99, 99, 99, 99, 99, 99, 99, 19, 16, 10 },
    { 99, 99, 99, 99, 99, 99, 99, 99, 20, 19, 13 }
};

void far attacks_left_recalc(struct Hero far *h);
unsigned char far half_value(unsigned char v);
void far attack_register(struct Hero far *h, struct Hero far *t);
void far combat_attack(struct Hero far *h, struct Hero far *t, char mode,
                         struct Item far *missile, char far *done);
char far behind_target(struct Hero far *h, struct Hero far *t);
char far combat_target_allowed(struct Hero far *h, struct Hero far *t, char c);
unsigned char far fastest_enemy_speed(struct Hero far *h);
unsigned char far enemies_near(struct Hero far *h, unsigned char range);
void far range_penalty(struct Hero far *h, struct Hero far *t, unsigned char far *ac);
void far missile_hit(struct Hero far *h, struct Hero far *t, struct Item far *missile);
void far combat_aim(struct Hero far *h, char range, char a, char area, char friendly,
                         char far *found, struct Aim far *aim);
char far combat_target_pick(struct Hero far *h, char range, char a, char b, char friendly);
static char rank_target(struct Hero far *t, unsigned char spell, int side);
char far area_aim(unsigned char spell, struct Aim far *aim);
char far turn_target_find(struct Hero far *h, struct Hero far * far *target, int far *level);

/* The translated Pascal unit's empty initialization block (DKK cbtacts.pas). */
void far cbaction_unit_init(void)                                     /* 2dbf:0005 */
{
}

void far combat_round_begin(struct Hero far *h)                     /* 2dbf:0006 */
{
    unsigned char i;
    char cell;
    char blocked;

    h->fight->casting = 0;
    combat_direction_probe(h, 8, &cell, &cell, &blocked);
    h->fight->can_cast = 0;
    for (i = 0; i <= 0x8c; i++)
        if (h->spells[i] > 0 && h->spells[i] < 0x80)
            h->fight->can_cast = 1;
    h->fight->can_cast = h->fight->can_cast && (char)(blocked == 0);
    h->fight->can_use_item = blocked == 0;
    h->fight->attacked = 0;
    h->fight->hand = 2;
    attacks_left_recalc(h);
    move_rate = h->attack_rate_off;
    movement_stops_attack_rate = 0;
    fx_trigger_group(0x12, &h);
    h->attacks[1] = half_value(move_rate);
    h->fight->sweep_targets = h->sweep_targets;
    if (h->active) {
        h->fight->initiative = fx_roll_dice(1, 6) + dex_missile(h);
        if (h->fight->initiative < 1)
            h->fight->initiative = 1;
        if (cursor_obj[0x2e] & (h->side + 1))
            h->fight->initiative -= 6;
        if (h->fight->initiative < 0 || h->fight->initiative > 20)
            h->fight->initiative = 0;
    } else
        h->fight->initiative = 0;
    h->fight->moves = calc_move(h);
}

void far roll_damage(struct Hero far *h, struct Hero far *t, unsigned char hand) /* 2dbf:01eb */
{
    unsigned char mult;

    damage = fx_roll_damage((h->dice_n - 1)[hand], (h->dice_s - 1)[hand])
             + (h->dmg - 1)[hand];
    if (damage < 0)
        damage = 0;
    fx_trigger_group(4, &h);
    fx_trigger_group(5, &t);
    if (behind_target(h, t)) {
        mult = (class_level(h, 6) - 1) / 4 + 2;
        if (mult > 5)
            mult = 5;
        damage = mult * damage;
    }
    element_mask = 0;
}

void far hit_report(struct Hero far *h, struct Hero far *t, unsigned char mode,
                    unsigned dealt, int shown, char hit)         /* 2dbf:02b2 */
{
    unsigned char row;
    char text[0x52];

    switch (mode) {
    case 2:
        str_copy(text, "-Backstabs-");
        break;
    case 3:
        str_copy(text, "slays helpless");
        break;
    default:
        str_copy(text, "Attacks");
        break;
    }
    hero_text(h, text, 10, 0);
    row = 12;
    character_name_show(t, 0x17, row, 0);
    row++;
    if (mode == 1)
        str_copy(text, "(from behind) ");
    else
        str_copy(text, "");
    if (hit) {
        if (mode == 3)
            str_copy(text, "with one cruel blow");
        else {
            str_copy(text, msg_format("%sHitting for %s", text, byte_str(shown)));
            if (shown == 1)
                str_copy(text, msg_format("%s point ", text));
            else
                str_copy(text, msg_format("%s points ", text));
            str_copy(text, msg_format("%sof damage", text));
        }
        hero_damage(t, dealt);
    } else
        str_copy(text, msg_format("%sand Misses", text));
    if (t->status != 8)
        text_wrap(0x17, row, 0x26, row + 3, 7, 0, 1, text);
    row = text_row + 1;
    if (dealt > 0) {
        t->fight->can_cast = 0;
        if (t->fight->casting > 0) {
            text_pause();
            hero_text(t, "lost a spell", 12, 1);
            spell_mark(t, t->fight->casting);
            combat_actions_end(t);
        } else
            text_pause();
    } else
        text_pause();
    if (!t->active) {
        hero_text(t, "goes down", row, 0);
        if (!(t->immunity & 8)) {
            row += 2;
            if (t->status == 5)
                status_printf(0x17, row, 7, 0, "and is Dying");
            else if (t->status == 6 || t->status == 8 || t->status == 7)
                hero_text(t, "is killed", row, 0);
            row += 2;
        }
        fx_hp_effects_remove(t);
        fx_trigger_group(0xd, &t);
        if (!t->active && t->status != 10)
            hero_fall(t);
        else
            text_pause();
    }
    combat_text_clear();
}

void far guard_strikes(struct Hero far *h)                       /* 2dbf:05cb */
{
    unsigned char count;
    unsigned char i;
    struct Hero far *t;
    char result;

    count = combat_targets_near(h, 1, 0);
    for (i = 1; i <= count; i++)
        if (h->active) {
            t = (heroes - 1)[(near_ids - 1)[i]];
            if (t->fight->guarding && !is_helpless(t)) {
                combat_view_scroll(combatant_x(h), combatant_y(h), 2, 8);
                t->fight->guarding = 0;
                attack_register(t, h);
                combat_attack(t, h, 0, 0L, &result);
            }
        }
}

void far combat_move_step(struct Hero far *h, unsigned char dir)  /* 2dbf:06b2 */
{
    unsigned char cost;
    unsigned char slot;
    char x;
    char y;
    char x0;
    char y0;

    slot = combatant_slot(h);
    x0 = (combatants - 1)[slot].x;
    y0 = (combatants - 1)[slot].y;
    x = dir_dx[dir] + x0;
    y = dir_dy[dir] + y0;
    if (pool_lowbit(dir))
        cost = (tile_info - 1)[cur_map->cell[y][x]].move_cost * 3;
    else
        cost = (tile_info - 1)[cur_map->cell[y][x]].move_cost * 2;
    if (h->fight->moves < cost)
        h->fight->moves = 0;
    else
        h->fight->moves -= cost;
    cost = 1;
    if (h->quick_fight) {
        cost = 3;
        if (!in_combat_view(x - cur_map->view_x, y - cur_map->view_y) && move_flag)
            combat_view_scroll(x0, y0, 2, 8);
    }
    if (move_flag)
        tile_update(0, 0, slot);
    (combatants - 1)[slot].x = x;
    (combatants - 1)[slot].y = y;
    combat_grid_build();
    if (move_flag)
        combat_view_scroll(x, y, cost, 8);
    h->fight->attacks_taken = 0;
    h->fight->attack_angles = 0;
    sfx_play(0xb);
    guard_strikes(h);
    if (!h->active || is_helpless(h))
        h->fight->moves = 0;
}

void far opportunity_attacks(struct Hero far *h, unsigned char dir)  /* 2dbf:08d8 */
{
    unsigned char cands[12];
    struct Effect far *info;
    char dup;
    char done;
    unsigned char slot;
    unsigned char count;
    unsigned char count2;
    unsigned char scan;
    unsigned char i;
    unsigned char hand;
    unsigned char sel;
    unsigned char side;
    struct Hero far *t;
    struct Hero far *saved;

    slot = combatant_slot(h);
    count = combat_targets_near(h, 1, 0);
    if (count != 0) {
        mem_zero(cands, sizeof cands);
        i = 1;
        for (scan = 1; scan <= count; scan++)
            if (!is_helpless((heroes - 1)[(near_ids - 1)[scan]])
                && !(heroes - 1)[(near_ids - 1)[scan]]->fight->panicked
                && !find_effect((heroes - 1)[(near_ids - 1)[scan]], 0x1b, &info)) {
                (cands - 1)[i] = (near_ids - 1)[scan];
                i++;
            }
        count = i - 1;
        (combatants - 1)[slot].x += dir_dx[dir];
        (combatants - 1)[slot].y += dir_dy[dir];
        count2 = combat_targets_near(h, 1, 0);
        (combatants - 1)[slot].x -= dir_dx[dir];
        (combatants - 1)[slot].y -= dir_dy[dir];
        for (i = 1; i <= count; i++) {
            dup = 0;
            for (scan = 1; scan <= count2; scan++)
                if ((cands - 1)[i] == (near_ids - 1)[scan])
                    dup = 1;
            if (dup)
                (cands - 1)[i] = 0;
        }
        for (i = 1; i <= count; i++)
            if ((cands - 1)[i] > 0 && h->active) {
                panel_dirty = 1;
                move_flag = 1;
                done = 0;
                t = (heroes - 1)[(cands - 1)[i]];
                if (combat_target_allowed(t, h, 0)
                    && (!has_ranged_weapon(t) || has_throwable_weapon(t)))
                    for (side = t->fight->facing + 6; side <= t->fight->facing + 10; side++)
                        if (!done
                            && (t->fight->initiative > 0 || !t->fight->attacks_taken
                                || dir_ok(combatant_x(t), combatant_y(t),
                                                combatant_x(h), combatant_y(h),
                                                side % 8))) {
                            sel = 1;
                            if (t->attack_rate == 0)
                                sel = 2;
                            for (hand = 1; hand <= 2; hand++)
                                if ((t->attacks - 1)[hand] > 0)
                                    sel = hand;
                            if ((t->attacks - 1)[sel] == 0)
                                (t->attacks - 1)[sel] = 1;
                            t->fight->hand = sel;
                            saved = t->fight->target;
                            combat_attack(t, h, 1, 0L, &combat_action_done);
                            done = 1;
                            t->fight->target = saved;
                            if (h->active) {
                                panel_dirty = 1;
                                hero_panel(h);
                            }
                        }
            }
    }
}

char far combat_flee(struct Hero far *h)                     /* 2dbf:0d02 */
{
    char r;
    char got_away;
    unsigned char value;
    unsigned char need;

    got_away = 0;
    if (combat_targets_near(h, 0xff, 0) == 0)
        got_away = 1;
    else {
        need = calc_move(h) / 2;
        value = fastest_enemy_speed(h);
        if (value < need || value == need && fx_roll_dice(1, 2) == 1)
            got_away = 1;
    }
    if (got_away)
        fx_out_of_play(h, 3, "Got Away");
    else
        bottom_msg("Escape is blocked");
    r = combat_actions_end(h);
    return r;
}

void far attacks_left_recalc(struct Hero far *h)                     /* 2dbf:0db4 */
{
    unsigned char value;
    unsigned char orig;
    unsigned char cand;
    unsigned char rate;
    char found;
    struct Item far *sel;

    found = 0;
    orig = h->attacks[0];
    h->attacks[0] = h->attack_rate;
    if (has_ranged_weapon(h) && ranged_ammo_find(h, &sel)) {
        found = 1;
        rate = item_table[h->equipped[0]->type].fire_rate;
        if (rate < 2)
            rate = 2;
        move_rate = rate;
    } else
        move_rate = h->attacks[0];
    movement_stops_attack_rate = 0;
    fx_trigger_group(0x12, &h);
    value = half_value(move_rate);
    if (found && sel != 0) {
        cand = 1;
        if (sel != 0 && sel->count > cand)
            cand = sel->count;
        if (cand < value && sel->count > 0)
            value = cand;
    }
    if (h->fight->attacked == 0 || value < orig)
        h->attacks[0] = value;
    else
        h->attacks[0] = orig;
}

unsigned char far half_value(unsigned char v)                    /* 2dbf:0ed1 */
{
    unsigned char r;
    unsigned char n;

    n = v;
    if (pool_lowbit(combat_round))
        n++;
    r = n / 2;
    return r;
}

char far combat_target_allowed(struct Hero far *h, struct Hero far *t, char any) /* 2dbf:0f05 */
{
    char r;
    struct Hero far *saved;

    r = 0;
    if (t != 0)
        if (h == t)
            r = 1;
        else {
            refused = 0;
            saved = cur_node;
            cur_node = h;
            fx_trigger_group(1, &t);
            cur_node = saved;
            if (!refused && (t->side != h->side || any)) {
                saved_aim = h->fight->target;
                h->fight->target = t;
                fx_trigger_group(0, &h);
                h->fight->target = saved_aim;
            } else
                refused = 1;
            r = !refused;
        }
    return r;
}

void far turn_undead(struct Hero far *h)                     /* 2dbf:0fec */
{
    unsigned char roll;
    unsigned char left;
    unsigned char count;
    char row;
    char turned;
    struct Hero far *t;
    char col;
    int level;

    hero_text(h, "turns undead...", 10, 0);
    box_draw();
    text_pause();
    turned = 0;
    h->fight->turn_attempts++;
    level = 0xd;
    count = 6;
    roll = fx_roll_dice(1, 0x14);
    col = class_level(h, 0);
    if (class_level(h, 3) - 2 > col)
        col = class_level(h, 3) - 2;
    if (col > 8)
        if (col < 14)
            col = 9;
        else
            col = 10;
    if (cursor_obj[0xf] > col)
        col = 0;
    else
        col -= cursor_obj[0xf];
    left = 0xff;
    while (turn_target_find(h, &t, &level) && left > 0) {
        row = turn_table[t->undead - 1][col];
        if (left == 0xff)
            if (row < 0)
                left = fx_roll_dice(1, 6) + 6;
            else
                left = fx_roll_dice(1, 0xc);
        if (abs(row) <= roll) {
            turned = 1;
            view_center(t, 3, 0);
            panel_dirty = 1;
            hero_panel(t);
            if (row > 0)
                fx_out_of_play(t, 3, "is turned");
            else {
                hero_text(t, "Is destroyed", 10, 0);
                hero_fall(t);
                t->status = 8;
                t->active = 0;
            }
            if (count > 0)
                count--;
            left--;
            if (left == 0 && count > 0 && row < 0)
                left++;
            combat_text_clear();
        } else
            level--;
    }
    if (!turned)
        bottom_msg("Nothing Happens...");
    combat_sides_count();
    combat_action_done = combat_actions_end(h);
    combat_text_clear();
}

char far turn_target_find(struct Hero far *h, struct Hero far * far *target,
                       int far *level)                           /* 2dbf:1229 */
{
    char found;
    unsigned char count;
    unsigned char i;
    short best;
    struct Hero far *t;

    best = 0;
    *target = 0;
    count = combat_targets_near(h, 0xff, 0);
    found = 0;
    for (i = 1; i <= count; i++) {
        t = (heroes - 1)[(near_ids - 1)[i]];
        if (!t->fight->panicked && t->undead > 0 && t->undead > best
            && t->undead <= *level) {
            best = t->undead;
            *target = t;
            found = 1;
        }
    }
    *level = best;
    return found;
}

void far melee_attack(struct Hero far *h, struct Hero far *t, char mode, char far *done,
                      struct Item far *missile)                  /* 2dbf:12eb */
{
    struct ItemType info;
    char hit;
    char stop;
    char rear;
    unsigned char ac;
    unsigned char hand;
    unsigned char hands;
    unsigned char how;
    short side;

    rear = mode;
    *done = 0;
    hits_left[0] = 0;
    hits_left[1] = 0;
    attacks_used[0] = 0;
    attacks_used[1] = 0;
    hit = 0;
    stop = 0;
    damage = 0;
    h->fight->attacked = 1;
    if (is_helpless(t)) {
        sfx_play(8);
        while ((h->attacks - 1)[h->fight->hand] == 0)
            h->fight->hand--;
        (attacks_used - 1)[h->fight->hand]++;
        hit_report(h, t, 3, t->hp + 5, 1, 1);
        fx_invisibility_remove(h);
        h->attacks[0] = 0;
        h->attacks[1] = 0;
        hit = 1;
        *done = 1;
    } else {
        if (h->equipped[0] != 0 && (t->icon_dim > 0x80 || (t->icon_dim & 7) > 1)) {
            info = item_table[h->equipped[0]->type];
            h->dice_n[0] = info.dice_n_large;
            h->dice_s[0] = info.dice_s_large;
            h->dmg[0] = h->dmg[0] - info.dmg + info.dmg_large;
        }
        recalc_stats(t);
        fx_trigger_group(0xb, &t);
        side = 0;
        if (behind_target(h, t))
            ac = t->ac_front_rear[1] - 4;
        else {
            if (t->fight->attacks_taken > 1 && combatant_direction(h, t) == t->fight->facing
                && t->fight->attack_angles > 4)
                rear = 1;
            if (rear)
                side = 1;
            ac = t->ac_front_rear[side];
        }
        range_penalty(h, t, &ac);
        how = 0;
        if (rear)
            how = 1;
        if (behind_target(h, t))
            how = 2;
        hands = h->fight->hand;
        for (hand = hands; hand >= 1; hand--) {
            while ((h->attacks - 1)[hand] > 0 && !stop) {
                (h->attacks - 1)[hand]--;
                h->fight->hand = hand;
                (attacks_used - 1)[hand]++;
                if (fx_attack_roll(h, t, ac) || is_helpless(t)) {
                    (hits_left - 1)[hand]++;
                    sfx_play(8);
                    hit = 1;
                    roll_damage(h, t, hand);
                    if (t->active)
                        hit_report(h, t, how, damage, damage, 1);
                    if (t->active && damage > 0)
                        fx_trigger_group(hand + 1, &h);
                    if (!t->active)
                        stop = 1;
                    if (!h->active) {
                        hands = 0;
                        (h->attacks - 1)[hand] = 0;
                    }
                }
            }
        }
        if (!hit) {
            if (missile == 0)
                sfx_play(10);
            hit_report(h, t, how, 0, 0, 0);
        }
        *done = 1;
        for (hand = 1; hand <= 2; hand++)
            if ((h->attacks - 1)[hand] > 0)
                *done = 0;
        h->fight->sweep_targets = 0;
    }
    if (!h->active)
        *done = 1;
    if (*done)
        *done = combat_actions_end(h);
}

void far attack_register(struct Hero far *h, struct Hero far *t) /* 2dbf:170a */
{
    unsigned char turn;
    unsigned char dir;

    t->fight->attacks_taken++;
    dir = combatant_direction(t, h);
    turn = (dir - t->fight->facing + 8) % 8;
    if (turn > 4)
        turn = 8 - turn;
    t->fight->attack_angles = (t->fight->attack_angles + turn) % 8;
}

void far combat_attack(struct Hero far *h, struct Hero far *t, char mode,
                         struct Item far *missile, char far *done) /* 2dbf:178d */
{
    struct Item far *copy;
    struct Item far *p;
    struct Hero far *saved;
    unsigned char dir;

    move_flag = 1;
    panel_dirty = 1;
    combat_round_limit = combat_round + 0xf;
    if (t->fight->attacks_taken < 2 && !mode) {
        dir = combatant_direction(t, h);
        t->fight->facing = (dir + 4) % 8;
    } else if (combatant_visible(t, 0)) {
        dir = t->fight->facing;
        if (!mode)
            t->fight->facing = (dir + 4) % 8;
    }
    if (combatant_visible(t, 0))
        combatant_facing_set(t, dir, 0, 0);
    dir = combatant_direction(h, t);
    hero_panel(h);
    combatant_facing_set(h, dir, 1, 0);
    h->fight->target = t;
    delay(100);
    if (missile != 0)
        missile_hit(h, t, missile);
    if (h->equipped[0]->type == 0x1c || h->equipped[0]->type == 0x1d) /* slot[0] unchecked */
        missile_hit(h, t, h->equipped[0]);
    *done = 1;
    if (h->attacks[0] > 0 || h->attacks[1] > 0) {
        saved = cur_node;
        cur_node = h;
        melee_attack(h, t, mode, done, missile);
        if (missile != 0) {
            if (missile->count > 0)
                missile->count -= attacks_used[0];
            if (missile->count == 0)
                if (has_throwable_weapon(h) && missile->effect != 0x17) {
                    pool_alloc((void far * far *)&copy, 0x3e, &item_record_pool);
                    mem_move5(missile, copy, 0x3e);
                    p = pool_items;
                    if (p == 0) {
                        pool_items = copy;
                        copy->next = 0;
                    } else {
                        while (p->next != 0 && p != dropped_item_boundary)
                            p = p->next;
                        copy->next = p->next;
                        p->next = copy;
                    }
                    copy->readied = 0;
                    item_remove(h, missile);
                    dropped_item_boundary = copy;
                } else
                    item_remove(h, missile);
        }
        recalc_stats(h);
        cur_node = saved;
    }
    if (*done)
        *done = combat_actions_end(h);
    if (combatant_visible(h, 0)) {
        combatant_facing_set(h, h->fight->facing, 1, 1);
        combatant_facing_set(h, h->fight->facing, 0, 0);
    }
}

void far range_penalty(struct Hero far *h, struct Hero far *t,
                       unsigned char far *ac)                    /* 2dbf:1afe */
{
    char step;
    char dist;

    dist = get_distance(h, t);
    if (has_ranged_weapon(h))
        step = (item_table[h->equipped[0]->type].range - 1) / 3;
    else
        step = dist;
    if (dist - step > 0) {
        dist = dist - step;
        *ac = *ac + 2;
    }
    if (dist - step > 0) {
        dist = dist - step;
        *ac = *ac + 3;
    }
}

char far aim_target(struct Aim far *aim, int far *side, struct Hero far * far *target,
                    unsigned char far *spell, char far *ok)      /* 2dbf:1ba3 */
{
    char r;
    char found;
    char friendly;

    found = 0;
    *ok = 1;
    friendly = spell_info[*spell].hostile == 0;
    if (combat_target_pick(cur_node, spell_range(*spell), 0, 1, friendly)) {
        *target = cur_node->fight->target;
        if (friendly)
            *side = cur_node->side;
        else
            *side = enemy_team(cur_node);
        *ok = rank_target(*target, *spell, *side) > 0;
        if (*ok) {
            aim->target = cur_node->fight->target;
            aim->x = combatant_x(aim->target);
            aim->y = combatant_y(aim->target);
            found = 1;
        }
    }
    r = found;
    return r;
}

char far choose_aim(unsigned char spell, char chosen, char area,
                    struct Aim far *aim)                         /* 2dbf:1cdb */
{
    char r;
    char found;
    char ok;
    struct Hero far *target;
    char friendly;
    int side;

    found = 0;
    if (!chosen) {
        friendly = spell != 0x53;
        combat_aim(cur_node, spell_range(spell), 0, area, friendly, &found, aim);
        cur_node->fight->target = aim->target;
    } else if (!area || spell_info[spell].targeting == 8)
        found = aim_target(aim, &side, &target, &spell, &ok);
    else
        found = area_aim(spell, aim);
    if (found) {
        r = 1;
        view_cx = aim->x;
        view_cy = aim->y;
    } else {
        r = 0;
        mem_fill(aim, sizeof(struct Aim), 0);
    }
    return r;
}

void far combat_spell_targets(unsigned char spell, char mode, char far *result) /* 2dbf:1ddb */
{
    unsigned char left;
    unsigned char count;
    unsigned char i;
    unsigned char limit;
    unsigned char cost;
    struct Aim aim;
    char dup;
    char stop;

    mem_zero(targets, 4);
    *result = 1;
    target_count = 0;
    mirror_image_bypass = 0;
    view_cx = combatant_x(cur_node);
    view_cy = combatant_y(cur_node);
    switch (spell_info[spell].targeting & 0xf) {
    case 0:
        targets[1] = cur_node;
        target_count = 1;
        break;
    case 5:
        cost = 0;
        count = 0;
        if (spell == 0x4f)
            limit = caster_level(0x4f, 0);
        else
            limit = fx_roll_dice(2, 4);
        stop = 0;
        do {
            if (choose_aim(spell, mode, 0, &aim)) {
                dup = 0;
                for (i = 1; i <= count; i++)
                    if (targets[i] == aim.target)
                        dup = 1;
                if (!dup) {
                    count++;
                    targets[count] = aim.target;
                    view_cx = combatant_x(aim.target);
                    view_cy = combatant_y(aim.target);
                    target_count++;
                    if (spell != 0x4f)
                        switch (targets[count]->hit_dice) {
                        case 0:
                        case 1:
                            cost++;
                            break;
                        case 2:
                            cost += 2;
                            break;
                        case 3:
                            cost += 4;
                            break;
                        default:
                            cost += 8;
                        }
                    else
                        switch (targets[count]->icon_dim) {
                        case 1:
                            cost++;
                            break;
                        case 2:
                        case 3:
                            cost += 2;
                            break;
                        case 4:
                            cost += 4;
                        }
                    if (count > 1 && cost > limit)
                        stop = 1;
                } else if (mode)
                    limit--;
                else
                    bottom_msg("Already been targeted");
                cell_redraw(combatant_x(aim.target), combatant_y(aim.target));
            } else
                stop = 1;
        } while (!stop && limit != 0);
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        if (choose_aim(spell, mode, 1, &aim)) {
            if (spell == 0x57 && cursor_obj[0x3c])
                plan_steps(view_cx, view_cy, 3, 0xff, 1);
            else if (spell == 0x33 && cursor_obj[0x3c] || spell == 0x73 && !cursor_obj[0x22]
                     || spell == 0x2f && !cursor_obj[0x22])
                plan_steps(view_cx, view_cy, 2, 0xff, 1);
            else
                plan_steps(view_cx, view_cy, spell_info[spell].targeting & 7, 0xff, 1);
            for (count = 1; count <= step_count; count++)
                targets[count] = (heroes - 1)[(steps - 1)[count].who];
            target_count = step_count;
            mirror_image_bypass = 1;
        } else
            *result = 0;
        break;
    case 15:
        if (choose_aim(spell, mode, 0, &aim)) {
            if (cur_node->fight->target != 0) {
                targets[1] = cur_node->fight->target;
                target_count = 1;
            } else {
                plan_steps(view_cx, view_cy, spell_info[spell].targeting >> 4, 0xff, 1);
                for (count = 1; count <= step_count; count++)
                    targets[count] = (heroes - 1)[(steps - 1)[count].who];
                target_count = step_count;
                mirror_image_bypass = 1;
            }
        } else
            *result = 0;
        break;
    default:
        left = (spell_info[spell].targeting & 3) + 1;
        count = 0;
        while (left > 0)
            if (choose_aim(spell, mode, 0, &aim)) {
                dup = 0;
                for (i = 1; i <= count; i++)
                    if (targets[i] == aim.target)
                        dup = 1;
                if ((spell == 0x17 || spell == 0x31) && aim.target == cur_node)
                    dup = 1;
                if (!dup) {
                    count++;
                    targets[count] = aim.target;
                    left--;
                    view_cx = combatant_x(aim.target);
                    view_cy = combatant_y(aim.target);
                } else if (!mode) {
                    if (aim.target == cur_node)
                        bottom_msg("Cannot target yourself");
                    else
                        bottom_msg("Already been targeted");
                } else
                    left--;
                cell_redraw(combatant_x(aim.target), combatant_y(aim.target));
            } else
                left = 0;
        target_count = count;
        if (count == 0)
            *result = 0;
        else {
            view_cx = combatant_x(targets[count]);
            view_cy = combatant_y(targets[count]);
        }
    }
}

void far combat_cast_begin(unsigned char spell, char mode, char far *done) /* 2dbf:23a6 */
{
    unsigned char kind;
    unsigned char delay_by;
    unsigned char i;
    int choice;
    char more;
    struct Hero far *cur;
    struct Hero far *next;

    cur = cur_node;
    more = 1;
    choice = -1;
    *done = 0;
    kind = spell;
    if (kind == 0)
        kind = spell_menu(0, 1, &choice, &more);
    if (kind > 0 && spell_info[kind].in_combat == 0) {
        bottom_msg("Camp Only Spell");
        kind = 0;
    }
    if (cursor_obj[0x3c] && (kind == 0x4a || kind == 9 || kind == 0x2f || kind == 0x76
                             || kind == 0x73 || kind == 0x7c)) {
        bottom_msg("That has no effect underwater!");
        kind = 0;
    }
    if (!mode) {
        combat_view_show();
        move_flag = 1;
        panel_dirty = 1;
        view_center(cur, 3, 1);
        hero_panel(cur);
    }
    if (kind > 0) {
        delay_by = spell_info[kind].casting_time / 3;
        if (delay_by == 0) {
            spell_cast(kind, mode, 1, done);
            *done = combat_actions_end(cur);
        } else {
            *done = 1;
            hero_text(cur, "Begins Casting", 10, 1);
            cur->fight->casting = kind;
            if (cur->fight->initiative > delay_by)
                cur->fight->initiative -= delay_by;
            else
                cur->fight->initiative = 1;
            i = order_idx;
            goto check;
        shift:
            combat_order[i] = next;
            i++;
        check:
            next = combat_order[i + 1];
            if (next != 0
                && (next->fight->initiative > cur->fight->initiative
                    || next->fight->initiative == cur->fight->initiative
                       && fx_roll_dice(1, 2) == 1))
                goto shift;
            combat_order[i] = cur;
            order_idx--;
            *done = 1;
        }
    }
}

char far behind_target(struct Hero far *h, struct Hero far *t)   /* 2dbf:25f2 */
{
    char r;
    struct Item far *w;
    unsigned char type;
    char able;

    able = 0;
    w = h->equipped[0];
    if (w != 0)
        type = w->type;
    if (class_level(h, 6) > 0)
        if (w == 0 || type == 3 || type == 4 || type == 0x11 || type == 0xd || type == 0x12
            || type == 0x13 || type == 0x3f || type == 0x43)
            able = 1;
    if (able && t->fight->attacks_taken > 1 && get_distance(h, t) < 2 && (t->icon_dim & 0x7f) <= 1
        && combatant_direction(h, t) == t->fight->facing)
        r = 1;
    else
        r = 0;
    return r;
}

void far missile_hit(struct Hero far *h, struct Hero far *t,
                     struct Item far *missile)                   /* 2dbf:26d7 */
{
    unsigned char frames;
    unsigned char dir;
    unsigned char pace;

    dir = combatant_direction(h, t);
    frames = 1;
    pace = 10;
    switch (missile->type) {
    case 4:
    case 5:
    case 7:
    case 0xc:
    case 0x1e:
    case 0x42:
    case 0x4a:
        missile_facing(dir, 0xf);
        sfx_play(0xd);
        break;
    case 2:
    case 3:
    case 6:
        icon_facings(0x12);
        frames = 4;
        pace = 0x32;
        sfx_play(10);
        break;
    case 0x1c:
    case 0x1d:
    case 0x43:
        icon_facing(0x16, 0, 0, 0);
        icon_facing(0x16, 1, 1, 0);
        frames = 2;
        pace = 10;
        sfx_play(7);
        break;
    default:
        icon_facing(0x15, 0, 0, 0);
        icon_facing(0x15, 1, 1, 0);
        frames = 2;
        pace = 0x14;
        sfx_play(10);
    }
    icon_fly(combatant_x(h), combatant_y(h), combatant_x(t), combatant_y(t),
             frames, pace);
}

void far hostile_hp_percent_update(void)                                   /* 2dbf:2840 */
{
    unsigned short total;
    unsigned short left;
    struct Hero far *h;

    total = 0;
    left = 0;
    for (h = node_list; h != 0; h = h->next)
        if (h->side == 1 && h->status != 9) {
            if (h->active)
                left = left + h->hp;
            total = total + h->hp_max;
        }
    if (total > 0)
        morale_base = left * 20 / total * 5;
}

unsigned char far fastest_enemy_speed(struct Hero far *h)              /* 2dbf:28c5 */
{
    unsigned char r;
    unsigned char best;
    unsigned char rate;
    struct Hero far *p;

    best = 0;
    for (p = node_list; p != 0; p = p->next)
        if (enemy_team(h) == p->side && p->active) {
            rate = calc_move(p) / 2;
            if (rate > best)
                best = rate;
        }
    r = best;
    return r;
}

char far attack_confirm(struct Hero far *h, struct Hero far *t) /* 2dbf:2942 */
{
    char r;
    char saved;

    saved = menu_escaped;
    if (enemy_team(t) == h->side || h->quick_fight)
        r = 1;
    else if (!confirm(ally_prompt, 0))
        r = 0;
    else {
        r = 1;
        same_team_attack_approved = 1;
        cursor_obj[0x36] = 1;
        combat_sides_count();
    }
    menu_escaped = saved;
    return r;
}

unsigned char far target_menu(struct Hero far *h, struct Hero far *t, unsigned char range,
                              char attack, char empty_ok, char any_ok, char enabled, char x,
                              char y)                            /* 2dbf:29b7 */
{
    unsigned char r;
    struct Item far *sel;
    unsigned dist;
    char open;
    unsigned char i;
    unsigned char count;

    dist = 0xff;
    open = cur_map->ignore_blocking;
    cur_map->ignore_blocking = 1;
    line_clear(combatant_x(h), combatant_y(h), &x, &y, &dist);
    cur_map->ignore_blocking = open;
    dist = dist >> 1;
    if (attack)
        status_printf(0, 0x17, 7, 0, msg_format("%s%d ", range_text, dist));
    mem_fill(hotkeys, sizeof hotkeys, 0xff);
    count = 0;
    for (i = 0; i <= 5; i++)
        if (i == 3) {
            if (range >= dist && enabled)
                if (!attack) {
                    if (any_ok && t != 0 || empty_ok && t == 0)
                        hotkey_add(3, &count);
                } else if (h != t)
                    if (!has_ranged_weapon(h))
                        hotkey_add(3, &count);
                    else if (ranged_ammo_find(h, &sel))
                        if (combat_targets_near(h, 1, 0) == 0 || has_throwable_weapon(h))
                            hotkey_add(3, &count);
        } else
            hotkey_add(i, &count);
    if (t != 0) {
        view_center(t, 3, 1);
        panel_dirty = 1;
        hero_panel(t);
    }
    r = menu_run(blank_text, aim_items, 1, 0);
    return r;
}

void far aim_at(struct Hero far *h, struct Hero far *t, char mode, char far *result,
                struct Aim far *aim)                             /* 2dbf:2b9d */
{
    struct Item far *sel;

    sel = 0;
    *result = 1;
    if (mode == 1 && !attack_confirm(h, t))
        *result = 0;
    if (*result) {
        aim->target = t;
        aim->x = combatant_x(t);
        aim->y = combatant_y(t);
        cur_map->cursor_on = 0;
        combat_view_scroll(cur_map->view_x + 3, cur_map->view_y + 3, 3, 8);
        if (mode == 1) {
            attack_register(h, t);
            if (has_ranged_weapon(h) && ranged_ammo_find(h, &sel) && has_throwable_weapon(h)
                && get_distance(h, t) == 1)
                sel = 0;
            combat_attack(h, t, 0, sel, result);
        }
    } else
        mem_fill(aim, sizeof(struct Aim), 0);
}

void far aim_menu(struct Hero far *h, struct Hero far *t, unsigned char range, char attack,
                  char mode, char pair_ok, char far *active, char far *accepted,
                  struct Aim far *aim, char x, char y)           /* 2dbf:2cf3 */
{
    unsigned char count;
    unsigned char key;
    unsigned char dir;
    unsigned char who;
    unsigned char kind;
    unsigned dist;
    char tx;
    char ty;
    unsigned char i;
    struct Item far *sel;

    mem_fill(aim, sizeof(struct Aim), 0);
    x = combatant_x(t);
    y = combatant_y(t);
    target_x = x;
    target_y = y;
    key = 0xff;
    dir = 8;
    *accepted = 0;
    cur_map->cursor_on = 1;
    cur_map->cursor_size = 1;
    moved_by_click = 0;
    while (key != 0 && key != 2 || moved_by_click) {
        if (!moved_by_click) {
            combat_view_scroll(x, y, 3, dir);
            x += dir_dx[dir];
            y += dir_dy[dir];
            if (x < 0)
                x = 0;
            if (y < 0)
                y = 0;
            if (x > 0x31)
                x = 0x31;
            if (y > 0x18)
                y = 0x18;
        } else {
            x = target_x;
            y = target_y;
        }
        cell_get(x, y, &who, &kind);
        dist = 0xff;
        *active = 0;
        tx = x;
        ty = y;
        target_x = x;
        target_y = y;
        if (line_clear(combatant_x(h), combatant_y(h), &tx, &ty, &dist)) {
            *active = 1;
            if (attack)
                status_printf(0, 0x17, 7, 0, msg_format("%s%d\t", range_text, dist >> 1));
        } else if (attack)
            box_draw_top();
        dist = dist >> 1;
        t = 0;
        if (*active)
            if (who > 0)
                t = (heroes - 1)[who];
            else if (kind == 0x1f || kind == 0x1e)
                for (i = 1; i <= rec_count; i++)
                    if ((recs - 1)[i].x == x && (recs - 1)[i].y == y)
                        t = (recs - 1)[i].hero;
        if (t != 0) {
            panel_dirty = 1;
            hero_panel(t);
        } else
            text_clear(0x17, 1, 0x26, 0x15);
        if (range < dist || (tile_info - 1)[kind].move_cost == 0xff)
            *active = 0;
        if (t != 0) {
            if (!combat_target_allowed(h, t, 1) || !pair_ok)
                *active = 0;
            if (attack == 1)
                if (h == t || who == 0 && kind == 0x1f)
                    *active = 0;
                else if (has_ranged_weapon(h) && (dist != 1 || !has_throwable_weapon(h))
                         && (!ranged_ammo_find(h, &sel)
                             || combat_targets_near(h, 1, 0) > 0 && !has_throwable_weapon(h)))
                    *active = 0;
        } else if (!mode)
            *active = 0;
        mem_fill(hotkeys, sizeof hotkeys, 0xff);
        count = 0;
        if (*active)
            hotkey_reset(2);
        else {
            hotkey_add(1, &count);
            hotkey_add(2, &count);
        }
        set_menu_kind(5);
        moved_by_click = 0;
        click_accept = 0;
        if (next_key == 0)
            key = grid_pick(blank_text, target_items, 1, 0);
        else {
            key = next_key;
            next_key = 0;
        }
        if (!moved_by_click) {
            if (click_accept || menu_escaped && key == 0xd) {
                menu_escaped = 0;
                key = 0;
                click_accept = 0;
            }
            if (menu_escaped)
                switch (key) {
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
                case 0x80:
                    combat_view_scroll(x, y, 0, 8);
                    dir = 8;
                    break;
                case 0x1b:
                    cell_redraw(x, y);
                    mem_fill(aim, sizeof(struct Aim), 0);
                    *accepted = 0;
                    key = 2;
                    break;
                default:
                    dir = 8;
                }
            else
                switch (key) {
                case 0:
                    *accepted = 1;
                    aim->x = x;
                    aim->y = y;
                    if (t != 0)
                        aim->target = t;
                    else
                        aim->target = 0;
                    cur_map->cursor_on = 0;
                    cell_redraw(x, y);
                    turn_done = 0;
                    break;
                case 1:
                    combat_view_scroll(x, y, 0, 8);
                    dir = 8;
                    break;
                case 2:
                    cell_redraw(x, y);
                    mem_fill(aim, sizeof(struct Aim), 0);
                    *accepted = 0;
                    break;
                default:
                    dir = 8;
                }
        }
    }
}

struct Hero far *far step_target(char delta, struct Hero far *h) /* 2dbf:32bc */
{
    struct Hero far *t;
    struct Hero far *next;
    char found;

    saved_aim = (heroes - 1)[reachable_target_list[cur_pick].who];
    if (saved_aim == 0)
        saved_aim = h;
    if (pick_cnt < 1)
        game_exit();
    fly_last_x = (combatants - 1)[reachable_target_list[cur_pick].who].x;
    fly_last_y = (combatants - 1)[reachable_target_list[cur_pick].who].y;
    cur_pick += delta;
    if (cur_pick == 0)
        cur_pick = pick_cnt;
    if (cur_pick > pick_cnt)
        cur_pick = 1;
    t = (heroes - 1)[reachable_target_list[cur_pick].who];
    if (delta != 0 && t->side != 1) {
        found = 0;
        for (tick_slot = 0; tick_slot < pick_cnt && !found; tick_slot++)
            if (t->side != 0)
                found = 1;
            else {
                cur_pick += delta;
                if (cur_pick == 0)
                    cur_pick = pick_cnt;
                if (cur_pick > pick_cnt)
                    cur_pick = 1;
                t = (heroes - 1)[reachable_target_list[cur_pick].who];
            }
    }
    next = (heroes - 1)[reachable_target_list[cur_pick].who];
    fly_next_x = (combatants - 1)[reachable_target_list[cur_pick].who].x;
    fly_next_y = (combatants - 1)[reachable_target_list[cur_pick].who].y;
    if (next == 0 || fly_next_x < 0 || fly_next_y < 0) {
        t = saved_aim;
        fly_next_x = fly_last_x;
        fly_next_y = fly_last_y;
    } else {
        t = next;
        icon_fly(fly_last_x, fly_last_y, fly_next_x, fly_next_y, 1, 0);
        fly_last_x = fly_next_x;
        fly_last_y = fly_next_y;
    }
    return t;
}

void far build_picks(struct Hero far *h)                         /* 2dbf:351d */
{
    unsigned char i;
    struct Hero far *c;

    cur_pick = 1;
    plan_steps(combatant_x(h), combatant_y(h), 0xff, 0xff, combatant_size(h));
    pick_cnt = 0;
    for (i = 1; i <= step_count; i++) {
        c = (heroes - 1)[(steps - 1)[i].who];
        if (combat_target_allowed(h, c, 0)) {
            pick_cnt++;
            reachable_target_list[pick_cnt] = (steps - 1)[i];
            if (h->fight->target == c)
                cur_pick = pick_cnt;
        }
    }
    target_hero = step_target(0, h);
    if (target_hero != h)
        ui_mode = 4;
}

void far combat_aim(struct Hero far *h, char range, char gate, char flag, char mode,
                         char far *result, struct Aim far *aim)  /* 2dbf:3634 */
{
    char active;
    char accepted;
    char cur_x;
    char cur_y;
    char org_x;
    char org_y;
    char stop;

    ui_mode = 1;
    next_key = 0;
    icon_facing(0x1e, 0, 0, 0);
    mem_fill(aim, sizeof(struct Aim), 0);
    *result = 0;
    reach = range;
    if (reach == 0xff)
        if (h->equipped[0] != 0)
            reach = item_table[h->equipped[0]->type].range;
        else
            reach = 1;
    if (reach == 0 || reach == 0xff)
        reach = 1;
    org_x = combatant_x(h);
    org_y = combatant_y(h);
    cell_redraw(org_x, org_y);
    build_picks(h);
    cur_x = combatant_x(target_hero);
    cur_y = combatant_y(target_hero);
    active = 1;
    target_step_direction = 0;
    stop = 0;
    while (!stop) {
        target_menu_choice = target_menu(h, target_hero, reach, gate, flag, mode, active, cur_x,
                                         cur_y);
        if (menu_escaped)
            if (target_menu_choice == 0x1b) {
                target_menu_choice = 5;
                menu_escaped = 0;
            } else if (target_menu_choice == 0x80) {
                target_menu_choice = 4;
                menu_escaped = 0;
            }
        if (!menu_escaped)
            switch (target_menu_choice) {
            case 0:
                active = 1;
                target_hero = step_target(1, h);
                cur_x = combatant_x(target_hero);
                cur_y = combatant_y(target_hero);
                break;
            case 1:
                active = 1;
                target_hero = step_target(-1, h);
                cur_x = combatant_x(target_hero);
                cur_y = combatant_y(target_hero);
                break;
            case 2:
                aim_menu(h, target_hero, reach, gate, flag, mode, &active, &accepted, aim,
                         cur_x, cur_y);
                if (accepted) {
                    target_hero = aim->target;
                    cur_x = aim->x;
                    cur_y = aim->y;
                } else
                    active = 1;
                if (active && accepted) {
                    target_menu_choice = 3;
                    stop = 1;
                    ui_mode = 4;
                    if (target_hero != 0)
                        aim->target = target_hero;
                    else
                        aim->target = 0;
                    *result = 1;
                    if (gate == 1)
                        aim_at(h, target_hero, gate, result, aim);
                    if (!*result) {
                        cur_map->cursor_on = 0;
                        combat_view_scroll(cur_x, cur_y, 0, 8);
                    }
                } else if (h != 0 && !accepted) {
                    panel_dirty = 1;
                    hero_panel(h);
                }
                ui_mode = 3;                /* after the 4 set on acceptance, as shipped */
                break;
            case 3:
                aim->x = cur_x;
                aim->y = cur_y;
                if (target_hero != 0)
                    aim->target = target_hero;
                else
                    aim->target = 0;
                *result = 1;
                if (gate == 1)
                    aim_at(h, target_hero, gate, result, aim);
                if (!*result) {
                    cur_map->cursor_on = 0;
                    combat_view_scroll(cur_x, cur_y, 0, 8);
                }
                stop = 1;
                break;
            case 4:
                cur_map->cursor_on = 1;
                cur_map->cursor_size = 1;
                combat_view_scroll(cur_x, cur_y, 0, 8);
                cur_map->cursor_on = 0;
                target_step_direction = 0;
                break;
            case 5:
                stop = 1;
                break;
            }
        else if (target_menu_choice == 0x88 || target_menu_choice == 0x84
                 || target_menu_choice == 0x82 || target_menu_choice == 0x86
                 || target_menu_choice == 0x87 || target_menu_choice == 0x85
                 || target_menu_choice == 0x81 || target_menu_choice == 0x83) {
            next_key = target_menu_choice;
            aim_menu(h, target_hero, reach, gate, flag, mode, &active, &accepted, aim, cur_x,
                     cur_y);
            if (accepted) {
                target_hero = aim->target;
                cur_x = aim->x;
                cur_y = aim->y;
            } else
                active = 1;
            if (active && accepted) {
                target_menu_choice = 3;
                stop = 1;
                ui_mode = 4;
                if (target_hero != 0)
                    aim->target = target_hero;
                else
                    aim->target = 0;
                *result = 1;
                if (gate == 1)
                    aim_at(h, target_hero, gate, result, aim);
                if (!*result) {
                    cur_map->cursor_on = 0;
                    combat_view_scroll(cur_x, cur_y, 0, 8);
                }
            } else
                ui_mode = 3;
        }
        cell_redraw(combatant_x(target_hero), combatant_y(target_hero));
        target_hero = step_target(target_step_direction, h);
        if (target_hero != 0
            && (target_menu_choice == 0 || target_menu_choice == 1 || target_menu_choice == 3)) {
            cur_x = combatant_x(target_hero);
            cur_y = combatant_y(target_hero);
        }
    }
    if (gate)
        box_draw_top();
}

char far combat_target_pick(struct Hero far *h, char range, char any, char fresh,
                       char flag)                                /* 2dbf:3bf5 */
{
    char r;
    unsigned char count;
    unsigned char tries_left;
    unsigned char pick;
    char last;
    char found;
    char again;
    struct Hero far *t;

    again = 0;
    found = 0;
    last = 0;
    t = h->fight->target;
    if (fresh || t != 0 && (t->side == h->side || !t->active || !combat_target_allowed(h, t, flag)))
        h->fight->target = 0;
    if (h->fight->target != 0)
        found = 1;
    while (!found && !last) {
        last = again;
        if (again && !fresh)
            cur_map->ignore_blocking = 1;
        count = combat_targets_near(h, range, flag);
        if (count > 0)
            pick = fx_roll_dice(1, count);
        tries_left = count;
        while (tries_left > 0 && !found && count > 0) {
            tries_left--;
            if ((near_ids - 1)[pick] > 0) {
                t = (heroes - 1)[(near_ids - 1)[pick]];
                if (any && cur_map->ignore_blocking || combat_target_allowed(h, t, flag)) {
                    found = 1;
                    h->fight->target = t;
                } else {
                    (near_ids - 1)[pick] = 0;
                    for (pick = 1; pick <= count; pick++)
                        if ((near_ids - 1)[pick] > 0)
                            found = 1;
                    if (found)
                        found = 0;
                    else
                        count = 0;
                }
            }
            if (!found) {
                pick++;
                if (pick > count)
                    pick = 1;
            }
        }
        if (!again)
            again = 1;
    }
    cur_map->ignore_blocking = 0;
    r = found;
    return r;
}

char far gods_intervene(char skip_check)                        /* 2dbf:3de3 */
{
    struct Hero far *h;
    struct Effect far *fx;

    if (!skip_check && str_cmp(msg_text(3), debug_word) != 0)
        return 0;
    bottom_msg("The Gods intervene!");
    for (h = node_list; h != 0; h = h->next) {
        fx = 0;
        fx_remove(h, 0xb, fx);
        fx = 0;
        fx_remove(h, 0x4d, fx);
        if (h->side == 1) {
            h->active = 0;
            h->status = 6;
            (combatants - 1)[combatant_slot(h)].size = 0;
        }
        combat_actions_end(h);
    }
    combat_view_scroll(cur_map->view_x + 3, cur_map->view_y + 3, 0xff, 8);
    return 1;
}

void far cursor_step(int dir)                                    /* 2dbf:3eea */
{
    combat_view_scroll(target_x, target_y, 3, dir);
    target_x += dir_dx[dir];
    target_y += dir_dy[dir];
}

void far view_click(int zone, int ty, int tx)                    /* 2dbf:3f19 */
{
    short x;
    short dir;
    register int y;
    register int i;

    y = target_y - cur_map->view_y;
    x = target_x - cur_map->view_x;
    if (zone == 5 && ty == y && tx == x) {
        click_accept = 1;
        return;
    }
    switch (zone) {
    case 1:
        for (i = x; i >= 0; i--) {
            combat_view_scroll(target_x, target_y, 3, 6);
            target_x += dir_dx[6];
            target_y += dir_dy[6];
        }
        break;
    case 2:
        for (i = y; i >= 0; i--) {
            combat_view_scroll(target_x, target_y, 3, 0);
            target_x += dir_dx[0];
            target_y += dir_dy[0];
        }
        break;
    case 3:
        for (i = x; i <= tx; i++) {
            combat_view_scroll(target_x, target_y, 3, 2);
            target_x += dir_dx[2];
            target_y += dir_dy[2];
        }
        break;
    case 4:
        for (i = y; i <= ty; i++) {
            combat_view_scroll(target_x, target_y, 3, 4);
            target_x += dir_dx[4];
            target_y += dir_dy[4];
        }
        break;
    case 5:
        while (ty != y || tx != x) {
            if (x < tx) {
                x++;
                if (y < ty) {
                    y++;
                    dir = 3;
                } else if (y > ty) {
                    y--;
                    dir = 1;
                } else
                    dir = 2;
            } else if (x > tx) {
                x--;
                if (y < ty) {
                    y++;
                    dir = 5;
                } else if (y > ty) {
                    y--;
                    dir = 7;
                } else
                    dir = 6;
            } else if (y < ty) {
                y++;
                dir = 4;
            } else if (y > ty) {
                y--;
                dir = 0;
            }
            cursor_step(dir);
        }
        break;
    }
    if (target_x < 0)
        target_x = 0;
    if (target_y < 0)
        target_y = 0;
    if (target_x > 0x31)
        target_x = 0x31;
    if (target_y > 0x18)
        target_y = 0x18;
    moved_by_click = 1;
}

static char rank_target(struct Hero far *t, unsigned char spell, int side) /* 2dbf:410f */
{
    char rank;
    struct Effect far *effect;
    unsigned char level;
    unsigned char blocker;

    if (t->side != side)
        return -40;
    rank = 0;
    blocker = spell_info[spell].effect;
    switch (spell) {
    case 1:
        if (!find_effect(t, 1, &effect) && combat_targets_near(t, 1, 0) == 0)
            rank = 4;
        break;
    case 0x2a:
        if (!find_effect(cur_node, 0x31, &effect))
            rank = 4;
        break;
    case 3:
    case 0x24:
    case 0x3a:
    case 0x47:
        if ((char)(t->hp_max > t->hp))
            rank = 4;
        break;
    case 0x60:
        if ((char)(t->race < 6 || t->type_flags & 2))
            rank = 4;
        break;
    case 0xa:
        if (t->immunity & 2 && (char)(t->icon_dim == 1))
            rank = 4;
        break;
    case 0xf:
    case 0x86:
        if (!find_effect(t, 0x11, &effect))
            rank = 4;
        break;
    case 0x15:
        if ((char)(t->hit_dice < 5) && !find_effect(t, 0x8d, &effect)
            && !find_effect(t, 0x35, &effect))
            rank = 4;
        break;
    case 0x17:
    case 0x31:
        if ((char)(t->race < 6) && t->immunity & 2 && !find_effect(t, 0x34, &effect))
            rank = 4;
        break;
    case 0x25:
        if (find_effect(t, 0x21, &effect))
            rank = 4;
        break;
    case 0x2b:
        if (find_effect(t, 0x24, &effect))
            rank = 4;
        break;
    case 0x2f:
    case 0x4a:
    case 0x73:
    case 0x85:
        if (!find_effect(t, 0x5d, &effect))
            rank = 4;
        break;
    case 0x33:
        if (!find_effect(t, 6, &effect))
            rank = 4;
        break;
    case 0x4c:
    case 0x6e:
        if (!(t->immunity & 8))
            rank = 4;
        break;
    case 0x44:
        if (!(t->immunity & 0x10) && !find_effect(t, 0x8f, &effect))
            rank = 4;
        break;
    case 0x5e:
        if ((t->immunity & 2) == 0)
            break;
        rank = 4;
        break;
    case 0x82:
        rank = 4;
        break;
    case 0x51:
    case 0x78:
        if (t->immunity & 2)
            rank = 4;
        break;
    case 0x54:
        if (!find_effect(t, 0x5c, &effect))
            rank = 4;
        break;
    case 0x5b:
        if (t->hit_dice < 6)
            rank = 4;
        break;
    default:
        if (blocker == 0 || (char)!find_effect(t, blocker, &effect))
            rank = 4;
    }
    level = spell_info[spell].level;
    if (rank > 0) {
        if (level < 5 && find_effect(t, 0x39, &effect))
            rank = 0;
        if (level < 4 && find_effect(t, 0x3f, &effect))
            rank = 0;
        if (t->magic_resist > 0x28)
            if (t->magic_resist > 0x82)
                rank = 0;
            else if (t->magic_resist > 0x64)
                rank = 1;
            else if (t->magic_resist > 0x46)
                rank = 2;
            else
                rank = 3;
    }
    return rank;
}

char far spell_target_rating(struct Hero far *h, unsigned char spell)  /* 2dbf:4530 */
{
    char r;
    char best;
    short side;
    unsigned char i;
    unsigned char count;
    unsigned char range;

    r = 0;
    if (spell_info[spell].hostile == 0)
        side = h->side;
    else
        side = enemy_team(h);
    if (spell_info[spell].targeting == 0)
        best = rank_target(h, spell, side);
    else {
        range = spell_range(spell);
        plan_steps(combatant_x(h), combatant_y(h), range, 0xff, combatant_size(h));
        count = step_count;
        i = 1;
        best = 0;
        if (count > 0)
            while (i <= count && best <= 0) {
                best = rank_target((heroes - 1)[(steps - 1)[i].who], spell, side);
                i++;
            }
    }
    r = best;
    return r;
}

char far area_aim(unsigned char spell, struct Aim far *aim)      /* 2dbf:4653 */
{
    char r;
    struct Hero far *cur;
    char cx;
    char cy;
    short outer;
    short inner;
    short base_x;
    short base_y;
    short best_dx;
    short best_dy;
    short my_x;
    short my_y;
    short dx;
    short min_x;
    short max_x;
    short min_dy;
    short max_dy;
    short score;
    short best;
    unsigned short cost;
    struct MapBox obox;
    struct MapBox ibox;
    short side;
    register int dy;
    register int radius;

    if (spell_info[spell].hostile == 0)
        side = cur_node->side;
    else
        side = enemy_team(cur_node);
    r = 0;
    if (spell == 0x57 && cursor_obj[0x3c])
        radius = 3;
    else if (spell == 0x33 && cursor_obj[0x3c])
        radius = 2;
    else
        radius = spell_info[spell].targeting - 8;
    if ((spell == 0x2f || spell == 0x73) && !cursor_obj[0x22])
        radius--;
    cur = 0;
    outer = spell_range(spell) + radius;
    if (outer > 0x12) {
        outer = 0x12;
        cur = cur_node->fight->target;
    }
    inner = outer - radius;
    if (cur == 0)
        cur = cur_node;
    base_x = combatant_x(cur);
    base_y = combatant_y(cur);
    if (spell == 0x22 || spell == 0x65) {
        obox.x0 = base_x - inner;
        obox.y0 = base_y - inner;
    } else {
        obox.x0 = base_x - outer;
        obox.y0 = base_y - outer;
    }
    ibox.x0 = base_x - inner;
    ibox.y0 = base_y - inner;
    obox.x1 = base_x + outer;
    obox.y1 = base_y + outer;
    ibox.x1 = base_x + inner;
    ibox.y1 = base_y + inner;
    mem_zero(target_scores, 0xab2);
    for (cur = node_list; cur != 0; cur = cur->next) {
        cx = combatant_x(cur);
        cy = combatant_y(cur);
        if (cur->active && cx >= obox.x0 && cx <= obox.x1 && cy >= obox.y0 && cy <= obox.y1) {
            score = rank_target(cur, spell, side);
            if (spell == 0x22 || spell == 0x65) {
                min_dy = -radius;
                max_dy = 0;
            } else {
                min_dy = -radius;
                max_dy = radius;
            }
            if (cy + min_dy < ibox.y0)
                min_dy = ibox.y0 - cy;
            if (cy + max_dy > ibox.y1)
                max_dy = ibox.y1 - cy;
            for (dy = min_dy; dy <= max_dy; dy++) {
                if (spell == 0x22 || spell == 0x65) {
                    min_x = cx - area_widths[dy + 3][radius - 1];
                    max_x = cx;
                } else {
                    min_x = cx - area_widths[dy + 3][radius - 1];
                    max_x = cx + area_widths[dy + 3][radius - 1];
                }
                if (ibox.x0 > min_x)
                    min_x = ibox.x0;
                if (ibox.x1 < max_x)
                    max_x = ibox.x1;
                for (dx = min_x; dx <= max_x; dx++)
                    target_scores[dx - base_x + 18][cy + dy - base_y + 18] += score;
            }
        }
    }
    my_x = combatant_x(cur_node);
    my_y = combatant_y(cur_node);
    best = 0;
    for (dx = -inner; dx <= inner; dx++)
        for (dy = -inner; dy <= inner; dy++) {
            score = target_scores[dx + 18][dy + 18];
            if (score > best || score == best && score > 0 && chance(0x19)) {
                cx = my_x;
                cy = my_y;
                cost = spell_range(spell);
                if (base_x + dx >= 0 && base_x + dx <= 0x31 && base_y + dy >= 0
                    && base_y + dy < 0x18
                    && line_clear(base_x + dx, base_y + dy, &cx, &cy, &cost)) {
                    best_dx = dx;
                    best_dy = dy;
                    best = score;
                }
            }
        }
    aim->target = 0;
    if (best > 0) {
        r = 1;
        aim->x = best_dx + base_x;
        aim->y = best_dy + base_y;
    } else {
        r = 0;
        aim->target = 0;
    }
    return r;
}

char far can_lead(struct Hero far *h)                            /* 2dbf:4b30 */
{
    char r;
    unsigned char bonus;
    unsigned char odds;

    switch (class_level(h, 3)) {
    case 0:
        odds = 0;
        break;
    case 1:
        odds = 0x14;
        break;
    case 2:
        odds = 0x28;
        break;
    default:
        odds = 0x50;
    }
    if (h->cha > 0xd)
        bonus = (h->cha - 0xd) * 5;
    else
        bonus = 0;
    if (odds + bonus > fx_roll_dice(1, 100))
        r = 1;
    else
        r = 0;
    return r;
}

void far combat_leadership_apply(void)                                   /* 2dbf:4bbf */
{
    struct Hero far *leader;
    struct Hero far *h;

    for (h = node_list; h != 0; h = h->next)
        if (h->side == 0)
            h->quick_fight = h->control & 0x80;
        else
            h->quick_fight = 1;
    for (leader = node_list; leader != 0; leader = leader->next)
        if (leader->side == 0 && !(leader->control & 0x80) && class_level(leader, 3) > 0
            && leader->status == 0)
            for (h = node_list; h != 0; h = h->next)
                if (h->side == 0 && h->control & 0x80 && h->quick_fight && h->status == 0
                    && h->intel > 3 && can_lead(leader))
                    h->quick_fight = 0;
}

void far fly_icon(unsigned char cell, struct Hero far *t, struct Hero far *h) /* 2dbf:4cc8 */
{
    icon_facings(cell);
    icon_fly(combatant_x(h), combatant_y(h), combatant_x(t), combatant_y(t), 1,
             0x1e);
}

void far sight_line(char fresh, unsigned far *dist, struct Hero far *h,
                    char far *blocked)                           /* 2dbf:4d1f */
{
    char tx;
    char ty;

    *blocked = 1;
    if (combat_target_pick(h, 0xff, 0, fresh, 0)) {
        tx = combatant_x(h->fight->target);
        ty = combatant_y(h->fight->target);
        if (line_clear(combatant_x(h), combatant_y(h), &tx, &ty, dist))
            *blocked = 0;
    }
}

void far eye_rays(struct Hero far *h, struct Effect far *fx, char removed) /* 2dbf:4dbe */
{                                       /* reads neither fx nor removed */
    unsigned char left;
    unsigned short dist;
    unsigned char used;
    char blocked;
    struct Hero far *t;

    used = 0;
    left = 4;
    blocked = 0;
    h->fight->target = 0;
    sight_line(1, &dist, h, &blocked);
    do {
        element_mask = 8;
        t = h->fight->target;
        dist = get_distance(h, t);
        left--;
        if (t != 0)
            if (dist <= 2 && !(used & 1)) {
                used = used | 1;
                hero_text(h, "fires a disintegrate ray", 10, 1);
                fly_icon(0x13, t, h);
                if (!fx_saving_throw(t, 3, 0))
                    fx_disable(t, 8, "is disintegrated");
                sight_line(0, &dist, h, &blocked);
            } else if (dist <= 3 && !(used & 2)) {
                used = used | 2;
                hero_text(h, "fires a flesh to stone ray", 10, 1);
                fly_icon(0x13, t, h);
                if (!fx_saving_throw(t, 1, 0))
                    fx_disable(t, 7, "is Stoned");
                sight_line(0, &dist, h, &blocked);
            } else if (dist <= 4 && !(used & 4)) {
                element_mask = 0x48;
                used = used | 4;
                hero_text(h, "fires a death ray", 10, 1);
                fly_icon(0x13, t, h);
                if (!fx_saving_throw(t, 0, 0))
                    fx_disable(t, 6, "is killed");
                sight_line(0, &dist, h, &blocked);
            } else if (dist <= 5 && !(used & 8)) {
                used = used | 8;
                hero_text(h, "Causes Serious Wounds", 10, 1);
                fly_icon(0x13, t, h);
                current_spell = 0x42;
                fx_damage(t, fx_roll_damage(2, 8) + 1, 0, 0);
                sight_line(0, &dist, h, &blocked);
            } else if (!(used & 0x10)) {
                current_spell = 0x54;
                spell_cast(0x54, 1, 1, &blocked);
                used = used | 0x10;
            } else if (!(used & 0x20)) {
                current_spell = 0x37;
                spell_cast(0x37, 1, 1, &blocked);
                used = used | 0x20;
            } else if (!(used & 0x40)) {
                current_spell = 0x15;
                spell_cast(0x15, 1, 1, &blocked);
                used = used | 0x40;
            }
    } while (left > 0 && h->fight->target != 0);
}

char far combat_sweep_attack(struct Hero far *h, struct Hero far *t) /* 2dbf:50d1 */
{
    char r;
    unsigned char count;
    unsigned char left;
    unsigned char i;
    unsigned char at;
    struct Hero far *c;

    r = 0;
    if (h->attacks[0] < h->fight->sweep_targets && t->hit_dice == 0 && get_distance(h, t) == 1) {
        count = enemies_near(h, 1);
        left = 0;
        for (i = 1; i <= count; i++) {
            c = (heroes - 1)[(near_ids - 1)[i]];
            if (c == t)
                at = i;
            if (c->hit_dice == 0)
                left++;
        }
        if (h->attacks[0] < left) {
            if (h->fight->sweep_targets < left)
                left = h->fight->sweep_targets;
            hero_text(h, "sweeps", 10, 1);
            c = (heroes - 1)[(near_ids - 1)[1]];
            if (c != t) {
                (near_ids - 1)[at] = (near_ids - 1)[1];
                (near_ids - 1)[1] = combatant_slot(t);
            }
            for (i = 1; i <= count; i++)
                if (left > 0 && (heroes - 1)[(near_ids - 1)[i]]->hit_dice == 0) {
                    c = (heroes - 1)[(near_ids - 1)[i]];
                    attack_register(h, c);
                    h->attacks[0] = 1;
                    combat_attack(h, c, 0, 0L, &combat_action_done);
                    left--;
                }
            r = 1;
        }
    }
    return r;
}

unsigned char far enemies_near(struct Hero far *h, unsigned char range) /* 2dbf:52a2 */
{
    unsigned char n;
    unsigned char count;
    unsigned char matching;
    unsigned char i;

    plan_steps(combatant_x(h), combatant_y(h), range, 0xff, combatant_size(h));
    count = step_count;
    matching = 0;
    if (count > 0) {
        for (i = 1; i <= count; i++)
            if (enemy_team(h) == (heroes - 1)[(steps - 1)[i].who]->side) {
                matching++;
                mem_move5(&(steps - 1)[i], &(steps - 1)[matching], 3);
            }
        count = matching;
        step_count = matching;
    }
    n = count;
    for (i = 1; i <= count; i++)
        (near_ids - 1)[i] = (steps - 1)[i].who;
    return n;
}
