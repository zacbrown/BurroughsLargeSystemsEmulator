// this is translator.cpp

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include "useful.h"
#include "reader.h"
#include "parser.h"
#include "translator.h"

using namespace std;

int labels=0,
    totalsize=0;    // total size of current function's local variables
ofstream fout;

typedef struct str_label_s {
    string label;
    string detail;
} str_label_t, *str_label_p;

vector<str_label_t> string_label_list;

static string format_assembly(string line) {
	int i = 0;
	string ret = "     ";
	bool done_begin = false;
	while (i < line.length()) {
		if (line[i] == ' ' && done_begin == false) {
			i++;
			continue;
		}
		else {
			ret = ret + line[i];
			done_begin = true;
		}
		i++;
	}
	return ret;
}

void trans_set_output_file_stream(const char *filename)
{
	fout.open(filename);
}

struct symbolinfo
 { char kind;
       // 'l' = local variable,  'L' = local array,
       // 'g' = global variable, 'G' = global array,
       // 'f' = function,
       // 'u' = unknown
   int info;
   symbolinfo * prev; 
   symbolinfo( char k, int i, symbolinfo * p )
    { kind = k;
      info = i;
      prev = p; } };

struct symbol
 { string name;
   symbolinfo * info;
   symbol * next;
   symbol( string n, symbolinfo * i, symbol * x )
    { name = n;
      info = i;
      next = x; } };

struct symboltable
 { static const int tablesize = 2000;
   symbol * table[tablesize];
   symboltable();
   int hash( string s );
   symbolinfo * lookup( string s );
   symbol * find_symbol ( string s );
   void print(void);
   void remove_declaration( string s );
   symbolinfo * declare( string s, char k, int o ); };

symboltable::symboltable()
 { for ( int i = 0; i < tablesize; i+=1 )
     table[i] = NULL; }

int symboltable::hash( string s )
 { int h = 9127451;
   for ( int i = 0; i < s.length(); i+=1 )
      h = h * 69 + s[i];
   if ( h < 0 )
      h = -h;
   return h % tablesize; }

void symboltable::print(void)
{
    int i = 0;
    for (i = 0; i < tablesize; i++) {
        symbol * blah = table[i];
        if (blah != NULL) {
            cout << "name: " << blah->name << endl;
            symbolinfo *next = blah->info;
            while(next != NULL) {
                cout << "\tkind: " << next->kind << endl;
                next = next->prev;
            }
        }
    }
}

symbolinfo * symboltable::lookup( string s )
    { int h = hash(s);
      symbol * p = table[h];
      while( p != NULL )
       { if( p->name == s )
            return p->info;
         p = p->next; }
      symbolinfo * n = new symbolinfo( 'u', 0, NULL );
      table[h] = new symbol( s, n, table[h] );
      return n; }

symbol * symboltable::find_symbol ( string s )
{
	int h = hash(s);
    symbol * p = table[h];
    while( p != NULL )
    { if( p->name == s )
          return p;
	  p = p->next; 
	}
	return NULL;
}

void symboltable::remove_declaration( string s )
{
	symbol* p = find_symbol(s);
	if (p == NULL) return;
	symbolinfo* sym_info_p = p->info;
	p->info = sym_info_p->prev;
	delete(sym_info_p);
}

symbolinfo * symboltable::declare( string s, char k, int o )
    { int h = hash(s);
      symbol * p = table[h];
      while( p != NULL )
       { if( p->name == s )
            break;
         p = p->next; }
      if( p == NULL )
       { symbolinfo * n = new symbolinfo( k, o, NULL );
         symbol * sy = new symbol( s, n, table[h] );
         table[h] = sy;
         return n; }
      else
       { symbolinfo * n = new symbolinfo( k, o, p->info );
         p->info = n;
         return n; } }

symboltable ST;


