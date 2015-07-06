            JMP main

            .define stuff 30

;Disc IO stuff
;op = 0 read / 1 write / 2 mount / 3 unmount / 4 create
;dev = 0 stdin / 1 stdout / 2 stderr

io:       
io_op:      .data   1
io_dev:     .data   1
io_block:   .data   0
io_addr:    .data   stuff


main:       LOAD    R1, 43
            STORE   R1, stuff
            STORE   R1, stuff + 1
            LOAD    R1, 0
            STORE   R1, stuff + 2
            IOOP    R1, io
            HALT
