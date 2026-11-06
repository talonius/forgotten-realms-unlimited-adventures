/* CKIT.EXE overlay 04 module 1ffa: saving and loading.
 * Characters are <name>.cch files and saved games SavGamA..J.csv in the
 * SAVE directory under the design directory (glbdata.c's design_dir).  A
 * character file is the 0x18e-byte record, each item's 18-byte record (an
 * 'I' item's sub chain after it) and each 10-byte effect; a saved game is
 * 0x400 bytes from cursor_obj + 1, the party's place, the view state, two
 * words written from an unset local and skipped on reading, the member
 * count and each member, padded out to 0x282d bytes from item_templates.  Also
 * the vault, Vault<cc6e>.DAT through section 3's pool_load and pool_save,
 * and monsters from the MONST data file joining the party.
 *
 * EXE 0x031279..0x032fc7, one Turbo C large-model _TEXT segment starting at
 * byte 9 of paragraph 1ffa in RTLink section 4 (payload 0x2db30, link
 * segment 1c86, parent section 3; section 5 has the same link segment).
 * The first 9 bytes of paragraph 1ffa are the tail of module 1cff, and
 * section 4's table files every fixup site from 0x031279 on under frame
 * 1ffa; nine zero bytes follow the last retf to the section's end.  Built
 * with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped
 * order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 4.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and a mod18de_ callee is an RTLink vector
 * stub whose target is named beside it.  Section 3, section 4's parent,
 * is reached by direct far calls.  Thirteen routines are vector targets:
 * 18de:1413 -> 15c7, 16a7 -> 1bac, 16b1 -> 1cba, 16bb -> 1a85,
 * 16c5 -> 04aa, 16cf -> 0c2c, 16d9 -> 0a69, 16e3 -> 043b, 16ed -> 0dfc,
 * 16f7 -> 0e01, 1701 -> 1d1b, 170b -> 1cd7, 1715 -> 1248.
 *
 * This is DQK's saveload.c with its Death Knights of Krynn import (the
 * .pc, .eff, .jnk and .wiz readers, their id maps and the savgam<c>.pty
 * party) removed, the drive-letter save path replaced by the design
 * directory's SAVE, and a new empty routine (0dfc) called on each
 * character read, write and monster load.  mon_load reads FRUA's own
 * MONST record.
 * diskfile.c passes a file callback (fd, path); these read only fd and
 * take (int fd), as DQK's do, and game_read_cb and game_write_cb call
 * hero_read_cb and hero_write_cb directly with fd alone.
 *
 * _DATA is charges and 21 stored literals, DGROUP 5d54..5e60; 5e61 is pad.
 * _BSS is b37a..b383: old_frame, cur_slot, load_hero and entry_text.
 */

struct Node {                   /* a menu list entry, 0x28 bytes          */
    struct Node far *next;      /* +0                                     */
    char key;                   /* +4                                     */
    char text[35];              /* +5                                     */
};

struct ItemRec {                /* an item's saved fields, 18 bytes       */
    unsigned char type;         /* +0  'I' carries count more items       */
    unsigned char name_ids[3];  /* +1                                     */
    int weight;                 /* +4                                     */
    int value;                  /* +6                                     */
    char plus;                  /* +8                                     */
    char save_bonus;            /* +9                                     */
    char readied;               /* +a                                     */
    unsigned char unid_mask;    /* +b                                     */
    char cursed;                /* +c                                     */
    unsigned char count;        /* +d                                     */
    unsigned char magic[3];     /* +e                                     */
    char pad_11;                /* +11 not read here                      */
};

struct Item {                   /* a item_record_pool record, 0x3e bytes         */
    struct Item far *next;      /* +0                                     */
    unsigned char in_use;       /* +4                                     */
    char name[35];              /* +5                                     */
    struct ItemRec rec;         /* +28                                    */
    struct Item far *more;      /* +3a                                    */
};

struct Effect {                 /* a effect_pool record, 10 bytes           */
    unsigned char kind;         /* +0                                     */
    char pad_1;                 /* +1  not read here                      */
    unsigned duration;          /* +2                                     */
    unsigned char strength;     /* +4                                     */
    char on_remove;             /* +5                                     */
    struct Effect far *next;    /* +6                                     */
};

