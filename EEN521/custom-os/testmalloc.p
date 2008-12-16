# asdlfasdfasdf

global test_proc_ctrl_block:3;

import function rmalloc, rmfree, malloc, free;
import function memset;

main {
    local addr, more_addr, new_addr, other_addr, itr;
    *test_proc_ctrl_block = 1;
    *(test_proc_ctrl_block + 1) = call rmalloc(20);
    *(test_proc_ctrl_block + 2) = 20;
    addr = *(test_proc_ctrl_block + 1);
    
    #call memset(addr, 1, 20); 
    #itr = 0;
    #while (itr < 10) do {
    #    print *(addr + itr);
    #    itr = (itr + 1)
    #};

    more_addr = call rmalloc(128);
    new_addr = call malloc(3, test_proc_ctrl_block);
    *new_addr = 3;
    *(new_addr + 1) = 6;
    *(new_addr + 2) = 9;
    <| BREAK;
    call free(new_addr, 3, test_proc_ctrl_block);
    
    <| BREAK;
    call rmfree(addr, 20);
    call rmfree(more_addr, 128);
    return
}

end
