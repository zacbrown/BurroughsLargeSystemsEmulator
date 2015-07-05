
     JUMP main

g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_get_elem_from_path:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-6]
     LOAD   R1, 0
     STORE  R1, [FP-1]
     LOAD   R1, 0
     STORE  R1, [FP-5]
     LOAD   R1, 47
     STORE  R1, [FP-7]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-3]
L1:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-3]
     COMP   R1, R2
     JCOND  GEQ, L2
     LOAD   R1, [FP-1]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-7]
     COMP   R1, R2
     JCOND  NEQ, L3
     LOAD   R1, [FP-6]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-6]
L3:
     LOAD   R1, [FP-1]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-1]
     LOAD   R1, [FP+4]
     LOAD   R2, [FP-6]
     COMP   R1, R2
     JCOND  NEQ, L4
     JUMP   L2
L4:
     JUMP   L1
L2:
     LOAD   R1, [FP-1]
     LOAD   R2, [FP-3]
     COMP   R1, R2
     JCOND  LSS, L5
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L5:
     LOAD   R1, [FP-1]
     STORE  R1, [FP-2]
L6:
     LOAD   R1, [FP-2]
     LOAD   R2, [FP-3]
     COMP   R1, R2
     JCOND  GEQ, L7
     LOAD   R1, [FP-2]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-7]
     COMP   R1, R2
     JCOND  NEQ, L8
     JUMP   L7
L8:
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-2]
     JUMP   L6
L7:
     LOAD   R1, [FP-2]
     LOAD   R2, [FP-3]
     COMP   R1, R2
     JCOND  NEQ, L9
     LOAD   R1, 1
     STORE  R1, [FP-5]
L9:
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-2]
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-7]
     COMP   R1, R2
     JCOND  NEQ, L10
     LOAD   R1, 1
     STORE  R1, [FP-5]
L10:
     LOAD   R1, [FP-3]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     FAKEIT R1, $printchar
     LOAD   R1, [FP-2]
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     LOAD   R1, [FP+3]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strcpy
     LOAD   R1, [FP-5]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

main:
     PUSH   FP
     LOAD   FP, SP
     SUB    SP, 3
     LOAD   R1, [0x00000100]
     STORE  R1, [g_MEMSTART]
     LOAD   R1, [0x00000101]
     STORE  R1, [g_MEMAVAIL]
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, string_0
     STORE  R1, [FP-1]
     SUB    SP, 10
     PUSH   R1
     LOAD   R1, 2
     PUSH   R1
     LOAD   R1, FP-12
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_get_elem_from_path
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, string_1
     FAKEIT R1, $printstr
     LOAD   R1, FP-12
     FAKEIT R1, $printstr
     LOAD   R1, string_2
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     PUSH   R1
     LOAD   R1, 3
     PUSH   R1
     LOAD   R1, FP-12
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_get_elem_from_path
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, string_3
     FAKEIT R1, $printstr
     LOAD   R1, FP-12
     FAKEIT R1, $printstr
     LOAD   R1, string_4
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     PUSH   R1
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, FP-12
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_get_elem_from_path
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, string_5
     FAKEIT R1, $printstr
     LOAD   R1, FP-12
     FAKEIT R1, $printstr
     LOAD   R1, string_6
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     LOAD   R1, string_7
     FAKEIT R1, $printstr
     LOAD   SP, FP
     POP    FP
     RET



string_0: .STRING "/abc/def/ghijk"
string_1: .STRING "elem 2: "
string_2: .STRING "\nret 2: "
string_3: .STRING "\nelem 3: "
string_4: .STRING "\nret 3: "
string_5: .STRING "\nelem 4: "
string_6: .STRING "\nret 4: "
string_7: .STRING "\n"


  .IMPORT   f_strlen
  .IMPORT   f_strcpy
