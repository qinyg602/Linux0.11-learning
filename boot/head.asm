section .text
bits 32
align 4
    extern qmain
    extern user_stack_top
    extern disp_str
    extern clear
	global _start,disp_pos,idt,gdt,pg_dir
__PAGE_OFFSET equ 0xC0000000
_start:
	mov	eax,010h
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	fs,ax

.1:	inc	eax
	mov	[000000h],eax
	cmp	eax,[100000h]
	je	.1

    ;setup_paging:
	xor	eax,eax
	mov	edi,pg_dir-__PAGE_OFFSET
	mov	ecx,1024*5
	cld
	rep	stosd
	mov	dword [pg_dir-__PAGE_OFFSET],pg0-__PAGE_OFFSET+7
	mov	dword [pg_dir-__PAGE_OFFSET+4],pg1-__PAGE_OFFSET+7
	mov	dword [pg_dir-__PAGE_OFFSET+8],pg2-__PAGE_OFFSET+7
	mov	dword [pg_dir-__PAGE_OFFSET+12],pg3-__PAGE_OFFSET+7

	mov	dword [pg_dir+768*4-__PAGE_OFFSET],pg0-__PAGE_OFFSET+7
	mov	dword [pg_dir+768*4-__PAGE_OFFSET+4],pg1-__PAGE_OFFSET+7
	mov	dword [pg_dir+768*4-__PAGE_OFFSET+8],pg2-__PAGE_OFFSET+7
	mov	dword [pg_dir+768*4-__PAGE_OFFSET+12],pg3-__PAGE_OFFSET+7

	mov	edi,pg3-__PAGE_OFFSET+4092
	mov	eax,0x0fff007
	std

.2:
	stosd
	sub	eax,0x1000
	jae	.2
	mov	eax,pg_dir-__PAGE_OFFSET
	mov	cr3,eax
	mov	eax,cr0
	or	eax,0x80000000
	mov	cr0,eax
	jmp	dword go1		;reload cs descriptor because descriptor redefined
go1:
    mov	esp,[user_stack_top]
    lgdt    [gdt_descr]
    call	setup_idt
	jmp dword 0x8:go2
go2:
    mov	eax,010h
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	fs,ax
	mov	eax,02bh
	mov	gs,ax

	jmp after_page_tables

setup_idt:
	mov	edx,_IdtHandler
	mov	eax,00080000h
	mov	ax,dx
	mov	dx,8E00h
	mov	edi,idt
	mov	ecx,256
rp_sidt:
	mov	[edi],eax
	mov	[edi+4],edx
	add	edi,8
	dec	ecx
	jnz rp_sidt
	cli
	lidt [idt_descr]
	ret
;page tables
	times 0x1000-($-$$) db 0
pg_dir:
    times 0x1000 db 0
pg0:
	times 0x1000 db 0
pg1:
	times 0x1000 db 0
pg2:
	times 0x1000 db 0
pg3:
	times 0x1000 db 0
tmp_floppy_area:
	times 1024 db 0

after_page_tables:
    push L6
	push qmain
    ret
L6:
    jmp $


_IdtHandler:

	iretd

section .data
align 4
disp_pos:
		dd	0
ALIGN	8
	dw 0
idt_descr:
	dw	256*8-1
	dd	idt
idt:
	times 256 dq 0
ALIGN	8
	dw  0
gdt_descr:
	dw	256*8-1
	dd	gdt
gdt:
	dw	0,0,0,0		; dummy

	;KERNEL_CS
	dw	 0FFFFh		; 4GB
	dw	 0000h		; base address=0
	dw	 9A00h		; code read/exec
	dw	 00CFh		; granularity=4096, 386

	;KERNEL_DS
	dw	0FFFFh		; 4GB
	dw	00000h		; base address=0
	dw	09200h		; data read/write
	dw	000CFh		; granularity=4096, 386

	;USER_CS
	dw	 0FFFFh		; 16Mb - limit=2047 (4096*4096=16Mb)
	dw	 0000h		; base address=0
	dw	 0FA00h		; code read/exec
	dw	 00CFh		; granularity=4096, 386

	;USER_DS
	dw	0FFFFh		; 16Mb - limit=2047 (4096*4096=16Mb)
	dw	00000h		; base address=0
	dw	0F200h		; data read/write
	dw	00CFh		; granularity=4096, 386

;	Video descriptor
	dw	 0FFFh		; 16Mb - limit=2047 (2048*4096=8Mb)
	dw	08000h		; base address=0x0B8000
	dw	0F20Bh		; data read/write
	dw	 00C0h		; granularity=4096, 386

	times 250 dq 0
