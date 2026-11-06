/* CKIT.EXE overlay section 22, module 2d91: the design editor's event chain
 * screen, screen 6 of editloop.c's loop.  chain_screen is called with the
 * screen the editor comes back from, editloop.c's env and the screen's
 * record (struct EvChain, editloop.c's aab); it takes in what env->bits
 * reports, and leaves in env->bits what the next screen (5, module 2fb8's
 * event screen, or 10 and 11) is handed.  A chain is the list of events one
 * place calls in turn: a map square, an area slot of kind 2 to 4 ("Rest",
 * "Step", "Shared" by the DGROUP table at 0474) or a field of another
 * event's record.  The chain lives in EvChain.chain, up to 30 links, and is
 * built by chain_follow from the event records (textbank.c's event_table,
 * 0x14 bytes each), following byte 3 of each record to the next.
 * chain_run draws the screen ("Previous Event", "Current Event", "Next
 * Event", the record and text pool usage) and polls the command line
 * "Leave Prev Next Edit Insert Delete Alt" (chain_command, the keys by
 * chain_key); chain_link writes a link back into the record, square or slot
 * before it.  The alternatives list (alt_* in _BSS) holds the events one
 * event's fields name: alt_reset starts it, alt_collect (evedit.c's stub
 * 18de:1f03, once per entry of its offset list) and alt_add (section 25) fill it,
 * alt_step walks it, and alt_delete and alt_drop free the events in it in
 * turn.
 *
 * Section 22 (parent section 20, which is empty and has no parent; no
 * section names 22 as its parent): payload at EXE 0x075ef0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 22.  The module is stored frame 2d91 (load-based 3d91),
 * EXE 0x075ef0..0x07816b (8,827 bytes, 45 routines), from the section's
 * first byte to its last retf at 2d91:227a; module 2fb8 follows at
 * 2fb8:000b (EXE 0x07816b, the target of vector 18de:1c65, editloop.c's
 * screen 5).  One Turbo C large-model _TEXT segment, built with Turbo C
 * 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the far
 * pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 22's relocation table files 85 sites: 78 under 2d91, all in this
 * range (EXE 0x075f89..0x078140), all far calls (71 resident, 7 through
 * vector stubs); the other 7 are module 2fb8's, filed under 2fb8.  Four
 * vectors land here: 18de:1c5b -> chain_screen (editloop.c's
 * chain_screen, screen 6, called once from section 10), 18de:1f03 ->
 * alt_collect (evedit.c's alt_collect, two calls from section 17),
 * 18de:1ffd -> alt_add (three calls from section 25) and 18de:1ff3 ->
 * alt_drop (one call from section 25).  No far call in section 22 stores
 * frame 2d91, and no section is its descendant; the other 41 routines are
 * each near-called ahead of their definitions, so static.  The module calls
 * the resident code directly, and sections 5, 10 and 26 only through
 * vector stubs.
 *
 * FRUA's own code: the sharing map pairs 43 routines a-only and two
 * partial (0a6e with DQK menu.c's menu_bar, 0fec with DQK create.c's
 * class_set), so every name here is chosen from the code.
 *
 * _DATA is DGROUP 90d2..9127: nine literals stored as seven (-d stores
 * "" on the NUL of "%s %s" and "%s" as its tail).  Below it fxcore.c's (section 19) _DATA
 * ends at 90d0 and 90d1 is a pad; 9128 ("Unable to write geo in
 * Save3DMap.") is referenced from section 23.  _BSS is the alternatives
 * list, d4aa..d527 (names chosen with goldbox.bss_order); nothing outside
 * this module references it.  Below it rest.c's (section 19) fx_live ends
 * at d494, and d495..d4a9 is not referenced by this module; above it d528
 * is referenced from section 23.
 *
 * Forced by the bytes: p->cmd = 0 << 8 | p->cur.ev (the shipped or ax,0);
 * (event_table + ev - 1) where event_table[ev - 1] gives dec ax; chain_follow's
 * four-byte struct LinkHead local, passed where chain_append takes a Link
 * (a struct Link local bakes 44 of 45); chain_command's default: ahead of
 * case 5, so the beep the other paths jump to is case 5's; the separate
 * l = chain_prev(...); if (l) in chain_command and pct = ...; in usage_draw
 * (or ax,dx: the value the statement before left in dx:ax); t < 0,
 * key == 0x86 and pos + (last >= 0) as relations, not ?: (mov ax,1);
 * switch (t) in the post-switch case 11; register n and row in usage_draw;
 * the if/else shapes of chain_splice and chain_link that put return 0 last.
 */

struct MapCell {                /* a square of the area, 6 bytes          */
    unsigned char wall[4];      /* +0                                     */
    unsigned char event;        /* +4                                     */
    unsigned char bits;         /* +5                                     */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0                                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[0x2c];           /* +4    not read here; the bounds of
                                         rest_ev, step and shared below are
                                         spacing, not read from the code  */
    struct {
        unsigned char ev;       /* +0                                     */
        char pad_1[3];          /* +1                                     */
    } rest_ev[8];               /* +30   kind 2                           */
    struct {
        unsigned char ev;       /* +0                                     */
        char pad_1[3];          /* +1                                     */
    } step_ev[7];               /* +50   kind 3                           */
    char pad_6c[2];             /* +6c   not read here                    */
    unsigned char shared_events[8]; /* +6e   kind 4                           */
    char pad_76[0xac];          /* +76   not read here                    */
    struct MapCell cells[0x240]; /* +122  kind 1                           */
};

