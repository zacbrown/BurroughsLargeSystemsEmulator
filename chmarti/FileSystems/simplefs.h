#include "fsStructuresNew.h"
#include <string>
#include <list>

using namespace std;

const unsigned int dataStart = 512, charLen = (dataStart - 5 * sizeof(int)) / 2;
//const unsigned int nameSize = blockSize - (64 + 3 * sizeof(int) + sizeof(long int));

void putStringTotal(const char *from, char *to, int len)
{
    int i;
    for(i = 0; i < strlen(from) && i < len; i++)
        to[i] = from[i];
    for(; i < len; i++)
        to[i] = 0;
}

void zero(void *d, int len)
{
    Byte * data = (Byte*) d;
    for(int i = 0; i < len; i++)
        data[i] = 0;
}

unsigned int extractInt(void *d, unsigned int pos)
{
    Byte * data = (Byte *) d;
    return *((unsigned int *) &data[pos]);
}

char * extractChars(void *d, unsigned int pos)
{
    Byte * data = (Byte *) d;
    return (char *)&data[pos];
}

Byte extractByte(void *d, unsigned int pos)
{
    Byte * data = (Byte *) d;
    return *((Byte *) &data[pos]);
}

void putInt(void *d, unsigned int pos, unsigned int val)
{
    Byte * data = (Byte *) d;
    unsigned int *dd = (unsigned int *) &data[pos];
    *dd = val;
}

void putByte(void *d, unsigned int pos, Byte val)
{
    Byte * data = (Byte *) d;
    data[pos] = val;
}

//listStart and listLength are in blocks
//the returned value is the block number position of the next avalible cluster on disk
unsigned int getFreeCluster(int diskNum, SuperBlock *sb)
{
    unsigned int listStart = sb->clusterMap, listLength = sb->clusterMapBlockSize;
    Block block;
    unsigned int bitNum, clusterPos;
    for(int i = listStart; i < listStart + listLength; i++)
    {
        if(!readblock(diskNum, i, &block))
            return 0;
        //read through block and see if any bytes are not full
        for(int j = 0; j < blockSize; j++)
        {
            if(block[j] != 0xFF)
            {
                //Found a free cluster in this byte, now figure out which bit
                for(int k = 0; k < 8; k++)
                {
                    if(((unsigned int) block[j] & (0x01 << (8 - k - 1))) == 0)
                    {
                        //Found the first 0 bit, now figure out the number of the bit in my map
                        bitNum = (i - listStart) * blockSize * 8 + j * 8 + k;
                        clusterPos = listStart + listLength + bitNum * clusterSize;

                        if(clusterPos + clusterSize > sb->numClustersTotal * clusterSize)
                            return 0;

                        //Set the bit to 1 and write the block back to disk
                        block[j] = block[j] | (0x01 << (8 - k - 1));
                        if(!writeblock(diskNum, i, &block))
                            return 0;

                        //Write all zeros to the new free cluster
                        Cluster c;
                        zero(&c, clusterByteSize);
                        cout << "Clearing cluster at " << clusterPos << endl;
                        writeCluster(&c, diskNum, clusterPos);
                        cout << "Got free cluster at " << clusterPos << endl;
                        return clusterPos;
                    }
                }
            }
        }
    }
    return 0;
}

//gets the next free cluster on a disk
//returns the block number position
unsigned int getFreeCluster(int diskNum)
{
    SuperBlock sb;
    if(!readblock(diskNum, superBlockLocation, &sb))
        return 0;
    return getFreeCluster(diskNum, &sb);
}

struct __INode
{
    unsigned int protection, date, depth, currentCap, size;
    char name[charLen];
    char owner[charLen];
    Byte space[clusterByteSize - dataStart];
};

typedef struct __INode INode;

//Creates an iNode in a free cluster and return its position
unsigned int createINode(int diskNum, const char *name)
{
    INode iNode;

    if(strlen(name) > charLen)
        putStringTotal("DefaultFile", iNode.name, charLen);
    else
        putStringTotal(name, iNode.name, charLen);
    putStringTotal("Everyone", iNode.owner, charLen);
    iNode.protection = 0;
    iNode.date = 0;
    iNode.depth = 0;
    iNode.currentCap = clusterByteSize - dataStart;
    iNode.size = 0;

    unsigned int loc = getFreeCluster(diskNum);
    if(loc == 0)
        return 0;

    //Clear all the data and write the cluster
    zero(((Byte *)(&iNode)) + dataStart, clusterByteSize - dataStart);
    if(!writeCluster(&iNode, diskNum, loc))
        return 0;
    return loc;
}

