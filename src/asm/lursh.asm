;  CKIT.EXE (FRUA v1.2) resident frame 10d7: the unsigned long right-shift helper
;  Turbo C calls for `>>` on an unsigned long; nothing in CKIT.EXE calls
;  it (nor in DQK).  EXE 0x005c8f..0x005cae, 31 bytes, 10d7:2b1f..10d7:2b3e.
;
;  These bytes are byte-identical to the LURSH module of Turbo C 2.0's and
;  2.01's CL.LIB (wiki games/dqk/runtime-library).  This source is written from
;  the shipped bytes, not copied from Borland's.
;
;  LXURSH@: dx:ax >>= cl, logical (zeros fill from the left), far call, result
;  in dx:ax; bx and cl are not preserved.  A count of 16 or more shifts the high
;  word into the low word by cl - 16 and clears dx.
;
;  Ported from Dark Queen of Krynn's reviewed source (dqk-disassembly,
;  v11/src/asm/lursh.asm): the same bytes, 2Bh lower in the frame than DQK's
;  (see sysint.asm).
;
;  v1.0's image has the same 31 bytes at the same offset, so v10/ uses this
;  source unchanged.
;
;  Assembled by TASM 3.1, /ml, single pass.  No segment fixups, and the MZ
;  relocation table has no entry in this range.

                .8086

_TEXT           segment byte public 'CODE'
                assume  cs:_TEXT

                public  LXURSH@

LXURSH@         proc    far                     ; 10d7:2b1f
                cmp     cl,16
                jae     word_or_more
                mov     bx,dx                   ; the high word's low bits ...
                shr     ax,cl
                shr     dx,cl
                neg     cl
                add     cl,16
                shl     bx,cl                   ; ... moved up by 16 - cl
                or      ax,bx                   ; ... into the low word
                ret
word_or_more:
                sub     cl,16
                mov     ax,dx
                xor     dx,dx
                shr     ax,cl
                ret
LXURSH@         endp

_TEXT           ends
                end
