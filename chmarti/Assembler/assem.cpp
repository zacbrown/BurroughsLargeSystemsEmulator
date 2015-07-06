#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "functions.h" 

using namespace std;

bool DEBUG = false;

//Used to format tabs
string getTab(int num)
{
    string text = "";
    for(; num >= 0; num--)
        text += "   ";
    return text;
}

class LabelInfo
{
    public:
	    
    int position;
    bool isConst;

    LabelInfo(void)
    {
        position = 0;
	    isConst = false;
    }

    LabelInfo(int p, bool i = false)
    {
        position = p;
	    isConst = i;
    }
};

//Gets the directive number of the command. -1 if error and 0 is the command doesn't have a '.'
int getDirective(string line, int indent, bool buildLabels = false)
{
    string text;
    int pos2, pos1 = line.find(".", 0);
    if(pos1 == string::npos)
        return 0;
    else
    { 
        pos2 = line.find(" ", pos1);
        if(pos2 == string::npos)
            pos2 = line.length();
        text = line.substr(pos1, pos2 - pos1);
        int i;
        for(i = 0; i < NUMDIRS; i++)
        {
            if(text.compare(directives[i]) == 0)
            {
                if(DEBUG && !buildLabels)
                    cout << getTab(indent) << "getDirective returning " << i << "\n";
                return i;
            }
        }
    }
    if(DEBUG && !buildLabels)
        cout << getTab(indent) << "getDirective returning -1\n";
    return -1;
}

//Gets the opcode of the command or returns -1 if there is an error
int getOpcode(string text, int indent, bool buildLabels = false)
{
    if(DEBUG && !buildLabels)
        cout << getTab(indent) << "CALLING getOpcode with \"" << text << "\"\n";
    int i;
    for(i = 0; i < NUMOPS; i++)
        if(text.compare(opcodes[i]) == 0)
            return i;
    if(DEBUG && !buildLabels)
        cout << getTab(indent) << "getOpcode is returning -1 for string \"" << text << "\"\n";
    return -1;
}

//returns the value of the register or -1 if it isn't a valid register specification
int getRegister(string text, int indent, bool buildLabels = false)
{
    if(DEBUG && !buildLabels)
        cout << getTab(indent) << "CALLING getRegister with \"" << text << "\"\n";
    if(text == "SP")
        return REG_SP;
    else if(text == "PC")
        return REG_PC;
    else if(text == "FP")
        return REG_FP;
    else if(text[0] != 'R' && text[0] != 'r')
    {
        if(DEBUG && !buildLabels)
            cout << getTab(indent) << "getRegister is returning -1 for string \"" << text << "\"\n";
        return -1;
    }

    int val = myatol(text.substr(1));
    if (val > NUMREGS - 1)
    {
        if(DEBUG && !buildLabels)
            cout << getTab(indent) << "getRegister is returning -1 for string \"" << text << "\"\n";
        return -1;
    }

    return val;
}

map<string, LabelInfo> labels;

//returns the value of the constant or -1 if it isn't valid
int getConstant(string text, int indent, bool checkLarge, bool buildLabels = false)
{
    if(DEBUG && !buildLabels)
        cout << getTab(indent) << "CALLING getConstant with \"" << text << "\"\n";
    int len = text.length();
    char lastchar = text[len - 1];
 
    if(lastchar == 'h' || lastchar == 'l')
        text.erase(len - 1);

    int value = myatol(text);
    if(value == -1)
    {   
        if(DEBUG && !buildLabels)
            cout << getTab(indent) << "getConstant found an error for string \"" << text << "\"\n";
        return -1;
    }

    if(lastchar == 'h')
        return (value >> 16) & 0xFFFF;
    else if(lastchar == 'l')
        return value & 0x0000FFFF;

    unsigned int maxvalue;    
    if(checkLarge)
        maxvalue = 0x7FFF;
    else
        maxvalue = 0xFFFFFFFF;
    if(value > maxvalue)
    {
        if(DEBUG && !buildLabels)
            cout << getTab(indent) << "getConstant found a number that was too large \"" << text << "\"\n";
        return -1;
    }
    return value;    
}

