/* CKIT.EXE resident module 2782: the state around module 213b's speaker
 * sound.  Five 14-byte slots whose first four bytes are a far pointer, a
 * word set to -1 alongside them, a flag and a far hook called with the
 * timer tick count.  Starting up calls 213b's start-up, zeroes the slot
 * pointers and the hook, sets the word to -1 and the flag to 1; stopping
 * clears the slots and silences 213b (port 0x61, and port 0xc0 when 213b's
 * flag at d99c is set).  sfx_update reads the tick count from 207e and
 * passes it to sfx_tick, which hands it to the hook when one is set; the
 * far call at 0x01222e hands sfx_tick sound_ticks's count the same way.
 * No operand outside this module in the recovered listings names DGROUP
 * e6b8..e704, so nothing found sets the hook to a function, reads the flag
 * or touches a slot's last ten bytes.
 *
 * EXE 0x019c2e..0x019cdb, one Turbo C large-model _TEXT segment starting at
 * byte e of paragraph 2782.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * It starts the byte after module 25e5's last retf (25e5:19dd); module 278d
 * starts at 0x019cdb (278d:000b) with no stub between.  All seven MZ
 * relocation sites that store segment 1782 are far calls to this module's
 * five routines, and no overlay listing names segment 1782.
 *
 * _BSS is e6b8..e704 and e705 is pad; the far pointer at e6b4 is named
 * only by overlay section 25, and e706 by 27a1:0135.  There is no _DATA.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  sfx_slots_init is DQK's name for this module's init, as sysutil.c
 * calls it; the mod1782_ names are sysutil.c's and diskfile.c's; the other
 * externs are placeholders that say only which segment the callee lives in.
 *
 * Turbo C far-calls a public function of this module that is not yet
 * defined: sfx_slots_done far-calls sfx_reset and sfx_update far-calls
 * sfx_tick, so those two carry prototypes.
 */

struct SfxSlot {
    void far *data;             /* +0, the only field this module touches */
    char rest[10];              /* +4                                     */
};

extern void far speaker_init(void);                              /* 113b:000b */
extern void far speaker_done(void);                              /* 113b:0045 */
extern void far speaker_silence(void);                              /* 113b:005d */
extern long far sound_ticks(void);                              /* 107e:00c8 */

void far sfx_reset(void);
void far sfx_tick(long ticks);

static int sfx_cur;                                              /* e6b8 */
static void (far *sfx_hook)(long ticks);                         /* e6ba */
static char sfx_armed;                                           /* e6be */
static struct SfxSlot sfx_slot[5];                               /* e6bf */

void far sfx_slots_init(int mode, char far *far *argv)             /* 2782:000e */
{
    register int i;

    speaker_init();
    for (i = 0; i < 5; i++) {
        sfx_slot[i].data = 0;
    }
    sfx_cur = -1;
    sfx_armed = 1;
    sfx_hook = 0;
}

void far sfx_slots_done(void)                                      /* 2782:0055 */
{
    sfx_reset();
    speaker_done();
}

void far sfx_reset(void)                                      /* 2782:0060 */
{
    register int i;

    sfx_cur = -1;
    for (i = 0; i < 5; i++) {
        sfx_slot[i].data = 0;
    }
    speaker_silence();
}

void far sfx_update(void)                                      /* 2782:0092 */
{
    sfx_tick(sound_ticks());
}

void far sfx_tick(long ticks)                                    /* 2782:00a1 */
{
    if (sfx_hook)
        sfx_hook(ticks);
}
