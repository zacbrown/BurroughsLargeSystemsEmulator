# Author: Shane Salta
# Date: 12.14.08
# Filename: userinput.p

import function memSet, strcpy, strcut, strcat;
export function init_keyboard_buffer, keyboard_interrupt, getline, getchar;
export global system_input_buffer;
const sizeof_term_control_struct = 3, sizeof_read_buffer = 10;
const sizeof_system_input_buffer = 45;   #45 words or 180 characters

global system_input_buffer:sizeof_system_input_buffer;
global system_input_buffer_num;

##############################################################
# init_keyboard_buffer: call when booting up, initializes variables
#  * returns: 0
##############################################################
function init_keyboard_buffer() {
    system_input_buffer_num = 0;
    call memSet(system_input_buffer, 0, sizeof_system_input_buffer);

    return 0
}

##############################################################
# keyboard_interrupt: reads in a specified number of words from keyboard buffer
#
#
#
#
#
#
#
#
#
##############################################################
function keyboard_interrupt( ) {
     local control_struct:sizeof_term_control_struct;
     local buff:sizeof_read_buffer;
     local readin;
     readin = -1;
 
#MUST Turn off interrupts here
     while (readin != 0) do 
     {
         call memSet(buff, 0, sizeof_read_buffer);
         <| LOAD R2, <control_struct>;
         <| LOAD R1, $TERMINC;
         <| STORE R1, [R2];
         *(control_struct + 1) = sizeof_read_buffer;
         *(control_struct + 2) = buff;
         <| DOIO R1, <control_struct>;
         <| STORE R1, [<readin>];
         if (readin < 0) then
         {
             printstr "ERROR: Reading in from keyboard: ";
             print readin;
             <| HALT
         };
         
         if (readin != 0) then
         {
             printstr(buff, readin);
             if ((system_input_buffer_num + readin) >= 45) then
                 printstr "ERROR: Keyboard input buffer overflow \n"
             else {
                 call strcat(buff, system_input_buffer, 0, readin, system_input_buffer_num);
                 system_input_buffer_num = (system_input_buffer_num + readin)
             }
         }
      };
      return
      #ireturn; #will be added by zac
}
##############################################################
# getline: copies over keyboard_input_buffer to string passed through
#       IF sizeof_str is not big enough, it will only take sizeof_str
#       out of the keyboard buffer. User must call getline again to get rest
# * arg 1: string that buffer can be saved to
# * arg 2: size of input_str
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function getline(input_str, sizeof_str) {
    local i, found;
    i = 0;
    found = 0;
    
    #this loop keeps running until it finds \n in the buffer
    while (found = 0) do
    {
        if (i < system_input_buffer_num) then 
        {
            #if ((char i of system_input_buffer) = 13) then
            #{   #found end of line  /n
                cont = 0
            #    found = 1
            #}else if ((char i of system_input_buffer) = 10) then
            #{   #found end of line  /n
            #    cont = 0;
            #    found = 1
            #}else i = (i + 1)
        }
    };
    if ( i < sizeof_str) then
    {
        call strcut (system_input_buffer, input_str, 0, i, 45)
    }else {
        call strcut (system_input_buffer, input_str, 0, sizeof_str, 45)
    };
    
    return 0
}

##############################################################
# getchar: copies over character from keyboard_input_buffer
# * arg 1: address to variable to save char
# * returns: 
#   * failure - 0
#   * success - 0
#
##############################################################
function getchar(char_var_ptr) {
    local done;
    done = 0;
    while (done = 0) do {
        if (system_input_buffer_num <= 0) then done = 1
    };
    call strcut (system_input_buffer, char_var_ptr, 0, 1, 45);
    system_input_buffer_num = (system_input_buffer_num - 1);
    return 0
}

end
