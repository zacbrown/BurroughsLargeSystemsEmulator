#include "fsStructures.h"
#include <string>
#include <list>

using namespace std;

class ReiserFileDes
{
    public:
    short int start, length, fileNum, partNum;

    ReiserFileDes()
    {
    }

    short int getPart(Byte high, Byte low)
    {
        return ((high << 8) & 0xFF00) | low;
    }

    bool get(Byte *&data, int pos)
    {
        int location = pos * 8;
        start = getPart(data[location], data[location + 1]);
        length = getPart(data[location + 2], data[location + 3]);
        fileNum = getPart(data[location + 4], data[location + 5]);
        partNum = getPart(data[location + 6], data[location + 7]);
        if(start == 0)
            return false;
        return true;
    }

    ReiserFileDes(Byte *&data, int pos)
    {
        get(data, pos);
    }

    bool put(Byte *&data, int pos)
    {
        int location = pos * 8;
        data[location] = (Byte) (start >> 8);
        data[location + 1] = (Byte) (start & 0x00FF);
        data[location + 2] = (Byte) (length >> 8);
        data[location + 3] = (Byte) (length & 0x00FF);
        data[location + 4] = (Byte) (fileNum >> 8);
        data[location + 5] = (Byte) (fileNum & 0x00FF);
        data[location + 6] = (Byte) (partNum >> 8);
        data[location + 7] = (Byte) (partNum & 0x00FF);
        return true;
    }
};

class ReiserFileDesList
{
    public:

    list<ReiserFileDes*> fileDesList;

    ReiserFileDesList()
    {
    }

    bool buildDesList(Byte *&data)
    {
        ReiserFileDes *fd;
        for(int i = 0; true; i++)
        {
            fd = new ReiserFileDes();
            if(!fd->get(data, i))
                break;
            fileDesList.push_front(fd);
        }
    }

    ReiserFileDesList(Byte *&data)
    {
        buildDesList(data);
    }

    bool destroyDesList()
    {
        ReiserFileDes* fd;
        for(list<ReiserFileDes*>::iterator cur = fileDesList.begin(); cur != fileDesList.end(); cur++)
        {
            fd = *cur;
            delete fd;
        }
        fileDesList.empty();
        return true;
    }

    bool writeDesList(Byte *&data)
    {
        ReiserFileDes *fd;
        int i = 0;
        for(list<ReiserFileDes*>::iterator cur = fileDesList.begin(); cur != fileDesList.end(); cur++)
        {
            fd = *cur;
            if(fd->put(data, i) == false)
                return false;
            i++;
        }
        return true;
    }
};

class ReiserData : public Cluster, public ReiserFileDesList
{
    ReiserData()
    {
        Cluster();
        ReiserFileDesList();
    }

    ReiserData(int dn, int loc)
    {
        Cluster(dn, loc);
        ReiserFileDesList(data);
    }
};

int formatFS(int disc)
{

}

int formatFS(const char *name)
{
    int disc = mountdisc(name);
    if(disc != -1)
        return formatFS(disc);
    return -1;    
}

