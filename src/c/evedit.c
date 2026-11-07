/* CKIT.EXE overlay section 17, module 29f4: the event editor's pages.  An
 * event is edited through the "script" library (opened in slot 0x18): its
 * item for the event kind, and for kinds below 0x33 also item 0, hold the
 * editor's title and pages, each page a list of field records (a label,
 * a check box, a pop-up menu, a text box, a number, a value shown as text
 * with a button, a text field) followed by data opcodes that tie the field
 * to bytes, words, longs or bits of the event record, give its colour,
 * menu entries, number range, default value and display transform.
 * page_build lays a page out as dialog items (or only runs its opcodes, to
 * set defaults and collect the event's texts), field_parse runs one
 * field's opcodes, field_load and field_store move a field's value between
 * the event record and the dialog, data_ofs, data_get and data_put address
 * and read and write the record.  event_edit is the entry: it copies the
 * caller's record in (or clears it), runs the page loop with the "Ok Prev
 * Next Undo Cancel" menu (Undo is never given a hotkey or handled) until a
 * pick ends it, checks the five-line
 * text and the text pool's room, stores the texts in the text bank and
 * copies the record back.  page_key handles Tab and the cursor keys and
 * puts the pointer on the current field; choice_show is a menu's callback
 * that marks the one of the five following fields its entry selects.
 *
 * Section 17 (parent section 15; no section names 17 as its parent):
 * payload at EXE 0x067f90, link segment 29f4, which sections 16 and 18
 * share.  A bare stored SEG:OFF in this file means section 17.  The module
 * is stored frame 29f4 (load-based 39f4), EXE 0x067f90..0x06b1ba (12,842
 * bytes, 12 routines), the whole section from its first byte; six zero
 * bytes follow to the section's end (0x06b1c0).  One Turbo C large-model
 * _TEXT segment, built with Turbo C 2.01, tcc -ml -O -Z -d; functions are
 * defined in shipped order.  Definition comments give the load-based
 * address; extern comments the far pointer as stored in the shipped call,
 * or the DGROUP offset.
 *
 * Section 17's relocation table files all 163 of its sites under 29f4, all
 * in this range: 159 far calls and four segment pushes (18de for the
 * handlers 18de:18b9, 18de:1765 and 18de:175b, 28aa for section 15's
 * num_proc).  event_edit is the section's one vector target, stub
 * 18de:1ea9, which sections 22 (once), 23 (once) and 25 (six times)
 * far-call.  No far call outside section 16 stores frame 29f4 (section
 * 16's are mapedit.c's own), and the module far-calls none of its own
 * routines: the other eleven are reached by near calls after their
 * definitions, page_key and choice_show also as handlers (push cs at
 * 39f4:28ec and 39f4:1574), so the bytes do not decide static or public
 * for them; they are public here.
 *
 * FRUA's own code: the sharing map pairs all 12 routines a-only, so every
 * name here is chosen from the code.
 *
 * _DATA is DGROUP 8de6..8e39: large_monsters, 28 bytes that field_parse
 * searches for the record's byte 0x18d, and nine stored literals.  It is
 * public: sections 25 and 26 push its address too (EXE 0x07fd24, 0x07fd48,
 * 0x083af2, 0x083b80).  Below it mapedit.c's (section 16) _DATA ends at
 * 8de4 and no word in the image names 8de5; 8e3a has a word hit in section
 * 18.  _BSS is d446..d44f, in the order the names give (goldbox.bss_order):
 * ed_page and ed_ev, public because section 26 reads them (EXE 0x083f7f,
 * 0x083244 and more), and ed_cur.  mapedit.c's _BSS ends at d444, no word
 * names d445, and rest.c's (section 19) fx_live is at d450.
 *
 * Forced by the bytes: cases 7 to 10 of the transforms written as
 * "if (... != N) break; goto load_2;" into case 6's body (with "if (... ==
 * N) goto" the jumps come out je/jmp, not jne/jmp, and with a copy of the
 * body -O merges it the other way); "(items + n - 1)->val" (items[n - 1]
 * computes dec ax); the three search loops' conditions in the for header
 * (a break in the body moves the test); "i + i + 9" in icons_load (i * 2
 * shifts); "++ed_cur >= count"; "(n = f()) >= 0" in event_edit's picker
 * cases (jointly: any one case alone bakes the same); case 9 ahead of cases 4 and 5 in field_store; "i == 0 ? 0x87 :
 * 0x180".
 */

struct ItemRec {                /* an item's saved fields, 18 bytes, shop.c's */
    char pad_0[0xb];            /* +0   not read here                     */
    unsigned char unid_mask;    /* +b   bits 0-2: not yet identified      */
    char pad_c[6];              /* +c   not read here                     */
};

struct Item {                   /* an item, 0x3e bytes                    */
    char pad_0[5];              /* +0   not read here                     */
    char name[0x23];            /* +5                                     */
    struct ItemRec rec;         /* +28                                    */
    char pad_3a[4];             /* +3a  not read here                     */
};

struct Entry {                  /* one menu entry, 8 bytes, as menubox.c  */
    char far *text;             /* +0                                     */
    int val;                    /* +4                                     */
    char key;                   /* +6                                     */
    char flags;                 /* +7                                     */
};

struct Menu {                   /* a pop-up menu, 0x14 bytes, menubox.c's */
    char far *title;            /* +0                                     */
    struct Entry far *items;    /* +4                                     */
    int base;                   /* +8                                     */
    int col;                    /* +a                                     */
    char count;                 /* +c   entries                           */
    char flags;                 /* +d                                     */
    int glyph;                  /* +e                                     */
    int row;                    /* +10                                    */
    int width;                  /* +12  4-column cells                    */
};

struct View {                   /* a scrolled menu's window, menubox.c's  */
    int above;                  /* +0                                     */
    int below;                  /* +2                                     */
    int count;                  /* +4                                     */
    int bottom;                 /* +6                                     */
};

struct NumSave {                /* a number field's val, numfield.c's     */
    long upper;                 /* +00                                    */
    long lower;                 /* +04                                    */
    long value;                 /* +08                                    */
    long saved;                 /* +0c                                    */
    long far *out;              /* +10  here the field's value            */
    char neg;                   /* +14                                    */
};

struct FieldSave {              /* a text field's val, textedit.c's       */
    char text[0x27];            /* +00                                    */
    char label[0x25];           /* +27                                    */
    unsigned char pos;          /* +4c                                    */
};

struct BoxSave {                /* a text box's val, textedit.c's         */
    char box[0xfa];             /* +00  its struct Box, the text first    */
    char label[0x27];           /* +fa                                    */
};

