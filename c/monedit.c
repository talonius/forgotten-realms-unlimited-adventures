/* CKIT.EXE overlay section 25, module 2e62: the design editor's monsters.
 * monster_screen is screen 7 of editloop.c's loop (stub 18de:1c33): coming
 * from screen 1 it sends the editor to artpick.c's art screen (8) to pick a
 * monster (kind 3); back from it with a pick (env->bits: mask << 8 | number,
 * mask bit 7 set) it loads that monster and runs evedit.c's event_edit on
 * the record (event kind 0x36, or 0x39 for numbers from 0x70, which first
 * offers "Re-create NPC?": create.c's hero_create on the record, for a
 * human "Try to change class?" through train.c's change_class, the
 * experience typed in, and the training loop; then the record's effects'
 * strengths go to its tail and its effect and item lists are freed); Ok
 * saves it.  It routes the editor by event_edit's link->next: back to the
 * art screen, to screens 10 and 11 with link->code, or to the event screen
 * (5) with link->code's low byte as the event and its high byte, an offset
 * into the area record from enc[0].kind, turned into a table (2, 3 or 4)
 * and an index.  monster_pick is screen 21 (stub 18de:1c79): coming from
 * screens 10 and 11 it sends the editor to the art screen (with sub, the
 * label filter, from monst_listed) and takes the pick back to screen 11; a
 * return from the art screen without a pending pick loads the monster
 * picked there, when there is one, and saves it under the current number.
 * monst_load reads MONST<n> (overlay 3's data_load) into the buffer at
 * cursor_obj + 0x65, unpacking it when it is shorter than 0x1c2 bytes, and
 * returns it; monst_save writes it back and updates the monster's label
 * (its name, and bit 0x40 of its bits); monst_default deletes the design's
 * loose MONST%03d.dat (DOS function 41h) and, when that succeeds, reloads
 * the record and copies its name out; monst_cur returns the record being edited; monst_listed and
 * monst_unlisted, the art screen's label filters (stubs 18de:187d and
 * 18de:1f0d), test a number against evedit.c's large_monsters; xp_input reads
 * "Experience: " as a signed decimal long (parse_long).
 *
 * Section 25 (parent section 20, which is empty and has no parent; no
 * section names 25 as its parent): payload at EXE 0x07e7c0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 25.  The module is stored frame 2e62 (load-based 3e62),
 * EXE 0x07f4d0..0x08002c (2,908 bytes, 10 routines), 3e62:0000..0b5b: from
 * the byte after prnout.c's last retf (2d91:0d0f) to the retf before
 * module 2f17, which starts at 2f17:000c (vector 18de:1c47).  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.
 *
 * Section 25's relocation table files 56 sites under 2e62, all in this
 * range (EXE 0x07f550..0x08001e), all far calls: 27 to vector stubs, 26
 * to the resident code, and three to this module's own monst_load,
 * monst_save and xp_input, from monster_screen ahead of their
 * definitions, so those three are public.  No other relocated word in
 * section 25 stores 2e62.  Eight of the section's thirteen vectors land
 * here: 18de:1c33 -> monster_screen and 18de:1c79 -> monster_pick
 * (editloop.c's screens 7 and 21), 18de:1f35 -> monst_load, 18de:1f2b ->
 * monst_save and 18de:1f21 -> monst_default (all three artpick.c's), 18de:187d ->
 * monst_listed (labels.c, artpick.c), 18de:1f0d -> monst_unlisted
 * (artpick.c) and 18de:19f9 -> xp_input (create.c).  parse_long is reached
 * only by xp_input's near call after its definition, and nothing in the
 * image reaches monst_cur: for those two the bytes do not decide static or
 * public.
 *
 * One object: every DGROUP address the code names past the shared ones is
 * in one _DATA run, 929c..92fa, in first-use order, and one _BSS variable,
 * mon_arg, which monster_screen, monster_pick, monst_load, monst_default
 * and monst_cur all use; near calls tie monst_load to monster_pick.  The
 * bytes alone would allow a split at an even _DATA address (92c0 before
 * monst_load's literal, 92ee before xp_input's), as prnout.c shares frame
 * 2d91 with mapprint.c; nothing here asks for one.
 *
 * FRUA's own code: the sharing map pairs all 10 routines a-only, so every
 * name here is chosen from the code.  The stubs keep the names landed
 * files give them.
 *
 * _DATA is DGROUP 929c..92fa, seven literals stored as six ("MONST" is
 * used twice), after mapprint.c's (9242..929b); 92fb is a zero
 * no instruction names, and section 26's code names 92fc..92fe.  _BSS is
 * mon_arg, the far pointer at d580..d583, after mapprint.c's links
 * (d57c..d57f) and before sysutil.c's _BSS (d584).  No instruction outside
 * this range names 929c..92fb or d580..d583.
 *
 * Forced by the bytes, each against its alternative: monster_pick's
 * "goto pick" into screen 10's else arm (the statements written out again
 * in case 8: 6 of 10 ranges); "*s++" in parse_long's loop against the
 * for-header form (s++ in a for header: 7 of 10; "++s" there with the for,
 * or "n = n * 10 - (*s - '0'), s++;", also bakes 10 of 10); parse_long's neg declared before n (9 of 10);
 * monst_load's short v (int: 0 of 10) and unsigned len (int: 9 of 10) and
 * "p = ..." inside movmem's arguments (a statement of its own: 0 of 10);
 * monst_save's short len (int: 1 of 10) and bits before nbits (9 of 10);
 * monst_default's locals ok, path, name (reversed: 9 of 10); monster_pick's
 * int code (short: 6 of 10); "!a->picking" (?: 0 of 10); "i / sizeof
 * (struct Encounter)" (i / 4: 0 of 10; (unsigned)i >> 2 bakes 10 of 10);
 * "((env->bits & 0xff00) >> 8) & 0x80" (env->bits & 0x8000: 0 of 10);
 * EdEnv.bits a signed long (LXRSH@; unsigned calls LXURSH@); MonArg.num
 * unsigned char (char: 2 of 10); the locals' declaration orders.  Not
 * decided, each 10 of 10: mode and rc int or short; "mon_arg = a = arg"
 * or two statements; "confirm(...) && hero_create(...)" or nested ifs;
 * "effects[i++] =" or two statements; i's two statements or one;
 * monst_listed and monst_unlisted char or int; register or plain prev and
 * i; parse_long's n initialized in its declaration.
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   signed: LXRSH@                    */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct EdLink {                 /* evedit.c's, event_edit's caller state  */
    int kind;                   /* +0                                     */
    int page;                   /* +2                                     */
    int field;                  /* +4                                     */
    int back;                   /* +6                                     */
    int ret;                    /* +8                                     */
    int next;                   /* +a                                     */
    int code;                   /* +c                                     */
};

