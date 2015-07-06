#include <stdlib.h>
#include "ioWrapper.h"

using namespace std;

//Type to identify a Lexeme
enum LexemeType 
{   
    OBRAC,  //Open Bracket
    CBRAC,  //Close Bracket
    NUM,    //Number
    OP,     //Operator 
    RES,    //Reserved Word
    VAR,    //Variable
    SEMI,   //Semicolon
    LEOF,   //End of File
    COMMA,  //Comma
    OPARA,  //Open Paranthesis
    CPARA,  //Close Paranthesis
    ERROR,  //Sytax Error
    COLON   //Colon
};

const int RES_PRINT = 0, RES_END = 1, RES_IF = 2, RES_WHILE = 3, RES_ELSE = 4, RES_THEN = 5, RES_WHEN = 6, RES_UNLESS = 7,
          RES_ELSIF = 8, RES_DO = 9, RES_VAR = 10, RES_SWITCH = 11, RES_CASE = 12, RES_FLOAT = 13, RES_INT = 14, RES_STRING = 15,
          RES_CHAR = 16, RES_FUNCTION = 17, RES_WITH = 18, RES_IS = 19, RES_GIVES = 20, RES_RETURN = 21, RES_CREATE = 22, 
          RES_PTR = 23, RES_DEST = 24, RES_READ = 25, RES_GLOBAL = 26, RES_CONST = 27;
const string reservedWords[] = { "PRINT", "END", "IF", "WHILE", "ELSE", "THEN", "WHEN", "UNLESS", "ELSIF", 
                                 "DO", "VAR", "SWITCH", "CASE", "FLOAT", "INT", "STRING", "CHAR", "FUNCTION",
                                 "WITH", "IS", "GIVES", "RETURN", "CREATE", "PTR", "DESTROY", "READ", "GLOBAL", "CONST" };
const int numReservered = sizeof(reservedWords) / sizeof(string);

const int OP_POW = 0, OP_ADD = 1, OP_SUB = 2, OP_DIV = 3, OP_MUL = 4, OP_EQU = 5, OP_GT = 6, OP_LT = 7,
          OP_AND = 8, OP_OR = 9, OP_MOD = 10, OP_GTE = 11, OP_LTE = 12, OP_NE = 13, OP_SFTL = 14, OP_SFTR = 15, OP_FOL = 16, OP_NOT = 17;
const string operators[] = { "^", "+", "-", "/", "*", "=", ">", "<", "AND", "OR", "%", ">=", "<=", "!=", "<<", ">>", "~", "!" };
const int numOperators = sizeof(operators) / sizeof(string);

int isOperator(string text)
{
    for(int i = 0; i < numOperators; i++)
        if(text == operators[i])
            return i;
    return -1;
}

string toUpper(string text)
{
    for(int i = 0; i < text.length(); i++)
        if(text[i] >= 'a' && text[i] <= 'z')
            text[i] = text[i] - ('a' - 'A');
    return text;
}

int isReserved(string text)
{
    string upper = toUpper(text);
    for(int i = 0; i < numReservered; i++)
        if(upper.compare(reservedWords[i]) == 0)
            return i;
    return -1;
}

class Lexeme
{
    public:
        LexemeType type;
        string text;
        int num;

        Lexeme(LexemeType t, string tx, int n)
        {
            type = t;
            text = tx;
            num = n;
        }
};

class LexicalAnalyzer
{
    private:
        IOWrapper *ioWrapper;
        Lexeme *prev;
        bool unget;

    public:
        LexicalAnalyzer(string name)
        {
            ioWrapper = new IOWrapper(name);
            prev = NULL;
            unget = false;
        }

        ~LexicalAnalyzer()
        {
            delete ioWrapper;
        }
        
        void printError(string msg)
        {
            ioWrapper->printError(cout, msg);
        }
        
        Lexeme * ungetLexeme(int indent = 0)
        {
            if(DEBUG)   cout << getTab(indent) << "Ungetting Last Lexeme\n";
            unget = true;
        }

        Lexeme * saveLexeme(Lexeme *lex, int indent = 0)
        {
            if(DEBUG)   cout << getTab(indent) << "Getting Lexeme(" << lex->type << ", " << lex->text << ", " << lex->num << ")\n";
            prev = lex;
            return lex;
        }

