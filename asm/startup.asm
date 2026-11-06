; CKIT.EXE resident frame 1000, 1000:0000..1000:00c2 (EXE 0x002400..0x0024c3, 195 bytes):
; the program's start-up.  The MZ entry point is RTLink/Plus's (src/rtlink/entry.asm),
; which loads the start-up sections, DGROUP's data among them, and jumps here.  This is
; not Turbo C's C0L: there is no exit chain, no floating-point hook and no null check.
; It sets up DGROUP and the stack, builds one command-line string and hands it to
; start_main (cmdargs.c), which splits it into argc/argv; then it exits with code 0.
;
;   1. DS = SS = DGROUP, _psp saved.  With 64K or more above DGROUP, SP = 0fffeh and
;      DOS shrinks the program's block to end there; otherwise SP is the top of memory.
;   2. The command tail (PSP:80h) is copied onto the stack, NUL-terminated.
;   3. Below it goes the program's path and a space: the path follows the environment's
;      double NUL under DOS 3 or later; DOS 2 has none, so "C " is pushed in its place;
;      DOS 1 exits at once.
;   4. _BSS is zeroed by words, its size halved with shr: an odd size leaves the last
;      byte unzeroed.  Then start_main(char far *) is far-called with the string.
;
; Assembled with TASM 3.1 in one pass (no /m), as pixels.asm is: the two forward
; `jmp`s then reserve three bytes and pad their short form with a nop, which is the
; shipped `eb 14 90` (1000:001e) and `eb 31 90` (1000:006b).  TASM's reg,reg encodings
; (2b d8 for sub bx,ax, 8b e3 for mov sp,bx, ...) are the shipped ones.  The byte after
; this module, 100c:0003, is AIL's word-alignment pad.

                LOCALS                          ; @@ labels are local

BSS_START       equ     0A7E4h                  ; _BSS, DGROUP a7e4 (cur_event) ...
BSS_BYTES       equ     3F67h                   ; ... e74a; by words, so e74a is left

_DATA           segment word public 'DATA'
                extrn   __psp:word              ; DGROUP 0020, the PSP's segment
                extrn   _stack_top:word         ; DGROUP 0026, SP once the stack is set
_DATA           ends
DGROUP          group   _DATA

                extrn   _start_main:far         ; cmdargs.c, 2793:000b

STARTUP_TEXT    segment byte public 'CODE'
                assume  cs:STARTUP_TEXT, ds:DGROUP

start:          mov     ax,DGROUP
                mov     ds,ax
                mov     __psp,es
                mov     bx,es:[2]               ; PSP: top of memory, a paragraph
                sub     bx,ax                   ; paragraphs above DGROUP
                test    bx,0F000h
                jnz     @@big
                mov     cl,4
                shl     bx,cl                   ; under 64K: they are the stack
                mov     ss,ax
                mov     sp,bx
                jmp     @@stack
@@big:          mov     ss,ax                   ; 64K or more: stop at 64K
                mov     sp,0FFFEh
                mov     bx,ds
                sub     bx,__psp
                add     bx,1000h                ; PSP .. DGROUP + 64K
                mov     ah,4Ah                  ; resize memory block
                int     21h
@@stack:        mov     _stack_top,sp

                mov     si,80h                  ; the command tail
                mov     cl,es:[si]
                inc     si
                xor     ch,ch
                mov     bx,cx
                shr     bx,1                    ; its length, plus its NUL, made even
                inc     bx
                shl     bx,1
                sub     sp,bx
                mov     di,sp
                jcxz    @@tail_end
@@tail:         lods    byte ptr es:[si]
                mov     [di],al
                inc     di
                loop    @@tail
@@tail_end:     xor     al,al
                mov     [di],al

                mov     ah,30h                  ; DOS version
                int     21h
                cmp     al,3
                jnc     @@path
                or      al,al
                jnz     @@dos2
                int     20h                     ; DOS 1: give up
@@dos2:         mov     ax,2043h                ; "C ": no path to be had
                push    ax
                jmp     @@args

@@path:         mov     es,es:[2Ch]             ; the environment
                xor     si,si
@@env:          lods    byte ptr es:[si]        ; skip each string ...
                or      al,al
                jnz     @@env
                lods    byte ptr es:[si]        ; ... up to the double NUL
                or      al,al
                jnz     @@env
                inc     si                      ; the string count word
                inc     si
                mov     cx,si
@@path_end:     lods    byte ptr es:[si]
                or      al,al
                jnz     @@path_end
                xchg    cx,si
                sub     cx,si                   ; the path's length, with its NUL
                sub     sp,cx
                dec     cx
                mov     di,sp
@@path_copy:    lods    byte ptr es:[si]
                mov     [di],al
                inc     di
                loop    @@path_copy
                mov     byte ptr [di],' '       ; its NUL becomes the space

@@args:         mov     ax,sp                   ; the string, path first
                test    al,1
                jz      @@even
                dec     sp
@@even:         push    ds                      ; start_main's char far *
                push    ax
                mov     ax,ds
                mov     es,ax
                mov     cx,BSS_BYTES
                shr     cx,1
                jz      @@cleared
                mov     di,BSS_START
                xor     ax,ax
                rep     stosw
@@cleared:      call    _start_main
                mov     ax,4C00h                ; exit 0, whatever start_main did
                int     21h

STARTUP_TEXT    ends
                end
