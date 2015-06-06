[bits 32]
	nop
;;;;mov ax, 0b11_0_00 ; screen
;;;;mov ds, ax

;;;;mov esi, 0x0
;;;;mov word[0], 0x0f00+'a'
;;;;mov word[2], 0x0f00+'b'
;;;;mov word[4], 0x8f00+' '
    jmp after_paging

after_paging:
    mov ax, 0b10_0_00
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x90000
;    push esp
	extern main
	call main

