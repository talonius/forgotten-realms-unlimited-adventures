/* CKIT.EXE overlay section 5, module 1e8f: the design's labels and a few
 * design services.  A label table is one STRG data entry (1 to 6, by
 * label_entry from an art or item kind 0..14), read through overlay 3's
 * data_load into the buffer scratch_buf points at, behind the 6-byte tag
 * "STR@n" that labels_load writes and checks to know which entry it holds;
 * str_tab points past the tag.  The table's first 14-byte record holds the
 * record count in its first two bytes; each later record (struct Label) is
 * an item number, the kind bits it labels (kind_mask) and 12 bytes of
 * packtext.c text.  label_find and label_store read and write one label,
 * label_default makes the label a record does not give ("Picture 5", "No
 * Monster", ...), label_count and label_nth count and pick through a
 * table, label_list fills a list_menu node list.  The rest: kind_mask and
 * mask_kind map an item kind to kind bits and back, label_kind finds a
 * record's kind; chain_label and area_label name a chain and an area;
 * game_load and design_new load and save the GAME entry (game_rec, saved
 * as 0x184 bytes) and create a design directory NAME.DSN with its SAVE directory;
 * msg_box shows a line on the bottom row and waits for a key; back2_load,
 * back_draw and piece_box draw a backdrop tile and a wall piece's box;
 * cell_used and cell_at read textbank.c's records.
 *
 * Section 5 (parent section 3): payload at EXE 0x0334e0, link segment 1c86.
 * A bare stored SEG:OFF in this file means section 5.  The module is stored
 * frame 1e8f (load-based 2e8f), EXE 0x03557e..0x036bb9 (5,691 bytes, 33
 * routines), from byte 0xe of the paragraph -- 1e8f:0000..000d is the tail
 * of module 1c86, whose last retf is at 1e8f:000d (EXE 0x03557d) -- to its
 * last retf at 1ff3:0008; menubox.c starts at 1ff3:0009.  One Turbo C
 * large-model _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d;
 * functions are defined in shipped order.  Definition comments give the
 * load-based address; extern comments the far pointer as stored in the
 * shipped call, or the DGROUP offset.
 *
 * Section 5's relocation table files 100 sites under 1e8f, all in this
 * range (0x035581..0x036bb1).  28 routines are vector targets, by stubs
 * 18de:144f (game_load) and 18de:176f..1873, which sections 3, 10, 15 to 18,
 * 21 to 23, 25 and 26 far-call; no section names 5 as its parent.  The module
 * far-calls kind_mask and area_start_select ahead of their definitions, so they
 * are public.  label_default, label_find, label_store and back2_load are
 * reached only by near calls after their definitions, and nothing reaches
 * labels_06e6: for these five the bytes do not decide static or public.
 *
 * FRUA's own code: the sharing map pairs none of the 33 routines with DQK
 * beyond 3 shape-weak matches that never count (30 a-only), so every name
 * here is chosen from the code.
 * gamemain.c calls game_load through its stub as game_load.
 *
 * _DATA is 18 stored literals, DGROUP 5e6c..5ec8; -d stores "" on the NUL
 * of the first and "A" as the tail of the eight A's.  Below it 5e62..5e6b
 * is module 1c86's (its literals); 5ec9 is a pad byte; 5eca..5ece, read by
 * module 1c86 and section 15, the pad 5ecf, and menubox.c's _DATA at 5ed0
 * follow.  _BSS
 * is str_tab, cur_area and back2_res, b386..b38e, in the order their
 * names give (goldbox.bss_order); only this module references them.  b384
 * (sections 16, 18, 21, 23 and 26) and b385 below, the pad b38f and menubox.c's
 * pop_attr at b390 above are not this module's.
 *
 * This is the v1.0 build (CKIT.EXE "Version 1.0 March 19,1993"): EXE
 * 0x03556a, 5,656 code bytes, byte-exact.  The addresses and claims above
 * are v1.2's.  v1.0 differs: label_list, label_count and label_nth do not
 * strip the 0x80 bit off mask (no "mask &= 0x7f"); back_draw returns only
 * for n == 0xff (v1.2 also for 0); piece_box draws the 3rd+ piece set via
 * glyph_palette_load without checking geo.area->sets[2].
 */

