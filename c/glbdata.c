/* CKIT.EXE overlay 3 module 1b41: data files and the mode switch.  Cleaning
 * a name of the characters a DOS file name cannot hold, reading a data file
 * into a caller's buffer -- a loose .dat file under the design directory
 * first, else one entry of a .glb library through file_open -- writing one
 * back, keeping the design directory and its start.dat record, and
 * switching the mode, which first calls what the mode being left names.
 *
 * EXE 0x02bff0..0x02c467, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 1b41, the first module of RTLink section 3 (payload
 * 0x2bff0, link segment 1b41, no parent).  Its final retf is byte 6 of
 * paragraph 1b88, where the next module begins at 1b88:0007 (section 3's
 * table files every fixup site before it under frame 1b41, and the next
 * module's under 1b88).  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Section 0's 1b41 is combat.c, other bytes.
 *
 * A bare stored seg:off in this file means section 3.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee and variable
 * names are placeholders chosen from use; a modNNNN_ name says only which
 * segment the callee lives in, and an ovlNN_ name which section and stored
 * segment.  A mod18de_ callee is an RTLink vector stub; its comment names
 * the routine the stub reaches.  Sections 4 to 9, which descend from 3,
 * far-call seven of these routines directly; eight are vector targets:
 * 18de:13a5 -> 0083, 18de:1391 -> 012b, 18de:1387 -> 0136, 18de:137d ->
 * 027f, 18de:10cb -> 02fe, 18de:1369 -> 0354, 18de:1373 -> 037c,
 * 18de:139b -> 038f.
 *
 * This is DQK's glbdata.c relinked and grown.  str_strip, name_clean,
 * data_read and mode_change are DQK's C; the disk setter is DQK's shape,
 * and data_load keeps DQK's .glb path behind a loose-file read that DQK
 * does not have.  DQK's two empty routines are not here; the other seven
 * routines are FRUA's own.
 *
 * _DATA is design_dir, bad_chars, data_disk, byte_5a88, cur_mode and four
 * stored literals, DGROUP 5a54..5ab0; -d stores "%s%03d.dat" and
 * "start.dat" once each, though the source writes each twice.
 * _BSS is b220..b24f, in the order its names give (Turbo C's symbol-table
 * order), chosen with goldbox.bss_order.
 */

extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_upr(char far *s);                       /* 00d7:25f5 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern int  far lib_find(int fd, int id);                        /* 147e:0415 */
extern long far lib_find_item(int fd, int n);                    /* 147e:04c5 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */
extern void far pic_release(void);                           /* 1bbc:02b4 */
extern void far bigpic_release(void);                              /* 1bbc:03e0 */
extern void far wall_sets_free(char on);  /* 18de:13af, vector to overlay 13 26e3:1721 */
extern void far back_free(void);     /* 18de:13cd, vector to overlay 13 26e3:15da */

extern char ed_mode;                                           /* d136 */

char design_dir[0x22] = "";                                        /* 5a54 */
static char bad_chars[17] = " =+<>\"[].*,?/\\:;|";              /* 5a76 */
static char data_disk = '1';                                     /* 5a87 */
static char byte_5a88 = 1;                                       /* 5a88 */
static int cur_mode = -1;                                        /* 5a89 */

static int data_len;                                             /* b220 */
static char far *data_path;                                      /* b222 */
static char far *data_ptr;                                       /* b226 */
static int data_bank;                                            /* b22a */
static char saved_dir[0x22];                                     /* b22c */
static int data_entry;                                           /* b24e */

void far str_strip(char far *s, char c)                          /* 2b41:0000 */
{
    char far *d;

    d = s;
    while (*s != 0) {
        if (*s != c)
            *d++ = *s;
        s++;
    }
    *d = 0;
}

void far name_clean(char far *name)                              /* 2b41:003f */
{
    unsigned char i;

    for (i = 0; i < 17; i++)
        str_strip(name, bad_chars[i]);
    name[8] = 0;
    str_upr(name);
}

