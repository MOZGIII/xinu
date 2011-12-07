; eidi.asm - _sys_disabl, _sys_enabl, _sys_restor, _sys_wait, _sys_hlt

        %include "../h/dos.asm"            ; segment macros
	dseg
; null data segment
	endds

	pseg

	global	sys_disabl,sys_restor,sys_enabl,sys_wait,sys_hlt

;-------------------------------------------------------------------------
; _sys_disabl  --  return interrupt status and disable interrupts
;-------------------------------------------------------------------------
; int sys_disabl()
sys_disabl:;	proc	far
;	pushf			; put flag word on the stack
;	cli			; disable interrupts!
;	pop	ax		; deposit flag word in return register
	ret
;_sys_disabl	endp

;-------------------------------------------------------------------------
; _sys_restor  --  restore interrupt status
;-------------------------------------------------------------------------
; void sys_restor(ps)
; int ps;
sys_restor:;	proc	far
;	push	bp
;	mov	bp,sp		; C calling convenion
;	push	dword [bp+6]
;	popf			; restore flag word
;	pop	bp
	ret
;_sys_restor	endp

;-------------------------------------------------------------------------
; _sys_enabl  --  enable interrupts unconditionally
;-------------------------------------------------------------------------
; void sys_enabl()
sys_enabl:;	proc	far
;	sti			; enable interrupts
	ret
;_sys_enabl	endp

;-------------------------------------------------------------------------
; _sys_wait  --  wait for interrupt
;-------------------------------------------------------------------------
; void sys_wait()
sys_wait:;	proc	far
	pushf
;	sti			; interrupts must be enabled here
;	hlt
	popf
	ret
;_sys_wait	endp

;-------------------------------------------------------------------------
; _sys_hlt  --  halt the current program and return to host
;-------------------------------------------------------------------------
; void sys_hlt()
sys_hlt:;	proc	far
	mov	ah,4ch		; terminate function
	xor	al,al		; OK return code
	int	21h		; MS-DOS function call
	ret
;_sys_hlt	endp

	endps

	end
