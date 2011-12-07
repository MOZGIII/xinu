; stack.asm - _sys_stknpb, _sys_stkinit

	%include	"../h/dos.asm"

	dseg
; MS-DOS C compiler stack limit register
	extern	STKHQQ;:word
	endds

	pseg

	global	sys_stknpb
	global	sys_stkinit

;-------------------------------------------------------------------------
; _sys_stknpb  --  return null process pbase stack base pointer
;-------------------------------------------------------------------------
; char *sys_stknpb()
sys_stknpb:;	proc	far
	mov	ax,STKHQQ		; get lower stack limit
	ret
;_sys_stknpb	endp

;-------------------------------------------------------------------------
; _sys_stkinit  --  run-time stack initialization
;-------------------------------------------------------------------------
; void sys_stkinit()
sys_stkinit:;	proc	far
	mov	word [STKHQQ],0		; set stack base to zero
	ret
;_sys_stkinit	endp

	endps

	end
