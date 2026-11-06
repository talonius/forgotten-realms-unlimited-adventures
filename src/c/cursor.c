/* CKIT.EXE overlay 08 module 2673: the cursor mark drawn over cursor_obj's cell.
 *
 * EXE 0x045a84..0x045b17, one Turbo C large-model _TEXT segment starting at
 * byte 4 of paragraph 2673, the last module of RTLink section 8 (payload
 * 0x41090, link segment 21d4; a child of section 3).  The first 4 bytes of
 * paragraph 2673 are the tail of module 254b; nine zero bytes follow to the
 * section's end at 0x45b20.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 8.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Module 254b far-calls
 * cursor_show and cursor_hide; cursor_show is also reached through the
 * vector stub 18de:149f.  No vector and no far call reaches the two empty
 * routines.
 *
 * Cell coordinates are biased by 0x1f40, the form draw_glyph takes them
 * in; cursor_obj holds a zero-based column and row.  The module has no
 * _DATA or _BSS.
 */

extern void far rows_save(int row, int col, int height, int width);    /* 05f5:0af4 */
extern void far rows_restore(int row, int col, int height, int width); /* 05f5:0bc3 */
extern void far draw_glyph(int row, int col, int n, int glyph);        /* 11d7:0bd1 */

extern unsigned char far *cursor_obj;                                  /* b75b */

enum { OBJ_COL = 0x25, OBJ_ROW = 0x26 };

/* The translated Pascal unit's empty initialization block (DKK mapcurs.pas). */
void far cursor_unit_init(void)                                          /* 3673:0004 */
{
}

void far cursor_stub2(void)                                          /* 3673:0005 */
{
}

void far cursor_show(void)                                          /* 3673:0006 */
{
    rows_save((cursor_obj[OBJ_ROW] + 1) * 4 + 0x1f40,
              (cursor_obj[OBJ_COL] + 1) * 4 + 0x1f40, 4, 4);
    draw_glyph((cursor_obj[OBJ_ROW] + 1) * 4 + 0x1f40,
               (cursor_obj[OBJ_COL] + 1) * 4 + 0x1f40, 0, 0x19);
}

void far cursor_hide(void)                                          /* 3673:0067 */
{
    rows_restore((cursor_obj[OBJ_ROW] + 1) * 4 + 0x1f40,
                 (cursor_obj[OBJ_COL] + 1) * 4 + 0x1f40, 4, 4);
}
