/* CKIT.EXE resident module 207e: the low-level services under the screen
 * and input code.  It allocates the one far work block that library.c and
 * other code borrow, reprograms timer 0 and reads the tick count the
 * runtime's timer handler keeps, reads the keyboard through the BIOS and
 * folds its scan codes into the game's key numbers, loads VGA DAC
 * registers, converts text cells to pixel coordinates, keeps the clip
 * rectangle, splits a chained 4-plane buffer into planes, and checks the
 * top of memory.
 *
 * EXE 0x012be1..0x013388, one Turbo C large-model _TEXT segment starting at
 * byte 1 of paragraph 207e.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1f5e's last routine ends with the retf at 0x012be0, and module
 * 20f8 starts at 0x013388 (20f8:0008) with no stub between.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module here gives an address is reused from there;
 * a modNNNN_ name is a placeholder that says only which segment the routine
 * lives in.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined: timer_unhook far-calls timer_ticks, which carries a prototype.
 * The port writes are <dos.h>'s outportb, which Turbo C compiles inline.
 *
 * _DATA is 9e7c..9eb4: alt_keys and the one literal.  Below it is a mask
 * table module 1f5e indexes from 9e68 (1f5e:0acf); 9eb5 is pad, and module
 * 20f8 indexes from 9eb6 (20f8:0083).
 * _BSS follows the statics' names (Turbo C's symbol-table order), not their
 * declaration order; these names reproduce the shipped d980..d998 layout.
 * ticks and fire_latch are public: the runtime's timer handlers, 10d7:0009
 * and the 10d7:019e timer_rehook installs, count ticks (10d7:000f, 01a4) and
 * OR the joystick buttons newly pressed (port 0x201) into fire_latch
 * (10d7:002b, 01c0).  The clip rectangle is public: glyph.c and module 1f5e
 * read it.
 *
 * spare_buffer, txthigh_dots and letter_step are extern: they lie among
 * module 1f5e's _BSS variables (d754, d972 between 1f5e's d970 and d976,
 * d97a), and 1f5e reads d754 and d97a.  Only this module references d972.
 */

#include <dos.h>

extern void far *far mem_alloc(int n);                        /* 00d7:2669 */
extern void far con_printf(char far *fmt, ...);                  /* 1860:0001 */
extern void far con_getch(void);                              /* 00d7:2450 */
extern long far clock_bios(int ah, ...);        /* 00d7:2a1b, int 1Ah: ah, dx, cx */
extern void far timer_install(void far *isr);                     /* 00d7:0153 */
extern void far timer_isr(void);                              /* 00d7:019e */
extern int  far bios_key(int cmd);                           /* 00d7:005c */
extern void far con_direct(int c);                             /* 00d7:2495 */
extern void far video_bios(int ah, int al, ...); /* 00d7:297b, int 10h: ah, al, bh, bl, cx, dh, dl */
extern void far dac_load(int first, int count, int wait,
                             void far *rgb);                     /* 00d7:20f6 */
extern void far screen_page_show(void);                              /* 0f5e:02e2 */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern int  far screen_cols(void);                              /* 0f5e:05d4 */
extern int  far int_max(int a, int b);                      /* 00d7:2625 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */
extern int  far peek_word(unsigned seg, unsigned off);        /* 00d7:286a */

long far timer_ticks(void);

extern unsigned _psp;                                            /* 0020 */
extern int letter_step;                                          /* d754 */
extern void far *spare_buffer;                                   /* d972 */
extern int txthigh_dots;                                         /* d97a */

long ticks;                                                      /* d980 */
static long bios_time;                                           /* d984 */
int clip_top;                                                    /* d988 */
static int clips_top;                                            /* d98a */
char fire_latch;                                                 /* d98c */
int clip_left;                                                   /* d98d */
int clip_right;                                                  /* d98f */
static int clips_left;                                           /* d991 */
int clip_bottom;                                                 /* d993 */
static int clips_right;                                          /* d995 */
static int clips_bottom;                                         /* d997 */

/* ALT-letter scan codes 0x10..0x19, 0x1e..0x26 and 0x2c..0x32 in keyboard
 * order; key_get returns the letter + 0xff. */
static char alt_keys[26] = "QWERTYUIOPASDFGHJKLZXCVBNM";         /* 9e7c */

void far *far db_buffer(void)                                 /* 207e:0001 */
{
    return spare_buffer;
}

void far db_init(long size)                                      /* 207e:0009 */
{
    if (!spare_buffer)
        spare_buffer = mem_alloc(size);
    if (!spare_buffer) {
        con_printf("Out of memory in DBInit (%ld)\n", size);
        con_getch();
    }
}