struct Pos {                    /* a place and facing, 5 bytes            */
    char x;                     /* +0                                     */
    char y;                     /* +1                                     */
    unsigned char facing;       /* +2                                     */
    char facing_wall;           /* +3                                     */
    char event;                 /* +4                                     */
};

struct Geo {                    /* view3d.c's geo record                  */
    struct Pos pos;             /* +0                                     */
    char attr_5;                /* +5                                     */
    char attr_6;                /* +6                                     */
    char show_map;              /* +7                                     */
    struct Area far *area;      /* +8                                     */
    char attr_c;                /* +c                                     */
    char attr_d;                /* +d                                     */
};

struct Event {                  /* an event record, textbank.c's 0x14     */
    unsigned char bytes[0x14];  /* +0   b[0] the kind, b[3] the next      */
};

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0                                     */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct Link {                   /* one step of a chain, 5 bytes           */
    int kind;                   /* +0   0 an event's field, 1 a square,
                                        2..4 an area slot                 */
    unsigned char ev;           /* +2   the event                         */
    unsigned char a;            /* +3   the field, row or slot            */
    unsigned char col;          /* +4   the column                        */
};

struct LinkHead {               /* a Link without its column              */
    int kind;                   /* +0                                     */
    unsigned char ev;           /* +2                                     */
    unsigned char a;            /* +3                                     */
};

struct Chain {
    int pos;                    /* +0   the current step                  */
    int last;                   /* +2   the last step, -1 none            */
    struct Link items[30];      /* +4                                     */
};

struct EvChain {                /* the screen's argument, editloop.c's
                                   aab                                    */
    int next;                   /* +0   the screen to go to               */
    int scr;                    /* +2   the screen that called            */
    int env_hi;                 /* +4   env->bits' high word              */
    unsigned char ev_ret;       /* +6                                     */
    unsigned char alt;          /* +7   the alternative shown, 1-based    */
    char alt_keep;              /* +8                                     */
    char clean;                 /* +9                                     */
    int cmd;                    /* +a                                     */
    unsigned flags;             /* +c                                     */
    unsigned char kind;         /* +e                                     */
    unsigned char next_ev;      /* +f                                     */
    unsigned char alt_last;     /* +10                                    */
    struct Link cur;            /* +11                                    */
    struct Chain chain;         /* +16                                    */
};

extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern int  far event_used_count(void);                             /* 03ad:0058 */
extern int  far event_free_slot(void);                             /* 03ad:0097 */
extern int  far event_release(int n);                            /* 03ad:00d7 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern int  far menu_line_key(int key);                              /* 083a:1e2d */
extern void far menu_line_show(char far *title, char far *text, char fkeys); /* 083a:1e9a */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern int  far txt_usage(char far *pool, int far *cap);         /* 0cf0:0548 */
extern void far frame_lower_panel(void);                                /* 0d65:020c */
extern void far beep(void);                                      /* 0da7:018c */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far input_poll(void);                                /* 1157:05c0 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_poll(void);                                   /* 15e5:15e6 */
extern void far dl_end(void);                               /* 15e5:187a */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far chain_label(unsigned char n, unsigned char k, char far *out);
                                /* 18de:178d, vector to overlay 5 1e8f:0e3e */
extern void far label_get(unsigned char num, unsigned char kind, char far *out);
                                /* 18de:1819, vector to overlay 5 1e8f:06b0 */
extern char far edit_dirty(void);
                                /* 18de:1b7f, vector to overlay 10 267d:047f */
extern void far edit_dirty_set(void);
                                /* 18de:1bb1, vector to overlay 10 267d:046d */
extern void far ed_header(int row, int col, int w, int h, int a, char b);
                                /* 18de:2007, vector to overlay 26 2e34:0de8 */

extern char far *at_text;                          /* 0370  "at"           */
extern char far *on_text;                          /* 037c  "on"           */
extern char far *no_text;                          /* 03e8  "No"           */
extern char far *chain_items;                          /* 040c  "Leave Prev Next Edit Insert Delete Alt" */
extern char far *event_word;                          /* 042c  "Event"        */
extern char far *prev_event_text;                          /* 0454  "Previous Event" */
extern char far *cur_event_text;                          /* 0458  "Current Event" */
extern char far *alt_of_fmt;                          /* 045c  "(ALT %d of %d)" */
extern char far *next_event_text;                          /* 0460  "Next Event"   */
extern char far *called_text;                          /* 0468  "Called"       */
extern char far *chain_text;                          /* 046c  "chain"        */
extern char far *map_word;                          /* 0470  "Map"          */
extern char far *slot_kind_names[3];                       /* 0474  "Rest", "Step", "Shared" */
extern char far *normal_text;                          /* 0480  "Normal"       */
extern char far *text_word;                          /* 0488  "Text"         */
extern char far *evnt_text;                          /* 048c  "Evnt"         */
extern char far *used_text;                          /* 0490  "used"         */
extern char far *free_text;                          /* 0494  "free"         */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *text_pool_block;                                /* a7ec */
extern struct Event far *event_table;                               /* a7f0 */
extern char menu_escaped;                                        /* ca2a */
extern struct Geo geo;                                           /* d414 */

