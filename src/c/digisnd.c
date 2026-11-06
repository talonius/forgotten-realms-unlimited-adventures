/* CKIT.EXE resident module 243b: digitized sounds.  A sound is a resource
 * item: a word sample rate, a long sample count, then the samples.
 * sample_library_set switches item lookup from a bare fcache slot to an item of
 * the library opened in a slot, sample_sequence_play hands a list of items, each
 * cut by a start and a length in 1/200 s units, to 10d7:10df, which plays
 * them on the PC speaker a number of times until a key is pressed, and
 * sample_play_rate hands it one item at a rate its caller gives.  Both do
 * nothing when module 213b's speaker_enabled returns 0, and both pass the rate
 * as the timer divisor 1193181 / rate, rounded (0x1234dd, the PIT input clock).
 *
 * A PORT of DQK's v11/src/c/digisnd.c (module 28fc), token for token: only
 * names, callee addresses and DGROUP offsets differ.
 *
 * EXE 0x0167b0..0x016be3, one Turbo C large-model _TEXT segment starting at
 * byte 0 of paragraph 243b.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * fcache.c's module 2336 ends with the retf at 0x0167af, and library.c's
 * module 247e starts at 0x016be3 with no stub between.
 *
 * The two empty routines at 243b:0000 and 0001 are claimed here: they open
 * this module's own paragraph, and no far call or far pointer in the image
 * reaches them as 143b:0000/0001 or as fcache.c's 1336:1050/1051.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  A name another module gives an address is reused from there; the
 * rest are placeholders.
 *
 * in_library (_DATA a3c0) and lib_slot (_BSS e66a) are read by nothing
 * outside this module.
 */

struct Clip {                   /* one item queued by sample_sequence_play          */
    long len;                   /* +0  samples to play                      */
    unsigned rate;              /* +4  Hz, then the timer divisor           */
    char far *data;             /* +6  first sample                         */
    char arg;                   /* +a  passed through to 10d7:10df          */
};

extern char far speaker_enabled(void);                              /* 113b:01c9 */
extern void far *far slot_base(int slot);                        /* 1336:08fe */
extern char far *far lib_item(void far *lib, int n);             /* 147e:01f9 */
extern char far key_pressed(void);                               /* 0da7:009a */
extern void far *far huge_add(void far *p, long n);          /* 00d7:0d7d */
extern void far huge_move(void far *src, void far *dst, unsigned n); /* 00d7:08c2 */
extern int  far swap16(int v);                             /* 00d7:098b */
extern long far swap32(long v);                            /* 00d7:0994 */
extern void far speaker_play(void far *p, int len, unsigned div, int arg); /* 00d7:10df */

static char in_library = 0;                                      /* a3c0 */
static char lib_slot;                                            /* e66a */

/* Empty init of the library init/done pair at the module head (plausible). */
void far digisnd_init(void)                                      /* 243b:0000 */
{
}

/* Empty done of the library init/done pair at the module head (plausible). */
void far digisnd_done(void)                                      /* 243b:0001 */
{
}

void far sample_library_set(int slot)                                  /* 243b:0002 */
{
    lib_slot = slot;
    in_library = 1;
}

/* The arguments after count come in fours -- item, start, length, arg --
 * ended by a negative item; a start or length of 0 or less leaves the
 * item's own.  clip[] holds 25 entries but the loop admits n up to 25, a
 * 26th, as shipped. */
void far sample_sequence_play(int count, ...)                            /* 243b:0012 */
{
    struct Clip clip[25];
    long div;
    int far *ap;
    char far *p;
    short rep;
    short step;
    register int n, j;

    if (!speaker_enabled())
        return;
    ap = &count;
    ap++;
    for (n = 0; *ap >= 0 && n <= 25; n++) {
        if (in_library)
            p = lib_item(slot_base(lib_slot), *ap++);
        else
            p = slot_base(*ap++);
        huge_move(p, &clip[n].rate, 2);
        clip[n].rate = swap16(clip[n].rate);
        step = clip[n].rate / 200;
        clip[n].rate = div = ((clip[n].rate >> 1) + 0x1234ddL) / clip[n].rate;
        p = huge_add(p, 2L);
        huge_move(p, &clip[n].len, 4);
        clip[n].len = swap32(clip[n].len);
        clip[n].data = huge_add(p, 4L);
        if (*ap > 0)
            clip[n].data = huge_add(clip[n].data, *ap * step);
        ap++;
        if (*ap > 0)
            clip[n].len = (long)*ap * step;
        ap++;
        clip[n].arg = *ap;
        ap++;
    }
    for (rep = 0; rep < count; rep++) {
        for (j = 0; j < n; j++) {
            /* len goes as its low word only, as shipped (push es:[bx]) */
            speaker_play(clip[j].data, clip[j].len, clip[j].rate, clip[j].arg);
            if (key_pressed())
                break;
        }
        if (key_pressed())
            break;
    }
}

/* The census's `rot` marker, `33 d2 d1 fa d1 d8` at 243b:03a8, is Turbo C's
 * inline (long)div >> 1: xor dx,dx; sar dx,1; rcr ax,1. */
void far sample_play_rate(int item, unsigned short rate, int arg)    /* 243b:0356 */
{
    long len;
    unsigned short div;
    void far *data;
    short unused;               /* referenced below only to keep its [bp-6] slot */
    char far *p;

    (void)unused;
    if (!speaker_enabled())
        return;
    if (in_library)
        p = lib_item(slot_base(lib_slot), item);
    else
        p = slot_base(item);
    div = rate;
    div = (((long)div >> 1) + 0x1234ddL) / div;
    p = huge_add(p, 2L);
    huge_move(p, &len, 4);
    len = swap32(len);
    data = huge_add(p, 4L);
    speaker_play(data, len, div, arg);
}