struct Hero {                   /* a party member, 0x18e bytes            */
    struct Hero far *next;      /* +0                                     */
    struct Effect far *effects; /* +4                                     */
    struct Item far *items;     /* +8                                     */
    struct Item far *equipped[13]; /* +c                                  */
    struct Fight far *fight;    /* +40                                    */
    char pad_44[0x1c];          /* +44  not read here                     */
    char name[0x11];            /* +60                                    */
    char pad_71[0x11];          /* +71  not read here                     */
    unsigned char icon_dim;     /* +82                                    */
    char pad_83[0x10];          /* +83  not read here                     */
    unsigned char control;      /* +93                                    */
    char pad_94[9];             /* +94  not read here                     */
    unsigned char level[7];     /* +9d                                    */
    char pad_a4[0x11];          /* +a4  not read here                     */
    unsigned char identity;     /* +b5                                    */
    char pad_b6;                /* +b6  not read here                     */
    unsigned char class_mask;   /* +b7                                    */
    char pad_b8[4];             /* +b8  not read here                     */
    unsigned char icon_body;    /* +bc                                    */
    unsigned char icon_slot;    /* +bd                                    */
    char pad_be[3];             /* +be  not read here                     */
    unsigned char item_count;   /* +c1                                    */
    char pad_c2[0xcb];          /* +c2  not read here                     */
    unsigned char mon_id;       /* +18d                                   */
};

struct HotKey {                 /* menu.c's                               */
    unsigned char key;          /* +0  menu.c: char; == 0xff needs this   */
    unsigned char ch;           /* +1                                     */
};

struct Geo {                    /* the party's place and the view state   */
    char x;                     /* +0   d414                              */
    char y;                     /* +1   d415                              */
    unsigned char facing;       /* +2   d416                              */
    char pad_3[4];              /* +3   not read here                     */
    char show_map;              /* +7   d41b                              */
};

struct MonTail {                /* a MONST record's tail, after the Hero  */
    unsigned char items[16];    /* +0   item_templates index, 0xff 18de:1733, 0 none */
    unsigned char charge_ids[16]; /* +10  index into charges               */
    unsigned char effects[10];  /* +20  effect ids, 0 none                */
};

struct Pool;                    /* strutil.c's; only its address is passed */

extern char far *far packbits_expand(char far *in, char far *out, int n);
                                                                 /* 00d7:0a1c */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern char far *far str_find(char far *s, char c);          /* 00d7:258e */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern long far file_seek(int fd, long pos, int whence);      /* 00d7:2705 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far recalc_stats(struct Hero far *h);                   /* 03d0:0b88 */
extern void far bottom_msg(char far *msg);                        /* 03d0:152e */
extern char far input_result(void);                              /* 06c9:001d */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern char far *far input_text(char far *prompt, unsigned char fg,
                                unsigned char bg, unsigned char max);
                                                                 /* 06c9:05bc */
extern void far status_msg(char far *fmt, char fg, char bg, ...); /* 06c9:08f7 */
extern void far hotkey_reset(unsigned char n);                   /* 083a:004b */
extern void far box_draw(void);                                  /* 083a:03b2 */
extern char far confirm(char far *prompt, char anim);            /* 083a:044d */
extern void far list_free(struct Node far * far *head);          /* 083a:0629 */
extern char far menu_run(char far *title, char far *text, char fkeys, char anim);
                                                                 /* 083a:1fb3 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern void far mem_move5(void far *src, void far *dst, unsigned n); /* 0a75:0339 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far icon_load(char far *name, unsigned char set, unsigned char idx);
                                                                 /* 0cb5:0093 */
extern void far icon_size(unsigned char idx, unsigned char bank, int far *rows,
                          int far *cols);                        /* 0cb5:035f */
extern void far *far rec_alloc(void);                            /* 0d48:013c */
extern void far frame_border(void);                              /* 0d65:002a */
extern void far frame_select(int n);                               /* 0d65:03c9 */
extern char far archive_open(char disk, char far *name, int flags,
                             char (far *fn)(int fd));            /* 0e23:01eb */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd));               /* 0e23:03fa */
extern char far dir_first(char disk, char far *name, char far *ext,
                          char files, char dirs);                /* 0e23:04d5 */
extern char far *far dir_next(char far *is_dir);                 /* 0e23:0608 */
extern char far str_match(char far *s, char far *at);            /* 1855:006a */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern char far path_default_ext(char far *path, char far *ext, char force);
                                                                 /* 17a1:0264 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern void far name_clean(char far *name);            /* overlay 3 1b41:003f */
extern void far set_data_disk(char disk);               /* overlay 3 1b41:0083 */
extern void far data_load(char far *name, unsigned char entry, unsigned far *len,
                          char far *buf);              /* overlay 3 1b41:0136 */