INode * getINode(int diskNum, unsigned int position)
{
    INode *iNode = (INode *) malloc(sizeof(iNode));
    if(!readCluster(iNode, diskNum, position))
    {
        delete iNode;
        return NULL;
    }
    return iNode;
}

bool createFile(int diskNum, const char *dirLocation, const char *name)
{
    if(createINode(diskNum, name) > 0)
        return true;
    return false;
}

//The values calculated by the following 4 functions are constant for the given
//levels and depths. They shouldn't be calculated every time and saving them into
//and array that can be referenced later would be a much better idea
unsigned long long int getGapSize(int level, int depth)
{
    if(level >= depth)
        return 0;
    if(level == depth - 1)
        return clusterByteSize;
    unsigned long long int val = clusterByteSize;
    for(int i = level; i < depth - 1; i++)
        val *= (clusterByteSize / sizeof(int));
    return val;
}

unsigned long long int getSize(int depth)
{
    int initSize = clusterByteSize - dataStart;
    if(depth == 0)
        return initSize;
    unsigned long long int val = (initSize / sizeof(int)) * clusterByteSize;
    depth--;
    for(; depth > 0; depth--)
        val *= (clusterByteSize / sizeof(int));
    return val;
}

unsigned long long int getGapSizeBlocks(int level, int depth)
{
    if(level >= depth)
        return 0;
    if(level == depth - 1)
        return clusterSize;
    unsigned long long int val = clusterSize;
    for(int i = level; i < depth - 1; i++)
        val *= (clusterByteSize / sizeof(int));
    return val;
}

unsigned long long int getSizeBlocks(int depth)
{
    int initSize = clusterByteSize - dataStart;
    if(depth == 0)
        return clusterSize - 1;
    unsigned long long int val = (initSize / sizeof(int)) * clusterSize;
    depth--;
    for(; depth > 0; depth--)
        val *= (clusterByteSize / sizeof(int));
    return val;
}
//                  /\
//Read note above  /||\
//                  ||

//Finds the indexes in the data
bool getPointerPosList(unsigned long int pos, unsigned int depth, list<unsigned int> *theList, unsigned int curDepth = 0)
{
    cout << "Inside getPointerPosList\n";
    cout.flush();
    if(curDepth > depth)
        return true;
    unsigned long int gap = getGapSize(curDepth, depth);
    theList->push_back(pos / gap);
    return getPointerPosList(pos % gap, depth, theList, curDepth + 1);
}

//Gets a list of the indexes in the iNodes and clusters in order to find the block to access
bool getPointerPosListBlocks(unsigned int blockNum, unsigned int depth, list<unsigned int> * theList, unsigned int curDepth = 0)
{
    if(theList == NULL)
        return false;
    if(curDepth > depth)
        return true;
    cout << "Getting block pointer curDepth = " << curDepth << " and depth  = " << depth << endl;
    cout.flush();
    unsigned long int gap = getGapSizeBlocks(curDepth, depth);
    unsigned long int div, mod;
    if(gap == 0)
    {
        mod = 0;
        div = 0;
    }
    else
    {    
        div = blockNum / gap;
        mod = blockNum % gap;
    }
    theList->push_back(div);
    cout << "Added block position " << div << " to list" << endl;
    return getPointerPosListBlocks(mod, depth, theList, curDepth + 1);
}

