/* CKIT.EXE resident module 1da7: start-up and shutdown, keyboard waits,
 * delays, the random number generator, cursor text output and the error box.
 *
 * EXE 0x00fe75..0x01030f, one Turbo C large-model _TEXT segment starting at
 * byte 5 of paragraph 1da7.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1d65's last retf is at 0x00fe74, and module 1df0 starts at
 * 0x01030f (1df0:000f) with no stub between.  Nothing found calls set_attr
 * (02bc): no far call, far jump, callback push or far-pointer dword in
 * CKIT.EXE names it.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module here gives an address is reused from there;
 * sfx_slots_init and file_cache_init are DQK's names for them.  A
 * modNNNN_ name is a placeholder that says only which segment the callee
 * lives in.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined and reaches it with `push cs; call near` once it is: sys_init
 * far-calls random_seed and set_attr_word, sys_shutdown far-calls sys_done,
 * so those three carry prototypes.
 *
 * _DATA is 968a..969a, the three literals; 9689 and 969b are pad, and the
 * literals at 969c are overlay section 3's.  _BSS follows the statics'
 * names (Turbo C's symbol-table order), not their declaration order; these
 * names reproduce the shipped d584..d58e layout.
 *
 * text_printf hands text_format one handler per escape letter of "vka":
 * %v sets the cursor row, %k the column, %a the attribute.
 */

extern void far display_init(int mode, char far *far *argv);     /* 18de:201b */
extern void far sfx_slots_init(int mode, char far *far *argv);     /* 1782:000e */
extern void far mouse_pointer_init(int mode, char far *far *argv);     /* 0f5e:09dc */
extern void far timer_rehook(void);                              /* 107e:0047 */
extern void far lowio_init(void);                              /* 107e:00d4 */
extern void far file_cache_init(int need_kb, int want_kb);          /* 1336:00cd */
extern void far lib_init(void);                                  /* 147e:0003 */
extern void far glyph_library_init(void);                                /* 11d7:0008 */
extern void far glyph_library_done(void);                                /* 11d7:0030 */
extern void far lib_done(void);                              /* 147e:0022 */
extern void far file_cache_done(void);                              /* 1336:0241 */
extern void far lowio_done(void);                              /* 107e:00d5 */
extern void far timer_unhook(void);                              /* 107e:007f */
extern void far mouse_pointer_done(void);                              /* 0f5e:0a74 */
extern void far sfx_slots_done(void);                              /* 1782:0055 */
extern void far video_restore(void);                              /* 0f5e:0006 */
extern char far kbd_waiting(void);                              /* 178d:005c */
extern char far inp_pending(void);                              /* 178d:0056 */
extern char far aux_pending(void);                              /* 178d:0068 */
extern int  far kbd_read(void);                              /* 178d:0062 */
extern int  far inp_press(char op, int far *a, int far *b);   /* 178d:000b */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far voice_set(int voice, unsigned freq, int volume); /* 113b:007b */
extern long far clock_now(void);                              /* 18de:2011 */
extern int  far screen_cols(void);                              /* 0f5e:05d4 */
extern int  far screen_rows(void);                              /* 0f5e:05d0 */
extern void far fill_rect(int row1, int col1, int row2, int col2, int attr); /* 0f5e:0040 */
extern void far text_xy(int row, int col, int far *out_row, int far *out_col); /* 107e:04a7 */
extern int  far draw_char(int row, int col, char c, char attr, char flags); /* 0f5e:03d6 */
extern int  far text_format(char far *fmt, void far *args,
                             void (far *put)(char), char far *codes, ...); /* 1867:011a */
extern char far draw_page_visible(void);                              /* 0f5e:03d2 */
extern void far clip_save(void);                              /* 107e:071c */
extern void far clip_reset(void);                              /* 107e:066e */
extern void far draw_page_set(char mode);                         /* 0f5e:03aa */
extern void far console_beep(void);                              /* 107e:02c5 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern void far clip_restore(void);                              /* 107e:0735 */

void far random_seed(void);
void far set_attr_word(int a);
void far sys_done(void);

static long seed;                                                /* d584 */
static int cur_col;                                              /* d588 */
static int cur_row;                                              /* d58a */
static int text_attr;                                            /* d58c */
static char text_attr_hi;                                        /* d58e */

void far sys_init(register int mode, char far *far *argv, int need_kb, int want_kb) /* 1da7:0005 */
{
    display_init(mode, argv);
    sfx_slots_init(mode, argv);
    mouse_pointer_init(mode, argv);
    timer_rehook();
    lowio_init();
    random_seed();
    set_attr_word(15);
    file_cache_init(need_kb, want_kb);
    lib_init();
    glyph_library_init();
}