struct Res;                     /* tiles.c's                              */

struct Event {                  /* textbank.c's record, 0x14 bytes        */
    char kind;                  /* +0                                     */
    char pad_1[0x13];           /* +1   not read here                     */
};

struct Node {                   /* a list_menu line, as menu.c's          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct Label {                  /* one label record of a STRG table, 0xe  */
    unsigned char num;          /* +0  the item's number, 0 the blank one */
    unsigned char kinds;        /* +1  kind_mask bits it is a label for   */
    char text[12];              /* +2  packtext.c's six bits a character  */
};

struct Area {                   /* view3d.c's, what geo.area points at    */
    unsigned version;           /* +0                                     */
    unsigned char rows;         /* +2                                     */
    unsigned char cols;         /* +3                                     */
    unsigned char sets[3];      /* +4  wall set by slot, 0 none           */
};

struct Geo {                    /* view3d.c's geo record                  */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
    char facing_wall;           /* +3   d417                              */
    char event;                 /* +4   d418                              */
    char attr_5;                /* +5   d419                              */
    char attr_6;                /* +6   d41a                              */
    char show_map;              /* +7   d41b                              */
    struct Area far *area;      /* +8   d41c                              */
    char attr_c;                /* +c   d420                              */
    char attr_d;                /* +d   d421                              */
};

extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far str_cmp(char far *a, char far *b);          /* 00d7:24db */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far str_upr(char far *s);                       /* 00d7:25f5 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern int  far dos_open(int ax, char far *name, ...);       /* 00d7:26bc */
extern void far res_free(struct Res far * far *pp);              /* 05f5:000b */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far tile_open_file(char far *name, unsigned char set, char kind,
                               char depth, struct Res far * far *pp);
                                                                 /* 05f5:01de */
extern char far tile_opened(void);                                /* 05f5:04b0 */
extern void far glyph_cell(int col, int row, int glyph, char unused,
                           struct Res far *r);                   /* 05f5:0564 */
extern void far screen_draw_begin(char on);                             /* 05f5:0671 */
extern void far screen_flush_mode_set(char mode);                           /* 05f5:069d */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far res_glyph_recolor(unsigned char far *map, int n, struct Res far *r);
                                                                 /* 05f5:08e1 */
extern void far res_palette_load(struct Res far *r);                   /* 05f5:0c95 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far txt_pack(char far *src, char far *dst);          /* 0cf0:00ab */
extern void far txt_unpack(char far *src, char far *dst, int n); /* 0cf0:01c2 */
extern void far key_wait(void);                                  /* 0da7:00f5 */
extern void far beep(void);                                      /* 0da7:018c */
extern char far dir_first(char disk, char far *name, char far *ext,
                          char files, char dirs);                /* 0e23:04d5 */
extern char far *far dir_next(char far *is_dir);                 /* 0e23:0608 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern char far shift_held(void);                                /* 107e:0299 */
extern void far glyph_palette_load(char far *lib, int n);              /* 12d2:0537 */
extern char far path_default_ext(char far *path, char far *ext, char force); /* 17a1:0264 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far name_clean(char far *name);             /* overlay 3 1b41:003f */
extern void far set_data_disk(char disk);                /* overlay 3 1b41:0083 */
extern void far data_load(char far *name, unsigned char entry, unsigned far *len,
                          char far *buf);               /* overlay 3 1b41:0136 */
extern void far data_save(char far *name, unsigned char entry, int len,
                             char far *buf);            /* overlay 3 1b41:027f */
extern void far design_dir_set(char far *dir);            /* overlay 3 1b41:0354 */
extern void far design_dir_restore(void);                     /* overlay 3 1b41:037c */
extern void far wall_sets_free(char on);  /* 18de:13af, vector to overlay 13 26e3:1721 */
extern char far monst_listed(unsigned char n);
                                /* 18de:187d, vector to overlay 25 2e62:0849 */
extern void far piece_glyph(int col, int row, unsigned char cell, unsigned char k);
                                /* 18de:1887, vector to overlay 13 26e3:0698 */
extern void far wall_sets_load(void);
                                /* 18de:1891, vector to overlay 13 26e3:0c85 */
extern void far back_load(char n);
                                /* 18de:189b, vector to overlay 13 26e3:15b1 */

