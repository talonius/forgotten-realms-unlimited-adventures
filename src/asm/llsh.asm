;  CKIT.EXE (FRUA v1.2) resident frame 10d7: the long left-shift helper Turbo C's code
;  calls for 32-bit `<<`.  EXE 0x005c52..0x005c71, 31 bytes, 10d7:2ae2..10d7:2b01.
;
;  These bytes are byte-identical to the LLSH module of Turbo C 2.0's and
;  2.01's CL.LIB (wiki games/dqk/runtime-library).  This source is written from
;  the shipped bytes, not copied from Borland's.
;
;  LXLSH@: dx:ax <<= cl, far call, result in dx:ax; bx and cl are not preserved.
;  A count of 16 or more shifts the low word into the high word by cl - 16.
;
;  Ported from Dark Queen of Krynn's reviewed source (dqk-disassembly,
;  v11/src/asm/llsh.asm): the same bytes, 2Bh lower in the frame than DQK's
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

                public  LXLSH@

LXLSH@          proc    far                     ; 10d7:2ae2
                cmp     cl,16
                jae     word_or_more
                mov     bx,ax                   ; the low word's top bits ...
                shl     ax,cl
                shl     dx,cl
                neg     cl
                add     cl,16
                shr     bx,cl                   ; ... moved down by 16 - cl
                or      dx,bx                   ; ... into the high word
                ret
word_or_more:
                sub     cl,16
                mov     dx,ax
                xor     ax,ax
                shl     dx,cl
                ret
LXLSH@          endp

_TEXT           ends
                end
