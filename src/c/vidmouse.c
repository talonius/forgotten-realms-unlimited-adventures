/* CKIT.EXE resident module 1f5e: the display layer under the editor's screens
 * -- the two video pages, filling a rectangle, drawing a font glyph, the
 * screen offset of a pixel row and column, and the mouse pointer: its driver
 * set-up, button events, the pointer shape and saving and redrawing the
 * pixels under it.  A port of DQK's vidmouse.c (DQK module 22b7) with every
 * path but video mode 15's taken out: no routine here tests video_mode
 * except is_xvga and video_plane_set, and those still compare it with 15.
 *
 * EXE 0x0119e6..0x012be1, one Turbo C large-model _TEXT segment starting at
 * byte 6 of paragraph 1f5e: the module before it ends with the retf at
 * 1f5e:0005.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are
 * defined in shipped order; bake consumes them in that order.  Its 102
 * segment fixups are the 102 MZ relocation entries inside the range.
 * Module 207e starts at 0x012be1 (207e:0001).
 *
 * _DATA is DGROUP 97a0..9e7b: the edge masks, the built-in 96-glyph font
 * (12 bytes a glyph), the default pointer shape and three small mask
 * tables; module 207e's _DATA starts at 9e7c.  979e below it is a word
 * overlay section 6 writes (3431:00fe).  Nothing in this module reads
 * lmask_tab, rmask_tab, lmask8 or rmask8.  DQK's CGA colour map, which lies
 * between rmask_tab and font_cells in DQK, is not here: font_cells follows
 * rmask_tab directly.
 * _BSS is d602..d97e, DQK's layout moved by +0xbda: overlay code at
 * 3999:0560..059c writes and reads the word at d600, and module 207e's _BSS
 * starts at d980.
 * Turbo C lays _BSS out in its symbol-table order, so the variable names are
 * DQK's, which the layout constrains; pat, fore_bytes, plback_pat and
 * sprite_xpos are referenced by no code in CKIT.EXE and are kept for the
 * layout.  text_bytes (d746), base_offset (d75a) and extent_text (d75d) are
 * read by the runtime (frame 10d7, from 10d7:007d) and written by overlay
 * section 6, and spare_buffer (d972) is used by module 207e.  The runtime's
 * mouse callback 10d7:0b8b writes column_mouse, mousey_now and buttons_now
 * and far-calls mouse_event; the runtime reads draw_seg and screen_addr
 * (frame 10d7, from 10d7:0094), tests hide_count (10d7:0041, 10d7:01e8) and
 * decrements show_count (10d7:0141).
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module here gives an address is reused from there; a
 * modNNNN_ name is a placeholder that says only which segment the routine
 * lives in.
 *
 * Nothing found calls page_copy, screen_page_present, draw_redirect, pix_rem or
 * mouse_up_which: no far call or far jump in resident or overlay bytes, near
 * call, callback push or (offset, segment) word pair names them.
 */

#include <dos.h>

struct Pointer {                /* a mouse pointer: hot spot, size, and its masks per plane */
    char hot_row;               /* hot spot */
    char hot_col;
    char rows;
    char cols;
    unsigned char mask[4][0x40];
    unsigned char image[4][0x40];
};

extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern void far fill_pattern(int color, int far *out);           /* 11d7:0138 */
extern void far mask_words(int fill, int keep, int rows, int n); /* 00d7:03b0 */
extern void far fill_bytes(int rows, int n, int attr);         /* 00d7:0348 */
extern void far video_bios(int ah, int al, ...); /* 00d7:297b, int 10h: ah, al, bh, bl, cx, dh, dl */
extern void far poke_byte(int a, int b, int c);               /* 00d7:2858 */
extern void far far_move(unsigned src, unsigned src_seg,
                             unsigned dst, unsigned dst_seg, int n); /* 00d7:0918 */
extern void far reg_write(int port, int index, int value);    /* 00d7:0da0 */
extern void far dissolve_planes(unsigned from_seg, unsigned to_seg); /* 00d7:0e64 */
extern void far xvga_char(int a, int b, void far *p);         /* 00d7:2171 */
extern void far expand_1to8(void far *src, void far *dst, int n); /* 00d7:07cb */
extern void far pointer_hide(void);                              /* 00d7:0141 */
extern void far mouse_call_regs(int far *ax, int far *bx, int far *cx, int far *dx); /* 00d7:0b5a */
extern void far mem_move(void far *src, void far *dst, int n); /* 00d7:2741 */
extern long far sound_ticks(void);                              /* 107e:00c8 */
extern void far plane_split(char far *src, int n, char far *dst); /* 107e:074e */
extern void far sfx_tick(long ticks);                            /* 1782:00a1 */
extern int  far mouse_call(int fn, ...);                       /* 00d7:0b3d */
extern void far mouse_handler(void);                              /* 00d7:0b8b */
extern void far mouse_handler_set(void (far *fn)(void), int mask);    /* 00d7:0203 */
extern int  far peek_word(unsigned seg, unsigned off);        /* 00d7:286a */
extern void far grab_bytes(void far *buf, int rows, int cols); /* 00d7:0c7f */
extern void far blit_shifted(void far *image, void far *mask, int rows, int cols,
                             int skip, int lmask, int rmask, int edge); /* 00d7:0bbc */
