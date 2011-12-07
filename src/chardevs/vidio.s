; vidio.asm - _wtty, _vidint

	%include "../h/dos.asm"
	
VID	equ	10h			; video interrupt
WTTY 	equ	0EH				; TTY write subfunction
FORE	equ	7			; foreground color
ADP	equ	0			; active display page

	dseg
; null data segment
	endds
	
	pseg
	
	global	wtty
	global	vidint

;--------------------------------------------------------------------------
; _wtty  --  put a character to the video display using WTTY call
;--------------------------------------------------------------------------
; void wtty(ch)
; char ch;
wtty:;	proc	far
	push	bp			; follows C calling conventions
	mov	bp,sp
	pushf				; push the flags
	push	si			; push registers since . . .
	push	di			; . . . INT VID clobbers these
	mov	al,[bp+6]		; character to write
	mov	ah,WTTY			; TTY write subcommand
	mov	bl,FORE			; foreground color
	mov	bh,ADP			; display page
	int	VID			; call the video function
;        cli                             ; is this needed ?
        pop     di
	pop	si			; restore registers
	popf
	pop	bp
	ret
;_wtty	endp

;-------------------------------------------------------------------------
; vidint  --  general access to VID interrupts; returns flags
;-------------------------------------------------------------------------
; int vidint(r)
; union REGS *r;
vidint:;	proc	far
	push	bp
	mov	bp,sp			; follows C calling conventions
	pushf				; save flags
	push	si
	push	di			; save registers
	mov	si,[bp+6]		; get pointer to register structure
	mov	es,[bp+8]
	mov	ax,[es:si]
	mov	bx,[es:si+2]
	mov	cx,[es:si+4]
	mov	dx,[es:si+6]		; set up registers for call
	push	si
	int	VID			; call the VID BIOS interrupt
	pop	si			; recover register pointer
	pushf				; save flags for return value
	mov	[es:si],ax
	mov	[es:si+2],bx
	mov	[es:si+4],cx
	mov	[es:si+6],dx		; return registers
	pop	ax			; return flags value
	pop	di
	pop	si			; restore registers
	popf				; and the flags
	pop	bp
	ret
;_vidint	endp

	endps
	
	end