/* Timer 0 at mode 3, divisor 0x4000: four times the BIOS rate. */
void far timer_rehook(void)                                      /* 207e:0047 */
{
    bios_time = clock_bios(0);
    outportb(0x43, 0x36);                       /* the census's weak `out` markers */
    outportb(0x40, 0);
    outportb(0x40, 0x40);
    ticks = 0;
    timer_install(timer_isr);
}

void far timer_unhook(void)                                      /* 207e:007f */
{
    outportb(0x43, 0x36);                       /* the census's weak `out` markers */
    outportb(0x40, 0);
    outportb(0x40, 0);
    timer_install(0);
    clock_bios(1, timer_ticks() / 4 + bios_time);
}

long far timer_ticks(void)                                      /* 207e:00c0 */
{
    return ticks;
}

long far sound_ticks(void)                                      /* 207e:00c8 */
{
    return ticks;
}

int far mod107e_00d0(void)                                       /* 207e:00d0 */
{
    return 0x1110;
}

void far lowio_init(void)                                      /* 207e:00d4 */
{
}

void far lowio_done(void)                                      /* 207e:00d5 */
{
}

char far key_poll(void)                                          /* 207e:00d6 */
{
    return bios_key(1);
}

/* `2e 3b 07` at 207e:00fd is Turbo C's sparse-switch value search: 64 case
 * values at cs:010e and their targets at cs:018e, scanned with loop. */
int far key_get(void)                                            /* 207e:00e1 */
{
    register unsigned k;

    while (!key_poll())
        ;
    k = bios_key(0);
    switch (k) {
    case 0x4800: case 0x4838: case 0x487e:
        return 0x108;
    case 0x5000: case 0x5032: case 0x5060:
        return 0x104;
    case 0x4b00: case 0x4b34: case 0x4b7c:
        return 0x106;
    case 0xf400: case 0x4d00: case 0x4d36:
        return 0x102;
    case 0x4700: case 0x4737: case 0x475c:
        return 0x107;
    case 0x4900: case 0x4939:
        return 0x101;
    case 0x4f00: case 0x4f31:
        return 0x105;
    case 0x5100: case 0x5133:
        return 0x103;
    case 0xf300: case 0x4c35:
        return 0x100;
    case 0x5200: case 0x5230:
        return 0x109;
    case 0x3b00: case 0x3c00: case 0x3d00: case 0x3e00: case 0x3f00:
    case 0x4000: case 0x4100: case 0x4200: case 0x4300: case 0x4400:
        return ((k >> 8) & 0xff) + 0xd5;
    case 0x5300: case 0x532d: case 0x532e:
        return 0x10a;
    case 0x1000: case 0x1100: case 0x1200: case 0x1300: case 0x1400:
    case 0x1500: case 0x1600: case 0x1700: case 0x1800: case 0x1900:
        return alt_keys[(k - 0x1000) >> 8] + 0xff;
    case 0x1e00: case 0x1f00: case 0x2000: case 0x2100: case 0x2200:
    case 0x2300: case 0x2400: case 0x2500: case 0x2600:
        return alt_keys[(k - 0x1400) >> 8] + 0xff;
    case 0x2c00: case 0x2d00: case 0x2e00: case 0x2f00: case 0x3000:
    case 0x3100: case 0x3200:
        return alt_keys[(k - 0x1900) >> 8] + 0xff;
    }
    return k & 0xff;
}

char far shift_held(void)                                        /* 207e:0299 */
{
    return (bios_key(2) & 3) ? 1 : 0;
}

/* Bit 3 of the BIOS shift flags: an Alt key is down. */
char far alt_held(void)                                      /* 207e:02af */
{
    return (bios_key(2) & 8) ? 1 : 0;
}

void far console_beep(void)                                      /* 207e:02c5 */
{
    con_direct(7);
}

/* DAC register idx (int 10h ax=1010h) from a 12-bit 0xRGB. */
void far pal_entry(int idx, unsigned rgb)                     /* 207e:02d0 */
{
    if (idx >= 0 && idx <= 0xff)
        video_bios(0x10, 0x10, 0, idx, ((rgb & 0xf0) << 6) + ((rgb & 0xf) << 2),
                     (rgb & 0xf00) >> 6);
}

/* Sixteen entries from 8-bit RGB triples: EGA register i names DAC register
 * i, loaded with each component's top 6 bits.  rgb, the entry rounded to
 * 0..15 per component, is computed and never used. */
void far pal_set16(unsigned char far *pal)                       /* 207e:031a */
{
    short r, g, b;
    register int i, rgb;

    for (i = 0; i < 16; i++) {
        r = *pal;
        rgb = (r + 8) / 17 << 8;
        pal++;
        g = *pal;
        rgb += (g + 8) / 17 << 4;
        pal++;
        b = *pal;
        rgb += (b + 8) / 17;
        pal++;
        video_bios(0x10, 0, i, i);
        video_bios(0x10, 0x10, 0, i, ((g & 0xfc) << 6) + ((b & 0xfc) >> 2),
                     (r & 0xfc) >> 2);
    }
}