void far sys_shutdown(void)                                      /* 1da7:006c */
{
    glyph_library_done();
    lib_done();
    file_cache_done();
    lowio_done();
    sys_done();
    timer_unhook();
    mouse_pointer_done();
    sfx_slots_done();
    video_restore();
}

char far key_pressed(void)                                       /* 1da7:009a */
{
    return kbd_waiting() || inp_pending() || aux_pending();
}

void far key_flush(void)                                         /* 1da7:00bd */
{
    int a;
    int b;

    while (kbd_waiting())
        kbd_read();
    while (inp_press(7, &a, &b))
        ;
    aux_pending();
}

void far key_wait(void)                                          /* 1da7:00f5 */
{
    key_flush();
    while (!key_pressed())
        ;
    key_flush();
}

void far wait_or_key(register int n)                             /* 1da7:0106 */
{
    long start;

    n *= 7;
    start = timer_ticks();
    while (!key_pressed() && (n == 0 || timer_ticks() < start + n))
        ;
}

void far wait_ticks(int n)                                       /* 1da7:0151 */
{
    long until;

    until = timer_ticks() + n * 7;
    while (timer_ticks() < until)
        ;
    key_flush();
}

void far beep(void)                                              /* 1da7:018c */
{
    long start;

    start = timer_ticks();
    voice_set(2, 0xa9, 0x7f);
    voice_set(4, 0x712, 0x7f);
    while (timer_ticks() < start + 5)
        ;
    voice_set(2, 0, 0);
    voice_set(4, 0, 0);
    while (timer_ticks() < start + 6)
        ;
}

void far random_seed(void)                                       /* 1da7:021f */
{
    seed = clock_now();
}

void far sys_done(void)                                          /* 1da7:022c */
{
}

int far random_int(register int n)                               /* 1da7:022d */
{
    long span;

    if (n <= 1)
        return 0;
    span = n + 0x3fffffffL;
    span /= n;
    seed = seed * 0x6d25L + 1;
    return (seed & 0x3fffffffL) / span;
}

void far clear_screen(int a)                                     /* 1da7:029d */
{
    fill_rect(0, 0, screen_rows(), screen_cols(), a);
}

void far set_attr(int fg, int bg)                                /* 1da7:02bc */
{
    text_attr = (bg << 4) + fg;
    text_attr_hi = 0;
}

void far set_attr_word(int a)                                    /* 1da7:02d3 */
{
    text_attr_hi = (text_attr = a) >> 8;
}

void far text_goto(int row, int col)                             /* 1da7:02e5 */
{
    text_xy(row, col, &cur_row, &cur_col);
}

void far text_code_row(int arg, int value)                       /* 1da7:0301 */
{
    text_xy(value, arg, &cur_row, &arg);
}

void far text_code_col(int arg, int value)                       /* 1da7:031d */
{
    text_xy(arg, value, &arg, &cur_col);
}

void far text_code_attr(int arg, int value)                      /* 1da7:0339 */
{
    set_attr_word(value);
}

void far text_putc(char c)                                       /* 1da7:0346 */
{
    cur_col += draw_char(cur_row, cur_col, c, text_attr, text_attr_hi);
}

void far text_printf(char far *fmt, ...)                         /* 1da7:0369 */
{
    text_format(fmt, (void far *)(&fmt + 1), text_putc, "vka",
                 text_code_row, text_code_col, text_code_attr);
}

void far text_print_at(int row, int col, int a, char far *fmt, ...) /* 1da7:0399 */
{
    set_attr_word(a);
    text_goto(row, col);
    text_printf("%r", fmt, (void far *)(&fmt + 1));
}

void far error_box(char far *fmt, ...)                           /* 1da7:03c8 */
{
    char mode;

    mode = draw_page_visible();
    clip_save();
    clip_reset();
    draw_page_set(1);
    fill_rect(0x1fa0, 0x1f40, 0x1fa4, 0x1fe0, 15);
    set_attr_word(0xf0);
    text_goto(0x1fa0, 0x1f40);
    text_printf("Error: %r", fmt, (void far *)(&fmt + 1));
    console_beep();
    key_flush();
    while (!key_pressed())
        ;
    if (key_poll() && key_get() == 'q') {
        sys_shutdown();
        dos_call(0x4c01);           /* terminate, exit code 1 */
    }
    key_flush();
    set_attr_word(15);
    fill_rect(0x1fa0, 0x1f40, 0x1fa4, 0x1fe0, 0);
    clip_restore();
    draw_page_set(mode);
}
