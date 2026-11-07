;  CKIT.EXE (FRUA v1.2) resident frame 2ae1, its first source file:
;  RTLink/Plus run-time code beside the overlay manager (frame 28de) -- the
;  hex formatter the manager's error messages use, the fatal-error exit and
;  the vector it is reached through, a table of exit calls and the hook on
;  the PSP's terminate address that runs them, a routine that runs the
;  table's mask-4 calls, and three two-instruction stubs.
;  EXE 0x01d219..0x01d49b, 642 bytes, 2ae1:0009..2ae1:028b.  rtlfind.asm
;  starts at 0x1d49b.
;
;  Ported from Dark Queen of Krynn's reviewed source (dqk-disassembly,
;  v11/src/asm/rtlerr.asm, DQK frame 2eef:0007..027f).  FRUA's code is
;  DQK's, 2 bytes higher in its frame up to ierr_list and 0Ch higher after
;  it, with two differences, both written below as instructions:
;
;  - rtl_on_exit hooks the PSP's terminate address for a registration whose
;    mask has bit 1 set (`test al,2`), where DQK's hooks it for any mask
;    other than 1 (`cmp al,1`).  Same length.
;  - rtl_run_init_calls (2ae1:01b7, 10 bytes) follows ierr_list: it runs
;    the exit-table routines registered with mask bit 2, as rtl_exit_cleanup
;    and term_handler run bits 0 and 1.  DQK has no such routine, so
;    term_handler, run_exit_calls, the stubs and the messages sit 0Ah
;    further on than the 2-byte shift alone puts them.
;
;  Everything else is DQK's bytes; the offsets the code stores are
;  frame-relative and placed by the assembler.  The stubs are named by
;  their FRUA offsets (DQK: rtl_stub_0202, 0206, 020a).
;
;  What precedes it: 2ae1:0000..0008 (0x1d210..0x1d218) is the tail of the
;  last of the manager's 409 vector stubs.  The file opens with
;  rtl_cache_none at 2ae1:0009, `stc; retf`: the manager's three far calls
;  to it (from rtlink_evict_section, rtlink_load_section and
;  rtlink_init_workspace) are stored 1ae1:0009, the frame of this segment,
;  so it is this segment's first routine, not manager code; that it is this
;  file's rather than a two-byte file of its own is a choice.
;
;  Hand assembly with a register interface: arguments and results in
;  registers, the others kept by the routines that return (rtl_fatal and
;  rtl_internal_error do not).  The manager reaches rtl_hex_word
;  (from rtlink_load_section and rtlink_check_vectors), rtl_save_term and
;  rtl_on_exit (rtlink_init_workspace), rtl_run_init_calls
;  (rtlink_init_sections) and rtl_stub_020e (rtlink_close_file) by far
;  calls stored 1ae1:OFF, and fatal_vector and exe_ovl_offset through far
;  pointers it holds at manager offsets aef and c7d.  rtl_set_fatal,
;  rtl_exit_cleanup, rtl_stub_0212 and rtl_stub_0216 are called by nothing
;  in CKIT.EXE.
;
;  Why these routines are one file, and why the file ends at 028b, is a
;  choice the bytes support but do not force: the message table at 021b,
;  which rtl_fatal and rtl_internal_error read, follows the stubs, so the
;  code, the stubs and the messages go together; nothing in 0009..028a
;  refers to 028b..05f0, nor the reverse, and each half ends with its own
;  table of message offsets and the messages.  The hex formatter at 000b
;  shares no data and no call with the rest and is kept with it by role:
;  the manager uses it to write numbers into its error messages.
;
;  Assembled by TASM 3.1, /ml, single pass.  One segment fixup: fatal_vector's
;  segment word at 2ae1:0052 (0x1d262), the range's one MZ relocation (stored
;  1ae1).  Every other address is an offset in this segment.

                .8086

DOS             equ     21h
VIDEO           equ     10h
DOS_PUTCHAR     equ     02h                     ; dl
DOS_PUTS        equ     09h                     ; ds:dx, ended by '$'
DOS_GETCH       equ     07h                     ; no echo
DOS_FORCEDUP    equ     46h                     ; handle cx becomes a copy of handle bx
DOS_EXIT_1      equ     4C01h                   ; terminate with code 1
VID_GETMODE     equ     0Fh                     ; al = the video mode
STDOUT          equ     1
STDERR          equ     2
PSP_TERM_IP     equ     0Ah                     ; PSP: the terminate address (int 22h)
PSP_TERM_CS     equ     0Ch
EXIT_SLOTS      equ     8                       ; exit_table: far routine, mask byte,
EXIT_ENTRY      equ     6                       ;   a pad byte
ERR_INTERNAL    equ     6                       ; ah for rtl_internal_error

