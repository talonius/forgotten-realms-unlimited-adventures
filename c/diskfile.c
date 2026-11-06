/* CKIT.EXE resident module 1e23: opening files by disk letter, with the
 * insert-disk prompts, and walking a directory.  A disk letter names one of
 * the game disks ('S' the save disk); disk_path makes a file's path from
 * it, prefixing "A:\" or "B:\" after a failure and, with disk_subdirs set,
 * the "SAVE" or "DISKx" directory.  archive_open tries up to three times,
 * each failure moving that disk on to the next drive in disk_drive, which
 * holds five words though a letter indexes it up to 25.  file_open repeats
 * archive_open behind the "Insert ... and Press a Key" prompt ("'%s' not
 * found" for disk 0) until it succeeds or the player picks C (give up) or
 * Q (quit); ? shows the name.  The callback archive_open hands an open
 * file is called with the handle and the path it opened.
 * disk_msg shows the prompts, through disk_handler when disk_setup set one.
 * dir_first and dir_next list a disk's directories and the files whose
 * extension matches dir_ext; with disk 0, dir_first searches name's
 * directory, or an empty path when name is null.
 *
 * EXE 0x01063d..0x010cf9, one Turbo C large-model _TEXT segment starting at
 * byte d of paragraph 1e23.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1df0's last retf is at 0x01063c, and module 1e8f starts at
 * 0x010cf9 (1e8f:0009) with no stub between.
 *
 * _DATA is 96d0..9757, right after prompt.c's, and _BSS d592..d59c, right
 * after prompt_frame; no operand outside this module in the recovered
 * listings names either.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module gives an address is reused from there,
 * sound.c's archive_open and file_open for this module's own routines
 * included; a modNNNN_ name is a placeholder that says only which segment
 * the callee lives in.
 */

extern void far win_text_printf(char far *fmt, ...);             /* 0df0:017b */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern char far draw_page_visible(void);                              /* 0f5e:03d2 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern void far path_append_name(char far *path, char far *name);    /* 17d3:03af */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern int  far memory_above_320k(void);                              /* 107e:078c */
extern void far sfx_reset(void);                              /* 1782:0060 */
extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern void far beep(void);                                      /* 0da7:018c */
extern void far key_wait(void);                                  /* 0da7:00f5 */
extern int  far int_call(int n);     /* 00d7:288a, executes int n */
extern void far key_flush(void);                                 /* 0da7:00bd */
extern void far sfx_update(void);                              /* 1782:0092 */
extern char far key_pressed(void);                               /* 0da7:009a */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far sys_shutdown(void);                              /* 0da7:006c */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern int  far file_find_first(char far *path, int attr);          /* 181e:0007 */
extern char far * far file_find_next(void);                        /* 181e:0166 */
extern int  far file_find_attrib(void);                              /* 181e:02e9 */
extern void far path_ext(char far *path, char far *out);         /* 17d3:02ea */
extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */

static char disk_subdirs = 0;                                    /* 96d0 */
static char far *disk_arg = 0;          /* 96d1, no read of it found */
static char save_disk = 'S';                                     /* 96d5 */
static int disk_drive[5] = { 0 };       /* 96d6, indexed up to 25 */

static char msg_mode;                                            /* d592 */
static char far *dir_ext;                                        /* d593 */
static char dir_dirs;                                            /* d597 */
static char dir_files;                                           /* d598 */
static void (far *disk_handler)(int kind, int disk, char far *text); /* d599 */

void far disk_msg(char kind, char disk, char far *name)          /* 1e23:000d */
{
    char buf[100];

    switch (kind) {
    case 0:
        if (disk_handler)
            disk_handler(kind, disk, "");
        else
            win_text_printf("");
        draw_page_set(msg_mode);
        break;
    case 1:
        if (disk == 'S')
            disk = save_disk;
        if (disk)
            str_format(buf, "Insert %s and Press a Key",
                       disk != ' ' ? (disk != 'S' ? "Disk %c" : "SAVE Disk")
                                   : "the Disk");
        else
            str_format(buf, "'%s' not found", name);
        break;
    case 2:
        str_format(buf, "Disk %s error", disk ? "write" : "read");
        break;
    }
    if (kind) {
        msg_mode = draw_page_visible();
        draw_page_set(1);
        if (disk_handler)
            disk_handler(kind, disk, buf);
        else
            win_text_printf(buf, disk);
    }
}

void far disk_path(char far *out, char disk, int drive, char far *name) /* 1e23:013a */
{
    if (disk == 0) {
        str_copy(out, name);
        return;
    }
    *out = 0;
    if (drive) {
        *out = drive + '@';
        str_copy(out + 1, ":\\");
    }
    if (disk_subdirs) {
        if (disk == 'S')
            str_cat(out, "SAVE");
        else if (disk > ' ') {
            str_cat(out, "DISKA");
            out[str_len(out) - 1] = disk;
        }
    }
    path_append_name(out, name);
}

