#ifndef IN_PARSE
    #include "parse.h"
#endif

void printStatement(Node *, int );

void printVar(Identifier *id)
{
}

void printID(IdentifierID *id)
{
}

int codeGenExpr(ofstream& fout, Expression *expr);

void printExpression(Expression *expr)
{
    cout << " ( ";
    if(expr->left != NULL)
        printExpression(expr->left);
    if(expr->lex != NULL)
        cout << expr->lex->text;
    else if(expr->id != NULL)
        printID(expr->id);
    if(expr->right != NULL)
        printExpression(expr->right);
    cout << " ) ";
}

void printIfStmt(IfStmt *node, int indent)
{
    vector <Expression*> exprs = *(node->exprs);
    vector <Statement*> stmts = *(node->stmts);
    for(int i = 0; i < stmts.size(); i++)
    {
        if(i < exprs.size())
            cout << getTab(indent) << "Expression: ";
        printExpression(exprs[i]) ;
        cout << "\n";
        printStatement(stmts[i], indent);
    }
}

void printCondStmt(CondStmt *node, int indent)
{
    cout << getTab(indent) << "Expression: ";
    printExpression(node->expr);
    cout << "\n";
    printStatement(node->stmt, indent);
}

void printDecStmt(DecStmt *node, int indent)
{
    vector<Identifier*> varList = *(node->varList);
    cout << getTab(indent) << "DEC/FUN ";
    for(int i = 0; i < varList.size(); i++)
    {
        printVar(varList[i]);
        cout << ", ";
    }
    cout << "\n";
}

void printPrintStmt(PrintStmt *node, int indent)
{
    cout << getTab(indent) << "PRINT ";
    printExpression(node->expr);
    cout << "\n";
}

void printAssignStmt(AssignStmt *node, int indent)
{
    cout << getTab(indent);
    //printID(node->var);
    cout << " = ";
    printExpression(node->expr);
    cout << "\n";
}

void printStmtList(StatementList *node, int indent)
{
    vector<Statement*> list = *(node->list);
    for(int i = 0; i < list.size(); i++)
        printStatement(list[i], indent);
}



void printStatement(Node *node, int indent)
{
    cout << getTab(indent) << "Got ";
    switch(node->subtype)
    {
        case STMT_IF: cout << "IF Statment\n";
            printIfStmt((IfStmt*) node, indent + 1);
            break;

        case STMT_WHILE: cout << "WHILE Statment\n";
            printCondStmt((CondStmt*) node, indent + 1);
            break;

        case STMT_UNLESS: cout << "UNLESS Statment\n";
            printCondStmt((CondStmt*) node, indent + 1);
            break;

        case STMT_WHEN: cout << "WHEN Statment\n";
            printCondStmt((CondStmt*) node, indent + 1);
            break;

        case STMT_VAR: cout << "VAR DEC Statment\n";
            printDecStmt((DecStmt*) node, indent + 1);
            break;

        case STMT_FUN: cout << "FUN DEC Statment\n";
            printDecStmt((DecStmt*) node, indent + 1);
            break;

        case STMT_ASSIGN: cout << "ASSIGN Statment\n";
            printAssignStmt((AssignStmt*) node, indent + 1);
            break;

        case STMT_PRINT: cout << "PRINT Statment\n";
            printPrintStmt((PrintStmt*) node, indent + 1);
            break;
            
        case STMT_LIST: cout << "Statment List\n";
            printStmtList((StatementList*) node, indent + 1);
            break;

        default:
            cout << "ERROR\n";
            break;
    }
}

void printNode(Node *node, int indent)
{
    if(node->type == NODE_STMT)
        printStatement((Statement*)node, indent);
    else if(node->type == NODE_EXPR)
        printExpression((Expression*)node);
}