extern void far put_bytes(void far *buf, int rows, int cols); /* 00d7:0c46 */

extern void far screen_page_show(void);                               /* 0f5e:02e2 */
extern void far draw_page_set(char mode);                          /* 0f5e:03aa */
extern void far video_plane_set(int plane);                          /* 0f5e:05ef */
extern void far video_address_set(int row, int col);                   /* 0f5e:0708 */
extern void far pointer_show(void);                               /* 0f5e:0d1c */
extern void far pointer_hide_rect(int row1, int col1, int row2, int col2); /* 0f5e:0e12 */
extern void far pointer_draw_over_buffer(char far *buf);                      /* 0f5e:0e80 */
extern void far pointer_restore_from_buffer(char far *buf);                      /* 0f5e:0ebd */
extern void far pointer_shape_set(struct Pointer far *p);              /* 0f5e:1189 */

extern int clip_top;                                             /* d988 */
extern int clip_left;                                            /* d98d */
extern int clip_right;                                           /* d98f */
extern int clip_bottom;                                          /* d993 */

static char pat[2];                                             /* d602 */
static int btn_col[2];                                          /* d604 */
static int btn_at[2];                                           /* d608 */
char dos_mode;                                                  /* d60c */
unsigned draw_seg;                                              /* d60d */
static unsigned prev_seg;                                       /* d60f */
static int left_up[2];                                          /* d611 */
static int fore_mask;                                           /* d615 */
static int right_up[2];                                         /* d617 */
int video_mode;                                                 /* d61b */
unsigned video_len;                                             /* d61d */
static char ptr_under[4][0x40];                                 /* d61f */
unsigned display_seg;                                           /* d71f */
unsigned mem_top_seg;                                           /* d721 */
static char btn1_press;                                         /* d723 */
static int bkgnd_bits;                                          /* d724 */
char visible_on;                                                /* d726 */
static int btn1_clicked[2];                                     /* d727 */
static char fore_bytes[4];                                      /* d72b */
int column_mouse;                                               /* d72f */
int hide_count;                                                 /* d731 */
static int mouse_click[2];                                      /* d733 */
static int map_planes;                                          /* d737 */
char back_screen;                                               /* d739 */
static int seen_color;                                          /* d73a */
static char left_fetched;                                       /* d73c */
int pix_shifts;                                                 /* d73d */
int pixel_cols;                                                 /* d73f */
static char rbtn_press;                                         /* d741 */
static char plback_pat[4];                                      /* d742 */
int text_bytes;                                                 /* d746 */
int shape_boot;                                                 /* d748 */
int show_count;                                                 /* d74a */
static char lifted_lbtn;                                        /* d74c */
static int lastfx_seen;                                         /* d74d */
static int left_arrow;                                          /* d74f */
int mousey_now;                                                 /* d751 */
static char right_taken;                                        /* d753 */
int letter_step;                                                /* d754 */
static int arrow_bytes;                                         /* d756 */
static int button_last;                                         /* d758 */
int base_offset;                                                /* d75a */
char saved_equip;                                               /* d75c */
int extent_text;                                                /* d75d */
unsigned screen_addr;                                           /* d75f */
static int kept_offset;                                         /* d761 */
int buttons_now;                                                /* d763 */
static int sprite_xpos;                                         /* d765 */
static char rbtn_released;                                      /* d767 */
static int pointer_state;                                       /* d768 */
static struct Pointer pointer_now;                              /* d76a */
static int sprite_trow;                                         /* d96e */
static int sprite_lskip;                                        /* d970 */
void far *spare_buffer;                                         /* d972 */
int screen_lines;                                               /* d976 */
int planes_count;                                               /* d978 */
int txthigh_dots;                                               /* d97a */
static int shape_height;                                        /* d97c */
static char mouse_present;                                      /* d97e */

