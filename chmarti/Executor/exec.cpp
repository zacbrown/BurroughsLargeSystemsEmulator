#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "../FileSystems/disc.h"
#include "../Assembler/functions.h"

using namespace std;

bool notDone = true;
unsigned int numInstructions, opcode, indirect, reg1, reg2, negative = 0;
int constant;
bool intOn;
map<string, int> loadedPrograms;
map<string, int> sizeInfo;

//To store floats in things declared as int
union Conv
{
    unsigned int numi;
    float numf;
};

Conv converter;

unsigned int floatToInt(float f)
{
    converter.numf = f;
    return converter.numi;
}

float intToFloat(unsigned int i)
{
    converter.numi = i;
    return converter.numf;
}


//stack pointer is in registers[14]
//program counter is in registers[15]

void clearRegs(void)
{
    for(int i = 0; i < NUMREGS; i++)
        registers[i] = 0;
}

int compareInt(int one, int two)
{
    if(one < two)
        return -1;
    if(one == two)
        return 0;
    return 1;
}

const int defaultChunkSize = 4096;

class Memory
{
    private:
        unsigned int numChunks, sizeChunks, maxAddrOffset, maxPageOffset, addrOffsetBits, pageOffsetBits;
        unsigned int **memory;
        int lastWriteOld, lastWriteNew, lastWriteAddr, lastReadAddr;

    public:
        unsigned int getBitSize(unsigned int size)
        {
            unsigned int bitSize = 1, i;
            for(i = 0; bitSize <= size; i++)
                bitSize *= 2;
            return i - 1;
        }

        unsigned int getSize(unsigned int bit)
        {
            unsigned int size = 1;
            for(int i = 0; i <= bit; i++)
                size *= 2;
            return size;
        }

        void init(const unsigned int num, const unsigned int size)
        {
            numChunks = num;
            sizeChunks = size;
            maxAddrOffset = sizeChunks - 1;
            addrOffsetBits = getBitSize(sizeChunks);
            pageOffsetBits = 20 - addrOffsetBits; //Max of 2^8 = 256 pages per running process
            maxPageOffset = getSize(pageOffsetBits) - 1;
            memory = new unsigned int*[num];

            lastWriteOld = 0;
            lastWriteNew = 0;
            lastWriteAddr = 0;
            lastReadAddr = 0;
        }

        Memory(const unsigned int num, const unsigned int size)
        {
            init(num, size);
        }

        Memory(unsigned const int size)
        {
            init((size % defaultChunkSize == 0) ? size / defaultChunkSize : (size / defaultChunkSize) + 1, defaultChunkSize);
        }

        Memory()
        {
            init(12, defaultChunkSize);
        }

        unsigned int getSize()
        {
            return numChunks * sizeChunks;
        }

        void destroy()
        {
            int i;
            for(i = 0; i < numChunks; i++)
                if(memory[i] != NULL)
                    delete[] memory[i];
            delete[] memory;
        }

        void erase()
        {
            destroy();
            init(numChunks, sizeChunks);
        }

        unsigned int readDirect(const unsigned int addr)
        {
            int temp = addr / sizeChunks;
            if(memory[temp] == NULL)
            {
                memory[temp] = new unsigned int[sizeChunks];
                for (int i = 0; i < sizeChunks; i++)
				    memory[temp][i] = 0;            
            }

            lastReadAddr = addr;

            return memory[temp][addr % sizeChunks];
        }

        unsigned int translate(unsigned int pageTableLoc, const unsigned int addr)
        {
            return readDirect(pageTableLoc + ((addr >> addrOffsetBits) & maxPageOffset));
        }

        int read(unsigned int pageTableLoc, const unsigned int addr)
        {
            //We dont have an OS so don't use page translation table
            return readDirect(addr);
            /*
            unsigned int translatedAddr = translate(pageTableLoc, addr);
            if(translatedAddr == 0)
            {
                //The OS needs to add this page for the process... this should be done via interrupts
                //return an indicator of this
                return -1;
            }
            else //otherwise read the actual data
                return readDirect(translatedAddr + (addr & maxAddrOffset));*/
        }

