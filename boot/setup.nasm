[bits 16]

SETUP_LEN equ 1 ; (boot/bootstrap.nasm)
SETUP_START equ 34
SYS_SIZE equ 0x180 ; 384=0x180 sectors (build/build.sh)
SYS_INIT_SEG equ 0x1000
SECTOR_ONCE equ 64
BASE equ 0x9000

    mov ax, cs
    mov ds, ax
    mov es, ax
    call load_system

    mov ax, BASE 
    mov ds, ax      ; change ds to 0x9000
    call store_bias_config

    ; ready to enter protected mode
    cli

    ; move system to 0x0000
    mov ax, 0
    cld
move:
    mov es, ax
    xor di, di
    add ax, 0x1000
    cmp ax, 0x9000
    jz move_end
    mov ds, ax
    xor si, si
    mov cx, 0x8000
    rep movsw 
    jmp move
move_end:

    ; enable A20
    in al, 0x92
    or al, 0b0000_0010
    out 0x92, al

    ; install idt and gdt
    mov ax, cs
    mov ds, ax
    mov es, ax   ; change ds to 0x9020
    lidt [idt_48]
    lgdt [gdt_48]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp  0b01_0_00:0 ;0x000000090250

here:
    jmp here

    [bits 16]

    LBA: dw SETUP_START+SETUP_LEN  ; >= 0
    sector: db 18 ; >= 1
    head: db 1
    track: db 0
    sector_limit: db 0 
    head_limit: db 0 

load_system:
    mov ah, 08h
    mov dl, 80h
    int 13h

    ; get disk parameter
    mov byte[head_limit], dh
    inc byte[head_limit]
    mov byte[sector_limit], cl
    and byte[sector_limit], 0b0011_1111 
;   mov byte[track_limit], ch
;   shr cl, 6
;   mov byte[track_limit+1], cl

    ; calc how many times to run
    xor dx, dx
    mov ax, SYS_SIZE 
    mov bx, SECTOR_ONCE 
    div bx
    mov cx, ax
    
    xor bx, bx
    mov ax, SYS_INIT_SEG 
    mov es, ax 
loading: 
    ; read
    push cx
    mov ah, 02h
    mov al, SECTOR_ONCE ; 64 sectors at a time 
    mov ch, byte[track]
    mov cl, byte[sector]
    mov dl, 80h
    mov dh, byte[head]
    int 13h
    pop cx
    
    ; calc next es:bx
    xor bx, bx
    mov ax, es
    add ax, 1000h
    mov es, ax

    ; find the next CHS
    ; Cylinder = LBA/(HPC*SPT)
    ; Head = (LBA/SPT)%HPC
    ; Sector = (LBA%SPT)+1
    add word[LBA], SECTOR_ONCE 
    mov ax, word[LBA]
    mov bl, byte[sector_limit]
    div bl
    inc ah
    mov byte[sector],ah
    xor ah, ah
    mov bl, byte[head_limit]
    div bl
    mov byte[head], ah
    mov al, byte[head_limit]
    mov ah, byte[sector_limit]
    mul ah
    mov bx, ax
    mov ax, word[LBA]
    div bl
    mov byte[track], al

    loop loading
    ret

store_bias_config:
    ; [0x90000] cursor
    mov ah, 0x03
    xor bh, bh
    int 10h
    mov [0], dx

    ; [0x90002] memory size in KB
    mov ah, 0x88
    int 15h
    mov [2], ax 

    ; [0x90004] display page
    ; [0x90006] al = vedio mode, ah = window width
    mov ah, 0x0f
    int 10h
    mov [4], bx
    mov [6], ax

    ; [0x90008] VGA info
    mov ah, 0x12
    mov bl, 0x10
    int 10h
    mov [8], ax
    mov [10], bx
    mov [12], cx

    ret

gdt:
    ;null
    dd 0, 0
    ; code
    dw 0x07FF  ; 8Mb - limit=2047 (2048*4096=8Mb)
    dw 0x0000  ; base address=0
    dw 0x9A00  ; code read/exec
    dw 0x00C0  ; granularity=4096, 386
    ; data (flatten mode)
    ;dw 0x07FF  ; 8Mb - limit=2047 (2048*4096=8Mb)
    dw 0xFFFF  ; 0x100000 * 4K = 4G 
    dw 0x0000  ; base address=0
    dw 0x9200  ; data read/write
    dw 0x00CF  ; granularity=4096, 386
    ; screen
    dw 0xFFFF  ; 0x10000 * 1 = 64K
    dw 0x8000  ; base = 0xb8000
    dw 0x920b  ; data read/write
    dw 0x0040  ; granularity=1, 16bits
idt_48:
    dw 0        ; idt limit=0
    dw 0,0      ; idt base=0L
gdt_48:
    dw 0x800         ; gdt limit=2048, 256 GDT entries
    dw 512+gdt, 0x9  ; gdt base = 0X9xxxx, 
    ; 512+gdt is the real gdt after setup is moved to 0x9020 * 0x10