int getBinary(string line, int indent, int command, int offset, bool buildLabels = false)
{
    int opcode = 0, indirect = 0, reg1 = 0, reg2 = 0, constant = 0;
    bool negative = false, good = true, seenOpcode = false, seenFirstReg = false, seenSecondReg = false, seenConst = false, seenLabel = false;
    bool seenSign = false, noOffset = false;
    string temp;
    LabelInfo info;

    istringstream iss(line);
    while(true)
    {
        iss >> temp;
        if(iss.fail())
            break;

        if(DEBUG && !buildLabels)
            cout << getTab(indent) << "Dealing with part \"" << temp << "\"\n";
        //Remove comma if it is there
        int len = temp.length();
        if(temp[len - 1] == ',')
        {
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Removing comma.\n";
            temp.erase(len - 1);
            len--;
        }

        //Figure out what part it is and deal with it
        if(temp[len - 1] == ':')
        {
            //deal with labels
            if(DEBUG && buildLabels)
                cout << getTab(indent) << "Adding label \"" << temp << "\" (" << command + offset << ") \n";
            if(buildLabels)
                labels.insert(make_pair(string(temp.substr(0, len - 1)), LabelInfo(command, false)));
        }
        else if(!seenOpcode && getOpcode(temp, indent + 1, buildLabels) != -1) 
        {
            opcode = getOpcode(temp, indent + 1, buildLabels);
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found opcode \"" << opcode << "\" (" << opcodes[opcode] << ").\n";
            seenOpcode = true;
            if(isOpSingle(opcode))
                seenFirstReg = true;
        }
        else if(!seenFirstReg && getRegister(temp, indent + 1, buildLabels) != -1)
        {
            reg1 = getRegister(temp, indent + 1, buildLabels);
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found register \"" << reg1 << "\"\n";
            seenFirstReg = true;
        }
        else if(!seenSecondReg && getRegister(temp, indent + 1, buildLabels) != -1)
        {
            reg2 = getRegister(temp, indent + 1, buildLabels);
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found register2 \"" << reg2 << "\"\n";
            seenSecondReg = true;
        }
        else if(!seenConst && getConstant(temp, indent + 1, true, buildLabels) != -1)
        {
            if(seenLabel)
            {
                if(seenSign)
                {
                    if(negative)
                        constant = constant - getConstant(temp, indent + 1, true, buildLabels);
                    else
                        constant = constant + getConstant(temp, indent + 1, true, buildLabels);
                }
                else
                {
                    if(!buildLabels)
                        cout << "ASSEM error: Missing operator ( + or - )\n";
                    return -1;
                }
            }            
            else
                constant = getConstant(temp, indent + 1, true, buildLabels);
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found constant \"" << constant << "\"\n";
            seenConst = true;
        }
        else if(!seenLabel && labels.find(temp) != labels.end())
        {
            map<string, LabelInfo>::iterator iter = labels.find(temp);
            info = iter->second;
            //cout << "position = " << info.position << " command = " << command << endl;
            if(info.isConst || noOffset)
	            constant = info.position;
	        else
	        {
                constant = info.position - command - 1;
                seenSign = true;
                reg2 = REG_PC;
            }
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found label \"" << temp << "\" (" << constant << ")\n";
            seenLabel = true;
        }
        else if(!seenSign && temp == "+")
        {
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found \"+\"\n";
            seenSign = true;
        }
        else if(!seenSign && temp == "-")
        {
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found \"-\"\n";
            negative = true;
            seenSign = true;
        }
	    else if(temp == "#")
	    {
            if(DEBUG && !buildLabels)
		        cout << getTab(indent) << "Found noOffset\n";
	        noOffset = true;
        }
        else if(temp == "*")
        {
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found indirect\n";
            indirect = true;
        }
        else if(temp[0] == ';')
        {
            if(DEBUG && !buildLabels)
                cout << getTab(indent) << "Found comment\n";
            break; 
        }
        else
        {
            /*
            if(!buildLabels)
            {
                //deal with labels
                map<string, LabelInfo>::iterator iter = labels.find(temp);
                if(iter != labels.end())
                {
                    info = iter->second;
                    if(info.isConst || noOffset)
                        constant = info.position;
                    else
                    {
                        constant = info.position - command - 1;
                        seenSign = true;
                        reg2 = REG_PC;
                    }
        		}
                else
                {
                    cout << "ASSEM error: Unknown part \"" << temp << "\"\n";
                    return 0;
                }
            }
            else
            {*/
                //Just a placeholder. The 1 doesn't really mean much. I dont want an error (0) returned
                if(!buildLabels)
                {
                    cout << "ASSEM error: Unknown part \"" << temp << "\"\n";
                    return -1;
                }
                return 1;
            //}
        }
    }
    if(constant != 0 && reg2 != 0 && !seenSign && !buildLabels)
    {
        cout << "ASSEM error: Missing operator ( + or - )\n";
        return -1;
    }
    if(constant < 0)
    {
        constant *= -1;
        negative = !negative;
    }
    if(DEBUG && !buildLabels)
    {
        cout << getTab(indent) << "Opcode = " << opcode << "\n";
        cout << getTab(indent) << "Indirect = " << indirect << "\n";
        cout << getTab(indent) << "Register1 = " << reg1 << "\n";
        cout << getTab(indent) << "Register2 = " << reg2 << "\n";
        cout << getTab(indent) << "Constant = ";
        if(negative)
            cout << -1 * constant << "\n";
        else
            cout << constant << "\n";
    }
    unsigned int retval = ((opcode << 25) | (indirect << 24) | (reg1 << 20) | (reg2 << 16) | (negative << 15) | constant);
    return retval; 
}

