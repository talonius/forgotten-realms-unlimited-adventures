/* CKIT.EXE resident module 1c8e: sound effects and the sound switches.
 * sfx_play takes module 1c3f's event numbers and drops any above 0x29
 * but 0xff.  Events 0 and 1 mute and unmute effects through sound_toggle;
 * effects 3..0x0f go to sound_event when digital is set, and otherwise
 * sound a two-tick tone on the speaker; with sound_card 5 effects and songs
 * (0x20..0x27) are skipped.  sfx_init asks slot_used(-1) for the far
 * memory left: under 0x33c00 bytes it prints how much more the sound
 * driver needs and clears digital, otherwise it sets digital and passes
 * its address to sound_open, which stores through it; either way music_on
 * starts set.  game_sound_stop shuts sound down (misc.c's game_exit is one
 * of its two far callers).  sound_toggle flips the speaker's on flag and
 * sends event 1 or 0; music_toggle flips music_on and sends 0x28 or 0x29.
 * They are the Ins (0x152) and Del (0x153) keys in misc.c, menu.c and
 * listbox.c, among 16 and 15 far call sites (one of the 16 is this
 * module's own, at 1c8e:00bb).
 *
 * EXE 0x00ecee..0x00ef56, one Turbo C large-model _TEXT segment starting at
 * byte e of paragraph 1c8e.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * Module 1c3f's last routine ends with the retf at 0x00eced.  The lone
 * retf at 0x00ef56 is module 1cb5's (charpic.c).  Every routine here is
 * far-called: sfx_setup once, from 0x0391b6, and sfx_init once, from
 * 0x039236.  The 25 MZ relocation sites in the range are its far calls,
 * filed under frame 0c8e.
 *
 * _DATA is DGROUP 23bc..2436, sfx_init's four literals; 23bb (after
 * sndevent.c's _DATA) and 2437 are pad.  _BSS is ad92..ad93, between
 * sound.c's and packtext.c's.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call, or the DGROUP offset of an extern variable.  A name another module
 * gives an address is reused from there, this module's own mod0c8e_ names
 * included; sfxctl_ names are placeholders for routines nothing named.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined: sfx_play far-calls sound_toggle, so that one carries a
 * prototype.
 */

extern long far slot_used(int slot);                             /* 1336:0921 */
extern void far clear_screen(int a);                             /* 0da7:029d */
extern void far text_print_at(int row, int col, int a, char far *fmt, ...); /* 0da7:0399 */
extern void far key_wait(void);                                  /* 0da7:00f5 */
extern long far timer_ticks(void);                              /* 107e:00c0 */
extern void far speaker_silence(void);                              /* 113b:005d */
extern void far voice_set(int voice, unsigned freq, int volume); /* 113b:007b */
extern void far speaker_enable(char on);                           /* 113b:01b4 */
extern char far speaker_enabled(void);                              /* 113b:01c9 */
extern int  far sound_open(char far *digital);                   /* 0c3f:0003 */
extern void far sound_close(void);                               /* 0c3f:015c */
extern int  far sound_event(int n);                              /* 0c3f:016e */

extern int sound_card;                                           /* b394 */

static char digital;                                             /* ad92 */
static char music_on;                                            /* ad93 */

void far sound_toggle(void);

void far sfx_play(unsigned short n)                           /* 1c8e:000e */
{
    long until;

    if (n >= 0 && n < 0x2a || n == 0xff) {      /* n >= 0 is always true, as shipped */
        switch (n) {    /* 2e 3b 07 at 0033: Turbo C's sparse-switch value search */
        case 0:
            if (speaker_enabled())
                sound_toggle();
            break;
        case 1:
            if (!speaker_enabled())
                sound_toggle();
            break;
        case 2:
            sound_event(n);
            speaker_silence();
            break;
        case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10:
        case 11: case 12: case 13: case 14: case 15:
            if (sound_card == 5)
                break;
            if (digital) {
                sound_event(n);
                break;
            }
            until = timer_ticks() + 2;
            voice_set(0, 0xe24, 0x7f);
            voice_set(1, 2, 0x7f);
            while (timer_ticks() < until)
                ;
            speaker_silence();
            break;
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
        case 0x26: case 0x27:
            if (sound_card == 5)
                break;
        case 0x28: case 0x29: case 0xff:
            sound_event(n);
            break;
        }
    }
}

void far sfx_setup(void)                                       /* 1c8e:014e */
{
}

void far sfx_init(void)                                       /* 1c8e:014f */
{
    long avail;

    avail = slot_used(-1);
    if (avail < 0x33c00L) {
        digital = 0;
        clear_screen(0);
        text_print_at(0x1f5c, 0x1f44, 0xf, "There is not enough memory to load");
        text_print_at(0x1f64, 0x1f44, 0xf, "sound driver, need %l bytes more.",
                      0x33c00L - avail);
        text_print_at(0x1fa0, 0x1f44, 0xa, "Please hit any key to continue.");
        key_wait();
        clear_screen(0);
        text_print_at(0x1fa0, 0x1f40, 0xa, "Loading...Please Wait");
    } else {
        digital = 1;
        sound_open(&digital);
    }
    music_on = 1;
}

void far game_sound_stop(void)                                      /* 1c8e:0221 */
{
    sound_close();
}

void far sound_toggle(void)                                      /* 1c8e:0227 */
{
    speaker_enable(!speaker_enabled());
    sound_event(speaker_enabled() ? 1 : 0);
}

void far music_toggle(void)                                      /* 1c8e:0252 */
{
    music_on = !music_on;
    sound_event(music_on ? 0x28 : 0x29);
}
