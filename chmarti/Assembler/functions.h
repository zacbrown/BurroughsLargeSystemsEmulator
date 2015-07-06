#ifdef _UNIX
#define xopen open
#else
#define xopen _open
#endif

#ifdef _UNIX
#define xWRONLY O_WRONLY
#else
#define xWRONLY _O_WRONLY
#endif

#ifdef _UNIX
#define xCREAT O_CREAT
#else
#define xCREAT _O_CREAT
#endif

#ifdef _UNIX
#define xTRUNC O_TRUNC
#else
#define xTRUNC _0_TRUNC
#endif

#include <string>

using namespace std;

const int OP_NO_WRITE = 0, OP_HALT = 1, OP_LOAD = 2, OP_LOADL = 3, OP_LOADH = 4, OP_STORE = 5, 
          OP_ADD = 6, OP_SUB = 7, OP_MUL = 8, OP_DIV = 9, OP_SUBR = 10, OP_DIVR = 11,
          OP_CMP = 12, OP_JMPG = 13, OP_JMPL = 14, OP_JMPE = 15, OP_JMPGE = 16, OP_JMPLE = 17, 
          OP_JMP = 18, OP_PRINT = 19, OP_NEWL = 20, OP_CALL = 21, OP_RET = 22,
          OP_PUSH = 23, OP_POP = 24, OP_IOOP = 25, OP_JMPNE = 26, OP_MOD = 27, OP_IRET = 28, 
          OP_AND = 29, OP_OR = 30, OP_FLAG = 31, OP_MOV = 32, OP_FADD = 33, OP_FSUB = 34, 
          OP_FMUL = 35, OP_FDIV = 36, OP_FSUBR = 37, OP_FDIVR = 38, OP_CAST = 39, OP_ERROR = 0x7F;

const string opcodes[] = { "***NO_WRITE***", "HALT", "LOAD", "LOADL", "LOADH", "STORE",
                           "ADD", "SUB", "MUL", "DIV", "SUBR", "DIVR", "CMP", 
                           "JMPG", "JMPL", "JMPE", "JMPGE", "JMPLE", "JMP", "PRINT", 
                           "NEWL", "CALL", "RET", "PUSH", "POP", "IOOP", "JMPNE", "MOD",
                           "IRET", "AND", "OR", "FLAG", "MOV", "FADD", "FSUB", "FMUL", 
                           "FDIV", "FSUBR", "FDIVR", "CAST" };

const int DIR_DATA = 1, DIR_SPACE = 2, DIR_LOC = 3, DIR_DEFINE = 4, DIR_FORGET = 5, DIR_MACRO = 6;

const string directives[] = { "***NO_DIR***", ".data", ".space", ".loc", ".define", ".forget", ".macro" };

const int IOTYPE_CHAR = 0, IOTYPE_INT = 1, IOTYPE_STRING = 2, IOTYPE_FLOAT = 3;

const int NUMREGS = 20, NUMOPS = sizeof(opcodes)/sizeof(string), NUMDIRS = sizeof(directives)/sizeof(string);
int registers[NUMREGS];

//Special Registers
const int REG_IVT = 19;
const int REG_INTERRUPT_DATA = 18;
const int REG_PCB = 17;
const int REG_PAGE = 16;
const int REG_SP = 14;
const int REG_FP = 13;
const int REG_PC = 15;
const int REG_OS_PAGE = 0;

//Flag Indexs
const int FLAG_L = 0;
const int FLAG_G = 1;
const int FLAG_E = 2;
const int FLAG_LE = 3;
const int FLAG_GE = 4;
const int FLAG_NE = 5;
const int FLAG_RUN = 6;
const int FLAG_Z = 7;

//Doesn't allow negatives and returns -1 if there is an error
int myatol(string text)
{
    if(text.length() < 1 || text[0] == '-' || text[0] == '+')
        return -1;
    int value = atol(text.c_str());
    if(value == 0 && text[0] != '0')
        return -1;
    return value;
}

bool isOpSingle(const int opcode)
{
    switch(opcode)
    {
        case OP_JMPG:
        case OP_JMPL:
        case OP_JMPE:
        case OP_JMPGE:
        case OP_JMPLE:
        case OP_JMP:
        case OP_PRINT:
        case OP_NEWL:
        case OP_CALL:
        case OP_PUSH:
            return true;
    }
    return false;
}