extern char far *art_names[];                       /* 01b4  "No art", ...  */
extern char far *monster_text;                          /* 0338  "Monster"      */
extern char far *npc_text;                          /* 033c  "NPC"          */
extern char far *a_text;                          /* 0364  "a"            */
extern char far *an_text;                          /* 0368  "an"           */
extern char far *name_text;                          /* 03cc  "Name"         */
extern char far *no_text;                          /* 03e8  "No"           */
extern char far *dungeon_fmt;                          /* 0418  "Dungeon %02d" */
extern char far *overland_fmt;                          /* 041c  "Overland %02d" */
extern char far *nowhere_text;                          /* 0424  "Nowhere"      */
extern char far *module_text;                          /* 0428  "Module"       */
extern char far *event_word;                          /* 042c  "Event"        */
extern char far *effect_text;                          /* 0434  "Effect"       */
extern char far *chain_text;                          /* 046c  "chain"        */
extern char far *normal_text;                          /* 0480  "Normal"       */
extern char design_dir[0x22];                                      /* 5a54 */
extern unsigned char piece_cols[10];                             /* 88b1 */
extern struct Event far *event_table;                               /* a7f0 */
extern char game_rec[];                                          /* ba08 */
extern unsigned char start_area;                                  /* ba38 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Res far *back_res;                                 /* c39b */
extern unsigned char far *scratch_buf;                              /* c3c8 */
extern struct Res far *wall_set_res[3];                              /* caf8 */
extern struct Geo geo;                                           /* d414 */

unsigned char far kind_mask(unsigned char kind, int dir);
void far area_start_select(void);

static unsigned char far *str_tab;                               /* b386 */
static unsigned char cur_area;                                  /* b38a */
static struct Res far *back2_res;                                /* b38b */

char far shift_key_down(void)                                       /* 2e8f:000e */
{
    return shift_held();
}

void far labels_forget(void)                                     /* 2e8f:0014 */
{
    str_tab = 0;
}

/* Makes STRG entry `entry` the table str_tab points at, loading it unless
 * the tag says it is there, and returns its size in bytes, or 0.  The
 * cached path counts only the low byte of the record count, as shipped. */
int far labels_load(unsigned char far * far *out, int entry)     /* 2e8f:0021 */
{
    unsigned char far *p;
    unsigned len;

    if (out && out != &str_tab)
        *out = 0;
    p = str_tab;
    if (p && str_cmp(p - 6, msg_format("%3s@%1d", "STR", entry)) == 0) {
        if (out)
            *out = str_tab;
        return (p[0] + 1) * 0xe;
    }
    p = scratch_buf;
    str_format(p, "%3s@%1d", "STR", entry);
    p += 6;
    str_tab = p;
    len = 0;
    set_data_disk('3');
    data_load("STRG", entry, &len, p);
    if (p && ((p[1] << 8 | p[0]) + 1) * 0xe == len) {
        if (out)
            *out = str_tab;
        return len;
    }
    return 0;
}

/* Writes a table back as STRG entry `entry`; len == size always holds. */
int far labels_save(unsigned char far *buf, int entry)           /* 2e8f:015f */
{
    short len;
    short size;
    unsigned char far *p;

    if ((p = buf) == 0) {
        if ((p = str_tab) == 0
            || str_cmp(p - 6, msg_format("%3s@%1d", "STR", entry)) != 0)
            return 0;
    }
    len = size = ((p[1] << 8 | p[0]) + 1) * 0xe;
    set_data_disk('3');
    data_save("STRG", entry, len, p);
    if (len == size)
        return len;
    return 0;
}

