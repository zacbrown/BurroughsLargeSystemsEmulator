#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

void uppercase(string & s)
{ for (int i=0; i<s.length(); i+=1)
    s[i]=toupper(s[i]); }

void lowercase(string & s)
{ for (int i=0; i<s.length(); i+=1)
    s[i]=tolower(s[i]); }

string dedollar(string s)
{ string r="";
  for (int i=0; i<s.length(); i+=1)
  { char c=s[i];
    if (c>='a' && c<='z' || c>='A' && c<='Z' || c=='_' || c>='0' && c<='9')
      r+=c; }
  return r; }

int find(vector <string> & v, string s)
{ for (int i=0; i<v.size(); i+=1)
  { if (v[i]==s)
      return i; }
  return -1; }

vector <string> opcodes, fakecodes, iocommands, condcodes, flags, opcodenotes,
                interrupts, intdescriptions, specregs, pageprots;
vector <int> notes;

bool process(const char * filename)
{ string line, part;
  int linenum;
  ifstream fin(filename);
  if (fin.fail())
  { printf("Can not read file '%s'\n", filename);
    return false; }
  for (int linenum=1; true; linenum+=1)
  { getline(fin, line);
    if (fin.fail())
      break;
    istringstream iss(line);
    iss >> part;
    if (iss.fail() || part[0]=='/' && part[1]=='/')
      continue;
    lowercase(part);

    if (part=="opcodenote")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'opcodenote' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      opcodenotes.push_back(part); }

    else if (part=="opcode")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'opcodenote' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      opcodes.push_back(part);
      int note=0;
      while (true)
      { iss >> part;
        if (iss.fail() || part[0]=='/' && part[1]=='/')
          break;
        uppercase(part);
        int num = find(opcodenotes, part);
        if (num<0)
        { printf("line %d WRONG: '%s' not defined as an opcodenote\n", linenum, part.c_str());
          exit(1); }
        note |= 1<<num; }
      notes.push_back(note); }

    else if (part=="fakecode")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'fakecode' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      fakecodes.push_back(part); }

    else if (part=="iocommand")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'iocommand' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      iocommands.push_back(part); }

    else if (part=="pageprot")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'pageprot' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      pageprots.push_back(part); }

    else if (part=="condcode")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'condcode' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      condcodes.push_back(part); }

    else if (part=="interrupt")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'interrupt' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      interrupts.push_back(part);
      string descr;
      getline(iss, descr);
      int quote1=-1, quote2=-1;
      for (int i=0; i<descr.length(); i+=1)
      { if (descr[i]=='\"')
        { if (quote1==-1)
            quote1=i;
          else if (quote2==-1)
            quote2=i; } }
      if (quote1<0 || quote2<0)
      { printf("line %d WRONG: 'interrupt name' not followed by quoted string\n", linenum);
        exit(1); }
      intdescriptions.push_back(descr.substr(quote1+1, quote2-quote1-1)); }

    else if (part=="flag")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'flag' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      flags.push_back(part); }

    else if (part=="specreg")
    { iss >> part;
      if (iss.fail() || part[0]=='/' && part[1]=='/')
      { printf("line %d WRONG: 'specreg' not followed by anything\n", linenum);
        exit(1); }
      uppercase(part);
      specregs.push_back(part); }

    else
    { printf("line %d WRONG: '%s' doesn't mean anything to me\n", linenum, part.c_str());
      exit(1); } }

  fin.close();
  return true; }

void printarray(FILE * f, const char *name, vector <string> & v, const char * prefix = "")
{ fprintf(f, "const char * const %s[] =\n  {", name);
  int n = v.size();
  for (int i=0; i<n; i+=1)
  { fprintf(f, " \"%s%s\"", prefix, v[i].c_str());
    int len = v[i].length();
    if (i!=n-1)
      fprintf(f, ",");
    for (int j=len; j<12; j+=1)
      fprintf(f, " ");
    if (i%6==5 && i!=n-1)
      fprintf(f, "\n   "); }
  fprintf(f, " };\n"); }

void printarray(FILE * f, const char * name, vector <int> & v)
{ fprintf(f, "const int %s[] =\n  {", name);
  int n = v.size();
  for (int i=0; i<n; i+=1)
  { fprintf(f, " %6d", v[i]);
    if (i!=n-1)
      fprintf(f, ",");
    if (i%10==9 && i!=n-1)
      fprintf(f, "\n   "); }
  fprintf(f, " };\n"); }

