#include "disc.h"

const int superBlockLocation = 0, clusterSize = 8, clusterByteSize = clusterSize * 512;

class SuperBlock
{
    public:
    unsigned int root, freeList;
    char name[64];
    Byte space[444];

    SuperBlock(string n, int r = clusterByteSize)
    {
        root = r;
        int i;
        for(i = 0; i < n.length(); i++)
            name[i] = n[i];
        for(; i < 64; i++)
            name[i] = 0;
    }
    
    SuperBlock()
    {
    }

    bool read(int discnum)
    {
        if(readblock(discnum, superBlockLocation, this) == 0)
            return false;
        return true;
    }
    
    SuperBlock(int discnum)
    {
        read(discnum);
    }

    bool write(int discnum)
    {
        if(writeblock(discnum, superBlockLocation, this) == 0)
            return false;
        return true;
    }
};

class Cluster
{
    public:
    Byte data[clusterByteSize];

    Cluster()
    {
    }

    bool read(int discnum, int location)
    {
        for(int i = 0; i < clusterSize; i++)
            if(readblock(discnum, location * clusterSize, &data + i * clusterSize) == 0)
                return false;
        return true; 
    }

    Cluster(int discnum, int location)
    {
        read(discnum, location);
    }

    bool write(int discnum, int location)
    {
        for(int i = 0; i < clusterSize; i++)
            if(writeblock(discnum, location * clusterSize, &data + i * clusterSize) == 0)
                return false;
        return true;
    }
};
