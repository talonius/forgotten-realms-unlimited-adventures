/* CKIT.EXE resident module 27d3: taking paths apart and putting them
 * together.  cur_dir writes the current drive letter, ':', a separator and
 * the current directory (int 21h 19h, 47h); change_dir changes drive and
 * directory (0Eh, 3Bh), returning -1 for a drive letter at or past the
 * drive count 0Eh reports; full_path makes an absolute path from one
 * relative to the current directory, dropping "./", climbing "../" by
 * hand and adding a '.' to a name with no extension; path_dir, path_name
 * and path_ext cut out the directory, the name without its extension and
 * the extension, and path_base_copy the name with it; path_base finds where
 * the name starts, path_has_dir says whether a '/', '\\' or ':' occurs,
 * and path_append_name and path_in_dir join a directory and the name's last
 * component.  A separator added is path_sep, cur_dir and full_path turn
 * each '\\' DOS returns into it, change_dir sets it to '/' when the path
 * holds one, and path_base when the last separator is one.
 *
 * EXE 0x01a139..0x01a5e7, one Turbo C large-model _TEXT segment starting at
 * byte 9 of paragraph 27d3.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * The retf at 0x01a138 (27d3:0008) is module 27d0's: overlay section 25
 * far-calls it as 17d0:0038 (3e1a:0197).  Module 281e starts at 0x01a5e7
 * (281e:0007) with no stub between.  All 30 MZ relocation sites inside
 * it have frame 17d3.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  A name another module here gives
 * an address is reused from there: sound.c's path_name and path_ext,
 * fcache.c's placeholder path_base_copy and diskfile.c's path_append_name for this
 * module's own routines.  A modNNNN_ name is a placeholder that says only
 * which segment the callee lives in.  path_sep is the name DQK's pathfind.c
 * defines for it; frame 27a1 reads it at 27a1:0110 and sets it to '/' at
 * 02b1.  Nothing found calls cur_dir (0009), change_dir (006b), full_path
 * (00fd) or path_dir (024f): no far call, far jump, callback push or
 * far-pointer dword in CKIT.EXE names any of them.
 * path_in_dir (0355) is far-called once, from overlay section 24 at
 * 3d91:02d5.
 *
 * The one _DATA byte is the empty extension full_path hands to
 * path_default_ext (a6ec); a6ed is padding.  The module has no _BSS.
 */

extern char far *far str_find(char far *s, char c);          /* 00d7:258e */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_len(char far *s);                       /* 00d7:2577 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */
extern char far path_default_ext(char far *path, char far *ext, char force); /* 17a1:0264 */

extern char path_sep;                                            /* a6de */

char far *far path_base(char far *path);
char far path_has_dir(char far *path);
void far path_append_name(char far *dir, char far *name);

void far cur_dir(char far *buf)                                  /* 27d3:0009 */
{
    buf[0] = dos_call(0x1900) + 'A';
    buf[1] = ':';
    buf[2] = path_sep;
    buf[3] = 0;
    dos_call(0x4700, buf[0] - '@', buf + 3);
    while (*++buf)
        if (*buf == '\\')
            *buf = path_sep;
}

int far change_dir(char far *path)                               /* 27d3:006b */
{
    char drive;

    if (*str_find(path, '/'))
        path_sep = '/';
    if (*path && path[1] == ':') {
        drive = to_upper(*path) - 'A';
        if ((char)dos_call(0x0e00, to_upper(*path) - 'A') <= drive) /* byte cmp */
            return -1;
        path += 2;
    }
    if (*path)
        return dos_open(0x3b00, path);
    return 0;
}

void far full_path(char far *path, char far *out)               /* 27d3:00fd */
{
    if (*path && path[1] == ':') {
        *out = to_upper(*path);
        out++;
        path += 2;
    } else {
        *out = dos_call(0x1900) + 'A';
        out++;
    }
    *out = ':';
    out++;
    if (*path != '/' && *path != '\\') {
        out[0] = path_sep;
        out[1] = 0;
        dos_call(0x4700, out[-2] - '@', out + 1);
        for (; *out; out++)
            if (*out == '\\')
                *out = path_sep;
        if (out[-1] == '/' || out[-1] == '\\')
            out--;
        while (*path == '.') {
            if (path[1] == '.' && (path[2] == '/' || path[2] == '\\')) {
                do {
                    out--;
                    if (*out == ':') {
                        out++;
                        break;
                    }
                } while (*out != '/' && *out != '\\');
                path += 3;
            } else if (path[1] == '/' || path[1] == '\\')
                path += 2;
            else
                break;
        }
        *out = path_sep;
        out++;
    }
    str_copy(out, path);
    path_default_ext(out, "", 0);
}

void far path_dir(char far *path, char far *out)                 /* 27d3:024f */
{
    char far *end;

    str_copy(out, path);
    end = path_base(out);
    if (out + 2 <= end && out[1] == ':')
        out += 2;
    if (out + 1 < end)
        end--;
    *end = 0;
}

void far path_name(char far *path, char far *out)                /* 27d3:02b1 */
{
    str_copy(out, path_base(path));
    *str_find(out, '.') = 0;
}

void far path_ext(char far *path, char far *out)                 /* 27d3:02ea */
{
    char far *p;

    p = str_find(path_base(path), '.');
    if (*p)
        p++;
    str_copy(out, p);
}

void far path_base_copy(char far *path, char far *out)             /* 27d3:0334 */
{
    str_copy(out, path_base(path));
}

void far path_in_dir(char far *path, char far *dir, char force)  /* 27d3:0355 */
{
    char buf[80];

    if (force || !path_has_dir(path)) {
        str_copy(buf, dir);
        path_append_name(buf, path);
        str_copy(path, buf);
    }
}

void far path_append_name(char far *dir, char far *name)             /* 27d3:03af */
{
    char far *end;

    end = dir + str_len(dir);
    if (*dir && (dir[1] != ':' || dir[2]) && end[-1] != '/' && end[-1] != '\\') {
        *end = path_sep;
        end++;
    }
    str_copy(end, path_base(name));
}

char far *far path_base(char far *path)                          /* 27d3:042b */
{
    char far *p;

    p = path + str_len(path);
    while (--p >= path && *p != ':' && *p != '/' && *p != '\\')
        ;
    if (*p == '/')              /* reads path[-1] when none is found, as shipped */
        path_sep = '/';
    return p + 1;
}

char far path_has_dir(char far *path)                            /* 27d3:0489 */
{
    while (*path) {
        if (*path == '/' || *path == '\\' || *path == ':')
            return 1;
        path++;
    }
    return 0;
}
