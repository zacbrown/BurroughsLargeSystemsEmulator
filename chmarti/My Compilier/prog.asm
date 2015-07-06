	JMP main

;Disc IO stuff
;op = 0 read / 1 write / 2 mount / 3 unmount / 4 create
;dev = 0 stdin / 1 stdout / 2 stderr

;Type constants
.define IOTYPE_CHAR 0
.define IOTYPE_INT 1
.define IOTYPE_STRING 2
.define IOTYPE_FLOAT 3

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
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1
	LOAD	R2, * # FP + 3
	LOAD	R3, * # FP + 2
	LOAD	R1, 1
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_CHAR
	STORE R1, R2 + 2    ; Set the type to int
	STORE R3, R2 + 3    ; Set the int to i
	IOOP R1, R2         ; Write the char i
	POP	FP
	RET

	;Prints a float
printf:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1
	LOAD	R2, * # FP + 3
	LOAD	R3, * # FP + 2
	LOAD	R1, 1
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_FLOAT
	STORE R1, R2 + 2    ; Set the type to int
	STORE R3, R2 + 3    ; Set the int to i
	IOOP R1, R2         ; Write the char i
	POP	FP
	RET

	;Prints an integer
printi:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1
	LOAD	R2, * # FP + 3
	LOAD	R3, * # FP + 2
	LOAD	R1, 1
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_INT
	STORE R1, R2 + 2    ; Set the type to int
	STORE R3, R2 + 3    ; Set the int to i
	IOOP R1, R2         ; Write the char i
	POP	FP
	RET

	;Reads an integer
readi:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1
	LOAD	R2, * # FP + 2
	LOAD	R1, 0
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_INT
	STORE R1, R2 + 2    ; Set the type to int
	IOOP R11, R2        ; Read the int
	POP	FP
	RET

	;Reads a float
readf:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1
	LOAD	R2, * # FP + 2
	LOAD	R1, 0
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_FLOAT
	STORE R1, R2 + 2    ; Set the type to float
	IOOP R11, R2        ; Read the float
	POP	FP
	RET

	;Reads a char
readc:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1
	LOAD	R2, * # FP + 2
	LOAD	R1, 0
	STORE R1, R2        ; Set the op
	STORE R1, R2 + 1    ; Set the device
	LOAD R1, IOTYPE_CHAR
	STORE R1, R2 + 2    ; Set the type to char
	IOOP R11, R2        ; Read the char
	POP	FP
	RET

	;START FUNCTION create
create:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

		;NEW SCOPE
		SUB	SP, 1
		SUB	SP, 1
		LOAD	 R1, 0
		LOAD	 R2,  # FP - 2
		STORE	 R1, R2
		LOAD	 R1, * # HP
		LOAD	 R2,  # FP - 1
		STORE	 R1, R2

		;if stmt 0
		LOAD	 R1, * # FP + 2
		LOAD	 R2, 2
		MOD	R1, R2
		LOAD	 R2, 0
		CMP	R1, R2
		FLAG	R1, FLAG_NE
		CMP	R1, 0
		JMPE	destroyif0e1

		;NEW SCOPE
		LOAD	 R1, * # FP + 2
		LOAD	 R2, 1
		ADD	R1, R2
		LOAD	 R2,  # FP + 2
		STORE	 R1, R2
		;OLD SCOPE

		JMP	destroyifd0
		destroyif0e1:
destroyifd0:
		;end of if stmt 0