void far set_data_disk(char disk)                                 /* 2b41:0083 */
{
    data_disk = disk;
}

char far data_read(int fd, char far *path)                       /* 2b41:008e */
{
    int n;
    long size;

    if ((n = lib_find(fd, data_entry)) < 0
        || n == 0 && (n = lib_find(fd, data_bank * 100 + data_entry)) < 0)
        return 0;
    if (n == 0) {
        data_len = 0;
        return 1;
    }
    if ((size = lib_find_item(fd, n)) < 0
        || dos_io(0x3f00, data_ptr, fd, (int)size) != size)
        return 0;
    data_len = size;
    return 1;
}

void far glbdata_012b(char c)                                    /* 2b41:012b */
{
    byte_5a88 = c;
}

void far data_load(char far *name, unsigned char entry, unsigned far *len,
                   char far *buf)                                /* 2b41:0136 */
{
    int fd;
    char tmp[80];
    char path[80];

    str_format(path, "%s%03d.dat", name, entry);
    tmp[0] = 0;
    path_append_name(tmp, design_dir);
    path_append_name(tmp, path);
    str_copy(path, name);
    if ((fd = dos_open(0x3d00, tmp)) >= 0) {
        *len = dos_io(0x3f00, buf, fd, 0x7ffe);
        dos_call(0x3e00, 0, fd);
        if (*len >= 0)
            return;
    }
    str_format(tmp, "%s.glb", path);
    data_entry = entry;
    data_bank = 0;
    data_ptr = buf;
    data_path = tmp;
    file_open(data_disk, tmp, 0, data_read);
    *len = data_len;
}

char far data_write(int fd, char far *path)                    /* 2b41:0253 */
{
    return dos_io(0x4000, data_ptr, fd, data_len) == data_len;
}

void far data_save(char far *name, unsigned char entry, int len,
                      char far *buf)                             /* 2b41:027f */
{
    char tmp[80];
    char path[80];

    str_format(path, "%s%03d.dat", name, entry);
    tmp[0] = 0;
    path_append_name(tmp, design_dir);
    path_append_name(tmp, path);
    data_ptr = buf;
    data_len = len;
    file_open(0, tmp, 4, data_write);
}

void far mode_change(int mode)                                   /* 2b41:02fe */
{
    if (mode != cur_mode) {
        switch (cur_mode) {
        case 0:
            pic_release();
            wall_sets_free(0);
            back_free();
            break;
        case 2:
            pic_release();
            break;
        case 3:
            bigpic_release();
            break;
        }
    }
    cur_mode = mode;
    if (mode == 4)
        wall_sets_free(1);
}

void far design_dir_set(char far *dir)                             /* 2b41:0354 */
{
    str_copy(saved_dir, design_dir);
    str_copy(design_dir, dir);
}

void far design_dir_restore(void)                                      /* 2b41:037c */
{
    str_copy(design_dir, saved_dir);
}

void far start_save(void)                                      /* 2b41:038f */
{
    int fd;

    if ((fd = dos_open(0x3c00, "start.dat", 0, 0)) >= 0) {
        dos_io(0x4000, design_dir, fd, sizeof design_dir);
        dos_io(0x4000, &ed_mode, fd, 1);
        dos_call(0x3e00, 0, fd);
    }
}

void far start_load(void)                                   /* 2b41:03eb */
{
    int fd;

    design_dir[0] = 0;
    if ((fd = dos_open(0x3d00, "start.dat")) >= 0) {
        if (dos_io(0x3f00, design_dir, fd, sizeof design_dir) != sizeof design_dir)
            design_dir[0] = 0;
        else if (dos_io(0x3f00, &ed_mode, fd, 1) != 1)
            ed_mode = 0;
        dos_call(0x3e00, 0, fd);
    }
    if (!design_dir[0])
        str_copy(design_dir, "heirs.dsn");
}
