#include "myHash.h"
#include <iostream>

#define IN_PARSE 1

bool isLeftOp = false;

using namespace std;

bool isStatementList(LexicalAnalyzer *, int);
bool isStatement(LexicalAnalyzer *, int);
bool isExpression(LexicalAnalyzer *, int);

Expression* parseExpr(LexicalAnalyzer *, int);
Statement* parseStatement(LexicalAnalyzer *, int);

VarType getType(Lexeme *lex)
{
    switch(lex->num)
    {
        case RES_INT:
            return VAR_INT;
        case RES_STRING:
            return VAR_STRING;
        case RES_CHAR:
            return VAR_CHAR;
        case RES_FLOAT:
            return VAR_FLOAT;
        case RES_PTR:
            return VAR_PTR;
    }
    return VAR_ERROR;
}

IdType* parseType(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseType\n";
    
    IdType * type = new IdType();
    IdType * start = type;   
    Lexeme* lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == RES && lex->num == RES_PTR)
    {
        type->varType = VAR_PTR;
        type->ptr = new IdType();
        type = type->ptr;
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    if(lex->type == RES)
    {
        type->ptr = NULL;
        switch(lex->num)
        {
            case RES_INT:
                type->varType = VAR_INT;
                break;
            case RES_CHAR:
                type->varType = VAR_CHAR;
                break;
            case RES_FLOAT:
                type->varType = VAR_FLOAT;
                break;
            default:
                type->varType = VAR_ERROR;
                lexicalAnalyzer->ungetLexeme(indent + 1);
                break;
        }
    }
    else
    {
        lexicalAnalyzer->ungetLexeme(indent + 1);
        if(DEBUG) cout << getTab(indent) << "Returning WRONG from parseType\n";
        return (IdType*) new Node(NODE_TYPE, NODE_WRONG);
    }
    if(DEBUG) cout << getTab(indent) << "Returning from parseType\n";
    return start;
}

Expression* parseCreate(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseCreate\n";
    IdType* type = parseType(lexicalAnalyzer, indent + 1);
    if(type->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected a type after the keyword CREATE");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR from parseCreate\n";
        return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
    }

    Lexeme* lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == NUM)
        type->size = lex->num;
    else
        lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseCreate\n";
    return new Expression(EXPR_CREATE, NULL, NULL, NULL, NULL, type);
}

vector<Identifier *>* parseParams(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseParams\n";
    Statement* s;
    string name;
    //VarType retType, type;
    IdentifierID *id;
    FunctionDeclaration* funct;
    vector<Identifier*> *vars = new vector<Identifier*>();

    //get the first param
            IdType* type = parseType(lexicalAnalyzer, indent + 1);
            if(type->subtype == NODE_WRONG || type->varType == VAR_ERROR)
            {
                if(DEBUG) cout << getTab(indent) << "Returning from parseParams\n";
                return vars;
            }

            Lexeme* lex = lexicalAnalyzer->getLexeme(indent + 1);
            if(lex->type == VAR)
            {
                id = addIfNew(lex->text);
                vars->push_back(new Variable(lex->text, type));
            }
            else
            {
                lexicalAnalyzer->printError("Expected an identifier in parameter list");
                if(DEBUG) cout << getTab(indent) << "Returning NULL from parseParams\n";
                return NULL;
            }
            
            //continue reading params
            lex = lexicalAnalyzer->getLexeme(indent + 1);
            while(lex->type == COMMA)
            {
                type = parseType(lexicalAnalyzer, indent + 1);
                if(type->subtype == NODE_WRONG)
                {
                    delete vars;
                    lexicalAnalyzer->printError("Expected a type after the comma ',' in the parameter list");
                    if(DEBUG) cout << getTab(indent) << "Returning NULL from parseParams\n";
                    return NULL;
                }

                lex = lexicalAnalyzer->getLexeme(indent + 1);
                if(lex->type != VAR)
                {
                    delete vars;
                    lexicalAnalyzer->printError("Expected a variable name after the type in the parameter list");
                    if(DEBUG) cout << getTab(indent) << "Returning NULL from parseParams\n";
                    return NULL;
                }
                id = addIfNew(lex->text);
                vars->push_back(new Variable(lex->text, type));
                
                lex = lexicalAnalyzer->getLexeme(indent + 1);
            }

            lexicalAnalyzer->ungetLexeme(indent + 1);
            if(DEBUG) cout << getTab(indent) << "Returning from parseParams\n";
            return vars;
}