static void far chain_start(struct EvChain far *p, unsigned char ev, unsigned char kind,
                            unsigned char x);
static void far chain_to_field(struct EvChain far *p, unsigned char ev, unsigned char a);
static void far chain_05fe(struct EvChain far *p, char kind);
static void far chain_new(struct EvChain far *p);
static void far new_event_link(struct EvChain far *p);
static void far new_event_cancel(struct EvChain far *p);
static void far delete_begin(struct EvChain far *p);
static char far delete_unlink(struct EvChain far *p);
static void far delete_end(struct EvChain far *p, char flag);
static void far chain_refresh(struct EvChain far *p);
static void far chain_run(struct EvChain far *p);
static void far chain_command(struct EvChain far *p, register int cmd);
static int  far chain_key(struct EvChain far *p, register int key);
static void far chain_draw(struct EvChain far *p);
static void far link_draw(struct EvChain far *p, struct Link far *l, register int row,
                          int col, char which);
static void far usage_draw(struct EvChain far *p);
static void far chain_reset(struct EvChain far *p);
static char far chain_splice(struct EvChain far *p);
static char far chain_add_last(struct EvChain far *p);
static unsigned char far chain_unlink(struct EvChain far *p);
static void far chain_cut(struct EvChain far *p);
static void far chain_clear(struct Chain far *c);
static char far chain_insert(struct Chain far *c, struct Link far *l);
static char far chain_append(struct Chain far *c, struct Link far *l);
static char far chain_delete(struct Chain far *c);
static struct Link far *far chain_cur(struct Chain far *c);
static struct Link far *far chain_prev(struct Chain far *c, char move);
static struct Link far *far chain_next(struct Chain far *c, char move);
static struct Link far *far chain_first(struct Chain far *c, char move);
static struct Link far *far chain_last(struct Chain far *c, char move);
static void far chain_load(struct EvChain far *p);
static char far chain_follow(struct EvChain far *p, unsigned char k);
static char far chain_link(struct EvChain far *p);
static char far chain_root_clear(struct EvChain far *p);
static void far event_kind_set(unsigned char ev, unsigned char kind);
static void far alt_sync(struct EvChain far *p);
static void far alt_step(struct EvChain far *p, char back, char keep);
static void far alt_reset(unsigned char ev, char shared);
static char far delete_confirm(struct EvChain far *p);
static void far delete_scan(struct EvChain far *p);
static char far alt_delete(struct EvChain far *p);

static int alt_pos;                                              /* d4aa */
static int alt_last;                                             /* d4ac */
static char alt_shared;                                          /* d4ae */
static unsigned char alt_root;                                   /* d4af */
static unsigned char alt_items[100];                             /* d4b0 */
static unsigned char alt_fields[20];                             /* d514 */

int far chain_screen(int prev, struct EdEnv far *env, struct EvChain far *arg) /* 3d91:0000 */
{
    unsigned char ev;
    unsigned char kind;
    struct EvChain far *p;
    int t;

    if (arg == 0)
        return 0;
    p = arg;
    p->next = prev;
    switch (prev) {
    case 5:
        p->scr = prev;
        p->flags &= 0x3fff;
        p->flags |= env->bits & 0xc000;
        p->env_hi = env->bits >> 16;
        ev = env->bits & 0xff;
        if (env->bits & 0x8000)
            chain_start(p, ev, (env->bits & 0x700) >> 8, (env->bits & 0x3800) >> 11);
        else {
            t = p->cmd;
            p->cmd = 0;
            kind = (env->bits & 0x3f00) >> 8;
            switch (kind) {
            case 0:
            case 1:
                if (p->flags & 0x40)
                    chain_run(p);
                else if (p->flags & 0x20) {
                    if (!delete_confirm(p))
                        delete_unlink(p);
                } else if (p->flags & 0x100)
                    delete_scan(p);
                else if (p->flags & 4) {
                    if ((t = alt_delete(p)) <= 0)
                        delete_end(p, t < 0);
                } else if (p->flags & 2)
                    new_event_link(p);
                else
                    chain_05fe(p, kind);
                break;
            default:
                switch (t >> 8) {
                case 2:
                case 3:
                    chain_to_field(p, env->bits & 0xff, kind);
                }
            }
        }
        break;
    case 11:
        p->next = p->scr;
        t = p->cmd;
        p->cmd = 0;
        if ((t & 0xf) == 1) {
            if (env->bits & 0xf) {
                p->kind = (env->bits & 0xff0) >> 4;
                event_kind_set(p->cur.ev, p->kind);
                p->cmd = 0 << 8 | p->cur.ev;
                p->next = 5;
            } else
                new_event_cancel(p);
        }
        break;
    case 10:
        p->next = p->scr;
        t = p->cmd;
        p->cmd = 0;
        switch (t & 0x3f) {
        case 10:
            new_event_cancel(p);
            break;
        case 4:
        case 6:
            if (env->bits & 0xf)
                delete_unlink(p);
            else
                delete_end(p, 0);
        }
        break;
    default:
        p->next = p->scr;
    }
    env->bits &= 0xffff0000L;
    switch (p->next) {
    case 10:
        t = p->cmd & 0x3f;
        env->bits |= p->cmd;
        break;
    case 11:
        t = p->cmd & 0xf;
        switch (t) {
        case 1:
            env->bits |= t;
            break;
        default:
            env->bits |= p->cmd;
        }
        break;
    case 5:
        if ((t = (p->cmd & 0x3f00) >> 8) == 2 || t == 3)
            p->ev_ret = p->cmd & 0xff;
        env->bits |= p->flags & 0xc000;
        env->bits |= p->cmd & 0x3fff;
        if (p->flags & 0x2000)
            env->bits |= 0x8000;
    }
    return p->next;
}

