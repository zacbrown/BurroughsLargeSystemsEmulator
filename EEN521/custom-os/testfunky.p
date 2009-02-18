#testfunky

import function strlen, strcpy;

function get_elem_from_path(path, store_path, elem_num) {
    local start_ind, mid_ind, end_ind;
    local cur_char, ret, break_count, forward_slash;

    break_count = 0; start_ind = 0; ret = 0;
    forward_slash = 47; #ascii value for '/'
    end_ind = call strlen(path);

    while (start_ind < end_ind) do {
        if (char start_ind of path = forward_slash) then
            break_count = (break_count + 1);
        start_ind = (start_ind + 1);
        if (elem_num = break_count) then break
    };
    if (start_ind >= end_ind) then return neg 1;

    mid_ind = start_ind;
    while (mid_ind < end_ind) do {
        if (char mid_ind of path = forward_slash) then break;
        mid_ind = (mid_ind + 1)
    };
    if (mid_ind = end_ind) then ret = 1;
    mid_ind = (mid_ind - 1);

    call strcpy(path, store_path, start_ind, mid_ind);

    return ret
}

main {
    local x, ret;
    x = "/abc/def/ghijk";
    local y:10;
    
    ret = call get_elem_from_path(x, y, 2);
    printstr "elem 2: "; printstr y;
    printstr "\nret 2: "; print ret;
    ret = call get_elem_from_path(x, y, 3);
    printstr "\nelem 3: "; printstr y;
    printstr "\nret 3: "; print ret;
    ret = call get_elem_from_path(x,y,4);
    printstr "\nelem 4: "; printstr y;
    printstr "\nret 4: "; print ret;
    printstr "\n";
    
    return
}



end