struct Field {                  /* one field of the page, 0x11 bytes      */
    unsigned char far *spec;    /* +0   its record in the script item:
                                        kind, length, row, column, label
                                        length, label                     */
    unsigned char far *data;    /* +4   its data opcode (0x32..0x35), or 0 */
    long value;                 /* +8                                     */
    char kind;                  /* +c   spec[0]                           */
    char id;                    /* +d   its dialog item, -1 none          */
    unsigned char cls;          /* +e   set by opcodes 0x3a..0x44 and
                                        0x4a..0x4f; a kind
                                        8 field's picker 0x60, 0x80..0x85 */
    char idx;                   /* +f   its slot in the page's tables, or
                                        a label's colour                  */
    char fmt;                   /* +10  opcode 0x36's transform           */
};

struct Page {                   /* the page being edited, 0x115e bytes    */
    int count;                  /* +0                                     */
    struct Field fields[30];    /* +2                                     */
    int check;                  /* +200 field kind 0x20's code, for
                                        18de:1ee5 before a page is left   */
    int nmenu;                  /* +202                                   */
    struct Menu menus[8];       /* +204                                   */
    struct View views[8];       /* +2a4                                   */
    int nentry;                 /* +2e4                                   */
    struct Entry entries[200];  /* +2e6                                   */
    int nnum;                   /* +926                                   */
    struct NumSave nums[16];    /* +928                                   */
    int nstr;                   /* +a78                                   */
    char strs[16][16];          /* +a7a the kind 8 fields' text           */
    int nedit;                  /* +b7a                                   */
    struct FieldSave edits[12]; /* +b7c                                   */
    int nbox;                   /* +f18                                   */
    struct BoxSave boxes[2];    /* +f1a                                   */
    int choice_id;              /* +115c the menu choice_show serves      */
};

struct EvRec {                  /* the event record, 0x9cf bytes          */
    unsigned char data[0x1c2];  /* +0   what the data opcodes address;
                                        +28..+ef five lines of 0x28 that
                                        a '~' text box edits              */
    char far *title;            /* +1c2 the editor's title                */
    int pages;                  /* +1c6                                   */
    int ntext;                  /* +1c8                                   */
    int text_ofs[8];            /* +1ca where each text's number is kept  */
    char texts[8][0xfa];        /* +1da                                   */
    int text_sel;               /* +9aa the text the five lines make, + 1 */
    int nlist;                  /* +9ac                                   */
    int list[16];               /* +9ae the low byte of each offset opcode
                                        0x44 marks, used as an offset     */
    char dirty;                 /* +9ce                                   */
};

struct EdLink {                 /* the caller's state                     */
    int kind;                   /* +0                                     */
    int page;                   /* +2                                     */
    int field;                  /* +4   the field a picker runs for, + 1  */
    int back;                   /* +6   the page to come back to          */
    int ret;                    /* +8                                     */
    int next;                   /* +a   the screen to go to               */
    int code;                   /* +c   its argument                      */
};

extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern unsigned far mem_scan(void far *p, int n, char c);    /* 00d7:27b1 */
extern int  far bank_text_add(char far *pool, char far *s);      /* 03ad:011a */
extern void far bank_text_free(char far *pool, int slot);         /* 03ad:0217 */
extern void far item_name(void far *h, struct Item far *it, char col,
                          char row, char full, char show);       /* 03d0:03d1 */
extern void far set_open_mode(char mode);                           /* 05f5:01d3 */
extern void far pal_select(void far *r);                         /* 05f5:06b3 */
extern void far screen_flush(void);                              /* 05f5:0756 */
extern void far hotkey_add(char key, unsigned char far *count);  /* 083a:0009 */
extern int  far menu_line_key(int key);                              /* 083a:1e2d */
extern void far menu_line_show(char far *title, char far *text, char fkeys); /* 083a:1e9a */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern void far pic_load(char far *name, unsigned char set,
                         unsigned char mode, char n);            /* 0cb5:0007 */
extern void far icon_clear(unsigned char idx);                   /* 0cb5:004d */
extern void far icon_load(char far *name, unsigned char set, unsigned char idx); /* 0cb5:0093 */
extern void far pic_free(void);                                  /* 0cb5:0352 */
extern void far txt_get(char far *pool, int slot, char far *out); /* 0cf0:046e */
extern int  far txt_usage(char far *pool, int far *cap);         /* 0cf0:0548 */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern int  far pointer_mode_set(int mode);                          /* 0f5e:0a85 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far mouse_move_to(int row, int col);                 /* 0f5e:10a3 */
extern void far mouse_fake(int button, char both);               /* 0f5e:10d5 */
extern void far *far db_buffer(void);                         /* 107e:0001 */
extern char far shift_held(void);                                /* 107e:0299 */
extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern void far lib_open(char mode, char far *name, int slot);   /* 147e:00d1 */
extern unsigned char far *far lib_item(void far *lib, int n);    /* 147e:01f9 */
extern void far dl_begin(void);                               /* 15e5:1276 */
extern void far ui_add(int op, ...);                             /* 15e5:1293 */
extern int  far dl_item_count(void);                               /* 15e5:151e */
extern void far dl_draw(char all);                           /* 15e5:1522 */
extern int  far ui_run(int (far *idle)(void));              /* 15e5:1589 */
extern void far ui_mark(int id, int attr, ...);                  /* 15e5:1894 */
extern void far ui_send(int first, int last, int msg, ...);      /* 15e5:18f0 */
extern int  far ui_query(int id, int what);                      /* 15e5:1936 */
extern int  far str_format(char far *buf, char far *fmt, ...);   /* 1860:0037 */
extern void far mode_change(int n);
                                /* 18de:10cb, vector to overlay 3 1b41:02fe */
extern void far glbdata_012b(char c);
                                /* 18de:1391, vector to overlay 3 1b41:012b */
extern void far scroll_make(struct ItemRec far *rec);
                                /* 18de:1733, vector to overlay 10 2431:0fe1 */
extern int  far field_proc();
                                /* 18de:175b, vector to overlay 5 1c86:0000 */
extern int  far box_proc();
                                /* 18de:1765, vector to overlay 5 1c86:07a0 */
extern unsigned char far cur_area_get(void);
                                /* 18de:17bf, vector to overlay 5 1e8f:0e8d */
extern void far area_label(int v, char far *out);
                                /* 18de:17e7, vector to overlay 5 1e8f:0eb5 */
extern unsigned char far label_kind(unsigned char m, unsigned char num);
                                /* 18de:1805, vector to overlay 5 1e8f:108b */