extern void far pool_clear(void);                      /* overlay 3 1bfb:0002 */
extern char far pool_save(int fd);                     /* overlay 3 1bfb:00df */
extern char far pool_load(int fd);                     /* overlay 3 1bfb:023f */
extern void far wild_pic_open(void);
                                /* 18de:14a9, vector to overlay 13 26e3:17e2 */
extern void far view_3d(void);
                                /* 18de:15d5, vector to overlay 8 21d4:0a1e */
extern void far view_map(void);
                                /* 18de:15df, vector to overlay 8 21d4:0a09 */
extern void far geo_load(unsigned char pic);
                                /* 18de:15e9, vector to overlay 13 26e3:1750 */
extern int  far party_scrolls(void);
                                /* 18de:171f, vector to overlay 14 28aa:1e59 */
extern void far class_update(struct Hero far *hero);
                                /* 18de:1729, vector to overlay 14 2ccd:05c5 */
extern void far scroll_make(struct ItemRec far *rec);
                                /* 18de:1733, vector to overlay 10 2431:0fe1 */
extern void far saving_throws(struct Hero far *hero);
                                /* 18de:173d, vector to overlay 14 2ccd:0a0f */
extern void far item_effect(struct Item far *item, char on);
                                /* 18de:1747, vector to overlay 14 28aa:16f0 */
extern char far ready_check(struct Hero far *hero, struct Item far *item,
                             unsigned char far *slot);
                                /* 18de:1751, vector to overlay 14 28aa:189f */

extern char far *load_which_text;                                /* 3bec */
extern char far *save_which_text;                                /* 3bf0 */
extern char far *game_items;                                     /* 3c70 */
extern unsigned char class_mask_bits[];                                 /* 624b */
extern int cur_level;                                            /* 6719 */
extern char in_service_event;                                           /* 671c */
extern struct Hero far *cur_node;                                /* b3dd */
extern char tick_slot;                                           /* b3f2 */
extern int view_mode;                                            /* b421 */
extern char game_saved;                                           /* b425 */
extern int saved_view_mode;                                            /* b433 */
extern struct HotKey hotkeys[20];                                /* b4c0 */
extern unsigned char far *cursor_obj;                            /* b75b */
extern struct Pool party_pool;                                    /* b76e */
extern int file_kind;                                            /* bcfb */
extern struct Item far *cur_item;                                /* bcfd */
extern char byte_c3c0;                                           /* c3c0 */
extern struct Pool effect_pool;                                    /* c3c4 */
extern char menu_escaped;                                        /* ca2a */
extern struct Pool item_record_pool;                                    /* cb0e */
extern char vault_slot;                                           /* cc6e */
extern char test_mode;                                           /* cc7a */
extern void far *node_pool;                                      /* ce5d */
extern struct ItemRec far *item_templates;                             /* d122 */
extern struct Hero far *node_list;                               /* d129 */
extern struct Geo geo;                                           /* d414 */
extern char design_dir[];                            /* overlay 3's data, 5a54 */

static unsigned char charges[16] = { 0, 1, 2, 4, 6, 8, 10, 12, 15, 20, 25, 30,
                                     40, 50, 75, 100 };          /* 5d54 */

static char old_frame;                                           /* b37a */
static char cur_slot;                                            /* b37b */
static struct Hero far *load_hero;                               /* b37c */
static char far *entry_text;                                     /* b380 */

void far character_noop(struct Hero far *hero);
void far party_character_append(struct Hero far *hero);
void far vault_load_current(void);
void far vault_save_current(void);

void far save_open(char far *name, int flags,
                   char (far *fn)(int fd))       /* 2ffa:0009 */
{
    short disk;
    char path[80];

    disk = 0;
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    path_append_name(path, name);
    file_open(disk, path, flags, fn);
}

char far save_ready(void)                                        /* 2ffa:006c */
{
    char path[79];              /* the frame holds 79: is_dir sits at -1  */
    char is_dir;

    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    while (!dir_first(0, path, 0, 1, 0)) {
        hotkey_reset(1);
        if (menu_run("Please insert save disk.", "Ok Exit", 0, 0) == 1)
            return 0;
    }
    while (dir_next(&is_dir) != 0)
        ;
    return 1;
}

char far save_write(char far *name, char (far *fn)(int fd))      /* 2ffa:00f9 */
{
    char path[80];

    hotkey_reset(1);
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    path_append_name(path, name);
    while (!archive_open(0, path, 4, fn)
           && menu_run("Save error:", "Try again Exit", 0, view_mode == 2) != 1)
        hotkey_reset(1);
    return 1;
}

