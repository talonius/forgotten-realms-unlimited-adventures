/* CKIT.EXE resident module 1c3f: sound events over sound.c's layer.
 * sound_open starts sound for the configured card: its music and digital
 * drivers, the song set and the effect set; when the digital driver fails,
 * effects fall back to card 0's effect mode.  sound_close shuts it down.
 * sound_event takes the event numbers the game plays by: 0 and 1 clear and
 * set sfx_enabled, 2 waits for the song, 3..0x0f play an effect, 0x20..0x27
 * play a song, 0x28 and 0x29 set the music loud or quiet, and 0xff stops
 * the song and waits for the effect.
 *
 * EXE 0x00e7f3..0x00ecee, one Turbo C large-model _TEXT segment starting at
 * byte 3 of paragraph 1c3f, the byte after sound.c's last retf.  Built with
 * Turbo C 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order;
 * bake consumes them in that order.  Its last routine, music_volume, ends
 * at 1c3f:04fd (1c8e:000d); module 1c8e begins at 1c8e:000e.  The 23 MZ
 * relocation sites in the range are filed under frame 0c3f and module
 * 1c8e's under 0c8e; 1c8e reaches this module only by far calls through
 * 0c3f, so the two are separate objects.
 *
 * _DATA is DGROUP 1dfa..23ba: the sound state, the per-card driver and
 * mode tables, the song and effect file names, and two literals.  There is
 * no _BSS.  1df9 and 23bb are pad.  No instruction outside the module
 * names this _DATA as a DGROUP operand: a scan of the whole image's bytes
 * (moffs, disp16 and imm16 forms) with sound_card as its positive control
 * finds none.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  A name another module
 * here gives an address is reused from there; a modNNNN_ name is a
 * placeholder that says only which segment the callee lives in.
 * music_volume is static: sound_event near-calls it ahead of its
 * definition.
 */

extern int  far sound_init(char far *music_drv, char far *voc_drv); /* 0ac2:000d */
extern void far sound_shutdown(void);                            /* 0ac2:0182 */
extern int  far sound_play(char far *file, int index, int volume); /* 0ac2:026a */
extern int  far sound_volume(char far *file, int index, int volume); /* 0ac2:04d0 */
extern int  far sound_wait(char far *file, int index, char interruptible); /* 0ac2:0625 */
extern int  far sound_stop(char far *file, int index);           /* 0ac2:07b0 */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_format(char far *buf, char far *fmt, ...);  /* 1860:0037 */
extern void far slot_close(int slot);                           /* 1336:06fb */
extern void far lib_open(char mode, char far *name, int slot);   /* 147e:00d1 */
extern void far sample_library_set(int slot);                          /* 143b:0002 */
extern void far sample_sequence_play(int count, ...);                    /* 143b:0012 */

extern int sound_card;                                           /* b394 */

static char music_ok = 0;                                        /* 1dfa */
static char sfx_ok = 0;                                          /* 1dfb */
static char sound_live = 0;                                      /* 1dfc */
static char music_drv[0x50] = "";                                /* 1dfd */
static char voc_drv[0x50] = "";                                  /* 1e4d */
static int music_mode = 0;                                       /* 1e9d */
static int sfx_mode = 0;                                         /* 1e9f */
static char sfx_file[13] = "";                                   /* 1ea1 */
static int sfx_index = -999;                                     /* 1eae */
static char music_file[13] = "";                                 /* 1eb0 */
static int music_index = -999;                                   /* 1ebd */
static int song_part[8] = { 1, 2, 2, 2, 2, 2, 2, 3 };            /* 1ebf */
static int song_index[8] = { 0, 0, 1, 2, 3, 4, 5, 0 };           /* 1ecf */
static int card_music_mode[7] = { 3, 1, 1, 0, 0, 0, 0 };         /* 1edf */
static int card_sfx_mode[7] = { 1, 1, 0, 1, 0, 0, 0 };           /* 1eed */
static char card_music_drv[7][0x50] = {                          /* 1efb */
    "PCSPKR.ADV", "ADLIB.ADV", "ADLIB.ADV",
    "MT32MPU.ADV", "MT32MPU.ADV", "", ""
};
static char card_voc_drv[7][0x50] = {                            /* 212b */
    "", "", "SBDIG.ADV", "", "SBDIG.ADV", "", ""
};
static char music_names[4][13] = { "RODQ", "ADDQ", "TYDQ", "PCDQ" }; /* 235b */
static char sfx_names[2][13] = { "SFXDQ.VOC", "" };              /* 238f */
static char sfx_enabled = 1;                                     /* 23a9 */
static char music_loud = 1;                                      /* 23aa */

static int music_volume(char loud, int mode);

