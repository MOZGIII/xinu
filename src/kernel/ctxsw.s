SECTION .text
global ctxsw

;-------------------------------------------------------------------------
; _ctxsw  --  context switch
;-------------------------------------------------------------------------
; void ctxsw(opp,npp)
; char *opp, *npp;
;---------------------------------------------------------------------
; Stack contents upon entry to ctxsw:
;	SP+4 => address of new context stack save area
;       SP+2 => address of old context stack save area
;       SP   => return address
; The addresses of the old and new context stack save areas are
; relative to the DS segment register, which must be set properly
; to access the save/restore locations.
;
; The saved state consists of the current BP, SI and DI registers,
; and the FLAGS register
;---------------------------------------------------------------------

ctxsw:
	push	ebp
	mov	ebp,esp		; frame pointer
	pushf			; flags save interrupt condition
;	cli			; disable interrupts just to be sure (CAN'T DO THIS IN USER MODE)
	push	esi
	push	edi		; preserve registers
	mov	ebx,[ebp+8]	; old stack save address
	mov	[ebx],esp
	mov	ebx,[ebp+12]	; new stack save address
	mov	esp,[ebx]
	pop	edi
	pop	esi
	popf			; restore interrupt state
	pop	ebp
	ret
	