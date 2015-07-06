// this is compiler.cpp

#include "everything.h"

const bool exit_on_single_error = true;
const int debug = 0;

struct symbol_table;
struct symbol_description;
static FILE * input, *outfile, *copy_file;
string input_buffer;
static int input_buffer_len, input_buffer_pos, current_line_number,
    current_symbol_start, current_symbol_end, current_symbol_start_line,
    sym, sym_val, loop_depth, locals_now, locals_max, freeregs, nextfreelabel,
    breaklabel, continuelabel;
string sym_str;
static symbol_table * all_symbols;
static symbol_description * sym_ident;
static int error_count=0;
static string function_name;
static vector <string> strings;
static vector <string> assemblumps;

const int S_IDENTIFIER=1, S_NUMBER=2, S_STRING=3, S_EXIT=4, S_END_OF_FILE=5, S_BLOCK=6,
          S_FUNCTION_CALL=7, S_ARRAY_ACCESS=8, S_DOT_SELECT=9, S_POINTER_FOLLOW=10,
          S_ARITH_EXPR=11, S_LOGICAL_EXPR=12, S_NOT_EXPR=13, S_RELATIONAL_EXPR=14,
          S_MINUS_EXPR=15, S_FUNC_DEF=16, S_PARAM_NAMES=17, S_PROGRAM=18, S_OUT=19,
          S_VAR_DECL=20, S_VAR_DECL_ITEM=21, S_ASSIGN=22, S_BREAK=23, S_CONTINUE=24,
          S_RETURN=25, S_IF=26, S_WHILE=27, S_FOR=28, S_ADDRESS_OF=29, S_BYTEMAKE=30,
          S_BYTESEL=31, S_ASSEMBLY=32;

static const int max_syntax_names=100;
static char * syntax_name[max_syntax_names];

void register_syntax_names()
{ for (int i=0; i<max_syntax_names; i+=1)
    syntax_name[i]="error";
  syntax_name[S_IDENTIFIER] =      "identifier";
  syntax_name[S_NUMBER] =          "number";
  syntax_name[S_STRING] =          "string";
  syntax_name[S_EXIT] =            "exit";
  syntax_name[S_END_OF_FILE] =     "end-of-file";
  syntax_name[S_BLOCK] =           "block";
  syntax_name[S_FUNCTION_CALL] =   "function-call";
  syntax_name[S_ARRAY_ACCESS] =    "array-access";
  syntax_name[S_DOT_SELECT] =      "dot-select";
  syntax_name[S_POINTER_FOLLOW] =  "follow-pointer";
  syntax_name[S_ADDRESS_OF] =      "address-of";
  syntax_name[S_ARITH_EXPR] =      "arith-expr";
  syntax_name[S_LOGICAL_EXPR] =    "logic-expr";
  syntax_name[S_NOT_EXPR] =        "not-expr";
  syntax_name[S_RELATIONAL_EXPR] = "relational-expr";
  syntax_name[S_MINUS_EXPR] =      "minus-expr";
  syntax_name[S_FUNC_DEF] =        "function-def";
  syntax_name[S_PARAM_NAMES] =     "param-names";
  syntax_name[S_PROGRAM] =         "program";
  syntax_name[S_VAR_DECL] =        "variable-decls";
  syntax_name[S_VAR_DECL_ITEM] =   "var-decl-item";
  syntax_name[S_ASSIGN] =          "assign";
  syntax_name[S_BREAK] =           "break";
  syntax_name[S_CONTINUE] =        "continue";
  syntax_name[S_RETURN] =          "return";
  syntax_name[S_OUT] =             "out";
  syntax_name[S_FOR] =             "for";
  syntax_name[S_IF] =              "if";
  syntax_name[S_WHILE] =           "while";
  syntax_name[S_ASSEMBLY] =        "inline-assembly";
  syntax_name[S_BYTEMAKE] =        "byte-make";
  syntax_name[S_BYTESEL] =         "byte-select"; }

const int _ERROR=0, _NUM_ERROR=1, _EOF=2, _IDENTIFIER=3, _NUMBER=4, _SEMICOLON=5,
          _OPERATOR=6, _OPEN_P=7, _CLOSE_P=8, _OPEN_B=9, _CLOSE_B=10, _OPEN_C=11, _CLOSE_C=12,
          _RESERVED=13, _COMMA=14, _STRING=15, _COLON=16, _ASSEMBLY=17;

const int _EQUAL=1, _NOT_EQUAL=2, _LESS=3, _NOT_LESS=4, _MORE=5, _NOT_MORE=6,
          _PLUS=7, _MINUS=8, _TIMES=9, _DIVIDE=10, _REMAINDER=12, _ARROW=13, _NOT=14,
          _AND=15, _OR=16, _DOT=17, _AT=18;

const int _IF=1, _ELSE=2, _WHILE=3, _MAIN=4, _FUNCTION=5, _LOCAL=6, _GLOBAL=7, _THEN=8,
          _DO=10, _BREAK=11, _CONTINUE=12, _RETURN=13, _OUTN=14, _OUTCH=15, _OUTS=16,
          _EXIT=17, _FOR=18, _TO=19, _DOWNTO=20, _BIT=21, _BYTE=22, _OF=23, _FROM=24;

struct { char * form; int code; int subcode; } preloads[] =
 { { "if", _RESERVED, _IF },
   { "else", _RESERVED, _ELSE },
   { "while", _RESERVED, _WHILE },
   { "do", _RESERVED, _DO },
   { "for", _RESERVED, _FOR },
   { "to", _RESERVED, _TO },
   { "downto", _RESERVED, _DOWNTO },
   { "then", _RESERVED, _THEN },
   { "break", _RESERVED, _BREAK },
   { "continue", _RESERVED, _CONTINUE },
   { "return", _RESERVED, _RETURN },
   { "exit", _RESERVED, _EXIT },
   { "outn", _RESERVED, _OUTN },
   { "outch", _RESERVED, _OUTCH },
   { "outs", _RESERVED, _OUTS },
   { "main", _RESERVED, _MAIN },
   { "bit", _RESERVED, _BIT },
   { "byte", _RESERVED, _BYTE },
   { "of", _RESERVED, _OF },
   { "from", _RESERVED, _FROM },
   { "function", _RESERVED, _FUNCTION },
   { "local", _RESERVED, _LOCAL },
   { "global", _RESERVED, _GLOBAL },
   { "true", _NUMBER, 1 },
   { "false", _NUMBER, 0 },
   { "null", _NUMBER, 0 },
   { "not", _OPERATOR, _NOT },
   { "and", _OPERATOR, _AND },
   { "or", _OPERATOR, _OR },
   { NULL, 0, 0 } };

struct start
{ string name;
  static int level;
  start(string n): name(n)
   { if (debug)
     { level+=1;
       printf("%*.*sEnter %s\n", level*2, level*2, "", name.c_str());
       fflush(stdout); } }
  ~start()
   { if (debug)
     { printf("%*.*sExit %s\n\n", level*2, level*2, "", name.c_str());
       fflush(stdout);
       level-=1; } } };

int start::level=0;

const char * operator_name(int subcode)
{ switch(subcode)
  { case _EQUAL:     return "EQUAL";
    case _NOT_EQUAL: return "NOT-EQUAL";
    case _LESS:      return "LESS";
    case _NOT_LESS:  return "NOT-LESS";
    case _MORE:      return "MORE";
    case _NOT_MORE:  return "NOT-MORE";
    case _PLUS:      return "PLUS";
    case _MINUS:     return "MINUS";
    case _TIMES:     return "TIMES";
    case _DIVIDE:    return "DIVIDE";
    case _REMAINDER: return "REMAINDER";
    case _ARROW:     return "ARROW";
    case _NOT:       return "NOT";
    case _AND:       return "AND";
    case _AT:        return "AT";
    case _OR:        return "OR";
    case _DOT:       return "DOT";
    default:         return "ERROR"; } }

const char * reserved_name(int subcode)
{ switch(subcode)
  { case _IF:         return "IF";
    case _ELSE:       return "ELSE";
    case _WHILE:      return "WHILE";
    case _FOR:        return "FOR";
    case _TO:         return "TO";
    case _DOWNTO:     return "DOWNTO";
    case _MAIN:       return "MAIN";
    case _FUNCTION:   return "FUNCTION";
    case _LOCAL:      return "LOCAL";
    case _GLOBAL:     return "GLOBAL";
    case _DO:         return "DO";
    case _THEN:       return "THEN";
    case _BREAK:      return "BREAK";
    case _CONTINUE:   return "CONTINUE";
    case _RETURN:     return "RETURN";
    case _EXIT:       return "EXIT";
    case _OUTN:       return "OUTN";
    case _OUTCH:      return "OUTCH";
    case _OUTS:       return "OUTS";
    case _BIT:        return "BIT";
    case _BYTE:       return "BYTE";
    case _OF:         return "OF";
    case _FROM:       return "FROM";
    default:          return "ERROR"; } }

