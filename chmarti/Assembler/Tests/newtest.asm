;This program calculates the factorial of 6
        LOAD    R1, 6
        LOAD    R2, 1
        LOAD    R3, 1 ;I'm testing comments
top:    CMP     R2, R1
        JMPG    bot    
        MUL     R3, R2 ;do they work here too?
        ADD     R2, 1
        JMP     top
bot:    STORE   R3, 1333
        HALT
