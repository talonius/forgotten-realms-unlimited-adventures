; CKIT.EXE resident frame 10d7, 10d7:2bd3..10d7:2d6a (EXE 0x005d43..0x005eda, 407 bytes):
; four FRUA-only hand-assembly routines that sit after the Turbo C runtime in the
; frame and before textbank.c (13ad:000a).  None is in CL.LIB, and no C compiler
; writes them: they shuffle bits through the carry flag with rcl, keep scratch in the
; saved-BP slot, and reuse an argument slot once it has been loaded.  v1.0's image has
; the same 407 bytes at the same offset, so v10/ uses this source unchanged.
;
; planes_to_pixels  (10d7:2bd3)  eight bitplanes -> one byte a pixel  (ilbm.c)
; pixels_to_planes  (10d7:2c49)  the inverse                          (ilbm.c)
; pixels2_to_planes (10d7:2cc7)  two bits a pixel -> two bitplanes    (no far call)
; pcx_unpack        (10d7:2d35)  PCX run-length decoder               (pcx.c)
;
; Assembled with TASM 3.1 in one pass (no /m): the forward `jmp` in pcx_unpack then
; reserves three bytes and pads its short form with a nop, which is the shipped
; 10d7:2d53 `eb 0c 90`.  TASM's reg,reg encodings (8b ec for mov bp,sp, 03 f0 for
; add si,ax, ...) are the shipped ones; NASM's are not, which is why the recovered
; listing spelled them as db.

                LOCALS                          ; @@ labels are local to each proc

PIXELS_TEXT     segment byte public 'CODE'
                assume  cs:PIXELS_TEXT

                public  _planes_to_pixels, _pixels_to_planes
                public  _pixels2_to_planes, _pcx_unpack

; void far planes_to_pixels(int n, unsigned char far *planes, int stride,
;                           unsigned char far *pixels);
;
; For each of n byte columns, reads one byte from each of eight planes stride bytes
; apart (plane 0 first) and writes eight pixels, plane i giving bit i.  The second
; push bp makes [bp+0] a scratch word; once les has loaded pixels, its offset word
; [bp+10h] is scratch too.
_planes_to_pixels proc far
                push    bp
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                les     di,[bp+10h]             ; pixels
@@column:       lds     si,[bp+0Ah]             ; planes, this column
                mov     ax,[bp+0Eh]             ; stride
                mov     dh,[si]                 ; plane 0
                add     si,ax
                mov     dl,[si]                 ; plane 1
                add     si,ax
                mov     bh,[si]                 ; plane 2
                add     si,ax
                mov     bl,[si]                 ; plane 3
                add     si,ax
                mov     [bp+10h],dx
                mov     [bp+0],bx
                mov     dh,[si]                 ; plane 4
                add     si,ax
                mov     dl,[si]                 ; plane 5
                add     si,ax
                mov     bh,[si]                 ; plane 6
                add     si,ax
                mov     bl,[si]                 ; plane 7
                inc     word ptr [bp+0Ah]
                mov     cx,8
@@pixel:        xor     al,al
                shl     bl,1                    ; planes 7..4
                rcl     al,1
                shl     bh,1
                rcl     al,1
                shl     dl,1
                rcl     al,1
                shl     dh,1
                rcl     al,1
                xchg    bx,[bp+0]
                xchg    dx,[bp+10h]
                shl     bl,1                    ; planes 3..0
                rcl     al,1
                shl     bh,1
                rcl     al,1
                shl     dl,1
                rcl     al,1
                shl     dh,1
                rcl     al,1
                xchg    bx,[bp+0]
                xchg    dx,[bp+10h]
                stosb
                loop    @@pixel
                dec     word ptr [bp+8]
                jnz     @@column
                pop     ds
                pop     di
                pop     si
                pop     bp
                pop     bp
                retf
_planes_to_pixels endp

; void far pixels_to_planes(int n, unsigned char far *pixels, int stride,
;                           unsigned char far *planes);
;
; For each of n groups of eight pixels, gathers bit i of each pixel into a byte for
; plane i and writes the eight plane bytes stride apart (plane 0 first).  Scratch as
; in planes_to_pixels: [bp+0], and pixels' offset word [bp+0Ah] once lds has it.
_pixels_to_planes proc far
                push    bp
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                push    ds
                pop     es
                lds     si,[bp+0Ah]             ; pixels