destroywhiles0:
		LOAD	 R1, * # FP - 1
		LOAD	 R2, 0
		CMP	R1, R2
		FLAG	R1, FLAG_NE
		CMP	R1, 0
		JMPE	destroywhiled0

		;NEW SCOPE
		LOAD	 R1, * # FP + 2
		LOAD	 R2, * # FP - 1
		LOAD	 R3, 1
		SUB	R2, R3
		LOAD	R2, * R2
		CMP	R1, R2
		FLAG	R1, FLAG_LE
		CMP	R1, 0
		JMPE	destroywhen0

		;NEW SCOPE
		SUB	SP, 1
		SUB	SP, 1
		LOAD	 R1, * # FP - 1
		LOAD	 R2, 1
		SUB	R1, R2
		LOAD	 R2,  # FP - 3
		STORE	 R1, R2
		LOAD	 R1, * # FP - 1
		LOAD	 R2, * # FP + 2
		ADD	R1, R2
		LOAD	 R2,  # FP - 4
		STORE	 R1, R2
		LOAD	 R1, * # FP - 3
		LOAD	R1, * R1
		LOAD	 R2, * # FP + 2
		SUB	R1, R2
		LOAD	 R2,  # FP - 4
		LOAD	R2, * R2
		STORE	 R1, R2
		LOAD	 R1, * # FP - 4
		LOAD	 R2, 1
		ADD	R1, R2
		LOAD	 R2,  # FP - 4
		STORE	 R1, R2
		LOAD	 R1, * # FP - 1
		LOAD	R1, * R1
		LOAD	 R2,  # FP - 4
		LOAD	R2, * R2
		STORE	 R1, R2
		LOAD	 R1, * # FP + 2
		LOAD	 R2,  # FP - 3
		LOAD	R2, * R2
		STORE	 R1, R2

	;if stmt 1
		LOAD	 R1, * # FP - 2
		LOAD	 R2, 0
		CMP	R1, R2
		FLAG	R1, FLAG_NE
		CMP	R1, 0
		JMPE	destroyif1e1

		;NEW SCOPE
		LOAD	 R1, * # FP - 4
		LOAD	 R2,  # FP - 2
		LOAD	R2, * R2
		STORE	 R1, R2
		;OLD SCOPE

		JMP	destroyifd1
destroyif1e1:

		;NEW SCOPE
		LOAD	 R1, * # FP - 4
		LOAD	 R2,  # HP
		STORE	 R1, R2
		;OLD SCOPE

destroyifd1:
		;end of if stmt 1

		LOAD	 R1, * # FP - 1
		MOV	R11, R1
		ADD	SP, 4
		POP	FP
		RET
		ADD	SP, 2
		;OLD SCOPE

destroywhen0:
		LOAD	 R1, * # FP - 1
		LOAD	 R2,  # FP - 2
		STORE	 R1, R2
		LOAD	 R1, * # FP - 1
		LOAD	R1, * R1
		LOAD	 R2,  # FP - 1
		STORE	 R1, R2
		;OLD SCOPE

		JMP	destroywhiles0

destroywhiled0:
		LOAD	 R1, 0
		MOV	R11, R1
		ADD	SP, 2
		POP	FP
		RET
		ADD	SP, 2
		;OLD SCOPE

		LOAD	R11, 0
		POP	FP
		RET	
		;END FUNCTION create

	;START FUNCTION destroy
destroy:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	LOAD	 R1, * # HP
	LOAD	 R2,  # FP + 2
	LOAD	R2, * R2
	STORE	 R1, R2
	LOAD	 R1, * # FP + 2
	LOAD	 R2,  # HP
	STORE	 R1, R2
	LOAD	 R1, 1
	MOV	R11, R1
	POP	FP
	RET
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION destroy

	HP:	.data 0
enter:	.data 0
savedRand:	.data 0
main:
	;void main(void)
	LOAD	R1, 1024
	STORE	R1, R11
	ADD	R11, 1
	LOAD	R1, 0
	STORE	R1, R11
	STORE	R11, # HP
	MOV	FP, SP
	PUSH	FP
	;Start program content

	LOAD	 R1, 10
	LOAD	 R2, # enter
	STORE	 R1, R2
	LOAD	 R1, 1239
	LOAD	 R2, # savedRand
	STORE	 R1, R2
	JMP funs0

	;START FUNCTION getNextRand
getNextRand:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	LOAD	 R1, * # savedRand
	LOAD	 R2, * # savedRand
	MUL	R1, R2
	LOAD	 R2, 4126
	MUL	R1, R2
	LOAD	 R2, * # savedRand
	LOAD	 R3, 2985
	MUL	R2, R3
	ADD	R1, R2
	LOAD	 R2, 13123
	ADD	R1, R2
	LOAD	 R2, # savedRand
	STORE	 R1, R2
	LOAD	 R1, * # savedRand
	LOAD	 R2, 1000
	MOD	R1, R2
	MOV	R11, R1
	POP	FP
	RET
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION getNextRand

