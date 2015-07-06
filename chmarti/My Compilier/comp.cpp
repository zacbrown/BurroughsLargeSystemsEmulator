#define DEBUG 0

#include <fstream>
#include "codegen.h"
#include "print.h"

using namespace std;

ofstream outputFile;

int main(int argc, char *argv[])
{
    string outFileName = "prog.asm", inFileName;
    if(argc < 2)
    {
        cout << "Error: Please supply file to compile.\n";
        return 1;
    }
    else if(argc == 2)
        inFileName = argv[1];
    else
    {
        outFileName = argv[2];
        inFileName = argv[1];
    }   

    LexicalAnalyzer *lexicalAnalyzer = new LexicalAnalyzer(inFileName);
    Statement* s = parseStatementList(lexicalAnalyzer, 0);
    if(s->subtype == NODE_ERROR)
    {
        cout << "Error parsing\n";
        return 1;
    }
    if(DEBUG) cout << "\n*****DONE PARSING*****\n\n";
    
    outputFile.open(outFileName.c_str());
    if(!codeGenSetup(outputFile, 0) || !codeGenStmt(outputFile, s, 0) || !codeGenEnd(outputFile, 0))
    {
        outputFile.close();
        delete lexicalAnalyzer;
        cout << "Error compiling " << inFileName << "\n";
        return -1;
    }
    outputFile.close();
    delete lexicalAnalyzer;

    cout << inFileName << " compiled succesfully to " << outFileName << "\n";
    return 0;
}