const char * symbol_name(int code, int subcode=0)
{ switch (code)
  { case _ERROR:      return "INCORRECT-SYMBOL";
    case _NUM_ERROR:  return "INCORRECT-NUMERIC-CONSTANT";
    case _EOF:        return "END-OF-FILE";
    case _IDENTIFIER: return "IDENTIFIER";
    case _NUMBER:     return "NUMERIC-CONSTANT";
    case _SEMICOLON:  return "SEMICOLON";
    case _COLON:      return "COLON";
    case _OPERATOR:   return operator_name(subcode);
    case _OPEN_P:     return "OPEN-ROUND-BRACKET";
    case _CLOSE_P:    return "CLOSE-ROUND-BRACKET";
    case _OPEN_B:     return "OPEN-SQUARE-BRACKET";
    case _CLOSE_B:    return "CLOSE-SQUARE-BRACKET";
    case _OPEN_C:     return "OPEN-CURLY-BRACKET";
    case _CLOSE_C:    return "CLOSE-CURLY-BRACKET";
    case _RESERVED:   return reserved_name(subcode);
    case _COMMA:      return "COMMA";
    case _STRING:     return "STRING";
    case _ASSEMBLY:   return "ASSEMBLY-LUMP";
    default:          return "ERROR"; } }

struct declaration
{ int kind;     // 'L', 'P', 'G', 'F'
  int detail;   // stack frame index or number of params
  int size;
  declaration * previous;
  declaration(int k, int d, int s, declaration * p): kind(k), detail(d), size(s), previous(p) { } };

struct symbol_description
{ string name;
  int code;
  int subcode;
  declaration * info;
  symbol_description * next;
  symbol_description(string n, int c1, int c2=0, symbol_description * x=NULL):
    name(n), code(c1), subcode(c2), next(x), info(NULL) { }
  void push_declaration(int k, int d, int s = 0);
  void pop_declaration(); };

symbol_description * error_symbol = new symbol_description("ERROR", _STRING);
symbol_description * no_symbol = new symbol_description("", _STRING);

struct syntax_element
{ int code, info, line_number;
  vector <syntax_element *> part;
  symbol_description * sym;
  symbol_description * get_symbol() { return sym; }
  void add(syntax_element * p) { part.push_back(p); }
  int sub_parts() { return part.size(); }
  syntax_element(int c, int i=0):
      sym(no_symbol), code(c), info(i) { line_number=current_symbol_start_line; }
  syntax_element(int c, symbol_description * s, int i=0):
      code(c), sym(s), info(i) { line_number=current_symbol_start_line; } };

void declare_variables(syntax_element * e, char kind);
void print_syntax_tree(syntax_element * e, int maxd=100000, int d=0);
void print_syntax_linear(FILE * f, syntax_element * e, int maxd=100000, int d=0);

void report_syntax_error(const char * format, ...)
{ va_list args;
  va_start(args, format);
  error_count+=1;
  printf("___________________________________________\n");
  printf("Line %d, chars %d-%d ** ERROR ** ", current_symbol_start_line, current_symbol_start+1, current_symbol_end+1);
  vprintf(format, args);
  int startfrom=0, endat=input_buffer_len-2;
  if (endat>99)
  { startfrom=current_symbol_start-30;
    if (startfrom<0) startfrom=0;
    endat=startfrom+98;
    if (endat>input_buffer_len-2)
      endat=input_buffer_len-2; }
  printf("\n");
  printf("  | | ");
  printstr(input_buffer.substr(startfrom, endat-startfrom+1).c_str());
  printf("\n  | | ");
  for (int i=startfrom; i<current_symbol_start; i+=1)
    putchar(' ');
  for (int i=current_symbol_start; i<=current_symbol_end; i+=1)
    putchar('*');
  printf("\n"); }

void report_semantic_error(syntax_element * e, const char * format, ...)
{ va_list args;
  va_start(args, format);
  error_count+=1;
  printf("___________________________________________\n");
  printf("Around line %d ** ERROR ** ", e->line_number);
  vprintf(format, args);
  printf("\n  | | ");
  print_syntax_linear(stdout, e, 4);
  printf("\n"); }

int char_to_int(char c, int base)
{ int val=-1;         
  if (c>='0' && c<='9')  
    val=c-'0';
  else if (c>='A' && c<='F') 
    val=c-'A'+10;             
  else if (c>='a' && c<='f')  
    val=c-'a'+10;
  if (val>=base)
    return -1;                
  return val; }          

void printchar(FILE * f, char c)
{ if (c=='\n') fprintf(f, "\\n");
  else if (c=='\a') fprintf(f, "\\a");
  else if (c=='\b') fprintf(f, "\\b");
  else if (c=='\t') fprintf(f, "\\t");
  else if (c=='\r') fprintf(f, "\\r");
  else if (c=='\'') fprintf(f, "\\\'");
  else if (c=='\"') fprintf(f, "\\\"");
  else if (c=='\\') fprintf(f, "\\\\");
  else if (c<' ' || c>=127) fprintf(f, "\\[%d]", c & 0xFF);
  else fprintf(f, "%c", c); }

void printchar(char c)
{ printchar(stdout, c); }

void printstr(const char * s)
{ for (int i=0; s[i]!=0; i+=1)
    printchar(stdout, s[i]); }

void printstr(FILE * f, const char * s)
{ for (int i=0; s[i]!=0; i+=1)
    printchar(f, s[i]); }

void printstr(string s)
{ printstr(stdout, s.c_str()); }

void printstr(FILE * f, string s)
{ printstr(f, s.c_str()); }

void symbol_description::push_declaration(int k, int d, int s)
{ info = new declaration(k, d, s, info); }

void symbol_description::pop_declaration()
{ if (info==NULL) return;
  declaration * old = info;
  info=info->previous;
  delete old; }

struct symbol_table
{ symbol_description * * table;
  int size;

  symbol_table(int sz)
  { size=sz;
    table = new symbol_description * [sz];
    for (int i=0; i<sz; i+=1)
      table[i]=NULL; }

  ~symbol_table()
  { for (int i=0; i<size; i+=1)
    { symbol_description * p = table[i];
      while (p!=NULL)
      { symbol_description * n = p->next;
        delete p;
        p=n; } }
    delete[] table; }

  int hash(string s)
  { int h = 87162381, len=s.length();
    for (int i=0; i<len; i+=1)
      h=h*69+toupper(s[i]);
    if (h<0) h=-h;
    return h%size; }

  symbol_description * add(string n, int c1, int c2)
  { int h = hash(n);
    table[h] = new symbol_description(n, c1, c2, table[h]);
    return table[h]; }

  symbol_description * look_up(string n)
  { int h = hash(n);
    symbol_description * ptr = table[h];
    while (ptr!=NULL)
    { if (strcasecmp(ptr->name.c_str(), n.c_str())==0)
        return ptr;
      ptr=ptr->next; }
    return NULL; } };

const int temp_buffer_len=1000;
static char temp_buffer[temp_buffer_len+2];

void refill_input_buffer()
{ input_buffer_len=0;
  input_buffer_pos=0;
  current_line_number+=1;
  current_symbol_start=0;
  input_buffer="";
  while (1)
  { char * x = fgets(temp_buffer, temp_buffer_len, input);
    if (x==NULL)
    { if (input_buffer_len==0)
      { strcpy(temp_buffer, "(end-of-file)");
        input_buffer_len=-strlen(temp_buffer);
        input_buffer_pos=input_buffer_len; }
      return; }
    if (copy_file!=NULL)
      fputs(temp_buffer, copy_file);
    int temp_len=strlen(temp_buffer);
    if (temp_buffer[temp_len-1]=='\n')
    { input_buffer+=temp_buffer;
      input_buffer_len+=temp_len;
      return; }
    input_buffer+=temp_buffer;
    input_buffer_len+=temp_len; } }

void symbol_starts_here()
{ if (input_buffer_pos>0)
  { current_symbol_start=input_buffer_pos-1;
    current_symbol_end=input_buffer_pos-1; }
  else
  { current_symbol_start=0;
    current_symbol_end=0; }
  current_symbol_start_line=current_line_number; }

void symbol_ends_here()
{ if (input_buffer_pos>0)
    current_symbol_end=input_buffer_pos-1;
  else
    current_symbol_end=0; }

const int INCLUDING_NEWLINES = 1;

char nextchar(int include_newlines = 0)
{ if (input_buffer_len<0)
    return 0;
  if (input_buffer_pos>=input_buffer_len)
  { refill_input_buffer();
    if (include_newlines)
      return '\n'; }
  char c = input_buffer[input_buffer_pos];
  input_buffer_pos+=1;
  return c; }

char sneakchar(int ahead)
{ ahead-=1;
  if (input_buffer_len<0)
    return 0;
  if (input_buffer_pos+ahead>=input_buffer_len)
    return ' ';
  return input_buffer[input_buffer_pos+ahead]; }

char go_back_one_char()
{ if (input_buffer_pos>0)
    input_buffer_pos-=1;
  if (input_buffer_pos>0)
    return input_buffer[input_buffer_pos-1];
  else
    return ' '; }

static string ignore="";

bool read_number_into(int & value, string & repr = ignore)
{ int base=10, error=0;
  value=0;
  char c = nextchar();
  repr=c;
  if (c=='0')
  { c=nextchar();
    if (c=='x' || c=='h')
    { base=16;
      c=nextchar();
      repr=repr+'0'+c; }
    else if (c=='b')
    { base=2;
      c=nextchar();
      repr=repr+'0'+c; }
    else if (c=='o')
    { base=8;
      c=nextchar();
      repr=repr+'0'+c; }
    else if (c=='d')
    { base=10;
      c=nextchar();
      repr=repr+'0'+c; } }
  while (c>='0' && c<='9' || c>='a' && c<='z' || c>='A' && c<='Z')
  { repr+=c;
    int digit=char_to_int(c, base);
    if (digit<0) error=1;
    value = value*base+digit;
    c=nextchar(); }
  go_back_one_char();
  if (error)
    return false;
  return true; }

