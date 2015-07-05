g_MEMSTART:      .space     1
g_MEMAVAIL:      .space     1
g_file_descriptor_array:      .space     15
g_process_control_block:      .space     3
g_superblock_struct:      .space     8

f_fsys_readSuperBlock:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 128
     SUB    SP, 1
     PUSH   R1
     LOAD   R1, FP-128
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_diskRead
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-129]
     LOAD   R1, [FP-129]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  NEQ, L1
     PUSH   R1
     LOAD   R1, 8
     PUSH   R1
     LOAD   R1, FP-128
     PUSH   R1
     LOAD   R1, [FP+3]
     PUSH   R1
     CALL   f_memcpy
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-129]
L1:
     LOAD   R1, [FP-129]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_fsys_writeSuperBlock:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 128
     SUB    SP, 1
     LOAD   R1, 8
     PUSH   R1
     LOAD   R1, [FP+3]
     PUSH   R1
     LOAD   R1, FP-128
     PUSH   R1
     CALL   f_memCopy
     PUSH   R1
     LOAD   R1, FP-128
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_diskWrite
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-129]
     LOAD   R1, [FP-129]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_fsys_listDirectory:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 2
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_getEntryLoc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-1]
     LOAD   R1, [FP-1]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L2
     LOAD   R1, string_0
     FAKEIT R1, $printstr
     LOAD   SP, FP
     POP    FP
     RET

L2:
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-4]
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-6]
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     LOAD   R1, [FP-1]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-5]
     LOAD   R1, [FP-5]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L3
     LOAD   R1, string_1
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     CALL   f_free
     LOAD   SP, FP
     POP    FP
     RET

L3:
     LOAD   R1, string_2
     FAKEIT R1, $printstr
     LOAD   R1, [FP+2]
     FAKEIT R1, $printstr
     LOAD   R1, [FP-4]
     LOAD   R2, 2
     ADD    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, 0
     STORE  R1, [FP-2]
L4:
     LOAD   R1, [FP-2]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L5
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  LEQ, L6
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     LOAD   R2, 12928
     COMP   R1, R2
     JCOND  GEQ, L7
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-5]
     LOAD   R1, [FP-5]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L8
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     CALL   f_free
     LOAD   R1, string_3
     FAKEIT R1, $printstr
     LOAD   SP, FP
     POP    FP
     RET

L8:
     LOAD   R1, string_4
     FAKEIT R1, $printstr
     LOAD   R1, [FP-6]
     LOAD   R2, 0
     ADD    R1, R2
     LOAD   R1, [R1]
     FAKEIT R1, $printstr
     LOAD   R1, string_5
     FAKEIT R1, $printstr
L7:
L6:
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-3]
     JUMP   L4
L5:
     LOAD   SP, FP
     POP    FP
     RET


f_getEntryLoc:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 2
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     LOAD   R1, [FP-12]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  NEQ, L9
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, string_6
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strcmp
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     LOAD   R1, [FP-12]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L10
     LOAD   R1, g_superblock_struct
     LOAD   R2, 7
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L10:
L9:
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 6
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-6]
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-7]
     LOAD   R1, 0
     STORE  R1, [FP-14]
     LOAD   R1, 1
     STORE  R1, [FP-9]
     LOAD   R1, g_superblock_struct
     LOAD   R2, 7
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-3]
L11:
     LOAD   R1, [FP-14]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L12
     PUSH   R1
     LOAD   R1, [FP-9]
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_get_elem_from_path
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-11]
     LOAD   R1, [FP-11]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L13
     JUMP   L12
L13:
     PUSH   R1
     LOAD   R1, FP-5
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     LOAD   R1, [FP-3]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     LOAD   R1, [FP-12]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L14
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 6
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L14:
     LOAD   R1, [FP-7]
     LOAD   R2, FP-5
     LOAD   R3, 1
     ADD    R2, R3
     LOAD   R2, [R2]
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-3]
     PUSH   R1
     LOAD   R1, FP-5
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     LOAD   R1, [FP-3]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     LOAD   R1, [FP-12]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L15
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 6
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L15:
     LOAD   R1, [FP-6]
     LOAD   R2, FP-5
     LOAD   R3, 1
     ADD    R2, R3
     LOAD   R2, [R2]
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-2]
     LOAD   R1, [FP-3]
     LOAD   R2, 7
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-17]
     LOAD   R1, [FP-2]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L16
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L16:
L17:
     LOAD   R1, [FP-17]
     LOAD   R2, [FP-2]
     LOAD   R3, 30
     ADD    R2, R3
     COMP   R1, R2
     JCOND  GEQ, L18
     LOAD   R1, [FP-17]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  LEQ, L19
     PUSH   R1
     LOAD   R1, FP-5
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     LOAD   R1, [FP-17]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     LOAD   R1, [FP-12]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L20
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 6
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L20:
     LOAD   R1, [FP-7]
     LOAD   R2, 0
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-18]
     PUSH   R1
     LOAD   R1, 24
     PUSH   R1
     LOAD   R1, [FP-18]
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     CALL   f_strcmp
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     LOAD   R1, [FP-12]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L21
     LOAD   R1, [FP-11]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L22
     LOAD   R1, [FP-7]
     LOAD   R2, 6
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  NEQ, L23
     LOAD   R1, [FP-17]
     STORE  R1, [FP-3]
