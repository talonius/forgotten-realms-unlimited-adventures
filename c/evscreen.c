/* CKIT.EXE overlay section 22, module 2fb8: the design editor's event
 * screen, screen 5 of editloop.c's loop.  event_screen is called with the
 * screen the editor comes back from, editloop.c's env and the screen's
 * record (struct EvArg, editloop.c's a5f, whose link points at the state
 * record's buf).  Coming from screen 2, 3, 4 or 7 it does not edit: it
 * keeps that screen and the event (env->bits' low byte), takes env->bits'
 * low 14 bits with bit 15 set as the value to hand on, and goes to screen
 * 6 (evchain.c's chain_screen).  Coming back from screen 6 it reads the
 * event and a mode from env->bits; with no event and a mode of 0x3e or
 * more it returns to the screen it kept.  Otherwise it runs evedit.c's
 * event_edit on the event's record (labels.c's cell_at and cell_used, the
 * record's 0x14 bytes and its kind), sets editloop.c's dirty flag when
 * env->bits' bit 14 asks for it and the flag was clear, and leaves in
 * env->bits what the next screen (link->next) is handed.  Returns that
 * screen.  mode and link->code are ints, so each OR into env->bits
 * sign-extends them (cwd): the bit-15 value handed to screen 6 also sets
 * env->bits' high word to 0xffff, as shipped.
 *
 * Section 22 (parent section 20, which is empty and has no parent; no
 * section names 22 as its parent): payload at EXE 0x075ef0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 22.  The module is stored frame 2fb8 (load-based 3fb8),
 * EXE 0x07816b..0x0783f4 (649 bytes, one routine), from byte 0xb of the
 * paragraph -- 2fb8:0000..000a is the tail of evchain.c (module 2d91),
 * whose last retf is 2d91:227a (EXE 0x07816a) -- to its retf at 2fb8:0293;
 * twelve zero bytes follow to the section's end (EXE 0x078400).  One
 * Turbo C large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O
 * -Z -d.  Definition comments give the load-based address; extern comments
 * the far pointer as stored in the shipped call.
 *
 * Section 22's relocation table files 7 sites under 2fb8, all in this
 * range (EXE 0x078213..0x0783d1), all far calls: LXRSH@ and 00d7:2625 in
 * the resident runtime, and five vector stubs.  event_screen is the target
 * of vector 18de:1c65 (editloop.c's event_screen, screen 5, called once
 * from section 10); no far call in any section stores frame 2fb8.  The
 * module calls sections 5, 10 and 17 only through vector stubs; each stub
 * here reaches a landed routine under another name: 18de:182d labels.c's
 * cell_at, 18de:1837 labels.c's cell_used, 18de:1b7f editloop.c's
 * edit_dirty, 18de:1bb1 editloop.c's edit_dirty_set, 18de:1ea9 evedit.c's
 * event_edit.
 *
 * FRUA's own code: the sharing map pairs the routine a-only, so its name
 * is chosen from the code.  No _DATA and no _BSS: the routine references
 * no DGROUP address.
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0                                     */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct EdLink {                 /* evedit.c's, the caller's state; here
                                   editloop.c's EdMain.link               */
    int kind;                   /* +0                                     */
    int page;                   /* +2                                     */
    int field;                  /* +4                                     */
    int back;                   /* +6                                     */
    int ret;                    /* +8                                     */
    int next;                   /* +a   the screen to go to               */
    int code;                   /* +c   its argument                      */
};

struct EvArg {                  /* the screen's argument, editloop.c's a5f */
    int from;                   /* +0   the screen that sent us to 6      */
    unsigned char ev;           /* +2   the event, a textbank.c record    */
    struct EdLink far *link;    /* +3                                     */
};

struct Event {                  /* textbank.c's event record, 0x14 bytes  */
    char kind;                  /* +0   its kind, 0 free                  */
    char pad_1[0x13];           /* +1   not read here                     */
};

extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern struct Event far *far cell_at(unsigned char n);
                                /* 18de:182d, vector to overlay 5 1e8f:0e15 */
extern unsigned char far cell_used(unsigned char n);
                                /* 18de:1837, vector to overlay 5 1e8f:0df0 */
extern char far edit_dirty(void);
                                /* 18de:1b7f, vector to overlay 10 267d:047f */
extern void far edit_dirty_set(void);
                                /* 18de:1bb1, vector to overlay 10 267d:046d */
extern int  far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                             void far *rec, int mode, int size);
                                /* 18de:1ea9, vector to overlay 17 29f4:24d4 */

int far event_screen(int prev, struct EdEnv far *env, struct EvArg far *arg) /* 3fb8:000b */
{
    char dirty;
    int rc;
    struct EvArg far *p;
    struct EdLink far *link;
    int mode;
    int skip;

    if (arg == 0)
        return 0;
    p = arg;
    link = p->link;
    skip = 0;
    mode = 3;
    switch (prev) {
    case 2:
    case 3:
    case 4:
    case 7:
        skip = 1;
        mode = (int)env->bits & 0x3fff | 0x8000;
        p->from = prev;
        p->ev = env->bits & 0xff;
        link->next = 6;
        break;
    case 6:
        p->ev = env->bits & 0xff;
        mode = (env->bits & 0x3f00) >> 8;
        if (p->ev == 0 && mode >= 0x3e) {
            skip = 1;
            link->next = p->from;
        }
        break;
    }
    if (!skip) {
        dirty = edit_dirty();
        env->bits &= 0xffffbfffL;
        rc = event_edit(prev, &env->bits, link, cell_used(p->ev), cell_at(p->ev),
                          mode, 0x14);
        if (!dirty && (env->bits & 0x4000))
            edit_dirty_set();
    }
    env->bits &= 0xffff0000L;
    switch (link->next) {
    case 8:
        env->shift = 1;
    case 10:
    case 11:
        env->bits |= link->code;
        break;
    case 6:
        if (!skip) {
            if (rc == 3)
                env->bits |= link->code & 0x3fff;
            else {
                env->bits &= 0xffffc0ffL;
                if (rc == 1)
                    env->bits |= 0x100;
            }
        } else
            env->bits |= mode;
        break;
    case 2:
    case 3:
    case 4:
    case 7:
        env->bits |= int_max(0, mode - 0x3e);
    }
    return link->next;
}