struct ArtSel {                 /* artpick.c's, editloop.c's a93          */
    int next;                   /* +0                                     */
    int scr;                    /* +2   not read here                     */
    int done;                   /* +4                                     */
    char pad_6[3];              /* +6   not read here                     */
    unsigned char sub;          /* +9                                     */
};

struct Effect {                 /* a effect_pool record, 10 bytes; saveload.c
                                   reads the tail back into +0            */
    char pad_0[4];              /* +0   not read here                     */
    unsigned char strength;     /* +4                                     */
    char pad_5;                 /* +5   not read here                     */
    struct Effect far *next;    /* +6                                     */
};

struct Item {                   /* a item_record_pool record, 0x3e bytes         */
    struct Item far *next;      /* +0                                     */
};

struct Hero {                   /* a party member, 0x18e bytes            */
    struct Hero far *next;      /* +0   not read here                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    char pad_c[0x38];           /* +c   not read here                     */
    long xp;                    /* +44                                    */
    char pad_48[0x10];          /* +48  not read here                     */
    unsigned char race;         /* +58  5 human                           */
    char pad_59[7];             /* +59  not read here                     */
    char name[0x11];            /* +60                                    */
    char pad_71[0x10];          /* +71  not read here                     */
    unsigned char hp_max;       /* +81                                    */
    char pad_82[0x11];          /* +82  not read here                     */
    unsigned char control;      /* +93                                    */
    char pad_94[0xf7];          /* +94  not read here                     */
    unsigned char hp;           /* +18b                                   */
    char pad_18c;               /* +18c not read here                     */
    unsigned char mon_id;       /* +18d                                   */
};

struct Monster {                /* a MONST record; the module moves 0x1c2
                                   bytes (event_edit's size)              */
    struct Hero hero;           /* +0                                     */
    unsigned char items[16];    /* +18e not read here                     */
    unsigned char charge_ids[16]; /* +19e not read here                   */
    unsigned char effects[10];  /* +1ae                                   */
};

