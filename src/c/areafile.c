/* CKIT.EXE overlay section 23, module 2d91: the area editor's File commands
 * and saving an area's map.  file_screen is screen 9 of editloop.c's loop.
 * Called from another screen (prev below 10), it reads a command number
 * from env->bits' low four bits (1 to 5 and 10; 6 to 9 do nothing) and
 * either answers at once or hands over to screen 11 or 10 (both in section
 * 26): commands 1 and 3 put the current area number in env->bits' bits 4
 * and up, command 4 sets 4 (screen 11), 5 sets 2 (screen 10), and 10 adds
 * nothing (screen 10 when edit_dirty); called back from screen
 * 11 or 10, it reads their answer from env->bits and leaves its own there.
 * save_items_mark gives the File menu's "Save" and "Revert to saved"
 * entries flags 0 when edit_dirty (editloop.c, through 18de:1b7f) is set
 * and 0x81 when it is not.  area_open makes area a (low six bits) current:
 * it loads its geo and sets the current area number, and when the number
 * changed puts geo's place on the area's spot 0, or on spot start_entry when
 * a equals start_area.  area_save, when edit_dirty is set, saves the
 * current area as number n and gives it the area's name as its label.
 * save_3d_map packs the area into the buffer db_buffer returns -- tagged
 * chunks with big-endian sizes: FORM, AMOD, then HDR (the area's first 0x122
 * bytes, the version and eight text words byte-swapped), MAP (its 0xd80
 * bytes of cells), ENCR (textbank.c's 0x7d0 bytes of records) and STRG
 * (textbank.c's 0x1c00-byte text pool), each padded to even length -- and
 * passes it, 0x32a2 bytes, to overlay 3's 1b41:027f as the "GEO" entry n;
 * its two error texts name it Save3DMap.
 *
 * Section 23 (parent section 20, which is empty and has no parent; no
 * section names 23 as its parent): payload at EXE 0x078790, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 23.  The module is stored frame 2d91 (load-based 3d91), EXE
 * 0x078790..0x079026 (2,198 bytes, 9 routines), from the section's first
 * byte to its last retf at 2d91:0895 (2e1a:0005); module 2e1a starts at
 * 2e1a:0006, the target of vector 18de:1bc5 (editloop.c's screen 19).  One
 * Turbo C large-model _TEXT segment, built with Turbo C 2.01,
 * tcc -ml -O -Z -d; functions are defined in shipped order.  Definition
 * comments give the load-based address; extern comments the far pointer as
 * stored in the shipped call, or the DGROUP offset.
 *
 * Section 23's relocation table files 44 sites under 2d91, all in this
 * range (EXE 0x07884c..0x079017), all far calls; its other 182 sites are
 * filed under 2e1a (8) and 2e3e (174).  Four of the section's nine vectors
 * land here: 18de:1c29 -> file_screen (editloop.c's file_screen, screen
 * 9), 18de:1fb7 -> save_items_mark, 18de:1fcb -> area_save and
 * 18de:1fd5 -> area_open (areaed.c's save_items_mark, area_save and
 * area_open, section 21).  Two far calls in section 23 store frame 2d91:
 * area_save's call of save_3d_map ahead of its definition (2d91:04f8), so
 * that one is public, and module 2e1a's call of area_open (2e1a:00d0, with
 * 1 and 0x29 or 0x2a).  save_3d_map near-calls map_pack, map_pack
 * map_write, map_write iff_head and iff_chunk, and iff_chunk iff_head, each
 * ahead of its definition, so those four are static.  It calls the
 * resident code directly, and sections 3, 5, 10 and 13 only through vector
 * stubs.
 *
 * FRUA's own code: the sharing map pairs all 9 routines a-only, so every
 * name here is chosen from the code.  area_open and area_save never read
 * their first parameter (areaed.c passes EdState.view, module 2e1a 1).
 *
 * _DATA is the three literals, DGROUP 9128..916e, after evchain.c's
 * (section 22, 90d2..9127); no instruction in the image references 916f,
 * and code after this module in section 23 reads 9170.  No _BSS.  The
 * module also writes the File menu's entries at 002c (edscreen.c's
 * file_menu, 007c, points at them), view3d.c's geo (geo.area->version in
 * map_pack), cursor_obj[0x13] and tick_slot (map_write's loop index), and
 * reads start_area and start_entry (inside labels.c's game_rec) and
 * textbank.c's two blocks.
 *
 * Forced by the bytes: FileArg's prev and cmd unsigned char (char bakes 0
 * of 9 ranges); EdEnv.bits a signed long (unsigned calls LXURSH@ where the
 * shipped code calls LXRSH@); 0 | k << 4 | c for the shipped two ORs after
 * the shift (k << 4 | 0 | c folds the constants, 0 of 9); the empty
 * env->bits |= 0 for the shipped or ax,0 / or dx,0 (dropped: 0 of 9); the
 * default's
 * break between cases 11 and 10, which lets -O cross-jump case 11's three
 * calls into case 10's copy (without it 8 of 9; `(void)0; goto` into case
 * 10's copy also bakes 9 of 9); short want and
 * if ((size = 0x32a2) & 1) in save_3d_map (int want, or the assignment as
 * a statement of its own, 4 of 9 each); map_pack, map_write, iff_chunk and
 * iff_head returning char (int: 4, 5, 6 and 6 of 9); the ternary for keep
 * (if/else: 0 of 9), the if/else for spot (ternary: 2 of 9), & 0xff on
 * 15ad's result (dropped: 2 of 9) and cur_area_set's unsigned char
 * parameter (int: 2 of 9).  Not decided by the bytes: case 11's goto
 * relabel (written out in case 1 it bakes 9 of 9: -O cross-jumps the two
 * copies), register or plain a in area_open, and int or unsigned char for
 * the entry of data_save and the num of label_get and label_set
 * (an int argument is pushed as a word either way).
 */

