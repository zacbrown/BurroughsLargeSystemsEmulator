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
