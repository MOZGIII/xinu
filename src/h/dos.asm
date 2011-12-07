;---------------------------------------------------------------------
; dos.asm
; macros for assembly-language interface to Microsoft C
; support small memory model only
;---------------------------------------------------------------------

;=====================================================================
%macro dseg 0
     segment word public data
;    group DGROUP _DATA
%endmacro

;dseg	macro
;DGROUP	group	_DATA
;_DATA	segment	word public 'DATA'
;	assume ds:DGROUP
;	endm
;=====================================================================

%macro endds 0
%endmacro

;endds	macro
;_DATA	ends
;	endm

%macro pseg 0
    segment byte public code
%endmacro

;=====================================================================
;pseg	macro
;_TEXT	segment byte public 'CODE'
;	assume cs:_TEXT, ds:DGROUP
;	endm

%macro endps 0
%endmacro
;=====================================================================
;endps	macro
;_TEXT	ends
;	endm
;=====================================================================