void node::translatejumpiffalse(int where, int reg)
 { if(tag == "binop") 
    { part[0]->translateexpression(reg);
      part[1]->translateexpression(reg+1);
      fout << "     COMP   R" << reg << ", R" << (reg+1) << "\n";
      if (detail == ">")
         fout << "     JCOND  LEQ, L" << where << "\n";
      else if (detail == "<")
         fout << "     JCOND  GEQ, L" << where << "\n";
      else if (detail == ">=")
         fout << "     JCOND  LSS, L" << where << "\n";
      else if (detail == "<=")
         fout << "     JCOND  GTR, L" << where << "\n";
      else if (detail == "=")
         fout << "     JCOND  NEQ, L" << where << "\n";
      else if (detail == "!=")
         fout << "     JCOND  EQL, L" << where << "\n";
      else if (detail == "and")
       { part[0]->translatejumpiffalse(where, reg);
         part[1]->translatejumpiffalse(where, reg); }
      else if (detail == "or")
       { int lab1 = labels+1;
         labels+=1;
         part[0]->translatejumpiftrue(lab1, reg);
         part[1]->translatejumpiffalse(where, reg);
         fout << "L" << lab1 << ":\n"; }
      else if (detail == "not")
         part[0]->translatejumpiftrue(where, reg);
      else 
       { translateexpression(reg);
         fout << "     JZER   R" << reg << ", L" << where << "\n"; } } 
   else 
    { translateexpression(reg);
      fout << "     JZER   R" << reg << ", L" << where << "\n"; } } 



void node::translatejumpiftrue(int where, int reg)
 { if(tag == "binop") 
    { part[0]->translateexpression(reg);
      part[1]->translateexpression(reg+1);
      fout << "     COMP   R" << reg << ", R" << (reg+1) << "\n";
      if (detail == "<=")
         fout << "     JCOND  LEQ, L" << where << "\n";
      else if (detail == ">=")
         fout << "     JCOND  GEQ, L" << where << "\n";
      else if (detail == "<")
         fout << "     JCOND  LSS, L" << where << "\n";
      else if (detail == ">")
         fout << "     JCOND  GTR, L" << where << "\n";
      else if (detail == "!=")
         fout << "     JCOND  NEQ, L" << where << "\n";
      else if (detail == "=")
         fout << "     JCOND  EQL, L" << where << "\n";
      else if (detail == "or")
       { part[0]->translatejumpiftrue(where, reg);
         part[1]->translatejumpiftrue(where, reg); }
      else if (detail == "and")
       { int lab1 = labels+1;
         labels+=1;
         part[0]->translatejumpiffalse(lab1, reg);
         part[1]->translatejumpiftrue(where, reg);
         fout << "L" << lab1 << ":\n"; }
      else if (detail == "not")
         part[0]->translatejumpiffalse(where, reg);
      else 
       { translateexpression(reg);
         fout << "     JNZ    R" << reg << ", L" << where << "\n"; } } 
   else 
    { translateexpression(reg);
      fout << "     JNZ    R" << reg << ", L" << where << "\n"; } } 




void node::translateexpression(int reg)
 { if (tag == "variable")
    { symbolinfo * s = ST.lookup( detail );
      string sign = "";
      if (s->info>=0)
         sign = "+";
      if (s->kind=='u')
         cout << "Error: undeclared variable '" << detail << "'\n";
      else if (s->kind=='l')  
         fout << "     LOAD   R" << reg << ", [FP" << sign << s->info << "]\n";
      else if (s->kind=='L')
         fout << "     LOAD   R" << reg << ", FP" << sign << s->info << "\n";
      else if (s->kind=='g')
         fout << "     LOAD   R" << reg << ", [g_" << detail << "]\n";
      else if (s->kind=='G')
         fout << "     LOAD   R" << reg << ", g_" << detail << "\n";
      else if (s->kind=='f')
         fout << "     LOAD   R" << reg << ", f_" << detail << "]\n"; }
     
   else if(tag == "number")
      fout << "     LOAD   R" << reg << ", " << value << "\n";

   else if(tag == "binop")
    { part[0]->translateexpression(reg);
      part[1]->translateexpression(reg+1);
      if (detail == "+")
         fout << "     ADD    R" << reg << ", R" << (reg+1) << "\n";
      else if (detail == "-")
         fout << "     SUB    R" << reg << ", R" << (reg+1) << "\n";
      else if (detail == "*")
         fout << "     MUL    R" << reg << ", R" << (reg+1) << "\n";
      else if (detail == "/")
         fout << "     DIV    R" << reg << ", R" << (reg+1) << "\n";
      else if (detail=="<=" || detail==">=" || detail=="<" || detail==">" || detail=="!=" ||
               detail=="=" || detail=="or" || detail=="and" || detail=="not")
       { int lab1 = labels+1;
         labels+=1;
         fout << "     LOAD   R" << reg << ", 0\n";
         translatejumpiffalse(lab1, reg+1);
         fout << "     LOAD   R" << reg << ", 1\n";
         fout << "L" << lab1 << ":\n"; }
      else
       { cout << "Error: Invalid expression binop node\n";
         print();
         fout << "\n";
         exit(1); } }

	else if (tag == "get_ptr") {
		symbolinfo * s = ST.lookup( part[0]->detail );
		string sign = "";
		if (s->info>=0)
			 sign = "+";
		if (s->kind=='u')
			cout << "Error: undeclared variable '" << part[0]->detail << "'\n";
		else if (s->kind=='l')  
			fout << "     LOAD   R" << reg << ", FP" << sign << s->info << "\n";
		else if (s->kind=='g')
			fout << "     LOAD   R" << reg << ", g_" << part[0]->detail << "\n";
	}

   else if (tag == "functioncall")
   {
	   for (int i = reg - 1; i > 0; i--)
		   fout << "     PUSH   R" << i << endl;
	   for (int i = value - 1; i >= 0; i--)
	   {
		   part[i]->translateexpression(1);
		   fout << "     PUSH   R1\n";
	   }
	   fout << "     CALL   f_" << detail << endl;
	   fout << "     ADD    SP, " << value << endl;
	   for (int i = 1; i < reg; i++)
		   fout << "     POP    R" << i << endl;
	   if (reg != 0)
		   fout << "     STORE  R0, R" << reg << endl;
   }

   else if (tag == "follow_ptr") {
	   part[0]->translateexpression(reg);
	   fout << "     LOAD   R" << reg << ", [R" << reg << "]\n";
   }

   else if (tag == "string") {
	   fout << "     LOAD   R" << reg << ", " << ref_var << endl;
       str_label_t tmp = { ref_var, detail };
	   string_label_list.push_back(tmp);
   }

   else if(tag == "inchar")
    { fout << "     FAKEIT R1, $readchar\n"; }

   else
    { cout << "Error: Invalid node\n";
      print();
      cout << "\n";
      exit(1); } }