void getsym()
{ while (1)
  { sym_str = "";
    sym_val = 0;
    sym_ident = NULL;
    sym = _ERROR;
    char c = nextchar();
    if (c==0)
    { symbol_starts_here();
      sym = _EOF;
      break; }
    if (c<=' ')
      continue;
    else if (c=='/')
    { c=sneakchar(+1);
      if (c=='/')
      { while (c!='\n' && c!=0)
          c=nextchar();
        continue; }
      else if (c=='*')
      { char last_c = ' ';
        c=nextchar();
        while (1)
        { c=nextchar();
          if (last_c=='*' && c=='/')
            break;
          last_c=c; }
        continue; }
      else
        c='/'; }
    symbol_starts_here();
    if (c>='a' && c<='z' || c>='A' && c<='Z')
    { while (c>='a' && c<='z' || c>='A' && c<='Z' || c>='0' && c<='9' || c=='_')
      { sym_str+=tolower(c);
        c=nextchar(); }
      go_back_one_char();
      sym_ident = all_symbols->look_up(sym_str);
      if (sym_ident==NULL)
        sym_ident = all_symbols->add(sym_str, _IDENTIFIER, 0);
      sym = sym_ident->code;
      sym_val = sym_ident->subcode;
      break; }
    else if (c>='0' && c<='9')
    { go_back_one_char();
      bool ok = read_number_into(sym_val, sym_str);
      if (!ok)
      { symbol_ends_here();
        report_syntax_error("\"%s\" looks like a number, but isn't", sym_str.c_str());
        continue; }
      sym = _NUMBER;
      break; }
    else if (c=='\'' || c=='\"')
    { sym_str="";
      sym_val=0;
      int error=0;
      char quote_type=c;
      while (1)
      { c=nextchar(INCLUDING_NEWLINES);
        if (c==quote_type || c=='\n' || c==0) break;
        if (c=='\\')
        { c=nextchar(INCLUDING_NEWLINES);
          if (c=='\n' || c==0) break;
          else if (c=='n') c='\n';
          else if (c=='r') c='\r';
          else if (c=='t') c='\t';
          else if (c=='a') c='\a';
          else if (c=='b') c='\b';
          else if (c=='0') c=0;
          else if (c=='[')
          { int number=0;
            bool ok = read_number_into(number);
            if (!ok) error=1;
            c=nextchar(INCLUDING_NEWLINES);
            if (c==']')
              c=number;
            else
            { error=1;
              go_back_one_char(); } } }
        if (quote_type=='\'')
          sym_val = (sym_val << 8) | (c & 0xFF);
        else
          sym_str+=c; }
      if (c!=quote_type)
      { symbol_ends_here();
        report_syntax_error("String (or character constant) did not end before the line did");
        continue; }
      else if (error)
      { symbol_ends_here();
        report_syntax_error("Error in escape sequence \\[n]: n is not a number");
        continue; }
      if (quote_type=='\'')
        sym = _NUMBER;
      else
      { sym = _STRING;
        sym_val = strings.size();
        strings.push_back(sym_str); }
      break; }
    else if (c=='(')
    { sym = _OPEN_P;
      break; }
    else if (c==')')
    { sym = _CLOSE_P;
      break; }
    else if (c=='[')
    { c=nextchar();
      if (c=='[')
      { sym_str="";
        int closes_seen=0;
        while (1)
        { c=nextchar(INCLUDING_NEWLINES);
          if (c==0)
          { report_syntax_error("End of file reached before [[ matched with ]]");
            break; }
          else if (c==']')
          { if (closes_seen==1)
              break;
            else
              closes_seen=1; }
          else
          { if (closes_seen==1)
              sym_str+=']';
            closes_seen=0;
            sym_str+=c; } }
        sym = _ASSEMBLY;
        sym_val = assemblumps.size();
        assemblumps.push_back(sym_str);
        sym_str="";
        break; }
      go_back_one_char();
      sym = _OPEN_B;
      break; }
    else if (c==']')
    { sym = _CLOSE_B;
      break; }
    else if (c=='{')
    { sym = _OPEN_C;
      break; }
    else if (c=='}')
    { sym = _CLOSE_C;
      break; }
    else if (c==',')
    { sym = _COMMA;
      break; }
    else if (c==':')
    { sym = _COLON;
      break; }
    else if (c=='@')
    { sym = _OPERATOR;
      sym_val=_AT;
      break; }
    else if (c=='.')
    { sym = _OPERATOR;
      sym_val = _DOT;
      break; }
    else if (c=='+')
    { sym = _OPERATOR;
      sym_val = _PLUS;
      break; }
    else if (c=='/')
    { sym = _OPERATOR;
      sym_val = _DIVIDE;
      break; }
    else if (c=='*')
    { sym = _OPERATOR;
      sym_val = _TIMES;
      break; }
    else if (c=='%')
    { sym = _OPERATOR;
      sym_val = _REMAINDER;
      break; }
    else if (c==';')
    { sym = _SEMICOLON;
      break; }
    else if (c=='-')
    { sym = _OPERATOR;
      c=nextchar();
      if (c=='>')
        sym_val = _ARROW;
      else
      { go_back_one_char();
        sym_val = _MINUS; }
      break; }
    else if (c=='=')
    { sym = _OPERATOR;
      sym_val = _EQUAL;
      c=nextchar();
      if (c!='=')
        go_back_one_char();
      break; }
    else if (c=='&')
    { sym = _OPERATOR;
      sym_val = _AND;
      c=nextchar();
      if (c!='&')
        go_back_one_char();
      break; }
    else if (c=='|')
    { sym = _OPERATOR;
      sym_val = _OR;
      c=nextchar();
      if (c!='|')
        go_back_one_char();
      break; }
    else if (c=='<')
    { sym = _OPERATOR;
      c=nextchar();
      if (c=='=')
        sym_val = _NOT_MORE;
      else if (c=='>')
        sym_val = _NOT_EQUAL;
      else
      { go_back_one_char();
        sym_val = _LESS; }
      break; }
    else if (c=='>')
    { sym = _OPERATOR;
      c=nextchar();
      if (c=='=')
        sym_val = _NOT_LESS;
      else
      { go_back_one_char();
        sym_val = _MORE; }
      break; }
    else if (c=='!')
    { sym = _OPERATOR;
      c=nextchar();
      if (c=='=')
        sym_val = _NOT_EQUAL;
      else
      { go_back_one_char();
        sym_val = _NOT; }
      break; }
    else
    { sym = _ERROR;
      sym_str="";
      while (c>' ')
      { sym_str+=c;
        c=nextchar(); }
      go_back_one_char();
      symbol_ends_here();
      report_syntax_error("Illegal symbol in the program: \"%s\"", sym_str.c_str());
      continue; } }
  symbol_ends_here();
  if (debug)
    printf("%*.*s  '%s'\n", start::level*2, start::level*2, "",
           input_buffer.substr(current_symbol_start, current_symbol_end-current_symbol_start+1).c_str()); }

void print_current_symbol()
{ char temp[50], desc[50];
  sprintf(temp, "line %d chars %d-%d:", current_symbol_start_line, current_symbol_start+1, current_symbol_end+1);
  if (sym==_OPERATOR)
    sprintf(desc, "OPERATOR(%s)", operator_name(sym_val));
  else if (sym==_RESERVED)
    sprintf(desc, "RESERVED(%s)", reserved_name(sym_val));
  else
    sprintf(desc, "%s", symbol_name(sym, sym_val));
  printf("%-21s%37s ", temp, desc);
  if (sym==_STRING)
  { printf("\"");
    printstr(sym_str);
    printf("\""); }
  else if (sym_str=="")
  { printf("\"");
    for (int i=current_symbol_start; i<=current_symbol_end; i+=1)
      putchar(input_buffer[i]);
    printf("\""); }
  else
  { printf("\"");
    printstr(sym_str);
    printf("\""); }
  if (sym==_NUMBER)
    printf(" = %d", sym_val);
  printf("\n"); }

syntax_element * main_def = NULL;
syntax_element * parse_expression();
syntax_element * parse_statement();

syntax_element * parse_basic_expression()
{ start _s_("basic-expression");
  if (sym==_OPEN_P)
  { getsym();
    syntax_element * result = parse_expression();
    if (sym!=_CLOSE_P)
      report_syntax_error("Something wrong in expression (expected to see ')')");
    getsym();
    return result; }
  else if (sym==_IDENTIFIER)
  { syntax_element * result = new syntax_element(S_IDENTIFIER, sym_ident);
    getsym();
    return result; }
  else if (sym==_NUMBER)
  { int val = sym_val;
    getsym();
    return new syntax_element(S_NUMBER, val); }
  else if (sym==_STRING)
  { syntax_element * result = new syntax_element(S_STRING, sym_val);
    getsym();
    return result; }
  else
  { report_syntax_error("This is not a valid operand for an expression");
    return NULL; } }

