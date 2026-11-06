/* CKIT.EXE resident module 20f8: scrolling and saving a screen window, and
 * loading a font.  win_scroll moves a window's pixels by a row and column
 * offset one pixel row at a time, in at most one pass after
 * video_plane_set(0xffef) -- none when video_planes() returns 0 -- and, when
 * attr is not negative, fills a row strip and a column strip, either of
 * which may be empty.  win_save hands the window, whole, to 00d7:0286 with
 * a far pointer into the video page draw_seg is not; "save" is a
 * placeholder, and nothing calls it.  font_load reads a .FON file: 0x480
 * bytes into font_cells, then 0x180 more into a far block allocated on
 * first use.
 *
 * EXE 0x013388..0x0137bb, one Turbo C large-model _TEXT segment starting at
 * byte 8 of paragraph 20f8.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 207e's last retf is at 0x013387, and module 213b starts at
 * 0x0137bb (213b:000b) with no stub between.
 *
 * _DATA is 9eb6..9ebd, col_round and "FON"; 9eb5 is pad after module 207e's
 * _DATA, and 9ebe is module 213b's.  There is no _BSS.  font_cells and
 * font_ext are extern: module 1f5e reads both (0x011e68, 0x011e2a).
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  A name another module here gives
 * an address is reused from there; a modNNNN_ name is a placeholder that
 * says only which segment the callee lives in.  display_seg and mem_top_seg
 * are the two video page segments (0xa000 and 0xa400 as overlay code at
 * 0x03b376 sets them), which 1f5e:0369 swaps after moving the CRTC start
 * address, so display_seg is the page shown; draw_seg is the segment the
 * drawing routines write through (1f5e:03aa sets it to either page), and
 * screen_addr the byte offset video_address_set computes for a row and column.
 */

extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern void far text_offset_xy(int row, int col, int row2, int col2,
                             int far *out_row, int far *out_col); /* 107e:0565 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern int  far screen_cols(void);                              /* 0f5e:05d4 */
extern void far video_plane_set(int plane);                         /* 0f5e:05ef */
extern void far video_address_set(int row, int col);                  /* 0f5e:0708 */
extern void far pointer_show(void);                              /* 0f5e:0d1c */
extern void far pointer_hide_rect(int row1, int col1, int row2, int col2); /* 0f5e:0e12 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far far_move(unsigned src, unsigned src_seg,
                             unsigned dst, unsigned dst_seg, int n); /* 00d7:0918 */
extern void far blit_bytes(void far *p, int a, int b, int c);  /* 00d7:0286 */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern char far path_default_ext(char far *path, char far *ext, char force); /* 17a1:0264 */
extern void far file_open(char disk, char far *name, int flags,
                          char (far *fn)(int fd, char far *path)); /* 0e23:03fa */

extern unsigned char font_cells[0x480];                          /* 97e0 */
extern unsigned char far *font_ext;                              /* 9c60 */
extern unsigned draw_seg;                                        /* d60d */
extern unsigned display_seg;                                     /* d71f */
extern unsigned mem_top_seg;                                     /* d721 */
extern unsigned screen_addr;                                     /* d75f */

static char col_round[4] = { 0, 1, 3, 7 };  /* 9eb6, (1 << n) - 1 per pixel_shift() */

void far win_scroll(int row1, int col1, int row2, int col2,
                    int drow, int dcol, int attr)                /* 20f8:0008 */
{
    int src;
    int i;
    int from_col;
    int to_col;
    int width;
    register int from;
    register int to;

    text_xy(row1, col1, &row1, &col1);
    text_xy(row2, col2, &row2, &col2);
    text_offset_xy(row1, col1, drow, dcol, &drow, &dcol);
    drow -= row1;
    dcol -= col1;
    if (dcol > 0) {
        from_col = col1 + dcol;
        to_col = col1;
        width = ((col_round[pixel_shift()] + col2) >> pixel_shift()) -
                (from_col >> pixel_shift());
    } else {
        from_col = col1;
        to_col = col1 - dcol;
        width = ((col_round[pixel_shift()] + col2) >> pixel_shift()) -
                (to_col >> pixel_shift());
    }
    pointer_hide_rect(row1, col1, row2, col2);
    for (i = 0; i < video_planes(); i++) {
        if (i) {
            video_plane_set(0);
            break;
        }
        video_plane_set(0xffef);
        if (drow > 0) {
            for (from = row1 + drow, to = row1; from < row2; from++, to++) {
                video_address_set(from, from_col);
                src = screen_addr;
                video_address_set(to, to_col);
                far_move(src, draw_seg, screen_addr, draw_seg, width);
            }
        } else {
            from = row2 + drow;
            to = row2;
            while (--from >= row1) {
                to--;
                video_address_set(from, from_col);
                src = screen_addr;
                video_address_set(to, to_col);
                far_move(src, draw_seg, screen_addr, draw_seg, width);
            }
        }
    }
    pointer_show();
    if (attr >= 0) {
        if (drow > 0)
            fill_rect(row2 - drow, col1, row2, col2, attr);
        else
            fill_rect(row1, col1, row1 - drow, col2, attr);
        if (dcol > 0)
            fill_rect(row1, col2 - dcol, row2, col2, attr);
        else
            fill_rect(row1, col1, row2, col1 - dcol, attr);
    }
}

void far win_save(int row1, int col1, int row2, int col2)       /* 20f8:0256 */
{
    long base;
    register int width;
    register int height;

    text_xy(row1, col1, &row1, &col1);
    text_xy(row2, col2, &row2, &col2);
    height = row2 - row1;
    width = ((col_round[pixel_shift()] + col2) >> pixel_shift()) -
            (col1 >> pixel_shift());
    if (height <= 0 || width <= 0)
        return;
    if (draw_seg == display_seg)
        base = mem_top_seg;
    else
        base = display_seg;
    base <<= 16;
    pointer_hide_rect(row1, col1, row2, col2);
    video_plane_set(0xffef);
    video_address_set(row1, col1);
    blit_bytes((void far *)(screen_addr + base), height, width,
                 (screen_cols() >> pixel_shift()) - width);
    video_plane_set(0);
    pointer_show();
}

char far font_read_cb(int fd, char far *path)                    /* 20f8:0368 */
{
    char buf[0x180];

    if (dos_io(0x3f00, font_cells, fd, 0x480) != 0x480)
        return 0;
    if (dos_io(0x3f00, buf, fd, 0x180) == 0x180) {
        if (font_ext == 0)
            font_ext = mem_alloc(0x180);
        if (font_ext != 0)
            mem_move(buf, font_ext, 0x180);
    }
    return 1;
}

void far font_load(char disk, char far *name)                    /* 20f8:03f1 */
{
    char path[80];

    str_copy(path, name);
    path_default_ext(path, "FON", 1);
    file_open(disk, path, 0, font_read_cb);
}
