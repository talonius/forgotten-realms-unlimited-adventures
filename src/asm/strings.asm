;  CKIT.EXE (FRUA v1.2) resident frame 10d7: the string and integer
;  helpers -- str_cat; str_cmp, the case-blind string compare, and three
;  wrappers on it; string copy, length and character search; case
;  conversion of a character and of a string in place; and abs, max and
;  min.  EXE 0x005626..0x0057d9, 435 bytes, 10d7:24b6..10d7:2669.
;  conio.asm ends at 0x5626; dosio.asm starts at 0x57d9.
;
;  Ported from Dark Queen of Krynn's reviewed sources (dqk-disassembly,
;  v11/src/asm/strcmp.asm and strings.asm), which FRUA's bytes put in ONE
;  file.  str_cmp has a second entry, str_cmp_bx (10d7:24dd, past its
;  `mov bx,sp`), which str_icmp, str_same and str_after reach.  In DQK
;  those are relocated far transfers (a far jmp and two far calls, three
;  MZ relocations), so DQK splits the two files there.  In FRUA they are
;  a short jmp, EB 9E at 10d7:253d, and `push cs; call near`, 0E E8, at
;  2541 and 2552, with no relocation in the range: what TASM 3.1's default
;  SMART mode makes of a far jmp or call to a far label in its own segment
;  (measured by the DQK port, see conio.asm).  So here they are one source.
;  The jmp is 3 bytes shorter than DQK's and each call one byte shorter, so
;  str_same sits 4 bytes lower in the frame than DQK's and everything from
;  str_copy on 6 bytes lower (one more for con_puts's call, conio.asm).
;  Every other byte is DQK's.
;
;  Where the file starts, 24b6, is a choice: str_cat keeps a bp frame, a
;  different house style from con_puts before it, and is a string routine,
;  not console output.  Where it ends, 2669, is a choice bounded by a
;  measurement: dosio.asm's mem_alloc (2669) far-calls farmem.asm's
;  far_alloc (27f5), so a file boundary lies somewhere in 2669..27f5; this
;  file stops where the string and integer helpers stop.  Inside it,
;  str_after shares str_same's `return 0` (10d7:2558 jumps back to 254d)
;  and str_upr shares str_lwr's exit (2606 to 25f0).
;
;  Register style: str_cat keeps a bp frame (arguments from [bp+6]) and
;  saves si, di, ds and es; every other routine has no bp frame and reads
;  its arguments through bx = sp, from ss:[bx+4]; result in ax or dx:ax.
;  si, di, ds and es are saved where used; bx and dx are not.  The string
;  routines rely on the direction flag being clear (str_cmp clears it).
;
;  v1.0's image has the same 435 bytes at the same offset, so v10/ uses this
;  source unchanged.
;
;  Assembled by TASM 3.1, /ml, single pass.  No segment fixups, and the MZ
;  relocation table has no entry in this range.

                .8086

CASE_BIT        equ     20h                     ; 'A' + 20h = 'a'

_TEXT           segment byte public 'CODE'
                assume  cs:_TEXT

                public  _str_cat, _str_cmp, _str_cmp_bx
                public  _str_icmp, _str_same, _str_after
                public  _str_copy, _str_len, _str_find
                public  _to_lower, _to_upper, _str_lwr, _str_upr
                public  _int_abs, _int_max, _int_min, _uint_max, _uint_min

;  void far str_cat(char far *dst, char far *src)               10d7:24b6
;
;  Appends src, with its NUL, at dst's NUL.  C far call, arguments from
;  [bp+6]; si, di, ds and es are saved.  Relies on the direction flag being
;  clear.
_str_cat        proc    far
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                push    es
                lds     si,dword ptr [bp+6]     ; ds:si = dst
find_end:
                lodsb
                or      al,al
                jne     find_end
                dec     si                      ; back onto the NUL
                mov     di,si
                mov     ax,ds
                mov     es,ax                   ; es:di = dst's NUL
                lds     si,dword ptr [bp+0Ah]   ; ds:si = src
append:
                lodsb
                stosb
                or      al,al
                jne     append
                pop     es
                pop     ds
                pop     di
                pop     si
                pop     bp
                ret
_str_cat        endp

;  int far str_cmp(char far *a, char far *b)                    10d7:24db
;
;  Compares a and b as unsigned bytes with the case of 'A'..'Z' ignored;
;  returns -1, 0 or 1.  When the two characters differ and the larger is
;  above 'Z' while the smaller is a capital, the capital is lowered and the
;  pair compared again; any other difference decides.  That orders the
;  strings as if every capital were lowered first: "ABC" equals "abc", and
;  '[' .. '`' (5Bh..60h) sort below every letter of either case.
;
;  Arguments through bx = sp, at ss:[bx+4] and ss:[bx+8].  ds, es, si and di
;  are saved; bx is not.  Clears the direction flag.  The second entry,
;  str_cmp_bx (10d7:24dd), takes bx already pointing at a caller's frame:
;  str_icmp, str_same and str_after, below, set bx = sp and jump to it or
;  call it, so the arguments read are their callers'.
_str_cmp        proc    far
                mov     bx,sp
_str_cmp_bx     label   far                     ; 10d7:24dd, bx = the frame
                push    ds
                push    es
                push    si
                push    di
                lds     si,dword ptr ss:[bx+4]  ; ds:si = a
                les     di,dword ptr ss:[bx+8]  ; es:di = b
                xor     ax,ax                   ; ah stays 0
                cld
next_pair:
                lodsb                           ; al = *a++
                mov     bl,byte ptr es:[di]     ; bl = *b++
                inc     di
                cmp     al,bl
                ja      a_above
                jb      a_below
                or      al,al
                jne     next_pair
                pop     di                      ; both ended: ax = 0
                pop     si
                pop     es
                pop     ds
                ret

a_above:                                        ; al > bl
                cmp     bl,'Z'
                ja      return_1
                cmp     al,'Z'
                jbe     return_1
                cmp     bl,'A'
                jb      return_1
                add     bl,CASE_BIT             ; b's capital, lowered
                cmp     al,bl
                je      next_pair
                jb      return_minus_1
return_1:
                mov     al,1
                pop     di
                pop     si
                pop     es
                pop     ds
                ret

a_below:                                        ; al < bl
                cmp     al,'Z'
                ja      return_minus_1
                cmp     bl,'Z'
                jbe     return_minus_1
                cmp     al,'A'
                jb      return_minus_1
                add     al,CASE_BIT             ; a's capital, lowered
                cmp     al,bl
                je      next_pair
                ja      return_1
return_minus_1:
                mov     ax,-1
                pop     di
                pop     si
                pop     es
                pop     ds
                ret
_str_cmp        endp

;  int far str_icmp(char far *a, char far *b)                   10d7:253b
;
;  str_cmp under a second name: sets bx = sp and jumps to str_cmp's body,
;  which returns to this routine's caller.  Nothing in CKIT.EXE calls it.
_str_icmp       proc    far
                mov     bx,sp
                jmp     _str_cmp_bx             ; a far label here: short jmp
_str_icmp       endp

;  int far str_same(char far *a, char far *b)                   10d7:253f
;
;  1 when str_cmp finds a and b equal (case ignored), else 0.  bx = sp
;  before the call, so str_cmp_bx reads this routine's arguments.
_str_same       proc    far
                mov     bx,sp
                call    _str_cmp_bx             ; a far label here: push cs; call near
                or      ax,ax
                jne     return_0
                mov     ax,1
                ret
return_0:
                xor     ax,ax
                ret
_str_same       endp

;  int far str_after(char far *a, char far *b)                  10d7:2550
;
;  1 when str_cmp orders a after b, else 0.  Nothing in CKIT.EXE calls it.
_str_after      proc    far
                mov     bx,sp
                call    _str_cmp_bx             ; a far label here: push cs; call near
                or      ax,ax
                jle     return_0
                mov     ax,1
                ret
_str_after      endp

;  void far str_copy(char far *dst, char far *src)              10d7:255e
;
;  Copies src, with its NUL, to dst.  Note the order: dst first.
_str_copy       proc    far
                mov     bx,sp
                push    si
                push    di
                push    ds
                push    es
                les     di,dword ptr ss:[bx+4]  ; es:di = dst
                lds     si,dword ptr ss:[bx+8]  ; ds:si = src
copy_char:
                lodsb
                stosb
                or      al,al
                jne     copy_char
                pop     es
                pop     ds
                pop     di
                pop     si
                ret
_str_copy       endp

;  int far str_len(char far *s)                                 10d7:2577
;
;  The number of characters before the NUL.
_str_len        proc    far
                mov     bx,sp
                push    si
                push    ds
                lds     si,dword ptr ss:[bx+4]
                mov     bx,si
find_nul:
                lodsb
                or      al,al
                jne     find_nul
                mov     ax,si                   ; one past the NUL
                sub     ax,bx
                dec     ax
                pop     ds
                pop     si
                ret
_str_len        endp

;  char far *far str_find(char far *s, char c)                  10d7:258e
;
;  A pointer to the first c in s; when s has none, a pointer to its NUL
;  (never a null pointer).  str_find(s, 0) finds the NUL.
_str_find       proc    far
                mov     bx,sp
                push    si
                push    ds
                lds     si,dword ptr ss:[bx+4]
                mov     dx,word ptr ss:[bx+8]   ; dl = c
next_char:
                lodsb
                cmp     al,dl
                je      found
                or      al,al
                jne     next_char
found:
                mov     ax,si
                dec     ax                      ; back onto the character
                mov     dx,ds
                pop     ds
                pop     si
                ret
_str_find       endp

;  int far to_lower(int c)                                      10d7:25ab
;
;  c's low byte lowered when it is 'A'..'Z' (signed byte compares, so
;  80h..FFh are left alone); ah is c's high byte, unchanged.
_to_lower       proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                cmp     al,'A'
                jl      lower_done
                cmp     al,'Z'
                jg      lower_done
                add     al,CASE_BIT