L23:
     JUMP   L23
L22:
     LOAD   R1, [FP-11]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  NEQ, L25
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 6
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     CALL   f_free
     LOAD   R1, [FP-17]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L25:
L24:
L21:
L19:
     JUMP   L17
L18:
     JUMP   L11
L12:
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-6]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-7]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 6
     PUSH   R1
     LOAD   R1, [FP-8]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_getBlockByAddress:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+4]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_transAddrToBlockOffset
     LOAD   R1, [FP+4]
     LOAD   R2, 0
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-4]
     PUSH   R1
     LOAD   R1, [FP+3]
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskRead
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-3]
     LOAD   R1, [FP-3]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_get_elem_from_path:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, 0
     STORE  R1, [FP-8]
     LOAD   R1, 0
     STORE  R1, [FP-3]
     LOAD   R1, 0
     STORE  R1, [FP-7]
     LOAD   R1, 47
     STORE  R1, [FP-9]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-5]
L26:
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     COMP   R1, R2
     JCOND  GEQ, L27
     LOAD   R1, [FP-3]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-9]
     COMP   R1, R2
     JCOND  NEQ, L28
     LOAD   R1, [FP-8]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-8]
L28:
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-3]
     LOAD   R1, [FP+4]
     LOAD   R2, [FP-8]
     COMP   R1, R2
     JCOND  NEQ, L29
     JUMP   L27
L29:
     JUMP   L26
L27:
     LOAD   R1, [FP-3]
     LOAD   R2, [FP-5]
     COMP   R1, R2
     JCOND  LSS, L30
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L30:
     LOAD   R1, [FP-3]
     STORE  R1, [FP-4]
L31:
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-5]
     COMP   R1, R2
     JCOND  GEQ, L32
     LOAD   R1, [FP-4]
     LOAD   R2, [FP+2]
     LDCH   R1, R2
     LOAD   R2, [FP-9]
     COMP   R1, R2
     JCOND  NEQ, L33
     JUMP   L32
L33:
     LOAD   R1, [FP-4]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-4]
     JUMP   L31
L32:
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-5]
     COMP   R1, R2
     JCOND  NEQ, L34
     LOAD   R1, 1
     STORE  R1, [FP-7]
L34:
     LOAD   R1, [FP-4]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-4]
     LOAD   R1, [FP-4]
     PUSH   R1
     LOAD   R1, [FP-3]
     PUSH   R1
     LOAD   R1, [FP+3]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strcpy
     LOAD   R1, [FP-7]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_get_next_free_file_ent:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 2
     SUB    SP, 1
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-4]
     LOAD   R1, 1
     STORE  R1, [FP-3]
L35:
     LOAD   R1, [FP-3]
     LOAD   R2, 101
     COMP   R1, R2
     JCOND  GEQ, L36
     PUSH   R1
     LOAD   R1, FP-7
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     LOAD   R1, [FP-3]
     LOAD   R1, [R1]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-5]
     LOAD   R1, [FP-5]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L37
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     CALL   f_free
     LOAD   SP, FP
     POP    FP
     RET

L37:
     LOAD   R1, 0
     STORE  R1, [FP-8]
L38:
     LOAD   R1, [FP-8]
     LOAD   R2, 128
     COMP   R1, R2
     JCOND  GEQ, L39
     LOAD   R1, [FP-8]
     LOAD   R2, 5
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L40
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R2, 128
     MUL    R1, R2
     LOAD   R2, [FP-8]
     ADD    R1, R2
     STORE  R1, [FP-5]
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     CALL   f_free
     LOAD   R1, [FP-5]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L40:
     LOAD   R1, [FP-8]
     LOAD   R2, 8
     ADD    R1, R2
     STORE  R1, [FP-8]
     JUMP   L38
L39:
     LOAD   R1, [FP-3]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-3]
     JUMP   L35