static void far chain_start(struct EvChain far *p, unsigned char ev, unsigned char kind,
                            unsigned char x)                    /* 3d91:041f */
{
    p->flags &= 0xc000;
    p->flags |= 0x2000;
    p->ev_ret = p->cmd = 0;
    p->clean = !edit_dirty();
    p->cur.ev = ev;
    p->cur.kind = kind;
    switch (kind) {
    case 1:
        p->cur.a = geo.pos.y;
        p->cur.col = geo.pos.x;
        if (x)
            p->flags |= 4;
        break;
    case 2:
    case 3:
    case 4:
        p->cur.a = x;
    }
    chain_reset(p);
    if (ev == 0)
        chain_new(p);
    else if (p->flags & 4) {
        chain_insert(&p->chain, &p->cur);
        delete_begin(p);
    } else {
        chain_load(p);
        chain_refresh(p);
    }
}

static void far chain_to_field(struct EvChain far *p, unsigned char ev, unsigned char a) /* 3d91:050d */
{
    char found;
    struct Link far *l;

    found = 0;
    p->flags &= 0xc000;
    p->flags |= 1;
    p->cur.ev = ev;
    p->cur.kind = 0;
    p->cur.a = a;
    if ((l = chain_next(&p->chain, 0)) != 0) {
        if (l->a != a)
            chain_cut(p);
        else if (l->ev != ev)
            p->cmd = ev | 0x300;
        else if (ev)
            found = 1;
    }
    if (found) {
        chain_next(&p->chain, 1);
        chain_refresh(p);
    } else if (p->cmd == 0) {
        if (ev == 0)
            chain_new(p);
        else {
            chain_add_last(p);
            chain_follow(p, 3);
            chain_refresh(p);
        }
    }
}

static void far chain_05fe(struct EvChain far *p, char kind)     /* 3d91:05fe */
{
    struct Link far *l;

    if ((l = chain_cur(&p->chain)) != 0) {
        if (kind == 0 && (p->flags & 0x80)) {
            p->flags &= ~0x90;
            p->flags |= 4;
            delete_unlink(p);
        } else if (l->kind != 0 && (p->flags & 0x2000)) {
            p->flags &= ~0x80;
            if (kind)
                p->flags &= ~0x2000;
            p->cmd = (kind + 0x3e) << 8;
            p->next = 5;
        } else
            chain_refresh(p);
    }
}

static void far chain_new(struct EvChain far *p)                 /* 3d91:069a */
{
    char n;

    p->flags |= 0x82;
    if ((n = event_free_slot()) != 0) {
        p->cur.ev = n;
        p->cmd = 1;
        p->next = 0xb;
    } else {
        p->cur.ev = 0;
        p->cmd = 0xa;
        p->next = 0xa;
    }
}

static void far new_event_link(struct EvChain far *p)                /* 3d91:06e3 */
{
    unsigned char ev;

    event_kind_set(ev = p->cur.ev, p->kind);
    edit_dirty_set();
    if ((p->flags & 1) && !chain_next(&p->chain, 1)) {
        chain_add_last(p);
        chain_follow(p, 3);
    } else
        chain_splice(p);
    p->flags &= ~2;
    p->flags |= 0x10;
    p->cmd = ev | 0x200;
    p->next = 5;
}

static void far new_event_cancel(struct EvChain far *p)                /* 3d91:0775 */
{
    p->flags &= ~0x82;
    if (p->cur.ev)
        event_release(p->cur.ev);
    if ((p->flags & 1) || (p->flags & 0x2000)) {
        p->flags |= 0x10;
        if (p->flags & 0x2000)
            p->cmd = p->ev_ret | 0x3e00;
        else {
            p->flags &= ~1;
            p->cmd = p->ev_ret | 0x300;
        }
        p->next = 5;
    } else
        chain_refresh(p);
}

static void far delete_begin(struct EvChain far *p)                /* 3d91:07f6 */
{
    unsigned char ev;
    struct Link far *l;

    if ((l = chain_cur(&p->chain)) != 0) {
        p->flags |= 0x24;
        ev = l->ev;
        if (!(p->flags & 0x2000)) {
            p->next_ev = (event_table + ev - 1)->bytes[3];
            (event_table + ev - 1)->bytes[3] = 0;
        } else
            p->next_ev = 0;
        alt_reset(ev, 0);
        p->cmd = ev | 0x600;
        p->next = 5;
    }
}

static char far delete_unlink(struct EvChain far *p)                /* 3d91:089a */
{
    char at_end;
    unsigned char ev;

    at_end = 0;
    if (chain_cur(&p->chain) == chain_first(&p->chain, 0)
        && chain_cur(&p->chain) == chain_last(&p->chain, 0)) {
        at_end = 1;
        chain_root_clear(p);
    }
    if ((ev = chain_unlink(p)) != 0) {
        alt_reset(ev, 0);
        p->cmd = ev & 0xff | 0x500;
        p->next = 5;
        return 1;
    }
    if (at_end)
        chain_link(p);
    beep();
    return 0;
}

