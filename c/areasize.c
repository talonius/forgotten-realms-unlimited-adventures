/* CKIT.EXE overlay section 25, module 2f17: the area editor's area settings
 * screen and resizing the map.  area_settings is screen 3 of editloop.c's
 * loop (stub 18de:1c47, arg editloop.c's a66).  Coming from screen 2 it
 * keeps 2 in arg->prev, and coming back from the event screen (5) it hands
 * event_edit that kept screen; both run evedit.c's event_edit in mode 2,
 * any other entry in mode 3.  It keeps the area's rows and cols in arg and
 * runs event_edit on the area's first 0x122 bytes (geo.area itself), event
 * kind 0x34 when mapedit.c's area_overland (through 18de:1e63) is nonzero
 * and 0x33 when it is 0; a next screen of 6 becomes 5.  When the rows or
 * cols changed it sets editloop.c's flag byte 1 (edit_flag_set, 18de:1ba7).
 * When event_edit returns 1, or 3 with next screen 5 (or 6), and flag byte 1 is
 * set (edit_flag, 18de:1b89): a size smaller in rows or cols sends the
 * editor to screen 10 with arg->ask 5; any other change resizes the map at
 * once.  Back from screen 10 with ask 5 it skips event_edit: with a nonzero
 * low nibble in env->bits it resizes the map and, when events fell outside
 * it, runs lost_events; otherwise it puts the kept rows and cols back and
 * clears flag byte 1 (edit_flag_clear, 18de:1b93).  Then it clears
 * env->bits' low word and fills it for the next screen: for 10 the link's
 * code when link->next is also 10, and arg->ask when it is not; for 8 the
 * code, with env->shift set; for 5 the code's low byte as the
 * event and its high byte (shifted into link->code in place, signed), an
 * offset into the area record from
 * enc[0].kind, as a table (2, 3 or 4) and an index, as monedit.c's
 * monster_screen does.  It returns the next screen, or 0 when arg is null.
 *
 * area_resize(area, rows, cols) takes the size before the change (the area
 * already holds the new one) and returns 0 at once when the rows did not
 * shrink and the cols did not change.  Otherwise it resets evchain.c's
 * alternatives list (alt_add(0), through 18de:1ffd), adds to it every
 * nonzero event byte of the cells that fall outside the new size (the
 * dropped rows, then the dropped columns of the kept rows) and counts them;
 * it moves the kept rows from the old row width to the new one: forward
 * when narrower, then zeroing from the kept rows' end in the new layout to
 * the old map's end; when wider, first zeroing from the kept rows' end in
 * the old layout to the old map's end, then backward from the last kept
 * row, zeroing the d bytes before each moved row; with the width unchanged
 * it zeroes from the new map's end to the old one's.  It returns the
 * count.  It reads the sizes through its parameter and the cells through
 * geo.area; both callers pass geo.area.
 * lost_events(prev, env, link) saves *link, runs event_edit (mode 5, size
 * 0x14) once per positive result of evchain.c's alt_drop (18de:1ff3), with
 * the textbank record labels.c's cell_at and cell_used (18de:182d, 1837)
 * give for ev, then restores *link.  As shipped the comparison binds before
 * the assignment ("ev = alt_drop() > 0"), so ev is 1 on every pass and
 * every pass edits cell_at(1), the first of textbank.c's records; the loop
 * ends with ev 0, so the routine returns 0, which area_settings does not
 * read.
 *
 * Section 25 (parent section 20, which is empty and has no parent; no
 * section names 25 as its parent): payload at EXE 0x07e7c0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 25.  The module is stored frame 2f17 (load-based 3f17),
 * EXE 0x08002c..0x0807cd (1,953 bytes, 3 routines), 3f17:000c..07ac: from
 * the byte after monedit.c's last retf (2f17:000b) to its own last retf at
 * 2f17:07ac; module 2f91 (one routine, to the section's end at 0x080a30)
 * starts at 2f91:000d, the target of vector 18de:1c3d.  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.
 *
 * Section 25's relocation table files 27 sites under 2f17, all in this
 * range (EXE 0x080110..0x0807ba), all far calls: 15 to the runtime in
 * frame 10d7 and 12 to vector stubs.  No relocated word in any section
 * stores 2f17 (control: three store 2e62, monedit.c's).  area_settings is
 * reached only through vector 18de:1c47, editloop.c's area_settings; it
 * near-calls area_resize and lost_events ahead of their definitions, so
 * both are static (public, each bakes 0 of 3 ranges).  The near calls tie
 * the three routines into one object.
 *
 * FRUA's own code: the sharing map pairs all 3 routines a-only, so every
 * name here is chosen from the code.  The stubs keep the names landed
 * files give them; 18de:1ffd and 18de:1ff3, which no landed file names,
 * are evchain.c's alt_add and alt_drop.
 *
 * No _DATA and no _BSS: the only DGROUP operands in the range are view3d.c's
 * geo.area (d41c..d41f).
 *
 * Forced by the bytes, each against its alternative: the assignment inside
 * area_resize's last test (a statement of its own: 0 of 3) and inside its
 * two event tests (separate: 0 of 3); "ev = alt_drop() > 0" (with the
 * parentheses the code means: 2 of 3); switch (a->ask) with one case (an
 * if: 0 of 3); the second switch's cases in the order 10, 8, 5 (5 first:
 * 0 of 3); "k > 0" (k != 0: 2 of 3); k unsigned char (char: 0 of 3);
 * Area.rows/cols unsigned char (char: 0 of 3); area_resize's rows and cols
 * unsigned char (char: 0 of 3); alt_add's byte parameter (int: 0 of
 * 3); cell_used returning unsigned char (char: 2 of 3);
 * "(cols - area->cols) * sizeof" (-(area->cols - cols): 0 of 3);
 * lost_events returning int (void: 2 of 3; "return ev;" 2 of 3); each
 * routine's locals' declaration order (reversed: 2 of 3 each).  Not decided,
 * each 3 of 3: ev short or int; d and w register or plain, and their order;
 * "ev ? 1 : 0" or "ev != 0"; rc and mode int or short; i register or plain;
 * case 2 written out or as a goto into case 5's tail; the rc test as one
 * condition or nested ifs; n and area_resize's return char or unsigned
 * char; SizeArg.rows/cols char or unsigned char; EdEnv.bits long or
 * unsigned long; cell_at's parameter unsigned char or int.
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

struct SizeArg {                /* editloop.c's a66, 0xc bytes            */
    int prev;                   /* +0   the screen before (2), kept for
                                        the return from screen 5          */
    int next;                   /* +2   the next screen                   */
    int ask;                    /* +4   5: screen 10 asks about a smaller
                                        size                              */
    unsigned char rows;         /* +6   the area's size before the edit   */
    unsigned char cols;         /* +7                                     */
    struct EdLink far *link;    /* +8   ed->buf                           */
};

