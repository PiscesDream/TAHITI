[bits 32]

%macro ISR 2
    global isr_%1
    isr_%1:
        cli
        %ifidn %2, noerror
            push byte 0
        %endif
        push dword %1
        jmp isr_debug_stub
%endmacro

ISR 0, noerror
ISR 1, noerror
ISR 2, noerror
ISR 3, noerror
ISR 4, noerror
ISR 5, noerror
ISR 6, noerror
ISR 7, noerror
ISR 8, error
ISR 9, noerror
ISR 10, error
ISR 11, error
ISR 12, error
ISR 13, error
ISR 14, error
ISR 15, noerror
ISR 16, noerror
ISR 17, noerror
ISR 18, noerror
ISR 19, noerror
ISR 20, noerror
ISR 21, noerror
ISR 22, noerror
ISR 23, noerror
ISR 24, noerror
ISR 25, noerror
ISR 26, noerror
ISR 27, noerror
ISR 28, noerror
ISR 29, noerror
ISR 30, noerror
ISR 31, noerror

ISR 127, noerror
ISR 128, noerror

extern _fault_handler

isr_debug_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    push ss
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
;   mov gs, ax // caller stack
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, _fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop ss
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    sti
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
