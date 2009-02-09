# Author: Shane Salta
# Date: 2.01.08
# Filename: bash.p

import function fsys_listDirectory, getEntryLoc;

import function getline, getchar, find_next_space;
import function memSet, strcpy, strcut, strcat;
export function bash;

const sizeof_input_str_buffer = 45;   #45 words or 180 characters
const sizeof_input_args = 10;         #10 words or 40 characters
const sizeof_pwd = 64;                #64 words or 256 characters

#getEntryLoc tells wether or not a directory or file exists ********

global pwd:sizeof_pwd;

##############################################################
# bash: bash is the shell
#
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function bash() {
    
    call init_keyboard_buffer();
    
    local input_str:sizeof_input_str_buffer, found, cont;
     
    #variable to store the command once read in
    local command:sizeof_input_args;
    
    local space, start, error, i;
    cont = 0;
    space = 0;
    pwd = "/";
    
    while (cont = 0) do
    {
        #reset the input_str to null
        i = 0;
        while (i < sizeof_input_str_buffer) do
        {
            char i of input_str = 0;
        }
        #reset command to null
        i = 0;
        while (i < sizeof_input_args) do
        {
            char i of command = 0;
        }
        
        printstr "$>";
        #reads in until /n in the keyboard buffer
        call getline(input_str, sizeof_input_str_buffer);
        
        #find the next space after the last space found
        start = space;
        space = call find_next_space(input_str, sizeof_input_str_buffer, (space +1));
        
        call strcpy(input_str, command, start, space);
        printstr "\n";
        
        if ( (strcmp(command, "ls", 2)) = 0 ) then
        {
            error = call ls( input_str );
        }
        else if ( (strcmp(command, "rm", 2)) = 0 ) then
        {
            error = call rm( input_str );
        }
        else if ( (strcmp(command, "mkdir", 5)) = 0 ) then
        {
            error = call mkdir( input_str );
        }
        else if ( (strcmp(command, "mkfile", 6)) = 0 ) then
        {
            error = call mkfile( input_str );
        }else error = -2;
        
        
        if (error = -2 ) then
        {
            printstr "ERROR: Invalid Command\n";
        } else if (error = -1 ) then
        {
            printstr "ERROR\n";
        }
                
        
        printstr"\n"
    };


    return 0;
}

##############################################################
# ls:
#
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function ls(input_str) {
    
    local space;
    
    
    if ( (char 2 of input_str) = 0) then
    {
    }else {
        space = call find_next_space(input_str, 3)
    };
    ######checks to see if position 2 is /n, then no args... else it gets
    ######args starting from pos 3
    
    fsys_listDirectory(pwd)
    
    return 0;
}

##############################################################
# rm:
#
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function rm() {
    
   

    return 0;
}

##############################################################
# mkdir:
#
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function mkdir() {
    
   

    return 0;
}

##############################################################
# mkfile:
#
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function mkfile() {
    
   

    return 0;
}


end