static void far delete_end(struct EvChain far *p, char flag)     /* 3d91:096f */
{
    p->flags &= ~4;
    if (chain_cur(&p->chain) && !(p->flags & 0x2000))
        chain_refresh(p);
    else {
        p->cmd = ((flag ? 1 : 0) + 0x3e) << 8;
        p->next = 5;
    }
}

static void far chain_refresh(struct EvChain far *p)                /* 3d91:09c8 */
{
    unsigned char ev;
    struct Link far *l;

    p->flags &= 0xc000;
    p->flags |= 0x40;
    if (p->alt_keep && p->alt)
        l = chain_cur(&p->chain);
    else
        l = chain_prev(&p->chain, 0);
    if (l) {
        ev = l->ev;
        alt_reset(ev, 1);
        p->cmd = ev | 0x600;
        p->next = 5;
    } else {
        alt_reset(0, 1);
        chain_run(p);
    }
}

static void far chain_run(struct EvChain far *p)                 /* 3d91:0a6e */
{
    int key;
    int cmd;

    screen_draw_begin(1);
    screen_flush_mode_set(1);
    frame_lower_panel();
    usage_draw(p);
    dl_begin();
    hotkey_reset(6);
    menu_line_show(empty_text, chain_items, 1);
    ui_add(5, 0x1f48, 0x1f44, 0xc, 0x99, 0x29, 0xb, 0x14,
           5, 0x1f70, 0x1f44, 0xc, 0x99, 0x29, 7, 0x14,
           5, 0x1f58, 0x1f44, 0x14, 0x99, 0x14,
           5, 0x1f58, 0x1f40, 0x14, 4, 0x29, 9, 0x14,
           5, 0x1f58, 0x1fdc, 0x14, 4, 0x29, 5, 0x14, 0);
    alt_sync(p);
    if (!p->alt) {
        ui_mark(7, 0x10);
        ui_mark(0xb, 0x10);
        ui_mark(0xc, 0x10);
    }
    dl_draw(1);
    screen_flush_mode_set(0);
    screen_flush();
    do {
        chain_draw(p);
        while ((key = ui_poll()) < 0)
            input_poll();
        if ((cmd = menu_line_key(key)) >= 0)
            chain_command(p, cmd);
        else
            switch (key) {
            case 8:
            case 9:
                menu_escaped = 1;
                chain_command(p, key == 8 ? 0x88 : 0x84);
                break;
            case 10:
                chain_command(p, 3);
                break;
            case 11:
            case 12:
                menu_escaped = 1;
                chain_command(p, key == 11 ? 0x86 : 0x82);
            }
    } while (p->cmd == 0);
    dl_end();
}

static void far chain_command(struct EvChain far *p, register int cmd) /* 3d91:0c84 */
{
    unsigned char ev;
    struct Link far *l;
    int key;

    ev = 0;
    if (menu_escaped) {
        key = cmd;
        cmd = chain_key(p, cmd);
    } else
        key = 0;
    if (cmd < 0) {
        if (cmd == -1)
            beep();
    } else
        switch (cmd) {
        case 0:
            p->flags &= ~0x40;
            p->cmd = 0x3f00;
            p->next = 5;
            break;
        case 1:
            chain_prev(&p->chain, 1);
            l = chain_prev(&p->chain, 0);
            if (l)
                ev = l->ev;
            else
                ev = 0;
            alt_reset(ev, 1);
            p->cmd = ev | 0x600;
            p->next = 5;
            break;
        case 2:
            if ((l = chain_cur(&p->chain)) != 0 && (ev = l->ev) != 0) {
                alt_reset(ev, 1);
                p->cmd = ev | 0x600;
                p->next = 5;
            }
            chain_next(&p->chain, 1);
            break;
        case 3:
            if ((l = chain_cur(&p->chain)) != 0 && (ev = l->ev) != 0) {
                p->flags &= ~0x40;
                p->cmd = ev | 0x200;
                p->next = 5;
            } else
                beep();
            break;
        case 4:
            p->flags &= ~0x40;
            chain_new(p);
            break;
        default:
            beep();
            break;
        case 5:
            if ((l = chain_cur(&p->chain)) != 0 && (ev = l->ev) == 0)
                beep();
            else {
                p->flags &= ~0x40;
                delete_begin(p);
            }
            break;
        case 6:
            alt_step(p, key == 0x86, 0);
        }
}

static int far chain_key(struct EvChain far *p, register int key) /* 3d91:0e7c */
{
    if (key == 0xd)
        key = 3;
    else if (key == 0x1b || key == 0x60)
        key = 0;
    else
        switch (key) {
        case 0x88:
            if (chain_cur(&p->chain) != chain_first(&p->chain, 0))
                key = 1;
            else
                key = -1;
            break;
        case 0x84:
            if (chain_cur(&p->chain) != chain_last(&p->chain, 0))
                key = 2;
            else
                key = -1;
            break;
        case 0x82:
        case 0x86:
            if (p->alt)
                key = 6;
            else
                key = -1;
            break;
        case 0x87:
            if (chain_cur(&p->chain) != chain_first(&p->chain, 0)) {
                chain_first(&p->chain, 1);
                key = -2;
            } else
                key = -1;
            break;
        case 0x85:
            if (chain_cur(&p->chain) != chain_last(&p->chain, 0)) {
                chain_last(&p->chain, 1);
                key = -2;
            } else
                key = -1;
            break;
        default:
            key = -1;
        }
    menu_escaped = 0;
    return key;
}

