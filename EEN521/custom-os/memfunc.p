# Author: Zac Brown
# Date: 12.13.08
# Filename: memfunc.p

export function memCopy, memCompare;

##############################################################
# memCopy: copy specified number of words from ptr1 to ptr2
# * arg 1: pointer to destination
# * arg 2: pointer to source
# * arg 3: number of words to copy
# * returns: 
#   * failure - -1
#   * success - 0
##############################################################

function memCopy(dest, source, num_words) {
    local ind;
    ind = 0;

    while (ind < num_words) do {
        *(dest + ind) = *(source + ind);
        ind = (ind + 1)
    };
    return 0
}

##############################################################
# memCompare: compared specified number of words from ptr1 to ptr2
# * arg 1: pointer to memory location 1
# * arg 2: pointer to memory location 2
# * arg 3: number of words to compare
# * returns: 
#   * failure - -1
#   * success - 0
##############################################################

function memCompare(memA, memB, num_words) {
    local ind;
    ind = 0;

    while (ind < num_words) do {
        if (*(memA + ind) < *(memB + ind)) then return neg 1;
        if (*(memB + ind) > *(memB + ind)) then return 1;
        ind = (ind + 1)
    };
    
    return 0
}

end
