#include <iostream>
#include <string>
#include "simplefs.h"

using namespace std;

int menuRoot()
{
    cout << "\n1.  Manipulate disk\n";
    cout << "2.  Create disk\n";
    cout << "3.  Mount disk\n";
    cout << "4.  Unmout disk\n";
    cout << "5.  Get disk size\n";
    cout << "6.  List mounted disks\n";
    cout << "7.  Exit\n";
    cout << "> ";
    int input;
    cin >> input;
    return input;
}

int menu(string name)
{
    cout << "\nUsing Disk \"" << name << "\"\n";
    cout << "1.  Format Disk\n";
    cout << "2.  Read SuperBlock of Disk\n";
    cout << "3.  Read an INode\n";
    cout << "4.  Get next free cluster\n";
    cout << "5.  Create iNode in a free cluster\n";
    cout << "6.  Write data to a file (iNode)\n";
    cout << "7.  Read data from a file (iNode)\n";
    cout << "8.  Use a different disk (Back)\n";
    cout << "> ";
    int input;
    cin >> input;
    return input;
}

class File
{
    public:
        int num;
        string name;
};

void putString(Block *block, string str)
{
    Byte * data = (Byte *) block;
    int i;
    for(i = 0; i < blockSize && i < str.length(); i++)
        data[i] = str[i];
    for(; i < blockSize; i++)
        data[i] = 0;
}

void display(Block *block, int len)
{
    for(int i = 0; i < blockSize && i < len; i++)
        cout << extractByte(block, i) << "(" << (int) extractByte(block, i) << ")\n";
}

void useDisk(File *file)
{
    SuperBlock sb;
    INode *iNode;
    Block block;

    int val, loc;
    int num = file->num;
    string fileName = file->name;
    string name;
    bool back = false;

    while(!back)
    {
        int input = menu(fileName);
        switch(input)
        {
            case 1:
                if(formatDiskFull(num))
                    cout << "Disk formatted.\n";
                else
                    cout << "Error: couldn't format.\n";
                break;

            case 2:
                if(!readblock(num, superBlockLocation, &sb))
                    cout << "Error: couldn't read the SuperBlock.\n";
                else
                {
                    cout << sb.name << endl;
                    cout << "rootDirLocation    = " << sb.rootDir << endl;
                    cout << "clusterMapLocation = " << sb.clusterMap << endl;
                    cout << "clusterMapSize     = " << sb.clusterMapBlockSize << endl;
                    cout << "numClustersTotal   = " << sb.numClustersTotal << endl;
                    cout << "usableClusterStart = " << sb.usableClusterStart << endl;
                    cout << "numClustersUsable  = " << sb.numClustersUsable << endl;
                }                
                break;

            case 3:
                cout << "Read iNode.\n";
                cout << "Block Location: ";
                cin >> loc;
                iNode = getINode(num, loc);
                if(iNode != NULL)
                {
                    cout << "File: " << iNode->name << " Owned by: " << iNode->owner << endl;
                    cout << "protection = " << iNode->protection << endl;
                    cout << "date       = " << iNode->date << endl;
                    cout << "depth      = " << iNode->depth << endl;
                    cout << "currentCap = " << iNode->currentCap << endl;
                    cout << "size       = " << iNode->size << endl;
                    //free(iNode);
                }
                else
                    cout << "Error: couldn't read iNode\n";
                break;

            case 4:
                cout << "Get next free cluster\nNumber: ";
                cin >> num;
                if(readblock(num, superBlockLocation, &sb))
                {
                    val = getFreeCluster(num, &sb);
                    if(val > 0)
                        cout << "Got block number " << val << endl;
                    else
                        cout << "Error: couldn't get free cluster.\n";
                }
                else
                    cout << "Error: couldn't read SuperBlock when trying to get free cluster.\n";
                break;

            case 5:
                cout << "File Name: ";
                cin >> name;
                val = createINode(num, name.c_str());
                if(val)
                    cout << "iNode \"" << name << "\" created at position " << val << endl;
                else
                    cout << "Error: iNode couldn't be created.\n";
                break;

            case 6:
                cout << "Write To File\n";
                cout << "iNode Position: ";
                cin >> loc;
                cout << "Block Number in File: ";
                cin >> val;
                cout << "Enter a string to put into the file: ";
                cin >> name;
                putString(&block, name);
                if(writeFileBlock(num, loc, val, &block))
                    cout << "Data written successfully.\n";
                else
                    cout << "Error: data couldn't be written.\n";
                break;

            case 7:
                cout << "Read From File\n";
                cout << "iNode Position: ";
                cin >> loc;
                cout << "Block Number in File: ";
                cin >> val;
                if(readFileBlock(num, loc, val, &block))
                    display(&block, 30);
                else
                    cout << "Error: data couldn't be read.\n";
                break;

            case 8:
                back = true;
                break;
            default:
                cout << "Invalid entry.\n";
                break;
        }
    }
}


list<File*> files;
list<File*>::iterator it;
File* file;

bool mountDisk(string name)
{
    int num = mountdisk(name.c_str());
    if(num > 0)
    {   
        file = new File();
        file->name = name;
        file->num = num;
        cout << "Added disk \"" << file->name << "\" at " << file->num << endl;
        cout.flush();
        files.push_back(file);
        return true;
    }
    cout << "Error: Couldn't mount disk.\n";
    return false;
}

int main(void)
{
    string name;
    int blocks, num, loc;
    bool exit = false;
    
    mountDisk("fsLarge.disc");

    while(!exit)
    {
        int inputRoot = menuRoot();
        switch(inputRoot)
        {
            case 1:
                cout << "Manipulate Disk\nNumber: ";
                cin >> num;
                for ( it=files.begin() ; it != files.end(); it++ )
                {
                    file = *it;
                    if(file->num == num)
                    {
                        useDisk(file);
                        break;
                    }
                }
                break;

            case 2:
                cout << "Create Disk\nName: ";
                cin >> name;
                cout << "Size in blocks: ";
                cin >> blocks;
                if(createdisk(name.c_str(), blocks))
                    cout << "Disk \"" << name << "\" created.\n";
                else
                    cout << "Error: Couldn't create disk.\n";
                break;

            case 3:
                cout << "Mount Disk\nName: ";
                cin >> name;
                mountDisk(name);
                break;

            case 4:
                cout << "Dismount Disk\nNumber: ";
                cin >> num;
                dismountdisk(num);
                for ( it=files.begin() ; it != files.end(); it++ )
                {
                    file = *it;
                    if(file->num == num)
                    {
                        files.erase(it);
                        break;
                    }
                }
                cout << "Disk " << num << " removed.\n";
                break;

            case 5:
                cout << "Get Size\nNumber: ";
                cin >> num;
                cout << disksize(num) << endl;
                break;

            case 6:
                for ( it=files.begin() ; it != files.end(); it++ )
                {
                    file = *it;
                    cout << file->name << " (" << file->num << ")\n";
                }
                break;

            case 7:
                exit = true;
                break;

            default:
                cout << "Invalid entry.\n";
                break;
        }
    }
}
