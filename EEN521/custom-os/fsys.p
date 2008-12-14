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
const   block_size          = 128;

##############################################################
# readSuperBlock: read superblock from specified disk
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
    ret = call diskRead(driveNum, 1, 1, store_block);
    
    if (ret = 1) then
        ret = call memCopy(storeAddress, store_block, sizeof_superblock_s);
    
    return ret
}
        
end