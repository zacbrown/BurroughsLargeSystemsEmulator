
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
     LOAD   R1, 0
     STORE  R1, [FP-1]
     LOAD   R1, 3
     STORE  R1, [FP-2]
L1:
     LOAD   R1, [FP-1]
     LOAD   R2, 10
     COMP   R1, R2
     JCOND  GEQ, L2
     LOAD   R1, [FP-1]
     LOAD   R2, 5
     COMP   R1, R2
     JCOND  NEQ, L3
     JUMP   L2
L3:
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
L4:
     LOAD   R1, [FP-2]
     LOAD   R2, [FP-1]
     COMP   R1, R2
     JCOND  GEQ, L5
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-2]
     COMP   R1, R2
     JCOND  NEQ, L6
     JUMP   L5
L6:
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-2]
     JUMP   L4
L5:
     JUMP   L1
L2:
     LOAD   SP, FP
     POP    FP
     RET