extern void far label_get(unsigned char num, unsigned char kind, char far *out);
                                /* 18de:1819, vector to overlay 5 1e8f:06b0 */
extern unsigned char far cell_used(unsigned char n);
                                /* 18de:1837, vector to overlay 5 1e8f:0df0 */
extern int  far pop_proc();
                                /* 18de:18b9, vector to overlay 5 1ff3:0033 */
extern void far var_name(char far *text, int n, char c);
                                /* 18de:1a99, vector to overlay 26 2fc3:05b8 */
extern void far edit_dirty_set(void);
                                /* 18de:1bb1, vector to overlay 10 267d:046d */
extern void far page_extra(int n);
                                /* 18de:1eb3, vector to overlay 26 2fc3:0af9 */
extern void far npc_spells(void);
                                /* 18de:1ebd, vector to overlay 26 2fc3:0002 */
extern int  far var_pick(int n);
                                /* 18de:1ec7, vector to overlay 26 2fc3:06b0 */
extern int  far item_pick(int n);
                                /* 18de:1ed1, vector to overlay 26 2fc3:03d3 */
extern int  far kind_pick(int n, char c);
                                /* 18de:1edb, vector to overlay 26 2fc3:025f */
extern char far edit_check(int code, int n);
                                /* 18de:1ee5, vector to overlay 26 2fc3:07da */
extern void far text_print(int n);
                                /* 18de:1eef, vector to overlay 25 2d91:04cd */
extern void far link_add(int n, int v);
                                /* 18de:1ef9, vector to overlay 25 2d91:0497 */
extern void far alt_collect(int n);
                                /* 18de:1f03, vector to overlay 22 2d91:200f */
extern int  far num_proc();                         /* overlay 15 28aa:0000 */

extern char far *item_kind_names[];                                     /* 02f0 */
extern char far *event_edit_items;                                       /* 0408 */
extern char far *empty_text;                                     /* 3bc0 */
extern char far *text_pool_block;                                /* a7ec */
extern struct EvRec far *event_buf;                               /* b772 */
extern void far *pic_res;                                        /* cfd8 */
extern struct ItemRec far *item_templates;                             /* d122 */

unsigned char large_monsters[28] = {                                  /* 8de6 */
    0x05, 0x06, 0x18, 0x1a, 0x29, 0x4b, 0x51, 0x53, 0x55, 0x58,
    0x59, 0x5a, 0x5b, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
    0x64, 0x65, 0x66, 0x67, 0x69, 0x6a, 0x6c, 0x6e
};

struct Page far *ed_page;                                        /* d446 */
struct EvRec far *ed_ev;                                         /* d44a */
static int ed_cur;                                               /* d44e */

/* The record offset a data opcode names: a byte offset (0x32..0x34), or
 * for a bit list (0x35) the byte of its first bit. */
int far data_ofs(unsigned char far *d)                           /* 39f4:0000 */
{
    if (*d == 0x35)
        return (d[2] >> 3) + (d[3] << 5);
    return d[1] + (d[2] << 8);
}

/* The value a data opcode names: a byte, a word, a long, or bits gathered
 * from the bit numbers listed after a count. */
long far data_get(unsigned char far *d)                          /* 39f4:003d */
{
    long v;
    int pos;
    int bit;
    unsigned char far *q;
    register int n;
    register int byte;

    q = ed_ev->data + d[1] + (d[2] << 8);
    switch (*d) {
    case 0x32:
        v = *q;
        break;
    case 0x33:
        v = ((long)q[1] << 8) + q[0];
        break;
    case 0x34:
        v = ((long)q[1] << 8) + q[0] + ((long)q[2] << 16) + ((long)q[3] << 24);
        break;
    case 0x35:
        q = ed_ev->data;
        v = 0;
        d++;
        bit = 0;
        for (n = *d++; --n >= 0; bit++) {
            pos = (d[1] << 8) + d[0];
            d += 2;
            byte = pos >> 3;
            pos &= 7;
            v |= (long)((q[byte] >> pos) & 1) << bit;
        }
        break;
    }
    return v;
}

/* Field n's value from the record, through its transform, into its item. */
void far field_load(int n)                                       /* 39f4:01bb */
{
    struct Item item;
    struct Field far *f;
    struct Entry far *e;
    int i;
    int far *pv;
    unsigned char far *q;

    f = &ed_page->fields[n];
    if (f->data == 0)
        return;
    f->value = data_get(f->data);
    switch (f->fmt) {
    case 2:
        f->value = !f->value;
        break;
    case 3:
        f->value++;
        if (f->value == 1 && ed_ev->data[data_ofs(f->data)] == 0
            && ed_ev->data[data_ofs(f->data) + 1] == 0
            && ed_ev->data[data_ofs(f->data) + 2] == 0)
            f->value = 0;
        break;
    case 4:
        f->value = 60 - f->value;
        break;
    case 5:
        f->value++;
        break;
    case 6:
        switch ((ed_ev->data[1] >> 3) & 0x1f) {
        case 1: case 2: case 9: case 10: case 11:
        load_2:
            f->value = ed_ev->data[2];
        }
        break;
    case 7:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 5)
            break;
        goto load_2;
    case 8:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 8)
            break;
        goto load_2;
    case 9:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 0x10)
            break;
        goto load_2;
    case 10:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 0x11)
            break;
        goto load_2;
    }
    switch (f->kind) {
    case 4:
        pv = (int far *)&f->value;
        *pv = f->value;
        break;
    case 5:
        e = ed_page->menus[f->idx].items;
        for (i = 0; i < ed_page->menus[f->idx].count; i++, e++)
            if (f->value + 1 == e->val)
                ui_mark(f->id, 0x24, i + 1);
        break;
    case 7:
        ed_page->nums[f->idx].value = f->value;
        break;
    case 8:
        switch (f->cls) {
        case 0x80:
            area_label((int)f->value, ed_page->strs[f->idx]);
            break;
        case 0x81:
            str_copy(ed_page->strs[f->idx], item_kind_names[(int)f->value]);
            break;
        case 0x82:
            if (f->value) {
                if (f->value == 0xff)
                    scroll_make(&item.rec);
                else
                    mem_move5(&item_templates[(int)f->value], &item.rec, 0x12);
                item.rec.unid_mask &= 0xf8;
                item_name(0, &item, 0, 0, 0, 0);
            } else
                str_copy(item.name, "No item");
            str_copy(ed_page->strs[f->idx], item.name);
            break;
        case 0x83:
            label_get((unsigned char)f->value, 0xc, ed_page->strs[f->idx]);
            break;
        case 0x84:
            var_name(ed_page->strs[f->idx], (int)f->value, 0);
            break;
        case 0x85:
            i = 0;
            for (q = ed_ev->data + 0x152; ed_ev->data + 0xc6 <= q; q--)
                if (*q)
                    i++;
            str_format(ed_page->strs[f->idx], "%d spells", i);
            break;
        default:
            if (f->cls == 0x60)
                i = cell_used((unsigned char)f->value);
            else
                i = (int)f->value;
            label_get(i, label_kind(f->cls, i), ed_page->strs[f->idx]);
        }
        break;
    }
}

