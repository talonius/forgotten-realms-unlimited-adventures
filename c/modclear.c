/* CKIT.EXE overlay section 23, module 2e1a: the area editor's "Clear
 * module" command.  module_clear is screen 19 of editloop.c's loop, which
 * areaed.c enters from the Utilities menu's "Clear module" entry.  It runs
 * evedit.c's event_edit in mode 2 on a 4-byte record (a flag byte set to 1,
 * then the area's rows and cols), with event kind 0x37 when mapedit.c's
 * area_overland (through 18de:1e63) is nonzero and 0x38 when it is 0, and
 * clears env->bits' low word.  When event_edit returns 1: with the flag
 * still set, it sets editloop.c's flag byte 1 (edit_flag_set, through
 * 18de:1ba7) if the rows or cols changed, keeps the current area number
 * (labels.c's, through 18de:17bf), opens area 0x29 or 0x2a (by the same
 * test) with areafile.c's area_open, puts the kept number back (through
 * 18de:17b5) and into cursor_obj[0x13], and stores the record's rows and
 * cols into the area; with the flag cleared, it resets textbank.c's text
 * pool (txt_init, 0x1c00 bytes) and clears the area's eight text words
 * (texts_clear) and its events (events_clear).  Either way it then sets
 * env->bits' bit 0.  Returns prev, or 0 when arg is null.  events_clear
 * zeroes the event byte of the area's first rows * cols cells, gives the
 * two eight-entry tables at +2e and +4e ticks 1 and kind and chance 0,
 * zeroes the eight bytes at +6e and frees textbank.c's 100 records.
 *
 * Section 23 (parent section 20, which is empty and has no parent; no
 * section names 23 as its parent): payload at EXE 0x078790, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 23.  The module is stored frame 2e1a (load-based 3e1a), EXE
 * 0x079026..0x079268 (578 bytes, 3 routines), from 2e1a:0006, the byte
 * after module 2d91's (areafile.c's) last retf at 2d91:0895 (2e1a:0005),
 * to its own last retf at 2e1a:0247 (2e3e:0007); module 2e3e starts at
 * 2e3e:0008, the target of vector 18de:1c01 (editloop.c's screen 13).  One
 * Turbo C large-model _TEXT segment, built with Turbo C 2.01,
 * tcc -ml -O -Z -d; functions are defined in shipped order.  Definition
 * comments give the load-based address; extern comments the far pointer as
 * stored in the shipped call, or the DGROUP offset.
 *
 * Section 23's relocation table files 8 sites under 2e1a, all in this range
 * (EXE 0x079069..0x07912e), all far calls: txt_init in the resident code,
 * area_open (2d91:041b) in section 23, and six calls through five vector
 * stubs.  module_clear is the target of vector 18de:1bc5 (editloop.c's
 * module_clear, screen 19, called from section 10); no relocated word in
 * any section stores frame 2e1a.  module_clear near-calls texts_clear and
 * events_clear ahead of their definitions, so both are static.  Each stub
 * here reaches a landed routine under another name: 18de:1e63 mapedit.c's
 * area_overland, 18de:1ea9 evedit.c's event_edit, 18de:1ba7 editloop.c's
 * edit_flag_set, 18de:17bf labels.c's cur_area_get, 18de:17b5 labels.c's
 * cur_area_set.
 *
 * FRUA's own code: the sharing map pairs all 3 routines a-only, so every
 * name here is chosen from the code.  The meaning of the record's flag and
 * of event kinds 0x37 and 0x38 lies in evedit.c's "script" library entry,
 * not in these bytes.
 *
 * Forced by the bytes: module_clear's locals declared n, rec, link (link
 * first bakes 2 of 3 ranges: the frame slots reverse); cur_area_set's
 * parameter a byte type, as areaed.c declares it (int: 0 of 3).  Not
 * decided by the bytes: char or unsigned char for n, ClearRec.whole_area and
 * area_overland's return; register or plain rc and texts_clear's i; and
 * the order of events_clear's first loop test (i < rows * cols or
 * rows * cols > i); texts_clear's and events_clear's void (int or char
 * bakes the same); ClearRec.rows/cols char or unsigned char; EdEnv.bits
 * long or unsigned long (this module makes no shift); cur_area_get's
 * return and cur_area_set's parameter as either byte type; each bakes 3
 * of 3.  Area.rows/cols must be unsigned char (char: 2 of 3).
 *
 * No _DATA and no _BSS: the module references DGROUP only at view3d.c's
 * geo.area (d41c), cursor_obj (b75b) and textbank.c's text_pool_block
 * (a7ec) and event_table (a7f0).
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
    int next;                   /* +a                                     */
    int code;                   /* +c                                     */
};

