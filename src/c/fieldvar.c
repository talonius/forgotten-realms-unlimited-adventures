/* CKIT.EXE data-only module between labels.c and menubox.c: the text and
 * number fields' mouse state, DGROUP 5eca..5ece.  edit_begin (textedit.c) and
 * its number twin (numfield.c) keep the pointer's place in focus_mouse_row and
 * focus_mouse_col and clear field_released; edit_end sets it again.  5ec9
 * (after labels.c's literals) and 5ecf (before menubox.c's) are the link's
 * word-alignment pads.  No routines; its empty _TEXT sits at menubox.c's
 * start in section 5.
 */

char field_released = 1;                        /* 5eca */
int focus_mouse_row = -1;                       /* 5ecb */
int focus_mouse_col = -1;                       /* 5ecd */
