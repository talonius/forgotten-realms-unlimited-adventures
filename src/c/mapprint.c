/* CKIT.EXE overlay section 25, frame 2d91, first object: printing the
 * current area.  print_screen is screen 17 of editloop.c's loop (stub
 * 18de:1bd9).  It hands labels.c's current area number (through 18de:17bf)
 * to print_area, clears env->bits' low word and leaves print_area's answer
 * (always 1) there, and returns prev.  print_area shows "Printing . . .",
 * opens a print job titled with the area's label and name (prnout.c),
 * prints the title between two "%(-%)" lines and ejects the page, then
 * prints the map in blocks of up to 24 by 24 squares, one block a page:
 * print_block writes a row of column numbers, then for each row a line of
 * corners and horizontal walls and a line of vertical walls and event tags,
 * and a closing line of corners and horizontal walls.  corner_char gives '+' when any of the
 * eight wall sides that meet at a corner is nonzero (view3d.c's map_wall,
 * through 18de:1d05); hwall_char and vwall_char give ' ', '.' or '-'
 * and ' ', ':' or '|' for map_wall's 0, 1 or 2.  Each square with an event
 * (view3d.c's area_cell_at, through 18de:15ad) gets the next tag, 1 to 99
 * and then a letter and a digit, and its event is appended to print_area's
 * 200-byte list (no bound is checked).  After the map print_area prints
 * each listed event, then the area's eight step events (+4e) and eight
 * rest-in-zone events (+2e) that are set.  print_event prints one event as
 * "label - kind name", indented by its depth, and runs evedit.c's
 * event_edit in mode 7 on its record: event_edit then calls back link_add
 * (18de:1ef9) with each of the record's chain links, which link_add keeps,
 * when its event is set, in the Links record links points at, and
 * text_print (18de:1eef) with each of its text numbers, which text_print
 * prints from textbank.c's pool word-wrapped at 38 characters under the
 * event.  print_event then prints each chained event the same way, one
 * level deeper (at most 18), labelled with labels.c's chain_label and
 * " Chain".
 *
 * Section 25 (parent section 20, which is empty and has no parent; no
 * section names 25 as its parent): payload at EXE 0x07e7c0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 25.  This object is EXE 0x07e7c0..0x07f1ba (2,554 bytes,
 * 9 routines), from the section's first byte to the retf at 2d91:09f9.
 * prnout.c's object follows in the same frame, 2d91:09fa..0d0f, and
 * module 2e62 starts at 2e62:0000 (EXE 0x07f4d0).  One Turbo C large-model
 * _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d; functions are
 * defined in shipped order.  Definition comments give the load-based
 * address; extern comments the far pointer as stored in the shipped call,
 * or the DGROUP offset.
 *
 * Section 25's relocation table has 198 sites: 113 filed under 2d91, 80 of
 * them in this range (EXE 0x07e7c9..0x07f1b0) and 33 in prnout.c's, and
 * 56 under 2e62, 27 under 2f17 and 2 under 2f91.  The 80 are all far
 * calls: 21 to prnout.c's routines (stored frame 2d91), the rest to the
 * resident code and to vector stubs.  Three of the section's thirteen
 * vectors land in frame 2d91, all in this object: 18de:1bd9 ->
 * print_screen (editloop.c's print_screen, screen 17), 18de:1ef9 ->
 * link_add and 18de:1eef -> text_print (evedit.c's link_add and
 * text_print, section 17).  No relocated word in section 25 outside
 * frame 2d91 stores 2d91.
 * print_screen near-calls print_area ahead of its definition, so that one
 * is static; corner_char, hwall_char, vwall_char, print_block and
 * print_event are reached only by near calls after their definitions, so
 * the bytes do not decide static or public for them.
 *
 * FRUA's own code: the sharing map pairs all 9 routines a-only, so every
 * name here is chosen from the code.  print_screen never reads arg
 * (editloop.c passes 0).
 *
 * _DATA is DGROUP 9242..929b, thirteen literals stored as eleven ("  " is
 * the tail of "    ", "%2d" of " %2d"), after section 24's (artimp.c,
 * ends at 9241); module 2e62's code (EXE 0x07f61b) pushes 929c.
 * _BSS is links, the far pointer at d57c..d57f, after section 24's
 * glyphcut.c (trn_name ends at d57b); module 2e62 reads d580.
 * The module also reads view3d.c's geo.area (d41c), textbank.c's two
 * blocks (a7ec, a7f0) and writes mapevent.c's msg_buf (txt_get's
 * buffer).
 *
 * Forced by the bytes: map_wall's x and y int (map_wall's own char
 * types narrow the "- 1" to dec al: 0 of 9 ranges); the (unsigned char)
 * cast on area_cell_at's result (& 0xff: 3 of 9); "++n < 100" (n++ as a
 * statement of its own: 3 of 9); cell_used returning unsigned char, as
 * evedit.c declares it (char: 6 of 9); Links.count unsigned char (char: 4
 * of 9); print_event's locals declared l, result, name (reversed: 8 of 9);
 * text_print's "q <= p" branch first (the q > p arm first: 5 of 9);
 * print_area's unsigned char parameter (int: 0 of 9) and char return
 * (int: 8 of 9); Area.rows/cols, Encounter.kind and print_area's evs
 * unsigned char (char: print_area 4 bytes short, the other 8 identical).
 * Not decided by the bytes, each 9 of 9: register on corner_char's w,
 * alone or with r (r alone: 8 of 9); register on print_event's i; int or
 * short in print_block; env->bits |= r or
 * env->bits = env->bits | r; EdEnv.bits long or unsigned long (no shift
 * here); "(i - 99)" or "(i + 1 - 100)"; and declaring area_cell_at
 * unsigned char in place of the cast.
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0                                     */
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