syntax_element * parse_very_high_prio_expression()
{ start _s_("very-high-prio-expression");
  syntax_element * result = parse_basic_expression();
  while (1)
  { if (sym==_OPEN_P)
    { getsym();
      syntax_element * replacement = new syntax_element(S_FUNCTION_CALL);
      replacement->add(result);
      while (sym!=_CLOSE_P)
      { syntax_element * param = parse_expression();
        replacement->add(param);
        if (sym==_COMMA)
          getsym();
        else
          break; }
      if (sym==_CLOSE_P)
        getsym();
      else
      { report_syntax_error("Something wrong with the parameter values");
        return NULL; }
      result=replacement; }
    else if (sym==_OPEN_B)
    { getsym();
      syntax_element * index = parse_expression();
      syntax_element * replacement = new syntax_element(S_ARRAY_ACCESS);
      replacement->add(result);
      replacement->add(index);
      result=replacement;
      if (sym==_CLOSE_B)
        getsym();
      else
      { report_syntax_error("Something wrong with the index value");
        return NULL; } }
    else if (sym==_OPERATOR && sym_val==_DOT)
    { getsym();
      if (sym!=_IDENTIFIER)
      { report_syntax_error("Only a name may follow a dot selector");
        return NULL; }
      syntax_element * replacement = new syntax_element(S_DOT_SELECT, sym_ident);
      replacement->add(result);
      result=replacement;
      getsym(); }
    else if (sym==_OPERATOR && sym_val==_ARROW)
    { syntax_element * replacement = new syntax_element(S_POINTER_FOLLOW);
      replacement->add(result);
      result=replacement;
      getsym();
      if (sym==_IDENTIFIER)
      { syntax_element * replacement = new syntax_element(S_DOT_SELECT, sym_ident);
        replacement->add(result);
        result=replacement;
        getsym(); } }
    else
      break; }
  return result; }

syntax_element * parse_pointery()
{ start _s_("pointery");
  if (sym==_OPERATOR && sym_val==_TIMES)
  { syntax_element * result = new syntax_element(S_POINTER_FOLLOW);
    getsym();
    result->add(parse_pointery());
    return result; }
  if (sym==_OPERATOR && (sym_val==_AND || sym_val==_AT))
  { syntax_element * result = new syntax_element(S_ADDRESS_OF);
    getsym();
    result->add(parse_very_high_prio_expression());
    return result; }
  return parse_very_high_prio_expression(); }
  
syntax_element * parse_lvalue()
{ start _s_("lvalue");
  if (sym==_OPERATOR && (sym_val==_AT || sym_val==_AND))
  { report_syntax_error("Addresses can't be used here");
    return NULL; }
  return parse_pointery(); }

syntax_element * parse_unary_expression()
{ start _s_("unary-expression");
  int minusses=0;
  while (sym==_OPERATOR && (sym_val==_PLUS || sym_val==_MINUS))
  { if (sym_val==_MINUS)
      minusses+=1;
    getsym(); }
  syntax_element * result = parse_pointery();
  if (minusses % 2 == 0)
    return result;
  syntax_element * replacement = new syntax_element(S_MINUS_EXPR, _MINUS);
  replacement->add(result);
  return replacement; }

syntax_element * parse_mul_expression()
{ start _s_("mul-expression");
  syntax_element * result = parse_unary_expression();
  while (sym==_OPERATOR && (sym_val==_TIMES || sym_val==_DIVIDE || sym_val==_REMAINDER))
  { syntax_element * replacement = new syntax_element(S_ARITH_EXPR, sym_val);
    replacement->add(result);
    getsym();
    replacement->add(parse_unary_expression());
    result=replacement; }
  return result; }

syntax_element * parse_add_expression()
{ start _s_("add-expression");
  syntax_element * result = parse_mul_expression();
  while (sym==_OPERATOR && (sym_val==_PLUS || sym_val==_MINUS))
  { syntax_element * replacement = new syntax_element(S_ARITH_EXPR, sym_val);
    replacement->add(result);
    getsym();
    replacement->add(parse_mul_expression());
    result=replacement; }
  return result; }

syntax_element * parse_byte_expression()
{ if (sym!=_RESERVED || (sym_val!=_BIT && sym_val!=_BYTE))
    return parse_add_expression();
  start _s_("byte-expression");
  syntax_element * result = new syntax_element(S_BYTEMAKE);
  result->info=sym_val;
  getsym();
  result->add(parse_add_expression());
  if (sym==_COLON)
  { getsym();
    result->add(parse_add_expression()); }
  else
    result->add(new syntax_element(S_NUMBER, 0));
  return result; }

syntax_element * parse_of_expression()
{ start _s_("of-expression");
  syntax_element * result = parse_byte_expression();
  if (sym==_RESERVED && (sym_val==_OF || sym_val==_FROM))
  { syntax_element * replacement = new syntax_element(S_BYTESEL, sym_val);
    replacement->add(result);
    getsym();
    replacement->add(parse_byte_expression());
    result=replacement; }
  return result; }

syntax_element * parse_relational_expression()
{ start _s_("relational-expression");
  syntax_element * result = parse_of_expression();
  if (sym==_OPERATOR && (sym_val==_EQUAL || sym_val==_LESS || sym_val==_MORE ||
                         sym_val==_NOT_EQUAL || sym_val==_NOT_LESS || sym_val==_NOT_MORE))
  { syntax_element * replacement = new syntax_element(S_RELATIONAL_EXPR, sym_val);
    replacement->add(result);
    getsym();
    replacement->add(parse_of_expression());
    result=replacement; }
  return result; }

syntax_element * parse_not_expression()
{ start _s_("not-expression");
  int nots=0;
  while (sym==_OPERATOR && sym_val==_NOT)
  { nots+=1;
    getsym(); }
  syntax_element * result = parse_relational_expression();
  if (nots % 2 == 0)
    return result;
  syntax_element * replacement = new syntax_element(S_NOT_EXPR, _NOT);
  replacement->add(result);
  return replacement; }

syntax_element * parse_and_expression()
{ start _s_("and-expression");
  syntax_element * result = parse_not_expression();
  if (sym==_OPERATOR && sym_val==_AND)
  { syntax_element * replacement = new syntax_element(S_LOGICAL_EXPR, sym_val);
    replacement->add(result);
    getsym();
    replacement->add(parse_not_expression());
    result=replacement; }
  return result; }

syntax_element * parse_or_expression()
{ start _s_("or-expression");
  syntax_element * result = parse_and_expression();
  if (sym==_OPERATOR && sym_val==_OR)
  { syntax_element * replacement = new syntax_element(S_LOGICAL_EXPR, sym_val);
    replacement->add(result);
    getsym();
    replacement->add(parse_or_expression());
    result=replacement; }
  return result; }

syntax_element * parse_expression()
{ start _s_("expression");
  return parse_or_expression(); }

syntax_element * parse_block()
{ start _s_("block");
  getsym();
  syntax_element * result = new syntax_element(S_BLOCK);
  while (1)
  { if (sym==_CLOSE_C)
      break;
    result->add(parse_statement());
    if (sym!=_SEMICOLON)
      break;
    getsym(); }
  if (sym==_CLOSE_C)
    sym=_SEMICOLON;
  else
  { report_syntax_error("Probably a semicolon missing. Something is wrong anyway");
    return NULL; }
  return result; }

syntax_element * parse_var_decl_item()
{ start _s_("var_decl_item");
  if (sym!=_IDENTIFIER)
  { report_syntax_error("(inside variable declaration list) This is not a variable name");
    return NULL; }
  symbol_description * sd = sym_ident;
  getsym();
  if (sym==_OPEN_B)
  { getsym();
    if (sym!=_NUMBER)
    { report_syntax_error("Dimension of an array variable must be a numeric constant");
      return NULL; }
    int v=sym_val;
    getsym();
    if (sym!=_CLOSE_B)
    { report_syntax_error("Dimension of an array variable must be a simple numeric constant");
      return NULL; }
    getsym();
    return new syntax_element(S_VAR_DECL_ITEM, sd, v); }
  else if (sym==_OPERATOR && sym_val==_EQUAL)
  { syntax_element * result = new syntax_element(S_VAR_DECL_ITEM, sd, 0);
    getsym();
    result->add(parse_expression());
    return result; }
  else
    return new syntax_element(S_VAR_DECL_ITEM, sd, 0); }

syntax_element * parse_var_decl()
{ start _s_("var-decl");
  syntax_element * result = new syntax_element(S_VAR_DECL);
  result->info = sym_val;
  getsym();
  while (1)
  { result->add(parse_var_decl_item());
    if (sym!=_COMMA)
      break;
    getsym(); }
  return result; }

syntax_element * parse_if()
{ start _S_("if");
  syntax_element * result = new syntax_element(S_IF);
  getsym();
  result->add(parse_expression());
  if (sym!=_RESERVED || sym_val!=_THEN)
  { report_syntax_error("\"then\" is required between a condition and its consequence");
    return NULL; }
  getsym();
  if (sym==_RESERVED && sym_val==_IF)
  { report_syntax_error("\"if\" not allowed directly after \"then\", use { } to disambiguate");
    return NULL; }
  result->add(parse_statement());
  if (sym==_RESERVED && sym_val==_ELSE)
  { getsym();
    result->add(parse_statement()); }
  return result; }

syntax_element * parse_while()
{ start _S_("while");
  syntax_element * result = new syntax_element(S_WHILE);
  getsym();
  result->add(parse_expression());
  if (sym!=_RESERVED || sym_val!=_DO)
  { report_syntax_error("\"do\" is required between the condition and the loop body");
    return NULL; }
  getsym();
  loop_depth+=1;
  result->add(parse_statement());
  loop_depth-=1;
  return result; }

