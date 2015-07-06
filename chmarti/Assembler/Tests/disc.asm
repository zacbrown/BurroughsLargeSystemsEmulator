            JMP main

            .define stuff 30

;Disc IO stuff
;op = 0 read / 1 write / 2 mount / 3 unmount / 4 create
;dev = 0 stdin / 1 stdout / 2 stderr

io:       
io_op:      .data   1
io_dev:     .data   1
io_block:   .data   0
io_addr:    .data   30


main:       LOAD    R1, 67
            STORE   R1, stuff

            LOAD    R1, 104
            STORE   R1, stuff + 1

            LOAD    R1, 114
            STORE   R1, stuff + 2

            LOAD    R1, 105
            STORE   R1, stuff + 3

            LOAD    R1, 115
            STORE   R1, stuff + 4

            LOAD    R1, 10
            STORE   R1, stuff + 5

            LOAD    R1, 0
            STORE   R1, stuff + 6

            IOOP    R1, io

            HALT
