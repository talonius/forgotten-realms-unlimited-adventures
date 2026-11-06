/* CKIT.EXE overlay 6 module 21d4: the configuration file, the memory and
 * video checks made against it, and hex-valued command options.
 *
 * EXE 0x038da0..0x039001, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 21d4, the first module of RTLink section 6 (payload
 * 0x38da0, link segment 21d4).  Its final retf is byte 0 of paragraph 21fa,
 * where the next module begins.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  A modNNNN_ callee name
 * is a placeholder that says only which segment the callee lives in, and
 * display_options_set names a routine of section 6's module 2428.  The 00d7 DOS
 * wrappers load AX, DX, BX and CX from however many arguments a caller
 * pushes.  arg_hex is reached only through the vector stub 18de:10c1,
 * sound.c's arg_hex.
 *
 * _DATA is the six literals, DGROUP 5ee8..5fb1.  config is the whole of
 * _BSS, b392..b3db; sound.c, sndevent.c and sfxctl.c read its +2 field as
 * sound_card (b394).
 */

extern int  far dos_open(int ax, char far *name, ...);      /* 00d7:26bc */
extern int  far dos_io(int ax, void far *buf, int bx, int cx); /* 00d7:26e9 */
extern int  far dos_call(int ax, ...);                       /* 00d7:2697 */
extern int  far peek_word(unsigned seg, unsigned off);        /* 00d7:286a */
extern void far con_printf(char far *fmt, ...);                  /* 1860:0001 */
extern void far display_options_set(int mode);                       /* 2428:0188 */
extern void far sys_shutdown(void);                              /* 0da7:006c */
extern char far is_xvga(void);                              /* 0f5e:05e0 */
extern int  far pixel_shift(void);                              /* 0f5e:05d8 */
extern int  far video_planes(void);                              /* 0f5e:05dc */
extern char far *far msg_text(int i);                            /* 0a75:039b */
extern int  far to_upper(int c);                             /* 00d7:25bc */
extern int  far str_len(char far *s);                       /* 00d7:2577 */

extern unsigned _psp;                                            /* 0020 */

struct Config {
    int video;                  /* +0  0 VGA, 1 EGA, 2 Tandy 16-colour  */
    int sound_card;             /* +2  sound.c's sound_card             */
    unsigned char rest[0x46];
};

struct Config config;                                            /* b392 */

void far config_load(void)                                       /* 31d4:0000 */
{
    unsigned long avail;
    int fd;

    if ((fd = dos_open(0x3d00, "disk1\\ckit.cfg")) >= 0) {
        if (dos_io(0x3f00, &config, fd, sizeof config) == sizeof config) {
            dos_call(0x3e00, 0, fd);
            avail = (unsigned long)(peek_word(_psp, 2) - _psp) << 4;
            if (avail < 524000L) {
                con_printf("Not enough memory to run!\n"
                           "%l bytes Required, %l bytes available.\n",
                           524000L, avail);
                dos_call(0x4c01);           /* terminate, exit code 1 */
            }
            if (config.video == 0)
                display_options_set(5);
            return;
        }
        dos_call(0x3e00, 0, fd);
    }
    con_printf("Error reading configuration file!\n");
    dos_call(0x4c01);
}

void far video_check(void)                                       /* 31d4:00e2 */
{
    char far *msg;

    msg = 0;
    switch (config.video) {
    case 0:
        if (!is_xvga())
            msg = "Installed for VGA only!\n";
        break;
    case 1:
        if (pixel_shift() != 3 || video_planes() != 4)
            msg = "Installed for EGA only!\n";
        break;
    case 2:
        if (pixel_shift() != 1)
            msg = "Installed for TANDY 16 color only!\n";
        break;
    }
    if (msg) {
        sys_shutdown();
        con_printf(msg);
        dos_call(0x4c01);
    }
}

int far arg_hex(char far *key, int value)                        /* 31d4:0170 */
{
    char far *arg;
    int n;
    int i;

    for (i = 1; i < 10; i++) {
        arg = msg_text(i);
        if (to_upper(arg[0]) == key[0] && to_upper(arg[1]) == key[1]
            && str_len(arg) > 2) {
            arg += 2;
            n = 0;
            do {
                if (*arg >= '0' && *arg <= '9')
                    n += *arg - '0';
                else if (*arg >= 'A' && *arg <= 'Z' || *arg >= 'a' && *arg <= 'z')
                    n += to_upper(*arg) - ('A' - 10);
                arg++;
                if (*arg)
                    n <<= 4;
            } while (*arg);
            if (n)
                value = n;
        }
    }
    return value;
}
