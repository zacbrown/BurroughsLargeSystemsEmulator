g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_fsys_readSuperBlock:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 128
     SUB    SP, 1
     PUSH   R1
     CALL   f_diskRead
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-129]
     LOAD   R1, [FP-129]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  NEQ, L1
     PUSH   R1
     CALL   f_memCopy
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-129]
L1:
     LOAD   R1, [FP-129]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET



  .IMPORT   f_diskRead
  .IMPORT   f_diskWrite
  .IMPORT   f_diskSize
  .IMPORT   f_memCopy