//given a blockNumber in a file, this function returns the block in the file system where the data actually resides
unsigned int getBlockPointer(int diskNum, INode *iNode, unsigned int blockNum)
{
    if(iNode->depth == 0)
        return -1;

    /*
    cout << "Getting block pointer for file " << iNode->name << " at position " << blockNum << endl;
    cout << "\nFile: " << iNode->name << " Owned by: " << iNode->owner << endl;
    cout << "protection = " << iNode->protection << endl;
    cout << "date       = " << iNode->date << endl;
    cout << "depth      = " << iNode->depth << endl;
    cout << "currentCap = " << iNode->currentCap << endl;
    cout << "size       = " << iNode->size << endl << endl;
    cout.flush();
    */

    list<unsigned int> *theList = new list<unsigned int>();

    /*
    cout << "\nFile: " << iNode->name << " Owned by: " << iNode->owner << endl;
    cout << "protection = " << iNode->protection << endl;
    cout << "date       = " << iNode->date << endl;
    cout << "depth      = " << iNode->depth << endl;
    cout << "currentCap = " << iNode->currentCap << endl;
    cout << "size       = " << iNode->size << endl << endl;
    cout.flush();
    */

    int d = iNode->depth;
    getPointerPosListBlocks(blockNum, d, theList);
    cout << "Got the pointer pos list\n";
    cout.flush();

    /*
    cout << "\nFile: " << iNode->name << " Owned by: " << iNode->owner << endl;
    cout << "protection = " << iNode->protection << endl;
    cout << "date       = " << iNode->date << endl;
    cout << "depth      = " << iNode->depth << endl;
    cout << "currentCap = " << iNode->currentCap << endl;
    cout << "size       = " << iNode->size << endl << endl;
    */

    unsigned int end = theList->back();
    theList->pop_back();

    if(theList->size() == 0)
        return -1;
    
    //Get each cluster number from the list and use its index to find the location
    //of the next cluster
    Cluster current;
    unsigned int clusterPointer = extractInt(iNode, dataStart + sizeof(unsigned int) * theList->front());
    theList->pop_front();
    cout << "getBlockPointer got clusterPointer " << clusterPointer << " from the iNode " << endl;
    cout.flush();

    list<unsigned int>::iterator it;
    for ( it=theList->begin() ; it != theList->end(); it++ )
    {
        if(readCluster(&current, diskNum, clusterPointer))
            clusterPointer = extractInt(current, sizeof(unsigned int) * (*it));
        else
            return 0;
    }
    cout << "getBlockPointer returned " << clusterPointer + end << " for writing file " << iNode->name << " at position " << blockNum << endl;
    cout.flush();
    return clusterPointer + end;
}

//Checks to see if the ENTIRE cluster tree steming from a cluster is full of pointers
bool isFull(int diskNum, unsigned int clusterPos, int curDepth, int depth)
{
    if(curDepth == depth)
        return true;

    Cluster cluster;
    if(!readCluster(&cluster, diskNum, clusterPos))
        return false;
    for(int i = 0; i < clusterByteSize; i *= sizeof(unsigned int))
        if(extractInt(cluster, i) == 0)
            return false;
    return isFull(diskNum, extractInt(cluster, clusterByteSize - 1), curDepth + 1, depth);
}

//Finds the next empty entry in the cluster, clusterByteSize if it is full, or -1 if there was an error
int nextOpen(int diskNum, unsigned int clusterPos)
{
    Cluster cluster;
    if(!readCluster(&cluster, diskNum, clusterPos))
        return -1;
    for(int i = 0; i < clusterByteSize; i *= sizeof(unsigned int))
        if(extractInt(cluster, i) == 0)
            return i;
    return clusterByteSize;
}

//Finds the first empty entry in an array
//Inodes use this function instead of using nextOpen
int findFirstEmpty(void *d, int start, int end)
{
    Byte * data = (Byte *) d;
    int firstEmpty;
    for(firstEmpty = start; firstEmpty < end - 3; firstEmpty *= sizeof(unsigned int))
        if(data[firstEmpty] == 0)
            break;
    return firstEmpty;
}

