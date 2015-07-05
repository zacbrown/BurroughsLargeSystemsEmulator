
     JUMP main

g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1
g_test_proc_ctrl_block:      .space     3

main:
     PUSH   FP
     LOAD   FP, SP
     SUB    SP, 5
     LOAD   R1, [0x00000100]
     STORE  R1, [g_MEMSTART]
     LOAD   R1, [0x00000101]
     STORE  R1, [g_MEMAVAIL]
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 1
     LOAD   R2, g_test_proc_ctrl_block
     STORE  R1, [R2]
     PUSH   R1
     LOAD   R1, 20
     PUSH   R1
     CALL   f_rmalloc
     POP    R1
     STORE  R0, R1
     LOAD   R2, g_test_proc_ctrl_block
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, 20
     LOAD   R2, g_test_proc_ctrl_block
     LOAD   R3, 2
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, g_test_proc_ctrl_block
     LOAD   R2, 1
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-1]
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_rmalloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-2]
     PUSH   R1
     LOAD   R1, g_test_proc_ctrl_block
     PUSH   R1
     LOAD   R1, 3
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-3]
     LOAD   R1, 3
     LOAD   R2, [FP-3]
     STORE  R1, [R2]
     LOAD   R1, 6
     LOAD   R2, [FP-3]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, 9
     LOAD   R2, [FP-3]
     LOAD   R3, 2
     ADD    R2, R3
     STORE  R1, [R2]
     BREAK
     LOAD   R1, g_test_proc_ctrl_block
     PUSH   R1
     LOAD   R1, 3
     PUSH   R1
     LOAD   R1, [FP-3]
     PUSH   R1
     CALL   f_free
     BREAK
     LOAD   R1, 20
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_rmfree
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-2]
     PUSH   R1
     CALL   f_rmfree
     LOAD   SP, FP
     POP    FP
     RET





  .IMPORT   f_rmalloc
  .IMPORT   f_rmfree
  .IMPORT   f_malloc
  .IMPORT   f_free
  .IMPORT   f_memset