void far palette_load(unsigned char far *pal)                    /* 207e:03ce */
{
    unsigned char dac[0x300];
    unsigned char far *q;
    register int i;

    q = dac;
    for (i = 0; i < 0x100; i++) {
        *q++ = *pal++ >> 2;
        *q++ = *pal++ >> 2;
        *q++ = *pal++ >> 2;
    }
    dac_load(0, 0x56, 1, dac);
    dac_load(0x56, 0x56, 1, dac + 0x102);
    dac_load(0xac, 0x54, 1, dac + 0x204);
}

void far palette_page_load(unsigned char far *pal, char flag)         /* 207e:0480 */
{
    if (flag)
        screen_page_show();
    palette_load(pal);
    if (!flag)
        screen_page_show();
}

/* A coordinate above 6000 is 8000 plus a count of txthigh_dots / 2 (a row) or
 * letter_step / 2 (a column) units; a smaller one passes through. */
void far text_xy(int row, int col, int far *out_row, int far *out_col) /* 207e:04a7 */
{
    if (row > 6000)
        *out_row = (txthigh_dots >> 1) * (row - 8000);
    else
        *out_row = row;
    if (col > 6000)
        *out_col = (letter_step >> 1) * (col - 8000);
    else
        *out_col = col;
}

void far text_xy_to(int row, int col, int x, int y,
                    int far *out_row, int far *out_col)          /* 207e:04f8 */
{
    short h, w;

    h = txthigh_dots >> 1;
    w = letter_step >> 1;
    text_xy(row, col, &row, &col);
    x -= row;
    if (x < 0)
        x -= h - 1;
    *out_row = x / h;
    y -= col;
    if (y < 0)
        y -= w - 1;
    *out_col = y / w;
}

void far text_offset_xy(int row, int col, int row2, int col2,
                      int far *out_row, int far *out_col)        /* 207e:0565 */
{
    text_xy(row, col, &row, &col);
    if (row2 > 6000)
        *out_row = (txthigh_dots >> 1) * (row2 - 8000) + row;
    else
        *out_row = row + row2;
    if (col2 > 6000)
        *out_col = (letter_step >> 1) * (col2 - 8000) + col;
    else
        *out_col = col + col2;
}

void far clip_set(int row1, int col1, int row2, int col2)    /* 207e:05dd */
{
    text_xy(row1, col1, &clip_top, &clip_left);
    text_xy(row2, col2, &clip_bottom, &clip_right);
    if (clip_top < 0)
        clip_top = 0;
    if (clip_left < 0)
        clip_left = 0;
    if (screen_rows() < clip_bottom)
        clip_bottom = screen_rows();
    if (screen_cols() < clip_right)
        clip_right = screen_cols();
    if (clip_top >= clip_bottom || clip_left >= clip_right)
        clip_top = clip_left = clip_bottom = clip_right = 0;
}

void far clip_reset(void)                                      /* 207e:066e */
{
    clip_top = clip_left = 0;
    clip_bottom = screen_rows();
    clip_right = screen_cols();
}

void far clip_narrow(int row1, int col1, int row2, int col2)     /* 207e:0687 */
{
    text_xy(row1, col1, &row1, &col1);
    text_xy(row2, col2, &row2, &col2);
    clip_top = int_max(clip_top, row1);
    clip_left = int_max(clip_left, col1);
    clip_bottom = int_min(clip_bottom, row2);
    clip_right = int_min(clip_right, col2);
    if (clip_top >= clip_bottom || clip_left >= clip_right)
        clip_top = clip_left = clip_bottom = clip_right = 0;
}

void far clip_save(void)                                      /* 207e:071c */
{
    clips_top = clip_top;
    clips_left = clip_left;
    clips_bottom = clip_bottom;
    clips_right = clip_right;
}

void far clip_restore(void)                                      /* 207e:0735 */
{
    clip_top = clips_top;
    clip_left = clips_left;
    clip_bottom = clips_bottom;
    clip_right = clips_right;
}

/* Every fourth byte from src + plane, n of them per plane. */
void far plane_split(char far *src, int n, char far *dst)        /* 207e:074e */
{
    char far *p;
    register int plane, k;

    for (plane = 0; plane < 4; plane++) {
        p = src + plane;
        k = n;
        while (--k >= 0) {
            *dst++ = *p;
            p += 4;
        }
    }
}

int far memory_above_320k(void)                                       /* 207e:078c */
{
    return (unsigned)peek_word(_psp, 2) >= 0x5000 ? 1 : 0;
}
