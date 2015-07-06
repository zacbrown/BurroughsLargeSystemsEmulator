#include "simplefs.h"
#include <string>

bool buildRootDir(int discNum)
{
    SuperBlock sb;
    if(!readblock(discNum, superBlockLocation, &sb))
        return false;
    INode iNode = getInode(discNum, sb.rootDir);
    
}

bool createDir(char * name)