char far hero_name_cb(int fd)                                    /* 2ffa:019a */
{
    struct Hero far *hero;

    hero = 0;                   /* hero->name is then the field's offset  */
    if (file_seek(fd, (long)hero->name, 0) < 0)
        return 0;
    return dos_io(0x3f00, entry_text, fd, 16) == 16;
}

void far file_scan(char far *ext, struct Node far * far *list,
                   struct Node far * far *files)                 /* 2ffa:01fc */
{
    struct Node far *node;
    struct Node far *file;
    char far *name;
    char first;
    char is_dir;
    char path[80];

    *list = *files = 0;
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    dir_first(0, path, ext, 1, 0);
    if ((name = dir_next(&is_dir)) == 0)
        return;
    first = 1;
    do {
        if (first) {
            pool_alloc(&node, sizeof(struct Node), node_pool);
            pool_alloc(&file, sizeof(struct Node), node_pool);
            if (file == 0) {
                *files = *list = 0;
                return;
            }
            *files = file;
            *list = node;
            first = 0;
        } else {
            pool_alloc(&node->next, sizeof(struct Node), node_pool);
            pool_alloc(&file->next, sizeof(struct Node), node_pool);
            if (file->next == 0) {
                node->next = *list;
                *list = (*list)->next;
                file->next = *files;
                *files = (*files)->next;
            }
            node = node->next;
            file = file->next;
        }
        mem_fill(node, sizeof(struct Node), 0);
        mem_fill(file, sizeof(struct Node), 0);
        str_copy(file->text, name);
        entry_text = node->text;
        switch (file_kind) {
        case 1:
            save_open(name, 0, hero_name_cb);
            break;
        }
    } while ((name = dir_next(&is_dir)) != 0);
}

void far character_files_scan(struct Node far * far *list,
                       struct Node far * far *files, char kind)  /* 2ffa:043b */
{
    if (!save_ready()) {
        *list = *files = 0;
        return;
    }
    switch (file_kind) {
    case 1:
        file_scan("cch", list, files);
        break;
    }
    if (*list == 0)
        bottom_msg(kind ? "No characters to delete." : "No characters to load.");
}

void far character_icon_load(char on)                                  /* 2ffa:04aa */
{
    icon_load("CBODYS", cur_node->icon_body, cur_node->icon_slot);
}

