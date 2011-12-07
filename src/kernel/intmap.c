#include <kernel.h>
#include <io.h>
struct intmap *sys_imp;

void init_intmap()
{
    sys_imp = (struct intmap*)malloc(sizeof(struct intmap)*100);
}

/*;-------------------------------------------------------------------------
; intmap  --  interrupt dispatch table
;-------------------------------------------------------------------------
intmap	label	byte

	rept	tblsize

	db	?			; ivec   - interrupt vector number
	call	intcom
	dd	?			; oldisr - old isr from bios (s:o)
	dw	-1			; newisr - new isr code address
	dw	?			; mdevno - minor device number
	dw	?			; iflag  - interrupt flag

	endm

	ASSUME DS:NOTHING

;-------------------------------------------------------------------------
; intcom  --  common interrupt dispatcher
;-------------------------------------------------------------------------
; This procedure is interrupt handling code that is common to all
; interrupt service routines.
intcom	proc	near
	push	bp
	mov	bp,sp
	push	ax			; push registers
	push	bx
	mov	bx,[bp+2]		; get pointer to intmap data
	mov	ax,cs:[bx+8]		; get interrupt flag
	or	al,al			; zero?
	je	short nobios  		; yes, skip the call to the BIOS
	pushf				; push flags to simulate interrupt
	call	cs:dword ptr[bx]	; call BIOS ISR
	cli				; be sure interrupts are back off
nobios:
	push	cx			; save rest of registers
	push	dx
	push	si
	push	di
	push	ds
	push	es
	mov	cs:sssave,ss		; save stack environment
	mov	cs:spsave,sp
	mov	cx,cs			; get code segment
; bp+6 points to code segment where interrupt occurred
	cmp	cx,[bp+6]		; check if we own interrupt
	jnz	short newstack
; time to do our ISR, since the stack and data segments are OK
	push	cs:word ptr[bx+6]	; pass minor dev. no.
	call	cs:word ptr[bx+4]	; call C ISR (saves si, di)
	add	sp,2			; deallocate parm. (C convention)
	jmp	short popregs
newstack:
; now set up temporary stack in DGROUP and do our ISR
	mov	ax,DGROUP
	mov	ds,ax			; set ds to DGROUP
	ASSUME	DS:DGROUP
	mov	ss,ax			; set up temporary stack in DGROUP
	mov	sp,offset topstack
	xor	ax,ax			; clear pcxflag to prevent resched
	xchg	ax,cs:pcxflag
	push	ax			; save for later
	push	cs:word ptr[bx+6]	; pass minor dev. no.
	call	cs:word ptr[bx+4]	; call our routine (saves si, di)
	add	sp,2			; deallocate parm. (C convention)
	pop	cs:pcxflag		; restore pcxflag
	mov	ss,cs:sssave		; restore old stack
	mov	sp,cs:spsave
	ASSUME	DS:NOTHING
popregs:
	pop	es			; restore all registers
	pop	ds
	pop	di
	pop	si
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	pop	bp
	add	sp,2			; remove pointer to intmap area
	iret
intcom	endp

	endps

	end
*/