lower_done:
                ret
_to_lower       endp

;  int far to_upper(int c)                                      10d7:25bc
;
;  c's low byte raised when it is 'a'..'z'.
_to_upper       proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                cmp     al,'a'
                jl      upper_done
                cmp     al,'z'
                jg      upper_done
                sub     al,CASE_BIT
upper_done:
                ret
_to_upper       endp

;  void far str_lwr(char far *s)                                10d7:25cd
;
;  Lowers the capitals of s in place; only the bytes that change are
;  written.  Nothing in CKIT.EXE calls it.
_str_lwr        proc    far
                mov     bx,sp
                push    si
                push    di
                push    ds
                push    es
                lds     si,dword ptr ss:[bx+4]
                mov     ax,ds
                mov     es,ax
lwr_next:
                lodsb
                or      al,al
                je      case_done
                cmp     al,'A'
                jl      lwr_next
                cmp     al,'Z'
                jg      lwr_next
                add     al,CASE_BIT
                mov     di,si
                dec     di                      ; the byte just read
                stosb
                jmp     lwr_next
case_done:
                pop     es
                pop     ds
                pop     di
                pop     si
                ret
_str_lwr        endp

;  void far str_upr(char far *s)                                10d7:25f5
;
;  Raises the small letters of s in place.  Leaves through str_lwr's exit.
_str_upr        proc    far
                mov     bx,sp
                push    si
                push    di
                push    ds
                push    es
                lds     si,dword ptr ss:[bx+4]
                mov     ax,ds
                mov     es,ax
