#include "disc.h"
#include <string.h>
#include <iostream>

using namespace std;

const int superBlockLocation = 0, clusterSize = 8, clusterByteSize = clusterSize * blockSize;
const int superBlockNameLength = blockSize - 6 * sizeof(unsigned int);

typedef Byte Cluster[clusterByteSize];

bool readCluster(void *cluster, int discnum, unsigned int location)
{
    //cout << "Reading cluster located at " << location << endl;
    Byte * data = (Byte *) cluster;
    for(int i = 0; i < clusterSize; i++)
        if(readblock(discnum, location + i, data + i * blockSize) == 0)
            return false;
    return true; 
}

bool writeCluster(void *cluster, int discnum, unsigned int location)
{
    //cout << "Writing cluster located at " << location << endl;
    Byte * data = (Byte *) cluster;
    for(int i = 0; i < clusterSize; i++)
        if(writeblock(discnum, location + i, data + i * blockSize) == 0)
            return false;
    return true;
}

struct __SuperBlock
{
    unsigned int rootDir, clusterMap, clusterMapBlockSize, numClustersTotal, numClustersUsable, usableClusterStart;
    char name[superBlockNameLength];
};

typedef struct __SuperBlock SuperBlock;