struct MonArg {                 /* editloop.c's a7c, 0xd bytes            */
    int prev;                   /* +0   where a cancelled pick goes: the
                                        screen before screen 7 (1)        */
    char picking;               /* +2   sent to the art screen to pick    */
    unsigned char num;          /* +3   the monster                       */
    unsigned char mask;         /* +4   the pick's mask, less bit 7       */
    void far *buf;              /* +5   ed->buf, an EdLink, on screen 7;
                                        ed->a93, an ArtSel, on screen 21  */
    struct Monster far *rec;    /* +9   monst_load's buffer               */
};

struct Encounter {              /* mapevent.c's, 4 bytes                  */
    unsigned ticks;             /* +0   not read here                     */
    unsigned char event;        /* +2                                     */
    unsigned char chance;       /* +3   not read here                     */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    char pad_0[0x2e];           /* +0   not read here                     */
    struct Encounter rest_enc[8]; /* +2e                                    */
};

struct Geo {                    /* view3d.c's geo record                  */
    char pad_0[8];              /* +0   not read here                     */
    struct Area far *area;      /* +8   d41c                              */
    char pad_c[2];              /* +c   not read here                     */
};

struct Pool;                    /* strutil.c's; only its address is passed */

extern char far *far packbits_expand(char far *in, char far *out, int n); /* 00d7:0a1c */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far dos_open(int ax, char far *name, ...);       /* 00d7:26bc */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern unsigned far mem_scan(void far *p, int n, char c);    /* 00d7:27b1 */
extern char far *far input_text(char far *prompt, unsigned char fg, unsigned char bg,
                                unsigned char max);              /* 06c9:05bc */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far set_extra_row(char c);                           /* 083a:23ae */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool); /* 0a75:02c8 */
extern void far game_exit(void);                                 /* 0d48:002f */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far mode_change(int n);
                                /* 18de:10cb, vector to overlay 3 1b41:02fe */
extern void far design_dir_restore(void);
                                /* 18de:1373, vector to overlay 3 1b41:037c */
extern void far data_save(char far *name, unsigned char entry, int len, char far *buf);
                                /* 18de:137d, vector to overlay 3 1b41:027f */
extern void far data_load(char far *name, unsigned char entry, unsigned far *len,
                             char far *buf);
                                /* 18de:1387, vector to overlay 3 1b41:0136 */
extern void far set_data_disk(char disk);
                                /* 18de:13a5, vector to overlay 3 1b41:0083 */
extern void far game_load(char on);
                                /* 18de:144f, vector to overlay 5 1e8f:1150 */
extern void far character_noop(struct Hero far *hero);
                                /* 18de:16ed, vector to overlay 4 1ffa:0dfc */
extern void far label_set_bits(unsigned char num, unsigned char kind, char far *text,
                             unsigned char far *bits);
                                /* 18de:176f, vector to overlay 5 1e8f:079e */
extern void far label_get_bits(unsigned char num, unsigned char kind, char far *out,
                             unsigned char far *bits);
                                /* 18de:1779, vector to overlay 5 1e8f:0766 */
extern unsigned char far kind_mask(unsigned char kind, int dir);
                                /* 18de:1797, vector to overlay 5 1e8f:0f1b */
extern void far labels_forget(void);
                                /* 18de:184b, vector to overlay 5 1e8f:0014 */
extern void far mu_learn(struct Hero far *hero);
                                /* 18de:1963, vector to overlay 7 21d4:334c */
extern char far hero_create(struct Hero far *out);
                                /* 18de:19b3, vector to overlay 7 21d4:31bb */
extern void far train_hero(void);
                                /* 18de:19db, vector to overlay 7 25e1:06b9 */
extern int  far change_class(struct Hero far *hero);
                                /* 18de:1a0d, vector to overlay 7 25e1:0154 */
extern int  far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                             void far *rec, int mode, int size);
                                /* 18de:1ea9, vector to overlay 17 29f4:24d4 */

extern char design_dir[0x22];                                      /* 5a54 */
extern unsigned char large_monsters[28];                              /* 8de6 */
extern struct Hero far *cur_node;                                /* b3dd */
extern char training_done;                                           /* b722 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Pool effect_pool;                                    /* c3c4 */
extern char free_train;                                          /* c524 */
extern struct Pool item_record_pool;                                    /* cb0e */
extern struct Geo geo;                                           /* d414 */

char far *far monst_load(unsigned char num);
char far monst_save(unsigned char num);
long far xp_input(void);

static struct MonArg far *mon_arg;                               /* d580 */

