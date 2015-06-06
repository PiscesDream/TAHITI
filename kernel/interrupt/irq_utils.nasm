[bits 32]

%macro IRQ 1
    global irq_%1
    irq_%1:
        cli
        push byte 0  ; error code
        push byte %1+32
        jmp irq_debug_stub
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

extern _irq_handler
irq_debug_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    push ss
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
;    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _irq_handler
    call eax
    pop eax
    pop ss
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
