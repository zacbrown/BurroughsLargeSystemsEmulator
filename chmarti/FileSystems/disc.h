#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>

const int blockSize = 512;

typedef unsigned char Byte;
typedef Byte Block[blockSize];

int createdisk(const char *name, int blocks)
{
    Block b;
    int disknum, x;
    for(int i = 0; i <blockSize; i++)
        b[i] = 0;
    disknum = open(name, O_RDWR | O_CREAT | O_TRUNC, 0600);
    if(disknum < 0)
    {
        fprintf(stderr, "Create failed for \"s\"\n", name);
        return 0;
    }
    for(int i = 0; i < blocks; i++)
    {
        x = write(disknum, b, blockSize);
        if(x<blockSize)
        {
            fprintf(stderr, "Failed to create block %d of \"%s\"\n", i, name);
            return 0;
        }
    } 
    close(disknum);
    return 1;
}

int mountdisk(const char *name)
{
    int disknum = open(name, O_RDWR);
    if(disknum < 0)
    {
        fprintf(stderr, "Failed to open \"s\"\n", name);
        return -1;
    }
    return disknum;
}

int opendisk(const char *name)
{
    return mountdisk(name);
}

int disksize(int disknum)
{
    int sz = lseek(disknum, 0, SEEK_END);
    return sz / blockSize;
}

int dismountdisk(int disknum)
{
    return close(disknum);
}

int closedisk(int disknum)
{
    return close(disknum);
}

int writeblock(int disknum, unsigned long int blocknum, void *b)
{
    //printf("Writing block at %i\n", (int) blocknum);
    int x = lseek(disknum, blocknum*blockSize, SEEK_SET);
    if(x < 0)
        return 0;
    x = write(disknum, b, blockSize);
    if(x < blockSize)
        return 0;
    return 1;
}

int readblock(int disknum, unsigned long int blocknum, void *b)
{
    //printf("Reading block at %i\n", (int) blocknum);
    int x = lseek(disknum, blocknum*blockSize, SEEK_SET);
    if(x < 0)
        return 0;
    x = read(disknum, b, blockSize);
    if(x < 512)
        return 0;
    return 1;
}
