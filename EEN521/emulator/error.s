g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1

f_isErrorSet:
     PUSH   FP
     LOAD   FP, SP

     GETFL R0, $ERR
     LOAD   SP, FP
     POP    FP
     RET




  .EXPORT   f_isErrorSet
