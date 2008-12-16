#testloops.p

main {
    local x, y;
    x = 15; y = 8947848;
    
    #<| LOAD R1, <y>;
    #<| LOAD R2, 0x8888;
    #<| LOADH R2, 0x0088;
    #<| STORE R2, [R1];
    printstr "x: ";
    print x;
    while (x < y) do {
        x = (x + 1)
    };
    
    printstr "\nx: ";
    print x;
    printstr "\ny: ";
    print y;
    
    return
}

end