        int writeDirect(const unsigned int addr, const unsigned int data)
        {
            int chunk = addr / sizeChunks, pos = addr % sizeChunks;    
            if(memory[chunk] == NULL)
                memory[chunk] = new unsigned int[sizeChunks];            

            lastWriteAddr = addr;
            lastWriteOld = memory[chunk][pos];
            lastWriteNew = data;

            memory[chunk][pos] = data;
            return 1;
        }

        int write(unsigned int pageTableLoc, const unsigned int addr, const unsigned int data)
        {
            //We dont have an OS so don't use page translation table
            writeDirect(addr, data);
            /*
            unsigned int translatedAddr = translate(pageTableLoc, addr);
            if(translatedAddr == 0)
            {
                //The OS needs to add this page for the process... this should be done via interrupts
                //return an indicator of this
                return -1;
            }
            else
                return writeDirect(translatedAddr + (addr & maxAddrOffset), data);*/
        }

        int getLastWriteAddr()  { return lastWriteAddr; }
        int getLastReadAddr()  { return lastReadAddr; }
        int getLastWriteOld()  { return lastWriteOld; }
        int getLastWriteNew()  { return lastWriteNew; }
};

class Flags
{
    private:
        bool flags[7];
    
    public:
        Flags() { for(int i = 0; i < sizeof(flags); i++) flags[i] = 0; }

        void setFlag(unsigned int flag, bool val) { flags[flag] = val; }

        bool getFlag(unsigned int flag) { return flags[flag]; }

        bool* getFlags() { return flags; }

        unsigned int clearFlags() { for(int i = 0; i < sizeof(flags); i++) flags[i] = 0; };

        void setFlagsCmp(int val)
        {
            if(val < 0)
            {
                setFlag(FLAG_E, false);
                setFlag(FLAG_L, true);
                setFlag(FLAG_G, false);
                setFlag(FLAG_GE, false);
                setFlag(FLAG_LE, true);
                setFlag(FLAG_NE, true);
            }
            else if(val == 0)
            {
                setFlag(FLAG_E, true);
                setFlag(FLAG_L, false);
                setFlag(FLAG_G, false);
                setFlag(FLAG_GE, true);
                setFlag(FLAG_LE, true);
                setFlag(FLAG_NE, false);
            }
            else
            {
                setFlag(FLAG_E, false);
                setFlag(FLAG_L, false);
                setFlag(FLAG_G, true);
                setFlag(FLAG_GE, true);
                setFlag(FLAG_LE, false);
                setFlag(FLAG_NE, true);
            }
        }

        void setFlagsOp(int val)
        {
            if(val == 0)
                setFlag(FLAG_Z, true);
            else
                setFlag(FLAG_Z, false);
        }
};

Flags *flags = new Flags();
Memory *memory = new Memory();
void execute();
bool executeNext();

string readString(int addr)
{
    string thestring = "";
    bool done = false;
    int data;
    for(; !done; addr++)
    {
        data = memory->read(registers[REG_PAGE], addr);
        char c[4];
        c[0] = (char) ((data >> 24) & 0x000000FF);
        c[1] = (char) ((data >> 16) & 0x000000FF);
        c[2] = (char) ((data >> 8) & 0x000000FF);
        c[3] = (char) (data & 0x000000FF);
        
        for(int i = 0; i < 4 && !done; i++)
        {
            if(c[i] == 0)
                done = true;
            else
                thestring += c[i];
        }
    }
    return thestring;
}

