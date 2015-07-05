
     JUMP main

g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_test_Size:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 1
     STORE  R1, [FP-3]
     PUSH   R1
     LOAD   R1, [FP-3]
     PUSH   R1
     CALL   f_diskSize
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-1]
     PUSH   R1
     CALL   f_isErrorSet
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L1
     LOAD   R1, string_0
     FAKEIT R1, $printstr
     JUMP   L2
L1:
     LOAD   R1, string_1
     FAKEIT R1, $printstr
L2:
     LOAD   R1, [FP-1]
     FAKEIT R1, $printint
     LOAD   SP, FP
     POP    FP
     RET


f_test_Write:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 128
     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-131]
L3:
     LOAD   R1, [FP-131]
     LOAD   R2, 5
     COMP   R1, R2
     JCOND  GEQ, L4
     LOAD   R1, 255
     LOAD   R2, FP-130
     LOAD   R3, [FP-131]
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP-131]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-131]
     JUMP   L3
L4:
     PUSH   R1
     LOAD   R1, FP-130
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskWrite
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-1]
     PUSH   R1
     CALL   f_isErrorSet
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L5
     LOAD   R1, string_2
     FAKEIT R1, $printstr
     JUMP   L6
L5:
     LOAD   R1, string_3
     FAKEIT R1, $printstr
L6:
     LOAD   R1, [FP-1]
     FAKEIT R1, $printint
     LOAD   SP, FP
     POP    FP
     RET


f_test_Read:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 128
     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-131]
     PUSH   R1
     LOAD   R1, FP-130
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskRead
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-1]
     PUSH   R1
     CALL   f_isErrorSet
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     LOAD   R1, [FP-2]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L7
     LOAD   R1, string_4
     FAKEIT R1, $printstr
     JUMP   L8
L7:
     LOAD   R1, string_5
     FAKEIT R1, $printstr
L8:
     LOAD   R1, [FP-1]
     FAKEIT R1, $printint
L9:
     LOAD   R1, [FP-131]
     LOAD   R2, 5
     COMP   R1, R2
     JCOND  GEQ, L10
     LOAD   R1, FP-130
     LOAD   R2, [FP-131]
     ADD    R1, R2
     LOAD   R1, [R1]
     FAKEIT R1, $printint
     LOAD   R1, string_6
     FAKEIT R1, $printstr
     LOAD   R1, [FP-131]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-131]
     JUMP   L9
L10:
     LOAD   SP, FP
     POP    FP
     RET


main:
     PUSH   FP
     LOAD   FP, SP
     LOAD   R1, [0x00000100]
     STORE  R1, [g_MEMSTART]
     LOAD   R1, [0x00000101]
     STORE  R1, [g_MEMAVAIL]
     CALL   f_test_Size
     CALL   f_test_Write
     CALL   f_test_Read
     LOAD   SP, FP
     POP    FP
     RET



string_0: .STRING "\ndiskSize() failed\n"
string_1: .STRING "\ndiskSize() succeeded\n"
string_2: .STRING "\ndiskWrite() failed\n"
string_3: .STRING "\ndiskWrite() succeeded\n"
string_4: .STRING "\ndiskRead() failed\n"
string_5: .STRING "\ndiskRead() succeeded\n"
string_6: .STRING "\n"


  .IMPORT   f_diskSize
  .IMPORT   f_diskRead
  .IMPORT   f_diskWrite
  .IMPORT   f_isErrorSet
