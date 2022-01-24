SYSSIZE		equ	3000h
BOOTSEG		equ	07c0h
INITSEG		equ 	9000h
SETUPLEN	equ	4
SETUPSEG	equ	9020h
SYSSEG		equ	1000h
ENDSEG		equ	SYSSEG+SYSSIZE
	mov	ax,BOOTSEG
	mov	ds,ax
	mov	ax,INITSEG 
	mov	es,ax
	xor	si,si
	xor	di,di
	mov	cx,256
	rep	
	movsw
	jmp	INITSEG:go
go:	
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,0ff00h
;load the setup-sectors directly after bootblock
	mov	dx,0000h
	mov	cx,0002h
	mov	bx,0200h
	mov	ax,0200h+SETUPLEN
	int	13h
;get disk sectors	
	mov	dl,00
	mov	ax,0800h		; AH=8 is get drive parameters
	int	13h
	mov	ch,00
	mov	word[sectors],cx

;get cursor position,and print some message
	mov	ah,03h
	xor	bh,bh
	int	10h;
;print message	
	mov	ax,INITSEG
	mov	es,ax
	mov	ax,1301h
	mov	bx,000ch
	mov	cx,18
	mov	bp,msg1
	int	10h
;now load the system at 0x1000:0
	mov	ax,SYSSEG
	mov	es,ax

	call	read_it
	call	kill_motor

	jmp	SETUPSEG:0

sread	dw	1+SETUPLEN
head	dw	0
track	dw	0

read_it:
	mov	ax,es
	test	ax,0FFFh
die:	jne	die
	xor	bx,bx
rp_read:
	mov	ax,es
	cmp	ax,ENDSEG
	jb	ok1_read
	ret
ok1_read:
	mov	ax,word[sectors]
	sub	ax,word[sread]
	mov	cx,ax
	shl	cx,9
	add	cx,bx
	jnc	ok2_read
	je	ok2_read
	xor	ax,ax
	sub	ax,bx
	shr	ax,9
ok2_read:
	call	read_track
	mov	cx,ax
	add	ax,word[sread]
	cmp	ax,word[sectors]
	jne	ok3_read
	mov	ax,1
	sub	ax,word[head]
	jne	ok4_read
	inc	word[track]
ok4_read:
	mov	word[head],ax		
	xor	ax,ax
ok3_read:
	mov	word[sread],ax
	shl	cx,9
	add	bx,cx
	jnc	rp_read
	mov	ax,es
	add	ax,01000h
	mov	es,ax
	xor	bx,bx
	jmp	rp_read
read_track:
	push	ax
	push 	bx
	push	cx
	push	dx
	mov	cx,word[sread]
	inc	cx
	mov	dx,word[track]
	mov	ch,dl
	mov	dx,word[head]
	mov	dh,dl
	mov	dl,0
	and	dx,0100h
	mov	ah,02
	int 	13h
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	jc	bad_rt
	jmp	success_rt
bad_rt:
	push	ax
	push	dx
	mov	ax,0
	mov	dx,0
	int	13h
	pop	dx
	pop	ax
	jmp	read_track	
success_rt:
	ret
kill_motor:
	push	dx
	mov	dx,03f2h
	mov	al,0
	out	dx,al
	pop dx
	ret
msg1:
	db "Loading system ..."
sectors:
	dw 0
	times 510-($-$$) db 0
	dw 0aa55h

