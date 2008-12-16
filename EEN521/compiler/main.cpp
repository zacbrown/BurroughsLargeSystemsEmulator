#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include "useful.h"
#include "reader.h"
#include "parser.h"
#include "translator.h"

using namespace std;
	
int main(int argc, char **argv)
 { if (argc < 2)
    { cout << "ERROR: no input file provided\n";
      return 1; }
   ifstream infile (argv[1]);
   reader in (infile);
   char *new_filename = strdup(argv[1]);
   char *dot_delim = strrchr(new_filename, '.');
   if (dot_delim != NULL) {
       dot_delim++;
       *dot_delim = 's';
   }
   else strcat(new_filename, ".s");
    
   trans_set_output_file_stream(new_filename);

   node *prog = parse(in);

   prog->print(); cout << endl;
   prog->translate_program();
   return 0; }