void far label_default(unsigned char num, unsigned char kind, unsigned char mask,
                       char far *out)                            /* 2e8f:0209 */
{
    if (mask == 0)
        *out = 0;
    else if (num != 0)
        switch (kind) {
        case 11:
            *out = 0;
            break;
        case 10:
            str_format(out, "%s %s", an_text, event_word);
            break;
        case 3:
            str_format(out, "%*s %d", 11, mask & 4 ? monster_text : npc_text, num);
            break;
        case 12:
            str_format(out, "%s %s", an_text, effect_text);
            break;
        case 13:
            str_format(out, "%s %s", a_text, chain_text);
            break;
        case 14:
            str_format(out, "%s %s", a_text, name_text);
            break;
        default:
            str_format(out, "%*s %d", 11, art_names[kind], num);
            break;
        }
    else
        switch (kind) {
        case 11:
            str_format(out, "%s %s", no_text, module_text);
            break;
        case 10:
            str_format(out, "%s %s", no_text, event_word);
            break;
        case 3:
            str_format(out, "%s %s", no_text, mask & 4 ? monster_text : npc_text);
            break;
        case 12:
            str_format(out, "%s %s", no_text, effect_text);
            break;
        case 13:
            str_format(out, "%s %s", no_text, chain_text);
            break;
        case 14:
            str_format(out, "%s %s", no_text, name_text);
            break;
        default:
            str_copy(out, art_names[0]);
            break;
        }
}

char far label_find(unsigned char far *tab, unsigned char num, unsigned char kind,
                    char far *out, unsigned char far *bits)      /* 2e8f:0486 */
{
    unsigned char mask;
    struct Label far *r;
    register int i;

    if (kind >= 6 && kind < 10 && num == 0xff)
        num = 0;
    mask = kind_mask(kind, 0);
    if (tab == 0 || num == 0) {
        if (out)
            label_default(0, kind, mask, out);
        if (bits)
            *bits = mask;
        return tab != 0;
    }
    for (i = 0, r = (struct Label far *)(tab + 0xe); i < (tab[1] << 8 | tab[0]); i++, r++)
        if ((r->kinds & mask) && r->num == num) {
            if (out)
                txt_unpack(r->text, out, 0xf);
            if (bits)
                *bits = r->kinds;
            return 1;
        }
    label_default(num, kind, mask, out);        /* out not tested, as shipped */
    return 0;
}

char far label_store(unsigned char far *tab, unsigned char num, unsigned char kind,
                     char far *text, unsigned char far *bits)    /* 2e8f:059c */
{
    unsigned char mask;
    struct Label far *r;
    register int i;

    if (kind >= 6 && kind < 10 && num == 0xff)
        num = 0;
    if (tab && num) {
        mask = kind_mask(kind, 0);
        for (i = 0, r = (struct Label far *)(tab + 0xe); i < (tab[1] << 8 | tab[0]); i++, r++)
            if ((r->kinds & mask) && r->num == num) {
                if (text)
                    txt_pack(text, r->text);
                if (bits)
                    r->kinds = *bits;
                return 1;
            }
    }
    return 0;
}

int far label_entry(unsigned char kind)                          /* 2e8f:065d */
{
    register int n;

    if (kind < 6)
        n = 1;
    else if (kind <= 10)
        n = 2;
    else
        switch (kind) {
        case 11:
            n = 3;
            break;
        case 12:
            n = 4;
            break;
        case 13:
            n = 5;
            break;
        case 14:
            n = 6;
            break;
        default:
            n = 0;
        }
    return n;
}

void far label_get(unsigned char num, unsigned char kind, char far *out) /* 2e8f:06b0 */
{
    labels_load(0, label_entry(kind));
    label_find(str_tab, num, kind, out, 0);
}

void far labels_06e6(unsigned char num, unsigned char kind, char far *out) /* 2e8f:06e6 */
{
    design_dir_set(0);
    labels_forget();
    label_get(num, kind, out);
    design_dir_restore();
    labels_forget();
}

void far label_set(unsigned char num, unsigned char kind, char far *text) /* 2e8f:0715 */
{
    labels_load(0, label_entry(kind));
    if (label_store(str_tab, num, kind, text, 0))
        labels_save(str_tab, label_entry(kind));
}

void far label_get_bits(unsigned char num, unsigned char kind, char far *out,
                        unsigned char far *bits)                 /* 2e8f:0766 */
{
    labels_load(0, label_entry(kind));
    label_find(str_tab, num, kind, out, bits);
}

void far label_set_bits(unsigned char num, unsigned char kind, char far *text,
                        unsigned char far *bits)                 /* 2e8f:079e */
{
    labels_load(0, label_entry(kind));
    if (label_store(str_tab, num, kind, text, bits))
        labels_save(str_tab, label_entry(kind));
}

