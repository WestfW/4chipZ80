;==================================================================================
; S091216 - Z80 Bootloader (Phase 1 boot program) - HW ref: A041116 
;
; This bootloader is embedded into IOS - I/O Subsystem - S221116
;==================================================================================



DstAdr			.equ	$fc00				; Address where to move the bootloader at run time
IOboot2			.equ	02h					; I/O port address for reading the phase 2 payload, one byte each read access

;--------------------------------------------------------------------------

				.org	$0000
Reset:			jp		Start1
Boot2StrAdr:	.block	2					; starting address of the phase 2 payload (dinamically written by IOS during loading)
											; Boot2StrAdr -> LSB, Boot2StrAdr+1 -> MSB
Boot2Lenght:	.block	2					; lenght in bytes of the phase 2 payload (dinamically written by IOS during loading)
											; Boot2Lenght -> LSB, Boot2Lenght+1 -> MSB
											
Start1:			; move to address DstAdr all the bootloader section starting from ToMove address, and jump to it
				ld		hl,ToMove			; HL = source address (ToMove)
				ld		de,DstAdr			; DE = destination address (DstAdr)
				ld		bc,LastByte-ToMove	; BC = bytes to move
				ldir						; move all the code section starting from ToMove to LastByte, at address DstAdr
				jp		ToMove+Delta		; and jump there	

;--------------------------------------------------------------------------
				
ToMove:			; load the phase 2 payload and jump to it
				; NOTE: this address after the move corresponds to DstAdr, so run time (after the move) addresses are given
				; adding Delta = (DstAdr -Start2) to the address
				ld		de,(Boot2Lenght)	; DE = lenght in bytes of the phase 2 payload
				ld		hl,(Boot2StrAdr)	; HL = starting address of the phase 2 payload
				ld		ix,(Boot2StrAdr)	; IX = starting address of the phase 2 payload
				ld		c,IOboot2			; C = I/O address of phase 2 boot payload storage
				ld		b,0					; B = 0 (= 256 bytes to move)
				ld		a,d					; A = MSB of Boot2Lenght
				or		a					; set Z flag
LoopMSB:		jp		z,LoopLSB+Delta		; jump if A = 0 (< 256 bytes to load)
				inir						; load a 256 bytes lot of the phase 2 boot program
				dec		a					; A = A - 1
				jp		LoopMSB+Delta		; load next 256 bytes lot
								
LoopLSB:		
				ld		a,e					; A = LSB of Boot2Lenght
				or		a					; set flags
				jp		z,JmpPhase2+Delta	; jump if A = 0 (0 bytes to load, so all done)
				ld		b,e					; B = bytes to move (= LSB of Boot2Lenght < 256)
				inir						; load last (<256) bytes of the phase 2 boot program
JmpPhase2:		jp		(ix)				; all done, so jump to the phase 2 boot program
LastByte:

;--------------------------------------------------------------------------

Delta			.equ	DstAdr-ToMove	    ; Displacement to add to address for the run time moved part			
				.end