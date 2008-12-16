# Author: Zac Brown
# Date: 12.13.08
# Filename: memfunc.p

export function memcpy, memcmp, memset;

##############################################################
# memcpy: copy specified number of words from ptr1 to ptr2
# * arg 1: pointer to destination
# * arg 2: pointer to source
# * arg 3: number of words to copy
# * returns: 
#   * failure - -1
#   * success - 0
##############################################################

function memcpy(dest, source, num_words) {
    local ind;
    ind = 0;

    while (ind < num_words) do {
        *(dest + ind) = *(source + ind);
        ind = (ind + 1)
    };
    return 0
}

##############################################################
# memcmp: compared specified number of words from ptr1 to ptr2
# * arg 1: pointer to memory location 1
# * arg 2: pointer to memory location 2
# * arg 3: number of words to compare
# * returns: 
#   * failure - -1
#   * success - 0
##############################################################

function memcmp(memA, memB, num_words) {
    local ind;
    ind = 0;

    while (ind < num_words) do {
        if (*(memA + ind) < *(memB + ind)) then return neg 1;
        if (*(memB + ind) > *(memB + ind)) then return 1;
        ind = (ind + 1)
    };
    
    return 0
}

##############################################################
# memset: set specified memory location up to *range* to value
# * arg 1: pointer to memory location
# * arg 2: value to set memory to
# * arg 3: number of words to set
# * returns: 
#   * failure - -1
#   * success - 0
##############################################################

function memset(memLoc, set_value, num_words) {
    local ind, end_ind;
    ind = 0; end_ind = (num_words - 1);
    
    while (ind < end_ind) do {
        *(memLoc + ind) = set_value;
        ind = (ind + 1)
    };
    return 0
}
end