int far label_count(unsigned char kind, unsigned char mask, unsigned char far *lo,
                    unsigned char far *hi, unsigned char far *cur, int far *pos,
                    char (far *filter)(unsigned char))           /* 2e8f:07f1 */
{
    char want;
    char first;
    int max;
    int target;
    int save_n;
    int save_num;
    int k;
    unsigned char far *tab;
    struct Label far *r;
    register int min;
    register int n;

    if (labels_load(&tab, label_entry(kind)) == 0)
        return 0;
    if ((want = pos != 0) != 0)
        *pos = -1;
    min = lo ? *lo : 0;
    max = hi ? (*hi >= min ? *hi : min) : 0xff;
    target = cur && *cur >= min && *cur <= max ? *cur : 0;
    k = 0;
    r = (struct Label far *)(tab + 0xe);
    first = 1;
    for (n = 0; k < (tab[1] << 8 | tab[0]); k++, r++) {
        if (!(r->kinds & mask))
            continue;
        if (filter && r->num && !filter(r->num))
            continue;
        if (r->num == 0) {
            n++;
            continue;
        }
        if (r->num < min || r->num > max)
            continue;
        if (want) {
            if (r->num == target)
                *pos = n;
            else if (first && *pos < 0) {
                save_n = n;
                save_num = r->num;
            }
        }
        n++;
        first = 0;
    }
    if (want) {
        if (*pos < 0) {
            *pos = save_n;
            if (cur)
                *cur = save_num;
        }
        if (min == 0)
            (*pos)++;
    }
    if (min == 0)
        n++;
    return n;
}

int far label_list(struct Node far *list, unsigned char kind, unsigned char mask,
                   unsigned char first, char wide, char unused,
                   char (far *filter)(unsigned char))            /* 2e8f:099d */
{
    char buf[40];
    char far *s;
    char none;
    char is3;
    char mark;
    struct Node far *node;
    unsigned char far *tab;
    struct Label far *r;
    register int i;
    register int n;

    if ((node = list) == 0)
        return 0;
    none = 0;
    is3 = kind == 3;
    tab = 0;
    if (labels_load(&tab, label_entry(kind)) == 0)
        return 0;
    i = 0;
    r = (struct Label far *)(tab + 0xe);
    if (r->num == 0)
        none = 1;
    if (first == 0) {
        label_default(0, kind, mask, buf);
        if (wide) {
            if (buf[0]) {
                s = msg_format("%s%s - %s", none ? " " : "", node->text, buf);
                str_format(node->text, "%*s", 0x11, s);
            }
        } else
            str_format(node->text, "%s%*s", none ? " " : "", 0x11 - none, buf);
        node->key = 0;
        node = node->next;
    }
    for (n = 0; i < (tab[1] << 8 | tab[0]) && node; i++, r++) {
        if (!(r->kinds & mask))
            continue;
        if (filter && r->num && !filter(r->num))
            continue;
        if (r->num && r->num < first)
            continue;
        n++;
        txt_unpack(r->text, buf, 0xf);
        if (r->num == 0) {
            str_format(node->text, "%*s", 0x11, buf);
            node->key = 1;
            none = 1;
        } else {
            if ((mark = is3 && monst_listed(r->num)) != 0) {
                s = msg_format("%s%s", "*", buf);
                str_copy(buf, s);
            }
            if (is3 && r->num >= 0x70) {
                s = msg_format("%s%s", "#", buf);
                str_copy(buf, s);
                mark = 1;
            }
            if (wide) {
                if (buf[0]) {
                    s = msg_format("%s%s - %s", none && !mark ? " " : "", node->text, buf);
                    str_format(node->text, "%*s", 0x11, s);
                }
            } else
                str_format(node->text, "%s%*s", none && !mark ? " " : "", 0x11 - none, buf);
            node->key = 0;
        }
        node = node->next;
    }
    return (first ? 0 : 1) + n;
}

