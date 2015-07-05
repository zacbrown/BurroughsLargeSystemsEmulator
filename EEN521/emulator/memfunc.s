g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_memcpy:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-1]
L1:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+4]
     COMP   R1, R2
     JCOND  GEQ, L2
     LOAD   R1, [FP+3]
     LOAD   R2, [FP-1]
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP+2]
     LOAD   R3, [FP-1]
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L1
L2:
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_memcmp:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-1]
L3:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+4]
     COMP   R1, R2
     JCOND  GEQ, L4
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-1]
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP+3]
     LOAD   R3, [FP-1]
     ADD    R2, R3
     LOAD   R2, [R2]
     COMP   R1, R2
     JCOND  GEQ, L5
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L5:
     LOAD   R1, [FP+3]
     LOAD   R2, [FP-1]
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP+3]
     LOAD   R3, [FP-1]
     ADD    R2, R3
     LOAD   R2, [R2]
     COMP   R1, R2
     JCOND  LEQ, L6
     LOAD   R1, 1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L6:
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L3
L4:
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_memset:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-1]
     LOAD   R1, [FP+4]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-2]
L7:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-2]
     COMP   R1, R2
     JCOND  GEQ, L8
     LOAD   R1, [FP+3]
     LOAD   R2, [FP+2]
     LOAD   R3, [FP-1]
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L7
L8:
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET



  .EXPORT   f_memcpy
  .EXPORT   f_memcmp
  .EXPORT   f_memset
