/* CKIT.EXE data-only module between mapevent.c and saveload.c: sixteen
 * bytes, DGROUP 5d44..5d53, holding 0..15 in a shuffled order.  Nothing in
 * the image reads them; what they were for is not known.  5d43, after
 * mapevent.c's literals, is the link's word-alignment pad.  No routines; its
 * empty _TEXT sits at saveload.c's start in section 4.
 */

unsigned char shuffle16[16] = {                 /* 5d44 */
    0, 7, 12, 13, 2, 14, 11, 9, 8, 6, 5, 15, 3, 10, 4, 1
};
