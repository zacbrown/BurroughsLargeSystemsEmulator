# Author: Zac Brown
# Date: 12.08.08
# Filename: disk_io.p

export function diskRead, diskWrite, diskSize;
const sizeof_rw_control_struct = 5, sizeof_size_control_struct = 3;

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
    #<| LOAD R2, <rw_control_struct>;
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
# * arg 2: number of consecutive blocks to read
# * arg 3: number of the first block to write to
# * arg 4: address of the memory location to store read data
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
    <| LOAD R1, [<driveNum>];
    <| STORE R1, [R2+1];
    <| LOAD R1, [<numBlocksToWrite>];
    <| STORE R1, [R2+2];
    <| LOAD R1, [<firstBlockNumber>];
    <| STORE R1, [R2+3];
    <| LOAD R1, [<addrWriteData>];
    <| STORE R1, [R2+4];
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
    <| LOAD R1, [<driveNum>];
    <| STORE R1, [R2+1];
    <| LOAD R1, 0;
    <| STORE R1, [R2+2];
    <| DOIO R0, <control_struct>;
    return
}

end