Statement* parseFunctionDeclaration(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseFunctionDeclaration\n";
    Statement* s;
    string name;
    IdType* retType = NULL;
    FunctionDeclaration* funct;
    IdentifierID* id;
    vector<Identifier *>* idvec;
    vector<Identifier *>* params;

    Lexeme *lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == VAR)
    {
        id = addIfNew(lex->text);
        
        Lexeme *lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type != COLON)
        {
            lexicalAnalyzer->printError("Expected a colon ':' after the identifier in the function declaration");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionDeclaration\n";
            return new Statement(NODE_ERROR);
        }
        
        params = parseParams(lexicalAnalyzer, indent + 1);
        if(params == NULL)
        {
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionDeclaration\n";
            return new Statement(NODE_ERROR);
        }
        
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == RES && lex->num == RES_GIVES)
        {
            retType = parseType(lexicalAnalyzer, indent + 1);
            if(retType->subtype == NODE_WRONG)
            {
                lexicalAnalyzer->printError("Expected a type after the GIVES keyword in the function declaration");
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionDeclaration\n";
                return new Statement(NODE_ERROR);
            }
            lex = lexicalAnalyzer->getLexeme(indent + 1);
        }
        if(lex->type == RES && lex->num == RES_IS)
        {
            s = parseStatement(lexicalAnalyzer, indent + 1);
            lex = lexicalAnalyzer->getLexeme(indent + 1);
            if(lex->type == RES && lex->num == RES_END)
            {
                funct = new FunctionDeclaration(id->name, retType, params, s);

                if(DEBUG) cout << getTab(indent) << "Returning from parseFunctionDeclaration\n";
                vector<Identifier*> *idvec = new vector<Identifier*>();
                idvec->push_back((Identifier*) funct);
                DecStmt * ds = new DecStmt(idvec, STMT_FUN);
                symbolList.push_back(ds);
                return ds;
            }
            else
            {
                lexicalAnalyzer->printError("Expected keyword END after the statement in the function declaration");
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionDeclaration\n";
                return new Statement(NODE_ERROR);        
            }
        }
        else
            lexicalAnalyzer->ungetLexeme(indent + 1);

        funct = new FunctionDeclaration(id->name, retType, params, new Statement(STMT_BLANK));
        if(DEBUG) cout << getTab(indent) << "Returning from parseFunctionDeclaration\n";
        idvec->push_back(funct);
        DecStmt * ds = new DecStmt(idvec, STMT_FUN);
        symbolList.push_back(ds);
        return ds;
    }
    else
    {
        lexicalAnalyzer->printError("Expected an identifier after the keyword FUNCTION in the function declaration");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionDeclaration\n";
        return new Statement(NODE_ERROR);
    }
}

Expression* parseFunctionCall(IdentifierID *fun, LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseFunctionCall\n";
    
    Lexeme * lex;
    Expression *e;
    vector<Expression*> *exprs = new vector<Expression*>();
    e = parseExpr(lexicalAnalyzer, indent + 1);
        
        if(e->subtype == NODE_ERROR)
        {
            delete exprs;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionCall\n";
            return e;
        }
        if(e->subtype == NODE_WRONG)
        {
            //just conditinue, functions can have no params
        }
        else
        {
            exprs->push_back(e);
                
            //look for commas and other expressions
            lex = lexicalAnalyzer->getLexeme(indent + 1);           
            while(lex->type == COMMA)
            {
                e = parseExpr(lexicalAnalyzer, indent + 1);
                if(e->subtype == NODE_ERROR)
                {
                    delete exprs;
                    if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionCall\n";
                    return e;
                }
                else if(e->subtype == NODE_WRONG)
                {
                    delete exprs; delete e;
                    lexicalAnalyzer->printError("Expected an expression following the comma ',' in function call");
                    if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionCall\n";
                    return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
                }
                exprs->push_back(e);
                lex = lexicalAnalyzer->getLexeme(indent + 1);     
            }

            lexicalAnalyzer->ungetLexeme(indent + 1); 
        }

        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == CPARA)
        {
            if(DEBUG) cout << getTab(indent) << "Returning EXPR from parseFunctionCall\n";
            return new Function(fun, exprs);
        }
        else
        {
            delete e; delete exprs; //Should be deleting entire vector
            lexicalAnalyzer->printError("Expected a closing paranthesis ')' around the function call");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFunctionCall\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
}