RTL_TEXT        segment byte public 'CODE'
                assume  cs:RTL_TEXT

                public  rtl_cache_none
                public  rtl_hex_word, rtl_set_fatal, rtl_fatal, rtl_exit_cleanup
                public  rtl_save_term, rtl_on_exit, rtl_internal_error
                public  rtl_run_init_calls
                public  rtl_stub_020e, rtl_stub_0212, rtl_stub_0216
                public  exe_ovl_offset, fatal_vector

;  rtl_cache_none                                           2ae1:0009
;
;  Returns carry set and nothing else.  The manager far-calls it while
;  evicting, loading and initialising sections; goldbox.toml's manager_names
;  called it rtlink_unavailable_cache_hook, reading it as the answer of an
;  overlay cache that is not linked in.  That role is a reading of the
;  callers, not shown by these two bytes.
rtl_cache_none  proc    far
                stc
                ret
rtl_cache_none  endp

;  rtl_hex_word                                                  2ae1:000b
;
;  cx as four upper-case hex digits at ds:dx.  ax, di and es are kept.  The
;  manager writes section numbers and vector addresses into its error
;  messages with it.
rtl_hex_word    proc    far
                push    ax
                push    di
                push    es
                mov     ax,ds
                mov     es,ax
                mov     ax,cx
                mov     di,dx
                call    hex_ax
                pop     es
                pop     di
                pop     ax
                ret
rtl_hex_word    endp

;  hex_ax, hex_al, hex_digit                          2ae1:001d, 0024, 0031
;
;  ax, al, or al's low nibble as hex digits stored at es:di++.  Each of the
;  first two does its high half by a call and falls into the next for the
;  low half.  The digit: 0..9 + 90h, daa, adc 40h, daa gives '0'..'9' and
;  'A'..'F'.
hex_ax          proc    near
                push    ax
                mov     al,ah
                call    hex_al
                pop     ax
hex_ax          endp                            ; falls into hex_al

hex_al          proc    near
                push    ax
                shr     al,1
                shr     al,1
                shr     al,1
                shr     al,1
                call    hex_digit
                pop     ax
hex_al          endp                            ; falls into hex_digit

hex_digit       proc    near
                and     al,0Fh
                add     al,90h
                daa
                adc     al,40h
                daa
                stosb
                ret
hex_digit       endp

;  exe_ovl_offset                                                2ae1:003b
;
;  A long the manager adds to its seek position when it reads a section from
;  a file it found as an .exe (rtlink_load_section, through its far pointer
;  at manager offset c7d).  Zero here: the sections' positions in CKIT.EXE are
;  absolute.
exe_ovl_offset  dd      0

;  rtl_set_fatal                                                 2ae1:003f
;
;  Installs ds:si as the fatal-error routine and returns the one it replaces
;  in ds:si.  ax is kept.
rtl_set_fatal   proc    far
                push    ax
                xchg    cs:fatal_off,si
                mov     ax,ds
                xchg    cs:fatal_seg,ax
                mov     ds,ax
                pop     ax
                ret
rtl_set_fatal   endp

;  fatal_vector, the routine the manager's rtlink_error and rtl_internal_error
;  jump to (rtl_fatal unless rtl_set_fatal replaced it); saved_term, the
;  terminate address rtl_save_term found in the PSP; psp_seg, that PSP.
fatal_vector    label   dword                   ; 2ae1:0050
fatal_off       dw      offset rtl_fatal
fatal_seg       dw      seg rtl_fatal           ; 2ae1:0052, the MZ relocation
saved_term      label   dword                   ; 2ae1:0054
saved_term_off  dw      0
saved_term_seg  dw      0
psp_seg         dw      0                       ; 2ae1:0058

;  rtl_fatal                                                     2ae1:005a
;
;  The fatal-error exit, entered by a far jump with ds:bx the address of a
;  list of NUL-ended strings, ended by a zero word (its callers also set ax
;  and cx, which this routine does not read).  It
;  makes standard output a copy of standard error, prints "Fatal Error "
;  and the strings, and, when the screen is in a graphics mode (not 0..3
;  and not 7), "Press any key to terminate." and waits for a key.  Exits to
;  DOS with code 1.
rtl_fatal       proc    far
                push    bx
                mov     bx,STDERR
                mov     cx,STDOUT
                mov     ah,DOS_FORCEDUP
                int     DOS
                pop     bx
                push    ds
                mov     ax,cs
                mov     ds,ax
                assume  ds:RTL_TEXT
                mov     dx,msg_fatal
                mov     ah,DOS_PUTS
                int     DOS
                pop     ds
                assume  ds:nothing
