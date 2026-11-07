;  CKIT.EXE (FRUA v1.2) resident frame 10d7: the signed long right-shift helper
;  Turbo C's code calls for `>>` on a long.  EXE 0x005c71..0x005c8f, 30
;  bytes, 10d7:2b01..10d7:2b1f.
;
;  These bytes are byte-identical to the LRSH module of Turbo C 2.0's and
;  2.01's CL.LIB (wiki games/dqk/runtime-library).  This source is written from
;  the shipped bytes, not copied from Borland's.
;
;  LXRSH@: dx:ax >>= cl, arithmetic (the sign fills from the left), far call,
;  result in dx:ax; bx and cl are not preserved.  A count of 16 or more shifts the
;  high word into the low word by cl - 16 and fills dx with the sign.
;
;  Ported from Dark Queen of Krynn's reviewed source (dqk-disassembly,
;  v11/src/asm/lrsh.asm): the same bytes, 2Bh lower in the frame than DQK's
;  (see sysint.asm).
;
;  v1.0's image has the same 30 bytes at the same offset, so v10/ uses this
;  source unchanged.
;
;  Assembled by TASM 3.1, /ml, single pass.  No segment fixups, and the MZ
;  relocation table has no entry in this range.

                .8086

_TEXT           segment byte public 'CODE'
                assume  cs:_TEXT

                public  LXRSH@

LXRSH@          proc    far                     ; 10d7:2b01
                cmp     cl,16
                jae     word_or_more
                mov     bx,dx                   ; the high word's low bits ...
                shr     ax,cl
                sar     dx,cl
                neg     cl
                add     cl,16
                shl     bx,cl                   ; ... moved up by 16 - cl
                or      ax,bx                   ; ... into the low word
                ret
word_or_more:
                sub     cl,16
                mov     ax,dx
                cwd                             ; dx = the sign
                sar     ax,cl
                ret
LXRSH@          endp

_TEXT           ends
                end
