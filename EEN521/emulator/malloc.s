g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_rmalloc:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 257
     LOAD   R1, [R1]
     STORE  R1, [FP-1]
     LOAD   R1, 0
     STORE  R1, [FP-4]
     LOAD   R1, [FP-1]
     LOAD   R1, [R1]
     LOAD   R2, 32767
     COMP   R1, R2
     JCOND  EQL, L1
     LOAD   R1, [FP-1]
     LOAD   R1, [R1]
     LOAD   R2, -32767
     COMP   R1, R2
     JCOND  EQL, L2
     LOAD   R1, 32767
     LOAD   R2, [FP-1]
     STORE  R1, [R2]
     LOAD   R1, 256
     LOAD   R1, [R1]
     LOAD   R2, [FP-1]
     SUB    R1, R2
     LOAD   R2, 4
     SUB    R1, R2
     LOAD   R2, [FP-1]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, 32767
     LOAD   R2, 256
     LOAD   R2, [R2]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, 256
     LOAD   R2, [R2]
     LOAD   R3, 2
     SUB    R2, R3
     STORE  R1, [R2]
L2:
L1:
L3:
     LOAD   R1, [FP-4]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L4
L5:
     LOAD   R1, [FP-1]
     LOAD   R1, [R1]
     LOAD   R2, 32767
     COMP   R1, R2
     JCOND  EQL, L6
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L5
L6:
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-2]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-2]
     ADD    R1, R2
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-5]
     LOAD   R1, [FP-2]
     LOAD   R2, [FP+2]
     COMP   R1, R2
     JCOND  LSS, L7
     LOAD   R1, 1
     STORE  R1, [FP-4]
L7:
     JUMP   L3
L4:
     LOAD   R1, -32767
     LOAD   R2, [FP-1]
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-1]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R2, [FP+2]
     ADD    R1, R2
     STORE  R1, [FP-5]
     LOAD   R1, -32767
     LOAD   R2, [FP-5]
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-5]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-2]
     COMP   R1, R2
     JCOND  GEQ, L8
     LOAD   R1, [FP-2]
     LOAD   R2, [FP+2]
     SUB    R1, R2
     LOAD   R2, 4
     SUB    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-5]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-5]
     LOAD   R1, 32767
     LOAD   R2, [FP-5]
     STORE  R1, [R2]
     LOAD   R1, [FP-6]
     LOAD   R2, [FP-5]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-5]
     LOAD   R2, [FP-2]
     LOAD   R3, [FP+2]
     SUB    R2, R3
     LOAD   R3, 2
     SUB    R2, R3
     ADD    R1, R2
     STORE  R1, [FP-5]
     LOAD   R1, 32767
     LOAD   R2, [FP-5]
     STORE  R1, [R2]
     LOAD   R1, [FP-6]
     LOAD   R2, [FP-5]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
L8:
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_rmfree:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+2]
     LOAD   R2, 2
     SUB    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-1]
     LOAD   R1, [R1]
     LOAD   R2, -32767
     COMP   R1, R2
     JCOND  EQL, L9
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L9:
     LOAD   R1, 32767
     LOAD   R2, [FP-1]
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R2, [FP+3]
     ADD    R1, R2
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP+3]
     COMP   R1, R2
     JCOND  EQL, L10
     LOAD   R1, 2
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L10:
     LOAD   R1, [FP-2]
     LOAD   R1, [R1]
     LOAD   R2, -32767
     COMP   R1, R2
     JCOND  EQL, L11
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L11:
     LOAD   R1, 32767
     LOAD   R2, [FP-2]
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 257
     LOAD   R2, [R2]
     COMP   R1, R2
     JCOND  LEQ, L12
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 32767
     COMP   R1, R2
     JCOND  NEQ, L13
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 4
     ADD    R1, R2
     STORE  R1, [FP-4]
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     SUB    R1, R2
     LOAD   R2, 2
     SUB    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R1, [R1]
     LOAD   R2, 32767
     COMP   R1, R2
     JCOND  NEQ, L14
     LOAD   R1, 0
     LOAD   R2, [FP-3]
     STORE  R1, [R2]
     LOAD   R1, 0
     LOAD   R2, [FP-3]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-2]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-6]
     STORE  R1, [FP-1]
L14:
L13:
L12:
     LOAD   R1, [FP-2]
     LOAD   R2, 256
     LOAD   R2, [R2]
     LOAD   R3, 1
     SUB    R2, R3
     COMP   R1, R2
     JCOND  GEQ, L15
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 32767
     COMP   R1, R2
     JCOND  NEQ, L16
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 4
     ADD    R1, R2
     STORE  R1, [FP-4]
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R1, [R1]
     LOAD   R2, 32767
     COMP   R1, R2
     JCOND  NEQ, L17
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-1]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, 2
     SUB    R1, R2
     STORE  R1, [FP-4]
L17:
L16:
L15:
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-5]
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_memset
     LOAD   SP, FP
     POP    FP
     RET


