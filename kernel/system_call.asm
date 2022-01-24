extern schedule,sys_fork,sys_calls_table,do_timer,do_hd
global timer_interrupt,system_call,ret_from_sys_call,hd_interrupt

nr_system_calls equ 72
_EBX equ 0
_ECX equ 4
_EDX equ 0x08
_ESI equ 0x0C
_EDI equ 0x10
_EBP equ 0x14
_EAX equ 0x18
_DS  equ 0x1C
_ES  equ 0x20
_FS  equ 0x24
_GS  equ 0x28
_OLDEIP equ 0x2c
_OLDCS  equ 0x30
_OLDEFLAGS  equ 0x34
_OLDESP equ 0x38
_OLDSS  equ 0x3c

align 4
bad_sys_call:
    cmp eax,-1
    iret

align 4
system_call:
	cmp eax,nr_system_calls
	ja  bad_sys_call
	push    gs
	push    fs
	push    es
	push    ds
	push	eax
	push    ebp
	push    edi
	push    esi
	push    edx
	push    ecx
	push    ebx

	mov	edx,0x10
	mov	ds,	dx
	mov	es,	dx
	mov 	edx,0x23
	mov 	fs, dx
	mov 	edx,sys_calls_table
	call    [edx+eax*4]
	mov	[esp+_EAX],eax
ret_from_sys_call:
	pop     ebx
	pop     ecx
	pop     edx
	pop     esi
	pop     edi
	pop     ebp
	pop	eax
	pop     ds
	pop     es
	pop     fs
	pop     gs
	iret
align 4
;int 32--clock()
timer_interrupt:
	push    gs
	push    fs
	push    es
	push    ds
	push    ebp
	push    edi
	push    esi
	push    edx
	push    ecx
	push    ebx
	push    eax

	mov eax,0x10
	mov ds, ax
	mov es, ax
	mov edx,0x23
	mov fs, dx
	mov al,0x20
	out 0x20,al
	mov eax,[esp+_OLDCS]
	and eax,3

	push    eax
	call do_timer
	add esp,4

	pop     eax
	pop     ebx
	pop     ecx
	pop     edx
	pop     esi
	pop     edi
	pop     ebp
	pop     ds
	pop     es
	pop     fs
	pop     gs
    iret

    ;hd interrupt
hd_interrupt:
	push    gs
	push    fs
	push    es
	push    ds
	push    ebp
	push    edi
	push    esi
	push    edx
	push    ecx
	push    ebx
	push    eax

	mov eax,0x10
	mov ds, ax
	mov es, ax
	mov edx,0x23
	mov fs, dx
	mov al,0x20
	out 0xA0,al
	nop
	nop
	out 0x20,al
	nop
	nop

	call dword [do_hd]

	pop     eax
	pop     ebx
	pop     ecx
	pop     edx
	pop     esi
	pop     edi
	pop     ebp
	pop     ds
	pop     es
	pop     fs
	pop     gs
    iret