L36:
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-4]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_get_next_free_inode:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 2
     SUB    SP, 1
     SUB    SP, 1
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-5]
     LOAD   R1, 101
     STORE  R1, [FP-4]
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskSize
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-10]
L41:
     LOAD   R1, [FP-4]
     LOAD   R2, [FP-10]
     COMP   R1, R2
     JCOND  GEQ, L42
     PUSH   R1
     LOAD   R1, FP-8
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     LOAD   R1, [FP-4]
     LOAD   R1, [R1]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-6]
     LOAD   R1, [FP-6]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L43
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   SP, FP
     POP    FP
     RET

L43:
     LOAD   R1, 0
     STORE  R1, [FP-9]
L44:
     LOAD   R1, [FP-9]
     LOAD   R2, 128
     COMP   R1, R2
     JCOND  GEQ, L45
     LOAD   R1, [FP-9]
     LOAD   R2, 0
     ADD    R1, R2
     LOAD   R1, [R1]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L46
     LOAD   R1, [FP-4]
     LOAD   R2, 1
     SUB    R1, R2
     LOAD   R2, 128
     MUL    R1, R2
     LOAD   R2, [FP-9]
     ADD    R1, R2
     STORE  R1, [FP-6]
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, [FP-6]
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L46:
     LOAD   R1, [FP-9]
     LOAD   R2, 32
     ADD    R1, R2
     STORE  R1, [FP-9]
     JUMP   L44
L45:
     LOAD   R1, [FP-4]
     LOAD   R2, 1
     ADD    R1, R2
     STORE  R1, [FP-4]
     JUMP   L41
L42:
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET

f_fsys_createFile:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_fsys_createDirectory:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_createInode:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 2
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 2
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     SUB    SP, 1
     PUSH   R1
     LOAD   R1, [FP+3]
     PUSH   R1
     CALL   f_getEntryLoc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-9]
     LOAD   R1, [FP-9]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L47
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L47:
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-5]
     PUSH   R1
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     CALL   f_malloc
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-12]
     PUSH   R1
     LOAD   R1, FP-7
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     LOAD   R1, [FP-9]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     LOAD   R1, [FP-8]
     LOAD   R2, 1
     COMP   R1, R2
     JCOND  EQL, L48
     LOAD   R1, string_7
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L48:
     PUSH   R1
     CALL   f_get_next_free_file_ent
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-15]
     LOAD   R1, [FP-15]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L49
     LOAD   R1, string_8
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L49:
     PUSH   R1
     CALL   f_get_next_free_inode
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-16]
     LOAD   R1, [FP-16]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L50
     LOAD   R1, string_9
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L50:
     LOAD   R1, [FP-5]
     LOAD   R2, FP-7
     LOAD   R3, 1
     ADD    R2, R3
     LOAD   R2, [R2]
     ADD    R1, R2
     STORE  R1, [FP-10]
     LOAD   R1, [FP-10]
     LOAD   R2, 7
     ADD    R1, R2
     STORE  R1, [FP-10]
     LOAD   R1, 0
     STORE  R1, [FP-11]
L51:
     LOAD   R1, [FP-11]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L52
     LOAD   R1, [FP-10]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  NEQ, L53
     LOAD   R1, [FP-15]
     LOAD   R2, [FP-10]
     STORE  R1, [R2]
     PUSH   R1
     LOAD   R1, FP-14
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     LOAD   R1, [FP-15]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     LOAD   R1, [FP-8]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L54
     LOAD   R1, string_10
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L54:
     LOAD   R1, [FP-12]
     LOAD   R2, FP-14
     LOAD   R3, 1
     ADD    R2, R3
     LOAD   R2, [R2]
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-17]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strlen
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-18]
     LOAD   R1, [FP-18]
     LOAD   R2, 1
     SUB    R1, R2
     STORE  R1, [FP-18]
     LOAD   R1, [FP-18]
     PUSH   R1
     LOAD   R1, 0
     PUSH   R1
     LOAD   R1, [FP-17]
     PUSH   R1
     LOAD   R1, [FP+2]
     PUSH   R1
     CALL   f_strcpy
     LOAD   R1, 1
     LOAD   R2, [FP-17]
     LOAD   R3, 5
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   R1, [FP+4]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  EQL, L55
     LOAD   R1, 1
     LOAD   R2, [FP-17]
     LOAD   R3, 6
     ADD    R2, R3
     STORE  R1, [R2]
     JUMP   L56
L55:
     LOAD   R1, 0
     LOAD   R2, [FP-17]
     LOAD   R3, 6
     ADD    R2, R3
     STORE  R1, [R2]
