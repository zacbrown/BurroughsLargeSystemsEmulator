g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_strcpy:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+4]
     STORE  R1, [FP-1]
     LOAD   R1, 0
     STORE  R1, [FP-2]
L1:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+5]
     COMP   R1, R2
     JCOND  GTR, L2
     LOAD   R3, [FP-2]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP+3]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-2]
     JUMP   L1
L2:
     LOAD   R3, [FP-2]
     LOAD   R1, 0
     LOAD   R2, [FP+3]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_strcmp:
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
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-1]
     LOAD   R3, [FP+3]
     LDCH   R2, R3
     COMP   R1, R2
     JCOND  GEQ, L5
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L5:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-1]
     LOAD   R3, [FP+3]
     LDCH   R2, R3
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

f_strcut:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+4]
     STORE  R1, [FP-1]
     LOAD   R1, 0
     STORE  R1, [FP-2]
L7:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+5]
     COMP   R1, R2
     JCOND  GTR, L8
     LOAD   R3, [FP-2]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP+3]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-2]
     JUMP   L7
L8:
     LOAD   R1, [FP+4]
     STORE  R1, [FP-1]
     LOAD   R1, [FP+5]
     STORE  R1, [FP-2]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-2]
     COMP   R1, R2
     JCOND  GEQ, L9
L10:
     LOAD   R1, [FP-2]
     LOAD   R2, [FP+6]
     COMP   R1, R2
     JCOND  GEQ, L11
     LOAD   R3, [FP-1]
     LOAD   R1, [FP-2]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP+2]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R3, [FP-2]
     LOAD   R1, 0
     LOAD   R2, [FP+2]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-2]
     JUMP   L10
L11:
L9:
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_strcat:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+4]
     STORE  R1, [FP-1]
     LOAD   R1, [FP+6]
     STORE  R1, [FP-2]
L12:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+5]
     COMP   R1, R2
     JCOND  GTR, L13
     LOAD   R3, [FP-2]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP+3]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-2]
     JUMP   L12
L13:
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_strlen:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-1]
L14:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L15
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L14
L15:
     LOAD   R1, [FP-1]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_strlen_word:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-1]
L16:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L17
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     JUMP   L16
L17:
     LOAD   R1, [FP-1]
     LOAD   R2, 4
     DIV    R1, R2
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_strrchr:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-1]
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-1]
L18:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, 0
     LOAD   R3, [FP+3]
     LDCH   R2, R3
     COMP   R1, R2
     JCOND  EQL, L19
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-1]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L20
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L20:
     JUMP   L18
L19:
     LOAD   R1, [FP-1]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_strchr:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-1]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
L21:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, 0
     LOAD   R3, [FP+3]
     LDCH   R2, R3
     COMP   R1, R2
     JCOND  EQL, L22
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-2]
     COMP   R1, R2
     JCOND  NEQ, L23
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L23:
     JUMP   L21
L22:
     LOAD   R1, [FP-1]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET



  .EXPORT   f_strcpy
  .EXPORT   f_strcut
  .EXPORT   f_strcat
  .EXPORT   f_strlen
  .EXPORT   f_strchr
  .EXPORT   f_strrchr
  .EXPORT   f_strlen_word
  .EXPORT   f_strcmp
  .IMPORT   f_memset
