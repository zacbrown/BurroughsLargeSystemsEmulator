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
#   file_name           : 5
#   flag                : 1
#       FREE - 0
#       USED - 1
#   is directory        : 1
#   first inode         : 1
#
# inode_s               : 32
#   type                : 1 (EOF:-1, FREE:0, USED:1)
#   next cluster        : 1
#   data                : 30
#
####################################################

import function diskRead, diskWrite, diskSize, memcpy, strcmp, strcpy;
import function strcut, strcat, strchr, strrchr, strlen_word;
export function fsys_readSuperBlock, fsys_writeSuperBlock, fsys_listDirectory,
                fsys_createFile, fsys_createDirectory, fsys_removeDirectory,
                fsys_removeFile, fsys_fileAppend, fsys_fileOpen, fsys_fileClose;

# struct sizes
const   sizeof_superblock_s = 8,
        sizeof_filent_s     = 8,
        sizeof_inode_s      = 32;

# superblock_s offsets
const   off_disk_id         = 0,
        off_disk_name       = 1,
        off_num_w_blocks    = 6,
        off_root_dir_addr   = 7;

# disk offsets in blocks
# representative of the *first* block to use
const   off_superblock      = 0,
        off_filent          = 1,
        off_inode           = 101; 
        
const   off_superblock_word = 0,
        off_filent_word     = 128,
        off_inode_word      = 12928;
        
# file_ent_s offsets
const   off_file_name       = 0,
        off_file_ent_flag   = 5,
        off_is_directory    = 6,
        off_first_inode     = 7;

# file_ent_s flags
const   FILE_ENT_FREE       = 0,
        FILE_ENT_USED       = 1;

# inode_s offsets
const   off_inode_type      = 0,
        off_inode_next_clust= 1,
        off_inode_data      = 2;

# inode types
const   INODE_EOF           = -1,
        INODE_FREE          = 0,
        INODE_USED          = 1;

# various consts
const   block_size          = 128,
        first_block         = 0,
        blk_num             = 0,
        blk_offset          = 1,
        disk_number         = 1,
        inode_data_size     = 30,
        max_filename_word   = 6,
        max_filename_len    = 24;

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
        ret = call memcpy(storeAddress, store_block, sizeof_superblock_s);

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
# * arg 1: path to list files in
# * returns:
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - 0
##############################################################
function fsys_listDirectory(directoryPath) {
    local dir_entry_loc, done, ind_ptr, inode_block_storage, ret;
    local filent_block_storage, block_offset_s:2;
    dir_entry_loc = call getEntryLoc(directoryPath);
    
    if (dir_entry_loc = neg 1) then { 
        printstr "ERROR: no such directory\n";
        return
    };
    
    # initialize storage area
    inode_block_storage = call malloc(128, process_control_block);
    filent_block_storage = call malloc(128, process_control_block);
    
    ret = call getBlockByAddress(dir_entry_loc, inode_block_storage);
    if (ret != 1) then {
        printstr "ERROR: unable to read block, possible memory error";
        call free(filent_block_storage, 128, process_control_block);
        call free(inode_block_storage, 128, process_control_block);
        return
    };
    
    printstr "\n"; printstr directoryPath;
    ind_ptr = (inode_block_storage + off_inode_data);
    done = 0;
    while (done = 0) do {
    # non-zero inodes are considered places to check
    # removed files/directories have inode loc zeroed
        if (*ind_ptr > 0) then {
            if (*ind_ptr < off_inode_word) then {
                ret = call getBlockByAddress(*ind_ptr, filent_block_storage, block_offset_s);
                if (ret != 1) then {
                    call free(filent_block_storage, 128, process_control_block);
                    call free(inode_block_storage, 128, process_control_block);
                    printstr "ERROR: unable to read file entry\n";
                    return
                };
                printstr "\n\t"; printstr *(filent_block_storage + off_file_name);
                printstr "\n"
            }
        };
        ind_ptr = (ind_ptr + 1) # move to next position
    };
    
    return
}