struct Encounter {              /* mapevent.c's, 4 bytes                  */
    unsigned ticks;             /* +0   not read here                     */
    unsigned char kind;         /* +2   the event                         */
    unsigned char chance;       /* +3   not read here                     */
};

struct Area {                   /* view3d.c's, what geo.area points at;
                                   +2e..+75 as modclear.c's events_clear
                                   writes them                            */
    unsigned version;           /* +0   not read here                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    char pad_4[0x2a];           /* +4   not read here                     */
    struct Encounter rest_enc[8]; /* +2e  resting in each zone              */
    struct Encounter step_events[8]; /* +4e  the step events                   */
    char pad_6e[8];             /* +6e  not read here                     */
    char name_wide[0x9a];       /* +76  bound: spacing (texts at +110)    */
};

struct Geo {                    /* view3d.c's geo record                  */
    char pad_0[8];              /* +0   not read here                     */
    struct Area far *area;      /* +8   d41c                              */
    char pad_c[2];              /* +c   not read here                     */
};

struct Event {                  /* textbank.c's event record, 0x14 bytes  */
    char pad_0[0x14];           /* +0   not read here                     */
};

struct Links {                  /* an event's chain links, 14 bytes, on
                                   print_event's stack                    */
    unsigned char count;        /* +0                                     */
    unsigned char depth;        /* +1   the event's print depth           */
    unsigned char ofs[6];       /* +2   the link's record offset; bound:
                                        spacing                           */
    unsigned char ev[6];        /* +8   the linked event; bound: spacing  */
};

extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_upr(char far *s);                       /* 00d7:25f5 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far txt_get(char far *pool, int slot, char far *out); /* 0cf0:046e */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far prompt_window_push(int row, int col);                   /* 0df0:0020 */
extern void far prompt_attr_set(int attr);                           /* 0df0:005a */
extern void far prompt_window_pop(void);                               /* 0df0:0077 */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern char far area_cell_at(char x, char y);
                                /* 18de:15ad, vector to overlay 13 26e3:0ae7 */
extern void far chain_label(unsigned char n, unsigned char k, char far *out);
                                /* 18de:178d, vector to overlay 5 1e8f:0e3e */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern void far area_label(int v, char far *out);
                                /* 18de:17e7, vector to overlay 5 1e8f:0eb5 */
extern void far label_get(unsigned char num, unsigned char kind, char far *out);
                                /* 18de:1819, vector to overlay 5 1e8f:06b0 */
extern unsigned char far cell_used(unsigned char n);
                                /* 18de:1837, vector to overlay 5 1e8f:0df0 */
extern int  far map_wall(int x, int y, int dir);
                                /* 18de:1d05, vector to overlay 13 26e3:02d1 */
extern int  far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                             void far *rec, int mode, int size);
                                /* 18de:1ea9, vector to overlay 17 29f4:24d4 */