syntax_element * parse_for()
{ start _S_("for");
  syntax_element * result = new syntax_element(S_FOR);
  getsym();
  if (sym!=_IDENTIFIER)
  { report_syntax_error("No variable name after \"for\"");
    return NULL; }
  result->sym=sym_ident;
  getsym();
  if (sym!=_OPERATOR || sym_val!=_EQUAL)
  { report_syntax_error("Equal sign must come directly after a simple identifier");
    return NULL; }
  getsym();
  result->add(parse_expression());
  if (sym==_RESERVED && sym_val==_TO)
    result->info=+1;
  else if (sym==_RESERVED && sym_val==_DOWNTO)
    result->info=-1;
  else
  { report_syntax_error("Either \"to\" or \"downto\" is required in a for-loop");
    return NULL; }
  getsym();
  result->add(parse_expression());
  if (sym!=_RESERVED || sym_val!=_DO)
  { report_syntax_error("\"do\" is required between the condition and the loop body");
    return NULL; }
  getsym();
  loop_depth+=1;
  result->add(parse_statement());
  loop_depth-=1;
  return result; }

syntax_element * parse_assembly_lump()
{ syntax_element * result = new syntax_element(S_ASSEMBLY, sym_val);
  getsym();
  return result; }

syntax_element * parse_assignment_or_fncall()
{ start _s_("assignment-or-fncall");
  syntax_element * part1 = parse_lvalue();
  if (sym!=_OPERATOR || sym_val!=_EQUAL)
    return part1;
  syntax_element * result = new syntax_element(S_ASSIGN);
  result->add(part1);
  getsym();
  result->add(parse_expression());
  return result; }

syntax_element * parse_break()
{ getsym();
  if (loop_depth==0)
  { report_syntax_error("\"break\" appears outside of any loops");
    return NULL; }
  return new syntax_element(S_BREAK); }

syntax_element * parse_continue()
{ getsym();
  if (loop_depth==0)
  { report_syntax_error("\"continue\" appears outside of any loops");
    return NULL; }
  return new syntax_element(S_CONTINUE); }

syntax_element * parse_output()
{ syntax_element * result = new syntax_element(S_OUT);
  if (sym_val==_OUTN)
    result->info='N';
  else if (sym_val==_OUTCH)
    result->info='C';
  else if (sym_val==_OUTS)
    result->info='S';
  getsym(); 
  result->add(parse_expression());
  return result; }

bool expression_can_begin()
{ if (sym==_NUMBER || sym==_STRING || sym==_OPEN_P || sym==_OPERATOR)
    return true;
  return false; }

syntax_element * parse_return()
{ syntax_element * result = new syntax_element(S_RETURN);
  getsym();
  if (expression_can_begin())
    result->add(parse_expression());
  return result; }

syntax_element * parse_exit()
{ syntax_element * result = new syntax_element(S_EXIT);
  getsym();
  if (expression_can_begin())
    result->add(parse_expression());
  return result; }

syntax_element * parse_statement()
{ start _s_("statement");
  while (sym==_SEMICOLON)
    getsym();
  if (sym==_OPEN_C)
    return parse_block();
  else if (sym==_RESERVED && sym_val==_LOCAL)
    return parse_var_decl();
  else if (sym==_IDENTIFIER || sym==_OPEN_P || sym==_OPERATOR && sym_val==_TIMES)
    return parse_assignment_or_fncall();
  else if (sym==_RESERVED && sym_val==_IF)
    return parse_if();
  else if (sym==_RESERVED && sym_val==_WHILE)
    return parse_while();
  else if (sym==_RESERVED && sym_val==_FOR)
    return parse_for();
  else if (sym==_RESERVED && sym_val==_BREAK)
    return parse_break();
  else if (sym==_RESERVED && sym_val==_CONTINUE)
    return parse_continue();
  else if (sym==_RESERVED && sym_val==_RETURN)
    return parse_return();
  else if (sym==_RESERVED && sym_val==_EXIT)
    return parse_exit();
  else if (sym==_ASSEMBLY)
    return parse_assembly_lump();
  else if (sym==_RESERVED && sym_val==_OUTN)
    return parse_output();
  else if (sym==_RESERVED && sym_val==_OUTCH)
    return parse_output();
  else if (sym==_RESERVED && sym_val==_OUTS)
    return parse_output();
  else
  { report_syntax_error("This is not a good beginning for a statement");
    print_current_symbol();
    return NULL; } }

syntax_element * parse_param_names()
{ start _s_("param-names");
  syntax_element * result = new syntax_element(S_PARAM_NAMES);
  getsym();
  while (1)
  { if (sym!=_IDENTIFIER)
      break;
    result->add(new syntax_element(S_IDENTIFIER, sym_ident));
    getsym();
    if (sym!=_COMMA)
      break;
    getsym(); }
  if (sym!=_CLOSE_P)
  { report_syntax_error("Something is wrong with the parameter list");
    return NULL; }
  getsym();
  return result; }

syntax_element * parse_main()
{ if (main_def!=NULL)
    report_syntax_error("Multiple definitions of main");
  main_def = new syntax_element(S_FUNC_DEF, sym_ident);
  main_def->add(new syntax_element(S_PARAM_NAMES));
  getsym();
  if (sym!=_OPEN_C)
  { report_syntax_error("No \"{\" after \"main\"");
    return NULL; }
  main_def->add(parse_block());
  main_def->sym->push_declaration('F', main_def->part[0]->sub_parts(), 0);
  return main_def; }

syntax_element * parse_function_def()
{ start _s_("function-def");
  getsym();
  if (sym!=_IDENTIFIER)
  { report_syntax_error("Function name missing");
    return NULL; }
  syntax_element * result = new syntax_element(S_FUNC_DEF, sym_ident);
  getsym();
  if (sym!=_OPEN_P)
  { report_syntax_error("Parameter list missing");
    return NULL; }
  result->add(parse_param_names());
  if (sym!=_OPEN_C)
  { report_syntax_error("No \"{\" after function heading");
    return NULL; }
  result->add(parse_block());
  result->sym->push_declaration('F', result->part[0]->sub_parts(), 0);
  return result; }

syntax_element * parse_top_level()
{ start _s_("top-level");
  while (sym==_SEMICOLON)
    getsym();
  if (sym==_EOF)
    return new syntax_element(S_END_OF_FILE);
  if (sym==_RESERVED && sym_val==_MAIN)
    return parse_main();
  if (sym==_RESERVED && sym_val==_FUNCTION)
    return parse_function_def();
  if (sym==_RESERVED && sym_val==_GLOBAL)
  { syntax_element * result = parse_var_decl();
    declare_variables(result, 'G');
    return result; }
  report_syntax_error("Invalid top-level declaration");
  return NULL; }

syntax_element * parse_all()
{ main_def=NULL;
  syntax_element * result = new syntax_element(S_PROGRAM);
  result->add(NULL);
  while (1)
  { syntax_element * e = parse_top_level();
    if (e->code==S_END_OF_FILE) break;
    if (e==main_def)
      result->part[0]=e;
    else
      result->add(e); }
  return result; }

void print_head(FILE * f, syntax_element * e)
{ int kind = e->code, num=e->info;
  if (kind<0 || kind>=max_syntax_names)
  { fprintf(f, "error");
    return; }
  if (kind==S_OUT)
  { if (num=='C') fprintf(f, "outch");
    else if (num=='N') fprintf(f, "outn");
    else if (num=='S') fprintf(f, "outs");
    else fprintf(f, "out"); }
  else if (kind==S_VAR_DECL)
    fprintf(f, "local");
  else if (kind==S_VAR_DECL_ITEM)
  { fprintf(f, "%s", e->sym->name.c_str());
    if (num>0)
      fprintf(f, "[%d]", num); }
  else if (kind==S_IDENTIFIER)
    fprintf(f, "%s", e->sym->name.c_str());
  else if (kind==S_NUMBER)
    fprintf(f, "%d", num);
  else if (kind==S_STRING)
    fprintf(f, "string#%d", num);
  else if (kind==S_ARITH_EXPR || kind==S_LOGICAL_EXPR || kind==S_RELATIONAL_EXPR ||
           kind==S_NOT_EXPR || kind==S_MINUS_EXPR)
    fprintf(f, "%s", operator_name(num));
  else if (kind==S_FUNCTION_CALL)
    fprintf(f, "call");
  else
  { fprintf(f, "%s", syntax_name[e->code]);
    if (e->sym!=NULL && e->sym!=no_symbol)
      fprintf(f, " \"%s\"", e->sym->name.c_str());
    if (e->info!=0)
      fprintf(f, " [%d]", e->info); } }

void print_syntax_tree(syntax_element * e, int maxd, int depth)
{ if (e==NULL)
    return;
  for (int i=0; i<depth/2; i+=1)
    printf("  | ");
  if (depth&1)
    printf("  ");
  print_head(stdout, e);
  if (depth==maxd)
    printf("  ... ...\n");
  else
  { printf("\n");
    for (int i=0; i<e->sub_parts(); i+=1)
      print_syntax_tree(e->part[i], maxd, depth+1); } }

void print_syntax_linear(FILE * f, syntax_element * e, int maxd, int depth)
{ if (e==NULL)
    return;
  if (depth>maxd)
  { fprintf(f, "~");
    return; }
  print_head(f, e);
  int n=e->sub_parts();
  if (n>0)
  { fprintf(f, "(");
    for (int i=0; i<n; i+=1)
    { if (i>0)
        fprintf(f, ", ");
      print_syntax_linear(f, e->part[i], maxd, depth+1); }
    fprintf(f, ")"); } }

const char * regform(int i)
{ static char ans[10];
  if (i==15)
    return "$pc";
  else if (i==14)
    return "$sp";
  else if (i==13)
    return "$fp";
  sprintf(ans, "$%d", i);
  return ans; }

