g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_diskRead:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 5
     LOAD R2, FP-5
     LOAD R1, $READDISC
     STORE R1, [R2]
     LOAD R1, [FP+2]
     STORE R1, [R2+1]
     LOAD R1, [FP+3]
     STORE R1, [R2+2]
     LOAD R1, [FP+4]
     STORE R1, [R2+3]
     LOAD R1, [FP+5]
     STORE R1, [R2+4]
     DOIO R0, FP-5
     LOAD   SP, FP
     POP    FP
     RET


f_diskWrite:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 5
     LOAD R2, FP-5
     LOAD R1, $WRITEDISC
     STORE R1, [R2]
     LOAD R1, [FP+2]
     STORE R1, [R2+1]
     LOAD R1, [FP+3]
     STORE R1, [R2+2]
     LOAD R1, [FP+4]
     STORE R1, [R2+3]
     LOAD R1, [FP+5]
     STORE R1, [R2+4]
     DOIO R0, FP-5
     LOAD   SP, FP
     POP    FP
     RET


f_diskSize:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 3
     LOAD R2, FP-3
     LOAD R1, $SIZEDISC
     STORE R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, FP-3
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, 0
     LOAD   R2, FP-3
     LOAD   R3, 2
     ADD    R2, R3
     STORE  R1, [R2]
     DOIO R0, FP-3
     LOAD   SP, FP
     POP    FP
     RET




  .EXPORT   f_diskRead
  .EXPORT   f_diskWrite
  .EXPORT   f_diskSize
