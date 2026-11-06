/* CKIT.EXE resident module 278d: seven far wrappers.  Four pass their
 * arguments to, or return, routines in frame 1f5e: 0ee9 (a mask byte and two
 * far pointers), 0fa2 and 100f (two far pointers each) and 1173; two forward
 * to key_poll and key_get; and the last returns 0.  None has a local, and
 * none reads DGROUP.
 *
 * EXE 0x019cdb..0x019d3b, one Turbo C large-model _TEXT segment starting at
 * byte 0xb of paragraph 278d.  Built with Turbo C 2.01, tcc -ml -O -Z -d.
 * Functions are defined in shipped order; bake consumes them in that order.
 * It starts the byte after module 2782's last retf (0x019cda).  All six MZ
 * relocation sites inside it have frame 178d, and all 15 far calls into it
 * store segment 178d, the last two routines' included, though their bytes
 * round to paragraph 2793.  Module 2793 starts at 0x019d3b with no stub
 * between.
 *
 * The seg:off beside each extern is the far pointer as stored in the shipped
 * call.  This module's own names are the placeholders sysutil.c and
 * dialog.c call it by.
 */

extern int  far mouse_press(char mask, int far *x, int far *y); /* 0f5e:0ee9 */
extern char far mouse_clicked(int far *x, int far *y);            /* 0f5e:0fa2 */
extern void far mouse_pos(int far *x, int far *y);            /* 0f5e:100f */
extern int  far mouse_pending(void);                              /* 0f5e:1173 */
extern char far key_poll(void);                                  /* 107e:00d6 */
extern int  far key_get(void);                                   /* 107e:00e1 */

int far inp_press(char mask, int far *x, int far *y)          /* 278d:000b */
{
    return mouse_press(mask, x, y);
}

char far inp_clicked(int far *x, int far *y)                    /* 278d:0026 */
{
    return mouse_clicked(x, y);
}

void far inp_pos(int far *x, int far *y)                    /* 278d:003e */
{
    mouse_pos(x, y);
}

char far inp_pending(void)                                      /* 278d:0056 */
{
    return mouse_pending();
}

char far kbd_waiting(void)                                      /* 278d:005c */
{
    return key_poll();
}

int far kbd_read(void)                                       /* 278d:0062 */
{
    return key_get();
}

char far aux_pending(void)                                      /* 278d:0068 */
{
    return 0;
}