struct ClearRec {               /* the record event_edit edits, 4 bytes   */
    char whole_area;            /* +0   1 on entry                        */
    char pad_1;                 /* +1   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
};

struct Encounter {              /* mapevent.c's, 4 bytes                  */
    unsigned ticks;             /* +0                                     */
    unsigned char kind;         /* +2                                     */
    unsigned char chance;       /* +3                                     */
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
    struct Encounter step_events[8]; /* +4e                                    */
    unsigned char shared_events[8]; /* +6e                                    */
    char pad_76[0x9a];          /* +76  not read here                     */
    int zone_texts[8];          /* +110                                   */
    char pad_120[2];            /* +120 not read here                     */
    struct MapCell cells[0x240]; /* +122                                  */
};

struct Geo {                    /* view3d.c's geo record                  */
    char pad_0[8];              /* +0   not read here                     */
    struct Area far *area;      /* +8   d41c                              */
    char pad_c[2];              /* +c   not read here                     */
};

struct Event {                  /* textbank.c's record, 0x14 bytes        */
    char kind;                  /* +0                                     */
    char pad_1[0x13];           /* +1   not read here                     */
};

extern char far txt_init(char far *pool, int size);              /* 0cf0:02b4 */
extern void far cur_area_set(unsigned char n);
                                /* 18de:17b5, vector to overlay 5 1e8f:0e91 */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern void far edit_flag_set(void);
                                /* 18de:1ba7, vector to overlay 10 267d:0487 */
extern char far area_overland(void);
                                /* 18de:1e63, vector to overlay 16 29f4:378b */
extern int  far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                             void far *rec, int mode, int size);
                                /* 18de:1ea9, vector to overlay 17 29f4:24d4 */
extern void far area_open(int view, int a);                      /* 2d91:041b */

extern char far *text_pool_block;                                /* a7ec */
extern struct Event far *event_table;                               /* a7f0 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Geo geo;                                           /* d414 */

static void far texts_clear(void);
static void far events_clear(void);

int far module_clear(int prev, struct EdEnv far *env, void far *arg) /* 3e1a:0006 */
{
    unsigned char n;
    struct ClearRec rec;
    struct EdLink far *link;
    register int rc;

    if (arg == 0)
        return 0;
    link = arg;
    rec.whole_area = 1;
    rec.rows = geo.area->rows;
    rec.cols = geo.area->cols;
    rc = event_edit(prev, &env->bits, link, area_overland() ? 0x37 : 0x38, &rec, 2, 4);
    env->bits &= 0xffff0000L;
    if (rc == 1) {
        if (rec.whole_area) {
            if (geo.area->rows != rec.rows || geo.area->cols != rec.cols)
                edit_flag_set();
            n = cur_area_get();
            area_open(1, area_overland() ? 0x29 : 0x2a);
            cur_area_set(n);
            cursor_obj[0x13] = n;
            geo.area->rows = rec.rows;
            geo.area->cols = rec.cols;
        } else {
            txt_init(text_pool_block, 0x1c00);
            texts_clear();
            events_clear();
        }
        env->bits |= 1;
    }
    return prev;
}

static void far texts_clear(void)                                /* 3e1a:013b */
{
    register int i;

    for (i = 0; i < 8; i++)
        geo.area->zone_texts[i] = 0;
}

static void far events_clear(void)                               /* 3e1a:0159 */
{
    struct Event far *c;
    register int i;

    for (i = 0; i < geo.area->rows * geo.area->cols; i++)
        geo.area->cells[i].event = 0;
    for (i = 0; i < 8; i++) {
        geo.area->step_events[i].ticks = 1;
        geo.area->step_events[i].kind = 0;
        geo.area->step_events[i].chance = 0;
    }
    for (i = 0; i < 8; i++) {
        geo.area->rest_enc[i].ticks = 1;
        geo.area->rest_enc[i].kind = 0;
        geo.area->rest_enc[i].chance = 0;
    }
    for (i = 0; i < 8; i++)
        geo.area->shared_events[i] = 0;
    c = event_table;
    for (i = 0; i < 100; i++)
        c[i].kind = 0;
}