int far sound_open(char far *digital)                            /* 1c3f:0003 */
{
    short want;
    register int card;
    register int rc;

    if (sound_card == 5)
        goto off;
    sound_live = 1;
    card = sound_card;
    str_copy(music_drv, card_music_drv[card]);
    str_copy(voc_drv, card_voc_drv[card]);
    want = (*music_drv ? 1 : 0) + (*voc_drv ? 1 : 0);
    music_mode = card_music_mode[card];
    sfx_mode = card_sfx_mode[card];
    if ((rc = sound_init(music_drv, voc_drv)) == want)
        goto on;
    if (rc >= 0)
        goto off;
    rc = rc * -1;                       /* mul by 0xffff, not neg */
    music_ok = rc & 1 ? 0 : 1;
    *digital = sfx_ok = rc & 2 ? 0 : 1;
    if (music_ok && !sfx_ok && *voc_drv) {
        *voc_drv = 0;
        *digital = sfx_ok = 1;
        sfx_mode = card_sfx_mode[0];
        slot_close(0x17);
        goto loaded;
    }
    return -2;
off:
    *digital = sfx_ok = music_ok = 0;
    return 0;
on:
    *digital = sfx_ok = music_ok = 1;
loaded:
    if (!*voc_drv) {
        sample_library_set(0x17);
        lib_open('1', "sounds", 0x17);
    }
    return 0;
}

void far sound_close(void)                                       /* 1c3f:015c */
{
    if (sound_live) {
        sound_shutdown();
        sound_live = 0;
    }
}

int far sound_event(register int n)                              /* 1c3f:016e */
{
    short vol;
    char far *name;
    char file[14];
    register int rc;

    rc = 0;
    switch (n) {        /* `2e 3b 07` at 0183: Turbo C's sparse-switch value search */
    case 0:
        sfx_enabled = 0;
        break;
    case 1:
        sfx_enabled = 1;
        break;
    case 0x29:
        music_loud = 0;
        if (music_index < 0)
            break;
        vol = music_volume(music_loud, music_mode);
        rc = sound_volume(music_file, music_index, vol);
        if (music_mode != 3)
            break;
        rc = sound_stop(music_file, music_index);
        goto stopped;   /* shipped keeps case 2's copy of this tail */
    case 0x28:
        music_loud = 1;
        if (music_index < 0)
            break;
        vol = music_volume(music_loud, music_mode);
        rc = sound_volume(music_file, music_index, vol);
        break;
    case 2:
        if (music_index < 0)
            break;
        rc = sound_wait(music_file, music_index, 0);
    stopped:
        if (rc == 0)
            music_index = -1;
        break;
    case 0xff:
        if (music_index >= 0) {
            rc = sound_stop(music_file, music_index);
            if (rc == 0)
                music_index = -1;
        }
        if (rc)
            break;
        if (sfx_index < 0)
            break;
        if (sfx_mode == 1)
            break;
        rc = sound_wait(sfx_file, sfx_index, 0);
        if (rc == 0)
            sfx_index = -1;
        break;
    default:
        if (n >= 3 && n < 0x10) {
            if (!sfx_ok)
                return 0;
            n -= 3;
            if (sfx_mode == 1) {
                if (music_index >= 0) {
                    rc = sound_stop(music_file, music_index);
                    if (rc == 0)
                        music_index = -1;
                }
                if (rc)
                    break;
                sample_sequence_play(1, n, -1, -1, 1, -1);
                break;
            }
            if (!sfx_enabled)
                break;
            if (sfx_index >= 0) {
                rc = sound_wait(sfx_file, sfx_index, 0);
                if (rc == 0)
                    sfx_index = -1;
            }
            if (rc)
                break;
            rc = sound_play(sfx_names[sfx_mode], n, 100);
            if (rc)
                break;
            if (sfx_index < -1)         /* -999 until the first effect */
                str_copy(sfx_file, sfx_names[sfx_mode]);
            sfx_index = n;
        } else if (n >= 0x20 && n < 0x28) {
            if (!music_ok)
                return 0;
            n -= 0x20;
            name = music_names[music_mode];
            str_format(file, "%s%d.XMI", name, song_part[n]);
            n = song_index[n];
            vol = music_volume(music_loud, music_mode);
            if (music_index >= 0) {
                rc = sound_stop(music_file, music_index);
                if (rc == 0)
                    music_index = -1;
            }
            if (rc == 0 && sfx_index >= 0 && sfx_mode != 1) {
                rc = sound_wait(sfx_file, sfx_index, 0);
                if (rc == 0)
                    sfx_index = -1;
            }
            if (rc)
                break;
            if (!music_loud && music_mode == 3)
                break;
            rc = sound_play(file, n, vol);
            if (rc)
                break;
            str_copy(music_file, file);
            music_index = n;
        } else {
            rc = -5;
        }
        break;
    }
    return rc;
}

static int music_volume(char loud, int mode)                     /* 1c3f:04d9 */
{
    register int v;

    v = loud ? 100 : 0;
    if (loud && mode == 0)
        v = 90;
    return v;
}
