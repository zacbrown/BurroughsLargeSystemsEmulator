#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>

using namespace std;

struct reader
 { string line;
   int pos, length, line_num, startpos;
   istream &in;
   bool debugging;

   reader(istream &i); 
   char getchar();
   int get_linenum(void);
   string read(string comment="");
   string read_assembly(string comment="");
   string read_string(string comment="");
   string visiblestring(string str);
   void debug(bool onoroff);
   void reset();
   void putback1char();
   void putbacksymbol();
   void error(string err); };

reader & operator >> (reader & r, string & s);

