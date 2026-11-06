/* CKIT.EXE resident module 213b: the PC speaker and the Tandy/PCjr sound
 * chip.  speaker_init turns sound on and records in spk_chip whether either
 * of two section-6 tests (18de:202f, 18de:2025) found the chip; with the chip
 * it saves port 0x61 in spk_old and sets bits 5 and 6.  speaker_done silences
 * both and, with the chip, puts port 0x61 back.  speaker_silence clears the
 * speaker gate bits of port 0x61 and, with the chip, sets its four channels
 * to attenuation 15 through port 0xc0.  voice_set sets one voice: with the
 * chip, voices below 4 go to it; any other voice outside 1..3 goes to the
 * speaker through PIT channel 2.  voice_note turns a note into a divisor --
 * a semitone from one of two 12-entry tables, then an octave shift -- and
 * hands it to voice_set.  speaker_enable and speaker_enabled set and read
 * spk_on, without which voice_set does nothing.
 *
 * EXE 0x0137bb..0x01397d, one Turbo C large-model _TEXT segment starting at
 * byte b of paragraph 213b.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * It starts the byte after module 20f8's last retf (0x0137ba); module 2157
 * starts at 0x01397d (2157:000d) with no stub between.  All five MZ
 * relocation sites in the range are this module's far calls.  Nothing in
 * CKIT.EXE calls speaker_noop, speaker_argument_noop or voice_note.
 *
 * _DATA is 9ebe..9eed, the two divisor tables.  _BSS is d99a..d99c; d999
 * (after lowio.c's clips_bottom) and d99d are pad.  Nothing outside this
 * module names any of them.  The _BSS names
 * reproduce the shipped order (Turbo C's symbol-table order, checked with
 * goldbox.bss_order).
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call; a variable's is its DGROUP offset.  mod113b_ names are the ones
 * sfxslots.c and sysutil.c call these routines by; is_tandy and
 * int_min are fcache.c's and lowio.c's; a modNNNN_ name is a
 * placeholder that says only which segment the called address lives in.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined: speaker_init and speaker_done far-call speaker_silence, so it
 * carries a prototype.
 */

#include <dos.h>

extern int  far is_pcjr(void);      /* 18de:202f, RTLink vector into section 6 */
extern int  far is_tandy(void);      /* 18de:2025, RTLink vector into section 6 */
extern int  far int_min(int a, int b);                      /* 00d7:2636 */

void far speaker_silence(void);

static unsigned pit_divs[12] = {         /* 9ebe, 1193180 Hz / C4..B4 */
    0x11d1, 0x10d1, 0x0fdf, 0x0efb, 0x0e24, 0x0d59,
    0x0c99, 0x0be4, 0x0b39, 0x0a98, 0x0a00, 0x0970
};
static unsigned sn_divs[12] = {          /* 9ed6, 3579545 / 32 Hz / C4..B4 */
    0x01ab, 0x0194, 0x017d, 0x0167, 0x0153, 0x0140,
    0x012e, 0x011d, 0x010d, 0x00fe, 0x00f0, 0x00e2
};

static char spk_old;                    /* d99a, port 0x61 as start-up found it */
static char spk_on;                      /* d99b */
static char spk_chip;                    /* d99c, the Tandy/PCjr chip is there */

void far speaker_init(void)                                      /* 213b:000b */
{
    spk_on = 1;
    spk_chip = is_pcjr() || is_tandy();
    speaker_silence();
    if (spk_chip) {
        spk_old = inportb(0x61);
        outportb(0x61, spk_old | 0x60);
    }
}

void far speaker_done(void)                                      /* 213b:0045 */
{
    speaker_silence();
    if (spk_chip)
        outportb(0x61, spk_old);
}

void far speaker_noop(void)                                      /* 213b:0057 */
{
}

void far speaker_argument_noop(int unused)                                /* 213b:0058 */
{
}

void far speaker_silence(void)                                      /* 213b:005d */
{
    outportb(0x61, inportb(0x61) & 0xfc);
    if (spk_chip) {
        outportb(0xc0, 0x9f);
        outportb(0xc0, 0xbf);
        outportb(0xc0, 0xdf);
        outportb(0xc0, 0xff);
    }
}

void far voice_set(int voice, unsigned freq, int volume)      /* 213b:007b */
{
    char bits;

    if (spk_on) {
        if (spk_chip && voice < 4) {
            volume = (volume >> 3) & 0xf;
            bits = voice << 5;
            outportb(0xc0, (freq & 0xf) | bits | 0x80);
            if (voice < 3)
                outportb(0xc0, (freq >> 4) & 0x3f);
            outportb(0xc0, (0xf - volume) | bits | 0x90);
        } else if (voice < 1 || voice > 3) {
            bits = inportb(0x61) & 0xfc;
            if (freq) {
                outportb(0x43, 0xb6);
                outportb(0x42, freq);
                outportb(0x42, freq >> 8);
                outportb(0x61, bits | 3);
            } else
                outportb(0x61, bits);
        }
    }
}

void far voice_note(int voice, int note, int volume)           /* 213b:011d */
{
    register unsigned div;

    if (volume < 0) {
        div = volume = 0;
    } else {
        if (spk_chip && voice < 3)
            div = sn_divs[note % 12];
        else
            div = pit_divs[note % 12];
        note = note / 12 - 5;
        if (note < 0)
            div <<= int_min(4, -note);
        else if (note)
            div >>= note;
        if (spk_chip)
            while (div >= 0x400)
                div >>= 1;
    }
    voice_set(voice, div, volume);
}

void far speaker_enable(char on)                                   /* 213b:01b4 */
{
    spk_on = on;
    if (!on)
        speaker_silence();
}

char far speaker_enabled(void)                                      /* 213b:01c9 */
{
    return spk_on;
}
