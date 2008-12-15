# Author: Zac Brown
# Date: 12.14.08
# Filename: malloc.p

const   HEAP_BASE_ADDR  = 257,
        HEAP_MEM_MAX    = 256, 
        F_RM_MEM_FREE   = 32767,
        F_RM_MEM_USED   = -32767,
        F_PM_MEM_FREE   = 30591,
        F_PM_MEM_USED   = -30591;

# struct sizes
const   sizeof_process_ctrl_block = 3;

# struct offsets
const   process_id = 0,
        base_addr = 1,
        alloc_size = 2;

export function rmalloc, rmfree, malloc, free;
import function memSet;
        
##############################################################
# rmalloc: used to allocate heap space for a process
# * arg 1: size in words to allocate for process
# * returns: 
#   * failure - -1
#   * success - address of start of heap space
##############################################################
function rmalloc(size) {
    local ind_ptr, poss_len, ret, done, end_ptr;
    ind_ptr = *HEAP_BASE_ADDR;
    done = 0;
    
    if (*ind_ptr != F_RM_MEM_FREE) then {
        if (*ind_ptr != F_RM_MEM_USED) then {
            *ind_ptr = F_RM_MEM_FREE;
            *(ind_ptr + 1) = (*HEAP_MEM_MAX - 1) 
        }
    };
    
    while (done = 0) do {
    
        while (*ind_ptr != F_RM_MEM_FREE) do {
            ind_ptr = (ind_ptr + 1)
        };
        poss_len = *(ind_ptr + 1);
        end_ptr = ((ind_ptr + poss_len) + 2);

        # is there enough space?
        if (poss_len >= size) then done = 1
    };
    
    *ind_ptr = F_RM_MEM_USED;
    *(ind_ptr + 1) = size;
    end_ptr = ((ind_ptr + 2) + size);
    printstr "end_ptr: ";
    print end_ptr;
    *end_ptr = F_RM_MEM_USED;
    *(end_ptr - 1) = size;

    if (size < poss_len) then {
        *(end_ptr + 1) = F_RM_MEM_FREE;
        *(end_ptr + 2) = (poss_len - size);
        end_ptr = ((ind_ptr + poss_len) + 1);
        *end_ptr = (poss_len - size)
    };
    
    return (ind_ptr + 2)
}

##############################################################
# rmfree: used to free heap space for a process
# * arg 1: base address to free from
# * arg 2: size in words to free, must be same as called with rmalloc
# * returns: 
#   * failure - error code in one of following occured:
#       * -1 : invalid base address
#       * -2 : invalid size
#   * success - address of start of heap space
##############################################################
function rmfree(base, size) {
    local ind_ptr, end_ptr, merge_ptr, new_size, tmp_size, tmp_ptr;

    ind_ptr = (base - 2); # special markers start at base - 2
    if (*ind_ptr != F_RM_MEM_USED) then return neg 1;
    *ind_ptr = F_RM_MEM_FREE;

    # make sure that we got the right size passed in
    end_ptr = ((ind_ptr + 3) + size);
    if (*end_ptr != size) then return neg 2;
    
    # check the end word to make sure its marked properly
    end_ptr = (end_ptr + 1);
    if (*end_ptr != F_RM_MEM_USED) then return neg 1;
    *end_ptr = F_RM_MEM_FREE;
        
    # see if we can merge ourself to the block before
    if (ind_ptr != *HEAP_BASE_ADDR) then {
        merge_ptr = (ind_ptr - 1);
        if (*merge_ptr = F_RM_MEM_FREE) then {
            tmp_size = *(merge_ptr - 1);
            new_size = ((size + tmp_size) + 2);
            tmp_ptr = (merge_ptr - tmp_size);
            if (*(tmp_ptr - 1) = F_RM_MEM_FREE) then { 
                *merge_ptr = 0;
                *tmp_ptr = new_size;
                *(end_ptr - 1) = new_size;
                tmp_ptr = (tmp_ptr + 1);
                call memSet(tmp_ptr, 0, new_size)
            }
        }
    };
    
    return
}

##############################################################
# malloc: used to allocate heap space for a process
# * arg 1: size in words to allocate for process
# * arg 2: pointer to process control block
# * returns: 
#   * failure - -1
#   * success - address of start of heap space
##############################################################

function malloc(size, proc_ctrl_block) {
    local proc_heap_size, proc_heap_base, ind_ptr, done, poss_len;
    local end_ptr;
    proc_heap_size = *(proc_ctrl_block + alloc_size);
    proc_heap_base = *(proc_ctrl_block + base_addr);
    
    ind_ptr = proc_heap_base;
    done = 0;
    
    # heaps do not grow yet
    if (((ind_ptr + 2) + size) > proc_heap_size) then {
        printstr "FIXME: expand heap\n";
        return neg 1
    };
    
    while (done = 0) do {
        while (*ind_ptr != F_PM_MEM_FREE) do {
            ind_ptr = (ind_ptr + 1)
        };
        poss_len = *(ind_ptr + 1);
        end_ptr = ((ind_ptr + poss_len) + 2);
        
        # enough space?
        if (poss_len >= size) then done = 1
    };

    *ind_ptr = F_PM_MEM_USED;
    *(ind_ptr + 1) = size;
    end_ptr = ((ind_ptr + 2) + size);
    *end_ptr = F_PM_MEM_USED;
    *(end_ptr - 1) = size;
    
    if (size < poss_len) then {
        *(end_ptr + 1) = F_PM_MEM_FREE;
        *(end_ptr + 2) = (poss_len - size);
        end_ptr = ((ind_ptr + poss_len) + 1);
        *end_ptr = (poss_len - size)
    };
    
    return
}

##############################################################
# free: used to free space on the heap for a process
# * arg 1: base address to free from
# * arg 2: size in words to free, must be same as called with rmalloc
# * arg 3: pointer to process control block
# * returns: 
#   * failure - error code in one of following occured:
#       * -1 : invalid base address
#       * -2 : invalid size
#   * success - address of start of heap space
##############################################################
function free(base, size, proc_ctrl_block) {
    local ind_ptr, end_ptr, merge_ptr, new_size, tmp_size, tmp_ptr;
    local proc_heap_size, proc_heap_base;
    
    proc_heap_size = *(proc_ctrl_block + alloc_size);
    proc_heap_base = *(proc_ctrl_block + base_addr);

    ind_ptr = (base - 2); # special markers start at base - 2
    if (*ind_ptr != F_PM_MEM_USED) then return neg 1;
    *ind_ptr = F_PM_MEM_FREE;

    # make sure that we got the right size passed in
    end_ptr = ((ind_ptr + 3) + size);
    if (*end_ptr != size) then return neg 2;
    
    # check the end word to make sure its marked properly
    end_ptr = (end_ptr + 1);
    if (*end_ptr != F_PM_MEM_USED) then return neg 1;
    *end_ptr = F_PM_MEM_FREE;
        
    # see if we can merge ourself to the block before
    if (ind_ptr != proc_heap_base) then {
        merge_ptr = (ind_ptr - 1);
        if (*merge_ptr = F_PM_MEM_FREE) then {
            tmp_size = *(merge_ptr - 1);
            new_size = ((size + tmp_size) + 2);
            tmp_ptr = (merge_ptr - tmp_size);
            if (*(tmp_ptr - 1) = F_PM_MEM_FREE) then { 
                *merge_ptr = 0;
                *tmp_ptr = new_size;
                *(end_ptr - 1) = new_size;
                tmp_ptr = (tmp_ptr + 1);
                call memSet(tmp_ptr, 0, new_size)
            }
        }
    };
    
    return
}

end