struct operand
{ int number;
  const char * globalname;
  unsigned char idxreg;
  bool star;
  char kind;
  operand();
  operand(int n, int r=0, bool s=false);
  operand(const char * nm, bool s=false, int n=0);
  operand(const char * k, int n, int r=0, bool s=false);
  operand(const char * k, const char * nm, bool s=false, int n=0);
  char * form(); };

operand::operand()
{ }

operand::operand(int n, int r, bool s):
         number(n), idxreg(r), star(s), globalname(NULL), kind(' ')
{ }

operand::operand(const char * nm, bool s, int n):
         number(n), idxreg(0), star(s), globalname(nm), kind(' ')
{ }

operand::operand(const char * k, int n, int r, bool s):
         number(n), idxreg(r), star(s), globalname(NULL), kind(k[0])
{ }

operand::operand(const char * k, const char * nm, bool s, int n):
         number(n), idxreg(0), star(s), globalname(nm), kind(k[0])
{ }

char * operand::form()
  { static char answer[100];
    int len=0;
    if (star)
    { strcpy(answer, "* ");
      len=2; }
    if (idxreg!=0)
    { strcpy(answer+len, regform(idxreg));
      len=strlen(answer);
      answer[len]=' ';
      len+=1;
      answer[len]=0; }
    if (kind!=' ')
    { sprintf(answer+len, "_%c_", kind);
      len+=3; }
    if (globalname!=NULL)
    { strcat(answer+len, globalname);
      len+=strlen(globalname);
      answer[len]=' ';
      len+=1;
      answer[len]=0;
      return answer; }
    if (number < 0)
      sprintf(answer+len, "- %d", -number);
    else if (number==0 && !star && (idxreg!=0 || globalname!=NULL)) 
     { }
    else if (number==0 && idxreg!=0)
     { }
    else if (kind!=' ' || idxreg==0 && globalname==NULL)
      sprintf(answer+len, "%d", number);
    else
      sprintf(answer+len, "+ %d", number);
    return answer; }

void codegen(syntax_element * e);
vector <symbol_description *> locals;

void declare_variables(syntax_element * e, char kind)
{ int k=e->code;
  int n=e->sub_parts();
  if (k==S_PARAM_NAMES)
  { for (int i=0; i<n; i+=1)
    { syntax_element * item = e->part[i];
      item->sym->push_declaration(kind, 2+i, 0);
      locals.push_back(item->sym); } }
  else if (k==S_VAR_DECL)
  { for (int i=0; i<n; i+=1)
    { syntax_element * item = e->part[i];
      declare_variables(item, kind); } }
  else if (k==S_VAR_DECL_ITEM)
  { int size=e->info;
    if (size==0) size=1;
    if (kind=='G')
      e->sym->push_declaration('G', 0, size);
    else if (kind=='L')
    { locals_now += size;
      if (locals_now>locals_max)
        locals_max=locals_now;
      e->sym->push_declaration(kind, -locals_now, size);
      locals.push_back(e->sym); } } }

void undeclare_variable(symbol_description * s)
{ s->pop_declaration(); }

bool inrange(int number)
{ if (number<0)
    return (number | 0x7FFF)==-1;
  return (number & 0xFFFF8000)==0; }

int get_register()
{ for (int i=1, m=2; i<13; i+=1, m<<=1)
  { if (freeregs & m)
    { freeregs&=~m;
      return i; } }
  report_semantic_error(NULL, "expression is too complicated, not enough registers");
  return 0; }

void release_register(int r)
{ freeregs |= 1<<r; }

void release(operand x)
{ if (x.number==0 && x.globalname==NULL && !x.star && x.kind==' ')
    release_register(x.idxreg); }

operand codegen_expr(syntax_element * e);

operand codegen_expr_to_register(syntax_element * e, int reg=-1)
{ operand x = codegen_expr(e);
  if (x.number==0 && x.globalname==NULL && x.kind==' ' && x.idxreg!=0 && (x.idxreg==reg || reg==-1))
  { if (x.star)
    { fprintf(outfile, "      load  %3s, %s\n", regform(x.idxreg), x.form());
      return operand(0, x.idxreg); }
    else
      return x; }
  if (reg==-1 || (x.idxreg==0 && reg!=0))
  { reg = get_register();
    release(x); }
  fprintf(outfile, "      load  %3s, %s\n", regform(reg), x.form());
  return operand(0, reg); }

int newlabel()
{ nextfreelabel+=1;
  return nextfreelabel; }

void codegen_cond_jumptrue(syntax_element * e, int t);
void codegen_cond_jumpfalse(syntax_element * e, int f);

void codegen_cond_jump(syntax_element * e, int t, int f)
{ if (e==NULL)
    return;
  int op=e->info;
  switch (e->code)
  { case S_RELATIONAL_EXPR:
     { operand a = codegen_expr_to_register(e->part[0]);
       operand b = codegen_expr(e->part[1]);
       fprintf(outfile, "      cmp   %3s, %s\n", regform(a.idxreg), b.form());
       char * cond = " $0";
       switch (op)
       { case _EQUAL: cond="$eq"; break;
         case _NOT_EQUAL: cond="$ne"; break;
         case _LESS: cond="$lt"; break;
         case _NOT_MORE: cond="$le"; break;
         case _MORE: cond="$gt"; break;
         case _NOT_LESS: cond="$ge"; break; }
       fprintf(outfile, "      jcond %s, _L_%d\n", cond, t);
       fprintf(outfile, "      jump       _L_%d\n", f);
       release(b);
       release(a);
       break; }
    case S_NOT_EXPR:
     { codegen_cond_jump(e->part[0], f, t);
       break; }
    case S_LOGICAL_EXPR:
     { if (op==_AND)
       { codegen_cond_jumpfalse(e->part[0], f);
         codegen_cond_jumpfalse(e->part[1], f);
         fprintf(outfile, "      jump       _L_%d\n", t); }
       else if (op==_OR)
       { codegen_cond_jumptrue(e->part[0], t);
         codegen_cond_jumptrue(e->part[1], t);
         fprintf(outfile, "      jump       _L_%d\n", f); }
       break; }
    case S_NUMBER:
     { if (op==0)
         fprintf(outfile, "      jump       _L_%d\n", f);
       else
         fprintf(outfile, "      jump       _L_%d\n", t);
       break; }
    default:
     { operand a = codegen_expr(e);
       fprintf(outfile, "      test       %s\n", a.form());
       fprintf(outfile, "      jcond  $z, _L_%d\n", f);
       fprintf(outfile, "      jump       _L_%d\n", t);
       release(a);
       break; } } }

void codegen_cond_jumptrue(syntax_element * e, int t)
{ if (e==NULL)
    return;
  int op=e->info;
  switch (e->code)
  { case S_RELATIONAL_EXPR:
     { operand a = codegen_expr_to_register(e->part[0]);
       operand b = codegen_expr(e->part[1]);
       fprintf(outfile, "      cmp   %3s, %s\n", regform(a.idxreg), b.form());
       char * cond = " $0";
       switch (op)
       { case _EQUAL: cond="$eq"; break;
         case _NOT_EQUAL: cond="$ne"; break;
         case _LESS: cond="$lt"; break;
         case _NOT_MORE: cond="$le"; break;
         case _MORE: cond="$gt"; break;
         case _NOT_LESS: cond="$ge"; break; }
       fprintf(outfile, "      jcond %s, _L_%d\n", cond, t);
       release(b);
       release(a);
       break; }
    case S_NOT_EXPR:
     { codegen_cond_jumpfalse(e->part[0], t);
       break; }
    case S_LOGICAL_EXPR:
     { if (op==_AND)
       { int lab=newlabel();
         codegen_cond_jumpfalse(e->part[0], lab);
         codegen_cond_jumptrue(e->part[1], t);
         fprintf(outfile, "_L_%d:\n", lab); }
       else if (op==_OR)
       { codegen_cond_jumptrue(e->part[0], t);
         codegen_cond_jumptrue(e->part[1], t); }
       break; }
    case S_NUMBER:
     { if (op!=0)
         fprintf(outfile, "      jump       _L_%d\n", t);
       break; }
    default:
     { operand a = codegen_expr(e);
       fprintf(outfile, "      test       %s\n", a.form());
       fprintf(outfile, "      jcond $nz, _L_%d\n", t);
       release(a);
       break; } } }

void codegen_cond_jumpfalse(syntax_element * e, int f)
{ if (e==NULL)
    return;
  int op=e->info;
  switch (e->code)
  { case S_RELATIONAL_EXPR:
     { operand a = codegen_expr_to_register(e->part[0]);
       operand b = codegen_expr(e->part[1]);
       fprintf(outfile, "      cmp   %3s, %s\n", regform(a.idxreg), b.form());
       char * cond = " $0";
       switch (op)
       { case _EQUAL: cond="$eq"; break;
         case _NOT_EQUAL: cond="$ne"; break;
         case _LESS: cond="$lt"; break;
         case _NOT_MORE: cond="$le"; break;
         case _MORE: cond="$gt"; break;
         case _NOT_LESS: cond="$ge"; break; }
       fprintf(outfile, "      jcndf %s, _L_%d\n", cond, f);
       release(b);
       release(a);
       break; }
    case S_NOT_EXPR:
     { codegen_cond_jumptrue(e->part[0], f);
       break; }
    case S_LOGICAL_EXPR:
     { if (op==_OR)
       { int lab=newlabel();
         codegen_cond_jumptrue(e->part[0], lab);
         codegen_cond_jumpfalse(e->part[1], f);
         fprintf(outfile, "_L_%d:\n", lab); }
       else if (op==_AND)
       { codegen_cond_jumpfalse(e->part[0], f);
         codegen_cond_jumpfalse(e->part[1], f); }
       break; }
    case S_NUMBER:
     { if (op==0)
         fprintf(outfile, "      jump       _L_%d\n", f);
       break; }
    default:
     { operand a = codegen_expr(e);
       fprintf(outfile, "      test       %s\n", a.form());
       fprintf(outfile, "      jcond  $z, _L_%d\n", f);
       release(a);
       break; } } }

