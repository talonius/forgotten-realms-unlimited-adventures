/* CKIT.EXE overlay 06 module 2294: the manual-word check.
 * journal_check picks one of twenty headings in the rule book and one of
 * its three listed words, prints the page and which word after the
 * heading to type, and reads the reply; after three wrong replies it
 * gives up.  It returns 1 when the reply matched, else 0.  The headings
 * and words are stored scrambled -- reversed, each character raised by
 * the word's length -- and unscramble undoes that.  Each stage it reaches
 * sets one of copyprot_markers[1..4].
 *
 * EXE 0x0399a5..0x039c87, one Turbo C large-model _TEXT segment starting at
 * byte 5 of paragraph 2294 in RTLink section 6 (payload 0x38da0, link
 * segment 21d4; a child of section 3).  The first 5 bytes of paragraph
 * 2294 are the tail of module 226c, and the last 7 bytes of the range lie
 * in paragraph 22c2, whose module starts at 22c2:0007.  Built with Turbo C
 * 2.01, tcc -ml -O -Z -d.  Functions are defined in shipped order; bake
 * consumes them in that order.
 *
 * A bare stored seg:off in this file means section 6.  The seg:off beside
 * each extern is the far pointer as stored in the shipped call, or the
 * DGROUP offset of an extern variable; a definition's comment gives the
 * load-based address, as the resident modules' do.  copyprot_unit_init is the
 * lone retf at 2294:0005, reached by no vector and no far call, and is
 * this module's by the run's rule; the bytes do not decide it.
 * journal_check is reached only through the vector stub 18de:143b, which
 * section 3's gamemain.c calls journal_check.
 *
 * _DATA is DGROUP 6bc8..706e: quiz_table, whose 80 far pointers are
 * relocated in DGROUP's own table (section 27), then the scrambled words
 * and eight stored literals; "" is stored on the first heading's NUL.
 * _BSS is d13c..d191.  Nothing found in the image references unused; it
 * keeps the shipped layout.  Turbo C lays _BSS out by the statics' names,
 * and these names give the shipped order.
 */

struct Quiz {
    char far *heading;          /* +0  scrambled                          */
    unsigned char page;         /* +4  rule book page                     */
    unsigned char word_no[3];   /* +5  word's place after the heading     */
    char far *word[3];          /* +8  scrambled                          */
};

extern int  far str_same(char far *a, char far *b);          /* 00d7:253f */
extern void far str_copy(char far *dst, char far *src);      /* 00d7:255e */
extern int  far str_length(char far *s);                         /* 0a75:0054 */
extern int  far random_n(int a);                                 /* 0a75:0354 */
extern char far *far msg_format(char far *fmt, ...);             /* 0a75:03fa */
extern void far mem_zero(void far *p, int n);                   /* 0d48:0017 */
extern char far *far byte_str(unsigned char n);                  /* 0d48:0181 */
extern void far status_printf(unsigned char col, unsigned char row, unsigned char fg,
                              unsigned char bg, char far *fmt, ...); /* 06c9:0044 */
extern char far *far input_text(char far *prompt, unsigned char fg, unsigned char bg,
                                unsigned char max);              /* 06c9:05bc */
extern void far text_clear(char x1, char y1, char x2, char y2);  /* 06c9:0994 */
extern void far frame_border(void);                              /* 0d65:002a */

extern char copyprot_markers[];                                          /* c650 */

