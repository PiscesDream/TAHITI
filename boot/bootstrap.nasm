[bits 16]
global _start

INIT_SEG equ 0x07c0
BOOTSTRAP_SEG equ 0x9000
SETUP_SEG equ 0x9020
SETUP_LEN equ 1 ; (boot/setup.nasm)

__hd_info:
    BS_jmpBOOT:
        jmp short booting
        nop
    BS_OEMName db '13349047'

    BPB_BytesPerSec dw 0x200
    BPB_SecPerClus db 0x01
    BPB_ResvdSecCnt dw 0x1
    BPB_NumFATs db 0x2
    BPB_RootEntCnt dw 0xe0
    BPB_TotSec16 dw 0xB40
    BPB_Media db 0xF8
    BPB_FATSz16 dw 0x09
    BPB_SecPerTrk dw 0x12
    BPB_NumHeads dw 0x2
    BPB_HiddSec dd 0
    BPB_TotSec32 dd 0

    BS_DrvNum db 80h
    BS_Reserved1 db 0
    BS_BootSig db 0x29
    BS_VolID dd 0
    BS_VolLab db 'MYFAT12DISK' 
    BS_FileSysType db 'FAT12   ' 

booting:
    jmp INIT_SEG:moving
moving:
    mov ax, INIT_SEG
    mov ds, ax
    xor si, si
    mov ax, BOOTSTRAP_SEG
    mov es, ax
    xor di, di
    mov cx, 0x100
    cld
    rep movsw

    jmp BOOTSTRAP_SEG:bootstrap
bootstrap:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0xFFFF

; load setup to SETUP_SEG:SETUP_OFFSET
    mov dh, 0x01 ; head 1
    mov dl, 0x80 ; driver 0
    mov ch, 0x00 ; track 0
    mov cl, 0x11 ; sector 17 
    mov bx, 0; offset
    mov ax, SETUP_SEG
    mov es, ax
    mov al, SETUP_LEN 
    mov ah, 02h
    int 13h

; jmp to setup
    jmp SETUP_SEG:0x0

;   mov ah, 02h
;   al sector count
;   ch cylinder number
;   cl sector number
;   mov dl, 1 ; hd
;   dh head number
;   es:bx
;   int 13h

tail:
    times 508-($-$$) db 0
    dd 0xaa550000 
