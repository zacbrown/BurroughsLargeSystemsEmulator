# Author: Zac Brown
# Date: 12.13.08
# Filename: fsys.p

####################################################
# format - 'item : size'
#
# superblock_s          : 8
#   disk id             : 1
#   disk name           : 5
#   num writable blocks : 1
#   root dir address    : 1
#
# file_ent_s            : 8
#   file_name           : 6
#   is directory        : 1
#   first inode         : 1
#
# inode_s               : 32
#   type                : 1 (EOF:-1, FREE:0, USED:1)
#   next cluster        : 1
#   data                : 30
#
####################################################

import function diskRead, diskWrite, diskSize, memCopy;
export function fsys_readSuperBlock, fsys_writeSuperBlock, fsys_listDirectory,
                fsys_createFile, fsys_createDirectory, fsys_removeDirectory,
                fsys_removeFile, fsys_fileAppend, fsys_fileOpen, fsys_fileClose;

# struct sizes
const   sizeof_superblock_s = 8,
        sizeof_file_entry_s = 8,
        sizeof_inode_s      = 32;

# superblock_s offsets        
const   disk_id             = 0,
        disk_name           = 1,
        num_writable_blocks = 6,
        root_dir_addr       = 7;
        
# file_ent_s offsets
const   file_name           = 0,
        is_directory        = 6,
        first_inode         = 7;
        
# inode_s offsets
const   type                = 0,
        next_cluster        = 1,
        data                = 2;

# inode types
const   INODE_EOF           = -1,
        INODE_FREE          = 0,
        USED                = 1;

# various consts
const   block_size          = 128,
        first_block         = 0,
        blk_num             = 0,
        blk_offset          = 1,
        disk_number         = 1,
        inode_data_size     = 30;
        
const max_files_open = 15;
global file_descriptor_array:max_files_open;
global process_control_block:3;
global superblock_struct:sizeof_superblock_s;

##############################################################
# fsys_readSuperBlock: read superblock from specified disk
# * arg 1: disk drive number
# * arg 2: pointer to place to store superblock
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - 0
##############################################################

function fsys_readSuperBlock(driveNum, storeAddress) {
    local store_block:block_size, ret;
    ret = call diskRead(driveNum, 1, first_block, store_block);
    
    if (ret = 1) then
        ret = call memCopy(storeAddress, store_block, sizeof_superblock_s);
    
    return ret
}

##############################################################
# fsys_writeSuperBlock: write superblock to specified disk
# * arg 1: disk drive number
# * arg 2: pointer to place to copy superblock from
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - 0
##############################################################

function fsys_writeSuperBlock(driveNum, superBlockStruct) {
    local store_block:block_size, ret;

    call memCopy(store_block, superBlockStruct, sizeof_superblock_s);
    ret = call diskWrite(driveNum, 1, first_block, store_block);

    return ret
}

##############################################################
# fsys_listDirectory: list files in specified path
# * arg 1: disk drive number
# * arg 2: path to list files in
# * returns: 
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - 0
##############################################################
function fsys_listDirectory(driveNum, directoryPath) {
    return
}

function getEntryLoc(path) {
    local cur_inode, cur_filent_ind, block_offset_s:2, block_read_storage;
    local ret, block_to_read, done, start_ind, end_ind, cur_ptr;
    
    ret = call strlen(path);
    if (ret = 1) then {
        ret = call strcmp(path, "/", 1);
        if (ret = 0) then return *(superblock_struct + root_dir_addr)
    };
    
    # allocate memory for block being read 
    block_read_storage = call malloc(128, process_control_block);
    
    done = 0;
    cur_filent_ind = *(superblock_struct + root_dir_addr);
    while (done = 0) do {
        # get directory/file entry address
        call transAddrToBlockOffset(cur_filent_ind, block_offset_s);
        block_to_read = *(block_offset_s + blk_num);
        ret = call diskRead(disk_number, 1, block_to_read, block_read_storage);
    
        # get inode entry address
        cur_filent_ind = *(block_read_storage + *(block_offset_s + blk_offset));
        call transAddrToBlockOffset(cur_filent_ind, block_offset_s);
        block_to_read = *(block_offset_s + blk_num);
        ret = call diskRead(disk_number, 1, block_to_read, block_read_storage);
    
        cur_inode = *(block_read_storage + *(block_offset_s + blk_offset));
        cur_inode = *(cur_filent_ind + first_inode);
        
        while (cur_inode < (cur_inode + 30)) do {
            if (cur_inode = 
            
        }
    };
    
    return
}

function get_elem_from_path(path, elem_num) {
    return
}

function fsys_createFile(driveNum, fileName, destDirectoryName) {
    return
}

function fsys_createDirectory(driveNum, directoryName, destDirectoryName) {
    return
}

function fsys_removeDirectory(driveNum, directoryPath) {
    return
}

function fsys_removeFile(driveNum, filePath) {
    return
}

function fsys_fileAppend(driveNum, filePath) {
    return
}

function fsys_fileOpen(driveNum, filePath) {
    return
}

function fsys_fileClose(driveNum, fileDescriptor) {
    return
}

function transAddrToBlockOffset(addrToTrans, storeLoc) {
    local offset, block_num;
    *storeLoc = (addrToTrans / 128);
    *(storeLoc + 1) = (addrToTrans % 128);
    
    return
}

end