        Lexeme * getLexeme(int indent = 0)
        {
            if(unget)
            {
                unget = false;
                return saveLexeme(prev, indent);
            }
            
            Lexeme *lex;
            string text = "";
            int pos;

            char c = ioWrapper->getChar();
            while(c == '\n' || c == ' ' || c == 9 || c == '\t' /*TAB*/) c = ioWrapper->getChar();

            if(c == ioWrapper->IO_EOF)
                return saveLexeme(new Lexeme(LEOF, "", -1), indent);

            bool foundDecimalPoint = false;
            switch(c)
            {
                case 'A':	case 'J':	case 'S':	case 'a':	case 'j':	case 's':
		        case 'B':	case 'K':	case 'T':	case 'b':	case 'k':	case 't':
		        case 'C':	case 'L':	case 'U':	case 'c':	case 'l':	case 'u':
		        case 'D':	case 'M':	case 'V':	case 'd':	case 'm':	case 'v':
		        case 'E':	case 'N':	case 'W':	case 'e':	case 'n':	case 'w':
		        case 'F':	case 'O':	case 'X':	case 'f':	case 'o':	case 'x':
		        case 'G':	case 'P':	case 'Y':	case 'g':	case 'p':	case 'y':
		        case 'H':	case 'Q':	case 'Z':	case 'h':	case 'q':	case 'z':
		        case 'I':	case 'R':	case '_':	case 'i':	case 'r':  
                    while((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
                    {
                        text += c;
                        c = ioWrapper->getChar();
                    }
                    ioWrapper->ungetChar();
                    pos = isReserved(text);
                    if(pos != -1)
                        return saveLexeme(new Lexeme(RES, text, pos), indent);
                    pos = isOperator(text);
                    if(pos != -1)
                        return saveLexeme(new Lexeme(OP, text, pos), indent);
                    return saveLexeme(new Lexeme(VAR, text, -1), indent);  
                    break;
                    
                case '{':
                    return saveLexeme(new Lexeme(OBRAC, "{", -1), indent);
                    break;
                
                case '}':
                    return saveLexeme(new Lexeme(CBRAC, "}", -1), indent);
                    break;

                case '(':
                    return saveLexeme(new Lexeme(OPARA, "(", -1), indent);
                    break;
                
                case ')':
                    return saveLexeme(new Lexeme(CPARA, ")", -1), indent);
                    break;

                case '0':	case '1':	case '2':	case '3':	case '4':
		        case '5':   case '6':	case '7':	case '8':	case '9':
                    while((c >= '0' && c <= '9') || c == '.')
                    {
                        if(c == '.')
                        {
                            if(!foundDecimalPoint)
                                foundDecimalPoint = true;
                            else
                                break;
                        }
                        text += c;
                        c = ioWrapper->getChar();
                    }
                    ioWrapper->ungetChar();
                    return saveLexeme(new Lexeme(NUM, text, atof(text.c_str())), indent);
                    break;
            
                case '+': case '-': case '*': case '/': case '%': case '!':
                case '<': case '>': case '&': case '|': case '=': case '~':
                    text += c;
                    c = ioWrapper->getChar();
                    pos = isOperator(text + c);
    
                    if(pos != -1)
                        return saveLexeme(new Lexeme(OP, text + c, pos), indent);
                    else
                    {
                        ioWrapper->ungetChar();
                        return saveLexeme(new Lexeme(OP, text, isOperator(text)), indent);
                    }
                    break;

                case ';':
                    return saveLexeme(new Lexeme(SEMI, ";", -1), indent);
                    break;
                
                case ':':
                    return saveLexeme(new Lexeme(COLON, ":", -1), indent);
                    break;

                case ',':
                    return saveLexeme(new Lexeme(COMMA, ",", -1), indent);
                    break;

                default:
                    printError("Unknown char \'" + string(1, c) + "\'");
                    return saveLexeme(new Lexeme(ERROR,  text, -1), indent);
                    break;
            }
            return saveLexeme(new Lexeme(ERROR,  text, -1), indent);;
        }
};

