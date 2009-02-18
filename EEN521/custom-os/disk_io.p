# Author: Zac Brown
# Date: 12.08.08
# Filename: disk_io.p

export function diskRead, diskWrite, diskSize;
const   sizeof_rw_control_struct = 5, sizeof_size_control_struct = 3,
        sizeof_magtape_control_struct = 3;

##############################################################
# diskRead: read a specified number of blocks from spec. disk
# * arg 1: disk drive number
# * arg 2: number of consecutive blocks to read
# * arg 3: number of the first block to read
# * arg 4: address of the memory location to store read data
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - the number of blocks read from disk to memory
##############################################################
function diskRead(driveNum, numBlocksToRead, firstBlockNumber, addrStoreData) {
    local control_struct:sizeof_rw_control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $READDISC;
    <| STORE R1, [R2];
    <| LOAD R1, [<driveNum>];
    <| STORE R1, [R2+1];
    <| LOAD R1, [<numBlocksToRead>];
    <| STORE R1, [R2+2];
    <| LOAD R1, [<firstBlockNumber>];
    <| STORE R1, [R2+3];
    <| LOAD R1, [<addrStoreData>];
    <| STORE R1, [R2+4];
    <| DOIO R0, <control_struct>;
    return
}

##############################################################
# diskWrite: write specified mem address to disk
# * arg 1: disk drive number
# * arg 2: number of consecutive blocks to write
# * arg 3: number of the first block to write to
# * arg 4: address of the memory location to write data from
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - the number of blocks read from disk to memory
##############################################################
function diskWrite(driveNum, numBlocksToWrite, firstBlockNumber, addrWriteData) {
    local control_struct:sizeof_rw_control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $WRITEDISC;
    <| STORE R1, [R2];
    *(control_struct + 1) = driveNum;
    *(control_struct + 2) = numBlocksToWrite;
    *(control_struct + 3) = firstBlockNumber;
    *(control_struct + 4) = addrWriteData;
    <| DOIO R0, <control_struct>;
    return
}

##############################################################
# diskSize: get the disk size 
# * arg 1: disk drive number
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#   * success - the size of the disk
##############################################################
function diskSize(driveNum) {
    local control_struct:sizeof_size_control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $SIZEDISC;
    <| STORE R1, [R2];
    *(control_struct + 1) = driveNum;
    *(control_struct + 2) = 0;
    <| DOIO R0, <control_struct>;
    return
}

##############################################################
# mtLoad: load a file on magnetic tape drive
# * arg 1: read filename on outside operating system to load
# * arg 2: (0) or write (1)
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -5 : could not open real file
#   * success - 1
##############################################################
function mtLoad(filename, rw) {
    local control_struct:sizeof_magtape_control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $MTLOAD;
    <| STORE R1, [R2];
    *(control_struct + 1) = rw;
    *(control_struct + 2) = filename;
    <| DOIO R0, <control_struct>;
    return
}

##############################################################
# mtUnload: unload a file from magnetic tape drive
# * no arguments
# * returns: 
#   * failure - error code if one occured
#       * -3 : invalid request, no tape loaded
#   * success - 1
##############################################################
function mtUnload() {
    local control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $MTUNLOAD;
    <| STORE R1, [R2];
    <| DOIO R0, <control_struct>;
    return
}

##############################################################
# mtRead: read file loaded on magnetic tape drive
# * arg 1: number of blocks to read
# * arg 2: memory location where data should be stored
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem reading/writing of control struct
#       * -3 : invalid request, no tape loaded
#       * -5 : error in accessing real file
#   * success - number of blocks transferred to memory
##############################################################
function mtRead(num_blocks, mem_loc) {
    local control_struct:sizeof_magtape_control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $MTREAD;
    <| STORE R1, [R2];
    *(control_struct + 1) = num_blocks;
    *(control_struct + 2) = mem_loc;
    <| DOIO R0, <control_struct>;
    return
}

##############################################################
# mtWrite: write file loaded on magnetic tape drive
# * arg 1: number of blocks to write
# * arg 2: memory location where data to write can be found
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem reading/writing of control struct
#       * -3 : invalid request, no tape loaded
#       * -5 : error in writing real file
#   * success - number of blocks transferred to tape/file
##############################################################
function mtWrite(num_blocks, mem_loc) {
    local control_struct:sizeof_magtape_control_struct;
    <| LOAD R2, <control_struct>;
    <| LOAD R1, $MTWRITE;
    <| STORE R1, [R2];
    *(control_struct + 1) = num_blocks;
    *(control_struct + 2) = mem_loc;
    <| DOIO R0, <control_struct>;
    return
}

end