void output(const char * filename)
{ FILE * f = fopen(filename, "w");
  if (f==NULL)
  { fprintf(stderr, "Can't create new file '%s'\n", filename);
    exit(1); }
  fprintf(f, "// %s\n\n", filename);

  int n=opcodenotes.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int OPCODENOTE_%s = %d;\n", dedollar(opcodenotes[i]).c_str(), 1<<i);
  fprintf(f, "\n");

  n=opcodes.size();
  for (int i=n; i<128; i+=1)
  { char name[20];
    sprintf(name, "OP%d", i);
    opcodes.push_back(name);
    notes.push_back(0); }
  n=opcodes.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int OP_%-8s = %d;\n", dedollar(opcodes[i]).c_str(), i);
  printarray(f, "instruction_name", opcodes);
  printarray(f, "instruction_notes", notes);
  fprintf(f, "const int num_opcodes = %d;\n\n", n);

  n=fakecodes.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int FAKE_%-12s = %d;\n", dedollar(fakecodes[i]).c_str(), i);
  printarray(f, "fakecodes", fakecodes);
  fprintf(f, "const int num_fakecodes = %d;\n\n", n);

  n=iocommands.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int IO_%-14s = %d;\n", dedollar(iocommands[i]).c_str(), i);
  printarray(f, "iocommands", iocommands);
  fprintf(f, "const int num_iocommands = %d;\n\n", n);

  n=condcodes.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int COND_%-12s = %d;\n", dedollar(condcodes[i]).c_str(), i);
  while (condcodes.size()<16)
    condcodes.push_back("???");
  printarray(f, "condcodes", condcodes);
  fprintf(f, "const int num_condcodes = %d;\n\n", n);

  n=flags.size();
  for (int i=0; i<n; i+=1)
  { string ddf=dedollar(flags[i]);
    fprintf(f, "const int FLAG_%-9s = %d;\n", ddf.c_str(), i); }
  for (int i=0; i<n; i+=1)
  { string ddf=dedollar(flags[i]);
    fprintf(f, "const int FLAGMASK_%-9s = %d;\n", ddf.c_str(), 1<<i); }
  printarray(f, "flags", flags);
  fprintf(f, "const int num_flags = %d;\n\n", n);

  n=specregs.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int SR_%-12s = %d;\n", dedollar(specregs[i]).c_str(), i);
  while (specregs.size()<16)
    specregs.push_back("???");
  printarray(f, "specregs", specregs);
  fprintf(f, "const int num_specregs = %d;\n\n", n);

  n=pageprots.size();
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int PAGE_%-12s = %d;\n", dedollar(pageprots[i]).c_str(), 1<<i);
  printarray(f, "pageprots", pageprots, "PAGE");
  fprintf(f, "const int num_pageprots = %d;\n\n", n);

  n=interrupts.size();
  printarray(f, "short_ints", interrupts, "IV$");
  for (int i=0; i<n; i+=1)
    fprintf(f, "const int INTR_%-12s = %d;\n", dedollar(interrupts[i]).c_str(), i);
  fprintf(f, "const char * const interrupt_name[] =\n");
  for (int i=0; i<n; i+=1)
  { if (i==0)
      fprintf(f, "  { ");
    else
      fprintf(f, "    ");
    fprintf(f, "\"%s: %s\"", dedollar(interrupts[i]).c_str(), intdescriptions[i].c_str());
    if (i==n-1)
      fprintf(f, " };\n");
    else
      fprintf(f, ",\n"); }
  fprintf(f, "const int num_interrupts = %d;\n\n", interrupts.size());

  fprintf(f, "const char * const register_name[] =\n");
  fprintf(f, "  { \"R0\", \"R1\", \"R2\", \"R3\", \"R4\", \"R5\", \"R6\", \"R7\",\n");
  fprintf(f, "    \"R8\", \"R9\", \"R10\", \"R11\", \"R12\", \"SP\", \"FP\", \"PC\" };\n");
  fprintf(f, "const int SP=13, FP=14, PC=15;\n\n");
  fclose(f); }

void main()
{ bool ok=process("standard-codes");
  if (!ok)
  { printf("There is no 'standard-codes' file. Can not continue\n");
    exit(1); }
  ok=process("local-codes");
  if (!ok)
    printf("Warning: there is no 'local-codes' file\n");
  output("codes.h"); }
