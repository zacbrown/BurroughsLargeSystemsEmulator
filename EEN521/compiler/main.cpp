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
 { if (argc < 3)
    { cout << "ERROR: no input file provided or no output file provided\n";
      return 1; }
   ifstream infile (argv[1]);
   reader in (infile);
   trans_set_output_file_stream(argv[2]);

   node *prog = parse(in);

   prog->print();
   cout << endl;
   prog->translate_program();
   return 0; }