Expression* parseOperand(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseOperand\n";
    
    bool ans = false;
    Expression *e;
    IdType * type;
    Lexeme *lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == NUM)
    {
        if(DEBUG) cout << getTab(indent) << "Returning NUM from parseOperand\n";
        return new Expression(EXPR_NUM, NULL, NULL, lex, NULL, new IdType(VAR_INT));
    }
    else if(lex->type == OPARA)
    {
        e = parseExpr(lexicalAnalyzer, indent + 1);
        if(e->subtype == NODE_ERROR)
        {
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOperand\n";
            return e;
        }
        else if(e->subtype == NODE_WRONG)
        {
            delete e;
            lexicalAnalyzer->printError("Expected an expression following the opening paranthesis '('");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOperand\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == CPARA)
        {
            if(DEBUG) cout << getTab(indent) << "Returning EXPR from parseOperand\n";
            return e;
        }
        else
        {
            delete e;
            lexicalAnalyzer->printError("Expected a closing paranthesis ')' around the expression");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOperand\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
    }
    else if(lex->type == VAR)
    {
        IdentifierID* id = addIfNew(lex->text);

        Lexeme *lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == OPARA)
        {  
            e = parseFunctionCall(id, lexicalAnalyzer, indent + 1);
            if(e->subtype == NODE_ERROR)
            {
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOperand\n";
                return e;
            }
            else if(e->subtype == NODE_WRONG)
            {
                delete e;
                lexicalAnalyzer->printError("Expected a parameter list following the function identifier " + lex->text);
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOperand\n";
                return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
            }
            if(DEBUG) cout << getTab(indent) << "Returning FUN from parseOperand\n";
            return e;   
        }
        else
        {
            lexicalAnalyzer->ungetLexeme(indent + 1);
            if(DEBUG) cout << getTab(indent) << "Returning VAR from parseOperand\n";
            return new Expression(EXPR_ID, NULL, id, lex, NULL, NULL);
        }
    }
    else if(lex->type == RES && lex->num == RES_CREATE)
    {
        e = parseCreate(lexicalAnalyzer, indent + 1);
        if(e->subtype == NODE_ERROR)
        {
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOperand\n";
            return e;
        }
        if(DEBUG) cout << getTab(indent) << "Returning from parseOperand\n";
        return e;
    }
    else
    {
        //Maybe not an error... return WRONG
        lex = lexicalAnalyzer->ungetLexeme(indent + 1);
        if(DEBUG) cout << getTab(indent) << "Returning WRONG NODE from parseOperand\n";
        return new Expression(NODE_WRONG, NULL, NULL, NULL, NULL, NULL);
    }
}

Expression* parseFol(LexicalAnalyzer *lexicalAnalyzer, int indent, Expression * cur)
{
    if(DEBUG) cout << getTab(indent) << "In parseFol\n";
    Expression *right, *left;
    Lexeme * lex;
    if(cur == NULL)
    {
        left = parseOperand(lexicalAnalyzer, indent + 1);
        if(left->subtype == NODE_ERROR)
        {
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFol\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == OP && lex->num == OP_FOL)
        {
            right = parseFol(lexicalAnalyzer, indent + 1, left);
            if(right->subtype == NODE_ERROR)
            {
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFol\n";
                return right;
            }
            if(DEBUG) cout << getTab(indent) << "Returning one from parseFol\n";
            return new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        }
        else
        {
            lexicalAnalyzer->ungetLexeme(indent + 1);
            if(DEBUG) cout << getTab(indent) << "Returning operand from parseFol\n";
            return left;
        }
    }
    else
    {
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == OP && lex->num == OP_FOL)
        {
            right = parseFol(lexicalAnalyzer, indent + 1, left);
            if(right->subtype == NODE_ERROR)
            {
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFol\n";
                return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
            }
            return new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node;
        }
        else
        {
            lexicalAnalyzer->ungetLexeme(indent + 1);
            //lexicalAnalyzer->printError("Expected a '~' following the operand in the fol expression");
            if(DEBUG) cout << getTab(indent) << "Returning from parseFol\n";
            return new Expression(NODE_WRONG, NULL, NULL, NULL, NULL, NULL);
        }

        /*
        left = parseOperand(lexicalAnalyzer, indent + 1);
        if(left->subtype == NODE_WRONG)
        {
            if(DEBUG) cout << getTab(indent) << "Returning NULL from parseFol\n";
            return new Expression(NODE_WRONG, NULL, NULL, NULL, NULL, NULL);;
        }
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == OP && lex->num == OP_FOL)
        {
            right = parseFol(lexicalAnalyzer, indent + 1, left);
            if(right->subtype == NODE_ERROR)
            {
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseFol\n";
                return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
            }
            return new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node;
        }
        else
        {
            lexicalAnalyzer->printError("Expected a '~' following the operand in the fol expression");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR from parseFol\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }*/
    }
}

Expression* parseNot(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseNot\n";

    Lexeme *lex;
    Expression *right, *left = parseFol(lexicalAnalyzer, indent + 1, NULL);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseNot\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && lex->num == OP_NOT)
    {
        right = parseFol(lexicalAnalyzer, indent + 1, NULL);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseNot\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after not operation '!'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseNot\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseNot\n";
    return left;
}

