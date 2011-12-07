; dskio.asm - _dskio, _dskint

	%include	"../h/dos.asm"

DSK	equ	13H			; disk i/o BIOS function

	dseg
; dummy data segment
	endds

	pseg

	global	dskio

;-------------------------------------------------------------------------
; _dskio  --  perform disk read/write operation
;-------------------------------------------------------------------------
; int dskio(op,buf,drive,cyl,surf,sect)
; int op;			/* 2=read, 3=write			*/
; char *buf;			/* transfer address			*/
; int drive;			/* disk drive number			*/
; int cyl,surf,sect;		/* (cylinder,surface,sector) disk addr	*/
dskio:;	proc	far
	push	bp			; set up the stack frame
	mov	bp,sp			; stack frame pointer
	pushf				; push the flags
;       sti                             ; interrupts on
	push	si
	push	di			; save registers
	mov	ah,[bp+6]		; operation code in ah
	mov	bx,[bp+8]		; buffer pointer in bx
	mov	es,[bp+10]		; buffer segment in es
	mov	dl,[bp+12]		; drive number in dl
	mov	ch,[bp+14]		; cylinder number in ch
	mov	dh,[bp+16]		; surface in dh
	mov	cl,[bp+18]		; sector number in cl
	mov	al,1	 		; transfer one block
        int     DSK                     ; call the DSK BIOS interrupt
        mov     al,ah                   ; error return in al
	xor	ah,ah			; clear upper byte
	pop	di
	pop	si			; restore registers
	popf				; restore the flags
	pop	bp
	ret
;_dskio	endp

	endps

	end
