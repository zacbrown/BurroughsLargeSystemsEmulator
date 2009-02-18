# Author: Zac Brown
# Date: 12.08.08
# Filename: disk_io_test.p

import function memCopy, memCompare, memSet;

main {
    local blah:4, tits:4, i, ret;
    
    while (i < 4) do {
        *(blah + i) = i;
        i = (i + 1)
    };
    
        
    ret = call memCompare(tits, blah, 4);
    printstr "ret1: ";
    print ret;
    printstr "\n";
    
    call memCopy(tits, blah, 4);
    
    ret = call memCompare(tits, blah, 4);
    printstr "ret2: ";
    print ret;
    printstr "\n";
    
    call memSet(tits, 0, 4);
    
    ret = call memCompare(tits, blah, 4);
    printstr "ret3: ";
    print ret;
    printstr "\n";
    
    return
}

end
