#ifndef IN_PARSE
    #include "parse.h"
#endif

//Flag Indexs
const int FLAG_L = 0;
const int FLAG_G = 1;
const int FLAG_E = 2;
const int FLAG_LE = 3;
const int FLAG_GE = 4;
const int FLAG_NE = 5;
const int FLAG_RUN = 6;
const int FLAG_Z = 7;

//code gen numbers
unsigned int WHEN_NUM = 0;
unsigned int UNLESS_NUM = 0;
unsigned int WHILE_NUM = 0;
unsigned int IF_NUM = 0;
unsigned int FUN_NUM = 0;
unsigned int TEMP_NUM = 0;

vector<string> scopeList;
stack<int*> scopeDecs;
stack<int*> functDecs;
int curStackdiff = 0;

bool statRegs[] = { true, false, false, false, false, false, false, false, false, false, false, false };
const int numRegs = sizeof(statRegs) / sizeof(bool);

//Used to return values from expressions... 
//you need to know what register they are in and what type they are
class ExprRet
{
    public:
    int reg;
    IdType* type;

    ExprRet(int r, IdType* t) { reg = r; type = t; }
    ExprRet() { reg = -1; type = NULL; }
};

int getFreeReg()
{
    int i; 
    for(i = 0; i < numRegs; i++)
    {    
        if(statRegs[i] == false)
        {
            statRegs[i] = true;
            return i;
        }
    }
    return -1;
}

void putFreeReg(int num)
{
    statRegs[num] = false;
}

void putFullReg(int num)
{
    statRegs[num] = true;
}

void newFunction(ofstream& fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside newFunction\n";
    functDecs.push(new int(0));
    if(DEBUG) cout << getTab(indent) << "Returing from newFunction\n";
}

void oldFunction(ofstream& fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside oldFunction\n";
    functDecs.pop();
    if(DEBUG) cout << getTab(indent) << "Returing from oldFunction\n";
}

void newScope(ofstream& fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside newScope\n";
    fout << "\n\t;NEW SCOPE\n";
    scopeDecs.push(new int(0));
    if(DEBUG) cout << getTab(indent) << "Returing from newScope\n";
}

//used for return statements
void peekScope(ofstream& fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside peekScope\n";
    IdentifierID * id;
    int* decp = functDecs.top();
    int dec = *decp;

    if(dec > 0)
        fout << "\tADD\tSP, " << dec << "\n";

    if(DEBUG) cout << getTab(indent) << "Returing from peekScope\n";
}   

void oldScope(ofstream& fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside oldScope\n";
    IdentifierID * id;
    int* decp = scopeDecs.top();
    int dec = *decp;
    scopeDecs.pop();
    int *fdec = functDecs.top();
    *(fdec) = *(fdec) - dec;
    if(dec > 0)
        fout << "\tADD\tSP, " << dec << "\n";
    for(int i = scopeList.size(); i > scopeList.size() - dec; i--) 
    {
        string name = scopeList[i - 1];
        id = symbolTable.find(name);
        if(id == NULL)
            cout << "Didn't recognize identifier in oldscope \n";
        else
            id->ids->pop();
    }
    scopeList.resize(scopeList.size() - dec);
    fout << "\t;OLD SCOPE\n\n";
    if(DEBUG) cout << getTab(indent) << "Returning from oldScope\n";
    //delete decp;
}

bool codeGenStmt(ofstream&, Node *, int );
ExprRet codeGenExpr(ofstream&, Expression *, int);
bool handleDecStmt(ofstream&, DecStmt*, int);

bool codeGenFunctionCall(ofstream& fout, Function *fun, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenFunctionCall\n";
    vector<Expression *> exprs;
    int TEMP_CUR = TEMP_NUM;
    TEMP_NUM++;

    fout << "\t;function call to " << fun->id->name << "\n";

    /*
    vector<Identifier *> *varList = new vector<Identifier *>();
    varList->push_back(new Variable("temp" + TEMP_CUR));
    handleDecStmt(fout, new DecStmt(varList, STMT_VAR), indent + 1)
    */

    int i;
    ExprRet val;
    for(i = 0; i < fun->params->size(); i++)
    {
        exprs = *(fun->params);
        val = codeGenExpr(fout, exprs[i], indent + 1);
        if(val.reg == -1)
        {
            if(DEBUG) cout << getTab(indent) << "Returning from codeGenFunctionCall\n";
            return false;
        }
        fout << "\tPUSH\tR" << val.reg << "\n";
        putFreeReg(val.reg);
    }
    fout << "\tCALL\t" << fun->id->name << "\n";
    if(i > 0)
        fout << "\tADD\tSP, " << i << "\n";
    //fout << "\tSTORE R11, # " <<   
    fout << "\t;end of function call to " << fun->id->name << "\n\n";
    if(DEBUG) cout << getTab(indent) << "Returning from codeGenFunctionCall\n";
    return true;
}

bool codeGenCast(ofstream& fout, ExprRet res, VarType type, int indent)
{   
    //cout << VAR_INT << " " << VAR_FLOAT << " " << VAR_CHAR << " " << VAR_PTR << "\n";
    //cout << "Got cast from " << res.type->varType << " to " << type << "\n";
    if((res.type->varType == VAR_INT || res.type->varType == VAR_CHAR) && type == VAR_FLOAT)
        fout << "\tCAST\tR" << res.reg << ", IOTYPE_FLOAT\n";
    else if((type == VAR_INT || type == VAR_CHAR) && res.type->varType == VAR_FLOAT)
        fout << "\tCAST\tR" << res.reg << ", IOTYPE_INT\n";
    else if((res.type->varType == VAR_INT && type == VAR_CHAR) ||  (type == VAR_INT && res.type->varType == VAR_CHAR))
        return true;
    else if(((type == VAR_INT || type == VAR_CHAR) && res.type->varType == VAR_PTR) || ((res.type->varType == VAR_INT || res.type->varType == VAR_CHAR) && type == VAR_PTR))
        return true;
    else
    {
        cout << "Error casting";
        return false;
    }
    return true;
}