int far monster_screen(int prev, struct EdEnv far *env, struct MonArg far *arg) /* 3e62:0000 */
{
    struct Hero far *node_save;
    struct Item far *item;
    struct Item far *item_next;
    struct Effect far *eff;
    struct Effect far *eff_next;
    int mode;
    struct MonArg far *a;
    struct EdLink far *link;
    int rc;
    char cursor_save;
    char free_save;
    char done;
    register int i;

    if (!arg)
        return 0;
    mon_arg = a = arg;
    link = a->buf;
    done = 0;
    mode = 3;
    switch (prev) {
    case 1:
        a->rec = 0;
        done = 1;
        a->prev = prev;
        a->picking = 1;
        link->next = 8;
        link->code = kind_mask(3, 0) << 8;
        break;
    case 5:
        if (link->next == 6)
            prev = 6;
        mode = 2;
        break;
    case 8:
        if (((env->bits & 0xff00) >> 8) & 0x80 && monst_load(env->bits & 0xff)) {
            a->num = env->bits & 0xff;
            a->mask = (env->bits >> 8) & 0x7f;
            if (a->picking) {
                prev = 1;
                a->picking = 0;
            }
            mode = 2;
        } else if (a->picking) {
            done = 1;
            link->next = a->prev;
        }
        break;
    }
    if (!done) {
        if (a->num >= 0x70) {
            if (confirm("Re-create NPC?", 0) && hero_create(&a->rec->hero)) {
                if (a->rec->hero.race == 5 && confirm("Try to change class?", 0)) {
                    frame_border();
                    if (change_class(&a->rec->hero) != 0x11) {
                        a->rec->hero.xp = xp_input();
                        cursor_save = cursor_obj[0x30];
                        cursor_obj[0x30] = 0xff;
                        training_done = 0;
                        set_extra_row(1);
                        free_save = free_train;
                        free_train = 0;
                        node_save = cur_node;
                        cur_node = &a->rec->hero;
                        do
                            train_hero();
                        while (!training_done);
                        mu_learn(cur_node);
                        cur_node = node_save;
                        free_train = free_save;
                        cursor_obj[0x30] = cursor_save;
                        set_extra_row(0);
                    }
                }
                a->rec->hero.mon_id = a->num;
                a->rec->hero.control = 0xb2;
                i = 0;
                for (eff = a->rec->hero.effects; eff; eff = eff->next) {
                    a->rec->effects[i] = eff->strength;
                    i++;
                }
                for (; i < 10; i++)
                    a->rec->effects[i] = 0;
                for (eff = a->rec->hero.effects; eff; eff = eff_next) {
                    eff_next = eff->next;
                    pool_free(eff, 10, &effect_pool);
                }
                for (item = a->rec->hero.items; item; item = item_next) {
                    item_next = item->next;
                    pool_free(item, 0x3e, &item_record_pool);
                }
                character_noop(&a->rec->hero);
            }
            rc = event_edit(prev, &env->bits, link, 0x39, a->rec, mode, 0x1c2);
        } else
            rc = event_edit(prev, &env->bits, link, 0x36, a->rec, mode, 0x1c2);
        if (rc == 1)
            monst_save(a->num);
    }
    env->bits &= 0xffff0000L;
    switch (link->next) {
    case 1:
        if (a->picking)
            break;
        a->picking = 1;
        link->next = 8;
        link->code = kind_mask(3, 0) << 8 | a->num;
    case 8:
        env->shift = !a->picking;
    case 10:
    case 11:
        env->bits |= link->code;
        break;
    case 6:
        link->next = 5;
    case 5:
        env->bits |= link->code & 0xff;
        link->code >>= 8;
        i = link->code;
        i -= (char far *)&geo.area->rest_enc[0].event - (char far *)geo.area;
        if (i < 0x20)
            env->bits |= i / sizeof(struct Encounter) << 11 | 0x200;
        else if ((i -= 0x20) < 0x20)
            env->bits |= i / sizeof(struct Encounter) << 11 | 0x300;
        else if ((i -= 0x20) < 8)
            env->bits |= i << 11 | 0x400;
        break;
    default:
        link->next = 1;
    }
    return link->next;
}