/* Puts v where a data opcode names; a changed byte sets the record dirty. */
void far data_put(unsigned char far *d, long v)                  /* 39f4:072b */
{
    int pos;
    unsigned char far *q;
    char c;
    register int byte;
    register int n;

    q = ed_ev->data + d[1] + (d[2] << 8);
    switch (*d) {
    case 0x32:
        if (*q != (v & 0xff))
            ed_ev->dirty = 1;
        *q = v;
        break;
    case 0x33:
        if (q[0] != (v & 0xff))
            ed_ev->dirty = 1;
        q[0] = v;
        if (q[1] != ((v >> 8) & 0xff))
            ed_ev->dirty = 1;
        q[1] = v >> 8;
        break;
    case 0x34:
        if (q[0] != (v & 0xff))
            ed_ev->dirty = 1;
        q[0] = v;
        if (q[1] != ((v >> 8) & 0xff))
            ed_ev->dirty = 1;
        q[1] = v >> 8;
        if (q[2] != ((v >> 16) & 0xff))
            ed_ev->dirty = 1;
        q[2] = v >> 16;
        if (q[3] != ((v >> 24) & 0xff))
            ed_ev->dirty = 1;
        q[3] = v >> 24;
        break;
    case 0x35:
        q = ed_ev->data;
        d++;
        for (n = *d++; --n >= 0; v >>= 1) {
            pos = (d[1] << 8) + d[0];
            d += 2;
            byte = pos >> 3;
            pos &= 7;
            c = q[byte] & ~(1 << pos) | ((char)v & 1) << pos;
            if (q[byte] != c)
                ed_ev->dirty = 1;
            q[byte] = c;
        }
        break;
    }
}

/* Field n's value from its item, back through its transform, into the
 * record. */
void far field_store(int n)                                      /* 39f4:09cc */
{
    struct Field far *f;
    long v;
    int far *pv;
    register int i;

    f = &ed_page->fields[n];
    if (f->data == 0)
        return;
    switch (f->kind) {
    case 3:
    case 6:
    case 10:
        return;
    case 9:
        if (str_same((char far *)ed_ev->data + data_ofs(f->data),
                         ed_page->edits[f->idx].text) == 0)
            ed_ev->dirty = 1;
        return;
    case 4:
        pv = (int far *)&f->value;
        f->value = *pv;
        break;
    case 5:
        v = ui_query(f->id, 0x24);
        f->value = (ed_page->menus[f->idx].items + (int)v - 1)->val - 1;
        break;
    }
    v = f->value;
    switch (f->fmt) {
    case 1:
        v = label_kind(f->cls, (unsigned char)f->value) == 2 ? 0 : 1;
        if (((ed_ev->data[7] >> 7) & 1) != (v & 1))
            ed_ev->dirty = 1;
        ed_ev->data[7] = ed_ev->data[7] & 0x7f | ((char)v & 1) << 7;
        v = f->value;
        break;
    case 2:
        v = !f->value;
        break;
    case 3:
        field_load(n);
        if (f->value != v) {
            f->value = v;
            if (v)
                v = 0xff;
            ed_ev->data[data_ofs(f->data)] = ed_ev->data[data_ofs(f->data) + 1]
                = ed_ev->data[data_ofs(f->data) + 2] = v;
        }
        v = int_max(0, (int)f->value - 1);
        break;
    case 4:
        v = 60 - v;
        break;
    case 5:
        v--;
        break;
    case 6:
        switch ((ed_ev->data[1] >> 3) & 0x1f) {
        case 1: case 2: case 9: case 10: case 11:
        store_2:
            ed_ev->data[2] = v;
        }
        break;
    case 7:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 5)
            break;
        goto store_2;
    case 8:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 8)
            break;
        goto store_2;
    case 9:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 0x10)
            break;
        goto store_2;
    case 10:
        if (((ed_ev->data[1] >> 3) & 0x1f) != 0x11)
            break;
        goto store_2;
    }
    data_put(f->data, v);
    switch (f->fmt) {
    case 1:
        v = label_kind(f->cls, (unsigned char)f->value) == 2 ? 0 : 1;
        if (((ed_ev->data[7] >> 7) & 1) != (v & 1))
            ed_ev->dirty = 1;
        ed_ev->data[7] = ed_ev->data[7] & 0x7f | ((char)v & 1) << 7;
        break;
    case 11:
        ed_ev->data[0x89] = 0;
        for (i = 0x9d; i <= 0xa3; i++)
            ed_ev->data[0x89] = int_max(ed_ev->data[0x89], ed_ev->data[i]);
        break;
    }
}

/* The DungCom1 pictures and the record's six CPIC icons, as icons 8..13. */
void far icons_load(void)                                        /* 39f4:0e23 */
{
    register int i;

    pic_load("DungCom1", 2, 0, 0);
    pal_select(pic_res);
    for (i = 0; i < 6; i++)
        if (ed_ev->data[i + i + 9])
            icon_load("CPIC", ed_ev->data[i + i + 9], i + 8);
}

void far icons_free(void)                                        /* 39f4:0e87 */
{
    register int i;

    for (i = 0; i < 6; i++)
        icon_clear(i + 8);
    pic_free();
}

/* A menu's callback (opcode 0x36 transform 0x35): the entry picked selects
 * one of the five items after it, marked with "->", 0x18 against 0x10, and
 * the label of the field after the menu is drawn dim unless it is the
 * first. */
void far choice_show(int id)                                     /* 39f4:0ea4 */
{
    struct Field far *f;
    register int i;
    register int j;

    f = ed_page->fields;
    for (i = 0; i < ed_page->count && f->id != id; i++, f++)
        ;
    fill_rect(0x1f67, 0x1f4c, 0x1f83, 0x1f54, 8);
    i = (ed_page->menus[f->idx].items + ui_query(id, 0x24) - 1)->val - 1;
    switch (i) {
    case 1: case 2: case 9: case 10: case 11:
        i = 0;
        break;
    case 5:
        i = 1;
        break;
    case 8:
        i = 2;
        break;
    case 16:
        i = 3;
        break;
    case 17:
        i = 4;
        break;
    default:
        i = -1;
    }
    if (i >= 0)
        text_print_at(i * 6 + 0x1f67, 0x1f4c, 0x87, "->");
    if (dl_item_count() > id + 1) {
        for (j = 0; j <= 4; j++)
            ui_mark(id + j + 1, j == i ? 0x18 : 0x10);
        text_print_at(f[1].spec[3] + 0x1f40, f[1].spec[4] + 0x1f40, i == 0 ? 0x87 : 0x180,
                      "%s", f[1].spec + 6);
    }
}

