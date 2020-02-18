        org     0x7c00
        mov     ax, cs
        mov     ds, ax
        mov     es, ax
        call    DispStr
        jmp     $                       ; infinite loop

DispStr:
        mov     ax, 0x0003
        int     0x10                    ; clear screen
        mov     ax, BootMessage
        mov     bp, ax                  ; string address
        mov     cx, 14+2+14             ; string length
        mov     ax, 0x1301
        mov     bx, 0x000d              ; bh=00 (page), bl=0d (color)
        mov     dl, 0
        int     0x10
        ret

BootMessage:            
        db      "Hello, Litchi!"
        db      0x0d                    ; cl
        db      0x0a                    ; rf
        db      "BugenZhao 2020"

times   510-($-$$)      db      0       ; fill blank
dw      0xaa55                          ; end (little endian)