struct Encounter {              /* mapevent.c's, 4 bytes                  */
    unsigned ticks;             /* +0   not read here                     */
    unsigned char event;        /* +2                                     */
    unsigned char chance;       /* +3   not read here                     */
};

struct MapCell {                /* view3d.c's square of the area, 6 bytes */
    unsigned char wall[4];      /* +0   not read here                     */
    unsigned char event;        /* +4                                     */
    unsigned char bits;         /* +5   not read here                     */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[0x2a];           /* +4   not read here                     */
    struct Encounter rest_enc[8]; /* +2e                                    */
    char pad_4e[0xd4];          /* +4e  not read here                     */
    struct MapCell cells[0x240]; /* +122                                  */
};

struct Geo {                    /* view3d.c's geo record                  */
    char pad_0[8];              /* +0   not read here                     */
    struct Area far *area;      /* +8   d41c                              */
    char pad_c[2];              /* +c   not read here                     */
};

struct Event;                    /* textbank.c's record; only passed on    */

extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern struct Event far *far cell_at(unsigned char n);
                                /* 18de:182d, vector to overlay 5 1e8f:0e15 */
extern unsigned char far cell_used(unsigned char n);
                                /* 18de:1837, vector to overlay 5 1e8f:0df0 */
extern char far edit_flag(void);
                                /* 18de:1b89, vector to overlay 10 267d:049b */
extern void far edit_flag_clear(void);
                                /* 18de:1b93, vector to overlay 10 267d:0491 */
extern void far edit_flag_set(void);
                                /* 18de:1ba7, vector to overlay 10 267d:0487 */
extern char far area_overland(void);
                                /* 18de:1e63, vector to overlay 16 29f4:378b */
extern int  far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                             void far *rec, int mode, int size);
                                /* 18de:1ea9, vector to overlay 17 29f4:24d4 */
extern int  far alt_drop(void);
                                /* 18de:1ff3, vector to overlay 22 2d91:221c */
