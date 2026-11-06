/* CKIT.EXE resident module 27a1: finding and naming files.  path_open opens
 * a file by name: first as given, then under each directory of the
 * environment's path (read through 00d7:2b69) and of an optional
 * ';'-separated list, and returns the handle or the first error.  A name
 * starting with '.', '/' or '\\' is tried only as given.  With try_exec
 * set, a name without an extension is tried with the extension exe and
 * then com instead.  file_size returns an open file's length and leaves
 * its position where it was; file_attr returns a file's DOS attributes
 * (int 21h ax=4300h hands them back in cx) or the negative error;
 * path_default_ext appends an extension to a name that has none, replaces
 * one only when force is set, and sets path_sep to '/' when the scan back
 * from the end meets a '/' first.
 *
 * EXE 0x019e19..0x01a103, one Turbo C large-model _TEXT segment starting at
 * byte 9 of paragraph 27a1.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 2793's last retf is at 0x019e18.  The retf at 0x01a103
 * (27a1:02f3) is module 27d0's: overlay section 25 far-calls it as
 * 17d0:0003.  All 14 MZ relocation sites inside the range have frame 17a1.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module here gives an address is reused from there;
 * a modNNNN_ name is a placeholder that says only which segment the routine
 * lives in.  Nothing found calls path_open (0009) or file_attr (023b): no
 * far call, far jump, callback push or far-pointer dword in CKIT.EXE names
 * either.
 *
 * path_default_ext returns a char: it loads al alone, and path_open tests
 * al alone.  Its prototype is in scope before path_open, which passes force
 * with mov al.
 *
 * path_sep (a6de) is public: module 27d3 reads and writes it, and its own
 * _DATA starts at a6ec.  try_exec (e706) is read here and written by no
 * direct operand found in the image; e707 is padding.
 */

extern void far env_get(char far *name, char far *buf);     /* 00d7:2b69 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */
extern long far file_seek(int fd, long pos, int whence);      /* 00d7:2705 */

char far path_default_ext(char far *path, char far *ext, char force);

char path_sep = '\\';                                            /* a6de */

static char try_exec;                                            /* e706 */

int far path_open(char far *name, char mode, char far *dirs,
                  char far *found)                               /* 27a1:0009 */
{
    char path[80];
    char list[256];
    char far *p;
    char far *q;
    register int fd;
    register int err;

    list[0] = 0;
    if (*name == '.' || *name == '/' || *name == '\\') {
        list[1] = 0;
    } else {
        env_get("path", list + 1);
        for (p = list; *++p; )
            if (*p == ';')
                *p = 0;
        if (dirs) {
            if (p[-1]) {
                *p = 0;
                p++;
            }
            str_copy(p, dirs);
            for (; *++p; )
                if (*p == ';')
                    *p = 0;
        }
        p[1] = 0;
    }
    p = list;
    err = 0;
    do {
        q = path;
        if (*p) {
            while ((*q = *p++) != 0)
                q++;
            if (q[-1] != '/' && q[-1] != '\\') {
                *q = path_sep;
                q++;
            }
        } else
            p++;
        str_copy(q, name);
        if (try_exec && path_default_ext(path, "exe", 0)) {
            if ((fd = dos_open(mode | 0x3d00, path)) >= 0)
                break;
            path_default_ext(path, "com", 1);
        }
        if ((fd = dos_open(mode | 0x3d00, path)) >= 0)
            break;
        if (err == 0)
            err = fd;
        else
            fd = err;
    } while (*p);
    if (fd >= 0 && found)
        str_copy(found, path);
    return fd;
}

long far file_size(int fd)                                    /* 27a1:01e6 */
{
    long pos;
    long end;

    pos = file_seek(fd, 0L, 1);
    end = file_seek(fd, 0L, 2);
    file_seek(fd, pos, 0);
    return end;
}

int far file_attr(char far *path)                                /* 27a1:023b */
{
    register int rc;
    register int attr;

    rc = dos_open(0x4300, path);
    attr = _CX;
    if (rc < 0)
        return rc;
    return attr;
}

char far path_default_ext(char far *path, char far *ext, char force) /* 27a1:0264 */
{
    char far *p;
    char far *dot;

    p = dot = path + str_len(path);
    while (--p > path) {
        if (*p == '.') {
            if (!force)
                return 0;
            dot = p;
            break;
        } else if (*p == '/') {
            path_sep = '/';
            break;
        } else if (*p == '\\')
            break;
    }
    *dot = '.';
    dot++;
    str_copy(dot, ext);
    return 1;
}