Expression* parsePow(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parsePow\n";

    Lexeme *lex;
    Expression *right, *left = parseNot(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parsePow\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && lex->num == OP_POW)
    {
        right = parseNot(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parsePow\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after the follow operation '->'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parsePow\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parsePow\n";
    return left;
}

Expression* parseMD(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseMD\n";

    Lexeme *lex;
    Expression *right, *left = parsePow(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseMD\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && ( lex->num == OP_MUL || lex->num == OP_DIV || lex->num == OP_MOD ))
    {
        right = parsePow(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseMD\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after MD operation '*', '/', or '%'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseMD\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseMD\n";
    return left;
}

Expression* parseAS(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseAS\n";

    Lexeme *lex;
    Expression *right, *left = parseMD(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAS\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && ( lex->num == OP_ADD || lex->num == OP_SUB ))
    {
        right = parseMD(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAS\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after AS operation '+', or '-'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAS\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseAS\n";
    return left;
}

Expression* parseSH(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseSH\n";

    Lexeme *lex;
    Expression *right, *left = parseAS(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseSH\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && ( lex->num == OP_SFTL || lex->num == OP_SFTR ))
    {
        right = parseAS(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseSH\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after SH operation '<<', or '>>'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseSH\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseSH\n";
    return left;
}

Expression* parseCMP(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseCMP\n";

    Lexeme *lex;
    Expression *right, *left = parseSH(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseCMP\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && ( lex->num == OP_LT || lex->num == OP_GT || (lex->num == OP_EQU && !isLeftOp) || lex->num == OP_LTE|| lex->num == OP_GTE || lex->num == OP_NE))
    {
        right = parseSH(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseCMP\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after CMP operation '=', '<=', '>=', or '!='");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseCMP\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseCMP\n";
    return left;
}

Expression* parseAND(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseAND\n";

    Lexeme *lex;
    Expression *right, *left = parseCMP(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAND\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && lex->num == OP_AND)
    {
        right = parseCMP(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAND\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after AND operation 'AND'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAND\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseAND\n";
    return left;
}

Expression* parseOR(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseOR\n";

    Lexeme *lex;
    Expression *right, *left = parseAND(lexicalAnalyzer, indent + 1);
    if(left->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOR\n";
        return left;
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    while(lex->type == OP && lex->num == OP_AND)
    {
        right = parseAND(lexicalAnalyzer, indent + 1);
        if(right->subtype == NODE_ERROR)
        {
            delete left;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOR\n";
            return right;
        }
        else if(right->subtype == NODE_WRONG)
        {
            delete left;
            delete right;
            lexicalAnalyzer->printError("Expected an expression after OR operation 'OR'");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseOR\n";
            return new Expression(NODE_ERROR, NULL, NULL, NULL, NULL, NULL);
        }
        left = new Expression(EXPR_OP, left, NULL, lex, right, NULL); //Combine into 1 node
        lex = lexicalAnalyzer->getLexeme(indent + 1);
    }
    lex = lexicalAnalyzer->ungetLexeme(indent + 1);
    if(DEBUG) cout << getTab(indent) << "Returning from parseOR\n";
    return left;
}

Expression* parseExpr(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    return parseOR(lexicalAnalyzer, indent);
}

Statement* parseVarStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseVarStmt\n";
    IdType* type;
    vector<Identifier*> *idList = new vector<Identifier*>();

    Lexeme *lex2, *lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == VAR)
    {
        addIfNew(lex->text);
        idList->push_back(new Variable(lex->text, NULL));
        lex2 = lexicalAnalyzer->getLexeme(indent + 1);
        while(lex2->type == COMMA)
        {
            lex = lexicalAnalyzer->getLexeme(indent + 1);
            if(lex->type == VAR)
            {
                addIfNew(lex->text);
                idList->push_back(new Variable(lex->text, NULL));
            }            
            else
            {
                delete lex; delete lex2;
                lexicalAnalyzer->printError("Expected a identifier after the comma ',' in the VAR statement");
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseVarStmt\n";
                return new Statement(NODE_ERROR);
            }
            lex2 = lexicalAnalyzer->getLexeme(indent + 1);
        }
        if(lex2->type == COLON)    
        {
            bool global = false, con = false;
            lex = lexicalAnalyzer->getLexeme(indent + 1);
            if(lex->type == RES && lex->num == RES_GLOBAL)
                global = true;
            else 
                lexicalAnalyzer->ungetLexeme(indent + 1);
            lex = lexicalAnalyzer->getLexeme(indent + 1);
            if(lex->type == RES && lex->num == RES_CONST)
                con = true;
            else 
                lexicalAnalyzer->ungetLexeme(indent + 1);
            
            //Get the type
            type = parseType(lexicalAnalyzer, indent + 1);
            if(type->subtype == NODE_WRONG)
            {
                delete lex; delete lex2;
                lexicalAnalyzer->printError("Expected a type ':' in the VAR statement");
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseVarStmt\n";
                return new Statement(NODE_ERROR);
            }
            
            /*
            //Add the Variables to the hash table
            Identifier* v;
            for(int i = 0; i < vars.size(); i++)
            {
                v = symbolTable.find(vars[i]);
                if(v != NULL)
                {
                    delete lex; delete lex2; delete v; //Should really be deleting entire vector
                    lexicalAnalyzer->printError("The indentifier " + vars[i] + " was already declared");
                    if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseVarStmt\n";
                    return new Statement(NODE_ERROR);
                }
                v = new Variable(vars[i], type);
                symbolTable.add(v);
                varlist->push_back((Variable*) v);
            }*/

            //Add the type
            for(int i = 0; i < idList->size(); i++)
            {
                
                Variable *v = (Variable*) (*idList)[i];
                v->varType = type;
                if(global)
                    v->dectype = DEC_GLOBAL;
                else
                    v->dectype = DEC_LOCAL;
                if(con)
                    v->isConst = true;
            }

            DecStmt * ds = new DecStmt(idList, STMT_VAR);
            if(global) 
                symbolList.push_back(ds);
            if(DEBUG) cout << getTab(indent) << "Returning from parseVarStmt\n";
            return ds;
        }
        else
        {
            delete lex; delete lex2;
            lexicalAnalyzer->printError("Expected a colon ':' after the identifier in the VAR statement");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseVarStmt\n";
            return new Statement(NODE_ERROR);
        }        
    }
    else
    {
        delete lex;
        lexicalAnalyzer->printError("Expected a identifier after VAR");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseVarStmt\n";
        return new Statement(NODE_ERROR);
    }
}

Statement* parseAssignStmt(LexicalAnalyzer *lexicalAnalyzer, string name, int indent, Expression* start, int fol = 0)
{
    if(DEBUG) cout << getTab(indent) << "In parseAssignStmt\n";
    Expression *expr;
    IdentifierID *id;
    Lexeme* lex;

    lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == OP && lex->num == OP_EQU)
    {
        id = addIfNew(name);
        expr = parseExpr(lexicalAnalyzer, indent + 1);
        if(expr->subtype == NODE_ERROR)
        {
            delete expr; delete lex;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAssignStmt\n";
            return new Statement(NODE_ERROR);
        }
        else if(expr->subtype == NODE_WRONG)
        {
            lexicalAnalyzer->printError("Expected an expression following the equal sign '=' in the Assignment statement");
            delete expr; delete lex;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAssignStmt\n";
            return new Statement(NODE_ERROR);
        }

        if(DEBUG) cout << getTab(indent) << "Returning from parseAssignStmt\n";
        return new AssignStmt(new Variable(id->name, NULL) , expr, fol, start);
    }
    else if(start == NULL && lex->type == OP && lex->num == OP_NOT)
    {
        isLeftOp = true;
        expr = parseExpr(lexicalAnalyzer, indent + 1);
        isLeftOp = false;
        if(expr->subtype == NODE_ERROR)
        {
            delete expr;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAssignStmt\n";
            return new Statement(NODE_ERROR);
        }
        else if(expr->subtype == NODE_WRONG)
        {
            lexicalAnalyzer->printError("Expected an expression following the ! in the assignment statement");
            delete expr;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAssignStmt\n";
            return new Statement(NODE_ERROR);
        }
        return parseAssignStmt(lexicalAnalyzer, name, indent + 1, expr, fol + 1);
    }
    else if(lex->type == OP && lex->num == OP_FOL)
    {
        return parseAssignStmt(lexicalAnalyzer, name, indent + 1, start, fol + 1);
    }
    else
    {
        lexicalAnalyzer->printError("Expected an equal sign '=' following the identifier");
        delete lex;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseAssignStmt\n";
        return new Statement(NODE_ERROR);
    }
}

Statement* parsePrintStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parsePrintStmt\n";
    Expression *expr;

    expr = parseExpr(lexicalAnalyzer, indent + 1);
    if(expr->subtype == NODE_ERROR)
    {
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parsePrintStmt\n";
        return new Statement(NODE_ERROR);
    }
    else if(expr->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected an expression following the PRINT in the PRINT statement");
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parsePrintStmt\n";
        return new Statement(NODE_ERROR);
    }
    if(DEBUG) cout << getTab(indent) << "Returning from parsePrintStmt\n";
    return new PrintStmt(expr);
}

Statement* parseIdStmt(NodeSubType type, LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseIdStmt\n";
    Lexeme *lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == VAR)
    {
        addIfNew(lex->text);
        if(DEBUG) cout << getTab(indent) << "Returning from parseIdStmt\n";
        return new IdStmt(new Variable(lex->text, NULL), type);
    }
    lexicalAnalyzer->printError("Expected an identifier after the statement");
    if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIdStmt\n";
    return new Statement(NODE_ERROR);
}

Statement* parseRetStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseRetStmt\n";
    Expression *expr;

    expr = parseExpr(lexicalAnalyzer, indent + 1);
    if(expr->subtype == NODE_ERROR)
    {
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseRetStmt\n";
        return new Statement(NODE_ERROR);
    }
    else if(expr->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected an expression following the RETURN in the RETURN statement");
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseRetStmt\n";
        return new Statement(NODE_ERROR);
    }
    if(DEBUG) cout << getTab(indent) << "Returning from parseRetStmt\n";
    return new RetStmt(expr);
}

Statement* parseWhileStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseWhileStmt\n";
    Lexeme *lex;
    Expression *expr;
    Statement *s2 = NULL;

    expr = parseExpr(lexicalAnalyzer, indent + 1);
    if(expr->subtype == NODE_ERROR)
    {
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhileStatement\n";
        return new Statement(NODE_ERROR);
    }
    else if(expr->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected an expression following the WHILE in the WHILE statement");
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhileStatement\n";
        return new Statement(NODE_ERROR);
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == RES && lex->num == RES_DO)
    {
        s2 = parseStatement(lexicalAnalyzer, indent + 1);
        if(s2->subtype == NODE_ERROR)
        {
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhileStatement\n";
            return new Statement(NODE_ERROR);
        }
        else if(s2->subtype == NODE_WRONG)
        {
            lexicalAnalyzer->printError("Expected a statement following DO in the WHILE statement");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhileStatement\n";
            return new Statement(NODE_ERROR);
        }
        
        //Check for END
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == RES && lex->num == RES_END)
        {
            if(DEBUG) cout << getTab(indent) << "Returning from parseWhileStatement\n";
            return new CondStmt(STMT_WHILE, expr, s2);
        }
        else
        {
            lexicalAnalyzer->printError("Expected END following the statment in the WHILE");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhileStatement\n";
            return new Statement(NODE_ERROR);
        }
    }
    else
    {
        delete lex;
        lexicalAnalyzer->printError("Expected DO after the expression in the WHILE statement");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhileStatement\n";
        return new Statement(NODE_ERROR);
    }
}

Statement* parseUnlessStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseUnlessStmt\n";
    Lexeme *lex;
    Expression *expr;
    Statement *s2 = NULL;

    expr = parseExpr(lexicalAnalyzer, indent + 1);
    if(expr->subtype == NODE_ERROR)
    {
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseUnlessStmt\n";
        return new Statement(NODE_ERROR);
    }
    else if(expr->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected an expression following the THEN in the UNLESS statement");
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseUnlessStmt\n";
        return new Statement(NODE_ERROR);
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == RES && lex->num == RES_THEN)
    {
        s2 = parseStatement(lexicalAnalyzer, indent + 1);
        if(s2->subtype == NODE_ERROR)
        {
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseUnlessStmt\n";
            return new Statement(NODE_ERROR);
        }
        else if(s2->subtype == NODE_WRONG)
        {
            lexicalAnalyzer->printError("Expected a statement following THEN in the UNLESS statement");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseUnlessStmt\n";
            return new Statement(NODE_ERROR);
        }
        
        //Check for END
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == RES && lex->num == RES_END)
        {
            if(DEBUG) cout << getTab(indent) << "Returning from parseUnlessStmt\n";
            return new CondStmt(STMT_UNLESS, expr, s2);
        }
        else
        {
            lexicalAnalyzer->printError("Expected END following the statment in the UNLESS");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseUnlessStmt\n";
            return new Statement(NODE_ERROR);
        }
    }
    else
    {
        delete lex;
        lexicalAnalyzer->printError("Expected THEN after the expression in the UNLESS statement");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseUnlessStmt\n";
        return new Statement(NODE_ERROR);
    }
}

Statement* parseWhenStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseWhenStmt\n";
    Lexeme *lex;
    Expression *expr;
    Statement *s2 = NULL;

    expr = parseExpr(lexicalAnalyzer, indent + 1);
    if(expr->subtype == NODE_ERROR)
    {
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhenStmt\n";
        return new Statement(NODE_ERROR);
    }
    else if(expr->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected an expression following the WHEN in the WHEN statement");
        delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhenStmt\n";
        return new Statement(NODE_ERROR);
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == RES && lex->num == RES_THEN)
    {
        s2 = parseStatement(lexicalAnalyzer, indent + 1);
        if(s2->subtype == NODE_ERROR)
        {
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhenStmt\n";
            return new Statement(NODE_ERROR);
        }
        else if(s2->subtype == NODE_WRONG)
        {
            lexicalAnalyzer->printError("Expected a statement following THEN in the WHEN statement");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhenStmt\n";
            return new Statement(NODE_ERROR);
        }
        
        //Check for END
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == RES && lex->num == RES_END)
        {
            if(DEBUG) cout << getTab(indent) << "Returning from parseWhenStmt\n";
            return new CondStmt(STMT_WHEN, expr, s2);
        }
        else
        {
            lexicalAnalyzer->printError("Expected END following the statment in the WHEN");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhenStmt\n";
            return new Statement(NODE_ERROR);
        }
    }
    else
    {
        delete lex;
        lexicalAnalyzer->printError("Expected THEN after the expression in the WHEN statement");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseWhenStmt\n";
        return new Statement(NODE_ERROR);
    }
}

Statement* parseIfStmt(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseIfStmt\n";
    Lexeme *lex;
    Expression *expr;
    Statement *s2 = NULL;
    vector<Statement*>* stmts = new vector<Statement*>(0);
    vector<Expression*>* exprs = new vector<Expression*>(0);

    expr = parseExpr(lexicalAnalyzer, indent + 1);
    if(expr->subtype == NODE_ERROR)
    {
        delete stmts; delete exprs; delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
        return new Statement(NODE_ERROR);
    }
    else if(expr->subtype == NODE_WRONG)
    {
        lexicalAnalyzer->printError("Expected an expression following the IF in the IF statement");
        delete stmts; delete exprs; delete expr;
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
        return new Statement(NODE_ERROR);
    }
    lex = lexicalAnalyzer->getLexeme(indent + 1);
    if(lex->type == RES && lex->num == RES_THEN)
    {
        s2 = parseStatement(lexicalAnalyzer, indent + 1);
        if(s2->subtype == NODE_ERROR)
        {
            delete stmts; delete exprs; delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
            return new Statement(NODE_ERROR);
        }
        else if(s2->subtype == NODE_WRONG)
        {
            lexicalAnalyzer->printError("Expected a statement following THEN in the IF statement");
            delete stmts; delete exprs; delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
            return new Statement(NODE_ERROR);
        }
        
        stmts->push_back(s2);
        exprs->push_back(expr);

        //Search for ELSIFs
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        while(lex->type == RES && lex->num == RES_ELSIF) 
        {
            expr = parseExpr(lexicalAnalyzer, indent + 1);
            if(expr->subtype == NODE_ERROR)
            {
                delete lex; delete expr;
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                return new Statement(NODE_ERROR);
            }
            else if(expr->subtype == NODE_WRONG)
            {
                lexicalAnalyzer->printError("Expected an expression following the IF in the IF statement");
                delete lex; delete expr;
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                return new Statement(NODE_ERROR);
            }
            lex = lexicalAnalyzer->getLexeme(indent + 1);
            if(lex->type == RES && lex->num == RES_THEN)
            {
                s2 = parseStatement(lexicalAnalyzer, indent + 1);
                if(s2->subtype == NODE_ERROR)
                {
                    delete stmts; delete exprs; delete expr; delete lex; delete s2; //Should be deleting all of the things in vectors....
                    if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                    return new Statement(NODE_ERROR);
                }
                else if(s2->subtype == NODE_WRONG)
                {
                    lexicalAnalyzer->printError("Expected a statement following THEN in an ELSIF section of the IF statement");
                    delete stmts; delete exprs; delete expr; delete lex; delete s2; //Should be deleting all of the things in vectors....
                    if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                    return new Statement(NODE_ERROR);
                }
                stmts->push_back(s2);
                exprs->push_back(expr);
            }
            else
            {
                delete stmts; delete exprs; delete lex; delete expr; //Should be deleting all of the things in vectors....
                lexicalAnalyzer->printError("Expected THEN after the expression in an ELSIF section of the IF statement");
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                return new Statement(NODE_ERROR);
            }
        }
        lexicalAnalyzer->ungetLexeme(indent + 1);

        //Check for ELSE
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == RES && lex->num == RES_ELSE) 
        {
            s2 = parseStatement(lexicalAnalyzer, indent + 1);
            if(s2->subtype == NODE_ERROR)
            {
                delete stmts; delete exprs; delete expr; delete lex; delete s2; //Should be deleting all of the things in vectors....
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                return new Statement(NODE_ERROR);
            }
            else if(s2->subtype == NODE_WRONG)
            {
                lexicalAnalyzer->printError("Expected a statement following ELSE in the IF statement");
                delete stmts; delete exprs; delete expr; delete lex; delete s2; //Should be deleting all of the things in vectors....
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
                return new Statement(NODE_ERROR);
            }
            
            stmts->push_back(s2);
            //exprs->push_back(expr);
        }
        else
            lexicalAnalyzer->ungetLexeme();
        
        //Check for END
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == RES && lex->num == RES_END)
        {
            if(DEBUG) cout << getTab(indent) << "Returning from parseIfStatement\n";
            return new IfStmt(exprs, stmts);
        }
        else
        {
            lexicalAnalyzer->printError("Expected END following the statment in the IF");
            delete expr; delete lex; delete s2;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
            return new Statement(NODE_ERROR);
        }
    }
    else
    {
        delete lex;
        lexicalAnalyzer->printError("Expected THEN after the expression in the IF statement");
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseIfStatement\n";
        return new Statement(NODE_ERROR);
    }
}

