;  CKIT.EXE (FRUA v1.2) resident frame 10d7: far memory blocks -- move,
;  fill, swap, and scans for a byte or a word.  EXE 0x0058b1..0x005965,
;  180 bytes, 10d7:2741..10d7:27f5.  dosio.asm ends at 0x58b1; farmem.asm
;  starts at 0x5965.
;
;  Ported from Dark Queen of Krynn's reviewed source (dqk-disassembly,
;  v11/src/asm/memops.asm).  One routine differs: FRUA's mem_move is 10
;  bytes longer than DQK's.  Where DQK's returns at once when the two
;  OFFSETS are equal, whatever the segments, FRUA's goes on to compare the
;  segments and returns only when both halves are equal, so a move between
;  equal offsets in different segments is done (forward).  The other four
;  routines are DQK's bytes, 2Bh lower in the frame.
;
;  Why these routines are one file is a choice, not a measurement: they
;  share no data, near call or jump with each other or with their
;  neighbours, and the frame has no relocation or pad byte at 2741 or 27f5.
;  They are grouped by role and by style: all five keep a bp frame
;  (arguments from [bp+6]), where the routines on either side use bx = sp.
;  A file boundary is measured somewhere in 2669..27f5 (see dosio.asm).
;
;  v1.0's image has the same 180 bytes at the same offset, so v10/ uses this
;  source unchanged.
;
;  Assembled by TASM 3.1, /ml, single pass: the forward jmp in mem_move
;  takes three bytes, EB xx 90, as it shipped.  No segment fixups, and the
;  MZ relocation table has no entry in this range.

                .8086

_TEXT           segment byte public 'CODE'
                assume  cs:_TEXT

                public  _mem_move, _mem_fill, _mem_exchange, _mem_scan, _mem_scan_w

;  void far mem_move(void far *src, void far *dst, unsigned n)  10d7:2741
;
;  Copies n bytes from src to dst, backward when src's OFFSET is below
;  dst's, so overlapping blocks in one segment move correctly; the
;  segments are not compared for that choice.  Does nothing when n is 0 or
;  src and dst are the same far pointer (offsets and segments both equal;
;  DQK's version stops on equal offsets alone).  Leaves the direction flag
;  clear.  si, di, ds and es are saved.
_mem_move       proc    far
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                push    es
                lds     si,dword ptr [bp+6]     ; ds:si = src
                les     di,dword ptr [bp+0Ah]   ; es:di = dst
                mov     cx,[bp+0Eh]
                or      cx,cx
                je      move_done
                cmp     si,di
                jne     move_offsets            ; offsets differ: move
                mov     ax,ds                   ; equal offsets: the same
                mov     bx,es                   ;   block only when the
                cmp     ax,bx                   ;   segments are equal too
                je      move_done
                cmp     si,di                   ; equal again: sets ZF, not CF
move_offsets:
                jb      move_down
                cld
                rep     movsb
                jmp     move_done
move_down:
                add     si,cx                   ; from the last byte of each
                dec     si
                add     di,cx
                dec     di
                std
                rep     movsb
                cld
move_done:
                pop     es
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret
_mem_move       endp

;  void far mem_fill(void far *p, unsigned n, int c)            10d7:277b
;
;  Stores c in n bytes at p (none when n is 0).  di is saved.
_mem_fill       proc    far
                push    bp
                mov     bp,sp
                push    di
                les     di,dword ptr [bp+6]
                mov     cx,[bp+0Ah]
                mov     al,[bp+0Ch]
                or      cx,cx
                je      fill_done
                cld
                rep     stosb
fill_done:
                pop     di
                pop     bp
                ret
_mem_fill       endp

;  void far mem_exchange(void far *a, void far *b, unsigned n)  10d7:2792
;
;  Exchanges n bytes between a and b.  n = 0 swaps 65536 bytes (loop).
;  Relies on the direction flag being clear.  Nothing in CKIT.EXE calls it.
;  Its 31 bytes are the same as gfxutil.asm's mem_swap (10d7:0e16), which
;  glyphfx.c calls; two names keep the link's publics distinct.
_mem_exchange   proc    far
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                push    es
                lds     si,dword ptr [bp+6]     ; ds:si = a
                les     di,dword ptr [bp+0Ah]   ; es:di = b
                mov     cx,[bp+0Eh]
swap_byte:
                mov     al,byte ptr es:[di]
                xchg    al,byte ptr [si]
                inc     si
                stosb
                loop    swap_byte
                pop     es
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret
_mem_exchange   endp

;  unsigned far mem_scan(void far *p, unsigned n, int c)        10d7:27b1
;
;  The index of the first c among n bytes at p, or n when there is none.
;  di and es are saved.
_mem_scan       proc    far
                push    bp
                mov     bp,sp
                push    di
                push    es
                les     di,dword ptr [bp+6]
                mov     cx,[bp+0Ah]
                mov     al,[bp+0Ch]
                cmp     cx,0
                je      scan_done
                cld
                repne   scasb
                jne     scan_done               ; not found: cx = 0
                inc     cx                      ; found: count it back
scan_done:
                mov     ax,[bp+0Ah]
                pop     es
                sub     ax,cx                   ; n - the bytes not scanned
                pop     di
                pop     bp
                ret
_mem_scan       endp

;  unsigned far mem_scan_w(int far *p, unsigned n, int w)       10d7:27d3
;
;  mem_scan for words: the index of the first w among n words at p, or n.
;  Nothing in CKIT.EXE calls it.
_mem_scan_w     proc    far
                push    bp
                mov     bp,sp
                push    di
                push    es
                les     di,dword ptr [bp+6]
                mov     cx,[bp+0Ah]
                mov     ax,[bp+0Ch]
                cmp     cx,0
                je      scanw_done
                cld
                repne   scasw
                jne     scanw_done
                inc     cx
scanw_done:
                mov     ax,[bp+0Ah]
                pop     es
                sub     ax,cx
                pop     di
                pop     bp
                ret
_mem_scan_w     endp

_TEXT           ends
                end