ExprRet codeGenFol(ofstream& fout, ExprRet ret, Expression *expr, int indent)
{
    fout << "\tLOAD\tR" << ret.reg << ", * R" << ret.reg << "\n";
    if(ret.type->ptr == NULL || ret.type->ptr->varType != VAR_PTR)
    {
        cout << "Error: You tried to follow type that wasn't a pointer.\n";        
        return *(new ExprRet());
    }
    ret.type = ret.type->ptr;
    if(expr->right->subtype != NODE_WRONG)
        return codeGenFol(fout, ret, expr->right, indent);
    else
        return ret;
}

ExprRet codeGenExpr(ofstream& fout, Expression *expr, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenExpr\n";
    ExprRet left, right;  
    if(expr->type != NODE_EXPR)
    {
        cout << "Error: codeGenExpr should be passed an expression\n";
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
        left.reg = -1;
        left.type->varType = VAR_ERROR;
        return left;
    }
    if(expr->subtype == EXPR_OP && expr->lex->type == OP)
    {
        if(expr->lex->num == OP_FOL)
        {
            //handle ~ specially
            left = codeGenExpr(fout, expr->left, indent + 1);
            if(left.reg == -1)
            {
                if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                    return left;
            }
            fout << "\tLOAD\tR" << left.reg << ", * R" << left.reg << "\n";
            if(expr->right->subtype != NODE_WRONG)
            {
                left = codeGenFol(fout, left, expr->right, indent + 1);
                if(left.reg == -1)
                {    
                    if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                    return left;
                }
            }
            
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
            return left;
        }

        if(expr->lex->num == OP_NOT)
        {
            //handle ! specially
            left = codeGenExpr(fout, expr->left, indent + 1);
            right = codeGenExpr(fout, expr->right, indent + 1);
            if(left.reg == -1)
            {
                if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                return left;
            }
            else if(right.reg == -1)
            {    
                if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                return right;
            }

            fout << "\tADD\tR" << left.reg << ", R" << right.reg << "\n";
            fout << "\tLOAD\tR" << left.reg << ", * R" << left.reg << "\n";
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
            return left;
        }

        int flag = -1;
        VarType type = VAR_INT;
        bool floatCalc = false;
        if(expr->left != NULL && expr->right != NULL)
        {
            left = codeGenExpr(fout, expr->left, indent + 1);
            right = codeGenExpr(fout, expr->right, indent + 1);
            if(left.reg == -1)
            {
                if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                return left;
            }
            else if(right.reg == -1)
            {    
                if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                return right;
            }

            if(left.type->varType == VAR_FLOAT || right.type->varType == VAR_FLOAT)
            {
                bool retval = true;
                type = VAR_FLOAT;
                floatCalc = true;
                if(left.type->varType != VAR_FLOAT)
                    retval = codeGenCast(fout, left, VAR_FLOAT, indent + 1);
                else if(right.type->varType != VAR_FLOAT)
                    retval = codeGenCast(fout, right, VAR_FLOAT, indent + 1);
                if(!retval)
                {
                    left.reg = -1;
                    left.type->varType = VAR_ERROR;
                    if(DEBUG) cout << getTab(indent) << "Returing ERROR from codeGenExpr\n";
                    return left;
                }
            }
        }
        else
        {
            cout << "Error: An expression had a NULL value.\n";
            left.reg = -1;
            left.type->varType = VAR_ERROR;
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
            return left;
        }

        fout << "\t";
        if(floatCalc)
        {
            switch(expr->lex->num)
            {
                case OP_POW:
                case OP_ADD:
                case OP_SUB:
                case OP_DIV:
                case OP_MUL:
                case OP_EQU:
                case OP_GT:
                case OP_LT:
                case OP_GTE:
                case OP_LTE:
                case OP_NE:
                    fout << "F";
                    break;

                default:
                    cout << "Error: Unknown operation for floating point numbers\n";
                    left.reg = -1;
                    left.type->varType = VAR_ERROR;
                    if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
                    return left;
            }
        }

        switch(expr->lex->num)
        {
            case OP_POW:
                //Implement this later
                break;

            case OP_ADD:
                fout << "ADD\t";
                break;

            case OP_SUB:
                fout << "SUB\t";
                break;

            case OP_DIV:
                fout << "DIV\t";
                break;

            case OP_MUL:
                fout << "MUL\t";
                break;

            case OP_EQU:
                fout << "CMP\t";
                flag = FLAG_E;
                break;

            case OP_GT:
                fout << "CMP\t";
                flag = FLAG_G;
                break;

            case OP_LT:
                fout << "CMP\t";
                flag = FLAG_L;
                break;

            case OP_AND:
                fout << "AND\t";
                break;

            case OP_OR:
                fout << "OR\t";
                break;

            case OP_MOD:
                fout << "MOD\t";
                break;

            case OP_GTE:
                fout << "CMP\t";
                flag = FLAG_GE;
                break;

            case OP_LTE:
                fout << "CMP\t";
                flag = FLAG_LE;
                break;

            case OP_NE:
                fout << "CMP\t";
                flag = FLAG_NE;
                break;

            case OP_SFTL:
                fout << "SFTL\t";
                break;

            case OP_SFTR:
                fout << "SFTR\t";
                break;

            default:
                cout << "Error: Unknown operation\n";
                left.reg = -1;
                left.type->varType = VAR_ERROR;
                if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
                return left;
        }
        fout << "R" << left.reg << ", R" << right.reg << "\n";
        
        if(flag != -1)
        {
            fout << "\tFLAG\tR" << left.reg << ", ";
            switch(flag)
            {
                case FLAG_G: fout << "FLAG_G"; break;
                case FLAG_GE: fout << "FLAG_GE"; break;
                case FLAG_L: fout << "FLAG_L"; break;
                case FLAG_LE: fout << "FLAG_LE"; break;
                case FLAG_E: fout << "FLAG_E"; break;
                case FLAG_NE: fout << "FLAG_NE"; break;
            }
            fout << "\n";
        }
        putFreeReg(right.reg);
        left.type->varType = type;
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
        return left;
    }
    else if(expr->subtype == EXPR_NUM)
    {
        left.reg = getFreeReg();
        fout << "\tLOAD\t R" << left.reg << ", " << expr->lex->text << "\n";
        left.type = new IdType(VAR_INT);
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
        return left;
    }
    else if(expr->subtype == EXPR_CREATE)
    {
        
        left.reg = 11;
        fout << "\t;function call to create\n";
	    fout << "\tLOAD	 R1, " << expr->idType->size << "\n";
	    fout << "\tPUSH	R1\n";
	    fout << "\tCALL	create\n";
	    fout << "\tADD	SP, 1\n";
	    fout << "\t;end of function call to create\n";
        left.type = expr->idType;
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
        cout.flush();
        return left;
    }
    else if(expr->subtype == EXPR_ID)
    {
        if(expr->id->ids == NULL)
        {
            cout << "Undeclared identifier: " << expr->id->name << "\n";
            left.reg = -1;
            left.type->varType = VAR_ERROR;
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
            return left;
        }        

        Identifier * id = expr->id->ids->top();
        if(id->type == ID_VAR)
        {
            Variable * var = (Variable *) id;
            left.reg = getFreeReg();
            left.type = var->varType;
            if(id->dectype == DEC_GLOBAL)
                fout << "\tLOAD\t R" << left.reg << ", * # " << var->name << "\n";
            else if(id->dectype == DEC_PARAM)
                fout << "\tLOAD\t R" << left.reg << ", * # FP + " << var->loc + 1 << "\n";
            else if(id->dectype == DEC_LOCAL)
                fout << "\tLOAD\t R" << left.reg << ", * # FP - " << var->loc << "\n";
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
            return left;
        }
        else if(id->type == ID_FUN)
        {
            FunctionDeclaration * fundec = (FunctionDeclaration *) id;
            if(!codeGenFunctionCall(fout, (Function *) expr, indent + 1))
            {
                left.reg = -1;
                left.type->varType = VAR_ERROR;
                if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
                return left;
            }
            putFullReg(11);       
            left.reg = 11;
            left.type = fundec->retType;
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
            return left;
        }   
        else
        {
            cout << "Error: Unknown type of identifier\n";
        }    
    }
    else
    {
        cout << "Error: Unknown type of expression\n";
    }
    left.reg = -1;
    left.type->varType = VAR_ERROR;
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenExpr\n";
    return left;
}

