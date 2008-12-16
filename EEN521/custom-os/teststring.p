#teststring.p

import function strlen, strrchr, strchr, malloc, rmalloc, rmfree, free;
import function strcpy, strlen_word, strcmp;
global process_control_block:3;

main {
    local x, y, len, len_words;
    
    *process_control_block = 1;
    *(process_control_block + 1) = call rmalloc(128);
    *(process_control_block + 2) = 128;
    
    
    x = "poopy";
    printstr "string before: "; printstr x;
    printstr "\n";
    y = char 1 of x;
    printchar y;
    char 2 of x = "z";
    printstr "\nstring after: "; printstr x;
    
    y = call strlen(x);
    printstr "\nstrlen(x): "; print y;
    printstr "\n";
    
    y = call strchr(x, "y");
    printstr "\nfirst ind of y: "; print y;
    y = call strrchr(x, "o");
    printstr "\nlast ind of o: "; print y;
    printstr "\n";
    
    len_words = call strlen_word(x);
    y = call malloc(len_words, process_control_block);
    len = call strlen(x);
    call strcpy(x, y, 0, len);
    printstr "len: "; print len;
    printstr "y now: "; printstr y;
    printstr "\n";
    char 4 of y = 0;
    printstr "y now: "; printstr y;
    printstr "\n";
    
    x = "poopy";
    len = call strcmp(x, "paap", 4);
    printstr "strcmp ret: "; print len; 
    return
}

end
