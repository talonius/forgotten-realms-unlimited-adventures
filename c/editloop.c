/* CKIT.EXE overlay 10 module 267d: the design editor's screen loop.
 * edit_main(mode) clears a state record on its stack, points ed_flags at
 * its two flag bytes, picks the first screen for the mode (6, 7, 8 or 9;
 * any other mode runs nothing) and hands the record to edit_loop.  Each
 * screen is a routine in another section, reached through its vector
 * stub; it is called with the previous screen, the record's shared env
 * and a per-screen argument, and returns the next screen.  A screen
 * number outside 2..21 ends the loop (after storing next, which that
 * path never sets, into state, as shipped).  The six routines after it set,
 * clear and read the two flag bytes for the screens.
 *
 * EXE 0x04efe2..0x04f484, one Turbo C large-model _TEXT segment starting
 * at 267d:0002 in RTLink section 10 (payload 0x4a550, link segment 21d4,
 * parent section 3), after temple.c's final retf at 267d:0001.  Its own
 * final retf is 267d:04a3; twelve zero bytes follow to the section's end
 * (0x04f490).  Section 10's table files all 23 fixup sites in the range
 * under frame 267d (0x04eff9..0x04f41f).  Built with Turbo C 2.01,
 * tcc -ml -O -Z -d.  Functions are defined in shipped order; bake consumes
 * them in that order.
 *
 * A bare stored seg:off in this file means section 10.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call; a
 * definition's comment gives the load-based address, as the resident
 * modules' do.  A mod18de_ callee is an RTLink vector stub; its comment
 * names the routine the stub reaches.  Nothing in section 10 far-calls
 * this module.  Code outside the section reaches it only through seven
 * vector stubs: 18de:1bbb -> edit_main (section 26, twice),
 * 18de:1bb1 -> 046d (13 calls from sections 16, 17, 21 and 22),
 * 18de:1b9d -> 0476 (section 23), 18de:1b7f -> 047f (sections 21 to 23),
 * 18de:1ba7 -> 0487 (sections 23 and 25), 18de:1b93 -> 0491 and
 * 18de:1b89 -> 049b (sections 21 and 25).  edit_main near-calls
 * edit_loop ahead of its definition, so edit_loop is static.
 *
 * FRUA's own code: the sharing map pairs none of the 8 routines with DQK
 * (all a-only), so every name here is chosen from the code.  The flag
 * names are readings of a few call sites: flag byte 0 is set after the
 * screens change a record and cleared by section 23 right after its two
 * calls to stub 18de:1783 (overlay 5 1e8f:134f); flag byte 1 is set in
 * sections 23 and 25 when a value differs from the one stored, and tested
 * and cleared in sections 21 and 25.  Struct EdState's layout past +0xd
 * is only where the code addresses it; the sizes of its parts are the gaps
 * between those offsets, and setmem's count, 0x16d, is its size.
 *
 * No _DATA.  _BSS is ed_flags, the far pointer at DGROUP d212..d215;
 * nothing else in the image references it.  shop.c's shop_redraw is
 * d20e; no instruction in the image references d20f..d211; spellfx.c's
 * fx_pool starts at d216.
 */

struct EdFlags {
    char dirty;                 /* +0  set after edits                    */
    char flag;                  /* +1  set when a value changes           */
};

struct EdEnv {                  /* passed to every screen                 */
    long bits;                  /* +0  high word from labels.c's 0e8d     */
    char shift;                 /* +4  mode 8: shift held at entry        */
    int sel;                    /* +5  mode 8: -1                         */
};

struct EdLink {                 /* the caller's state, handed to a screen */
    int kind;                   /* +0                                     */
    int page;                   /* +2                                     */
    int field;                  /* +4                                     */
    int back;                   /* +6                                     */
    int ret;                    /* +8                                     */
    int next;                   /* +a   the screen to go to               */
    int code;                   /* +c   its argument                      */
};

struct EvArg {
    char pad_0[3];              /* +0   not read here                     */
    struct EdLink far *link;    /* +3                                     */
};

struct SizeArg {
    char pad_0[8];              /* +0   not read here                     */
    struct EdLink far *link;    /* +8                                     */
};

struct GameArg {
    char pad_0[6];              /* +0   not read here                     */
    struct EdLink far *link;    /* +6                                     */
};

struct MonArg {
    char pad_0[5];              /* +0   not read here                     */
    void far *buf;              /* +5                                     */
    char pad_9[4];              /* +9   not read here                     */
};

struct ImportArg {
    char pad_0[6];              /* +0   not read here                     */
    struct EdLink far *link;    /* +6                                     */
};

struct EdMain {                 /* edit_main's record, 0x16d bytes        */
    char done;                  /* +0   ends the loop                     */
    int prev;                   /* +1   the screen before                 */
    int state;                  /* +3   the screen to run                 */
    struct EdFlags flags;       /* +5   ed_flags points here              */
    struct EdEnv env;           /* +7                                     */
    char settings[0x51];        /* +e   screens 2, 13, 14, 15 and 20      */
    struct EvArg event_arg;     /* +5f  screen 5                          */
    struct SizeArg size_arg;    /* +66  screen 3                          */
    struct GameArg game_arg;    /* +72  screen 4                          */
    struct MonArg monster_arg;  /* +7c  screens 7 and 21                  */
    struct ImportArg import_arg; /* +89  screen 16                         */
    char art_arg[0x12];         /* +93  screens 8 and 18; 21's buf        */
    char file_arg[6];           /* +a5  screen 9                          */
    char chain_arg[0xb4];       /* +ab  screen 6                          */
    struct EdLink link;         /* +15f screen 19; the bufs above         */
};

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern char far shift_key_down(void);
                                /* 18de:1823, vector to overlay 5 1e8f:000e */