char far hero_read_cb(register int fd)                           /* 2ffa:04cb */
{
    struct Effect far *eff_new;
    struct Effect far *eff;
    struct Item far *item;
    struct Item far *sub;
    struct Item far *list;
    struct Item far *prev;
    struct Item tmp;
    unsigned char extra;
    char err;
    char scrolls;
    register int n;

    err = dos_io(0x3f00, load_hero, fd, sizeof(struct Hero))
          != sizeof(struct Hero);
    if (!err) {
        extra = 0;
        scrolls = 0;
        list = load_hero->items;
        load_hero->items = 0;
        character_noop(load_hero);
        n = load_hero->item_count;
        item = 0;
        while (list != 0 && !err) {
            if (load_hero->items != 0) {
                pool_alloc(&item->next, sizeof(struct Item), &item_record_pool);
                prev = item;
                item = item->next;
            } else {
                pool_alloc(&load_hero->items, sizeof(struct Item), &item_record_pool);
                item = load_hero->items;
                prev = 0;
            }
            mem_fill(item, sizeof(struct Item), 0);
            err = dos_io(0x3f00, &item->rec, fd, sizeof(struct ItemRec))
                  != sizeof(struct ItemRec);
            if (--n > 0)
                list = item;
            else
                list = 0;
            item->next = 0;
            item->more = 0;
            if (item->rec.type == 'I') {
                sub = item;
                if (party_scrolls() + item->rec.count + extra > 0x78) {
                    scrolls = 1;
                    for (tick_slot = 1; tick_slot <= item->rec.count; tick_slot++)
                        if (!err)
                            err = dos_io(0x3f00, &tmp.rec, fd, sizeof(struct ItemRec))
                                  != sizeof(struct ItemRec);
                    if (prev != 0) {
                        pool_free(prev->next, sizeof(struct Item), &item_record_pool);
                        prev->next = 0;
                    } else {
                        pool_free(load_hero->items, sizeof(struct Item), &item_record_pool);
                        load_hero->items = 0;
                    }
                    item = prev;
                } else
                    for (tick_slot = 1; tick_slot <= item->rec.count; tick_slot++)
                        if (!err) {
                            pool_alloc(&sub->more, sizeof(struct Item), &item_record_pool);
                            sub = sub->more;
                            mem_fill(sub, sizeof(struct Item), 0);
                            err = dos_io(0x3f00, &sub->rec, fd, sizeof(struct ItemRec))
                                  != sizeof(struct ItemRec);
                            sub->next = 0;
                            sub->more = 0;
                            if (err)
                                item->rec.count = tick_slot - 1;
                            else
                                extra++;
                        }
            }
        }
        if (err) {
            for (item = load_hero->items; item != 0; ) {
                if (item->rec.type == 'I') {
                    sub = item->more;
                    /* cur_item, not item: pool_clear's loop, as shipped */
                    for (tick_slot = 1; tick_slot <= cur_item->rec.count; tick_slot++)
                        if (sub != 0) {
                            prev = sub;
                            sub = sub->more;
                            pool_free(prev, sizeof(struct Item), &item_record_pool);
                        }
                }
                prev = item;
                item = item->next;
                pool_free(prev, sizeof(struct Item), &item_record_pool);
            }
            load_hero->items = 0;
        }
        if (!err) {
            eff = load_hero->effects;
            load_hero->effects = 0;
            eff_new = 0;
            while (eff != 0 && !err) {
                if (load_hero->effects != 0) {
                    pool_alloc(&eff_new->next, sizeof(struct Effect), &effect_pool);
                    eff_new = eff_new->next;
                } else {
                    pool_alloc(&load_hero->effects, sizeof(struct Effect), &effect_pool);
                    eff_new = load_hero->effects;
                }
                err = dos_io(0x3f00, eff_new, fd, sizeof(struct Effect))
                      != sizeof(struct Effect);
                eff = eff_new->next;
                eff_new->next = 0;
            }
            if (err) {
                eff_new = load_hero->effects;
                while (eff_new != 0) {
                    eff = eff_new;
                    eff_new = eff_new->next;
                    pool_free(eff, sizeof(struct Effect), &effect_pool);
                }
                load_hero->effects = 0;
            }
        }
        load_hero->fight = 0;
        load_hero->next = 0;
        if (!err && scrolls)
            status_msg("SCROLLS DROPPED!", 0xb, 0);
    }
    return !err;
}

void far qch_load(char far *name, struct Hero far *hero)         /* 2ffa:0a2d */
{
    char file[42];

    str_copy(file, name);
    load_hero = hero;
    save_open(file, 0, hero_read_cb);
}

void far character_file_load(char far *name, struct Hero far *hero, char a, char b) /* 2ffa:0a69 */
{
    mem_fill(hero, sizeof(struct Hero), 0);
    switch (file_kind) {
    case 1:
        qch_load(name, hero);
        break;
    }
    recalc_stats(hero);
    class_update(hero);
}

char far hero_write_cb(register int fd)                          /* 2ffa:0ab8 */
{
    struct Effect far *eff;
    struct Item far *item;
    struct Item far *sub;
    char err;

    character_noop(load_hero);
    err = dos_io(0x4000, load_hero, fd, sizeof(struct Hero))
          != sizeof(struct Hero);
    character_noop(load_hero);
    for (item = load_hero->items; item != 0 && !err; item = item->next) {
        err = dos_io(0x4000, &item->rec, fd, sizeof(struct ItemRec))
              != sizeof(struct ItemRec);
        if (!err && item->rec.type == 'I') {
            sub = item;
            for (tick_slot = 1; tick_slot <= item->rec.count; tick_slot++)
                if (!err) {
                    sub = sub->more;
                    err = dos_io(0x4000, &sub->rec, fd, sizeof(struct ItemRec))
                          != sizeof(struct ItemRec);
                }
        }
    }
    for (eff = load_hero->effects; eff != 0 && !err; eff = eff->next)
        err = dos_io(0x4000, eff, fd, sizeof(struct Effect))
              != sizeof(struct Effect);
    return !err;
}