next_string:
                mov     si,[bx]
                or      si,si
                je      strings_done
put_char:
                lodsb
                or      al,al
                je      string_done
                mov     dl,al
                mov     ah,DOS_PUTCHAR
                int     DOS
                jmp     put_char
string_done:
                add     bx,2
                jmp     next_string
strings_done:
                mov     ax,cs
                mov     ds,ax
                assume  ds:RTL_TEXT
                mov     ah,VID_GETMODE
                int     VIDEO
                cmp     al,3
                jbe     terminate               ; a colour text mode
                cmp     al,7
                je      terminate               ; monochrome text
                mov     dx,msg_press
                mov     ah,DOS_PUTS
                int     DOS
                mov     ah,DOS_GETCH
                int     DOS
                assume  ds:nothing
terminate:
                mov     ax,DOS_EXIT_1
                int     DOS
rtl_fatal       endp

;  rtl_exit_cleanup                                              2ae1:00ad
;
;  Calls the exit routines registered with mask bit 0, then puts the PSP's
;  terminate address back and clears term_hooked.  ax and es are kept.
rtl_exit_cleanup proc   far
                push    ax
                push    es
                mov     al,1
                call    run_exit_calls
                mov     es,cs:psp_seg
                cli
                mov     ax,cs:saved_term_off
                mov     es:[PSP_TERM_IP],ax
                mov     ax,cs:saved_term_seg
                mov     es:[PSP_TERM_CS],ax
                mov     cs:term_hooked,0
                sti
                pop     es
                pop     ax
                ret
rtl_exit_cleanup endp

;  rtl_save_term                                                 2ae1:00d5
;
;  es = the PSP.  Records es in psp_seg and, the first time only (while
;  saved_term is zero), the PSP's terminate address in saved_term.  ax is
;  kept.  rtlink_init_workspace calls it first, with the PSP DOS passed.
rtl_save_term   proc    far
                push    ax
                mov     cs:psp_seg,es
                mov     ax,cs:saved_term_off
                or      ax,cs:saved_term_seg
                jne     term_saved
                cli
                mov     ax,es:[PSP_TERM_IP]
                mov     cs:saved_term_off,ax
                mov     ax,es:[PSP_TERM_CS]
                mov     cs:saved_term_seg,ax
                sti
term_saved:
                pop     ax
                ret
rtl_save_term   endp