unsigned int extractInt(void *d, unsigned int pos)
{
    Byte * data = (Byte *) d;
    return *((unsigned int *) &data[pos]);
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

/*
void savePCB(unsigned int location)
{
    for(int i = 0; i < 16; i++)
        memory->writeDirect(location + i, registers[i]);
    memory->writeDirect(location + 16, flags->getFlags());
}

void loadPCB(unsigned int location)
{


}*/


//When reading from stdin op = 0 and dev = 0 but the others dont matter. reg1 give the char read

//When writing to stdout or stderr the block field is the type of data.
//If the type is char or int addr is the data. If the type is string addr is the location of the null terminated data
//Block Field  IOTYPE_CHAR = 0, IOTYPE_INT = 1, IOTYPE_STRING = 2;

int handleIO(int location)
{
    Block b;
    int op = memory->read(registers[REG_PAGE], location), dev = memory->read(registers[REG_PAGE], location + 1), 
        block = memory->read(registers[REG_PAGE], location + 2), addr = memory->read(registers[REG_PAGE], location + 3);
    string name;
    unsigned int num = 0;
    unsigned char val;
    float fnum;

    switch(op)
    {
        case 0: //Read Block
            if(dev == 0)//handle stdin
            {   
                switch(block)
                {
                    case IOTYPE_CHAR:
                        cin >> val;
                        num = (int) val;
                        break;
                    
                    case IOTYPE_INT:
                        cin >> num;
                        break;
    
                    case IOTYPE_FLOAT:
                        cin >> fnum;
                        num = floatToInt(fnum);
                        break;
                    
                    case IOTYPE_STRING:
                        cin >> name;
                        break;
                    
                    default:
                        num = 0;
                }
            }
            else
            {
                num = readblock(dev, block, &b);

                for(int i = 0; i < blockSize / 4; i++)
                    memory->write(registers[REG_PAGE], addr + i, extractInt(&b, i * 4));
            }

            break;
        
        case 1: //Write Block  
            if(!(dev == 1 || dev == 2) && block == IOTYPE_FLOAT)
            {
                for(int i = 0; i < blockSize / 4; i++)
                    putInt(&b, i * 4, memory->read(registers[REG_PAGE], addr + i)); 
            }
            if(dev == 1)//handle stdout
            {
                num = 1;
                switch(block)
                {
                    case IOTYPE_CHAR:
                        cout << (char) addr;
                        break;
                    
                    case IOTYPE_INT:
                        cout << (int) addr;
                        break;

                    case IOTYPE_FLOAT:
                        cout << intToFloat(addr);
                        break;
    
                    case IOTYPE_STRING:
                        cout << readString(addr);
                        break;
                    
                    default:
                        num = 0;
                }
            } 
            else if(dev == 2)//handle stdout
            {
                num = 1;
                switch(block)
                {
                    case IOTYPE_CHAR:
                        cerr << (char) addr;
                        break;
                    
                    case IOTYPE_INT:
                        cerr << (int) addr;
                        break;

                    case IOTYPE_FLOAT:
                        cerr << intToFloat(addr);
                        break;
    
                    case IOTYPE_STRING:
                        cerr << readString(addr);
                        break;
                    
                    default:
                        num = 0;
                }
            } 
            else
            {
                for(int i = 0; i < blockSize / sizeof(int); i++)
                    putInt(&b, 4 * i, memory->read(registers[REG_PAGE], addr + i));
                num = writeblock(dev, block, &b);
            }
            break;
        
        case 2: //Mount
            name = readString(addr);
            num = mountdisk(name.c_str());
            break;
        
        case 3: //Unmount or Dismount
            num = dismountdisk(dev);
            break;
        
        case 4: //Create
            name = readString(addr);
            num = createdisk(name.c_str(), block);
            break;
    }
    return num;
}

void showProgs(void)
{
    cout << "Loaded Programs:\n";
    map<string, int>::iterator iter;
    for(iter = loadedPrograms.begin(); iter != loadedPrograms.end(); iter++)
        cout << "\t" << iter->first << endl;
}

int loadFile(string filename, int position)
{
    int f = open(filename.c_str(), O_RDONLY);
    unsigned int addr, temp;
    if(f < 0)
    {
        cout << "EXECUTOR error: Can not open file \"" << filename << "\".\n";
        return -1;
    }
    else if(position < 0)
    {
        cout << "EXECUTOR error: The file position \"" << position << "\" is invalid.\n";
        return -1;
    }

    for(addr = 0; true; addr++)
    {
        int n = read(f, &temp, sizeof(temp));
        if(n <= 0)
            break;
        memory->write(registers[REG_PAGE], addr + position, temp);
    }
    memory->write(registers[REG_PAGE], addr + position, 0);
    cout << "EXECUTOR: " << filename << " loaded into memory at " << position << " (" << addr << " instructions).\n";
    loadedPrograms.insert(make_pair(filename, position));
    sizeInfo.insert(make_pair(filename, addr));
    return numInstructions;
}

bool getProgramReady(string name)
{
    map<string, int>::iterator iter = loadedPrograms.find(name);
    map<string, int>::iterator iter2 = sizeInfo.find(name);
    if(iter != loadedPrograms.end() && iter2 != sizeInfo.end())
    {
        clearRegs();
        flags->clearFlags();
        registers[REG_PC] = iter->second; //Should be handled by the OS
        registers[REG_SP] = memory->getSize(); //probably not going to work now that virtual memory is being used
        registers[11] = iter2->second + 1;
        notDone = true;
        intOn = true;
    }
    else
    {
        cout << "EXECUTOR error: Unknown program \"" << name << "\"\n";
        return false;
    }
    return true;
}

bool runProgram(string name)
{
    bool val = getProgramReady(name);
    if(val)
        execute();
    return val;
}

void printHelp()
{
    cout << "------------\n";
    cout << "Command Help\n";
    cout << "------------\n";
    cout << "HELP - Displays this message.\n";
    cout << "LOAD [binary file] - Loads the binary file.\n";
    cout << "RUN [binary file] - Runs the binary file without any debugging.\n";
    cout << "DEBUG [binary file] - Gets the binary file ready to run and allows debugging commands.\n";
    cout << "STEP [number] - Execute the next [number] of binary commands.\n";
    cout << "SHOW PROGS - Shows the programs currently loaded into memory.\n";
    cout << "SHOW REG [number] - Shows the register contents.\n";
    cout << "SHOW MEM [low] [high] - Shows memory contents in the range low to high (Base 16).\n";
    cout << "SHOW SP CLOSE - Shows the stack values close to the current SP.\n";
    cout << "SHOW LASTMEM - Shows what the last change to memory was.\n";
    cout << "SHOW PC - Shows the program counter value, the previous and next commands.\n";
    cout << "EXIT - Exits EXECUTOR\n\n";
}

void showMem(string input, bool reverse)
{
    string value = "";
    unsigned int low = -1, high = -1;
    stringstream iss(input);
    
    //get rid of SHOW REG
    iss >> value >> value;

    //Read low and high
    iss >> value;
    low = myatol(value);
    iss >> value;

    if(!iss.fail())
        high = myatol(value);
    else
        high = low;
    
    if(low == -1 || high == -1 || low > high)
        cout << "Error with command format!" << endl;
    else
    {
        if(reverse)
            for(; low <= high; high--)
                cout << "0x" << setbase(16) << high << " = 0x" << memory->read(registers[REG_PAGE], high)
                     << setbase(10) << "\t\t" << high << " = " << memory->read(registers[REG_PAGE], high) << endl;
        else
            for(; low <= high; low++)
                cout << "0x" << setbase(16) << low << " = 0x" << memory->read(registers[REG_PAGE], low) 
                     << setbase(10) << "\t\t" << low << " = " << memory->read(registers[REG_PAGE], low) << endl;    
    }
}

void showReg(string input)
{
    unsigned int reg;
    string value;

    stringstream iss(input);

    //get rid of SHOW REG
    iss >> value >> value;
    
    //Read register number or ALL
    iss >> value;
    if(value == "ALL" || value == "All" || value == "all")
    {
        int i;
        for(i = 0; i < NUMREGS; i++)
            cout << "REG " << setbase(10) << i << " = 0x" << setbase(16) << registers[i] << setbase(10) << endl; 
    }
    else
    {
        reg = atol(value.c_str());
        if(reg >= 0 && reg < NUMREGS)
            cout << "REG " << setbase(10) << reg << " = 0x" << setbase(16) << registers[reg] << setbase(10) << endl; 
        else
            cout << "There is no register \"" << value << "\". Only numbers 0 - 19 are valid." << endl;
    }
}

string extractFileName(string text)
{
    int pos1 = text.find(' ');
    if(pos1 == string::npos)
        return "";
    int pos2 = text.find(' ', pos1 + 1);
    if(pos2 == string::npos)
        pos2 = text.length();
    pos1++;
    return text.substr(pos1, pos2 - pos1); 
}

int extractFilePosition(string text)
{
    int pos = text.find(' ');
    if(pos == string::npos)
        return -1;
    pos = text.find(' ', pos + 1);
    if(pos == string::npos)
        return -1;
    pos++;
    return myatol(text.substr(pos, text.length() - pos));
}

void showInstr()
{
    cout << "Last Instruction Executed:\n";
    cout << "Opcode = " << opcode << " (" << opcodes[opcode] << ")\n";
    cout << "Indirect = " << indirect << "\n";
    cout << "Register1 = " << reg1 << "\n";
    cout << "Register2 = " << reg2 << "\n";
    cout << "Negative = " << negative << "\n";
    cout << "Constant = " << constant << "\n";
}

void showSPClose()
{
    int end = registers[REG_SP], max = memory->getSize(), start;
    if(end + 8 > max)
        start = max;
    else
        start = end + 8;
    for(int i = start; i >= end; i--)
        cout << "0x" << setbase(16) << i << " = 0x" << memory->read(registers[REG_PAGE], i) 
             << setbase(10) << "\t\t" << i << " = " << memory->read(registers[REG_PAGE], i) << endl;
}

bool executeNext()
{

    /*
    if(intOn && 1)
    {
        intOn = false;
        savePCB();
        handleInterrupt();
    }*/

    //cout << "PC = " << pc << endl;
    unsigned int cur = memory->read(registers[REG_PAGE], registers[REG_PC]);
    registers[REG_PC] += 1;

    //printf("cur = %08x\n", cur);
    opcode = (cur >> 25) & 0x7F;
    indirect = (cur >> 24) & 0x1;
    reg1 = (cur >> 20) & 0xF;
    reg2 = (cur >> 16) & 0xF;
    negative = (cur >> 15) & 0x0001;
    constant = cur & 0x7FFF;
    //printf("opcode = %i\n", opcode);
    //printf("indirect = %i\n", indirect);
    //printf("reg1 = %i\n", reg1);
    //printf("reg2 = %i\n", reg2);
    //printf("negative = %i\n", negative);
    //printf("constant = %i\n", constant);
    if(negative == 1)
        constant = -1 * constant;

    unsigned int effectiveAddr;
    /*
    if(isOpSingle(opcode))
    {
        if(reg1 == 0)
            effectiveAddr = constant;
        else
            effectiveAddr = registers[reg1] + constant;
    }
    else
    {*/
        if(reg2 == 0)
            effectiveAddr = constant;
        else
            effectiveAddr = registers[reg2] + constant;
    //}

    if(indirect)
        effectiveAddr = memory->read(registers[REG_PAGE], effectiveAddr);

    switch(opcode)
    {
        case OP_ADD:
            registers[reg1] += effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_SUB:
            registers[reg1] -= effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;
    
        case OP_SUBR:
            registers[reg1] = effectiveAddr - registers[reg1];
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_DIV:
            registers[reg1] /= effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_DIVR:
            registers[reg1] = effectiveAddr / registers[reg1];
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_MUL:
            registers[reg1] *= effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;

        case OP_MOD:
            registers[reg1] %= effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;

        case OP_STORE:
            memory->write(registers[REG_PAGE], effectiveAddr, registers[reg1]);
            break;       
 
        case OP_LOADH:
            registers[reg1] &= 0x00FF; 
            registers[reg1] |= (effectiveAddr << 16) && 0xFF00;
            break;

        case OP_LOADL:
            registers[reg1] &= 0xFF00;
            registers[reg1] |= effectiveAddr && 0x00FF;
            break;
        
        case OP_LOAD:
            registers[reg1] = effectiveAddr;
            break;       
        
        case OP_MOV:
            registers[reg1] = registers[reg2];
            break;     

        case OP_CMP:
            flags->setFlagsCmp(compareInt(registers[reg1], effectiveAddr));
            break;
        
        case OP_JMP:
            registers[REG_PC] = effectiveAddr;
            break;        

        case OP_JMPE:
            if(flags->getFlag(FLAG_E))
                registers[REG_PC] = effectiveAddr;
            break;
        
        case OP_JMPNE:
            if(!flags->getFlag(FLAG_E))
                registers[REG_PC] = effectiveAddr;
            break;

        case OP_JMPL:
            if(flags->getFlag(FLAG_L))
                registers[REG_PC] = effectiveAddr;
            break;

        case OP_JMPG:
            if(flags->getFlag(FLAG_G))
                registers[REG_PC] = effectiveAddr;
            break;

        case OP_JMPGE:
            if(flags->getFlag(FLAG_G) || flags->getFlag(FLAG_E))
                registers[REG_PC] = effectiveAddr;
            break;

        case OP_JMPLE:
            if(flags->getFlag(FLAG_L) || flags->getFlag(FLAG_E))
                registers[REG_PC] = effectiveAddr;
            break;
        
        case OP_PRINT:
            cout << effectiveAddr;
            break;
            
        case OP_NEWL:
            //Very unrealistic
            cout << endl;
            break;

        case OP_CALL:
            //memory->write(registers[REG_SP], registers[REG_FP]); //Put FP on the stack
            memory->write(registers[REG_PAGE], registers[REG_SP], registers[REG_PC]); //Put ret addr on the stack
            //registers[REG_FP] = registers[REG_SP] - 1;
            registers[REG_SP] -= 1;
            registers[REG_PC] = effectiveAddr;
            break;
        
        case OP_RET:
            registers[REG_SP] += 1;
            registers[REG_PC] = memory->read(registers[REG_PAGE], registers[REG_SP]); //Get return addr
            //registers[REG_FP] = memory->read(registers[REG_FP]); //Get old fp location
            break;
        
        case OP_PUSH:
            memory->write(registers[REG_PAGE], registers[REG_SP], effectiveAddr);
            registers[REG_SP] -= 1;
            break;

        case OP_POP:
            registers[REG_SP] += 1;
            registers[reg1] = memory->read(registers[REG_PAGE], registers[REG_SP]);
            break;

        case OP_HALT:
            return false;
            break;

        case OP_IOOP:
            registers[reg1] = handleIO(effectiveAddr);
            break;

        case OP_IRET:
            /*
            loadPCB();
            intOn = true;*/
            break;

        case OP_AND:
            registers[reg1] = registers[reg1] && effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;            
            
        case OP_OR:
            registers[reg1] = registers[reg1] || effectiveAddr;
            flags->setFlagsOp(registers[reg1]);
            break;

        case OP_FLAG:
            registers[reg1] = flags->getFlag(effectiveAddr);
            break;

        case OP_FADD:
            registers[reg1] = floatToInt(intToFloat(registers[reg1]) + intToFloat(effectiveAddr));
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_FSUB:
            registers[reg1] = floatToInt(intToFloat(registers[reg1]) - intToFloat(effectiveAddr));
            flags->setFlagsOp(registers[reg1]);
            break;
    
        case OP_FSUBR:
            registers[reg1] = floatToInt(intToFloat(effectiveAddr) - intToFloat(registers[reg1]));
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_FDIV:
            registers[reg1] = floatToInt(intToFloat(registers[reg1]) / intToFloat(effectiveAddr));
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_FDIVR:
            registers[reg1] = floatToInt(intToFloat(effectiveAddr) / intToFloat(registers[reg1]));
            flags->setFlagsOp(registers[reg1]);
            break;
        
        case OP_FMUL:
            registers[reg1] = floatToInt(intToFloat(registers[reg1]) * intToFloat(effectiveAddr));
            flags->setFlagsOp(registers[reg1]);
            break;

        case OP_CAST:
            switch(effectiveAddr)
            {
                case IOTYPE_CHAR:
                case IOTYPE_INT:
                    registers[reg1] = (int) intToFloat(registers[reg1]);
                    break;

                case IOTYPE_FLOAT:
                    registers[reg1] = floatToInt((float) registers[reg1]);
                    break;
            }
            break;

        default:
            cout << "EXECUTOR: Error. Unrecognized opcode (" << opcode << ")!\n";
            return false;
            break;
    }

    return true;
}

void step(string input)
{
    unsigned int num;
    string value;

    stringstream iss(input);

    //get rid of SHOW REG
    iss >> value >> value;
    if(iss.fail())
        num = 1;
    else
    {
        num = myatol(value);
        if(num == -1)
        {   
            cout << "EXECUTOR: Error. Incorrect command format!" << endl;
            return;
        }
    }

    if(!notDone)
    {
        cout << "EXECUTOR: Error. The HALT command has already been reached.\n";
        return;
    }

    for(; num > 0; num--)
    {
        notDone = executeNext();
        if(!notDone)
            break;   
    }
}

void execute()
{
    if(!notDone)
    {
        cout << "EXECUTOR: Error. The HALT command has already been reached.\n";
        return;
    }

    while(true)
    {
        notDone = executeNext();
        if(!notDone)
            break;   
    }
    cout << "EXECUTOR: Execution complete!\n";
}

int main(int argc, char *argv[])
{
    int infile;

    //If a binary is specified run it normally, otherwise enter interactive mode
    if(argc < 2)
    {
        string input;
        while(true)
        {
            cout << "[EXECUTOR]$ ";
            getline(cin, input);
            if(input.find("STEP") != string::npos || input.find("step") != string::npos || input.find("Step") != string::npos)
            {
                step(input);
            }
            else if(input.find("SHOWR MEM ") != string::npos || input.find("showr mem ") != string::npos || input.find("Showr Mem ") != string::npos)
                showMem(input, true);
            else if(input.find("SHOW MEM ") != string::npos || input.find("show mem ") != string::npos || input.find("Show Mem ") != string::npos)
                showMem(input, false);
            else if(input.find("SHOW PROGS") != string::npos || input.find("show progs") != string::npos || input.find("Show Progs") != string::npos)
                showProgs();
            else if(input.find("SHOW REG ") != string::npos || input.find("show reg ") != string::npos || input.find("Show Reg ") != string::npos)
            {
                showReg(input);
            }
            else if(input == "SHOW LASTMEM" || input == "show lastmem" || input == "Show Lastmem")
            {
                cout << "Last memory access at 0x" << setbase(16) << memory->getLastReadAddr() << endl;
                cout << "Last memory write at 0x" << memory->getLastWriteAddr() << endl;
                cout << "Old value = 0x" << memory->getLastWriteOld() << endl;
                cout << "New value = 0x" << memory->getLastWriteNew() << setbase(10) << endl;
            }
            else if(input == "SHOW SP CLOSE" || input == "show sp close" || input == "Show SP Close")
                showSPClose();
            else if(input == "SHOW SP" || input == "show sp" || input == "Show SP")
                cout << "SP = " << registers[REG_SP] << endl;
            else if(input == "SHOW PC" || input == "show pc" || input == "Show PC")
                cout << "PC = " << registers[REG_PC] << endl;
            else if(input == "SHOW FP" || input == "show fp" || input == "Show FP")
                cout << "FP = " << registers[REG_FP] << endl;
            else if(input == "SHOW FLAGS" || input == "show flags" || input == "Show Flags")
            {
                cout << "Equal = " << flags->getFlag(FLAG_E) << "\n";
                cout << "Less = " << flags->getFlag(FLAG_L) << "\n";
                cout << "Grader = " << flags->getFlag(FLAG_G) << "\n";
                cout << "Less or Equal = " << flags->getFlag(FLAG_LE) << "\n";
                cout << "Grader or Equal = " << flags->getFlag(FLAG_GE) << "\n";
                cout << "Not Equal = " << flags->getFlag(FLAG_NE) << "\n";
            }
            else if(input == "SHOW INSTR" || input == "show instr" || input == "Show Instr")
                showInstr();
            else if(input.find("LOAD ") != string::npos || input.find("Load ") != string::npos || input.find("load ") != string::npos)
                loadFile(extractFileName(input), extractFilePosition(input));
            else if(input.find("RUN ") != string::npos || input.find("Run ") != string::npos || input.find("run ") != string::npos)
                runProgram(extractFileName(input));
            else if(input.find("DEBUG ") != string::npos || input.find("debug ") != string::npos || input.find("Debug ") != string::npos)
                getProgramReady(extractFileName(input));
            else if(input == "HELP" || input == "help" || input == "Help")
                printHelp();
            else if(input == "EXIT" || input == "Exit" || input == "exit")
                break;
            else if(input == "")
                ;
            else
                cout << "The command \"" << input << "\" was not found!\nFor a list of commands type \"HELP\"\n";
        }
    }   
    else if(argc < 4)
    {
        int loadPos = 0;
        if(argc == 3)
            loadPos = myatol((string) argv[2]);
        else
            cout << "EXECUTOR warning: You did not enter an offset so " << loadPos << " will be used\n";

        if(loadFile((string) argv[1], loadPos) != -1)
            runProgram((string) argv[1]);
    }
    else
        cout << "EXECUTOR error: Too many arguments\n"; 

    memory->destroy();
    delete[] memory;
    delete[] flags;
}