static unsigned lmask_tab[16] = {                               /* 97a0 */
    0xffff, 0xff7f, 0xff3f, 0xff1f, 0xff0f, 0xff07, 0xff03, 0xff01,
    0xff00, 0x7f00, 0x3f00, 0x1f00, 0x0f00, 0x0700, 0x0300, 0x0100
};
static unsigned rmask_tab[16] = {                               /* 97c0 */
    0x0000, 0x0080, 0x00c0, 0x00e0, 0x00f0, 0x00f8, 0x00fc, 0x00fe,
    0x00ff, 0x80ff, 0xc0ff, 0xe0ff, 0xf0ff, 0xf8ff, 0xfcff, 0xfeff
};
unsigned char font_cells[0x480] = {                             /* 97e0 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* space */
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, 0x00, /* ! */
    0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* " */
    0x00, 0x36, 0x36, 0x7f, 0x36, 0x36, 0x7f, 0x36, 0x36, 0x00, 0x00, 0x00, /* # */
    0x38, 0x10, 0x7c, 0x7c, 0x10, 0x38, 0x38, 0x6c, 0x6c, 0xc6, 0x00, 0x00, /* $ */
    0x00, 0xc6, 0x06, 0x0c, 0x18, 0x18, 0x30, 0x60, 0x66, 0x00, 0x00, 0x00, /* % */
    0x00, 0x38, 0x6c, 0x6c, 0x38, 0x6c, 0x6f, 0xc6, 0xc6, 0x7f, 0x00, 0x00, /* & */
    0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' */
    0x0e, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0x0e, 0x00, 0x00, /* ( */
    0x70, 0x18, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x18, 0x70, 0x00, 0x00, /* ) */
    0x00, 0x00, 0x00, 0x66, 0x3c, 0x18, 0xff, 0x18, 0x3c, 0x66, 0x00, 0x00, /* asterisk */
    0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x00, 0x00, /* + */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x00, /* , */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, /* - */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, /* . */
    0x00, 0x06, 0x06, 0x0c, 0x18, 0x18, 0x30, 0x60, 0x60, 0x00, 0x00, 0x00, /* slash */
    0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* 0 */
    0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, /* 1 */
    0x3c, 0x66, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x30, 0x30, 0x7e, 0x00, 0x00, /* 2 */
    0x7e, 0x06, 0x06, 0x0c, 0x18, 0x0c, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x00, /* 3 */
    0x66, 0x66, 0x66, 0x66, 0x7e, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00, 0x00, /* 4 */
    0x7e, 0x60, 0x60, 0x60, 0x7c, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x00, /* 5 */
    0x3c, 0x66, 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* 6 */
    0x7e, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x18, 0x30, 0x30, 0x30, 0x00, 0x00, /* 7 */
    0x3c, 0x66, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* 8 */
    0x3c, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x00, /* 9 */
    0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, /* : */
    0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x00, 0x00, /* ; */
    0x00, 0x06, 0x0c, 0x18, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00, 0x00, 0x00, /* < */
    0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, /* = */
    0x00, 0x60, 0x30, 0x18, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00, 0x00, /* > */
    0x3c, 0x66, 0x06, 0x06, 0x0c, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, 0x00, /* ? */
    0x00, 0x7c, 0xc6, 0x92, 0xaa, 0x82, 0xaa, 0x92, 0x8c, 0x70, 0x00, 0x00, /* @ */
    0x3c, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, /* A */
    0x7c, 0x66, 0x66, 0x66, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, /* B */
    0x3c, 0x66, 0x66, 0x60, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3c, 0x00, 0x00, /* C */
    0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, /* D */
    0x7e, 0x60, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00, 0x00, /* E */
    0x7e, 0x60, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, /* F */
    0x3c, 0x66, 0x60, 0x60, 0x6e, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* G */
    0x66, 0x66, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, /* H */
    0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x00, /* I */
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* J */
    0x66, 0x66, 0x6c, 0x6c, 0x78, 0x6c, 0x6c, 0x66, 0x66, 0x66, 0x00, 0x00, /* K */
    0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00, 0x00, /* L */
    0x63, 0x77, 0x77, 0x7f, 0x6b, 0x6b, 0x63, 0x63, 0x63, 0x63, 0x00, 0x00, /* M */
    0x66, 0x66, 0x76, 0x76, 0x7e, 0x6e, 0x6e, 0x66, 0x66, 0x66, 0x00, 0x00, /* N */
    0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* O */
    0x7c, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, /* P */
    0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x36, 0x00, 0x00, /* Q */
    0x7c, 0x66, 0x66, 0x66, 0x7c, 0x6c, 0x6c, 0x66, 0x66, 0x66, 0x00, 0x00, /* R */
    0x3c, 0x66, 0x60, 0x70, 0x1c, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x00, /* S */
    0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, /* T */
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* U */
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00, 0x00, /* V */
    0x63, 0x63, 0x63, 0x63, 0x6b, 0x6b, 0x7f, 0x77, 0x77, 0x63, 0x00, 0x00, /* W */
    0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x3c, 0x66, 0x66, 0x66, 0x00, 0x00, /* X */
    0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, /* Y */
    0x7e, 0x06, 0x06, 0x0c, 0x18, 0x18, 0x30, 0x60, 0x60, 0x7e, 0x00, 0x00, /* Z */
    0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0x00, /* [ */
    0x00, 0x60, 0x60, 0x30, 0x18, 0x18, 0x0c, 0x06, 0x06, 0x00, 0x00, 0x00, /* backslash */
    0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c, 0x00, 0x00, /* ] */
    0x18, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ^ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, /* _ */
    0x18, 0x18, 0x18, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ` */
    0x00, 0x00, 0x00, 0x3c, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x3e, 0x00, 0x00, /* a */
    0x60, 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, /* b */
    0x00, 0x00, 0x00, 0x3c, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3c, 0x00, 0x00, /* c */
    0x06, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00, 0x00, /* d */
    0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x7e, 0x60, 0x60, 0x3e, 0x00, 0x00, /* e */
    0x1e, 0x30, 0x30, 0x30, 0x7c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, /* f */
    0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x7c, 0x00, /* g */
    0x60, 0x60, 0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, /* h */
    0x18, 0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, /* i */
    0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x70, 0x00, /* j */
    0x60, 0x66, 0x66, 0x6c, 0x78, 0x6c, 0x6c, 0x66, 0x66, 0x66, 0x00, 0x00, /* k */
    0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x00, 0x00, /* l */
    0x00, 0x00, 0x00, 0x76, 0x7f, 0x6b, 0x6b, 0x63, 0x63, 0x63, 0x00, 0x00, /* m */
    0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, /* n */
    0x00, 0x00, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x00, /* o */
    0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x00, /* p */
    0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x00, /* q */
    0x00, 0x00, 0x00, 0x7c, 0x66, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, /* r */
    0x00, 0x00, 0x00, 0x3e, 0x60, 0x60, 0x3c, 0x06, 0x06, 0x7c, 0x00, 0x00, /* s */
    0x18, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0e, 0x00, 0x00, /* t */
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x00, 0x00, /* u */
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00, 0x00, /* v */
    0x00, 0x00, 0x00, 0x63, 0x63, 0x63, 0x6b, 0x6b, 0x7f, 0x36, 0x00, 0x00, /* w */
    0x00, 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x38, 0x38, 0x6c, 0xc6, 0x00, 0x00, /* x */
    0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x7c, 0x00, /* y */
    0x00, 0x00, 0x00, 0x7e, 0x0c, 0x0c, 0x18, 0x30, 0x30, 0x7e, 0x00, 0x00, /* z */
    0x1c, 0x30, 0x30, 0x30, 0x60, 0x60, 0x30, 0x30, 0x30, 0x1c, 0x00, 0x00, /* { */
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, /* | */
    0x38, 0x0c, 0x0c, 0x0c, 0x06, 0x06, 0x0c, 0x0c, 0x0c, 0x38, 0x00, 0x00, /* } */
    0x00, 0x00, 0x00, 0x33, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ~ */
    0x00, 0x18, 0x34, 0x34, 0x62, 0x62, 0xc1, 0xc1, 0xff, 0x00, 0x00, 0x00  /* DEL */
};
unsigned char far *font_ext = 0;                                /* 9c60 */
static struct Pointer pointer = {                               /* 9c64 */
    0, 0, 16, 16,
    { {
        0x3f, 0xff, 0x1f, 0xff, 0x0f, 0xff, 0x07, 0xff,
        0x03, 0xff, 0x01, 0xff, 0x00, 0xff, 0x00, 0x7f,
        0x00, 0x3f, 0x00, 0x3f, 0x01, 0xff, 0x10, 0xff,
        0x30, 0xff, 0xf8, 0x7f, 0xf8, 0x7f, 0xfc, 0x7f
    } },
    { {
        0x00, 0x00, 0x40, 0x00, 0x60, 0x00, 0x70, 0x00,
        0x78, 0x00, 0x7c, 0x00, 0x7e, 0x00, 0x7f, 0x00,
        0x7f, 0x80, 0x7c, 0x00, 0x6c, 0x00, 0x46, 0x00,
        0x06, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00
    } }
};
static char pix_round[4] = { 0, 1, 3, 7 };                      /* 9e68 */
static unsigned char lmask8[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 }; /* 9e6c */
static unsigned char rmask8[8] = { 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80 }; /* 9e74 */

void far video_restore(void)                                      /* 1f5e:0006 */
{
    poke_byte(0x40, 0x10, saved_equip);
    video_bios(0, dos_mode);
    video_bios(1, 0, 0, 0, shape_boot);
}

void far fill_rect(int row1, int col1, int row2, int col2, register int attr) /* 1f5e:0040 */
{
    short lmask;
    char patterned;
    char cga4;

    text_xy(row1, col1, &row1, &col1);
    text_xy(row2, col2, &row2, &col2);
    row1 = int_max(row1, clip_top);
    col1 = int_max(col1, clip_left);
    row2 = int_min(row2, clip_bottom);
    col2 = int_min(col2, clip_right);
    if (row1 >= row2 || col1 >= col2)
        return;
    patterned = cga4 = 0;         /* 1f5e:00d4: stored, never read, as shipped */
    if (visible_on)
        pointer_hide_rect(row1, col1, row2, col2);
    if ((attr & 0xff) < 0x10) {
        fill_pattern(attr & 0xf, &lmask);
        attr = (attr & 0x100) | lmask;
    }
    if (attr & 0x100)
        for (; col1 < col2; col1++) {
            video_address_set(row1, col1);
            video_plane_set(col1 & 3);
            mask_words(attr & 0xff, 0xffff, row2 - row1, 2);
        }
    if (col1 & 3) {
        video_address_set(row1, col1);
        for (; (col1 & 3) && col1 < col2; col1++) {
            video_plane_set(col1 & 3);
            fill_bytes(row2 - row1, 1, attr);
        }
    }
    if (col2 - col1 >= 4) {
        video_address_set(row1, col1);
        video_plane_set(0xffee);
        fill_bytes(row2 - row1, (col2 - col1) >> 2, attr);
        col1 = col2 & 0xfffc;
    }
    if (col2 > col1) {
        video_address_set(row1, col1);
        for (; col2 > col1; col1++) {
            video_plane_set(col1 & 3);
            fill_bytes(row2 - row1, 1, attr);
        }
    }
    if (visible_on)
        pointer_show();
}

void far page_copy(void)                                      /* 1f5e:024b */
{
    char buf[0x100];

    pointer_hide_rect(-1, -1, -1, -1);
    pointer_draw_over_buffer(buf);
    dissolve_planes(mem_top_seg, display_seg);
    pointer_restore_from_buffer(0L);
    pointer_show();
}

void far screen_page_save(void)                                      /* 1f5e:0291 */
{
    pointer_hide_rect(-1, -1, -1, -1);
    video_plane_set(0xffef);
    far_move(0, display_seg, 0, mem_top_seg, video_len);
    video_plane_set(0);
    pointer_restore_from_buffer(0L);
    pointer_show();
}

void far page_swap(void)                                      /* 1f5e:02dc */
{
    screen_page_show();
}

void far screen_page_show(void)                                      /* 1f5e:02e2 */
{
    char buf[0x100];
    register unsigned seg;

    pointer_hide_rect(-1, -1, -1, -1);
    pointer_draw_over_buffer(buf);
    reg_write(0x3c4, 4, 6);
    reg_write(0x3d4, 0x17, 0xc3);
    reg_write(0x3d4, 0x14, 0);
    reg_write(0x3d4, 0xc, display_seg == 0xa000 ? 0x40 : 0);
    while (!(inportb(0x3da) & 8))         /* in al,dx at 0x011d44: wait for vertical retrace */
        ;
    seg = display_seg;
    display_seg = mem_top_seg;
    mem_top_seg = seg;
    back_screen = 1 - back_screen;
    draw_page_set(visible_on);
    pointer_restore_from_buffer(buf);
    pointer_show();
}

void far screen_page_present(void)                                      /* 1f5e:03a1 */
{
    screen_page_show();
    screen_page_save();
}

void far draw_page_set(char mode)                                 /* 1f5e:03aa */
{
    if (mode)
        draw_seg = display_seg;
    else
        draw_seg = mem_top_seg;
    visible_on = mode ? 1 : 0;
}

char far draw_page_visible(void)                                      /* 1f5e:03d2 */
{
    return visible_on;
}

int far draw_char(int row, int col, char c, char attr, char flags) /* 1f5e:03d6 */
{
    short bg;
    char rows[8];
    char plane[0x40];
    unsigned char far *g;
    register int i;
    register int fg;

    text_xy(row, col, &row, &col);
    if (row < 0) {
        seen_color = lastfx_seen = 0x100;
        return 0;
    }
    if (row < clip_top || col < clip_left || row + txthigh_dots + txthigh_dots > clip_bottom
        || col + letter_step + letter_step > clip_right)
        return 0;
    c &= 0x7f;
    if (c < 0x20 && font_ext == 0)
        c = 0;
    else
        c -= 0x20;
    if (c < 0)
        g = font_ext + (c + 0x20) * 12;
    else
        g = font_cells + c * 12;
    rows[0] = *g;
    g++;
    rows[1] = *g;
    g++;
    g++;
    rows[2] = *g;
    g++;
    rows[3] = *g;
    g++;
    g++;
    rows[4] = *g;
    g++;
    g++;
    rows[5] = *g;
    g++;
    rows[6] = *g;
    g++;
    rows[7] = *g;
    if (attr != seen_color || flags != lastfx_seen) {
        seen_color = attr;
        lastfx_seen = flags;
        fg = attr & 0xf;
        bg = (attr >> 4) & 0xf;
        fill_pattern(fg, &fore_mask);
        fill_pattern(bg, &bkgnd_bits);
        fore_mask ^= bkgnd_bits;
    }
    i = 0;                        /* 1f5e:0559: the loop below never runs, as shipped */
    while (--i >= 0)
        plane[i] = plane[i] & fore_mask ^ bkgnd_bits;
    if (visible_on)
        pointer_hide_rect(row, col, row + 8, col + 8);
    video_address_set(row, col);
    xvga_char(fore_mask ^ bkgnd_bits, bkgnd_bits, rows);
    if (visible_on)
        pointer_show();
    return letter_step << 1;
}

int far screen_rows(void)                                       /* 1f5e:05d0 */
{
    return screen_lines;
}

int far screen_cols(void)                                       /* 1f5e:05d4 */
{
    return pixel_cols;
}

int far pixel_shift(void)                                       /* 1f5e:05d8 */
{
    return pix_shifts;
}

int far video_planes(void)                                       /* 1f5e:05dc */
{
    return planes_count;
}

char far is_xvga(void)                                      /* 1f5e:05e0 */
{
    return video_mode == 15;
}

void far video_plane_set(int plane)                                 /* 1f5e:05ef */
{
    map_planes = plane;
    if (plane >= 0) {
        reg_write(0x3c4, 2, 1 << plane);
        reg_write(0x3ce, 4, plane);
        reg_write(0x3ce, 1, 0);
    } else if (plane < -0x10) {
        reg_write(0x3c4, 2, 0xf);
        if (plane >= -0x11) {
            reg_write(0x3ce, 5, video_mode == 15 ? 0x41 : 1);
            return;
        }
    } else {
        reg_write(0x3c4, 2, 0xf);
        reg_write(0x3ce, 0, -plane - 1);
        reg_write(0x3ce, 1, 0xf);
    }
    reg_write(0x3ce, 5, video_mode == 15 ? 0x40 : 0);
}

void far draw_redirect(char far *buf)                            /* 1f5e:06d6 */
{
    if (buf) {
        kept_offset = screen_addr;
        prev_seg = draw_seg;
        screen_addr = (int)buf;
        draw_seg = _DS;
    } else {
        screen_addr = kept_offset;
        draw_seg = prev_seg;
    }
}

void far video_address_set(int row, int col)                          /* 1f5e:0708 */
{
    if (col < 0 && row <= 0)
        row = col = 0;
    screen_addr = (row << 6) + (row << 4) + (col >> 2);
}

void far mouse_event(events)                                     /* 1f5e:073c */
char events;                    /* old-style: callers push mov ax,imm, and the body tests a byte */
{
    if (events & 2) {
        btn_at[0] = mousey_now;
        btn_col[0] = column_mouse;
        btn1_press = 1;
        lifted_lbtn = 0;
    }
    if (events & 4) {
        left_up[0] = mousey_now;
        left_up[1] = column_mouse;
        lifted_lbtn = 1;
        if (left_fetched) {
            btn1_clicked[0] = mousey_now;
            btn1_clicked[1] = column_mouse;
            left_fetched = 0;
        }
    }
    if (events & 8) {
        btn_at[1] = mousey_now;
        btn_col[1] = column_mouse;
        rbtn_press = 1;
        rbtn_released = 0;
    }
    if (events & 0x10) {
        right_up[0] = mousey_now;
        right_up[1] = column_mouse;
        rbtn_released = 1;
        if (right_taken) {
            mouse_click[0] = mousey_now;
            mouse_click[1] = column_mouse;
            right_taken = 0;
        }
    }
}

void far mouse_poll(void)                                        /* 1f5e:07d7 */
{
    short fn;
    short y;
    short x;

    if (pointer_state == 1) {
        fn = 3;
        mouse_call_regs(&fn, &buttons_now, &x, &y);
        mousey_now = y >> 3;
        column_mouse = x >> 3;
    }
}

void far pointer_follow(void)                                    /* 1f5e:0821 */
{
    if (pointer_now.hot_row + sprite_trow != mousey_now || pointer_now.hot_col + left_arrow != column_mouse) {
        pointer_hide();
        pointer_show();
    }
    sfx_tick(sound_ticks());
}

void far pointer_scale(void)                                     /* 1f5e:0856 */
{
    struct Pointer save;
    long far *src;
    register int i;

    for (i = 1; i < 4; i++)
        mem_move(pointer.image[0], pointer.image[i], 0x40);
    mem_move(&pointer, &save, sizeof(struct Pointer));
    expand_1to8(save.mask, pointer.mask, 0x20);
    expand_1to8(save.image, pointer.image, 0x20);
    src = (long far *)pointer.image;
    for (i = 0; i < 0x40; i++)
        *src++ &= 0x0f0f0f0fL;
    mem_move(&pointer, &save, sizeof(struct Pointer));
    plane_split(save.mask, 0x40, pointer.mask);
    plane_split(save.image, 0x40, pointer.image);
}

void far mouse_setup(void)                                       /* 1f5e:0942 */
{
    if (mouse_present) {
        mouse_call(0);
        mouse_call(7, 0, 0, (screen_cols() << 3) - 1);
        mouse_call(8, 0, 0, (screen_rows() << 3) - 1);
        mouse_call(0xf, 0, 3, 4);
        mouse_call(4, 0, column_mouse << 3, mousey_now << 3);
        mouse_handler_set(mouse_handler, 0x7f);
    }
}

void far mouse_pointer_init(int mode, char far *far *argv)             /* 1f5e:09dc */
{
    mouse_present = peek_word(0, 0xcc) && peek_word(0, 0xce) && mouse_call(0);
    show_count = 0;
    hide_count = 0;
    button_last = 0;
    pointer_scale();
    pointer_shape_set(0L);
    text_xy(0x1f44, 0x1f44, &mousey_now, &column_mouse);
    if (mouse_present) {
        mouse_setup();
        pointer_state = 1;
    } else
        pointer_state = 0;
    rbtn_released = lifted_lbtn = 1;
}

void far mouse_pointer_done(void)                                      /* 1f5e:0a74 */
{
    if (mouse_present)
        mouse_call(0);
}

int far pointer_mode_set(int mode)                                   /* 1f5e:0a85 */
{
    if (mode != pointer_state) {
        switch (mode) {
        case 0:
            if (pointer_state == 1)
                mouse_call(0);
            break;
        case 1:
            if (!mouse_present)
                return pointer_state;
            mouse_setup();
            break;
        default:
            return pointer_state;
        }
        pointer_state = mode;
    }
    return pointer_state;
}

int far pix_rem(int x)                                           /* 1f5e:0ac6 */
{
    return (pix_round[pixel_shift()] & x) << (3 - pixel_shift());
}

void far pointer_draw(int row, short col, int page)              /* 1f5e:0ae5 */
{
    short off;
    short old_plane;
    short old_ofs;
    short skip;
    short rskip;
    short extra;
    char old_page;
    register int i;

    row -= pointer_now.hot_row;
    col -= pointer_now.hot_col;
    sprite_trow = row;
    left_arrow = col;
    shape_height = pointer_now.rows;
    if (screen_rows() <= row)
        goto hide;
    if (row + shape_height <= 0)
        goto hide;
    if (screen_cols() <= col)
        goto hide;
    if (pointer_now.cols + col <= 0)
        goto hide;
    if (row < 0)
        skip = -row;
    else
        skip = 0;
    if (screen_rows() < row + shape_height)
        shape_height = screen_rows() - row;
    row += skip;
    shape_height -= skip;
    if (screen_cols() < pointer_now.cols + col)
        arrow_bytes = screen_cols() - col;
    else
        arrow_bytes = pointer_now.cols;
    if (col < 0)
        sprite_lskip = -col;
    else
        sprite_lskip = 0;
    if (shape_height <= 0)
        goto hide;
    if (arrow_bytes - sprite_lskip <= 0)
        goto hide;
    old_plane = map_planes;
    old_ofs = screen_addr;
    old_page = visible_on;
    draw_page_set(page);
    for (i = 0; i < video_planes(); i++) {
        video_address_set(row, col + sprite_lskip + i);
        video_plane_set((col + sprite_lskip + i) & 3);
        grab_bytes(ptr_under[i], shape_height, (arrow_bytes - sprite_lskip + 3) >> 2);
        if (sprite_lskip) {
            extra = (arrow_bytes - sprite_lskip + ((sprite_lskip + i) & 3)) >> 2;
            off = (pointer_now.cols >> 2) * skip + (((sprite_lskip + i) & 3) << 6)
                  + ((sprite_lskip + 3 - ((sprite_lskip + i) & 3)) >> 2);
        } else {
            extra = (arrow_bytes + 3 - i) >> 2;
            off = (pointer_now.cols >> 2) * skip + ((i & 3) << 6);
        }
        if (extra) {
            rskip = (pointer_now.cols >> 2) - extra;
            /* the (long) casts load each address into dx:ax before off is added (0x0126b0) */
            blit_shifted((char far *)(long)pointer_now.image + off,
                         (char far *)(long)pointer_now.mask + off,
                         shape_height, extra, rskip, 0xff, 0xff, 0);
        }
    }
    draw_page_set(old_page);
    video_plane_set(old_plane);
    screen_addr = old_ofs;
    return;
hide:
    shape_height = 0;
}

void far pointer_show(void)                                      /* 1f5e:0d1c */
{
    if (hide_count > 0) {
        if (--hide_count <= 0)
            pointer_follow();
    } else {
        if (show_count == 0)
            pointer_draw(mousey_now, column_mouse, 1);
        show_count++;
    }
}

void far pointer_restore(int page, char far *buf)                /* 1f5e:0d4e */
{
    int old_plane;
    short old_ofs;
    char old_page;
    register int i;

    if (shape_height > 0) {
        old_plane = map_planes;
        old_ofs = screen_addr;
        old_page = visible_on;
        draw_page_set(page);
        for (i = 0; i < video_planes(); i++) {
            video_address_set(int_max(0, sprite_trow), left_arrow + sprite_lskip + i);
            video_plane_set((left_arrow + sprite_lskip + i) & 3);
            put_bytes(buf + (i << 6), shape_height, (arrow_bytes - sprite_lskip + 3) >> 2);
        }
        draw_page_set(old_page);
        video_plane_set(old_plane);
        screen_addr = old_ofs;
    }
}

void far pointer_erase(void)                                     /* 1f5e:0dfb */
{
    pointer_restore(1, ptr_under);
    shape_height = 0;
}

void far pointer_hide_rect(int row1, int col1, int row2, int col2)    /* 1f5e:0e12 */
{
    register int col;

    if (show_count != 1) {
        pointer_hide();
        return;
    }
    hide_count++;
    if (sprite_trow < row2 && pointer_now.rows + sprite_trow > row1) {
        col = -1 << pixel_shift() & left_arrow;
        if (col < col2 && pointer_now.cols + col + (1 << pixel_shift()) > col1)
            while (hide_count > 0) {
                pointer_hide();
                hide_count--;
            }
    }
}

void far pointer_draw_over_buffer(char far *buf)                             /* 1f5e:0e80 */
{
    if (show_count > 0) {
        mem_move(ptr_under, buf, 0x100);
        pointer_draw(pointer_now.hot_row + sprite_trow, pointer_now.hot_col + left_arrow, 0);
    }
}

void far pointer_restore_from_buffer(char far *buf)                             /* 1f5e:0ebd */
{
    if (show_count > 0)
        pointer_restore(0, buf ? buf : ptr_under);
}

int far mouse_press(char mask, int far *x, int far *y)          /* 1f5e:0ee9 */
{
    mouse_poll();
    if (btn1_press && (mask & 1)) {
        if (lifted_lbtn) {
            btn1_clicked[0] = left_up[0];
            btn1_clicked[1] = left_up[1];
            left_fetched = 0;
        } else
            left_fetched = 1;
        *x = btn_at[0];
        *y = btn_col[0];
        btn1_press = 0;
        button_last = 0;
        return 1;
    }
    if (rbtn_press && (mask & 2)) {
        if (rbtn_released) {
            mouse_click[0] = right_up[0];
            mouse_click[1] = right_up[1];
            right_taken = 0;
        } else
            right_taken = 1;
        *x = btn_at[1];
        *y = btn_col[1];
        rbtn_press = 0;
        button_last = 1;
        return 2;
    }
    *x = mousey_now;
    *y = column_mouse;
    return 0;
}

char far mouse_clicked(int far *x, int far *y)                    /* 1f5e:0fa2 */
{
    mouse_poll();
    if (!button_last && !left_fetched) {
        *x = btn1_clicked[0];
        *y = btn1_clicked[1];
        left_fetched = 1;
        return 1;
    }
    if (button_last && !right_taken) {
        *x = mouse_click[0];
        *y = mouse_click[1];
        right_taken = 1;
        return 1;
    }
    *x = mousey_now;
    *y = column_mouse;
    return 0;
}

void far mouse_pos(int far *x, int far *y)                    /* 1f5e:100f */
{
    mouse_poll();
    *x = mousey_now;
    *y = column_mouse;
}

void far mouse_move_by(int drow, int dcol)                       /* 1f5e:102a */
{
    mousey_now = int_max(0, int_min(mousey_now + drow, screen_rows() - 1));
    column_mouse = int_max(0, int_min(column_mouse + dcol, screen_cols() - 1));
    if (mouse_present && pointer_state == 1)
        mouse_call(4, 0, column_mouse << 3, mousey_now << 3);
}

void far mouse_move_to(int row, int col)                         /* 1f5e:10a3 */
{
    text_xy(row, col, &row, &col);
    mouse_move_by(row - mousey_now, col - column_mouse);
}

void far mouse_fake(int button, char both)                       /* 1f5e:10d5 */
{
    if (button == 2) {
        if (!rbtn_released)
            mouse_event(0x10);
        else
            mouse_event(8);
        if (both) {
            if (!rbtn_released)
                mouse_event(0x10);
            else
                mouse_event(0x18);
        }
    } else {
        if (!lifted_lbtn)
            mouse_event(4);
        else
            mouse_event(2);
        if (both) {
            if (!lifted_lbtn)
                mouse_event(4);
            else
                mouse_event(6);
        }
    }
}

char far mouse_up_which(void)                                    /* 1f5e:115a */
{
    if (!lifted_lbtn)
        return 1;
    if (!rbtn_released)
        return 2;
    return 0;
}

int far mouse_pending(void)                                       /* 1f5e:1173 */
{
    return btn1_press || rbtn_press;
}

void far pointer_shape_set(struct Pointer far *p)                     /* 1f5e:1189 */
{
    struct Pointer far *src;
    int far *a;
    int far *b;
    register int n;

    if (!p)
        src = &pointer;
    else
        src = p;
    a = (int far *)&pointer_now;
    b = (int far *)src;
    n = 0x102;
    while (*a++ == *b++ && --n > 0)
        ;
    if (n) {
        mem_move(src, &pointer_now, sizeof(struct Pointer));
        pointer_hide();
        pointer_show();
    }
}