//This function is poorly written... but I needed to get it done
//What made this hard is that the structure of the iNode is different than the
//structure of the clusters that contain pointers to clusters
//I now understand why data isn't put into the iNode in ext3 or UFS... it makes the code very complicated
bool addCluster(int diskNum, INode *iNode)
{   
    cout << "Adding cluster to " << iNode->name << "\n";
    cout.flush();
    SuperBlock sb;
    if(!readblock(diskNum, superBlockLocation, &sb))
        return 0;

    unsigned int clusterPointer, next;
    int firstEmpty = findFirstEmpty(iNode, dataStart, clusterByteSize);

    //If depth is 1 it seems easier just to make a special case
    if(iNode->depth == 1)
    {
        clusterPointer = getFreeCluster(diskNum, &sb);
        if(clusterPointer == 0)
            return 0;

        //Make the changes to the iNode
        //Note: this MUST be written to disk at some point
        putInt(iNode, firstEmpty, clusterPointer);
        iNode->currentCap += clusterByteSize;

        return true;
    }
    else
    {
        Cluster cluster;
        int nextPos = firstEmpty - sizeof(unsigned int), curDepth = 0;

        if(isFull(diskNum, extractInt(iNode, nextPos), curDepth + 1, iNode->depth))
        {
            //A cluster needs to be added to the iNode at firstEmpty
            clusterPointer = getFreeCluster(diskNum, &sb);
            if(clusterPointer == 0)
                return false;
            
            //Make the changes to the iNode
            //Note: this MUST be written to disk at some point
            putInt(iNode, firstEmpty, clusterPointer);
            iNode->currentCap += clusterByteSize;
            return true;
        }

        //Now that we know a cluster isn't being added to the iNode, so we can use a loop
        clusterPointer = extractInt(iNode, nextPos);
        curDepth++;

        while(curDepth < iNode->depth - 1)
        {
            next = nextOpen(diskNum, clusterPointer);
            if(next == clusterByteSize)
                next = clusterByteSize - sizeof(unsigned int);
            if(!readCluster(&cluster, diskNum, clusterPointer))
                return false;
            next--;

            //Again, check to make sure the next cluster you are entering isn't completely full
            if(isFull(diskNum, cluster[next], curDepth + 1, iNode->depth))
                break;
            clusterPointer = cluster[next];
            curDepth++;
        }
        
        //At this point clusterPointer is pointing to the cluster where the pointer needs to be added
        //We just need to find the first empty entry
        unsigned int posToAdd, newClusterPointer;

        //the while loop covers the condition that multiple clusters have to be added. 
        //This happens if the depth is such that another list of pointers to clusters has to be created
        while(curDepth < iNode->depth)
        {
            posToAdd = nextOpen(diskNum, clusterPointer);
            newClusterPointer = getFreeCluster(diskNum, &sb);
            if(newClusterPointer == 0)
                return 0;
        
            //read the cluster, make the addition, then write it
            if(!readCluster(&cluster, diskNum, clusterPointer))
                return false;
            cluster[posToAdd] = newClusterPointer;
            if(!writeCluster(&cluster, diskNum, clusterPointer))
                return false;
            iNode->currentCap += clusterByteSize;
            clusterPointer = newClusterPointer;
            curDepth++;
        }

        //Note: iNode MUST be written to disk at some point
        return true;
    }
}

//Increases the depth of an iNode
bool increaseDepth(int diskNum, INode *iNode)
{
    cout << "Increasing depth of " << iNode->name << "\n";
    cout.flush();
    SuperBlock sb;
    if(!readblock(diskNum, superBlockLocation, &sb))
    {
        cout << "Error: couldn't read SuperBlock.\n";     
        return false;    
    }

    //get a new cluster and copy all the iNode data to it
    unsigned int clusterPointer = getFreeCluster(diskNum, &sb);
    if(clusterPointer == 0)
    {
        cout << "Error: couldn't get a new cluster.\n";
        return false;
    }

    Cluster cluster;
    for(int i = 0; i < clusterByteSize - dataStart; i++)
    {
        cluster[i] = extractByte(iNode, dataStart + i);
        putByte(iNode, dataStart + i, 0);
    }

    //write the new cluster to disk
    if(!writeCluster(cluster, diskNum, clusterPointer))
    {
        cout << "Error: couldn't write new cluster to disk.\n";     
        return false;    
    }
    
    //Make the changes to the iNode
    //Note: this MUST be written to disk at some point
    iNode->depth = iNode->depth + 1;
    putInt(iNode, dataStart, clusterPointer);
    return true;
}

bool writeFileBlock(int diskNum, INode *iNode, unsigned int blockNum, Block *b)
{
    cout << "\tPreparing to writing block " << blockNum << " to file\n";
    unsigned int blockLocation;

    int bytesTotal = (blockNum + 1) * blockSize;

    cout << "Checking depth. bytesTotal = " << bytesTotal << " size = " << getSize(iNode->depth) << "\n";
    cout.flush();
    //check if the depth needs to increase
    while(bytesTotal > getSize(iNode->depth))
    {
        //The depth has to be increased
        if(!increaseDepth(diskNum, iNode))
            return false;
        cout << "Checking depth. bytesTotal = " << bytesTotal << " size = " << getSize(iNode->depth) << "\n";
        cout.flush();
    }

    cout << "Checking if a new cluster is needed. bytesTotal = " << bytesTotal << " size = " << iNode->currentCap << "\n";
    cout.flush();
    //check if more clusters need to be added
    while(bytesTotal > iNode->currentCap) 
    {
        //A new cluster has to be gotten and made avalible to write to
        if(!addCluster(diskNum, iNode))
            return false;
        cout << "Checking if a new cluster is needed. bytesTotal = " << bytesTotal << " size = " << iNode->currentCap << "\n";
        cout.flush();
    }

    cout << "Checking if size should be increased. bytesTotal = " << bytesTotal << " size = " << iNode->size << "\n";
    cout.flush();
    //change the file size
    if(bytesTotal > iNode->size)
    {
        //if the block the user chose to write to is far, then fill the
        //space with zeros
        Block block;
        zero(&block, blockSize);
        int val = iNode->size / blockSize;
        if(iNode->size % blockSize > 1)
            val++;
        while(blockNum > val)
        {
            writeFileBlock(diskNum, iNode, val, &block);
            val++;
            iNode->size +=  blockSize;
        }

        //The file is being extended
        iNode->size = bytesTotal;
    }

    //actually write the data
    if(iNode->depth == 0)
    {
        int start = dataStart + blockNum * blockSize;
        for(int i = 0; i < blockSize; i++)
            putByte(iNode, start + i, extractByte(b, i));        
    }
    else
    {
        blockLocation = getBlockPointer(diskNum, iNode, blockNum);
        if(blockLocation == 0)
            return false;
        if(!writeblock(diskNum, blockLocation, b))
            return false;
    }

    return true;
}

