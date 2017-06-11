;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; UART ISR implementation
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Once the data is received from PC, we send it back to it.

			.cdecls C,LIST,"msp430.h", "servo_ctrl.h"       ; Include device header file //TODO: Maybe I need servo.h here, maye not

			.ref	ascii_got

; USCIA0 ISR
			.text
UARTISR		add.w	&UCA0IV, PC				; use UCA0IV register to decode highest priority interrupt
			reti
			jmp		RXISR					; use only RXISR
TXISR		reti
RXISR		push	R12
			mov.b	&UCA0RXBUF, R12			; save received data in R12
			mov.b	R12, &UCA0TXBUF			; send received data to PC
			call	#ascii_got
			pop		R12
			reti

; Vectors
			.sect	.int57
			.short	UARTISR