extern int far module_clear(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1bc5, vector to overlay 23 2e1a:0006 */
extern int far rename_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1bcf, vector to overlay 18 29f4:2bbc */
extern int far print_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1bd9, vector to overlay 25 2d91:0000 */
extern int far import_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1be3, vector to overlay 24 2d91:0000 */
extern int far marker_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1bed, vector to overlay 23 2e3e:0729 */
extern int far entry_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1bf7, vector to overlay 23 2e3e:0bdc */
extern int far access_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c01, vector to overlay 23 2e3e:0008 */
extern int far wall_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c0b, vector to overlay 26 2e34:0f71 */
extern int far pick_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c15, vector to overlay 26 2e34:02e8 */
extern int far ask_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c1f, vector to overlay 26 2e34:000e */
extern int far file_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c29, vector to overlay 23 2d91:0000 */
extern int far monster_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c33, vector to overlay 25 2e62:0000 */
extern int far game_settings(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c3d, vector to overlay 25 2f91:000d */
extern int far area_settings(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c47, vector to overlay 25 2f17:000c */
extern int far art_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c51, vector to overlay 18 29f4:0000 */
extern int far chain_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c5b, vector to overlay 22 2d91:0000 */
extern int far event_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c65, vector to overlay 22 2fb8:000b */
extern int far area_edit(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c6f, vector to overlay 21 2d91:0000 */
extern int far monster_pick(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c79, vector to overlay 25 2e62:0891 */
extern int far replace_screen(int prev, struct EdEnv far *env, void far *arg);
                                /* 18de:1c83, vector to overlay 23 2e3e:0f6a */

static void far edit_loop(struct EdMain far *ed);

static struct EdFlags far *ed_flags;                             /* d212 */

void far edit_main(int mode)                                     /* 367d:0002 */
{
    struct EdMain ed;

    mem_fill(&ed, sizeof ed, 0);
    ed_flags = &ed.flags;
    ed.prev = 1;
    switch (mode) {
    case 6:
        ed.state = 4;
        break;
    case 7:
        ed.state = 2;
        break;
    case 8:
        ed.env.shift = shift_key_down();
        ed.env.sel = -1;
        ed.state = 8;
        break;
    case 9:
        ed.state = 7;
        break;
    default:
        ed.done = 1;
    }
    ed.env.bits = ((unsigned long)cur_area_get() << 16) | ed.env.bits;
    if (!ed.done)
        edit_loop(&ed);
}

static void far edit_loop(struct EdMain far *ed)                 /* 367d:00b8 */
{
    int next;

    while (!ed->done) {
        switch (ed->state) {
        case 2:
            next = area_edit(ed->prev, &ed->env, ed->settings);
            break;
        case 5:
            ed->event_arg.link = &ed->link;
            next = event_screen(ed->prev, &ed->env, &ed->event_arg);
            break;
        case 6:
            next = chain_screen(ed->prev, &ed->env, ed->chain_arg);
            break;
        case 8:
            next = art_screen(ed->prev, &ed->env, ed->art_arg);
            break;
        case 3:
            ed->size_arg.link = &ed->link;
            next = area_settings(ed->prev, &ed->env, &ed->size_arg);
            break;
        case 4:
            ed->game_arg.link = &ed->link;
            next = game_settings(ed->prev, &ed->env, &ed->game_arg);
            break;
        case 7:
            ed->monster_arg.buf = &ed->link;
            next = monster_screen(ed->prev, &ed->env, &ed->monster_arg);
            break;
        case 9:
            next = file_screen(ed->prev, &ed->env, ed->file_arg);
            break;
        case 10:
            next = ask_screen(ed->prev, &ed->env, 0);
            break;
        case 11:
            next = pick_screen(ed->prev, &ed->env, 0);
            break;
        case 12:
            next = wall_screen(ed->prev, &ed->env, 0);
            break;
        case 13:
            next = access_screen(ed->prev, &ed->env, ed->settings);
            break;
        case 14:
            next = entry_screen(ed->prev, &ed->env, ed->settings);
            break;
        case 15:
            next = marker_screen(ed->prev, &ed->env, ed->settings);
            break;
        case 16:
            ed->import_arg.link = &ed->link;
            next = import_screen(ed->prev, &ed->env, &ed->import_arg);
            break;
        case 17:
            next = print_screen(ed->prev, &ed->env, 0);
            break;
        case 18:
            next = rename_screen(ed->prev, &ed->env, ed->art_arg);
            break;
        case 19:
            next = module_clear(ed->prev, &ed->env, &ed->link);
            break;
        case 20:
            next = replace_screen(ed->prev, &ed->env, ed->settings);
            break;
        case 21:
            ed->monster_arg.buf = ed->art_arg;
            next = monster_pick(ed->prev, &ed->env, &ed->monster_arg);
            break;
        default:
            ed->done = 1;
        }
        ed->prev = ed->state;
        ed->state = next;
    }
}

void far edit_dirty_set(void)                                    /* 367d:046d */
{
    ed_flags->dirty = 1;
}

void far edit_dirty_clear(void)                                  /* 367d:0476 */
{
    ed_flags->dirty = 0;
}

char far edit_dirty(void)                                        /* 367d:047f */
{
    return ed_flags->dirty;
}

void far edit_flag_set(void)                                     /* 367d:0487 */
{
    ed_flags->flag = 1;
}

void far edit_flag_clear(void)                                   /* 367d:0491 */
{
    ed_flags->flag = 0;
}

char far edit_flag(void)                                         /* 367d:049b */
{
    return ed_flags->flag;
}