;  rtl_on_exit                                                   2ae1:00fa
;
;  Registers the far routine ds:dx in the first free slot of exit_table with
;  the mask al: bit 0 has rtl_exit_cleanup call it, bit 1 term_handler,
;  bit 2 rtl_run_init_calls.  The
;  first registration with bit 1 set in al points the PSP's terminate
;  address at term_handler.  With all eight slots taken it ends in
;  rtl_internal_error.  ax and bx are kept.  rtlink_init_workspace registers
;  the manager's routine at 0a64, which closes the overlay file through
;  rtlink_close_file and sets the manager byte at 0c81, with al = 1, so in
;  CKIT.EXE the hook is never installed.  (DQK's hooks for any al but 1.)
rtl_on_exit     proc    far
                push    bx
                push    ax
                mov     bx,offset exit_table
find_free:
                mov     ax,cs:[bx]
                or      ax,cs:[bx+2]
                je      slot_found
                add     bx,EXIT_ENTRY
                jmp     find_free

term_hooked     dw      0                       ; 2ae1:010d, 0FFh while term_handler is hooked

slot_found:
                test    cs:term_hooked,0FFh
                jne     store_entry
                pop     ax
                push    ax
                test    al,2                    ; DQK: cmp al,1
                je      store_entry
                mov     cs:term_hooked,0FFh
                push    es
                mov     es,cs:psp_seg
                cli
                mov     word ptr es:[PSP_TERM_IP],offset term_handler
                mov     es:[PSP_TERM_CS],cs
                sti
                pop     es
store_entry:
                cmp     bx,offset exit_table_end
                je      table_full
                mov     cs:[bx],dx
                mov     cs:[bx+2],ds
                pop     ax
                mov     cs:[bx+4],al
                pop     bx
                ret
table_full:
                push    cs                      ; a far call within this segment;
                call    near ptr rtl_internal_error ; it does not return
rtl_on_exit     endp

;  exit_table: eight entries of a far routine, its mask byte and a pad byte;
;  a zero routine is a free slot.  The zero dword after them ends the search.
exit_table      label   byte                    ; 2ae1:0152
                rept    EXIT_SLOTS              ; (not `dup`: the build splices no
                dd      0                       ;  iterated data)
                db      0,0
                endm
exit_table_end  dd      0                               ; 2ae1:0182

;  rtl_internal_error                                            2ae1:0186
;
;  "Fatal Error ege0001: Internal error in .RTLink(R)/Plus run-time code."
;  Builds the string list in ierr_list and jumps through fatal_vector with
;  ds:bx at it, ah = 6, al = 1, cx = 0.  Does not return.
rtl_internal_error proc far
                mov     ax,cs
                mov     ds,ax
                assume  ds:RTL_TEXT
                mov     bx,msg_ierr_code
                mov     cs:ierr_list,bx
                mov     bx,msg_ierr_text
                mov     cs:ierr_list+2,bx
                mov     cs:ierr_list+4,0
                assume  ds:nothing
                mov     bx,offset ierr_list
                mov     ah,ERR_INTERNAL
                mov     al,1
                xor     cx,cx
                jmp     cs:fatal_vector
rtl_internal_error endp

ierr_list       dw      0,0,0                   ; 2ae1:01b1

;  rtl_run_init_calls                                            2ae1:01b7
;
;  Calls the exit routines registered with mask bit 2.  ax and es are kept.
;  rtlink_init_sections far-calls it once, after it has opened the overlay
;  file; nothing in CKIT.EXE registers a routine with that bit, so the call
;  runs nothing here.  Not in DQK's run-time.
rtl_run_init_calls proc far
                push    ax
                push    es
                mov     al,4
                call    run_exit_calls
                pop     es
                pop     ax
                ret
rtl_run_init_calls endp

;  term_handler                                                  2ae1:01c1
;
;  Where the PSP's terminate address points once rtl_on_exit has hooked it:
;  DOS jumps here as the program ends.  Calls the exit routines registered
;  with mask bit 1, puts the terminate address back, and jumps to it.
term_handler    proc    far
                push    ax
                push    es
                mov     al,2
                call    run_exit_calls
                mov     es,cs:psp_seg
                cli
                mov     ax,cs:saved_term_off
                mov     es:[PSP_TERM_IP],ax
                mov     ax,cs:saved_term_seg
                mov     es:[PSP_TERM_CS],ax
                mov     cs:term_hooked,0
                sti
                pop     es
                pop     ax
                jmp     cs:saved_term
term_handler    endp

;  run_exit_calls                                                2ae1:01ed
;
;  Far-calls, in table order, every exit_table routine whose mask shares a
;  bit with al, each with the flags saved around it.  bx and dx are kept.
run_exit_calls  proc    near
                push    bx
                push    dx
                mov     bx,offset exit_table
next_entry:
                mov     dx,cs:[bx]
                or      dx,cs:[bx+2]
                je      entries_done
                test    cs:[bx+4],al
                je      skip_entry
                pushf
                call    dword ptr cs:[bx]
                popf
skip_entry:
                add     bx,EXIT_ENTRY
                jmp     next_entry
entries_done:
                pop     dx
                pop     bx
                ret
run_exit_calls  endp

;  rtl_stub_020e, rtl_stub_0212, rtl_stub_0216              2ae1:020e, 0212, 0216
;
;  ax = -1; cx = -1; dx:ax = 0.  rtlink_close_file calls the first before it
;  closes the overlay file and discards its ax; nothing calls the other two.
;  What component they stand in for is not established.
rtl_stub_020e   proc    far
                mov     ax,0FFFFh
                ret
rtl_stub_020e   endp

rtl_stub_0212   proc    far
                mov     cx,0FFFFh
                ret
rtl_stub_0212   endp

rtl_stub_0216   proc    far
                xor     ax,ax
                mov     dx,ax
                ret
rtl_stub_0216   endp

;  The messages, through a table of their offsets (2ae1:021b): rtl_fatal's
;  two are printed by DOS function 9 and end in '$'; rtl_internal_error's
;  two are strings for rtl_fatal's list and end in NUL.
msg_fatal       dw      offset s_fatal
msg_press       dw      offset s_press
msg_ierr_code   dw      offset s_ierr_code
msg_ierr_text   dw      offset s_ierr_text
s_fatal         db      'Fatal Error $'
s_press         db      0Dh,0Ah,'Press any key to terminate.$'
s_ierr_code     db      'ege0001: ',0
s_ierr_text     db      'Internal error in .RTLink(R)/Plus run-time code.',0Dh,0Ah,0

RTL_TEXT        ends
                end
