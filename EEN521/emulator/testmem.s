
     JUMP main

g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

main:
     PUSH   FP
     LOAD   FP, SP
     SUB    SP, 4
     LOAD   R1, [0x00000100]
     STORE  R1, [g_MEMSTART]
     LOAD   R1, 1000000
     STORE  R1, R2
     LOAD   R1, R2
     FAKEIT R2, $PRINTINT
     LOAD   R1, [0x00000101]
     STORE  R1, [g_MEMAVAIL]
     SUB    SP, 4
     SUB    SP, 4
     SUB    SP, 1
     SUB    SP, 1
L1:
     LOAD   R1, [FP-9]
     LOAD   R2, 4
     COMP   R1, R2
     JCOND  GEQ, L2
     LOAD   R1, [FP-9]
     LOAD   R2, FP-4
     LOAD   R3, [FP-9]
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-9]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-9]
     JUMP   L1
L2:
     PUSH   R1
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, FP-4
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     CALL   f_memCompare
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-10]
     LOAD   R1, string_0
     FAKEIT R1, $printstr
     LOAD   R1, [FP-10]
     FAKEIT R1, $printint
     LOAD   R1, string_1
     FAKEIT R1, $printstr
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, FP-4
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     CALL   f_memCopy
     PUSH   R1
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, FP-4
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     CALL   f_memCompare
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-10]
     LOAD   R1, string_2
     FAKEIT R1, $printstr
     LOAD   R1, [FP-10]
     FAKEIT R1, $printint
     LOAD   R1, string_3
     FAKEIT R1, $printstr
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     CALL   f_memSet
     PUSH   R1
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, FP-4
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     CALL   f_memCompare
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-10]
     LOAD   R1, string_4
     FAKEIT R1, $printstr
     LOAD   R1, [FP-10]
     FAKEIT R1, $printint
     LOAD   R1, string_5
     FAKEIT R1, $printstr
     LOAD   SP, FP
     POP    FP
     RET



string_0: .STRING "ret1: "
string_1: .STRING "\n"
string_2: .STRING "ret2: "
string_3: .STRING "\n"
string_4: .STRING "ret3: "
string_5: .STRING "\n"


  .IMPORT   f_memCopy
  .IMPORT   f_memCompare
  .IMPORT   f_memSet