int handleDirective(int outfile, string line, int directive, int &offset, int indent, bool buildLabels)
{
    int first = 0, second = 0;
    bool seenDirective = false, seenFirst = false, seenSecond = false;
    string temp, label = "";
    istringstream iss(line);
    while(true)
    {
        iss >> temp;
        int len = temp.length();

        if(DEBUG && !buildLabels)
            cout << getTab(indent) << "Dealing with part \"" << temp << "\"\n";

        if(iss.fail() || temp[0] == ';')
            break;
        
        if(temp[len - 1] == ':')
        {
            //deal with labels
            if(buildLabels)
            {
                if(DEBUG)
                    cout << getTab(indent) << "Adding label \"" << temp << "\" (" << offset << ") \n";
                labels.insert(make_pair(string(temp.substr(0, len - 1)), LabelInfo(offset, false)));
            }
        }
        else if(!seenDirective && getDirective(temp, indent + 1, buildLabels) != -1)
            seenDirective = true;
        else if(!seenFirst && getConstant(temp, indent + 1, false, buildLabels) != -1)
        {
            first = getConstant(temp, indent + 1, false, buildLabels);
            seenFirst = true;
        }
        else if(!seenSecond && getConstant(temp, indent + 1, false, buildLabels) != -1)
        {
            second = getConstant(temp, indent + 1, false, buildLabels);
            seenSecond = true;
        }
        else
        {
            if(!buildLabels)
            {   
                map<string, LabelInfo>::iterator iter = labels.find(temp);
                if(iter != labels.end())
                {
                    if(!seenFirst)
                        first = iter->second.position;
                    else if(!seenSecond)
                        second = iter->second.position;
                    else
                    {
                        cout << "ASSEM error: Too many operands in directive \"" << line << "\"\n";
                        return -1;
                    }
                }
                else
                {
                    cout << "ASSEM error: Unknown part \"" << temp << "\"\n";
                    return -1;
                }
            }
            else
            {   
                if(!seenFirst)
                {
                    label = temp;
                    seenFirst = true;
                }
                else
                {
                    cout << "ASSEM error: Unknown part \"" << temp << "\"\n";
                    return -1;
                }
            }
        }
    }

    int i = 0, zero = 0;
    switch(directive)
    {
        case DIR_DATA:
            if(!buildLabels)
            { 
                if(DEBUG)
                    cout << "WRITING WORD \"" << first << "\" at location " << offset << ".\n\n";
                write(outfile, (void *) &first, sizeof(first));
            }
            i = 1;
            break;
        case DIR_SPACE:
            for(i = 0; i < first; i++)
            {
                if(!buildLabels)
                {
                    if(DEBUG)
                        cout << "WRITING WORD \"" << zero << "\" at location " << offset + i << ".\n\n";
                    write(outfile, (void *) &zero, sizeof(int));   
                }
            }
            break;
        case DIR_DEFINE:
            if(buildLabels)
            {
                if(DEBUG)
                    cout << getTab(indent) << "Adding constant \"" << temp << "\" (" << second << ") \n";
                labels.insert(make_pair(label, LabelInfo(second, true)));
            }
            break;
        case DIR_LOC:
            if(offset == 0)
            {
                lseek(outfile, first, SEEK_SET);
                offset = first;
            }
            else
            {
                cout << "ASSEM error: Directive .loc must be the first line in the program and -f option cant be passed\n";
                return -1;
            }
            break;
        default:
            cout << "ASSEM error: Directive \"" << directives[directive] << "\" isn't implemented\n";
            return -1;
            break;
    }
    return i;
}

