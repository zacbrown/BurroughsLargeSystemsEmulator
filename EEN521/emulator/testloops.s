
     JUMP main

g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

main:
     PUSH   FP
     LOAD   FP, SP
     SUB    SP, 2
     LOAD   R1, [0x00000100]
     STORE  R1, [g_MEMSTART]
     LOAD   R1, [0x00000101]
     STORE  R1, [g_MEMAVAIL]
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 15
     LOADH  R1, 0
     STORE  R1, [FP-1]
     LOAD   R1, 34952
     LOADH  R1, 136
     STORE  R1, [FP-2]
     LOAD   R1, string_0
     FAKEIT R1, $printstr
     LOAD   R1, [FP-1]
     FAKEIT R1, $printint
L1:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-2]
     COMP   R1, R2
     JCOND  GEQ, L2
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     LOADH  R2, 0
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L1
L2:
     LOAD   R1, string_1
     FAKEIT R1, $printstr
     LOAD   R1, [FP-1]
     FAKEIT R1, $printint
     LOAD   R1, string_2
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     LOAD   SP, FP
     POP    FP
     RET



string_0: .STRING "x: "
string_1: .STRING "\nx: "
string_2: .STRING "\ny: "