f_malloc:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+3]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-1]
     LOAD   R1, [FP+3]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     STORE  R1, [FP-3]
     LOAD   R1, 0
     STORE  R1, [FP-4]
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 30591
     COMP   R1, R2
     JCOND  EQL, L18
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, -30591
     COMP   R1, R2
     JCOND  EQL, L19
     LOAD   R1, 30591
     LOAD   R2, [FP-3]
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 4
     SUB    R1, R2
     LOAD   R2, [FP-3]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, 30591
     LOAD   R2, [FP-2]
     LOAD   R3, [FP-1]
     ADD    R2, R3
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP-1]
     LOAD   R3, [FP-1]
     ADD    R2, R3
     LOAD   R3, 2
     SUB    R2, R3
     STORE  R1, [R2]
L19:
L18:
     LOAD   R1, [FP-3]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP+2]
     ADD    R1, R2
     LOAD   R2, [FP-1]
     COMP   R1, R2
     JCOND  LEQ, L20
     LOAD   R1, string_0
     FAKEIT R1, $printstr
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L20:
L21:
     LOAD   R1, [FP-4]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L22
L23:
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 30591
     COMP   R1, R2
     JCOND  EQL, L24
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-3]
     JUMP   L23
L24:
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-5]
     LOAD   R2, [FP+2]
     COMP   R1, R2
     JCOND  LSS, L25
     LOAD   R1, 1
     STORE  R1, [FP-4]
L25:
     JUMP   L21
L22:
     LOAD   R1, -30591
     LOAD   R2, [FP-3]
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-3]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-3]
     LOAD   R2, 3
     ADD    R1, R2
     LOAD   R2, [FP+2]
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, -30591
     LOAD   R2, [FP-6]
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, [FP-5]
     COMP   R1, R2
     JCOND  GEQ, L26
     LOAD   R1, [FP-5]
     LOAD   R2, [FP+2]
     SUB    R1, R2
     LOAD   R2, 4
     SUB    R1, R2
     STORE  R1, [FP-7]
     LOAD   R1, [FP-6]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, 30591
     LOAD   R2, [FP-6]
     STORE  R1, [R2]
     LOAD   R1, [FP-7]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-6]
     LOAD   R2, [FP-5]
     LOAD   R3, [FP+2]
     SUB    R2, R3
     LOAD   R3, 1
     SUB    R2, R3
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, 30591
     LOAD   R2, [FP-6]
     STORE  R1, [R2]
     LOAD   R1, [FP-7]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
L26:
     LOAD   R1, [FP-3]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_free:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+4]
     LOAD   R2, 2
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-7]
     LOAD   R1, [FP+4]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-8]
     LOAD   R1, [FP+2]
     LOAD   R2, 2
     SUB    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-1]
     LOAD   R1, [R1]
     LOAD   R2, -30591
     COMP   R1, R2
     JCOND  EQL, L27
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L27:
     LOAD   R1, 30591
     LOAD   R2, [FP-1]
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 3
     ADD    R1, R2
     LOAD   R2, [FP+3]
     ADD    R1, R2
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP+3]
     COMP   R1, R2
     JCOND  EQL, L28
     LOAD   R1, 2
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L28:
     LOAD   R1, [FP-2]
     LOAD   R1, [R1]
     LOAD   R2, -30591
     COMP   R1, R2
     JCOND  EQL, L29
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L29:
     LOAD   R1, 30591
     LOAD   R2, [FP-2]
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-8]
     COMP   R1, R2
     JCOND  LEQ, L30
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 30591
     COMP   R1, R2
     JCOND  NEQ, L31
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 4
     ADD    R1, R2
     STORE  R1, [FP-4]
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     SUB    R1, R2
     LOAD   R2, 2
     SUB    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R1, [R1]
     LOAD   R2, 30591
     COMP   R1, R2
     JCOND  NEQ, L32
     LOAD   R1, 0
     LOAD   R2, [FP-3]
     STORE  R1, [R2]
     LOAD   R1, 0
     LOAD   R2, [FP-3]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-2]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-6]
     STORE  R1, [FP-1]
L32:
L31:
L30:
     LOAD   R1, [FP-2]
     LOAD   R2, [FP-8]
     LOAD   R3, [FP-7]
     ADD    R2, R3
     LOAD   R3, 1
     SUB    R2, R3
     COMP   R1, R2
     JCOND  GEQ, L33
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 30591
     COMP   R1, R2
     JCOND  NEQ, L34
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 4
     ADD    R1, R2
     STORE  R1, [FP-4]
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     ADD    R1, R2
     LOAD   R2, 3
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R1, [R1]
     LOAD   R2, 30591
     COMP   R1, R2
     JCOND  NEQ, L35
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-6]
     LOAD   R3, 1
     SUB    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-1]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, 2
     SUB    R1, R2
     STORE  R1, [FP-4]
L35:
L34:
L33:
     LOAD   R1, [FP-1]
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-5]
     LOAD   R1, [FP-5]
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_memset
     LOAD   SP, FP
     POP    FP
     RET


string_0: .STRING "FIXME: expand heap\n"


  .EXPORT   f_rmalloc
  .EXPORT   f_rmfree
  .EXPORT   f_malloc
  .EXPORT   f_free
  .IMPORT   f_memset
  .IMPORT   f_setInterrupt