bool codeGenIf(ofstream& fout, IfStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenIf\n";
    vector <Expression*> exprs = *(node->exprs);
    vector <Statement*> stmts = *(node->stmts);
    int CUR_NUM = IF_NUM;
    IF_NUM++;
    int ifnum;
    fout << "\n\t;if stmt " << CUR_NUM << "\n";
    for(ifnum = 0; ifnum < exprs.size(); ifnum++)
    {
        ExprRet res = codeGenExpr(fout, exprs[ifnum], indent);
        if(res.reg == -1)
        {
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenIf\n";
            return false;
        }
        fout << "\tCMP\tR" << res.reg << ", 0\n";
        fout << "\tJMPE\tif" << CUR_NUM << "e" << ifnum + 1 << "\n";
        putFreeReg(res.reg);

        newScope(fout, indent + 1);
        if(!codeGenStmt(fout, stmts[ifnum], indent))
        {
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenIf\n";
            return false;
        }
        oldScope(fout, indent + 1);

        fout << "\tJMP\tifd" << CUR_NUM << "\n";
        fout << "\tif" << CUR_NUM << "e" << ifnum + 1 << ":\n";
    }

    //deal with else
    if(stmts.size() > exprs.size())
    {
        /*
        int res = codeGenExpr(fout, exprs[ifnum], indent);
        fout << "\tCMP\tR" << res << ", 0\n";
        fout << "\tJMPE\tifd" << CUR_NUM << "\n";
        putFreeReg(res);*/

        newScope(fout, indent + 1);
        if(!codeGenStmt(fout, stmts[ifnum], indent))
        {   
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenIf\n";
            return false;
        }
        oldScope(fout, indent + 1);
    }

    fout << "ifd" << CUR_NUM << ":\n";
    fout << "\t;end of if stmt " << CUR_NUM << "\n\n";
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenIf\n";
    return true;
}

bool codeGenWhile(ofstream& fout, CondStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenWhile\n";
    int CUR_NUM = WHILE_NUM;
    WHILE_NUM++;
    fout << "whiles" << CUR_NUM << ":\n";
    ExprRet res = codeGenExpr(fout, node->expr, indent);
    if(res.reg == -1)
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenWhile\n";
        return false;
    }
    fout << "\tCMP\tR" << res.reg << ", 0\n";
    fout << "\tJMPE\twhiled" << CUR_NUM << "\n";
    putFreeReg(res.reg);

    newScope(fout, indent + 1);    
    if(!codeGenStmt(fout, node->stmt, indent))
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenWhile\n";
        return false;
    }
    oldScope(fout, indent + 1);

    fout << "\tJMP\twhiles" << CUR_NUM << "\n";
    fout << "\nwhiled" << CUR_NUM << ":\n";
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenWhile\n";
    return true;
}

bool codeGenWhen(ofstream& fout, CondStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenWhen\n";
    int CUR_NUM = WHEN_NUM;
    WHEN_NUM++;
    ExprRet res = codeGenExpr(fout, node->expr, indent);
    if(res.reg == -1)
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenWhen\n";    
        return false;
    }
    fout << "\tCMP\tR" << res.reg << ", 0\n";
    fout << "\tJMPE\twhen" << CUR_NUM << "\n";
    putFreeReg(res.reg);

    newScope(fout, indent + 1);
    if(!codeGenStmt(fout, node->stmt, indent))
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenWhen\n";
        return false;
    }
    oldScope(fout, indent + 1);    
    
    fout << "\nwhen" << CUR_NUM << ":\n";
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenWhen\n";
    return true;
}

bool codeGenUnless(ofstream& fout, CondStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenUnless\n";
    int CUR_NUM = WHEN_NUM;
    UNLESS_NUM++;
    ExprRet res = codeGenExpr(fout, node->expr, indent);
    if(res.reg == -1)
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenUnless\n";
        return false;
    }
    fout << "\tCMP\tR" << res.reg << ", 0\n";
    fout << "\tJMPNE\tunl" << CUR_NUM << "\n";
    putFreeReg(res.reg);

    newScope(fout, indent + 1);
    codeGenStmt(fout, node->stmt, indent);
    oldScope(fout, indent + 1);    

    fout << "\nunl" << CUR_NUM << ":\n";
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenUnless\n";
    return true;
}