static void far chain_draw(struct EvChain far *p)                /* 3d91:0fec */
{
    struct Link far *before;
    struct Link far *cur;
    struct Link far *after;
    int col;
    int row;

    if ((before = chain_prev(&p->chain, 0)) != 0) {
        ui_mark(2, 0x18);
        ui_mark(8, 0x18);
    } else {
        ui_mark(2, 0x10);
        ui_mark(8, 0x10);
    }
    cur = chain_cur(&p->chain);
    if ((after = chain_next(&p->chain, 0)) != 0 || cur->ev) {
        ui_mark(3, 0x18);
        ui_mark(9, 0x18);
    } else {
        ui_mark(3, 0x10);
        ui_mark(9, 0x10);
    }
    if (cur->ev) {
        ui_mark(4, 0x18);
        ui_mark(6, 0x18);
        ui_mark(0xa, 0x18);
    } else {
        ui_mark(4, 0x10);
        ui_mark(6, 0x10);
        ui_mark(0xa, 0x10);
    }
    col = 0x1f44;
    row = 0x1f48;
    text_print_at(row, col, before ? 0x87 : 0x80, prev_event_text);
    link_draw(p, before, row + 4, col, 1);
    row = 0x1f58;
    text_print_at(row, col, 0x8b, "%s %s", cur_event_text,
                  p->alt ? msg_format(alt_of_fmt, p->alt, alt_last) : "");
    link_draw(p, cur, row + 4, col, 0);
    row = 0x1f70;
    if (after)
        text_print_at(row, col, after->ev ? 0x87 : 0x80, next_event_text);
    else
        fill_rect(row, col, row + 4, 0x1fdc, 8);
    link_draw(p, after, row + 4, col, -1);
}

static void far link_draw(struct EvChain far *p, struct Link far *l, register int row,
                          int col, char which)                  /* 3d91:1251 */
{
    unsigned char ev;
    unsigned char kind;
    unsigned char a;
    char buf[0x10];
    int h;
    struct Link far *n;
    int attr;

    ev = l ? l->ev : 0;
    if (which) {
        h = 0xc;
        attr = ev ? 0x87 : 0x80;
    } else {
        h = 0x14;
        attr = ev ? 0x8b : 0x80;
    }
    fill_rect(row, col, row + h, 0x1fdc, 8);
    if (l && (ev || which < 1)) {
        if (ev) {
            kind = (event_table + ev - 1)->bytes[0];
            label_get(kind, 0xa, buf);
        } else
            str_format(buf, "%s %s", no_text, event_word);
        text_print_at(row, col, attr, buf);
        switch (l->kind) {
        case 0:
            a = l->a;
            if (a != 3 && (n = chain_prev(&p->chain, 0)) != 0) {
                kind = (event_table + n->ev - 1)->bytes[0];
                chain_label(kind, a, buf);
            } else
                str_format(buf, "%s %s", normal_text, chain_text);
            text_print_at(row + 4, col, attr, "%s %s %s", called_text, on_text, buf);
            break;
        case 1:
            text_print_at(row + 4, col, attr, "%s %s %s col: %d, row: %d", map_word, event_word,
                          at_text, l->col, l->a);
            break;
        case 2:
        case 3:
        case 4:
            text_print_at(row + 4, col, attr, "%s %s #%d", slot_kind_names[l->kind - 2], event_word,
                          l->a + 1);
        }
    } else if (which > 0 || l)
        text_print_at(row, col, 0x80, "%s %s", no_text, event_word);
}

static void far usage_draw(struct EvChain far *p)                /* 3d91:14c8 */
{
    char buf[0x100];
    int col;
    int used;
    int cap;
    long pct;
    register int n;
    register int row;

    row = 0x1f88;
    col = 0x1f44;
    ed_header(row, col, 0x26, 6, 0, 0);
    n = txt_usage(text_pool_block, &cap);
    used = event_used_count();
    n = cap - n;
    pct = ((long)n * 1000 / cap + 5) / 10;
    str_format(buf, "%s %d %s, %d %s. %s %ld%% %s%s", evnt_text, used, used_text, 100 - used,
               free_text, text_word, pct, free_text, pct ? "." : "!");
    text_print_at(row + 0x10, (0x26 - str_len(buf)) / 2 * 4 + col, 0x87, "%s", buf);
}

static void far chain_reset(struct EvChain far *p)               /* 3d91:15ef */
{
    chain_clear(&p->chain);
}

static char far chain_splice(struct EvChain far *p)                /* 3d91:1605 */
{
    unsigned char ev;
    struct Link far *nx;
    struct Link far *cur;

    if (chain_insert(&p->chain, &p->cur)) {
        cur = chain_cur(&p->chain);
        if ((nx = chain_next(&p->chain, 0)) != 0) {
            ev = cur->ev;
            mem_move(nx, cur, 5);
            cur->ev = ev;
            (event_table + ev - 1)->bytes[3] = nx->ev;
            nx->kind = 0;
            nx->a = 3;
        }
        return chain_link(p);
    }
    return 0;
}

static char far chain_add_last(struct EvChain far *p)                /* 3d91:16ce */
{
    if (chain_append(&p->chain, &p->cur)) {
        chain_last(&p->chain, 1);
        return chain_link(p);
    }
    return 0;
}

