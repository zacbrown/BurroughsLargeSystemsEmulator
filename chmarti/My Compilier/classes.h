#include "types.h"
#include <stack>
#include <vector>
#include "LexicalAnalyzer.h"

class Node
{
    public:
    NodeType type;
    NodeSubType subtype;

    Node(NodeType t, NodeSubType st) { type = t; subtype = st; }
    Node(){}
};

class IdType : public Node
{
    public:
    IdType * ptr;
    VarType varType;
    int size;
    IdType(IdType *p) { ptr = p; type = NODE_TYPE; varType = VAR_PTR; size = 1; }
    IdType(VarType t) { ptr = NULL; type = NODE_TYPE; varType = t; size = 1; }
    IdType(IdType *p, int s) { ptr = p; type = NODE_TYPE; varType = VAR_PTR; size = s; }
    IdType(VarType t, int s) { ptr = NULL; type = NODE_TYPE; varType = t; size = s; }
    IdType() { size = 1; }          
};

class Statement: public Node
{
    public:
    Statement(NodeSubType st) { type = NODE_STMT; subtype = st; }
    Statement() {  }
};

class Identifier;

class IdentifierID
{
    public:
    string name;
    stack<Identifier*> *ids;

    IdentifierID(string n) { name = n; ids = new stack<Identifier*>(); }
    //~IdentifierID() { delete ids; }
};

class Identifier
{
    public:
    IdentifierType type;
    string name;
    int loc, dectype;

    Identifier(IdentifierType t, string n, int l = -1, int dt = DEC_LOCAL) { type = t; n = name; loc = l; dectype = dt; }
    Identifier(IdentifierType t) { type = t; }
    Identifier() { }
};

class Variable : public Identifier
{
    public:
    IdType* varType;
    bool isConst;
    Variable(string n, IdType *t) { name = n; type = ID_VAR; varType = t; isConst = false; }
};

class FunctionDeclaration : public Identifier
{
    public:
    IdType* retType;
    vector<Identifier *>* params; // What about scope? 
    Statement* stmt;
    //int val;
    FunctionDeclaration(string n, IdType* t, vector<Identifier *>* p, Statement * s) { name = n; type = ID_FUN; retType = t; params = p; stmt = s; }
};

class Expression: public Node
{
    public:
    Expression* left;
    IdentifierID* id;
    IdType* idType;
    Lexeme* lex;
    Expression* right;

    Expression(NodeSubType st, Expression *l, IdentifierID* i, Lexeme* le, Expression * r, IdType* t) { type = NODE_EXPR; subtype = st; left = l; lex = le; right = r; id = i; idType = t; }
    Expression() { type = NODE_EXPR; subtype = NODE_ERROR; left = NULL; lex = NULL; right = NULL; id = NULL; }
    //~Expression(){ delete left; delete right; delete lex; }
};

class Function : public Expression
{
    public:
    vector<Expression*>* params; // What about scope?
    //int val;
    Function(IdentifierID* i, vector<Expression*>* p) { id = i; type = NODE_EXPR; subtype = EXPR_ID; params = p; left = NULL; lex = NULL; right = NULL; }
};

class StatementList: public Statement
{
    public:
    vector<Statement*>* list;

    StatementList(vector<Statement*>* l) { type = NODE_STMT; subtype = STMT_LIST; list = l; }
    //~Statement() { for(int i = 0; i < list->size(); i++) delete (list*)[i]; delete list; }
};

class IfStmt: public Statement //If + Then Statement
{
    public:
    vector <Expression*>* exprs;
    vector <Statement*>* stmts; //You can have an infinite number of ELSIF statements... 
                               //the last statement is for then... so the # of expressions will always be 1 less than # of statements
    IfStmt(vector<Expression*> *e, vector<Statement*> *is) { type = NODE_STMT; subtype = STMT_IF; exprs = e; stmts = is;}
    //~IfStmt() { for(int i = 0; i < exprs->size(); i++) delete (exprs*)[i]; delete exprs; 
    //            for(int i = 0; i < stmts->size(); i++) delete (stmts*)[i]; delete stmts; }
};

class PrintStmt: public Statement //Print Statement
{
    public:
    Expression *expr;
    PrintStmt(Expression *e) { type = NODE_STMT; subtype = STMT_PRINT; expr = e; }
    //~PrintStmt() { delete expr; }
};

class IdStmt: public Statement //Print Statement
{
    public:
    Identifier *id;
    IdStmt(Identifier* i, NodeSubType s) { type = NODE_STMT; subtype = s; id = i;}
    //~PrintStmt() { delete expr; }
};

class RetStmt: public Statement //Return Statement
{
    public:
    Expression *expr;
    RetStmt(Expression *e) { type = NODE_STMT; subtype = STMT_RET; expr = e; }
    //~RetStmt() { delete expr; }
};

class CondStmt: public Statement //While, Unless, When Statements
{
    public:
    Expression *expr;
    Statement *stmt;
    CondStmt(NodeSubType st, Expression *e, Statement *s) { type = NODE_STMT; subtype = st; expr = e; stmt = s; }
    //~CondStmt() { delete expr; delete stmt; }
};

class AssignStmt: public Statement //Assignment Statement
{
    public:
    Identifier *var;
    Expression *expr;
    int follow;
    Expression *start;
    AssignStmt(Identifier *v, Expression *e) { type = NODE_STMT; subtype = STMT_ASSIGN; expr = e; var = v; follow = 0; start = NULL; }
    AssignStmt(Identifier *v, Expression *e, int f) { type = NODE_STMT; subtype = STMT_ASSIGN; expr = e; var = v; follow = f; start = NULL; }
    AssignStmt(Identifier *v, Expression *e, int f, Expression* o) { type = NODE_STMT; subtype = STMT_ASSIGN; expr = e; var = v; follow = f; start = o; }
    //~AssignStmt() { delete var; delete expr; }
};

class DecStmt: public Statement //Declaration Statement
{
    public:
    vector<Identifier *> *varList;
    //vector<Expression*> exprList; //Optional assignment
    DecStmt(vector<Identifier *> *v, NodeSubType t) { type = NODE_STMT; subtype = t; varList = v; }
    //~PrintStmt() { delete varList; }
};

class CaseStmt: public Statement //Case Statement
{
    public:
    vector<int>* intList;
    Statement *stmt;
    CaseStmt(vector<int> * i, Statement *s) { type = NODE_STMT; subtype = STMT_CASE; intList = i; stmt = s; }
    //~CaseStmt() { delete intList; }
};

class SwitchStmt: public Statement //Switch Statement
{
    public:
    Expression *expr;
    vector<CaseStmt*>* caseList;
    SwitchStmt(Expression *e, vector<CaseStmt*> *c) { type = NODE_STMT; subtype = STMT_SWITCH; caseList = c; expr = e; }
    //~StatementStmt() { for(int i = 0; i < caseList->size(); i++) delete (caseList*)[i]; delete caseList; delete expr; }
};


