; LCD.s
; Student names: Ayan Basu & Ayush Roychowdhury
; Last modification date: change this to the last modification date or look very silly

; Runs on TM4C123
; Use I2C3 to send data to SSD1306 128 by 64 pixel oLED

; As part of Lab 7, students need to implement I2C_Send2

      EXPORT   I2C_Send2
      PRESERVE8
      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB
      ALIGN
I2C3_MSA_R  EQU 0x40023000
I2C3_MCS_R  EQU 0x40023004
I2C3_MDR_R  EQU 0x40023008
; sends two bytes to specified slave
; Input: R0  7-bit slave address
;        R1  first 8-bit data to be written.
;        R2  second 8-bit data to be written.
; Output: 0 if successful, nonzero (error code) if error
; Assumes: I2C3 and port D have already been initialized and enabled
I2C_Send2

;; --UUU-- 
; 1) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
; 2) write slave address to I2C3_MSA_R, 
;     MSA bits7-1 is slave address
;     MSA bit 0 is 0 for send data
; 3) write first data to I2C3_MDR_R
; 4) write 0x03 to I2C3_MCS_R,  send no stop, generate start, enable
; add 4 NOPs to wait for I2C to start transmitting
; 5) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
; 6) check for errors, if any bits 3,2,1 I2C3_MCS_R is high 
;    a) if error set I2C3_MCS_R to 0x04 to send stop 
;    b) if error return R0 equal to bits 3,2,1 of I2C3_MCS_R, error bits
; 7) write second data to I2C3_MDR_R
; 8) write 0x05 to I2C3_MCS_R, send stop, no start, enable
; add 4 NOPs to wait for I2C to start transmitting
; 9) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
; 10) return R0 equal to bits 3,2,1 of I2C3_MCS_R, error bits
;     will be 0 is no error

; 1) Wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0: BUSY-WAIT SYNCHRONIZATION	
	PUSH {R4, R5, R6, R7, R8,LR};
loopStart1
	LDR R7, =I2C3_MCS_R				;i) Load I2C3_MCS_R into register		
	LDR R6, [R7]					;ii) Load in data at that location
	AND R4, R6, #0x01				;iii) Extract bit 0
	CMP	R4, #0						;iv) Compare to 0; if not 0, loop back to start
	BNE loopStart1    				;iv) Compare to 0; if not 0, loop back to start
	
; 2) write slave address to I2C3_MSA_R, ;MSA bits7-1 is slave address ;MSA bit 0 is 0 for send data
	LSL R0, #1						; i) Left shift R0
	;AND R0, #0xFE 					; ii) Make sure bit 0 of R0 is 0
	LDR R3, =I2C3_MSA_R 			; iii) Load MSA_R location address in register
	STRB R0, [R3] 					; iv) Store R0 in MSA register

; 3) write first data to I2C3_MDR_R
	LDR R3, =I2C3_MDR_R				; i) Load MDR location address into register
	STRB R1, [R3]					; ii) Store R1 into MDR register

; 4) write 0x03 to I2C3_MCS_R,  send no stop, generate start, enable ; add 4 NOPs to wait for I2C to start transmitting
	MOV R4, #0x03					; i) MOV 0x03 in register
	LDR R3, =I2C3_MCS_R				; ii) Load MCS_R location address into register
	STRB R4, [R3]					; iii) Store register in MCS register
	NOP								; iv) NOP (x4)
	NOP
	NOP
	NOP			

; 5) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
									; i) Same as 1)
loopStart5
	LDR R3, =I2C3_MCS_R				;i) Load I2C3_MCS_R into register		
	LDR R4, [R3]					;ii) Load in data at that location
	AND R4, #0x01				;iii) Extract bit 0
	CMP	R4, #0						;iv) Compare to 0; if not 0, loop back to start
	BNE loopStart5    				;iv) Compare to 0; if not 0, loop back to start

; 6) check for errors, if any bits 3,2,1 I2C3_MCS_R is high 
;    a) if error set I2C3_MCS_R to 0x04 to send stop 
;    b) if error return R0 equal to bits 3,2,1 of I2C3_MCS_R, error bits
	LDRB R4, [R3]					; i) Use MCS loaded in 5)
	;LSR R4, R4, #1					; ii) Right shift by 1
	AND R4, #0x0E					; iii) Extract bit 2, 1, 0 (0x07) Keep bits 1,2&3 in the place they are located
	CMP R4, #0						; iv) Compare with 0 (if not 0, ERROR, else continue)
	BNE Error						; iv) Compare with 0 (if not 0, ERROR, else continue)
; 7) write second data to I2C3_MDR_R
									; i) Same as 3)
	LDR R3, =I2C3_MDR_R				; i) Load MDR location address into register
	STRB R2, [R3]					; ii) Store R2 into MDR register

; 8) write 0x05 to I2C3_MCS_R, send stop, no start, enable, add 4 NOPs to wait for I2C to start transmitting
									; i) Same as 4) but write 0x05
	MOV R4, #0x05					; i) MOV 0x05 in register
	LDR R3, =I2C3_MCS_R				; ii) Load MCS_R location address into register
	STRB R4, [R3]					; iii) Store register in MCS register
	NOP								; iv) NOP (x4)
	NOP
	NOP
	NOP	

; 9) wait while I2C is busy, wait for I2C3_MCS_R bit 0 to be 0
									; i) Same as 5)
loopStart9
	LDR R3, =I2C3_MCS_R				;i) Load I2C3_MCS_R into register		
	LDR R4, [R3]					;ii) Load in data at that location
	AND R4, #0x01					;iii) Extract bit 0
	CMP	R4, #0						;iv) Compare to 0; if not 0, loop back to start
	BNE loopStart9    				;iv) Compare to 0; if not 0, loop back to start

; 10) return R0 equal to bits 3,2,1 of I2C3_MCS_R, error bits
;     will be 0 is no error 		; i) Same as 6)
	LDRB R4, [R3]					; i) Use MCS loaded in 5)
	;LSR R4, R4, #1					; ii) Right shift by 1
	AND R4, #0x0E					; iii) Extract bit 2, 1, 0 (0x07)
	CMP R4, #0						; iv) Compare with 0 (if not 0, ERROR, else continue)
	BNE Error
	MOV R0, #0						; ii) If no error, MOV 0 into R0 and return
	POP {R4, R5, R6, R7, R8, LR};
    BX  LR                          ;   return
Error
	MOV R0, R4						; i) Move error code to R0
	MOV R5, #0x04 					; ii) Move 0x04 to register
	LDR R4, =I2C3_MCS_R				; iii) Load MCS_R location in register
	STRB R5, [R4] 					; iv) Store 0x04 to MCS register
	POP {R4, R5, R6, LR};
	
	BX LR 							; v) return BX LR

    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file