g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_setInterrupt:
     PUSH   FP
     LOAD   FP, SP

     LOAD R1, FP+2
     SETFL R1, $IP
     LOAD   SP, FP
     POP    FP
     RET


f_getInterrupt:
     PUSH   FP
     LOAD   FP, SP

     GETFL R0, $IP
     LOAD   SP, FP
     POP    FP
     RET


f_setError:
     PUSH   FP
     LOAD   FP, SP

     LOAD R1, FP+2
     SETFL R1, $IP
     LOAD   SP, FP
     POP    FP
     RET


f_getError:
     PUSH   FP
     LOAD   FP, SP

     GETFL R0, $ERR
     LOAD   SP, FP
     POP    FP
     RET




  .EXPORT   f_setError
  .EXPORT   f_setInterrupt
  .EXPORT   f_getInterrupt
  .EXPORT   f_getError