char far archive_open(char disk, char far *name, int flags,
                      char (far *fn)(int fd, char far *path))    /* 1e23:01eb */
{
    short tries;
    short drive;
    char path[79];
    char quiet;
    register int n;
    register int fd;

    if (disk & 0x80) {
        quiet = 1;
        disk &= 0x7f;
    } else if (disk >= 'a' && disk <= 'z') {
        quiet = 1;
        disk = to_upper(disk);
    } else
        quiet = 0;
    if (disk == 'S')
        n = 4;
    else if (disk >= 'A' && disk <= 'Z')
        n = disk - 'A';
    else if (disk >= '0' && disk <= '9')
        n = disk - '0';
    else
        n = 0;
    if (!memory_above_320k())
        sfx_reset();
    tries = 3;
    while (--tries >= 0) {
        drive = disk ? disk_drive[n] : 0;
        disk_path(path, disk, drive, name);
        if (flags == 4 ||
            (fd = dos_open((char)(flags == 3 ? 0 : flags) | 0x3d00, path)) >= 0) {
            if (flags == 1) {
                dos_call(0x3e00, 0, fd);
                fd = dos_open(0x3c00, path, 0, 0);
            } else if (flags == 4)
                fd = dos_open(0x3c00, path, 0, 0);
            if (fd >= 0 && (fn == 0 || fn(fd, path))) {
                dos_call(0x3e00, 0, fd);
                if (flags == 3)
                    dos_open(0x4100, path);
                return 1;
            }
            if (fd >= 0)
                dos_call(0x3e00, 0, fd);
            disk_msg(2, flags != 0, 0L);
            beep();
            key_wait();
            disk_msg(0, 0, 0L);
        }
        if (disk == 0 || quiet)
            break;
        if (++disk_drive[n] > 2)
            disk_drive[n] = 0;
        else if (disk_drive[n] == 2 && !(int_call(0x11) & 0xc0)) {
            disk_drive[n] = 0;          /* int 0x11 reports one floppy */
            tries--;
        }
    }
    return 0;
}

void far file_open(char disk, char far *name, int flags,
                   char (far *fn)(int fd, char far *path))       /* 1e23:03fa */
{
    register int key;

    while (!archive_open(disk, name, flags, fn)) {
        disk_msg(1, disk, name);
        beep();
        key_flush();
        while (!key_pressed())
            sfx_update();
        key = 0;
        if (key_poll())
            key = key_get();
        else
            key_flush();
        disk_msg(0, 0, 0L);
        if (key == 'Q') {
            sys_shutdown();
            dos_call(0x4c01);
        } else if (key == 'C')
            break;
        else if (key == '?')
            error_box("On '%s'", name);
    }
}

void far disk_setup(void (far *handler)(int kind, int disk, char far *text),
                    char subdirs, char far *arg, char save)      /* 1e23:04ae */
{
    disk_handler = handler;
    disk_subdirs = subdirs;
    disk_arg = arg;
    save_disk = save;
}

char far dir_first(char disk, char far *name, char far *ext,
                   char files, char dirs)                        /* 1e23:04d5 */
{
    char path[80];
    register int n;
    register int tries;

    dir_ext = ext;
    dir_dirs = dirs;
    dir_files = files;
    if (disk == 'S')
        n = 4;
    else if (disk >= 'A' && disk <= 'Z')
        n = disk - 'A';
    else if (disk >= '0' && disk <= '9')
        n = disk - '0';
    else
        n = 0;
    tries = 3;
    while (--tries >= 0) {
        if (disk)
            disk_path(path, disk, disk_drive[n], "*.*");
        else {
            str_copy(path, name ? name : "");
            path_append_name(path, "*.*");
        }
        if (file_find_first(path, dirs ? 0x10 : 0) >= 0)
            return 1;
        if (disk == 0)
            break;
        if (++disk_drive[n] > 2)
            disk_drive[n] = 0;
        else if (disk_drive[n] == 2 && !(int_call(0x11) & 0xc0)) {
            disk_drive[n] = 0;
            tries--;
        }
    }
    return 0;
}

char far * far dir_next(char far *is_dir)                        /* 1e23:0608 */
{
    char far *entry;
    char ext[80];
    register int dir;

    while ((entry = file_find_next()) != 0) {
        dir = file_find_attrib() & 0x10;
        if (dir && !dir_dirs)
            continue;
        if (!dir && !dir_files)
            continue;
        if (dir) {
            if (entry[0] == '.' &&
                (entry[1] == 0 || entry[1] == '.' && entry[2] == 0))
                continue;
        } else if (dir_ext) {
            path_ext(entry, ext);
            if (!str_same(dir_ext, ext))
                continue;
        }
        if (dir)
            *is_dir = 1;
        else
            *is_dir = 0;
        return entry;
    }
    return 0;
}
