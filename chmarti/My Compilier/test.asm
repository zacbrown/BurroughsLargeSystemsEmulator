	JMP main

;Disc IO stuff
;op = 0 read / 1 write / 2 mount / 3 unmount / 4 create
;dev = 0 stdin / 1 stdout / 2 stderr

;Type constants
.define IOTYPE_CHAR 0
.define IOTYPE_INT 1
.define IOTYPE_STRING 2

;Flag constants
.define FLAG_L 0
.define FLAG_G 1
.define FLAG_E 2
.define FLAG_LE 3
.define FLAG_GE 4
.define FLAG_NE 5
.define FLAG_RUN 6
.define FLAG_Z 7

io:       
io_op:      .data   1
io_dev:     .data   1
io_block:   .data   IOTYPE_INT
io_addr:    .data   0

;Prints a character
printc:
	;void printc(int io, char c)
	LOAD FP, SP + 1     ; Position FP 
	LOAD R2, * FP + 3   ; Retrieve the value of the first parameter
	LOAD R3, * FP + 2   ; Retrieve the value of the second parameter
	LOAD R1, 1
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_CHAR
	STORE R1, R2 + 2    ; Set the type to char
	STORE R3, R2 + 3    ; Set the char to c
	IOOP R1, R2         ; Write the char c
	RET

;Prints an integer
printi:
	;void printi(int io, int i)
	LOAD FP, SP + 1     ; Position FP 
	LOAD R2, * FP + 3   ; Retrieve the value of the first parameter
	LOAD R3, * FP + 2   ; Retrieve the value of the second parameter
	LOAD R1, 1
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_INT
	STORE R1, R2 + 2    ; Set the type to int
	STORE R3, R2 + 3    ; Set the int to i
	IOOP R1, R2         ; Write the char i
	RET

hi:	.data 0
main:
	;void main(void)
	LOAD	 R1, 5
	STORE	R1, # hi
whiles0:
	LOAD	 R1, * # hi
	LOAD	 R2, 0
	CMP	R1, R2
	FLAG	R1, FLAG_G
	CMP	R1, 0
	JMPE	whiled0
	LOAD	 R1, * # hi
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	PUSH FP             ; PUSH the old FP onto stack
	CALL printi
	POP FP              ; Get the old FP from stack
	POP R1              ; POP locals back off stack
	POP R1

	JMP	whiles0

whiled0:
	HALT