@@group:        mov     cx,8
@@pixel:        lodsb
                xchg    bx,[bp+0]
                xchg    dx,[bp+0Ah]
                shl     al,1                    ; bits 7..4 -> planes 7..4
                rcl     bl,1
                shl     al,1
                rcl     bh,1
                shl     al,1
                rcl     dl,1
                shl     al,1
                rcl     dh,1
                xchg    bx,[bp+0]
                xchg    dx,[bp+0Ah]
                shl     al,1                    ; bits 3..0 -> planes 3..0
                rcl     bl,1
                shl     al,1
                rcl     bh,1
                shl     al,1
                rcl     dl,1
                shl     al,1
                rcl     dh,1
                loop    @@pixel
                les     di,[bp+10h]             ; planes, this group
                mov     ax,[bp+0Eh]             ; stride
                mov     es:[di],dh              ; plane 0
                add     di,ax
                mov     es:[di],dl              ; plane 1
                add     di,ax
                mov     es:[di],bh              ; plane 2
                add     di,ax
                mov     es:[di],bl              ; plane 3
                add     di,ax
                xchg    bx,[bp+0]
                xchg    dx,[bp+0Ah]
                mov     es:[di],dh              ; plane 4
                add     di,ax
                mov     es:[di],dl              ; plane 5
                add     di,ax
                mov     es:[di],bh              ; plane 6
                add     di,ax
                mov     es:[di],bl              ; plane 7
                inc     word ptr [bp+10h]
                dec     word ptr [bp+8]
                jnz     @@group
                pop     ds
                pop     di
                pop     si
                pop     bp
                pop     bp
                retf
_pixels_to_planes endp

; void far pixels2_to_planes(unsigned char far *pixels, unsigned char far *planes,
;                            int stride, int n);
;
; Two bits a pixel, high pair first, as CGA lays them out: each two source bytes
; (eight pixels) give one byte of plane 0 (each pixel's low bit) at planes and one of
; plane 1 (its high bit) stride bytes on.  n counts source bytes; n / 2 is held in a
; byte and tested after the decrement, so n below 2 runs 256 times.  No far call in
; either image reaches it.
_pixels2_to_planes proc far
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                lds     si,[bp+6]               ; pixels
                les     di,[bp+0Ah]             ; planes
                mov     dx,[bp+0Eh]             ; stride
                mov     ax,[bp+10h]             ; n
                sar     ax,1
                mov     ah,al
@@pair:         lodsb                           ; pixels 0..3
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                lodsb                           ; pixels 4..7
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                rol     al,1
                rcl     bl,1
                rol     al,1
                rcl     bh,1
                push    di
                mov     es:[di],bh              ; plane 0
                add     di,dx
                mov     es:[di],bl              ; plane 1
                pop     di
                inc     di
                dec     ah
                jz      @@done
                jmp     short @@pair
@@done:         pop     ds
                pop     di
                pop     si
                pop     bp
                retf
_pixels2_to_planes endp

; unsigned char far *far pcx_unpack(unsigned char far *in, unsigned char far *out,
;                                   int n);
;
; Decodes PCX run-length data from in until n bytes are written to out, and returns
; in past the last byte read.  A byte of 0C0h or more is a count (its low six bits)
; for the byte after it; any other byte is itself.  A run that overshoots n is
; written whole.
_pcx_unpack     proc far
                push    bp
                mov     bp,sp
                push    si
                push    di
                push    ds
                lds     si,[bp+6]               ; in
                les     di,[bp+0Ah]             ; out
                mov     dx,[bp+0Eh]             ; n
                xor     ch,ch
                cld
@@next:         lodsb
                mov     ah,al
                add     ah,40h                  ; carry: al >= 0C0h, ah = count
                jc      @@run
                stosb
                dec     dx
                jnz     @@next
                jmp     @@done                  ; forward: one pass leaves a nop
@@run:          mov     cl,ah
                sub     dx,cx
                lodsb
                rep     stosb
                or      dx,dx
                jg      @@next
@@done:         mov     dx,ds
                mov     ax,si
                pop     ds
                pop     di
                pop     si
                pop     bp
                retf
_pcx_unpack     endp

PIXELS_TEXT     ends
                end
