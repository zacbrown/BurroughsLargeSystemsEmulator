
     JUMP main

g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1
g_process_control_block:      .space     3

main:
     PUSH   FP
     LOAD   FP, SP
     SUB    SP, 4
     LOAD   R1, [0x00000100]
     STORE  R1, [g_MEMSTART]
     LOAD   R1, [0x00000101]
     STORE  R1, [g_MEMAVAIL]
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 1
     LOAD   R2, g_process_control_block
     STORE  R1, [R2]
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_rmalloc
     POP    R1
     STORE  R0, R1
     LOAD   R2, g_process_control_block
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, 128
     LOAD   R2, g_process_control_block
     LOAD   R3, 2
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, string_0
     STORE  R1, [FP-1]
     LOAD   R1, string_1
     FAKEIT R1, $printstr
     LOAD   R1, [FP-1]
     FAKEIT R1, $printstr
     LOAD   R1, string_2
     FAKEIT R1, $printstr
     LOAD   R1, 1
     LOAD   R2, [FP-1]
     LDCH   R1, R2
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     FAKEIT R1, $printchar
     LOAD   R3, 2
     LOAD   R1, string_3
     LOAD   R2, [FP-1]
     LOAD   R0, 0
     LDCH   R0, R1
     STCH   R3, R2
     LOAD   R1, string_4
     FAKEIT R1, $printstr
     LOAD   R1, [FP-1]
     FAKEIT R1, $printstr
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, string_5
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     LOAD   R1, string_6
     FAKEIT R1, $printstr
     PUSH   R1
     LOAD   R1, string_7
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strchr
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, string_8
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     PUSH   R1
     LOAD   R1, string_9
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strrchr
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, string_10
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printint
     LOAD   R1, string_11
     FAKEIT R1, $printstr
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strlen_word
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-4]
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, [FP-2]
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strcpy
     LOAD   R1, string_12
     FAKEIT R1, $printstr
     LOAD   R1, [FP-3]
     FAKEIT R1, $printint
     LOAD   R1, string_13
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printstr
     LOAD   R1, string_14
     FAKEIT R1, $printstr
     LOAD   R3, 4
     LOAD   R1, 0
     LOAD   R2, [FP-2]
     LOAD   R0, R1
     STCH   R3, R2
     LOAD   R1, string_15
     FAKEIT R1, $printstr
     LOAD   R1, [FP-2]
     FAKEIT R1, $printstr
     LOAD   R1, string_16
     FAKEIT R1, $printstr
     LOAD   R1, string_17
     STORE  R1, [FP-1]
     PUSH   R1
     LOAD   R1, 4
     PUSH   R1
     LOAD   R1, string_18
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_strcmp
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-3]
     LOAD   R1, string_19
     FAKEIT R1, $printstr
     LOAD   R1, [FP-3]
     FAKEIT R1, $printint
     BREAK
     LOAD   R1, 1
     LOAD   R2, [FP-1]
     LDCH   R1, R2
     LOAD   R2, string_20
     COMP   R1, R2
     JCOND  NEQ, L1
     LOAD   R1, string_21
     FAKEIT R1, $printstr
L1:
     LOAD   SP, FP
     POP    FP
     RET



string_0: .STRING "poopy"
string_1: .STRING "string before: "
string_2: .STRING "\n"
string_3: .STRING "z"
string_4: .STRING "\nstring after: "
string_5: .STRING "\nstrlen(x): "
string_6: .STRING "\n"
string_7: .STRING "y"
string_8: .STRING "\nfirst ind of y: "
string_9: .STRING "o"
string_10: .STRING "\nlast ind of o: "
string_11: .STRING "\n"
string_12: .STRING "len: "
string_13: .STRING "y now: "
string_14: .STRING "\n"
string_15: .STRING "y now: "
string_16: .STRING "\n"
string_17: .STRING "poopy"
string_18: .STRING "paap"
string_19: .STRING "strcmp ret: "
string_20: .STRING "p"
string_21: .STRING "\nYES!\n"


  .IMPORT   f_strlen
  .IMPORT   f_strrchr
  .IMPORT   f_strchr
  .IMPORT   f_malloc
  .IMPORT   f_rmalloc
  .IMPORT   f_rmfree
  .IMPORT   f_free
  .IMPORT   f_strcpy
  .IMPORT   f_strlen_word
  .IMPORT   f_strcmp