int assemble(ifstream& infile, int outfile, bool buildLabels = false)
{
    string line;
    unsigned int binary;
    int linecount, linesWritten = 0, directive;
    lseek(outfile, 0, SEEK_SET);

    for(linecount = 1; true; linecount++)
    {
        //Read the next line in the file and break if it is the end
        getline(infile, line);
        if(infile.eof())
            break;
        if(DEBUG)
            cout << "Parsing line \"" << line << "\".\n";
        directive = getDirective(line, 1, buildLabels);
        if(directive == 0)
        {
            //No directive... handle normally
            binary = getBinary(line, 1, linesWritten, 0, buildLabels);
            if(((binary >> 25) & 0x7F) == OP_ERROR)
            {
                if(!buildLabels)
                    cout << "Error on line " << linecount << ".\n";
                return -1;
            }
            else if(binary != OP_NO_WRITE)
            {
                if(!buildLabels)
                {
                    if(DEBUG)
                        cout << "WRITING WORD \"" << binary << "\" at location " << linesWritten << ".\n\n";
                    write(outfile, (void *) &binary, sizeof(binary));
                }
                linesWritten++;
            }
            else
                if(DEBUG && !buildLabels)
                    cout << endl;
        }
        else if(directive == -1)
        {
            //Invalid directive
            cout << "ASSEM error: Line " << linecount << " contains an invalid directive\n";
            return -1;
        }
        else
        {
            //Handle directive
            int written = handleDirective(outfile, line, directive, linesWritten, 1, buildLabels);
            if(written == -1)
            {
                cout << "ASSEM error: Line " << linecount << " has a badly formatted directive";
                return -1;
            }
            else
                linesWritten += written;
        }
    }
    return linesWritten;
}

int main(int argc, char *argv[])
{
    int offset = 0, argNum;

    //parse the command line options
    for(argNum = 1; argNum < argc && argv[argNum][0] == '-'; argNum++)
    {
        string arg = argv[argNum];
        if(arg.compare("-d") == 0 || arg.compare("--debug") == 0)
        {
            cout << "DEBUG MODE ON!\n";
            DEBUG = true;
        }
        else if(arg.compare("-f") == 0 || arg.compare("--offset") == 0)
        {
            if(argNum + 1 < argc)
            {
                offset = myatol(argv[argNum + 1]);
                if(offset != -1)
                {
                    cout << "Offset is " << offset << "\n";
                    argNum++;
                }
                else
                {
                    cout << "ASSEM warning: The offset \"" << argv[argNum + 1] << "\" is not valid so 0 is being used\n";
                    offset = 0;
                }
            }
            else
                cout << "ASSEM warning: You did not supply the offset number so 0 is being used\n"; 
        }
        else
            cout << "ASSEM warning: Unknown option \"" << arg << "\"\n";
    }

    if(argc < 1 + argNum)
    {
        cout << "Error: Please supply file to assemble.\n";
        return 1;
    }

    string outFileName = "binary.out", inFileName = argv[argNum];
    if(argc == argNum + 2)
        outFileName = argv[argNum + 1];
    
    ifstream infile;
    int outfile = open(outFileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    infile.open(inFileName.c_str());

    if(!infile.good())
    {
        cout << "Error: There was an error opening the input file\n";
        return 1;
    }
    if(outfile < 0)
    {
        cout << "Error: There was an error opening the output file\n";
        return 1;
    }

    //Build the symbol table on the first pass 
    if(DEBUG)
        cout << "\nBuilding the label map.\n";
    assemble(infile, outfile, true);
    if(DEBUG)
        cout << "Done building the label map.\n\n\n";

    infile.close();
    infile.open(inFileName.c_str());
 
    //Now actually assemble and write
    int commands = assemble(infile, outfile, false);
    if(commands >= 0)
        cout << "Wrote " << commands << " commands to " << outFileName << " successfully!\n";

    close(outfile);
    infile.close();
    return 0;
}