extern void far alt_add(unsigned char ev);
                                /* 18de:1ffd, vector to overlay 22 2d91:1fd1 */

extern struct Geo geo;                                           /* d414 */

static char far area_resize(struct Area far *area, unsigned char rows, unsigned char cols);
static int far lost_events(int prev, struct EdEnv far *env, struct EdLink far *link);

int far area_settings(int prev, struct EdEnv far *env, struct SizeArg far *arg) /* 3f17:000c */
{
    char done;
    int rc;
    int mode;
    struct SizeArg far *a;
    struct EdLink far *link;
    register int i;

    if (!arg)
        return 0;
    a = arg;
    link = a->link;
    done = 0;
    mode = 3;
    switch (prev) {
    case 2:
        a->prev = prev;
        mode = 2;
        break;
    case 5:
        prev = a->prev;
        mode = 2;
        break;
    case 10:
        a->next = link->next;
        switch (a->ask) {
        case 5:
            if (env->bits & 0xf) {
                if (area_resize(geo.area, a->rows, a->cols))
                    lost_events(prev, env, link);
            } else {
                geo.area->rows = a->rows;
                geo.area->cols = a->cols;
                edit_flag_clear();
            }
            done = 1;
            a->next = link->next;
        }
        break;
    }
    if (!done) {
        a->rows = geo.area->rows;
        a->cols = geo.area->cols;
        rc = event_edit(prev, &env->bits, link, area_overland() ? 0x34 : 0x33, geo.area, mode,
                          0x122);
        if (link->next == 6)
            link->next = 5;
        a->next = link->next;
        if (geo.area->rows != a->rows || geo.area->cols != a->cols)
            edit_flag_set();
        if ((rc == 1 || rc == 3 && link->next == 5) && edit_flag()) {
            if (geo.area->rows < a->rows || geo.area->cols < a->cols) {
                a->next = 10;
                a->ask = 5;
            } else
                area_resize(geo.area, a->rows, a->cols);
        }
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

static char far area_resize(struct Area far *area, unsigned char rows, unsigned char cols) /* 3f17:03ac */
{
    unsigned char k;
    unsigned char j;
    unsigned char lim;
    struct MapCell far *from;
    struct MapCell far *to;
    struct MapCell far *end;
    unsigned char ev;
    char n;
    int d;
    int w;

    if (area->rows >= rows && area->cols == cols)
        return 0;
    n = 0;
    alt_add(0);
    for (k = area->rows; k < rows; k++)
        for (j = 0; j < cols; j++)
            if ((ev = geo.area->cells[k * cols + j].event) != 0) {
                alt_add(ev);
                n++;
            }
    lim = int_min(rows, area->rows);
    for (j = area->cols; j < cols; j++)
        for (k = 0; k < lim; k++)
            if ((ev = geo.area->cells[k * cols + j].event) != 0) {
                alt_add(ev);
                n++;
            }
    d = (cols - area->cols) * sizeof(struct MapCell);
    end = &geo.area->cells[rows * cols];
    if (d > 0) {
        from = &geo.area->cells[cols];
        to = &geo.area->cells[area->cols];
        w = area->cols * sizeof(struct MapCell);
        for (k = 1; k < lim; k++) {
            mem_move(from, to, w);
            from += cols;
            to += area->cols;
        }
        mem_fill(to, (end - to) * sizeof(struct MapCell), 0);
    } else if (d < 0) {
        d = -d;
        from = &geo.area->cells[lim * cols];
        mem_fill(from, (end - from) * sizeof(struct MapCell), 0);
        from -= cols;
        to = &geo.area->cells[area->cols * (lim - 1)];
        w = cols * sizeof(struct MapCell);
        for (k = lim - 1; k > 0; k--) {
            mem_move(from, to, w);
            mem_fill((char far *)to - d, d, 0);
            from -= cols;
            to -= area->cols;
        }
    } else {
        if ((from = &geo.area->cells[area->rows * area->cols]) < end)
            mem_fill(from, (end - from) * sizeof(struct MapCell), 0);
    }
    return n;
}

static int far lost_events(int prev, struct EdEnv far *env, struct EdLink far *link) /* 3f17:0723 */
{
    int ev;
    struct EdLink save;

    save = *link;
    while (ev = alt_drop() > 0)
        event_edit(prev, &env->bits, link, cell_used(ev), cell_at(ev), 5, 0x14);
    *link = save;
    return ev ? 1 : 0;
}