/* Runs the data opcodes of the field record at p.  sel: the record is on
 * the page shown and f its field; fresh: write the defaults (0x46..0x49);
 * init: collect the record's texts and list (0x32..0x34 of a text box,
 * 0x44). */
void far field_parse(unsigned char far *p, struct Field far *f, char fresh, char init,
                     char sel)                                   /* 39f4:1022 */
{
    long v;
    int n;
    int k;
    unsigned char far *spec;
    unsigned char far *data;
    char far *t;
    char flag;
    register int len;
    register int j;

    spec = p;
    data = 0;
    len = p[1] + (p[2] << 8);
    if (*p >= 0x20) {
        if (*p == 0x22)
            n = 3;
        else
            n = 4;
    } else
        n = p[5] + 5;
    p += n;
    len -= n;
    flag = 0;
    while (len > 0) {
        switch (*p) {
        case 0x30:
            if (sel) {
                switch (p[1]) {
                case 0:
                    n = 0x87;
                    break;
                case 1:
                    n = 0x8b;
                    break;
                case 2:
                    n = 0x8c;
                    break;
                }
                if (f->id < 0 || f->kind == 8) {
                    text_print_at(spec[3] + 0x1f40, spec[4] + 0x1f40, n, "%s", spec + 6);
                    if (f->id < 0)
                        f->idx = n;
                } else
                    ui_add(0x26, n, 0);
            }
            p++;
            len--;
            break;
        case 0x31:
            if (sel) {
                fill_rect(spec[3] + 0x1f40, 0x1f44, spec[3] + 0x1f44, 0x1fdc, 8);
                text_print_at(spec[3] + 0x1f40, (0x28 - spec[5]) * 2 + 0x1f40, f->idx, "%s",
                              spec + 6);
            }
            break;
        case 0x32:
        case 0x33:
        case 0x34:
            if (sel) {
                f->data = p;
                field_load(f - ed_page->fields);
                if (f->kind == 9)
                    ui_add(0x27, ed_ev->data + data_ofs(p), 0);
                else if (f->kind == 6 || f->kind == 0xa) {
                    for (n = 0; n < ed_ev->ntext && data_ofs(f->data) != ed_ev->text_ofs[n]; n++)
                        ;
                    if (spec[6] != '~')
                        ui_add(0x27, ed_ev->texts[n], 0);
                }
            } else
                data = p;
            if (init && (spec[0] == 6 || spec[0] == 0xa)) {
                ed_ev->text_ofs[ed_ev->ntext] = data_ofs(p);
                if (data_get(data))
                    txt_get(text_pool_block, data_get(data) - 1, ed_ev->texts[ed_ev->ntext]);
                else
                    ed_ev->texts[ed_ev->ntext][0] = 0;
                if (spec[6] == '~') {
                    t = ed_ev->texts[ed_ev->ntext];
                    for (k = 0; k < 5; k++) {
                        if (*t == '^')
                            t++;
                        for (j = 0; t[j] != 0 && t[j] != '^'; j++)
                            ;
                        mem_move(t, ed_ev->data + (k + 1) * 0x28, j);
                        ed_ev->data[(k + 1) * 0x28 + j] = 0;
                        t += j;
                    }
                    ed_ev->text_sel = ed_ev->ntext + 1;
                }
                ed_ev->ntext++;
            }
            if (init && flag) {
                ed_ev->list[ed_ev->nlist] = p[1];
                ed_ev->nlist++;
            }
            p += 2;
            len -= 2;
            break;
        case 0x35:
            if (sel) {
                f->data = p;
                field_load(f - ed_page->fields);
            } else
                data = p;
            n = p[1] * 2 + 1;
            p += n;
            len -= n;
            break;
        case 0x36:
            if (sel) {
                f->fmt = p[1];
                n = 0;
                switch (f->fmt) {
                case 0x32:
                    if (f->id >= 0)
                        ui_mark(f->id, 0x10);
                    break;
                case 0x33:
                    if (ed_ev->data[0x18d] < 0x70)
                        n = 1;
                    break;
                case 0x34:
                    if (ed_ev->data[0x18d] < 0x70
                        && mem_scan(large_monsters, 0x1c, ed_ev->data[0x18d]) >= 0x1c)
                        n = 1;
                    break;
                case 0x35:
                    ui_mark(f->id, 0x22, choice_show);
                    ed_page->choice_id = f->id;
                    break;
                }
                if (n && f->id >= 0) {
                    ui_mark(f->id, 0x11);
                    if (spec[0] == 8)
                        fill_rect(spec[3] + 0x1f40, spec[4] + 0x1f40, spec[3] + 0x1f44,
                                     str_len((char far *)spec + 6) * 4 + spec[4] + 0x1f40, 8);
                }
            }
            p++;
            len--;
            break;
        case 0x37:
            if (sel) {
                ed_page->entries[ed_page->nentry].text = p + 3;
                ed_page->entries[ed_page->nentry].val = p[1] + 1;
                ed_page->menus[f->idx].count++;
                ed_page->nentry++;
            }
            n = p[2] + 1;
            p += n;
            len -= n;
            break;
        case 0x38:
            if (sel)
                ed_page->nums[f->idx].lower = ((long)p[2] << 8) + p[1] + ((long)p[3] << 16)
                    + ((long)p[4] << 24);
            p += 4;
            len -= 4;
            break;
        case 0x39:
            if (sel)
                ed_page->nums[f->idx].upper = ((long)p[2] << 8) + p[1] + ((long)p[3] << 16)
                    + ((long)p[4] << 24);
            p += 4;
            len -= 4;
            break;
        case 0x3a: case 0x3b: case 0x3c: case 0x3d: case 0x3e: case 0x3f:
            if (*p == 0x3b && cur_area_get() < 5)
                break;
            if (sel)
                f->cls |= 1 << (*p - 0x3a);
            break;
        case 0x40: case 0x41: case 0x42: case 0x43:
            if (sel)
                f->cls = 1 << (*p - 0x40) | 0x40;
            break;
        case 0x44:
            if (sel)
                f->cls = 0x60;
            flag = 1;
            break;
        case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
            if (sel) {
                f->cls = *p + 0x36;
                if (*p == 0x4c) {
                    ui_add(0x24, 0x1e, 0);
                    ed_page->nstr++;
                }
            }
            break;
        case 0x45:
            if (sel)
                ui_add(0x2a, p[1], 0);
            p++;
            len--;
            break;
        case 0x46:
            v = p[1];
            if (fresh) {
                if (sel)
                    f->value = v;
                else
                    data_put(data, v);
            }
            p++;
            len--;
            break;
        case 0x47:
            v = p[1] + (p[2] << 8);
            if (fresh) {
                if (sel)
                    f->value = v;
                else
                    data_put(data, v);
            }
            p += 2;
            len -= 2;
            break;
        case 0x48:
            v = ((long)p[2] << 8) + p[1] + ((long)p[3] << 16) + ((long)p[4] << 24);
            if (fresh) {
                if (sel)
                    f->value = v;
                else
                    data_put(data, v);
            }
            p += 4;
            len -= 4;
            break;
        case 0x49:
            if (fresh) {
                if (spec[0] == 9)
                    str_copy((char far *)ed_ev->data + data_ofs(sel ? f->data : data),
                                 (char far *)p + 2);
                else if (init && (spec[0] == 6 || spec[0] == 0xa))
                    str_copy(ed_ev->texts[ed_ev->ntext - 1], (char far *)p + 2);
            }
            n = p[1];
            p += n;
            len -= n;
            break;
        }
        p++;
        len--;
    }
}