void far character_file_save(struct Hero far *hero, char far *file)    /* 2ffa:0c2c */
{
    char name[41];
    char done;
    char msg[42];
    char far *ext;
    short disk;
    char path[80];

    ext = "cch";
    if (*file == 0) {
        str_copy(name, hero->name);
        name_clean(name);
        path_default_ext(name, ext, 1);
        done = 0;
    } else {
        str_copy(name, file);
        path_default_ext(name, ext, 1);
        done = 1;
    }
    if (!save_ready())
        return;
    while (!done) {
        disk = 0;
        path[0] = 0;
        path_append_name(path, design_dir);
        path_append_name(path, "SAVE");
        path_append_name(path, name);
        if (!archive_open(disk, path, 0, 0))
            break;
        frame_border();
        str_copy(msg, msg_format("Update %s?", hero->name));
        done = confirm(msg, 0);
        if (!done) {
            box_draw();
            do {
                str_copy(name, input_text("New file name:", 0, 0, 0xf));
                name_clean(name);
            } while (name[0] == 0 && input_result());
            if (!input_result())
                return;
            path_default_ext(name, ext, 1);
        }
    }
    load_hero = hero;
    save_write(name, hero_write_cb);
}

void far character_noop(struct Hero far *hero)                    /* 2ffa:0dfc */
{
}

void far mon_load(unsigned char id, char recs, struct Hero far *hero) /* 2ffa:0e01 */
{
    struct Hero far *saved;
    unsigned char slot;
    struct MonTail far *p;
    unsigned len;
    unsigned short j;
    unsigned short i;
    unsigned short pos;
    short k;
    struct Item far *item;
    struct Effect far *eff;
    unsigned char buf[0x1ea];
    char far *packed;

    set_data_disk('2');
    data_load("MONST", id, &len, buf);
    if (len < 0x1c2) {
        pos = buf[0] << 8 | buf[1];
        mem_move(buf + 2, buf + sizeof buf - len, len);
        packed = buf + sizeof buf - len;
        packbits_expand(packed, buf, pos);
    } else
        pos = sizeof(struct Hero);
    mem_move(buf, hero, sizeof(struct Hero));
    hero->items = 0;
    hero->effects = 0;
    hero->next = 0;
    hero->fight = 0;
    mem_fill(hero->equipped, sizeof hero->equipped, 0);
    character_noop(hero);
    hero->mon_id = id;
    pos = sizeof(struct Hero);
    p = (struct MonTail far *)(buf + pos);
    item = 0;
    hero->item_count = 0;
    for (i = 0; i < 0x10; i++) {
        if (p->items[i] != 0) {
            if (item != 0) {
                item->next = rec_alloc();
                item = item->next;
            } else
                item = hero->items = rec_alloc();
            item->name[0] = 0;
            item->in_use = 0;
            item->next = 0;
            if (p->items[i] == 0xff)
                scroll_make(&item->rec);
            else
                mem_move5(&item_templates[p->items[i]], &item->rec, sizeof(struct ItemRec));
            if (p->charge_ids[i] != 0) {
                if (item->rec.count != 0)
                    item->rec.count = charges[p->charge_ids[i]];
                else if (item->rec.magic[2] < 0x80 && item->rec.magic[1] != 0
                         && item->rec.magic[0] != 0)
                    item->rec.magic[0] = charges[p->charge_ids[i]];
            }
            item->more = 0;
            hero->item_count++;
        }
    }
    for (j = 0; j < 10; j++)
        if (p->effects[j] != 0) {
            eff = hero->effects;
            pool_alloc(&hero->effects, sizeof(struct Effect), &effect_pool);
            hero->effects->next = eff;
            eff = hero->effects;
            eff->kind = p->effects[j];
            eff->duration = 0;
            eff->strength = 0xff;
            eff->on_remove = 0;
        }
    hero->class_mask = 0;
    for (k = 0; k <= 6; k++)
        if (hero->level[k] > 0)
            hero->class_mask += class_mask_bits[k];
    for (item = hero->items; item != 0; item = item->next)
        if (!ready_check(hero, item, &slot)) {
            item->rec.readied = 1;
            hero->equipped[slot] = item;
            if (item->rec.magic[2] & 0x80) {
                saved = cur_node;
                cur_node = hero;
                item_effect(item, 1);
                cur_node = saved;
            }
        }
    recalc_stats(hero);
    saving_throws(hero);
}

void far mon_add(unsigned char id)                               /* 2ffa:1248 */
{
    struct Hero far *hero;
    int rows;
    int cols;

    if (cursor_obj[0x20] <= 7) {
        pool_alloc(&hero, sizeof(struct Hero), &party_pool);
        mon_load(id, 1, hero);
        party_character_append(hero);
        hero->control |= 0x80;
        hero->identity = id;
        icon_load("CPIC", id, hero->icon_slot);
        icon_size(hero->icon_slot, 0, &rows, &cols);
        hero->icon_dim = hero->icon_dim & 0x80 | cols * 2 + rows - 2;
    }
}

