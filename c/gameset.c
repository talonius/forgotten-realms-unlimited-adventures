/* CKIT.EXE overlay section 25, module 2f91: the design editor's game
 * settings screen, screen 4 of editloop.c's loop (stub 18de:1c3d, arg
 * editloop.c's a72), the screen edit_main starts on in mode 6.  Coming
 * from screen 1 (edit_main's start) it keeps 1 in arg->prev, and coming
 * back from the event screen (5) it hands event_edit that kept screen;
 * both run evedit.c's event_edit in mode 2, any other entry in mode 3;
 * back from screen 10 it first copies the link's next screen to arg->next.
 * Every entry then runs event_edit on labels.c's game_rec, the GAME
 * entry's 0x184 bytes, as event kind 0x35; a next screen of 6 becomes 5,
 * and when event_edit returns 1 it calls labels.c's design_new (stub
 * 18de:17d3, overlay 5 1e8f:119b), which makes the design directory when
 * there is none and saves the GAME entry.  Then it clears env->bits' low
 * word and fills it for the next screen: for 10 the link's code when
 * link->next is also 10, and arg->ask when it is not -- which cannot happen
 * here: done is never set, so a->next = link->next always runs first; for
 * 8 the code, with env->shift set; for 11 the code; for 5 the code's low
 * byte as the event and its high byte (shifted into link->code in place,
 * signed), an offset into geo.area's record from enc[0].kind, as a table (2, 3 or 4) and an index, as areasize.c's
 * area_settings does.  It returns the next screen, or 0 when arg is null.
 * Its done flag is cleared and tested but never set, so the edit always
 * runs, as shipped (areasize.c's twin sets it on its screen-10 path).
 *
 * Section 25 (parent section 20, which is empty and has no parent; no
 * section names 25 as its parent): payload at EXE 0x07e7c0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 25.  The module is stored frame 2f91 (load-based 3f91),
 * EXE 0x0807cd..0x080a30 (611 bytes, one routine), 3f91:000d..026f: from
 * byte 0xd of the paragraph -- 2f91:0000..000c is the tail of module 2f17
 * (areasize.c), whose last retf is 2f91:000c (EXE 0x0807cc) -- to its
 * retf at 2f91:026f, the section's last initialized byte.  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Definition comments give the load-based address; extern comments the
 * far pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 25's relocation table files 2 sites under 2f91, both in this
 * range (EXE 0x08085d and 0x08088c), both far calls to vector stubs.
 * game_settings is the target of vector 18de:1c3d (editloop.c's
 * game_settings, called once, from section 10); no relocated word in any
 * section stores 2f91 (control: three store 2e62, monedit.c's).  It calls
 * sections 5 and 17 only through vector stubs: 18de:1ea9 is evedit.c's
 * event_edit and 18de:17d3, which no landed file names, labels.c's
 * design_new.
 *
 * FRUA's own code: the sharing map pairs the routine a-only, so its name
 * is chosen from the code.  No _DATA and no _BSS: the only DGROUP operands
 * in the range are view3d.c's geo.area (d41c..d41f) and labels.c's
 * game_rec (ba08, pushed as a far pointer with ds).
 *
 * Forced by the bytes, each against its alternative: mode short (int: 0
 * of 1 range; an int mode takes di, where the shipped code keeps prev);
 * i an int, register or not (short: 0 of 1); the locals' declaration
 * order (reversed: 0 of 1); the second switch's case 5 after cases 10
 * and 8 (first: 0 of 1).  Not decided, each 1 of 1: rc int or short;
 * done char or unsigned char; "arg == 0" or "!arg"; EdEnv.bits long or
 * unsigned long; case 8 falling into case 11 or each written out, and
 * case 11 before case 8.
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0                                     */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct EdLink {                 /* evedit.c's, event_edit's caller state;
                                   here editloop.c's EdMain.link          */
    int kind;                   /* +0                                     */
    int page;                   /* +2                                     */
    int field;                  /* +4                                     */
    int back;                   /* +6                                     */
    int ret;                    /* +8                                     */
    int next;                   /* +a                                     */
    int code;                   /* +c                                     */
};

struct GameArg {                /* editloop.c's a72, 0xa bytes            */
    int prev;                   /* +0   the screen before (1), kept for
                                        the return from screen 5          */
    int next;                   /* +2   the next screen                   */
    int ask;                    /* +4   read only on a dead path here;
                                        meaning open                      */
    struct EdLink far *link;    /* +6   ed->buf                           */
};

struct Encounter {              /* mapevent.c's, 4 bytes                  */
    unsigned ticks;             /* +0   not read here                     */
    unsigned char event;        /* +2                                     */
    unsigned char chance;       /* +3   not read here                     */
};

struct Area {                   /* mapevent.c's layout; only &enc[0].kind
                                   - area (0x30) is computed here        */
    unsigned version;           /* +0   not read here                     */
    unsigned char rows;         /* +2   not read here                     */
    unsigned char cols;         /* +3   not read here                     */
    char pad_4[0x2a];           /* +4   not read here                     */
    struct Encounter rest_enc[8]; /* +2e                                    */
};

struct Geo {                    /* view3d.c's geo record                  */
    char pad_0[8];              /* +0   not read here                     */
    struct Area far *area;      /* +8   d41c                              */
    char pad_c[2];              /* +c   not read here                     */
};

extern void far design_new(void);
                                /* 18de:17d3, vector to overlay 5 1e8f:119b */
extern int  far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                             void far *rec, int mode, int size);
                                /* 18de:1ea9, vector to overlay 17 29f4:24d4 */

extern char game_rec[];                                          /* ba08 */
extern struct Geo geo;                                           /* d414 */

int far game_settings(int prev, struct EdEnv far *env, struct GameArg far *arg) /* 3f91:000d */
{
    char done;
    int rc;
    short mode;
    struct GameArg far *a;
    struct EdLink far *link;
    register int i;

    if (arg == 0)
        return 0;
    a = arg;
    link = a->link;
    done = 0;
    mode = 3;
    switch (prev) {
    case 1:
        a->prev = prev;
        mode = 2;
        break;
    case 5:
        prev = a->prev;
        mode = 2;
        break;
    case 10:
        a->next = link->next;
        break;
    }
    if (!done) {
        rc = event_edit(prev, &env->bits, link, 0x35, game_rec, mode, 0x184);
        if (link->next == 6)
            link->next = 5;
        a->next = link->next;
        if (rc == 1)
            design_new();
    }
    env->bits &= 0xffff0000L;
    switch (a->next) {
    case 10:
        if (a->next == link->next)
            env->bits |= link->code;
        else
            env->bits |= a->ask;
        break;
    case 8:
        env->shift = 1;
    case 11:
        env->bits |= link->code;
        break;
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
    }
    return a->next;
}