bool codeGenRead(ofstream& fout, IdStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenRead\n";

    IdentifierID *id = symbolTable.find(node->id->name);
    if(id == NULL || id->ids == NULL || id->ids->top() == NULL)
    {
        cout << "Error: Undeclared identifier: " << node->id->name << "\n";
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenRead\n";
        return false;
    }
    Identifier* identifier = id->ids->top();
    if(identifier->type == ID_VAR)
    {
        Variable* var = (Variable *) identifier;
        fout << "\n\t;function call to readi\n";
        fout << "\tPUSH # io           ; Push io block\n";
        if(var->varType->varType == VAR_FLOAT)
            fout << "\tCALL readf\n";
        else if(var->varType->varType == VAR_CHAR)
            fout << "\tCALL readc\n";
        else 
            fout << "\tCALL readi\n";
        fout << "\tADD\tSP, 1  ; POP locals back off stack\n";
        fout << "\t;end of function call to readi\n";
        if(var->dectype == DEC_GLOBAL)
            fout << "\tSTORE\t R11, # " << var->name << "\n\n";
        else if(var->dectype == DEC_PARAM)
            fout << "\tSTORE\t R11, # FP + " << var->loc + 1 << "\n\n";
        else if(var->dectype == DEC_LOCAL)
            fout << "\tSTORE\t R11, # FP - " << var->loc << "\n\n";
    }
    else
    {
        cout << "Error: Identifier passed to destroy must be a pointer (PTR) type\n";
        if(DEBUG) cout << getTab(indent) << "Returning ERROR from codeGenRead\n";
        return false;
    }
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenRead\n";
    return true;
}

bool codeGenDestroy(ofstream& fout, IdStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenDestroy\n";

    IdentifierID *id = symbolTable.find(node->id->name);
    if(id == NULL || id->ids == NULL || id->ids->top() == NULL)
    {
        cout << "Error: Undeclared identifier: " << node->id->name << "\n";
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenDestroy\n";
        return false;
    }
    Identifier* identifier = id->ids->top();
    if(identifier->type == ID_VAR)
    {
        Variable* var = (Variable *) identifier;
        if(var->varType->varType = VAR_PTR)
        {
            fout << "\t;function call to destroy\n";
            if(var->dectype == DEC_GLOBAL)
                fout << "\tLOAD\t R1, * # " << var->name << "\n";
            else if(var->dectype == DEC_PARAM)
                fout << "\tLOAD\t R1, * # FP + " << var->loc + 1 << "\n";
            else if(var->dectype == DEC_LOCAL)
                fout << "\tLOAD\t R1, * # FP - " << var->loc << "\n";
	        fout << "\tPUSH	R1\n";
	        fout << "\tCALL	destroy\n";
	        fout << "\tADD	SP, 1\n";
	        fout << "\t;end of function call to destroy\n";
        }
        else
        {
            cout << "Error: Identifier passed to destroy must be a pointer (PTR) type\n";
            if(DEBUG) cout << getTab(indent) << "Returning ERROR from codeGenDestroy\n";
            return false;
        }
    }
    else
    {
        cout << "Identifier passed to destroy must be a pointer (PTR) type\n";
        if(DEBUG) cout << getTab(indent) << "Returning ERROR from codeGenDestroy\n";
        return false;
    }
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenDestroy\n";
    return true;
}

bool codeGenPrint(ofstream& fout, PrintStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenPrint\n";
    ExprRet res = codeGenExpr(fout, node->expr, indent);
    if(res.reg == -1)
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenPrint\n";
        return false;
    }

    fout << "\n\t;function call to print\n";
    fout << "\tPUSH # io           ; Push io block\n";
    fout << "\tPUSH R" << res.reg << "       ; Push the value to print\n";
    if(res.type->varType == VAR_FLOAT)
        fout << "\tCALL printf\n";
    else if(res.type->varType == VAR_CHAR)
        fout << "\tCALL printc\n";
    else
        fout << "\tCALL printi\n";
    fout << "\tADD\tSP, 2  ; POP locals back off stack\n";
    fout << "\t;end of function call to print\n\n";
    putFreeReg(res.reg);
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenPrint\n";
    return true;
}

bool codeGenAssign(ofstream& fout, AssignStmt *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenAssign\n";
    IdentifierID *id = symbolTable.find(node->var->name);
    if(id == NULL || id->ids == NULL || id->ids->top() == NULL)
    {
        cout << "Error: Undeclared identifier: " << node->var->name << "\n";
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenAssign\n";
        return false;
    }
    Identifier* identifier = id->ids->top();
    if(identifier->type == ID_VAR)
    {
        Variable *var = (Variable *) identifier;
        ExprRet res = codeGenExpr(fout, node->expr, indent + 1);
        if(res.reg == -1)
        {
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenAssign\n";
            return false;
        }
        //cout << res.type << "( " << VAR_FLOAT << ", " << VAR_INT << ", " << VAR_ERROR << ") " << var->varType << "\n";
        VarType testing = res.type->varType;
        VarType testing2 = var->varType->varType;
        if(testing != testing2)
        {
            if(!codeGenCast(fout, res, var->varType->varType, indent + 1))
            {
                if(DEBUG) cout << getTab(indent) << "Returing from codeGenAssign\n";
                return false;
            }
        }

        int place = getFreeReg();
        ExprRet sec;
        if(identifier->dectype == DEC_GLOBAL)
            fout << "\tLOAD\t R" << place << ", # " << identifier->name << "\n";
        else if(identifier->dectype == DEC_PARAM)
            fout << "\tLOAD\t R" << place << ", # FP + " << identifier->loc + 1 << "\n";
        else if(identifier->dectype == DEC_LOCAL)
            fout << "\tLOAD\t R" << place << ", # FP - " << identifier->loc << "\n";

        for(int i = 0; i < node->follow; i++)
            fout << "\tLOAD\tR" << place << ", * R" << place << "\n";
        
        if(node->start != NULL)
        {
            fout << ";getting expr\n";
            sec = codeGenExpr(fout, node->start, indent);
            if(sec.reg == -1)
            {
                if(DEBUG) cout << getTab(indent) << "Returing from codeGenAssign\n";
                return false;
            }
            //fout << "\tLOAD\tR" << place << ", * R" << place << "\n";
            fout << "\tADD R" << place << ", R" << sec.reg << "\n";
            putFreeReg(sec.reg);
            fout << ";done getting expr\n";
        }

        fout << "\tSTORE\t R" << res.reg << ", R" << place << "\n";

        putFreeReg(place);
        putFreeReg(res.reg);
    }
    else
    {
        cout << "Error: You can't assign to a function.\n";
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenAssign\n";
        return false;
    }
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenAssign\n";
    return true;
}