funs0:
	JMP funs1

	;START FUNCTION printArray
printArray:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	SUB	SP, 1
	LOAD	 R1, 0
	LOAD	 R2, # FP - 1
	STORE	 R1, R2
whiles0:
	LOAD	 R1, * # FP - 1
	LOAD	 R2, * # FP + 2
	CMP	R1, R2
	FLAG	R1, FLAG_L
	CMP	R1, 0
	JMPE	whiled0

	;NEW SCOPE
	LOAD	 R1, * # FP + 3
	LOAD	 R2, * # FP - 1
	ADD	R1, R2
	LOAD	R1, * R1

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printi
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	LOAD	 R1, * # FP - 1
	LOAD	 R3, 1
	ADD	R1, R3
	LOAD	 R3, # FP - 1
	STORE	 R1, R3
	;OLD SCOPE

	JMP	whiles0

whiled0:
	LOAD	 R1, 0
	MOV	R11, R1
	ADD	SP, 1
	POP	FP
	RET
	ADD	SP, 1
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION printArray

funs1:
	JMP funs2

	;START FUNCTION fillArray
fillArray:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	SUB	SP, 1
	LOAD	 R1, 0
	LOAD	 R3, # FP - 1
	STORE	 R1, R3
whiles1:
	LOAD	 R1, * # FP - 1
	LOAD	 R3, * # FP + 2
	CMP	R1, R3
	FLAG	R1, FLAG_L
	CMP	R1, 0
	JMPE	whiled1

	;NEW SCOPE
	;function call to getNextRand
	CALL	getNextRand
	;end of function call to getNextRand

	LOAD	 R1, # FP + 3
	LOAD	R1, * R1
;getting expr
	LOAD	 R3, * # FP - 1
	ADD R1, R3
;done getting expr
	STORE	 R11, R1
	LOAD	 R1, * # FP - 1
	LOAD	 R3, 1
	ADD	R1, R3
	LOAD	 R3, # FP - 1
	STORE	 R1, R3
	;OLD SCOPE

	JMP	whiles1

whiled1:
	LOAD	 R1, * # FP + 3
	MOV	R11, R1
	ADD	SP, 1
	POP	FP
	RET
	ADD	SP, 1
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION fillArray

funs2:
	JMP funs3

	;START FUNCTION findSmallestPos
findSmallestPos:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	SUB	SP, 2
	LOAD	 R1, * # FP + 2
	LOAD	 R3, # FP - 1
	STORE	 R1, R3
	LOAD	 R1, * # FP + 4
	LOAD	 R3, * # FP + 2
	ADD	R1, R3
	LOAD	R1, * R1
	LOAD	 R4, # FP - 2
	STORE	 R1, R4
	LOAD	 R1, * # FP + 2
	LOAD	 R4, 1
	ADD	R1, R4
	LOAD	 R4, # FP + 2
	STORE	 R1, R4
whiles2:
	LOAD	 R1, * # FP + 2
	LOAD	 R4, * # FP + 3
	CMP	R1, R4
	FLAG	R1, FLAG_L
	CMP	R1, 0
	JMPE	whiled2

	;NEW SCOPE

	;if stmt 0
	LOAD	 R1, * # FP + 4
	LOAD	 R4, * # FP + 2
	ADD	R1, R4
	LOAD	R1, * R1
	LOAD	 R5, * # FP - 2
	CMP	R1, R5
	FLAG	R1, FLAG_G
	CMP	R1, 0
	JMPE	if0e1

	;NEW SCOPE
	LOAD	 R1, * # FP + 4
	LOAD	 R5, * # FP + 2
	ADD	R1, R5
	LOAD	R1, * R1
	LOAD	 R6, # FP - 2
	STORE	 R1, R6
	LOAD	 R1, * # FP + 2
	LOAD	 R6, # FP - 1
	STORE	 R1, R6
	;OLD SCOPE

	JMP	ifd0
	if0e1:
ifd0:
	;end of if stmt 0

	LOAD	 R1, * # FP + 2
	LOAD	 R6, 1
	ADD	R1, R6
	LOAD	 R6, # FP + 2
	STORE	 R1, R6
	;OLD SCOPE

	JMP	whiles2

whiled2:
	LOAD	 R1, * # FP - 1
	MOV	R11, R1
	ADD	SP, 2
	POP	FP
	RET
	ADD	SP, 2
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION findSmallestPos

funs3:
	JMP funs4

	;START FUNCTION swapPos
swapPos:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	SUB	SP, 1
	LOAD	 R1, * # FP + 4
	LOAD	 R6, * # FP + 3
	ADD	R1, R6
	LOAD	R1, * R1
	LOAD	 R7, # FP - 1
	STORE	 R1, R7
	LOAD	 R1, * # FP + 4
	LOAD	 R7, * # FP + 2
	ADD	R1, R7
	LOAD	R1, * R1
	LOAD	 R8, # FP + 4
	LOAD	R8, * R8
;getting expr
	LOAD	 R9, * # FP + 3
	ADD R8, R9
;done getting expr
	STORE	 R1, R8
	LOAD	 R1, * # FP - 1
	LOAD	 R8, # FP + 4
	LOAD	R8, * R8
;getting expr
	LOAD	 R9, * # FP + 2
	ADD R8, R9
;done getting expr
	STORE	 R1, R8
	LOAD	 R1, 0
	MOV	R11, R1
	ADD	SP, 1
	POP	FP
	RET
	ADD	SP, 1
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION swapPos

funs4:
	JMP funs5

	;START FUNCTION sortArray
sortArray:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	SUB	SP, 2
	LOAD	 R1, 0
	LOAD	 R8, # FP - 1
	STORE	 R1, R8
whiles3:
	LOAD	 R1, * # FP - 1
	LOAD	 R8, * # FP + 2
	CMP	R1, R8
	FLAG	R1, FLAG_L
	CMP	R1, 0
	JMPE	whiled3

	;NEW SCOPE
	;function call to findSmallestPos
	LOAD	 R1, * # FP + 3
	PUSH	R1
	LOAD	 R1, * # FP + 2
	PUSH	R1
	LOAD	 R1, * # FP - 1
	PUSH	R1
	CALL	findSmallestPos
	ADD	SP, 3
	;end of function call to findSmallestPos

	LOAD	 R1, # FP - 2
	STORE	 R11, R1
	;function call to swapPos
	LOAD	 R1, * # FP + 3
	PUSH	R1
	LOAD	 R1, * # FP - 1
	PUSH	R1
	LOAD	 R1, * # FP - 2
	PUSH	R1
	CALL	swapPos
	ADD	SP, 3
	;end of function call to swapPos

	LOAD	 R1, # FP - 2
	STORE	 R11, R1
	LOAD	 R1, * # FP - 1
	LOAD	 R8, 1
	ADD	R1, R8
	LOAD	 R8, # FP - 1
	STORE	 R1, R8
	;OLD SCOPE

	JMP	whiles3

whiled3:
	LOAD	 R1, 0
	MOV	R11, R1
	ADD	SP, 2
	POP	FP
	RET
	ADD	SP, 2
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION sortArray

funs5:
	JMP funs6

	;START FUNCTION isSorted
isSorted:
	PUSH	FP
	MOV	FP, SP
	ADD	FP, 1

	;NEW SCOPE
	SUB	SP, 1
	LOAD	 R1, 0
	LOAD	 R8, # FP - 1
	STORE	 R1, R8
whiles4:
	LOAD	 R1, * # FP - 1
	LOAD	 R8, * # FP + 2
	LOAD	 R9, 1
	SUB	R8, R9
	CMP	R1, R8
	FLAG	R1, FLAG_L
	CMP	R1, 0
	JMPE	whiled4

	;NEW SCOPE

	;if stmt 1
	LOAD	 R1, * # FP + 3
	LOAD	 R8, * # FP - 1
	ADD	R1, R8
	LOAD	R1, * R1
	LOAD	 R9, * # FP + 3
	LOAD	 R10, * # FP - 1
	LOAD	 R11, 1
	ADD	R10, R11
	ADD	R9, R10
	LOAD	R9, * R9
	CMP	R1, R9
	FLAG	R1, FLAG_L
	CMP	R1, 0
	JMPE	if1e1

	;NEW SCOPE
	LOAD	 R1, 0
	MOV	R11, R1
	ADD	SP, 1
	POP	FP
	RET
	;OLD SCOPE

	JMP	ifd1
	if1e1:
ifd1:
	;end of if stmt 1

	LOAD	 R1, * # FP - 1
	LOAD	 R9, 1
	ADD	R1, R9
	LOAD	 R9, # FP - 1
	STORE	 R1, R9
	;OLD SCOPE

	JMP	whiles4

whiled4:
	LOAD	 R1, 1
	MOV	R11, R1
	ADD	SP, 1
	POP	FP
	RET
	ADD	SP, 1
	;OLD SCOPE

	LOAD	R11, 0
	POP	FP
	RET	
	;END FUNCTION isSorted

funs6:
	SUB	SP, 1
	SUB	SP, 1
	SUB	SP, 1
	;function call to fillArray
	;function call to create
	LOAD	 R1, 18
	PUSH	R1
	CALL	create
	ADD	SP, 1
	;end of function call to create
	PUSH	R11
	LOAD	 R1, 18
	PUSH	R1
	CALL	fillArray
	ADD	SP, 2
	;end of function call to fillArray

	LOAD	 R1, # FP - 3
	STORE	 R11, R1
	;function call to printArray
	LOAD	 R1, * # FP - 3
	PUSH	R1
	LOAD	 R1, 18
	PUSH	R1
	CALL	printArray
	ADD	SP, 2
	;end of function call to printArray

	LOAD	 R1, # FP - 1
	STORE	 R11, R1
	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	;function call to sortArray
	LOAD	 R1, * # FP - 3
	PUSH	R1
	LOAD	 R1, 18
	PUSH	R1
	CALL	sortArray
	ADD	SP, 2
	;end of function call to sortArray

	LOAD	 R1, # FP - 1
	STORE	 R11, R1
	;function call to printArray
	LOAD	 R1, * # FP - 3
	PUSH	R1
	LOAD	 R1, 18
	PUSH	R1
	CALL	printArray
	ADD	SP, 2
	;end of function call to printArray

	LOAD	 R1, # FP - 1
	STORE	 R11, R1
	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	;function call to isSorted
	LOAD	 R1, * # FP - 3
	PUSH	R1
	LOAD	 R1, 18
	PUSH	R1
	CALL	isSorted
	ADD	SP, 2
	;end of function call to isSorted


	;function call to print
	PUSH # io           ; Push io block
	PUSH R11       ; Push the value to print
	CALL printi
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	LOAD	 R1, 30
	LOAD	 R9, # FP - 3
	LOAD	R9, * R9
;getting expr
	LOAD	 R11, 1
	ADD R9, R11
;done getting expr
	STORE	 R1, R9
	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	;function call to printArray
	LOAD	 R1, * # FP - 3
	PUSH	R1
	LOAD	 R1, 18
	PUSH	R1
	CALL	printArray
	ADD	SP, 2
	;end of function call to printArray

	LOAD	 R1, # FP - 1
	STORE	 R11, R1
	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	;function call to isSorted
	LOAD	 R1, * # FP - 3
	PUSH	R1
	LOAD	 R1, 18
	PUSH	R1
	CALL	isSorted
	ADD	SP, 2
	;end of function call to isSorted


	;function call to print
	PUSH # io           ; Push io block
	PUSH R11       ; Push the value to print
	CALL printi
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	LOAD	 R1, * # enter

	;function call to print
	PUSH # io           ; Push io block
	PUSH R1       ; Push the value to print
	CALL printc
	ADD	SP, 2  ; POP locals back off stack
	;end of function call to print

	;function call to destroy
	LOAD	 R1, * # FP - 3
	PUSH	R1
	CALL	destroy
	ADD	SP, 1
	;end of function call to destroy
	ADD	SP, 3
	POP	FP
	HALT