upr_next:
                lodsb
                or      al,al
                je      case_done
                cmp     al,'a'
                jl      upr_next
                cmp     al,'z'
                jg      upr_next
                sub     al,CASE_BIT
                mov     di,si
                dec     di
                stosb
                jmp     upr_next
_str_upr        endp

;  int far int_abs(int n)                                       10d7:2618
;
;  Nothing in CKIT.EXE calls it.
_int_abs        proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                or      ax,ax
                jge     abs_done
                neg     ax
abs_done:
                ret
_int_abs        endp

;  int far int_max(int a, int b)                                10d7:2625
_int_max        proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                mov     bx,word ptr ss:[bx+6]
                cmp     ax,bx
                jge     max_done
                mov     ax,bx
max_done:
                ret
_int_max        endp

;  int far int_min(int a, int b)                                10d7:2636
_int_min        proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                mov     bx,word ptr ss:[bx+6]
                cmp     ax,bx
                jle     min_done
                mov     ax,bx
min_done:
                ret
_int_min        endp

;  unsigned far uint_max(unsigned a, unsigned b)                10d7:2647
;
;  Nothing in CKIT.EXE calls it.
_uint_max       proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                mov     bx,word ptr ss:[bx+6]
                cmp     ax,bx
                jae     umax_done
                mov     ax,bx
umax_done:
                ret
_uint_max       endp

;  unsigned far uint_min(unsigned a, unsigned b)                10d7:2658
;
;  Nothing in CKIT.EXE calls it.
_uint_min       proc    far
                mov     bx,sp
                mov     ax,word ptr ss:[bx+4]
                mov     bx,word ptr ss:[bx+6]
                cmp     ax,bx
                jbe     umin_done
                mov     ax,bx
umin_done:
                ret
_uint_min       endp

_TEXT           ends
                end
