; netutil.asm - net2hs, hs2net, net2hl, hl2net

	%include		"../h/dos.asm"

	dseg
; null data segment
	endds

	pseg

	global	net2hs,hs2net,net2hl,hl2net

;----------------------------------------------------------------------
;  net2hl, hl2net  --  convert longs from/to network byte order
;----------------------------------------------------------------------
; long net2hl( i )
; long i;
net2hl:;	proc	far		;define procedure entry point
hl2net:;	label	far

	push	bp
	mov	bp,sp	
	mov	dx,[bp+6]	;get high-order word
	xchg	dl,dh		;swap bytes high word
	mov	ax,[bp+8]	;get low-order word
	xchg	al,ah		;swap bytes low word
	pop	bp
	ret			;return

;_net2hl	endp

;----------------------------------------------------------------------
;  net2hs, hs2net  --  convert shorts from/to network byte order
;----------------------------------------------------------------------
; short net2hs( i )
; short i;
net2hs:;	proc	far
hs2net:;	label	far

	push	bp
	mov	bp,sp	
	mov	ax,[bp+6]	;get word
	xchg	al,ah		;swap bytes word
	pop	bp
	ret			;return

;_net2hs	endp


	endps

	end