/* Runs the script items common (item 0, or none when < 0) and kind: page
 * page is laid out as dialog items; with init or fresh every page's
 * opcodes run too.  page < 0 takes the item's title. */
void far page_build(char init, char fresh, int page, int common, int kind) /* 39f4:1a66 */
{
    struct Field far *f;
    int len;
    unsigned char far *p;
    char sel;
    register int i;

    if (init)
        mem_fill(&ed_ev->title, 0x80d, 0);
    if (page > 0)
        mem_fill(ed_page, 0x115e, 0);
    for (i = 0; i < 2; i++) {
        if (i) {
            if (kind < 0)
                continue;
        } else if (common < 0)
            continue;
        p = lib_item(slot_base(0x18), i ? kind : common);
        if (page < 0)
            ed_ev->title = (char far *)p + 1;
        p += *p;
        while (*p) {
            if (init)
                ed_ev->pages = int_max(ed_ev->pages, p[3]);
            sel = p[3] == page;
            if (init || fresh || sel) {
                if (sel) {
                    ed_page->count = 0;
                    f = ed_page->fields;
                }
                for (p += 4; *p != 2; p += p[1] + (p[2] << 8)) {
                    if (sel) {
                        f->spec = p;
                        f->kind = *p;
                        f->id = dl_item_count();
                        f->data = 0;
                        switch (*p) {
                        case 3:
                            text_print_at(p[3] + 0x1f40, p[4] + 0x1f40, 0x8c, "%s", p + 6);
                            f->id = -1;
                            f->idx = 0x8c;
                            break;
                        case 4:
                            ui_add(4, p[3] + 0x1f40, p[4] + 0x1f42, p + 6, 0x25, 0x10, 0x26, 0x87,
                                   0x23, &f->value, 0);
                            break;
                        case 5:
                            f->idx = ed_page->nmenu;
                            ed_page->nmenu++;
                            ed_page->menus[f->idx].title = p + 6;
                            ed_page->menus[f->idx].items = &ed_page->entries[ed_page->nentry];
                            ed_page->menus[f->idx].count = 0;
                            ui_add(8, pop_proc, 0x27, &ed_page->views[f->idx], 0x23,
                                   &ed_page->menus[f->idx], 0x24, 1, 0x28, p[3] + 0x1f40,
                                   p[4] + 0x1f40, 0);
                            break;
                        case 6:
                            if (p[6] == '~')
                                break;
                            f->idx = ed_page->nbox;
                            ed_page->nbox++;
                            ui_add(8, box_proc, 0x23, &ed_page->boxes[f->idx], 0x28,
                                   p[3] + 0x1f45, p[4] + 0x1f40, 0);
                            str_copy(ed_page->boxes[f->idx].label, (char far *)p + 6);
                            break;
                        case 7:
                            f->idx = ed_page->nnum;
                            ed_page->nnum++;
                            ed_page->nums[f->idx].lower = 0;
                            ed_page->nums[f->idx].upper = 100;
                            ed_page->nums[f->idx].value = 0;
                            ed_page->nums[f->idx].out = &f->value;
                            ui_add(8, num_proc, 0x27, p + 6, 0x23, &ed_page->nums[f->idx], 0x28,
                                   p[3] + 0x1f40, p[4] + 0x1f40, 0);
                            break;
                        case 8:
                            f->idx = ed_page->nstr;
                            ed_page->nstr++;
                            text_print_at(p[3] + 0x1f40, p[4] + 0x1f40, 0x87, "%s", p + 6);
                            ui_add(1, p[3] + 0x1f40, str_len((char far *)p + 6) * 4 + p[4]
                                   + 0x1f44, ed_page->strs[f->idx], 0x24, 0xf, 0x26, 0x87, 0x14, 0);
                            break;
                        case 9:
                        case 10:
                            f->idx = ed_page->nedit;
                            ed_page->nedit++;
                            ui_add(8, field_proc, 0x23, &ed_page->edits[f->idx], 0x28,
                                   p[3] + 0x1f40, p[4] + 0x1f40, 0x2a, 0xf, 0);
                            str_copy(ed_page->edits[f->idx].label, (char far *)p + 6);
                            break;
                        case 0x20:
                            f->id = -1;
                            ed_page->check = p[3];
                            break;
                        case 0x21:
                            f->id = -1;
                            page_extra(p[3]);
                            break;
                        case 0x22:
                            f->id = -1;
                            break;
                        }
                        ed_page->count++;
                        f++;
                    }
                    field_parse(p, f - 1, fresh, init, sel);
                    if (sel)
                        switch (f[-1].kind) {
                        case 3:
                        case 0x20:
                        case 0x21:
                        case 0x22:
                            f--;
                            ed_page->count--;
                        }
                }
                p++;
            } else {
                len = p[1] + (p[2] << 8);
                p += len;
            }
        }
    }
}

/* The page's key handler (an op-7 item), also called with id -10 and -11
 * around the dialog: Tab gives the focus to the first field that edits;
 * keys 0x106 and 0x102 step the current field back and on, 0x108 and
 * 0x104 to the row above and below, Return presses it; then the pointer is
 * put on the current field. */