Statement* parseStatementList(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseStatementList\n";
    Statement *stmt;   
    Lexeme *lex;   
    vector<Statement*> *stmts = new vector<Statement *>(); 

    while(true)
    {    
        Statement *stmt = parseStatement(lexicalAnalyzer, indent + 1);
        if(stmt->subtype == NODE_ERROR)
        {
            delete stmt;
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseStatementList\n";
            return new Statement(NODE_ERROR);
        }
        else if(stmt->subtype == NODE_WRONG)
            break;

        stmts->push_back(stmt);

        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type == CBRAC || lex->type == LEOF)
        {
            lexicalAnalyzer->ungetLexeme(indent + 1);
            break;
        }
        else if(lex->type != SEMI)
        {
            delete lex; delete stmt; //Should really be deleting all of the vector
            lexicalAnalyzer->printError("Expected a semicolon ';' or closing bracket '}' after the Statement in the StatementList");
            if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseStatementList\n";
            return new Statement(NODE_ERROR);
        }
    }
    if(DEBUG) cout << getTab(indent) << "Returning from parseStatementList\n";
    return new StatementList(stmts);
}

Statement* parseStatement(LexicalAnalyzer *lexicalAnalyzer, int indent)
{
    if(DEBUG) cout << getTab(indent) << "In parseStatement\n";

    Lexeme *lex = lexicalAnalyzer->getLexeme(indent + 1);
    Expression *expr;
    Statement *s;
    NodeSubType st;

    if(lex->type == RES)
    {
        switch(lex->num)
        {
            //case RES_SWITCH:
            //    s = parseSwitchStmt(lexicalAnalyzer, indent + 1);
            //    break;
            case RES_IF:
                s = parseIfStmt(lexicalAnalyzer, indent + 1);
                break;
            case RES_WHILE:
                s = parseWhileStmt(lexicalAnalyzer, indent + 1);
                break;
            case RES_VAR:
                s = parseVarStmt(lexicalAnalyzer, indent + 1);
                break;
            case RES_PRINT:
                s = parsePrintStmt(lexicalAnalyzer, indent + 1);
                break;
            case RES_WHEN:
                s = parseWhenStmt(lexicalAnalyzer, indent + 1);
                break;
            case RES_UNLESS:
                s = parseUnlessStmt(lexicalAnalyzer, indent + 1);
                break;
            case RES_FUNCTION:
                s = parseFunctionDeclaration(lexicalAnalyzer, indent + 1);
                break;
            case RES_DEST:
                s = parseIdStmt(STMT_DEST, lexicalAnalyzer, indent + 1);
                break;
            case RES_READ:
                s = parseIdStmt(STMT_READ, lexicalAnalyzer, indent + 1);
                break;
            case RES_RETURN:
                s = parseRetStmt(lexicalAnalyzer, indent + 1);
                break;

            default:
                lexicalAnalyzer->printError("Unknown Reserved Word \"" + lex->text + "\"");
                if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseStatement\n";
                return new Statement(NODE_ERROR);
                break;
        }
    }
    else if(lex->type == VAR)
    {
        s = parseAssignStmt(lexicalAnalyzer, lex->text, indent + 1, NULL);
    }
    else if(lex->type == OBRAC)
    {
        s = parseStatementList(lexicalAnalyzer, indent + 1);
        lex = lexicalAnalyzer->getLexeme(indent + 1);
        if(lex->type != CBRAC)
        {
            lexicalAnalyzer->printError("Expected a closing bracket after StatementList\n");
            return new Statement(NODE_ERROR);
        }
    }
    else
    {
        //Maybe not an error... just put the lexeme back
        lexicalAnalyzer->ungetLexeme(indent + 1);
        if(DEBUG) cout << getTab(indent) << "Returning WRONG NODE from parseStatement\n";
        return new Statement(NODE_WRONG);
    }

    if(s->subtype == NODE_ERROR)
    {
        if(DEBUG) cout << getTab(indent) << "Returning ERROR NODE from parseStatement\n";
        return s;
    }
    if(DEBUG) cout << getTab(indent) << "Returning from parseStatement\n";
    return s;
}