struct EdEnv {                  /* editloop.c's, passed to every screen   */
    long bits;                  /* +0   a screen's result; signed here    */
    char shift;                 /* +4                                     */
    int sel;                    /* +5                                     */
};

struct FileArg {                /* editloop.c's EdState.aa5, 6 bytes      */
    unsigned char prev;         /* +0   the screen the command came from  */
    unsigned char cmd;          /* +1   the File command, 1 to 10         */
    int area;                   /* +2   screen 11's answer                */
    char keep;                  /* +4   not read here                     */
    char pending;               /* +5   commands 4 and 5 under way        */
};

struct Pos {                    /* a place and facing, 5 bytes            */
    char x;                     /* +0                                     */
    char y;                     /* +1                                     */
    unsigned char facing;       /* +2                                     */
    char facing_wall;           /* +3                                     */
    char event;                 /* +4                                     */
};

struct Spot {                   /* a place the area keeps, 4 bytes        */
    unsigned char y;            /* +0                                     */
    unsigned char x;            /* +1                                     */
    unsigned char facing;       /* +2                                     */
    unsigned char which;        /* +3                                     */
};

struct AreaHdr {                /* an area's first 0x122 bytes: GEO's HDR */
    unsigned version;           /* +0                                     */
    char pad_2[0xc];            /* +2   not read here                     */
    struct Spot spots[26];      /* +e   bound: spacing to name            */
    char name[0x9a];            /* +76  the area's name; bound: spacing   */
    int zone_texts[8];          /* +110                                   */
    char pad_120[2];            /* +120 not read here                     */
};

struct MapCell {                /* a square of the area, 6 bytes          */
    unsigned char wall[4];      /* +0                                     */
    unsigned char event;        /* +4                                     */
    unsigned char bits;         /* +5                                     */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    struct AreaHdr hdr;         /* +0                                     */
    struct MapCell cells[0x240]; /* +122 GEO's MAP                         */
};

struct Geo {                    /* view3d.c's geo record                  */
    struct Pos pos;             /* +0   d414..d418                        */
    char attr_5;                /* +5                                     */
    char attr_6;                /* +6                                     */
    char show_map;              /* +7                                     */
    struct Area far *area;      /* +8   d41c                              */
    char attr_c;                /* +c                                     */
    char attr_d;                /* +d                                     */
};

struct Entry {                  /* one menu entry, 8 bytes, as menubox.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6                                     */
    char flags;                 /* +7                                     */
};

struct Event;                    /* textbank.c's record                    */

extern int  far swap16(int v);                             /* 00d7:098b */
extern long far swap32(long v);                            /* 00d7:0994 */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_upr(char far *s);                       /* 00d7:25f5 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far txt_touch(char far *pool);                       /* 0cf0:0338 */
extern void far game_exit(void);                                 /* 0d48:002f */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern void far design_dir_restore(void);
                                /* 18de:1373, vector to overlay 3 1b41:037c */
extern void far data_save(char far *name, unsigned char entry, int len, char far *buf);
                                /* 18de:137d, vector to overlay 3 1b41:027f */
extern void far wall_sets_free(char on);
                                /* 18de:13af, vector to overlay 13 26e3:1721 */