int far page_key(int id, int key)                                /* 39f4:21ea */
{
    struct Field far *f;
    int row;
    int rc;
    char c;
    register int i;
    register int col;

    rc = 0;
    if (key == 9) {
        for (i = 0; i < ed_page->count; i++) {
            c = ed_page->fields[i].kind;
            if (c == 9 || c == 7 || c == 6 || c == 10) {
                ui_mark(ed_page->fields[i].id, 0x12);
                rc = 1;
                break;
            }
        }
    } else if (pointer_mode_set(-1) == 0) {
        if (id == -10) {
            pointer_show();
            ed_cur = 0;
        } else if (id == -11)
            pointer_hide();
        else if (key == 0x106) {
            if (ed_cur)
                ed_cur--;
            else
                ed_cur = ed_page->count - 1;
            rc = 1;
        } else if (key == 0x102) {
            if (++ed_cur >= ed_page->count)
                ed_cur = 0;
            rc = 1;
        } else if (key == 0x108) {
            row = ed_page->fields[ed_cur].spec[3];
            while (ed_page->fields[ed_cur].spec[3] == row) {
                if (ed_cur)
                    ed_cur--;
                else {
                    ed_cur = ed_page->count - 1;
                    break;
                }
            }
            rc = 1;
        } else if (key == 0x104) {
            row = ed_page->fields[ed_cur].spec[3];
            while (ed_page->fields[ed_cur].spec[3] == row) {
                if (ed_cur < ed_page->count - 1)
                    ed_cur++;
                else {
                    ed_cur = 0;
                    break;
                }
            }
            rc = 1;
        } else if (key == 0xd) {
            c = ed_page->fields[ed_cur].kind;
            if (c == 5)
                mouse_fake(1, 0);
            else
                mouse_fake(shift_held() ? 2 : 1, 1);
            rc = 1;
        }
        f = &ed_page->fields[ed_cur];
        row = f->spec[3] + 0x1f42;
        col = f->spec[4] + 0x1f40;
        i = str_len((char far *)f->spec + 6) * 4;
        switch (f->kind) {
        case 4:
            col += i + 9;
            break;
        case 5:
            col += ed_page->menus[f->idx].width * 4 + i + 3;
            break;
        case 6:
            row += 0x1a;
            col = 0x1fdb;
            break;
        case 8:
            col += i + 0x41;
            if (f->cls == 0x82)
                col += 0x3c;
            break;
        case 7:
        case 9:
        case 10:
            col += ui_query(f->id, 0x2a) * 4 + i + 5;
            break;
        }
        mouse_move_to(row, col);
    }
    return rc;
}

/* Edits the event record rec (size bytes) of kind kind.  mode 0 and 1 clear
 * it, 2 and 5 to 7 copy it in; 0, 4 and 5 store it without the dialog, 6
 * and 7 only run their stubs over its list and texts, 3 comes back from a
 * picker with its value in *result.  Returns 1 (Ok), 2 (Cancel) or 3 (a
 * picker: link->next and link->code say which). */
