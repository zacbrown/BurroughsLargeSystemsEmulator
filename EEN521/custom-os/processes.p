# Author: James McNaul
# Date: 12/12/09
# Filename: processes.p


import fsys_init, shell_init;
export process_init, process_array, processcount;

const process_array_initial_size = 8;

global processcount, currentprocess;
processcount = 0;
currentprocess = 0;

global process_array:process_array_size;

##############################################################################
# process_init: Initializes a process, stores the process in
# the array of current processes
# * arg 1: Base Address of memory where process resides
# * returns:
#   * failure - -1
#   * success - 1
##############################################################################
function process_init(baseaddress){
    if(processcount > 7) then {
	return neg 1;
    }
    
    process_array+processcount = baseaddress;
    
    return 1
}

##############################################################################
# timer_interupt: Prints the interupt handler in assembly
# simply using this function to abstract the code.
#
##############################################################################
function timer_interupt(){
    <| TIMHANDLER:;
    <| LOAD R1, 25
    <| SETSR R1, $TIMER
    <| IRET
}

##############################################################################
# main: this function initializes the file system and shell.
# Therefore the shell is the first process in the array at
# index zero. It will also call the timer_interupt() in order
# to print the interupt handler as assembly.
##############################################################################

main {
    call fsys_init(1024);
    call shell_init();

    
// implement timer and timer interupt.
    call timer_interupt();

    <| LOAD R1, TIMHANDLER;
    <| STORE R1, [IVEC+IV$TIMER];
    <| LOAD R1, IVEC;
    <| SETSR R1,$INTVEC;
    <| LOAD R1, 0;
    <| SETFL R1, $IP;
    <| LOAD R1, [<process_array>];
    <| JUMP R1;

    if(processcount>0){
    
}

end