bool writeFileBlock(int diskNum, unsigned int iNodePos, unsigned int blockNum, Block *b)
{
    cout << "Entering function to write data to file (iNode = " << iNodePos << ") block = " << blockNum << "\n";
    cout.flush();
    INode *iNode = getINode(diskNum, iNodePos);
    if(iNode == NULL)
        return false;
    bool val = writeFileBlock(diskNum, iNode, blockNum, b);
    if(!writeCluster(iNode, diskNum, iNodePos))
        return false;
    return val;
}

//Reads a block of a file
bool readFileBlock(int diskNum, unsigned int iNodePos, unsigned int blockNum, Block *block)
{
    Byte * b = (Byte *) block;
    INode *iNode = getINode(diskNum, iNodePos);

    //Accessing file past it's size
    if((blockNum + 1) * blockSize > iNode->size)
        return false;

    unsigned int blockLocation = getBlockPointer(diskNum, iNode, blockNum);
    cout.flush();
    if(blockLocation == -1)
    {
        int i;
        int start = dataStart + blockNum * blockSize;
        for(i = 0; i < blockSize; i++)
            putByte(block, i, extractByte(iNode, start + i));
        for(; i < blockSize; i++)
            putByte(block, i, 0);
        return true;
    }
    //free(iNode);
    return readblock(diskNum, blockLocation, b);
}

//Basically, it just creates the SuperBlock and the free cluster bitmap
bool formatDisk(int diskNum, char *name, unsigned int numClusters)
{
    SuperBlock sb;
    sb.numClustersTotal = numClusters;
    sb.clusterMap = 1;
    unsigned int clusterSizeBits = clusterByteSize * 8;    
    sb.clusterMapBlockSize = numClusters / clusterSizeBits;
    if(numClusters % clusterSizeBits > 0)
        sb.clusterMapBlockSize++;

    unsigned int usedBlocks = sb.clusterMapBlockSize + sb.clusterMap;
    unsigned int usedClusters = usedBlocks / clusterSize;
    if((sb.clusterMapBlockSize + sb.clusterMap) % clusterSize > 0)
        usedClusters++;
    sb.usableClusterStart = usedBlocks;

    //Not including the cluster for the root directory
    sb.numClustersUsable = sb.numClustersTotal - usedClusters - 1;   
    sb.rootDir = sb.usableClusterStart;

    //Write all zeros to the filesystem clusterMap since everything is free
    //Except the first cluster because it is used for the iNode of the root directory
    Block b;
    zero(&b, blockSize);
    b[0] = 0x80;
    if(!writeblock(diskNum, sb.clusterMap, &b))
        return false;
    b[0] = 0x00;
    for(int i = 1; i < sb.clusterMapBlockSize; i++)
        if(!writeblock(diskNum, sb.clusterMap + i, &b))
            return false;
    
    //copy the name over unless it is too long
    if(strlen(name) >  superBlockNameLength)
        return false;
    int i;
    for(i = 0; i < strlen(name); i++)
        sb.name[i] = name[i];
    for(; i < superBlockNameLength; i++)
        sb.name[i] = 0;

    //finally, write the SuperBlock to the disk
    return writeblock(diskNum, superBlockLocation, &sb);
} 

bool formatDiskFull(int diskNum)
{
    int numBlocks = disksize(diskNum);
    return formatDisk(diskNum, "Only Partition", numBlocks / 8);
}

//Creates a disk and formats it
bool createSimpleDisk(char *name, int numClusters)
{
    if(!createdisk(name, numClusters * clusterSize))
        return false;
    int diskNum = opendisk(name);
    if(diskNum == -1)
        return false;
    if(!formatDisk(diskNum, name, numClusters))
        return false;
    closedisk(diskNum);
    return true;
}