extern void far prn_open(char header, char far *title);          /* 2d91:09fa */
extern void far prn_close(void);                                 /* 2d91:0a22 */
extern void far prn_eject(void);                                 /* 2d91:0a2d */
extern void far prn_line(char far *fmt, ...);                    /* 2d91:0ae0 */
extern void far prn_center(char far *fmt, ...);                  /* 2d91:0c50 */
extern void far prn_skip(int n);                                 /* 2d91:0cd1 */
extern void far prn_indent(int col, char far *fmt, ...);         /* 2d91:0ce7 */

extern char far *text_pool_block;                                /* a7ec */
extern struct Event far *event_table;                               /* a7f0 */
extern char msg_buf[];                                          /* b259 */
extern struct Geo geo;                                           /* d414 */

static char far print_area(unsigned char area);

static struct Links far *links;                                  /* d57c */

/* Screen 17: print the current area; env->bits' low word gets the result. */
int far print_screen(int prev, struct EdEnv far *env, void far *arg) /* 3d91:0000 */
{
    char r;

    r = print_area(cur_area_get());
    env->bits &= 0xffff0000L;
    env->bits |= r;
    return prev;
}

/* The corner squares (c - 1..c, r - 1..r) share: '+' when a wall meets it. */
void far corner_char(char far *out, int r, int c)                /* 3d91:0055 */
{
    int w;

    w = map_wall(c, r, 0);
    w = int_max(w, map_wall(c, r - 1, 4));
    w = int_max(w, map_wall(c, r, 6));
    w = int_max(w, map_wall(c - 1, r, 2));
    w = int_max(w, map_wall(c, r - 1, 6));
    w = int_max(w, map_wall(c - 1, r - 1, 2));
    w = int_max(w, map_wall(c - 1, r, 0));
    w = int_max(w, map_wall(c - 1, r - 1, 4));
    *out = w ? '+' : ' ';
}

/* The wall between squares (c, r - 1) and (c, r). */
void far hwall_char(char far *out, int r, int c)                 /* 3d91:0152 */
{
    int w;

    w = map_wall(c, r, 0);
    w = int_max(w, map_wall(c, r - 1, 4));
    switch (w) {
    case 0:
        *out = ' ';
        break;
    case 1:
        *out = '.';
        break;
    case 2:
        *out = '-';
        break;
    }
}

/* The wall between squares (c - 1, r) and (c, r). */
void far vwall_char(char far *out, int r, int c)                 /* 3d91:01b6 */
{
    int w;

    w = map_wall(c, r, 6);
    w = int_max(w, map_wall(c - 1, r, 2));
    switch (w) {
    case 0:
        *out = ' ';
        break;
    case 1:
        *out = ':';
        break;
    case 2:
        *out = '|';
        break;
    }
}

/* Rows r0..r1 - 1 and columns c0..c1 - 1 of the map; each square with an
 * event gets the next tag, and its event goes to evs[*nev]. */
void far print_block(int r0, int c0, int r1, int c1, unsigned char far *evs, int far *nev)
                                                                 /* 3d91:021b */
{
    int r;
    int c;
    int ev;
    char line[0x52];
    char cell[12];
    int n;

    n = *nev;
    str_copy(line, "    ");
    for (c = c0; c < c1; c++) {
        str_format(cell, " %2d", c);
        str_cat(line, cell);
    }
    prn_line(line);
    for (r = r0; r < r1; r++) {
        str_copy(line, "    ");
        for (c = c0; c <= c1; c++) {
            corner_char(cell, r, c);
            if (c < c1) {
                hwall_char(cell + 1, r, c);
                hwall_char(cell + 2, r, c);
                cell[3] = 0;
            } else
                cell[1] = 0;
            str_cat(line, cell);
        }
        prn_line(line);
        str_format(line, " %2d ", r);
        for (c = c0; c <= c1; c++) {
            vwall_char(cell, r, c);
            if (c < c1) {
                if ((ev = (unsigned char)area_cell_at(c, r)) != 0) {
                    evs[n] = ev;
                    if (++n < 100)
                        str_format(cell + 1, "%2d", n);
                    else
                        str_format(cell + 1, "%c%d", (n - 100) / 10 + 'A', (n - 100) % 10);
                } else
                    str_copy(cell + 1, "  ");
            } else
                cell[1] = 0;
            str_cat(line, cell);
        }
        prn_line(line);
    }
    str_copy(line, "    ");
    for (c = c0; c <= c1; c++) {
        corner_char(cell, r, c);
        if (c < c1) {
            hwall_char(cell + 1, r, c);
            hwall_char(cell + 2, r, c);
            cell[3] = 0;
        } else
            cell[1] = 0;
        str_cat(line, cell);
    }
    prn_line(line);
    *nev = n;
}