int far label_nth(int idx, unsigned char kind, unsigned char mask, unsigned char first,
                  struct Label far * far *out,
                  char (far *filter)(unsigned char))             /* 2e8f:0d05 */
{
    unsigned char far *tab;
    struct Label far *r;
    register int i;
    register int n;

    if (labels_load(&tab, label_entry(kind)) == 0) {
        if (out)
            *out = 0;
        return 0;
    }
    i = 0;
    r = (struct Label far *)(tab + 0xe);
    if (first == 0)
        idx--;
    if (out)
        *out = r;
    for (n = 0; i < (tab[1] << 8 | tab[0]); i++, r++) {
        if (!(r->kinds & mask))
            continue;
        if (filter && r->num && !filter(r->num))
            continue;
        if (n++ == idx) {
            if (out)
                *out = r;
            return r->num;
        }
    }
    return first;
}

char far cell_used(unsigned char n)                              /* 2e8f:0df0 */
{
    if (n)
        return (event_table + n - 1)->kind;
    return 0;
}

struct Event far *far cell_at(unsigned char n)                   /* 2e8f:0e15 */
{
    if (n)
        return event_table + n - 1;
    return 0;
}

void far chain_label(unsigned char n, unsigned char k, char far *out) /* 2e8f:0e3e */
{
    short v;

    if (k == 3)
        str_copy(out, normal_text);
    else {
        v = k * 10 + n;
        label_get(v, 13, out);
    }
}

unsigned char far cur_area_get(void)                              /* 2e8f:0e8d */
{
    return cur_area;
}

void far cur_area_set(unsigned char n)                            /* 2e8f:0e91 */
{
    cur_area = int_max(0, int_min(n, 0x28));
}

void far area_label(int v, char far *out)                        /* 2e8f:0eb5 */
{
    register int n;

    if ((n = v & 0x3f) > 4)
        str_format(out, dungeon_fmt, n - 4);
    else if (n)
        str_format(out, overland_fmt, n);
    else
        str_copy(out, nowhere_text);
}

unsigned char far kind_mask(unsigned char kind, int dir)         /* 2e8f:0f1b */
{
    unsigned char m;

    m = 0;
    if (dir > 0)
        m |= 0x80;
    if (kind >= 6)
        m |= 0x40;
    switch (kind) {
    case 3:
        m |= 0xc;
        break;
    case 4:
        if (!(m & 2))
            m |= 0x10;
        break;
    case 2:
    case 7:
        m |= 2;
        break;
    case 8:
        m |= 4;
        break;
    case 9:
        m |= 8;
        break;
    case 5:
    case 10:
        m |= 0x20;
        break;
    case 1:
    case 6:
    case 11:
    case 12:
    case 13:
    case 14:
        m |= 1;
        break;
    }
    return m;
}

unsigned char far mask_kind(unsigned char m, int far *dir)       /* 2e8f:0f9e */
{
    unsigned char k;
    unsigned char i;

    if (dir)
        *dir = m & 0x80 ? 1 : -1;
    k = 0;
    if (m & 0x40) {
        for (i = 0; k == 0 && i < 6; i++, m >>= 1)
            if (m & 1)
                switch (i) {
                case 0:
                    k = 6;
                    break;
                case 1:
                    k = 7;
                    break;
                case 2:
                    k = 8;
                    break;
                case 3:
                case 4:
                    k = 9;
                    break;
                case 5:
                    k = 10;
                    break;
                }
    } else
        for (i = 0; k == 0 && i < 6; i++, m >>= 1)
            if (m & 1)
                switch (i) {
                case 0:
                    k = 1;
                    break;
                case 1:
                    k = 2;
                    break;
                case 2:
                case 3:
                    k = 3;
                    break;
                case 4:
                    if (!(m & 2)) {
                        k = 4;
                        break;
                    }
                case 5:
                    k = 5;
                    break;
                }
    return k;
}

unsigned char far label_kind(unsigned char m, unsigned char num) /* 2e8f:108b */
{
    unsigned char hi;
    struct Label far *r;
    register int i;
    register int count;

    if (labels_load(0, m & 0x40 ? 2 : 1) == 0)
        return 0;
    if (num == 0)
        return mask_kind(m, 0);
    hi = m & 0xc0;
    m &= 0x3f;
    i = 0;
    r = (struct Label far *)str_tab;
    count = r->kinds << 8 | r->num;
    r++;
    for (; i < count; i++, r++)
        if (r->num == num && (r->kinds & m))
            break;
    if (i >= count) {
        if ((hi & 0x40) && (m & 0x20))
            return 10;
        return 0;
    }
    return mask_kind(r->kinds & m | hi, 0);
}

