/* CKIT.EXE resident module 1d48: small game services.  The Esc flag, set
 * when Esc or ` is read while esc_check is on; zeroing a block; leaving
 * the program; reading a key, where Ins and Del (0x152, 0x153) also call
 * into frame 1c8e and every extended key comes back less 0x80; allocating
 * and freeing records in the pool at item_record_pool, passing 0x3e as the word
 * strutil.c's pool_alloc and pool_free name unused; and formatting a byte,
 * an int or an unsigned long into a static buffer.
 *
 * EXE 0x00f888..0x00fa57, one Turbo C large-model _TEXT segment starting at
 * byte 8 of paragraph 1d48, right after packtext.c's last retf.  Module
 * 1d65 starts at 0x00fa57.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined: input_pending_clear far-calls input_press_discard, so that one carries a prototype.
 */

struct Pool {                   /* strutil.c's slot pool                  */
    int capacity;               /* +0                                     */
    int stride;                 /* +2                                     */
    char far *base;             /* +4                                     */
    unsigned char bitmap[1];    /* +8                                     */
};

extern void far mem_fill(void far *p, int n, int c);         /* 00d7:277b */
extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far game_sound_stop(void);                              /* 0c8e:0221 */
extern void far sys_shutdown(void);                              /* 0da7:006c */
extern void far game_done(void);
                                 /* 18de:10d5, stub: jmp far 2209:010c, section 6 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */
extern void far sound_toggle(void);                              /* 0c8e:0227 */
extern void far music_toggle(void);                              /* 0c8e:0252 */
extern int  far mouse_press(char mask, int far *a, int far *b); /* 0f5e:0ee9 */
extern void far pool_alloc(void far * far *out, int unused, struct Pool far *pool);
                                                                 /* 0a75:0203 */
extern void far pool_free(void far *ptr, int unused, struct Pool far *pool);
                                                                 /* 0a75:02c8 */
extern void far int_to_str(int value, char far *buf);            /* 0a75:000d */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */

extern char esc_check;                                           /* b423 */
extern struct Pool item_record_pool;                                    /* cb0e */

void far input_press_discard(void);

static char b_buf[10];                                           /* ada6 */
static char i_buf[12];                                           /* adb0 */
static char l_buf[12];                                           /* adbc */
static char esc_hit;                                             /* adc8 */

void far esc_set(char on)                                      /* 1d48:0008 */
{
    esc_hit = on;
}

char far esc_get(void)                                         /* 1d48:0013 */
{
    return esc_hit;
}

void far mem_zero(void far *p, int n)                           /* 1d48:0017 */
{
    mem_fill(p, n, 0);
}

void far game_exit(void)                                         /* 1d48:002f */
{
    slot_close(0);
    game_sound_stop();
    sys_shutdown();
    game_done();
    dos_call(0x4c00);
}

unsigned char far game_key_get(void)                                /* 1d48:0052 */
{
    short key;

    if (esc_check) {
        if (key_poll())
            key = key_get();
        else
            key = 0;
        if (key == 0x1b || key == 0x60)
            esc_set(1);
    } else
        key = key_get();
    if (key == 0x152)
        sound_toggle();
    if (key == 0x153)
        music_toggle();
    if (key != 0)
        while (key_poll()) {
            key = key_get();
            if (key == 0x152)
                sound_toggle();
            if (key == 0x153)
                music_toggle();
            if (esc_check && (key == 0x1b || key == 0x60))
                esc_set(1);
        }
    if (key >= 0x100)
        key -= 0x80;
    return key;
}

void far input_pending_clear(void)                                         /* 1d48:010a */
{
    input_press_discard();
}

void far input_press_discard(void)                                         /* 1d48:0110 */
{
    int a;
    int b;

    if (key_poll())
        game_key_get();
    mouse_press(7, &a, &b);
}

void far *far rec_alloc(void)                                    /* 1d48:013c */
{
    void far *p;

    pool_alloc(&p, 0x3e, &item_record_pool);
    return p;
}

void far rec_free(void far * far *pp)                            /* 1d48:0162 */
{
    pool_free(*pp, 0x3e, &item_record_pool);
}

char far *far byte_str(unsigned char n)                          /* 1d48:0181 */
{
    int_to_str(n, b_buf);
    return b_buf;
}

char far *far int_str(int n)                                     /* 1d48:019d */
{
    int_to_str(n, i_buf);
    return i_buf;
}

char far *far ulong_str(unsigned long n)                         /* 1d48:01b6 */
{
    str_format(l_buf, "%lu", n);
    return l_buf;
}
