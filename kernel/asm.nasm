[bits 32]
    global gdt_flush 
    extern _sys_gdt_ptr, main
gdt_flush:
    lgdt [_sys_gdt_ptr]
    jmp 0x08:.flush
    .flush:
    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov fs, ax 
    mov ss, ax 
    mov sp, 0xffff
    call main

    global _set_idt
    extern _sys_idt_ptr
_set_idt:
    lidt [_sys_idt_ptr]
    ret


; from jamesM's tutorial
    global copy_page_physical
copy_page_physical:
    push ebx              ; According to __cdecl, we must preserve the contents of EBX.
    pushf                 ; push EFLAGS, so we can pop it and reenable interrupts
                          ; later, if they were enabled anyway.
    cli                   ; Disable interrupts, so we aren't interrupted.
                          ; Load these in BEFORE we disable paging!
    mov ebx, [esp+12]     ; Source address
    mov ecx, [esp+16]     ; Destination address
  
    mov edx, cr0          ; Get the control register...
    and edx, 0x7fffffff   ; and...
    mov cr0, edx          ; Disable paging.
  
    mov edx, 1024         ; 1024*4bytes = 4096 bytes to copy

.loop:
    mov eax, [ebx]        ; Get the word at the source address
    mov [ecx], eax        ; Store it at the dest address
    add ebx, 4            ; Source address += sizeof(word)
    add ecx, 4            ; Dest address += sizeof(word)
    dec edx               ; One less word to do
    jnz .loop
    
    mov edx, cr0          ; Get the control register again
    or  edx, 0x80000000   ; and...
    mov cr0, edx          ; Enable paging.
    
    popf                  ; Pop EFLAGS back.
    pop ebx               ; Get the original value of EBX back.
    ret 

    global read_eip
read_eip:
    pop eax
    jmp eax

    global switch_to_user_mode
switch_to_user_mode:
    cli
    mov ax, 0x23 ; user data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push 0x23
    push eax

    pushf
    pop eax     ; sti will cause error in user mode 
    or eax, 0x200
    push eax  
    push 0x1b ; user code selector
    push .flush
    iret
    .flush: ; user mode
    ret


    global tss_load
tss_load:
    mov eax, [esp+4]
    ltr ax
    ret

    global tss_save
tss_save:
    mov eax, [esp+4]
    str ax
    ret

    global jmp_tss
jmp_tss:
    mov eax, [esp+4]
    jmp far [eax]

;; what to change: copy_estatus, copy_estatus_back
    global copy_estatus
copy_estatus: ; ebp; eip; 
    enter 0, 0
    push ecx
    push ds

    mov ecx, [ebp+8]
    mov ax, ss
    mov ds, ax 
    mov esi, [ebp+12] ; ds:esi -> es:edi
    mov edi, 0x60000 

.for:
    cmp ecx, 0
    je .endfor

    mov byte al, [ds:esi]
    mov byte [es:edi], al

    inc esi
    inc edi
    
    dec ecx
    jmp .for
.endfor:

    pop ds
    pop ecx
    leave
    ret

; move_stack(next_task->estatus.ss, next_task->estatus.esp, cur_task->estatus.esp);
    global move_stack
move_stack:
    mov ecx, [esp+12]
    sub ecx, esp

    mov ax, ss
    mov ds, ax
    mov esi, [esp+12] 
    
    mov ax, [esp+4]
    mov es, ax
    mov edi, [esp+8]  

;    std
;    rep movsb
.for: 
    cmp ecx, 0
    je .end
    dec esi
    dec edi
    mov byte al, [ds:esi] 
    mov byte [es:edi], al 
    dec ecx
    jmp .for
    .end:

    mov ax, es
    mov ss, ax 
    mov esp, edi

    mov ax, 0x10
    mov ds, ax 

    ret

    ; ds:si -> es:di
    global copy_estatus_back
copy_estatus_back: ;ebp ; eip
    enter 0, 0
    mov ecx, [esp+8]
    mov ax, ss
    mov es, ax 
    mov esi, 0x60000 ; ds:si->es:di
    mov edi, [esp+12]
    cld
    rep movsb
    leave
    ret


    global read_hd_to_addr
read_hd_to_addr: ; ebp, eip, LBA, buffer_ip
    enter 0, 0
    push edx
    push ebx
    push ecx

    mov dx, 0x1f2
    mov al, 0x01 ; read 1 sector
    out dx, al

    mov eax, [ebp+8]

    ; 0~7, 0x1f3
    inc dx
    out dx, al
    shr eax, 8

    ; 8~15, 0x1f4
    inc dx
    out dx, al
    shr eax, 8

    ; 16~23, 0x1f5
    inc dx
    out dx, al
    shr eax, 8

    ; 23~27, Master/slave, 0x1f5
    inc dx
    or al, 0b11100000
    out dx, al

    ; read
    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    mov dx, 0x1f7
    .waits: ; ready
        in al, dx
        and al, 0x88
        cmp al, 0x08
    jnz .waits

    mov ebx, [ebp+12]
    mov ecx, 256
    mov dx, 0x1f0
    .readw:
        in ax, dx
        mov [ebx], ax
        add ebx, 2
    loop .readw
    
    pop ecx
    pop ebx
    pop edx
    leave 
    ret