static unsigned char far chain_unlink(struct EvChain far *p)       /* 3d91:1714 */
{
    unsigned char ev;
    unsigned char t;
    unsigned char far *rec;
    unsigned char rc;
    struct Link far *l;
    struct Link far *m;

    if ((l = chain_cur(&p->chain)) == 0)
        return 0;
    rc = 0;
    p->alt_keep = 0;
    mem_move(l, &p->cur, 5);
    ev = l->ev;
    if ((m = chain_next(&p->chain, 0)) != 0 && m->a != 3) {
        chain_cut(p);
        chain_follow(p, 3);
        m = chain_next(&p->chain, 0);
    }
    if (!(p->flags & 0x2000))
        (event_table + ev - 1)->bytes[3] = 0;
    if (chain_delete(&p->chain)) {
        if (m) {
            m = chain_cur(&p->chain);
            t = m->ev;
            mem_move(&p->cur, m, 5);
            m->ev = t;
            if (chain_link(p))
                rc = ev;
        } else if ((m = chain_cur(&p->chain)) == 0)
            rc = ev;
        else if (p->cur.kind == 0) {
            rec = (event_table + m->ev - 1)->bytes;
            rec[p->cur.a] = 0;
            p->alt_keep = 1;
            rc = ev;
        }
    }
    return rc;
}

static void far chain_cut(struct EvChain far *p)                 /* 3d91:18cd */
{
    if (p->chain.last >= 0)
        p->chain.last = p->chain.pos;
}

static void far chain_clear(struct Chain far *c)                 /* 3d91:18e4 */
{
    c->pos = 0;
    c->last = -1;
}

static char far chain_insert(struct Chain far *c, struct Link far *l) /* 3d91:18f7 */
{
    int i;

    if (c->last < 0x1d && c->pos >= 0) {
        for (i = c->last; c->pos <= i; i--)
            c->items[i + 1] = c->items[i];
        c->items[c->pos] = *l;
        c->last++;
        return 1;
    }
    return 0;
}

static char far chain_append(struct Chain far *c, struct Link far *l) /* 3d91:1973 */
{
    if (c->last < 0x1d && c->last >= -1) {
        c->items[++c->last] = *l;
        return 1;
    }
    return 0;
}

static char far chain_delete(struct Chain far *c)                /* 3d91:19b0 */
{
    int i;

    if (c->last >= 0 && c->pos >= 0 && c->pos < 0x1e) {
        for (i = c->pos; c->last > i; i++)
            c->items[i] = c->items[i + 1];
        c->last--;
        c->pos = int_max(0, int_min(c->pos, c->last));
        return 1;
    }
    return 0;
}

static struct Link far *far chain_cur(struct Chain far *c)       /* 3d91:1a32 */
{
    if (c->last >= 0)
        return &c->items[c->pos];
    return 0;
}

static struct Link far *far chain_prev(struct Chain far *c, char move) /* 3d91:1a5b */
{
    if (c->pos > 0)
        return &c->items[move ? --c->pos : c->pos - 1];
    return 0;
}

static struct Link far *far chain_next(struct Chain far *c, char move) /* 3d91:1a96 */
{
    if (c->last > c->pos)
        return &c->items[move ? ++c->pos : c->pos + 1];
    return 0;
}

static struct Link far *far chain_first(struct Chain far *c, char move) /* 3d91:1ad4 */
{
    if (c->last >= 0)
        return &c->items[move ? (c->pos = 0) : 0];
    return 0;
}

static struct Link far *far chain_last(struct Chain far *c, char move) /* 3d91:1b0a */
{
    if (c->last >= 0)
        return &c->items[move ? (c->pos = c->last) : c->last];
    return 0;
}

static void far chain_load(struct EvChain far *p)                /* 3d91:1b47 */
{
    chain_insert(&p->chain, &p->cur);
    chain_follow(p, 3);
}

static char far chain_follow(struct EvChain far *p, unsigned char k) /* 3d91:1b74 */
{
    unsigned char ev;
    unsigned char count;
    struct Link far *l;
    struct LinkHead link;

    if ((l = chain_cur(&p->chain)) == 0)
        return 0;
    ev = l->ev;
    count = p->chain.pos + (p->chain.last >= 0);
    link.kind = 0;
    link.a = k;
    link.ev = (event_table + ev - 1)->bytes[k];
    while (link.ev && chain_append(&p->chain, (struct Link far *)&link)) {
        count++;
        link.a = 3;
        ev = link.ev;
        link.ev = (event_table + ev - 1)->bytes[3];
    }
    if (link.ev)
        return 0;
    if (ev)
        return chain_append(&p->chain, (struct Link far *)&link);
    return 1;
}

static char far chain_link(struct EvChain far *p)                /* 3d91:1c61 */
{
    unsigned char ev;
    unsigned char kind;
    unsigned char far *rec;
    struct Link far *prv;
    struct Link far *cur;
    int i;

    cur = chain_cur(&p->chain);
    ev = cur->ev;
    kind = cur->kind;
    if ((prv = chain_prev(&p->chain, 0)) != 0) {
        if (kind == 0) {
            rec = (event_table + prv->ev - 1)->bytes;
            rec[cur->a] = ev;
        } else
            return 0;
    } else
        switch (kind) {
        case 1:
            i = cur->a * geo.area->cols + cur->col;
            geo.area->cells[i].event = ev;
            break;
        case 2:
            i = cur->a;
            geo.area->rest_ev[i].ev = ev;
            break;
        case 3:
            i = cur->a;
            geo.area->step_ev[i].ev = ev;
            break;
        case 4:
            i = cur->a;
            geo.area->shared_events[i] = ev;
        }
    return 1;
}