char far *far monst_load(unsigned char num)                      /* 3e62:05a6 */
{
    unsigned len;
    short v;
    unsigned char far *rec;
    unsigned char far *p;
    char ok;

    ok = 0;
    set_data_disk('2');
    mode_change(6);
    rec = cursor_obj + 0x65;
    data_load("MONST", num, &len, rec);
    if (len == 0 || len > 0x1c2)
        game_exit();
    else {
        if (len < 0x1c2) {
            v = rec[0] << 8 | rec[1];
            mem_move(rec + 2, p = rec + 0x1ea - len, len);
            packbits_expand(p, rec, v);
        }
        if (mon_arg) {
            mon_arg->rec = (struct Monster far *)rec;
            mon_arg->rec->hero.mon_id = num;
        }
        ok = 1;
    }
    return ok ? rec : 0;
}

char far monst_save(unsigned char num)                           /* 3e62:0698 */
{
    short len;
    struct Hero far *rec;
    unsigned char bits;
    unsigned char nbits;
    char ok;
    char name[16];

    ok = 0;
    set_data_disk('2');
    mode_change(6);
    rec = (struct Hero far *)(cursor_obj + 0x65);
    rec->mon_id = num;
    rec->hp = rec->hp_max;
    len = 0x1c2;
    data_save("MONST", num, len, rec);
    if (len != 0x1c2) {
        error_box("Unable to save monster %d", num);
        game_exit();
    } else {
        label_get_bits(num, 3, name, &bits);
        nbits = bits | 0x40;
        if (str_cmp(rec->name, name))
            label_set_bits(num, 3, rec->name, &nbits);
        else if (nbits != bits)
            label_set_bits(num, 3, 0, &nbits);
        ok = 1;
    }
    return ok;
}

char far monst_default(unsigned char num, char far *buf)         /* 3e62:0798 */
{
    char ok;
    char path[0x50];
    char name[0x10];

    ok = 0;
    str_format(name, "MONST%03d.dat", num);
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, name);
    if (dos_open(0x4100, path) >= 0 && monst_load(num)) {
        str_copy(buf, mon_arg->rec->hero.name);
        ok = 1;
    }
    return ok;
}

struct Monster far *far monst_cur(void)                          /* 3e62:082d */
{
    return mon_arg ? mon_arg->rec : 0;
}

char far monst_listed(unsigned char num)                         /* 3e62:0849 */
{
    return mem_scan(large_monsters, 0x1c, num) < 0x1c;
}

char far monst_unlisted(unsigned char num)                       /* 3e62:086d */
{
    return mem_scan(large_monsters, 0x1c, num) >= 0x1c;
}

int far monster_pick(int prev, struct EdEnv far *env, struct MonArg far *arg) /* 3e62:0891 */
{
    struct MonArg far *a;
    struct ArtSel far *sel;
    int code;

    if (!arg)
        return 0;
    mon_arg = a = arg;
    sel = a->buf;
    sel->next = prev;
    sel->done = 1;
    switch (prev) {
    case 10:
    case 11:
        if (env->bits & 0xf) {
            game_load(0);
            sel->sub = monst_listed(a->num);
            sel->done = 0;
            code = kind_mask(3, 0) << 8;
        } else {
        pick:
            sel->done = 1;
            a->picking = 1;
            code = kind_mask(3, 0) << 8 | a->num;
        }
        sel->next = 8;
        break;
    case 8:
        if (a->picking) {
            a->picking = 0;
            a->num = env->bits & 0xff;
            a->mask = (env->bits >> 8) & 0x7f;
            sel->done = 4;
            sel->next = 0xb;
            break;
        }
        if (((env->bits & 0xff00) >> 8) & 0x80 && monst_load(env->bits & 0xff)) {
            design_dir_restore();
            labels_forget();
            game_load(0);
            monst_save(a->num);
        } else {
            design_dir_restore();
            labels_forget();
            game_load(0);
        }
        goto pick;
    }
    env->bits &= 0xffff0000L;
    switch (sel->next) {
    case 10:
    case 11:
        env->bits |= sel->done;
        break;
    case 8:
        env->shift = 0;
        env->bits |= code;
        break;
    default:
        sel->next = 1;
    }
    return sel->next;
}

long far parse_long(char far *s)                                /* 3e62:0abb */
{
    char neg;
    long n;

    n = 0;
    if (*s == '-') {
        neg = 1;
        s++;
    } else
        neg = 0;
    while (*s >= '0' && *s <= '9')
        n = n * 10 - (*s++ - '0');
    return neg ? n : -n;
}

long far xp_input(void)                                          /* 3e62:0b3d */
{
    return parse_long(input_text("Experience: ", 7, 8, 8));
}