extern void far game_load(char on);
                                /* 18de:144f, vector to overlay 5 1e8f:1150 */
extern char far area_cell_at(char x, char y);
                                /* 18de:15ad, vector to overlay 13 26e3:0ae7 */
extern void far geo_load(unsigned char pic);
                                /* 18de:15e9, vector to overlay 13 26e3:1750 */
extern void far labels_134f(void);
                                /* 18de:1783, vector to overlay 5 1e8f:134f */
extern void far cur_area_set(unsigned char n);
                                /* 18de:17b5, vector to overlay 5 1e8f:0e91 */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern void far label_set(unsigned char num, unsigned char kind, char far *text);
                                /* 18de:180f, vector to overlay 5 1e8f:0715 */
extern void far label_get(unsigned char num, unsigned char kind, char far *out);
                                /* 18de:1819, vector to overlay 5 1e8f:06b0 */
extern void far labels_forget(void);
                                /* 18de:184b, vector to overlay 5 1e8f:0014 */
extern char far edit_dirty(void);
                                /* 18de:1b7f, vector to overlay 10 267d:047f */
extern void far edit_dirty_clear(void);
                                /* 18de:1b9d, vector to overlay 10 267d:0476 */

extern struct Entry file_items[10];                 /* 002c  "Open...", ...    */
extern char far *text_pool_block;                                /* a7ec */
extern struct Event far *event_table;                                /* a7f0 */
extern char tick_slot;                                           /* b3f2 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern unsigned char start_area;                                  /* ba38 */
extern unsigned char start_entry;                                  /* ba39 */
extern struct Geo geo;                                           /* d414 */

void far save_3d_map(int n);
static char far map_pack(char far *buf);
static char far map_write(char far *p);
static char far iff_chunk(char far * far *pp, long tag, void far *src, long size);
static char far iff_head(char far * far *pp, long tag, long size);

int far file_screen(int prev, struct EdEnv far *env, void far *arg)
                                                                 /* 3d91:0000 */
{
    int next;
    int k;
    int sub;
    struct FileArg far *f;

    if (arg == 0)
        return 0;
    f = arg;
    if (prev < 10) {
        f->prev = next = prev;
        f->keep = prev == 2 && (env->bits & 0x2000) ? 0 : 1;
        f->cmd = env->bits & 0xf;
        f->pending = f->area = 0;
        env->bits &= 0xfffffff0L;
        switch (f->cmd) {
        case 1:
            next = 11;
            k = cur_area_get();
            env->bits |= 0 | k << 4 | 0;
            break;
        case 2:
            env->bits |= 1;
            break;
        case 3:
            next = 11;
            k = cur_area_get();
            env->bits |= 0 | k << 4 | 0x400;
            break;
        case 4:
            f->pending = 1;
            next = 11;
            env->bits |= 4;
            break;
        case 5:
            f->pending = 1;
            env->bits |= 2;
            next = 10;
            break;
        case 10:
            if (edit_dirty()) {
                env->bits |= 0;
                next = 10;
            } else
                env->bits |= 1;
            break;
        }
    } else {
        next = f->prev;
        switch (prev) {
        case 11:
            f->area = (env->bits & 0xff0) >> 4;
            sub = env->bits & 0xf;
            env->bits &= 0xffffe000L;
            if (sub) {
                switch (f->cmd) {
                case 1:
                    if (cur_area_get() == (f->area & 0x3f))
                        break;
                    goto relabel;
                case 3:
                    env->bits &= 0xffc0ffffL;
                    env->bits = (f->area & 0x3fL) << 16 | env->bits | 1;
                    break;
                case 4:
                    if (f->pending) {
                        game_load(0);
                        f->pending = 0;
                        next = 11;
                        k = cur_area_get() <= 4 ? 1 : 5;
                        env->bits |= 0 | k << 4 | 0x800;
                        break;
                    }
                relabel:
                    env->bits &= 0xffc0ffffL;
                    env->bits |= (f->area & 0x3fL) << 16;
                    if (edit_dirty()) {
                        env->bits |= 0;
                        next = 10;
                    } else
                        env->bits |= 1;
                    break;
                }
            } else if (f->cmd == 4 && !f->pending) {
                design_dir_restore();
                labels_forget();
                game_load(0);
            }
            break;
        default:
            break;
        case 10:
            sub = env->bits & 0xf;
            env->bits &= 0xfffff000L;
            if (sub) {
                switch (f->cmd) {
                case 5:
                    env->bits |= 1;
                    break;
                case 1:
                case 4:
                case 10:
                    env->bits |= sub & 0xfL;
                    break;
                }
            } else if (f->cmd == 4) {
                design_dir_restore();
                labels_forget();
                game_load(0);
            }
            break;
        }
    }
    return next;
}