function getEntryLoc(path) {
    local cur_inode, cur_filent_ind, block_offset_s:2, inode_read_storage,
          filent_read_storage, cur_elem_str, cur_elem_index, cur_elem_len,
          last_elem;
    local ret, block_to_read, done, start_ind, end_ind, cur_data_ptr, cur_filename;

    ret = call strlen(path);
    if (ret = 1) then {
        ret = call strcmp(path, "/", 1);
        if (ret = 0) then return *(superblock_struct + off_root_dir_addr)
    };

    # max_filename_word is specified in words, not characters!
    cur_elem_str = call malloc(max_filename_word, process_control_block);

    # allocate memory for blocks being read
    inode_read_storage = call malloc(128, process_control_block);
    filent_read_storage = call malloc(128, process_control_block);

    done = 0; cur_elem_index = 1;
    cur_filent_ind = *(superblock_struct + off_root_dir_addr);
    while (done = 0) do {
        last_elem = call get_elem_from_path(path, cur_elem_str, cur_elem_index);
        if (last_elem = neg 1) then break; # should this just be a return?
        
        # get directory/file entry address
        ret = call getBlockByAddress(cur_filent_ind, filent_read_storage, block_offset_s);
        if (ret != 1) then {
            call free(inode_read_storage, 128, process_control_block);
            call free(filent_read_storage, 128, process_control_block);
            call free(cur_elem_str, max_filename_word, process_control_block);
            return neg 1
        };

        # get inode entry address
        cur_filent_ind = *(filent_read_storage + *(block_offset_s + blk_offset));
        ret = call getBlockByAddress(cur_filent_ind, inode_read_storage, block_offset_s);
        if (ret != 1) then {
            call free(inode_read_storage, 128, process_control_block);
            call free(filent_read_storage, 128, process_control_block);
            call free(cur_elem_str, max_filename_word, process_control_block);
            return neg 1
        };

        cur_inode = *(inode_read_storage + *(block_offset_s + blk_offset));
        cur_data_ptr = *(cur_filent_ind + off_first_inode);

        if (cur_inode != INODE_USED) then return neg 1;
        while (cur_data_ptr < (cur_inode + 30)) do {
            if (cur_data_ptr > 0) then {
                ret = call getBlockByAddress(cur_data_ptr, filent_read_storage, block_offset_s);
                if (ret != 1) then {
                    call free(inode_read_storage, 128, process_control_block);
                    call free(filent_read_storage, 128, process_control_block);
                    call free(cur_elem_str, max_filename_word, process_control_block);
                    return neg 1
                };

                cur_filename = *(filent_read_storage + off_file_name);

                ret = call strcmp(cur_elem_str, cur_filename, max_filename_len);
                if (ret = 0) then {
                    if (last_elem = 0) then {
                        if (*(filent_read_storage + off_is_directory) = 1) then
                            cur_filent_ind = cur_data_ptr
                    }
                    else if (last_elem = 1) then { # we found it
                        call free(inode_read_storage, 128, process_control_block);
                        call free(filent_read_storage, 128, process_control_block);
                        call free(cur_elem_str, max_filename_word, process_control_block);
                        return cur_data_ptr
                    }
                }
            }
        }
    };
    call free(inode_read_storage, 128, process_control_block);
    call free(filent_read_storage, 128, process_control_block);
    call free(cur_elem_str, max_filename_word, process_control_block);
    
    return neg 1
}

function getBlockByAddress(address, store_addr, block_offset_s) {
    local ret, block_to_read;

    call transAddrToBlockOffset(address, block_offset_s);
    block_to_read = *(block_offset_s + blk_num);
    ret = call diskRead(disk_number, 1, block_to_read, store_addr);

    return ret
}

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

# return -1 if no free file entry is found
function get_next_free_file_ent() {
    local block_ind, filent_block_storage, ret, block_offset_s:2, filent_ind;
    
    # initialize storage area
    filent_block_storage = call malloc(128, process_control_block);
    
    block_ind = off_filent; 
    while (block_ind < off_inode) do {
        ret = call getBlockByAddress(*block_ind, filent_block_storage, block_offset_s);
        if (ret != 0) then {
            call free(filent_block_storage, 128, process_control_block);
            return
        };
        
        # run through and figure out the first free file entry
        filent_ind = 0;
        while (filent_ind < 128) do {
            if (*(filent_ind + off_file_ent_flag) = FILE_ENT_FREE) then {
                ret = (((block_ind - 1) * 128) + filent_ind);
                call free(filent_block_storage, 128, process_control_block);
                return ret
            };
            
            filent_ind = (filent_ind + sizeof_filent_s)
        };
        
        block_ind = (block_ind + 1)
    };
    
    call free(filent_block_storage, 128, process_control_block);
    return neg 1
}

# returns -1 if no free inode exists
function get_next_free_inode() {
    local block_ind, inode_block_storage, ret, block_offset_s:2, inode_ind;
    local disk_size;
    
    # initialize storage area
    inode_block_storage = call malloc(128, process_control_block);
    
    block_ind = off_inode;
    disk_size = call diskSize(1);
    
    while (block_ind < disk_size) do {
        ret = call getBlockByAddress(*block_ind, inode_block_storage, block_offset_s);
        if (ret != 0) then {
            call free(inode_block_storage, 128, process_control_block);
            return
        };
        
        # run through and figure out the first free inode
        inode_ind = 0;
        while (inode_ind < 128) do {
            if (*(inode_ind + off_inode_type) = INODE_FREE) then {
                ret = (((block_ind - 1) * 128) + inode_ind);
                call free(inode_block_storage, 128, process_control_block);
                return ret
            };
            
            inode_ind = (inode_ind + sizeof_inode_s)
        };
        
        block_ind = (block_ind + 1)
    };
    
    call free(inode_block_storage, 128, process_control_block);
    return neg 1
}

##############################################################
# fsys_createFile: list file in specified directory
# * arg 1: name to assign new file
# * arg 2: directory to create file in
# * returns:
#   * failure - error code if one occured
#       * -2 : memory problem, reading or writing
#       * -3 : invalid disk drive number
#       * -4 : invalid block number
#   * success - address of 
# * notes: creates a file entry and allocates a single inode
##############################################################
function fsys_createFile(fileName, destDirectoryName) {
    return
}

function fsys_createDirectory(directoryName, destDirectoryName) {
    return
}

function fsys_removeDirectory(directoryPath) {
    return
}

function fsys_removeFile(filePath) {
    return
}

function fsys_fileAppend(filePath) {
    return
}

function fsys_fileOpen(filePath) {
    return
}

function fsys_fileClose(fileDescriptor) {
    return
}

function transAddrToBlockOffset(addrToTrans, storeLoc) {
    local offset, block_num;
    *storeLoc = (addrToTrans / 128);
    *(storeLoc + 1) = (addrToTrans % 128);

    return
}

end