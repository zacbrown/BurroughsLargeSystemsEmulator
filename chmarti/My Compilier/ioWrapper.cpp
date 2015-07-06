#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class IOWrapper
{
    protected:
        string filename, line, lastline;
        ifstream file;
        bool isready, unget;
        int pos, lineNum;
        char lastc;

        bool getNextLine()
        {
            lastline = line;
            getline(file, line);
            lineNum++;
            if(file.fail())
                return false;
            return true;
        }

    public:
        static const char IO_ERROR = -1;
        static const char IO_EOF = -2;

        IOWrapper(string name)
        {
            lineNum = 0;
            unget = false;
            isready = false;
            pos = 0;
            filename = name;
            file.open(name.c_str());
            if(file.is_open())
            {
                getNextLine();
                isready = true;
            }
        }

        ~IOWrapper()
        {
            file.close();
        }
        
        bool isReady()
        {
            return isready;
        }
    
        char getChar()
        {
            if(!file.is_open())
                return IO_ERROR;
             
            char c;
            if(unget)
            {   
                c = lastc;
                unget = false;
            }
            else
            {
                if(pos < line.length())
                    c = line[pos];
                else
                {
                    pos = 0;
                    if(getNextLine())
                        c = line[pos];
                    else
                        return IO_EOF;
                }
                pos++;
            }
            lastc = c;
            return c;
        }
        
        bool ungetChar()
        {
            unget = true;
        }
        
        void printError(ostream &stream, string msg)
        {
            stream << "Error on line " << lineNum << " \"" << line << "\": " << msg << endl;
        }
};

/*
int main(void)
{
    IOWrapper iowrapper("makefile");
    
    if(!iowrapper.isReady())
    {
        cout << "Error!\n";
        return -1;
    }

    cout << "***File START***\n";
    char c = iowrapper.getChar();

    for (int i = 0; c != iowrapper.IO_EOF; i++)
    {
        cout << c;
        if(i > 10 && i < 15)
            iowrapper.ungetChar();
        if(i == 90)
        {
            cout << endl;
            iowrapper.printError(cout, "there was a problem");
        }
        c = iowrapper.getChar();
    }
    
    cout << "\n***File EOF***\n";
    return 0;
}*/