void far save_items_mark(void)                                   /* 3d91:03fb */
{
    if (edit_dirty()) {
        file_items[1].flags = 0;
        file_items[4].flags = 0;
    } else {
        file_items[1].flags = 0x81;
        file_items[4].flags = 0x81;
    }
}

void far area_open(int view, int a)                     /* 3d91:041b */
{
    char spot;
    char changed;

    changed = cur_area_get() != a;
    geo.pos.event = area_cell_at(0, 0) & 0xff;
    geo_load(a & 0x3f);
    cur_area_set(a & 0x3f);
    labels_134f();
    edit_dirty_clear();
    if (changed) {
        if (start_area == a)
            spot = start_entry;
        else
            spot = 0;
        geo.pos.x = geo.area->hdr.spots[spot].x;
        geo.pos.y = geo.area->hdr.spots[spot].y;
        geo.pos.facing = geo.area->hdr.spots[spot].facing;
    }
}

void far area_save(int view, int n)                              /* 3d91:04cb */
{
    char name[0x12];

    if (edit_dirty()) {
        n &= 0x3f;
        str_upr(geo.area->hdr.name);
        save_3d_map(n);
        cur_area_set(n);
        label_get(n, 11, name);
        if (str_cmp(geo.area->hdr.name, name))
            label_set(n, 11, geo.area->hdr.name);
        labels_134f();
        edit_dirty_clear();
    }
}

void far save_3d_map(int n)                                      /* 3d91:055f */
{
    short want;
    char far *buf;
    int size;

    wall_sets_free(1);
    buf = db_buffer();
    if (map_pack(buf) < 0) {
        error_box("Unable to write geo in Save3DMap.");
        game_exit();
    }
    if ((size = 0x32a2) & 1)
        size++;
    want = size;
    data_save("GEO", n, size, buf);
    if (want != size) {
        error_box("Unable to save geo in Save3DMap.");
        game_exit();
    }
    cursor_obj[0x13] = n;
}

static char far map_pack(char far *buf)                          /* 3d91:05e4 */
{
    geo.area->hdr.version = 0x6a;
    return map_write(buf) ? 0 : -1;
}

static char far map_write(char far *p)                           /* 3d91:0608 */
{
    struct AreaHdr hdr;

    if (p == 0)
        return 0;
    if (!iff_head(&p, 0x4d524f46L, 0x329aL))                     /* "FORM" */
        return 0;
    if (!iff_head(&p, 0x444f4d41L, 0x3292L))                     /* "AMOD" */
        return 0;
    hdr = geo.area->hdr;
    hdr.version = swap16(hdr.version);
    for (tick_slot = 0; tick_slot < 8; tick_slot++)
        hdr.zone_texts[tick_slot] = swap16(hdr.zone_texts[tick_slot]);
    if (!iff_chunk(&p, 0x20524448L, &hdr, (long)sizeof hdr))     /* "HDR " */
        return 0;
    if (!iff_chunk(&p, 0x2050414dL, geo.area->cells, 0xd80L))    /* "MAP " */
        return 0;
    if (!iff_chunk(&p, 0x52434e45L, event_table, 0x7d0L))           /* "ENCR" */
        return 0;
    txt_touch(text_pool_block);
    if (!iff_chunk(&p, 0x47525453L, text_pool_block, 0x1c00L)) { /* "STRG" */
        txt_touch(text_pool_block);
        return 0;
    }
    txt_touch(text_pool_block);
    return 1;
}

static char far iff_chunk(char far * far *pp, long tag, void far *src, long size)
                                                                 /* 3d91:0792 */
{
    char pad;

    if (pp == 0 || *pp == 0 || !iff_head(pp, tag, size))
        return 0;
    mem_move(src, *pp, (int)size);
    *pp += (int)size;
    if (size & 1) {
        pad = 0;
        mem_move(&pad, *pp, 1);
        ++*pp;
    }
    return 1;
}

static char far iff_head(char far * far *pp, long tag, long size) /* 3d91:0825 */
{
    if (pp == 0 || *pp == 0)
        return 0;
    size = swap32(size);
    mem_move(&tag, *pp, 4);
    *pp += 4;
    mem_move(&size, *pp, 4);
    *pp += 4;
    return 1;
}