bool codeGenStmtList(ofstream& fout, StatementList *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenStmtList\n";
    vector<Statement*> list = *(node->list);
    for(int i = 0; i < list.size(); i++)
    {
        if(!codeGenStmt(fout, list[i], indent))
        {
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenStmtList\n";
            return false;
        }
    }
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenStmtList\n";
    return true;
}

bool codeGenFunction(ofstream& fout, FunctionDeclaration* node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenFunction\n";
    int CUR_NUM = FUN_NUM;
    FUN_NUM++;
    
    fout << "\tJMP funs" << CUR_NUM;  
    fout << "\n\n\t;START FUNCTION " << node->name << "\n";
    fout << node->name << ":\n";
    fout << "\tPUSH\tFP\n";
    fout << "\tMOV\tFP, SP\n";
    fout << "\tADD\tFP, 1\n";
    Identifier *identifier;
    IdentifierID * id;
    vector<Identifier *> ids;

    for(int i = 0; i < node->params->size(); i++)
    {
        ids = *(node->params);
        identifier = ids[i];
        id = symbolTable.find(identifier->name);
        if(id == NULL)
        {
            cout << "Error: Didn't recognize identifier: " << identifier->name << "\n";
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenFunction\n";
            return false;        
        }
        else
        {
            scopeList.push_back(identifier->name);
            identifier->dectype = DEC_PARAM;
            identifier->loc = node->params->size() - i ;
            id->ids->push(identifier);
        }
    }

    newFunction(fout, indent + 1);
    newScope(fout, indent + 1);
    if(!codeGenStmt(fout, node->stmt, indent + 1))
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenFunction\n";
        return false;
    }
    oldScope(fout, indent + 1);
    oldFunction(fout, indent + 1);

    fout << "\tLOAD\tR11, 0\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\t\n\t;END FUNCTION " << node->name << "\n\n";
    fout << "funs" << CUR_NUM << ":\n";  

    if(DEBUG) cout << getTab(indent) << "Returing from codeGenFunction\n";
    return true;
}

bool handleDecStmt(ofstream& fout, DecStmt* node, int indent)
{   
    if(DEBUG) cout << getTab(indent) << "Inside handleDecStmt\n";
    vector<Identifier*> all = *(node->varList);
    int size = 0;
    for(int i = 0; i < all.size(); i++)
    {
        Identifier* identifier = all[i];
        IdentifierID * id = symbolTable.find(identifier->name);
        if(id == NULL)
        {
            cout << "Error: Didn't recognize identifier: " << identifier->name << "\n";
            if(DEBUG) cout << getTab(indent) << "Returing from handleDecStmt\n";
            return false;
        }        
        else
        {
            if(identifier->type == ID_FUN)
            {
                scopeList.push_back(identifier->name);
                id->ids->push(identifier);
                if(!codeGenFunction(fout, (FunctionDeclaration*) identifier, indent + 1))
                {
                    if(DEBUG) cout << getTab(indent) << "Returing from handleDecStmt\n";
                    return false;
                }
            }
            else
            {
                if(identifier->dectype == DEC_LOCAL)
                {
                    //cout << "Adding dec of " << identifier->name << "\n";
                    *(scopeDecs.top()) = *(scopeDecs.top()) + 1;
                    *(functDecs.top()) = *(functDecs.top()) + 1;
                    identifier->loc = *(functDecs.top());
                    scopeList.push_back(identifier->name);
                    id->ids->push(identifier);
                    size++;
                }
                else if(identifier->dectype == DEC_GLOBAL)
                {
                    
                }
                else
                {
                    cout << "Error: Unknown type of declaraion type for variable " << identifier->name << "\n";
                    return false;
                }
            }
        }
    }
    if(size != 0)
        fout << "\tSUB\tSP, " << size << "\n";
    if(DEBUG) cout << getTab(indent) << "Returing from handleDecStmt\n";
    return true;
}

bool codeGenRet(ofstream& fout, RetStmt* node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenRet\n";
    ExprRet res = codeGenExpr(fout, node->expr, indent);
    if(res.reg == -1)
    {
        if(DEBUG) cout << getTab(indent) << "Returning from codeGenRet\n";
        return false;
    }
    fout << "\tMOV\tR11, R" << res.reg << "\n";

    peekScope(fout, indent + 1);

    fout << "\tPOP\tFP\n";
    fout << "\tRET\n";
    putFreeReg(res.reg);
    if(DEBUG) cout << getTab(indent) << "Returning from codeGenRet\n";
    return true;
}