char far game_read_cb(register int fd)                           /* 2ffa:12f7 */
{
    char err;
    short count;
    int skip;
    struct Hero far *hero;
    char place[5];
    register int i;

    err = dos_io(0x3f00, cursor_obj + 1, fd, 0x400) != 0x400;
    err |= dos_io(0x3f00, place, fd, 5) != 5;
    if (test_mode == 0)
        mem_move(place, &geo, 5);
    err |= dos_io(0x3f00, &saved_view_mode, fd, 2) != 2;
    err |= dos_io(0x3f00, &view_mode, fd, 2) != 2;
    err |= dos_io(0x3f00, &skip, fd, 2) != 2;
    err |= dos_io(0x3f00, &skip, fd, 2) != 2;
    err |= dos_io(0x3f00, &count, fd, 2) != 2;
    byte_c3c0 = 1;
    geo.show_map = 0;
    cursor_obj[0x20] = 0;
    for (i = 0; i < count && !err; i++) {
        pool_alloc(&hero, sizeof(struct Hero), &party_pool);
        load_hero = hero;
        err |= !hero_read_cb(fd);
        if (err)
            pool_free(hero, sizeof(struct Hero), &party_pool);
        else {
            recalc_stats(hero);
            party_character_append(hero);
        }
    }
    cur_node = node_list;
    return !err;
}

void far game_load_file(char far *name)                          /* 2ffa:14d0 */
{
    short pic;

    old_frame = cursor_obj[0x3a];
    vault_slot = cur_slot;
    pic = cursor_obj[0x13];
    save_open(name, 0, game_read_cb);
    while (cur_node != 0) {
        if (cur_node->control < 0x80)
            character_icon_load(1);
        else
            icon_load("CPIC", cur_node->identity, cur_node->icon_slot);
        cur_node = cur_node->next;
    }
    cur_node = node_list;
    if (test_mode != 0) {
        cursor_obj[0x13] = pic;
        cursor_obj[0x31] = 0;
        cursor_obj[0x85] = 0;
    } else
        geo_load(cursor_obj[0x13]);
    if (cursor_obj[0x13] <= 4) {
        view_map();
        wild_pic_open();
    } else
        view_3d();
    if (cursor_obj[0x3a] != old_frame)
        frame_select(cursor_obj[0x3a]);
}

void far game_load_menu(void)                                     /* 2ffa:15c7 */
{
    char file[41];
    char k;
    unsigned char n;
    unsigned char pick;
    char far *name;
    char found[10];
    char is_dir;
    char far *ext;
    char path[80];

    frame_border();
    hotkey_reset(2);
    pick = 0;                   /* DQK's load-source menu is gone: case 0 */
    switch (pick) {
    case 0:
        file_kind = 1;
        ext = "csv";
        break;
    case 2:
        return;
    default:                    /* ext stays unset here, as shipped       */
        if (menu_escaped && pick == 0x1b)
            return;
    }
    mem_fill(found, sizeof found, 0);
    str_format(file, "%sX", "SavGam");
    path_default_ext(file, ext, 1);
    if (!save_ready())
        return;
    path[0] = 0;
    path_append_name(path, design_dir);
    path_append_name(path, "SAVE");
    dir_first(0, path, ext, 1, 0);
    while ((name = dir_next(&is_dir)) != 0)
        if (str_match("SavGam", name) && name[6] >= 'A' && name[6] <= 'J')
            found[name[6] - 'A'] = 1;
    mem_fill(hotkeys, sizeof hotkeys, 0xff);
    n = 0;
    k = 0;
    for (cur_slot = 'A'; cur_slot <= 'J'; cur_slot++) {
        if (found[cur_slot - 'A']) {
            hotkeys[n].key = k;
            n++;
        }
        k++;
    }
    if (hotkeys[0].key == 0xff)
        return;
    do {
        pick = menu_run(load_which_text, game_items, 0, 0);
        if (menu_escaped && pick == 0x1b)
            cur_slot = 0x1b;
        else
            cur_slot = pick + 'A';
    } while (cur_slot != 'A' && cur_slot != 'B' && cur_slot != 'C'
             && cur_slot != 'D' && cur_slot != 'E' && cur_slot != 'F'
             && cur_slot != 'G' && cur_slot != 'H' && cur_slot != 'I'
             && cur_slot != 'J' && cur_slot != 0x1b);
    if (cur_slot == 0x1b)
        return;
    str_find(file, '.')[-1] = cur_slot;
    box_draw();
    status_printf(0, 0x18, 7, 0, "Loading...Please Wait");
    switch (file_kind) {
    case 1:
        game_load_file(file);
        break;
    }
    box_draw();
    view_mode = 0;
    if (test_mode != 0) {
        cursor_obj[0x26] = geo.y;
        cursor_obj[0x25] = geo.x;
    } else {
        geo.x = cursor_obj[0x43];
        geo.y = cursor_obj[0x44];
        geo.facing = cursor_obj[0x11];
    }
    cur_level = cursor_obj[0x13];
}