operand codegen_expression(syntax_element * e);
operand codegen_lvalue(syntax_element * e);

operand codegen_expr(symbol_description * s)
{ declaration * d = s->info;
  if (d==NULL)
  { report_semantic_error(NULL, "Undeclared identifier: %s", s->name.c_str());
    return operand(0); }
  if (d->kind=='L')
    return operand(d->detail, R_FP, true);
  if (d->kind=='P')
    return operand(d->detail, R_FP, true);
  if (d->kind=='G')
    return operand("G", s->name.c_str(), true);
  if (d->kind=='F')
    return operand("F", s->name.c_str(), false);
  return operand(0); }

operand codegen_expr_to_register(symbol_description * s, int reg=-1)
{ operand x = codegen_expr(s);
  if (x.number==0 && x.globalname==NULL && !x.star && x.kind==' ' && x.idxreg!=0 && (x.idxreg==reg || reg==-1))
    return x;
  if (reg==-1 || x.idxreg==0)
  { reg = get_register();
    release(x); }
  fprintf(outfile, "      load  %3s, %s\n", regform(reg), x.form());
  return operand(0, reg); }

operand codegen_expr(syntax_element * e)
{ if (e==NULL)
    return operand(0);
  int n=e->sub_parts();
  switch (e->code)
  { case S_NUMBER:
     { int n=e->info;
       if (inrange(n))
         return operand(e->info);
       else
       { int r=get_register();
         fprintf(outfile, "      load  %3s, %d\n", regform(r), n&0xFFFF);
         fprintf(outfile, "      loadh %3s, %d\n", regform(r), (n>>16)&0xFFFF);
         return operand(0, r); } }
    case S_IDENTIFIER:
      return codegen_expr(e->sym);
    case S_FUNCTION_CALL:
     { int oldfreeregs=freeregs;
       fprintf(outfile, "      pusha\n");
       for (int i=e->sub_parts()-1; i>0; i-=1)
       { operand a = codegen_expression(e->part[i]);
         fprintf(outfile, "      push       %s\n", a.form());
         release(a); }
       operand f = codegen_expression(e->part[0]);
       fprintf(outfile, "      call       %s\n", f.form());
       fprintf(outfile, "      add   $sp, %d\n", e->sub_parts()-1);
       fprintf(outfile, "      popa\n");
       freeregs=oldfreeregs;
       int reg=get_register();
       fprintf(outfile, "      store  $0, %s\n", regform(reg));
       return operand(0, reg); }
    case S_STRING:
       return operand("S", e->info);
    case S_ARRAY_ACCESS:
     { operand a = codegen_lvalue(e);
       fprintf(outfile, "      load  %3s, %s\n", regform(a.idxreg), a.form());
       return operand(0, a.idxreg); }
    case S_BYTEMAKE:
     { operand a = codegen_expr_to_register(e->part[1]);
       operand b = codegen_expr(e->part[0]);
       fprintf(outfile, "      loadh %3s, %s\n", regform(a.idxreg), b.form());
       return operand(0, a.idxreg); }
    case S_BYTESEL:
     { operand a;
       a = codegen_expr(e->part[1]);
       operand b = codegen_expr_to_register(e->part[0], 0);
       release(a);
       int reg = get_register();
       if (e->info==_OF)
         fprintf(outfile, "      GTBOF  %3s, %s\n", regform(reg), a.form());
       else
         fprintf(outfile, "      GTBFR  %3s, %s\n", regform(reg), a.form());
       return operand(0, reg); }
    case S_ADDRESS_OF:
     { operand a = codegen_lvalue(e->part[0]);
       return a; }
    case S_POINTER_FOLLOW:
     { operand a = codegen_expr_to_register(e->part[0]);
       return operand(0, a.idxreg, true); } 
    case S_MINUS_EXPR:
     { syntax_element * p1 = e->part[0];
       if (p1->code==S_NUMBER && inrange(p1->info))
         return operand(-p1->info);
       operand a = codegen_expr_to_register(p1);
       fprintf(outfile, "      neg  %3s, %s\n", regform(a.idxreg), regform(a.idxreg));
       return a; }
    case S_ARITH_EXPR:
     { operand a = codegen_expr_to_register(e->part[0]);
       operand b = codegen_expr(e->part[1]);
       char * instr = "bad";
       switch (e->info)
       { case _PLUS: instr="add"; break;
         case _MINUS: instr="sub"; break;
         case _TIMES: instr="mul"; break;
         case _DIVIDE: instr="div"; break;
         case _REMAINDER: instr="mod"; break; }
       fprintf(outfile, "      %s   %3s, %s\n", instr, regform(a.idxreg), b.form());
       release(b);
       return a; }
    case S_LOGICAL_EXPR:
    case S_NOT_EXPR:
    case S_RELATIONAL_EXPR:
     { int reg = get_register();
       int lab=newlabel();
       fprintf(outfile, "      zero       %s\n", regform(reg));
       codegen_cond_jumpfalse(e, lab);
       fprintf(outfile, "      inc        %s\n", regform(reg));
       fprintf(outfile, "_L_%d:\n", lab);
       return operand(0, reg); }
    default:
     { report_semantic_error(e, "Unexpected expression syntax, I don't know what to do");
       error_count-=1;
       return operand(0); } } }

operand codegen_lvalue(symbol_description * s)
{ if (s==NULL)
  { report_semantic_error(NULL, "Impossible error, null symbol");
    return operand(0); }
  declaration * d=s->info;
  if (d==NULL)
  { report_semantic_error(NULL, "Undeclared identifier: %s", s->name.c_str());
    return operand(0); }
  if (d->kind=='L')
    return operand(d->detail, R_FP);
  if (d->kind=='P')
    return operand(d->detail, R_FP);
  if (d->kind=='G')
    return operand("G", s->name.c_str());
  if (d->kind=='F')
    return operand("F", s->name.c_str());
  return operand(0); }

operand codegen_lvalue(syntax_element * e)
{ if (e==NULL)
    return operand(0);
  int n=e->sub_parts();
  switch (e->code)
  { case S_IDENTIFIER:
      return codegen_lvalue(e->sym);
    case S_ARRAY_ACCESS:
     { operand i = codegen_expr(e->part[1]);
       operand a = codegen_lvalue(e->part[0]);
       release(a);
       int r=get_register();
       fprintf(outfile, "      load  %3s, %s\n", regform(r), a.form());
       fprintf(outfile, "      add   %3s, %s\n", regform(r), i.form());
       release(i);
       return operand(0, r, true); }
    case S_POINTER_FOLLOW:
     { operand a = codegen_expr_to_register(e->part[0]);
       return operand(0, a.idxreg, true); } 
    default:
     { report_semantic_error(e, "This may not be the destination of an assignment");
       error_count-=1;
       return operand(0); } } }

operand codegen_expression(syntax_element * e)
{ freeregs=0xFFFF;
  return codegen_expr(e); }

operand codegen_expression_to_register(syntax_element * e)
{ freeregs=0xFFFF;
  return codegen_expr_to_register(e); }

void codegen(vector <syntax_element *> & v)
{ int n=v.size();
  for (int i=0; i<n && error_count==0; i+=1)
    codegen(v[i]); }