/*
void genJumpIfTrue ( ofstream& fout, Expression *expr, string label )
{
    int left = codeGenExpr( fout, expr->left );
    int right = codeGenExpr( fout, expr->right );
    
    fout << "\tCMP\t R" << left << ", R" << right << endl;
    putFreeReg ( right );
    putFreeReg ( left );
    
    switch ( expr->lex->num )
    {
        case OP_GTE:
            fout << "\tJMPGE\t " << label << endl;
            break;
        case OP_LTE:
            fout << "\tJMPLE\t " << label << endl;
            break;
        case OP_GT:
            fout << "\tJMPG\t " << label << endl;
            break;
        case OP_LT:
            fout << "\tJMPL\t " << label << endl;
            break;
        case OP_EQU:
            fout << "\tJMPE\t " << label << endl;
            break;
        case OP_NE:
            fout << "\tJMPN\t " << label << endl;
            break;
    }   
    
}

void genJumpIfFalse ( ofstream& fout, Expression *expr, string label )
{
    int left = codeGenExpr( fout, expr->left );
    int right = codeGenExpr( fout, expr->right );
    
    fout << "\tCMP\t R" << left << ", R" << right << endl;
    putFreeReg ( right );
    putFreeReg ( left );
    
    switch ( expr->lex->num )
    {
        case OP_GTE:
            fout << "\tJMPGE\t " << label << endl;
            break;
        case OP_LTE:
            fout << "\tJMPLE\t " << label << endl;
            break;
        case OP_GT:
            fout << "\tJMPG\t " << label << endl;
            break;
        case OP_LT:
            fout << "\tJMPL\t " << label << endl;
            break;
        case OP_EQU:
            fout << "\tJMPE\t " << label << endl;
            break;
        case OP_NE:
            fout << "\tJMPN\t " << label << endl;
            break;
    }   
}
*/

bool codeGenStmt(ofstream &fout, Node *node, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenStmt\n";
    bool res;
    switch(node->subtype)
    {
        case STMT_IF:
            res = codeGenIf(fout, (IfStmt*) node, indent + 1);
            break;

        case STMT_RET:
            res = codeGenRet(fout, (RetStmt*) node, indent + 1);
            break;

        case STMT_WHILE:
            res = codeGenWhile(fout, (CondStmt*) node, indent + 1);
            break;

        case STMT_UNLESS:
            res = codeGenUnless(fout, (CondStmt*) node, indent + 1);
            break;

        case STMT_WHEN:
            res = codeGenWhen(fout, (CondStmt*) node, indent + 1);
            break;

        case STMT_VAR:
            res = handleDecStmt(fout, (DecStmt*) node, indent + 1);
            break;

        case STMT_FUN:
            res = handleDecStmt(fout, (DecStmt*) node, indent + 1);
            break;

        case STMT_ASSIGN:
            res = codeGenAssign(fout, (AssignStmt*) node, indent + 1);
            break;

        case STMT_READ:
            res = codeGenRead(fout, (IdStmt*) node, indent + 1);
            break;
        
        case STMT_DEST:
            res = codeGenDestroy(fout, (IdStmt*) node, indent + 1);
            break;

        case STMT_PRINT:
            res = codeGenPrint(fout, (PrintStmt*) node, indent + 1);
            break;
            
        case STMT_LIST:
            res = codeGenStmtList(fout, (StatementList*) node, indent + 1);
            break;

        default:
            cout << "Error writing\n";
            res = false;
            break;
    }
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenStmt\n";
    if(!res)
        return false;
    return true;
}

void codeGenFunction(ofstream &fout, Identifier * id, int indent)
{
    

}

bool codeGenVars(ofstream &fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenVars\n";
    DecStmt *stmt;
    vector<Identifier *> list;
    for(int i = 0; i < symbolList.size(); i++)
    {
        stmt = symbolList[i];
        list = *(stmt->varList);
        if(stmt->subtype == STMT_VAR)
        {
            Identifier * id; 
            IdentifierID *ids;
            for(int j = 0; j < list.size(); j++)
            {
                id = (Identifier *) list[j];
                id->dectype = DEC_GLOBAL;
                fout << id->name << ":\t.data 0\n";
                ids = symbolTable.find(id->name);
                ids->ids->push(id);
            }
        }        
        else if(stmt->subtype == STMT_FUN)
        {
            //codeGenFunction(fout, (Identifier *) list[0], indent + 1);
        }
    }
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenVars\n";
    return true;
}