char far game_write_cb(register int fd)                          /* 2ffa:1897 */
{
    struct Hero far *hero;
    long pos;
    short count;
    int skip;
    char err;

    err = dos_io(0x4000, cursor_obj + 1, fd, 0x400) != 0x400;
    err |= dos_io(0x4000, &geo, fd, 5) != 5;
    err |= dos_io(0x4000, &saved_view_mode, fd, 2) != 2;
    err |= dos_io(0x4000, &view_mode, fd, 2) != 2;
    err |= dos_io(0x4000, &skip, fd, 2) != 2;
    err |= dos_io(0x4000, &skip, fd, 2) != 2;
    count = 0;
    for (hero = node_list; hero != 0 && count < 8; hero = hero->next)
        count++;
    err |= dos_io(0x4000, &count, fd, 2) != 2;
    for (hero = node_list, count = 0; hero != 0 && count < 8; hero = hero->next, count++) {
        load_hero = hero;
        err |= !hero_write_cb(fd);
    }
    pos = file_seek(fd, 0L, 1);
    if (pos < 0x282d && dos_io(0x4000, item_templates, fd, 0x282d - pos) != 0x282d - pos)
        return 0;
    return !err;
}

void far game_save_menu(void)                                     /* 2ffa:1a85 */
{
    char letter;
    unsigned char pick;
    char file[42];

    cursor_obj[0x43] = geo.x;
    cursor_obj[0x44] = geo.y;
    cursor_obj[0x11] = geo.facing;
    cursor_obj[0x13] = cur_level;
    if (in_service_event == 0)
        cursor_obj[0x31] = 0;
    do {
        hotkey_reset(9);
        pick = menu_run(save_which_text, game_items, 0, view_mode == 2);
    } while (pick >= 10 && (!menu_escaped || pick != 0x1b));
    if (pick > 9)
        return;
    letter = pick + 'A';
    if (!save_ready())
        return;
    box_draw();
    status_printf(0, 0x18, 7, 0, "Saving...Please Wait");
    str_format(file, "%s%c", "SavGam", letter);
    path_default_ext(file, "csv", 1);
    file_kind = 1;
    if (save_write(file, game_write_cb)) {  /* always 1 */
        if (vault_slot != letter) {
            vault_load_current();
            vault_slot = letter;
            vault_save_current();
        }
        game_saved = 1;
    }
    box_draw();
}

void far party_character_append(struct Hero far *hero)                    /* 2ffa:1bac */
{
    struct Hero far *p;
    char used[8];

    hero->icon_slot = 0xff;
    if (node_list == 0)
        node_list = hero;
    else {
        for (p = node_list; p->next != 0; p = p->next)
            ;
        p->next = hero;
    }
    cur_node = hero;
    mem_fill(used, sizeof used, 0);
    for (p = node_list; p != 0; p = p->next)
        if (p->icon_slot >= 0 && p->icon_slot <= 7)
            used[p->icon_slot] = 1;
    for (cur_node->icon_slot = 0; cur_node->icon_slot < 8 && used[cur_node->icon_slot];
         cur_node->icon_slot++)
        ;
    cursor_obj[0x20]++;
    if (hero->control & 0x80)
        class_update(hero);
}

void far character_file_lists_free(struct Node far *list, struct Node far *files) /* 2ffa:1cba */
{
    list_free(&list);
    list_free(&files);
}

void far vault_load_current(void)                                     /* 2ffa:1cd7 */
{
    char name[42];

    pool_clear();
    if (vault_slot != 'Z') {
        str_format(name, "Vault%c.DAT", vault_slot);
        save_open(name, 0, pool_load);
    }
}

void far vault_save_current(void)                                     /* 2ffa:1d1b */
{
    char name[42];

    if (vault_slot != 'Z') {
        str_format(name, "Vault%c.DAT", vault_slot);
        save_write(name, pool_save);
    }
}
