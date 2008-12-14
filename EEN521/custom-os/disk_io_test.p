# Author: Zac Brown
# Date: 12.08.08
# Filename: disk_io_test.p

import function diskSize, diskRead, diskWrite;
import function isErrorSet;

const first_block=0, store_size=128, num_blocks=1, disk_num = 1;

function test_Size() {
    local ret, error, disk_num;
    disk_num = 1;
    ret = call diskSize(disk_num);
    error = call isErrorSet();
    if (error != 0) then printstr "\ndiskSize() failed\n"
    else printstr "\ndiskSize() succeeded\n";
    print ret;
    return
}

function test_Write() { 
    local ret, error, store:store_size, i;
    i = 0;
    while (i < 5) do {
        *(store+i) = 255;
        i = (i + 1)
    };
    ret = call diskWrite(disk_num, num_blocks, first_block, store);
    error = call isErrorSet();
    if (error != 0) then printstr "\ndiskWrite() failed\n"
    else printstr "\ndiskWrite() succeeded\n";
    print ret;
    return
}

function test_Read() {
    local ret, error, store:store_size, i;
    i = 0;
    ret = call diskRead(disk_num, num_blocks, first_block, store);
    error = call isErrorSet();
    if (error != 0) then printstr "\ndiskRead() failed\n"
    else printstr "\ndiskRead() succeeded\n";
    print ret;
    while (i < 5) do {
        print *(store + i);
        printstr "\n";
        i = (i + 1)
    };
    return
}

main {
    call test_Size();
    call test_Write();
    call test_Read();
    return
}

end