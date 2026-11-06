/* CKIT.EXE overlay 6 module 2428: display initialization.  display_init,
 * what remains of DQK's routine of that name, reads the machine's state and sets video mode 15;
 * video_set programs that mode: the BIOS mode 13h, the screen and page
 * geometry the drawing code reads, the two page segments at a000 and
 * a400, unchained planes through the sequencer and CRTC, and the palette.
 * The small routines ask the machine what it is: a Tandy's ROM signature,
 * the PCjr's ROM model byte, and the BIOS tick count.
 *
 * EXE 0x03b2e9..0x03b4bd, one Turbo C large-model _TEXT segment starting at
 * 2428:0009 in RTLink section 6 (payload 0x38da0, link segment 21d4,
 * parent section 3), the section's last module: module 22c2's final retf
 * is 2428:0008, and three zero bytes follow this module's final retf,
 * 2428:01dc, to the payload's end at 0x03b4c0.  Section 6's table files its
 * last 22c2 site at 0x03b26b and 19 sites under 2428, from 0x03b2f3 to
 * 0x03b4a5.  Built with Turbo C 2.01, tcc -ml -O -Z -d.  Functions are
 * defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  Callee names are
 * placeholders chosen from use; a modNNNN_ name says only which segment
 * the callee lives in.  Code outside the section reaches four routines
 * through vector stubs:
 * is_tandy (fcache.c, speaker.c), is_pcjr (speaker.c), clock_now and
 * display_init (sysutil.c).  config.c far-calls
 * display_options_set.
 *
 * This is DQK's dispinit.c with every display path but video mode 15 taken
 * out, as vidmouse.c's is: video_set has no switch and keeps DQK's case 15
 * body after the equipment-word store; display_init keeps DQK's first
 * reads and sets mode 15 without the monitor.dat file, the command line
 * or the adapter probes, whose routines are gone with DQK's ega_pals,
 * herc_crtc, mem_small, is_ps2_30, dos_version, detect_adapter and
 * herc_init.  video_bios's return is used here and the call passes one
 * argument, where lowio.c and vidmouse.c declare it void with two.
 *
 * _DATA is DGROUP 977e..979f: pal_rgb and display_opts, DQK's values; the
 * resident vidmouse.c's _DATA follows at 97a0.  No _BSS: every variable
 * video_set and display_init write is vidmouse.c's.
 */

extern int  far peek_byte(unsigned seg, unsigned off);        /* 00d7:2848 */
extern void far fill_far_words();                                  /* 00d7:08a2 */
extern void far reg_write(int port, int index, int value);    /* 00d7:0da0 */
extern void far poke_byte(int a, int b, int c);               /* 00d7:2858 */
extern int  far peek_word(unsigned seg, unsigned off);        /* 00d7:286a */
extern void far crit_install(void);                              /* 00d7:28f9 */
extern int  far video_bios(int ah, ...);    /* 00d7:297b, int 10h: ah, al, ... */
extern long far clock_bios(int ah, ...);    /* 00d7:2a1b, int 1Ah: ah, dx, cx */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern int  far draw_char(int row, int col, char c, char attr, char flags);
                                                                 /* 0f5e:03d6 */
extern void far video_plane_set(int plane);                         /* 0f5e:05ef */
extern void far pal_entry(int idx, unsigned rgb);             /* 107e:02d0 */
extern void far clip_reset(void);                              /* 107e:066e */

extern char dos_mode;                                            /* d60c */
extern unsigned draw_seg;                                        /* d60d */
extern int video_mode;                                           /* d61b */
extern unsigned video_len;                                       /* d61d */
extern unsigned display_seg;                                     /* d71f */
extern unsigned mem_top_seg;                                     /* d721 */
extern char visible_on;                                          /* d726 */
extern char back_screen;                                         /* d739 */
extern int pix_shifts;                                           /* d73d */
extern int pixel_cols;                                           /* d73f */
extern int text_bytes;                                           /* d746 */
extern int shape_boot;                                           /* d748 */
extern int letter_step;                                          /* d754 */
extern int base_offset;                                          /* d75a */
extern char saved_equip;                                         /* d75c */
extern int extent_text;                                          /* d75d */
extern unsigned screen_addr;                                     /* d75f */
extern int screen_lines;                                         /* d976 */
extern int planes_count;                                         /* d978 */
extern int txthigh_dots;                                         /* d97a */

static int pal_rgb[16] = {                                       /* 977e */
    0x000, 0x00a, 0x0a0, 0x0aa, 0xa00, 0xa0a, 0xa80, 0xaaa,
    0x555, 0x55f, 0x5f5, 0x5ff, 0xf55, 0xf5f, 0xff5, 0xfff
};
static int display_opts = 0;                                     /* 979e */

char far is_tandy(void)                                          /* 3428:0009 */
{
    return peek_byte(0xfc00, 0) == 0x21;
}

char far is_pcjr(void)                                     /* 3428:0024 */
{
    return peek_byte(0xf000, 0xfffe) == 0xfd;
}

long far clock_now(void)                                     /* 3428:0040 */
{
    return clock_bios(0);
}

void far video_set(int mode)                                     /* 3428:004a */
{
    int i;

    video_mode = mode;
    poke_byte(0x40, 0x10, saved_equip & ~0x30 | 0x10);
    pix_shifts = 3;
    letter_step = txthigh_dots = 4;
    screen_lines = 200;
    pixel_cols = 320;
    pix_shifts = 2;
    video_len = 0x3e80;
    display_seg = 0xa000;
    mem_top_seg = 0xa400;
    extent_text = 0x3e80;
    text_bytes = base_offset = 0x50;
    planes_count = 4;
    video_bios(0, 0x13);
    reg_write(0x3c4, 4, 6);
    fill_far_words(0, display_seg, video_len, 0);
    fill_far_words(0, mem_top_seg, video_len, 0);
    reg_write(0x3d4, 0x17, 0xc3);
    reg_write(0x3d4, 0x14, 0);
    for (i = 0; i < 16; i++)
        pal_entry(i, pal_rgb[i]);
    draw_seg = display_seg;
    screen_addr = 0;
    draw_char(-1, 0, 0, 0, 0);
    video_plane_set(0);
    visible_on = back_screen = 1;
    clip_reset();
    draw_page_set(1);
}

void far display_options_set(int opts)                               /* 3428:0188 */
{
    display_opts = opts;
}

void far display_init(int mode, char far * far *argv)           /* 3428:0193 */
{
    crit_install();
    dos_mode = video_bios(0x0f);
    saved_equip = peek_byte(0x40, 0x10);
    shape_boot = peek_word(0x40, 0x60);
    video_mode = 0xf;
    video_set(video_mode);
}
