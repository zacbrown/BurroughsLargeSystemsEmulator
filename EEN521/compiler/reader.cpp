#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include "useful.h"
#include "reader.h"

using namespace std;

void reader::error(string err)
 { cout << "\n***** ERROR ON LINE " << line_num << ": " << err << "\n";
   cout << "    * " << line << "\n";
   cout << "    * ";
   for(int i=0; i<startpos-1; i+=1)
      cout << " ";
   for(int i=startpos; i<=pos; i+=1)
      cout << "*";
   cout << "\n";
   exit(1); }

void reader::reset()
{
	in.seekg(0);
}

int reader::get_linenum(void) {
    return line_num;
}

void reader::putback1char()
 { pos -= 1; }

void reader::putbacksymbol()
 { if (debugging)
      cout << "reader putting back symbol\n";
   pos = startpos; }

string reader::visiblestring(string str)
{
    return str;
}

char reader::getchar()
 { if (pos < 0)
    { pos += 1;
      return ' '; }
   else if (pos > length || line[pos] == '#')
    { getline(in, line);
      pos = 0;
      length = line.length();
      line_num += 1;
      return ' '; }
   else if (pos == length)
    { pos += 1;
      return ' '; }
   pos += 1;
   return line[pos-1]; }

reader::reader(istream &i):in(i)
 { pos = 0;
   length = 0;
   line_num = 0;
   debugging = false; } 

void reader::debug(bool b)
 { debugging = b; }

string reader::read(string comment)
 { string result = "";
   char c = ' ';
   while (c == ' ')
      c = getchar(); 
   startpos = pos-1;
   if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '_')
    { while (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '_')
       { result += c;
         c = getchar(); }
      putback1char();
      if (debugging)
        cout << "reader gets '" << visiblestring(result) << "' for " << comment << "\n";
      return result; }
   else if (c=='<' || c=='>' || c=='!')
    { result += c;
      c = getchar();
      if (c=='=')
         result += c;
	  else if (c=='|')
		  result += c;
      else
         putback1char();
      if (debugging)
        cout << "reader gets '" << visiblestring(result) << "' for " << comment << "\n";
      return result; }
   else if (c=='\"') {
	  putback1char();
	  return read_string();
   }
   else
    { result += c;
      if (debugging)
        cout << "reader gets '" << visiblestring(result) << "' for " << comment << "\n";
      return result; } }

string reader::read_string(string comment) {
	string result = "";
    result += getchar();
	char c = getchar();
	
	while (c != '\"') {
		result += c;
		c = getchar();
	}
	result += c;
	return result;
}

string reader::read_assembly(string comment) {
	string result = "";
	char c = ' ';
	while (pos != 0 && c != ';') {
		result += c;
		c = getchar();
    } cout << "result: " << result << endl;
    if (c == ';') putback1char();
	return result;
}

reader & operator >> (reader & r, string & s)
 { s = r.read();
   return r; }