bool codeGenSetup(ofstream &fout, int indent)
{   
    if(DEBUG) cout << getTab(indent) << "Inside codeGenSetup\n";
    fout << "\tJMP main\n\n";
       
    fout << ";Disc IO stuff\n";
    fout << ";op = 0 read / 1 write / 2 mount / 3 unmount / 4 create\n";
    fout << ";dev = 0 stdin / 1 stdout / 2 stderr\n\n";

    fout << ";Type constants\n";
    fout << ".define IOTYPE_CHAR 0\n";
    fout << ".define IOTYPE_INT 1\n";
    fout << ".define IOTYPE_STRING 2\n";
    fout << ".define IOTYPE_FLOAT 3\n\n";

    fout << ";Flag constants\n";
    fout << ".define FLAG_L 0\n";
    fout << ".define FLAG_G 1\n";
    fout << ".define FLAG_E 2\n";
    fout << ".define FLAG_LE 3\n";
    fout << ".define FLAG_GE 4\n";
    fout << ".define FLAG_NE 5\n";
    fout << ".define FLAG_RUN 6\n";
    fout << ".define FLAG_Z 7\n";
    //fout << "HP:	.data 0\n\n";

    fout << "io:       \n";
    fout << "io_op:      .data   1\n";
    fout << "io_dev:     .data   1\n";
    fout << "io_block:   .data   IOTYPE_INT\n";
    fout << "io_addr:    .data   0\n\n";

    fout << "\t;Prints a character\n";
    fout << "printc:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n";

	fout << "\tLOAD\tR2, * # FP + 3\n";
	fout << "\tLOAD\tR3, * # FP + 2\n";
    fout << "\tLOAD\tR1, 1\n";
    fout << "\tSTORE R1, R2        ; Set the op\n";
    fout << "\tSTORE R1, R2 + 1    ; Set the device\n";
    fout << "\tLOAD R1, IOTYPE_CHAR\n";
    fout << "\tSTORE R1, R2 + 2    ; Set the type to int\n";
    fout << "\tSTORE R3, R2 + 3    ; Set the int to i\n";
    fout << "\tIOOP R1, R2         ; Write the char i\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\n\n";

    fout << "\t;Prints a float\n";
    fout << "printf:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n";

	fout << "\tLOAD\tR2, * # FP + 3\n";
	fout << "\tLOAD\tR3, * # FP + 2\n";
    fout << "\tLOAD\tR1, 1\n";
    fout << "\tSTORE R1, R2        ; Set the op\n";
    fout << "\tSTORE R1, R2 + 1    ; Set the device\n";
    fout << "\tLOAD R1, IOTYPE_FLOAT\n";
    fout << "\tSTORE R1, R2 + 2    ; Set the type to int\n";
    fout << "\tSTORE R3, R2 + 3    ; Set the int to i\n";
    fout << "\tIOOP R1, R2         ; Write the char i\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\n\n";

    fout << "\t;Prints an integer\n";
    fout << "printi:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n";

	fout << "\tLOAD\tR2, * # FP + 3\n";
	fout << "\tLOAD\tR3, * # FP + 2\n";
    fout << "\tLOAD\tR1, 1\n";
    fout << "\tSTORE R1, R2        ; Set the op\n";
    fout << "\tSTORE R1, R2 + 1    ; Set the device\n";
    fout << "\tLOAD R1, IOTYPE_INT\n";
    fout << "\tSTORE R1, R2 + 2    ; Set the type to int\n";
    fout << "\tSTORE R3, R2 + 3    ; Set the int to i\n";
    fout << "\tIOOP R1, R2         ; Write the char i\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\n\n";

    fout << "\t;Reads an integer\n";
    fout << "readi:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n";

	fout << "\tLOAD\tR2, * # FP + 2\n";
    fout << "\tLOAD\tR1, 0\n";
    fout << "\tSTORE R1, R2        ; Set the op\n";
    fout << "\tSTORE R1, R2 + 1    ; Set the device\n";
    fout << "\tLOAD R1, IOTYPE_INT\n";
    fout << "\tSTORE R1, R2 + 2    ; Set the type to int\n";
    fout << "\tIOOP R11, R2        ; Read the int\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\n\n";

    fout << "\t;Reads a float\n";
    fout << "readf:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n";

	fout << "\tLOAD\tR2, * # FP + 2\n";
    fout << "\tLOAD\tR1, 0\n";
    fout << "\tSTORE R1, R2        ; Set the op\n";
    fout << "\tSTORE R1, R2 + 1    ; Set the device\n";
    fout << "\tLOAD R1, IOTYPE_FLOAT\n";
    fout << "\tSTORE R1, R2 + 2    ; Set the type to float\n";
    fout << "\tIOOP R11, R2        ; Read the float\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\n\n";

    fout << "\t;Reads a char\n";
    fout << "readc:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n";

	fout << "\tLOAD\tR2, * # FP + 2\n";
    fout << "\tLOAD\tR1, 0\n";
    fout << "\tSTORE R1, R2        ; Set the op\n";
    fout << "\tSTORE R1, R2 + 1    ; Set the device\n";
    fout << "\tLOAD R1, IOTYPE_CHAR\n";
    fout << "\tSTORE R1, R2 + 2    ; Set the type to char\n";
    fout << "\tIOOP R11, R2        ; Read the char\n";
    fout << "\tPOP\tFP\n";
    fout << "\tRET\n\n";

	fout << "\t;START FUNCTION create\n";
	fout << "create:\n";
	fout << "\tPUSH	FP\n";
	fout << "\tMOV	FP, SP\n";
	fout << "\tADD	FP, 1\n\n";

	fout << "\t	;NEW SCOPE\n";
	fout << "\t	SUB	SP, 1\n";
	fout << "\t	SUB	SP, 1\n";
	fout << "\t	LOAD	 R1, 0\n";
	fout << "\t	LOAD	 R2,  # FP - 2\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # HP\n";
	fout << "\t	LOAD	 R2,  # FP - 1\n";
	fout << "\t	STORE	 R1, R2\n\n";

	fout << "\t	;if stmt 0\n";
	fout << "\t	LOAD	 R1, * # FP + 2\n";
	fout << "\t	LOAD	 R2, 2\n";
	fout << "\t	MOD	R1, R2\n";
	fout << "\t	LOAD	 R2, 0\n";
	fout << "\t	CMP	R1, R2\n";
	fout << "\t	FLAG	R1, FLAG_NE\n";
	fout << "\t	CMP	R1, 0\n";
	fout << "\t	JMPE	destroyif0e1\n\n";

	fout << "\t	;NEW SCOPE\n";
	fout << "\t	LOAD	 R1, * # FP + 2\n";
	fout << "\t	LOAD	 R2, 1\n";
	fout << "\t	ADD	R1, R2\n";
	fout << "\t	LOAD	 R2,  # FP + 2\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	;OLD SCOPE\n\n";

	fout << "\t	JMP	destroyifd0\n";
	fout << "\t	destroyif0e1:\n";
	fout << "destroyifd0:\n";
	fout << "\t	;end of if stmt 0\n\n";

	fout << "destroywhiles0:\n";
	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	LOAD	 R2, 0\n";
	fout << "\t	CMP	R1, R2\n";
	fout << "\t	FLAG	R1, FLAG_NE\n";
	fout << "\t	CMP	R1, 0\n";
	fout << "\t	JMPE	destroywhiled0\n\n";

	fout << "\t	;NEW SCOPE\n";
	fout << "\t	LOAD	 R1, * # FP + 2\n";
	fout << "\t	LOAD	 R2, * # FP - 1\n";
	fout << "\t	LOAD	 R3, 1\n";
	fout << "\t	SUB	R2, R3\n";
	fout << "\t	LOAD	R2, * R2\n";
	fout << "\t	CMP	R1, R2\n";
	fout << "\t	FLAG	R1, FLAG_LE\n";
	fout << "\t	CMP	R1, 0\n";
	fout << "\t	JMPE	destroywhen0\n\n";

	fout << "\t	;NEW SCOPE\n";
	fout << "\t	SUB	SP, 1\n";
	fout << "\t	SUB	SP, 1\n";
	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	LOAD	 R2, 1\n";
	fout << "\t	SUB	R1, R2\n";
	fout << "\t	LOAD	 R2,  # FP - 3\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	LOAD	 R2, * # FP + 2\n";
	fout << "\t	ADD	R1, R2\n";
	fout << "\t	LOAD	 R2,  # FP - 4\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # FP - 3\n";
	fout << "\t	LOAD	R1, * R1\n";
	fout << "\t	LOAD	 R2, * # FP + 2\n";
	fout << "\t	SUB	R1, R2\n";
	fout << "\t	LOAD	 R2,  # FP - 4\n";
	fout << "\t	LOAD	R2, * R2\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # FP - 4\n";
	fout << "\t	LOAD	 R2, 1\n";
	fout << "\t	ADD	R1, R2\n";
	fout << "\t	LOAD	 R2,  # FP - 4\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	LOAD	R1, * R1\n";
	fout << "\t	LOAD	 R2,  # FP - 4\n";
	fout << "\t	LOAD	R2, * R2\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # FP + 2\n";
	fout << "\t	LOAD	 R2,  # FP - 3\n";
	fout << "\t	LOAD	R2, * R2\n";
	fout << "\t	STORE	 R1, R2\n\n";

	fout << "\t;if stmt 1\n";
	fout << "\t	LOAD	 R1, * # FP - 2\n";
	fout << "\t	LOAD	 R2, 0\n";
	fout << "\t	CMP	R1, R2\n";
	fout << "\t	FLAG	R1, FLAG_NE\n";
	fout << "\t	CMP	R1, 0\n";
	fout << "\t	JMPE	destroyif1e1\n\n";

	fout << "\t	;NEW SCOPE\n";
	fout << "\t	LOAD	 R1, * # FP - 4\n";
	fout << "\t	LOAD	 R2,  # FP - 2\n";
	fout << "\t	LOAD	R2, * R2\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	;OLD SCOPE\n\n";

	fout << "\t	JMP	destroyifd1\n";
	fout << "destroyif1e1:\n\n";

	fout << "\t	;NEW SCOPE\n";
	fout << "\t	LOAD	 R1, * # FP - 4\n";
	fout << "\t	LOAD	 R2,  # HP\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	;OLD SCOPE\n\n";

	fout << "destroyifd1:\n";
	fout << "\t	;end of if stmt 1\n\n";

	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	MOV	R11, R1\n";
	fout << "\t	ADD	SP, 4\n";
	fout << "\t	POP	FP\n";
	fout << "\t	RET\n";
	fout << "\t	ADD	SP, 2\n";
	fout << "\t	;OLD SCOPE\n\n";

	fout << "destroywhen0:\n";
	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	LOAD	 R2,  # FP - 2\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	LOAD	 R1, * # FP - 1\n";
	fout << "\t	LOAD	R1, * R1\n";
	fout << "\t	LOAD	 R2,  # FP - 1\n";
	fout << "\t	STORE	 R1, R2\n";
	fout << "\t	;OLD SCOPE\n\n";

	fout << "\t	JMP	destroywhiles0\n\n";

	fout << "destroywhiled0:\n";
	fout << "\t	LOAD	 R1, 0\n";
	fout << "\t	MOV	R11, R1\n";
	fout << "\t	ADD	SP, 2\n";
	fout << "\t	POP	FP\n";
	fout << "\t	RET\n";
	fout << "\t	ADD	SP, 2\n";
	fout << "\t	;OLD SCOPE\n\n";

	fout << "\t	LOAD	R11, 0\n";
	fout << "\t	POP	FP\n";
	fout << "\t	RET	\n";
	fout << "\t	;END FUNCTION create\n\n";

    fout << "\t;START FUNCTION destroy\n";
    fout << "destroy:\n";
    fout << "\tPUSH	FP\n";
    fout << "\tMOV	FP, SP\n";
    fout << "\tADD	FP, 1\n\n";

    fout << "\t;NEW SCOPE\n";
    fout << "\tLOAD	 R1, * # HP\n";
    fout << "\tLOAD	 R2,  # FP + 2\n";
    fout << "\tLOAD	R2, * R2\n";
    fout << "\tSTORE	 R1, R2\n";
    fout << "\tLOAD	 R1, * # FP + 2\n";
    fout << "\tLOAD	 R2,  # HP\n";
    fout << "\tSTORE	 R1, R2\n";
    fout << "\tLOAD	 R1, 1\n";
    fout << "\tMOV	R11, R1\n";
    fout << "\tPOP	FP\n";
    fout << "\tRET\n";
    fout << "\t;OLD SCOPE\n\n";

    fout << "\tLOAD	R11, 0\n";
    fout << "\tPOP	FP\n";
    fout << "\tRET	\n";
    fout << "\t;END FUNCTION destroy\n\n";

    fout << "\tHP:	.data 0\n";
    if(!codeGenVars(fout, indent + 1))
    {
        if(DEBUG) cout << getTab(indent) << "Returing from codeGenSetup\n";
        return false;    
    }
    fout << "main:\n";
    fout << "\t;void main(void)\n";
    fout << "\tLOAD\tR1, 1024\n";
    fout << "\tSTORE\tR1, R11\n";
    fout << "\tADD\tR11, 1\n";
    fout << "\tLOAD\tR1, 0\n";
    fout << "\tSTORE\tR1, R11\n";
    fout << "\tSTORE\tR11, # HP\n";
    fout << "\tMOV\tFP, SP\n";
    fout << "\tPUSH\tFP\n";
    fout << "\t;Start program content\n\n";
    scopeDecs.push(new int(0));
    functDecs.push(new int(0));
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenSetup\n";
    return true;
}

bool codeGenEnd(ofstream &fout, int indent)
{
    if(DEBUG) cout << getTab(indent) << "Inside codeGenEnd\n";
    int* decp = scopeDecs.top();
    int dec = *decp;
    scopeDecs.pop();
    IdentifierID* id;
    if(dec > 0)
        fout << "\tADD\tSP, " << dec << "\n";
    for(int i = scopeList.size(); i > scopeList.size() - dec; i--) 
    {
        string name = scopeList[i - 1];
        id = symbolTable.find(name);
        if(id == NULL)
        {
            cout << "Didn't recognize identifier: " << name << "\n";
            if(DEBUG) cout << getTab(indent) << "Returing from codeGenEnd\n";
            return false;
        }
        else
            id->ids->pop();
    }
    scopeList.resize(scopeList.size() - dec);
    //delete decp;
    fout << "\tPOP\tFP\n";
    fout << "\tHALT\n";
    if(DEBUG) cout << getTab(indent) << "Returing from codeGenEnd\n";
    return true;
}  