/* event_edit's mode-7 callback for each chain link of the record. */
void far link_add(int ofs, int ev)                               /* 3d91:0497 */
{
    if (ev) {
        links->ofs[links->count] = ofs;
        links->ev[links->count++] = ev;
    }
}

/* event_edit's mode-7 callback for each text: text n (from 1) of the
 * pool, word-wrapped at 38 characters, under the event being printed. */
void far text_print(int n)                                       /* 3d91:04cd */
{
    char far *p;
    char far *q;
    char c;

    if (n == 0)
        return;
    txt_get(text_pool_block, n - 1, msg_buf);
    for (p = msg_buf; *p; ) {
        while (*p == ' ')
            p++;
        for (q = p; *q && q - p < 0x26; q++)
            ;
        while (*q && *q != ' ' && q > p)
            q--;
        if (q <= p) {
            for (q = p; *q && q - p < 0x26; q++)
                ;
            c = *q;
            *q = 0;
            prn_indent(links->depth * 2 + 6, "%s", p);
            *q = c;
            p = q;
        } else if (*q) {
            *q = 0;
            prn_indent(links->depth * 2 + 6, "%s", p);
            p = q + 1;
        } else {
            prn_indent(links->depth * 2 + 6, "%s", p);
            p = q;
        }
    }
    prn_skip(1);
}

/* Event ev as "label - kind name", its texts, then each chained event. */
void far print_event(int depth, char far *label, int ev, struct EdLink far *link)
                                                                 /* 3d91:0642 */
{
    struct Links l;
    long result;
    char name[0x1e];
    int i;

    label_get(cell_used(ev), 10, name);
    prn_indent(depth * 2 + 4, "%s - %s", label, name);
    l.depth = depth;
    l.count = 0;
    links = &l;
    event_edit(0, &result, link, cell_used(ev), event_table + ev - 1, 7, 0x14);
    for (i = 0; i < l.count; i++) {
        chain_label(cell_used(ev), l.ofs[i], name);
        str_cat(name, " Chain");
        print_event(int_min(depth + 1, 0x12), name, l.ev[i], link);
    }
}

/* The title page, the map a block a page, and every event the map, the
 * step table and the rest table name. */
static char far print_area(unsigned char area)                   /* 3d91:0748 */
{
    struct EdLink link;
    int nev;
    unsigned char evs[200];
    char title[0x28];
    char buf[0x14];
    register int i;
    register int j;

    frame_border();
    status_printf(0xd, 6, 7, 0, "Printing . . .");
    timer_ticks();
    prompt_window_push(0x1f96, 1);
    prompt_attr_set(0x8b);
    area_label(area, title);
    str_format(title, "%s - %s", title, geo.area->name_wide);
    str_upr(title);
    prn_open(1, title);
    prn_skip(6);
    prn_center("%(-%)", 0x28);
    prn_center(title);
    prn_center("%(-%)", 0x28);
    prn_eject();
    nev = 0;
    for (i = 0; i < geo.area->rows; i += 0x18)
        for (j = 0; j < geo.area->cols; j += 0x18) {
            print_block(i, j, int_min(i + 0x18, geo.area->rows),
                        int_min(j + 0x18, geo.area->cols), evs, &nev);
            prn_eject();
        }
    prn_skip(2);
    for (i = 0; i < nev; i++) {
        if (i + 1 < 100)
            str_format(buf, "%2d", i + 1);
        else
            str_format(buf, "%c%d", (i - 99) / 10 + 'A', (i - 99) % 10);
        print_event(0, buf, evs[i], &link);
        prn_skip(1);
    }
    for (i = 0; i < 8; i++)
        if (geo.area->step_events[i].kind) {
            str_format(buf, "Step Event %d", i + 1);
            print_event(0, buf, geo.area->step_events[i].kind, &link);
            prn_skip(1);
        }
    for (i = 0; i < 8; i++)
        if (geo.area->rest_enc[i].kind) {
            str_format(buf, "Rest in Zone %d", i + 1);
            print_event(0, buf, geo.area->rest_enc[i].kind, &link);
            prn_skip(1);
        }
    prn_close();
    prompt_window_pop();
    return 1;
}