void node::translatestatement()
 { if(tag == "sequence")
    { for(int i=0; i<part.size(); i++)
       { fout << "\n";
         part[i]->translatestatement(); } }

   else if(tag == "assignment")
    { part[1]->translateexpression(1);
      string variable = part[0]->detail;
      symbolinfo * s = ST.lookup( variable );
      string sign = "";
      if (s->info>=0)
         sign = "+";
      if (s->kind=='u')
         cout << "Error: undeclared variable '" << variable << "'\n";
      else if (s->kind=='f')
         cout << "Error: assignment to function name '" << variable << "'\n";
      else if (s->kind=='L' || s->kind=='G')
         cout << "Error: assignment to array name '" << variable << "'\n";
      else if (s->kind=='l')
         fout << "     STORE  R1, [FP" << sign << s->info << "]\n";
      else if (s->kind=='g')
		 fout << "     STORE  R1, [g_" << variable << "]\n"; }

	else if(tag == "assign_ptr")
    { part[1]->translateexpression(1);
      part[0]->translateexpression(2);
	  fout << "     STORE  R1, [R2]\n"; }

   else if(tag == "print")
    { part[0]->translateexpression(1);
      fout << "     FAKEIT R1, $printint\n"; }

   else if(tag == "printchar")
    { part[0]->translateexpression(1);
      fout << "     FAKEIT R1, $printchar\n"; }

   else if(tag == "printstr")
    { part[0]->translateexpression(1);
      fout << "     FAKEIT R1, $printstr\n"; }

   else if (tag == "const") {
        for (int i = 0; i < part.size(); i++) {
            node *item = part[i];
            symbolinfo * s = ST.lookup(item->detail);
            if (s->kind != 'u') continue;
            ST.declare(item->detail, 'c', item->value);
        }
    }

   else if(tag == "while")
    { int lab1=labels+1, lab2=labels+2;
      labels+=2;
      fout << "L" << lab1 << ":\n";
      part[0]->translatejumpiffalse(lab2, 1);
      part[1]->translatestatement();
      fout << "     JUMP   L" << lab1 << "\n";
      fout << "L" << lab2 << ":\n"; }

   else if(tag == "if")
    { int lab1 = labels+1;
      labels+=1;
      part[0]->translatejumpiffalse(lab1, 1);
      part[1]->translatestatement();
      if (detail == "else")
      { fout << "     JUMP   L" << lab1+1 << "\n";
        fout << "L" << lab1 << ":\n";
        part[2]->translatestatement(); }
      else fout << "L" << lab1 << ":\n"; }

   else if(tag == "else")
    { int lab1 = labels+1;
      labels+=1;
      part[0]->translatestatement();
      fout << "L" << lab1 << ":\n"; }


   else if(tag == "halt")
      fout << "     HALT\n";

   else if(tag == "local")
    { for (int i=0; i<part.size(); i++)
       { node * n = part[i];
         string name = n->detail;
         int size = n->value;
         char kind = 'l';
         if ( size != 0 ) {
            kind = 'L';
            if (size < 0) {
                symbolinfo * s = ST.lookup(ref_var);
                if (s->kind == 'u') {
                    cout << "Error: undeclared const referenced in size declaration of local array\n";
                    exit(0);
                }
                if (s->kind != 'c') {
                    cout << "Error: only const values can be used to reference size of array or struct\n";
                    exit(0);
                }
                size = s->info;
            }
         }
         else
            size = 1;

         totalsize += size;
         ST.declare( name, kind, -totalsize ); } }

   else if (tag == "functioncall")
   {
	   translateexpression(0);
   }

   else if (tag == "assembly")
   {
	   string::size_type open_bracket = detail.find("<");
	   string::size_type close_bracket = detail.find(">");
	   string lookup_symbol = "";
	   stringstream formatted_str;

	   if (open_bracket != string::npos) {
		   if (close_bracket != string::npos) {
				lookup_symbol = detail.substr(open_bracket+1, close_bracket-open_bracket-1);
				symbolinfo *s = ST.lookup(lookup_symbol);
				
				string sign = "";
				if (s->info >= 0) sign = "+";
				formatted_str << detail.substr(0, open_bracket);
				if (s->kind == 'g')
					formatted_str << "g_" << lookup_symbol <<
						detail.substr(close_bracket+1);
				else
					formatted_str << "FP" << sign << s->info <<
						detail.substr(close_bracket+1);
				fout << format_assembly(formatted_str.str());
		   }
		   else {
			   cout << "Error: invalid embedded assembly\n";
			   print();
			   exit(0);
		   }
	   }
	   else {
		   formatted_str << detail;
		   fout << format_assembly(formatted_str.str());
	   }
   }

   else if(tag == "return")
    { if (detail == "void")
	  { fout << "     LOAD   SP, FP\n";
		fout << "     POP    FP\n";
		fout << "     RET\n"; }

      else
      { part[0]->translateexpression(1);
        fout << "     LOAD   R0, R1\n";
		fout << "     LOAD   SP, FP\n";
		fout << "     POP    FP\n";
        fout << "     RET\n"; } }

   else
    { cout << "Error: Invalid statement node\n";
      print();
      cout << "\n";
      exit(1); } }


