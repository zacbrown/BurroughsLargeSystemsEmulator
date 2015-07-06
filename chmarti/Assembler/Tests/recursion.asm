    JMP main
       
prompt:
    .data   1164866661  ; 0x456E7465 "Ente"
    .data   1914719861  ; 0x72204E75 "r Nu"
    .data   1835165042  ; 0x6D626572 "mber"
    .data   975175680   ; 0x3A200000 ": "

;Disc IO stuff
;op = 0 read / 1 write / 2 mount / 3 unmount / 4 create
;dev = 0 stdin / 1 stdout / 2 stderr

.define IOTYPE_CHAR 0
.define IOTYPE_INT 1
.define IOTYPE_STRING 2

io:       
io_op:      .data   1
io_dev:     .data   1
io_block:   .data   IOTYPE_STRING
io_addr:    .data   prompt

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

main:
    ;void main(void)
    LOAD FP, SP         ; No return addr or paramaters initially
    PUSH R1             ; Push a fake return addr    

    ;cout << "Enter Number: ";    
    IOOP R1, # io       ; Print prompt
    
    ; int num
    PUSH R1             ; Local variable NUM

    ;cin >> num
    LOAD R1, 0
    STORE R1, # io_op   ; Set the op
    STORE R1, # io_dev  ; Set the device
    LOAD R1, IOTYPE_INT
    STORE R1, # io_block ; Set the type to int
    IOOP R1, # io       ; Read the number
    STORE R1, FP - 1    ; Store it into the local variable NUM
    
    ; printRev(num)
    PUSH * FP - 1       ; PUSH local variable NUM onto stack as parameter 1
    PUSH FP             ; PUSH the old FP onto stack
    CALL printRev       ; Call the function printRev
    POP FP              ; Get the old FP from stack
    POP R1              ; Remove the parameter from the stack

    ; printc(io, '\n')
    PUSH # io
    PUSH 10
    PUSH FP
    CALL printc
    POP FP
    POP R1
    POP R1

    ;return
    POP R1              ; Remove local variable NUM from stack
    HALT                ; End

printRev:
    ;void printRev(int num)
    LOAD FP, SP + 1     ; Position FP    
    
    ; int next
    PUSH R1             ; Make space for local variable NEXT

    ; int rem
    PUSH R2             ; Make space for local variable REM

    ; next = num / 10
    LOAD R1, * FP + 2   ; Retrieve the value of the first parameter
    DIV R1, 10;         ; Divide it by 10
    STORE R1, FP - 1    ; Store the result in local variable NEXT

    ; rem = num % 10
    LOAD R1, * FP + 2   ; Retrieve the value of the first parameter
    MOD R1, 10;         ; Modulus it by 10
    STORE R1, FP - 2    ; Store the result in local variable REM

    ; cout << rem;
    PUSH # io           ; Push io block
    PUSH * FP - 2       ; Push the local variable REM
    PUSH FP             ; PUSH the old FP onto stack
    CALL printi
    POP FP              ; Get the old FP from stack
    POP R1              ; POP locals back off stack
    POP R1
    
    ; if(next != 0)
    LOAD R1, * FP - 1   ; Load the local variable NEXT
    CMP R1, 0           ; Compare R1 with 0
    JMPE de             ; Jump to label de if equal

    ;printRev(num)
    PUSH R1             ; Push local variable NEXT as first parameter 
    PUSH FP             ; PUSH the old FP onto stack
    CALL printRev       ; Call function printRev
    POP FP              ; Get the old FP from stack
    POP R1              ; Remove parameter from stack

de: ;return
    POP R2;             ; POP locals back off stack
    POP R1; 
    RET          


