enum NodeType {
    NODE_STMT,  //Statement
    NODE_EXPR,  //Expression
    NODE_STMTL,  //Statement List
    NODE_TYPE
};

enum NodeSubType {
    NODE_ERROR, // Syntax Error
    NODE_WRONG, // Node wasn't what the caller thought it should be
    STMT_IF, STMT_WHEN, STMT_UNLESS, STMT_WHILE, STMT_VAR, STMT_FUN, STMT_SWITCH, STMT_PRINT, STMT_ASSIGN, STMT_LIST,
    STMT_CASE, STMT_BLANK, STMT_RET, STMT_DEST, STMT_READ, //Statements
    EXPR_NUM, EXPR_ID, EXPR_OP, EXPR_CREATE //Expressions
};

enum IdentifierType
{
    ID_VAR, ID_CONST, ID_FUN
};

enum VarType
{
    VAR_CHAR, VAR_INT, VAR_STRING, VAR_FLOAT, VAR_ERROR, VAR_UN, VAR_PTR
};

enum DecType
{
    DEC_LOCAL, DEC_PARAM, DEC_GLOBAL, DEC_CONST
};

