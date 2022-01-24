extern do_divide_error,do_debug,do_nmi,do_int3,do_overflow,do_bounds,do_invalid_op,do_device_not_available,do_double_fault
extern do_coprocessor_segment_overrun,do_invalid_TSS,do_segment_not_present,do_stack_segment,do_general_protection,
extern do_coprocessor_error,do_reserved
extern do_keyboard

global divide_error,debug,nmi,int3,overflow,bounds,invalid_op,device_not_available,double_fault
global coprocessor_segment_overrun,invalid_TSS,segment_not_present,stack_segment,general_protection,
global coprocessor_error,reserved
global keyboard

divide_error:
    push    do_divide_error
no_error_code:
    xchg    eax,    dword[esp]
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp
    push    ds
    push    es
    push    fs

    push    0
	lea		edx,[esp+44]
    push    edx
    mov     ebx,    0x10
    mov     ds,bx
    mov     es,bx
    mov     fs,bx
    call    eax

    pop     fs
    pop     es
    pop     ds
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    iret

error_code:
    xchg    eax,    dword[esp+4]
    xchg    ebx,    dword[esp]
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp
    push    ds
    push    es
    push    fs
    push    eax
    lea     eax,[esp+44]
    push    eax
    mov     eax,    0x10
    mov     ds,ax
    mov     es,ax
    mov     fs,ax

    call    ebx
    add     esp,8

    pop     fs
    pop     es
    pop     ds
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    iret
;int 1--debug
debug:
    push do_debug
    jmp no_error_code

;int 2--
nmi:
    push do_nmi
    jmp no_error_code

;int 3--
int3:
    push do_int3
    jmp no_error_code

;int 4--overflow
overflow:
    push do_overflow
    jmp no_error_code

;int 5--bounds
bounds:
    push do_bounds
    jmp no_error_code

;int 6--invalid op
invalid_op:
    push do_invalid_op
    jmp no_error_code

;int 7--device not available
device_not_available:
    push do_device_not_available
    jmp no_error_code

;int 8--double fault
double_fault:
    push do_double_fault
    jmp error_code

;int 9--
coprocessor_segment_overrun:
    push do_coprocessor_segment_overrun
    jmp no_error_code

;int 10--invalid TSS
invalid_TSS:
    push do_invalid_TSS
    jmp error_code

;int 11--
segment_not_present:
    push do_segment_not_present
    jmp error_code

;int 12--
stack_segment:
    push do_stack_segment
    jmp error_code

;int 13--
general_protection:
    push do_general_protection
    jmp	error_code


;int 15--reserved
reserved:
    push do_reserved
    jmp no_error_code

;int 16--
coprocessor_error:
    push do_coprocessor_error
    jmp no_error_code
keyboard:
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp
    push    ds
    push    es
    push    fs

    mov eax,0x10
    mov ds, ax
    mov es, ax
    mov fs, ax

    call    do_keyboard
    mov al,0x20
    out 0x20,al
    pop     fs
    pop     es
    pop     ds
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    iret