static struct Quiz quiz_table[] = {                              /* 6bc8 */
    /* INTRODUCTION: WELCOME OFFICIAL DUNGEONS */
    { "Z[U`OaP[^`ZU", 1, { 1, 4, 6 },
      { "LTVJSL^", "TIQKQNNW", "[VWMOV]L" } },
    /* BEFORE YOU BEGIN: COPY PROTECTION YOUR */
    { "^YWUR0e_i0Ub_VUR", 1, { 4, 5, 7 },
      { "]TSG", "XYS^MO^Y\\Z", "VYS]" } },
    /* USING MENUS: COMMANDS BASED GAME */
    { "^`YPX+RYT^`", 1, { 2, 5, 7 },
      { "[LVIUUWK", "IJXFG", "IQEK" } },
    /* BEGINNING TO PLAY: START RUNNING FIND */
    { "jR]a1`e1X_Z__ZXVS", 2, { 3, 4, 9 },
      { "YWFYX", "NUPUU\\Y", "HRMJ" } },
    /* NON-PLAYER CHARACTERS: DURING PARTY ENCOUNTERS */
    { "hgZiXVgV]X5gZnVaeBcdc", 4, { 1, 5, 6 },
      { "MTOX[J", "^YWFU", "]\\O^X_YMXO" } },
    /* VIEWING CHARACTERS: SUMMARY SCREEN DISPLAYED */
    { "edWfUSdSZU2Y`[iW[h", 5, { 3, 4, 6 },
      { "`YHTT\\Z", "TKKXIY", "MNbJUY\\RM" } },
    /* CHARACTER STATUS: MEANS POSITIVE HIT */
    { "cedQdc0bUdSQbQXS", 5, { 3, 8, 9 },
      { "XSFJR", "M^Q\\Q[WX", "WLK" } },
    /* ADVENTURING: SETTING READY HEAD */
    { "RYT]`_YPaOL", 7, { 2, 8, 10 },
      { "NUP[[LZ", "^IFJW", "HEIL" } },
    /* ADVENTURING OPTIONS: FOLLOWING CONTROLS BASIC */
    { "fab\\gcb3Za\\ehgaXiWT", 7, { 2, 4, 5 },
      { "PWR`XUUXO", "[TWZ\\VWK", "HNXFG" } },
    /* ENCAMPING: OPTIONS SUCH SAVING */
    { "PWRYVJLWN", 8, { 5, 6, 8 },
      { "ZUVP[WV", "LGYW", "MTO\\GY" } },
    /* MAGIC: GET ACTIVE CHARACTER */
    { "HNLFR", 9, { 2, 8, 9 },
      { "WHJ", "K\\OZIG", "[N]LJ[JQL" } },
    /* CIVILIZATION: TOWNS VALUABLE SUPPLIES */
    { "Z[U`MfUXUbUO", 10, { 1, 4, 7 },
      { "XS\\TY", "MTJI]TI^", "[MQTXX][" } },
    /* ENCOUNTERS: WHEN ACROSS MONSTERS */
    { "]\\O^X_YMXO", 12, { 1, 5, 6 },
      { "RIL[", "YYUXIG", "[ZM\\[VWU" } },
    /* COMBAT: COMPUTER CHOOSES WITH */
    { "ZGHSUI", 12, { 4, 5, 10 },
      { "ZM\\]XUWK", "ZLZVVOJ", "LXM[" } },
    /* AFTER COMBAT: OVER MANY EXPERIENCE */
    { "`MNY[O,^Q`RM", 13, { 4, 8, 9 },
      { "VIZS", "]REQ", "OMXOS\\OZbO" } },
    /* PLAYER RACES: SIX WHICH CONSTRUCT */
    { "_QOM^,^QeMX\\", 14, { 3, 6, 9 },
      { "[LV", "MHNM\\", "]L^[]\\WXL" } },
    /* ABILITY SCORES: EVERY RANDOMLY GENERATED */
    { "aS`]Qa.gbWZWPO", 15, { 1, 5, 6 },
      { "^WJ[J", "aTUWLVIZ", "MN]J[NWNP" } },
    /* CHARACTER CLASSES: PROFESSIONS CHARACTER MUST */
    { "dVddR]T1cVeTRcRYT", 16, { 5, 7, 8 },
      { "^YZT^^PQZ][", "[N]LJ[JQL", "XWYQ" } },
    /* ALIGNMENT: PHILOSOPHY LIVES AFFECT */
    { "]WNVWPRUJ", 17, { 4, 7, 11 },
      { "cRZY]YVSRZ", "XJ[NQ", "ZIKLLG" } },
    /* OTHER ATTRIBUTES: ALSO IMPORTANT VALUES */
    { "cUdeRYbddQ0bUXd_", 18, { 3, 6, 7 },
      { "SWPE", "]WJ][XYVR", "YK[RG\\" } },
};

static char unused;                                              /* d13c */
static char answer[41];                                          /* d13d */
static unsigned char ntries;                                     /* d166 */
static char response[41];                                        /* d167 */
static unsigned char section;                                    /* d190 */
static unsigned char word_no;                                    /* d191 */

/* The translated Pascal unit's empty initialization block (DKK copyprot.pas). */
void far copyprot_unit_init(void)                                     /* 3294:0005 */
{
}

void far unscramble(char far *dst, char far *src)                /* 3294:0006 */
{
    char buf[42];
    unsigned char i;
    unsigned char len;

    len = str_length(src);
    str_copy(buf, "");
    for (i = len; i >= 1; i--)
        str_copy(buf, msg_format("%s%c", buf, *(src + i - 1) - len));
    str_copy(dst, buf);
}

char far journal_check(void)                                     /* 3294:008e */
{
    char ok;
    char heading[42];

    mem_zero(copyprot_markers, 5);
    frame_border();
    ntries = 0;
    do {
        ntries++;
        str_copy(answer, "");
        copyprot_markers[1] = 1;
        section = random_n(20);
        do
            word_no = random_n(3);
        while (quiz_table[section].word_no[word_no] == 0xff);
        unscramble(answer, quiz_table[section].word[word_no]);
        status_printf(3, 2, 10, 0, " rule book, page ");
        status_printf(0x13, 2, 13, 0, msg_format("%s%s%s", " (",
                      byte_str(quiz_table[section].page), ")\t\t\t\t "));
        status_printf(3, 4, 10, 0, " what is word # ");
        status_printf(0x13, 4, 13, 0, msg_format("%s%s",
                      byte_str(quiz_table[section].word_no[word_no]), "\t\t "));
        status_printf(3, 6, 10, 0, " after the heading ");
        text_clear(4, 8, 0x26, 8);
        unscramble(heading, quiz_table[section].heading);
        status_printf(4, 8, 13, 0, heading);
        str_copy(response, input_text("Enter ", 11, 0, 20));
        copyprot_markers[2] = 1;
    } while (ntries != 3 && !str_same(response, answer));
    copyprot_markers[3] = 1;
    if (str_same(response, answer))
        ok = 1;
    else
        ok = 0;
    copyprot_markers[4] = 1;
    return ok;
}