L56:
     LOAD   R1, [FP-16]
     LOAD   R2, [FP-17]
     LOAD   R3, 7
     ADD    R2, R3
     STORE  R1, [R2]
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     LOAD   R1, FP-14
     LOAD   R1, [R1]
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskWrite
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     LOAD   R1, [FP-8]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  GEQ, L57
     LOAD   R1, string_11
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L57:
     PUSH   R1
     LOAD   R1, FP-14
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     LOAD   R1, [FP-16]
     PUSH   R1
     CALL   f_getBlockByAddress
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     LOAD   R1, [FP-8]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L58
     LOAD   R1, string_12
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L58:
     LOAD   R1, [FP-12]
     LOAD   R2, FP-14
     LOAD   R3, 1
     ADD    R2, R3
     LOAD   R2, [R2]
     ADD    R1, R2
     LOAD   R1, [R1]
     STORE  R1, [FP-17]
     LOAD   R1, -1
     LOAD   R2, [FP-17]
     STORE  R1, [R2]
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     LOAD   R1, FP-14
     LOAD   R1, [R1]
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskWrite
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     LOAD   R1, [FP-8]
     LOAD   R2, 0
     COMP   R1, R2
     JCOND  GEQ, L59
     LOAD   R1, string_13
     FAKEIT R1, $printstr
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L59:
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     LOAD   R1, FP-7
     LOAD   R1, [R1]
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     LOAD   R1, 1
     PUSH   R1
     CALL   f_diskWrite
     POP    R1
     STORE  R0, R1
     STORE  R1, [FP-8]
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-5]
     PUSH   R1
     CALL   f_free
     LOAD   R1, g_process_control_block
     PUSH   R1
     LOAD   R1, 128
     PUSH   R1
     LOAD   R1, [FP-12]
     PUSH   R1
     CALL   f_free
     LOAD   R1, [FP-8]
     LOAD   R1, 1
     MUL    R1, -1
     COMP   R1, R2
     JCOND  NEQ, L60
     LOAD   R1, 1
     MUL    R1, -1
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
     JUMP   L61
L60:
     LOAD   R1, 0
     LOAD   R0, R1
     LOAD   SP, FP
     POP    FP
     RET
L61:
L53:
     JUMP   L51
L52:

f_fsys_removeDirectory:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_fsys_removeFile:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_fsys_fileAppend:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_fsys_fileOpen:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_fsys_fileClose:
     PUSH   FP
     LOAD   FP, SP

     LOAD   SP, FP
     POP    FP
     RET


f_transAddrToBlockOffset:
     PUSH   FP
     LOAD   FP, SP

     SUB    SP, 1
     SUB    SP, 1
     LOAD   R1, [FP+2]
     LOAD   R2, 128
     DIV    R1, R2
     LOAD   R2, [FP+3]
     STORE  R1, [R2]
     LOAD   R1, [FP+2]
     LOAD   R2, 128
     MOD    R1, R2
     LOAD   R2, [FP+3]
     LOAD   R3, 1
     ADD    R2, R3
     STORE  R1, [R2]
     LOAD   SP, FP
     POP    FP
     RET


string_0: .STRING "ERROR: no such directory\n"
string_1: .STRING "ERROR: unable to read block, possible memory error"
string_2: .STRING "\n"
string_3: .STRING "ERROR: unable to read file entry\n"
string_4: .STRING "\n\t"
string_5: .STRING "\n"
string_6: .STRING "/"
string_7: .STRING "ERROR: unable to read block, possible memory error"
string_8: .STRING "ERROR: no free file entry exists."
string_9: .STRING "ERROR: no free free inode exists."
string_10: .STRING "ERROR: unable to load file entry location."
string_11: .STRING "ERROR: could not write new file entry to disk."
string_12: .STRING "ERROR: could not write new file entry to disk."
string_13: .STRING "ERROR: could not write new inode entry to disk."


  .IMPORT   f_diskRead
  .IMPORT   f_diskWrite
  .IMPORT   f_diskSize
  .IMPORT   f_memcpy
  .IMPORT   f_strcmp
  .IMPORT   f_strcpy
  .IMPORT   f_strcut
  .IMPORT   f_strcat
  .IMPORT   f_strchr
  .IMPORT   f_strrchr
  .IMPORT   f_strlen_word
  .EXPORT   f_fsys_readSuperBlock
  .EXPORT   f_fsys_writeSuperBlock
  .EXPORT   f_fsys_listDirectory
  .EXPORT   f_fsys_createFile
  .EXPORT   f_fsys_createDirectory
  .EXPORT   f_fsys_removeDirectory
  .EXPORT   f_fsys_removeFile
  .EXPORT   f_fsys_fileAppend
  .EXPORT   f_fsys_fileOpen
  .EXPORT   f_fsys_fileClose