void node::translate_top_level()
{
    if (tag == "export") {
        int size = part.size();

        for (int i = 0; i < size; i++) {
            string prefix;
            if (detail == "function")
                prefix = "f_";
            else prefix = "g_";

            fout << "  .EXPORT   " << prefix << part[i]->detail << endl;
        }
    }

	else if (tag == "import") {
        int size = part.size();

        for (int i = 0; i < size; i++) {
            string prefix;
            if (detail == "function")
                prefix = "f_";
            else prefix = "g_";

            fout << "  .IMPORT   " << prefix << part[i]->detail << endl;
        }
    }

	else if(tag == "functiondef")
	{ 
		int offset = 2;
		fout << "\nf_" << detail << ":\n";
		fout << "     PUSH   FP\n";
		fout << "     LOAD   FP, SP\n";
		fout << "     SUB    SP, " << value << endl;
		for (int i = 0; i < value; i++)
		{
			int size = part[i]->value;
			char kind = 'l';
			if (size != 0)
				kind = 'L';
			else
				size = 1;
			offset += size;
			ST.declare(part[i]->detail, kind, offset - size);
		}
		part[value]->translatestatement();

		/* remove argument declarations */
		for (int i = 0; i < value; i++)
			ST.remove_declaration(part[i]->detail);

		/* remove local declarations from the block */
		node* n = part[value];
		for (int i = 0; i < n->part.size(); i++)
		{
			node* tmp_loc = n->part[i];
			if (tmp_loc->tag == "local")
			{
				for (int k = 0; k < tmp_loc->part.size(); k++)
				{
					node* tmp = tmp_loc->part[k];
					ST.remove_declaration(tmp->detail);
				}
			}
		}
	}
	else if (tag == "main")
	{
		int offset = 0;
		fout << "\nmain:\n";
		fout << "     PUSH   FP\n";
		fout << "     LOAD   FP, SP\n";
		for (int i = 0; i < part[0]->part.size(); i++) {
			if (part[0]->part[i]->tag == "local")
				offset += part[0]->part[i]->part.size();
		}
		fout << "     SUB    SP, " << offset << endl;
		fout << "     LOAD   R1, [0x00000100]\n";
		fout << "     STORE  R1, [g_MEMSTART]\n";
		fout << "     LOAD   R1, [0x00000101]\n";
		fout << "     STORE  R1, [g_MEMAVAIL]\n";
		part[0]->translatestatement();
		fout << endl;
	}

	else if (tag == "global")
    { for (int i=0; i<part.size(); i++)
       { node * n = part[i];
         string name = n->detail;
         int size = n->value;
         char kind = 'g';
         if ( size != 0 ) { 
            kind = 'G';
            if (size < 0) {
                symbolinfo * s = ST.lookup(n->ref_var);
                if (s->kind == 'u') {
                    cout << "Error: undeclared const referenced in size declaration of global array\n";
                    exit(0);
                }
                if (s->kind != 'c') {
                    cout << "Error: only const values can be used to reference size of array or struct\n";
                    exit(0);
                }
                size = s->info;
            }
         }
         else
             size = 1;
		 fout << "g_" << name << ":      .space     " << size << endl; }}

    else if (tag == "const") {
        for (int i = 0; i < part.size(); i++) {
            node *item = part[i];
            symbolinfo * s = ST.lookup(item->detail);
            if (s->kind != 'u') continue;
            ST.declare(item->detail, 'c', item->value);
        }
    }

	else if (tag == "end") return;

	else
	{
		cout << "Error: Invalid top level declaration \'" << tag << "\'" << endl;
		exit(1);
	}
}

