; cksum.asm - _cksum

	%include	"../h/dos.asm"

	dseg
; null data segment
	endds

	pseg

	global	cksum
	
; ---------------------------------------------------------------------------
;  cksum  --  compute the ones complement of the 16 bit ones complement 
;	      chksum of an array of 16 bit words.  Adapted from a version
;	      by Noel Chiappa
;
;  int	cksum(buf,nwords)
;	int	*buf
;	int	nwords
; ---------------------------------------------------------------------------
cksum:;	proc	far
	push	bp			; set up base pointer
	mov	bp,sp			
	
	mov	bx,[bp+6]		; bx = address of buf
	mov	es,[bp+8]		; es = segment of buf
	mov	cx,[bp+10]		; cx = nwords
	xor	ax,ax			; ax = 0
	clc				; clear carry
	
cksum1:					; repeat
	adc	ax,[es:bx]		;    ax += buf[i] (ones complement)
	inc	bx			;    bx++
	inc	bx			;    (word pointer)
	loop	cksum1		; until ( --cx == 0 )

	adc	ax,0			; add final carry
	not	ax			; complement sum
	pop	bp
	ret

;_cksum	endp

	endps
	
	end