void far game_load(char also)                                    /* 2e8f:1150 */
{
    unsigned len;

    set_data_disk('3');
    data_load("GAME", 1, &len, game_rec);
    if (also)
        area_start_select();
}

void far area_start_select(void)                                       /* 2e8f:1188 */
{
    cur_area_set(start_area);
    geo.area->version = 0;
}

void far design_new(void)                                        /* 2e8f:119b */
{
    char path[0x50];
    char name[0x4f];
    char is_dir;
    register int i;

    set_data_disk('3');
    if (design_dir[0] == 0) {
        str_copy(path, game_rec);
        name_clean(path);
        str_upr(path);
        if (path[0] == 0)
            str_copy(path, "AAAAAAAA");
        while (str_copy(name, path), path_default_ext(name, "DSN", 1),
               dir_first(0, name, 0, 1, 1)) {
            while (dir_next(&is_dir) != 0)
                ;
            if (str_len(path) < 8)
                str_cat(path, "A");
            else {
                for (i = 7; i >= 0 && path[i] == 'Z'; i--)
                    ;
                if (i < 0)
                    i = 0;
                if (path[i] >= 'A' && path[i] <= 'Z' || path[i] >= 'a' && path[i] <= 'z') {
                    path[i]++;
                    for (i++; i < 8; i++)
                        path[i] = 'A';
                } else
                    path[i] = 'A';
            }
        }
        path[0] = 0;
        path_append_name(path, name);
        dos_open(0x3900, path);
        design_dir_set(name);
        path_append_name(path, "SAVE");
        dos_open(0x3900, path);
    }
    data_save("GAME", 1, 0x184, game_rec);
}

void far labels_134f(void)                                       /* 2e8f:134f */
{
    wall_sets_free(1);
}

void far msg_box(char far *fmt, ...)                             /* 2e8f:1359 */
{
    short col;
    char buf[0x28];

    screen_draw_begin(1);
    screen_flush_mode_set(1);
    beep();
    box_draw();
    str_format(buf, "%r", fmt, (void far *)(&fmt + 1));
    col = int_max(0, 0x26 - str_len(buf)) >> 1;
    status_printf(col, 0x18, 0xf, 8, buf);
    screen_flush_mode_set(0);
    screen_flush();
    key_wait();
    screen_page_show();
}

void far back2_load(unsigned char n)                             /* 2e8f:13ee */
{
    short i;
    unsigned char map[0x100];

    set_open_mode('2');
    tile_open_file("back2", n, 0, geo.attr_c, &back2_res);
    if (tile_opened() && (is_xvga() || !pixel_shift())) {
        for (i = 0; i < 0x100; i++)
            map[i] = i;
        for (i = 0x90; i < 0xb0; i++)
            map[i] = i + 0x20;
        for (i = 0; i < 2; i++)
            res_glyph_recolor(map, i, back2_res);
    }
    res_palette_load(back2_res);
}

void far back_draw(unsigned char col, unsigned char row, unsigned char n, char own,
                   char raw)                                     /* 2e8f:14c3 */
{
    if (!raw && n >= 0x20 && n != 0xff && (cursor_obj[8] < 6 || cursor_obj[8] >= 0x14))
        n++;
    if (n == 0xff)
        return;
    if (own) {
        back2_load(n);
        glyph_cell(col, row, 1, 0, back2_res);
        res_free(&back2_res);
    } else {
        back_load(n);
        glyph_cell(col, row, 1, 0, back_res);
        res_palette_load(back_res);
    }
}

void far piece_box(register int row, register int col, int n, int k) /* 2e8f:1576 */
{
    unsigned char c;

    wall_sets_load();
    k = k * 3 / 3;                              /* a mul and an idiv, as shipped */
    c = piece_cols[k] << 2;
    fill_rect(row, col, c + row, c + col, 0);
    if (n != 0) {
        c = (n - 1) / 5;
        if (c < 2)
            res_palette_load(wall_set_res[c]);
        else
            glyph_palette_load(db_buffer(), 0);
        piece_glyph(col, (k == 7 || k == 8 ? 0 : -4) + row, n, k);
        screen_flush();
    }
}