void node::add_top_level_decl()
{
	string name;
	char kind = 0;
	int size = 0;
    if (tag == "functiondef") 
	{
		name = detail;
		kind = 'f';
		ST.declare(name, kind, 0);
	}

	if (tag == "import") 
	{
		for (int i=0; i<part.size(); i++)
       { node * n = part[i];
         string name = n->detail;
         char kind = 'f';
         ST.declare(name, kind, 0); 
	    }
	}

	if (tag == "global") 
	{
		for (int i=0; i<part.size(); i++)
       { node * n = part[i];
         string name = n->detail;
         int size = n->value;
         char kind = 'g';
         if ( size != 0 ) { 
            kind = 'G';
            if (size < 0) {
                symbolinfo * s = ST.lookup(n->ref_var);
                if (s->kind == 'u') {
                    cout << "Error: undeclared const referenced in size declaration of global array\n";
                    ST.print();
                    exit(0);
                }
                if (s->kind != 'c') {
                    cout << "Error: only const values can be used to reference size of array or struct\n";
                    exit(0);
                }
                size = s->info; cout << "ref_var: " << n->ref_var <<endl;
            }
         }
         else
             size = 1;
         ST.declare( name, kind, size ); 
	    }
	}

    if (tag == "const") {
        for (int i = 0; i < part.size(); i++) {
            node *item = part[i];
            symbolinfo * s = ST.lookup(item->detail);
            if (s->kind != 'u') {
                continue;
            }
            ST.declare(item->detail, 'c', item->value);
        }
    }
}

static void write_string_labels() {
	int size = string_label_list.size();
	fout << "\n\n";
	for (int i = 0; i < size; i++) {
        str_label_p tmp = &string_label_list[i];
        fout << tmp->label << ": .STRING " << 
			tmp->detail << endl;
	}
}

void node::translate_program()
{
    vector<string> function_list;

	if (fout.is_open() == false) {
		cout << "ERROR: no initialized filename for output\n";
		exit(0);
	}

    for (int i = 0; i < part.size(); i++) {
        if (part[i]->tag == "main") { 
	        fout << "\n     JUMP main\n\n";
            break;
        }
    }

    for (int i = 0; i < part.size(); i++) {
        if (part[i]->tag == "functiondef")
            function_list.push_back(part[i]->detail);
    }

    for (int i = 0; i < part.size(); i++) {
        vector<string> not_found;
        bool found = false;

        if (part[i]->tag == "export") {
            vector<node*> n_part = part[i]->part;
            for (int j = 0; j < n_part.size(); j++) {
                for (int k = 0; k < function_list.size(); k++) {
                    if (n_part[j]->detail == function_list[k]) {
                        found = true;
                        break;
                    }
                }
                if (found == false) not_found.push_back(n_part[j]->detail);
                found = false;
            }
        }
        
        if (not_found.size() > 0) {
            cout << "Error, exported undefined global or functions: ";
            for (int j = 0; j < not_found.size() - 1; j++)
                cout << not_found[j] << ", ";
            cout << not_found[not_found.size() - 1] << endl;
            exit(0);
        }

		part[i]->translate_top_level();
	}

	write_string_labels();
}