int far event_edit(int ret, long far *result, struct EdLink far *link, int kind,
                   void far *rec, int mode, int size)            /* 39f4:24d4 */
{
    struct EdLink far *s;
    long m;
    int page;
    int n;
    int pick;
    int common;
    int total;
    unsigned char count;
    unsigned char ref[3];
    char far *t;
    char far *u;
    char save;
    register int i;
    register int rc;

    s = link;
    mode_change(4);
    glbdata_012b(0);
    set_open_mode(0x33);
    lib_open(0x33, "script", 0x18);
    ed_page = db_buffer();
    ed_ev = event_buf;
    if (mode == 2 || mode == 5 || mode == 6 || mode == 7) {
        ed_ev->data[0x14] = 0x14;
        ed_ev->data[0x15] = 0x32;
        ed_ev->data[0x16] = 0xf;
        mem_move(rec, ed_ev, size);
        ed_ev->dirty = 0;
    } else if (mode == 1 || mode == 0) {
        mem_fill(ed_ev, 0x1c2, 0);
        ed_ev->dirty = 1;
    }
    common = kind < 0x33 ? 0 : -1;
    s->kind = kind;
    if (mode == 3 || mode == 4)
        page = s->page;
    else {
        page = kind < 0x33 ? 2 : 1;
        if ((kind == 0x33 || kind == 0x34) && s->back) {
            page = s->back;
            s->back = 0;
        }
        s->field = 0;
        s->ret = ret;
    }
    if (mode == 3 && s->field) {
        frame_border();
        dl_begin();
        page_build(0, 0, page, common, kind);
        if (ed_page->fields[s->field - 1].cls == 0x80 || ed_page->fields[s->field - 1].cls == 0x83)
            m = 1;
        else
            m = 0x8000L;
        if (*result & m) {
            if (ed_page->fields[s->field - 1].cls == 0x80)
                m = (*result >> 4) & 0x3f;
            else if (ed_page->fields[s->field - 1].cls == 0x83)
                m = (*result >> 4) & 0xff;
            else
                m = *result & 0xff;
            if (m == 0xff)
                m = 0;
            ed_page->fields[s->field - 1].value = m;
            field_store(s->field - 1);
            field_load(s->field - 1);
        }
        s->field = 0;
    }
    if (mode == 0 || mode == 4 || mode == 5) {
        rc = 1;
        save = 1;
    } else if (mode == 6 || mode == 7) {
        rc = 1;
        save = 0;
    } else {
        rc = 0;
        save = 0;
        if (kind == 1 || kind == 0x21)
            icons_load();
    }
    page_build(mode != 3, mode == 0 || mode == 1, -1, common, kind);
    while (rc == 0) {
retry:
        frame_border();
        dl_begin();
        page_build(0, 0, page, common, kind);
        text_print_at(0x1f45, 0x1f44, 0x8b, "%s", ed_ev->title);
        text_print_at(0x1f45, 0x1fc0, 0x8b, "Page %2d", page);
        count = 0;
        hotkey_add(0, &count);
        if (ed_ev->pages > 1) {
            hotkey_add(1, &count);
            hotkey_add(2, &count);
        }
        hotkey_add(4, &count);
        menu_line_show(empty_text, event_edit_items, 0);
        ui_add(7, page_key, 0);
        page_key(-10, 0);
        if (ed_page->choice_id)
            choice_show(ed_page->choice_id);
        dl_draw(1);
        screen_flush();
        pick = ui_run(0);
        page_key(-11, 0);
        ui_send(0, dl_item_count() - 1, 0x80);
        for (i = 0; i < ed_page->count; i++)
            field_store(i);
        i = menu_line_key(pick);
        if (i != 4 && ed_page->check && !edit_check(ed_page->check, 0))
            continue;
        if (i >= 0) {
            switch (i) {
            case 0:
                rc = 1;
                save = 1;
                break;
            case 1:
                if (page > 1)
                    page--;
                else
                    page = ed_ev->pages;
                break;
            case 2:
                if (page < ed_ev->pages)
                    page++;
                else
                    page = 1;
                break;
            case 4:
            case 0x1b:
                rc = 2;
                save = 0;
                break;
            }
        } else {
            for (i = 0; i < ed_page->count && ed_page->fields[i].id != pick; i++)
                ;
            if (i < ed_page->count) {
                if (ed_page->fields[i].kind == 8) {
                    rc = 3;
                    switch (ed_page->fields[i].cls) {
                    case 0x60:
                        s->next = 6;
                        s->code = data_ofs(ed_page->fields[i].data) << 8
                            | (int)ed_page->fields[i].value;
                        save = 1;
                        if (kind == 0x33 || kind == 0x34)
                            s->back = page;
                        break;
                    case 0x80:
                        s->next = 0xb;
                        s->code = ((int)ed_page->fields[i].value << 4) + 0x1000;
                        s->field = i + 1;
                        break;
                    case 0x81:
                        if ((n = kind_pick((int)ed_page->fields[i].value, 1)) >= 0) {
                            ed_page->fields[i].value = n;
                            field_store(i);
                        }
                        rc = 0;
                        break;
                    case 0x82:
                        if ((n = item_pick((int)ed_page->fields[i].value)) >= 0) {
                            ed_page->fields[i].value = n;
                            field_store(i);
                        }
                        rc = 0;
                        break;
                    case 0x83:
                        s->next = 0xb;
                        s->code = ((int)ed_page->fields[i].value << 4) + 6;
                        s->field = i + 1;
                        break;
                    case 0x84:
                        if ((n = var_pick((int)ed_page->fields[i].value)) >= 0) {
                            ed_page->fields[i].value = n;
                            field_store(i);
                        }
                        rc = 0;
                        break;
                    case 0x85:
                        npc_spells();
                        rc = 0;
                        break;
                    default:
                        s->next = 8;
                        s->code = ed_page->fields[i].cls << 8 | (int)ed_page->fields[i].value;
                        s->field = i + 1;
                        break;
                    }
                    s->page = page;
                }
            } else
                rc = 2;
        }
    }
    if (save) {
        if (ed_ev->text_sel) {
            t = ed_ev->texts[ed_ev->text_sel - 1];
            m = 0;
            *t = 0;
            for (i = 0; i < 5; i++) {
                str_cat(t, "^");
                for (u = (char far *)ed_ev->data + (i + 1) * 0x28; *u; u++)
                    if (*u == '^' || *u == '~')
                        *u = ' ';
                u = (char far *)ed_ev->data + (i + 1) * 0x28;
                if (*u && (!(*u >= 'A' && *u <= 'Z' || *u >= 'a' && *u <= 'z')
                           || 1L << (to_upper(*u) - 'A') & m) && kind != 0x15) {
                    edit_check(0x102, i + 1);
                    rc = 0;
                    goto retry;
                }
                m |= 1L << (to_upper(*u) - 'A');
                str_cat(t, u);
                while (t[str_len(t) - 1] == ' ')
                    t[str_len(t) - 1] = 0;
            }
            if (str_len(t) > 0x27 && kind != 0x15) {
                edit_check(0x101, str_len(t) - 0x27);
                rc = 0;
                goto retry;
            }
            if (str_len(t) <= 5)
                *t = 0;
        }
        if (rc == 1 && mode != 0 && mode != 5) {
            total = 0;
            for (i = 0; i < ed_ev->ntext; i++) {
                ref[0] = 0x33;
                ref[1] = ed_ev->text_ofs[i] & 0xff;
                ref[2] = ed_ev->text_ofs[i] >> 8 & 0xff;
                if (data_get(ref)) {
                    txt_get(text_pool_block, data_get(ref) - 1, ed_page->boxes[0].box);
                    total -= str_len(ed_page->boxes[0].box);
                }
                total += str_len(ed_ev->texts[i]);
            }
            total += txt_usage(text_pool_block, &n) + 2;
            if (total > n) {
                edit_check(0x100, 0);
                rc = 0;
                goto retry;
            }
        }
        for (i = 0; i < ed_ev->ntext; i++) {
            ref[0] = 0x33;
            ref[1] = ed_ev->text_ofs[i] & 0xff;
            ref[2] = ed_ev->text_ofs[i] >> 8 & 0xff;
            if (mode == 5) {
                if (data_get(ref))
                    bank_text_free(text_pool_block, data_get(ref));
                continue;
            }
            if (data_get(ref))
                txt_get(text_pool_block, data_get(ref) - 1, ed_page->boxes[0].box);
            else
                ed_page->boxes[0].box[0] = 0;
            if (str_same(ed_page->boxes[0].box, ed_ev->texts[i]))
                continue;
            if (data_get(ref)) {
                bank_text_free(text_pool_block, data_get(ref));
                data_put(ref, 0L);
            }
            if (ed_ev->texts[i][0]) {
                n = bank_text_add(text_pool_block, ed_ev->texts[i]);
                if (n <= 0) {
                    if (rc == 1 && mode != 0) {
                        edit_check(0x100, 0);
                        rc = 0;
                        goto retry;
                    }
                    n = 0;
                }
            } else
                n = 0;
            data_put(ref, n);
            ed_ev->dirty = 1;
        }
        if (mode == 5)
            for (i = 0; i < ed_ev->nlist; i++)
                alt_collect(ed_ev->list[i]);
        mem_move(ed_ev, rec, size);
        if (ed_ev->dirty)
            edit_dirty_set();
    }
    if (mode == 6)
        for (i = 0; i < ed_ev->nlist; i++)
            alt_collect(ed_ev->list[i]);
    else if (mode == 7) {
        for (i = 0; i < ed_ev->nlist; i++)
            link_add(ed_ev->list[i], ed_ev->data[ed_ev->list[i]]);
        for (i = 0; i < ed_ev->ntext; i++)
            text_print(ed_ev->data[ed_ev->text_ofs[i]]
                         + (ed_ev->data[ed_ev->text_ofs[i] + 1] << 8));
    }
    if (kind == 1 || kind == 0x21)
        icons_free();
    slot_close(0x18);
    if (rc != 3)
        s->next = s->ret;
    glbdata_012b(1);
    return rc;
}