void codegen(syntax_element * e)
{ if (e==NULL)
    return;
  freeregs=0xFFFF;
  int n=e->sub_parts();
  switch (e->code)
  { case S_BLOCK:
     { int n=e->sub_parts(), lowtide=locals.size();
       for (int i=0; i<n && error_count==0; i+=1)
       { syntax_element * stmt = e->part[i];
         fprintf(outfile, "                      ; %d: ", stmt->line_number);
         print_syntax_linear(outfile, stmt, 3, 0);
         fprintf(outfile, "\n");
         codegen(e->part[i]); }
       for (int i=locals.size()-1; i>=lowtide; i-=1)
       { undeclare_variable(locals[i]);
         locals.pop_back(); }
       break; }
    case S_VAR_DECL:
     { if (e->info!=_LOCAL)
       { report_semantic_error(e, "Only local declarations are allowed inside functions");
         break; }
       declare_variables(e, 'L');
       for (int i=0; i<e->sub_parts(); i+=1)
       { syntax_element * d = e->part[i];
         if (d->sub_parts()<1) continue;
         operand var=codegen_lvalue(d->sym);
         operand val=codegen_expression_to_register(d->part[0]);
         fprintf(outfile, "      store %3s, %s\n", regform(val.idxreg), var.form()); }
       break; }
    case S_OUT:
     { operand o = codegen_expression(e->part[0]);
       char * x="n ";
       if (e->info=='S')
         x="s ";
       else if (e->info=='C')
         x="ch";
       fprintf(outfile, "      out%s      %s\n", x, o.form());
       break; }
    case S_ASSIGN:
     { operand var=codegen_lvalue(e->part[0]);
       operand val=codegen_expr_to_register(e->part[1]);
       fprintf(outfile, "      store %3s, %s\n", regform(val.idxreg), var.form());
       break; }
    case S_FUNCTION_CALL:
     { for (int i=e->sub_parts()-1; i>0; i-=1)
       { operand a = codegen_expression(e->part[i]);
         fprintf(outfile, "      push       %s\n", a.form());
         release(a); }
       operand f = codegen_expression(e->part[0]);
       fprintf(outfile, "      call       %s\n", f.form());
       fprintf(outfile, "      add   $sp, %d\n", e->sub_parts()-1);
       break; }
    case S_RETURN:
     { if (e->sub_parts()==1)
         codegen_expr_to_register(e->part[0], 0);
       fprintf(outfile, "      jump       _E_%s\n", function_name.c_str());
       break; }
    case S_IF:
     { if (e->sub_parts()==3)
       { int no=newlabel(), end=newlabel();
         codegen_cond_jumpfalse(e->part[0], no);
         codegen(e->part[1]);
         fprintf(outfile, "      jump       _L_%d\n", end);
         fprintf(outfile, "_L_%d:\n", no);
         codegen(e->part[2]);
         fprintf(outfile, "_L_%d:\n", end); }
       else
       { int over=newlabel();
         codegen_cond_jumpfalse(e->part[0], over);
         codegen(e->part[1]);
         fprintf(outfile, "_L_%d:\n", over); }
       break; }
    case S_BREAK:
     { fprintf(outfile, "      jump       _L_%d\n", breaklabel);
       break; }
    case S_CONTINUE:
     { fprintf(outfile, "      jump       _L_%d\n", continuelabel);
       break; }
    case S_EXIT:
     { fprintf(outfile, "      load  $sp, _V_EXITSP%d\n");
       fprintf(outfile, "      pop        $fp\n");
       fprintf(outfile, "      ret\n");
       break; }
    case S_WHILE:
     { int oldc=continuelabel, oldb=breaklabel;
       continuelabel=newlabel();
       breaklabel=newlabel();
       fprintf(outfile, "_L_%d:\n", continuelabel);
       codegen_cond_jumpfalse(e->part[0], breaklabel);
       codegen(e->part[1]);
       fprintf(outfile, "      jump       _L_%d\n", continuelabel);
       fprintf(outfile, "_L_%d:\n", breaklabel);
       continuelabel=oldc;
       breaklabel=oldb;
       break; }
    case S_FOR:
     { int oldc=continuelabel, oldb=breaklabel;
       continuelabel=newlabel();
       breaklabel=newlabel();
       freeregs=0xFFFF;
       if (e->sym->info==NULL)
       { report_semantic_error(e, "Control variable in loop has not been declared");
         break; }
       operand var=codegen_lvalue(e->sym);
       operand val=codegen_expr_to_register(e->part[0]);
       fprintf(outfile, "      store %3s, %s\n", regform(val.idxreg), var.form());
       fprintf(outfile, "_L_%d:\n", continuelabel);
       release(var);
       release(val);
       operand lim=codegen_expr_to_register(e->part[1]);
       var=codegen_expr(e->sym);
       fprintf(outfile, "      rcmp  %3s, %s\n", regform(lim.idxreg), var.form());
       release(var);
       if (e->info==+1)
         fprintf(outfile, "      jcond $gt, _L_%d\n", breaklabel);
       else
         fprintf(outfile, "      jcond $lt, _L_%d\n", breaklabel);
       release(lim);
       release(val);
       codegen(e->part[2]);
       var=codegen_expr(e->sym);
       int r = get_register();
       fprintf(outfile, "      load  %3s, %s\n", regform(r), var.form());
       if (e->info==+1)
         fprintf(outfile, "      add   %3s, 1\n", regform(r));
       else
         fprintf(outfile, "      sub   %3s, 1\n", regform(r));
       release(var);
       var=codegen_lvalue(e->sym);
       fprintf(outfile, "      store %3s, %s\n", regform(r), var.form());
       release(r);
       release(var);
       fprintf(outfile, "      jump       _L_%d\n", continuelabel);
       fprintf(outfile, "_L_%d:\n", breaklabel);
       continuelabel=oldc;
       breaklabel=oldb;
       break; }
    case S_ASSEMBLY:
     { string ass=assemblumps[e->info];
       int len=ass.length();
       for (int i=0; i<len; i+=1)
       { char c=ass[i];
         if (c=='[')
         { string s = "";
           i+=1;
           while (i<len)
           { c=ass[i];
             if (c==']' || c=='\n')
               break;
             if (c!=' ')
               s+=c;
             i+=1; }
           operand v = codegen_lvalue(all_symbols->look_up(s));
           fprintf(outfile, " %s ", v.form()); }
         else
           fputc(c, outfile); }
       fputc('\n', outfile);
       assemblumps[e->info]="";
       break; }
    case S_FUNC_DEF:
     { report_semantic_error(e, "Probably not enough }s, this appears to be a nested function");
       break; }
    default:
     { report_semantic_error(e, "Unexpected statement syntax, I don't know what to do");
       error_count-=1;
       break; } } }

void funcdef_codegen(syntax_element * e)
{ function_name=e->sym->name;
  breaklabel=0;
  continuelabel=0;
  if (function_name=="main")
    fprintf(outfile, "\nmain:");
  fprintf(outfile, "\n_F_%s:\n", function_name.c_str());
  fprintf(outfile, "      push       $fp\n");
  fprintf(outfile, "      load  $fp, $sp\n");
  if (function_name=="main")
    fprintf(outfile, "      store $fp, _V_EXITSP\n");
  fprintf(outfile, "      sub   $sp, _K_%s\n", function_name.c_str());
  locals.clear();
  declare_variables(e->part[0], 'P');
  locals_now=0;
  locals_max=0;
  codegen(e->part[1]);
  fprintf(outfile, "_E_%s:\n", function_name.c_str());
  fprintf(outfile, "      load  $sp, $fp\n");
  fprintf(outfile, "      pop        $fp\n");
  fprintf(outfile, "      ret\n");
  fprintf(outfile, "_K_%s = %d\n", function_name.c_str(), locals_max);
  for (int i=locals.size()-1; i>=0; i-=1)
  { undeclare_variable(locals[i]);
    locals.pop_back(); } }

void top_level_codegen(syntax_element * e)
{ if (e==NULL)
    return;
  int n=e->sub_parts();
  switch (e->code)
  { case S_PROGRAM:
     { for (int i=0; i<n && error_count==0; i+=1)
         top_level_codegen(e->part[i]);
       break; }
    case S_END_OF_FILE:
       break;
    case S_FUNC_DEF:
     { funcdef_codegen(e);
       break; }
    case S_VAR_DECL:
     { if (e->info!=_GLOBAL)
       { report_semantic_error(e, "Only global declarations are allowed outside functions");
         break; }
       fprintf(outfile, "\n");
       for (int i=0; i<e->sub_parts(); i+=1)
       { syntax_element * d = e->part[i];
         fprintf(outfile, "_G_%s: ", d->sym->name.c_str());
         if (d->info>1)
           fprintf(outfile, ".space %d\n", d->info);
         else if (d->sub_parts()<1)
           fprintf(outfile, ".data 0\n");
         else
         { operand a = codegen_expression(d->part[0]);
           if (a.star!=0 || a.idxreg!=0)
             report_semantic_error(d, "Global variable initialisations must be constants");
           fprintf(outfile, ".data %s\n", a.form()); } }
       break; }
    default:
     { report_semantic_error(e, "This may not appear outside of a function definition");
       break; } } }

void initialise_compiler(char * basename)
{ strings.clear();
  strings.push_back("");
  input_buffer="";
  input_buffer_len=0;
  input_buffer_pos=0;
  loop_depth=0;
  all_symbols = new symbol_table(2000);
  for (int i=0; 1; i+=1)
  { if (preloads[i].form==NULL)
      break;
    all_symbols->add(preloads[i].form, preloads[i].code, preloads[i].subcode); }
  register_syntax_names();
  current_line_number=0;
  current_symbol_start_line=0;
  current_symbol_start=0;
  current_symbol_end=0;
  char filename[300];
  strcpy(filename, basename);
  strcat(filename, ".p");
  input = fopen(filename, "r");
  if (input==NULL)
  { printf("can't read '%s'\n", filename);
    exit(1); }
  strcpy(filename, basename);
  strcat(filename, ".ass");
  outfile = fopen(filename, "w");
  if (input==NULL)
  { printf("can't overwrite '%s'\n", filename);
    exit(1); }
  copy_file=fopen("/dev/null", "w");
  error_count=0; }


void compile(char * basename)
{ initialise_compiler(basename);
  getsym();
  syntax_element * se = parse_all();
  if (main_def==NULL)
    report_syntax_error("This program has no main");
  if (error_count==0)
  { top_level_codegen(se);
    fprintf(outfile, "\n");
    for (int i=1; i<strings.size(); i+=1)
    { fprintf(outfile, "_S_%d: .ascii \"", i);
      printstr(outfile, strings[i]);
      fprintf(outfile, "\"\n"); }
    fprintf(outfile, "_V_EXITSP: .data 0\n");
    fprintf(outfile, "\n"); }
  if (error_count>0)
    printf("\nThere were %d errors reported\n", error_count); }

int main(int argc, char * argv[])
{ if (argc<2)
  { printf("Give me a file name, without the extension\n");
    return 1; }
  compile(argv[1]); 
  return 0;  }

