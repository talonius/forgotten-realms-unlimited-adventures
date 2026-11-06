/* CKIT.EXE overlay section 24, module 2e44: writing a cache slot's group to a file.
 * slot_save is the write-side twin of fcache.c's slot_load: it checks the
 * slot as fcache.c's slot_check does ("Invalid group number" for a slot
 * outside 0..47, "Refrencing a non-loaded group" for one whose slot_of
 * entry is negative; the misspelling is shipped), keeps the slot's group
 * in save_grp and passes the disk, the path, flags 1 and save_cb to
 * file_open.  save_cb, the callback file_open hands the open file, copies
 * the group (grp_ptrs[save_grp] up to grp_ptrs[save_grp + 1], measured with
 * huge_diff) through a 512-byte stack buffer and writes it to the handle
 * with DOS function 40h, 512 bytes at a time; it returns 0 when fewer
 * than n bytes are written (a short or failed write), else 1.  Module 2d91
 * (pending, artimp.c) calls slot_save once, with 0, the
 * path of the library file it has just created and slot 0x18.
 *
 * Section 24 (parent section 20, which is empty and has no parent; no
 * section names 24 as its parent): payload at EXE 0x07b5b0, link segment
 * 2d91, which sections 19 to 26 share.  A bare stored SEG:OFF in this file
 * means section 24.  The module is stored frame 2e44 (load-based 3e44),
 * EXE 0x07c0ec..0x07c230 (324 bytes, 2 routines), from byte c of the frame,
 * right after module 2d91's last retf at 2d91:0b3b, to its last retf at
 * 2e44:014f; module 2e59 starts at 2e59:0000 (EXE 0x07c230, push bp) with
 * no pad between.  One Turbo C large-model _TEXT segment, built with Turbo
 * C 2.01, tcc -ml -O -Z -d; functions are defined in shipped order.
 * Definition comments give the load-based address; extern comments the far
 * pointer as stored in the shipped call, or the DGROUP offset.
 *
 * Section 24's relocation table files 7 sites under 2e44, all in this range
 * (EXE 0x07c12c..0x07c228), all far calls into resident code; the next
 * site, EXE 0x07c23c, is filed under 2e59.  Nothing reaches save_cb but
 * slot_save's push cs; mov ax,000c.  The one far call to 2e44 in the image
 * is artimp.c's to slot_save (EXE 0x07bf9b); no vector lands here, and
 * section 24 has no descendants.  save_cb is static by choice: the bytes
 * do not decide it.
 *
 * FRUA's own code: the sharing map pairs both routines a-only (slot_save's
 * top DQK routine is fcache.c's slot_open, at 51% coverage), so every name
 * here is chosen from the code.
 *
 * _DATA is DGROUP a382..a3be, the two error texts, between fcache.c's
 * (a210..a380, then a pad byte at a381) and digisnd.c's in_library (a3c0,
 * after a pad byte at a3bf).  _BSS is save_grp at e668, between fcache.c's
 * (e294..e667) and digisnd.c's lib_slot (e66a): this module was linked
 * between those two resident modules.
 *
 * Forced by the bytes, each against the module's two ranges: save_grp an
 * int (char, 0 of 2); the slot_of test < 0 (fcache.c's & 0x80, 1 of 2);
 * left -= n (left = left - n, 0 of 2); the chunk as left >= 0x200 ? 0x200
 * : left (> 0x1ff, > 0x200 or the < 0x200 form, 1 of 2 each); n an int
 * (short, 0 of 2); size, stored and never read (without it, 0 of 2); the
 * locals declared p, size, left (reversed, 1 of 2).
 *
 * Not decided by the bytes (each bakes 2 of 2): register or plain int n;
 * register or plain slot; save_grp int or unsigned; one chained or two separate assignments of size
 * and left; save_cb static or public, with or without its unused path.
 */

extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern void far error_box(char far *fmt, ...);                   /* 0da7:03c8 */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */
extern long far huge_diff(char far *a, char far *b);             /* 1336:1001 */

extern char slot_of[48];                                         /* e564 */
extern char far *grp_ptrs[49];                                   /* e596 */

static int save_grp;                                             /* e668 */

static char far save_cb(int fd, char far *path)                  /* 3e44:000c */
{
    char far *p;
    long size;
    long left;
    char buf[0x200];
    register int n;

    p = grp_ptrs[save_grp];
    left = size = huge_diff(grp_ptrs[save_grp + 1], grp_ptrs[save_grp]);
    while (left > 0) {
        n = left >= 0x200 ? 0x200 : left;
        huge_move(p, buf, n);
        if (dos_io(0x4000, buf, fd, n) < n)
            return 0;
        p = huge_add(p, (long)n);
        left -= n;
    }
    return 1;
}

void far slot_save(char disk, char far *path, int slot)          /* 3e44:00f8 */
{
    if (slot < 0 || slot >= 48)
        error_box("Invalid group number (%d)", slot);
    if (slot_of[slot] < 0)
        error_box("Refrencing a non-loaded group (%d)", slot); /* sic */
    save_grp = slot_of[slot];
    file_open(disk, path, 1, save_cb);
}
