/* CKIT.EXE resident module 281e: listing a directory through DOS find first
 * and find next (int 21h 4Eh, 4Fh) into one disk transfer area, dta.
 * file_find_first cuts the name out of a path into pattern, at most eight
 * characters and an extension of up to four with its '.', appends a
 * wildcard extension to both when the name has none, and starts the
 * search; file_find_next returns each next name that wild_match matches
 * against pattern, name_dot adding a '.' to a file name without one (not
 * to a directory or volume label), before the match when the pattern has
 * an extension and after it when not.  wild_match takes '*' and '?' and
 * compares a pattern character upper-cased with the name's.  The entry's
 * attribute, date and time, and size are read back by file_find_attrib,
 * find_stamp and find_size.  name_kind scans a whole path: it gives 0
 * when any byte is a control character, a space, past '~' or a ',', or when
 * the last component is empty or only dots; 2 when any byte is a wildcard;
 * else 1.
 *
 * EXE 0x01a5e7..0x01a952, one Turbo C large-model _TEXT segment starting at
 * byte 7 of paragraph 281e.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * pathutil.c's last routine ends at 0x01a5e7, and strscan.c starts at
 * 0x01a952 (2855:0002) with no stub between.  Ghidra's index misses
 * find_stamp and find_size.  Nothing found calls find_stamp,
 * find_size or name_kind (02fe): no far call, far jump, callback push
 * or far-pointer dword in CKIT.EXE names them.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  diskfile.c's placeholders
 * file_find_first, file_find_next and file_find_attrib name this module's own
 * routines; the callees reuse pathutil.c's and diskfile.c's names.
 */

extern char far *far str_find(char far *s, char c);          /* 00d7:258e */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern void far str_cat(char far *dst, char far *src);      /* 00d7:24b6 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */

struct Dta {                        /* DOS find first/next record, 43 bytes */
    char reserved[21];
    char attrib;                    /* +15 */
    long stamp;                     /* +16, time in the low word, date high */
    long size;                      /* +1a */
    char name[13];                  /* +1e */
};

static char pending = 0;            /* a6ee, find first's entry not yet read */

static struct Dta dta;                                           /* e708 */
static char pattern[13];                                         /* e733 */
static char has_ext;                                             /* e740 */

void far name_dot(void);
int far wild_match(char far *name, char far *pat);

int far file_find_first(char far *path, int attr)                   /* 281e:0007 */
{
    char far *q;
    char far *p;
    char far *mark;
    char buf[80];
    register int rc;

    p = path - 1;
    while (*++p)
        ;
    while (--p >= path && *p != ':' && *p != '/' && *p != '\\')
        ;
    p++;
    q = pattern;
    for (;;) {
        if (*p == 0) {
            has_ext = 0;
            str_copy(buf, path);
            str_cat(buf, ".*");
            *q = '.';
            q++;
            *q = '*';
            q++;
            path = buf;
            break;
        }
        if (*p == '.') {
            has_ext = 1;
            mark = q;
            while (*p && mark + 4 > q) {
                *q = *p;
                p++;
                q++;
            }
            break;
        }
        if (pattern + 8 > q) {
            *q = *p;
            q++;
        }
        p++;
    }
    *q = 0;
    dos_open(0x1a00, (char far *)&dta);
    rc = dos_open(0x4e00, path, 0, attr);
    pending = rc >= 0;
    if (pending || rc == -18)       /* DOS 12h, no more files */
        return 0;
    return rc;
}

char far *far file_find_next(void)                                 /* 281e:0166 */
{
    do {
        if (!pending) {
            dos_open(0x1a00, (char far *)&dta);
            if (dos_call(0x4f00) < 0)
                return 0;
        }
        pending = 0;
        if (has_ext)
            name_dot();
    } while (!wild_match(dta.name, pattern));
    if (!has_ext)
        name_dot();
    return dta.name;
}

void far name_dot(void)                                          /* 281e:01cb */
{
    char far *p;

    if (!(dta.attrib & 0x18)) {
        p = str_find(dta.name, '.');
        if (!*p) {
            *p = '.';
            p++;
            *p = 0;
        }
    }
}

int far wild_match(char far *name, char far *pat)                /* 281e:020b */
{
    char far *p;

    while (*pat) {
        if (*pat == '*') {
            if (pat[1] == 0 || pat[1] == '.' && pat[2] == 0 ||
                pat[1] == '.' && pat[2] == '*' && pat[3] == 0)
                return 1;
            for (p = name; *p; p++)
                if (wild_match(p, pat + 1))
                    return 1;
            return 0;
        }
        if (*pat == '?') {
            if (*name == 0)
                return 0;
        } else if (*pat != *name && to_upper(*pat) != *name)
            return 0;
        pat++;
        name++;
    }
    return !*name;
}

int far file_find_attrib(void)                                       /* 281e:02e9 */
{
    return dta.attrib;
}

long far find_stamp(void)                                      /* 281e:02ee */
{
    return dta.stamp;
}

long far find_size(void)                                      /* 281e:02f6 */
{
    return dta.size;
}

int far name_kind(char far *s)                                   /* 281e:02fe */
{
    register int len;
    register int kind;

    len = 0;
    kind = 1;
    while (*s) {
        if (*s <= ' ' || *s > '~' || *s == ',')
            return 0;
        if (*s == '*' || *s == '?')
            kind = 2;
        if (*s == ':' || *s == '/' || *s == '\\')
            len = 0;
        else if (*s != '.')
            len++;
        s++;
    }
    return len ? kind : 0;
}