static char far chain_root_clear(struct EvChain far *p)                /* 3d91:1da6 */
{
    char rc;
    unsigned char ev;
    struct Link far *l;

    rc = 0;
    if ((l = chain_first(&p->chain, 0)) != 0) {
        ev = l->ev;
        l->ev = 0;
        rc = chain_link(p);
        l->ev = ev;
    }
    return rc;
}

static void far event_kind_set(unsigned char ev, unsigned char kind) /* 3d91:1dfe */
{
    if (kind > 0)
        (event_table + ev - 1)->bytes[0] = kind;
}

static void far alt_sync(struct EvChain far *p)                /* 3d91:1e26 */
{
    struct Link far *l;

    if (p->alt_keep && p->alt) {
        if (alt_last > 0) {
            if (--p->alt < 1)
                p->alt = alt_last;
            alt_step(p, 0, 1);
        }
        if (alt_last <= 1)
            p->alt = 0;
    } else if (alt_last > 1 && (l = chain_cur(&p->chain)) != 0) {
        for (p->alt = 1; p->alt != alt_last && l->a != alt_fields[p->alt]; p->alt++)
            ;
        if (alt_fields[p->alt] != l->a)
            p->alt = 0;
    } else
        p->alt = 0;
    p->alt_keep = 0;
}

static void far alt_step(struct EvChain far *p, char back, char keep) /* 3d91:1efd */
{
    if (!p->alt)
        return;
    if (!keep && !chain_prev(&p->chain, 1))
        return;
    if (back && --p->alt < 1)
        p->alt = alt_last;
    else if (!back && ++p->alt > alt_last)
        p->alt = 1;
    chain_cut(p);
    chain_follow(p, alt_fields[p->alt]);
    chain_next(&p->chain, 1);
}

static void far alt_reset(unsigned char ev, char shared)         /* 3d91:1fa6 */
{
    alt_root = ev;
    alt_pos = alt_last = 0;
    alt_items[0] = ev;
    if (shared)
        alt_shared |= 1;
    else
        alt_shared &= ~1;
}

void far alt_add(unsigned char ev)                               /* 3d91:1fd1 */
{
    if (ev) {
        if (alt_last < 0)
            alt_root = ev;
        if (alt_last < 99)
            alt_items[++alt_last] = ev;
    } else {
        alt_root = alt_pos = 0;
        alt_last = -1;
    }
}

void far alt_collect(int ofs)                                      /* 3d91:200f */
{
    unsigned char n;

    n = (event_table + alt_root - 1)->bytes[ofs];
    if ((n || (alt_shared & 1)) && alt_last < 99) {
        alt_items[++alt_last] = n;
        if (alt_last < 20)
            alt_fields[alt_last] = ofs & 0xff;
    }
}

static char far delete_confirm(struct EvChain far *p)                /* 3d91:2066 */
{
    unsigned char ev;
    struct Link far *l;
    int n;

    p->flags &= ~0x20;
    if (p->next_ev && (l = chain_cur(&p->chain)) != 0 && (ev = l->ev) != 0
        && p->next_ev != ev)
        (event_table + ev - 1)->bytes[3] = p->next_ev;
    if ((n = alt_last - alt_pos) > 0) {
        if (p->flags & 0x2000) {
            p->flags |= 0x100;
            p->alt_last = alt_last;
            delete_scan(p);
        } else {
            p->cmd = n << 8 | 6;
            p->next = 0xa;
        }
    } else {
        p->cmd = 4;
        p->next = 0xa;
    }
    return 1;
}

static void far delete_scan(struct EvChain far *p)                /* 3d91:212b */
{
    int n;

    if (alt_last - alt_pos > 0 && alt_last < 100) {
        alt_root = alt_items[++alt_pos];
        p->cmd = alt_root | 0x600;
        p->next = 5;
    } else {
        p->flags &= ~0x100;
        n = alt_last;
        alt_last = p->alt_last;
        alt_pos = 0;
        alt_root = alt_items[0];
        p->cmd = n << 8 | 6;
        p->next = 0xa;
    }
}

static char far alt_delete(struct EvChain far *p)                /* 3d91:219b */
{
    char rc;
    unsigned char ev;

    rc = 0;
    if ((p->flags & 4) && alt_pos >= 0 && alt_pos <= alt_last) {
        ev = alt_items[alt_pos];
        if (event_release(ev)) {
            edit_dirty_set();
            if (++alt_pos <= alt_last) {
                alt_root = alt_items[alt_pos];
                p->cmd = alt_root & 0xff | 0x500;
                p->next = 5;
                rc = 1;
            } else
                rc = -1;
        }
    }
    return rc;
}

int far alt_drop(void)                                           /* 3d91:221c */
{
    int r;
    unsigned char ev;

    r = 0;
    if (alt_pos >= 0 && alt_pos <= alt_last) {
        ev = alt_items[alt_pos];
        if (event_release(ev)) {
            edit_dirty_set();
            if (++alt_pos <= alt_last) {
                alt_root = alt_items[alt_pos];
                r = alt_root;
            } else
                r = -1;
        }
    }
    return r;
}
